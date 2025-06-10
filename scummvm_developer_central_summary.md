# ScummVM Developer Central Summary

This document summarizes key information from the ScummVM Developer Central wiki page, focusing on aspects relevant to engine development and documentation.

## Getting Started

To get started with ScummVM development, one should:

*   Get the ScummVM code from their Git repository.
*   Fork the ScummVM main Git repository.
*   Compile ScummVM.
*   Read the [Code Formatting Conventions](https://wiki.scummvm.org/index.php/Code_Formatting_Conventions) and [Coding Conventions](https://wiki.scummvm.org/index.php/Coding_Conventions).
*   Familiarize themselves with the [Commit Guidelines](https://wiki.scummvm.org/index.php/Commit_Guidelines) and [Git tips](https://wiki.scummvm.org/index.php/Git_tips).

## Code Base Structure

The ScummVM code base is structured into five main components:

*   **OSystem API**: Defines available features for games (e.g., drawing, input). Located in `common/system.h`.
*   **Backends**: Implement the OSystem API for various platforms. Located in `backends/`.
*   **Game Engines**: Implement the game logic for specific games. Located in `engines/`.
*   **Common Code**: Provides utility classes, audio, image, and video codecs. Located in `common/`, `audio/`, `image/`, `video/`.
*   **GUI Code**: Provides the graphical user interface for the game launcher and options dialog. Located in `gui/`.

Additional directories include:

*   `base/`: Entry point for the application, handles command-line arguments and main loop.
*   `test/`: Contains unit tests.
*   `po/`: Contains translations for the ScummVM GUI.

## For Engine Authors

Engine authors should focus on the OSystem API to understand what features are available. The `plumber` engine is recommended as a starting point for understanding engine structure due to its simplicity. Other engines like `queen` (for Flight of the Amazon Queen) can also be studied.

## Contribution Guide for Technical Writers

For large documentation changes, the process involves:

1.  Setting up the environment.
2.  Creating and editing documentation.
3.  Previewing the documentation.
4.  Committing the work.
5.  Creating a Pull Request.

## Relevant Links

*   [ScummVM Wiki](https://wiki.scummvm.org/)
*   [ScummVM Main Git Repository](https://github.com/scummvm/scummvm)
*   [Code Formatting Conventions](https://wiki.scummvm.org/index.php/Code_Formatting_Conventions)
*   [Coding Conventions](https://wiki.scummvm.org/index.php/Coding_Conventions)
*   [Commit Guidelines](https://wiki.scummvm.org/index.php/Commit_Guidelines)
*   [Git Tips](https://wiki.scummvm.org/index.php/Git_tips)
*   [Doxygen Source Code Documentation](https://docs.scummvm.org/doxygen/)
*   [Platform Limitations](https://wiki.scummvm.org/index.php/Platform_Limitations)
*   [Bug Tracker](https://bugs.scummvm.org/)
*   [Projects, plans, things to do](https://wiki.scummvm.org/index.php/Projects,_plans,_things_to_do)
*   [Contact Page (IRC/Discord)](https://www.scummvm.org/contact/)


