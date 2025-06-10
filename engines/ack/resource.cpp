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
#include "common/system.h"
#include "common/textconsole.h"
#include "common/debug.h"

#include "ack/ack.h"
#include "ack/resource.h"
#include "ack/structs.h"

namespace Ack {

ResourceManager::ResourceManager(AckEngine *vm) : _vm(vm) {
    _masterFile = "MASTER.DAT";
}

ResourceManager::~ResourceManager() {
}

/**
 * Load a resource
 * 
 * @param resourceType The type of resource to load (RESOURCETYPE_LOGIC, RESOURCETYPE_SOUND, etc.)
 * @param resourceNr The resource number to load
 * @return True if the resource was loaded successfully, false otherwise
 */
bool ResourceManager::loadResource(int16 resourceType, int16 resourceNr) {
    if (resourceNr >= MAX_DIRECTORY_ENTRIES) {
        warning("Resource number out of range: %d", resourceNr);
        return false;
    }
    
    // Check if the resource is already loaded
    AckDir *dir = nullptr;
    switch (resourceType) {
    case RESOURCETYPE_LOGIC:
        dir = &_vm->_dirLogic[resourceNr];
        if (dir->flags & RES_LOADED)
            return true;
        break;
        
    case RESOURCETYPE_SOUND:
        dir = &_vm->_dirSound[resourceNr];
        if (dir->flags & RES_LOADED)
            return true;
        break;
        
    case RESOURCETYPE_VIEW:
        dir = &_vm->_dirView[resourceNr];
        if (dir->flags & RES_LOADED)
            return true;
        break;
        
    case RESOURCETYPE_PICTURE:
        dir = &_vm->_dirPic[resourceNr];
        if (dir->flags & RES_LOADED)
            return true;
        break;
        
    default:
        warning("Unknown resource type: %d", resourceType);
        return false;
    }
    
    // Resource not loaded, load it now
    if (dir->volume == 0xff) {
        warning("Resource not found in directory: type %d, number %d", resourceType, resourceNr);
        return false;
    }
    
    // Construct the resource filename
    Common::String filename;
    switch (resourceType) {
    case RESOURCETYPE_LOGIC:
        filename = Common::String::format("%s/LOGIC%d.DAT", _advName.c_str(), resourceNr);
        break;
        
    case RESOURCETYPE_SOUND:
        filename = Common::String::format("%s/SOUND%d.DAT", _advName.c_str(), resourceNr);
        break;
        
    case RESOURCETYPE_VIEW:
        filename = Common::String::format("%s/VIEW%d.DAT", _advName.c_str(), resourceNr);
        break;
        
    case RESOURCETYPE_PICTURE:
        filename = Common::String::format("%s/PIC%d.DAT", _advName.c_str(), resourceNr);
        break;
    }
    
    // Open the resource file
    Common::File file;
    if (!file.open(filename)) {
        warning("Could not open resource file: %s", filename.c_str());
        return false;
    }
    
    // Seek to the resource data
    file.seek(dir->offset);
    
    // Read the resource data
    byte *data = nullptr;
    uint32 size = dir->len;
    
    if (dir->flags & RES_COMPRESSED) {
        // Resource is compressed, decompress it
        data = (byte *)malloc(size);
        if (!data) {
            warning("Could not allocate memory for resource data");
            file.close();
            return false;
        }
        
        // Read compressed data
        byte *compData = (byte *)malloc(dir->clen);
        if (!compData) {
            warning("Could not allocate memory for compressed resource data");
            free(data);
            file.close();
            return false;
        }
        
        if (file.read(compData, dir->clen) != dir->clen) {
            warning("Could not read compressed resource data from file: %s", filename.c_str());
            free(compData);
            free(data);
            file.close();
            return false;
        }
        
        // Decompress the data
        // This would use the decompression algorithm
        // For now, we'll just use a placeholder
        // decompressData(compData, dir->clen, data, size);
        
        free(compData);
    } else {
        // Resource is not compressed, read it directly
        data = (byte *)malloc(size);
        if (!data) {
            warning("Could not allocate memory for resource data");
            file.close();
            return false;
        }
        
        if (file.read(data, size) != size) {
            warning("Could not read resource data from file: %s", filename.c_str());
            free(data);
            file.close();
            return false;
        }
    }
    
    file.close();
    
    // Store the resource data
    switch (resourceType) {
    case RESOURCETYPE_LOGIC:
        _vm->_logics[resourceNr].data = data;
        _vm->_logics[resourceNr].size = size;
        _vm->_logics[resourceNr].sIP = 0;
        _vm->_logics[resourceNr].cIP = 0;
        break;
        
    case RESOURCETYPE_SOUND:
        // This would use the sound system
        // For now, we'll just use a placeholder
        // _vm->_sounds[resourceNr] = createSoundFromData(data, size);
        free(data); // Free the data since we're not using it yet
        break;
        
    case RESOURCETYPE_VIEW:
        _vm->_views[resourceNr].rdata = data;
        break;
        
    case RESOURCETYPE_PICTURE:
        _vm->_pictures[resourceNr].rdata = data;
        break;
    }
    
    // Mark the resource as loaded
    dir->flags |= RES_LOADED;
    
    return true;
}

/**
 * Unload a resource
 * 
 * @param resourceType The type of resource to unload (RESOURCETYPE_LOGIC, RESOURCETYPE_SOUND, etc.)
 * @param resourceNr The resource number to unload
 */
void ResourceManager::unloadResource(int16 resourceType, int16 resourceNr) {
    if (resourceNr >= MAX_DIRECTORY_ENTRIES) {
        warning("Resource number out of range: %d", resourceNr);
        return;
    }
    
    // Check if the resource is loaded
    AckDir *dir = nullptr;
    switch (resourceType) {
    case RESOURCETYPE_LOGIC:
        dir = &_vm->_dirLogic[resourceNr];
        if (!(dir->flags & RES_LOADED))
            return;
            
        // Free the resource data
        free(_vm->_logics[resourceNr].data);
        _vm->_logics[resourceNr].data = nullptr;
        _vm->_logics[resourceNr].size = 0;
        _vm->_logics[resourceNr].sIP = 0;
        _vm->_logics[resourceNr].cIP = 0;
        break;
        
    case RESOURCETYPE_SOUND:
        dir = &_vm->_dirSound[resourceNr];
        if (!(dir->flags & RES_LOADED))
            return;
            
        // Free the resource data
        // This would use the sound system
        // For now, we'll just use a placeholder
        // delete _vm->_sounds[resourceNr];
        // _vm->_sounds[resourceNr] = nullptr;
        break;
        
    case RESOURCETYPE_VIEW:
        dir = &_vm->_dirView[resourceNr];
        if (!(dir->flags & RES_LOADED))
            return;
            
        // Free the resource data
        free(_vm->_views[resourceNr].rdata);
        _vm->_views[resourceNr].rdata = nullptr;
        break;
        
    case RESOURCETYPE_PICTURE:
        dir = &_vm->_dirPic[resourceNr];
        if (!(dir->flags & RES_LOADED))
            return;
            
        // Free the resource data
        free(_vm->_pictures[resourceNr].rdata);
        _vm->_pictures[resourceNr].rdata = nullptr;
        break;
        
    default:
        warning("Unknown resource type: %d", resourceType);
        return;
    }
    
    // Mark the resource as unloaded
    dir->flags &= ~RES_LOADED;
}

/**
 * Load configuration data
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
 * 
 * @return True if the configuration was loaded successfully, false otherwise
 */
bool ResourceManager::loadConfig() {
    Common::String configFile = _advName + _masterFile;
    Common::File file;
    
    if (!file.open(configFile)) {
        warning("Could not open config file: %s", configFile.c_str());
        _advName = "NONAME";
        return false;
    }
    
    // Read the master record
    MasterRec ack;
    if (file.read(&ack, sizeof(ack)) != sizeof(ack)) {
        warning("Failed to read master record from %s", configFile.c_str());
        file.close();
        return false;
    }
    
    file.close();
    
    // Set the master record in the engine
    _vm->setMasterRec(ack);
    
    // Load the palette
    _vm->getGraphicsRenderer()->loadBmpPalette(ack.ackVersion, _advName, _systemDir);
    
    return true;
}

} // End of namespace Ack
