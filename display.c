#include <SDL2/SDL.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "pixel.h"
#include "display.h"

DisplayContext *DisplayInit(int buffer_width, int buffer_height) {
    if (buffer_width < 0 | buffer_height < 0) {
        return NULL;
    }

    const int WINDOW_SCALE = 3;
    DisplayContext *display_context = malloc(sizeof(DisplayContext));
    if (display_context == NULL) {
        return NULL;
    }

    display_context->window = NULL;
    display_context->renderer = NULL;
    display_context->texture = NULL;
    display_context->frame_buffer_height = buffer_height;
    display_context->frame_buffer_width = buffer_width;

    int vid_failure = SDL_Init(SDL_INIT_VIDEO);

    if (vid_failure) {
        const char *error_msg = SDL_GetError();
        printf("Error Occurred: %s\n", error_msg);
        
        DisplayShutdown(display_context);
        return NULL;
    }

    display_context->window = SDL_CreateWindow(
        "Test Window", SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        display_context->frame_buffer_width * WINDOW_SCALE,
        display_context->frame_buffer_height * WINDOW_SCALE,
        SDL_WINDOW_SHOWN
    );

    if (display_context->window == NULL) {
        const char *error_msg = SDL_GetError();
        printf("Error Occurred: %s\n", error_msg);

        DisplayShutdown(display_context);
        return NULL;
    }

    display_context->renderer = SDL_CreateRenderer(
        display_context->window,
        -1,
        0
    );

    if (display_context->renderer == NULL) {
        const char *error_msg = SDL_GetError();
        printf("Error Occurred: %s\n", error_msg);

        DisplayShutdown(display_context);
        return NULL;
    }

    display_context->texture = SDL_CreateTexture(
        display_context->renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        display_context->frame_buffer_width,
        display_context->frame_buffer_height
    );

    if (display_context->texture == NULL) {
        const char *error_msg = SDL_GetError();
        printf("Error Occurred: %s\n", error_msg);

        DisplayShutdown(display_context);
        return NULL;
    }

    SDL_PixelFormat *pixel_format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);

    if (pixel_format == NULL) {
        const char *error_msg = SDL_GetError();
        printf("Error Occurred: %s\n", error_msg);

        DisplayShutdown(display_context);
        return NULL;
    }

    display_context->pixel_palette.black_pixel = SDL_MapRGBA(pixel_format, 0, 0, 0, 255);
    display_context->pixel_palette.white_pixel = SDL_MapRGBA(pixel_format, 255, 255, 255, 255);
    SDL_FreeFormat(pixel_format);

    return display_context;
}

int DisplayFrame(DisplayContext *display_context, const uint8_t *frame_buffer) {

}

void DisplayShutdown(DisplayContext *display_context) {
    if (display_context == NULL) {
        return;
    }

    if (display_context->texture != NULL) {
        SDL_DestroyTexture(display_context->texture);
    }

    if (display_context->renderer != NULL) {
        SDL_DestroyRenderer(display_context->renderer);
    }

    if (display_context->window != NULL) {
        SDL_DestroyWindow(display_context->window);
    }

    SDL_Quit();

    free(display_context);
}
