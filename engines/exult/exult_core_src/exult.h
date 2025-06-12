/**
 ** Exult.h - Multiplatform Ultima 7 game engine
 **
 ** Written: 7/22/98 - JSF
 **/
/*
 *  Copyright (C) 1998-1999  Jeffrey S. Freedman
 *  Copyright (C) 2000-2022  The Exult Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef EXULT_H
#define EXULT_H

#include "Configuration.h"
#include "KeyBinder.h"
#include "gamemgr/modmgr.h"
#include "gamewin.h"
#include "mouse.h"
#include "touchui.h"

#include <string>
#include <vector>

#include <SDL.h>

// Forward declarations
class Configuration;
class KeyBinder;
class GameManager;
class Game_window;
class TouchUI;

extern Configuration* config;
extern KeyBinder*     keybinder;
extern GameManager*   gamemanager;
extern Game_window*       gwin;
extern quitting_time_enum quitting_time;
extern bool intrinsic_trace;
extern int  usecode_trace;
extern bool combat_trace;
extern int  save_compression;
extern bool ignore_crc;
extern TouchUI* touchui;
extern bool              g_waiting_for_click;
extern ShortcutBar_gump* g_shortcutBar;

// Exposed functions for ScummVM integration
namespace Exult {
    void Init();
    int Play();
    void Handle_events();
    void Handle_event(SDL_Event& event);
}

#endif // EXULT_H


