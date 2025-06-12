#ifndef EXULT_SCUMMVM_AUDIO_ADAPTER_H
#define EXULT_SCUMMVM_AUDIO_ADAPTER_H

#include "IAudioStream.h"
#include "audio/mixer.h"

struct ScummVMAudioAdapter : IAudioStream {
  Audio::Mixer* _mixer;

  ScummVMAudioAdapter(Audio::Mixer* m) : _mixer(m) {}

  void init(int rate, int channels) override {
    // optional: configure mixer format
  }

  void fillBuffer(uint8_t* buf, int len) override {
    _mixer->buffer()->writeSamples(reinterpret_cast<int16_t*>(buf), len / 2);
  }
};

#endif // EXULT_SCUMMVM_AUDIO_ADAPTER_H

