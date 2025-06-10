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
			// The ScummVM stream is now opened. We need to determine the file type
			// and instantiate the correct U7file subclass.
			// The U7file object will take ownership of the stream via std::unique_ptr.
			std::unique_ptr<ScummVM::Common::SeekableReadStream> stream_owner(stream); // Take ownership

			// Type sniffing:
			// Order can be important. IFF and Flex have strong magic numbers.
			// Table is more heuristic. Flat is often a fallback.
			// Ensure stream position is reset before passing to constructors.

			if (IFF::is_iff(stream_owner.get())) {
				ScummVM::debug(1, "U7FileManager: Identified '%s' as IFF.", s.name.c_str());
				stream_owner->seek(0, SEEK_SET); // Reset for constructor
				uf = std::make_unique<IFF>(s, std::move(stream_owner));
			} else if (Flex_header::is_flex(stream_owner.get())) { // is_flex is in Flex_header
				ScummVM::debug(1, "U7FileManager: Identified '%s' as Flex.", s.name.c_str());
				stream_owner->seek(0, SEEK_SET); // Reset for constructor
				uf = std::make_unique<Flex>(s, std::move(stream_owner));
			} else if (Table::is_table(stream_owner.get())) {
				ScummVM::debug(1, "U7FileManager: Identified '%s' as Table.", s.name.c_str());
				stream_owner->seek(0, SEEK_SET); // Reset for constructor
				uf = std::make_unique<Table>(s, std::move(stream_owner));
			} else {
				// Fallback to Flat if no other type identified, or if specific logic determines it's Flat.
				// Flat files don't have a common magic number, often identified by name or context.
				// For now, assume if it's not other types, it might be Flat.
				// This could be refined (e.g. check s.name against known flat files).
				ScummVM::debug(1, "U7FileManager: Assuming '%s' is Flat (no other type identified).", s.name.c_str());
				stream_owner->seek(0, SEEK_SET); // Reset for constructor
				uf = std::make_unique<Flat>(s, std::move(stream_owner));
			}

			// Check if the U7file object was successfully created and initialized (e.g. stream not reset)
			if (uf && uf->hasStream()) { // hasStream() checks if _scummvmStream is valid
				ScummVM::debug(1, "U7FileManager: Successfully created U7file object for '%s' with ScummVM stream.", s.name.c_str());
			} else {
				ScummVM::warning("U7FileManager: Failed to create U7file object or it invalidated the stream for '%s'.", s.name.c_str());
				// uf might be null or !uf->hasStream(). If stream_owner still holds the stream (e.g. make_unique failed or uf constructor reset its own ptr),
				// it will be deleted when stream_owner goes out of scope. If it was moved to uf, and uf failed init and reset its own ptr,
				// then uf's destructor handles its unique_ptr.
				if (uf) uf.reset(); // Ensure uf is null if creation failed to pass to old logic
				// If stream_owner was not moved, it will delete the stream here.
				// If it was moved, uf is responsible or it's already null.
			}

		} else { // stream == nullptr or !stream->isOpen()
			if (stream) delete stream; // Clean up if created but not open
			ScummVM::debug(1, "U7FileManager: Failed to open '%s' via ScummVM VFS or stream not usable.", s.name.c_str());
		}
	}


	// If 'uf' was not successfully created with a ScummVM stream, then 'uf' will be nullptr here.
	// The original Exult file opening logic is removed as we now exclusively use ScummVM VFS.
	// If _scummvm_file_adapter is not set, or if stream opening/typing fails, 'uf' remains null.

	if (!uf) { // This means ScummVM VFS path failed or adapter not set
		if (!_scummvm_file_adapter) {
			ScummVM::warning("U7FileManager: ScummVM File Adapter not set. Cannot open '%s'.", s.name.c_str());
		} else {
			// This path means adapter was set, but stream opening or U7file creation failed.
			// Specific error/warning already logged above.
		}
		// Original logic for s.index and direct file opening is removed.
		// All file access must now go through ScummVM VFS.
		// If we reach here, it means the file could not be processed via ScummVM VFS.
	}


	if (uf == nullptr) { // Check if uf is still null after all attempts
		if (!allow_errors) {
			ScummVM::error("U7FileManager: Failed to get file object for '%s' (ScummVM VFS path). Not found or type unknown/error.", s.name.c_str());
			throw file_open_exception(s.name);
		}
		ScummVM::debug(1, "U7FileManager: Failed to get file object for '%s', returning nullptr (errors allowed).", s.name.c_str());
		return nullptr;
	}

	// Successfully created a U7file object (uf) using ScummVM stream.
	U7file* pt = uf.get();
	file_list[s] = std::move(uf); // Store the new U7file object in the map
	ScummVM::debug(1, "U7FileManager: Successfully obtained and stored U7file for '%s' via ScummVM VFS.", s.name.c_str());
	return pt;
}

/**
 *  Cleans the whole file list.
 */
void U7FileManager::reset() {
	ScummVM::debug(1, "U7FileManager: reset() called, clearing file list.");
	file_list.clear();
}

