// engines/exult/exult_graphics_adapter.h

#ifndef EXULT_GRAPHICS_ADAPTER_H
#define EXULT_GRAPHICS_ADAPTER_H

#include "graphics/surface.h"
#include "common/rect.h"
#include "common/system.h" // Added for OSystem

// Forward declarations for Exult_Engine_s graphics classes
// These would typically be found in exult_core_src/imagewin/imagewin.h, exult_core_src/palette.h etc.
namespace ExultCore {
    class ImageWin;     // Main rendering window/surface in Exult
    class Palette;      // Exult_Engine_s palette management
    // class Shape;     // If Exult has a distinct Shape class for drawing
}

namespace ScummVM {
namespace Exult {

class ExultGraphicsAdapter {
public:
    // Constructor might take a pointer to Exult_Engine_s ImageWin or other core graphics components
    ExultGraphicsAdapter(OSystem* system /*, ExultCore::ImageWin* exultImageWin */);
    ~ExultGraphicsAdapter();

    bool init();

    void renderExultFrame();

    // Example methods to adapt Exult_Engine_s drawing primitives:
    // void drawShape(ExultCore::Shape* shape, int x, int y, const ExultCore::Palette* palette = nullptr);
    // void blitSurface(const byte* pixels, int src_x, int src_y, int width, int height, int dest_x, int dest_y, int pitch, const ExultCore::Palette* palette);
    // void fillRect(const Common::Rect& rect, const Color& color);
    // void present();

    // Method to convert an Exult palette to ScummVM_Engine_s 32-bit format
    // static void convertExultPaletteToScummVM(const ExultCore::Palette* exultPalette, uint32* scummvmPalette, int numColors = 256);

private:
    OSystem* _osystem;
    Graphics::Surface* _screenSurface; // Cached pointer to ScummVM_Engine_s screen surface
    ExultCore::ImageWin* _exultImageWin; // Pointer to Exult_Engine_s main rendering/image window class

    Graphics::Surface* lockScreen();
    void unlockScreen(bool updateScreen = true);
};

} // namespace Exult
} // namespace ScummVM

#endif // EXULT_GRAPHICS_ADAPTER_H

