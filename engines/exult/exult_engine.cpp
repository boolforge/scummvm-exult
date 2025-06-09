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
        return Common::kErrorSystem;
    }

    // Initialize adapters first, as Exult core might depend on them
    if (!_fileAdapter->init(gamePath)) {
        error("ExultEngine: Failed to initialize File Adapter.");
        return Common::kErrorSystem;
    }
    if (!_graphicsAdapter->init()) {
        error("ExultEngine: Failed to initialize Graphics Adapter.");
        return Common::kErrorSystem;
    }
    if (!_audioAdapter->init()) {
        error("ExultEngine: Failed to initialize Audio Adapter.");
        return Common::kErrorSystem;
    }

    _initialized = true;
    debug(1, "ExultEngine: Placeholder initialization complete. Adapters initialized.");
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

void ExultEngine::run() {
    debug(1, "ExultEngine: run() called.");
    if (!_initialized) {
        warning("ExultEngine::run() called before successful initialization.");
        return;
    }

    while (!shouldQuit()) {
        processInputEvents();
        updateGameLogic();
        renderFrame();
        // _system->yield(); // OSystem::yield() is not a direct member. Needs proper event loop integration.
    }

    debug(1, "ExultEngine: run() loop finished.");
}

void ExultEngine::processInputEvents() {
    if (_inputAdapter) {
        _inputAdapter->processScummVMEvents();
    } else {
        Common::Event event;
        while (getEventManager()->pollEvent(event)) { /* Discard */ }
    }
}

void ExultEngine::updateGameLogic() {
    // TODO: Call Exult core game state update function.
}

void ExultEngine::renderFrame() {
    if (_graphicsAdapter) {
        _graphicsAdapter->renderExultFrame();
    } else {
        // Fallback: clear screen or show error if graphics adapter is missing
        // Graphics::Surface *screen = _system->lockScreenSurface(); // Not a direct member of OSystem
        // if (screen) {
        //     screen->clear(Graphics::Color(128, 0, 0)); // Red to indicate error
        //     _system->unlockScreenSurface(true); // Not a direct member of OSystem
        // }
    }
}

// --- ExultMetaEngine Implementation ---

ExultMetaEngine::ExultMetaEngine() : MetaEngine() {
    debug(1, "ExultMetaEngine: Constructor called.");
}

bool ExultMetaEngine::canDetect(OSystem *syst, const Common::FSNode& node, DetectionLevel level) const {
    debug(1, "ExultMetaEngine: canDetect() called for path: %s", node.getPath().toString().c_str());

    // Actual game detection logic for Ultima VII games.
    Common::FSNode gamedatDir = node.getChild("GAMEDAT");
    Common::FSNode staticDir = node.getChild("STATIC");

    // Placeholder for actual detection logic
    return gamedatDir.exists() && staticDir.exists();
}

Engine *ExultMetaEngine::createInstance(OSystem *syst, const Common::FSNode& gamePath, const Common::String& gameLanguage, const void *meDesc) {
    return new ExultEngine(syst, gamePath, gameLanguage);
}

void ExultMetaEngine::getSupportedGames(Common::Array<GameDescription> &games) const {
    // Placeholder for adding supported game descriptions
    GameDescription desc;
    desc.id = "ultima7";
    desc.description = "Ultima VII: The Black Gate";
    desc.engine = "Exult";
    desc.language = "en";
    games.push_back(desc);

    desc.id = "ultima7si";
    desc.description = "Ultima VII Part Two: Serpent Isle";
    desc.engine = "Exult";
    desc.language = "en";
    games.push_back(desc);
}

void ExultMetaEngine::freeInstance(Engine *engine) {
    delete engine;
}

} // namespace Exult
} // namespace ScummVM


