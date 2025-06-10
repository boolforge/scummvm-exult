#ifndef EXULT_SCUMMVMTIMERADAPTER_H
#define EXULT_SCUMMVMTIMERADAPTER_H

#include "ITimer.h"
#include "common/time.h"

struct ScummVMTimerAdapter : ITimer {
  uint64_t nowMicros() override {
    return ScummVM::Common::Time::getMonotonicTime();
  }

  void waitMicros(uint64_t micros) override {
    ScummVM::Common::Time::sleep(micros / 1000); // ScummVM sleep is in milliseconds
  }
};

#endif // EXULT_SCUMMVMTIMERADAPTER_H

