/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "common/system.h"
#include "common/textconsole.h"
#include "graphics/palette.h"
#include "graphics/cursorman.h"
#include "graphics/font.h"
#include "graphics/fonts/ttf.h"

#include "ack/ack.h"
#include "ack/graphics.h"

namespace Ack {

Graphics::Graphics(AckEngine *vm) : _vm(vm), _surface(nullptr), 
    _screenWidth(320), _screenHeight(200), _mouseCursorVisible(true) {
    // Initialize palette to grayscale
    for (int i = 0; i < 256; i++) {
        _palette[i * 3 + 0] = i;
        _palette[i * 3 + 1] = i;
        _palette[i * 3 + 2] = i;
    }
}

Graphics::~Graphics() {
    if (_surface) {
        _surface->free();
        delete _surface;
    }
}

/**
 * Initialize the graphics system
 */
void Graphics::initGraphics() {
    // Initialize the screen surface
    _surface = new Graphics::Surface();
    _surface->create(_screenWidth, _screenHeight, Graphics::PixelFormat::createFormatCLUT8());
    
    // Clear the screen
    clearScreen();
    
    // Set the initial palette
    setPalette(_palette, 0, 256);
    
    // Show the mouse cursor
    setMouseCursor(true);
}

/**
 * Update the screen
 */
void Graphics::updateScreen() {
    if (!_surface)
        return;
        
    // Copy the surface to the screen
    g_system->copyRectToScreen(_surface->getPixels(), _surface->pitch, 0, 0, _surface->w, _surface->h);
    
    // Update the screen
    g_system->updateScreen();
}

/**
 * Clear the screen
 */
void Graphics::clearScreen() {
    if (!_surface)
        return;
        
    // Clear the surface
    _surface->fillRect(Common::Rect(0, 0, _surface->w, _surface->h), 0);
    
    // Update the screen
    updateScreen();
}

/**
 * Draw a line
 * 
 * @param x1 Starting x coordinate
 * @param y1 Starting y coordinate
 * @param x2 Ending x coordinate
 * @param y2 Ending y coordinate
 * @param color Color to use
 */
void Graphics::drawLine(int x1, int y1, int x2, int y2, byte color) {
    if (!_surface)
        return;
        
    // Draw the line
    _surface->drawLine(x1, y1, x2, y2, color);
}

/**
 * Draw a box
 * 
 * @param x1 Top-left x coordinate
 * @param y1 Top-left y coordinate
 * @param x2 Bottom-right x coordinate
 * @param y2 Bottom-right y coordinate
 * @param color Color to use
 */
void Graphics::drawBox(int x1, int y1, int x2, int y2, byte color) {
    if (!_surface)
        return;
        
    // Draw the box
    _surface->frameRect(Common::Rect(x1, y1, x2 + 1, y2 + 1), color);
}

/**
 * Fill a box
 * 
 * @param x1 Top-left x coordinate
 * @param y1 Top-left y coordinate
 * @param x2 Bottom-right x coordinate
 * @param y2 Bottom-right y coordinate
 * @param color Color to use
 */
void Graphics::fillBox(int x1, int y1, int x2, int y2, byte color) {
    if (!_surface)
        return;
        
    // Fill the box
    _surface->fillRect(Common::Rect(x1, y1, x2 + 1, y2 + 1), color);
}

/**
 * Draw text
 * 
 * @param x X coordinate
 * @param y Y coordinate
 * @param text Text to draw
 * @param color Color to use
 */
void Graphics::drawText(int x, int y, const Common::String &text, byte color) {
    if (!_surface)
        return;
        
    // Draw the text
    // In ScummVM, we would typically use the built-in font renderer
    // For now, we'll just use a simple implementation
    const Graphics::Font *font = Graphics::FontManager::getFontByUsage(Graphics::FontManager::kGUIFont);
    font->drawString(_surface, text, x, y, _surface->w - x, color);
}

/**
 * Set the palette
 * 
 * @param palette Palette data (R,G,B triplets)
 * @param start Starting index
 * @param num Number of entries
 */
void Graphics::setPalette(const byte *palette, int start, int num) {
    if (!palette)
        return;
        
    // Copy the palette data
    for (int i = 0; i < num * 3; i++) {
        _palette[start * 3 + i] = palette[i];
    }
    
    // Set the palette
    g_system->getPaletteManager()->setPalette(_palette + start * 3, start, num);
}

/**
 * Set a palette entry
 * 
 * @param index Palette index
 * @param r Red component
 * @param g Green component
 * @param b Blue component
 */
void Graphics::setPaletteEntry(int index, byte r, byte g, byte b) {
    if (index < 0 || index >= 256)
        return;
        
    // Set the palette entry
    _palette[index * 3 + 0] = r;
    _palette[index * 3 + 1] = g;
    _palette[index * 3 + 2] = b;
    
    // Update the palette
    g_system->getPaletteManager()->setPalette(_palette + index * 3, index, 1);
}

/**
 * Fade in the screen
 * 
 * @param steps Number of steps
 * @param delay Delay between steps
 */
void Graphics::fadeIn(int steps, int delay) {
    byte tempPalette[256 * 3];
    
    // Fade in
    for (int step = 0; step < steps; step++) {
        for (int i = 0; i < 256 * 3; i++) {
            tempPalette[i] = _palette[i] * step / steps;
        }
        
        g_system->getPaletteManager()->setPalette(tempPalette, 0, 256);
        g_system->updateScreen();
        g_system->delayMillis(delay);
    }
    
    // Set the final palette
    g_system->getPaletteManager()->setPalette(_palette, 0, 256);
    g_system->updateScreen();
}

/**
 * Fade out the screen
 * 
 * @param steps Number of steps
 * @param delay Delay between steps
 */
void Graphics::fadeOut(int steps, int delay) {
    byte tempPalette[256 * 3];
    
    // Fade out
    for (int step = steps - 1; step >= 0; step--) {
        for (int i = 0; i < 256 * 3; i++) {
            tempPalette[i] = _palette[i] * step / steps;
        }
        
        g_system->getPaletteManager()->setPalette(tempPalette, 0, 256);
        g_system->updateScreen();
        g_system->delayMillis(delay);
    }
    
    // Set the final palette (all black)
    memset(tempPalette, 0, sizeof(tempPalette));
    g_system->getPaletteManager()->setPalette(tempPalette, 0, 256);
    g_system->updateScreen();
}

/**
 * Set the mouse cursor visibility
 * 
 * @param visible Whether the cursor should be visible
 */
void Graphics::setMouseCursor(bool visible) {
    _mouseCursorVisible = visible;
    
    if (visible) {
        CursorMan.showMouse(true);
    } else {
        CursorMan.showMouse(false);
    }
}

} // End of namespace Ack
