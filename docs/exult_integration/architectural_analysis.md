# Architectural Analysis for Exult Integration into ScummVM

This document outlines the analysis of Exult and ScummVM architectures to identify key integration points, mismatches, and strategies for integrating Exult as a ScummVM engine. This analysis is based on the provided documentation.

## 1. Overview of Systems

### 1.1. Exult Engine Architecture

Exult is a C++ reimplementation of Ultima VII, featuring a monolithic architecture with several key subsystems:

*   **UI (Gumps & Gumps_manager):** Manages UI widgets (Gumps) deriving from `Gump` or `Modal_gump`. Handles drawing (`paint()`) and input (`mouse_down()`, `mouse_up()`). Managed by a central `Gumps_manager` which routes SDL mouse/keyboard events.
*   **Scripting (Usecode VM):** Executes Ultima VII's "USECODE" bytecode via a `Usecode_machine` object. Handles NPC AI, triggers, and dialogues. Integrated with the game's event queue and scheduler.
*   **Game Logic & Scheduling (gamemgr/, schedule/):** Manages the main simulation loop, global clock, and timed events ("futures"). Processes input, updates game logic, advances scheduled tasks, and triggers rendering.
*   **Rendering (imagewin/, desktop/):** The `imagewin` module (specifically `Game_window` class) handles rendering of the game view and UI gumps to SDL surfaces. Supports multiple view modes, custom tile/sprite rendering, texture scaling, and palette management via SDL2/SDL3.
*   **Audio (audio/):** Handles MIDI music, OPL/FM synthesis, and sound effects. Historically supported AdLib/Sound Blaster emulation. Modern builds often wrap SDL_audio or OpenAL. Includes an audio mixer filling an SDL callback buffer.
*   **AI & Movement (pathfinder/, actors/):** Implements A* pathfinding. Actors (characters, monsters) have goals, and the pathfinder computes walk paths. Combat AI is driven by usecode scripts combined with the pathfinder.
*   **Resource Loading (files/, shapes/, flic/):** `files/` reads Ultima VII's custom archive formats. `shapes/` parses `.SHA` tile files into textures/palettes. `flic/` decodes `.FLI` movies for cutscenes.

Exult's main loop drives all subsystems in lock-step, with communication often via global managers or singletons.

### 1.2. ScummVM Engine Plugin Architecture

ScummVM uses a plugin/engine model:

*   **Engine & MetaEngine:** Each game engine is a subclass of `Engine`, created by a corresponding `MetaEngine`. The `MetaEngine` (e.g., `ExultMetaEngine`) registers the engine and implements `createInstance()`.
*   **Core Interfaces:** The `Engine` subclass receives pointers to:
    *   `OSystem* _system`: Platform abstraction for windowing, filesystem, audio backend, timers, etc.
    *   `Audio::Mixer* _mixer`: For sound output.
*   **Engine Responsibilities:** Implement `run()` for the game loop, respond to `Engine::quitGame()`, etc.
*   **Common Managers:** Access to `getEventManager()`, `getTimerManager()`, `getSaveFileManager()` for unified input, timing, and save/load.
*   **OSystem:** Provides methods for graphics surfaces (`Graphics::Surface`), audio streams, virtual file access (`getFilesystemFactory()`, `createConfigReadStream()`).
*   **Graphics::Surface:** Wraps pixel buffers, offers blitting methods. Engines draw to these surfaces.
*   **EventManager:** Decouples engine from direct event polling (like SDL_PollEvent). Engines poll `Common::Event` objects.
*   **Audio::Mixer:** Mixes audio channels and sends to `OSystem`'s audio backend.
*   **SaveFileManager & TimerManager:** Standardized save/load and event scheduling.
*   **Build System:** Uses SCons. New engines are added to `engines/` and build files (e.g., `engines.mk`). Registration via macros like `REGISTER_ENGINE()`.

## 2. Architectural Mismatches

The integration requires addressing several key differences:

1.  **SDL3 (Exult) vs. SDL2/Native (ScummVM):**
    *   **Mismatch:** Exult targets SDL3, ScummVM's SDL backend is SDL2 (or native OSystem).
    *   **Impact:** Direct SDL calls in Exult (window creation, audio callbacks) will conflict with ScummVM's control.

