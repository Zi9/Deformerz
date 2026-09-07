#define LIBTEREP_INTERNAL
#include "TerepCar.h"
#include "LibTerep.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

static bool _ParseChunk1(TerepCar* car, TerepDat* dat)
{
    dat->cur = dat->data + U16(dat->data, 0);
    car->pointCount = U16(dat->cur, 0);
    car->points = calloc(car->pointCount, sizeof(TerepCarPoint));
    if (!car->points) {
        LTERROR("Failed to allocate memory for points\n");
        return false;
    }
    dat->cur += 2;
    for (size_t i = 0; i < car->pointCount; i++) {
        car->points[i].index = i;
        car->points[i].unknown[0] = I16(dat->cur, 0);
        car->points[i].pos[0] = I16(dat->cur, 2) / SCALE;
        car->points[i].unknown[1] = I16(dat->cur, 8);
        car->points[i].pos[1] = I16(dat->cur, 10) / SCALE;
        car->points[i].unknown[2] = I16(dat->cur, 4);
        car->points[i].pos[2] = I16(dat->cur, 6) / SCALE;
        uint16_t size = I16(dat->cur, 24);
        car->points[i].size = size > 0 ? size / SCALE : size;
        car->points[i].type = I16(dat->cur, 26);
        if (car->points[i].type > 2 && car->points[i].type != 65535) {
            LTERROR("Failure parsing %s -- Unknown type point: %i\n", dat->name, car->points[i].type);
            return false;
        }
        dat->cur += 28;
    }
    LTINFO("Loaded %d points\n", car->pointCount);
    return true;
}

static bool _ParseChunk2(TerepCar* car, TerepDat* dat)
{
    dat->cur = dat->data + U16(dat->data, 2);
    car->physLinkCount = U16(dat->cur, 0);
    car->physLinks = calloc(car->physLinkCount, sizeof(TerepCarPhysLink));
    if (!car->points) {
        LTERROR("Failed to allocate memory for links\n");
        return false;
    }
    dat->cur += 2;
    for (size_t i = 0; i < car->physLinkCount; i++) {
        car->physLinks[i].pointA = &car->points[U16(dat->cur, 0)];
        car->physLinks[i].pointB = &car->points[U16(dat->cur, 2)];
        car->physLinks[i].len = U16(dat->cur, 4) / PHYS_LINK_SCALE;
        car->physLinks[i].len2 = U16(dat->cur, 6) / PHYS_LINK_SCALE;
        car->physLinks[i].type = U16(dat->cur, 8);
        car->physLinks[i].len_min = U16(dat->cur, 10) / PHYS_LINK_SCALE;
        car->physLinks[i].len_max = U16(dat->cur, 12) / PHYS_LINK_SCALE;
        if (car->physLinks[i].type != 0 && car->physLinks[i].type != 1 && car->physLinks[i].type != 4 &&
            car->physLinks[i].type != 6 && car->physLinks[i].type != 10 && car->physLinks[i].type != 12) {
            LTERROR("Failure parsing %s -- Unknown type physics link: %i\n", dat->name,
                   car->physLinks[i].type);
            return false;
        }
        if (car->physLinks[i].len != car->physLinks[i].len2) {
            LTWARN("Physics link length values do not match, car may be unstable\n ");
        }
        dat->cur += 14;
    }
    LTINFO("Loaded %d physics links\n", car->physLinkCount);
    return true;
}

