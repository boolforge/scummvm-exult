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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "common/system.h"
#include "common/events.h"
#include "common/textconsole.h"
#include "common/debug.h"
#include "common/config-manager.h"
#include "engines/util.h"

#include "ack/ack.h"
#include "ack/input.h"

namespace Ack {

Input::Input(AckEngine *vm) : _vm(vm), _keyboardBufferHead(0), _keyboardBufferTail(0) {
    // Clear the keyboard buffer
    clearKeyboardBuffer();
    
    // Initialize mouse state
    _mouseX = 160;
    _mouseY = 100;
    _mouseButtons = 0;
}

Input::~Input() {
}

/**
 * Process events from the event manager
 */
void Input::processEvents() {
    Common::Event event;
    
    // Poll for events
    Common::EventManager *eventMan = g_system->getEventManager();
    while (eventMan->pollEvent(event)) {
        switch (event.type) {
        case Common::EVENT_KEYDOWN:
            // Add key to keyboard buffer
            addKeyToBuffer(event.kbd.keycode);
            break;
            
        case Common::EVENT_MOUSEMOVE:
            // Update mouse position
            _mouseX = event.mouse.x;
            _mouseY = event.mouse.y;
            break;
            
        case Common::EVENT_LBUTTONDOWN:
            // Set left mouse button flag
            _mouseButtons |= 1;
            break;
            
        case Common::EVENT_LBUTTONUP:
            // Clear left mouse button flag
            _mouseButtons &= ~1;
            break;
            
        case Common::EVENT_RBUTTONDOWN:
            // Set right mouse button flag
            _mouseButtons |= 2;
            break;
            
        case Common::EVENT_RBUTTONUP:
            // Clear right mouse button flag
            _mouseButtons &= ~2;
            break;
            
        case Common::EVENT_MBUTTONDOWN:
            // Set middle mouse button flag
            _mouseButtons |= 4;
            break;
            
        case Common::EVENT_MBUTTONUP:
            // Clear middle mouse button flag
            _mouseButtons &= ~4;
            break;
            
        case Common::EVENT_QUIT:
            // Quit the game
            _vm->quitGame();
            break;
            
        default:
            break;
        }
    }
}

/**
 * Add a key to the keyboard buffer
 * 
 * @param key The key to add
 */
void Input::addKeyToBuffer(Common::KeyCode key) {
    // Check if buffer is full
    if ((_keyboardBufferHead + 1) % KEYBOARD_BUFFER_SIZE == _keyboardBufferTail) {
        // Buffer is full, ignore key
        return;
    }
    
    // Add key to buffer
    _keyboardBuffer[_keyboardBufferHead] = key;
    _keyboardBufferHead = (_keyboardBufferHead + 1) % KEYBOARD_BUFFER_SIZE;
}

/**
 * Get a key from the keyboard buffer
 * 
 * @return The key, or 0 if buffer is empty
 */
Common::KeyCode Input::getKeyFromBuffer() {
    // Check if buffer is empty
    if (_keyboardBufferHead == _keyboardBufferTail) {
        // Buffer is empty
        return Common::KEYCODE_INVALID;
    }
    
    // Get key from buffer
    Common::KeyCode key = _keyboardBuffer[_keyboardBufferTail];
    _keyboardBufferTail = (_keyboardBufferTail + 1) % KEYBOARD_BUFFER_SIZE;
    
    return key;
}

/**
 * Clear the keyboard buffer
 */
void Input::clearKeyboardBuffer() {
    _keyboardBufferHead = 0;
    _keyboardBufferTail = 0;
}

/**
 * Check if a key is available in the keyboard buffer
 * 
 * @return True if a key is available, false otherwise
 */
bool Input::isKeyAvailable() {
    return _keyboardBufferHead != _keyboardBufferTail;
}

/**
 * Wait for a key press
 * 
 * @return The key pressed
 */
Common::KeyCode Input::waitForKey() {
    Common::KeyCode key = Common::KEYCODE_INVALID;
    
    // Wait for a key
    while (key == Common::KEYCODE_INVALID) {
        // Process events
        processEvents();
        
        // Check if a key is available
        if (isKeyAvailable()) {
            key = getKeyFromBuffer();
        }
        
        // Delay
        g_system->delayMillis(10);
    }
    
    return key;
}

/**
 * Get the mouse position
 * 
 * @param x Pointer to store x coordinate
 * @param y Pointer to store y coordinate
 */
void Input::getMousePosition(int *x, int *y) {
    *x = _mouseX;
    *y = _mouseY;
}

/**
 * Get the mouse button state
 * 
 * @return The mouse button state (bit 0 = left, bit 1 = right, bit 2 = middle)
 */
byte Input::getMouseButtons() {
    return _mouseButtons;
}

/**
 * Wait for a mouse click
 * 
 * @param x Pointer to store x coordinate
 * @param y Pointer to store y coordinate
 * @return The mouse button clicked (1 = left, 2 = right, 4 = middle)
 */
byte Input::waitForMouseClick(int *x, int *y) {
    byte buttons = 0;
    
    // Wait for a mouse click
    while (buttons == 0) {
        // Process events
        processEvents();
        
        // Check if a mouse button is pressed
        buttons = _mouseButtons;
        
        // Delay
        g_system->delayMillis(10);
    }
    
    // Store mouse position
    *x = _mouseX;
    *y = _mouseY;
    
    return buttons;
}

/**
 * Convert a ScummVM key code to an ACK key code
 * 
 * @param key The ScummVM key code
 * @return The ACK key code
 */
byte Input::convertKeyCode(Common::KeyCode key) {
    switch (key) {
    case Common::KEYCODE_BACKSPACE:
        return 8;
    case Common::KEYCODE_TAB:
        return 9;
    case Common::KEYCODE_RETURN:
    case Common::KEYCODE_KP_ENTER:
        return 13;
    case Common::KEYCODE_ESCAPE:
        return 27;
    case Common::KEYCODE_SPACE:
        return 32;
    case Common::KEYCODE_F1:
        return 59;
    case Common::KEYCODE_F2:
        return 60;
    case Common::KEYCODE_F3:
        return 61;
    case Common::KEYCODE_F4:
        return 62;
    case Common::KEYCODE_F5:
        return 63;
    case Common::KEYCODE_F6:
        return 64;
    case Common::KEYCODE_F7:
        return 65;
    case Common::KEYCODE_F8:
        return 66;
    case Common::KEYCODE_F9:
        return 67;
    case Common::KEYCODE_F10:
        return 68;
    case Common::KEYCODE_F11:
        return 133;
    case Common::KEYCODE_F12:
        return 134;
    case Common::KEYCODE_UP:
        return 72;
    case Common::KEYCODE_DOWN:
        return 80;
    case Common::KEYCODE_LEFT:
        return 75;
    case Common::KEYCODE_RIGHT:
        return 77;
    case Common::KEYCODE_HOME:
        return 71;
    case Common::KEYCODE_END:
        return 79;
    case Common::KEYCODE_PAGEUP:
        return 73;
    case Common::KEYCODE_PAGEDOWN:
        return 81;
    case Common::KEYCODE_INSERT:
        return 82;
    case Common::KEYCODE_DELETE:
        return 83;
    default:
        // For regular ASCII keys, return the ASCII value
        if (key >= Common::KEYCODE_SPACE && key <= Common::KEYCODE_TILDE) {
            return key;
        }
        return 0;
    }
}

} // End of namespace Ack
