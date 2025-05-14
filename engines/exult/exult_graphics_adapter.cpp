// engines/exult/exult_graphics_adapter.cpp

#include "exult_graphics_adapter.h"
#include "common/system.h"
#include "common/debug.h"
#include "graphics/surface.h"
#include "common/pixels.h"

// Placeholder for actual Exult includes if needed for direct interaction
// #include "exult_core/shape.h"
// #include "exult_core/palette.h"

namespace ScummVM {
namespace Exult {

ExultGraphicsAdapter::ExultGraphicsAdapter(OSystem* system)
    : _osystem(system), _screenSurface(nullptr) {
    debug(1, "ExultGraphicsAdapter: Constructor called.");
    // Ensure _osystem is valid
    if (!_osystem) {
        error("ExultGraphicsAdapter: OSystem pointer is null!");
    }
}

ExultGraphicsAdapter::~ExultGraphicsAdapter() {
    debug(1, "ExultGraphicsAdapter: Destructor called.");
    // If screen surface was locked and not released, it should be handled by OSystem
    // or a more robust lock/unlock mechanism within this adapter.
}

bool ExultGraphicsAdapter::init() {
    debug(1, "ExultGraphicsAdapter: init() called.");
    // Initialization for the graphics adapter.
    // This might involve pre-calculating palette conversions or setting up
    // specific rendering states if Exult requires them globally.
    // For now, it mainly ensures we can access the OSystem.
    return _osystem != nullptr;
}

Graphics::Surface* ExultGraphicsAdapter::lockScreen() {
    if (!_osystem) {
        warning("ExultGraphicsAdapter: OSystem not available to lock screen.");
        return nullptr;
    }
    _screenSurface = _osystem->lockScreenSurface();
    if (!_screenSurface) {
        warning("ExultGraphicsAdapter: Failed to lock screen surface.");
    }
    return _screenSurface;
}

void ExultGraphicsAdapter::unlockScreen(bool updateScreen) {
    if (!_osystem) {
        warning("ExultGraphicsAdapter: OSystem not available to unlock screen.");
        return;
    }
    if (_screenSurface) {
        _osystem->unlockScreenSurface(updateScreen);
        _screenSurface = nullptr; // Release the cached pointer after unlocking
    } else {
        // If updateScreen is true but we didn_t have a lock, we might still want to trigger an update
        // This depends on ScummVM_Engine_s OSystem behavior, typically unlockScreenSurface handles the update.
        if (updateScreen) {
             _osystem->updateScreen(); // Or equivalent call if unlockScreenSurface(true) wasn_t called with a surface
        }
    }
}

void ExultGraphicsAdapter::renderExultFrame() {
    debug(2, "ExultGraphicsAdapter: renderExultFrame() called.");
    Graphics::Surface* targetSurface = lockScreen();
    if (!targetSurface) {
        warning("ExultGraphicsAdapter: Cannot render Exult frame, screen surface unavailable.");
        return;
    }

    // --- Placeholder Rendering Logic ---
    // This is where the actual calls to Exult_Engine_s rendering functions would go.
    // These Exult functions would, in turn, use methods provided by this adapter
    // (or a similar interface) to draw shapes, tiles, gumps, etc., onto the targetSurface.

    // Example: Clear the screen to a specific color (e.g., black for Exult_Engine_s background)
    // targetSurface->clear(Color(0, 0, 0));

    // Example: Draw a placeholder message
    // This would ideally use ScummVM_Engine_s font rendering capabilities if Exult_Engine_s text rendering
    // is not yet adapted.
    // FontManager* fm = FontManager::create(_osystem);
    // if (fm) {
    //     const Font* defaultFont = fm->getFont(DEFAULT_FONT);
    //     if (defaultFont) {
    //         defaultFont->drawString(*targetSurface, 20, 20, "Exult Frame (Adapter Placeholder)", Color::white, TextAlignment::LEFT, 0);
    //     }
    //     delete fm;
    // }

    // TODO: Integrate calls to Exult_Engine_s actual rendering pipeline.
    // For example, if Exult has a central rendering manager:
    // Exult::RenderingManager::getInstance()->renderScene(*this, *targetSurface);
    // where `*this` could be passed if the adapter itself provides drawing primitive methods,
    // or `*targetSurface` is passed for Exult to draw on directly (less ideal for abstraction).

    // For now, let_s just fill with a test color to show it_s working.
    targetSurface->box(0, 0, targetSurface->w - 1, targetSurface->h - 1, Color(20, 20, 80)); // Dark blue

    unlockScreen(true); // Unlock and request screen update
}

/*
// Example implementation of a drawing primitive (if Exult calls this adapter)
void ExultGraphicsAdapter::drawShape(::Exult::Shape* shape, int x, int y, const ::Exult::Palette* palette) {
    if (!_screenSurface) {
        warning("ExultGraphicsAdapter::drawShape: Screen surface not locked.");
        return;
    }

    // 1. Get pixel data and dimensions from ::Exult::Shape
    // const byte* shapePixels = shape->getPixelData();
    // int shapeWidth = shape->getWidth();
    // int shapeHeight = shape->getHeight();
    // const byte* exultPaletteData = palette ? palette->getData() : getDefaultExultPalette();

    // 2. Convert/Blit pixel data to _screenSurface
    // This is complex: involves palette lookup for each pixel and writing to _screenSurface
    // in its native format (likely 32-bit RGBA).
    // for (int r = 0; r < shapeHeight; ++r) {
    //     for (int c = 0; c < shapeWidth; ++c) {
    //         byte paletteIndex = shapePixels[r * shape->getPitch() + c];
    //         uint32 scummvmColor = convertExultPaletteColor(exultPaletteData, paletteIndex);
    //         _screenSurface->putPixel(x + c, y + r, scummvmColor);
    //     }
    // }
}

// Example palette conversion (simplified)
// static void ExultGraphicsAdapter::convertPalette(const byte* exultPaletteData, uint32* scummvmPalette, int numColors) {
//     for (int i = 0; i < numColors; ++i) {
//         byte r = exultPaletteData[i*3 + 0]; // Assuming 6-bit VGA values, scale to 8-bit
//         byte g = exultPaletteData[i*3 + 1];
//         byte b = exultPaletteData[i*3 + 2];
//         scummvmPalette[i] = MAKE_COLOR_32( (r << 2) | (r >> 4), (g << 2) | (g >> 4), (b << 2) | (b >> 4) );
//     }
// }
*/

} // namespace Exult
} // namespace ScummVM