2.  **Window Management & Main Loop Lifecycle:**
    *   **Mismatch:** Exult creates its own `Game_window` and runs its own main loop. ScummVM's `OSystem` creates the window and controls frame timing; the engine's `run()` is called within ScummVM's loop.
    *   **Impact:** Exult's main loop structure must be refactored to fit ScummVM's callback-driven model.

3.  **Surface and Memory Models:**
    *   **Mismatch:** Exult operates on raw SDL surfaces/pixel arrays. ScummVM uses `Graphics::Surface` objects with its own pixel format and potential hardware backing.
    *   **Impact:** Exult's rendering calls need wrappers to translate to `Graphics::Surface` operations.

4.  **Input Handling:**
    *   **Mismatch:** Exult uses direct SDL polling (`SDL_PollEvent`). ScummVM uses its `EventManager` queue.
    *   **Impact:** ScummVM `Common::Event` objects must be mapped to Exult's Gump calls or game inputs.

5.  **Graphics Pipeline (Palettes, Scaling):**
    *   **Mismatch:** Exult uses 256-color palettes and software blits. ScummVM engines often expect true-color or ScummVM's color handling.
    *   **Impact:** Palette operations and indexed blits need translation to RGBA copies on `Graphics::Surface`. Scaling differences also need adaptation.

6.  **Audio Subsystem:**
    *   **Mismatch:** Exult's mixer (OPL/MIDI, custom filling of SDL buffers) vs. ScummVM's `Audio::Mixer` interface.
    *   **Impact:** Exult's sound generation must feed into ScummVM's mixer. SDL3's audio callback timing changes might also need adjustments in Exult's mixer logic if that part of Exult is retained.

7.  **File Handling & Resource Loading:**
    *   **Mismatch:** Exult uses direct file paths (`fopen`) or its own archive readers. ScummVM uses a virtual filesystem (`FilesystemFactory`, `SeekableReadStream`).
    *   **Impact:** Exult's file I/O must be rerouted through ScummVM's file stream APIs.

8.  **Save/Load Mechanism:**
    *   **Mismatch:** Exult's custom save format and logic vs. ScummVM's `SaveFileManager` and slot system.
    *   **Impact:** Exult's save/load routines must be hooked into ScummVM's system, potentially with format conversion.

## 3. Migration Strategies & Integration Points

Based on the provided documentation, the following strategies will be employed for each subsystem:

### 3.1. Core Engine Wrapper (`ExultEngine : ScummVM::Engine`)
*   **Implementation:** Create `ExultEngine` subclassing `ScummVM::Engine` and `ExultMetaEngine` subclassing `ScummVM::MetaEngine`.
*   **Initialization (`ExultEngine::initialize()`):**
    *   Store `OSystem*` (`_system`) and `Audio::Mixer*` (`_mixer`) provided by ScummVM.
    *   Initialize Exult's core systems (Game_manager, Usecode_machine, resource loaders, etc.), passing ScummVM's `OSystem` for platform-dependent operations.
    *   Set up data paths using `gamePath` from ScummVM.
*   **Main Loop (`ExultEngine::run()`):**
    *   Implement a loop that continues until `shouldQuit()` is true.
    *   Inside the loop: process input, update game state, render frame, yield to ScummVM.
    *   `processInputEvents()`: Poll `getEventManager()` and translate `Common::Event` to Exult's input handlers.
    *   `exultGameManager.update()`: Call Exult's main game logic tick.
    *   `exultRenderer.drawFrame()`: Call Exult's rendering logic (which will use the graphics wrapper).
    *   `_system->yield()`: Yield control to ScummVM.
*   **Shutdown (`~ExultEngine()`):** Clean up Exult subsystems.
*   **Save/Load (`ExultEngine::save()`, `ExultEngine::load()`):** Override to use `getSaveFileManager()` and call Exult's save/load logic, adapting data to ScummVM's streams.

### 3.2. Graphics Adapter
*   **Target:** Replace Exult's `Game_window` direct SDL operations.
*   **Strategy:** Create a wrapper layer or modify Exult's rendering functions.
*   **Integration Points:** Exult's `paint()` methods in Gumps, and `Game_window`'s screen update logic.
*   **Implementation:**
    *   Obtain ScummVM's screen surface: `Graphics::Surface &screen = *_system->getScreenSurface();`
    *   Convert Exult's shape/tile data (e.g., from `Shape_set::loadShape()`) into temporary `Graphics::Surface` objects or directly blit pixel data.
    *   Replace SDL blits (e.g., `SDL_BlitSurface`) with `screen.copyRectToSurface(...)` or similar `Graphics::Surface` methods.
    *   Handle palette translation: Convert Exult's 8-bit palette-indexed pixels to the 32-bit (or other) format used by ScummVM's `Graphics::Surface` during blitting or surface creation.
    *   Adapt scaling logic if necessary.

