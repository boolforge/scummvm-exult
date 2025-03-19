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

#ifndef ACK_GRAPHICS_RENDERER_H
#define ACK_GRAPHICS_RENDERER_H

#include "common/scummsys.h"
#include "common/str.h"

namespace Ack {

class AckEngine;

/**
 * Graphics renderer for ACK engine
 * Handles specialized rendering functions beyond basic graphics operations
 */
class GraphicsRenderer {
private:
    AckEngine *_vm;

public:
    GraphicsRenderer(AckEngine *vm);
    ~GraphicsRenderer();

    // Menu skin loading and display
    void menuSkinBmp(const Common::String &systemDir);

    // Icon handling
    void loadIcons(const Common::String &filename);
    void saveIcons(const Common::String &filename);
    void putIcon(int x, int y, int iconIndex);

    // Option display
    void showOption(byte x, byte n, int16 mo);

    // Palette handling
    void loadBmpPalette(uint16 version, const Common::String &advName, const Common::String &systemDir);
};

} // End of namespace Ack

#endif // ACK_GRAPHICS_RENDERER_H
