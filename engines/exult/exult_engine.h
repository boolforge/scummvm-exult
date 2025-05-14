// engines/exult/exult_engine.h

#ifndef EXULT_ENGINE_H
#define EXULT_ENGINE_H

#include "engines/engine.h"
#include "common/config-manager.h"
#include "common/fsnode.h"
#include "metaengine/metaengine.h"

// Forward declarations for our adapters
namespace ScummVM {
namespace Exult {
    class ExultGraphicsAdapter;
    class ExultInputAdapter;
    class ExultAudioAdapter;
    class ExultFileAdapter;
} // namespace Exult
} // namespace ScummVM

// Forward declarations for Exult_Engine_s core components (to be defined/included later)
// namespace ExultCore {
//     class Game_Manager;
//     class Usecode_Machine;
//     // ... other Exult classes
// }

namespace ScummVM {
namespace Exult {

class ExultEngine : public Engine {
public:
    ExultEngine(OSystem *system, const Common::FSNode& gamePath, const Common::String& gameLanguage = "en");
    virtual ~ExultEngine();

    virtual const char *getName() const { return "Exult"; }
    virtual const char *getOriginalName() const { return "Ultima VII"; }
    virtual const char *getEngineVersion() const { return "0.1-dev"; } 
    virtual const char *getGameVersion() const { return "Various"; } 

    virtual Error initialize(const Common::FSNode& gamePath, const Common::String& gameLanguage = "en");
    virtual void run();
    virtual void shutdown();

    // Accessors for adapters if needed by other parts of the bridge (unlikely for now)
    // ExultGraphicsAdapter* getGraphicsAdapter() { return _graphicsAdapter; }
    // ExultInputAdapter* getInputAdapter() { return _inputAdapter; }
    // ExultAudioAdapter* getAudioAdapter() { return _audioAdapter; }
    // ExultFileAdapter* getFileAdapter() { return _fileAdapter; }

private:
    // Adapters for ScummVM subsystems
    ExultGraphicsAdapter* _graphicsAdapter;
    ExultInputAdapter* _inputAdapter;
    ExultAudioAdapter* _audioAdapter;
    ExultFileAdapter* _fileAdapter;

    // Pointers to Exult_Engine_s core systems (bridged or direct)
    // ::ExultCore::Game_Manager* _exultGameManager;
    // ::ExultCore::Usecode_Machine* _exultUsecodeVM;
    // ...

    bool _initialized;
    Common::FSNode _gamePath;

    // These methods will now likely use the adapters
    void processInputEvents(); // Uses _inputAdapter
    void updateGameLogic();    // Calls Exult_Engine_s core logic
    void renderFrame();        // Uses _graphicsAdapter
};

class ExultMetaEngine : public MetaEngine {
public:
    ExultMetaEngine();

    virtual const char *getName() const { return "Exult"; }
    virtual const char *getOriginalName() const { return "Ultima VII"; }
    virtual const char *getDesc() const { return "Exult engine for Ultima VII: The Black Gate and Serpent Isle"; }

    virtual bool canDetect(OSystem *syst, const Common::FSNode& node, DetectionLevel level = kDetectionLevel_Complete) const;
    virtual Engine *createInstance(OSystem *syst, const Common::FSNode& gamePath, const Common::String& gameLanguage = "en", const void *meDesc = nullptr);
    virtual void getSupportedGames(Common::Array<GameDescription> &games) const;
    virtual void freeInstance(Engine *engine);
};

} // namespace Exult
} // namespace ScummVM

#endif // EXULT_ENGINE_H

