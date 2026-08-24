#include "DFCar.h"
#include "LibTerep/TerepCar.h"
#include <raylib.h>

#include <assert.h>
#include <stdlib.h>

DFCar* DFCar_Load()
{
    DFCar* dfcar = calloc(1, sizeof(DFCar));
    assert(dfcar);
    dfcar->car = TerepCar_Load("./data/car1.dat", "./data/car1.pcx");
    dfcar->carTex = LoadTextureFromImage((Image){
        .data = dfcar->car->carTexture->data,
        .height = dfcar->car->carTexture->height,
        .width = dfcar->car->carTexture->width,
        .format = 7,
        .mipmaps = 1,
    });
    return dfcar;
}
void DFCar_Unload(DFCar* dfcar)
{
    TerepCar_Unload(dfcar->car);
    UnloadTexture(dfcar->carTex);
    free(dfcar);
}
