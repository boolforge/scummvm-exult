#ifndef EXULT_GUMPSFACADE_H
#define EXULT_GUMPSFACADE_H

#include "IWindow.h"
#include "extern/gumps/Gump.h" // Assuming Gump.h is in extern/gumps

struct GumpsFacade {
  IWindow* _window;

  GumpsFacade(IWindow* window) : _window(window) {}

  void paintGump(Gump* gump) {
    // Example: call Exult's Gump::paint() and route through IWindow::blit()
    // This is a placeholder. Actual implementation will depend on Exult's Gump rendering.
    // For now, we'll just demonstrate the blit concept.
    // gump->paint(); // This would be the Exult internal call
    // _window->blit(gump->getPixels(), gump->getPitch(), gump->getX(), gump->getY(), gump->getWidth(), gump->getHeight());
  }
};

#endif // EXULT_GUMPSFACADE_H

