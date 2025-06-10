// engines/exult/exult_input_adapter.h

#ifndef EXULT_INPUT_ADAPTER_H
#define EXULT_INPUT_ADAPTER_H

#include "common/events.h"
// #include "common/eventmanager.h" // Removed, as EventManager is in common/events.h
#include "common/system.h" // Added for OSystem

// Forward declarations for Exult_Engine_s input-related classes
// Based on exult_core_src/gumps/Gump_manager.h, exult_core_src/keys.h, exult_core_src/mouse.h
namespace ExultCore {
    class Gump_Manager;   // Manages UI elements (gumps)
    class KeyBinder;      // Handles keyboard input (actual class in keys.h)
    class Mouse;          // Handles mouse input
    // class Game_Manager; // If it also handles direct game input, might be needed
}

namespace ScummVM {

// Forward declaration of ExultEngine to avoid circular dependencies if needed
namespace Exult { class ExultEngine; }

namespace Exult {

class ExultInputAdapter {
public:
    // Constructor: Exult component pointers may be null initially.
    ExultInputAdapter(OSystem* system, Common::EventManager* eventManager,
                      ExultCore::Gump_Manager* gumpsManager, // can be null
                      ExultCore::KeyBinder* keysHandler,      // can be null (changed from Keys*)
                      ExultCore::Mouse* mouseHandler);       // can be null
    ~ExultInputAdapter();

    bool init(); // New method to fetch Exult component pointers
    void processScummVMEvents();

private:
    OSystem* _osystem;
    Common::EventManager* _eventManager;

    ExultCore::Gump_Manager* _gumpsManager;
    ExultCore::KeyBinder* _keysHandler; // Changed from Keys*
    ExultCore::Mouse* _mouseHandler;

    void translateAndDispatch(const Common::Event& event);
};

} // namespace Exult
} // namespace ScummVM

#endif // EXULT_INPUT_ADAPTER_H

