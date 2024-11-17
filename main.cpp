#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION

#include "stb_image/stb_image.h"
#include "stb_image/stb_image_resize.h"

uint8_t get_intensity(uint8_t *image, int i)
{
    const int channels = STBI_rgb;

    uint8_t *pixel_offset = image + i * channels;
    uint8_t r = pixel_offset[0];
    uint8_t g = pixel_offset[1];
    uint8_t b = pixel_offset[2];

    // Each floating point represent the weight of the specific color.
    // All of these weights add to 1. If all color values are 255, it'll
    // return 255.
    return (uint8_t)round(0.299 * r + 0.587 * g + 0.114 * b);
}

int main()
{
    char *characters = strdup("$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ");
    int characters_count = strlen(characters);
    const int channels = STBI_rgb;
    int width, height;

    uint8_t *image = stbi_load("HK.png", &width, &height, NULL, channels);

    if (image == NULL)
    {
        fprintf(stderr, "Could not load image\n");
        exit(EXIT_FAILURE);
    }

    // Add height to w x h to accomodate for line breaks and 1 to
    // accomodate null termination character.
    int output_size = width * height + height + 1;
    char *output = (char *)malloc(output_size * sizeof(char));

    int ptr = 0; // Output str index

    for (int i = 0; i < width * height; ++i)
    {
        int intensity = get_intensity(image, i);
        // Since 255 is the max intensity, we just divide it by the amount of characters available
        // per each intensity. Then, divide the intensity of the pixel to that amount. If we get an intensity of 255,
        // we'll get the last character, which is a white space.
        int char_index = intensity / (255 / (float)(characters_count - 1));

        output[ptr] = characters[char_index];

        if ((i + 1) % width == 0)
        {
            output[++ptr] = '\n';
        }

        ptr++;
    }

    output[ptr] = '\0';

    std::cout << output;

    stbi_image_free(image);
    free(output);
    free(characters);

    return 0;
}
