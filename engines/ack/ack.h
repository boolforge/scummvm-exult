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

#ifndef ACK_ACK_H
#define ACK_ACK_H

#include "common/scummsys.h"
#include "common/error.h"
#include "common/util.h"
#include "common/file.h"
#include "common/keyboard.h"
#include "common/rect.h"
#include "common/rendermode.h"
#include "common/stack.h"
#include "common/str.h"
#include "common/system.h"
#include "engines/engine.h"
#include "gui/debugger.h"

#include "ack/structs.h"

namespace Common {
class RandomSource;
}

/**
 * This is the namespace of the ACK engine.
 *
 * Status of this engine: In Development
 *
 * Games using this engine:
 * - Adventure Construction Kit games
 */
namespace Ack {

#define TITLE "ACK engine"
#define MAX_DIRECTORY_ENTRIES 256
#define MAX_VARS 256
#define MAX_FLAGS 256
#define MAX_STRINGS 24
#define MAX_STRINGLEN 40
#define SCREENOBJECTS_MAX 255

class Console;
class Graphics;
class GraphicsRenderer;
class Sound;
class Input;
class ResourceManager;
class ScriptInterpreter;

enum kDebugLevels {
    kDebugLevelMain = 1,
    kDebugLevelResources,
    kDebugLevelSprites,
    kDebugLevelPictures,
    kDebugLevelInventory,
    kDebugLevelInput,
    kDebugLevelMenu,
    kDebugLevelScripts,
    kDebugLevelSound,
    kDebugLevelText,
    kDebugLevelSavegame
};

/**
 * ACK resources.
 */
enum {
    RESOURCETYPE_LOGIC = 1,
    RESOURCETYPE_SOUND,
    RESOURCETYPE_VIEW,
    RESOURCETYPE_PICTURE
};

enum {
    RES_LOADED = 0x01,
    RES_COMPRESSED = 0x40
};

/**
 * ACK engine class.
 * This class contains all global data of an ACK game executed by ScummVM.
 */
class AckEngine : public Engine {
private:
    Common::RandomSource *_rnd;
    Graphics *_graphics;
    GraphicsRenderer *_graphicsRenderer;
    Sound *_sound;
    Input *_input;
    ResourceManager *_resourceManager;
    ScriptInterpreter *_scriptInterpreter;
    Console *_console;

    // Game data
    byte _flags[MAX_FLAGS];
    byte _vars[MAX_VARS];
    char _strings[MAX_STRINGS][MAX_STRINGLEN];

    // Resource directories
    AckDir _dirView[MAX_DIRECTORY_ENTRIES];
    AckDir _dirPic[MAX_DIRECTORY_ENTRIES];
    AckDir _dirLogic[MAX_DIRECTORY_ENTRIES];
    AckDir _dirSound[MAX_DIRECTORY_ENTRIES];

    // Resources
    ViewRecord _views[MAX_DIRECTORY_ENTRIES];
    PictureRecord _pictures[MAX_DIRECTORY_ENTRIES];
    LogicRecord _logics[MAX_DIRECTORY_ENTRIES];
    AckSound *_sounds[MAX_DIRECTORY_ENTRIES];

    // Game objects
    Common::Array<AckObject> _objects;
    
    // Game state
    MasterRec _masterRec;
    Common::String _systemDir;
    Common::String _advName;
    Common::String _masterFile;
    GrapArray256 *_icons;
    bool _mouseActive;

    // Friends
    friend class ResourceManager;

public:
    AckEngine(OSystem *syst, const AckGameDescription *gameDesc);
    ~AckEngine() override;

    Common::Error run() override;
    bool hasFeature(EngineFeature f) const override;

    // Engine APIs
    Common::Error loadGameState(int slot) override;
    Common::Error saveGameState(int slot, const Common::String &desc, bool isAutosave = false) override;
    bool canLoadGameStateCurrently() override;
    bool canSaveGameStateCurrently() override;

    // Game functions
    void initGame();
    void gameLoop();
    void shutdown();

    // Resource management
    int loadResource(int16 resourceType, int16 resourceNr);
    void unloadResource(int16 resourceType, int16 resourceNr);

    // Accessors
    Graphics *getGraphics() const { return _graphics; }
    GraphicsRenderer *getGraphicsRenderer() const { return _graphicsRenderer; }
    Sound *getSound() const { return _sound; }
    Input *getInput() const { return _input; }
    ResourceManager *getResourceManager() const { return _resourceManager; }
    ScriptInterpreter *getScriptInterpreter() const { return _scriptInterpreter; }
    Console *getConsole() const { return _console; }
    
    // Game state accessors
    bool getFlag(byte flagId) const { return _flags[flagId] != 0; }
    void setFlag(byte flagId, bool value) { _flags[flagId] = value ? 1 : 0; }
    byte getVar(byte varId) const { return _vars[varId]; }
    void setVar(byte varId, byte value) { _vars[varId] = value; }
    const char *getString(byte stringId) const { return _strings[stringId]; }
    void setString(byte stringId, const char *value);
    
    // Game configuration
    void setSystemDir(const Common::String &dir) { _systemDir = dir; }
    Common::String getSystemDir() const { return _systemDir; }
    void setAdvName(const Common::String &name) { _advName = name; }
    Common::String getAdvName() const { return _advName; }
    void setMasterFile(const Common::String &file) { _masterFile = file; }
    Common::String getMasterFile() const { return _masterFile; }
    void setMasterRec(const MasterRec &rec) { _masterRec = rec; }
    const MasterRec &getMasterRec() const { return _masterRec; }
    
    // Icon management
    GrapArray256 *getIcons() const { return _icons; }
    
    // Mouse state
    bool isMouseActive() const { return _mouseActive; }
    void setMouseActive(bool active) { _mouseActive = active; }
};

} // End of namespace Ack

#endif // ACK_ACK_H