static bool _ParseChunk3(TerepCar* car, TerepDat* dat)
{
    dat->cur = dat->data + U16(dat->data, 4);
    car->renderDataCount = 0;
    while (dat->cur < dat->data + dat->size) {
        car->renderData = realloc(car->renderData, (car->renderDataCount + 1) * sizeof(TerepCarRenderDataItem));
        if (!car->renderData) {
            LTERROR("Failed to allocate memory for render data\n");
            return false;
        }
        car->renderData[car->renderDataCount].type = U8(dat->cur, 0);
        dat->cur++;
        switch (car->renderData[car->renderDataCount].type) {
        case TEREP_RENDERDATA_NULL:
            break;
        case TEREP_RENDERDATA_CAMERA: {
            TerepCarCameraData* cam = calloc(1, sizeof(TerepCarCameraData));
            if (!cam) {
                LTERROR("Failed to allocate memory for camera data\n");
                return false;
            }
            car->renderData[car->renderDataCount].camera = cam;
            cam->cameraPoint = &car->points[U16(dat->cur, 0) >> 1];
            if (cam->cameraPoint->type != TEREP_POINT_CAMERA) {
                LTERROR("Failure parsing %s -- Chunk3 -> Camera point (id 0x1) index is not a camera "
                       "point, "
                       "read index %i\n",
                       dat->name, cam->cameraPoint->index);
                return false;
            }
            cam->unknown1 = U8(dat->cur, 2);
            cam->unknown2 = U8(dat->cur, 3);
            dat->cur += 4;
            break;
        }
        case TEREP_RENDERDATA_UNK3_POLYGON: {
            // possibly some culling thing, changing these values seems to do render glitches
            TerepCarPolygonData* polygon = calloc(1, sizeof(TerepCarPolygonData));
            if (!polygon) {
                LTERROR("Failed to allocate memory for polygon data\n");
                return false;
            }
            car->renderData[car->renderDataCount].polygon = polygon;
            polygon->vertexCount = 3;
            polygon->vertices[0] = &car->points[U16(dat->cur, 0) >> 1];
            polygon->vertices[1] = &car->points[U16(dat->cur, 2) >> 1];
            polygon->vertices[2] = &car->points[U16(dat->cur, 4) >> 1];
            polygon->unknown3values[0] = U16(dat->cur, 6);
            polygon->unknown3values[1] = U16(dat->cur, 8);
            polygon->unknown3values[2] = U16(dat->cur, 10);
            dat->cur += 12;
            break;
        }
        case TEREP_RENDERDATA_COLOR_POLYGON: {
            TerepCarPolygonData* polygon = calloc(1, sizeof(TerepCarPolygonData));
            if (!polygon) {
                LTERROR("Failed to allocate memory for polygon data\n");
                return false;
            }
            car->renderData[car->renderDataCount].polygon = polygon;
            polygon->vertexCount = U8(dat->cur, 0);
            dat->cur++;
            for (size_t i = 0; i < polygon->vertexCount; i++) {
                if (i == 0) {
                    polygon->isProjectedOnGround = (U16(dat->cur, 2 * i) & 1);
                } else if (polygon->isProjectedOnGround != (U16(dat->cur, 2 * i) & 1)) {
                    LTWARN("One of the polygons has inconsistent 'snap to ground' mapping, this is "
                           "awful...\n");
                }
                polygon->vertices[i] = &car->points[U16(dat->cur, 2 * i) >> 1];
            }
            polygon->closed = U16(dat->cur, 0) == U16(dat->cur, 2 * polygon->vertexCount);
            polygon->colors[0] = U8(dat->cur, 2 * polygon->vertexCount + 2);
            polygon->colors[1] = U8(dat->cur, 2 * polygon->vertexCount + 3); // NOTE: Dithering
            dat->cur += polygon->vertexCount * 2 + 4;
            break;
        }
        case TEREP_RENDERDATA_TEXTURE_POLYGON: {
            TerepCarPolygonData* polygon = calloc(1, sizeof(TerepCarPolygonData));
            if (!polygon) {
                LTERROR("Failed to allocate memory for polygon data\n");
                return false;
            }
            car->renderData[car->renderDataCount].polygon = polygon;
            polygon->vertexCount = U8(dat->cur, 0);
            dat->cur++;
            for (size_t i = 0; i < polygon->vertexCount; i++) {
                if (i == 0) {
                    polygon->isProjectedOnGround = (U16(dat->cur, 2 * i * 3) & 1);
                } else if (polygon->isProjectedOnGround != (U16(dat->cur, 2 * i * 3) & 1)) {
                    LTWARN("One of the polygons has inconsistent 'snap to ground' mapping, this is "
                           "awful...\n");
                }
                polygon->vertices[i] = &car->points[U16(dat->cur, 2 * i * 3) >> 1];
                polygon->uv[i].x = U16(dat->cur, (2 * i * 3) + 2) / UV_SCALE;
                polygon->uv[i].y = U16(dat->cur, (2 * i * 3) + 4) / UV_SCALE;
            }
            polygon->closed = U16(dat->cur, 0) == U16(dat->cur, 2 * 3 * polygon->vertexCount);
            dat->cur += (polygon->vertexCount + 1) * 3 * 2;
            break;
        }
        case TEREP_RENDERDATA_WHEEL: {
            TerepCarWheelData* wheel = calloc(1, sizeof(TerepCarWheelData));
            if (!wheel) {
                LTERROR("Failed to allocate memory for wheel data\n");
                return false;
            }
            car->renderData[car->renderDataCount].wheel = wheel;
            wheel->wheelPoint = &car->points[U16(dat->cur, 0) >> 1];
            wheel->unknown1 = U16(dat->cur, 2);
            wheel->unknown2 = U16(dat->cur, 4);
            dat->cur += 3 * 2;
            for (int i = 0; i < 9; i++) {
                wheel->wheelSprites[i].sz_height = U16(dat->cur, 0);
                wheel->wheelSprites[i].sz_width = U16(dat->cur, 2);
                dat->cur += 4;
                for (int j = 0; j < 4; j++) {
                    wheel->wheelSprites[i].UV[j].x = U16(dat->cur, 0) / UV_SCALE;
                    wheel->wheelSprites[i].UV[j].y = U16(dat->cur, 2) / UV_SCALE;
                    dat->cur += 4;
                }
            }
            break;
        }
        default:
            LTERROR("Failure parsing % s-- Chunk3->Unknown data block %d\n ", dat->name,
                   car->renderData[car->renderDataCount].type);
            return false;
        }
        car->renderDataCount++;
    }
    LTINFO("Loaded %i render data items\n", car->renderDataCount);
    return true;
}

TerepCar* TerepCar_Load(const char* cardat, const char* carpcx)
{
    TerepCar* ret = NULL;
    TerepCar* car = calloc(1, sizeof *car);
    if (!car) {
        LTERROR("Failed to allocate memory for TerepCar\n");
        return NULL;
    }

    TerepDat* dat = _LoadDat(cardat);
    if (!dat) {
        goto CLEANERR;
    }
    // TODO: Attempt to detect the Terep1 dat format and load that too

    car->unknownHeaderValue1 = U16(dat->data, 6);
    car->engineSound = U16(dat->data, 8);

    bool ok;
    ok = _ParseChunk1(car, dat);
    if (!ok) {
        goto CLEANERR;
    }
    ok = _ParseChunk2(car, dat);
    if (!ok) {
        goto CLEANERR;
    }
    ok = _ParseChunk3(car, dat);
    if (!ok) {
        goto CLEANERR;
    }
    LTINFO("Finished loading %s (%zu bytes)\n", dat->name, dat->size);

    if (carpcx != NULL) {
        car->carTexture = PCX_LoadImage(carpcx);
        if (!car->carTexture) {
            goto CLEANERR;
        }
        LTINFO("Loaded car texture %s\n", carpcx);
    } else {
        LTINFO("No texture provided for loading\n");
    }

    ret = car;
    _UnloadDat(dat);
    return ret;

CLEANERR:
    _UnloadDat(dat);
    free(car);
    return NULL;
}
