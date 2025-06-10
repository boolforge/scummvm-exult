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
// #include "common/eventpipe.h" // Removed, functionality likely in common/events.h
#include "common/events.h"
#include "common/timer.h"
#include "graphics/surface.h"
#include "audio/mixer.h"
#include "engines/metaengine.h" // For engine registration

// Exult core includes
#include "exult_core_src/conf/Configuration.h"
#include "exult_core_src/gamemgr/modmgr.h" // For ExultCore::GameManager and BaseGameInfo
#include "exult_core_src/game.h"          // For ExultCore::Game and Game::create_game
#include "exult_core_src/gamewin.h"       // For ExultCore::Game_window
#include "exult_core_src/imagewin.h"      // For ExultCore::Image_window constants
#include "exult_core_src/keys.h"          // For ExultCore::KeyBinder
#include "exult_core_src/mouse.h"         // For ExultCore::Mouse
#include "exult_core_src/audio/Audio.h"   // For ExultCore::Audio
#include "exult_core_src/tqueue.h"        // For ExultCore::Time_queue (used by Game_window)
#include "exult_core_src/ucmachine.h"     // For ExultCore::Usecode_machine (used by Game_window)
#include "exult_core_src/paths.h"         // For setup_program_paths, setup_data_dir etc.
#include "exult_core_src/font.h"          // For Setup_text, Free_text
#include "exult_core_src/args.h"          // For EXULT_DATADIR (though this might be ScummVM managed)
#include "exult_core_src/exult_constants.h" // For game type constants
#include "exult_core_src/fnames.h"         // For BUNDLE_EXULT_FLX etc.
#include "exult_core_src/utils.h"          // For U7set_istream_factory etc.
#include "exult_core_src/sdlrwopsistream.h"
#include "exult_core_src/sdlrwopsostream.h"


// Placeholder for actual Exult includes and namespace
// These would be paths relative to the Exult source once it_s integrated
// or if using a library version of Exult.
// For now, we assume Exult_Engine_s core logic will be linked in or compiled alongside.
// #include "exult_bridge/game_manager_bridge.h" // Example of a bridge header
// #include "exult_bridge/resource_manager_bridge.h"

