// engines/exult/exult_file_adapter.cpp

#include "exult_file_adapter.h"
#include "common/system.h"
#include "common/debug.h"
#include "common/fsnode_file.h"
#include "common/path.h"

// Removed ExultCore include as it's no longer directly used in the constructor
// #include "exult_core_src/files/U7fileman.h" 

namespace ScummVM {
namespace Exult {

// Modified constructor to match the updated header
ExultFileAdapter::ExultFileAdapter(OSystem* system)
    : _osystem(system)
    // Removed initialization of _exultFileManager
    {
    debug(1, "ExultFileAdapter: Constructor called.");
    if (!_osystem) {
        error("ExultFileAdapter: OSystem pointer is null!");
    }
    // Removed warning about _exultFileManager being null
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

    // Removed _exultFileManager initialization and checks

    debug(1, "ExultFileAdapter: Placeholder initialization for U7FileManager interaction.");
    return true;
}

Common::SeekableReadStream* ExultFileAdapter::openFileRead(const char* filename) {
    debug(2, "ExultFileAdapter: openFileRead(\"%s\") called.", filename);
    if (!_osystem || !_gamePathNode.exists()) {
        warning("ExultFileAdapter: OSystem or gamePathNode not available to open file: %s", filename);
        return nullptr;
    }

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

    warning("ExultFileAdapter: openFileWrite is a placeholder and needs proper integration with Exult_Engine_s file writing and ScummVM_Engine_s save mechanisms.");
    return nullptr; // Placeholder, needs proper implementation
}


bool ExultFileAdapter::fileExists(const char* filename) {
    debug(2, "ExultFileAdapter: fileExists(\"%s\") called.", filename);
    if (!_gamePathNode.exists()) {
        return false;
    }
    return _gamePathNode.getChild(filename).exists();
}

Common::FSNode ExultFileAdapter::getGamePath() const {
    return _gamePathNode;
}

} // namespace Exult
} // namespace ScummVM


