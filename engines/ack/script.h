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

#ifndef ACK_SCRIPT_H
#define ACK_SCRIPT_H

#include "common/scummsys.h"
#include "common/str.h"
#include "common/array.h"

namespace Ack {

class AckEngine;

/**
 * Script interpreter for ACK engine
 * Handles script loading, parsing, and execution
 */
class ScriptInterpreter {
private:
    AckEngine *_vm;
    
    // Script data
    byte *_scriptData;
    uint32 _scriptSize;
    uint32 _scriptPos;
    
    // Script execution state
    bool _scriptRunning;
    bool _waitForInput;
    
    // Helper functions
    byte readByte();
    uint16 readWord();
    Common::String readString();
    
public:
    ScriptInterpreter(AckEngine *vm);
    ~ScriptInterpreter();
    
    // Script loading and execution
    bool loadScript(const Common::String &filename);
    bool runScript();
    bool executeCommand();
    void stopScript();
    
    // Script state
    bool isScriptRunning() const { return _scriptRunning; }
    bool isWaitingForInput() const { return _waitForInput; }
    void setWaitForInput(bool wait) { _waitForInput = wait; }
    
    // Script commands
    void cmdSay(const Common::String &text);
    void cmdMove(byte direction);
    void cmdShowPicture(uint16 pictureId);
    void cmdPlaySound(uint16 soundId);
    void cmdSetFlag(byte flagId, bool value);
    void cmdIfFlag(byte flagId, bool value);
    void cmdGoto(uint16 offset);
    void cmdWait(uint16 ticks);
    void cmdEnd();
};

} // End of namespace Ack

#endif // ACK_SCRIPT_H
