#include "sphere.h"

#include <stdio.h>
#include <stdlib.h>

void initializeSpheres(sphere_t *spheres, const uint8_t SQRT_NUMBER_OF_SPHERES)
{
    // Ground
    spheres[0].radius = 1000.0f;
    spheres[0].position = (vec3_t){ 0.0f, -spheres[0].radius, 0.0f };
    spheres[0].albedo = (color_t){ 0.5f, 0.5f, 0.5f };
    spheres[0].roughness = 1.0f;
    spheres[0].material = LAMBERTIAN;

    // Big spheres
    spheres[1].radius = 1.0f;
    spheres[1].position = (vec3_t){ -4.0f, spheres[1].radius, 0.0f };
    spheres[1].albedo = (color_t){ 0.4f, 0.2f, 0.1f };
    spheres[1].roughness = 1.0f;
    spheres[1].material = LAMBERTIAN;

    spheres[2].radius = 1.0f;
    spheres[2].position = (vec3_t){ 4.0f, spheres[2].radius, 0.0f };
    spheres[2].albedo = (color_t){ 0.7f, 0.6f, 0.5f };
    spheres[2].fuzziness = 0.0f;
    spheres[2].material = METAL;

    spheres[3].radius = 1.0f;
    spheres[3].position = (vec3_t){ 0.0f, spheres[3].radius, 0.0f };
    spheres[3].albedo = (color_t){ 1.0f, 1.0f, 1.0f };
    spheres[3].refractionIndex = 1.5f;
    spheres[3].material = DIELECTRIC;

    // Small spheres
    uint8_t i, j;
    for (i = 0; i < SQRT_NUMBER_OF_SPHERES; i++)
    {
        for (j = 0; j < SQRT_NUMBER_OF_SPHERES; j++)
        {   
            uint16_t index = 4 + i*SQRT_NUMBER_OF_SPHERES + j;
            float radius = 0.1f + 0.2f * ((float)rand() / RAND_MAX);

            spheres[index].position.x = -(SQRT_NUMBER_OF_SPHERES / 2) + i + ((float)rand() / RAND_MAX);
            spheres[index].position.y = radius;
            spheres[index].position.z = -(SQRT_NUMBER_OF_SPHERES / 2) + j + ((float)rand() / RAND_MAX);
            
            float materialValue = (float)rand() / RAND_MAX;
            if (materialValue < 0.8)
                spheres[index].material = LAMBERTIAN;
            else if (materialValue < 0.95)
                spheres[index].material = METAL;
            else
                spheres[index].material = DIELECTRIC;
                
            switch (spheres[index].material)
            {
                case LAMBERTIAN:
                    spheres[index].albedo.r = ((float)rand() / RAND_MAX);
                    spheres[index].albedo.g = ((float)rand() / RAND_MAX);
                    spheres[index].albedo.b = ((float)rand() / RAND_MAX);
                    spheres[index].roughness = 0.8f + 0.2f * ((float)rand() / RAND_MAX);
                    break;

                case METAL:
                    spheres[index].albedo.r = 0.5f + 0.5f * ((float)rand() / RAND_MAX);
                    spheres[index].albedo.g = 0.5f + 0.5f * ((float)rand() / RAND_MAX);
                    spheres[index].albedo.b = 0.5f + 0.5f * ((float)rand() / RAND_MAX);
                    spheres[index].fuzziness = 0.1f * ((float)rand() / RAND_MAX);
                    break;
                    
                case DIELECTRIC:
                    spheres[index].albedo.r = 1.0f;
                    spheres[index].albedo.g = 1.0f;
                    spheres[index].albedo.b = 1.0f;
                    spheres[index].refractionIndex = 1.1f + 2.9f * ((float)rand() / RAND_MAX);
                    break;

                default:
                    printf("ERROR::BAD_SPHERE_MATERIAL: %d\n", spheres[index].material);
                    exit(EXIT_FAILURE);
                    break;
            }
            spheres[index].radius = radius;
        }
    }
}
