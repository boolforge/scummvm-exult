#ifndef EXULT_SCUMMVMWINDOWADAPTER_H
#define EXULT_SCUMMVMWINDOWADAPTER_H

#include "IWindow.h"
#include "common/system.h"
#include "graphics/Surface.h"

struct ScummVMWindowAdapter : IWindow {
  OSystem* _sys;
  Graphics::Surface* _screen;

  ScummVMWindowAdapter(OSystem* s) : _sys(s), _screen(s->getScreenSurface()) {}

  void createSurface(int w, int h) override {
    // no-op: ScummVM window already exists
  }

  void blit(const uint8_t* src, int srcPitch, int x, int y, int w, int h) override {
    _screen->copyRectToSurface(src, srcPitch, x, y, w, h);
  }

  void present() override {
    _sys->updateScreen();
  }
};

#endif // EXULT_SCUMMVMWINDOWADAPTER_H

