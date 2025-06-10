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

#include "exult_core_src/gamewin.h" // For Game_window to access its components if necessary
#include "exult_keymap.h" // For key mapping utilities

// It seems KeyBinder is not directly part of ExultCore namespace in keys.h, but global.
// For now, we'll assume a way to get it. If Keys.h defines ExultCore::KeyBinder, this is fine.
// If KeyBinder is global, we'd need to include "exult_core_src/exult.h" or similar for extern.
// This is a known problematic area.

ExultInputAdapter::ExultInputAdapter(OSystem* system, Common::EventManager* eventManager,
                                     ExultCore::Gump_Manager* gumpsManager, // Initial values, likely null
                                     ExultCore::KeyBinder* keysHandler,     // Initial values, likely null
                                     ExultCore::Mouse* mouseHandler)        // Initial values, likely null
    : _osystem(system),
      _eventManager(eventManager),
      _gumpsManager(gumpsManager), // Store null initially
      _keysHandler(keysHandler),   // Store null initially
      _mouseHandler(mouseHandler)  // Store null initially
{
    debug(1, "ExultInputAdapter: Constructor called.");
    if (!_osystem) {
        error("ExultInputAdapter: OSystem pointer is null!");
    }
    if (!_eventManager) {
        error("ExultInputAdapter: EventManager pointer is null!");
    }
    // Warnings for null core components are removed here, as they'll be fetched in init()
}

ExultInputAdapter::~ExultInputAdapter() {
    debug(1, "ExultInputAdapter: Destructor called.");
}

