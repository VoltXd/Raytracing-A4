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
        printf("ERROR::BAD_ARGUMENTS -> PATH WIDTH HEIGHT RAYS_PER_PIXEL RAYS_DEPTH SQRT_NUMBER_OF_SPHERES\n");
        return EXIT_FAILURE;
    }

    const uint16_t WIDTH = atoi(argv[1]);
    const uint16_t HEIGHT = atoi(argv[2]);
    const uint16_t RAYS_PER_PIXEL = atoi(argv[3]);
    const uint8_t RAYS_DEPTH = atoi(argv[4]);
    const uint8_t SQRT_NUMBER_OF_SPHERES = atoi(argv[5]);
    if (!WIDTH || !HEIGHT || !RAYS_PER_PIXEL || !RAYS_DEPTH || !SQRT_NUMBER_OF_SPHERES)
    {
        printf("ERROR:BAD_ARGUMENTS_VALUE -> Must be Non-Zero INTEGER\n");
        return EXIT_FAILURE;
    }


    // ****************** Hello image ****************** //
    // Configuration
    stbi_flip_vertically_on_write(1);
    srand(time(NULL));

    // Pixels allocation
    printf("Allocating image pixels.");
    color_t* image_f = malloc(WIDTH * HEIGHT * sizeof(color_t));
    printf("\tDone!\n");

    // Camera
    printf("Initialising camera.");
    camera_t camera = initializeCamera(WIDTH, HEIGHT);
    printf("\t\tDone!\n");
    
    // Spheres
    printf("Initialising spheres.");
    const uint16_t NUMBER_OF_SPHERES = SQRT_NUMBER_OF_SPHERES * SQRT_NUMBER_OF_SPHERES + 4; 
    sphere_t* spheres = malloc(NUMBER_OF_SPHERES  * sizeof(sphere_t));
    initializeSpheres(spheres, SQRT_NUMBER_OF_SPHERES);
    printf("\t\tDone!\n");
    

    // **************** Trace rays **************** //
    // Time measure
    struct timeval start, end;
    printf("Trace rays!");
    fflush(stdout);
    gettimeofday(&start, NULL);

    // Render image
    raytracing(image_f, WIDTH, HEIGHT, RAYS_PER_PIXEL, RAYS_DEPTH, spheres, NUMBER_OF_SPHERES, &camera);

    // Elapsed time
    gettimeofday(&end, NULL);
    printf("\t\t\tDone!\n");
    
    uint64_t elapsedTime = (end.tv_sec - start.tv_sec) * 1000000ull + (end.tv_usec - start.tv_usec);
    printf("Raytracing elapsed time: %lu us\n", elapsedTime);
    printf("Cycles per pixel: %f\n", elapsedTime * 2.8e3f / (WIDTH * HEIGHT));

    // Free spheres memory
    free(spheres);

    // Linear space to Gamma space tranformation
    imageLinearToGamma(image_f, WIDTH * HEIGHT);

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