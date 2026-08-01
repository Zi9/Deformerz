#pragma once

#include <raylib.h>
#include <stdint.h>

uint8_t* PCX_LoadArray(const char* path);
Image PCX_LoadImage(const char* path);