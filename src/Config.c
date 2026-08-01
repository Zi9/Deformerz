#include "Config.h"

Config Config_Terep()
{
    Config cfg = {
        .render = {.width = 320, .height = 200, .upscaleMultiplier = 2},
        .baseDataPath = "./data/terep/",
        .scenario = "terep",
        .skyColor = (Color){93, 199, 255, 255},
        .restart = false,
    };
    return cfg;
}

Config Config_Default()
{
    Config cfg = Config_Terep();
    cfg.render.width = 1920;
    cfg.render.height = 1080;
    cfg.render.upscaleMultiplier = 1;
    return cfg;
}
