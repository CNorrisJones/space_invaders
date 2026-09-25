#include <stdio.h>
#include <stdlib.h>
#include "video.h"
#include "display.h"

int main(void) {
    printf("Run!\n");

    DisplayContext *test_display = DisplayInit(VIDEO_WIDTH, VIDEO_HEIGHT);
    if (test_display == NULL) {
        return 1;
    }

    uint8_t *video_ram = calloc(VIDEO_RAM_SIZE, sizeof(uint8_t));
    if (video_ram == NULL) {
        DisplayShutdown(test_display);
        return 1;
    }

    uint32_t *frame_buffer = calloc(VIDEO_WIDTH * VIDEO_HEIGHT, sizeof(uint32_t));
    if (frame_buffer == NULL) {
        DisplayShutdown(test_display);
        free(video_ram);
        return 1;
    }

    int vid_response = ConvertVideoRamToBuffer(video_ram, frame_buffer, &test_display->pixel_palette);

    DisplayShutdown(test_display);
    free(video_ram);
    free(frame_buffer);

    return vid_response;
}