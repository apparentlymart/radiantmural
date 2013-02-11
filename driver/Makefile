
CXX=g++
ifndef ALAMBRE_GIT_REPO
	ALAMBRE_GIT_REPO=git://github.com/apparentlymart/alambre
endif
ALAMBRE_ROOT_DIR:=extlib/alambre
ALAMBRE_BUILD_ARGS:=
ALAMBRE_TARGET_DIR:=
CXX_OPTS:=-std=gnu++0x
LINK_OPTS:=
ifndef TARGET
	TARGET:=dummy
endif
ifeq ($(TARGET),arduino_uno)
	CXX=avr-g++
	ALAMBRE_BUILD_ARGS += GNU_TOOLS_PREFIX=avr- MCU=atmega328p
	ALAMBRE_TARGET := avr-atmega328p
	UPLOAD_MECHANISM := avrdude
	CXX_OPTS += -mmcu=atmega328p
else
	ifeq ($(TARGET),dummy)
		ALAMBRE_BUILD_ARGS += SDL_DUMMIES=1
		ALAMBRE_TARGET := $(shell $(CXX) -dumpmachine)
	    CXX_OPTS+=$(shell sdl-config --cflags)
	    LINK_OPTS+=$(shell sdl-config --libs)
	else
		ERR:=$(error Unsupported target $(TARGET))
	endif
endif
OBJ_DIR:=obj/$(TARGET)
ALAMBRE_LIB=$(ALAMBRE_ROOT_DIR)/lib/$(ALAMBRE_TARGET)/libalambre.a
ALAMBRE_INCLUDE_DIR=$(ALAMBRE_ROOT_DIR)/include
CXX_OPTS += -I$(ALAMBRE_INCLUDE_DIR) -DRADIANTMURAL_TARGET_$(TARGET) -DRADIANTMURAL_TARGET=$(TARGET)
GCC_MACHINE := $(shell $(CXX) -dumpmachine)
TARGET_DIR := bin/$(TARGET)
TARGET_EXEC := $(TARGET_DIR)/radiantmural
TARGET_HEX := $(TARGET_DIR)/radiantmural.hex

SOURCE_FILES:=$(shell find src -type f -name '*.cpp')
HEADER_FILES:=$(shell find src -type f -name '*.h')
OBJ_FILES:=$(patsubst src/%.cpp,$(OBJ_DIR)/%.o,$(SOURCE_FILES))

all: $(TARGET_EXEC)

$(TARGET_EXEC): $(OBJ_FILES) $(ALAMBRE_LIB) $(TARGET_DIR)
	$(CXX) -O3 $(CXX_OPTS) $(OBJ_FILES) $(ALAMBRE_LIB) $(LINK_OPTS) -o $(TARGET_EXEC)

$(OBJ_DIR)/%.o: src/%.cpp $(ALAMBRE_INCLUDE_DIR) $(OBJ_DIR) $(HEADER_FILES)
	@mkdir -p $(@D)
	$(CXX) -I$(INCLUDE_DIR) -O3 $(CXX_OPTS) -c $< -o $@

$(OBJ_DIR):
	[ -d $(OBJ_DIR) ] || mkdir -p $(OBJ_DIR)

$(TARGET_DIR):
	[ -d $(TARGET_DIR) ] || mkdir -p $(TARGET_DIR)

extlib:
	@[ -d extlib ] || mkdir extlib

$(ALAMBRE_ROOT_DIR): extlib
	[ -d $(ALAMBRE_ROOT_DIR) ] || git clone $(ALAMBRE_GIT_REPO) $(ALAMBRE_ROOT_DIR)

$(ALAMBRE_INCLUDE_DIR): $(ALAMBRE_ROOT_DIR)

$(ALAMBRE_LIB): $(ALAMBRE_ROOT_DIR) $(ALAMBRE_ROOT_DIR)/.git/HEAD
	cd $(ALAMBRE_ROOT_DIR) && make $(ALAMBRE_BUILD_ARGS)

show_config:
	@echo -------------------------------------
	@echo Building for target \"$(TARGET)\"
	@echo Building with $(CXX) $(CXX_OPTS)
	@echo Using Alambre lib at $(ALAMBRE_LIB)
	@echo Using Alambre headers at $(ALAMBRE_INCLUDE_DIR)
	@echo Building Alambre with $(ALAMBRE_BUILD_ARGS)
	@echo Will build object files to $(OBJ_DIR)
	@echo Will produce $(GCC_MACHINE) executable at $(TARGET_EXEC)
	@echo Will compile source files $(SOURCE_FILES)
	@echo Depending on header files $(HEADER_FILES)
	@echo Will produce object files $(OBJ_FILES)
ifdef UPLOAD_MECHANISM
	@echo Will use the \"$(UPLOAD_MECHANISM)\" upload mechanism
else
	@echo No upload mechanism for this target
endif
	@echo -------------------------------------

clean:
	[ -d $(ALAMBRE_ROOT_DIR) ] && rm -rf $(ALAMBRE_ROOT_DIR)
	[ -d $(TARGET_DIR) ] && rm -rf $(TARGET_DIR)
	[ -d $(OBJ_DIR) ] && rm -rf $(OBJ_DIR)

upload: $(TARGET_EXEC)
ifeq ($(UPLOAD_MECHANISM),avrdude)
ifdef AVRDUDE_SERIAL_PORT
	avr-objcopy -O ihex -R .eeprom $(TARGET_EXEC) $(TARGET_HEX)
	avrdude -F -V -c arduino -p ATMEGA328P -P $(AVRDUDE_SERIAL_PORT) -b 115200 -U flash:w:$(TARGET_HEX)
else
	@echo Must set AVRDUDE_SERIAL_PORT
	@exit 1
endif
else
	@echo No upload mechanism for target \"$(TARGET)\"
	@exit 1
endif