// engines/exult/exult_file_adapter.h

#ifndef EXULT_FILE_ADAPTER_H
#define EXULT_FILE_ADAPTER_H

#include "common/fsnode.h"
#include "common/stream.h"
#include "common/system.h"

// Forward declarations for Exult classes if needed
// namespace Exult {
//     class File_Manager; // Or Exult_Engine_s equivalent for resource access
// }

namespace ScummVM {

// Forward declaration of ExultEngine to avoid circular dependencies if needed
namespace Exult { class ExultEngine; }

namespace Exult {

// This class will provide an interface for Exult_Engine_s file I/O operations
// to use ScummVM_Engine_s virtual filesystem (OSystem, FSNode, Streams).
class ExultFileAdapter {
public:
    ExultFileAdapter(OSystem* system /*, ::Exult::File_Manager* exultFileManager */);
    ~ExultFileAdapter();

    // Initializes the file adapter, potentially setting up base paths from ScummVM
    bool init(const Common::FSNode& gamePath);

    // Opens a file using ScummVM_Engine_s filesystem and returns a ScummVM stream
    // Exult_Engine_s code would then need to be adapted to read from this stream type,
    // or this adapter would provide a wrapper around ScummVM_Engine_s stream that
    // mimics Exult_Engine_s expected file stream interface.
    Common::SeekableReadStream* openFileRead(const char* filename);
    Common::WriteStream* openFileWrite(const char* filename); // For saves, logs etc.

    // Helper to check if a file exists via ScummVM_Engine_s VFS
    bool fileExists(const char* filename);

    // Get the base game path provided by ScummVM
    Common::FSNode getGamePath() const;

    // If Exult has its own file/archive handling that needs to be integrated
    // more deeply, methods for that would go here.
    // For example, registering ScummVM_Engine_s stream handlers with Exult_Engine_s archive system.

private:
    OSystem* _osystem;
    Common::FSNode _gamePathNode; // Base path for the game data

    // Pointer to Exult_Engine_s file manager if it_s being actively managed or wrapped
    // ::Exult::File_Manager* _exultFileManager;
};

} // namespace Exult
} // namespace ScummVM

#endif // EXULT_FILE_ADAPTER_H

