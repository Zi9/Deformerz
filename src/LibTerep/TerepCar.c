#include "TerepCar.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define SCALE 256.0f
#define PHYS_LINK_SCALE 16384.0f

#define U8(base, ofs) *(uint8_t*)(base + ofs)
#define U16(base, ofs) *(uint16_t*)(base + ofs)
#define I8(base, ofs) *(int8_t*)(base + ofs)
#define I16(base, ofs) *(int16_t*)(base + ofs)

typedef struct {
    uint8_t* data;
    size_t size;
    char name[32];
    uint8_t* cur;
} TerepDat;

static TerepDat* _LoadDat(const char* path)
{
    TerepDat* dat = calloc(1, sizeof(TerepDat));
    assert(dat);
    FILE* f = fopen(path, "rb");
    strncpy(dat->name, path, 32);
    assert(fseek(f, 0, SEEK_END) == 0);
    dat->size = ftell(f);
    assert(fseek(f, 0, SEEK_SET) == 0);
    dat->data = calloc(1, dat->size);
    assert(dat->data);
    assert(fread(dat->data, dat->size, 1, f) == 1);
    fclose(f);
    return dat;
}
static TerepDat* _CreateDat()
{
    TerepDat* dat = calloc(1, sizeof(TerepDat));
    assert(dat);
    dat->data = calloc(1, 10000);
    return dat;
}
static void _UnloadDat(TerepDat* dat)
{
    free(dat->data);
    free(dat);
}

static void _ParseChunk1(TerepCar* car, TerepDat* dat)
{
    dat->cur = dat->data + U16(dat->data, 0);
    car->pointCount = U16(dat->cur, 0);
    car->points = calloc(car->pointCount, sizeof(TerepCarPoint));
    assert(car->points);
    dat->cur += 2;
    for (size_t i = 0; i < car->pointCount; i++) {
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
            printf("LibTerep | ERROR: Failure parsing %s -- Unknown type point: %i\n", dat->name, car->points[i].type);
        }
        dat->cur += 28;
    }
    printf("LibTerep | INFO: Loaded %d points\n", car->pointCount);
}

static void _ParseChunk2(TerepCar* car, TerepDat* dat)
{
    dat->cur = dat->data + U16(dat->data, 2);
    car->physSegmentCount = U16(dat->cur, 0);
    car->physSegments = calloc(car->physSegmentCount, sizeof(TerepCarPhysSegment));
    assert(car->points);
    dat->cur += 2;
    for (size_t i = 0; i < car->physSegmentCount; i++) {
        car->physSegments[i].pointA = U16(dat->cur, 0);
        car->physSegments[i].pointB = U16(dat->cur, 2);
        car->physSegments[i].len = U16(dat->cur, 4) / PHYS_LINK_SCALE;
        car->physSegments[i].len2 = U16(dat->cur, 6) / PHYS_LINK_SCALE;
        car->physSegments[i].type = U16(dat->cur, 8);
        car->physSegments[i].len_min = U16(dat->cur, 10) / PHYS_LINK_SCALE;
        car->physSegments[i].len_max = U16(dat->cur, 12) / PHYS_LINK_SCALE;
        if (car->physSegments[i].type != 0 && car->physSegments[i].type != 1 && car->physSegments[i].type != 4 &&
            car->physSegments[i].type != 6 && car->physSegments[i].type != 10 && car->physSegments[i].type != 12) {
            printf("LibTerep | ERROR: Failure parsing %s -- Unknown type physics segment: %i\n", dat->name,
                   car->physSegments[i].type);
            return;
        }
        if (car->physSegments[i].len != car->physSegments[i].len2)
        {
            printf("XXXX\n");
        }
        dat->cur += 14;
    }
    printf("LibTerep | INFO: Loaded %d physics segments\n", car->physSegmentCount);
}

