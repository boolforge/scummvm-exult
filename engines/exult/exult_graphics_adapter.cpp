// engines/exult/exult_graphics_adapter.cpp

#include "exult_graphics_adapter.h"
#include "exult_engine.h" // May need access to engine state or Exult components via engine
#include "common/system.h"
#include "common/debug.h"
#include "graphics/surface.h"

// Actual Exult include for its ImageWin and potentially Palette classes
// Paths are relative to the engines/exult/ directory where this .cpp file resides.
#include "exult_core_src/gamewin.h" // For Game_window::get_instance()
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

    // Fetch the Image_window instance from Exult's Game_window singleton
    if (ExultCore::Game_window::get_instance()) {
        _exultImageWin = ExultCore::Game_window::get_instance()->get_win();
    }

    if (!_exultImageWin) {
        error("ExultGraphicsAdapter: Failed to get Image_window instance from ExultCore::Game_window.");
        return false;
    }

    // Configure ImageWin for ScummVM mode. This should prevent it from creating SDL windows/renderers.
    _exultImageWin->setScummVMMode(true);

    // Initialize ImageWin's surface.
    // The Game_window constructor (called in ExultEngine) already calls create_surface on Image_window
    // with initial dimensions. Image_window::create_surface, when in ScummVM mode, should now
    // correctly create its internal 8-bit draw_surface based on its member game_width/game_height
    // (which were set by Game_window's constructor from ExultEngine's specified internal dimensions).
    // So, an explicit call to _exultImageWin->create_surface() here might be redundant or
    // could be used to re-confirm/re-initialize if ScummVM provides different dimensions later.
    // For now, let's assume the one in Game_window constructor is sufficient.
    // If a resize/reinit is needed based on ScummVM specifics:
    // int scummvm_preferred_width = 320; // Or get from config / _osystem
    // int scummvm_preferred_height = 200; // Or get from config / _osystem
    // _exultImageWin->resized(scummvm_preferred_width, scummvm_preferred_height, false,
    //                         scummvm_preferred_width, scummvm_preferred_height, 1,
    //                         ExultCore::Image_window::point, ExultCore::Image_window::Fit);
    // This `resized` call is complex; `create_surface` called by `Game_window` constructor is simpler.
    // Let's ensure that `Image_window::game_width` and `Image_window::game_height` are correctly set
    // to Exult's native rendering resolution (e.g., 320x200 or 320x240) by the time `create_surface` is called.
    // This was done in ExultEngine when constructing Game_window.

    // Verify draw_surface was created
    if (!_exultImageWin->getRawDrawSurface()) {
        error("ExultGraphicsAdapter: Exult's draw_surface is null after Image_window initialization in ScummVM mode.");
        return false;
    }
		debug(1, "ExultGraphicsAdapter: Exult ImageWin configured for ScummVM mode. Draw surface expected to be %dx%d.", _exultImageWin->get_game_width(), _exultImageWin->get_game_height());


    debug(1, "ExultGraphicsAdapter: Initialization complete.");
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
    // 1. Ensure Exult's internal frame buffer (_exultImageWin->draw_surface) is updated.
    //    Exult's main game logic, when run by ExultEngine::updateGameLogic(), should populate this.
    //    A direct call to something like _exultImageWin->paint_all() or Game_window::get_instance()->paint()
    //    might be needed if Exult's rendering isn't automatically happening to the buffer every frame.
    //    For now, we assume draw_surface is ready.

    if (!_exultImageWin) {
        warning("ExultGraphicsAdapter: _exultImageWin is null, cannot render.");
        unlockScreen(false);
        return;
    }

    SDL_Surface* exultBuffer = _exultImageWin->getRawDrawSurface();
    if (!exultBuffer) {
        warning("ExultGraphicsAdapter: Exult's draw_surface is null.");
        unlockScreen(false);
        return;
    }

    ExultCore::Palette* exultPalette = nullptr;
    if (ExultCore::Game_window::get_instance()) {
        exultPalette = ExultCore::Game_window::get_instance()->get_pal();
    }

    if (!exultPalette) {
        warning("ExultGraphicsAdapter: Exult's palette is null.");
        unlockScreen(false);
        return;
    }

    // Convert Exult's 6-bit/channel palette to ScummVM's 32-bit format.
    uint32 scummvmPal32[256];
    for (int i = 0; i < 256; ++i) {
        byte r_byte = exultPalette->get_red(i);
        byte g_byte = exultPalette->get_green(i);
        byte b_byte = exultPalette->get_blue(i);

        // Scale 6-bit (0-63) to 8-bit (0-255)
        uint32 r = (r_byte * 255U) / 63U;
        uint32 g = (g_byte * 255U) / 63U;
        uint32 b = (b_byte * 255U) / 63U;
        scummvmPal32[i] = Common::mapRGB(r, g, b);
    }

    // Blit and convert 8-bit indexed to 32-bit RGBA
    int widthToBlit = Common::min(targetSurface->w, exultBuffer->w);
    int heightToBlit = Common::min(targetSurface->h, exultBuffer->h);

    uint8* srcPixels = (uint8*)exultBuffer->pixels;
    int srcPitch = exultBuffer->pitch;

    // Determine destination format and blit accordingly
    if (targetSurface->format.bytesPerPixel == 4) { // Target is 32-bit
        uint32* destPixels = (uint32*)targetSurface->pixels;
        int destPitch = targetSurface->pitch / 4;

        for (int y = 0; y < heightToBlit; ++y) {
            uint8* srcRow = srcPixels + y * srcPitch;
            uint32* destRow = destPixels + y * destPitch;
            for (int x = 0; x < widthToBlit; ++x) {
                destRow[x] = scummvmPal32[srcRow[x]];
            }
        }
    } else if (targetSurface->format.bytesPerPixel == 2) { // Target is 16-bit
        uint16* destPixels = (uint16*)targetSurface->pixels;
        int destPitch = targetSurface->pitch / 2;

        for (int y = 0; y < heightToBlit; ++y) {
            uint8* srcRow = srcPixels + y * srcPitch;
            uint16* destRow = destPixels + y * destPitch;
            for (int x = 0; x < widthToBlit; ++x) {
                uint32 rgba = scummvmPal32[srcRow[x]];
                uint8 r_8bit = (rgba >> 16) & 0xFF;
                uint8 g_8bit = (rgba >>  8) & 0xFF;
                uint8 b_8bit = (rgba      ) & 0xFF;
                destRow[x] = targetSurface->format.mapRGB(r_8bit, g_8bit, b_8bit);
            }
        }
    } else {
         warning("ExultGraphicsAdapter: Target surface is not 32-bit or 16-bit. Cannot render Exult frame.");
    }

    unlockScreen(true); // Unlock and update the screen
}

// void ExultGraphicsAdapter::convertExultPaletteToScummVM(const ExultCore::Palette* exultPalette, uint32* scummvmPalette, int numColors) {
//     // This logic is now inlined in renderExultFrame for simplicity, can be extracted if needed.
//     // }
// }

// Implement other drawing primitive bridge methods if Exult_Engine_s rendering is refactored
// to call this adapter for each drawing operation.
// void ExultGraphicsAdapter::drawShape(...) { ... }
// void ExultGraphicsAdapter::blitSurface(...) { ... }
// void ExultGraphicsAdapter::fillRect(...) { ... }

} // namespace Exult
} // namespace ScummVM

