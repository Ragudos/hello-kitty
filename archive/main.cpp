#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#include "stb_image/stb_image_resize.h"

const int SCREEN_WIDTH = 200;
const int SCREEN_HEIGHT = 100;
const int ASCII_LUMINOSITY_LENGTH = 60;
/*
 * Ascii characters that give off a darker or lighter luminosity.
 * The order is lightest (first) to darkest (last).
 */
const char ASCII_LUMINOSITY[ASCII_LUMINOSITY_LENGTH] = {
    '@', '#', '%', '8', '&', 'M', 'W', '*', 'o', '0', 'H', 'Q', 'U', 'A', 'O',
    'X', 'q', 'd', 'b', 'k', 'x', 'z', 'u', 'n', 'c', 'v', 'r', 'j', 'f', 't', '/',
    '|', '(', ')', '1', '{', '}', '[', ']', '?', '+', '~', '-', 'i', '!', 'l', 'I',
    ';', ':', ',', '"', '^', '`', '\'', '.', ' '};

uint8_t *load_image(const char *input_path, int *desired_width, int *desired_height, bool *resize_image)
{
    const int channels = STBI_rgb;
    int width, height;

    uint8_t *image = stbi_load(input_path, &width, &height, NULL, channels);

    if (image == NULL)
    {
        fprintf(stderr, "Could not load image \n");
        exit(EXIT_FAILURE);
    }

    if (*resize_image)
    {
        if (*desired_width <= 0)
        {
            fprintf(stderr, "Argument 'width' must be greater than 0 \n");
            exit(EXIT_FAILURE);
        }
        else if (*desired_width > width)
        {
            fprintf(stderr, "Argument 'width' can not be greater than the original image width (%ipx) \n", width);
            exit(EXIT_FAILURE);
        }

        *desired_height = height / (width / (float)*desired_width);

        stbir_resize_uint8(
            image, width, height, width * channels,
            image, *desired_width, *desired_height, *desired_width * channels, channels);
    }
    else
    {
        *desired_width = width;
        *desired_height = height;
    }

    return image;
}

uint8_t get_intensity(uint8_t *image, int i)
{
    const int channels = 3;
    uint8_t *pixel_offset = image + i * channels;
    uint8_t r = pixel_offset[0];
    uint8_t g = pixel_offset[1];
    uint8_t b = pixel_offset[2];

    return (uint8_t)round(0.299 * r + 0.587 * g + 0.114 * b);
}

void get_rgb(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *image, int i)
{
    const int channels = 3;
    uint8_t *pixel_offset = image + i * channels;

    *r = pixel_offset[0];
    *g = pixel_offset[1];
    *b = pixel_offset[2];
}

int main()
{
    int d_w = 360;
    int d_h = 360;
    bool r = true;
    char *characters = strdup("$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ");

    uint8_t *image = load_image("image.png", &d_w, &d_h, &r);

    int length = (d_h * d_w + d_h) * 81 + 1;
    char *output = (char *)malloc(length * sizeof(char));

    int characters_count = strlen(characters);
    int ptr = 0;

    uint8_t r_prev, g_prev, b_prev;

    for (int i = 0; i < d_h * d_w; ++i)
    {
        int intensity = get_intensity(image, i);
        int char_index = intensity / (255 / (float)(characters_count - 1));

        uint8_t r, g, b;

        get_rgb(&r, &g, &b, image, i);

        if (!(r == r_prev && g == g_prev && b == b_prev))
        {
            ptr += snprintf(output + ptr, length - ptr, "\e[38;2;%i;%i;%im]", r, g, b);
        }

        r_prev = r;
        g_prev = g;
        b_prev = b;

        output[ptr++] = characters[char_index];

        if ((i + 1) % d_w == 0)
        {
            output[ptr++] = '\n';
        }
    }

    snprintf(output + ptr, length - ptr, "\e[0m");

    std::cout << output;

    stbi_image_free(image);
    free(output);
    free(characters);

    return 0;
}
