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

#ifndef ACK_GRAPHICS_H
#define ACK_GRAPHICS_H

#include "common/scummsys.h"
#include "common/rect.h"
#include "graphics/surface.h"
#include "graphics/palette.h"

namespace Ack {

class AckEngine;

/**
 * Graphics manager for ACK engine
 */
class Graphics {
private:
    AckEngine *_vm;
    Graphics::Surface *_surface;
    
    // Palette data
    byte _palette[256 * 3];
    
    // Screen dimensions
    int _screenWidth;
    int _screenHeight;
    
    // Mouse cursor
    bool _mouseCursorVisible;
    
public:
    Graphics(AckEngine *vm);
    ~Graphics();
    
    // Initialization
    void initGraphics();
    
    // Screen update
    void updateScreen();
    
    // Drawing functions
    void clearScreen();
    void drawLine(int x1, int y1, int x2, int y2, byte color);
    void drawBox(int x1, int y1, int x2, int y2, byte color);
    void fillBox(int x1, int y1, int x2, int y2, byte color);
    void drawText(int x, int y, const Common::String &text, byte color);
    
    // Palette functions
    void setPalette(const byte *palette, int start, int num);
    void setPaletteEntry(int index, byte r, byte g, byte b);
    void fadeIn(int steps = 16, int delay = 10);
    void fadeOut(int steps = 16, int delay = 10);
    
    // Mouse cursor
    void setMouseCursor(bool visible = true);
    
    // Accessors
    int getScreenWidth() const { return _screenWidth; }
    int getScreenHeight() const { return _screenHeight; }
    Graphics::Surface *getSurface() const { return _surface; }
};

} // End of namespace Ack

#endif // ACK_GRAPHICS_H
