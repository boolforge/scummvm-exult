/*
 *  shapevga.cc - Handle the 'shapes.vga' file and associated info.
 *
 *  Copyright (C) 1999  Jeffrey S. Freedman
 *  Copyright (C) 2000-2022  The Exult Team
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

#include "shapevga.h"

#include "ammoinf.h"
#include "aniinf.h"
#include "armorinf.h"
#include "bodyinf.h"
#include "continf.h"
#include "data/exult_bg_flx.h"
#include "data/exult_si_flx.h"
#include "data_utils.h"
#include "effhpinf.h"
#include "exceptions.h"
#include "expinf.h"
#include "frflags.h"
#include "frnameinf.h"
#include "frusefun.h"
#include "ignore_unused_variable_warning.h"
#include "lightinf.h"
#include "monstinf.h"
#include "npcdollinf.h"
#include "objdollinf.h"
#include "ready.h"
#include "sfxinf.h"
#include "utils.h"
#include "warminf.h"
#include "weaponinf.h"

// TODO: Include ScummVM headers for SeekableReadStream and ExultFileAdapter
// #include "common/streams.h"
// #include "ExultEngine.h" // Assuming ExultEngine provides access to ExultFileAdapter

using std::ifstream;
using std::ios;
using namespace std;

// Special case ID reader functors.

extern int get_skinvar(const std::string& key);

// Multiracial support in Exult.
class Paperdoll_npc_ID_reader {
public:
	int operator()(std::istream& in, int index, int version, bool binary) {
		ignore_unused_variable_warning(index, version, binary);
		if (in.peek() == '%') {
			const std::string key = ReadStr(in);
			// We need these for Exult, but not for ES.
			// For now, a compromise/hack in that ES defines
			// a version of this function which always returns
			// -1, while Exult has another which forwards to
			// Shapeinfo_lookup::get_skinvar
			const int id = get_skinvar(key);
			return id < 0 ? -1 : id;
		}
		return ReadInt(in);
	}
};

// For backward compatibility.
class Body_ID_reader {
public:
	int operator()(std::istream& in, int index, int version, bool binary) {
		ignore_unused_variable_warning(binary);
		return version == 1 ? index : ReadInt(in);
	}
};

// Special case reader functor.
class Gump_reader_functor {
public:
	bool operator()(
			std::istream& in, int version, bool patch, Exult_Game game,
			Shape_info& info) {
		ignore_unused_variable_warning(patch, game);
		info.gump_shape = little_endian::Read2(in);
		if (version >= 2) {
			info.gump_font = little_endian::Read2(in);
		} else {
			info.gump_font = -1;
		}
		return true;
	}
};

// A few custom post-read functors.
class Ready_type_functor {
	Patch_flags_functor<ready_type_flag, Shape_info> setflags;

public:
	void operator()(
			std::istream& in, int version, bool patch, Exult_Game game,
			Shape_info& info) {
		unsigned char ready = info.ready_type;
		info.spell_flag     = (ready & 1) != 0;
		ready >>= 3;
		const unsigned char spot = game == BLACK_GATE
										   ? Ready_spot_from_BG(ready)
										   : Ready_spot_from_SI(ready);
		info.ready_type          = spot;
		// Init alternate spots.
		switch (spot) {
		case lfinger:
			info.alt_ready1 = rfinger;
			break;
		case lhand:
			info.alt_ready1 = rhand;
			info.alt_ready2 = belt;
			break;
		case both_hands:
			info.alt_ready1 = back_2h;
			break;
		default:
			break;
		}
		setflags(in, version, patch, game, info);
	}
};

class Actor_flags_functor {
	Patch_flags_functor<actor_flags_flag, Shape_info> setflags;

public:
	void operator()(
			std::istream& in, int version, bool patch, Exult_Game game,
			Shape_info& info) {
		setflags(in, version, patch, game, info);
	}
};

class Paperdoll_npc_functor {
public:
	void operator()(
			std::istream& in, int version, bool patch, Exult_Game game,
			Shape_info& info) {
		ignore_unused_variable_warning(patch, game);
		if (version < 3) {
			// We need this for backward compatibility.
			// We use the setter methods so that the info
			// will get saved by ES if that is needed.
			info.set_gump_data(ReadInt(in, -1), -1);
		}
	}
};

/*
 *  Reads text data file and parses it according to passed
 *  parser functions.
 */
