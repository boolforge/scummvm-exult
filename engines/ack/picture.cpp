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
#include "ack/picture.h"
#include "ack/graphics.h"

namespace Ack {

PictureManager::PictureManager(AckEngine *vm) : _vm(vm), _currentPicture(-1), 
    _pictureData(nullptr), _pictureWidth(0), _pictureHeight(0) {
}

PictureManager::~PictureManager() {
    free(_pictureData);
}

/**
 * Load a picture from resource
 * 
 * @param pictureId The ID of the picture to load
 * @return True if the picture was loaded successfully, false otherwise
 */
bool PictureManager::loadPicture(int pictureId) {
    // Load the picture resource
    if (!_vm->loadResource(RESOURCETYPE_PICTURE, pictureId)) {
        warning("Failed to load picture %d", pictureId);
        return false;
    }
    
    // Free any existing picture data
    free(_pictureData);
    _pictureData = nullptr;
    
    // Get the picture data from the resource
    byte *data = _vm->_pictures[pictureId].rdata;
    if (!data) {
        warning("Picture %d has no data", pictureId);
        return false;
    }
    
    // Parse the picture header
    // Format depends on the specific ACK version, but typically:
    // - First 2 bytes: width
    // - Next 2 bytes: height
    // - Remaining bytes: picture data
    _pictureWidth = READ_LE_UINT16(data);
    _pictureHeight = READ_LE_UINT16(data + 2);
    
    // Allocate memory for the picture data
    int size = _pictureWidth * _pictureHeight;
    _pictureData = (byte *)malloc(size);
    if (!_pictureData) {
        warning("Could not allocate memory for picture %d", pictureId);
        return false;
    }
    
    // Copy the picture data
    memcpy(_pictureData, data + 4, size);
    
    // Set the current picture
    _currentPicture = pictureId;
    
    return true;
}

/**
 * Show a picture on the screen
 * 
 * @param pictureId The ID of the picture to show
 */
void PictureManager::showPicture(int pictureId) {
    // Load the picture if it's not already loaded
    if (_currentPicture != pictureId) {
        if (!loadPicture(pictureId)) {
            return;
        }
    }
    
    // Get the surface to draw on
    Graphics::Surface *surface = _vm->getGraphics()->getSurface();
    
    // Draw the picture
    for (int y = 0; y < _pictureHeight && y < surface->h; y++) {
        byte *dst = (byte *)surface->getBasePtr(0, y);
        byte *src = _pictureData + (y * _pictureWidth);
        
        for (int x = 0; x < _pictureWidth && x < surface->w; x++) {
            dst[x] = src[x];
        }
    }
    
    // Update screen
    _vm->getGraphics()->updateScreen();
}

/**
 * Clear the current picture
 */
void PictureManager::clearPicture() {
    // Free the picture data
    free(_pictureData);
    _pictureData = nullptr;
    _pictureWidth = 0;
    _pictureHeight = 0;
    _currentPicture = -1;
    
    // Clear the screen
    _vm->getGraphics()->clearScreen();
}

} // End of namespace Ack
