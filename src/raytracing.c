#include "raytracing.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

void raytracing(color_t *image, const uint16_t width, const uint16_t height, const uint16_t raysPerPixel, const sphere_t* spheres, uint8_t numberOfSpheres, const camera_t* camera)
{
    // Boundaries from fov
    const float aspectRatio = (float)width / height;
    const float dirHorizontalMax = tanf(camera->fov * 0.5f);
    const float dirHorizontalMin = -dirHorizontalMax;
    const float dirVecticalMax = tanf(camera->fov * 0.5 / aspectRatio);
    const float dirVecticalMin = -dirVecticalMax;

    // Steps
    const float stepHorizontal = (dirHorizontalMax - dirHorizontalMin) / width;
    const float stepVertical = (dirVecticalMax - dirVecticalMin) / height;

    // Rays weight
    const float inv_raysPerPixel = 1.0f / raysPerPixel;

    uint16_t i, j, k, rayIdx;
    #pragma omp parallel for private(i, k, rayIdx)
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            // Direction (Let's assume cam.dir.z == -1 or 1)
            color_t pixelColor = (color_t){ 0.0f, 0.0f, 0.0f };
            uint32_t seed = omp_get_thread_num();
            for (rayIdx = 0; rayIdx < raysPerPixel; rayIdx++)
            {
                vec3_t rayDirection;
                rayDirection.x = dirHorizontalMin + (i + (float)rand_r(&seed) / RAND_MAX) * stepHorizontal;
                rayDirection.y = dirVecticalMin + (j + (float)rand_r(&seed) / RAND_MAX) * stepVertical;
                rayDirection.z = camera->direction.z;
                float skyGradiant = 0.5f * (rayDirection.y + 1.0f);
                const color_t skyColor = (color_t){ (1.0f - skyGradiant)*1.0f + skyGradiant*0.5f, (1.0f - skyGradiant)*1.0f + skyGradiant*0.7f, (1.0f- skyGradiant)*1.0f + skyGradiant*1.0f };

                float closestSphereDistance = INFINITY;
                int closestSphereIndex = -1;
                for (k = 0; k < numberOfSpheres; k++)
                {   
                    vec3_t originToCenter = vec3_sub(&(camera->position), &(spheres[k].position));
                    float a = vec3_dot(&rayDirection, &rayDirection);
                    float half_b = vec3_dot(&originToCenter, &rayDirection);
                    float c = vec3_dot(&originToCenter, &originToCenter) - spheres[k].radius * spheres[k].radius;
                    float delta = half_b*half_b - a*c;

                    if (delta >= 0)
                    {
                        float newDistance = (-half_b - sqrtf(delta))/ a;

                        if (newDistance <=  0.0f)
                            continue;

                        if (newDistance < closestSphereDistance)
                        {
                            closestSphereDistance = newDistance;
                            closestSphereIndex = k;
                        }
                    }
                }

                if (closestSphereIndex != -1)
                    pixelColor = color_add(&spheres[closestSphereIndex].albedo, &pixelColor);
                else
                    pixelColor = color_add(&skyColor, &pixelColor);
            }
            color_scalarMul(&pixelColor, inv_raysPerPixel);
            image[i + j * width] = pixelColor;
        }
    }
}

vec3_t getRayHitPosition(const vec3_t *origin, const vec3_t *direction, float distance)
{
    vec3_t distanceVector = *direction;
    vec3_scalarMul(&distanceVector, distance);
    return vec3_add(origin, &distanceVector);
}
