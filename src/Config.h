#pragma once

#include <raylib.h>
#include <stdint.h>

#define GAME_WINDOW_TITLE "Deformers - A recreation of Terep2"
#define WMARK "Deformers - Development Build"

typedef struct Config {
    struct {
        uint16_t width;
        uint16_t height;
        uint8_t upscaleMultiplier;
        uint8_t targetFPS;
    } render;
    char* baseDataPath;
    char* scenario;
    bool restart;
} Config;

Config Config_Terep();
Config Config_Default();