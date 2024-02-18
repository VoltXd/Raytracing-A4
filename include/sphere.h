#ifndef SPHERE_H
#define SPHERE_H

#include "vec3_color.h"


typedef struct sphere_t
{
    vec3_t position;
    color_t albedo;
    float radius;
    union { float roughness, fuzziness, refractionIndex; };
    enum { LAMBERTIAN, METAL, DIELECTRIC, NUMBER_OF_MATERIAL } material;     
} sphere_t;

#endif