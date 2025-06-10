// engines/exult/exult_audio_adapter.h

#ifndef EXULT_AUDIO_ADAPTER_H
#define EXULT_AUDIO_ADAPTER_H

#include "audio/mixer.h"
#include "common/system.h"

// Forward declarations for Exult_Engine_s audio classes
// Based on exult_core_src/audio/Audio.h, AudioMixer.h, etc.
namespace ExultCore {
    class Audio;          // Main Exult audio system interface
    class AudioMixer;     // Exult_Engine_s internal mixer, if distinct from ScummVM_Engine_s
    // class Music_Player; // Or similar if Exult has a dedicated music player class
    // class Sfx_Player;   // Or similar for sound effects
}

namespace ScummVM {

// Forward declaration of ExultEngine to avoid circular dependencies if needed
namespace Exult { class ExultEngine; }

namespace Exult {

class ExultAudioAdapter : public Audio::Stream {
public:
    // Constructor might take pointers to Exult_Engine_s audio components
    ExultAudioAdapter(OSystem* system, Audio::Mixer* mixer,
                      ExultCore::Audio* exultAudioSystem /*, ExultCore::AudioMixer* exultInternalMixer */);
    ~ExultAudioAdapter() override;

    bool init();
    void shutdown();

    // If ExultAudioAdapter is a ScummVM Audio::Stream, this would be the callback:
    // ScummVM Audio::Stream interface
    bool getSamples(int16 *stream, int length) override;

    // Methods to control Exult_Engine_s audio (called from ExultEngine or Exult game logic via bridge)
    // void playSoundEffect(int soundId);
    // void playMusic(int musicId);
    // void stopMusic();
    // void setVolume(int masterVolume, int sfxVolume, int musicVolume);

private:
    OSystem* _osystem;
    Audio::Mixer* _mixer; // ScummVM_Engine_s mixer to send audio data to

    ExultCore::Audio* _exultAudioSystem; // Pointer to Exult_Engine_s main audio system
    // ExultCore::AudioMixer* _exultInternalMixer; // If Exult has its own mixer to manage

    Audio::ChannelId _channelId = Audio::ChannelId::INVALID; // Registered ScummVM audio channel/stream
};

} // namespace Exult
} // namespace ScummVM

#endif // EXULT_AUDIO_ADAPTER_H

