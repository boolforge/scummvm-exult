/*
 *  Copyright (C) 2001-2022  The Exult Team
 */

#include "exult_engine.h"
#include "common/debug.h"
#include "common/fs.h"
#include "common/system.h"
#include "exult_constants.h"
#include "exult_file_adapter.h"
#include "exult_graphics_adapter.h"
#include "exult_input_adapter.h"
#include "exult_audio_adapter.h"
#include "exult_core_src/exult.h"
#include "exult_core_src/files/U7fileman.h"
#include "exult_core_src/imagewin/imagewin.h"

namespace Exult {

ExultEngine::ExultEngine(OSystem* o, const char* data_dir)
	: Engine(o), _data_dir(data_dir), _graphics(nullptr), _input(nullptr), _audio(nullptr), _file_adapter(nullptr) {
	debug(1, "ExultEngine: Constructor");
}

ExultEngine::~ExultEngine() {
	debug(1, "ExultEngine: Destructor");
	// Adapters are owned by ScummVM and will be deleted by it.
}

Common::Error ExultEngine::init() {
	debug(1, "ExultEngine: init()");

	// Create and initialize adapters
	_file_adapter = new ScummVM::FileAdapter(this, _data_dir.c_str());
	ExultCore::U7FileManager::setScummVMFileAdapter(_file_adapter);

	_graphics = new ExultGraphicsAdapter(this);
	_input = new ExultInputAdapter(this);
	_audio = new ExultAudioAdapter(this, _mixer);

	// Initialize Exult core systems
	ExultCore::Image_window::setScummVMMode(true);
	ExultCore::Audio::get_ptr()->setScummVMMode(true, _mixer->getCapabilities().preferredRate, _mixer->getCapabilities().channels);

	// Initialize Exult game
	if (!ExultCore::Game::get_ptr()->init(this)) {
		return Common::kError_GameInitFailed;
	}

	return Common::kError_NoError;
}

void ExultEngine::run() {
	debug(1, "ExultEngine: run()");
	ExultCore::Game::get_ptr()->run();
}

void ExultEngine::shutdown() {
	debug(1, "ExultEngine: shutdown()");
	ExultCore::Game::get_ptr()->shutdown();
}

} // namespace Exult

