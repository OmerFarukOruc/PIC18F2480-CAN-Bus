# Makefile for PIC18F2480 CAN Bus Project
# Requires MPLAB XC8 compiler

# Project configuration
PROJECT = canbus
CHIP = 18F2480
SRC_DIR = src
BUILD_DIR = build

# Compiler settings
CC = /opt/microchip/xc8/v3.00/bin/xc8-cc
CFLAGS = -mcpu=18F2480
CFLAGS += -I$(SRC_DIR)
CFLAGS += -I$(SRC_DIR)/app
CFLAGS += -I$(SRC_DIR)/drivers/can
CFLAGS += -I$(SRC_DIR)/drivers/timer
CFLAGS += -I$(SRC_DIR)/common
CFLAGS += -O1
CFLAGS += -std=c99

# Source files
SOURCES = $(SRC_DIR)/app/main.c \
          $(SRC_DIR)/app/data_manager.c \
          $(SRC_DIR)/drivers/can/can_driver.c \
          $(SRC_DIR)/drivers/timer/timer_handler.c

# Object files
OBJECTS = $(BUILD_DIR)/app/main.p1 \
          $(BUILD_DIR)/app/data_manager.p1 \
          $(BUILD_DIR)/drivers/can/can_driver.p1 \
          $(BUILD_DIR)/drivers/timer/timer_handler.p1

# Output files
HEX_FILE = $(BUILD_DIR)/$(PROJECT).hex
ELF_FILE = $(BUILD_DIR)/$(PROJECT).elf

# Default target
all: link_hex

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

link_hex: $(OBJECTS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(HEX_FILE) $(OBJECTS)

# Pattern rules for compiling C files into object files
$(BUILD_DIR)/app/%.p1: $(SRC_DIR)/app/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/drivers/can/%.p1: $(SRC_DIR)/drivers/can/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/drivers/timer/%.p1: $(SRC_DIR)/drivers/timer/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)/*
	rm -f *.sym *.lst *.map *.hxl *.obj *.sdb *.d

# Flash to microcontroller (requires PICkit2/3 or similar)
flash: $(HEX_FILE)
	pk2cmd -P $(CHIP) -F $(HEX_FILE) -M

# Alternative flash with IPE (MPLAB IPE)
flash-ipe: $(HEX_FILE)
	@echo "Flash $(HEX_FILE) to PIC$(CHIP) using MPLAB IPE"
	@echo "Load $(HEX_FILE) in MPLAB IPE and program the device"

# Build info
debug:
	@echo "Project: $(PROJECT)"
	@echo "Chip: PIC$(CHIP)"
	@echo "Sources: $(SOURCES)"
	@echo "Objects: $(OBJECTS)"
	@echo "Output: $(HEX_FILE)"
	@echo "Compiler: $(CC)"

# Help target
help:
	@echo "Available targets:"
	@echo "  all      - Build hex file (default)"
	@echo "  clean    - Remove build artifacts"
	@echo "  flash    - Flash hex file to microcontroller"
	@echo "  flash-ipe- Instructions for MPLAB IPE flashing"
	@echo "  info     - Show project information"
	@echo "  debug    - Show debug information"
	@echo "  help     - Show this help message"

# Check if XC8 compiler is available
check-compiler:
	@test -x $(CC) || (echo "Error: XC8 compiler not found at $(CC). Please install MPLAB XC8." && exit 1)
	@echo "XC8 compiler found: $(CC)"

# Build with compiler check
build_all: check-compiler all

.PHONY: all clean flash flash-ipe info help check-compiler build_all debug