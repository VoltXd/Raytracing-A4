#include "raytracing.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "utils.h"

void raytracing(color_t *image, const uint16_t width, const uint16_t height, const uint16_t raysPerPixel, const uint8_t raysDepth, const sphere_t* spheres, const uint8_t numberOfSpheres, const camera_t* camera)
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
    
    uint16_t i, j, k, rayIdx, depthIdx;
    #pragma omp parallel for private(i, k, rayIdx, depthIdx)
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            // Direction (Let's assume cam.dir.z == -1 or 1)
            color_t pixelColor = (color_t){ 0.0f, 0.0f, 0.0f };
            uint32_t seed = i  + width * j;
            for (rayIdx = 0; rayIdx < raysPerPixel; rayIdx++)
            {
                // Ray
                vec3_t rayPosition = camera->position;
                vec3_t rayDirection;
                rayDirection.x = dirHorizontalMin + (i + randomFloatInUnitInterval(&seed)) * stepHorizontal;
                rayDirection.y = dirVecticalMin + (j + randomFloatInUnitInterval(&seed)) * stepVertical;
                rayDirection.z = camera->direction.z;
                color_t rayColor = { 1.0f, 1.0f, 1.0f };

                // Sky
                float skyGradiant = 0.5f * (rayDirection.y + 1.0f);
                const color_t skyColor = (color_t){ (1.0f - skyGradiant)*1.0f + skyGradiant*0.5f, (1.0f - skyGradiant)*1.0f + skyGradiant*0.7f, (1.0f- skyGradiant)*1.0f + skyGradiant*1.0f };
                uint8_t isSkyHit = 0;

                for (depthIdx = 0; depthIdx < raysDepth && !isSkyHit; depthIdx++)
                {
                    float closestSphereDistance = INFINITY;
                    int8_t closestSphereIndex = -1;
                    for (k = 0; k < numberOfSpheres; k++)
                    {   
                        vec3_t originToCenter = vec3_sub(&rayPosition, &spheres[k].position);
                        float a = vec3_dot(&rayDirection, &rayDirection);
                        float half_b = vec3_dot(&originToCenter, &rayDirection);
                        float c = vec3_dot(&originToCenter, &originToCenter) - spheres[k].radius * spheres[k].radius;
                        float delta = half_b*half_b - a*c;

                        if (delta >= 0)
                        {
                            float newDistance = (-half_b - sqrtf(delta))/ a;

                            if (newDistance <=  0.001f)
                                continue;

                            if (newDistance < closestSphereDistance)
                            {
                                closestSphereDistance = newDistance;
                                closestSphereIndex = k;
                            }
                        }
                    }

                    if (closestSphereIndex != -1)
                    {
                        // Ray-sphere hit position
                        vec3_t hitPosition = vec3_scalarMul_return(&rayDirection, closestSphereDistance);
                        hitPosition = vec3_add(&rayPosition, &hitPosition);

                        // Get sphere normal on hit position
                        vec3_t sphereNormal = vec3_sub(&hitPosition, &spheres[closestSphereIndex].position);
                        vec3_scalarMul(&sphereNormal, 1.0f / spheres[closestSphereIndex].radius);   

                        // Which face hit ?
                        uint8_t isFrontFace = vec3_dot(&rayDirection, &sphereNormal) > 0 ? 0 : 1;

                        // The ray hit a sphere => Update ray position + direction & add color info
                        rayPosition = hitPosition;
                        switch (spheres[closestSphereIndex].material)
                        {
                            case LAMBERTIAN:
                                // Bad hemisphere diffusion  
                                // rayDirection = randomInHemisphere(&seed, &sphereNormal); 
                                
                                // True Lambertian diffusion
                                rayDirection = randomUnitVector(&seed);
                                vec3_scalarMul(&rayDirection, spheres[closestSphereIndex].roughness);
                                rayDirection = vec3_add(&rayDirection, &sphereNormal);      
                                if (vec3_isNearZero(&rayDirection))
                                    rayDirection = sphereNormal;
                                break;
                                
                            case METAL:
                            {
                                vec3_t roughnessVector = randomUnitVector(&seed);
                                vec3_scalarMul(&roughnessVector, spheres[closestSphereIndex].fuzziness);
                                rayDirection = vec3_reflect(&rayDirection, &sphereNormal);
                                rayDirection = vec3_add(&rayDirection, &roughnessVector);
                                break;
                            }

                            case DIELECTRIC:
                            {

                                float refractionRatio = isFrontFace ? 1.0f / spheres[closestSphereIndex].refractionIndex : spheres[closestSphereIndex].refractionIndex;
                                vec3_t uRayDirection = rayDirection;
                                vec3_normalize(&uRayDirection);
                                rayDirection = vec3_refract(&uRayDirection, &sphereNormal, refractionRatio, &seed);
                                break;
                            }
                            
                            default:
                                printf("ERROR::BAD_SPHERE_MATERIAL: %d\n", spheres[closestSphereIndex].material);
                                exit(EXIT_FAILURE);
                                break;
                        }
                        rayColor = color_mul(&rayColor, &spheres[closestSphereIndex].albedo);
                    }
                    else
                    {
                        // The ray hit the sky, the loop must stop
                        isSkyHit = 1;
                        rayColor = color_mul(&rayColor, &skyColor);
                    }
                }

                if (isSkyHit)
                    pixelColor = color_add(&pixelColor, &rayColor);
                else 
                    pixelColor = color_add(&pixelColor, &BLACK);
            }
            color_scalarMul(&pixelColor, inv_raysPerPixel);
            image[i + j * width] = pixelColor;
        }
    }
}
