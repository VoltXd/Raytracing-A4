#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

#include "stb_image_write.h"

#include "vec3_color.h"
#include "utils.h"  
#include "camera.h"
#include "sphere.h"
#include "raytracing.h"


#define CHANNEL_NUM 3


int main(int argc, char* argv[])
{
    // Arguments verification
    if (argc != 6)
    {
        printf("ERROR::BAD_ARGUMENTS -> PATH WIDTH HEIGHT RAYS_PER_PIXEL RAYS_DEPTH NUMBER_OF_SPHERES\n");
        return EXIT_FAILURE;
    }

    const uint16_t WIDTH = atoi(argv[1]);
    const uint16_t HEIGHT = atoi(argv[2]);
    const uint16_t RAYS_PER_PIXEL = atoi(argv[3]);
    const uint8_t RAYS_DEPTH = atoi(argv[4]);
    const uint8_t NUMBER_OF_SPHERES = atoi(argv[5]);
    if (!WIDTH || !HEIGHT || !RAYS_PER_PIXEL || !RAYS_DEPTH || !NUMBER_OF_SPHERES)
    {
        printf("ERROR:BAD_ARGUMENTS_VALUE -> Must be Non-Zero INTEGER\n");
        return EXIT_FAILURE;
    }
    else if (NUMBER_OF_SPHERES < 4)
    {
        printf("ERROR:BAD_ARGUMENTS_VALUE -> NUMBER_OF_SPHERES must be at least 4!\n");
        return EXIT_FAILURE;
    }   


    // ****************** Hello image ****************** //
    // Configuration
    stbi_flip_vertically_on_write(1);
    srand(time(NULL));

    // Pixels allocation
    color_t* image_f = malloc(WIDTH * HEIGHT * sizeof(color_t));

    // Camera
    camera_t camera;
    camera.position = (vec3_t){ 0.0f, 1.0f, 10.0f };
    camera.direction = (vec3_t){ 0.0f, 0.0f, -1.0f };
    camera.fov = degrees_to_radians(45.0f);

    vec3_normalize(&camera.direction);

    sphere_t* spheres = malloc(NUMBER_OF_SPHERES * sizeof(sphere_t));

    // Ground
    spheres[0].radius = 10000.0f;
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

    // Spheres
    uint8_t i;
    for (i = 4; i < NUMBER_OF_SPHERES; i++)
    {   
        float radius = 0.2f + 0.4f * ((float)rand() / RAND_MAX);

        spheres[i].position.x = -5.0f + 10.0f * ((float)rand() / RAND_MAX);
        spheres[i].position.y = radius;
        spheres[i].position.z = -5.0f + 10.f * ((float)rand() / RAND_MAX);
        spheres[i].material = i % MATERIAL_SIZE;
        if (spheres[i].material == DIELECTRIC)
        {
            spheres[i].albedo.r = 1.0f;
            spheres[i].albedo.g = 1.0f;
            spheres[i].albedo.b = 1.0f;
            spheres[i].roughness = 1.0f + 3.0f * ((float)rand() / RAND_MAX);
        }
        else
        {
            spheres[i].albedo.r = ((float)rand() / RAND_MAX);
            spheres[i].albedo.g = ((float)rand() / RAND_MAX);
            spheres[i].albedo.b = ((float)rand() / RAND_MAX);
            spheres[i].roughness = ((float)rand() / RAND_MAX);
        }
        spheres[i].radius = radius;
    }

    // **************** Trace rays **************** //
    // Time measure
    struct timeval start, end;
    gettimeofday(&start, NULL);

    // Render image
    raytracing(image_f, WIDTH, HEIGHT, RAYS_PER_PIXEL, RAYS_DEPTH, spheres, NUMBER_OF_SPHERES, &camera);

    // Elapsed time
    gettimeofday(&end, NULL);
    printf("Raytracing elapsed time: %u us\n", (end.tv_sec - start.tv_sec) * 1000000u + (end.tv_usec - start.tv_usec));

    // Free spheres memory
    free(spheres);

    // Linear space to Gamma space tranformation
    // imageLinearToGamma(image_f, WIDTH * HEIGHT);

    // Output image allocation & copy
    color_u8_t* image_u8 = malloc(WIDTH * HEIGHT * sizeof(color_u8_t));
    imageFloatToU8(image_f, image_u8, WIDTH * HEIGHT);
    
    // Release float image
    free(image_f);

    // Save image
    stbi_write_png("test.png", WIDTH, HEIGHT, CHANNEL_NUM, image_u8, WIDTH * CHANNEL_NUM);

    // Destroy image
    free(image_u8);
    return EXIT_SUCCESS;
}