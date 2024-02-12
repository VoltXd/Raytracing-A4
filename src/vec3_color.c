#include "vec3_color.h"

#include <math.h>

float vec3_magnitude(const vec3_t* v)
{
    return sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
}

float vec3_dot(const vec3_t* v1, const vec3_t* v2)
{
    return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
}

void vec3_unitVector(vec3_t* v)
{
    float mag = vec3_magnitude(v);
    v->x /= mag;
    v->y /= mag;
    v->z /= mag;
}

vec3_t vec3_add(const vec3_t *v1, const vec3_t *v2)
{
    return (vec3_t){ v1->x + v2->x, v1->y + v2->y, v1->z + v2->z };
}

vec3_t vec3_sub(const vec3_t *v1, const vec3_t *v2)
{
    return (vec3_t){ v1->x - v2->x, v1->y - v2->y, v1->z - v2->z };
}

void vec3_scalarAdd(vec3_t *v, float s)
{
    v->x += s;
    v->y += s;
    v->z += s;
}

void vec3_scalarMul(vec3_t *v, float s)
{
    v->x *= s;
    v->y *= s;
    v->z *= s;
}
