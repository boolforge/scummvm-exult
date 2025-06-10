#include "engine.h"
#include "metaengine.h"
#include "ScummVMWindowAdapter.h"
#include "ScummVMAudioAdapter.h"
#include "ScummVMFileAdapter.h"
#include "ScummVMTimerAdapter.h"
#include "GumpsFacade.h"
#include "AudioFacade.h"
#include "FileFacade.h"
#include "TimerFacade.h"

// Forward declaration of ExultCore (assuming it will be linked)
namespace ExultCore {
    void init(OSystem* sys, IWindow* window, IAudioStream* audio, IFileSource* file, ITimer* timer);
    void tick();
    void render();
}

class ExultEngine : public Engine {
private:
    ScummVMWindowAdapter* _window;
    ScummVMAudioAdapter* _audio;
    ScummVMFileAdapter* _file;
    ScummVMTimerAdapter* _timer;
    GumpsFacade* _gumpsFacade;
    AudioFacade* _audioFacade;
    FileFacade* _fileFacade;
    TimerFacade* _timerFacade;

public:
    ExultEngine(OSystem* s) : Engine(s) {
        _window = new ScummVMWindowAdapter(s);
        _audio = new ScummVMAudioAdapter(s->getMixer());
        // TODO: Pass a valid SeekableReadStream to ScummVMFileAdapter constructor
        _file = new ScummVMFileAdapter(nullptr); // Placeholder
        _timer = new ScummVMTimerAdapter();

        _gumpsFacade = new GumpsFacade(_window);
        _audioFacade = new AudioFacade(_audio);
        _fileFacade = new FileFacade(_file);
        _timerFacade = new TimerFacade(_timer);
    }

    ~ExultEngine() {
        delete _window;
        delete _audio;
        delete _file;
        delete _timer;
        delete _gumpsFacade;
        delete _audioFacade;
        delete _fileFacade;
        delete _timerFacade;
    }

    void initialize(const Common::FSNode &gamePath) override {
        // Call ExultCore::init with adapters
        ExultCore::init(_system, _window, _audio, _file, _timer);
    }

    void run() override {
        while (!shouldQuit()) {
            // 1. Event loop
            Common::Event ev;
            while (getEventManager()->pollEvent(ev)) {
                // translate to Exult input...
            }
            // 2. Tick
            ExultCore::tick();
            // 3. Render
            ExultCore::render();
            // 4. Present & yield
            _window->present();
            _system->yield();
        }
    }
};

class ExultMetaEngine : public MetaEngine {
public:
    ExultMetaEngine() : MetaEngine("exult") {}

    Engine* createEngine(OSystem* s) override {
        return new ExultEngine(s);
    }
};

MetaEngine* getMetaEngine() {
    return new ExultMetaEngine();
}


