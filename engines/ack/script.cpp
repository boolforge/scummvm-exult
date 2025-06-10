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

#include "common/file.h"
#include "common/str.h"
#include "common/system.h"
#include "common/textconsole.h"

#include "ack/ack.h"
#include "ack/script.h"
#include "ack/graphics.h"
#include "ack/sound.h"

namespace Ack {

ScriptInterpreter::ScriptInterpreter(AckEngine *vm) : _vm(vm), _scriptData(nullptr), 
    _scriptSize(0), _scriptPos(0), _scriptRunning(false), _waitForInput(false) {
}

ScriptInterpreter::~ScriptInterpreter() {
    free(_scriptData);
}

/**
 * Read a byte from the script data
 * 
 * @return The byte read
 */
byte ScriptInterpreter::readByte() {
    if (_scriptPos >= _scriptSize)
        return 0;
        
    return _scriptData[_scriptPos++];
}

/**
 * Read a word (16-bit value) from the script data
 * 
 * @return The word read
 */
uint16 ScriptInterpreter::readWord() {
    uint16 value = readByte();
    value |= (readByte() << 8);
    return value;
}

/**
 * Read a string from the script data
 * 
 * @return The string read
 */
Common::String ScriptInterpreter::readString() {
    Common::String result;
    byte length = readByte();
    
    for (int i = 0; i < length; i++) {
        result += (char)readByte();
    }
    
    return result;
}

/**
 * Load a script from a file
 * 
 * @param filename The name of the script file to load
 * @return True if the script was loaded successfully, false otherwise
 */
bool ScriptInterpreter::loadScript(const Common::String &filename) {
    // Free any existing script data
    free(_scriptData);
    _scriptData = nullptr;
    _scriptSize = 0;
    _scriptPos = 0;
    _scriptRunning = false;
    _waitForInput = false;
    
    // Open the script file
    Common::File file;
    if (!file.open(filename)) {
        warning("Could not open script file: %s", filename.c_str());
        return false;
    }
    
    // Read the script data
    _scriptSize = file.size();
    _scriptData = (byte *)malloc(_scriptSize);
    if (!_scriptData) {
        warning("Could not allocate memory for script data");
        file.close();
        return false;
    }
    
    if (file.read(_scriptData, _scriptSize) != _scriptSize) {
        warning("Could not read script data from file: %s", filename.c_str());
        free(_scriptData);
        _scriptData = nullptr;
        _scriptSize = 0;
        file.close();
        return false;
    }
    
    file.close();
    
    // Reset script position
    _scriptPos = 0;
    _scriptRunning = true;
    
    return true;
}

/**
 * Run the currently loaded script
 * 
 * @return True if the script is still running, false if it has ended
 */
bool ScriptInterpreter::runScript() {
    if (!_scriptData || !_scriptRunning)
        return false;
        
    // If waiting for input, don't execute commands
    if (_waitForInput)
        return true;
        
    // Execute commands until the script ends or we need to wait
    while (_scriptRunning && !_waitForInput && _scriptPos < _scriptSize) {
        if (!executeCommand())
            break;
    }
    
    return _scriptRunning;
}

/**
 * Execute a single script command
 * 
 * @return True if the command was executed successfully, false otherwise
 */
bool ScriptInterpreter::executeCommand() {
    if (!_scriptData || _scriptPos >= _scriptSize)
        return false;
        
    byte cmd = readByte();
    
    switch (cmd) {
    case 0x01: // Say
        cmdSay(readString());
        break;
        
    case 0x02: // Move
        cmdMove(readByte());
        break;
        
    case 0x03: // Show picture
        cmdShowPicture(readWord());
        break;
        
    case 0x04: // Play sound
        cmdPlaySound(readWord());
        break;
        
    case 0x05: // Set flag
        cmdSetFlag(readByte(), readByte() != 0);
        break;
        
    case 0x06: // If flag
        cmdIfFlag(readByte(), readByte() != 0);
        break;
        
    case 0x07: // Goto
        cmdGoto(readWord());
        break;
        
    case 0x08: // Wait
        cmdWait(readWord());
        break;
        
    case 0xFF: // End
        cmdEnd();
        break;
        
    default:
        warning("Unknown script command: 0x%02X at position %d", cmd, _scriptPos - 1);
        return false;
    }
    
    return true;
}

/**
 * Stop the currently running script
 */
void ScriptInterpreter::stopScript() {
    _scriptRunning = false;
    _waitForInput = false;
}

/**
 * Display text on the screen
 * 
 * @param text The text to display
 */
void ScriptInterpreter::cmdSay(const Common::String &text) {
    debug(1, "Script: Say \"%s\"", text.c_str());
    
    // Display the text
    // This would use the text rendering system
    // For now, we'll just use a placeholder
    // _vm->getGraphics()->drawText(10, 10, text);
    
    // Wait for input
    _waitForInput = true;
}

/**
 * Move the player character
 * 
 * @param direction The direction to move (0 = north, 1 = east, 2 = south, 3 = west)
 */
void ScriptInterpreter::cmdMove(byte direction) {
    debug(1, "Script: Move %d", direction);
    
    // Move the player character
    // This would use the movement system
    // For now, we'll just use a placeholder
    // _vm->movePlayer(direction);
}

/**
 * Show a picture on the screen
 * 
 * @param pictureId The ID of the picture to show
 */
void ScriptInterpreter::cmdShowPicture(uint16 pictureId) {
    debug(1, "Script: Show picture %d", pictureId);
    
    // Show the picture
    // This would use the picture rendering system
    // For now, we'll just use a placeholder
    // _vm->getGraphics()->showPicture(pictureId);
}

/**
 * Play a sound effect
 * 
 * @param soundId The ID of the sound to play
 */
void ScriptInterpreter::cmdPlaySound(uint16 soundId) {
    debug(1, "Script: Play sound %d", soundId);
    
    // Play the sound
    // This would use the sound system
    // For now, we'll just use a placeholder
    // _vm->getSound()->playSound(soundId);
}

/**
 * Set a game flag
 * 
 * @param flagId The ID of the flag to set
 * @param value The value to set the flag to
 */
void ScriptInterpreter::cmdSetFlag(byte flagId, bool value) {
    debug(1, "Script: Set flag %d to %d", flagId, value);
    
    // Set the flag
    if (flagId < MAX_FLAGS)
        _vm->setFlag(flagId, value);
}

/**
 * Conditional branch based on a flag
 * 
 * @param flagId The ID of the flag to check
 * @param value The value to compare the flag to
 */
void ScriptInterpreter::cmdIfFlag(byte flagId, bool value) {
    debug(1, "Script: If flag %d == %d", flagId, value);
    
    // Check the flag
    bool flagValue = false;
    if (flagId < MAX_FLAGS)
        flagValue = _vm->getFlag(flagId);
        
    // If the condition is false, skip the next command
    if (flagValue != value) {
        // Skip the next command
        byte nextCmd = readByte();
        
        switch (nextCmd) {
        case 0x01: // Say
            readString(); // Skip the string
            break;
            
        case 0x02: // Move
            readByte(); // Skip the direction
            break;
            
        case 0x03: // Show picture
        case 0x04: // Play sound
        case 0x07: // Goto
        case 0x08: // Wait
            readWord(); // Skip the parameter
            break;
            
        case 0x05: // Set flag
        case 0x06: // If flag
            readByte(); // Skip the flag ID
            readByte(); // Skip the value
            break;
            
        case 0xFF: // End
            // No parameters to skip
            break;
            
        default:
            warning("Unknown script command to skip: 0x%02X at position %d", nextCmd, _scriptPos - 1);
            break;
        }
    }
}

/**
 * Jump to a position in the script
 * 
 * @param offset The offset to jump to
 */
void ScriptInterpreter::cmdGoto(uint16 offset) {
    debug(1, "Script: Goto %d", offset);
    
    // Jump to the specified position
    if (offset < _scriptSize)
        _scriptPos = offset;
    else
        warning("Script: Goto offset %d is out of bounds (script size: %d)", offset, _scriptSize);
}

/**
 * Wait for a number of ticks
 * 
 * @param ticks The number of ticks to wait
 */
void ScriptInterpreter::cmdWait(uint16 ticks) {
    debug(1, "Script: Wait %d ticks", ticks);
    
    // Wait for the specified number of ticks
    // This would use the timing system
    // For now, we'll just use a placeholder
    // _vm->wait(ticks);
}

/**
 * End the script
 */
void ScriptInterpreter::cmdEnd() {
    debug(1, "Script: End");
    
    // End the script
    _scriptRunning = false;
}

} // End of namespace Ack
