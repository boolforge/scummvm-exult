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

#ifndef ACK_PICTURE_H
#define ACK_PICTURE_H

#include "common/scummsys.h"
#include "common/rect.h"
#include "graphics/surface.h"

namespace Ack {

class AckEngine;

/**
 * Picture manager for ACK engine
 * Handles background pictures loading and display
 */
class PictureManager {
private:
    AckEngine *_vm;
    
    // Current picture data
    int _currentPicture;
    byte *_pictureData;
    int _pictureWidth;
    int _pictureHeight;
    
public:
    PictureManager(AckEngine *vm);
    ~PictureManager();
    
    // Picture loading and display
    bool loadPicture(int pictureId);
    void showPicture(int pictureId);
    void clearPicture();
    
    // Accessors
    int getCurrentPicture() const { return _currentPicture; }
    byte *getPictureData() const { return _pictureData; }
    int getPictureWidth() const { return _pictureWidth; }
    int getPictureHeight() const { return _pictureHeight; }
};

} // End of namespace Ack

#endif // ACK_PICTURE_H
