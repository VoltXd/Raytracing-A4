#include "raytracing.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

void raytracing(color_t *image, const uint16_t width, const uint16_t height, const sphere_t* spheres, uint8_t numberOfSpheres, const camera_t* camera)
{
    // Boundaries from fov
    float aspectRatio = (float)width / height;
    float dirHorizontalMax = tanf(camera->fov * 0.5f);
    float dirHorizontalMin = -dirHorizontalMax;
    float dirVecticalMax = tanf(camera->fov * 0.5 / aspectRatio);
    float dirVecticalMin = -dirVecticalMax;

    // Steps
    float stepHorizontal = (dirHorizontalMax - dirHorizontalMin) / width;
    float stepVertical = (dirVecticalMax - dirVecticalMin) / height;

    uint16_t i, j, k;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            // Direction (Let's assume cam.dir.z == -1 or 1)
            vec3_t rayDirection = { dirHorizontalMin + i * stepHorizontal, dirVecticalMin + j * stepVertical, camera->direction.z };
            // vec3_unitVector(&rayDirection);

            float a = 0.5f * (rayDirection.y + 1.0f);
            color_t pixelColor = (color_t){ (1.0f - a)*1.0f + a*0.5f, (1.0f - a)*1.0f + a*0.7f, (1.0f- a)*1.0f + a*1.0f };
            float closestSphereDistance = INFINITY;
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
                        pixelColor = spheres[k].albedo;
                    }
                }

            }
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
