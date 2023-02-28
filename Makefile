# $^ names of all prerequisites
# $< name of first prerequisite
# $@ name of target of the rule

# ** Build for Arduino NANO **
# > make build MCU=NANO
# > make flash COMPORT=COM5

# ** Build for Arduino UNO **
# > make build MCU=UNO
# > make flash COMPORT=COM5

# ** Clean Build ** 
# > make clean


COMPORT ?= COM8
MCU ?= NANO

PROJDIR := $(realpath $(CURDIR)/..)
MAIN_APP = DuckHunter
MAIN_HEX = $(BUILD_DIR)/$(MAIN_APP).hex
MAIN_ELF = $(BUILD_DIR)/$(MAIN_APP).elf
ATMEGA328P_LIB = $(BUILD_DIR)/libs/libatmega328p.a
MMCU_TYPE = atmega328p
BAUDRATE ?= 57600


ifeq ($(MCU), NANO)
	ADC_CHANNEL = -DADC_CHANNEL=7
	BAUDRATE = 57600
endif

ifeq ($(MCU), UNO)
	ADC_CHANNEL = -DADC_CHANNEL=0
	BAUDRATE ?= 115200
endif

ifeq ($(MCU), MINI)
	ADC_CHANNEL = -DADC_CHANNEL=7
	BAUDRATE = 57600
endif


CXX = avr-g++
CC = avr-gcc
LINKER = avr-g++
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
AVRDUDE = avrdude
AVR_SIZE = avr-size

# Enable automatic removal of unused code
FLAGS = -ffunction-sections -fdata-sections -flto

# Options for avr-g++
CPPFLAGS = -c -Os -fno-exceptions -fshort-enums -std=gnu++14 
CPPFLAGS += $(ADC_CHANNEL) -D__AVR_ATmega328P__ -DF_CPU=16000000L
CPPFLAGS += -Wall
CPPFLAGS += -Wextra -Wpedantic
CPPFLAGS += -Werror
CPPFLAGS += -fmessage-length=0 -Wno-sign-compare -Wno-unused-parameter 
CPPFLAGS += -Wno-unused-variable -Wno-unused-but-set-variable
CPPFLAGS += -mmcu=$(MMCU_TYPE) -o

# ---------------- Library Options ----------------
PRINTF_LIB_MIN = -Wl,-u,vfprintf -lprintf_min
PRINTF_LIB_FLOAT = -Wl,-u,vfprintf -lprintf_flt -lm
# If this is left blank, then it will use the Standard printf version.
PRINTF_LIB = 
# PRINTF_LIB = $(PRINTF_LIB_MIN)
# PRINTF_LIB = $(PRINTF_LIB_FLOAT)

# Linking options for avr-gcc
LFLAGS = -Os -flto -fshort-enums
LFLAGS += -Wl,--gc-sections -Wl,-Map=$(BUILD_DIR)/$(MAIN_APP).map
LFLAGS += -mmcu=$(MMCU_TYPE)
LFLAGS += $(PRINTF_LIB)

# Options for HEX file generation
HFLAGS = -j .text -j .data -R .eeprom -O ihex

# Options for avrdude to burn the hex file
AVRDUDE_VERBOSE = -v -v
AVRDUDE_NO_VERIFY = -V
DUDEFLAGS += -c arduino
DUDEFLAGS += -p m328p -P $(COMPORT)
DUDEFLAGS += $(AVRDUDE_VERBOSE) $(AVRDUDE_NO_VERIFY)
DUDEFLAGS += -b $(BAUDRATE)

# Add libraries
LIB_BASE_DIR = libs
LIB_DIRS = $(LIB_BASE_DIR)/I2C $(LIB_BASE_DIR)/LCD_I2C_Driver $(LIB_BASE_DIR)/Timer
LIB_DIRS += $(LIB_BASE_DIR)/TM1637 $(LIB_BASE_DIR)/ShiftRegister
LIB_DIRS += $(LIB_BASE_DIR)/UART $(LIB_BASE_DIR)/ADC
LIB_INCLUDES =  $(foreach dir,$(LIB_DIRS),$(addprefix -I, $(dir)))

INCLUDE_DIRS = include include/Components
SOURCE_DIRS = src src/Components 
SOURCE_DIR = src
BUILD_DIR = build

