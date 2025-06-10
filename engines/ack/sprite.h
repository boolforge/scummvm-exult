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

#ifndef ACK_SPRITE_H
#define ACK_SPRITE_H

#include "common/scummsys.h"
#include "common/rect.h"
#include "graphics/surface.h"

namespace Ack {

class AckEngine;

/**
 * Sprite manager for ACK engine
 * Handles sprite loading, manipulation, and rendering
 */
class SpriteManager {
private:
    AckEngine *_vm;
    
    // Sprite data
    struct Sprite {
        byte *data;
        int width;
        int height;
        
        Sprite() : data(nullptr), width(0), height(0) {}
        ~Sprite() { free(data); }
    };
    
    Common::Array<Sprite> _sprites;
    
public:
    SpriteManager(AckEngine *vm);
    ~SpriteManager();
    
    // Sprite loading
    bool loadSprite(int spriteId, const byte *data, int width, int height);
    void unloadSprite(int spriteId);
    
    // Sprite rendering
    void drawSprite(int spriteId, int x, int y);
    void drawSpriteWithMask(int spriteId, int maskId, int x, int y);
    
    // Sprite manipulation
    void flipSpriteHorizontal(int spriteId);
    void flipSpriteVertical(int spriteId);
    void rotateSprite(int spriteId, int angle);
};

} // End of namespace Ack

#endif // ACK_SPRITE_H
