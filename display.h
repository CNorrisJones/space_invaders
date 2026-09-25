#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <stdint.h>
#include "pixel.h"

enum DisplayEventStatus {
    DISPLAY_CONTINUE = 0,
    DISPLAY_QUIT = 1
};

enum DisplayFrameRenderStatus {
    DISPLAY_FAILURE = 1,
    DISPLAY_SUCCESS = 0
};

enum WindowDisplayBools {
    WINDOW_HIDDEN = 0,
    WINDOW_NOT_HIDDEN = 1
};

typedef struct DisplayContext {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    PixelPalette pixel_palette;
    int frame_buffer_width;
    int frame_buffer_height; 
    int window_visible;
} DisplayContext;

DisplayContext *DisplayInit(int buffer_width, int buffer_height);
enum DisplayFrameRenderStatus DisplayFrame(DisplayContext *display_context, const uint32_t *frame_buffer);
enum DisplayEventStatus DisplayPollEvents(void);
void DisplayShutdown(DisplayContext *display_context);

#endif
