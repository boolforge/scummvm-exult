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

#include "common/config-manager.h"
#include "common/debug.h"
#include "common/error.h"
#include "common/events.h"
#include "common/file.h"
#include "common/fs.h"
#include "common/system.h"

#include "engines/util.h"

#include "ack/ack.h"
#include "ack/detection.h"
#include "ack/graphics.h"
#include "ack/graphics_renderer.h"
#include "ack/input.h"
#include "ack/resource.h"
#include "ack/script.h"
#include "ack/sound.h"
#include "ack/sprite.h"
#include "ack/picture.h"
#include "ack/console.h"

namespace Ack {

AckEngine::AckEngine(OSystem *syst, const AckGameDescription *gameDesc) : Engine(syst) {
    // Initialize debug channels
    DebugMan.addDebugChannels(debugFlagList);

    // Initialize random number generator
    _rnd = new Common::RandomSource("ack");

    // Initialize subsystems
    _graphics = new Graphics(this);
    _graphicsRenderer = new GraphicsRenderer(this);
    _sound = new Sound(this);
    _input = new Input(this);
    _resourceManager = new ResourceManager(this);
    _scriptInterpreter = new ScriptInterpreter(this);
    _console = new Console(this);

    // Clear game data
    memset(_flags, 0, sizeof(_flags));
    memset(_vars, 0, sizeof(_vars));
    memset(_strings, 0, sizeof(_strings));

    // Clear resource directories
    for (int i = 0; i < MAX_DIRECTORY_ENTRIES; i++) {
        _dirView[i].reset();
        _dirPic[i].reset();
        _dirLogic[i].reset();
        _dirSound[i].reset();
        
        _sounds[i] = nullptr;
    }
    
    // Initialize game state
    _systemDir = ConfMan.get("path");
    _advName = "NONAME";
    _masterFile = "MASTER.DAT";
    _icons = new GrapArray256();
    _mouseActive = false;
    
    // Set the game description
    _gameDescription = gameDesc;
}

AckEngine::~AckEngine() {
    // Free subsystems
    delete _console;
    delete _scriptInterpreter;
    delete _resourceManager;
    delete _input;
    delete _sound;
    delete _graphicsRenderer;
    delete _graphics;
    delete _rnd;
    delete _icons;
    
    // Free resources
    for (int i = 0; i < MAX_DIRECTORY_ENTRIES; i++) {
        if (_sounds[i]) {
            delete _sounds[i];
            _sounds[i] = nullptr;
        }
    }
}

Common::Error AckEngine::run() {
    // Initialize the graphics mode
    initGraphics(320, 200);

    // Initialize the game
    initGame();

    // Run the game
    gameLoop();

    // Shutdown
    shutdown();

    return Common::kNoError;
}

bool AckEngine::hasFeature(EngineFeature f) const {
    return
        (f == kSupportsReturnToLauncher) ||
        (f == kSupportsLoadingDuringRuntime) ||
        (f == kSupportsSavingDuringRuntime);
}

Common::Error AckEngine::loadGameState(int slot) {
    // Create the save file name
    Common::String filename = getSaveStateName(slot);
    
    // Open the save file
    Common::InSaveFile *in = _saveFileMan->openForLoading(filename);
    if (!in)
        return Common::kReadingFailed;
    
    // Read the save file header
    char header[4];
    in->read(header, 4);
    if (memcmp(header, "ACKS", 4) != 0) {
        delete in;
        return Common::kReadingFailed;
    }
    
    // Read the save file version
    uint32 version = in->readUint32LE();
    if (version != 1) {
        delete in;
        return Common::kReadingFailed;
    }
    
    // Read the game state
    // Flags
    in->read(_flags, sizeof(_flags));
    
    // Variables
    in->read(_vars, sizeof(_vars));
    
    // Strings
    for (int i = 0; i < MAX_STRINGS; i++) {
        uint8 len = in->readByte();
        in->read(_strings[i], len);
        _strings[i][len] = '\0';
    }
    
    // Objects
    uint32 numObjects = in->readUint32LE();
    _objects.clear();
    for (uint32 i = 0; i < numObjects; i++) {
        AckObject obj;
        obj.location = in->readSint32LE();
        
        uint8 nameLen = in->readByte();
        char name[256];
        in->read(name, nameLen);
        name[nameLen] = '\0';
        obj.name = name;
        
        _objects.push_back(obj);
    }
    
    // Close the save file
    delete in;
    
    return Common::kNoError;
}

Common::Error AckEngine::saveGameState(int slot, const Common::String &desc, bool isAutosave) {
    // Create the save file name
    Common::String filename = getSaveStateName(slot);
    
    // Open the save file
    Common::OutSaveFile *out = _saveFileMan->openForSaving(filename);
    if (!out)
        return Common::kWritingFailed;
    
    // Write the save file header
    out->write("ACKS", 4);
    
    // Write the save file version
    out->writeUint32LE(1);
    
    // Write the game state
    // Flags
    out->write(_flags, sizeof(_flags));
    
    // Variables
    out->write(_vars, sizeof(_vars));
    
    // Strings
    for (int i = 0; i < MAX_STRINGS; i++) {
        uint8 len = strlen(_strings[i]);
        out->writeByte(len);
        out->write(_strings[i], len);
    }
    
    // Objects
    out->writeUint32LE(_objects.size());
    for (uint i = 0; i < _objects.size(); i++) {
        out->writeSint32LE(_objects[i].location);
        
        uint8 nameLen = _objects[i].name.size();
        out->writeByte(nameLen);
        out->write(_objects[i].name.c_str(), nameLen);
    }
    
    // Close the save file
    out->finalize();
    delete out;
    
    return Common::kNoError;
}

bool AckEngine::canLoadGameStateCurrently() {
    // Check if loading is possible
    return !_scriptInterpreter->isScriptRunning();
}

bool AckEngine::canSaveGameStateCurrently() {
    // Check if saving is possible
    return !_scriptInterpreter->isScriptRunning();
}

void AckEngine::initGame() {
    debug(1, "Initializing ACK engine");

    // Load configuration
    _resourceManager->loadConfig();
    
    // Load icons
    _graphicsRenderer->loadIcons("ICONS.DAT");
    
    // Initialize graphics
    _graphics->initGraphics();
    
    // Initialize sound
    _sound->setVolume(255); // Full volume
    
    // Initialize input
    _input->clearKeyboardBuffer();
    
    // Load the main script if available
    if (Common::File::exists(_systemDir + "/MAIN.SCR")) {
        _scriptInterpreter->loadScript(_systemDir + "/MAIN.SCR");
    }
}

void AckEngine::gameLoop() {
    debug(1, "Starting game loop");

    // Main game loop
    bool quit = false;
    while (!quit && !shouldQuit()) {
        // Process events
        _input->processEvents();
        
        // Run script interpreter if active
        if (_scriptInterpreter->isScriptRunning()) {
            _scriptInterpreter->runScript();
        }

        // Update game state
        // TODO: Implement game state update

        // Render graphics
        _graphics->updateScreen();

        // Delay
        g_system->delayMillis(10);
    }
}

void AckEngine::shutdown() {
    debug(1, "Shutting down ACK engine");

    // Stop all sounds
    for (int i = 0; i < 4; i++) {
        _sound->stopSound(i);
    }
    _sound->stopMusic();
    
    // Unload all resources
    for (int i = 0; i < MAX_DIRECTORY_ENTRIES; i++) {
        unloadResource(RESOURCETYPE_LOGIC, i);
        unloadResource(RESOURCETYPE_SOUND, i);
        unloadResource(RESOURCETYPE_VIEW, i);
        unloadResource(RESOURCETYPE_PICTURE, i);
    }
}

int AckEngine::loadResource(int16 resourceType, int16 resourceNr) {
    return _resourceManager->loadResource(resourceType, resourceNr);
}

void AckEngine::unloadResource(int16 resourceType, int16 resourceNr) {
    _resourceManager->unloadResource(resourceType, resourceNr);
}

void AckEngine::setString(byte stringId, const char *value) {
    if (stringId < MAX_STRINGS) {
        strncpy(_strings[stringId], value, MAX_STRINGLEN - 1);
        _strings[stringId][MAX_STRINGLEN - 1] = '\0';
    }
}

Common::String AckEngine::getSaveStateName(int slot) const {
    return Common::String::format("%s.%03d", _targetName.c_str(), slot);
}

uint32 AckEngine::getFeatures() const {
    return _gameDescription->desc.flags;
}

Common::Language AckEngine::getLanguage() const {
    return _gameDescription->desc.language;
}

Common::Platform AckEngine::getPlatform() const {
    return _gameDescription->desc.platform;
}

bool AckEngine::isDemo() const {
    return _gameDescription->desc.flags & ADGF_DEMO;
}

} // End of namespace Ack
