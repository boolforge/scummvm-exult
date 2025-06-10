# ScummVM Coding Conventions Summary

This document summarizes the key coding conventions for the ScummVM project, based on their wiki page. The primary goal of these conventions is to ensure code portability across various systems.

## Language Features

ScummVM is written in a subset of C++11. The following features are currently *not* used due to portability limitations and overhead:

*   **C++ exceptions (throw/catch)**: Not supported by all compilers, and incur binary size overhead.
*   **Global C++ objects**: Constructors/destructors may not be called on certain targets, leading to issues. Use of objects requiring global constructors is forbidden.
*   **Standard Template Library (STL)**: Wrappers for a subset of STL are provided in `common/std/` for porting existing codebases. For new engines, classes in `/common/` should be used directly.

## Endianness Issues

ScummVM runs on both little-endian and big-endian hosts. Engine authors must account for endianness when reading/writing data (e.g., savegames) to files. This can be handled using `READ_` and `WRITE_` macros from `common/endian.h` (e.g., `READ_LE_UINT32`, `WRITE_BE_UINT16`) or corresponding `Stream` class methods.

Some games may have different versions for different endian systems, requiring specific reading calls.

## Struct Packing

Do *not* assume a specific memory layout for structs. If packed structs are absolutely necessary, use the `common/pack-start.h`, `common/pack-end.h` includes, and the `PACKED_STRUCT` macro within the struct definition.

## File Access

Direct use of C standard library file I/O functions (e.g., `fopen()`, `fread()`, `fwrite()`) or POSIX file I/O functions (e.g., `open()`, `close()`) is generally forbidden, unless developing a backend. Instead, use the file access APIs provided by ScummVM. Refer to the [HOWTO Open Files](https://wiki.scummvm.org/index.php/HOWTO_Open_Files) page for details.

## Use of Global / Static Variables

Global variables and static variables within function bodies should be avoided due to two major issues:

1.  **Global objects with constructors**: The order of global constructor invocation is not well-defined, and they may not be called on all targets or when dynamically loading plugins. This is strictly forbidden.
2.  **Non-reentrant behavior**: Static variables can retain state across game restarts (e.g., using the 

