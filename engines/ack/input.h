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

#ifndef ACK_INPUT_H
#define ACK_INPUT_H

#include "common/scummsys.h"
#include "common/events.h"
#include "common/str.h"

namespace Ack {

class AckEngine;

/**
 * Input manager for ACK engine
 * Handles keyboard and mouse input
 */
class Input {
private:
    AckEngine *_vm;
    
    // Input state
    bool _mouseActive;
    int _mouseX, _mouseY;
    bool _mouseButtonDown;
    
    // Keyboard state
    bool _keyboardBuffer[256]; // Track which keys are pressed
    
public:
    Input(AckEngine *vm);
    ~Input();
    
    // Event handling
    void handleEvent(const Common::Event &event);
    
    // Keyboard functions
    void clearKeyboardBuffer();
    bool isKeyPressed(byte keyCode);
    Common::String readLine(int x, int y, byte maxChars, byte charType);
    
    // Mouse functions
    bool isMouseActive() const { return _mouseActive; }
    void setMouseActive(bool active) { _mouseActive = active; }
    int getMouseX() const { return _mouseX; }
    int getMouseY() const { return _mouseY; }
    bool isMouseButtonDown() const { return _mouseButtonDown; }
    
    // Process input events
    void processEvents();
};

} // End of namespace Ack

#endif // ACK_INPUT_H
