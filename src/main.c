#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "stb_image_write.h"
#include "vec3_color.h"
#include "utils.h"  


int main(int argc, char* argv[])
{
    // Arguments verification
    if (argc != 3)
    {
        printf("ERROR::BAD_ARGUMENTS -> PATH WIDTH HEIGHT\n");
        return EXIT_FAILURE;
    }

    const uint16_t WIDTH = atoi(argv[1]);
    const uint16_t HEIGHT = atoi(argv[2]);
    
    if (!WIDTH || !HEIGHT)
    {
        printf("ERROR:BAD_ARGUMENTS_VALUE -> Must be Non-Zero INTEGER");
        return EXIT_FAILURE;
    }

    // ****************** Hello image ****************** //
    // STBI configuration
    stbi_flip_vertically_on_write(1);

    // Pixels allocation
    color_t* colors = malloc(WIDTH * HEIGHT * sizeof(color_t));

    // Image gradient 
    uint16_t i, j;
    const float INV_WIDTH_MINUS_1 = 1.0f / (WIDTH - 1.0f);
    const float INV_HEIGHT_MINUS_1 = 1.0f / (HEIGHT - 1.0f);
    for (j = 0; j < HEIGHT; j++)
    {
        for (i = 0; i < WIDTH; i++)
        {
            colors[j*WIDTH + i].r = i * INV_WIDTH_MINUS_1;
            colors[j*WIDTH + i].g = j * INV_HEIGHT_MINUS_1;
            colors[j*WIDTH + i].b = 0;
        }
    }

    // Output image allocation & copy
    color_u8_t* colors_u8 = malloc(WIDTH * HEIGHT * sizeof(color_u8_t));
    imageFloatToU8(colors, colors_u8, WIDTH * HEIGHT);
    
    // Release float image
    free(colors);

    // Save image
    stbi_write_png("test.png", WIDTH, HEIGHT, 3, colors_u8, WIDTH * 3);

    // Destroy image
    free(colors_u8);
    return EXIT_SUCCESS;
}