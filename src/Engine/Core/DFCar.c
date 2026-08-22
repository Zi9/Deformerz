#include "DFCar.h"
#include "LibTerep/PCX.h"
#include "LibTerep/TerepCar.h"
#include "Engine/Engine.h"
#include <raylib.h>
#include <stdlib.h>

static void grabPalette(PCXData* pcx)
{
    for (size_t i = 0; i < 256; i++)
    {
        Engine.palette[i].r = pcx->palette[i].red;
        Engine.palette[i].g = pcx->palette[i].green;
        Engine.palette[i].b = pcx->palette[i].blue;
        Engine.palette[i].a = 255;
    }
}

DFCar* DFCar_Load()
{
    DFCar* dfcar = malloc(sizeof(DFCar));
    PCX_postprocess_callback = grabPalette;
    dfcar->car = TerepCar_Load("./data/car1.dat", "./data/car1.pcx");
    PCX_postprocess_callback = NULL;
    Image img = *(Image*)dfcar->car->carTexture;
    dfcar->carTex = LoadTextureFromImage(img);
    return dfcar;
}
void DFCar_Unload(DFCar* dfcar)
{
    TerepCar_Unload(dfcar->car);
    UnloadTexture(dfcar->carTex);
    free(dfcar);
}
