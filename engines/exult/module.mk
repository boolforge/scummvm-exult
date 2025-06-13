## Module: exult

EXULT_ENGINE_SRCS = \
	exult_engine.cpp \
	metaengine.cpp

EXULT_ENGINE_HDRS = \
	exult_engine.h

EXULT_ENGINE_OBJS = $(addprefix $(OBJ_DIR)/engines/exult/, $(EXULT_ENGINE_SRCS:.cpp=.o))

$(OBJ_DIR)/engines/exult/%.o: engines/exult/%.cpp
	$(COMPILE) -c -o $@ $<

# Exult core source files (will be copied here)
EXULT_CORE_SRCS = \
	$(wildcard engines/exult/exult_core_src/**/*.cc)

EXULT_CORE_OBJS = $(addprefix $(OBJ_DIR)/engines/exult/exult_core_src/, $(EXULT_CORE_SRCS:.cc=.o))

$(OBJ_DIR)/engines/exult/exult_core_src/%.o: engines/exult/exult_core_src/%.cc
	$(COMPILE) -c -o $@ $<

# Combine all objects for the Exult engine
EXULT_OBJS = $(EXULT_ENGINE_OBJS) $(EXULT_CORE_OBJS)

# Add Exult engine to the list of engines to build
ENGINES += exult

# Define the Exult engine library
$(LIB_DIR)/libexult.a: $(EXULT_OBJS)
	$(AR) rcs $@ $(EXULT_OBJS)

# Clean rules for Exult engine
CLEAN_OBJS += $(EXULT_OBJS)
CLEAN_LIBS += $(LIB_DIR)/libexult.a


