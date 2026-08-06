#pragma once

#include <raylib.h>
#include <stdint.h>

struct __attribute__((__packed__)) RGBColor {
    uint8_t red, green, blue;
};

typedef struct PCXData {
    uint8_t* indices;
    struct RGBColor palette[256];
} PCXData;

extern void (*PCX_postprocess_callback)(PCXData *);

uint8_t* PCX_LoadArray(const char* path);
Image PCX_LoadImage(const char* path);