INCLUDES =  $(foreach dir,$(INCLUDE_DIRS),$(addprefix -I, $(dir)))

BUILD_DIRS = $(subst $(SOURCE_DIR),$(BUILD_DIR)/$(SOURCE_DIR),$(SOURCE_DIRS))
BUILD_DIRS += $(subst $(LIB_BASE_DIR),$(BUILD_DIR)/$(LIB_BASE_DIR),$(LIB_DIRS))

# Source files needed for building the application
SRC_FILES = $(foreach dir,$(SOURCE_DIRS),$(wildcard $(dir)/*.cpp))
OBJ_FILES = $(subst $(SOURCE_DIR),$(BUILD_DIR)/$(SOURCE_DIR),$(SRC_FILES:.cpp=.o))

LIB_SRC_FILES = $(foreach dir,$(LIB_DIRS),$(wildcard $(dir)/*.cpp))
LIB_OBJ_FILES =  $(subst $(LIB_BASE_DIR),$(BUILD_DIR)/$(LIB_BASE_DIR),$(LIB_SRC_FILES:.cpp=.o))


all: INFOTEXT build flash

build: INFOTEXT $(MAIN_ELF) list

flash: $(MAIN_HEX)
	@echo flashing $@ file
	$(AVRDUDE) $(DUDEFLAGS) -D -U flash:w:$(MAIN_HEX):i

list: $(MAIN_ELF)
	$(OBJDUMP) -h -S -z $< > $(BUILD_DIR)/list.lss
	$(OBJDUMP) -h $< > $(BUILD_DIR)/sections_summary.txt
	$(OBJDUMP) -D $< > $(BUILD_DIR)/disassembly_all.d
#   $(OBJDUMP) -d $< > $(BUILD_DIR)/disassembly_text_section.d

lib: $(ATMEGA328P_LIB)
	@echo Testing Library $<
	avr-ar -t $<

$(ATMEGA328P_LIB): $(LIB_OBJ_FILES)
	@echo Building... $@
	avr-ar rcs $(ATMEGA328P_LIB) $(LIB_OBJ_FILES)

# Rule for Building .hex flashable file
$(MAIN_HEX): $(MAIN_ELF)
	@echo Generating flashable $(MAIN_HEX) file
	$(OBJCOPY) $(HFLAGS) $< $(MAIN_HEX)

# Rule for Building .elf file
$(MAIN_ELF): $(OBJ_FILES) $(ATMEGA328P_LIB)
	@echo Linking $@ file
	$(LINKER) $(LFLAGS) $^ --output $@
	$(AVR_SIZE) -A $(MAIN_ELF)

# Rule for Compiling .cpp files with header file
$(BUILD_DIR)/%.o: %.cpp %.h
	@echo Building... $@ from $^
	$(CXX) $(INCLUDES) $(LIB_INCLUDES) $(FLAGS) $(CPPFLAGS) $@ $<

# Rule for Compiling .cpp files without header file
$(BUILD_DIR)/%.o: %.cpp
	@echo Building... $@ from $^
	$(CXX) $(INCLUDES) $(LIB_INCLUDES) $(FLAGS) $(CPPFLAGS) $@ $<


.PHONY: clean INFOTEXT

# Create Build Folders
$(foreach dir, $(BUILD_DIRS), $(shell mkdir -p $(dir) 2>/dev/null))

clean:
	rm -rf $(BUILD_DIR)/*.elf
	rm -rf $(BUILD_DIR)/*.hex
	rm -rf $(OBJ_FILES)
	rm -rf $(LIB_OBJ_FILES)
	rm -rf $(BUILD_DIR)/.dep/*
	rm -rf $(BUILD_DIR)/libs/*.a
	rm -rf $(BUILD_DIR)


INFOTEXT:
	@echo ====== BOARD: $(MCU) =======
	@echo ProjectDir:: $(PROJDIR)
	@echo CURRENT PROJECT: $(CURDIR)
	@echo Source Files: $(SRC_FILES)
	@echo Object Files: $(OBJ_FILES)
	@echo Lib Object Files: $(LIB_OBJ_FILES)
	@echo Build Dirs: $(BUILD_DIRS)
	@echo Include Folders: $(INCLUDES)
