#include <stdio.h>
#include <stdlib.h>
#include "video.h"
#include "display.h"

int main(void) {
    printf("Run!\n");

    DisplayContext *test_display = DisplayInit(VIDEO_WIDTH, VIDEO_HEIGHT);
    if (test_display == NULL) {
        return VIDEO_CONVERT_FAILURE;
    }

    uint8_t *video_ram = calloc(VIDEO_RAM_SIZE, sizeof(*video_ram));
    if (video_ram == NULL) {
        DisplayShutdown(test_display);
        return VIDEO_CONVERT_FAILURE;
    }

    video_ram[875] = 0xD1;

    uint32_t *frame_buffer = calloc(VIDEO_WIDTH * VIDEO_HEIGHT, sizeof(*frame_buffer));
    if (frame_buffer == NULL) {
        DisplayShutdown(test_display);
        free(video_ram);
        return VIDEO_CONVERT_FAILURE;
    }

    int vid_response = ConvertVideoRamToBuffer(video_ram, frame_buffer, &test_display->pixel_palette);
    if (vid_response == VIDEO_CONVERT_FAILURE) {
        DisplayShutdown(test_display);
        free(video_ram);
        free(frame_buffer);
        return VIDEO_CONVERT_FAILURE;
    }

    int quit = 0;

    while(!quit) {
        quit = DisplayPollEvents();

        int frame_status = DisplayFrame(test_display, frame_buffer);
        if (frame_status != 0) {
            DisplayShutdown(test_display);
            free(video_ram);
            free(frame_buffer);
            return VIDEO_CONVERT_FAILURE;
        }

        // Optionally Delay Briefly
        SDL_Delay(16);
    }

    DisplayShutdown(test_display);
    free(video_ram);
    free(frame_buffer);

    return vid_response;
}