### 3.3. Input Adapter
*   **Target:** Exult's `Gumps_manager` and other input-sensitive parts that directly use SDL events.
*   **Strategy:** In `ExultEngine::processInputEvents()` (called from `run()` loop):
*   **Implementation:**
    *   Poll `getEventManager()->pollEvent(ev)`.
    *   Translate `ev.type`, `ev.mouse`, `ev.key` etc. into parameters for Exult's input handlers (e.g., `gumps_manager->mouseDown(x, y)`, `gumps_manager->keyPress(scancode)`).
    *   Map ScummVM mouse coordinates to Exult's gump/game coordinates.

### 3.4. Audio Adapter
*   **Target:** Exult's audio subsystem and its SDL audio callback.
*   **Strategy:** Route Exult's audio sample generation to ScummVM's `Audio::Mixer`.
*   **Implementation Options:**
    1.  **Custom Audio Stream/Channel:** Create a class that implements a ScummVM audio stream interface (if available, or adapt existing channel mechanisms). Its `fill(buffer, length)` method would call Exult's internal audio mixing routines to provide sample data.
        `_mixer->addChannel(exultAudioChannel);` (conceptual)
    2.  **Callback Registration:** If `Audio::Mixer` supports a direct sample callback: `_mixer->setSamplesCallback(exultAudioCallback, userdata);` where `exultAudioCallback` wraps Exult's sample generation.
    3.  **Intermediate Buffer:** Exult's mixer writes to an intermediate buffer. ScummVM's audio system periodically reads from this buffer.
    *   Ensure Exult's music (MIDI/OPL) and sound effects are mixed and fed through this adapter.
    *   Address potential SDL3 audio callback timing differences if Exult's mixer is sensitive to this (e.g., fixed-size buffer requests).

### 3.5. File/Resource Loading Adapter
*   **Target:** Exult's `files/` module (specifically `U7file` and its subclasses like `Flat`, `Flex`, `IFF`, `Table`), `U7FileManager`, `Shape_set::loadShape()`, `Flic::play()`, and any direct file operations (`fopen`, `std::ifstream`, etc.). The old `IDataSource` hierarchy (e.g., `IExultDataSource`, `IFileDataSource`, `IBufferDataSource` from `databuf.h`) is also targeted for replacement or significant adaptation.
*   **Strategy:** Replace all direct file access and the old `IDataSource` system with ScummVM's virtual filesystem APIs, primarily using `ScummVM::Common::SeekableReadStream`. The `ExultFileAdapter` serves as a bridge, utilizing `OSystem::getFilesystemFactory()` to obtain streams.
*   **Implementation Details for `U7file` and subclasses:**
    *   The `ExultFileAdapter` is responsible for opening files via ScummVM's VFS and providing `ScummVM::Common::SeekableReadStream` instances.
    *   The base class `U7file` has been refactored to accept and manage a `std::unique_ptr<ScummVM::Common::SeekableReadStream>` in its constructor. Its `retrieve` method now uses this stream to read object data.
    *   Subclasses of `U7file` (`Flat`, `Flex`, `IFF`, `Table`) have been updated:
        *   Their constructors now accept a `std::unique_ptr<ScummVM::Common::SeekableReadStream>` which is passed to the `U7file` base class.
        *   Their internal `index_file()` methods (or equivalent logic for parsing file headers and object/chunk tables) have been refactored to use the provided ScummVM stream for all read operations (e.g., using `stream->read_le32()`, `stream->read()`).
        *   Dependencies on the old `IDataSource` system (e.g., `IFileDataSource`, `IExultDataSource` from `databuf.h`) and related helper templates like `U7DataFile` and `U7DataBuffer` have been removed from these classes.
    *   The `U7FileManager` will utilize `ExultFileAdapter` to get streams and then instantiate the appropriate `U7file` subclass (`Flat`, `Flex`, `IFF`, `Table`) by passing the stream to its constructor. Type detection logic (e.g., `is_flex`, `is_iff`) has been or will be adapted to operate on ScummVM streams.
