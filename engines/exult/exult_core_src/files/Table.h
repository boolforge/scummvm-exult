/*
 *  Copyright (C) 2001-2022  The Exult Team
 */

#ifndef TABLE_H
#define TABLE_H

#include "common/types.h"
#include "common/ReadStream.h"
#include "common/WriteStream.h"
#include "files/U7file.h"

namespace ExultCore {

class Table : public U7file {
public:
	Table(const File_spec& s, std::unique_ptr<ScummVM::Common::SeekableReadStream> stream);
	Table(const File_spec& s, std::unique_ptr<IExultDataSource> data);
	Table(const std::string& name);
	~Table() override;

	// U7file interface
	uint32 get_size() override;
	uint32 read(void* buffer, uint32 bytes) override;
	uint32 seek(int offset, int origin) override;
	uint32 tell() override;
	bool eof() override;

	// Specific Table methods
	bool is_table_type(int type);
	uint32 get_entry_size(int entry_num);
	uint32 get_entry_offset(int entry_num);
	uint32 get_num_entries();

	// ScummVM integration
	static bool is_table(ScummVM::Common::SeekableReadStream* stream);

private:
	// Private members for Table implementation
	uint32 _num_entries;
	std::vector<uint32> _entry_offsets;
	std::vector<uint32> _entry_sizes;
};

} // namespace ExultCore

#endif // TABLE_H

