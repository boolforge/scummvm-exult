/*
 *  Copyright (C) 2001-2022  The Exult Team
 */

#ifndef EXULT_ENGINE_H
#define EXULT_ENGINE_H

#include "common/engine.h"
#include "common/types.h"

namespace ScummVM {
	class Mixer;
}

namespace Exult {

class ExultFileAdapter;
class ExultGraphicsAdapter;
class ExultInputAdapter;
class ExultAudioAdapter;

class ExultEngine : public Engine {
public:
	ExultEngine(OSystem* o, const char* data_dir);
	~ExultEngine() override;

	Common::Error init() override;
	void run() override;
	void shutdown() override;

private:
	std::string _data_dir;
	ExultGraphicsAdapter* _graphics;
	ExultInputAdapter* _input;
	ExultAudioAdapter* _audio;
	ExultFileAdapter* _file_adapter;
};

} // namespace Exult

#endif // EXULT_ENGINE_H

