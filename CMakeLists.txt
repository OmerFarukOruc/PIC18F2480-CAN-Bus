# CMakeLists.txt for PIC18F2480 CAN Bus Project
# Requires MPLAB XC8 compiler

cmake_minimum_required(VERSION 3.10)

# Project configuration
project(canbus C)

# Set chip type
set(CHIP "18F2480")

# Find XC8 compiler
find_program(XC8_COMPILER xc8-cc PATHS /opt/microchip/xc8/v3.00/bin)
if(NOT XC8_COMPILER)
    set(XC8_COMPILER "/opt/microchip/xc8/v3.00/bin/xc8-cc")
    if(NOT EXISTS ${XC8_COMPILER})
        message(FATAL_ERROR "XC8 compiler not found. Please install MPLAB XC8.")
    endif()
endif()

# Set compiler
set(CMAKE_C_COMPILER ${XC8_COMPILER})

# Compiler flags
set(CMAKE_C_FLAGS "-mcpu=${CHIP}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O1")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

# Include directories
include_directories(src)

# Source files
file(GLOB SOURCES "src/*.c")

# Output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

# Create hex file target
add_custom_target(hex
    COMMAND ${XC8_COMPILER} -mcpu=${CHIP} -O1 -std=c99 -o ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/canbus.hex ${SOURCES}
    DEPENDS ${SOURCES}
    COMMENT "Building hex file for PIC${CHIP}"
    VERBATIM
)

# Default target
add_custom_target(all ALL
    DEPENDS hex
)

# Clean target
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E remove -f *.sym *.lst *.map *.hxl *.obj *.sdb *.d
    COMMENT "Cleaning build artifacts"
)

# Flash target
add_custom_target(flash
    COMMAND pk2cmd -P ${CHIP} -F ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/canbus.hex -M
    DEPENDS hex
    COMMENT "Flashing hex file to PIC${CHIP}"
)

# Info target
add_custom_target(info
    COMMAND ${CMAKE_COMMAND} -E echo "Project: canbus"
    COMMAND ${CMAKE_COMMAND} -E echo "Chip: PIC${CHIP}"
    COMMAND ${CMAKE_COMMAND} -E echo "Compiler: ${XC8_COMPILER}"
    COMMAND ${CMAKE_COMMAND} -E echo "Sources: ${SOURCES}"
    COMMAND ${CMAKE_COMMAND} -E echo "Output: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/canbus.hex"
    COMMENT "Project information"
)

# Help target
add_custom_target(help
    COMMAND ${CMAKE_COMMAND} -E echo "Available targets:"
    COMMAND ${CMAKE_COMMAND} -E echo "  hex      - Build hex file"
    COMMAND ${CMAKE_COMMAND} -E echo "  all      - Build all (default)"
    COMMAND ${CMAKE_COMMAND} -E echo "  clean-all- Clean build artifacts"
    COMMAND ${CMAKE_COMMAND} -E echo "  flash    - Flash to microcontroller"
    COMMAND ${CMAKE_COMMAND} -E echo "  info     - Show project information"
    COMMAND ${CMAKE_COMMAND} -E echo "  help     - Show this help"
    COMMENT "Available targets"
)