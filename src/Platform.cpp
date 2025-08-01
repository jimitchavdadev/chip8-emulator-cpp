#include "Platform.hpp"
#include <SDL2/SDL.h>

// Key mapping from Chip-8 keypad to a standard keyboard
const SDL_Scancode keymap[16] = {
    SDL_SCANCODE_X, // 0
    SDL_SCANCODE_1, // 1
    SDL_SCANCODE_2, // 2
    SDL_SCANCODE_3, // 3
    SDL_SCANCODE_Q, // 4
    SDL_SCANCODE_W, // 5
    SDL_SCANCODE_E, // 6
    SDL_SCANCODE_A, // 7
    SDL_SCANCODE_S, // 8
    SDL_SCANCODE_D, // 9
    SDL_SCANCODE_Z, // A
    SDL_SCANCODE_C, // B
    SDL_SCANCODE_4, // C
    SDL_SCANCODE_R, // D
    SDL_SCANCODE_F, // E
    SDL_SCANCODE_V  // F
};

Platform::Platform(const char *title, int windowWidth, int windowHeight, int textureWidth, int textureHeight)
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, textureWidth, textureHeight);
}

Platform::~Platform()
{
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Platform::update(const void *buffer, int pitch)
{
    SDL_UpdateTexture(texture, nullptr, buffer, pitch);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

bool Platform::processInput(uint8_t *keys)
{
    bool quit = false;
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            quit = true;
            break;

        case SDL_KEYDOWN:
            if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
            {
                quit = true;
            }
            for (int i = 0; i < 16; ++i)
            {
                if (event.key.keysym.scancode == keymap[i])
                {
                    keys[i] = 1;
                }
            }
            break;

        case SDL_KEYUP:
            for (int i = 0; i < 16; ++i)
            {
                if (event.key.keysym.scancode == keymap[i])
                {
                    keys[i] = 0;
                }
            }
            break;
        }
    }
    return !quit;
}

void Platform::playBeep(bool play)
{
    // Basic beep sound can be implemented here if desired.
    // For simplicity, this is often omitted or a simple console beep is used.
}