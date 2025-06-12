#ifndef EXULT_IWINDOW_H
#define EXULT_IWINDOW_H
#include <cstdint>
struct IWindow {
  virtual ~IWindow() = default;
  virtual void createSurface(int w, int h)=0;
  virtual void blit(const uint8_t* src, int srcPitch,
                    int x, int y, int w, int h)=0;
  virtual void present()=0;
};
#endif

