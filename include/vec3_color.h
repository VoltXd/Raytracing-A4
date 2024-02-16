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

float vec3_magnitude(const vec3_t* v);
float vec3_dot(const vec3_t* v1, const vec3_t* v2);
void vec3_normalize(vec3_t* v);
vec3_t vec3_add(const vec3_t* v1, const vec3_t* v2);
vec3_t vec3_sub(const vec3_t* v1, const vec3_t* v2);
void vec3_scalarAdd(vec3_t* v, float s);
void vec3_scalarMul(vec3_t* v, float s);

color_t color_add(const color_t* v1, const color_t* v2);
void color_scalarMul(color_t* v, float s);


#endif