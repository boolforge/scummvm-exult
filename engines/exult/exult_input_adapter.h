// engines/exult/exult_input_adapter.h

#ifndef EXULT_INPUT_ADAPTER_H
#define EXULT_INPUT_ADAPTER_H

#include "common/events.h"
#include "common/system.h"

// Forward declarations for ExultCore components (if needed later for direct integration)
namespace ExultCore {
    // class Gump_Manager;
    // class Keys;
    // class Mouse;
}

namespace ScummVM {
namespace Exult {

class ExultInputAdapter {
public:
    // Modified constructor to only take system and eventManager for now
    ExultInputAdapter(OSystem* system, Common::EventManager* eventManager);
    ~ExultInputAdapter();

    void processScummVMEvents();

private:
    OSystem* _osystem;
    Common::EventManager* _eventManager;

    // ExultCore components are not directly managed by this adapter for now
    // ExultCore::Gump_Manager* _gumpsManager;
    // ExultCore::Keys* _keysHandler;
    // ExultCore::Mouse* _mouseHandler;

    void translateAndDispatch(const Common::Event& event);
};

} // namespace Exult
} // namespace ScummVM

#endif // EXULT_INPUT_ADAPTER_H


