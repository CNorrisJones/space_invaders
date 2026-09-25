#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "pixel.h"
#include "display.h"

DisplayContext *DisplayInit(int buffer_width, int buffer_height) {
    if (buffer_width <= 0 || buffer_height <= 0) {
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
    display_context->window_visible = WINDOW_HIDDEN;

    int vid_failure = SDL_Init(SDL_INIT_VIDEO);

    if (vid_failure) {
        const char *error_msg = SDL_GetError();
        printf("Error Occurred: %s\n", error_msg);
        
        DisplayShutdown(display_context);
        return NULL;
    }

    display_context->window = SDL_CreateWindow(
        "Test Window",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        display_context->frame_buffer_width * WINDOW_SCALE,
        display_context->frame_buffer_height * WINDOW_SCALE,
        SDL_WINDOW_HIDDEN
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

    // Set Initial Render Colour
    int renderer_draw_color = SDL_SetRenderDrawColor(
        display_context->renderer,
        0,
        0,
        0,
        255
    );

    if (renderer_draw_color < 0) {
        const char *error_msg = SDL_GetError();
        printf("Error Occurred: %s\n", error_msg);

        DisplayShutdown(display_context);
        return NULL;
    }

    return display_context;
}

enum DisplayFrameRenderStatus DisplayFrame(DisplayContext *display_context, const uint32_t *frame_buffer) {
    if (display_context == NULL || frame_buffer == NULL) {
        return DISPLAY_FAILURE;
    }

    const int texture_pitch = display_context->frame_buffer_width * sizeof(uint32_t);

    int update_texture = SDL_UpdateTexture(
        display_context->texture,
        NULL,
        frame_buffer,
        texture_pitch
    );

    if (update_texture < 0) {
        const char *error_msg = SDL_GetError();
        printf("Error Occurred: %s\n", error_msg);
        return DISPLAY_FAILURE;
    }

    // Clear the Renderer
    int render_clear_success = SDL_RenderClear(display_context->renderer);
    if (render_clear_success < 0) {
        const char *error_msg = SDL_GetError();
        printf("Error Occurred: %s\n", error_msg);
        return DISPLAY_FAILURE;
    }

    // Copy Texture to Renderer
    int render_copy_success = SDL_RenderCopy(
        display_context->renderer,
        display_context->texture,
        NULL,
        NULL
    );

    if (render_copy_success < 0) {
        const char *error_msg = SDL_GetError();
        printf("Error Occurred: %s\n", error_msg);
        return DISPLAY_FAILURE;
    }

    if (display_context->window_visible == WINDOW_HIDDEN) {
        SDL_ShowWindow(display_context->window);
        display_context->window_visible = WINDOW_VISIBLE;
    }

    // Present Renderer
    SDL_RenderPresent(display_context->renderer);
    return DISPLAY_SUCCESS;
}

enum DisplayEventStatus DisplayPollEvents(void) {
    SDL_Event event;
    int return_value = DISPLAY_CONTINUE;

    while(SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                return_value = DISPLAY_QUIT;
                break;
        }
    }

    return return_value;
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