static void _ParseChunk3(TerepCar* car, TerepDat* dat)
{
    dat->cur = dat->data + U16(dat->data, 4);
    car->renderDataCount = 0;
    while (dat->cur < dat->data + dat->size) {
        car->renderData = realloc(car->renderData, (car->renderDataCount + 1) * sizeof(TerepCarRenderDataItem));
        car->renderData[car->renderDataCount].type = U8(dat->cur, 0);
        dat->cur++;
        switch (car->renderData[car->renderDataCount].type) {
        case TEREP_RENDERDATA_NULL:
            break;
        case TEREP_RENDERDATA_CAMERA: {
            TerepCarCameraData* cam = calloc(1, sizeof(TerepCarCameraData));
            car->renderData[car->renderDataCount].camera = cam;
            cam->cameraPointIndex = U16(dat->cur, 0) / 2;
            if (car->points[cam->cameraPointIndex].type != TEREP_POINT_CAMERA) {
                printf("LibTerep | ERROR: Failure parsing %s -- Chunk3 -> Camera point (id 0x1) index is not a camera "
                       "point, "
                       "read index %i\n",
                       dat->name, cam->cameraPointIndex);
            }
            cam->unknown1 = U8(dat->cur, 2);
            cam->unknown2 = U8(dat->cur, 3);
            dat->cur += 4;
            break;
        }
        case TEREP_RENDERDATA_UNK3_POLYGON: {
            // possibly some culling thing, changing these values seems to do render glitches
            TerepCarPolygonData* polygon = calloc(1, sizeof(TerepCarPolygonData));
            car->renderData[car->renderDataCount].polygon = polygon;
            polygon->pointCount = 3;
            polygon->vertices[0] = U16(dat->cur, 0) / 2;
            polygon->vertices[1] = U16(dat->cur, 2) / 2;
            polygon->vertices[2] = U16(dat->cur, 4) / 2;
            polygon->unknown3values[0] = U16(dat->cur, 6);
            polygon->unknown3values[1] = U16(dat->cur, 8);
            polygon->unknown3values[2] = U16(dat->cur, 10);
            dat->cur += 12;
            break;
        }
        case TEREP_RENDERDATA_COLOR_POLYGON: {
            TerepCarPolygonData* polygon = calloc(1, sizeof(TerepCarPolygonData));
            car->renderData[car->renderDataCount].polygon = polygon;
            polygon->pointCount = U8(dat->cur, 0);
            dat->cur++;
            for (size_t i = 0; i < polygon->pointCount; i++) {
                polygon->vertices[i] = U16(dat->cur, 2 * i) / 2;
            }
            polygon->closed = U16(dat->cur, 0) == U16(dat->cur, 2 * polygon->pointCount);
            polygon->colors[0] = U8(dat->cur, 2 * polygon->pointCount + 2);
            polygon->colors[1] = U8(dat->cur, 2 * polygon->pointCount + 3); // NOTE: Dithering
            dat->cur += polygon->pointCount * 2 + 4;
            break;
        }
        case TEREP_RENDERDATA_TEXTURE_POLYGON: {
            TerepCarPolygonData* polygon = calloc(1, sizeof(TerepCarPolygonData));
            car->renderData[car->renderDataCount].polygon = polygon;
            polygon->pointCount = U8(dat->cur, 0);
            dat->cur++;
            for (size_t i = 0; i < polygon->pointCount; i++) {
                polygon->vertices[i] = U16(dat->cur, 2 * i * 3) / 2;
                polygon->uv[i].x = U16(dat->cur, (2 * i * 3) + 2);
                polygon->uv[i].y = U16(dat->cur, (2 * i * 3) + 4);
            }
            polygon->closed = U16(dat->cur, 0) == U16(dat->cur, 2 * 3 * polygon->pointCount);
            dat->cur += (polygon->pointCount + 1) * 3 * 2;
            break;
        }
        case TEREP_RENDERDATA_WHEEL: {
            TerepCarWheelData* wheel = calloc(1, sizeof(TerepCarWheelData));
            car->renderData[car->renderDataCount].wheel = wheel;
            wheel->wheelPointIndex = U16(dat->cur, 0) / 2;
            wheel->unknown1 = U16(dat->cur, 2);
            wheel->unknown2 = U16(dat->cur, 4);
            memcpy(wheel->unknown3, dat->cur + 6, 20 * 9);
            dat->cur += (3 * 2) + (20 * 9);
            break;
        }
        default:
            printf("LibTerep | ERROR: Failure parsing %s -- Chunk3 -> Unknown data block %d\n", dat->name,
                   car->renderData[car->renderDataCount].type);
            return;
        }
        car->renderDataCount++;
    }
    printf("LibTerep | INFO: Read %i items from chunk3\n", car->renderDataCount);
}

