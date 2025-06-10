/*
 *  Copyright (C) 2001-2022  The Exult Team
 */

#include "exult_graphics_adapter.h"
#include "common/debug.h"
#include "exult_core_src/imagewin/imagewin.h"

namespace Exult {

ExultGraphicsAdapter::ExultGraphicsAdapter(OSystem* o) : ScummVM::Graphics::GraphicsAdapter(o), _image_window(nullptr) {
	debug(1, "ExultGraphicsAdapter: Constructor");
}

ExultGraphicsAdapter::~ExultGraphicsAdapter() {
	debug(1, "ExultGraphicsAdapter: Destructor");
}

void ExultGraphicsAdapter::init() {
	debug(1, "ExultGraphicsAdapter: init()");
	// Get the singleton instance of ExultCore::Image_window
	_image_window = ExultCore::Image_window::get_ptr();

	// ExultCore::Image_window::create_surface will be called by ExultEngine init.
	// Here, we might set up any ScummVM-specific graphics contexts if needed.
}

void ExultGraphicsAdapter::shutdown() {
	debug(1, "ExultGraphicsAdapter: shutdown()");
	// No specific shutdown needed for Image_window as it's a singleton managed by ExultCore.
}

void ExultGraphicsAdapter::updateScreen() {
	// Request ExultCore::Image_window to update its internal screen representation.
	// In ScummVM mode, this will trigger a redraw request to ScummVM.
	_image_window->update_screen();
}

void ExultGraphicsAdapter::setPalette(const Common::Palette& palette) {
	// Convert ScummVM palette to ExultCore::Palette format and set it.
	ExultCore::Palette exult_palette;
	for (int i = 0; i < 256; ++i) {
		const Common::PaletteEntry& entry = palette.getEntry(i);
		exult_palette.set_rgb(i, entry.red, entry.green, entry.blue);
	}
	_image_window->set_palette(&exult_palette);
}

void ExultGraphicsAdapter::blitSurface(const Common::Surface* src, int x, int y, int w, int h) {
	// This is a complex operation. It involves blitting a ScummVM Common::Surface
	// onto ExultCore::Image_window's internal paletted_surface.
	// This will require converting the source surface format to Exult's 8-bit paletted format.

	// For simplicity, assuming src is an 8-bit paletted surface for now.
	// If src is not 8-bit, it needs conversion.

	SDL_Surface* target_surface = _image_window->get_paletted_surface();
	if (!target_surface) {
		error("ExultGraphicsAdapter: Target paletted surface is null for blitSurface.");
		return;
	}

	// Create a temporary SDL_Surface from the ScummVM Common::Surface
	// This is a simplified example, actual implementation needs to handle various formats.
	SDL_Surface* src_sdl_surface = SDL_CreateRGBSurfaceWithFormatFrom(
		(void*)src->getPixels(), src->getWidth(), src->getHeight(), src->getBitsPerPixel(), src->getPitch(),
		SDL_PIXELFORMAT_INDEX8 // Assuming 8-bit paletted for now
	);

	if (!src_sdl_surface) {
		error("ExultGraphicsAdapter: Failed to create SDL_Surface from Common::Surface for blit: %s", SDL_GetError());
		return;
	}

	// Set palette for the source SDL surface if it's paletted
	if (src->getBitsPerPixel() == 8) {
		SDL_Color colors[256];
		for (int i = 0; i < 256; ++i) {
			const Common::PaletteEntry& entry = src->getPalette()->getEntry(i);
			colors[i].r = entry.red;
			colors[i].g = entry.green;
			colors[i].b = entry.blue;
			colors[i].a = SDL_ALPHA_OPAQUE;
		}
		SDL_SetPaletteColors(src_sdl_surface->format->palette, colors, 0, 256);
	}

	SDL_Rect dest_rect = {x, y, w, h};
	SDL_BlitSurface(src_sdl_surface, NULL, target_surface, &dest_rect);

	SDL_FreeSurface(src_sdl_surface);
}

void ExultGraphicsAdapter::fillRect(int x, int y, int w, int h, uint32 color) {
	SDL_Surface* target_surface = _image_window->get_paletted_surface();
	if (!target_surface) {
		error("ExultGraphicsAdapter: Target paletted surface is null for fillRect.");
		return;
	}

	SDL_Rect rect = {x, y, w, h};
	// For an 8-bit paletted surface, the color needs to be a palette index.
	// This assumes `color` is already the correct palette index.
	// If `color` is an RGB value, it needs to be mapped to the closest palette index.
	SDL_FillRect(target_surface, &rect, color);
}

} // namespace Exult

