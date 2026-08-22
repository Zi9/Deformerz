#pragma once

#include <stdint.h>

struct __attribute__((__packed__)) RGBColor {
    uint8_t red, green, blue;
};
struct __attribute__((__packed__)) RGBAColor {
    uint8_t red, green, blue, alpha;
};

typedef struct {
    uint8_t* indices;
    struct RGBColor palette[256];
} PCXData;

typedef struct {
    void* data;
    int width;
    int height;
    int mipmaps;
    int format;
} PCXImage;

extern void (*PCX_postprocess_callback)(PCXData *);

uint8_t* PCX_LoadArray(const char* path);
PCXImage* PCX_LoadImage(const char* path);