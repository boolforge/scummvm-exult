/*
 *  Copyright (C) 2001-2022  The Exult Team
 */

#ifndef EXULT_INPUT_ADAPTER_H
#define EXULT_INPUT_ADAPTER_H

#include "common/input.h"
#include "common/types.h"

namespace Exult {

class ExultInputAdapter : public ScummVM::Input::InputAdapter {
public:
	ExultInputAdapter(OSystem* o);
	~ExultInputAdapter() override;

	// ScummVM Input::InputAdapter interface
	void init() override;
	void shutdown() override;
	void handleEvent(const Common::Event& event) override;

private:
	// Reference to the ExultCore::InputManager instance
	// This is a singleton managed by ExultCore, so we don't own it.
	ExultCore::InputManager* _input_manager;
};

} // namespace Exult

#endif // EXULT_INPUT_ADAPTER_H

