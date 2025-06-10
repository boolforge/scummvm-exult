# This file is included by the global makefile so that you can add your own
# building rules.
#
# The makefile expects to be invoked as:
# make -f engines/engine-name/module.mk
# This will ensure that the correct global definitions are in place.

MODULE := engines/ack

MODULE_OBJS := \
	ack.o \
	console.o \
	detection.o \
	graphics.o \
	graphics_renderer.o \
	input.o \
	metaengine.o \
	picture.o \
	resource.o \
	script.o \
	sound.o \
	sound_decoder.o \
	sprite.o

MODULE_DIRS += \
	engines/ack

# This module can be built as a plugin
ifeq ($(ENABLE_ACK), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk
