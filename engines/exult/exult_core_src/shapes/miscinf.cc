/*
 *  miscinf.cc - Information about several previously-hardcoded shape data.
 *
 *  Copyright (C) 2006-2022  The Exult Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the GNU General Public License as published by
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

#include "miscinf.h"

#include "actors.h"
#include "data_utils.h"
#include "databuf.h"
#include "exult_constants.h"
#include "game.h"
#include "gamewin.h"
#include "ignore_unused_variable_warning.h"
#include "msgfile.h"
#include "shapeid.h"
#include "ucmachine.h"
#include "utils.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

// TODO: Include ScummVM headers for SeekableReadStream and ExultFileAdapter
// #include "common/streams.h"
// #include "ExultEngine.h" // Assuming ExultEngine provides access to ExultFileAdapter

using std::ifstream;
using std::map;
using std::pair;
using std::skipws;
using std::string;
using std::stringstream;
using std::vector;

using namespace std;
using namespace std::string_view_literals;

struct Shapeinfo_data {
	vector<pair<string, int>> paperdoll_source_table;
	vector<pair<int, int>>    imported_gump_shapes;
	vector<pair<int, int>>    blue_shapes;
	vector<pair<int, int>>    imported_skin_shapes;
	map<string, int>          gumpvars;
	map<string, int>          skinvars;
};

struct Avatar_data {
	map<bool, Base_Avatar_info>     def_av_info;
	Avatar_default_skin             base_av_info;
	vector<Skin_data>               skins_table;
	map<int, bool>                  unselectable_skins;
	map<int, int>                   petra_table;
	map<int, Usecode_function_data> usecode_funs;
	static int                      last_skin;
};

std::unique_ptr<Shapeinfo_data> Shapeinfo_lookup::data(nullptr);
std::unique_ptr<Avatar_data>    Shapeinfo_lookup::avdata(nullptr);
int                             Avatar_data::last_skin = 0;

// HACK. NPC Paperdolls need this, but miscinf has too many
// Exult-dependant stuff to be included in ES. Thus, ES
// defines a non-operant version of this.
// Maybe we should do something about this...
int get_skinvar(const std::string& key) {
	return Shapeinfo_lookup::get_skinvar(key);
}

/*
 *  Base parser class shape data.
 */
class Shapeinfo_entry_parser {
public:
	virtual ~Shapeinfo_entry_parser() noexcept = default;
	virtual void parse_entry(
			int index, istream& src, bool for_patch, int version)
			= 0;

	int ReadInt(istream& src, int off = 1) {
		src.ignore(off);
		int ret;
		if (src.peek() == '0') {
			src.ignore(1);
			const char chr = src.peek();
			if (chr == 'x' || chr == 'X') {
				src >> hex;
			} else {
				src.unget();
			}
		}
		src >> ret >> skipws >> dec;
		return ret;
	}

	string ReadStr(istream& src, int off = 1) {
		src.ignore(off);
		string ret;
		getline(src, ret, '/');
		src.unget();
		return ret;
	}
};

class Int_pair_parser : public Shapeinfo_entry_parser {
	map<int, int>& table;

public:
	explicit Int_pair_parser(map<int, int>& tbl) : table(tbl) {}

	void parse_entry(
			int index, istream& src, bool for_patch, int version) final {
		ignore_unused_variable_warning(index, for_patch, version);
		const int key  = ReadInt(src, 0);
		const int data = ReadInt(src);
		table[key]     = data;
	}
};

class Bool_parser : public Shapeinfo_entry_parser {
	map<int, bool>& table;

public:
	explicit Bool_parser(map<int, bool>& tbl) : table(tbl) {}

	void parse_entry(
			int index, istream& src, bool for_patch, int version) final {
		ignore_unused_variable_warning(index, for_patch, version);
		const int key = ReadInt(src, 0);
		table[key]    = true;
	}
};

class Shape_imports_parser : public Shapeinfo_entry_parser {
	vector<pair<int, int>>& table;
	map<string, int>&       shapevars;
	int                     shape;

public:
	Shape_imports_parser(vector<pair<int, int>>& tbl, map<string, int>& sh)
			: table(tbl), shapevars(sh), shape(c_max_shapes) {}