void Read_text_data_file(
		// Name of file to read, sans extension
		const char* fname,
		// What to use to parse data.
		const tcb::span<std::unique_ptr<Base_reader>>& parsers,
		// The names of the sections
		const tcb::span<std::string_view>& sections, bool editing,
		Exult_Game game, int resource) {
	auto value_or = [](std::optional<int> opt) {
		return opt ? *opt : 1;
	};
	// TODO: Replace with ScummVM stream access via ExultFileAdapter
	// The final implementation will look something like:
	// ScummVM::Common::SeekableReadStream* static_stream = ExultEngine::getInstance()->getFileAdapter()->openFileForObject(file.c_str());
	// Text_msg_file_reader static_reader(static_stream);
	Text_msg_file_reader static_reader = [&]() {
		if (game == BLACK_GATE || game == SERPENT_ISLE) {
			/*  ++++ Not because of ES.
			snprintf(buf, sizeof(buf), "config/%s", fname);
			str_int_pair resource = game->get_resource(buf);
			U7object txtobj(resource.str, resource.num);
			*/
			const bool  bg = game == BLACK_GATE;
			const char* flexfile
					= bg ? BUNDLE_CHECK(BUNDLE_EXULT_BG_FLX, EXULT_BG_FLX)
						 : BUNDLE_CHECK(BUNDLE_EXULT_SI_FLX, EXULT_SI_FLX);
			IExultDataSource ds(flexfile, resource);
			return Text_msg_file_reader(ds);
		}
		std::string     file = "<STATIC>/" + std::string(fname) + ".txt";
		IFileDataSource ds(file, false);
		if (!ds.good()) {
			throw file_open_exception(file);
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
			if (!editing) {
				throw file_open_exception(file);
			}
			return Text_msg_file_reader();
		}
		return Text_msg_file_reader(ds);
	}();

	int static_version = value_or(static_reader.get_version());
	int patch_version  = value_or(patch_reader.get_version());
	std::vector<std::string> strings;

	for (size_t i = 0; i < sections.size(); i++) {
		static_reader.get_section_strings(sections[i], strings);
		parsers[i]->parse(strings, static_version, false, game);
		patch_reader.get_section_strings(sections[i], strings);
		parsers[i]->parse(strings, patch_version, true, game);
	}
}

