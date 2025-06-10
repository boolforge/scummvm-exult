#ifndef EXULT_IAUDIOSTREAM_H
#define EXULT_IAUDIOSTREAM_H
#include <cstdint>
struct IAudioStream {
  virtual ~IAudioStream() = default;
  virtual void init(int rate, int channels)=0;
  virtual void fillBuffer(uint8_t* buf, int len)=0;
};
#endif

