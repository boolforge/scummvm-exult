#ifndef EXULT_AUDIOFACADE_H
#define EXULT_AUDIOFACADE_H

#include "IAudioStream.h"
// Assuming Exult's audio processing logic is in extern/audio
// #include "extern/audio/AudioProcessor.h"

struct AudioFacade {
  IAudioStream* _audioStream;

  AudioFacade(IAudioStream* audioStream) : _audioStream(audioStream) {}

  void processAudio(uint8_t* buffer, int length) {
    // Example: call Exult's internal audio processing and route through IAudioStream::fillBuffer()
    // This is a placeholder. Actual implementation will depend on Exult's audio processing.
    _audioStream->fillBuffer(buffer, length);
  }
};

#endif // EXULT_AUDIOFACADE_H