	void parse_entry(
			int index, istream& src, bool for_patch, int version) final {
		ignore_unused_variable_warning(index, for_patch, version);
		pair<int, int> data;
		data.second = ReadInt(src, 0);    // The real shape.
		for (auto& elem : table) {
			if (elem.second == data.second) {
				return;    // Do nothing for repeated entries.
			}
		}
		src.ignore(1);
		if (src.peek() == '%') {
			data.first = shape;    // The assigned shape.
			string key;
			src >> key;
			shapevars[key] = shape;
			shape++;    // Leave it ready for the next shape.
		} else {
			data.first = ReadInt(src, 0);
		}
		table.push_back(data);
	}
};

class Shaperef_parser : public Shapeinfo_entry_parser {
	vector<pair<int, int>>& table;
	map<string, int>&       shapevars;

public:
	Shaperef_parser(vector<pair<int, int>>& tbl, map<string, int>& sh)
			: table(tbl), shapevars(sh) {}

	void parse_entry(
			int index, istream& src, bool for_patch, int version) final {
		ignore_unused_variable_warning(index, for_patch, version);
		pair<int, int> data;
		data.first = ReadInt(src, 0);    // The spot.
		src.ignore(1);
		if (src.peek() == '%') {
			string key;
			src >> key;
			auto it = shapevars.find(key);
			if (it != shapevars.end()) {
				data.second = it->second;    // The shape #.
			} else {
				return;    // Invalid reference; bail out.
			}
		} else {
			data.second = ReadInt(src, 0);
		}
		table.push_back(data);
	}
};

class Paperdoll_source_parser : public Shapeinfo_entry_parser {
	vector<pair<string, int>>& table;
	bool                       erased_for_patch;

public:
	explicit Paperdoll_source_parser(vector<pair<string, int>>& tbl)
			: table(tbl), erased_for_patch(false) {}

	void parse_entry(
			int index, istream& src, bool for_patch, int version) {
		ignore_unused_variable_warning(index, version);
	if (!erased_for_patch && for_patch) {
		table.clear();
	}
	string line;
	src >> line;
	if (line == "static" || (GAME_BG && line == "bg")
		|| (GAME_SI && line == "si")) {
		table.emplace_back(PAPERDOL, -1);
	} else if (line == "si") {
		table.emplace_back("<SERPENT_STATIC>/paperdol.vga", -1);
	} else if (GAME_SI && line == "flx") {
		// ++++ FIXME: Implement in the future for SI paperdoll patches.
		CERR("Paperdoll source file '" << line << "' is not implemented yet.");
	} else if (GAME_BG && line == "flx") {
		const str_int_pair& resource = game->get_resource("files/paperdolvga");
		table.emplace_back(resource.str, resource.num);
	} else {
		CERR("Unknown paperdoll source file '" << line << "' was specified.");
	}
}

class Def_av_shape_parser : public Shapeinfo_entry_parser {
	map<bool, Base_Avatar_info>& table;

public:
	explicit Def_av_shape_parser(map<bool, Base_Avatar_info>& tbl)
			: table(tbl) {}

	void parse_entry(
			int index, istream& src, bool for_patch, int version) final {
		ignore_unused_variable_warning(index, for_patch, version);
		const bool       fmale = ReadInt(src, 0) != 0;
		Base_Avatar_info entry;
		entry.shape_num  = ReadInt(src);
		entry.face_shape = ReadInt(src);
		entry.face_frame = ReadInt(src);
		table[fmale]     = entry;
	}
};

class Base_av_race_parser : public Shapeinfo_entry_parser {
	Avatar_default_skin& table;

public:
	explicit Base_av_race_parser(Avatar_default_skin& tbl) : table(tbl) {}

	void parse_entry(
			int index, istream& src, bool for_patch, int version) final {
		ignore_unused_variable_warning(index, for_patch, version);
		table.default_skin   = ReadInt(src, 0);
		table.default_female = ReadInt(src) != 0;
	}
};

class Multiracial_parser : public Shapeinfo_entry_parser {
	vector<Skin_data>& table;
	map<string, int>&  shapevars;

public:
	explicit Multiracial_parser(vector<Skin_data>& tbl, map<string, int>& sh)
			: table(tbl), shapevars(sh) {}

