# Project Bootstrap: Exult Engine Integration into ScummVM

## 1. Project Goal

The primary goal of this project is to integrate the Exult engine (a C++ reimplementation of Ultima VII) as a new, fully functional engine within a forked version of ScummVM. This involves adapting Exult_Engine_s core systems (graphics, audio, input, file I/O, game logic) to ScummVM_Engine_s architecture and APIs, ensuring compliance with ScummVM_Engine_s coding standards and build system.

## 2. Current Context & Repositories

*   **ScummVM Fork (with Exult integration work):** [https://github.com/boolforge/scummvm-ack-experimental](https://github.com/boolforge/scummvm-ack-experimental)
*   **Exult Fork (original Exult source for reference):** [https://github.com/boolforge/exult](https://github.com/boolforge/exult)
*   **GitHub Token User (for commits):** `boolforge` (Original request was for `binaryflow`, but token resolved to `boolforge`)

Initial setup involved forking both repositories. Exult_Engine_s source code has been copied into the ScummVM fork at `engines/exult/exult_core_src/`. A build system integration using `module.mk` (conforming to ScummVM standards) has been established in `engines/exult/module.mk`, and the ScummVM configure script runs successfully, detecting necessary dependencies like SDL2.

Adapter classes (`ExultFileAdapter`, `ExultGraphicsAdapter`, `ExultInputAdapter`, `ExultAudioAdapter`) have been created as a bridge between ScummVM and Exult_Engine_s core components. Initial bridging has been done to link these adapters with their respective Exult core classes (e.g., `U7FileManager`, `ImageWin`, `Gump_Manager`, `Audio`).

## 3. Core Operating Principles & Internal Knowledge (Manus AI)

This section outlines the core operating principles and internal knowledge base guiding the AI_Engine_s work on this project. Adherence to these principles is critical for consistency, quality, and effective collaboration.

### 3.1. Language and Communication
*   **Working Language:** All code, comments, commit messages, documentation (including this document, `todo.md`, `architectural_analysis.md`), and any other technical artifacts generated **must** be in **American English**. This is a strict requirement, regardless of the conversational language with the user.
*   **User Communication:** Maintain clear and timely communication with the user. Notify about significant progress, changes in strategy, potential issues, and estimated durations for complex tasks. Use `message_notify_user` for updates and `message_ask_user` sparingly for essential clarifications.

### 3.2. Version Control & Commits
*   **Atomic Commits:** All changes (code, documentation, build scripts, configuration) **must** be committed atomically. Each commit should represent a single, logical, self-contained unit of work.
*   **Commit Early, Commit Often, Push Often:** This principle **must** be strictly followed. Make small, frequent commits. Push these commits to the remote repository (`boolforge/scummvm-ack-experimental`) very frequently to ensure the remote is always up-to-date and progress is continuously tracked.
*   **Commit Messages:** Commit messages **must** be in American English, clear, concise, and descriptive. They should follow conventional formats (e.g., imperative mood, summary line, optional detailed body). Example: `feat: Integrate Exult ImageWin into ExultGraphicsAdapter` or `fix: Correct path resolution in U7FileManager`.

### 3.3. Coding Standards & Quality
*   **Project-Specific Guidelines:** Strict adherence to the target project_Engine_s coding guidelines, style, and architectural patterns is paramount. For this project, this means meticulously following **ScummVM_Engine_s Development Central, Coding Conventions, and Code Formatting rules** (links provided below).
*   **Code Documentation:** All code **must** be exhaustively documented. This includes explaining the purpose of every function, class, method, and significant variable. Algorithms, complex logic, and any special considerations must be clearly detailed in comments (American English).
*   **Modularity and Reusability:** Design code with modularity and reusability in mind, where appropriate within the constraints of the existing Exult and ScummVM architectures.
*   **Error Handling:** Implement robust error handling. Identify potential failure points, log errors clearly, and handle them gracefully.
*   **No Magic Numbers/Strings:** Avoid hardcoding unnamed constants or strings; use named constants or configuration values where appropriate.

### 3.4. Build System & Dependencies
*   **Conformity:** Understand and strictly conform to the target project_Engine_s build system (ScummVM_Engine_s `module.mk` based system). Avoid generic build solutions if they conflict with established project patterns.
*   **Dependency Management:** Identify and manage dependencies correctly. Ensure necessary tools and libraries (e.g., C++ compiler, SDL2) are installed and configured as required by the project.

### 3.5. Task Management & Planning
*   **Planner Adherence:** Follow the steps outlined by the Planner module. Each step in the plan represents a phase of work.
*   **`todo.md`:** Utilize `todo.md` for detailed task breakdown and progress tracking within each planned step. Update `todo.md` markers immediately upon completion of sub-tasks.
*   **Incremental Development:** Approach complex integrations incrementally. Break down large tasks into smaller, manageable pieces. Integrate and test one subsystem or feature at a time.

### 3.6. Information Gathering & Analysis
*   **Source Priority:** Prioritize information from authoritative sources: 1) Provided Data APIs (if any), 2) Official project documentation (e.g., ScummVM Wiki, Exult source), 3) Web searches for specific technical solutions, 4) Internal knowledge as a last resort for general principles.
*   **Thorough Analysis:** Before making changes to existing complex codebases like Exult or ScummVM, conduct a thorough analysis of the relevant modules, their interactions, and the intended design.

