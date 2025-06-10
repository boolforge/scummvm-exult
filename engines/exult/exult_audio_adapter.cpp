// engines/exult/exult_audio_adapter.cpp

#include "exult_audio_adapter.h"
#include "exult_engine.h" // May need access to engine state or Exult components via engine
#include "common/system.h"
#include "common/debug.h"
#include "audio/mixer.h"
#include <cstring> // For memset

// Actual Exult includes for its audio handling classes
// Paths are relative to the engines/exult/ directory where this .cpp file resides.
#include "exult_core_src/audio/Audio.h" // Main Exult audio system
// #include "exult_core_src/audio/AudioMixer.h" // If Exult_Engine_s internal mixer is used directly

namespace ScummVM {
namespace Exult {

ExultAudioAdapter::ExultAudioAdapter(OSystem* system, Audio::Mixer* mixer,
                                     ExultCore::Audio* exultAudioSystem /*, ExultCore::AudioMixer* exultInternalMixer */)
    : _osystem(system),
      _mixer(mixer),
      _exultAudioSystem(exultAudioSystem) /*,
      _exultInternalMixer(exultInternalMixer) */ {
    debug(1, "ExultAudioAdapter: Constructor called.");
    if (!_osystem) {
        error("ExultAudioAdapter: OSystem pointer is null!");
    }
    if (!_mixer) {
        error("ExultAudioAdapter: ScummVM Mixer pointer is null!");
    }
    // _exultAudioSystem may be null at this point if Audio::Init() hasn't been called yet by ExultEngine.
    // It will be fetched in this->init().
    if (!_exultAudioSystem) {
        debug(1, "ExultAudioAdapter: Exult Audio system pointer is null at construction (this is expected).");
    }
}

ExultAudioAdapter::~ExultAudioAdapter() {
    debug(1, "ExultAudioAdapter: Destructor called.");
    shutdown(); // Ensure resources are released
}

bool ExultAudioAdapter::init() {
    debug(1, "ExultAudioAdapter: init() called.");

    // Fetch the ExultCore::Audio singleton instance now that ExultCore::Audio::Init() should have been called by ExultEngine.
    if (!_exultAudioSystem) {
        _exultAudioSystem = ExultCore::Audio::get_ptr();
    }

    if (!_osystem || !_mixer || !_exultAudioSystem) {
        error("ExultAudioAdapter: Cannot init, essential pointers are null (OSystem, Mixer, or ExultAudioSystem after get_ptr).");
        return false;
    }

    // Configure ExultCore::Audio for ScummVM mode.
    Audio::MixerCaps caps = _mixer->getCapabilities();
    // Exult typically uses 16-bit audio. We assume stereo here, but Exult's internal mixer might be mono
    // or configurable. For now, align with typical ScummVM expectations.
    // The actual number of channels Exult's mixer produces should be handled in ExultCore::Audio::getMixedAudio.
    int desiredChannels = caps.channels == Audio::MixMode::MONO ? 1 : 2;
    _exultAudioSystem->setScummVMMode(true, caps.preferredRate, desiredChannels);

    // ExultCore::Audio::Init() was already called by ExultEngine.
    // Here, we might need to trigger other specific Exult audio initializations
    // that should happen *after* mode setting, e.g., loading SFX resources.
    // This depends on Exult's internal audio system structure.
    // For example, if Audio::Init_sfx() is relevant:
    // _exultAudioSystem->Init_sfx(); // Example, if needed.

    // Register this adapter as an audio stream with ScummVM's mixer.
    // The priority and mode should match what Exult will provide.
    // Assuming Exult provides interleaved stereo.
    _channelId = _mixer->playStream(this, Audio::MixPriority::MUSIC_AND_EFFECTS, Audio::MixMode::STEREO_INTERLEAVED, caps.preferredRate);

    if (_channelId == Audio::ChannelId::INVALID) {
        error("ExultAudioAdapter: Failed to register Exult audio stream with ScummVM mixer.");
        // Should we call _exultAudioSystem->setScummVMMode(false, 0, 0) here?
        return false;
    }

    debug(1, "ExultAudioAdapter: Initialization complete. Registered with ScummVM mixer on channel %d.", (int)_channelId);
    return true;
}

void ExultAudioAdapter::shutdown() {
    debug(1, "ExultAudioAdapter: shutdown() called.");

    // Stop and unregister the stream from ScummVM's mixer.
    if (_channelId != Audio::ChannelId::INVALID && _mixer) {
        _mixer->stopChannel(_channelId);
        _channelId = Audio::ChannelId::INVALID;
        debug(1, "ExultAudioAdapter: Unregistered stream from ScummVM mixer.");
    }

    // Potentially tell Exult's audio system to shut down or revert from ScummVM mode.
    // Note: ExultCore::Audio::Destroy() is static and deletes the singleton.
    // We should not call it here if ExultEngine might be reinitialized later.
    // Instead, call specific cleanup methods if they exist.
    if (_exultAudioSystem) {
        // _exultAudioSystem->stop_music(); // Example
        // _exultAudioSystem->stop_sound_effects(); // Example
        // _exultAudioSystem->setScummVMMode(false, 0, 0); // Revert mode if appropriate
        // For now, Exult's Audio::Destroy() will handle its internal cleanup when ExultEngine is destroyed.
        debug(1, "ExultAudioAdapter: Exult audio system cleanup (if any specific calls needed) would be here.");
    }
}

// ScummVM Audio::Stream interface implementation
    // }
}

// ScummVM Audio::Stream interface implementation
bool ExultAudioAdapter::getSamples(int16 *stream, int length) {
    if (!_exultAudioSystem || !_exultAudioSystem->is_audio_enabled()) {
        // Fill with silence if Exult audio is not ready or explicitly disabled in Exult
        memset(stream, 0, length * sizeof(int16)); // length is number of int16 samples
        return true;
    }
    // Request mixed audio data from ExultCore::Audio.
    // `length` is the total number of int16 samples ScummVM's mixer expects.
    int samples_read = _exultAudioSystem->getMixedAudio(stream, length);

    // If ExultCore::Audio provided fewer samples than requested, fill the remainder with silence.
    if (samples_read < length) {
        // Should not happen if getMixedAudio is implemented correctly to fill 'length' samples
        // or if it returns the actual number of samples it could fill (which should be 'length' or 0).
        // If it returns 0 (e.g. error or no data), the whole buffer should be silent.
        // If it returns N < length, that means it partially filled.
        debug(2, "ExultAudioAdapter::getSamples: ExultCore::Audio::getMixedAudio returned %d samples, expected %d. Filling remainder with silence.", samples_read, length);
        memset(stream + samples_read, 0, (length - samples_read) * sizeof(int16));
    } else if (samples_read > length) {
        // This would be an error in ExultCore::Audio::getMixedAudio implementation.
        error("ExultAudioAdapter::getSamples: ExultCore::Audio::getMixedAudio returned %d samples, but only %d were requested!", samples_read, length);
        // Potentially truncate or handle, but this indicates a deeper issue. For now, ScummVM mixer will only use `length`.
    }
    // If samples_read == length, buffer is correctly filled.
    // If samples_read == 0, buffer (hopefully already zeroed by getMixedAudio or here) is silent.

    return true; // Always return true as we handle silence filling.
}

// Placeholder implementations for control methods
// void ExultAudioAdapter::playSoundEffect(int soundId) {
//     if (_exultAudioSystem) {
//         // _exultAudioSystem->playSfx(soundId);
//     }
// }

// void ExultAudioAdapter::playMusic(int musicId) {
//     if (_exultAudioSystem) {
//         // _exultAudioSystem->playMusicTrack(musicId);
//     }
// }

// void ExultAudioAdapter::stopMusic() {
//     if (_exultAudioSystem) {
//         // _exultAudioSystem->stopMusic();
//     }
// }

// void ExultAudioAdapter::setVolume(int masterVolume, int sfxVolume, int musicVolume) {
//     if (_exultAudioSystem) {
//         // _exultAudioSystem->setMasterVolume(masterVolume);
//         // _exultAudioSystem->setSfxVolume(sfxVolume);
//         // _exultAudioSystem->setMusicVolume(musicVolume);
//     }
// }

} // namespace Exult
} // namespace ScummVM