void Shapes_vga_file::Read_Shapeinf_text_data_file(
		bool editing, Exult_Game game_type) {
	std::array sections{
			"explosions"sv,
			"shape_sfx"sv,
			"animation"sv,
			"usecode_events"sv,
			"mountain_tops"sv,
			"monster_food"sv,
			"actor_flags"sv,
			"effective_hps"sv,
			"lightweight_object"sv,
			"light_data"sv,
			"warmth_data"sv,
			"quantity_frames"sv,
			"locked_containers"sv,
			"content_rules"sv,
			"volatile_explosive"sv,
			"framenames"sv,
			"altready"sv,
			"barge_type"sv,
			"frame_powers"sv,
			"is_jawbone"sv,
			"is_mirror"sv,
			"on_fire"sv,
			"extradimensional_storage"sv,
			"field_type"sv,
			"frame_usecode"sv};
	// For explosions.
	using Explosion_reader = Functor_multidata_reader<
			Shape_info,
			Class_reader_functor<
					Explosion_info, Shape_info, &Shape_info::explosion>>;
	// For sound effects.
	using SFX_reader = Functor_multidata_reader<
			Shape_info,
			Class_reader_functor<SFX_info, Shape_info, &Shape_info::sfxinf>>;
	// For animations.
	using Animation_reader = Functor_multidata_reader<
			Shape_info,
			Class_reader_functor<
					Animation_info, Shape_info, &Shape_info::aniinf>>;
	// For usecode events.
	using Usecode_events_reader = Functor_multidata_reader<
			Shape_info,
			Bit_text_reader_functor<
					unsigned short, Shape_info, &Shape_info::shape_flags,
					Shape_info::usecode_events>,
			Patch_flags_functor<usecode_events_flag, Shape_info>>;
	// For mountain tops.
	using Mountain_top_reader = Functor_multidata_reader<
			Shape_info,
			Text_reader_functor<
					unsigned char, Shape_info, &Shape_info::mountain_top>,
			Patch_flags_functor<mountain_top_flag, Shape_info>>;
	// For monster food.
	using Monster_food_reader = Functor_multidata_reader<
			Shape_info,
			Text_reader_functor<short, Shape_info, &Shape_info::monster_food>,
			Patch_flags_functor<monster_food_flag, Shape_info>>;
	// For actor flags.
	using Actor_flags_reader = Functor_multidata_reader<
			Shape_info,
			Bit_field_text_reader_functor<
					unsigned char, Shape_info, &Shape_info::actor_flags>,
			Actor_flags_functor>;
	// For effective HPs.
	using Effective_hp_reader = Functor_multidata_reader<
			Shape_info,
			Vector_reader_functor<
					Effective_hp_info, Shape_info, &Shape_info::hpinf>>;
	// For lightweight objects.
	using Lightweight_reader = Functor_multidata_reader<
			Shape_info,
			Bit_text_reader_functor<
					unsigned short, Shape_info, &Shape_info::shape_flags,
					Shape_info::lightweight>,
			Patch_flags_functor<lightweight_flag, Shape_info>>;
	// For light data.
	using Light_data_reader = Functor_multidata_reader<
			Shape_info, Vector_reader_functor<
								Light_info, Shape_info, &Shape_info::lightinf>>;
	// For warmth data.
	using Warmth_data_reader = Functor_multidata_reader<
			Shape_info, Vector_reader_functor<
								Warmth_info, Shape_info, &Shape_info::warminf>>;
	// For quantity frames.
	using Quantity_frames_reader = Functor_multidata_reader<
			Shape_info,
			Bit_text_reader_functor<
					unsigned short, Shape_info, &Shape_info::shape_flags,
					Shape_info::quantity_frames>,
			Patch_flags_functor<quantity_frames_flag, Shape_info>>;
	// For locked objects.
	using Locked_containers_reader = Functor_multidata_reader<
			Shape_info,
			Bit_text_reader_functor<
					unsigned short, Shape_info, &Shape_info::shape_flags,
					Shape_info::locked>,
			Patch_flags_functor<locked_flag, Shape_info>>;
	// For content rules.
	using Content_rules_reader = Functor_multidata_reader<
			Shape_info,
			Vector_reader_functor<
					Content_rules, Shape_info, &Shape_info::cntrules>>;
	// For highly explosive objects.
	using Explosive_reader = Functor_multidata_reader<
			Shape_info,
			Bit_text_reader_functor<
					unsigned short, Shape_info, &Shape_info::shape_flags,
					Shape_info::is_volatile>,
			Patch_flags_functor<is_volatile_flag, Shape_info>>;
	// For frame names.
	using Frame_names_reader = Functor_multidata_reader<
			Shape_info,
			Vector_reader_functor<
					Frame_name_info, Shape_info, &Shape_info::nameinf>>;
	// For alternate ready spots.
	using Altready_reader = Functor_multidata_reader<
			Shape_info,
			Text_pair_reader_functor<
					unsigned char, Shape_info, &Shape_info::alt_ready1,
					&Shape_info::alt_ready2>,
			Patch_flags_functor<altready_type_flag, Shape_info>>;
	// For barge parts.
	using Barge_type_reader = Functor_multidata_reader<
			Shape_info,
			Text_reader_functor<
					unsigned char, Shape_info, &Shape_info::barge_type>,
			Patch_flags_functor<barge_type_flag, Shape_info>>;
	// For frame flags.
	using Frame_flags_reader = Functor_multidata_reader<
			Shape_info,
			Vector_reader_functor<
					Frame_flags_info, Shape_info, &Shape_info::frflagsinf>>;
	// For the jawbone.
	using Jawbone_reader = Functor_multidata_reader<
			Shape_info,
			Bit_text_reader_functor<
					unsigned short, Shape_info, &Shape_info::shape_flags,
					Shape_info::jawbone>,
			Patch_flags_functor<jawbone_flag, Shape_info>>;
	// Mirrors.
	using Mirror_reader = Functor_multidata_reader<
			Shape_info,
			Bit_text_reader_functor<
					unsigned short, Shape_info, &Shape_info::shape_flags,
					Shape_info::mirror>,
			Patch_flags_functor<mirror_flag, Shape_info>>;
	// Objects on fire.
	using On_fire_reader = Functor_multidata_reader<
			Shape_info,
			Bit_text_reader_functor<
					unsigned short, Shape_info, &Shape_info::shape_flags,
					Shape_info::on_fire>,
			Patch_flags_functor<on_fire_flag, Shape_info>>;
	// Containers with unlimited storage.
	using Extradimensional_storage_reader = Functor_multidata_reader<
			Shape_info,
			Bit_text_reader_functor<
					unsigned short, Shape_info, &Shape_info::shape_flags,
					Shape_info::extradimensional_storage>,
			Patch_flags_functor<extradimensional_storage_flag, Shape_info>>;
	// For field types.
	using Field_type_reader = Functor_multidata_reader<
			Shape_info,
			Text_reader_functor<
					signed char, Shape_info, &Shape_info::field_type>,
			Patch_flags_functor<field_type_flag, Shape_info>>;
	// For frame usecode.
	using Frame_usecode_reader = Functor_multidata_reader<
			Shape_info,
			Vector_reader_functor<
					Frame_usecode_info, Shape_info, &Shape_info::frucinf>>;

	std::array readers = make_unique_array<Base_reader>(
			std::make_unique<Explosion_reader>(info),
			std::make_unique<SFX_reader>(info),
			std::make_unique<Animation_reader>(info),
			std::make_unique<Usecode_events_reader>(info),
			std::make_unique<Mountain_top_reader>(info),
			std::make_unique<Monster_food_reader>(info),
			std::make_unique<Actor_flags_reader>(info),
			std::make_unique<Effective_hp_reader>(info),
			std::make_unique<Lightweight_reader>(info),
			std::make_unique<Light_data_reader>(info),
			std::make_unique<Warmth_data_reader>(info),
			std::make_unique<Quantity_frames_reader>(info),
			std::make_unique<Locked_containers_reader>(info),
			std::make_unique<Content_rules_reader>(info),
			std::make_unique<Explosive_reader>(info),
			std::make_unique<Frame_names_reader>(info),
			std::make_unique<Altready_reader>(info),
			std::make_unique<Barge_type_reader>(info),
			std::make_unique<Frame_flags_reader>(info),
			std::make_unique<Jawbone_reader>(info),
			std::make_unique<Mirror_reader>(info),
			std::make_unique<On_fire_reader>(info),
			std::make_unique<Extradimensional_storage_reader>(info),
			std::make_unique<Field_type_reader>(info),
			std::make_unique<Frame_usecode_reader>(info));
	static_assert(sections.size() == readers.size());
	const int flxres = game_type == BLACK_GATE ? EXULT_BG_FLX_SHAPE_INFO_TXT
											   : EXULT_SI_FLX_SHAPE_INFO_TXT;

	Read_text_data_file(
			"shape_info", readers, sections, editing, game_type, flxres);
}

