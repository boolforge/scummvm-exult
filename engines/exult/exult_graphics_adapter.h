// engines/exult/exult_graphics_adapter.h

#ifndef EXULT_GRAPHICS_ADAPTER_H
#define EXULT_GRAPHICS_ADAPTER_H

#include "graphics/surface.h"
#include "common/rect.h"

// Forward declarations for Exult classes if needed
// namespace Exult {
//     class Shape;
//     class Palette;
// }

namespace ScummVM {
namespace Exult {

// This class will act as a bridge between Exult_Engine_s rendering calls
// and ScummVM_Engine_s Graphics::Surface.
class ExultGraphicsAdapter {
public:
    ExultGraphicsAdapter(OSystem* system);
    ~ExultGraphicsAdapter();

    // Initializes the adapter, possibly linking to ScummVM_Engine_s screen surface
    bool init();

    // Called by Exult_Engine_s renderFrame() to draw the entire scene
    void renderExultFrame();

    // Example methods to adapt Exult_Engine_s drawing primitives:
    // These would be called by modified Exult rendering code or by the ExultEngine wrapper.

    // void drawShape(::Exult::Shape* shape, int x, int y, const ::Exult::Palette* palette = nullptr);
    // void drawTile(int tileNum, int x, int y);
    // void blitSurface(const byte* pixels, int src_x, int src_y, int width, int height, int dest_x, int dest_y, int pitch, const ::Exult::Palette* palette);
    // voidfillRect(const Common::Rect& rect, const Color& color);
    // void present(); // If Exult has a specific presentation call, map it to _system->updateScreen() or similar

    // Method to convert an Exult palette (e.g., 256-color) to ScummVM_Engine_s 32-bit format for a surface
    // static void convertPalette(const byte* exultPaletteData, uint32* scummvmPalette, int numColors = 256);

private:
    OSystem* _osystem; // Pointer to ScummVM_Engine_s OSystem for accessing screen surface etc.
    Graphics::Surface* _screenSurface; // Cached pointer to ScummVM_Engine_s screen surface

    // Helper to lock and get the screen surface
    Graphics::Surface* lockScreen();
    // Helper to unlock the screen surface and request update
    void unlockScreen(bool updateScreen = true);

    // Placeholder for Exult_Engine_s specific rendering context or objects if needed
    // void* _exultRenderContext;
};

} // namespace Exult
} // namespace ScummVM

#endif // EXULT_GRAPHICS_ADAPTER_H

