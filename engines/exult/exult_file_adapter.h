// engines/exult/exult_file_adapter.h

#ifndef EXULT_FILE_ADAPTER_H
#define EXULT_FILE_ADAPTER_H

#include "common/fs.h"
#include "common/stream.h"
#include "common/system.h"

// Forward declarations for Exult_Engine_s file management classes
// These would typically be found in exult_core_src/files/U7fileman.h or similar
namespace ExultCore {
    class U7FileManager; // Assuming this is Exult_Engine_s main file manager
}

namespace ScummVM {

// Forward declaration of ExultEngine to avoid circular dependencies if needed
namespace Exult { class ExultEngine; }

namespace Exult {

class ExultFileAdapter {
public:
    // Constructor now takes a pointer to Exult_Engine_s U7FileManager instance
    ExultFileAdapter(OSystem* system, ExultCore::U7FileManager* exultFileManager);
    ~ExultFileAdapter();

    bool init(const Common::FSNode& gamePath);

    // These methods will now try to use Exult_Engine_s file manager, which in turn
    // will need to be adapted to use ScummVM_Engine_s streams or VFS.
    // For now, the adapter might just pass through file requests to Exult_Engine_s FM.
    Common::SeekableReadStream* openFileRead(const char* filename);
    Common::WriteStream* openFileWrite(const char* filename);
    bool fileExists(const char* filename);

    Common::FSNode getGamePath() const;

private:
    OSystem* _osystem;
    Common::FSNode _gamePathNode;
    ExultCore::U7FileManager* _exultFileManager; // Pointer to Exult_Engine_s file manager
};

} // namespace Exult
} // namespace ScummVM

#endif // EXULT_FILE_ADAPTER_H

