#ifndef VEC3_COLOR_H
#define VEC3_COLOR_H

#include <stdint.h>


typedef struct color_t
{
    float r;
    float g;
    float b;
} color_t;

typedef struct color_u8_t
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} color_u8_t;

typedef struct vec3_t
{
    float x;
    float y;
    float z;
} vec3_t;



#endif