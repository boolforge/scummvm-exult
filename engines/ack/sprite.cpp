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
#include "graphics/surface.h"

#include "ack/ack.h"
#include "ack/sprite.h"
#include "ack/graphics.h"

namespace Ack {

SpriteManager::SpriteManager(AckEngine *vm) : _vm(vm) {
}

SpriteManager::~SpriteManager() {
    // Sprites are automatically cleaned up by the Common::Array destructor
    // which calls the Sprite destructor for each element
}

/**
 * Load a sprite from raw data
 * 
 * @param spriteId The ID to assign to the sprite
 * @param data The raw sprite data
 * @param width The width of the sprite
 * @param height The height of the sprite
 * @return True if the sprite was loaded successfully, false otherwise
 */
bool SpriteManager::loadSprite(int spriteId, const byte *data, int width, int height) {
    if (spriteId < 0) {
        warning("Invalid sprite ID: %d", spriteId);
        return false;
    }
    
    // Ensure the sprite array is large enough
    while (_sprites.size() <= (uint)spriteId) {
        _sprites.push_back(Sprite());
    }
    
    // Free any existing sprite data
    free(_sprites[spriteId].data);
    
    // Allocate memory for the sprite data
    int size = width * height;
    _sprites[spriteId].data = (byte *)malloc(size);
    if (!_sprites[spriteId].data) {
        warning("Could not allocate memory for sprite %d", spriteId);
        return false;
    }
    
    // Copy the sprite data
    memcpy(_sprites[spriteId].data, data, size);
    _sprites[spriteId].width = width;
    _sprites[spriteId].height = height;
    
    return true;
}

/**
 * Unload a sprite
 * 
 * @param spriteId The ID of the sprite to unload
 */
void SpriteManager::unloadSprite(int spriteId) {
    if (spriteId < 0 || spriteId >= (int)_sprites.size()) {
        warning("Invalid sprite ID: %d", spriteId);
        return;
    }
    
    // Free the sprite data
    free(_sprites[spriteId].data);
    _sprites[spriteId].data = nullptr;
    _sprites[spriteId].width = 0;
    _sprites[spriteId].height = 0;
}

/**
 * Draw a sprite on the screen
 * 
 * @param spriteId The ID of the sprite to draw
 * @param x The x coordinate to draw at
 * @param y The y coordinate to draw at
 */
void SpriteManager::drawSprite(int spriteId, int x, int y) {
    if (spriteId < 0 || spriteId >= (int)_sprites.size() || !_sprites[spriteId].data) {
        warning("Invalid sprite ID: %d", spriteId);
        return;
    }
    
    // Get the sprite data
    byte *data = _sprites[spriteId].data;
    int width = _sprites[spriteId].width;
    int height = _sprites[spriteId].height;
    
    // Get the surface to draw on
    Graphics::Surface *surface = _vm->getGraphics()->getSurface();
    
    // Draw the sprite
    for (int yy = 0; yy < height; yy++) {
        byte *dst = (byte *)surface->getBasePtr(x, y + yy);
        byte *src = data + (yy * width);
        
        for (int xx = 0; xx < width; xx++) {
            // Skip transparent pixels (value 0)
            if (src[xx] != 0) {
                dst[xx] = src[xx];
            }
        }
    }
    
    // Update screen
    _vm->getGraphics()->updateScreen();
}

/**
 * Draw a sprite with a mask on the screen
 * 
 * @param spriteId The ID of the sprite to draw
 * @param maskId The ID of the mask to use
 * @param x The x coordinate to draw at
 * @param y The y coordinate to draw at
 */
void SpriteManager::drawSpriteWithMask(int spriteId, int maskId, int x, int y) {
    if (spriteId < 0 || spriteId >= (int)_sprites.size() || !_sprites[spriteId].data) {
        warning("Invalid sprite ID: %d", spriteId);
        return;
    }
    
    if (maskId < 0 || maskId >= (int)_sprites.size() || !_sprites[maskId].data) {
        warning("Invalid mask ID: %d", maskId);
        return;
    }
    
    // Get the sprite data
    byte *data = _sprites[spriteId].data;
    int width = _sprites[spriteId].width;
    int height = _sprites[spriteId].height;
    
    // Get the mask data
    byte *mask = _sprites[maskId].data;
    int maskWidth = _sprites[maskId].width;
    int maskHeight = _sprites[maskId].height;
    
    // Use the smaller dimensions
    width = MIN(width, maskWidth);
    height = MIN(height, maskHeight);
    
    // Get the surface to draw on
    Graphics::Surface *surface = _vm->getGraphics()->getSurface();
    
    // Draw the sprite with mask
    for (int yy = 0; yy < height; yy++) {
        byte *dst = (byte *)surface->getBasePtr(x, y + yy);
        byte *src = data + (yy * width);
        byte *msk = mask + (yy * width);
        
        for (int xx = 0; xx < width; xx++) {
            // Only draw pixels where the mask is non-zero
            if (msk[xx] != 0 && src[xx] != 0) {
                dst[xx] = src[xx];
            }
        }
    }
    
    // Update screen
    _vm->getGraphics()->updateScreen();
}

/**
 * Flip a sprite horizontally
 * 
 * @param spriteId The ID of the sprite to flip
 */
void SpriteManager::flipSpriteHorizontal(int spriteId) {
    if (spriteId < 0 || spriteId >= (int)_sprites.size() || !_sprites[spriteId].data) {
        warning("Invalid sprite ID: %d", spriteId);
        return;
    }
    
    // Get the sprite data
    byte *data = _sprites[spriteId].data;
    int width = _sprites[spriteId].width;
    int height = _sprites[spriteId].height;
    
    // Create a temporary buffer for the flipped sprite
    byte *temp = (byte *)malloc(width * height);
    if (!temp) {
        warning("Could not allocate memory for flipped sprite");
        return;
    }
    
    // Flip the sprite horizontally
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            temp[y * width + x] = data[y * width + (width - 1 - x)];
        }
    }
    
    // Copy the flipped sprite back to the original
    memcpy(data, temp, width * height);
    
    // Free the temporary buffer
    free(temp);
}

