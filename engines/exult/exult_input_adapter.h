// engines/exult/exult_input_adapter.h

#ifndef EXULT_INPUT_ADAPTER_H
#define EXULT_INPUT_ADAPTER_H

#include "common/event.h"
#include "common/eventmanager.h"

// Forward declarations for Exult classes if needed
// namespace Exult {
//     class Gumps_Manager;
//     class Game_Manager; // For game-specific input like character movement
// }

namespace ScummVM {

// Forward declaration of ExultEngine to avoid circular dependencies if needed
namespace Exult { class ExultEngine; }

namespace Exult {

class ExultInputAdapter {
public:
    ExultInputAdapter(OSystem* system, Common::EventManager* eventManager /*, ::Exult::Gumps_Manager* gumpsManager, ::Exult::Game_Manager* gameManager */);
    ~ExultInputAdapter();

    // Called by ExultEngine during its run loop to process pending ScummVM events
    void processScummVMEvents();

private:
    OSystem* _osystem;
    Common::EventManager* _eventManager;

    // Pointers to Exult components that handle input
    // ::Exult::Gumps_Manager* _gumpsManager; // For UI interactions
    // ::Exult::Game_Manager* _gameManager;   // For direct game actions (e.g., avatar movement)

    // Translates a ScummVM Common::Event to an Exult-understandable input
    void translateAndDispatch(const Common::Event& event);
};

} // namespace Exult
} // namespace ScummVM

#endif // EXULT_INPUT_ADAPTER_H

