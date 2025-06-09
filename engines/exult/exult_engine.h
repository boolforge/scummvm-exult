// engines/exult/exult_engine.h

#ifndef EXULT_ENGINE_H
#define EXULT_ENGINE_H

#include "engines/engine.h"
#include "common/config-manager.h"
#include "common/fs.h"
#include "engines/metaengine.h"
#include "engines/game.h" // Required for GameDescription

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
    virtual void run();
    virtual void shutdown();

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

class ExultMetaEngine : public MetaEngine {
public:
    ExultMetaEngine();

    virtual const char *getName() const { return "Exult"; }
    virtual const char *getOriginalName() const { return "Ultima VII"; }
    virtual const char *getDesc() const { return "Exult engine for Ultima VII: The Black Gate and Serpent Isle"; }

    virtual bool canDetect(OSystem *syst, const Common::FSNode& node, MetaEngineDetection::DetectionLevel level = MetaEngineDetection::kDetectionLevel_Complete) const;
    virtual Engine *createInstance(OSystem *syst, const Common::FSNode& gamePath, const Common::String& gameLanguage = "en", const void *meDesc = nullptr);
    virtual void getSupportedGames(Common::Array<GameDescription> &games) const;
    virtual void freeInstance(Engine *engine);
};

} // namespace Exult
} // namespace ScummVM

#endif // EXULT_ENGINE_H


