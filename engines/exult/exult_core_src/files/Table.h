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

#ifndef TABLE_H
#define TABLE_H

#include "U7file.h"
#include "common_types.h"
#include "exceptions.h" // Retained for file_open_exception, etc.

#include "common/stream.h" // For ScummVM::Common::SeekableReadStream
#include "common/endians.h"  // For ScummVM_READ_LE32 etc. (may be needed in .cc)

#include <string> // For std::string (used in old is_table)
#include <vector>
#include <memory> // For std::unique_ptr

// class DataSource; // Replaced by ScummVM streams

/**
 *  The Table class is an data reader which reads data in the table
 *  file format. The actual data need not be in a file, however.
 *  MODIFIED: Now uses ScummVM::Common::SeekableReadStream via U7file base class.
 */
class Table : public U7file {
protected:
	/// List of objects in the table file.
	std::vector<Reference> object_list;

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
	/// @param stream ScummVM stream. Table (via U7file) takes ownership.
	Table(const File_spec& spec, std::unique_ptr<ScummVM::Common::SeekableReadStream> stream)
		: U7file(spec, std::move(stream)) {
		if (_scummvmStream && _scummvmStream->isOpen()) {
			index_file(); // Populate object_list
		} else {
            // ScummVM::debug(1, "Table: Stream not valid for %s", identifier.name.c_str());
        }
	}

	size_t number_of_objects() const override { // made const
		return object_list.size();
	}

	const char* get_archive_type() const override { // made const
		return "TABLE";
	}

	// static bool is_table(IDataSource* in); // Old signature, IDataSource is removed.
	// static bool is_table(const std::string& fname); // Direct file access removed.
	// U7FileManager will determine file type before creating Table instance.
	// A stream-based is_table could be added here if needed for U7FileManager.
	// static bool is_table(ScummVM::Common::SeekableReadStream* stream);
};

// using TableFile   = U7DataFile<Table>; // U7DataFile template removed
// using TableBuffer = U7DataBuffer<Table>; // U7DataBuffer template removed
// U7FileManager will directly create Table objects with streams.

#endif

