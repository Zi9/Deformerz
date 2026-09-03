#define LIBTEREP_INTERNAL
#include "TerepCar.h"

#include <stdlib.h>

void TerepCar_Unload(TerepCar* car)
{
    for (int i = 0; i < car->renderDataCount; i++) {
        switch (car->renderData[i].type) {
        case TEREP_RENDERDATA_NULL:
            break;
        case TEREP_RENDERDATA_CAMERA:
            free(car->renderData[i].camera);
            break;
        case TEREP_RENDERDATA_UNK3_POLYGON:
        case TEREP_RENDERDATA_COLOR_POLYGON:
        case TEREP_RENDERDATA_TEXTURE_POLYGON:
            free(car->renderData[i].polygon);
            break;
        case TEREP_RENDERDATA_WHEEL:
            free(car->renderData[i].wheel);
            break;
        }
    }
    free(car->renderData);
    free(car->points);
    free(car->physLinks);
    free(car->carTexture->data);
    free(car->carTexture);
    free(car);
}
