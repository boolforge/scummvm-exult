/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "engines/exult/exult_engine.h"
#include "common/config-manager.h"
#include "common/debug.h"
#include "common/error.h"
#include "common/events.h"
#include "common/file.h"
#include "common/fs.h"
#include "common/random.h"
#include "common/system.h"
#include "engines/util.h"
#include "graphics/cursorman.h"
#include "graphics/palette.h"
#include "graphics/pixelformat.h"

namespace Exult {

ExultEngine::ExultEngine(OSystem *syst) : Engine(syst) {
	_rnd = new Common::RandomSource("exult");
}

ExultEngine::~ExultEngine() {
	delete _rnd;
}

Common::Error ExultEngine::init() {
	// Initialize graphics mode
	initGraphics(320, 200);

	// Initialize cursor
	CursorMan.showMouse(true);

	debug("ExultEngine::init() - Exult engine initialized successfully");
	return Common::kNoError;
}

Common::Error ExultEngine::run() {
	Common::Event event;

	debug("ExultEngine::run() - Starting Exult main loop");

	// Main game loop
	while (!shouldQuit()) {
		// Process events
		while (_eventMan->pollEvent(event)) {
			switch (event.type) {
			case Common::EVENT_KEYDOWN:
				if (event.kbd.keycode == Common::KEYCODE_ESCAPE) {
					quitGame();
				}
				break;
			case Common::EVENT_QUIT:
				quitGame();
				break;
			default:
				break;
			}
		}

		// Clear screen
		_system->fillScreen(0);

		// Update display
		_system->updateScreen();

		// Delay to prevent 100% CPU usage
		_system->delayMillis(16); // ~60 FPS
	}

	debug("ExultEngine::run() - Exult main loop ended");
	return Common::kNoError;
}

bool ExultEngine::hasFeature(EngineFeature f) const {
	return (f == kSupportsReturnToLauncher) ||
		   (f == kSupportsLoadingDuringRuntime) ||
		   (f == kSupportsSavingDuringRuntime);
}

const char *ExultEngine::getGameId() const {
	return "ultima7";
}

Common::Platform ExultEngine::getPlatform() const {
	return Common::kPlatformDOS;
}

} // End of namespace Exult