	int ReadVar(istream& src) {
		src.ignore(1);
		if (src.peek() == '%') {
			const string key = ReadStr(src, 0);
			auto         it  = shapevars.find(key);
			if (it != shapevars.end()) {
				return it->second;    // The var value.
			}
			return -1;    // Invalid reference; bail out.
		}
		return ReadInt(src, 0);
	}

	void parse_entry(
			int index, istream& src, bool for_patch, int version) final {
		ignore_unused_variable_warning(index);
		Skin_data entry;
		entry.skin_id = ReadInt(src, 0);
		Avatar_data::last_skin
				= std::max(entry.skin_id, Avatar_data::last_skin);
		entry.is_female = ReadInt(src) != 0;
		if ((entry.shape_num = ReadVar(src)) < 0) {
			return;
		}
		if ((entry.naked_shape = ReadVar(src)) < 0) {
			return;
		}
		entry.face_shape       = ReadInt(src);
		entry.face_frame       = ReadInt(src);
		entry.alter_face_shape = ReadInt(src);
		entry.alter_face_frame = ReadInt(src);
		entry.copy_info        = version != 2 || src.eof() || ReadInt(src) != 0;
		if (for_patch && !table.empty()) {
			unsigned int i;
			int          found = -1;
			for (i = 0; i < table.size(); i++) {
				if (table[i].skin_id == entry.skin_id
					&& table[i].is_female == entry.is_female) {
					found = i;
					break;
				}
			}
			if (found > -1) {
				table[found] = entry;
				return;
			}
		}
		table.push_back(entry);
	}
};

class Avatar_usecode_parser : public Shapeinfo_entry_parser {
	map<int, Usecode_function_data>& table;
	Usecode_machine*                 usecode;

public:
	explicit Avatar_usecode_parser(map<int, Usecode_function_data>& tbl)
			: table(tbl), usecode(Game_window::get_instance()->get_usecode()) {}

