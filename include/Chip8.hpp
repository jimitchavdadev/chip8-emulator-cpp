#pragma once
#include <cstdint>

class Chip8 {
public:
    uint8_t keypad[16]{};
    uint32_t display[64 * 32]{};
    bool draw_flag = false;

    Chip8();
    void loadROM(const char* filename);
    void emulateCycle();

private:
    uint16_t pc; // Program Counter
    uint16_t I;  // Index Register
    uint16_t sp; // Stack Pointer

    uint8_t memory[4096]{};
    uint8_t V[16]{}; // General purpose registers V0-VF

    uint16_t stack[16]{};

    uint8_t delay_timer;
    uint8_t sound_timer;

    void initialize();
};