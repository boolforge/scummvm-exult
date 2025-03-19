# ACK Engine for ScummVM

This directory contains the implementation of the Adventure Construction Kit (ACK) engine for ScummVM. The engine has been converted from the original TurboPascal code to C++ and integrated with ScummVM's architecture.

## Directory Structure

- `ack.h/cpp` - Main engine class and implementation
- `console.h/cpp` - Debug console implementation
- `detection.h/cpp` - Game detection code
- `graphics.h/cpp` - Graphics system implementation
- `graphics_renderer.h/cpp` - ACK-specific graphics rendering
- `input.h/cpp` - Input handling (keyboard and mouse)
- `metaengine.cpp` - ScummVM plugin interface
- `module.mk` - Build configuration
- `picture.h/cpp` - Background picture handling
- `resource.h/cpp` - Resource management
- `script.h/cpp` - Script interpreter
- `sound.h/cpp` - Sound system implementation
- `sound_decoder.h/cpp` - Sound format decoder
- `sprite.h/cpp` - Sprite management
- `structs.h` - Data structures

## Integration with ScummVM

The engine follows ScummVM's standard engine structure and integrates with the following ScummVM systems:

1. **Graphics System**: Uses ScummVM's surface management and palette handling
2. **Input System**: Integrates with ScummVM's event system
3. **Sound System**: Uses ScummVM's mixer for sound and music playback
4. **Save/Load System**: Implements ScummVM's save/load interface
5. **Resource Management**: Custom implementation for ACK resources
6. **Game Detection**: Implements ScummVM's game detection interface

## Installation

To install this engine in ScummVM:

1. Copy the entire `ack` directory to `engines/` in your ScummVM source tree
2. Add the engine to the engines list in `engines/engines.mk`
3. Rebuild ScummVM

## Testing

The `test` directory contains unit tests for all major components of the engine. These tests can be run to verify the engine's functionality.

## Original Source

This engine is based on the Adventure Construction Kit source code available at:
https://github.com/boolforge/acksrc

## License

This code is licensed under the GPL v3, the same license as ScummVM.
