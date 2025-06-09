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

#include "exult_constants.h"
#include "exult_types.h"

#include <string>

class Configuration;
class KeyBinder;
class GameManager;
class Game_window;
class TouchUI;

extern Configuration* config;
extern KeyBinder*     keybinder;
extern GameManager*   gamemanager;
extern Game_window*   gwin;
extern TouchUI*       touchui;

enum quitting_time_enum {
	QUIT_TIME_NO,
	QUIT_TIME_NOW,
	QUIT_TIME_AFTER_MENU,
	QUIT_TIME_AFTER_GAME
};

extern quitting_time_enum quitting_time;

extern bool intrinsic_trace;
extern int  usecode_trace;
extern bool combat_trace;
extern int  save_compression;
extern bool ignore_crc;

extern bool g_waiting_for_click;

extern int current_scaleval;

extern void getVersionInfo(std::ostream& os);
extern void Free_text();
extern void setup_program_paths();
extern void verify_files(const char* game_path);

// Expose Init() and Play() for ScummVM integration
extern void Init();
extern int Play();

#endif


