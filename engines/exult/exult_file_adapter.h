// engines/exult/exult_file_adapter.h

#ifndef EXULT_FILE_ADAPTER_H
#define EXULT_FILE_ADAPTER_H

#include "common/fs.h"
#include "common/stream.h"
#include "common/system.h"

// Forward declarations for ExultCore components (if needed later for direct integration)
namespace ExultCore {
    // class U7FileManager;
}

namespace ScummVM {

// Forward declaration of ExultEngine to avoid circular dependencies if needed
namespace Exult { class ExultEngine; }

namespace Exult {

class ExultFileAdapter {
public:
    // Modified constructor to only take OSystem* for now
    ExultFileAdapter(OSystem* system);
    ~ExultFileAdapter();

    bool init(const Common::FSNode& gamePath);

    // These methods will need to be implemented to bridge ScummVM VFS with Exult file operations
    Common::SeekableReadStream* openFileRead(const char* filename);
    Common::WriteStream* openFileWrite(const char* filename);
    bool fileExists(const char* filename);

    Common::FSNode getGamePath() const;

private:
    OSystem* _osystem;
    Common::FSNode _gamePathNode;
    // ExultCore::U7FileManager* _exultFileManager; // Removed for now
};

} // namespace Exult
} // namespace ScummVM

#endif // EXULT_FILE_ADAPTER_H


