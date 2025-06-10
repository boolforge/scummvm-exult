# Exult Engine Integration into ScummVM: Detailed Task List

This document tracks the tasks for integrating the Exult engine into ScummVM, following an optimized hybrid approach that combines the current VFS adaptation with incremental architectural improvements.

## I. Audit and Preparation (Completed)

- [x] Review `project_bootstrap.md`, ScummVM coding guidelines, and other relevant documentation.
- [x] Audit existing codebase in `scummvm-ack-experimental` to understand current progress.
- [x] Clarify scope and expectations for Exult integration based on documentation.
- [x] Evaluate alternative approaches and confirm optimized hybrid strategy.

## II. Step 5: Adapt Exult Resource and Filesystem Access to ScummVM VFS

The goal is to refactor Exult's file I/O to use ScummVM's VFS via `ExultFileAdapter` and `ScummVM::Common::SeekableReadStream`.

### II.A. Refactoring IFFBuffer and Buffer Variants

- [ ] Locate all files related to `IFFBuffer` and similar buffer-based classes.
- [ ] Analyze their current functionality and dependencies.
- [ ] Modify constructors to accept `SeekableReadStream`.
- [ ] Adapt internal methods to use `_scummvmStream`.
- [ ] Update class and method documentation.
- [ ] Verify compilation.
- [ ] Commit and push with descriptive message.

### II.B. Refactoring the IExultDataSource Hierarchy

- [ ] Analyze all uses of `IExultDataSource` in the codebase.
- [ ] Identify classes that still depend on this hierarchy.
- [ ] For each use, replace with `SeekableReadStream`.
- [ ] Update `ExultFileAdapter` if necessary.
- [ ] Gradually remove obsolete code.
- [ ] Verify compilation after each change.
- [ ] Commit and push after each modified class.

### II.C. Fixing Compilation Issues

- [ ] Resolve header inclusion problems.
- [ ] Fix ScummVM API incompatibilities.
- [ ] Update references to `Path::getPath().c_str()`.
- [ ] Fix `MetaEngine` registration.
- [ ] Verify compilation after each fix.
- [ ] Commit and push after each resolved issue.

### II.D. Updating Documentation for VFS Changes

- [ ] Update `architectural_analysis.md` with VFS integration details.
- [ ] Create `vfs_integration_notes.md` with detailed explanation if needed.
- [ ] Ensure all code comments are exhaustive.
- [ ] Commit and push updated documentation.

## III. Incremental Architectural Improvement

### III.A. Introduction of Adapter Interfaces

- [ ] Create `engines/exult/include/` directory if it doesn't exist.
- [ ] Define `IFileSource`, `IWindow`, `IAudioStream`, `ITimer` interfaces.
- [ ] Document each interface exhaustively.
- [ ] Commit and push the new interfaces.

### III.B. Implementation of ScummVM Adapters

- [ ] Create `engines/exult/adapter/` directory if it doesn't exist.
- [ ] Implement adapters for each interface.
- [ ] Ensure compatibility with existing code.
- [ ] Verify compilation after each adapter.
- [ ] Commit and push each adapter individually.

### III.C. Gradual Refactoring Toward Facades

- [ ] Identify components suitable for facades.
- [ ] Create `engines/exult/facade/` directory if it doesn't exist.
- [ ] Implement facades gradually, one by one.
- [ ] Adapt existing code to use facades.
- [ ] Verify compilation after each facade.
- [ ] Commit and push each facade individually.

## IV. Validation and Testing

- [ ] Run clean build of the entire project.
- [ ] Document and resolve any errors or warnings.
- [ ] Verify that all components compile correctly.
- [ ] Commit and push final fixes.
- [ ] Verify that the Exult engine registers correctly.
- [ ] Test file loading through the VFS.
- [ ] Document any issues found.
- [ ] Implement fixes for identified issues.
- [ ] Commit and push issue fixes.

## V. Final Documentation

- [ ] Update all documentation to reflect the final state.
- [ ] Create detailed report of changes made.
- [ ] Update todo.md to mark completed tasks.
- [ ] Commit and push final documentation.

## Important Notes

- **Frequent Commits**: Make atomic commits and push after EACH significant change.
- **Compilation Verification**: Verify compilation after EACH code modification.
- **Exhaustive Documentation**: Keep code comments and documentation updated at all times.
- **Backups**: Create backups of critical files before modifying them.
