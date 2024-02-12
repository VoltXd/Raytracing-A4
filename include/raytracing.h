#ifndef RAYTRACING_H
#define RAYTRACING_H

#include "vec3_color.h"
#include "sphere.h"
#include "camera.h"

void raytracing(color_t* image, const uint16_t width, const uint16_t height, const sphere_t* spheres, uint8_t numberOfSpheres, const camera_t* camera);
vec3_t getRayHitPosition(const vec3_t* origin, const vec3_t* direction, float distance);

#endif