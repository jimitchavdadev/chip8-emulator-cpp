#include "Chip8.hpp"
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <random>

// Fontset for characters 0-F
uint8_t fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Chip8::Chip8()
{
    initialize();
}

void Chip8::initialize()
{
    pc = 0x200; // Program counter starts at 0x200
    I = 0;
    sp = 0;
    delay_timer = 0;
    sound_timer = 0;

    // Clear memory, registers, stack, display, and keypad
    for (int i = 0; i < 4096; ++i)
        memory[i] = 0;
    for (int i = 0; i < 16; ++i)
    {
        V[i] = 0;
        stack[i] = 0;
        keypad[i] = 0;
    }
    for (int i = 0; i < 64 * 32; ++i)
        display[i] = 0;

    // Load fontset into memory
    for (int i = 0; i < 80; ++i)
        memory[i] = fontset[i];

    // Seed random number generator
    srand(time(nullptr));
}

void Chip8::loadROM(const char *filename)
{
    FILE *rom = fopen(filename, "rb");
    if (rom == nullptr)
    {
        fprintf(stderr, "Error: Failed to open ROM %s\n", filename);
        exit(1);
    }

    // Get file size
    fseek(rom, 0, SEEK_END);
    long rom_size = ftell(rom);
    rewind(rom);

    // Allocate buffer and read ROM
    char *buffer = (char *)malloc(sizeof(char) * rom_size);
    if (buffer == nullptr)
    {
        fprintf(stderr, "Error: Failed to allocate memory for ROM\n");
        exit(1);
    }
    size_t result = fread(buffer, 1, rom_size, rom);
    if (result != rom_size)
    {
        fprintf(stderr, "Error: Failed to read ROM\n");
        exit(1);
    }

    // Copy ROM into Chip-8 memory
    if ((4096 - 0x200) >= rom_size)
    {
        for (int i = 0; i < rom_size; ++i)
        {
            memory[i + 0x200] = buffer[i];
        }
    }
    else
    {
        fprintf(stderr, "Error: ROM too large for memory\n");
        exit(1);
    }

    fclose(rom);
    free(buffer);
}

