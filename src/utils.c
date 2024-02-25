#include "utils.h"

#include <stdio.h>

static uint32_t pcg_hash(uint32_t* seed)
{
    uint32_t state = *seed;
    *seed = *seed * 747796405u + 2891336453u;
    uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

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

void imageLinearToGamma(color_t *image, uint32_t imgSize)
{
    uint32_t i;
    for (i = 0; i < imgSize; i++)
    {
        image[i].r = sqrtf(image[i].r); 
        image[i].g = sqrtf(image[i].g); 
        image[i].b = sqrtf(image[i].b); 
    }
}

float randomFloatInUnitInterval(uint32_t* seed)
{
    return (float)pcg_hash(seed) * (1.0f / UINT32_MAX);
}

float randomFloat(uint32_t *seed, float min, float max)
{
    return min + (max - min) * randomFloatInUnitInterval(seed);
}

vec3_t randomInUnitDisk(uint32_t *seed)
{
    float theta = 2.0f * M_PI * randomFloatInUnitInterval(seed);
    float r = sqrtf(randomFloatInUnitInterval(seed));
    return (vec3_t){ r * cosf(theta), r * sinf(theta), 0.0f };
}

vec3_t randomUnitVector(uint32_t *seed)
{
    float theta = 2.0f * M_PI * randomFloatInUnitInterval(seed);
    float phi = acosf(1.0f - 2.0f * randomFloatInUnitInterval(seed));
    vec3_t v;
    v.x = cosf(theta) * sinf(phi);
    v.y = sinf(theta) * sinf(phi);
    v.z = cosf(phi);
    return v;
}

vec3_t randomInHemisphere(uint32_t *seed, const vec3_t *normal)
{
    vec3_t v = randomUnitVector(seed);

    // The hemisphere is OUT of the sphere
    if (vec3_dot(&v, normal) < 0)
        vec3_opposite(&v);
    return v;
}

float shlickReflectance(float cos_theta, float refractionRatio)
{ 
    float r0 = (1.0f - refractionRatio) / (1.0f + refractionRatio);
    r0 *= r0;
    return r0 + (1.0f - r0) * powf((1.0f - cos_theta) ,5);
}

vec3_t randomDefocusedRayPosition(uint32_t *seed, const vec3_t *center, const vec3_t *defocus_disk_u, const vec3_t *defocus_disk_v)
{
    const vec3_t v = randomInUnitDisk(seed);
    const vec3_t randomDefocus_u = vec3_scalarMul_return(defocus_disk_u, v.x);
    const vec3_t randomDefocus_v = vec3_scalarMul_return(defocus_disk_v, v.y);
    const vec3_t defocusedRayPosition = vec3_add(&randomDefocus_u, &randomDefocus_v);
    return vec3_add(center, &defocusedRayPosition);
}
