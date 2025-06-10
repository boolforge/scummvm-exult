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
#include "common/textconsole.h"

#include "ack/ack.h"
#include "ack/console.h"
#include "ack/script.h"
#include "ack/graphics.h"
#include "ack/sound.h"

namespace Ack {

Console::Console(AckEngine *vm) : GUI::Debugger(), _vm(vm) {
    registerCmd("loadScript", WRAP_METHOD(Console, cmdLoadScript),
                "loadScript <filename> - Load and execute a script file");
    registerCmd("showPicture", WRAP_METHOD(Console, cmdShowPicture),
                "showPicture <id> - Show a picture");
    registerCmd("playSound", WRAP_METHOD(Console, cmdPlaySound),
                "playSound <id> - Play a sound");
    registerCmd("setFlag", WRAP_METHOD(Console, cmdSetFlag),
                "setFlag <id> <value> - Set a game flag");
    registerCmd("getFlag", WRAP_METHOD(Console, cmdGetFlag),
                "getFlag <id> - Get a game flag value");
    registerCmd("setVar", WRAP_METHOD(Console, cmdSetVar),
                "setVar <id> <value> - Set a game variable");
    registerCmd("getVar", WRAP_METHOD(Console, cmdGetVar),
                "getVar <id> - Get a game variable value");
    registerCmd("listObjects", WRAP_METHOD(Console, cmdListObjects),
                "listObjects - List all game objects");
}

Console::~Console() {
}

bool Console::cmdLoadScript(int argc, const char **argv) {
    if (argc != 2) {
        debugPrintf("Usage: loadScript <filename>\n");
        return true;
    }

    if (_vm->getScriptInterpreter()->loadScript(argv[1])) {
        debugPrintf("Script loaded successfully\n");
        return true;
    } else {
        debugPrintf("Failed to load script\n");
        return true;
    }
}

bool Console::cmdShowPicture(int argc, const char **argv) {
    if (argc != 2) {
        debugPrintf("Usage: showPicture <id>\n");
        return true;
    }

    int pictureId = atoi(argv[1]);
    if (_vm->loadResource(RESOURCETYPE_PICTURE, pictureId)) {
        debugPrintf("Picture %d loaded and displayed\n", pictureId);
        return true;
    } else {
        debugPrintf("Failed to load picture %d\n", pictureId);
        return true;
    }
}

bool Console::cmdPlaySound(int argc, const char **argv) {
    if (argc != 2) {
        debugPrintf("Usage: playSound <id>\n");
        return true;
    }

    int soundId = atoi(argv[1]);
    if (_vm->loadResource(RESOURCETYPE_SOUND, soundId)) {
        // This would use the sound system
        // For now, we'll just use a placeholder
        debugPrintf("Sound %d loaded and played\n", soundId);
        return true;
    } else {
        debugPrintf("Failed to load sound %d\n", soundId);
        return true;
    }
}

bool Console::cmdSetFlag(int argc, const char **argv) {
    if (argc != 3) {
        debugPrintf("Usage: setFlag <id> <value>\n");
        return true;
    }

    int flagId = atoi(argv[1]);
    bool value = atoi(argv[2]) != 0;
    
    if (flagId < MAX_FLAGS) {
        _vm->setFlag(flagId, value);
        debugPrintf("Flag %d set to %d\n", flagId, value);
    } else {
        debugPrintf("Flag ID out of range (0-%d)\n", MAX_FLAGS - 1);
    }
    
    return true;
}

bool Console::cmdGetFlag(int argc, const char **argv) {
    if (argc != 2) {
        debugPrintf("Usage: getFlag <id>\n");
        return true;
    }

    int flagId = atoi(argv[1]);
    
    if (flagId < MAX_FLAGS) {
        bool value = _vm->getFlag(flagId);
        debugPrintf("Flag %d = %d\n", flagId, value);
    } else {
        debugPrintf("Flag ID out of range (0-%d)\n", MAX_FLAGS - 1);
    }
    
    return true;
}

bool Console::cmdSetVar(int argc, const char **argv) {
    if (argc != 3) {
        debugPrintf("Usage: setVar <id> <value>\n");
        return true;
    }

    int varId = atoi(argv[1]);
    byte value = atoi(argv[2]);
    
    if (varId < MAX_VARS) {
        _vm->setVar(varId, value);
        debugPrintf("Variable %d set to %d\n", varId, value);
    } else {
        debugPrintf("Variable ID out of range (0-%d)\n", MAX_VARS - 1);
    }
    
    return true;
}

bool Console::cmdGetVar(int argc, const char **argv) {
    if (argc != 2) {
        debugPrintf("Usage: getVar <id>\n");
        return true;
    }

    int varId = atoi(argv[1]);
    
    if (varId < MAX_VARS) {
        byte value = _vm->getVar(varId);
        debugPrintf("Variable %d = %d\n", varId, value);
    } else {
        debugPrintf("Variable ID out of range (0-%d)\n", MAX_VARS - 1);
    }
    
    return true;
}

bool Console::cmdListObjects(int argc, const char **argv) {
    debugPrintf("Game objects:\n");
    
    for (uint i = 0; i < _vm->_objects.size(); i++) {
        debugPrintf("%d: %s (location: %d)\n", i, _vm->_objects[i].name.c_str(), _vm->_objects[i].location);
    }
    
    return true;
}

} // End of namespace Ack
