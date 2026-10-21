#include <stdio.h>
#include <stdlib.h>

#include "video.h"
#include "display.h"
#include "input_state.h"

int main(void) {
    printf("Run!\n");

    DisplayContext *test_display = DisplayInit(VIDEO_OUTPUT_WIDTH, VIDEO_OUTPUT_HEIGHT);
    if (test_display == NULL) {
        return VIDEO_CONVERT_FAILURE;
    }

    uint8_t *video_ram = calloc(VIDEO_RAM_SIZE, sizeof(*video_ram));
    if (video_ram == NULL) {
        DisplayShutdown(test_display);
        return VIDEO_CONVERT_FAILURE;
    }

    video_ram[875] = 0xD1;

    uint32_t *frame_buffer = calloc(VIDEO_OUTPUT_WIDTH * VIDEO_OUTPUT_HEIGHT, sizeof(*frame_buffer));
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
    InputState *input = malloc(sizeof(InputState));
    if (input == NULL) {
        printf("Error initializing the InputState\n");
        DisplayShutdown(test_display);
        free(video_ram);
        free(frame_buffer);
        return VIDEO_CONVERT_FAILURE;
    }

    input->credit = false;
    input->one_player = false;
    input->two_player = false;
    input->p1_fire = false;
    input->p1_left = false;
    input->p1_right = false;
    input->p2_fire = false;
    input->p2_left = false;
    input->p2_right = false;
    input->tilt = false;

    while(!quit) {
        quit = DisplayPollEvents(input);

        int frame_status = DisplayFrame(test_display, frame_buffer);
        if (frame_status != 0) {
            free(input);
            DisplayShutdown(test_display);
            free(video_ram);
            free(frame_buffer);
            return VIDEO_CONVERT_FAILURE;
        }

        // Optionally Delay Briefly
        SDL_Delay(16);
    }

    free(input);
    DisplayShutdown(test_display);
    free(video_ram);
    free(frame_buffer);

    return vid_response;
}