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
#include "ack/structs.h"

namespace Ack {

GraphicsRenderer::GraphicsRenderer(AckEngine *vm) : _vm(vm) {
}

GraphicsRenderer::~GraphicsRenderer() {
}

/**
 * Clears the keyboard buffer
 * 
 * Original Pascal:
 * procedure clear_keyboard_buffer;
 *  var regs:registers;
 * begin
 *  regs.ah:=$0c;regs.al:=$00;
 *  intr($21,regs);
 * end;
 */
void GraphicsRenderer::clearKeyboardBuffer() {
    // In ScummVM, we don't need to directly access the keyboard buffer
    // We can rely on the event system to handle keyboard input
    // This is a no-op in the ScummVM implementation
}

/**
 * Converts a version number to a string
 * 
 * Original Pascal:
 * function version(v:byte):ittystring;
 * var s:ittystring;
 * begin
 *  s:='V'+strnum(v DIV 10)+'.'+strnum(v MOD 10);
 *  version:=s;
 * end;
 */
Common::String GraphicsRenderer::versionToString(byte v) {
    // Convert version number to string in format "V1.0"
    Common::String s = Common::String::format("V%d.%d", v / 10, v % 10);
    return s;
}

/**
 * Loads icons from a file
 * 
 * Original Pascal:
 * procedure loadicons(fn:string);
 * var
 *  ifile:file of grap256unit;
 *  i:integer;
 * begin
 *  assign(ifile,systemdir+fn);
 *  {$I-} reset(ifile); {$I+}
 *  if ioresult<>0 then exit;
 *  for i:=1 to 31 do
 *   if not eof(ifile) then read(ifile,icons^[i]);
 *  close(ifile);
 * end;
 */
void GraphicsRenderer::loadIcons(const Common::String &filename) {
    Common::String fullPath = _vm->getSystemDir() + filename;
    Common::File iconFile;
    
    if (!iconFile.open(fullPath)) {
        warning("Could not open icon file: %s", fullPath.c_str());
        return;
    }
    
    // Load icons into the icon array
    // We'll need to adapt this based on how icons are stored in the ScummVM implementation
    GrapArray256 *icons = _vm->getIcons();
    
    for (int i = 1; i <= 31; i++) {
        if (iconFile.eos())
            break;
            
        Grap256Unit icon;
        if (iconFile.read(icon.data, sizeof(icon.data)) != sizeof(icon.data)) {
            warning("Failed to read icon %d from %s", i, fullPath.c_str());
            break;
        }
        
        if (i < icons->size())
            (*icons)[i] = icon;
        else
            icons->push_back(icon);
    }
    
    iconFile.close();
}

/**
 * Saves icons to a file
 * 
 * Original Pascal:
 * procedure saveicons(fn:string);
 * var
 *  ifile:file of grap256unit;
 *  i:integer;
 * begin
 *  loadicons(fn);
 *  assign(ifile,systemdir+fn);
 *  {$I-} rewrite(ifile); {$I+}
 *  if ioresult<>0 then exit;
 *  for i:=1 to 31 do
 *   write(ifile,icons^[i]);
 *  close(ifile);
 * end;
 */
void GraphicsRenderer::saveIcons(const Common::String &filename) {
    // First load existing icons
    loadIcons(filename);
    
    Common::String fullPath = _vm->getSystemDir() + filename;
    Common::OutSaveFile *iconFile = g_system->getSavefileManager()->openForSaving(fullPath);
    
    if (!iconFile) {
        warning("Could not open icon file for writing: %s", fullPath.c_str());
        return;
    }
    
    // Save icons from the icon array
    GrapArray256 *icons = _vm->getIcons();
    
    for (int i = 1; i <= 31 && i < icons->size(); i++) {
        iconFile->write((*icons)[i].data, sizeof((*icons)[i].data));
    }
    
    iconFile->finalize();
    delete iconFile;
}

/**
 * Displays an icon on the screen
 * 
 * Original Pascal:
 * procedure puticon(xb,yy,bb:integer);
 * var i:integer;
 * begin
 *  if ((mouseon) and (mouseactive)) then hidemouse;
 *  for i:=1 to 16 do
 *   move(icons^[bb,i],mem[scrnl:(xb*4)+scrnh[yy+i]],16);
 * end;
 */
void GraphicsRenderer::putIcon(int x, int y, int iconIndex) {
    // Hide mouse cursor if needed
    if (_vm->isMouseActive()) {
        _vm->getGraphics()->setMouseCursor(false);
    }
    
    // Get the icon data
    GrapArray256 *icons = _vm->getIcons();
    if (iconIndex >= icons->size()) {
        warning("Invalid icon index: %d", iconIndex);
        return;
    }
    
    // Get the surface to draw on
    Graphics::Surface *surface = _vm->getGraphics()->getSurface();
    
    // Draw the icon (16x16 pixels)
    for (int i = 1; i <= 16; i++) {
        byte *dst = (byte *)surface->getBasePtr(x * 4, y + i);
        // In the original: move(icons^[bb,i],mem[scrnl:(xb*4)+scrnh[yy+i]],16);
        // We need to copy 16 bytes from the icon data to the screen
        memcpy(dst, &(*icons)[iconIndex].data[i], 16);
    }
    
    // Update screen
    _vm->getGraphics()->updateScreen();
    
    // Restore mouse cursor if needed
    if (_vm->isMouseActive()) {
        _vm->getGraphics()->setMouseCursor(true);
    }
}

/**
 * Loads configuration data
 * 
 * Original Pascal:
 * procedure loadconfig;
 * var ackf:file of masterrec;
 *     palf:file of paletterec;
 *     paltemp:paletterec;
 *     i:byte;
 * begin
 *  assign(ackf,ADVNAME+MASTERFILE);
 *  {$I-} reset(ackf); {$I+}
 *  if ioresult<>0 then begin;ADVNAME:='NONAME';chdir(systemdir);exit;end;
 *  read(ackf,ack);
 *  close(ackf);
 *  loadbmppalette(ack.ackversion,ADVNAME,systemdir);
 * end;
 */
void GraphicsRenderer::loadConfig() {
    Common::String configFile = _vm->getAdvName() + _vm->getMasterFile();
    Common::File file;
    
    if (!file.open(configFile)) {
        warning("Could not open config file: %s", configFile.c_str());
        _vm->setAdvName("NONAME");
        // In ScummVM, we don't need to change directory
        return;
    }
    
    // Read the master record
    MasterRec ack;
    if (file.read(&ack, sizeof(ack)) != sizeof(ack)) {
        warning("Failed to read master record from %s", configFile.c_str());
        file.close();
        return;
    }
    
    file.close();
    
    // Set the master record in the engine
    _vm->setMasterRec(ack);
    
    // Load the palette
    loadBmpPalette(ack.ackVersion, _vm->getAdvName(), _vm->getSystemDir());
}

/**
 * Loads a BMP palette
 * 
 * This is a placeholder for the loadbmppalette function called in loadconfig
 */
void GraphicsRenderer::loadBmpPalette(uint16 version, const Common::String &advName, const Common::String &systemDir) {
    // This is a placeholder for the loadbmppalette function
    // The actual implementation would depend on the palette format and how it's stored
    
    // Try to load palette from adventure-specific file first
    Common::String palFile = advName + ".PAL";
    Common::File file;
    
    if (file.open(palFile)) {
        // Read palette data
        PaletteRec palette[256];
        for (int i = 0; i < 256; i++) {
            if (file.eos())
                break;
                
            if (file.read(&palette[i], sizeof(PaletteRec)) != sizeof(PaletteRec)) {
                warning("Failed to read palette entry %d from %s", i, palFile.c_str());
                break;
            }
            
            // Set palette entry
            _vm->getGraphics()->setPaletteEntry(i, palette[i].r, palette[i].g, palette[i].b);
        }
        
        file.close();
    } else {
        // Fall back to default palette based on version
        // This would be implemented based on the original palette generation code
        warning("Could not load palette file: %s, using default palette", palFile.c_str());
        
        // Set up a basic grayscale palette as fallback
        for (int i = 0; i < 256; i++) {
            _vm->getGraphics()->setPaletteEntry(i, i, i, i);
        }
    }
}

} // End of namespace Ack
