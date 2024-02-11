#include "utils.h"

void imageFloatToU8(const color_t *src, color_u8_t *dst, uint32_t imgSize)
{
    uint32_t i;
    for (i = 0; i < imgSize; i++)
    {
        dst[i].r = (uint8_t)(src[i].r * 255.999f); 
        dst[i].g = (uint8_t)(src[i].g * 255.999f); 
        dst[i].b = (uint8_t)(src[i].b * 255.999f); 
    }
}