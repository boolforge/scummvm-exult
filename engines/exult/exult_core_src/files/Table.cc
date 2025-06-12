/*
 *  Copyright (C) 2000-2022  The Exult Team
 *
 *  Original file by Dancer A.L Vesperman
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

#include "Table.h"

#include "exceptions.h"
#include "common/debug.h"   // For ScummVM_DEBUG_INFO, etc.
#include "common/stream.h"  // For ScummVM::Common::SeekableReadStream
#include "common/endians.h" // For ScummVM_READ_LE16, ScummVM_READ_LE32

// Removed: using std::FILE; - Not used
// Removed: using std::size_t; - Use directly
// Removed: using std::string; - Use directly

/**
 *  Reads the header from a table file and builds an object index.
 *  MODIFIED: Uses _scummvmStream (member of U7file base class).
 */
void Table::index_file() {
	if (!_scummvmStream || !_scummvmStream->isOpen()) {
		ScummVM::debug(1, "Table::index_file - Stream not available or not open for %s", identifier.name.c_str());
		// If the stream is bad, object_list will remain empty, which is handled by number_of_objects().
		// Consider if an exception should be thrown if the stream was expected to be valid.
		// For now, matching U7file.h behavior where retrieve() will fail.
		// throw file_read_exception(identifier.name.c_str()); // Or this, if constructor guarantees a good stream or throws.
		return;
	}

	// The type verification (is_table) should ideally be done by U7FileManager
	// before creating the Table object. If it must be done here, a stream-based
	// is_table static method would be needed.
	// For now, proceeding with reading, assuming the stream is a valid table file.

	_scummvmStream->seek(0, SEEK_SET); // Ensure we start reading from the beginning of the stream.
	if (_scummvmStream->readError()) {
		ScummVM::error("Table::index_file - Failed to seek to beginning of stream for %s", identifier.name.c_str());
		_scummvmStream.reset(); // Invalidate stream
		throw file_read_exception(identifier.name.c_str());
	}

	while (true) {
		Reference f;
		// Table files store size as uint16 and offset as uint32, typically little-endian.
		f.size = _scummvmStream->read_le16();

		if (_scummvmStream->readError()) {
			ScummVM::error("Table::index_file - Read error while reading object size for %s", identifier.name.c_str());
			object_list.clear();
			_scummvmStream.reset(); // Invalidate stream
			throw file_read_exception(identifier.name.c_str());
		}

		if (f.size == 0xFFFF) { // End of table marker (65535 for uint16)
			break;
		}

		f.offset = _scummvmStream->read_le32();

		if (_scummvmStream->readError()) {
			ScummVM::error("Table::index_file - Read error while reading object offset for %s", identifier.name.c_str());
			object_list.clear();
			_scummvmStream.reset(); // Invalidate stream
			throw file_read_exception(identifier.name.c_str());
		}

		// Note: In Exult, table file offsets are usually absolute from the start of the file.
		// If they were relative to some data area, adjustment would be needed here.
		// Assuming absolute offsets as per typical Exult table file structure.
		ScummVM::debug(3, "Table::index_file - Item: size %u, offset %u for %s", (unsigned int)f.size, (unsigned int)f.offset, identifier.name.c_str());
		object_list.push_back(f);
	}
	ScummVM::debug(2, "Table::index_file - Indexed %zu objects for %s", object_list.size(), identifier.name.c_str());
}

// Old static is_table methods are removed as they relied on IDataSource or direct file access.
// A new static method Table::is_table(ScummVM::Common::SeekableReadStream* stream) could be added
// in Table.h and implemented here if U7FileManager requires it for type detection before object creation.
// For example:
/*
bool Table::is_table(ScummVM::Common::SeekableReadStream* stream) {
	if (!stream || !stream->isOpen() || stream->size() < 2) return false; // Minimum for EOF marker
	long original_pos = stream->pos();
	stream->seek(0, SEEK_SET);
	bool looks_like_table = false;
	size_t items_read = 0;
	try {
		while(stream->tell() < stream->size() - 1) { // -1 to ensure read_le16 doesn't go past EOF if size is odd
			uint16 size = stream->read_le16();
			if (stream->readError()) break;
			if (size == 0xFFFF) {
				looks_like_table = true;
				break;
			}
			if (stream->tell() > stream->size() - 4) break; // Not enough for offset
			stream->read_le32(); // Read offset
			if (stream->readError()) break;
			items_read++;
			if (items_read > 10000) break; // Sanity break for very large non-table files
		}
	} catch (...) {
		// Stream errors might throw, or readError() catches them.
	}
	stream->seek(original_pos, SEEK_SET);
	return looks_like_table && !stream->readError();
}
*/