namespace ScummVM {
namespace Exult {

// --- ExultEngine Implementation ---

ExultEngine::ExultEngine(OSystem *system, const Common::FSNode& gamePath, const Common::String& gameLanguage)
    : Engine(system),
      _graphicsAdapter(nullptr),
      _inputAdapter(nullptr),
      _audioAdapter(nullptr),
      _fileAdapter(nullptr),
      _exultConfig(nullptr),             // Initialize new member
      _exultGameManagerInstance(nullptr), // Initialize new member
      _exultGameInstance(nullptr),       // Initialize new member
      _exultGameWindow(nullptr),         // Initialize new member
      // _exultGameManager(nullptr),
      // _exultUsecodeVM(nullptr),
      _initialized(false),
      _gamePath(gamePath) {
    debug(1, "ExultEngine: Constructor called.");

    // Instantiate adapters
    _fileAdapter = new ExultFileAdapter(_system);
    _graphicsAdapter = new ExultGraphicsAdapter(_system);
    // EventManager is retrieved from the Engine base class using getEventManager()
    // Pass nullptr for Exult core components initially; they will be fetched in ExultInputAdapter::init()
    _inputAdapter = new ExultInputAdapter(_system, getEventManager(), nullptr, nullptr, nullptr);
    // Pass nullptr for ExultCore::Audio initially; it will be fetched in ExultAudioAdapter::init()
    _audioAdapter = new ExultAudioAdapter(_system, _mixer, nullptr);
}

ExultEngine::~ExultEngine() {
    debug(1, "ExultEngine: Destructor called.");
    // Ensure shutdown is called if not already.
    // The shutdown method should handle deletion of adapters.
    if (_initialized) {
        shutdown(); 
    }
    // If not initialized, adapters might still be allocated if constructor succeeded partially
    // or if shutdown wasn_t called. Best practice is to delete them here too if not null.
    delete _graphicsAdapter; _graphicsAdapter = nullptr;
    delete _inputAdapter;   _inputAdapter = nullptr;
    delete _audioAdapter;   _audioAdapter = nullptr;
    delete _fileAdapter;    _fileAdapter = nullptr;

    // Cleanup Exult core objects
    // Order might matter: Game_window often depends on others.
    // Exult's original shutdown order: delete gwin, delete Mouse::mouse, Audio::Destroy, delete keybinder, delete config, delete game, delete gamemanager
    delete _exultGameWindow; _exultGameWindow = nullptr;
    // Mouse::mouse is static, managed by Game_window's constructor/destructor in Exult.
    // Audio is a singleton, ExultCore::Audio::Destroy() should be called.
    // Keybinder needs delete if we new'd it. (Exult's global keybinder is deleted in ~Play())
    // Config needs delete.
    // Game instance needs delete.
    // GameManager instance needs delete.
    // This simplified cleanup is likely insufficient and will be expanded in shutdown().
    delete _exultGameInstance; _exultGameInstance = nullptr;
    delete _exultGameManagerInstance; _exultGameManagerInstance = nullptr;
    delete _exultConfig; _exultConfig = nullptr;
    // ExultCore::Audio::Destroy(); // This will be called in shutdown
    // delete keybinder // This will be called in shutdown
}

Error ExultEngine::initialize(const Common::FSNode& gamePath, const Common::String& gameLanguage) {
    debug(1, "ExultEngine: initialize() called for gamePath: %s", gamePath.getPath().toString().c_str());
    _gamePath = gamePath;

    if (!_fileAdapter || !_graphicsAdapter || !_inputAdapter || !_audioAdapter) {
        error("ExultEngine: One or more adapters are null during initialize!");
        return Common::kErrorSystem;
    }

    // Initialize adapters first
    // File adapter needs gamePath
    if (!_fileAdapter->init(gamePath)) { // This will set up Exult's VFS to use ScummVM
        error("ExultEngine: Failed to initialize File Adapter.");
        return Common::kErrorSystem;
    }

    // Initialize Exult's core systems, similar to exult.cc:exult_main() and Init()
    // Path setup (might be partially covered by FileAdapter or need more here)
    // ExultCore::setup_program_paths(); // Needs careful review for ScummVM context
    // ExultCore::setup_data_dir(dataPathFromScummVM, runpathFromScummVM);
    // For now, assume _fileAdapter->init() has made paths accessible via Exult's U7FileManager.

		// Use SDL_RWops for file I/O (already done in exult.cc, ensure it's active)
		ExultCore::U7set_istream_factory(
			[](const char* s, std::ios_base::openmode mode) -> std::unique_ptr<std::istream> {
				return std::make_unique<ExultCore::SdlRwopsIstream>(s, mode);
			});
		ExultCore::U7set_ostream_factory(
			[](const char* s, std::ios_base::openmode mode) -> std::unique_ptr<std::ostream> {
				return std::make_unique<ExultCore::SdlRwopsOstream>(s, mode);
			});

    // 1. Configuration
    _exultConfig = new ExultCore::Configuration();
    // TODO: Load configuration. Exult does this from exult.cfg.
    // In ScummVM, this is usually handled by ScummVM's own config system (_conf).
    // We might need to bridge specific Exult settings from ScummVM's config.
    // For now, using default Exult config.
    // _exultConfig->read_config_file("exult.cfg"); // This path needs to be via ScummVM VFS.

    // 2. GameManager (manages game versions and mods)
    _exultGameManagerInstance = new ExultCore::GameManager(true /*silent*/); // true for silent to avoid cout/cerr

    // 3. SDL Init (minimal, as ScummVM handles most of it)
    // SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) is done by ScummVM.
    // Exult's Init() also does SDL_GameController related init.
    // SDL_SetEventFilter is also called in Exult's Init.

    // 4. Audio System (Singleton)
    ExultCore::Audio::Init(); // Initializes the Audio singleton and its internal Pentagram::AudioMixer

    // 5. Game_window (creates Image_window, Usecode_machine, Gump_manager, Palette, etc.)
    // This was already being created. Ensure its parameters are correct for ScummVM.
    int exultInternalWidth = 320;  // Native U7 resolution for rendering game world
    int exultInternalHeight = 200; // Often 200 for viewport, gumps are 320x240
                                   // These might need to be game specific (BG vs SI) via _conf.
    Graphics::Surface* screenSurf = _system->getScreenSurface();
    _exultGameWindow = new ExultCore::Game_window(
        screenSurf ? screenSurf->w : 640, screenSurf ? screenSurf->h : 480, // Window size (less critical for Exult if rendering offscreen)
        false, // Not fullscreen from Exult's perspective
        exultInternalWidth, exultInternalHeight, // Internal rendering size
        1, // Initial scale factor (ScummVM handles final scaling)
        ExultCore::Image_window::point, // Default scaler
        ExultCore::Image_window::AspectCorrectCentre,
        ExultCore::Image_window::point
    );
    if (!_exultGameWindow) {
        error("ExultEngine: Failed to create ExultCore::Game_window instance.");
        // delete _exultConfig; _exultConfig = nullptr;
        // delete _exultGameManagerInstance; _exultGameManagerInstance = nullptr;
        // ExultCore::Audio::Destroy();
        return Common::kErrorSystem;
    }
    // Exult's global gwin is set by Game_window constructor.
    // Mouse::mouse is created by Game_window constructor.
    // KeyBinder is created globally in Exult's standalone main.
    // We need to ensure it's created here for ScummVM.
    // Exult's global `keybinder` is assigned in exult.cc/exult_main before Init()
    // We should do the same or ensure KeyBinder::get_instance() works.
    // The KeyBinder constructor sets its static s_instance.
    if (!ExultCore::KeyBinder::get_instance()) {
         new ExultCore::KeyBinder(); // Creates and sets s_instance
    }
    ExultCore::KeyBinder::get_instance()->LoadDefaults(); // Load default keybindings
    // ExultCore::KeyBinder::get_instance()->LoadFromPatch(); // Optional: load user's custom keys

    // Initialize adapters that depend on Exult's core systems being up
    if (!_graphicsAdapter->init()) {
        error("ExultEngine: Failed to initialize Graphics Adapter.");
        return Common::kErrorSystem;
    }
    if (!_inputAdapter->init()) { // Fetches gwin components
        error("ExultEngine: Failed to initialize Input Adapter.");
        return Common::kErrorSystem;
    }
    if (!_audioAdapter->init()) { // Fetches Audio singleton, sets ScummVM mode
        error("ExultEngine: Failed to initialize Audio Adapter.");
        return Common::kErrorSystem;
    }

    // 6. Select Game and Mod (using ScummVM's game detection info)
    // ScummVM's `_game->id` (e.g., "ultima7bg", "ultima7si") needs to map to Exult's game identifiers.
    // Exult's GameManager uses names like "blackgate", "serpentisle".
    ExultCore::BaseGameInfo* selectedGameInfo = nullptr;
    Common::String gameId = _game->getGameId(); // From ScummVM's GameDescription
    if (gameId == "ultima7bg" || gameId == "exult-bg") { // exult-bg is from getSupportedGames
        selectedGameInfo = _exultGameManagerInstance->get_bg();
        if (!selectedGameInfo) selectedGameInfo = _exultGameManagerInstance->get_fov(); // Try FoV if plain BG not found
    } else if (gameId == "ultima7si" || gameId == "exult-si") {
        selectedGameInfo = _exultGameManagerInstance->get_si();
        if (!selectedGameInfo) selectedGameInfo = _exultGameManagerInstance->get_ss(); // Try SS if plain SI not found
    }
    // TODO: Handle Forge of Virtue (fov) and Silver Seed (ss) explicitly if they are distinct ScummVM gameId.
    // For now, they are fallbacks.

    if (!selectedGameInfo || !selectedGameInfo->is_there()) {
        error("ExultEngine: Game '%s' not found or configured correctly by Exult's GameManager.", gameId.c_str());
        return Common::kErrorSystem;
    }
    // TODO: Mod selection if ScummVM supports it via variants or extra data.
    // For now, using the base game selected.

    ExultCore::Game::create_game(selectedGameInfo); // Creates global ExultCore::game instance
    _exultGameInstance = ExultCore::g_manager->getGame(); // Get the created game instance
    if (!_exultGameInstance) {
        error("ExultEngine: Failed to create ExultCore::Game instance.");
        return Common::kErrorSystem;
    }

    // 7. Further initializations (from exult.cc after game selection)
    _exultAudio->Init_sfx(); // Audio singleton is ExultCore::Audio::get_ptr()
    ExultCore::Setup_text(_exultGameInstance->get_game_type(), _exultGameInstance->have_expansion(), _exultGameInstance->is_si_beta());

    // Palette setup for intro (Exult does this)
    _exultGameWindow->get_pal()->load(ExultCore::BUNDLE_CHECK(ExultCore::BUNDLE_EXULT_FLX, ExultCore::EXULT_FLX), ExultCore::EXULT_FLX_EXULT0_PAL);
    _exultGameWindow->get_pal()->apply();

    // TODO: Handle intro playback (_exultGameInstance->play_intro())
    // This might need to be managed by ScummVM's own intro player or skipped.
    // For now, skipping actual intro playback call.
    debug(1, "ExultEngine: Skipping Exult intro playback for now.");

    _exultGameWindow->init_files(); // Loads common game files (palettes.flx, fonts.vga)
    // _exultGameWindow->read_gwin(); // This loads savegame specific gwin state. Skip for new game.
    _exultGameWindow->setup_game(false); // false = not map editor mode. This is a major setup step.

    _initialized = true;
    debug(1, "ExultEngine: Full initialization sequence complete.");
    return Common::kNoError;
}

void ExultEngine::shutdown() {
    debug(1, "ExultEngine: shutdown() called.");
    if (!_initialized) {
        // If already shut down or never initialized, adapters might be null or already deleted.
        // The destructor will handle final cleanup if they are not null.
        // return;
    }

    // TODO: Properly shut down and clean up Exult_Engine_s core systems.
    // (e.g., delete _exultGameManager; _exultGameManager = nullptr;)

    // Shutdown Exult Core Systems first, in an order that avoids issues.
    // This order is approximated from exult.cc's shutdown sequence.

    // Game_window destructor handles deletion of many owned components like
    // Image_window (win), Usecode_machine (usecode), Gump_manager (gump_man), Palette (pal),
    // Mouse (Mouse::mouse static pointer is reset by Mouse destructor called from Game_window), etc.
    if (_exultGameWindow) {
        delete _exultGameWindow;
        _exultGameWindow = nullptr;
    }

    // Audio system
    ExultCore::Audio::Destroy(); // Static method to clean up Audio singleton and its resources

    // KeyBinder - Exult's global `keybinder` is deleted in its main Play() function's end.
    // If our initialize created it (which it ensures one exists), we should delete it.
    // Accessing it via get_instance() for deletion is okay if s_instance is nulled out afterwards.
    // However, KeyBinder's destructor doesn't nullify s_instance.
    // A dedicated shutdown/delete_instance for KeyBinder would be cleaner.
    // For now, if ExultEngine ensures its creation, it should ensure its deletion.
    // This assumes the KeyBinder instance we new'd in initialize is the one s_instance points to.
    if (ExultCore::KeyBinder::get_instance()) {
         // This is risky if s_instance points to a global not owned by this logic path.
         // But since we do `new KeyBinder()` if `get_instance()` is null, we are likely its owner.
         // Exult's original code: delete keybinder; (where keybinder is the global pointer)
         // We don't have the global pointer directly, but get_instance() should give it.
         // This requires KeyBinder to be allocated with new.
         // delete ExultCore::KeyBinder::get_instance(); // This might be problematic if s_instance isn't cleared.
         // Let's assume Exult's main way of cleaning up KeyBinder is just deleting the global pointer.
         // If KeyBinder::s_instance was the only reference, this would be fine.
         // For now, we won't delete it here to avoid double-deletion if Exult has other cleanup.
         // This needs to be robustly handled by Exult's own teardown if it's a true global singleton.
         // Given our `new KeyBinder()` in init, we probably should delete it.
         // Let's assume Exult's KeyBinder isn't a complex singleton that manages its own static memory internally beyond s_instance.
         delete ExultCore::KeyBinder::get_instance(); // This relies on s_instance being the one we new'd.
         // TODO: Add `KeyBinder::s_instance = nullptr;` in KeyBinder destructor or provide a static Destroy.
    }


    // Configuration
    if (_exultConfig) {
        delete _exultConfig;
        _exultConfig = nullptr;
    }

    // Game instance (BG_game, SI_game)
    if (_exultGameInstance) {
        // Exult's original code: delete game; (where game is the global pointer)
        // Game::destroy_game() might be safer if it exists, or if Game_Manager handles it.
        // Game_Manager doesn't seem to delete it.
        delete _exultGameInstance; // This should be ExultCore::g_manager->getGame()
        _exultGameInstance = nullptr;
        ExultCore::g_manager->setGame(nullptr); // Clear the global pointer in ExultCore
    }

    // GameManager instance (manages game list)
    if (_exultGameManagerInstance) {
        delete _exultGameManagerInstance;
        _exultGameManagerInstance = nullptr;
        ExultCore::g_manager = nullptr; // Clear Exult's global gamemanager pointer
    }

    // Text and Font manager cleanup
    ExultCore::Free_text();
    if (ExultCore::fontManager) { // fontManager is a global std::unique_ptr
        ExultCore::fontManager.reset();
    }


    // Shutdown adapters
    if (_audioAdapter) _audioAdapter->shutdown();
    if (_graphicsAdapter) { /* Add shutdown if it gets one */ }
    if (_inputAdapter) { /* Add shutdown if it gets one */ }
    if (_fileAdapter) { /* Add shutdown if it gets one */ }
    // Other adapters might have shutdown/cleanup methods too.

    // Deletion of adapters is now primarily handled in the destructor to ensure they are always cleaned up.
    // However, if they hold significant resources that can be released earlier, do it here.
    // For consistency, we can nullify them here after shutdown, destructor will skip if null.
    // delete _graphicsAdapter; _graphicsAdapter = nullptr; 
    // delete _inputAdapter;   _inputAdapter = nullptr;
    // delete _audioAdapter;   _audioAdapter = nullptr;
    // delete _fileAdapter;    _fileAdapter = nullptr;

    _initialized = false;
    debug(1, "ExultEngine: Placeholder shutdown complete.");
}

void ExultEngine::run() {
    debug(1, "ExultEngine: run() called.");
    if (!_initialized) {
        warning("ExultEngine::run() called before successful initialization.");
        return;
    }

    // TODO: Perform any one-time setup for Exult_Engine_s game loop if needed.
    // (e.g., _exultGameManager->prepareMainLoop();)

    // Exult's main loop is in exult.cc:Play() which calls Handle_events().
    // Handle_events() then has the SDL_PollEvent loop and calls game logic.
    // We replicate a simplified version here.
    // ScummVM's shouldQuit() is the primary loop condition.
    // Exult's `quitting_time` global will be checked to see if Exult internally wants to quit.

    uint32 last_exult_ticks = 0; // For Exult's timing logic if needed, though tqueue uses SDL_GetTicks()

    while (!shouldQuit()) {
        // 1. Process ScummVM events and translate them to Exult
        processInputEvents(); // This calls _inputAdapter->processScummVMEvents()
                              // which in turn calls Exult's Gump_Manager and KeyBinder.

        // 2. Update Exult's game time
        // Exult's original Handle_events() loop has a Delay() and then SDL_GetTicks().
        // Game::set_ticks() is called with current SDL_GetTicks().
        uint32 current_ticks = Common::Timer::getTicks();
        ExultCore::Game::set_ticks(current_ticks); // Update Exult's global game tick counter

        // 3. Activate Exult's time queue (animations, scheduled events, NPC AI ticks)
        // This is a major part of Exult's game logic update.
        if (_exultGameWindow && _exultGameWindow->get_tqueue() && _exultGameWindow->have_focus() /* && !dragging (Exult's global) */ ) {
            // TODO: Exult's `dragging` global state needs to be considered.
            // For now, assume not dragging or tqueue activation is safe.
            _exultGameWindow->get_tqueue()->activate(current_ticks);
        }

        // 4. Handle main actor movement and other continuous updates from original Handle_events()
        // This part is complex as Exult's Handle_events mixes event polling with game logic updates.
        // For example, right-mouse-button-down for movement, actor resting.
        // These might need to be refactored in Exult or carefully replicated.
        // For now, this is a simplified loop. Key/mouse actions from processInputEvents()
        // should trigger relevant actions in Exult (like starting actor movement).
        // The tqueue will then continue those actions.

		// 5. Update other game logic (placeholder for explicit calls if tqueue isn't enough)
        updateGameLogic(); // Currently empty, Exult's logic is largely event/tqueue driven.

        // 6. Render the frame
        renderFrame(); // Calls _graphicsAdapter->renderExultFrame()

        // 7. Check if Exult wants to quit
        // Exult's `quitting_time` is a global enum.
        if (ExultCore::quitting_time != ExultCore::QUIT_TIME_NO) {
            if (ExultCore::quitting_time == ExultCore::QUIT_TIME_RESTART) {
                // TODO: Handle Exult's restart request. This might involve re-initializing parts of Exult.
                // For now, just quit.
                debug(1, "ExultEngine: Exult requested restart, ScummVM will quit.");
                requestQuit();
            } else { // QUIT_TIME_YES or QUIT_TIME_MAINMENU
                debug(1, "ExultEngine: Exult requested quit/mainmenu, ScummVM will quit.");
                requestQuit();
            }
        }

        _system->yield(); // Yield to ScummVM's event loop / OS
    }

    // TODO: Perform any cleanup after Exult_Engine_s game loop finishes.
    // (e.g., _exultGameManager->cleanupMainLoop();)
    debug(1, "ExultEngine: run() loop finished.");
}

void ExultEngine::processInputEvents() {
    if (_inputAdapter) {
        _inputAdapter->processScummVMEvents();
    } else {
        // Fallback or warning if input adapter is missing
        Common::Event event;
        while (getEventManager()->pollEvent(event)) { /* Discard */ }
    }
}

void ExultEngine::updateGameLogic() {
    // TODO: Call Exult_Engine_s main game state update function (its "tick").
    // (e.g., if (_exultGameManager) _exultGameManager->tick();)
}

void ExultEngine::renderFrame() {
    if (_graphicsAdapter) {
        _graphicsAdapter->renderExultFrame();
    } else {
        // Fallback: clear screen or show error if graphics adapter is missing
        Graphics::Surface *screen = _system->lockScreenSurface();
        if (screen) {
            screen->clear(Color(128, 0, 0)); // Red to indicate error
            _system->unlockScreenSurface(true);
        }
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
    bool isBG = false;
    bool isSI = false;
    bool hasFoV = false;
    bool hasSS = false;

    if (gamedatDir.exists() && gamedatDir.isDirectory() && 
        staticDir.exists() && staticDir.isDirectory()) {
        
        // Check for Ultima VII: The Black Gate base files
        if (staticDir.getChild("U7.PAL").exists() && 
            gamedatDir.getChild("U7CHUNKS").exists() && 
            staticDir.getChild("SHAPES.VGA").exists()) {
            isBG = true;
            debug(1, "ExultMetaEngine: Detected Ultima VII: The Black Gate (base).");

            // Check for Forge of Virtue expansion
            if (staticDir.getChild("U7MOVE.FOV").exists() || gamedatDir.getChild("FURNFOVE.DAT").exists()) { // Common FoV files
                hasFoV = true;
                debug(1, "ExultMetaEngine: Detected Forge of Virtue expansion.");
            }
        }

        // Check for Ultima VII Part Two: Serpent Isle base files
        // These checks should be independent of BG checks in case of unusual directory merges.
        if (staticDir.getChild("SI.PAL").exists() && 
            gamedatDir.getChild("SICHUNKS").exists() &&
            staticDir.getChild("SISHAPES.VGA").exists()) {
            isSI = true;
            debug(1, "ExultMetaEngine: Detected Ultima VII Part Two: Serpent Isle (base).");

            // Check for The Silver Seed expansion
            if (staticDir.getChild("SILVER.DAT").exists() || gamedatDir.getChild("SILVER.DAT").exists()) { // Common SS files
                hasSS = true;
                debug(1, "ExultMetaEngine: Detected The Silver Seed expansion.");
            }
        }

        if (isBG || isSI) {
            // Log more specific detection if possible
            if (isBG && hasFoV) debug(1, "ExultMetaEngine: Final detection: BG with FoV.");
            else if (isBG) debug(1, "ExultMetaEngine: Final detection: BG only.");
            if (isSI && hasSS) debug(1, "ExultMetaEngine: Final detection: SI with SS.");
            else if (isSI) debug(1, "ExultMetaEngine: Final detection: SI only.");
            return true; // Return true if any valid Exult game combination is found
        }
    }

    debug(1, "ExultMetaEngine: No Exult game detected at path: %s", node.getPath().toString().c_str());
    return false;
}

Engine *ExultMetaEngine::createInstance(OSystem *syst, const Common::FSNode& gamePath, const Common::String& gameLanguage, const void *meDesc) {
    debug(1, "ExultMetaEngine: createInstance() called for gamePath: %s", gamePath.getPath().toString().c_str());
    return new ExultEngine(syst, gamePath, gameLanguage);
}

void ExultMetaEngine::getSupportedGames(Common::Array<PlainGameDescriptor> &games) const {
    debug(1, "ExultMetaEngine: getSupportedGames() called.");
    // TODO: Refine these descriptions, add GUIDs, features, etc. as per ScummVM standards.
    // GUIDs can be added later by ScummVM team based on TOSEC etc.
    // Path is usually empty for auto-detection. Platform is also often empty or generic.
    // Extra data can specify expansion files if needed for detection or setup.

    // Ultima VII: The Black Gate (base)
    games.append(PlainGameDescriptor("exult-bg", "Ultima VII: The Black Gate", "", "", ""));

    // Ultima VII: The Black Gate with Forge of Virtue
    // To differentiate, Exult's internal logic (or ScummVM detection if more sophisticated)
    // would check for FoV specific files. For now, we list it as a separate entry.
    // ScummVM's detection might need hints or specific checks for "U7MOVE.FOV" or similar.
    games.append(PlainGameDescriptor("exult-bg-fov", "Ultima VII: The Black Gate with Forge of Virtue", "", "", ""));

    // Ultima VII Part Two: Serpent Isle (base)
    games.append(PlainGameDescriptor("exult-si", "Ultima VII Part Two: Serpent Isle", "", "", ""));

    // Ultima VII Part Two: Serpent Isle with The Silver Seed
    // Similar to FoV, needs differentiation based on files like "SILVER.DAT".
    games.append(PlainGameDescriptor("exult-si-ss", "Ultima VII Part Two: Serpent Isle with The Silver Seed", "", "", ""));

    // Note: Exult also supports a combined "Serpent Isle Beta" (sib) which is less common.
    // games.append(PlainGameDescriptor("exult-sib", "Ultima VII Part Two: Serpent Isle (Beta)", "", "", ""));
}

void ExultMetaEngine::freeInstance(Engine *engine) {
    debug(1, "ExultMetaEngine: freeInstance() called.");
    delete static_cast<ExultEngine *>(engine);
}

// The ExultMetaEngine class, being a PluginObject derivative (via MetaEngine),
// should be automatically discovered by the ScummVM plugin system when compiled
// and linked as part of the Exult engine plugin. No explicit static registration
// instance or macro is typically needed in modern ScummVM for MetaEngine derivatives.
// The build system (module.mk) ensures this file is compiled and linked.

} // namespace Exult
} // namespace ScummVM

