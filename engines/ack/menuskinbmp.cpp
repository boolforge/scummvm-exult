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

#include "common/file.h"
#include "common/str.h"
#include "common/system.h"
#include "graphics/surface.h"

#include "ack/ack.h"
#include "ack/graphics.h"
#include "ack/graphics_renderer.h"

namespace Ack {

/**
 * Loads and displays a BMP skin for the menu
 * 
 * Original Pascal:
 * procedure menuskinbmp;
 * var bmpf:file;
 *     i,i2:integer;
 *     b,t:byte;
 *     line:array[0..319] of byte;
 *     header:array[1..54] of byte;
 *     name:string;
 * begin
 *     name:=systemdir+'\ACKDATA0.DAT';
 *     assign(bmpf,name);
 *     {$I-} reset(bmpf,1); {$I+}
 *     if ioresult=0 then
 *      begin
 *       BlockRead(bmpf,header,SizeOf(header));
 *       i2:=header[51];
 *       if i2=0 then i2:=256;
 *       seek(bmpf,54+(i2*4));
 *
 *       for i:=0 to 32 do
 *         begin
 *          blockread(bmpf,line[0],320);
 *          move(line[0],mem[scrnl:scrnh[i]],320);
 *        end;
 *       close(bmpf);
 *      end; {if ioresult=0}
 * end;
 */
void GraphicsRenderer::menuSkinBmp(const Common::String &systemDir) {
    Common::String name = systemDir + "/ACKDATA0.DAT";
    Common::File bmpFile;
    
    if (!bmpFile.open(name)) {
        warning("Could not open menu skin file: %s", name.c_str());
        return;
    }
    
    // Read BMP header (54 bytes)
    byte header[54];
    if (bmpFile.read(header, sizeof(header)) != sizeof(header)) {
        warning("Failed to read BMP header from %s", name.c_str());
        bmpFile.close();
        return;
    }
    
    // Get color count from header
    int colorCount = header[50]; // Using index 50 instead of 51 (0-based vs 1-based)
    if (colorCount == 0)
        colorCount = 256;
    
    // Skip palette data
    bmpFile.seek(54 + (colorCount * 4));
    
    // Read bitmap data line by line
    byte line[320];
    Graphics::Surface *surface = _vm->getGraphics()->getSurface();
    
    for (int i = 0; i < 33; i++) { // 0 to 32 inclusive
        if (bmpFile.read(line, 320) != 320) {
            warning("Failed to read bitmap data line %d from %s", i, name.c_str());
            break;
        }
        
        // Copy line to screen surface
        // In the original: move(line[0],mem[scrnl:scrnh[i]],320);
        byte *dst = (byte *)surface->getBasePtr(0, i);
        memcpy(dst, line, 320);
    }
    
    bmpFile.close();
    
    // Update screen
    _vm->getGraphics()->updateScreen();
}

} // End of namespace Ack