### 3.7. File Management & Tools
*   **File I/O:** Use provided file tools (`file_write`, `file_read`, `file_str_replace`) for manipulating files in the sandbox, rather than complex shell commands for text manipulation.
*   **Intermediate Results:** Actively save intermediate results, drafts, and important pieces of information to files.
*   **Tool Proficiency:** Utilize all available tools (shell, browser, code execution, image generation, etc.) effectively and appropriately for the task at hand.

### 3.8. General Best Practices
*   **Preservation of Functionality:** When refactoring or integrating existing code (like Exult_Engine_s), the goal is to adapt and extend its functionality to the new environment (ScummVM), not to remove or arbitrarily change core features unless explicitly part of the integration requirements (e.g., replacing SDL calls with ScummVM API calls).
*   **Security with Credentials:** Treat any provided credentials (like GitHub tokens) as highly sensitive. Do not expose them in logs, messages, or public code. (The provided token has been used as directed for repository pushes).
*   **Problem Solving:** When encountering errors or unfamiliar problems, first verify tool usage and arguments. Consult error messages. If necessary, search for solutions or try alternative approaches. Report persistent issues to the user with clear explanations.

## 4. ScummVM Specific Guidelines (Reiteration)
*   **Developer Central:** [https://wiki.scummvm.org/index.php/Developer_Central](https://wiki.scummvm.org/index.php/Developer_Central)
*   **Coding Conventions:** [https://wiki.scummvm.org/index.php/Coding_Conventions](https://wiki.scummvm.org/index.php/Coding_Conventions) (Covers C++11 subset, no exceptions, portability, endianness, struct packing, file access via ScummVM APIs, no global/static variables where avoidable, etc.)
*   **Code Formatting:** [https://wiki.scummvm.org/index.php?title=Code_Formatting_Conventions](https://wiki.scummvm.org/index.php?title=Code_Formatting_Conventions) (Covers brace style, indentation, whitespace, naming, comments, etc.)

## 5. Exult Engine Architecture (Summary)
Exult is a C++ reimplementation of Ultima VII, organized into distinct modules for UI (Gumps), scripting (Usecode VM), game logic, rendering (ImageWin, SDL-based), audio, AI, and resource management (U7FileManager, various file type handlers). The integration aims to map these systems to ScummVM_Engine_s equivalents. Refer to `architectural_analysis.md` for more details.

## 6. Current Status (as of 2025-05-14 ~14:20 UTC)

We are currently executing **Step 5: Adapt Exult resource and filesystem access to ScummVM VFS** of the refined project plan.

*   **Completed up to this point:**
    *   Forking repositories.
    *   Copying Exult source into ScummVM fork (`engines/exult/exult_core_src/`).
    *   Correcting build system integration to use `module.mk` for Exult engine.
    *   Successfully configuring ScummVM build environment with Exult sources included.
    *   Initial bridging of adapter classes (`ExultFileAdapter`, `ExultGraphicsAdapter`, `ExultInputAdapter`, `ExultAudioAdapter`) to reference corresponding Exult core classes.
    *   Began VFS integration: `U7FileManager.h` and `U7FileManager.cc` have been modified to include a pointer to `ScummVM::Exult::ExultFileAdapter` and to attempt file opening via this adapter. Debug/logging calls have been added.
    *   All documentation (`todo.md`, `architectural_analysis.md`, this bootstrap document) has been committed to `docs/exult_integration/`.

*   **Immediate Next Task within Step 5:**
    *   The critical next part of VFS integration is to refactor `U7file.h` and its subclasses (e.g., `IFFFile`, `FlexFile`, `TableFile`, `FlatFile`, and buffer-based variants like `IFFBuffer`) and the `IExultDataSource` hierarchy. These classes currently use direct file operations (like `std::ifstream` or platform-specific file access).
    *   They need to be modified to accept and use `ScummVM::Common::SeekableReadStream` (obtained via `ExultFileAdapter`) as their data source, instead of opening files directly.
    *   This involves changing their constructors and internal data reading logic to work with ScummVM_Engine_s stream API.

## 7. Overall Next Steps (Post Current VFS Integration Focus)

1.  **Complete VFS Integration (Step 5):** Fully refactor Exult_Engine_s file I/O to use ScummVM_Engine_s VFS via `ExultFileAdapter` and ScummVM streams. This includes handling game data files, save/load mechanisms (integrating with ScummVM_Engine_s `SaveFileManager`).
2.  **Implement Mainloop Tick and Scheduler Integration (Step 6):** Integrate Exult_Engine_s main game loop (`Game_manager::tick()`, `Usecode_machine` updates, `Scheduler` events) into `ExultEngine::updateGameLogic()`, ensuring compatibility with ScummVM_Engine_s timing and event systems.
3.  **Adapt Rendering Pipeline (Step 7):** Fully bridge Exult_Engine_s rendering (via `ExultGraphicsAdapter` and `ImageWin`) to ScummVM_Engine_s `Graphics::Surface`, including palette handling, blitting, and updates.
4.  **Connect Audio and Input (Step 8):** Complete the integration of Exult_Engine_s audio with ScummVM_Engine_s `Audio::Mixer` (via `ExultAudioAdapter`) and ScummVM_Engine_s `Common::Event` translation to Exult input actions (via `ExultInputAdapter`).
5.  **Perform Incremental Builds and Runtime Tests (Step 9):** Continuously build and test the engine at each stage of integration. Debug issues related to compilation, linking, and runtime behavior (crashes, incorrect rendering, missing audio/input, logic errors).
6.  **Document and Push (Step 10):** Maintain exhaustive documentation in American English (code comments, commit messages, `todo.md`, `architectural_analysis.md`, this bootstrap document). Commit and push all changes atomically and frequently.
7.  **Prepare Interim/Final Reports (Step 11):** Provide reports on progress, challenges, and outcomes.

## 8. Key Project Files in Repository

*   **Exult Engine Code:** `scummvm-ack-experimental/engines/exult/`
*   **Exult Core Source (copied):** `scummvm-ack-experimental/engines/exult/exult_core_src/`
*   **Build Script for Exult Engine:** `scummvm-ack-experimental/engines/exult/module.mk`
*   **Integration Documents:** `scummvm-ack-experimental/docs/exult_integration/`
    *   `todo.md`
    *   `architectural_analysis.md`
    *   `project_bootstrap.md` (this file)

## 9. Prompt for New AI Instance

"You are Manus, an AI agent. Please continue the task of integrating the Exult engine into ScummVM. The project context, current status, core operating principles, and detailed next steps are provided in the attached `project_bootstrap.md` document. Familiarize yourself with this document, the linked ScummVM coding guidelines, and the contents of the ScummVM fork at [https://github.com/boolforge/scummvm-ack-experimental](https://github.com/boolforge/scummvm-ack-experimental), particularly the `engines/exult/` and `docs/exult_integration/` directories.

Your immediate task is to continue **Step 5: Adapt Exult resource and filesystem access to ScummVM VFS**. Specifically, you need to refactor `U7file.h` and its subclasses (e.g., `IFFFile`, `FlexFile`, `TableFile`, `FlatFile`, `IFFBuffer`, etc.) and the `IExultDataSource` hierarchy. These classes currently use direct file operations. Modify them to accept and use `ScummVM::Common::SeekableReadStream` (obtained via `ExultFileAdapter`) as their data source. This involves changing their constructors and internal data reading logic to work with ScummVM_Engine_s stream API. Ensure all changes are committed atomically to the repository, with clear messages in American English, and that all documentation is kept up-to-date. Adhere strictly to ScummVM_Engine_s coding guidelines and the project_Engine_s established workflow (as detailed in the Core Operating Principles section of the bootstrap document)."
