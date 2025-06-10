/*
 *  Copyright (C) 2001-2022  The Exult Team
 */

#include "imagewin.h"
#include "common/debug.h"
#include "common/types.h"
#include "exult.h"
#include "files/Palette.h"
#include "files/Palette_formats.h"
#include "files/U7file.h"
#include "files/U7fileman.h"
#include "image.h"
#include "image_formats.h"
#include "pentagram/video/SDLVideoDevice.h"

#include <algorithm>
#include <iostream>
#include <memory>

#include <SDL.h>

namespace ExultCore {

// Static member initialization
bool Image_window::_isScummVMMode = false;

Image_window::Image_window(int w, int h, int scaleval, int scaler, FillMode fillmode, int fill_scaler)
	: _width(w), _height(h), _scaleval(scaleval), _scaler(scaler), _fillmode(fillmode), _fill_scaler(fill_scaler),
	  game_width(w), game_height(h), uses_palette(false), screen_window(nullptr), screen_renderer(nullptr),
	  screen_texture(nullptr), display_surface(nullptr), inter_surface(nullptr), paletted_surface(nullptr) {
	// Constructor logic, similar to original, but SDL device creation is now conditional.
	// The actual SDL window/renderer/texture creation is deferred to create_surface.
}

Image_window::~Image_window() {
	// Cleanup SDL resources if they were created.
	// In ScummVM mode, these might not have been created by Exult.
	if (screen_texture) {
		SDL_DestroyTexture(screen_texture);
		screen_texture = nullptr;
	}
	if (screen_renderer) {
		SDL_DestroyRenderer(screen_renderer);
		screen_renderer = nullptr;
	}
	if (screen_window) {
		SDL_DestroyWindow(screen_window);
		screen_window = nullptr;
	}

	// SDL surfaces are usually managed by SDL, but if we created them, free them.
	if (display_surface) {
		SDL_FreeSurface(display_surface);
		display_surface = nullptr;
	}
	if (inter_surface) {
		SDL_FreeSurface(inter_surface);
		inter_surface = nullptr;
	}
	if (paletted_surface) {
		SDL_FreeSurface(paletted_surface);
		paletted_surface = nullptr;
	}
}

// ScummVM Integration: Implementation of setScummVMMode
void Image_window::setScummVMMode(bool enabled) {
	_isScummVMMode = enabled;
	std::cout << "Image_window: ScummVM Mode " << (enabled ? "ENABLED" : "DISABLED") << std::endl;
	if (_isScummVMMode) {
		// If mode is enabled, existing SDL window/renderer/texture should be destroyed
		// or prevented from being created. This is mainly handled in create_surface.
		if (screen_window) { // If already created
			std::cout << "Image_window: ScummVM mode enabled, existing SDL resources will be ignored or should be cleaned up." << std::endl;
			// SDL_DestroyTexture(screen_texture); screen_texture = nullptr;
			// SDL_DestroyRenderer(screen_renderer); screen_renderer = nullptr;
			// SDL_DestroyWindow(screen_window); screen_window = nullptr;
			// display_surface, inter_surface, paletted_surface might also need cleanup if they point to screen related surfaces.
		}
	}
}

void Image_window::create_surface(int w, int h, int bpp, bool fullscreen, int setup_video_type) {
	// Update internal dimensions if changed
	_width = w;
	_height = h;

	// Cleanup existing surfaces/textures/renderers/windows if they exist
	if (screen_texture) {
		SDL_DestroyTexture(screen_texture);
		screen_texture = nullptr;
	}
	if (screen_renderer) {
		SDL_DestroyRenderer(screen_renderer);
		screen_renderer = nullptr;
	}
	if (screen_window) {
		SDL_DestroyWindow(screen_window);
		screen_window = nullptr;
	}
	if (display_surface) {
		SDL_FreeSurface(display_surface);
		display_surface = nullptr;
	}
	if (inter_surface) {
		SDL_FreeSurface(inter_surface);
		inter_surface = nullptr;
	}
	if (paletted_surface) {
		SDL_FreeSurface(paletted_surface);
		paletted_surface = nullptr;
	}

	// Only create SDL window/renderer/texture if NOT in ScummVM mode.
	// In ScummVM mode, ScummVM handles the main display.
	if (!_isScummVMMode) {
		// Original SDL window/renderer/texture creation logic
		// This part remains largely the same as original Exult if not integrated with ScummVM.
		// SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear"); // Make the scaled rendering look smoother.
		// screen_window = SDL_CreateWindow("Exult", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		//                                w, h, SDL_WINDOW_SHOWN | (fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0));
		// if (!screen_window) {
		//     error("Image_window: Could not create SDL window: %s", SDL_GetError());
		//     return;
		// }
		// screen_renderer = SDL_CreateRenderer(screen_window, -1, SDL_RENDERER_ACCELERATED);
		// if (!screen_renderer) {
		//     error("Image_window: Could not create SDL renderer: %s", SDL_GetError());
		//     return;
		// }
		// screen_texture = SDL_CreateTexture(screen_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, w, h);
		// if (!screen_texture) {
		//     error("Image_window: Could not create SDL texture: %s", SDL_GetError());
		//     return;
		// }
		// debug(1, "Image_window: Created SDL window/renderer/texture in standalone mode.");
		// For now, if not in ScummVM mode, we assume Pentagram::SDLVideoDevice handles this.
		// This is a placeholder for direct SDL setup if Pentagram::SDLVideoDevice is not used.
		std::cout << "Image_window: Not in ScummVM mode. Assuming SDLVideoDevice handles display setup." << std::endl;
	}

	// Always create paletted_surface as it's Exult's internal drawing buffer.
	// This surface will be converted to ScummVM's format or directly rendered in standalone mode.
	paletted_surface = SDL_CreateRGBSurface(0, game_width, game_height, 8, 0, 0, 0, 0);
	if (!paletted_surface) {
		error("Image_window: Could not create paletted_surface: %s", SDL_GetError());
		return;
	}
	uses_palette = true; // Exult is 8-bit paletted

	// Other surfaces (display_surface, inter_surface) might be created as needed for scaling/conversion.
	// Their creation depends on the scaler and fillmode settings.

	// Scaler setup (simplified, as ScummVM might handle scaling)
	if (!Scalers[_fill_scaler].arb) {
		if (Scalers[_scaler].arb) {
			_fill_scaler = _scaler;
		} else {
			_fill_scaler = point;
		}
	}

	// If in ScummVM mode, we don't need to manage SDL display directly.
	// We just need to ensure paletted_surface is ready for drawing.
	// The ScummVM engine will then request the pixels from this surface.
	std::cout << "Image_window::create_surface finished. Paletted surface created." << std::endl;
}

void Image_window::set_palette(Palette* p) {
	if (!paletted_surface) {
		return;
	}
	// Set the palette for the paletted_surface.
	SDL_Color colors[256];
	for (int i = 0; i < 256; ++i) {
		colors[i].r = p->get_red(i);
		colors[i].g = p->get_green(i);
		colors[i].b = p->get_blue(i);
		colors[i].a = SDL_ALPHA_OPAQUE;
	}
	SDL_SetPaletteColors(paletted_surface->format->palette, colors, 0, 256);
}

void Image_window::update_screen() {
	// In ScummVM mode, this method might trigger a redraw request to ScummVM.
	// In standalone mode, it would update the SDL texture and render.
	if (_isScummVMMode) {
		// Notify ScummVM that the screen needs updating.
		// This is a conceptual call, actual implementation depends on ScummVM's API.
		// ScummVM::Video::requestRedraw(); // Example
		std::cout << "Image_window: In ScummVM mode, requesting redraw from ScummVM." << std::endl;
	} else {
		// Original standalone rendering logic
		// if (screen_texture && screen_renderer && paletted_surface) {
		//     // Convert paletted_surface to ARGB8888 for texture
		//     SDL_Surface* converted_surface = SDL_ConvertSurfaceFormat(paletted_surface, SDL_PIXELFORMAT_ARGB8888, 0);
		//     if (converted_surface) {
		//         SDL_UpdateTexture(screen_texture, NULL, converted_surface->pixels, converted_surface->pitch);
		//         SDL_FreeSurface(converted_surface);
		//         SDL_RenderClear(screen_renderer);
		//         SDL_RenderCopy(screen_renderer, screen_texture, NULL, NULL);
		//         SDL_RenderPresent(screen_renderer);
		//     }
		// }
		std::cout << "Image_window: In standalone mode, performing direct SDL update." << std::endl;
	}
}

SDL_Surface* Image_window::get_paletted_surface() {
	return paletted_surface;
}

} // namespace ExultCore

