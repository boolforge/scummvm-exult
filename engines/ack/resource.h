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

#ifndef ACK_RESOURCE_H
#define ACK_RESOURCE_H

#include "common/scummsys.h"
#include "common/str.h"
#include "common/array.h"

namespace Ack {

class AckEngine;

/**
 * Resource manager for ACK engine
 * Handles loading and management of game resources
 */
class ResourceManager {
private:
    AckEngine *_vm;
    
    // Resource directories
    Common::String _systemDir;
    Common::String _advName;
    Common::String _masterFile;
    
public:
    ResourceManager(AckEngine *vm);
    ~ResourceManager();
    
    // Resource loading
    bool loadResource(int16 resourceType, int16 resourceNr);
    void unloadResource(int16 resourceType, int16 resourceNr);
    
    // Configuration loading
    bool loadConfig();
    
    // Directory management
    void setSystemDir(const Common::String &dir) { _systemDir = dir; }
    Common::String getSystemDir() const { return _systemDir; }
    
    void setAdvName(const Common::String &name) { _advName = name; }
    Common::String getAdvName() const { return _advName; }
    
    void setMasterFile(const Common::String &file) { _masterFile = file; }
    Common::String getMasterFile() const { return _masterFile; }
};

} // End of namespace Ack

#endif // ACK_RESOURCE_H
