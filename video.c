#include "video.h"
#include <stddef.h>


// Private translation function used to find the final location of a
// pixel in the frame buffer. Space Invaders Rotates the input array (256 x 224)
// by 90 degress counter-clockwise. Thus (x, y) -> (y, (N-1-x)), with N being
// the width of the input array
static size_t RotatePixelCounterClockwise(size_t initial_location_val) {
    int init_x = initial_location_val % VIDEO_INPUT_WIDTH;
    int init_y = (int) initial_location_val / (int) VIDEO_INPUT_WIDTH;

    int final_x = init_y;
    int final_y = (VIDEO_INPUT_WIDTH - 1 - init_x);

    size_t final_location_val = (final_y * VIDEO_OUTPUT_WIDTH) + final_x;
    return final_location_val;
}

enum VideoConvertStatus ConvertVideoRamToBuffer(const uint8_t *video_ram, uint32_t *frame_buffer, const PixelPalette *pixel_palette) {
    if (video_ram == NULL || frame_buffer == NULL || pixel_palette == NULL) {
        return VIDEO_CONVERT_FAILURE;
    }

    for (size_t i = 0; i < VIDEO_RAM_SIZE; i++) {
        uint8_t ram_byte = video_ram[i];

        for (size_t j = 0; j < 8; j++) {
            uint8_t ram_bit = (ram_byte >> j) & 0x01;
            size_t input_array_location = ((i * 8) + j);
            size_t array_location = RotatePixelCounterClockwise(input_array_location);

            if (ram_bit == 0x00) {
                frame_buffer[array_location] = pixel_palette->black_pixel;
            } else {
                frame_buffer[array_location] = pixel_palette->white_pixel;
            }
        }
    }
    
    return VIDEO_CONVERT_SUCCESS;
}
