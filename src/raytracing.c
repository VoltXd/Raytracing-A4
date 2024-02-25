#include "raytracing.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "utils.h"

void raytracing(color_t* image, const uint16_t width, const uint16_t height, const uint16_t raysPerPixel, const uint8_t raysDepth, const sphere_t* spheres, const uint16_t numberOfSpheres, const camera_t* camera)
{
    // Rays weight
    const float inv_raysPerPixel = 1.0f / raysPerPixel;
    
    uint16_t i, j, k, rayIdx, depthIdx;
    #pragma omp parallel for schedule(dynamic) private(i, k, rayIdx, depthIdx)
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            // Pixel initialisation
            color_t pixelColor = (color_t){ 0.0f, 0.0f, 0.0f };
            uint32_t seed = i  + width * j;

            for (rayIdx = 0; rayIdx < raysPerPixel; rayIdx++)
            {
                // Pixel position (Ray position in viewport plane)
                vec3_t pixelPosition_u = vec3_scalarMul_return(&camera->step_u, i + randomFloatInUnitInterval(&seed));
                vec3_t pixelPosition_v = vec3_scalarMul_return(&camera->step_v, j + randomFloatInUnitInterval(&seed));
                vec3_t pixelPosition = camera->viewportUpperLeft;
                pixelPosition = vec3_add(&pixelPosition, &pixelPosition_u);
                pixelPosition = vec3_add(&pixelPosition, &pixelPosition_v);

                // Ray Initialisation
                vec3_t rayPosition = (camera->defocusAngle <= 0.0f) ? camera->lookFrom : randomDefocusedRayPosition(&seed, &camera->lookFrom, &camera->defocus_disk_u, &camera->defocus_disk_v);
                vec3_t rayDirection = vec3_sub(&pixelPosition, &rayPosition);
                color_t rayColor = { 1.0f, 1.0f, 1.0f };

                // Bounce loop
                uint8_t isSkyHit = 0;
                for (depthIdx = 0; depthIdx < raysDepth && !isSkyHit; depthIdx++)
                {
                    // Iterate through spheres to get the closest one
                    float closestSphereDistance = INFINITY;
                    int16_t closestSphereIndex = -1;
                    for (k = 0; k < numberOfSpheres; k++)
                    {   
                        // Maths (line == sphere equation)
                        vec3_t originToCenter = vec3_sub(&rayPosition, &spheres[k].position);
                        float a = vec3_dot(&rayDirection, &rayDirection);
                        float half_b = vec3_dot(&originToCenter, &rayDirection);
                        float c = vec3_dot(&originToCenter, &originToCenter) - spheres[k].radius * spheres[k].radius;
                        float delta = half_b*half_b - a*c;

                        // 1 or 2 solutions => sphere hit
                        if (delta >= 0)
                        {
                            float newDistance = (-half_b - sqrtf(delta))/ a;

                            // Ignore digital noise
                            if (newDistance <=  0.001f)
                                continue;

                            // Update sphere
                            if (newDistance < closestSphereDistance)
                            {
                                closestSphereDistance = newDistance;
                                closestSphereIndex = k;
                            }
                        }
                    }

                    // If sphere hit, else sky hit
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
                        float skyGradiant = 0.5f * (rayDirection.y / vec3_magnitude(&rayDirection) + 1.0f);
                        const color_t skyColor = (color_t){ (1.0f - skyGradiant)*1.0f + skyGradiant*0.5f, (1.0f - skyGradiant)*1.0f + skyGradiant*0.7f, (1.0f- skyGradiant)*1.0f + skyGradiant*1.0f };
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