bool ExultInputAdapter::init() {
    debug(1, "ExultInputAdapter: init() called.");
    if (!ExultCore::Game_window::get_instance()) {
        error("ExultInputAdapter: ExultCore::Game_window not initialized before InputAdapter::init().");
        return false;
    }

    _gumpsManager = ExultCore::Game_window::get_instance()->get_gump_man();
    _mouseHandler = ExultCore::Mouse::mouse; // Mouse is a static member, set by Game_window

    // Accessing KeyBinder (Exult's global `kbd` typically) is problematic.
    // Exult's `KeyBinder *kbd` is a global variable in `exult_core_src/main.cc`.
    // To access it cleanly, Exult would need to provide a static getter, e.g., `ExultCore::GetGlobalKeyBinder()`.
    // Without such a change in Exult's core, we cannot directly access `kbd`.
    // For this adaptation, _keysHandler will remain null if no such getter exists.
    // Event translation for keyboard events will need to guard against _keysHandler being null.
    // Placeholder for where one might try to get it:
    // extern KeyBinder *g_exult_key_binder; // Hypothetical extern declaration
    // _keysHandler = g_exult_key_binder;
    // Or if Game_window stored it (it does not seem to):
    // _keysHandler = ExultCore::Game_window::get_instance()->getKeyBinder();
    _keysHandler = ExultCore::KeyBinder::get_instance();

    if (!_gumpsManager) {
        warning("ExultInputAdapter: Failed to get Exult Gump_Manager instance via Game_window.");
        // Depending on strictness, could return false. For now, allow continuing.
    }
    if (!_mouseHandler) {
        warning("ExultInputAdapter: Failed to get Exult Mouse instance via static member.");
        // Depending on strictness, could return false.
    }
    if (!_keysHandler) {
        warning("ExultInputAdapter: Failed to get Exult KeyBinder instance via static getter.");
    }

    debug(1, "ExultInputAdapter: Init finished. Gumps: %p, Mouse: %p, Keys: %p", (void*)_gumpsManager, (void*)_mouseHandler, (void*)_keysHandler);
    return true;
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
    // Exult's Gump_Manager, KeyBinder, and Mouse classes often expect SDL1.2-style events.
    // We need to construct an SDL_Event on the stack and populate it.
    // Note: This requires including SDL.h which Exult's headers (keys.h, mouse.h, Gump_manager.h) already do.
    // Ensure it's the correct SDL version (likely SDL1.2 for Exult's core).
    // ScummVM's Common::Event is SDL2-based. Direct field copying might not always work for keycodes/mods.

    SDL_Event exult_event; // Using SDL1.2 event structure as that's what Exult core expects.
                           // This might lead to issues if ScummVM's Common::Event uses SDL2 specific values
                           // not present or different in SDL1.2. For keys, this is important.

    memset(&exult_event, 0, sizeof(exult_event)); // Zero out the event structure.

    bool event_handled_by_gumps = false;

    switch (event.type) {
        case Common::EVENT_MOUSEMOTION:
            exult_event.type = SDL_MOUSEMOTION;
            exult_event.motion.x = event.mouse.x;
            exult_event.motion.y = event.mouse.y;
            // Exult's Mouse::move also uses these for relative motion, but ScummVM provides absolute.
            // exult_event.motion.xrel = event.mouse.xrel; // ScummVM doesn't directly provide this in EVENT_MOUSEMOTION
            // exult_event.motion.yrel = event.mouse.yrel; // It's in Common::EVENT_MOUSEINPUT
            // Exult's Mouse::move uses the absolute coords.
            if (_mouseHandler) {
                 // Exult's Mouse::move expects game coordinates.
                 // ScummVM events are screen coordinates. The adapter or Engine needs to handle this translation if necessary.
                 // For now, assume they are compatible or Game_window/Image_window handles it.
                int game_x = event.mouse.x;
                int game_y = event.mouse.y;
                _mouseHandler->move(game_x, game_y); // Updates internal mouse state and cursor
            }
            // Gump_Manager::handle_mouse_move (not a direct public method)
            // Gumps usually check mouse position on their own paint/update or via broader event handlers.
            // Direct call to a Gump_Manager method for mouse move is often for drag operations.
            // Exult's Gump::handle_event processes SDL_MOUSEMOTION for dragging.
            if (_gumpsManager && _gumpsManager->gump_mode()) {
                 // Gump_Manager itself doesn't have a public handle_mouse_move.
                 // Active gumps receive events. This might need to go through the kbd_focus gump or all gumps.
                 // For now, this specific motion is mainly for _mouseHandler. Gumps react to clicks/drags.
            }
            break;

        case Common::EVENT_LBUTTONDOWN:
        case Common::EVENT_RBUTTONDOWN:
        case Common::EVENT_MBUTTONDOWN:
            exult_event.type = SDL_MOUSEBUTTONDOWN;
            exult_event.button.x = event.mouse.x;
            exult_event.button.y = event.mouse.y;
            if (event.type == Common::EVENT_LBUTTONDOWN) exult_event.button.button = SDL_BUTTON_LEFT;
            else if (event.type == Common::EVENT_RBUTTONDOWN) exult_event.button.button = SDL_BUTTON_RIGHT;
            else if (event.type == Common::EVENT_MBUTTONDOWN) exult_event.button.button = SDL_BUTTON_MIDDLE;
            else exult_event.button.button = 0; // Should not happen

            // Mouse class in Exult doesn't have public button_down. It's usually handled by Game_window::handle_event
            // which then calls Gump_Manager or other game logic.
            // Gump_Manager::double_clicked or Gump::handle_event might be relevant.
            if (_gumpsManager && _gumpsManager->gump_mode()) {
                // Gump_Manager::handle_event is not public.
                // Gumps are iterated in Game_window::handle_event.
                // We might need to replicate that iteration here or pass the SDL_Event to a Game_window method.
                // For now, let's assume Gump_Manager has a way or Exult's main loop calls it.
                // Exult's Gump::handle_event is the one that matters.
                // This path is complex. The simplest is to assume Exult's main loop will call gump handling.
                // Our job is to ensure Exult's *state* (like Mouse button state) is updated if needed.
                // Exult's Mouse object doesn't seem to store button state publicly.
            }
            // KeyBinder also handles mouse events for certain actions.
            if (_keysHandler) {
                 _keysHandler->HandleEvent(exult_event);
            }
             if (_gumpsManager) { // Gump_Manager::handle_kbd_event also takes mouse events
                event_handled_by_gumps = _gumpsManager->handle_kbd_event(&exult_event);
             }

            break;

        case Common::EVENT_LBUTTONUP:
        case Common::EVENT_RBUTTONUP:
        case Common::EVENT_MBUTTONUP:
            exult_event.type = SDL_MOUSEBUTTONUP;
            exult_event.button.x = event.mouse.x;
            exult_event.button.y = event.mouse.y;
            if (event.type == Common::EVENT_LBUTTONUP) exult_event.button.button = SDL_BUTTON_LEFT;
            else if (event.type == Common::EVENT_RBUTTONUP) exult_event.button.button = SDL_BUTTON_RIGHT;
            else if (event.type == Common::EVENT_MBUTTONUP) exult_event.button.button = SDL_BUTTON_MIDDLE;
            else exult_event.button.button = 0;

            if (_keysHandler) {
                 _keysHandler->HandleEvent(exult_event);
            }
            if (_gumpsManager) {
               event_handled_by_gumps = _gumpsManager->handle_kbd_event(&exult_event);
            }
            break;

        // TODO: Mouse wheel translation if Exult supports it (likely via key presses for older games)

        case Common::EVENT_KEYDOWN:
            exult_event.type = SDL_KEYDOWN;
            exult_event.key.keysym.sym = (SDLKey)scummvmKeyToExultSDL12Key(event.key.keycode);
            exult_event.key.keysym.mod = (SDL_Keymod)scummvmModToExultSDL12Mod(event.key.mod);
            exult_event.key.keysym.unicode = event.key.unicode;
            // Exult's KeyBinder primarily uses sym and mod. Scancode is less critical.
            // SDL1.2's scancode was often just a hardware code, sometimes 0.
            // We'll set it to 0 or a simple cast for now.
            exult_event.key.keysym.scancode = 0; // Or perhaps: (uint8_t)(exult_event.key.keysym.sym & 0xFF);

            if (exult_event.key.keysym.sym != ExultSDL12::SDLK_UNKNOWN) {
                if (!event_handled_by_gumps && _gumpsManager) {
                     event_handled_by_gumps = _gumpsManager->handle_kbd_event(&exult_event);
                }
                if (!event_handled_by_gumps && _keysHandler) {
                    _keysHandler->HandleEvent(exult_event);
                }
            }
            break;

        case Common::EVENT_KEYUP:
            exult_event.type = SDL_KEYUP;
            exult_event.key.keysym.sym = (SDLKey)scummvmKeyToExultSDL12Key(event.key.keycode);
            exult_event.key.keysym.mod = (SDL_Keymod)scummvmModToExultSDL12Mod(event.key.mod);
            exult_event.key.keysym.unicode = 0; // No unicode on keyup
            exult_event.key.keysym.scancode = 0;

            if (exult_event.key.keysym.sym != ExultSDL12::SDLK_UNKNOWN) {
                if (!event_handled_by_gumps && _gumpsManager) {
                     event_handled_by_gumps = _gumpsManager->handle_kbd_event(&exult_event);
                }
                if (!event_handled_by_gumps && _keysHandler) {
                    _keysHandler->HandleEvent(exult_event);
                }
            }
            break;

        case Common::EVENT_TEXTINPUT:
            // Exult's Gump_textbox::handle_event looks for SDL_KEYDOWN with unicode.
            // So, we need to simulate a series of SDL_KEYDOWN events for each char in event.text.text.
            // This is complex as it requires knowing the current modifier state for shift etc.
            // For simplicity, we'll try to send the first character as a KEYDOWN event if it's ASCII.
            // A more robust solution would involve a dedicated text input handler in Exult or deeper Gump integration.
            if (_gumpsManager && _gumpsManager->gump_mode()) { // Only if a gump is active
                for (const char* p = event.text.text; *p != '\0'; ++p) {
                    SDL_Event text_char_event;
                    memset(&text_char_event, 0, sizeof(text_char_event));
                    text_char_event.type = SDL_KEYDOWN;
                    // Attempt to map char back to a basic SDLKey for sym, if possible and ASCII.
                    // This is a hack. Proper text input needs unicode field to be primary.
                    if (*p >= 32 && *p <= 126) { // Printable ASCII
                        text_char_event.key.keysym.sym = (SDLKey)(*p);
                        // Check if it's an uppercase char to potentially set shift modifier
                        // This logic is very basic and doesn't cover all keyboard layouts or symbols.
                        if (*p >= 'A' && *p <= 'Z') {
                             text_char_event.key.keysym.mod = (SDL_Keymod)ExultSDL12::KMOD_SHIFT;
                        } else {
                             text_char_event.key.keysym.mod = (SDL_Keymod)ExultSDL12::KMOD_NONE;
                        }
                    } else {
                        text_char_event.key.keysym.sym = ExultSDL12::SDLK_UNKNOWN; // Or some other default
                        text_char_event.key.keysym.mod = (SDL_Keymod)ExultSDL12::KMOD_NONE;
                    }
                    text_char_event.key.keysym.unicode = (uint16_t)(*p); // Pass the char as unicode
                    text_char_event.key.keysym.scancode = 0;

                    // Send this fabricated event to gumps
                    _gumpsManager->handle_kbd_event(&text_char_event);
                    // Optionally, also send a KEYUP event immediately after for character input fields
                    text_char_event.type = SDL_KEYUP;
                    text_char_event.key.keysym.unicode = 0; // No unicode on keyup
                    _gumpsManager->handle_kbd_event(&text_char_event);
                }
            }
            break;

        // TODO: Common::EVENT_TEXTINPUT for text entry fields in gumps.
        // Exult's Gump_textbox::handle_event looks for SDL_KEYDOWN with unicode.

        case Common::EVENT_QUIT:
            debug(1, "ExultInputAdapter: Quit event received. ScummVM engine loop will handle this.");
            // Exult's own quit mechanisms (e.g. via ALT-X mapped in KeyBinder) will call `gwin->abort()`.
            // We don't need to do anything special here as ScummVM's main loop handles EVENT_QUIT.
            break;

        default:
            // debug(3, "Unhandled event type: %d", event.type);
            break;
    }
}

} // namespace Exult
} // namespace ScummVM

