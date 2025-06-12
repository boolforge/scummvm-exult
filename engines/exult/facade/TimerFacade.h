#ifndef EXULT_TIMERFACADE_H
#define EXULT_TIMERFACADE_H

#include "ITimer.h"

struct TimerFacade {
  ITimer* _timer;

  TimerFacade(ITimer* timer) : _timer(timer) {}

  uint64_t nowMicros() {
    return _timer->nowMicros();
  }

  void waitMicros(uint64_t micros) {
    _timer->waitMicros(micros);
  }
};

#endif // EXULT_TIMERFACADE_H

