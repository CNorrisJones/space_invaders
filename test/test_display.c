#include <stdlib.h>
#include "display.h"

int main(void) {
    DisplayContext *display = DisplayInit(256, 224);
    if (display == NULL) {
        DisplayShutdown(display);
        exit(DISPLAY_FAILURE);
    }

    uint32_t *frame_buffer = malloc(256 * 224 * sizeof(uint32_t));

    if (frame_buffer == NULL) {
        DisplayShutdown(display);
        exit(DISPLAY_FAILURE);
    }
    
    // Set Each element in frame_buffer to black pixel
    for (int y = 0; y < 224; y++) {
        for (int x = 0; x < 256; x++) {
            if ((x == 0) || (y == 0) || (x == 1) || (y == 1)) {
                frame_buffer[y * 256 + x] = display->pixel_palette.white_pixel;
            } else {
                frame_buffer[y * 256 + x] = display->pixel_palette.black_pixel;
            }
        }
    }

    int quit = 0;

    while(!quit) {
        quit = DisplayPollEvents();

        int frame_status = DisplayFrame(display, frame_buffer);
        if (frame_status != 0) {
            free(frame_buffer);
            DisplayShutdown(display);
            exit(DISPLAY_FAILURE);
        }

        // Optionally Delay Briefly
        SDL_Delay(16);
    }

    free(frame_buffer);
    DisplayShutdown(display);

    return DISPLAY_SUCCESS;
}
