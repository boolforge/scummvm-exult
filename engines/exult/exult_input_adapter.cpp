// engines/exult/exult_input_adapter.cpp

#include "exult_input_adapter.h"
#include "exult_engine.h" // May need access to engine state or Exult components via engine
#include "common/system.h"
#include "common/debug.h"
#include "common/eventmanager.h"
#include "common/event.h"

// Actual Exult includes for its input handling classes
// Paths are relative to the engines/exult/ directory where this .cpp file resides.
#include "exult_core_src/gumps/gump_manager.h" // For Gump_Manager
#include "exult_core_src/keys.h"             // For Keys class
#include "exult_core_src/mouse.h"            // For Mouse class
// #include "exult_core_src/gamemgr.h"       // If Game_Manager is needed for input

namespace ScummVM {
namespace Exult {

ExultInputAdapter::ExultInputAdapter(OSystem* system, Common::EventManager* eventManager,
                                     ExultCore::Gump_Manager* gumpsManager,
                                     ExultCore::Keys* keysHandler,
                                     ExultCore::Mouse* mouseHandler)
    : _osystem(system),
      _eventManager(eventManager),
      _gumpsManager(gumpsManager),
      _keysHandler(keysHandler),
      _mouseHandler(mouseHandler) {
    debug(1, "ExultInputAdapter: Constructor called.");
    if (!_osystem) {
        error("ExultInputAdapter: OSystem pointer is null!");
    }
    if (!_eventManager) {
        error("ExultInputAdapter: EventManager pointer is null!");
    }
    // It_s expected that ExultEngine will create and pass these Exult core components.
    if (!_gumpsManager) {
        warning("ExultInputAdapter: Exult Gump_Manager pointer is null at construction!");
    }
    if (!_keysHandler) {
        warning("ExultInputAdapter: Exult Keys handler pointer is null at construction!");
    }
    if (!_mouseHandler) {
        warning("ExultInputAdapter: Exult Mouse handler pointer is null at construction!");
    }
}

ExultInputAdapter::~ExultInputAdapter() {
    debug(1, "ExultInputAdapter: Destructor called.");
}

void ExultInputAdapter::processScummVMEvents() {
    // debug(3, "ExultInputAdapter: processScummVMEvents() called.");
    if (!_eventManager) {
        warning("ExultInputAdapter: EventManager not available to process events.");
        return;
    }

    Common::Event event;
    while (_eventManager->pollEvent(event)) {
        translateAndDispatch(event);
    }
}

void ExultInputAdapter::translateAndDispatch(const Common::Event& event) {
    // debug(3, "ExultInputAdapter: Translating event type %d", event.type);

    // TODO: This is the core translation logic.
    // It needs to convert ScummVM_Engine_s Common::Event into calls that Exult_Engine_s
    // Gump_Manager, Keys, or Mouse classes understand.

    // Example for mouse events:
    if (event.type == Common::EVENT_MOUSEMOTION) {
        // debug(3, "Mouse motion: x=%d, y=%d", event.mouse.x, event.mouse.y);
        if (_mouseHandler) {
            // _mouseHandler->handle_motion(event.mouse.x, event.mouse.y);
        }
        if (_gumpsManager) {
            // _gumpsManager->handleMouseMove(event.mouse.x, event.mouse.y);
        }
    } else if (event.type == Common::EVENT_LBUTTONDOWN) {
        // debug(3, "Mouse LButton down: x=%d, y=%d", event.mouse.x, event.mouse.y);
        if (_mouseHandler) {
            // _mouseHandler->handle_button_down(event.mouse.x, event.mouse.y, 1 /*SDL_BUTTON_LEFT*/);
        }
        if (_gumpsManager) {
            // _gumpsManager->handleMouseDown(event.mouse.x, event.mouse.y, 1 /*SDL_BUTTON_LEFT*/);
        }
    } else if (event.type == Common::EVENT_LBUTTONUP) {
        // debug(3, "Mouse LButton up: x=%d, y=%d", event.mouse.x, event.mouse.y);
        if (_mouseHandler) {
            // _mouseHandler->handle_button_up(event.mouse.x, event.mouse.y, 1 /*SDL_BUTTON_LEFT*/);
        }
        if (_gumpsManager) {
            // _gumpsManager->handleMouseUp(event.mouse.x, event.mouse.y, 1 /*SDL_BUTTON_LEFT*/);
        }
    } else if (event.type == Common::EVENT_KEYDOWN) {
        // debug(3, "Key down: keycode=%d, scancode=%d, mod=%d", event.key.keycode, event.key.scancode, event.key.mod);
        if (_keysHandler) {
            // This requires mapping ScummVM_Engine_s keycodes/scancodes to Exult_Engine_s expected values.
            // Exult_Engine_s Keys class might expect SDL_Keycode or its own internal representation.
            // int exultKey = mapScummVMKeyToExult(event.key.keycode);
            // int exultMod = mapScummVMModToExult(event.key.mod);
            // _keysHandler->handle_key_down(exultKey, exultMod, event.key.unicode);
        }
        if (_gumpsManager) {
            // _gumpsManager->handleKeyDown(event.key.keycode, event.key.scancode, event.key.mod, event.key.unicode);
        }
    } else if (event.type == Common::EVENT_KEYUP) {
        // debug(3, "Key up: keycode=%d, scancode=%d, mod=%d", event.key.keycode, event.key.scancode, event.key.mod);
        if (_keysHandler) {
            // Similar mapping needed as for keydown.
            // int exultKey = mapScummVMKeyToExult(event.key.keycode);
            // _keysHandler->handle_key_up(exultKey);
        }
        if (_gumpsManager) {
            // _gumpsManager->handleKeyUp(event.key.keycode, event.key.scancode, event.key.mod);
        }
    } else if (event.type == Common::EVENT_QUIT) {
        debug(1, "ExultInputAdapter: Quit event received.");
        // ExultEngine should handle this, perhaps by setting a flag to terminate.
        // Or, if Exult has a specific quit mechanism:
        // if (_gameManager) _gameManager->quitGame();
        // For now, this adapter doesn_t directly tell the engine to quit, ScummVM_Engine_s main loop handles it.
    }
    // Add more event types as needed (mouse wheel, joystick, text input etc.)

}

} // namespace Exult
} // namespace ScummVM