void Shapes_vga_file::Read_Bodies_text_data_file(
		bool editing, Exult_Game game_type) {
	std::array sections{"bodyshapes"sv, "bodylist"sv};
	using Body_shape_reader = Functor_multidata_reader<
			Shape_info,
			Bit_text_reader_functor<
					unsigned short, Shape_info, &Shape_info::shape_flags,
					Shape_info::is_body>,
			Patch_flags_functor<is_body_flag, Shape_info>, Body_ID_reader>;
	using Body_list_reader = Functor_multidata_reader<
			Shape_info,
			Class_reader_functor<Body_info, Shape_info, &Shape_info::body>>;
	std::array readers = make_unique_array<Base_reader>(
			std::make_unique<Body_shape_reader>(info),
			std::make_unique<Body_list_reader>(info));
	static_assert(sections.size() == readers.size());
	const int flxres = game_type == BLACK_GATE ? EXULT_BG_FLX_BODIES_TXT
											   : EXULT_SI_FLX_BODIES_TXT;

	Read_text_data_file(
			"bodies", readers, sections, editing, game_type, flxres);
}

void Shapes_vga_file::Read_Paperdoll_text_data_file(
		bool editing, Exult_Game game_type) {
	std::array sections{"characters"sv, "items"sv};
	using NpcPaperdoll_reader_functor = Functor_multidata_reader<
			Shape_info,
			Class_reader_functor<
					Paperdoll_npc, Shape_info, &Shape_info::npcpaperdoll>,
			Paperdoll_npc_functor, Paperdoll_npc_ID_reader>;
	using ObjPaperdoll_reader_functor = Functor_multidata_reader<
			Shape_info,
			Vector_reader_functor<
					Paperdoll_item, Shape_info, &Shape_info::objpaperdoll>>;
	std::array readers = make_unique_array<Base_reader>(
			std::make_unique<NpcPaperdoll_reader_functor>(info),
			std::make_unique<ObjPaperdoll_reader_functor>(info));
	static_assert(sections.size() == readers.size());
	const int flxres = game_type == BLACK_GATE ? EXULT_BG_FLX_PAPERDOL_INFO_TXT
											   : EXULT_SI_FLX_PAPERDOL_INFO_TXT;

	Read_text_data_file(
			"paperdol_info", readers, sections, editing, game_type, flxres);
}

