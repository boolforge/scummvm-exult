/*
 *  Copyright (C) 2001-2022  The Exult Team
 */

#include "exult_input_adapter.h"
#include "common/debug.h"
#include "exult_core_src/input/InputManager.h"

namespace Exult {

ExultInputAdapter::ExultInputAdapter(OSystem* o) : ScummVM::Input::InputAdapter(o), _input_manager(nullptr) {
	debug(1, "ExultInputAdapter: Constructor");
}

ExultInputAdapter::~ExultInputAdapter() {
	debug(1, "ExultInputAdapter: Destructor");
}

void ExultInputAdapter::init() {
	debug(1, "ExultInputAdapter: init()");
	_input_manager = ExultCore::InputManager::get_ptr();
}

void ExultInputAdapter::shutdown() {
	debug(1, "ExultInputAdapter: shutdown()");
}

void ExultInputAdapter::handleEvent(const Common::Event& event) {
	// Convert ScummVM event to ExultCore event and pass to InputManager.
	// This is a simplified mapping, a full implementation would need to map all event types.
	// For now, focusing on key and mouse events.

	if (event.type == Common::Event::ET_KEY_DOWN) {
		ExultCore::InputManager::Key exult_key = ExultCore::InputManager::KEY_UNKNOWN;
		// Map ScummVM keycodes to Exult keycodes
		// This is a simplified mapping, needs full implementation.
		switch (event.key.sym) {
			case Common::Key::K_ESCAPE: exult_key = ExultCore::InputManager::KEY_ESCAPE; break;
			case Common::Key::K_RETURN: exult_key = ExultCore::InputManager::KEY_RETURN; break;
			case Common::Key::K_SPACE: exult_key = ExultCore::InputManager::KEY_SPACE; break;
			case Common::Key::K_LEFT: exult_key = ExultCore::InputManager::KEY_LEFT; break;
			case Common::Key::K_RIGHT: exult_key = ExultCore::InputManager::KEY_RIGHT; break;
			case Common::Key::K_UP: exult_key = ExultCore::InputManager::KEY_UP; break;
			case Common::Key::K_DOWN: exult_key = ExultCore::InputManager::KEY_DOWN; break;
			// Add more key mappings as needed
			default: break;
		}
		if (exult_key != ExultCore::InputManager::KEY_UNKNOWN) {
			_input_manager->handle_key_down(exult_key);
		}
	} else if (event.type == Common::Event::ET_KEY_UP) {
		ExultCore::InputManager::Key exult_key = ExultCore::InputManager::KEY_UNKNOWN;
		// Map ScummVM keycodes to Exult keycodes
		switch (event.key.sym) {
			case Common::Key::K_ESCAPE: exult_key = ExultCore::InputManager::KEY_ESCAPE; break;
			case Common::Key::K_RETURN: exult_key = ExultCore::InputManager::KEY_RETURN; break;
			case Common::Key::K_SPACE: exult_key = ExultCore::InputManager::KEY_SPACE; break;
			case Common::Key::K_LEFT: exult_key = ExultCore::InputManager::KEY_LEFT; break;
			case Common::Key::K_RIGHT: exult_key = ExultCore::InputManager::KEY_RIGHT; break;
			case Common::Key::K_UP: exult_key = ExultCore::InputManager::KEY_UP; break;
			case Common::Key::K_DOWN: exult_key = ExultCore::InputManager::KEY_DOWN; break;
			// Add more key mappings as needed
			default: break;
		}
		if (exult_key != ExultCore::InputManager::KEY_UNKNOWN) {
			_input_manager->handle_key_up(exult_key);
		}
	} else if (event.type == Common::Event::ET_MOUSE_DOWN) {
		_input_manager->handle_mouse_down(event.mouse.x, event.mouse.y, event.mouse.button);
	} else if (event.type == Common::Event::ET_MOUSE_UP) {
		_input_manager->handle_mouse_up(event.mouse.x, event.mouse.y, event.mouse.button);
	} else if (event.type == Common::Event::ET_MOUSE_MOTION) {
		_input_manager->handle_mouse_motion(event.mouse.x, event.mouse.y);
	}
}

} // namespace Exult
