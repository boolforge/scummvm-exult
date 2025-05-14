/*
 *  Copyright (C) 2000-2022  The Exult Team
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

#ifndef U7FILE_H
#define U7FILE_H

#include "U7obj.h"
#include "common_types.h"
// #include "databuf.h" // IDataSource, IExultDataSource, IFileDataSource, IBufferDataSource - to be replaced by ScummVM streams
#include "exceptions.h"

#include "common/stream.h" // ScummVM Stream API
#include "common/common.h" // For ScummVM_MAKE_UNIQUE

#include <string>
#include <utility>
#include <memory> // For std::unique_ptr

/**
 *  The U7file class is an abstract data object which is
 *  the basis of "file" reading classes for flex, table, iff
 *  and "flat" (single object) file formats.
 *  MODIFIED: This class now uses ScummVM::Common::SeekableReadStream for data access.
 */
class U7file {
protected:
	/// This is used to uniquely identify the data object.
	/// For files, this is {"path/filename", -1}.
	/// For objects in multi-object files, this is
	/// {"path/filename", index}.
	File_spec identifier;

	/// ScummVM stream for reading data. U7file takes ownership.
	std::unique_ptr<ScummVM::Common::SeekableReadStream> _scummvmStream;

	/// Causes file/buffer information to be read. Or will do,
	/// when it is implemented for derived classes.
	/// This method will now use _scummvmStream.
	virtual void index_file() {}

	struct Reference {
		size_t offset;
		size_t size;
	};

	virtual Reference get_object_reference(uint32 objnum) const {
		// Default implementation, derived classes like Flex, Table will override.
		// For Flat files, this might mean the whole file if only one object.
		if (objnum == 0 && _scummvmStream && _scummvmStream->isOpen()) {
			return Reference{0, (size_t)_scummvmStream->size()};
		}
		return Reference{0, 0};
	}

public:
	/// Initializes from a file spec and a ScummVM stream.
	/// @param spec A file specification.
	/// @param stream A ScummVM seekable read stream. U7file takes ownership.
	U7file(File_spec spec, std::unique_ptr<ScummVM::Common::SeekableReadStream> stream)
			: identifier(std::move(spec)), _scummvmStream(std::move(stream)) {
		// Derived classes should call index_file() in their constructors after this base constructor.
	}

	virtual ~U7file() noexcept = default; // std::unique_ptr will handle the stream
	U7file(const U7file&)            = delete;
	U7file& operator=(const U7file&) = delete;
	U7file(U7file&&)                 = delete; // Or define move constructor/assignment properly if needed
	U7file& operator=(U7file&&)      = delete;

	virtual size_t number_of_objects() const = 0;

	/**
	 *  Reads the desired object from the file via ScummVM stream.
	 *  @param objnum   Number of object to read.
	 *  @param len  Receives the length of the object, or zero in any failure.
	 *  @return Buffer created with new[] containing the object data or
	 *  null on any failure.
	 */
	std::unique_ptr<unsigned char[]> retrieve(uint32 objnum, std::size_t& len) {
		len = 0;
		if (!_scummvmStream || !_scummvmStream->isOpen() || objnum >= number_of_objects()) {
			return nullptr;
		}
		const Reference ref = get_object_reference(objnum);
		if (ref.size == 0) return nullptr; // Nothing to read

		_scummvmStream->seek(ref.offset, SEEK_SET);
		// Check if seek was successful, e.g., by comparing current position or checking error state if available.
		// For now, assume seek is successful if no exception is thrown and stream is still open.
		if (_scummvmStream->pos() != ref.offset) { // Basic check
		    // ScummVM::debug(1, "U7file::retrieve: Seek failed for object %u in %s", objnum, identifier.name.c_str());
		    return nullptr;
		}

		len = ref.size;
		auto buffer = ScummVM::make_unique_array<unsigned char>(len);
		if (_scummvmStream->read(buffer.get(), len) != len) {
			// ScummVM::debug(1, "U7file::retrieve: Read failed for object %u in %s", objnum, identifier.name.c_str());
			len = 0;
			return nullptr;
		}
		return buffer;
	}

	virtual const char* get_archive_type() const = 0;

	/**
	 *  Reads the desired object from the file and wraps it in an IBufferDataSource-like manner.
	 *  NOTE: IBufferDataSource is an Exult class. This might need to return a ScummVM equivalent
	 *  or a simple data structure if IBufferDataSource is being phased out.
	 *  For now, returning a struct or pair might be cleaner if IBufferDataSource is tied to old system.
	 */
	// IBufferDataSource retrieve(uint32 objnum) { // Old return type
	// 	std::size_t len;
	// 	auto        buffer = retrieve(objnum, len);
	// 	return IBufferDataSource(std::move(buffer), len); // This will fail as IBufferDataSource is from databuf.h
	// }

    // TODO: Decide on replacement for IBufferDataSource if it's removed.
    // For now, users can call retrieve(objnum, len) directly.

    bool hasStream() const { return _scummvmStream && _scummvmStream->isOpen(); }
};

// TODO: The U7DataFile and U7DataBuffer template classes are commented out as they
// were based on the old IDataSource system. The U7FileManager will now be responsible
// for creating U7file-derived objects (Flat, Flex, IFF, Table) and passing the
// ScummVM stream to their constructors. These derived classes will need to be updated accordingly.

/*
 *  This template class implements a basic framework for loading
 *  data from a file through a stream.
 *
 *  The template parameter class T should be derived from U7file.
 */
