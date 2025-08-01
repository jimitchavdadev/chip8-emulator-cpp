#!/bin/bash

# A simple script to compile and run the emulator.
# Usage: ./run.sh roms/MY_ROM_FILE

# Default ROM if none is provided
DEFAULT_ROM="roms/PONG"
ROM_FILE=${1:-$DEFAULT_ROM}

# Check if the ROM file exists
if [ ! -f "$ROM_FILE" ]; then
    echo "Error: ROM file not found at '$ROM_FILE'"
    echo "Usage: $0 [path_to_rom]"
    exit 1
fi

# Compile the project using the Makefile
echo "🛠️ Compiling project..."
make

# Check if compilation was successful
if [ $? -ne 0 ]; then
    echo "❌ Compilation failed."
    exit 1
fi

# Run the emulator with the specified ROM
echo "▶️ Running emulator with ROM: $ROM_FILE"
./chip8_emulator "$ROM_FILE"