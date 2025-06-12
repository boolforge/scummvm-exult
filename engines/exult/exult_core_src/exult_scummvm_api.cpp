#include "exult_scummvm_api.h"
#include "exult.h"
#include "game.h"
#include "gamewin.h"
#include "tqueue.h"

namespace Exult {

// Forward declarations from exult.cc for internal use
extern void Handle_events();

void processEvents() {
    Handle_events();
}

void updateLogic() {
    if (game && gwin) {
        Game::set_ticks(SDL_GetTicks()); // Update game ticks
        gwin->get_tqueue()->process(); // Process time queue events
        // Additional game logic updates can be added here
    }
}

void renderFrame() {
    if (game && gwin) {
        gwin->show();
    }
}

} // namespace Exult

