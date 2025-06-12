// engines/exult/exult_engine.cpp

#include "exult_engine.h"
#include "exult_graphics_adapter.h"
#include "exult_input_adapter.h"
#include "exult_audio_adapter.h"
#include "exult_file_adapter.h"

#include "common/system.h"
#include "common/file.h"
#include "common/archive.h"
#include "common/config-manager.h"
#include "common/debug.h"
#include "common/events.h"
#include "common/timer.h"
#include "graphics/surface.h"
#include "audio/mixer.h"
#include "engines/metaengine.h" // For engine registration and DetectionLevel
#include "graphics/VectorRenderer.h" // Corrected include for Color
#include "common/error.h" // For Common::Error and kNoError/kErrorSystem
#include "engines/game.h" // For ScummVM::GameDescription and PlainGameList

// Exult core includes
#include "exult_core_src/exult.h"
#include "exult_core_src/game.h"
#include "exult_core_src/gamerend.h"
#include "exult_core_src/exult_scummvm_api.h"

namespace ScummVM {
namespace Exult {

// --- ExultEngine Implementation ---

ExultEngine::ExultEngine(OSystem *system, const Common::FSNode& gamePath, const Common::String& gameLanguage)
    : Engine(system),
      _graphicsAdapter(nullptr),
      _inputAdapter(nullptr),
      _audioAdapter(nullptr),
      _fileAdapter(nullptr),
      _initialized(false),
      _gamePath(gamePath) {
    debug(1, "ExultEngine: Constructor called.");

    // Instantiate adapters
    _fileAdapter = new ExultFileAdapter(_system);
    _graphicsAdapter = new ExultGraphicsAdapter(_system);
    // EventManager is retrieved from the Engine base class using getEventManager()
    _inputAdapter = new ExultInputAdapter(_system, getEventManager()); 
    // _mixer is inherited from Engine, but ExultAudioAdapter constructor expects 3 args, not 2
    // For now, simplifying the constructor call. Will need to properly bridge audio later.
    _audioAdapter = new ExultAudioAdapter(_system, _mixer, nullptr); // Pass nullptr for ExultCore::Audio* for now
}

ExultEngine::~ExultEngine() {
    debug(1, "ExultEngine: Destructor called.");
    if (_initialized) {
        shutdown(); 
    }
    delete _graphicsAdapter; _graphicsAdapter = nullptr;
    delete _inputAdapter;   _inputAdapter = nullptr;
    delete _audioAdapter;   _audioAdapter = nullptr;
    delete _fileAdapter;    _fileAdapter = nullptr;
}

Common::Error ExultEngine::initialize(const Common::FSNode& gamePath, const Common::String& gameLanguage) {
    debug(1, "ExultEngine: initialize() called for gamePath: %s", gamePath.getPath().toString().c_str());
    _gamePath = gamePath;

    if (!_fileAdapter || !_graphicsAdapter || !_inputAdapter || !_audioAdapter) {
        error("ExultEngine: One or more adapters are null during initialize!");
        return Common::Error(Common::kUnknownError);
    }

    // Initialize adapters first, as Exult core might depend on them
    if (!_fileAdapter->init(gamePath)) {
        error("ExultEngine: Failed to initialize File Adapter.");
        return Common::Error(Common::kUnknownError);
    }
    if (!_graphicsAdapter->init()) {
        error("ExultEngine: Failed to initialize Graphics Adapter.");
        return Common::Error(Common::kUnknownError);
    }
    if (!_audioAdapter->init()) {
        error("ExultEngine: Failed to initialize Audio Adapter.");
        return Common::Error(Common::kUnknownError);
    }

    // Initialize Exult core
    Exult::Init();

    _initialized = true;
    debug(1, "ExultEngine: Placeholder initialization complete. Adapters and Exult core initialized.");
    return Common::kNoError;
}

void ExultEngine::shutdown() {
    debug(1, "ExultEngine: shutdown() called.");
    if (!_initialized) {
        return;
    }

    if (_audioAdapter) _audioAdapter->shutdown();

    _initialized = false;
    debug(1, "ExultEngine: Placeholder shutdown complete.");
}

Common::Error ExultEngine::run() {
    debug(1, "ExultEngine: run() called.");
    if (!_initialized) {
        warning("ExultEngine::run() called before successful initialization.");
        return Common::Error(Common::kUnknownError);
    }

    // ScummVM's main loop will call processInputEvents, updateGameLogic, and renderFrame.
    // This function should ideally not block and just set up the engine to be driven by those calls.
    // For now, the Exult::Play() call is removed, and the logic is moved to the respective methods.

    debug(1, "ExultEngine: run() setup complete. ScummVM will drive the game loop.");
    return Common::kNoError;
}

void ExultEngine::processInputEvents() {
    if (_inputAdapter) {
        _inputAdapter->processScummVMEvents();
    }
    // Call Exult's event processing
    Exult::processEvents();
}

void ExultEngine::updateGameLogic() {
    debug(1, "ExultEngine: updateGameLogic() called.");
    Exult::updateLogic();
}

void ExultEngine::renderFrame() {
    debug(1, "ExultEngine: renderFrame() called.");
    Exult::renderFrame();
    if (_graphicsAdapter) {
        _graphicsAdapter->renderExultFrame(); // This might be redundant if Exult::renderFrame() already draws to the ScummVM surface
    }
}

// --- ExultMetaEngine Implementation ---

ExultMetaEngine::ExultMetaEngine() : MetaEngineDetection() {
    debug(1, "ExultMetaEngine: Constructor called.");
}

Common::Error ExultMetaEngine::identifyGame(DetectedGame &game, const void **descriptor) {
    debug(1, "ExultMetaEngine: identifyGame() called for path: %s", game.path.toString().c_str());

    // Actual game detection logic for Ultima VII games.
    // game.path is Common::Path, convert to Common::FSNode to use getChild
    Common::FSNode gamePathFSNode(game.path);
    Common::FSNode gamedatDir = gamePathFSNode.getChild("GAMEDAT");
    Common::FSNode staticDir = gamePathFSNode.getChild("STATIC");

    // Check for specific files within GAMEDAT and STATIC directories for more robust detection
    bool isUltima7 = false;
    if (gamedatDir.exists() && staticDir.exists()) {
        // Example: Check for specific files like "GAMEDAT/GAME.DAT" and "STATIC/STATIC.DAT"
        if (gamedatDir.getChild("GAME.DAT").exists() && staticDir.getChild("STATIC.DAT").exists()) {
            isUltima7 = true;
        }
    }

    if (isUltima7) {
        game.engineId = getName();
        game.gameId = "ultima7"; // Placeholder game ID
        game.description = "Ultima VII: The Black Gate (Detected by Exult Engine)";
        game.canBeAdded = true;
        *descriptor = nullptr; // No specific descriptor needed for now
        return Common::kNoError;
    }

    return Common::Error(Common::kUnknownError); // Indicate no game detected
}

PlainGameList ExultMetaEngine::getSupportedGames() const {
    PlainGameList games;
    // Placeholder for adding supported game descriptions
    games.push_back(PlainGameDescriptor::of("ultima7", "Ultima VII: The Black Gate"));
    games.push_back(PlainGameDescriptor::of("ultima7si", "Ultima VII Part Two: Serpent Isle"));
    return games;
}

DetectedGames ExultMetaEngine::detectGames(const Common::FSList &fslist, uint32 skipADFlags, bool skipIncomplete) {
    DetectedGames detectedGames;
    debug(1, "ExultMetaEngine: detectGames() called. Implementing placeholder logic.");
    // Iterate through fslist and call identifyGame for each potential game path
    for (const Common::FSNode& fsNode : fslist) {
        DetectedGame game;
        game.path = fsNode.getPath();
        const void* descriptor = nullptr;
        if (identifyGame(game, &descriptor) == Common::kNoError) {
            detectedGames.push_back(game);
        }
    }
    return detectedGames;
}

void ExultMetaEngine::dumpDetectionEntries() const {
    debug(1, "ExultMetaEngine: dumpDetectionEntries() called. Placeholder implementation.");
    // In a real scenario, this would iterate through supported games and print their detection info.
    PlainGameList supportedGames = getSupportedGames();
    for (const PlainGameDescriptor& desc : supportedGames) {
        debug(1, "  Supported Game: ID=%s, Description=%s", desc.gameId.c_str(), desc.description.c_str());
    }
}

// These are not part of MetaEngineDetection, but are needed for the engine to function
// They will be called by the main ScummVM application through the MetaEngine interface
Engine *ExultMetaEngine::createInstance(OSystem *syst, const Common::FSNode& gamePath, const Common::String& gameLanguage, const void *meDesc) {
    return new ExultEngine(syst, gamePath, gameLanguage);
}

void ExultMetaEngine::freeInstance(Engine *engine) {
    delete engine;
}

} // namespace Exult
} // namespace ScummVM




