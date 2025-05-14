/*
 *  Copyright (C) 2008-2022  The Exult Team
 *
 *  Based on code by Dancer A.L Vesperman
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include "U7fileman.h"
#include "../../exult_file_adapter.h" // ScummVM ExultFileAdapter

#include "Flat.h"
#include "Flex.h"
#include "IFF.h"
#include "Table.h"
#include "U7file.h"

// ScummVM includes for debug/warning and streams
#include "common/debug.h"
#include "common/stream.h"

using std::make_unique;
// using std::string; // Not directly used in this file after modifications

// Initialize static members
// ExultEngine will be responsible for creating/managing the U7FileManager instance
// and setting both 'self' and '_scummvm_file_adapter'.
U7FileManager* U7FileManager::self = nullptr;
ScummVM::Exult::ExultFileAdapter* U7FileManager::_scummvm_file_adapter = nullptr;

// Constructor - public as defined in .h
// ExultEngine should create an instance of U7FileManager.
U7FileManager::U7FileManager() {
    // If this instance is intended to be the global 'self', it should set it.
    // However, to allow ExultEngine to control the lifetime, ExultEngine will set U7FileManager::self.
    // ScummVM::debug(1, "U7FileManager instance created.");
}

void U7FileManager::set_scummvm_file_adapter(ScummVM::Exult::ExultFileAdapter* adapter) {
    _scummvm_file_adapter = adapter;
    if (_scummvm_file_adapter) {
        ScummVM::debug(1, "U7FileManager: ScummVM File Adapter has been set.");
    } else {
        ScummVM::warning("U7FileManager: ScummVM File Adapter has been set to null.");
    }
}

/**
 *  Searches for the desired "file". If it is not already open,
 *  it is opened by the creation of an appropriate reader class.
 *  @param s    Unique identifier.
 *  @param allow_errors If false, this function will throw an
 *  exception if the file cannot be found. If true, it will return
 *  a null file pointer instead.
 *  @return Pointer to data reading class.
 */
U7file* U7FileManager::get_file_object(const File_spec& s, bool allow_errors) {
	if (file_list.count(s) != 0) {
		return file_list[s].get();
	}

	std::unique_ptr<U7file> uf;

	if (_scummvm_file_adapter) {
		ScummVM::debug(2, "U7FileManager: Attempting to open '%s' via ScummVM VFS.", s.name.c_str());
		ScummVM::Common::SeekableReadStream* stream = _scummvm_file_adapter->openFileRead(s.name.c_str());

		if (stream && stream->isOpen()) {
			ScummVM::debug(1, "U7FileManager: ScummVM stream opened for '%s'. TODO: Integrate U7file subclasses to use this stream.", s.name.c_str());
			// !!! CRITICAL TODO: Refactor U7file types (IFFFile, FlexFile, IFFBuffer, etc.)
			// to be constructible from a ScummVM::Common::SeekableReadStream.
			// This is the core of VFS integration for Exult_Engine_s file types.
			// For this incremental step, 'uf' is NOT created from 'stream' yet.
			// The U7file object, once created, should take ownership of the stream.
			delete stream; // TEMPORARY: Delete stream as it's not used by U7file types yet.
			stream = nullptr;
			// Fall through to old logic for now, with a warning. This is NOT the final state.
			ScummVM::warning("U7FileManager: Stream for '%s' obtained via ScummVM VFS, but U7file types not yet adapted. Falling back to old logic for this attempt.", s.name.c_str());
		} else {
			if (stream) delete stream; // Clean up if created but not open
			ScummVM::debug(1, "U7FileManager: Failed to open '%s' via ScummVM VFS or stream not usable. Falling back to old logic.", s.name.c_str());
		}
	}

	// Original Exult file opening logic (used if adapter not set, or if VFS path above doesn't yet populate 'uf')
	// This entire block will be replaced once U7file types are adapted for ScummVM streams.
	if (!uf) { // Only if 'uf' wasn't (or couldn't be) created via ScummVM path yet
		if (!_scummvm_file_adapter) {
			ScummVM::debug(2, "U7FileManager: ScummVM File Adapter not set. Using original Exult file logic for '%s'.", s.name.c_str());
		}

		if (s.index >= 0) { // This implies an archive or multi-part file, IExultDataSource handles this.
							// IExultDataSource itself uses direct file operations.
			auto data = make_unique<IExultDataSource>(s.name, s.index);
			if (data->good()) {
				if (IFF::is_iff(data.get())) {
					uf = std::make_unique<IFFBuffer>(s, std::move(data));
				} else if (Flex::is_flex(data.get())) {
					uf = std::make_unique<FlexBuffer>(s, std::move(data));
				} else if (Table::is_table(data.get())) {
					uf = std::make_unique<TableBuffer>(s, std::move(data));
				} else if (Flat::is_flat(data.get())) {
					uf = std::make_unique<FlatBuffer>(s, std::move(data));
				}
			}
		} else { // This implies a standalone file.
				 // The U7file subclasses (IFFFile, etc.) use direct file operations.
			if (IFF::is_iff(s.name)) {
				uf = std::make_unique<IFFFile>(s.name);
			} else if (Flex::is_flex(s.name)) {
				uf = std::make_unique<FlexFile>(s.name);
			} else if (Table::is_table(s.name)) {
				uf = std::make_unique<TableFile>(s.name);
			} else if (Flat::is_flat(s.name)) {
				uf = std::make_unique<FlatFile>(s.name);
			}
		}
	}

	if (uf == nullptr) {
		if (!allow_errors) {
			ScummVM::error("U7FileManager: Failed to get file object for '%s' and errors are not allowed.", s.name.c_str());
			throw file_open_exception(s.name);
		}
		ScummVM::debug(1, "U7FileManager: Failed to get file object for '%s', returning nullptr (errors allowed).", s.name.c_str());
		return nullptr;
	}

	U7file* pt = uf.get();
	file_list[s] = std::move(uf);
	ScummVM::debug(2, "U7FileManager: Successfully obtained U7file for '%s' (possibly via old logic).", s.name.c_str());
	return pt;
}

/**
 *  Cleans the whole file list.
 */
void U7FileManager::reset() {
	ScummVM::debug(1, "U7FileManager: reset() called, clearing file list.");
	file_list.clear();
}

