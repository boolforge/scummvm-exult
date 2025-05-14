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

#ifndef IFF_H
#define IFF_H

#include "U7file.h"
#include "common_types.h"
#include "exceptions.h" // Retained for file_open_exception, etc.

#include "common/stream.h" // For ScummVM::Common::SeekableReadStream
#include "common/endians.h"  // For ScummVM_READ_BE32 etc.

#include <cstring> // For strncmp, etc.
#include <string>
#include <vector>
#include <memory> // For std::unique_ptr

// class DataSource; // Replaced by ScummVM streams

/**
 *  The IFF class is an data reader which reads data in the IFF
 *  file format.
 *  MODIFIED: Now uses ScummVM::Common::SeekableReadStream via U7file base class.
 */
class IFF : public U7file {
public:
	// Standard IFF FORM header structure
	struct IFF_FORM_Chunk {
		char   chunkID[4]; // "FORM"
		uint32 chunkSize;  // Size of the rest of the file after this field
		char   formType[4]; // e.g., "CAT ", "LIST", specific game types like "SHAP"

		bool read(ScummVM::Common::SeekableReadStream* stream) {
			if (!stream || !stream->isOpen()) return false;
			if (stream->read(chunkID, 4) != 4) return false;
			chunkSize = stream->read_be32(); // IFF uses big-endian
			if (stream->read(formType, 4) != 4) return false;
			return !stream->readError();
		}

		bool isValid() const {
			return strncmp(chunkID, "FORM", 4) == 0;
		}
	};

	// General IFF chunk header structure
	struct IFF_Generic_Chunk_Header {
		char   chunkID[4];
		uint32 chunkSize;

		bool read(ScummVM::Common::SeekableReadStream* stream) {
			if (!stream || !stream->isOpen()) return false;
			if (stream->read(chunkID, 4) != 4) return false;
			chunkSize = stream->read_be32();
			return !stream->readError();
		}
	};

	// Exult-specific u7IFFobj structure (seems to be for named objects within some IFFs)
	// This might be specific to certain IFF types Exult handles, like shapes.iff
	// struct u7IFFobj {
	// 	char name[8];
	// 	// char    data[]; // Variable
	// };

protected:
	IFF_FORM_Chunk _formHeader;
	std::vector<Reference> object_list; // Stores offset and size for each identified chunk/object

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
	/// @param stream ScummVM stream. IFF (via U7file) takes ownership.
	IFF(const File_spec& spec, std::unique_ptr<ScummVM::Common::SeekableReadStream> stream)
		: U7file(spec, std::move(stream)) {
		if (_scummvmStream && _scummvmStream->isOpen()) {
			long original_pos = _scummvmStream->pos();
			_scummvmStream->seek(0, SEEK_SET);
			if (_formHeader.read(_scummvmStream.get()) && _formHeader.isValid()) {
				index_file(); // Populate object_list based on FORM type and subsequent chunks
			} else {
				_scummvmStream.reset(); // Invalidate if not a valid IFF FORM
                // ScummVM::debug(1, "IFF: Failed to read or validate IFF FORM header for %s", identifier.name.c_str());
			}
			// Restore position if stream is still valid, though index_file might have moved it.
			// if (_scummvmStream) _scummvmStream->seek(original_pos, SEEK_SET);
		} else {
            // ScummVM::debug(1, "IFF: Stream not valid for %s", identifier.name.c_str());
        }
	}

	size_t number_of_objects() const override { // made const
		return object_list.size();
	}

	const char* get_archive_type() const override { // made const
		return "IFF";
	}

	const IFF_FORM_Chunk& getFormHeader() const { return _formHeader; }

	// MODIFIED: Checks if stream is an IFF file
	static bool is_iff(ScummVM::Common::SeekableReadStream* stream) {
		if (!stream || !stream->isOpen() || stream->size() < 12) return false; // Minimum size for FORM header
		long old_pos = stream->pos();
		stream->seek(0, SEEK_SET);
		IFF_FORM_Chunk testHeader;
		bool result = testHeader.read(stream) && testHeader.isValid();
		stream->seek(old_pos, SEEK_SET); // Restore position
		return result && !stream->readError();
	}

	// static bool is_iff(const std::string& fname); // Direct file access removed
};

// using IFFFile   = U7DataFile<IFF>; // U7DataFile template removed
// using IFFBuffer = U7DataBuffer<IFF>; // U7DataBuffer template removed
// U7FileManager will directly create IFF objects with streams.

#endif