// template <class T>
// class U7DataFile final : public T {
// public:
// 	/// This constructor treats the identifier as a file name and
// 	/// opens the file if it exists. It also creates and initializes
// 	/// the data source, or sets it to null if the file is not there.
// 	/// @param spec Name of file to open. Ignores the index portion.
// 	explicit U7DataFile(const File_spec& spec) : T(spec) {
// 		this->data = std::make_unique<IFileDataSource>(this->identifier.name);
// 		if (this->data->good()) {
// 			this->index_file();
// 		}
// 	}
// };

/*
 *  This template class implements a basic framework for loading
 *  data from a memory buffer. This class TAKES OWNERSHIP of the
 *  buffer and deletes it at the end.
 *
 *  The template parameter class T should be derived from U7file.
 */
// template <class T>
// class U7DataBuffer final : public T {
// public:
// 	/// Creates and initializes the data source from the data source.
// 	/// @param spec Unique identifier for this data object.
// 	/// @param dt   Unique pointer to IExultDataSource that we shoud use.
// 	U7DataBuffer(const File_spec& spec, std::unique_ptr<IExultDataSource> dt)
// 			: T(spec) {
// 		this->data = std::move(dt);
// 		this->index_file();
// 	}

// 	/// Creates and initializes the data source from the buffer.
// 	/// Takes ownership of the parameters and deletes them when done,
// 	/// so callers should NOT delete them.
// 	/// @param spec Unique identifier for this data object.
// 	/// @param buf  Buffer to read from. The class deletes the buffer
// 	/// at the end. Can be null if l also is.
// 	/// @param l    Length of data in the buffer.
// 	U7DataBuffer(const File_spec& spec, const char* buf, unsigned int l)
// 			: T(spec) {
// 		this->data = std::make_unique<IExultDataSource>(buf, l);
// 		this->index_file();
// 	}

// 	/// Creates and initializes the data source from the specified
// 	/// file/number pair.
// 	/// @param spec Unique identifier for this data object. The 'name'
// 	/// member **MUST** be a valid file name.
// 	explicit U7DataBuffer(const File_spec& spec) : T(spec) {
// 		this->data = std::make_unique<IExultDataSource>(spec.name, spec.index);
// 		this->index_file();
// 	}
// };

/**
 *  This class stores data from each file.
 */
class File_data {
protected:
	/// The file this refers to. Should NOT be deleted by File_data!
	/// U7FileManager owns the U7file objects.
	U7file* file_ptr; // Changed from 'file' to avoid conflict if U7file has a member named 'file'
	/// Number of objects in the file.
	size_t count;
	/// Whether the file comes from patch dir.
	bool patch;

public:
	explicit File_data(const File_spec& spec);

	// Constructor to be used by U7FileManager when it creates U7file instances
	File_data(U7file* u7f, bool is_patch_file);

	bool from_patch() const {
		return patch;
	}

	size_t number_of_objects() const {
		return count;
	}

	std::unique_ptr<unsigned char[]> retrieve(
			uint32 objnum, std::size_t& len) const {
		bool pt; // patch info, not used by caller here
		return retrieve(objnum, len, pt);
	}

	std::unique_ptr<unsigned char[]> retrieve(
			uint32 objnum, std::size_t& len, bool& pt) const {
		pt  = patch;
		len = 0;
		if (file_ptr) {
			return file_ptr->retrieve(objnum, len);
		}
		return nullptr;
	}

	// IBufferDataSource retrieve(uint32 objnum) const { // Old return type
	// 	if (file_ptr) {
	// 		return file_ptr->retrieve(objnum); // This will also fail due to IBufferDataSource
	// 	}
	// 	return IBufferDataSource(nullptr, 0);
	// }

	const char* get_archive_type() const { // made const
		return file_ptr ? file_ptr->get_archive_type() : "NONE";
	}
};

/**
 *  Abstract file loader, adapted for loading different resources
 *  from different "files".
 */
class U7multifile {
protected:
	/// This is the list of contained files.
	std::vector<File_data> files;

public:
	U7multifile(const U7multifile&)            = delete;
	U7multifile& operator=(const U7multifile&) = delete;
	U7multifile() = default; // Allow default construction
	// Old constructors that took File_spec directly might need to change
	// if File_data construction changes significantly.
	explicit U7multifile(const File_spec& spec);
	U7multifile(const File_spec& spec0, const File_spec& spec1);
	U7multifile(
			const File_spec& spec0, const File_spec& spec1,
			const File_spec& spec2);
	U7multifile(const std::vector<File_spec>& specs);

    // Method to add a U7file (via File_data) managed by U7FileManager
    void add_file_data(File_data&& fd);

	size_t number_of_objects() const;

	std::unique_ptr<unsigned char[]> retrieve(
			uint32 objnum, std::size_t& len) const {
		bool pt;
		return retrieve(objnum, len, pt);
	}

	std::unique_ptr<unsigned char[]> retrieve(
			uint32 objnum, std::size_t& len, bool& patch) const;

	// IBufferDataSource retrieve(uint32 objnum) { // Old return type
	// 	bool pt;
	// 	return retrieve(objnum, pt);
	// }

	// IBufferDataSource retrieve(uint32 objnum, bool& patch) { // Old return type
	// 	std::size_t len;
	// 	auto        buffer = retrieve(objnum, len, patch);
	// 	return IBufferDataSource(std::move(buffer), len);
	// }
};

#endif

