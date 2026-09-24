#ifndef VIDEO_H
#define VIDEO_H

#include <stdint.h>
#include "pixel.h"

enum VideoConvertStatus {
    VIDEO_CONVERT_SUCCESS = 0,
    VIDEO_CONVERT_FAILURE = 1
};

enum VideoConstants {
    VIDEO_RAM_START = 0x2400,
    VIDEO_RAM_SIZE = 0x1C00,
    VIDEO_WIDTH = 256,
    VIDEO_HEIGHT = 224
};

enum VideoConvertStatus ConvertVideoRamToBuffer(const uint8_t *video_ram, uint32_t *frame_buffer, const PixelPalette *pixel_palette);

#endif
