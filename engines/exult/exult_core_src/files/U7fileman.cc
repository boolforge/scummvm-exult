/*
 *  Copyright (C) 2008-2022  The Exult Team
 */

#include "U7fileman.h"
#include "common/debug.h"
#include "common/ReadStream.h"
#include "files/Flat.h"
#include "files/Flex.h"
#include "files/IFF.h"
#include "files/Table.h"
#include "files/U7file.h"
#include "files/U7file_types.h"
#include "files/IExultDataSource.h"
#include "file_adapter.h"

#include <map>
#include <memory>
#include <string>

namespace ExultCore {

namespace {

// Map to store opened U7file objects
std::map<File_spec, std::unique_ptr<U7file>> file_list;

// ScummVM file adapter instance
ScummVM::FileAdapter* _scummvm_file_adapter = nullptr;

} // namespace

void U7FileManager::setScummVMFileAdapter(ScummVM::FileAdapter* adapter) {
	_scummvm_file_adapter = adapter;
}

U7file* U7FileManager::get_file(const File_spec& s, bool error_if_not_found) {
	// Check if the file is already open
	auto it = file_list.find(s);
	if (it != file_list.end()) {
		return it->second.get();
	}

	std::unique_ptr<U7file> uf = nullptr;

	// Try to open via ScummVM VFS if adapter is set
	if (_scummvm_file_adapter) {
		ScummVM::Common::SeekableReadStream* stream = _scummvm_file_adapter->openFile(s.name.c_str());
		if (stream && stream->isOpen()) {
			// The ScummVM stream is now opened. We need to determine the file type
			// and instantiate the correct U7file subclass.
			// The U7file object will take ownership of the stream via std::unique_ptr.
			std::unique_ptr<ScummVM::Common::SeekableReadStream> stream_owner(stream); // Take ownership

			// Type sniffing:
			// Order can be important. IFF and Flex have strong magic numbers.
			// Table is more heuristic. Flat is often a fallback.
			// Ensure stream position is reset before passing to constructors.

			if (IFF::is_iff(stream_owner.get())) {
				ScummVM::debug(1, "U7FileManager: Identified ", s.name.c_str(), " as IFF.");
				stream_owner->seek(0, SEEK_SET); // Reset for constructor
				uf = std::make_unique<IFF>(s, std::move(stream_owner));
			} else if (Flex_header::is_flex(stream_owner.get())) { // is_flex is in Flex_header
				ScummVM::debug(1, "U7FileManager: Identified ", s.name.c_str(), " as Flex.");
				stream_owner->seek(0, SEEK_SET); // Reset for constructor
				uf = std::make_unique<Flex>(s, std::move(stream_owner));
			} else if (Table::is_table(stream_owner.get())) {
				ScummVM::debug(1, "U7FileManager: Identified ", s.name.c_str(), " as Table.");
				stream_owner->seek(0, SEEK_SET); // Reset for constructor
				uf = std::make_unique<Table>(s, std::move(stream_owner));
			} else {
				// Fallback to Flat if no other type identified, or if specific logic determines it's Flat.
				// Flat files don't have a common magic number, often identified by name or context.
				// For now, assume if it's not other types, it might be Flat.
				// This could be refined (e.g. check s.name against known flat files).
				ScummVM::debug(1, "U7FileManager: Assuming ", s.name.c_str(), " is Flat (no other type identified).");
				stream_owner->seek(0, SEEK_SET); // Reset for constructor
				uf = std::make_unique<Flat>(s, std::move(stream_owner));
			}

			// Check if the U7file object was successfully created and initialized (e.g. stream not reset)
			if (uf && uf->hasStream()) { // hasStream() checks if _scummvmStream is valid
				ScummVM::debug(1, "U7FileManager: Successfully created U7file object for ", s.name.c_str(), " with ScummVM stream.");
			} else {
				ScummVM::warning("U7FileManager: Failed to create U7file object or it invalidated the stream for ", s.name.c_str(), ".");
				// uf might be null or !uf->hasStream(). If stream_owner still holds the stream (e.g. make_unique failed or uf constructor reset its own ptr),
				// it will be deleted when stream_owner goes out of scope. If it was moved to uf, and uf failed init and reset its own ptr,
				// then uf's destructor handles its unique_ptr.
				if (uf) uf.reset(); // Ensure uf is null if creation failed to pass to old logic
				// If stream_owner was not moved, it will delete the stream here.
				// If it was moved, uf is responsible or it's already null.
			}

		} else { // stream == nullptr or !stream->isOpen()
			ScummVM::debug(1, "U7FileManager: Failed to open ", s.name.c_str(), " via ScummVM VFS or stream not usable.");
		}
	}

	// If 'uf' was not successfully created with a ScummVM stream, then 'uf' will be nullptr here.
	// The original Exult file opening logic is removed as we now exclusively use ScummVM VFS.
	// If _scummvm_file_adapter is not set, or if stream opening/typing fails, 'uf' remains null.

	if (!uf) { // This means ScummVM VFS path failed or adapter not set
		if (!_scummvm_file_adapter) {
			ScummVM::warning("U7FileManager: ScummVM File Adapter not set. Cannot open ", s.name.c_str(), ".");
		} else {
			// This path means adapter was set, but stream opening or U7file creation failed.
			// Specific error/warning already logged above.
		}
		// Original logic for s.index and direct file opening is removed.
		// All file access must now go through ScummVM VFS.
		// If we reach here, it means the file could not be processed via ScummVM VFS.
	}

	if (uf == nullptr) { // Check if uf is still null after all attempts
		if (error_if_not_found) {
			ScummVM::error("U7FileManager: Failed to get file object for ", s.name.c_str(), " (ScummVM VFS path). Not found or type unknown/error.");
		}
		return nullptr;
	}

	// Successfully created a U7file object (uf) using ScummVM stream.
	file_list[s] = std::move(uf); // Store the new U7file object in the map
	ScummVM::debug(1, "U7FileManager: Successfully obtained and stored U7file for ", s.name.c_str(), " via ScummVM VFS.");

	return file_list[s].get();
}

void U7FileManager::close_file(const File_spec& s) {
	file_list.erase(s);
}

void U7FileManager::close_all() {
	file_list.clear();
}

} // namespace ExultCore

