// engines/exult/exult_file_adapter.cpp

#include "exult_file_adapter.h"
#include "exult_engine.h" // May need access to engine state or Exult components via engine
#include "common/system.h"
#include "common/debug.h"
#include "common/fsnode_file.h"
#include "common/path.h"

// Actual Exult include for its U7FileManager
// The path is relative to the engines/exult/ directory where this .cpp file resides.
#include "exult_core_src/files/U7fileman.h" 

namespace ScummVM {
namespace Exult {

ExultFileAdapter::ExultFileAdapter(OSystem* system, ExultCore::U7FileManager* exultFileManager)
    : _osystem(system),
      _exultFileManager(exultFileManager) {
    debug(1, "ExultFileAdapter: Constructor called.");
    if (!_osystem) {
        error("ExultFileAdapter: OSystem pointer is null!");
    }
    if (!_exultFileManager) {
        // This might be acceptable if Exult_Engine_s U7FileManager is created later by ExultEngine
        // and then set, but for now, we expect it at construction or early init.
        warning("ExultFileAdapter: Exult U7FileManager pointer is null at construction!");
    }
}

ExultFileAdapter::~ExultFileAdapter() {
    debug(1, "ExultFileAdapter: Destructor called.");
}

bool ExultFileAdapter::init(const Common::FSNode& gamePath) {
    debug(1, "ExultFileAdapter: init() called with gamePath: %s", gamePath.getPath().c_str());
    if (!_osystem) {
        error("ExultFileAdapter: Cannot init, OSystem is not available.");
        return false;
    }
    _gamePathNode = gamePath;
    if (!_gamePathNode.exists() || !_gamePathNode.isDirectory()) {
        warning("ExultFileAdapter: Provided gamePath does not exist or is not a directory: %s", gamePath.getPath().c_str());
    }

    if (!_exultFileManager) {
        error("ExultFileAdapter: Exult U7FileManager is null, cannot proceed with its initialization.");
        return false;
    }

    // TODO: Initialize Exult_Engine_s U7FileManager here.
    // This is a critical step. U7FileManager needs to be configured to:
    // 1. Know the base game path (from _gamePathNode.getPath().c_str()).
    // 2. Potentially use ScummVM_Engine_s stream objects for its file operations instead of direct stdio or SDL_RWops.
    //    This might involve modifying U7FileManager itself or providing it with factory methods/callbacks
    //    that use this ExultFileAdapter to open ScummVM streams.
    // Example (conceptual, actual U7FileManager API will differ):
    // _exultFileManager->setBasePath(_gamePathNode.getPath().c_str());
    // _exultFileManager->setStreamOpener(this); // If U7FM can take a helper to open streams

    debug(1, "ExultFileAdapter: Placeholder initialization for U7FileManager interaction.");
    return true;
}

Common::SeekableReadStream* ExultFileAdapter::openFileRead(const char* filename) {
    debug(2, "ExultFileAdapter: openFileRead(\"%s\") called.", filename);
    if (!_osystem || !_gamePathNode.exists()) {
        warning("ExultFileAdapter: OSystem or gamePathNode not available to open file: %s", filename);
        return nullptr;
    }

    // TODO: This method should ideally be used by a modified U7FileManager.
    // If Exult_Engine_s code directly calls this adapter (less ideal), then it proceeds as before.
    // If U7FileManager is adapted to use ScummVM streams, then U7FileManager itself
    // would call something like this to get a ScummVM stream for a given path.

    Common::FSNode fileNode = _gamePathNode.getChild(filename);

    if (!fileNode.exists() || fileNode.isDirectory()) {
        debug(1, "ExultFileAdapter: File not found or is a directory via ScummVM VFS: %s", fileNode.getPath().c_str());
        return nullptr;
    }

    Common::SeekableReadStream* stream = new Common::FSNodeReadStream(fileNode);
    if (!stream || !stream->isOpen()) {
        warning("ExultFileAdapter: Failed to open ScummVM stream for file: %s", fileNode.getPath().c_str());
        delete stream;
        return nullptr;
    }

    debug(2, "ExultFileAdapter: Successfully opened %s for reading via ScummVM VFS.", fileNode.getPath().c_str());
    return stream;
}

Common::WriteStream* ExultFileAdapter::openFileWrite(const char* filename) {
    debug(2, "ExultFileAdapter: openFileWrite(\"%s\") called.", filename);
    if (!_osystem) {
        warning("ExultFileAdapter: OSystem not available to open file for writing: %s", filename);
        return nullptr;
    }

    // TODO: Similar to openFileRead, this needs to be integrated with U7FileManager or
    // Exult_Engine_s save/write mechanisms. ScummVM_Engine_s SaveFileManager is the standard for savegames.
    // For other writable files, the approach depends on Exult_Engine_s needs and ScummVM_Engine_s capabilities.

    warning("ExultFileAdapter: openFileWrite is a placeholder and needs proper integration with Exult_Engine_s file writing and ScummVM_Engine_s save mechanisms.");
    return nullptr; // Placeholder, needs proper implementation
}


bool ExultFileAdapter::fileExists(const char* filename) {
    debug(2, "ExultFileAdapter: fileExists(\"%s\") called.", filename);
    if (!_gamePathNode.exists()) {
        return false;
    }
    // TODO: If _exultFileManager is fully integrated and uses ScummVM VFS,
    // this call might go through it. Otherwise, direct VFS check is fine.
    return _gamePathNode.getChild(filename).exists();
}

Common::FSNode ExultFileAdapter::getGamePath() const {
    return _gamePathNode;
}

} // namespace Exult
} // namespace ScummVM

