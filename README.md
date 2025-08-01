# Chip-8 Emulator in C++

This project is a fully functional emulator for the Chip-8, a simple interpreted programming language used on microcomputers in the mid-1970s. It is written in modern C++ and uses the SDL2 library for cross-platform graphics, audio, and input handling.

The primary objective is to accurately emulate the Chip-8's architecture, including its instruction set, memory layout, registers, and timers, providing a platform to play classic Chip-8 games.

### ✨ Features

  * **Accurate Emulation**: Implements all 35 original Chip-8 opcodes for instruction decoding and execution.
  * **Graphics and Input**: Uses SDL2 to render the 64x32 monochrome display and handle keyboard input.
  * **ROM Loading**: A file loader to read Chip-8 game ROMs directly into the emulator's memory.
  * **Modular Design**: The code is separated into a core emulation engine (`Chip8`) and a platform layer (`Platform`), making it easy to manage and understand.


## 📂 Project Structure

The project is organized into distinct directories for headers, source code, and game ROMs.

```
.
├── include/        # C++ Header files (.hpp) defining the classes
│   ├── Chip8.hpp
│   └── Platform.hpp
├── src/            # C++ Source files (.cpp) with the implementation
│   ├── Chip8.cpp
│   ├── main.cpp
│   └── Platform.cpp
├── roms/           # Directory for Chip-8 game ROMs (e.g., PONG, INVADERS)
├── Makefile        # Build script for compiling the project
└── run.sh          # A convenience script to compile and run
```


## ⚙️ How It Works

The emulator's design separates the core logic from the platform-specific code.

1.  **Initialization**: The program starts in `main.cpp`. It creates an instance of the `Platform` class to set up an SDL window and renderer. It also creates an instance of the `Chip8` class, which initializes the virtual machine's state (registers, memory, etc.).

2.  **ROM Loading**: The ROM file provided as a command-line argument is loaded by the `Chip8::loadROM` method into the emulator's memory, starting at address `0x200`.

3.  **The Emulation Loop**: The `main` function contains a loop that drives the entire emulation process. In each iteration, it:

      * **Processes Input**: Calls `platform.processInput()` to check for keyboard events. Any key presses are mapped to the Chip-8's 16-key keypad and stored in the `chip8.keypad` array.
      * **Emulates a CPU Cycle**: Calls `chip8.emulateCycle()` at a fixed frequency (around 500Hz). This function performs one full **Fetch-Decode-Execute** cycle:
          * **Fetch**: Reads a 2-byte opcode from memory at the location of the Program Counter (`pc`).
          * **Decode**: The 2-byte opcode is parsed to determine which instruction to perform (e.g., jump, add, draw).
          * **Execute**: The corresponding operation is executed. This might change register values, write to memory, or modify the display buffer.
      * **Updates Graphics**: If a draw instruction was executed, a flag is set. The `main` loop checks this flag and calls `platform.update()` to push the Chip-8's display buffer to the SDL texture and render it to the screen.

This cycle repeats continuously, creating the illusion of a running Chip-8 computer.


## 🚀 Getting Started

### Prerequisites

You need a C++ compiler (`g++`), `make`, and the **SDL2 development library**.

  * **On Debian/Ubuntu**:
    ```bash
    sudo apt-get install build-essential libsdl2-dev
    ```
  * **On macOS (with Homebrew)**:
    ```bash
    brew install sdl2
    ```

### Build and Run

1.  **Get a ROM**: Download a Chip-8 ROM (e.g., `PONG`, `INVADERS`) and place it in the `roms/` directory.

2.  **Compile**: Open a terminal in the project directory and run `make`.

    ```bash
    make
    ```

3.  **Run**: Execute the program using the `run.sh` script or by calling the executable directly.

    ```bash
    # Using the script (recommended)
    ./run.sh roms/PONG

    # Or manually
    ./chip8_emulator roms/PONG
    ```


## ⌨️ Key Mapping

The Chip-8's original 16-key hexadecimal keypad is mapped to a modern QWERTY keyboard as follows:

| Chip-8 Key | Keyboard Key |   | Chip-8 Key | Keyboard Key |
| :--------: | :----------: | - | :--------: | :----------: |
|    `1`     |      `1`     |   |    `2`     |      `2`     |
|    `3`     |      `3`     |   |    `C`     |      `4`     |
|    `4`     |      `Q`     |   |    `5`     |      `W`     |
|    `6`     |      `E`     |   |    `D`     |      `R`     |
|    `7`     |      `A`     |   |    `8`     |      `S`     |
|    `9`     |      `D`     |   |    `E`     |      `F`     |
|    `A`     |      `Z`     |   |    `0`     |      `X`     |
|    `B`     |      `C`     |   |    `F`     |      `V`     |