/*
 *  Copyright (C) 2001-2022  The Exult Team
 */

#ifndef EXULT_H
#define EXULT_H

#include <string>
#include <vector>

#include <SDL.h>

// Forward declarations
class Actor;
class Cheat;
class Game_window;
class Tile_coord;
class Paintable;
class ShortcutBar_gump;

/*
 *  Get a click, or, optionally, a keyboard char.
 *
 *  Output: false if user hit ESC.
 *      Chr gets keyboard char., or 0 if it's was a mouse click.
 */
extern bool g_waiting_for_click;

extern bool Get_click(
		int& x, int& y,               // Location returned (if not ESC).
		Mouse::Mouse_shapes shape,    // Mouse shape to use.
		char*               chr     = nullptr,    // Char. returned if not null.
		bool                drag_ok = false,      // Can drag while here.
		Paintable*          paint   = nullptr,    // Paint over everything else.
		bool rotate_colors = false    // If the palette colors should rotate.
);

/*
 *  Make a screenshot of the current screen display
 */
extern void make_screenshot(
		bool silent
		= false    // If false, will display a success/failure message
);

/*
 *  Wait for someone to stop walking.  If a timeout is given, at least
 *  one animation cycle will still always occur.
 */

extern void Wait_for_arrival(
		Actor*            actor,          // Whom to wait for.
		const Tile_coord& dest,           // Where he's going.
		long              maxticks = 0    // Max. # msecs. to wait, or 0.
);

extern void change_gamma(bool down);
extern void increase_scaleval();
extern void decrease_scaleval();
extern void setup_video(
		bool fullscreen, int setup_video_type, int resx = 0, int resy = 0,
		int gw = 0, int gh = 0, int scaleval = 0, int scaler = 0,
		Image_window::FillMode fillmode    = Image_window::Fill,
		int                    fill_scaler = 0);

enum setup_video_type {
	VIDEO_INIT        = 0,    // read and write initial settings and create gwin
	TOGGLE_FULLSCREEN = 1,    // toggle fullscreen using proper settings
	MENU_INIT  = 2,    // read and set initial settings for video gump menu
	SET_CONFIG = 3     // sets the config settings (doesn't write)
};

enum quitting_time_enum {
	QUIT_TIME_NO      = 0,
	QUIT_TIME_YES     = 1,
	QUIT_TIME_RESTART = 2
};

extern Configuration* config;
extern quitting_time_enum quitting_time;
extern ShortcutBar_gump* g_shortcutBar;
extern TouchUI*          touchui;
#endif

