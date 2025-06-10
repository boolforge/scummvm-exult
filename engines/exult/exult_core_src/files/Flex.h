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

#ifndef FLEX_H
#define FLEX_H

#include "U7file.h"
#include "common_types.h"
#include "exceptions.h" // Retained for file_open_exception, etc.

#include "common/stream.h" // For ScummVM::Common::SeekableReadStream
#include "common/endians.h"  // For ScummVM_READ_LE32 etc.

#include <cstring> // For strncpy, etc.
// #include <iosfwd> // Not directly used
#include <string>
#include <vector>
#include <memory> // For std::unique_ptr

struct Flex_header {
	constexpr static const uint32 FLEX_MAGIC1         = 0xffff1a00U;
	constexpr static const uint32 FLEX_MAGIC2         = 0x000000ccU;
	constexpr static const uint32 EXULT_FLEX_MAGIC2   = 0x0000cc00U;
	constexpr static const uint32 FLEX_TITLE_LEN      = 80;
	constexpr static const uint32 FLEX_HEADER_LEN     = 128;
	// constexpr static const uint32 FLEX_HEADER_PADDING = 9; // Not directly used in read logic
	constexpr static const uint32 FLEX_VERSION_MASK   = 0xffU;

	/// Exult has an extended flex file format to support
	/// extended shape numbers in IFIX objects.
	enum Flex_vers {
		orig     = 0,    ///<    Original file version.
		exult_v2 = 1     ///<    Exult extension for IFIX objects.
	};

	char   title[FLEX_TITLE_LEN];
	uint32 magic1_val; // Renamed from magic1 to avoid conflict with constant
	uint32 count_val;  // Renamed from count
	uint32 magic2_val; // Renamed from magic2
	// uint32 padding[FLEX_HEADER_PADDING]; // Padding is read but not stored explicitly

	Flex_vers get_vers() const {
		return (magic2_val & ~FLEX_VERSION_MASK) == EXULT_FLEX_MAGIC2
					   ? static_cast<Flex_vers>(magic2_val & FLEX_VERSION_MASK)
					   : orig;
	}

	// MODIFIED: Reads header from a ScummVM stream
	bool read(ScummVM::Common::SeekableReadStream* stream) {
		if (!stream || !stream->isOpen()) return false;
		stream->seek(0, SEEK_SET);
		if (stream->read(title, FLEX_TITLE_LEN) != FLEX_TITLE_LEN) return false;
		magic1_val = stream->read_le32();
		count_val  = stream->read_le32();
		magic2_val = stream->read_le32();

		if (stream->readError()) return false;

		// Check magic numbers
		if (magic1_val != FLEX_MAGIC1 || 
		    ((magic2_val & ~FLEX_VERSION_MASK) != FLEX_MAGIC2 && 
		     (magic2_val & ~FLEX_VERSION_MASK) != EXULT_FLEX_MAGIC2)) {
			return false; // Not a valid Flex file
		}
		// Skip padding to reach end of header for subsequent reads
		stream->seek(FLEX_HEADER_LEN, SEEK_SET);
		return !stream->readError();
	}

	// static void write(ODataSource* out, const char* title, size_t count, Flex_vers vers = orig); // Write logic deferred
	
	// MODIFIED: Checks if stream is a Flex file
	static bool is_flex(ScummVM::Common::SeekableReadStream* stream) {
		if (!stream || !stream->isOpen() || stream->size() < FLEX_HEADER_LEN) return false;
		long old_pos = stream->pos();
		stream->seek(0, SEEK_SET);

		char test_title[FLEX_TITLE_LEN];
		stream->read(test_title, FLEX_TITLE_LEN); // Read title, not strictly checked for magic
		uint32 m1 = stream->read_le32();
		stream->read_le32(); // count, not checked for magic
		uint32 m2 = stream->read_le32();
		
		stream->seek(old_pos, SEEK_SET); // Restore position
		if (stream->readError()) return false;

		return (m1 == FLEX_MAGIC1 && 
		        (((m2 & ~FLEX_VERSION_MASK) == FLEX_MAGIC2) || ((m2 & ~FLEX_VERSION_MASK) == EXULT_FLEX_MAGIC2)));
	}
};

/**
 *  The Flex class is an data reader which reads data in the flex
 *  file format.
 *  MODIFIED: Now uses ScummVM::Common::SeekableReadStream via U7file base class.
 */
class Flex : public U7file {
protected:
	Flex_header            hdr;
	std::vector<Reference> object_list;
	// size_t                 start_pos; // This was likely data start after header, now implicit in stream pos

	void index_file() override;

	Reference get_object_reference(uint32 objnum) const override {
		if (objnum < object_list.size()) {
			return object_list[objnum];
		}
		return Reference{0,0}; // Invalid index
	}

public:
	/// Constructor now takes a ScummVM stream.
	/// @param spec File name and object index pair.
	/// @param stream ScummVM stream. Flex (via U7file) takes ownership.
	Flex(const File_spec& spec, std::unique_ptr<ScummVM::Common::SeekableReadStream> stream)
		: U7file(spec, std::move(stream)) {
		if (_scummvmStream && _scummvmStream->isOpen()) {
			if (hdr.read(_scummvmStream.get())) {
				index_file(); // Populate object_list
			} else {
				// Handle header read error, maybe invalidate the stream or object
				_scummvmStream.reset(); // Invalidate if header is bad
                // Consider throwing an exception or setting an error state
                // ScummVM::debug(1, "Flex: Failed to read or validate Flex header for %s", identifier.name.c_str());
			}
		} else {
            // ScummVM::debug(1, "Flex: Stream not valid for %s", identifier.name.c_str());
        }
	}

	Flex_header::Flex_vers get_vers() const {
		return hdr.get_vers();
	}

	size_t number_of_objects() const override { // made const
		return object_list.size();
	}

	// size_t get_entry_info(uint32 objnum, size_t& len); // May need reimplementation if still used

	const char* get_archive_type() const override { // made const
		return "FLEX";
	}

	// static bool is_flex(IDataSource* in); // Replaced by stream version in Flex_header
	// static bool is_flex(const std::string& fname); // Direct file access removed
	// void IndexFlexFile(); // Merged into index_file()
};

// using FlexFile   = U7DataFile<Flex>; // U7DataFile template removed
// using FlexBuffer = U7DataBuffer<Flex>; // U7DataBuffer template removed
// U7FileManager will directly create Flex objects with streams.

// Flex_writer class definition (for writing Flex files) is omitted for now,
// as the primary focus is on reading/VFS integration.
// It would also need significant refactoring to use ScummVM_Engine_s WriteStream.

#endif

