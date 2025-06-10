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

// #define DEBUGFLEX 0 // Controlled by ScummVM_Engine_s debug levels now

#include "Flex.h"

// #include "databuf.h" // Replaced by ScummVM streams and VFS
#include "exceptions.h"
#include "common/debug.h" // For ScummVM_DEBUG_INFO, etc.
#include "common/endians.h" // For ScummVM_READ_LE32, ScummVM_WRITE_LE32
#include "common/stream.h"  // For ScummVM::Common::SeekableReadStream and WriteStream

// Flex_header::read(ScummVM::Common::SeekableReadStream* stream) is now in Flex.h
// Flex_header::is_flex(ScummVM::Common::SeekableReadStream* stream) is now in Flex.h

/**
 *  Write out a FLEX header.  Note that this is a STATIC method.
 *  MODIFIED: Takes a ScummVM WriteStream.
 *  @param stream  ScummVM WriteStream to which the data is written.
 *  @param title    Flex file title.
 *  @param count    Number of entries in the flex.
 *  @param vers Version of the flex file.
 */
void Flex_header::write(
		ScummVM::Common::WriteStream* stream, // File to write to.
		const char*            title,
		size_t                 count_val,    // # entries.
		Flex_header::Flex_vers vers)
{
	if (!stream || !stream->isOpen()) return;

	char titlebuf[FLEX_TITLE_LEN];
	strncpy(titlebuf, title, FLEX_TITLE_LEN -1);
	titlebuf[FLEX_TITLE_LEN -1] = '\0'; // Ensure null termination
	for(size_t i = strlen(title); i < FLEX_TITLE_LEN; ++i) titlebuf[i] = '\0'; // Pad with nulls

	stream->write(titlebuf, FLEX_TITLE_LEN);
	stream->write_le32(FLEX_MAGIC1);    // Magic number.
	stream->write_le32(static_cast<uint32>(count_val));
	if (vers == orig) {    // 2nd magic #:  use for version.
		stream->write_le32(FLEX_MAGIC2);
	} else {
		stream->write_le32(EXULT_FLEX_MAGIC2 + static_cast<uint32>(vers));
	}
	// Write padding to fill header to FLEX_HEADER_LEN
	// The original padding was 9 uint32s, plus 2 uint32s per entry for the table (written later).
	// The header itself is FLEX_HEADER_LEN (128 bytes).
	// Title (80) + magic1 (4) + count (4) + magic2 (4) = 92 bytes.
	// So, 128 - 92 = 36 bytes of padding needed if table is written separately.
	// Original code wrote FLEX_HEADER_PADDING (9) * 4 = 36 bytes of zeroes here for padding.
	// And then wrote 2 * count * 4 bytes of zeroes for the table area if it was written later.
	// For now, just pad the header to FLEX_HEADER_LEN. The table is written separately by Flex_writer.
	for (size_t i = 0; i < (FLEX_HEADER_LEN - (FLEX_TITLE_LEN + 3*4)); ++i) {
		stream->writeByte(0);
	}
}

/**
 *  Reads the header from a flex and builds an object index.
 *  MODIFIED: Uses _scummvmStream (member of U7file base class).
 */
void Flex::index_file() {
	if (!_scummvmStream || !_scummvmStream->isOpen()) {
		ScummVM::debug(1, "Flex::index_file - Stream not available or not open for %s", identifier.name.c_str());
		// hdr is already read or attempted in Flex constructor. If it failed, _scummvmStream might be reset.
		// If hdr was read successfully, but we are here with no stream, it_s an issue.
		if (hdr.count_val > 0 && !_scummvmStream) { // Header seemed okay but stream is gone
		    throw file_read_exception(identifier.name.c_str());
        }
        // If hdr.count_val is 0, or stream was bad from start, object_list remains empty, which is fine.
		return;
	}

	// hdr should have been read in the Flex constructor.
	// If hdr.read failed, _scummvmStream would have been reset, and we would have returned above.
	// So, we can assume hdr is valid here.

	ScummVM::debug(2, "Flex::index_file - Title: %s, Count: %u for %s", hdr.title, (unsigned int)hdr.count_val, identifier.name.c_str());

	// The stream should be positioned at the start of the object table (after the header).
	// Flex_header::read should have left it at FLEX_HEADER_LEN.
	_scummvmStream->seek(Flex_header::FLEX_HEADER_LEN, SEEK_SET);
	if (_scummvmStream->readError() || _scummvmStream->pos() != Flex_header::FLEX_HEADER_LEN) {
		ScummVM::error("Flex::index_file - Failed to seek to object table for %s", identifier.name.c_str());
		object_list.clear();
		_scummvmStream.reset(); // Invalidate stream
		throw file_read_exception(identifier.name.c_str());
	}

	object_list.reserve(hdr.count_val);
	for (uint32 c = 0; c < hdr.count_val; ++c) {
		Reference f;
		f.offset = _scummvmStream->read_le32();
		f.size   = _scummvmStream->read_le32();

		if (_scummvmStream->readError()) {
			ScummVM::error("Flex::index_file - Read error while reading object table entry %u for %s", c, identifier.name.c_str());
			object_list.clear();
			_scummvmStream.reset(); // Invalidate stream
			throw file_read_exception(identifier.name.c_str());
		}

		// Offset is relative to the start of the data area, which is after the object table.
		// The object table itself is hdr.count_val * 2 * 4 bytes long.
		f.offset += Flex_header::FLEX_HEADER_LEN + (hdr.count_val * 2 * 4);

		ScummVM::debug(3, "Flex::index_file - Item %u: size %u bytes @ offset %u (abs) for %s", c, (unsigned int)f.size, (unsigned int)f.offset, identifier.name.c_str());
		object_list.push_back(f);
	}
}

