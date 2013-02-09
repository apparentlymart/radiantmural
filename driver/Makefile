
CXX=g++
ALAMBRE_ROOT_DIR:=extlib/alambre
ALAMBRE_BUILD_ARGS:=
ALAMBRE_TARGET_DIR:=
CXX_OPTS:=
ifndef TARGET
	TARGET:=dummy
endif
ifeq ($(TARGET),arduino_uno)
	CXX=avr-g++
	ALAMBRE_BUILD_ARGS += GNU_TOOLS_PREFIX=avr- MCU=atmega328p
	ALAMBRE_TARGET := avr-atmega328p
else
	ifeq ($(TARGET),dummy)
		ALAMBRE_BUILD_ARGS += SDL_DUMMIES=1
		ALAMBRE_TARGET := $(shell $(CXX) -dumpmachine)
	else
		ERR:=$(error Unsupported target $(TARGET))
	endif
endif
OBJ_DIR:=obj/$(TARGET)
ALAMBRE_LIB=$(ALAMBRE_ROOT_DIR)/lib/$(ALAMBRE_TARGET)/libalambre.a
ALAMBRE_INCLUDE_DIR=$(ALAMBRE_ROOT_DIR)/include
CXX_OPTS += -I$(ALAMBRE_INCLUDE_DIR) -DRADIANTMURAL_TARGET=$(TARGET)
GCC_MACHINE := $(shell $(CXX) -dumpmachine)
TARGET_DIR := bin/$(TARGET)
TARGET_EXEC := $(TARGET_DIR)/radiantmural

SOURCE_FILES:=$(shell find src -type f -name '*.cpp')
HEADER_FILES:=$(shell find src -type f -name '*.h')
OBJ_FILES:=$(patsubst src/%.cpp,$(OBJ_DIR)/%.o,$(SOURCE_FILES))

all: $(TARGET_EXEC)

$(TARGET_EXEC): $(OBJ_FILES) $(ALAMBRE_LIB) $(TARGET_DIR)
	$(CXX) -O3 $(CXX_OPTS) $(OBJ_FILES) $(ALAMBRE_LIB) -o $(TARGET_EXEC)

$(OBJ_DIR)/%.o: src/%.cpp $(OBJ_DIR) $(HEADER_FILES)
	@mkdir -p $(@D)
	$(CXX) -I$(INCLUDE_DIR) -O3 $(CXX_OPTS) -c $< -o $@

$(OBJ_DIR):
	[ -d $(OBJ_DIR) ] || mkdir -p $(OBJ_DIR)

$(TARGET_DIR):
	[ -d $(TARGET_DIR) ] || mkdir -p $(TARGET_DIR)

extlib:
	@[ -d extlib ] || mkdir extlib

$(ALAMBRE_ROOT_DIR): extlib
	[ -d $(ALAMBRE_ROOT_DIR) ] || git clone git://github.com/apparentlymart/alambre $(ALAMBRE_ROOT_DIR)

$(ALAMBRE_INCLUDE_DIR): $(ALAMBRE_ROOT_DIR)

$(ALAMBRE_LIB): $(ALAMBRE_ROOT_DIR)
	cd $(ALAMBRE_ROOT_DIR) && make $(ALAMBRE_BUILD_ARGS)

show_config:
	@echo Building with $(CXX) $(CXX_OPTS)
	@echo Using Alambre lib at $(ALAMBRE_LIB)
	@echo Using Alambre headers at $(ALAMBRE_INCLUDE_DIR)
	@echo Building Alambre with $(ALAMBRE_BUILD_ARGS)
	@echo Will build object files to $(OBJ_DIR)
	@echo Will produce $(GCC_MACHINE) executable at $(TARGET_EXEC)
	@echo Source files are $(SOURCE_FILES)
	@echo Header files are $(HEADER_FILES)
	@echo Object files are $(OBJ_FILES)

clean:
	[ -d $(ALAMBRE_ROOT_DIR) ] && rm -rf $(ALAMBRE_ROOT_DIR)
	[ -d $(TARGET_DIR) ] && rm -rf $(TARGET_DIR)
	[ -d $(OBJ_DIR) ] && rm -rf $(OBJ_DIR)
