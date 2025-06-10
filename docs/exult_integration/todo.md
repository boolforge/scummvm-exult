# Project Todo List - Phase 2: Deep Exult Integration

## Task: Integrate Exult as a Functional Scum- [X] **Step 1: Add Exult Source Tree to ScummVM Fork**
  - Created a dedicated directory for Exult_Engine_s core source code at `engines/exult/exult_core_src/`.
  - Copied Exult_Engine_s source code from the `exult_fork` repository into this directory.
  - The source tree is now in place for build system integrati- [X] **Step 2: Configure Build System to Compile Exult Core within ScummVM (Initial Analysis & Correction)**
  - Analyzed ScummVM engine build system (e.g., `queen/module.mk`).
  - Corrected Exult integration: removed incorrect `Engine.SCons`, created `engines/exult/module.mk` listing wrapper and core Exult object files, following ScummV- [X] **Step 4: Incrementally Replace Adapter Placeholders with Real Exult Calls (Initial Bridging)**
  - Systematically updated each adapter (`ExultFileAdapter`, `ExultGraphicsAdapter`, `ExultInputAdapter`, `ExultAudioAdapter`) header and implementation to include and reference corresponding Exult core classes (e.g., `U7FileManager`, `ImageWin`, `Gump_Manager`, `Audio`).
  - Added `TODO` comments in adapter implementations for deeper integration logic.
  - All changes committed atomically and pushed to the repository.
- [ ] **Step 4: Adapt Exult Resource and Filesystem Access to ScummVM VFS**
  - Focus on `ExultFileAdapter` and Exult's resource loading mechanisms (`files/`, `shapes/`, `flic/`).
  - Modify Exult's file I/O routines to use ScummVM's `SeekableReadStream`, `WriteStream`, and `FSNode` via the `ExultFileAdapter`.
  - Ensure Exult can correctly locate and load game data files (e.g., `U7CHUNKS`, `.SHA` files, `.FLI` movies) through ScummVM's virtual filesystem.
  - Adapt save/load mechanisms to use ScummVM's `SaveFileManager`.

- [ ] **Step 5: Implement Exult Mainloop Tick and Scheduler Integration**
  - Integrate Exult's main game loop logic (`Game_manager::tick()`, `Usecode_machine` updates, `Scheduler` events) into `ExultEngine::updateGameLogic()`.
  - Ensure Exult's timing and event scheduling mechanisms are driven by or compatible with ScummVM's `TimerManager` and main loop (`_system->yield()`).

- [ ] **Step 6: Adapt Exult Rendering Pipeline to ScummVM Graphics::Surface**
  - Focus on `ExultGraphicsAdapter` and Exult's rendering code (`imagewin/`, Gump painting).
  - Translate Exult's SDL-based rendering operations (blits, surface manipulation, palette handling) to draw onto ScummVM's `Graphics::Surface`.
  - Handle palette conversions (e.g., 8-bit indexed to 32-bit RGBA) and scaling as needed.

- [ ] **Step 7: Connect Exult Audio and Input to ScummVM Mixers and EventManager**
  - **Audio:** Focus on `ExultAudioAdapter`. Ensure Exult's audio generation (music, SFX) is correctly piped into ScummVM's `Audio::Mixer`.
  - **Input:** Focus on `ExultInputAdapter`. Ensure ScummVM `Common::Event` objects are accurately translated into Exult's Gump events or game commands.

- [ ] **Step 8: Perform Incremental Builds and Runtime Tests**
  - After integrating each significant piece of Exult's logic, attempt to compile the ScummVM engine.
  - Perform basic runtime tests to check for crashes, initialization errors, and basic functionality (e.g., can the engine load game data? Does it reach the main menu or game view?).
  - Debug issues as they arise. This will be an iterative process.

- [ ] **Step 9: Document All Changes and Push Frequently in English**
  - Maintain exhaustive documentation in code comments (American English).
  - Make frequent, small, logical commits with clear messages in American English.
  - Push changes to the `boolforge/scummvm-ack-experimental` repository regularly.

- [ ] **Step 10: Prepare and Send Interim Report to User**
  - After significant milestones or a set period, prepare an interim report detailing progress, challenges, and current status.
  - This report will be less comprehensive than the final one but will keep the user informed.

