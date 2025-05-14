// engines/exult/exult_audio_adapter.h

#ifndef EXULT_AUDIO_ADAPTER_H
#define EXULT_AUDIO_ADAPTER_H

#include "audio/mixer.h"
#include "common/system.h"

// Forward declarations for Exult classes if needed
// namespace Exult {
//     class Audio_Manager; // Or whatever Exult calls its main audio component
// }

namespace ScummVM {

// Forward declaration of ExultEngine to avoid circular dependencies if needed
namespace Exult { class ExultEngine; }

namespace Exult {

// This class will bridge Exult_Engine_s audio generation with ScummVM_Engine_s Audio::Mixer.
// One common approach is to make this class a custom Audio::Stream or Audio::Channel
// that ScummVM_Engine_s mixer can poll for audio data.
class ExultAudioAdapter /* : public Audio::Stream (or other ScummVM base class if applicable) */ {
public:
    ExultAudioAdapter(OSystem* system, Audio::Mixer* mixer /*, ::Exult::Audio_Manager* exultAudioManager */);
    ~ExultAudioAdapter();

    // Initializes the audio adapter and registers with ScummVM_Engine_s mixer
    bool init();

    // Shuts down the audio adapter
    void shutdown();

    // If ExultAudioAdapter is a stream/channel, ScummVM_Engine_s mixer would call a method like this:
    // virtual bool getSamples(int16 *stream, int length);
    // This method would then call into Exult_Engine_s audio system to fill the buffer.

    // Alternatively, Exult_Engine_s audio system might have its own callback that we need to hook
    // or periodically pull data from.

    // Methods to control Exult_Engine_s audio (called from ExultEngine or Exult game logic via bridge)
    // void playSoundEffect(int soundId);
    // void playMusic(int musicId);
    // void stopMusic();
    // void setVolume(int masterVolume, int sfxVolume, int musicVolume);

private:
    OSystem* _osystem;
    Audio::Mixer* _mixer; // ScummVM_Engine_s mixer to send audio data to

    // Pointer to Exult_Engine_s audio manager/system
    // ::Exult::Audio_Manager* _exultAudioManager;

    // Internal buffer if needed for sample format conversion or temporary storage
    // Common::Array<int16> _internalBuffer;

    // ScummVM audio stream ID if registered as a stream
    // Audio::ChannelId _channelId;
};

} // namespace Exult
} // namespace ScummVM

#endif // EXULT_AUDIO_ADAPTER_H

