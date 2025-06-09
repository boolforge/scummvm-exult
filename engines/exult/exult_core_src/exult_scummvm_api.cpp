#include "exult_scummvm_api.h"
#include "exult.h"
#include "gamewin.h"
#include "mouse.h"
#include "keys.h"
#include "keyactions.h"
#include "Audio.h"
#include "game.h" // Include for Game class and global 'game' pointer

namespace Exult {

// Forward declarations of static functions from exult.cc that we need to call
// These are now exposed in exult.h

void processEvents() {
    Exult::Handle_events();
}

void updateLogic() {
    // Call Exult's game logic update function
    if (game) {
        game->update_game_logic();
    }
}

void renderFrame() {
    // Call Exult's rendering functions
    if (gwin) {
        gwin->draw_game();
        gwin->update_screen();
    }
}

} // namespace Exult


