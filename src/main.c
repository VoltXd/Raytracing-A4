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
    if (argc != 5)
    {
        printf("ERROR::BAD_ARGUMENTS -> PATH WIDTH HEIGHT RAYS_PER_PIXEL NUMBER_OF_SPHERES\n");
        return EXIT_FAILURE;
    }

    const uint16_t WIDTH = atoi(argv[1]);
    const uint16_t HEIGHT = atoi(argv[2]);
    const uint16_t RAYS_PER_PIXEL = atoi(argv[3]);
    const uint8_t NUMBER_OF_SPHERES = atoi(argv[4]);
    if (!WIDTH || !HEIGHT || !RAYS_PER_PIXEL || !NUMBER_OF_SPHERES)
    {
        printf("ERROR:BAD_ARGUMENTS_VALUE -> Must be Non-Zero INTEGER\n");
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
    camera.fov = M_PI * 0.5;

    sphere_t spheres[NUMBER_OF_SPHERES];

    // Ground
    spheres[0].position = (vec3_t){ 0.0f, -1000.0f, 0.0f };
    spheres[0].albedo = (color_t){ 0.5f, 0.5f, 0.5f };
    spheres[0].radius = 1000.0f;
    // spheres[0].position = (vec3_t){ 0.0f, 1.0f, 0.0f };
    // spheres[0].albedo = (color_t){ 0.5f, 0.5f, 0.5f };
    // spheres[0].radius = 0.5f;

    // Spheres
    uint8_t i;
    for (i = 1; i < NUMBER_OF_SPHERES; i++)
    {   
        float radius = 0.2f + 0.4f * ((float)rand() / RAND_MAX);

        spheres[i].position.x = -5.0f + 10.0f * ((float)rand() / RAND_MAX);
        spheres[i].position.y = radius;
        spheres[i].position.z = -5.0f + 10.f * ((float)rand() / RAND_MAX);
        spheres[i].albedo.r = ((float)rand() / RAND_MAX);
        spheres[i].albedo.g = ((float)rand() / RAND_MAX);
        spheres[i].albedo.b = ((float)rand() / RAND_MAX);
        spheres[i].radius = radius;
    }

    // **************** Trace rays **************** //
    raytracing(image_f, WIDTH, HEIGHT, RAYS_PER_PIXEL, spheres, NUMBER_OF_SPHERES, &camera);
    

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