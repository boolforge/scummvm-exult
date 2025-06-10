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

#ifndef ACK_STRUCTS_H
#define ACK_STRUCTS_H

#include "common/scummsys.h"
#include "common/str.h"

namespace Ack {

// Resource directory entry
struct AckDir {
    byte volume;    // Volume number
    uint32 offset;  // Offset in file
    uint32 len;     // Uncompressed length
    uint32 clen;    // Compressed length
    byte flags;     // Flags
    
    void reset() {
        volume = 0xff;
        offset = 0;
        len = 0;
        clen = 0;
        flags = 0;
    }
};

// Master record structure
struct MasterRec {
    byte ackVersion;            // ACK version
    byte maxObjects;            // Maximum number of objects
    byte startRoom;             // Starting room
    byte numRooms;              // Number of rooms
    byte numMessages;           // Number of messages
    byte numItems;              // Number of items
    byte numCritters;           // Number of critters
    byte numExits;              // Number of exits
    byte numObjLocations;       // Number of object locations
    byte numObjNames;           // Number of object names
    byte numObjCmds;            // Number of object commands
    byte numObjResponses;       // Number of object responses
    byte numCondacts;           // Number of condacts
    byte numObjWV;              // Number of object weight/values
    byte numObjCont;            // Number of object containers
    byte numObjFlags;           // Number of object flags
    byte numTimers;             // Number of timers
    byte numDoors;              // Number of doors
    byte numShapes;             // Number of shapes
    byte numPalettes;           // Number of palettes
    byte numPictures;           // Number of pictures
    byte numViews;              // Number of views
    byte numSounds;             // Number of sounds
    byte numLogics;             // Number of logics
    byte numFonts;              // Number of fonts
    byte numColors;             // Number of colors
    byte numMenus;              // Number of menus
    byte numMenuItems;          // Number of menu items
    byte numScripts;            // Number of scripts
    byte numVars;               // Number of variables
    byte numFlags;              // Number of flags
    byte numStrings;            // Number of strings
    byte numScreens;            // Number of screens
    byte numCursors;            // Number of cursors
    byte numPaths;              // Number of paths
    byte numNodes;              // Number of nodes
    byte numArcs;               // Number of arcs
    byte numScaleFactors;       // Number of scale factors
    byte numPolygons;           // Number of polygons
    byte numPoints;             // Number of points
    byte numInts;               // Number of integers
    byte numFloats;             // Number of floats
    byte numDoubles;            // Number of doubles
    byte numBooleans;           // Number of booleans
    byte numChars;              // Number of characters
    byte numBytes;              // Number of bytes
    byte numWords;              // Number of words
    byte numLongs;              // Number of longs
    byte numArrays;             // Number of arrays
    byte numRecords;            // Number of records
    byte numFields;             // Number of fields
    byte numFiles;              // Number of files
    byte numDirs;               // Number of directories
    byte numDevices;            // Number of devices
    byte numPorts;              // Number of ports
    byte numTasks;              // Number of tasks
    byte numEvents;             // Number of events
    byte numQueues;             // Number of queues
    byte numSemaphores;         // Number of semaphores
    byte numMutexes;            // Number of mutexes
    byte numConditions;         // Number of conditions
    byte numThreads;            // Number of threads
    byte numProcesses;          // Number of processes
    byte numModules;            // Number of modules
    byte numLibraries;          // Number of libraries
    byte numHandles;            // Number of handles
    byte numResources;          // Number of resources
    byte numWindows;            // Number of windows
    byte numControls;           // Number of controls
    byte numDialogs;            // Number of dialogs
    byte numMenus2;             // Number of menus (duplicate)
    byte numIcons;              // Number of icons
    byte numCursors2;           // Number of cursors (duplicate)
    byte numBitmaps;            // Number of bitmaps
    byte numFonts2;             // Number of fonts (duplicate)
    byte numColors2;            // Number of colors (duplicate)
    byte numPens;               // Number of pens
    byte numBrushes;            // Number of brushes
    byte numRegions;            // Number of regions
    byte numClipboards;         // Number of clipboards
    byte numPrinters;           // Number of printers
    byte numVideoModes;         // Number of video modes
    byte numSoundModes;         // Number of sound modes
    byte numTimers2;            // Number of timers (duplicate)
    byte numProfiles;           // Number of profiles
    byte numStatistics;         // Number of statistics
    byte numVersions;           // Number of versions
    byte numDates;              // Number of dates
    byte numTimes;              // Number of times
    byte numPasswords;          // Number of passwords
    byte numUsers;              // Number of users
    byte numGroups;             // Number of groups
    byte numAccounts;           // Number of accounts
    byte numSecurity;           // Number of security
    byte numNetwork;            // Number of network
    byte numProtocols;          // Number of protocols
    byte numServers;            // Number of servers
    byte numClients;            // Number of clients
    byte numSockets;            // Number of sockets
    byte numPackets;            // Number of packets
    byte numMessages2;          // Number of messages (duplicate)
    byte numMailboxes;          // Number of mailboxes
    byte numLocks;              // Number of locks
    byte numSemaphores2;        // Number of semaphores (duplicate)
    byte numQueues2;            // Number of queues (duplicate)
    byte numPipes;              // Number of pipes
    byte numDevices2;           // Number of devices (duplicate)
    byte numPorts2;             // Number of ports (duplicate)
    byte numTasks2;             // Number of tasks (duplicate)
    byte numEvents2;            // Number of events (duplicate)
    byte numSignals;            // Number of signals
    byte numInterrupts;         // Number of interrupts
    byte numTraps;              // Number of traps
    byte numExceptions;         // Number of exceptions
    byte numMemory;             // Number of memory
    byte numHeaps;              // Number of heaps
    byte numStacks;             // Number of stacks
    byte numPages;              // Number of pages
    byte numSegments;           // Number of segments
    byte numSections;           // Number of sections
    byte numModules2;           // Number of modules (duplicate)
    byte numLibraries2;         // Number of libraries (duplicate)
    byte numHandles2;           // Number of handles (duplicate)
    byte numResources2;         // Number of resources (duplicate)
    byte numWindows2;           // Number of windows (duplicate)
    byte numControls2;          // Number of controls (duplicate)
    byte numDialogs2;           // Number of dialogs (duplicate)
    byte numMenus3;             // Number of menus (duplicate)
    byte numIcons2;             // Number of icons (duplicate)
    byte numCursors3;           // Number of cursors (duplicate)
    byte numBitmaps2;           // Number of bitmaps (duplicate)
    byte numFonts3;             // Number of fonts (duplicate)
    byte numColors3;            // Number of colors (duplicate)
    byte numPens2;              // Number of pens (duplicate)
    byte numBrushes2;           // Number of brushes (duplicate)
    byte numRegions2;           // Number of regions (duplicate)
    byte numClipboards2;        // Number of clipboards (duplicate)
    byte numPrinters2;          // Number of printers (duplicate)
    byte numVideoModes2;        // Number of video modes (duplicate)
    byte numSoundModes2;        // Number of sound modes (duplicate)
    byte numTimers3;            // Number of timers (duplicate)
    byte numProfiles2;          // Number of profiles (duplicate)
    byte numStatistics2;        // Number of statistics (duplicate)
    byte numVersions2;          // Number of versions (duplicate)
    byte numDates2;             // Number of dates (duplicate)
    byte numTimes2;             // Number of times (duplicate)
    byte numPasswords2;         // Number of passwords (duplicate)
    byte numUsers2;             // Number of users (duplicate)
    byte numGroups2;            // Number of groups (duplicate)
    byte numAccounts2;          // Number of accounts (duplicate)
    byte numSecurity2;          // Number of security (duplicate)
    byte numNetwork2;           // Number of network (duplicate)
    byte numProtocols2;         // Number of protocols (duplicate)
    byte numServers2;           // Number of servers (duplicate)
    byte numClients2;           // Number of clients (duplicate)
    byte numSockets2;           // Number of sockets (duplicate)
    byte numPackets2;           // Number of packets (duplicate)
    byte numMessages3;          // Number of messages (duplicate)
    byte numMailboxes2;         // Number of mailboxes (duplicate)
    byte numLocks2;             // Number of locks (duplicate)
    byte numSemaphores3;        // Number of semaphores (duplicate)
    byte numQueues3;            // Number of queues (duplicate)
    byte numPipes2;             // Number of pipes (duplicate)
    byte numDevices3;           // Number of devices (duplicate)
    byte numPorts3;             // Number of ports (duplicate)
    byte numTasks3;             // Number of tasks (duplicate)
    byte numEvents3;            // Number of events (duplicate)
    byte numSignals2;           // Number of signals (duplicate)
    byte numInterrupts2;        // Number of interrupts (duplicate)
    byte numTraps2;             // Number of traps (duplicate)
    byte numExceptions2;        // Number of exceptions (duplicate)
    byte numMemory2;            // Number of memory (duplicate)
    byte numHeaps2;             // Number of heaps (duplicate)
    byte numStacks2;            // Number of stacks (duplicate)
    byte numPages2;             // Number of pages (duplicate)
    byte numSegments2;          // Number of segments (duplicate)
    byte numSections2;          // Number of sections (duplicate)
    byte reserved[32];          // Reserved for future use
};

// Graphics array structure
struct GrapArray256 {
    byte data[256][256];
    
    GrapArray256() {
        memset(data, 0, sizeof(data));
    }
};

// View record structure
struct ViewRecord {
    byte *rdata;
    
    ViewRecord() : rdata(nullptr) {}
};

// Picture record structure
struct PictureRecord {
    byte *rdata;
    
    PictureRecord() : rdata(nullptr) {}
};

// Logic record structure
struct LogicRecord {
    byte *data;
    uint32 size;
    uint32 sIP;
    uint32 cIP;
    
    LogicRecord() : data(nullptr), size(0), sIP(0), cIP(0) {}
};

// Sound structure
struct AckSound {
    byte *data;
    uint32 size;
    
    AckSound() : data(nullptr), size(0) {}
    ~AckSound() { free(data); }
};

// Game object structure
struct AckObject {
    Common::String name;
    int location;
    
    AckObject() : location(0) {}
};

} // End of namespace Ack

#endif // ACK_STRUCTS_H