TerepCar* TerepCar_Load(const char* cardat, const char* carpcx)
{
    TerepCar* car = calloc(1, sizeof *car);
    assert(car);
    TerepDat* dat = _LoadDat(cardat);
    // TODO: Attempt to detect the Terep1 dat format and load that too

    car->unknownHeaderValue1 = U16(dat->data, 6);
    car->unknownHeaderValue2 = U16(dat->data, 8);

    _ParseChunk1(car, dat);
    _ParseChunk2(car, dat);
    _ParseChunk3(car, dat);
    printf("LibTerep | INFO: Finished parsing %s (%zu bytes)\n", dat->name, dat->size);

    car->carTexture = PCX_LoadImage(carpcx);
    printf("LibTerep | INFO: Loaded car texture %s\n", carpcx);

    _UnloadDat(dat);
    return car;
}
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
    free(car->physSegments);
    free(car->carTexture->data);
    free(car->carTexture);
    free(car);
}
void _WriteChunk1(TerepCar* car, TerepDat* dat)
{
    I16(dat->data, 0) = dat->cur - dat->data;
    U16(dat->cur, 0) = car->pointCount;
    dat->cur += 2;
    for (size_t i = 0; i < car->pointCount; i++) {
        I16(dat->cur, 0) = car->points[i].unknown[0];
        I16(dat->cur, 2) = car->points[i].pos[0] * SCALE;
        I16(dat->cur, 8) = car->points[i].unknown[1];
        I16(dat->cur, 10) = car->points[i].pos[1] * SCALE;
        I16(dat->cur, 4) = car->points[i].unknown[2];
        I16(dat->cur, 6) = car->points[i].pos[2] * SCALE;
        I16(dat->cur, 24) = car->points[i].size * SCALE;
        I16(dat->cur, 26) = car->points[i].type;
        dat->cur += 28;
    }
    printf("LibTerep | INFO: Wrote %d points to file %s\n", car->pointCount, dat->name);
}
void _WriteChunk2(TerepCar* car, TerepDat* dat)
{
    I16(dat->data, 2) = dat->cur - dat->data;
    U16(dat->cur, 0) = car->physSegmentCount;
    dat->cur += 2;
    for (size_t i = 0; i < car->physSegmentCount; i++) {
        U16(dat->cur, 0) = car->physSegments[i].pointA;
        U16(dat->cur, 2) = car->physSegments[i].pointB;
        U16(dat->cur, 4) = round(car->physSegments[i].len * PHYS_LINK_SCALE);
        U16(dat->cur, 6) = round(car->physSegments[i].len2 * PHYS_LINK_SCALE);
        U16(dat->cur, 8) = car->physSegments[i].type;
        U16(dat->cur, 10) = round(car->physSegments[i].len_min * PHYS_LINK_SCALE);
        U16(dat->cur, 12) = round(car->physSegments[i].len_max * PHYS_LINK_SCALE);
        dat->cur += 14;
    }
    printf("LibTerep | INFO: Wrote %d physics segments %s\n", car->physSegmentCount, dat->name);
}

