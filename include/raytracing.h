#ifndef RAYTRACING_H
#define RAYTRACING_H

#include "vec3_color.h"
#include "sphere.h"
#include "camera.h"

void raytracing(color_t* image, const uint16_t width, const uint16_t height, const uint16_t raysPerPixel, const uint8_t raysDepth, const sphere_t* spheres, const uint16_t numberOfSpheres, const camera_t* camera);

#endif