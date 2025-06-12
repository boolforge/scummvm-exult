#ifndef EXULT_ITIMER_H
#define EXULT_ITIMER_H
#include <cstdint>
struct ITimer {
  virtual ~ITimer() = default;
  virtual uint64_t nowMicros()=0;
  virtual void waitMicros(uint64_t micros)=0;
};
#endif

