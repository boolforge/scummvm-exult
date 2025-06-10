/*
Copyright (C) 2005 The Pentagram team
Copyright (C) 2010-2022 The Exult Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#include "pent_include.h"

#include "AudioMixer.h"

#include "AudioChannel.h"
#include "Configuration.h"
#include "Midi.h"
#include "MidiDriver.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <mutex>

#ifdef __GNUC__
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wold-style-cast"
#	pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif    // __GNUC__
#include <SDL.h>
#ifdef __GNUC__
#	pragma GCC diagnostic pop
#endif    // __GNUC__

namespace Pentagram {
	class SDLAudioDevice {
	public:
		SDLAudioDevice(SDL_AudioDeviceID dev_) : dev(dev_) {}

		~SDLAudioDevice() {
			SDL_CloseAudioDevice(dev);
		}

		void pause() {
			SDL_PauseAudioDevice(dev, 1);
		}

		void unpause() {
			SDL_PauseAudioDevice(dev, 0);
		}

		void lock() {
			SDL_LockAudioDevice(dev);
		}

		void unlock() {
			SDL_UnlockAudioDevice(dev);
		}

	private:
		SDL_AudioDeviceID dev;
	};
}    // namespace Pentagram

using namespace Pentagram;

AudioMixer* AudioMixer::the_audio_mixer = nullptr;

void AudioMixer::setScummVMOutputMode(bool enabled) {
	_isScummVMOutputMode = enabled;
	std::cout << "AudioMixer: ScummVM Output Mode " << (enabled ? "ENABLED" : "DISABLED") << std::endl;
	if (_isScummVMOutputMode && device) {
		// If switching to ScummVM mode and SDL device was open, close it.
		std::cout << "AudioMixer: Closing existing SDL audio device due to ScummVM mode." << std::endl;
		device.reset(); // This will call SDLAudioDevice destructor, which closes SDL audio.
		audio_ok = false; // Mark SDL audio as not OK, but mixer can still function for sample provision.
	}
	// If switching from ScummVM mode to standalone, SDL audio would need to be reinitialized.
	// This is not currently handled here; assumes mode is set before full initialization.
}

AudioMixer::AudioMixer(int sample_rate_, bool stereo_, int num_channels_)
		: audio_ok(false), sample_rate(sample_rate_), stereo(stereo_),
		  midi(nullptr), midi_volume(255), id_counter(0), _isScummVMOutputMode(false) { // Ensure _isScummVMOutputMode is initialized
	the_audio_mixer = this;

	std::cout << "Creating AudioMixer... Target Sample Rate: " << sample_rate_ << " Stereo: " << stereo_ << std::endl;

	// _isScummVMOutputMode might be set by ExultCore::Audio calling setScummVMOutputMode()
	// *before* this constructor if ExultCore::Audio creates Pentagram::AudioMixer explicitly after setting its own mode.
	// However, current ExultCore::Audio::Init creates Pentagram::AudioMixer directly.
	// So, Pentagram::AudioMixer needs to be told about ScummVM mode by ExultCore::Audio *after* construction,
	// or ExultCore::Audio needs to pass a flag to this constructor.
	// Let's assume ExultCore::Audio will call setScummVMOutputMode() on the created instance.
	// The constructor will then proceed based on the initial _isScummVMOutputMode (false).
	// If ExultCore::Audio::Init then calls setScummVMOutputMode(true) and re-configures, that's one path.
	// For now, constructor behaves as it did, but SDL parts are conditional.

	if (!_isScummVMOutputMode) {
		SDL_AudioSpec desired{};
		SDL_AudioSpec obtained;

		desired.format   = AUDIO_S16SYS;
		desired.freq     = sample_rate_;
		desired.channels = stereo_ ? 2 : 1;
		desired.callback = sdlAudioCallback;
		desired.userdata = this;

		desired.samples = 1;
		const int SAMPLE_BUFFER_PER_SECOND = 30;
		while (desired.samples <= desired.freq / SAMPLE_BUFFER_PER_SECOND) {
			desired.samples <<= 1;
		}

		SDL_InitSubSystem(SDL_INIT_AUDIO); // Safe to call multiple times
		const SDL_AudioDeviceID dev = SDL_OpenAudioDevice(
				nullptr, 0, &desired, &obtained, SDL_AUDIO_ALLOW_ANY_CHANGE);
		audio_ok = (dev != 0);

		if (audio_ok) {
			std::cout << "AudioMixer: SDL Audio opened using format: " << obtained.freq << " Hz "
				 << static_cast<int>(obtained.channels) << " Channels" << std::endl;
			device = std::make_unique<SDLAudioDevice>(dev);
			// Update with obtained values if SDL changed them
			sample_rate = obtained.freq;
			stereo      = obtained.channels == 2;
			internal_buffer.resize((obtained.size + 1) / 2); // For SDL callback
			device->unpause(); // Start SDL audio playback
		} else {
			std::cerr << "AudioMixer: Failed to open SDL audio device: " << SDL_GetError() << std::endl;
		}
	} else {
		// In ScummVM mode, we don't open an SDL device.
		// Mixer is "ok" in terms of being able to mix, just not outputting via SDL.
		audio_ok = true;
		// internal_buffer for getMixedOutputSamples might need resizing based on ScummVM's expected buffer size,
		// but MixAudio takes the buffer as an argument, so it's okay.
		// ScummVM will provide the buffer.
		std::cout << "AudioMixer: Initialized in ScummVM Output Mode. SDL device NOT opened." << std::endl;
	}

	// Initialize channels regardless of output mode
	// Channels need correct sample_rate and stereo settings from the start.
	for (int i = 0; i < num_channels_; i++) {
		channels.emplace_back(sample_rate, stereo);
	}
}

AudioMixer::~AudioMixer() {
	std::cout << "Destroying AudioMixer..." << std::endl;

	closeMidiOutput();

	the_audio_mixer = nullptr;
}

void AudioMixer::reset() {
	if (!audio_ok) {
		return;
	}

	std::cout << "Resetting AudioMixer..." << std::endl;

	midi->stop_music();

	const std::lock_guard<SDLAudioDevice> lock(*device);
	for (auto& channel : channels) {
		channel.stop();
	}
}

sint32 AudioMixer::playSample(
		AudioSample* sample, int loop, int priority, bool paused,
		uint32 pitch_shift_, int lvol, int rvol) {
	if (!audio_ok || channels.empty()) {
		return -1;
	}

	const std::lock_guard<SDLAudioDevice> lock(*device);
	auto it = std::find_if(channels.begin(), channels.end(), [](auto& channel) {
		return !channel.isPlaying();
	});
	if (it == channels.end()) {
		it = std::min_element(
				channels.begin(), channels.end(), [](auto& c1, auto& c2) {
					return c1.getPriority() < c2.getPriority();
				});
	}
	if (it != channels.end()
		&& (!it->isPlaying() || it->getPriority() < priority)) {
		if (id_counter == std::numeric_limits<decltype(id_counter)>::max()) {
			id_counter = 0;
		} else {
			++id_counter;
		}
		it->playSample(
				sample, loop, priority, paused, pitch_shift_, lvol, rvol,
				id_counter);
		return id_counter;
	}
	return -1;
}

bool AudioMixer::isPlaying(sint32 instance_id) const {
	if (instance_id < 0 || channels.empty() || !audio_ok) {
		return false;
	}

	const std::lock_guard<SDLAudioDevice> lock(*device);
	auto                                  it = std::find_if(
            channels.cbegin(), channels.cend(), [instance_id](auto& channel) {
                return channel.getInstanceId() == instance_id;
            });
	return it != channels.end() && it->isPlaying();
}

bool AudioMixer::isPlaying(AudioSample* sample) const {
	if (!sample || channels.empty() || !audio_ok) {
		return false;
	}

	const std::lock_guard<SDLAudioDevice> lock(*device);
	auto                                  it = std::find_if(
            channels.cbegin(), channels.cend(), [sample](auto& channel) {
                return channel.getSample() == sample;
            });
	return it != channels.end();
}

bool AudioMixer::isPlayingVoice() const {
	if (channels.empty() || !audio_ok) {
		return false;
	}

	const std::lock_guard<SDLAudioDevice> lock(*device);
	auto                                  it = std::find_if(
            channels.cbegin(), channels.cend(), [](auto& channel) {
                return channel.isPlaying()
                       && channel.getSample()->isVocSample();
            });
	return it != channels.end();
}

void AudioMixer::stopSample(sint32 instance_id) {
	if (instance_id < 0 || channels.empty() || !audio_ok) {
		return;
	}

	const std::lock_guard<SDLAudioDevice> lock(*device);
	auto                                  it = std::find_if(
            channels.begin(), channels.end(), [instance_id](auto& channel) {
                return channel.getInstanceId() == instance_id;
            });
	if (it != channels.end()) {
		it->stop();
	}
}

void AudioMixer::stopSample(AudioSample* sample) {
	if (!sample || channels.empty() || !audio_ok) {
		return;
	}

	const std::lock_guard<SDLAudioDevice> lock(*device);
	for (auto& channel : channels) {
		if (channel.getSample() == sample) {
			channel.stop();
		}
	}
}

sint32 Pentagram::AudioMixer::getLoop(sint32 instance_id) const {
	if (instance_id < 0 || channels.empty() || !audio_ok) {
		return 0;
	}

	const std::lock_guard<SDLAudioDevice> lock(*device);
	auto                                  it = std::find_if(
            channels.cbegin(), channels.cend(), [instance_id](auto& channel) {
                return channel.getInstanceId() == instance_id;
            });
	return (it != channels.end()) ? it->getLoop() : 0;
}

void Pentagram::AudioMixer::setLoop(sint32 instance_id, sint32 newloop) {
	if (instance_id < 0 || channels.empty() || !audio_ok) {
		return;
	}

	const std::lock_guard<SDLAudioDevice> lock(*device);
	auto                                  it = std::find_if(
            channels.begin(), channels.end(), [instance_id](auto& channel) {
                return channel.getInstanceId() == instance_id;
            });
	if (it != channels.end()) {
		it->setLoop(newloop);
	}
}

void AudioMixer::setPaused(sint32 instance_id, bool paused) {
	if (instance_id < 0 || channels.empty() || !audio_ok) {
		return;
	}

	const std::lock_guard<SDLAudioDevice> lock(*device);
	auto                                  it = std::find_if(
            channels.begin(), channels.end(), [instance_id](auto& channel) {
                return channel.getInstanceId() == instance_id;
            });
	if (it != channels.end()) {
		it->setPaused(paused);
	}
}

bool AudioMixer::isPaused(sint32 instance_id) const {
	if (instance_id < 0 || channels.empty() || !audio_ok) {
		return false;
	}

	const std::lock_guard<SDLAudioDevice> lock(*device);
	auto                                  it = std::find_if(
            channels.cbegin(), channels.cend(), [instance_id](auto& channel) {
                return channel.getInstanceId() == instance_id;
            });
	return it != channels.end() && it->isPaused();
}

void AudioMixer::setPausedAll(bool paused) {
	if (channels.empty() || !audio_ok) {
		return;
	}

	const std::lock_guard<SDLAudioDevice> lock(*device);
	for (auto& channel : channels) {
		channel.setPaused(paused);
	}
}

void AudioMixer::setVolume(sint32 instance_id, int lvol, int rvol) {
	if (instance_id < 0 || channels.empty() || !audio_ok) {
		return;
	}

	const std::lock_guard<SDLAudioDevice> lock(*device);
	auto                                  it = std::find_if(
            channels.begin(), channels.end(), [instance_id](auto& channel) {
                return channel.getInstanceId() == instance_id;
            });
	if (it != channels.end()) {
		it->setVolume(lvol, rvol);
	}
}

void AudioMixer::getVolume(sint32 instance_id, int& lvol, int& rvol) const {
	if (instance_id < 0 || channels.empty() || !audio_ok) {
		return;
	}

	const std::lock_guard<SDLAudioDevice> lock(*device);
	auto                                  it = std::find_if(
            channels.cbegin(), channels.cend(), [instance_id](auto& channel) {
                return channel.getInstanceId() == instance_id;
            });
	if (it != channels.end()) {
		it->getVolume(lvol, rvol);
	}
}

bool AudioMixer::set2DPosition(sint32 instance_id, int distance, int angle) {
	if (instance_id < 0 || channels.empty() || !audio_ok) {
		return false;
	}

	const std::lock_guard<SDLAudioDevice> lock(*device);
	auto                                  it = std::find_if(
            channels.begin(), channels.end(), [instance_id](auto& channel) {
                return channel.getInstanceId() == instance_id;
            });
	if (it != channels.end()) {
		it->set2DPosition(distance, angle);
		return it->isPlaying();
	}
	return false;
}

void AudioMixer::get2DPosition(
		sint32 instance_id, int& distance, int& angle) const {
	if (instance_id < 0 || channels.empty() || !audio_ok) {
		return;
	}

	const std::lock_guard<SDLAudioDevice> lock(*device);
	auto                                  it = std::find_if(
            channels.cbegin(), channels.cend(), [instance_id](auto& channel) {
                return channel.getInstanceId() == instance_id;
            });
	if (it != channels.end()) {
		it->get2DPosition(distance, angle);
	}
}

uint32 Pentagram::AudioMixer::GetPlaybackLength(sint32 instance_id) {
	if (instance_id < 0 || channels.empty() || !audio_ok) {
		return UINT32_MAX;
	}
	const std::lock_guard<SDLAudioDevice> lock(*device);
	auto                                  it = std::find_if(
            channels.begin(), channels.end(), [instance_id](auto& channel) {
                return channel.getInstanceId() == instance_id;
            });
	if (it != channels.end()) {
		return it->getPlaybackLength();
	}
	return UINT32_MAX;
}

uint32 Pentagram::AudioMixer::GetPlaybackPosition(sint32 instance_id) {
	if (instance_id < 0 || channels.empty() || !audio_ok) {
		return UINT32_MAX;
	}
	const std::lock_guard<SDLAudioDevice> lock(*device);
	auto                                  it = std::find_if(
            channels.begin(), channels.end(), [instance_id](auto& channel) {
                return channel.getInstanceId() == instance_id;
            });
	if (it != channels.end()) {
		return it->getPlaybackPosition();
	}
	return UINT32_MAX;
}

void AudioMixer::sdlAudioCallback(void* userdata, Uint8* stream, int len) {
	auto* mixer = static_cast<AudioMixer*>(userdata);
	// Unfortunately, SDL does not guarantee that stream will be aligned to
	// the correct alignment for sint16.
	// There is no real solution except using an aligned buffer and copying.
	const size_t newlen = size_t(len + 1) / 2;
	// This should never be needed, as we set the vector length
	// based on information provided by SDL. Lets leave it in anyway
	// just in case...
	if (newlen > mixer->internal_buffer.size()) {
		mixer->internal_buffer.resize(newlen);
	}
	mixer->MixAudio(mixer->internal_buffer.data(), len);
	std::memcpy(stream, mixer->internal_buffer.data(), len);
}

void AudioMixer::MixAudio(sint16* stream, uint32 bytes) {
	// audio_ok now means "mixer is usable", not necessarily "SDL device is open"
	if (!audio_ok && !_isScummVMOutputMode) { // Only return if not ScummVM mode and not ok
		return;
	}
	std::memset(stream, 0, bytes);
	if (midi) { // MIDI should also work in ScummVM mode if MyMidiPlayer outputs to buffer
		midi->produceSamples(stream, bytes);
	}
	for (auto& channel : channels) {
		if (channel.isPlaying()) {
			channel.resampleAndMix(stream, bytes);
		}
	}
}

// Method for ScummVM to pull audio
int AudioMixer::getMixedOutputSamples(int16* buffer, int num_frames) {
	if (!_isScummVMOutputMode) {
		// This method should ideally not be called if not in ScummVM mode.
		// Or, it could simulate filling a buffer even for SDL output, though less efficient.
		std::cerr << "AudioMixer::getMixedOutputSamples called but not in ScummVMOutputMode!" << std::endl;
		memset(buffer, 0, num_frames * (stereo ? 2 : 1) * sizeof(int16));
		return 0;
	}
	if (!audio_ok) { // In ScummVM mode, audio_ok means mixer is ready.
		memset(buffer, 0, num_frames * (stereo ? 2 : 1) * sizeof(int16));
		return 0;
	}

	uint32 bytes_to_mix = num_frames * (stereo ? 2 : 1) * sizeof(int16);

	// TODO: Add locking if Exult game logic can modify channels vector or MIDI state concurrently.
	// SDL_LockAudioDevice(dev) was used in SDL callback context.
	// If Exult's main loop runs in a different thread than ScummVM's audio callback,
	// then access to shared data (channels, midi player state) needs protection.
	// For now, assuming single-threaded access for simplicity during this call.
	// std::lock_guard<std::mutex> lock(_mixer_mutex); // Hypothetical mutex

	MixAudio(buffer, bytes_to_mix);

	return num_frames * (stereo ? 2 : 1); // Return number of int16 samples written
}


void AudioMixer::openMidiOutput() {
	if (midi) {
		return;
	}
	if (_isScummVMOutputMode) { // Do not open direct MIDI device if ScummVM is managing audio
		std::cout << "AudioMixer: In ScummVM Output Mode, not opening direct MIDI output." << std::endl;
		// MyMidiPlayer might still be needed for its sample generation capabilities if it can do that.
		// If MyMidiPlayer *only* talks to a system MIDI device, then it's less useful here.
		// For now, assume MyMidiPlayer::produceSamples still works for PCM generation.
		// If Exult's MIDI handling is to be fully integrated, MyMidiPlayer should send MIDI events to ScummVM.
		return;
	}
	if (!audio_ok) { // In non-ScummVM mode, audio_ok implies SDL device is ok.
		return;
	}

	auto* new_driver = new MyMidiPlayer();

	{
		const std::lock_guard<SDLAudioDevice> lock(*device);
		midi = new_driver;
	}
	midi->load_timbres();
	// midi_driver->setGlobalVolume(midi_volume);
}

void AudioMixer::closeMidiOutput() {
	if (!midi) {
		return;
	}
	std::cout << "Destroying MidiDriver..." << std::endl;

	midi->stop_music(true);
	midi->destroyMidiDriver();

	const std::lock_guard<SDLAudioDevice> lock(*device);
	delete midi;
	midi = nullptr;
}
