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

#ifndef FLAT_H
#define FLAT_H

#include "U7file.h"
#include "common_types.h"
// #include "exceptions.h" // Not directly used in this header after changes

#include "common/stream.h" // For ScummVM::Common::SeekableReadStream
#include <memory> // For std::unique_ptr

// class DataSource; // Replaced by ScummVM streams

/**
 *  This is a "flat" file, which contains but a single object.
 *  MODIFIED: Now uses ScummVM::Common::SeekableReadStream via U7file base class.
 */
class Flat : public U7file {
protected:
	Reference get_object_reference(uint32 objnum) const override {
		// A flat file contains a single object (objnum 0) which is the entire file content.
		if (objnum == 0 && _scummvmStream && _scummvmStream->isOpen()) {
			return Reference{0, (size_t)_scummvmStream->size()};
		}
		return Reference{0, 0}; // Invalid object number or no stream
	}

public:
	/// Constructor now takes a ScummVM stream.
	/// @param spec File name and object index pair.
	/// @param stream ScummVM stream. Flat (via U7file) takes ownership.
	Flat(const File_spec& spec, std::unique_ptr<ScummVM::Common::SeekableReadStream> stream)
		: U7file(spec, std::move(stream)) {
		// No specific indexing needed for Flat files beyond what U7file provides.
		// index_file(); // U7file base class doesn't have a meaningful default index_file
	}

	size_t number_of_objects() const override { // made const
		// A flat file always contains exactly one object.
		return (_scummvmStream && _scummvmStream->isOpen()) ? 1 : 0;
	}

	const char* get_archive_type() const override { // made const
		return "FLAT";
	}

	// static bool is_flat(IDataSource* in); // Old signature, IDataSource is removed.
	// U7FileManager will determine file type before creating Flat instance.
	// This function might be re-purposed or removed.
	// For now, let's assume U7FileManager handles type detection.

	// static bool is_flat(const std::string& fname); // Old signature, direct file access removed.
	// U7FileManager will handle opening files via ScummVM VFS.
};

// using FlatFile   = U7DataFile<Flat>; // U7DataFile template removed
// using FlatBuffer = U7DataBuffer<Flat>; // U7DataBuffer template removed
// U7FileManager will directly create Flat objects with streams.

#endif

