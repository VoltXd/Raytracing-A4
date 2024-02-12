#ifndef CAMERA_H
#define CAMERA_H

#include "vec3_color.h"

typedef struct camera_t
{
    vec3_t position;
    vec3_t direction;
    float fov;
} camera_t;

#endif