#include "vec3_color.h"

#include <math.h>
#include "utils.h"

float vec3_lengthSquared(const vec3_t *v)
{
    return v->x * v->x + v->y * v->y + v->z * v->z;
}

float vec3_magnitude(const vec3_t *v)
{
    return sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
}

float vec3_dot(const vec3_t* v1, const vec3_t* v2)
{
    return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
}

vec3_t vec3_reflect(const vec3_t *v, const vec3_t *n)
{
    vec3_t temp = vec3_scalarMul_return(n, 2.0f * vec3_dot(v, n));
    return vec3_sub(v, &temp);
}

vec3_t vec3_refract(const vec3_t *uv, const vec3_t *n, float refractionRatio, uint32_t* seed)
{
    // Perpendicular component
    vec3_t refractPerpendicular = *uv;
    vec3_opposite(&refractPerpendicular);
    float cos_theta = fminf(vec3_dot(&refractPerpendicular, n), 1.0);
    float sin_theta = sqrtf(1.0f - cos_theta * cos_theta);
    if (sin_theta * refractionRatio > 1.0f || shlickReflectance(cos_theta, refractionRatio) > randomFloatInUnitInterval(seed))
        return vec3_reflect(uv, n);
    refractPerpendicular = vec3_scalarMul_return(n, cos_theta);
    refractPerpendicular = vec3_add(&refractPerpendicular, uv);
    vec3_scalarMul(&refractPerpendicular, refractionRatio);

    // Parallel component
    vec3_t refractParallel = vec3_scalarMul_return(n, -sqrtf(fabsf(1.0f - vec3_lengthSquared(&refractPerpendicular))));

    // Refraction vector
    return vec3_add(&refractPerpendicular, &refractParallel);
}

uint8_t vec3_isNearZero(const vec3_t *v)
{
    const float threshold = 1e-8f;
    return (v->x < threshold) && (v->y < threshold) && (v->z < threshold);
}

void vec3_normalize(vec3_t* v)
{
    float invMag = 1.0f / vec3_magnitude(v);
    v->x *= invMag;
    v->y *= invMag;
    v->z *= invMag;
}

void vec3_opposite(vec3_t *v)
{
    v->x = -v->x;
    v->y = -v->y;
    v->z = -v->z;
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

vec3_t vec3_scalarMul_return(const vec3_t *v, float s)
{
    vec3_t result;
    result.x = v->x * s;
    result.y = v->y * s;
    result.z = v->z * s;
    return result;
}

float color_dot(const color_t* v1, const color_t* v2)
{
    return v1->r * v2->r + v1->g * v2->g + v1->b * v2->b;
}

color_t color_add(const color_t *v1, const color_t *v2)
{
    return (color_t){ v1->r + v2->r, v1->g + v2->g, v1->b + v2->b };
}

color_t color_mul(const color_t *v1, const color_t *v2)
{
    return (color_t){ v1->r * v2->r, v1->g * v2->g, v1->b * v2->b };
}

color_t color_scalarMul_return(const color_t *v, float s)
{
    color_t result;
    result.r = v->r * s;
    result.g = v->g * s;
    result.b = v->b * s;
    return result;
}

void color_scalarMul(color_t *v, float s)
{
    v->r *= s;
    v->g *= s;
    v->b *= s;
}

const color_t BLACK = { 0.0f, 0.0f, 0.0f };