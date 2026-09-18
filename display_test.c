#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>

int main() {
    int vid_failure = SDL_Init(SDL_INIT_VIDEO);

    if (vid_failure) {
        const char *error_msg = SDL_GetError();
        printf("Error Occurred: %s\n", error_msg);

        exit(1);
    }

    printf("SDL initialized successfully\n");

    SDL_Window *window = SDL_CreateWindow("Test Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 768, 672, SDL_WINDOW_SHOWN);

    if (window == NULL) {
        const char *error_msg = SDL_GetError();
        printf("Error Occurred: %s\n", error_msg);

        SDL_Quit();
        exit(1);
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    if (renderer == NULL) {
        const char *error_msg = SDL_GetError();
        printf("Error Occurred: %s\n", error_msg);

        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }

    SDL_Texture *test_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 256, 224);

    if (test_texture == NULL) {
        const char *error_msg = SDL_GetError();
        printf("Error Occurred: %s\n", error_msg);

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }

    uint32_t *frame_buffer = malloc(256 * 224 * sizeof(uint32_t));

    if (frame_buffer == NULL) {
        SDL_DestroyTexture(test_texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }

    SDL_PixelFormat *pixel_format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);

    if (pixel_format == NULL) {
        const char *error_msg = SDL_GetError();
        printf("Error Occurred: %s\n", error_msg);

        free(frame_buffer);
        SDL_DestroyTexture(test_texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }

    uint32_t black_pixel = SDL_MapRGBA(pixel_format, 0, 0, 0, 255);
    uint32_t white_pixel = SDL_MapRGBA(pixel_format, 255, 255, 255, 255);
    SDL_FreeFormat(pixel_format);
    
    // Set Each element in frame_buffer to black pixel
    for (int y = 0; y < 224; y++) {
        for (int x = 0; x < 256; x++) {
            if ((x == 0) | (y == 0) | (x == 1) | (y == 1)) {
                frame_buffer[y * 256 + x] = white_pixel;
            } else {
                frame_buffer[y * 256 + x] = black_pixel;
            }
        }
    }

    int init_texture = SDL_UpdateTexture(test_texture, NULL, frame_buffer, 1024);

    if (init_texture < 0) {
        const char *error_msg = SDL_GetError();
        printf("Error Occurred: %s\n", error_msg);

        free(frame_buffer);
        SDL_DestroyTexture(test_texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }

    int renderer_draw_color = SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    if (renderer_draw_color < 0) {
        const char *error_msg = SDL_GetError();
        printf("Error Occurred: %s\n", error_msg);

        free(frame_buffer);
        SDL_DestroyTexture(test_texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }

    int render_clear_success = SDL_RenderClear(renderer);
    if (render_clear_success < 0) {
        const char *error_msg = SDL_GetError();
        printf("Error Occurred: %s\n", error_msg);

        free(frame_buffer);
        SDL_DestroyTexture(test_texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }

    int render_copy_success = SDL_RenderCopy(renderer, test_texture, NULL, NULL);
    if (render_copy_success < 0) {
        const char *error_msg = SDL_GetError();
        printf("Error Occurred: %s\n", error_msg);

        free(frame_buffer);
        SDL_DestroyTexture(test_texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }

    SDL_RenderPresent(renderer);

    // TODO: Generate event loop
    int running = 1;
    SDL_Event event;

    while(running) {
        while(SDL_PollEvent(&event)) {
            switch (event.type) {
                case(SDL_QUIT):
                    running = 0;
            }
        }

        // Redraw and present
        render_clear_success = SDL_RenderClear(renderer);
        if (render_clear_success < 0) {
            const char *error_msg = SDL_GetError();
            printf("Error Occurred: %s\n", error_msg);

            free(frame_buffer);
            SDL_DestroyTexture(test_texture);
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            exit(1);
        }

        render_copy_success = SDL_RenderCopy(renderer, test_texture, NULL, NULL);
        if (render_copy_success < 0) {
            const char *error_msg = SDL_GetError();
            printf("Error Occurred: %s\n", error_msg);

            free(frame_buffer);
            SDL_DestroyTexture(test_texture);
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            exit(1);
        }

        SDL_RenderPresent(renderer);

        // Optionally Delay Briefly
        SDL_Delay(16);
    }

    printf("Cleaning Up Objects\n");
    free(frame_buffer);
    SDL_DestroyTexture(test_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