/*
 *  Reload static data for weapons, ammo and armor.
 */
void Shapes_vga_file::Reload_static_data() {
	std::array sections{
			"weapons"sv, "ammo"sv, "armor"sv, "containers"sv, "monsters"sv};
	using Weapon_reader = Functor_multidata_reader<
			Shape_info,
			Class_reader_functor<
					Weapon_info, Shape_info, &Shape_info::weaponinf>>;
	using Ammo_reader = Functor_multidata_reader<
			Shape_info,
			Class_reader_functor<Ammo_info, Shape_info, &Shape_info::ammoinf>>;
	using Armor_reader = Functor_multidata_reader<
			Shape_info,
			Class_reader_functor<
					Armor_info, Shape_info, &Shape_info::armorinf>>;
	using Container_reader = Functor_multidata_reader<
			Shape_info,
			Class_reader_functor<
					Container_info, Shape_info, &Shape_info::continf>>;
	using Monster_reader = Functor_multidata_reader<
			Shape_info,
			Class_reader_functor<
					Monster_info, Shape_info, &Shape_info::monstinf>>;
	std::array readers = make_unique_array<Base_reader>(
			std::make_unique<Weapon_reader>(info),
			std::make_unique<Ammo_reader>(info),
			std::make_unique<Armor_reader>(info),
			std::make_unique<Container_reader>(info),
			std::make_unique<Monster_reader>(info));
	static_assert(sections.size() == readers.size());
	Read_text_data_file("static_data", readers, sections, false, game_type, -1);
}