void _WriteChunk3(TerepCar* car, TerepDat* dat)
{
    I16(dat->data, 4) = dat->cur - dat->data;
    for (int i = 0; i < car->renderDataCount; i++) {
        U8(dat->cur, 0) = car->renderData[i].type;
        dat->cur++;
        switch (car->renderData[i].type) {
        case TEREP_RENDERDATA_NULL:
            break;
        case TEREP_RENDERDATA_CAMERA: {
            TerepCarCameraData* cam = car->renderData[i].camera;
            U16(dat->cur, 0) = cam->cameraPointIndex * 2;
            U8(dat->cur, 2) = cam->unknown1;
            U8(dat->cur, 3) = cam->unknown2;
            dat->cur += 4;
            break;
        }
        case TEREP_RENDERDATA_UNK3_POLYGON: {
            // possibly some culling thing, changing these values seems to do render glitches
            TerepCarPolygonData* polygon = car->renderData[i].polygon;
            U16(dat->cur, 0) = polygon->vertices[0] * 2;
            U16(dat->cur, 2) = polygon->vertices[1] * 2;
            U16(dat->cur, 4) = polygon->vertices[2] * 2;
            U16(dat->cur, 6) = polygon->unknown3values[0];
            U16(dat->cur, 8) = polygon->unknown3values[1];
            U16(dat->cur, 10) = polygon->unknown3values[2];
            dat->cur += 12;
            break;
        }
        case TEREP_RENDERDATA_COLOR_POLYGON: {
            TerepCarPolygonData* polygon = car->renderData[i].polygon;
            U8(dat->cur, 0) = polygon->pointCount;
            dat->cur++;
            for (size_t i = 0; i < polygon->pointCount; i++) {
                if (!polygon->closed) {
                    U16(dat->cur, 0) = polygon->vertices[i] * 2 + 1;
                } else {
                    U16(dat->cur, 0) = polygon->vertices[i] * 2;
                }
                dat->cur += 2;
            }
            if (polygon->closed) {
                U16(dat->cur, 0) = polygon->vertices[0] * 2;
            } else {

                U16(dat->cur, 0) = 0;
            }
            dat->cur += 2;
            U8(dat->cur, 0) = polygon->colors[0];
            U8(dat->cur, 1) = polygon->colors[1]; // NOTE: Dithering
            dat->cur += 2;
            break;
        }
        case TEREP_RENDERDATA_TEXTURE_POLYGON: {
            TerepCarPolygonData* polygon = car->renderData[i].polygon;
            U8(dat->cur, 0) = polygon->pointCount;
            dat->cur++;
            for (size_t i = 0; i < polygon->pointCount; i++) {
                U16(dat->cur, 0) = polygon->vertices[i] * 2;
                U16(dat->cur, 2) = polygon->uv[i].x;
                U16(dat->cur, 4) = polygon->uv[i].y;
                dat->cur += 3 * 2;
            }
            if (polygon->closed) {
                U16(dat->cur, 0) = polygon->vertices[0] * 2;
                U16(dat->cur, 2) = polygon->uv[0].x;
                U16(dat->cur, 4) = polygon->uv[0].y;
            } else {
                U16(dat->cur, 0) = 0;
                U16(dat->cur, 2) = 0;
                U16(dat->cur, 4) = 0;
            }
            dat->cur += 3 * 2;
            break;
        }
        case TEREP_RENDERDATA_WHEEL: {
            TerepCarWheelData* wheel = car->renderData[i].wheel;
            U16(dat->cur, 0) = wheel->wheelPointIndex * 2;
            U16(dat->cur, 2) = wheel->unknown1;
            U16(dat->cur, 4) = wheel->unknown2;
            memcpy(dat->cur + 6, wheel->unknown3, 20 * 9);
            dat->cur += (3 * 2) + (20 * 9);
            break;
        }
        }
    }
    printf("LibTerep | INFO: Wrote %i items to %s\n", car->renderDataCount, dat->name);
}
void TerepCar_Write(TerepCar* car, const char* cardat, const char* carpcx)
{
    TerepDat* dat = _CreateDat();
    strncpy(dat->name, cardat, 32);

    dat->cur = dat->data + 132;
    _WriteChunk1(car, dat);
    _WriteChunk2(car, dat);
    _WriteChunk3(car, dat);
    U16(dat->data, 6) = car->unknownHeaderValue1;
    U16(dat->data, 8) = car->unknownHeaderValue2;
    dat->size = dat->cur - dat->data;

    FILE* fp = fopen(cardat, "wb");
    fwrite(dat->data, dat->size, 1, fp);
    fclose(fp);
    _UnloadDat(dat);
}

/*
static void load_dat_chunk2_terep1(TerepCar* car, uint8_t* chunkStart)
{
    car->physSegmentCount = U16(chunkStart, 0);
    uint8_t* p = chunkStart + 2;
    for (size_t i = 0; i < car->physSegmentCount; i++) {
        car->physSegments[i].pointA = U16(p, 0);
        car->physSegments[i].pointB = U16(p, 2);
        car->physSegments[i].type = TEREP_SEGMENT_NORMAL;
        if (car->physSegments[i].type != 0 && car->physSegments[i].type != 1 && car->physSegments[i].type != 4 &&
            car->physSegments[i].type != 6 && car->physSegments[i].type != 10 && car->physSegments[i].type != 12) {
            printf("Unknown type segment: %i\n", car->physSegments[i].type);
            return;
        }
        p += 12;
    }
    printf("INFO: CARLOAD: Loaded %d physics segments\n", car->physSegmentCount);
}
*/