/**
 * Flip a sprite vertically
 * 
 * @param spriteId The ID of the sprite to flip
 */
void SpriteManager::flipSpriteVertical(int spriteId) {
    if (spriteId < 0 || spriteId >= (int)_sprites.size() || !_sprites[spriteId].data) {
        warning("Invalid sprite ID: %d", spriteId);
        return;
    }
    
    // Get the sprite data
    byte *data = _sprites[spriteId].data;
    int width = _sprites[spriteId].width;
    int height = _sprites[spriteId].height;
    
    // Create a temporary buffer for the flipped sprite
    byte *temp = (byte *)malloc(width * height);
    if (!temp) {
        warning("Could not allocate memory for flipped sprite");
        return;
    }
    
    // Flip the sprite vertically
    for (int y = 0; y < height; y++) {
        memcpy(temp + (height - 1 - y) * width, data + y * width, width);
    }
    
    // Copy the flipped sprite back to the original
    memcpy(data, temp, width * height);
    
    // Free the temporary buffer
    free(temp);
}

/**
 * Rotate a sprite by a given angle
 * 
 * @param spriteId The ID of the sprite to rotate
 * @param angle The angle to rotate by (0-359 degrees)
 */
void SpriteManager::rotateSprite(int spriteId, int angle) {
    if (spriteId < 0 || spriteId >= (int)_sprites.size() || !_sprites[spriteId].data) {
        warning("Invalid sprite ID: %d", spriteId);
        return;
    }
    
    // Only support 90, 180, and 270 degree rotations for now
    angle = angle % 360;
    if (angle != 90 && angle != 180 && angle != 270) {
        warning("Unsupported rotation angle: %d", angle);
        return;
    }
    
    // Get the sprite data
    byte *data = _sprites[spriteId].data;
    int width = _sprites[spriteId].width;
    int height = _sprites[spriteId].height;
    
    // Create a temporary buffer for the rotated sprite
    byte *temp = (byte *)malloc(width * height);
    if (!temp) {
        warning("Could not allocate memory for rotated sprite");
        return;
    }
    
    // Rotate the sprite
    if (angle == 90) {
        // 90 degree rotation
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                temp[(width - 1 - x) * height + y] = data[y * width + x];
            }
        }
        
        // Swap width and height
        _sprites[spriteId].width = height;
        _sprites[spriteId].height = width;
    } else if (angle == 180) {
        // 180 degree rotation
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                temp[(height - 1 - y) * width + (width - 1 - x)] = data[y * width + x];
            }
        }
    } else if (angle == 270) {
        // 270 degree rotation
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                temp[x * height + (height - 1 - y)] = data[y * width + x];
            }
        }
        
        // Swap width and height
        _sprites[spriteId].width = height;
        _sprites[spriteId].height = width;
    }
    
    // Free the original data and replace it with the rotated data
    free(data);
    _sprites[spriteId].data = temp;
}

} // End of namespace Ack
