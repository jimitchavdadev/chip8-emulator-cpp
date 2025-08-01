#pragma once
#include <cstdint>

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

class Platform
{
public:
    Platform(const char *title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
    ~Platform();

    void update(const void *buffer, int pitch);
    bool processInput(uint8_t *keys);
    void playBeep(bool play);

private:
    SDL_Window *window{};
    SDL_Renderer *renderer{};
    SDL_Texture *texture{};
};