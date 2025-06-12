// engines/exult/exult_engine.h

#ifndef EXULT_ENGINE_H
#define EXULT_ENGINE_H

#include "engines/engine.h"
#include "common/config-manager.h"
#include "common/fs.h"
#include "engines/metaengine.h"
#include "engines/game.h" // Required for GameDescription and PlainGameList
#include "common/error.h" // Required for Common::Error and kNoError/kErrorSystem

// Forward declarations for our adapters
namespace ScummVM {
namespace Exult {
    class ExultGraphicsAdapter;
    class ExultInputAdapter;
    class ExultAudioAdapter;
    class ExultFileAdapter;
} // namespace Exult
} // namespace ScummVM

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

    virtual Common::Error initialize(const Common::FSNode& gamePath, const Common::String& gameLanguage = "en");
    virtual Common::Error run(); // Corrected return type to Common::Error
    void shutdown(); // Engine::shutdown() is not virtual, so no override

private:
    ExultGraphicsAdapter* _graphicsAdapter;
    ExultInputAdapter* _inputAdapter;
    ExultAudioAdapter* _audioAdapter;
    ExultFileAdapter* _fileAdapter;

    bool _initialized;
    Common::FSNode _gamePath;

    void processInputEvents();
    void updateGameLogic();
    void renderFrame();
};

// ExultMetaEngine should inherit from MetaEngineDetection to override identifyGame and getSupportedGames
class ExultMetaEngine : public MetaEngineDetection {
public:
    ExultMetaEngine();

    virtual const char *getName() const override { return "Exult"; } // Added override
    virtual const char *getEngineName() const override { return "Exult"; } // Added override
    virtual const char *getOriginalCopyright() const override { return "Exult Team"; } // Added override
    virtual const char *getDesc() const { return "Exult engine for Ultima VII: The Black Gate and Serpent Isle"; }

    virtual Common::Error identifyGame(DetectedGame &game, const void **descriptor) override;
    virtual PlainGameList getSupportedGames() const override;
    virtual DetectedGames detectGames(const Common::FSList &fslist, uint32 skipADFlags = 0, bool skipIncomplete = false) override; // Added override
    virtual uint getMD5Bytes() const override { return 0; } // Added override
    virtual void dumpDetectionEntries() const override; // Added override

    // These are not part of MetaEngineDetection, but are needed for the engine to function
    // They will be called by the main ScummVM application through the MetaEngine interface
    Engine *createInstance(OSystem *syst, const Common::FSNode& gamePath, const Common::String& gameLanguage = "en", const void *meDesc = nullptr);
    void freeInstance(Engine *engine);
};

} // namespace Exult
} // namespace ScummVM

#endif // EXULT_ENGINE_H


