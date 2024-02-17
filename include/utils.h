#ifndef UTILS_H
#define UTILS_H

#include "vec3_color.h"
#include <math.h>

void imageFloatToU8(const color_t* src, color_u8_t* dst, uint32_t imgSize);
void imageLinearToGamma(color_t* image, uint32_t imgSize);

float randomFloatInUnitInterval(uint32_t* seed);
float randomFloat(uint32_t* seed, float min, float max);
vec3_t randomUnitVector(uint32_t* seed);
vec3_t randomInHemisphere(uint32_t* seed, const vec3_t* normal);

float shlickReflectance(float cos_theta, float refractionRation);

inline float degrees_to_radians(float degrees) { return degrees * M_PI / 180.0f; }


#endif