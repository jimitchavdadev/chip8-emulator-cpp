#include "Chip8.hpp"
#include "Platform.hpp"
#include <iostream>
#include <chrono>
#include <thread>

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <ROM file>\n";
        return 1;
    }

    const int VIDEO_SCALE = 15;
    const int WINDOW_WIDTH = 64 * VIDEO_SCALE;
    const int WINDOW_HEIGHT = 32 * VIDEO_SCALE;
    const int EMULATION_SPEED_HZ = 500; // Chip-8 CPU speed in Hz

    Platform platform("Chip-8 Emulator", WINDOW_WIDTH, WINDOW_HEIGHT, 64, 32);
    Chip8 chip8;
    chip8.loadROM(argv[1]);

    auto last_cycle_time = std::chrono::high_resolution_clock::now();
    bool quit = false;

    while (!quit)
    {
        // Process input
        if (!platform.processInput(chip8.keypad))
        {
            quit = true;
        }

        // Emulate at a fixed speed
        auto current_time = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(current_time - last_cycle_time).count();

        if (dt > (1000.0f / EMULATION_SPEED_HZ))
        {
            last_cycle_time = current_time;
            chip8.emulateCycle();

            // Draw if needed
            if (chip8.draw_flag)
            {
                platform.update(chip8.display, 64 * sizeof(uint32_t));
                chip8.draw_flag = false;
            }
        }
    }

    return 0;
}