void Chip8::emulateCycle()
{
    // Fetch opcode
    uint16_t opcode = (memory[pc] << 8) | memory[pc + 1];
    pc += 2;

    // Decode and execute opcode
    uint16_t x = (opcode & 0x0F00) >> 8;
    uint16_t y = (opcode & 0x00F0) >> 4;
    uint8_t kk = opcode & 0x00FF;
    uint16_t nnn = opcode & 0x0FFF;

    switch (opcode & 0xF000)
    {
    case 0x0000:
        switch (kk)
        {
        case 0xE0: // 00E0: Clear screen
            for (int i = 0; i < 64 * 32; ++i)
                display[i] = 0;
            draw_flag = true;
            break;
        case 0xEE: // 00EE: Return from subroutine
            pc = stack[--sp];
            break;
        default:
            // 0nnn: Not implemented on modern interpreters
            break;
        }
        break;

    case 0x1000: // 1nnn: Jump to address nnn
        pc = nnn;
        break;

    case 0x2000: // 2nnn: Call subroutine at nnn
        stack[sp++] = pc;
        pc = nnn;
        break;

    case 0x3000: // 3xkk: Skip next instruction if Vx == kk
        if (V[x] == kk)
            pc += 2;
        break;

    case 0x4000: // 4xkk: Skip next instruction if Vx != kk
        if (V[x] != kk)
            pc += 2;
        break;

    case 0x5000: // 5xy0: Skip next instruction if Vx == Vy
        if (V[x] == V[y])
            pc += 2;
        break;

    case 0x6000: // 6xkk: Set Vx = kk
        V[x] = kk;
        break;

    case 0x7000: // 7xkk: Set Vx = Vx + kk
        V[x] += kk;
        break;

    case 0x8000:
        switch (opcode & 0x000F)
        {
        case 0x0:
            V[x] = V[y];
            break; // 8xy0: Set Vx = Vy
        case 0x1:
            V[x] |= V[y];
            break; // 8xy1: Set Vx = Vx OR Vy
        case 0x2:
            V[x] &= V[y];
            break; // 8xy2: Set Vx = Vx AND Vy
        case 0x3:
            V[x] ^= V[y];
            break; // 8xy3: Set Vx = Vx XOR Vy
        case 0x4:  // 8xy4: Set Vx = Vx + Vy, set VF = carry
            V[0xF] = (V[x] + V[y] > 255) ? 1 : 0;
            V[x] += V[y];
            break;
        case 0x5: // 8xy5: Set Vx = Vx - Vy, set VF = NOT borrow
            V[0xF] = (V[x] > V[y]) ? 1 : 0;
            V[x] -= V[y];
            break;
        case 0x6: // 8xy6: Set Vx = Vx SHR 1
            V[0xF] = V[x] & 0x1;
            V[x] >>= 1;
            break;
        case 0x7: // 8xy7: Set Vx = Vy - Vx, set VF = NOT borrow
            V[0xF] = (V[y] > V[x]) ? 1 : 0;
            V[x] = V[y] - V[x];
            break;
        case 0xE: // 8xyE: Set Vx = Vx SHL 1
            V[0xF] = (V[x] & 0x80) >> 7;
            V[x] <<= 1;
            break;
        }
        break;

    case 0x9000: // 9xy0: Skip next instruction if Vx != Vy
        if (V[x] != V[y])
            pc += 2;
        break;

    case 0xA000: // Annn: Set I = nnn
        I = nnn;
        break;

    case 0xB000: // Bnnn: Jump to location nnn + V0
        pc = nnn + V[0];
        break;

    case 0xC000: // Cxkk: Set Vx = random byte AND kk
        V[x] = (rand() % 256) & kk;
        break;

    case 0xD000:
    { // Dxyn: Display n-byte sprite starting at memory I at (Vx, Vy)
        uint16_t height = opcode & 0x000F;
        uint16_t x_coord = V[x] % 64;
        uint16_t y_coord = V[y] % 32;
        V[0xF] = 0; // Collision flag

        for (int row = 0; row < height; ++row)
        {
            uint8_t sprite_byte = memory[I + row];
            for (int col = 0; col < 8; ++col)
            {
                if ((sprite_byte & (0x80 >> col)) != 0)
                {
                    uint32_t *pixel = &display[(y_coord + row) * 64 + (x_coord + col)];
                    if (*pixel == 0xFFFFFFFF)
                    {
                        V[0xF] = 1; // Collision detected
                    }
                    *pixel ^= 0xFFFFFFFF; // XOR the pixel
                }
            }
        }
        draw_flag = true;
        break;
    }

    case 0xE000:
        switch (kk)
        {
        case 0x9E: // Ex9E: Skip next instruction if key with value Vx is pressed
            if (keypad[V[x]])
                pc += 2;
            break;
        case 0xA1: // ExA1: Skip next instruction if key with value Vx is not pressed
            if (!keypad[V[x]])
                pc += 2;
            break;
        }
        break;

    case 0xF000:
        switch (kk)
        {
        case 0x07:
            V[x] = delay_timer;
            break; // Fx07: Set Vx = delay timer value
        case 0x0A:
        { // Fx0A: Wait for key press, store value in Vx
            bool key_pressed = false;
            for (int i = 0; i < 16; ++i)
            {
                if (keypad[i])
                {
                    V[x] = i;
                    key_pressed = true;
                    break;
                }
            }
            if (!key_pressed)
                pc -= 2; // Repeat instruction
            break;
        }
        case 0x15:
            delay_timer = V[x];
            break; // Fx15: Set delay timer = Vx
        case 0x18:
            sound_timer = V[x];
            break; // Fx18: Set sound timer = Vx
        case 0x1E:
            I += V[x];
            break; // Fx1E: Set I = I + Vx
        case 0x29:
            I = V[x] * 5;
            break; // Fx29: Set I = location of sprite for digit Vx
        case 0x33: // Fx33: Store BCD representation of Vx in memory
            memory[I] = V[x] / 100;
            memory[I + 1] = (V[x] / 10) % 10;
            memory[I + 2] = V[x] % 10;
            break;
        case 0x55: // Fx55: Store registers V0 through Vx in memory
            for (int i = 0; i <= x; ++i)
                memory[I + i] = V[i];
            break;
        case 0x65: // Fx65: Read registers V0 through Vx from memory
            for (int i = 0; i <= x; ++i)
                V[i] = memory[I + i];
            break;
        }
        break;
    default:
        // Unknown opcode
        break;
    }

    // Update timers
    if (delay_timer > 0)
        --delay_timer;
    if (sound_timer > 0)
        --sound_timer;
}