*   **General Implementation:**
    *   Other parts of Exult that perform file I/O (e.g., `Shape_set::loadShape()`, `Flic::play()`) will also need to be modified to request streams from `ExultFileAdapter` or directly via `OSystem` if appropriate, and use ScummVM stream APIs for reading.
    *   For save games: Use `getSaveFileManager()->openFile(slot, Common::kFileModeWrite)` to get a `Common::WriteStream*` and `getSaveFileManager()->openFile(slot, Common::kFileModeRead)` for `Common::ReadStream*`. Exult's save/load logic will need to be adapted to use these streams.
    *   The `databuf.h` file, containing the old `IDataSource` hierarchy, will be progressively phased out or its components significantly refactored as direct stream usage becomes prevalent. Buffer-oriented classes like `IFFBuffer` or `U7DataBuffer` will need careful review to ensure they correctly integrate with the new stream-based approach, potentially by operating on data already read into memory via a ScummVM stream.

### 3.6. Scheduler/Tick Adapter
*   **Target:** Exult's `Game_manager::tick()` and its reliance on `SDL_Delay` or similar timing.
*   **Strategy:** Integrate Exult's game tick into ScummVM's main loop. Use `TimerManager` for timed events.
*   **Implementation:**
    *   Exult's main update logic (`Game_manager::tick()`, `Usecode_machine` updates, scheduler updates) will be called once per frame from `ExultEngine::run()`.
    *   Replace `SDL_Delay` calls with `_system->yield()` (for frame pacing) or use `getTimerManager()` for scheduling future Exult events if its internal scheduler needs to be mapped to ScummVM's timers.

### 3.7. Build System Integration
*   **Target:** ScummVM's SCons-based build system.
*   **Strategy:** Add Exult as a new engine.
*   **Implementation:**
    *   Create a new directory under `engines/` (e.g., `engines/exult/`).
    *   Add necessary SCons script files (`Engine.SCons` or similar) in this directory to compile Exult's source code (or the wrapper layer).
    *   Modify top-level SCons files (e.g., `engines.mk`, `engines/module.SCons`) to include the new Exult engine.
    *   Use `REGISTER_ENGINE(ExultMetaEngine, exult);` (or similar macro) in a C++ file to register the engine with ScummVM's MetaEngine factory.

## 4. Dual-Build Maintainability Strategy

To allow Exult to be built both standalone and as a ScummVM plugin:

*   **Abstraction Layers:** Define C++ interfaces (e.g., `IWindow`, `IScreenBuffer`, `IFile`, `IAudioStream`) for platform-dependent operations.
    *   Exult standalone build: Implements these interfaces using direct SDL calls.
    *   ScummVM plugin build: Implements these interfaces using ScummVM's `OSystem`, `Graphics::Surface`, `Audio::Mixer`, `SeekableReadStream`, etc.
*   **Conditional Compilation:** Use `#ifdef EXULT_SCUMMVM_PLUGIN` (or a similar preprocessor macro defined by the ScummVM build) to guard ScummVM-specific code or to select the correct implementation of the abstracted interfaces.
*   **Separate Wrapper Files:** Isolate ScummVM-specific integration code (e.g., `ExultEngine` implementation, `ExultMetaEngine`, adapter logic) into separate files (e.g., `exult_scummvm.cpp`, `scummvm_graphics_adapter.cpp`) that are only compiled as part of the ScummVM engine plugin build.
*   **Minimize Core Exult Changes:** Aim to keep the core Exult game logic (`gamemgr/`, `usecode/`, `actors/`, etc.) as unchanged as possible. Changes should ideally be confined to the abstraction layer calls or the new wrapper files.

## 5. Implementation Roadmap (Summary from User Input)

1.  **Plugin Skeleton:** Create `ExultEngine` and `ExultMetaEngine`, register in build system.
2.  **Initialization:** Implement `ExultEngine::initialize()` to set up Exult with `OSystem`.
3.  **Run Loop:** Implement `ExultEngine::run()` with input processing, game update, rendering, and yielding.
4.  **Audio Hookup:** Integrate Exult's audio with `_mixer`.
5.  **Saving/Loading:** Implement `save()`/`load()` using `SaveFileManager`.
6.  **Shutdown:** Implement cleanup in `~ExultEngine()`.

This analysis provides a structured approach to the integration task, highlighting the critical areas and the proposed solutions based on the provided documentation.