	void parse_entry(
			int index, istream& src, bool for_patch, int version) final {
		ignore_unused_variable_warning(index, for_patch, version);
		Usecode_function_data entry;
		const int             type = ReadInt(src, 0);
		if (src.peek() == ':') {
			const string name = ReadStr(src);
			entry.fun_id      = usecode->find_function(name.c_str(), true);
		} else {
			entry.fun_id = ReadInt(src);
		}
		entry.event_id = ReadInt(src);
		table[type]    = entry;
	}
};

void Paperdoll_source_parser::parse_entry(
		int index, istream& src, bool for_patch, int version) {
	ignore_unused_variable_warning(index, version);
	if (!erased_for_patch && for_patch) {
		table.clear();
	}
	string line;
	src >> line;
	if (line == "static" || (GAME_BG && line == "bg")
		|| (GAME_SI && line == "si")) {
		table.emplace_back(PAPERDOL, -1);
	} else if (line == "si") {
		table.emplace_back("<SERPENT_STATIC>/paperdol.vga", -1);
	} else if (GAME_SI && line == "flx") {
		// ++++ FIXME: Implement in the future for SI paperdoll patches.
		CERR("Paperdoll source file '" << line << "' is not implemented yet.");
	} else if (GAME_BG && line == "flx") {
		const str_int_pair& resource = game->get_resource("files/paperdolvga");
		table.emplace_back(resource.str, resource.num);
	} else {
		CERR("Unknown paperdoll source file '" << line << "' was specified.");
	}
}

/*
 *  Parses a shape data file.
 */
void Shapeinfo_lookup::Read_data_file(
		// Name of file to read, sans extension
		const char* fname,
		// The names of the sections
		const tcb::span<std::string_view>& sections,
		// Parsers to use for each section
		const tcb::span<std::unique_ptr<Shapeinfo_entry_parser>>& parsers) {
	auto value_or = [](std::optional<int> opt) {
		return opt ? *opt : 1;
	};
	// TODO: Replace with ScummVM stream access via ExultFileAdapter
	// The final implementation will look something like:
	// ScummVM::Common::SeekableReadStream* static_stream = ExultEngine::getInstance()->getFileAdapter()->openFileForObject(file.c_str());
	// Text_msg_file_reader static_reader(static_stream);
	Text_msg_file_reader static_reader = [&]() {
		if (GAME_BG || GAME_SI) {
			std::string         file     = "config/" + std::string(fname);
			const str_int_pair& resource = game->get_resource(file.c_str());
			IExultDataSource    ds(resource.str, resource.num);
			return Text_msg_file_reader(ds);
		}
		std::string     file = "<STATIC>/" + std::string(fname) + ".txt";
		IFileDataSource ds(file, false);
		if (!ds.good()) {
			if (!Game::is_editing()) {
				throw file_open_exception(file);
			}
			return Text_msg_file_reader();
		}
		return Text_msg_file_reader(ds);
	}();
	// TODO: Replace with ScummVM stream access via ExultFileAdapter
	// The final implementation will look something like:
	// ScummVM::Common::SeekableReadStream* patch_stream = ExultEngine::getInstance()->getFileAdapter()->openFileForObject(file.c_str());
	// Text_msg_file_reader patch_reader(patch_stream);
	Text_msg_file_reader patch_reader = [&]() {
		std::string file = "<PATCH>/" + std::string(fname) + ".txt";
		if (!U7exists(file)) {
			return Text_msg_file_reader();
		}
		IFileDataSource ds(file, false);
		if (!ds.good()) {
			throw file_open_exception(file);
		}
		return Text_msg_file_reader(ds);
	}();

	int static_version = value_or(static_reader.get_version());
	int patch_version  = value_or(patch_reader.get_version());
	std::vector<std::string> strings;

	for (size_t i = 0; i < sections.size(); i++) {
		uint32 firstMsg
				= static_reader.get_section_strings(sections[i], strings);
		for (size_t j = firstMsg; j < strings.size(); j++) {
			if (!strings[j].empty()) {
				stringstream src(strings[j]);
				parsers[i]->parse_entry(j, src, false, static_version);
			}
		}
	}

	for (size_t i = 0; i < sections.size(); i++) {
		uint32 firstMsg
				= patch_reader.get_section_strings(sections[i], strings);
		for (size_t j = firstMsg; j < strings.size(); j++) {
			if (!strings[j].empty()) {
				stringstream src(strings[j]);
				parsers[i]->parse_entry(j, src, true, patch_version);
			}
		}
	}
}

/*
 *  Setup shape file tables.
 */
void Shapeinfo_lookup::setup_shape_files() {
	if (data != nullptr) {
		return;
	}
	data = make_unique<Shapeinfo_data>();
	std::array sections{
			"paperdoll_source"sv, "gump_imports"sv, "blue_shapes"sv,
			"multiracial_imports"sv};
	std::array parsers = make_unique_array<Shapeinfo_entry_parser>(
			std::make_unique<Paperdoll_source_parser>(
					data->paperdoll_source_table),
			std::make_unique<Shape_imports_parser>(
					data->imported_gump_shapes, data->gumpvars),
			std::make_unique<Shaperef_parser>(
					data->blue_shapes, data->gumpvars),
			std::make_unique<Shape_imports_parser>(
					data->imported_skin_shapes, data->skinvars));
	static_assert(sections.size() == parsers.size());
	Read_data_file("shape_files", sections, parsers);
	// For safety.
	if (data->paperdoll_source_table.empty()) {
		data->paperdoll_source_table.emplace_back(PAPERDOL, -1);
	}
	// Add in patch paperdolls too.
	data->paperdoll_source_table.emplace_back(PATCH_PAPERDOL, -1);
}

/*
 *  Setup avatar data tables.
 */
void Shapeinfo_lookup::setup_avatar_data() {
	setup_shape_files();
	if (avdata != nullptr) {
		return;
	}
	avdata = make_unique<Avatar_data>();
	std::array sections{"defaultshape"sv,      "baserace"sv,
						"multiracial_table"sv, "unselectable_races_table"sv,
						"petra_face_table"sv,  "usecode_info"sv};
	std::array parsers = make_unique_array<Shapeinfo_entry_parser>(
			std::make_unique<Def_av_shape_parser>(avdata->def_av_info),
			std::make_unique<Base_av_race_parser>(avdata->base_av_info),
			std::make_unique<Multiracial_parser>(
					avdata->skins_table, data->skinvars),
			std::make_unique<Bool_parser>(avdata->unselectable_skins),
			std::make_unique<Int_pair_parser>(avdata->petra_table),
			std::make_unique<Avatar_usecode_parser>(avdata->usecode_funs));
	static_assert(sections.size() == parsers.size());
	Read_data_file("avatar_data", sections, parsers);
}

vector<pair<string, int>>* Shapeinfo_lookup::GetPaperdollSources() {
	setup_shape_files();
	return &data->paperdoll_source_table;
}

vector<pair<int, int>>* Shapeinfo_lookup::GetImportedSkins() {
	setup_shape_files();
	return &data->imported_skin_shapes;
}

bool Shapeinfo_lookup::IsSkinImported(int shape) {
	setup_shape_files();
	for (auto& elem : data->imported_skin_shapes) {
		if (elem.first == shape) {
			return true;
		}
	}
	return false;
}

vector<pair<int, int>>* Shapeinfo_lookup::GetImportedGumpShapes() {
	setup_shape_files();
	return &data->imported_gump_shapes;
}

int Shapeinfo_lookup::GetBlueShapeData(int spot) {
	setup_shape_files();
	for (auto& elem : data->blue_shapes) {
		if (elem.first == -1 || elem.first == spot) {
			return elem.second;
		}
	}
	return 54;
}

Base_Avatar_info* Shapeinfo_lookup::GetBaseAvInfo(bool sex) {
	setup_avatar_data();
	auto it = avdata->def_av_info.find(sex);
	if (it != avdata->def_av_info.end()) {
		return &(it->second);
	}
	return nullptr;
}

int Shapeinfo_lookup::get_skinvar(const string& key) {
	setup_shape_files();
	auto it = data->skinvars.find(key);
	if (it != data->skinvars.end()) {
		return it->second;    // The shape #.
	}
	return -1;    // Invalid reference; bail out.
}

int Shapeinfo_lookup::GetMaleAvShape() {
	setup_avatar_data();
	return avdata->def_av_info[false].shape_num;
}

int Shapeinfo_lookup::GetFemaleAvShape() {
	setup_avatar_data();
	return avdata->def_av_info[true].shape_num;
}

Avatar_default_skin* Shapeinfo_lookup::GetDefaultAvSkin() {
	setup_avatar_data();
	return &avdata->base_av_info;
}

vector<Skin_data>* Shapeinfo_lookup::GetSkinList() {
	setup_avatar_data();
	return &avdata->skins_table;
}

Skin_data* Shapeinfo_lookup::GetSkinData(int skin_id, bool is_female) {
	setup_avatar_data();
	for (auto& elem : avdata->skins_table) {
		if (elem.skin_id == skin_id && elem.is_female == is_female) {
			return &elem;
		}
	}
	return nullptr;
}

bool Shapeinfo_lookup::IsSkinUnselectable(int skin_id) {
	setup_avatar_data();
	auto it = avdata->unselectable_skins.find(skin_id);
	if (it != avdata->unselectable_skins.end()) {
		return it->second;
	}
	return false;
}

int Shapeinfo_lookup::GetPetraFace(int shape_id) {
	setup_avatar_data();
	auto it = avdata->petra_table.find(shape_id);
	if (it != avdata->petra_table.end()) {
		return it->second;
	}
	return -1;
}

Usecode_function_data* Shapeinfo_lookup::GetAvatarUsecodeFun(int type) {
	setup_avatar_data();
	auto it = avdata->usecode_funs.find(type);
	if (it != avdata->usecode_funs.end()) {
		return &(it->second);
	}
	return nullptr;
}

int Shapeinfo_lookup::GetLastSkin() {
	setup_avatar_data();
	return Avatar_data::last_skin;
}


