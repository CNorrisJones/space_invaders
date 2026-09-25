#include "video.h"
#include <stddef.h>

enum VideoConvertStatus ConvertVideoRamToBuffer(const uint8_t *video_ram, uint32_t *frame_buffer, const PixelPalette *pixel_palette) {
    // Check for NULL Parameters
    if (video_ram == NULL || frame_buffer == NULL || pixel_palette == NULL) {
        return VIDEO_CONVERT_FAILURE;
    }

    // Read each video-RAM byte
    for (size_t i = 0; i < VIDEO_RAM_SIZE; i++) {
        uint8_t ram_byte = video_ram[i];

        // For each bit in byte, set corresponding pixel in frame_buffer
        // 1 = white pixel, 0 = black pixel
        for (size_t j = 0; j < 8; j++) {
            uint8_t ram_bit = (ram_byte >> j) & 0x01;

            if (ram_bit == 0x00) {
                frame_buffer[(i * 8) + j] = pixel_palette->black_pixel;
            } else {
                frame_buffer[(i * 8) + j] = pixel_palette->white_pixel;
            }
        }
    }

    // Apply the space invaders bit order and rotated coordinate mapping
    // Write the results into the 256x224 frame_buffer
    
    return VIDEO_CONVERT_SUCCESS;
}
