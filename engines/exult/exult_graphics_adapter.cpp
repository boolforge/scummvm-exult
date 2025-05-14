// engines/exult/exult_graphics_adapter.cpp

#include "exult_graphics_adapter.h"
#include "exult_engine.h" // May need access to engine state or Exult components via engine
#include "common/system.h"
#include "common/debug.h"
#include "graphics/surface.h"

// Actual Exult include for its ImageWin and potentially Palette classes
// Paths are relative to the engines/exult/ directory where this .cpp file resides.
#include "exult_core_src/imagewin/imagewin.h"
#include "exult_core_src/palette.h" // Assuming palette.h is in the root of exult_core_src

namespace ScummVM {
namespace Exult {

ExultGraphicsAdapter::ExultGraphicsAdapter(OSystem* system /*, ExultCore::ImageWin* exultImageWin */)
    : _osystem(system),
      _screenSurface(nullptr),
      _exultImageWin(nullptr) { // Initialize _exultImageWin to nullptr
    debug(1, "ExultGraphicsAdapter: Constructor called.");
    if (!_osystem) {
        error("ExultGraphicsAdapter: OSystem pointer is null!");
    }
    // _exultImageWin would typically be created and managed by ExultEngine or Exult_Engine_s core,
    // and then passed to this adapter or set via a method.
    // For now, we assume it will be set later if needed by ExultEngine.
}

ExultGraphicsAdapter::~ExultGraphicsAdapter() {
    debug(1, "ExultGraphicsAdapter: Destructor called.");
    // _screenSurface is managed by OSystem, no need to delete here unless we explicitly created it.
}

bool ExultGraphicsAdapter::init() {
    debug(1, "ExultGraphicsAdapter: init() called.");
    if (!_osystem) {
        error("ExultGraphicsAdapter: Cannot init, OSystem is not available.");
        return false;
    }
    // _screenSurface = _osystem->lockScreenSurface(); // Locking here might be too early or for too long.
    // if (!_screenSurface) {
    //     error("ExultGraphicsAdapter: Failed to get screen surface from OSystem.");
    //     return false;
    // }
    // _osystem->unlockScreenSurface(false); // Unlock immediately if just checking

    // TODO: Initialize Exult_Engine_s graphics system (ImageWin) here.
    // This will involve:
    // 1. Creating an instance of ExultCore::ImageWin if not already done by ExultEngine.
    //    _exultImageWin = new ExultCore::ImageWin(...);
    // 2. Configuring ImageWin to render to a buffer that this adapter can access,
    //    or modifying ImageWin to directly use ScummVM_Engine_s Graphics::Surface operations.
    //    This is the most complex part of the graphics adaptation.
    //    (e.g., _exultImageWin->setRenderTarget(this_adapter_or_scummvm_surface);)

    debug(1, "ExultGraphicsAdapter: Placeholder initialization complete.");
    return true;
}

Graphics::Surface* ExultGraphicsAdapter::lockScreen() {
    if (!_osystem) return nullptr;
    _screenSurface = _osystem->lockScreenSurface();
    return _screenSurface;
}

void ExultGraphicsAdapter::unlockScreen(bool updateScreen) {
    if (_osystem && _screenSurface) {
        _osystem->unlockScreenSurface(updateScreen);
        _screenSurface = nullptr; // Surface pointer is invalid after unlock
    }
}

void ExultGraphicsAdapter::renderExultFrame() {
    // debug(2, "ExultGraphicsAdapter: renderExultFrame() called.");
    if (!_osystem) {
        warning("ExultGraphicsAdapter: OSystem not available for rendering.");
        return;
    }

    Graphics::Surface* targetSurface = lockScreen();
    if (!targetSurface) {
        warning("ExultGraphicsAdapter: Could not lock screen surface for rendering.");
        return;
    }

    // TODO: This is where the core rendering logic bridge happens.
    // 1. If Exult_Engine_s ImageWin renders to its own internal buffer:
    //    - Trigger Exult_Engine_s ImageWin to render the current game frame to its buffer.
    //      (e.g., if (_exultImageWin) _exultImageWin->paint();)
    //    - Get the pixel data and palette from Exult_Engine_s ImageWin.
    //    - Convert/blit this data to the ScummVM targetSurface.
    //      This might involve palette conversion (e.g., using convertExultPaletteToScummVM)
    //      and then blitting the indexed or converted RGB data.
    //
    // 2. If Exult_Engine_s ImageWin is modified to render directly using ScummVM primitives:
    //    - Its internal drawing calls would already be targeting a ScummVM surface
    //      (possibly via wrapper methods in this adapter that ImageWin calls).
    //      In this case, this function might just ensure the correct ScummVM surface is active
    //      or trigger a final presentation step if needed.

    // Placeholder: Fill screen with a color to indicate it_s working at a basic level
    // targetSurface->fill(Common::Color(0, 64, 0)); // Dark green

    unlockScreen(true); // Unlock and update the screen
}

// void ExultGraphicsAdapter::convertExultPaletteToScummVM(const ExultCore::Palette* exultPalette, uint32* scummvmPalette, int numColors) {
//     if (!exultPalette || !scummvmPalette) return;
//     // TODO: Implement palette conversion from Exult_Engine_s Palette format to ScummVM_Engine_s 32-bit RGBA
//     // for (int i = 0; i < numColors; ++i) {
//     //     ExultCore::RGB exultColor = exultPalette->getColor(i);
//     //     scummvmPalette[i] = Common::mapRGB(exultColor.r, exultColor.g, exultColor.b);
//     // }
// }

// Implement other drawing primitive bridge methods if Exult_Engine_s rendering is refactored
// to call this adapter for each drawing operation.
// void ExultGraphicsAdapter::drawShape(...) { ... }
// void ExultGraphicsAdapter::blitSurface(...) { ... }
// void ExultGraphicsAdapter::fillRect(...) { ... }

} // namespace Exult
} // namespace ScummVM

