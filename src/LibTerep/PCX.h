#pragma once

#include <stdint.h>

struct __attribute__((__packed__)) RGBColor {
    uint8_t red, green, blue;
};

struct __attribute__((__packed__)) RGBAColor {
    uint8_t red, green, blue, alpha;
};

typedef struct {
    uint8_t* data;
    struct RGBColor palette[256];
} PCXData;

typedef struct {
    void* data;
    struct RGBColor palette[256];
    int width;
    int height;
} PCXImage;

extern struct RGBColor PCX_GLOBAL_PALETTE[256];

PCXData* PCX_LoadArray(const char* path);
PCXImage* PCX_LoadImage(const char* path);
void PCX_EnableGlobalPalette(const char* path);
void PCX_DisableGlobalPalette();