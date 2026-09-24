#include "video.h"
#include <stddef.h>

enum VideoConvertStatus ConvertVideoRamToBuffer(const uint8_t *video_ram, uint32_t *frame_buffer, const PixelPalette *pixel_palette) {
    // Check for NULL Parameters
    if (video_ram == NULL || frame_buffer == NULL || pixel_palette == NULL) {
        return VIDEO_CONVERT_FAILURE;
    }
    // Read each video-RAM byte
    // Examine the 8 bits of each byte
    // Convert each bit to either black or white pixel
    // Apply the space invaders bit order and rotated coordinate mapping
    // Write the results into the 256x224 frame_buffer
    
    return VIDEO_CONVERT_SUCCESS;
}
