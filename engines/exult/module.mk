MODULE := engines/exult

# Object files for the ScummVM engine wrapper and adapters
WRAPPER_OBJS := \
	exult_engine.o \
	exult_graphics_adapter.o \
	exult_input_adapter.o \
	exult_audio_adapter.o \
	exult_file_adapter.o \
	exult_keymap.o

# Object files for Exult core
# This list needs to be comprehensive and accurate based on Exult_Engine_s build requirements.
# These are derived from the .cc files in exult_core_src/ and its subdirectories.
# Note: ScummVM_Engine_s build system typically lists .o files directly.
# The actual .cc to .o mapping will be handled by the build rules.

EXULT_CORE_OBJS := \
	exult_core_src/actions.o \
	exult_core_src/actorio.o \
	exult_core_src/actors.o \
	exult_core_src/args.o \
	exult_core_src/browser.o \
	exult_core_src/cheat.o \
	exult_core_src/cheat_screen.o \
	exult_core_src/combat.o \
	exult_core_src/dir.o \
	exult_core_src/drag.o \
	exult_core_src/effects.o \
	exult_core_src/exult.o \
	exult_core_src/exultmenu.o \
	exult_core_src/game.o \
	exult_core_src/gameclk.o \
	exult_core_src/gamedat.o \
	exult_core_src/gamemap.o \
	exult_core_src/gamerend.o \
	exult_core_src/gamewin.o \
	exult_core_src/istring.o \
	exult_core_src/keyactions.o \
	exult_core_src/keys.o \
	exult_core_src/menulist.o \
	exult_core_src/monsters.o \
	exult_core_src/mouse.o \
	exult_core_src/npcnear.o \
	exult_core_src/npctime.o \
	exult_core_src/palette.o \
	exult_core_src/party.o \
	exult_core_src/paths.o \
	exult_core_src/readnpcs.o \
	exult_core_src/schedule.o \
	exult_core_src/shapeid.o \
	exult_core_src/touchui.o \
	exult_core_src/tqueue.o \
	exult_core_src/txtscroll.o \
	exult_core_src/verify.o \
	exult_core_src/version.o \
	exult_core_src/windrag.o \
	$(addprefix exult_core_src/audio/, \
		music.o \
		sfx.o \
		sfxsched.o \
		midi.o \
		midi_drivers.o \
		mitchell.o \
		roland.o \
		soundbl.o \
		speaker.o \
		ultrasnd.o \
		wave.o \
	) \
	$(addprefix exult_core_src/desktop/, \
		desktop.o \
	) \
	$(addprefix exult_core_src/files/, \
		archive.o \
		config.o \
		file_utils.o \
		gamefile.o \
		listfiles.o \
		msgfile.o \
		pack.o \
		packtype.o \
		segfile.o \
		shp_decode.o \
		shp_resources.o \
		utils.o \
	) \
	$(addprefix exult_core_src/flic/, \
		flic.o \
		flicplay.o \
	) \
	$(addprefix exult_core_src/gamemgr/, \
		avatar.o \
		chests.o \
		combat_ai.o \
		eggs.o \
		gamemgr.o \
		itemgate.o \
		loadsave.o \
		pathfinder_queue.o \
		polygons.o \
		projectiles.o \
		spellbook.o \
		stats.o \
		usecode_manager.o \
		virtualsmallmap.o \
	) \
	$(addprefix exult_core_src/gumps/, \
		anim_gump.o \
		armorgump.o \
		bookgump.o \
		button_gump.o \
		cheatgump.o \
		combatgump.o \
		filebrowsergump.o \
		gameoptgump.o \
		gump.o \
		gump_choices.o \
		gump_utils.o \
		gumps_manager.o \
		listgump.o \
		mapgump.o \
		menugump.o \
		messagegump.o \
		modal_gump.o \
		newgamegump.o \
		paperdollgump.o \
		scrollgump.o \
		shapegump.o \
		spellsgump.o \
		statsgump.o \
		textgump.o \
		textwidgets.o \
		titlegump.o \
		tracked_gump.o \
		usecodegump.o \
		volumegump.o \
		weapongump.o \
		yesnogump.o \
	) \
	$(addprefix exult_core_src/imagewin/, \
		curses_win.o \
		cursfade.o \
		font.o \
		funfont.o \
		gamma.o \
		ibuf8.o \
		imagebuf.o \
		imagewin.o \
		mainactor.o \
		map_scene.o \
		scene.o \
		scrollmap.o \
		smallmap.o \
		video.o \
		wlist.o \
	) \
	$(addprefix exult_core_src/objs/, \
		actorflags.o \
		cont_ireg.o \
		conv_npc.o \
		egg.o \
		flags.o \
		font_ireg.o \
		gameobj.o \
		item.o \
		obj_names.o \
		objlist.o \
		objregs.o \
		objseq.o \
		qitem.o \
		readdat.o \
		region.o \
		shpreg.o \
		smlgame.o \
		smlitem.o \
		smlnpc.o \
		smlshp.o \
		spell.o \
		terrain.o \
		ucobj.o \
		usecode_obj.o \
	) \
	$(addprefix exult_core_src/pathfinder/, \
		a_star.o \
		pathfinder.o \
	) \
	$(addprefix exult_core_src/shapes/, \
		font_render.o \
		shape256.o \
		shapevga.o \
		shpread.o \
		shpscale.o \
		shptrans.o \
	) \
	$(addprefix exult_core_src/usecode/, \
		conversation.o \
		debugserver.o \
		intrinsics.o \
		keyring.o \
		stackframe.o \
		ucdebugging.o \
		ucdisasm.o \
		ucfunction.o \
		ucinternal.o \
		ucmachine.o \
		ucsched.o \
		ucserial.o \
		ucsymtbl.o \
		useval.o \
	)

MODULE_OBJS := $(WRAPPER_OBJS) $(EXULT_CORE_OBJS)

# This module can be built as a plugin
ifeq ($(ENABLE_EXULT), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk

# Detection objects
# exult_engine.o contains the ExultMetaEngine with detection logic.
DETECT_OBJS += $(MODULE)/exult_engine.o

# Skip building the following objects if a static
# module is enabled, because it already has the contents.
# This is similar to how the queen engine handles its version.o for detection.
# If exult_engine.o is always needed for detection, this might not be necessary
# or might need to list other specific files if they are conditionally compiled.
# For now, we assume exult_engine.o is sufficient and always built.
# ifneq ($(ENABLE_EXULT), STATIC_PLUGIN)
# # External dependencies for detection (if any, beyond exult_engine.o)
# DETECT_OBJS += $(MODULE)/some_other_detection_specific_file.o
# endif

