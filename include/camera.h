#ifndef CAMERA_H
#define CAMERA_H

#include "vec3_color.h"

typedef struct camera_t
{
    vec3_t lookFrom;
    vec3_t lookAt;
    vec3_t up;
    vec3_t u, v, w; // Camera orthonormal base
    vec3_t step_u, step_v;
    vec3_t viewportUpperLeft;
    vec3_t defocus_disk_u, defocus_disk_v;
    float fov;
    float defocusAngle;
    float focusDistance;
} camera_t;

camera_t initializeCamera(const uint16_t WITDH, const uint16_t HEIGHT);

#endif