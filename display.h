#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include "pixel.h"

typedef struct DisplayContext {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    PixelPalette pixel_palette;
    int frame_buffer_width;
    int frame_buffer_height; 
} DisplayContext;

DisplayContext *DisplayInit(int buffer_width, int buffer_height);
void DisplayShutdown(DisplayContext *display_context);

#endif
