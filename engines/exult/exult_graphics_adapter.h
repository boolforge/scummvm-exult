/*
 *  Copyright (C) 2001-2022  The Exult Team
 */

#ifndef EXULT_GRAPHICS_ADAPTER_H
#define EXULT_GRAPHICS_ADAPTER_H

#include "common/graphics.h"
#include "common/types.h"

namespace Exult {

class ExultGraphicsAdapter : public ScummVM::Graphics::GraphicsAdapter {
public:
	ExultGraphicsAdapter(OSystem* o);
	~ExultGraphicsAdapter() override;

	// ScummVM Graphics::GraphicsAdapter interface
	void init() override;
	void shutdown() override;
	void updateScreen() override;
	void setPalette(const Common::Palette& palette) override;
	void blitSurface(const Common::Surface* src, int x, int y, int w, int h) override;
	void fillRect(int x, int y, int w, int h, uint32 color) override;

private:
	// Reference to the ExultCore::Image_window instance
	// This is a singleton managed by ExultCore, so we don't own it.
	ExultCore::Image_window* _image_window;
};

} // namespace Exult

#endif // EXULT_GRAPHICS_ADAPTER_H

