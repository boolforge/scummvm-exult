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

#include "IFF.h"

// #include "databuf.h" // Replaced by ScummVM streams and VFS
#include "exceptions.h"
#include "common/debug.h" // For ScummVM_DEBUG_INFO, etc.
#include "common/stream.h"  // For ScummVM::Common::SeekableReadStream

// using std::cout; // No direct cout, use ScummVM::debug
// using std::endl;
// using std::memcmp; // Use strncmp for char arrays or compare byte-by-byte if needed
// using std::size_t; // Use size_t directly
// using std::string; // Use std::string directly

/**
 *  Reads the header from an IFF and builds an object index.
 *  MODIFIED: Uses _scummvmStream (member of U7file base class).
 */
void IFF::index_file() {
	if (!_scummvmStream || !_scummvmStream->isOpen()) {
		ScummVM::debug(1, "IFF::index_file - Stream not available or not open for %s", identifier.name.c_str());
		// _formHeader should have been read or attempted in IFF constructor.
		// If it failed, _scummvmStream might be reset.
		// If _formHeader was read successfully, but we are here with no stream, it_s an issue.
		if (_formHeader.isValid() && !_scummvmStream) { // Header seemed okay but stream is gone
		    throw file_read_exception(identifier.name.c_str());
        }
        // If _formHeader is invalid, or stream was bad from start, object_list remains empty.
		return;
	}

	// _formHeader should have been read in the IFF constructor.
	// If _formHeader.read failed or was invalid, _scummvmStream would have been reset,
	// and we would have returned above. So, we can assume _formHeader is valid.

	ScummVM::debug(2, "IFF::index_file - FORM Type: %.4s, Size: %u for %s", _formHeader.formType, (unsigned int)_formHeader.chunkSize, identifier.name.c_str());

	// The stream should be positioned after the FORM header (chunkID, chunkSize, formType - 12 bytes total).
	// _formHeader.read() in the constructor should have left it here.
	_scummvmStream->seek(12, SEEK_SET);
	if (_scummvmStream->readError() || _scummvmStream->pos() != 12) {
		ScummVM::error("IFF::index_file - Failed to seek to start of chunks for %s", identifier.name.c_str());
		object_list.clear();
		_scummvmStream.reset(); // Invalidate stream
		throw file_read_exception(identifier.name.c_str());
	}

	// Iterate through the chunks within the FORM
	// The total size of data within FORM is _formHeader.chunkSize (excluding FORM ID and its own size field, but including formType)
	// So, we read chunks until stream_pos >= 4 (for FORM ID) + 4 (for chunkSize) + _formHeader.chunkSize
	// Or, more simply, until stream_pos >= 8 + _formHeader.chunkSize (from start of file)
	// Or, since we are at pos 12, read until current_pos_relative_to_chunk_data_start >= _formHeader.chunkSize - 4 (formType size)
	long form_data_end_pos = 8 + _formHeader.chunkSize; 

	while (_scummvmStream->isOpen() && !_scummvmStream->eof() && _scummvmStream->pos() < form_data_end_pos) {
		IFF_Generic_Chunk_Header chunk_hdr;
		long chunk_data_start_offset = _scummvmStream->pos();

		if (!chunk_hdr.read(_scummvmStream.get())) {
			if (_scummvmStream->eof()) break; // Normal end of file if no more chunks
			ScummVM::error("IFF::index_file - Failed to read chunk header for %s at pos %ld", identifier.name.c_str(), chunk_data_start_offset);
			object_list.clear();
			_scummvmStream.reset();
			throw file_read_exception(identifier.name.c_str());
		}

		Reference r;
		r.offset = _scummvmStream->pos(); // Data starts immediately after chunk header
		r.size   = chunk_hdr.chunkSize;

		ScummVM::debug(3, "IFF::index_file - Found chunk '%.4s', size %u, offset %u for %s", 
		             chunk_hdr.chunkID, (unsigned int)r.size, (unsigned int)r.offset, identifier.name.c_str());

		if (r.size == 0 && strncmp(chunk_hdr.chunkID, "    ", 4) == 0) {
		    ScummVM::debug(2, "IFF::index_file - Encountered zero size null chunk, likely padding or end for %s", identifier.name.c_str());
		    break; // Often signifies end or padding
		}
		object_list.push_back(r);

		// Advance stream position to the start of the next chunk.
		// IFF chunks are padded to an even number of bytes.
		long next_chunk_pos = r.offset + r.size;
		if (r.size % 2 != 0) { // Odd size, add padding byte
			next_chunk_pos++;
		}
		_scummvmStream->seek(next_chunk_pos, SEEK_SET);

		if (_scummvmStream->readError()) {
		    ScummVM::error("IFF::index_file - Stream error after processing chunk '%.4s' for %s", chunk_hdr.chunkID, identifier.name.c_str());
		    object_list.clear();
		    _scummvmStream.reset();
		    throw file_read_exception(identifier.name.c_str());
        }
	}
	ScummVM::debug(2, "IFF::index_file - Indexed %zu objects for %s", object_list.size(), identifier.name.c_str());
}

// IFF::is_iff(IDataSource* in) and IFF::is_iff(const std::string& fname) were removed.
// The static IFF::is_iff(ScummVM::Common::SeekableReadStream* stream) is now in IFF.h.