// get_entry_info might be obsolete if U7file::retrieve is used directly.
// If it_s still needed by some Exult logic, it can be kept.
// size_t Flex::get_entry_info(uint32 objnum, size_t& len) {
// 	if (!_scummvmStream || !_scummvmStream->isOpen() || objnum >= object_list.size()) {
// 		len = 0;
// 		return 0;
// 	}
// 	len = object_list[objnum].size;
// 	return object_list[objnum].offset; // This offset is absolute from file start
// }

// Flex::is_flex(const std::string& fname) removed as direct file access is gone.
// U7FileManager will use Flex_header::is_flex(ScummVM::Common::SeekableReadStream*) for type detection.

/**
 *  Start writing out a new Flex file.
 *  MODIFIED: Takes a ScummVM WriteStream.
 */
Flex_writer::Flex_writer(
		ScummVM::Common::WriteStream* stream,    ///< Where to write.
		const char*            title,    ///< Flex title.
		size_t                 cnt,      ///< Number of entries we_ll write.
		Flex_header::Flex_vers vers      ///< Version of flex file.
		)
		: _stream(stream), _count(cnt), _start_pos(0), _cur_obj_data_start(0) {
	if (!_stream || !_stream->isOpen()) {
		// ScummVM::error("Flex_writer: Output stream is not valid.");
		// Consider throwing or setting an error state.
		_table.reset(); // Ensure table is not allocated if stream is bad
		return;
	}
	_start_pos = _stream->pos();
	// Write out header placeholder (actual counts/offsets filled in flush).
	Flex_header::write(_stream, title, _count, vers);
	// Create table for offsets and lengths.
	_table = ScummVM::make_unique_array<uint8>(2 * _count * 4);
	_table_ptr = _table.get();
	_cur_obj_data_start = _stream->pos();    // Store start of 1st entry_s data area.
}

Flex_writer::~Flex_writer() {
	// flush() should be called explicitly by the user of Flex_writer if they want to ensure
	// data is written before destruction, especially if stream lifetime is managed externally.
	// However, for safety, call it here if not already flushed.
	flush(); 
}

void Flex_writer::flush() {
	if (_table && _stream && _stream->isOpen()) {
		long current_pos = _stream->pos(); // Save current position (end of last object data)

		// Seek to the beginning of the object table area (right after the header)
		_stream->seek(_start_pos + Flex_header::FLEX_HEADER_LEN, SEEK_SET);
		_stream->write(_table.get(), 2 * _count * 4); // Write the actual table
		
		_stream->seek(current_pos, SEEK_SET); // Restore to end of data
		// _stream->flush(); // ScummVM WriteStream usually doesn_t have a flush like C++ iostreams
		
		_table.reset(); // Indicate table has been written
		_table_ptr = nullptr;
	}
}

/**
 *  Call this when done writing out a section (object_s data).
 */
void Flex_writer::finish_object() {
	if (!_table_ptr || !_stream || !_stream->isOpen()) return; // Table already flushed or stream error

	const long current_data_end_pos = _stream->pos();
	uint32 offset_val = static_cast<uint32>(_cur_obj_data_start - (_start_pos + Flex_header::FLEX_HEADER_LEN + (2 * _count * 4)));
	uint32 length_val = static_cast<uint32>(current_data_end_pos - _cur_obj_data_start);

	ScummVM::write_le32(_table_ptr, offset_val); // Store start of section relative to data area start
	_table_ptr += 4;
	ScummVM::write_le32(_table_ptr, length_val);    // Store length.
	_table_ptr += 4;

	_cur_obj_data_start = current_data_end_pos; // Next object_s data will start here
}

// write_object methods need to be adapted to use ScummVM::Common::WriteStream passed to constructor.
// For brevity, only one example is shown here. Others would follow a similar pattern.

void Flex_writer::write_object(const void* data, size_t len) {
    if (!_stream || !_stream->isOpen() || !_table) return;
    _stream->write(data, len);
    finish_object();
}

// Other Flex_writer::write_object overloads and template methods would need similar adaptation
// to use the member _stream instead of taking a OStreamDataSource& dout parameter.
// This is a significant refactor for Flex_writer and is marked as TODO if full write support is needed.
// For now, the focus is on VFS read integration.


