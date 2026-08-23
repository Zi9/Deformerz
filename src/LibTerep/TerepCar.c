#include "TerepCar.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SCALE 10000000.0f

#define GetU8(base, ofs) *(uint8_t*)(base + ofs)
#define GetU16(base, ofs) *(uint16_t*)(base + ofs)
#define GetU32(base, ofs) *(uint32_t*)(base + ofs)
#define GetI8(base, ofs) *(int8_t*)(base + ofs)
#define GetI16(base, ofs) *(int16_t*)(base + ofs)
#define GetI32(base, ofs) *(int32_t*)(base + ofs)

typedef struct {
    uint8_t* data;
    size_t size;
    char name[32];
    uint8_t* cur;
} TerepDat;

static TerepDat* load_dat(const char* path)
{
    TerepDat* dat = malloc(sizeof(TerepDat));
    FILE* f = fopen(path, "r");
    strncpy(dat->name, path, 32);
    fseek(f, 0, SEEK_END);
    dat->size = ftell(f);
    fseek(f, 0, SEEK_SET);
    dat->data = malloc(dat->size);
    fread(dat->data, dat->size, 1, f);
    fclose(f);
    return dat;
}
static void unload_dat(TerepDat* dat)
{
    free(dat->data);
    free(dat);
}

static void parse_chunk1(TerepCar* car, TerepDat* dat)
{
    dat->cur = dat->data + GetU16(dat->data, 0);
    car->pointCount = GetU16(dat->cur, 0);
    dat->cur += 2;
    for (size_t i = 0; i < car->pointCount; i++) {
        car->points[i].pos[0] = GetI32(dat->cur, 0) / SCALE;
        car->points[i].pos[1] = GetI32(dat->cur, 8) / SCALE;
        car->points[i].pos[2] = GetI32(dat->cur, 4) / SCALE;
        int32_t size = GetI32(dat->cur, 22);
        car->points[i].size = size > 0 ? size / SCALE : 0.0f;
        car->points[i].type = (TerepPointType)GetU16(dat->cur, 26);
        if (car->points[i].type > 2 && car->points[i].type != 65535) {
            printf("LibTerep | ERROR: Failure parsing %s -- Unknown type point: %i\n", dat->name, car->points[i].type);
        }
        dat->cur += 28;
    }
    printf("LibTerep | INFO: Loaded %d points\n", car->pointCount);
}

static void parse_chunk2(TerepCar* car, TerepDat* dat)
{
    dat->cur = dat->data + GetU16(dat->data, 2);
    car->physSegmentCount = GetU16(dat->cur, 0);
    dat->cur += 2;
    for (size_t i = 0; i < car->physSegmentCount; i++) {
        car->physSegments[i].pointA = GetU16(dat->cur, 0);
        car->physSegments[i].pointB = GetU16(dat->cur, 2);
        car->physSegments[i].other1 = GetU16(dat->cur, 4);
        car->physSegments[i].other2 = GetU16(dat->cur, 6);
        car->physSegments[i].type = GetU16(dat->cur, 8);
        car->physSegments[i].other3 = GetU16(dat->cur, 10);
        car->physSegments[i].other4 = GetU16(dat->cur, 12);
        if (car->physSegments[i].type != 0 && car->physSegments[i].type != 1 && car->physSegments[i].type != 4 &&
            car->physSegments[i].type != 6 && car->physSegments[i].type != 10 && car->physSegments[i].type != 12) {
            printf("LibTerep | ERROR: Failure parsing %s -- Unknown type physics segment: %i\n", dat->name,
                   car->physSegments[i].type);
            return;
        }
        dat->cur += 14;
    }
    printf("LibTerep | INFO: Loaded %d physics segments\n", car->physSegmentCount);
}

static void parse_chunk3_1camera(TerepCar* car, TerepDat* dat)
{
    car->cameraProperties.cameraPointIndex = GetU16(dat->cur, 0) / 2;
    if (car->points[car->cameraProperties.cameraPointIndex].type != TEREP_POINT_CAMERA) {
        printf("LibTerep | ERROR: Failure parsing %s -- Chunk3 -> Camera point (id 0x1) index is not a camera point, "
               "read index %i\n",
               dat->name, car->cameraProperties.cameraPointIndex);
    }
    car->cameraProperties.unknown1 = GetU8(dat->cur, 2);
    car->cameraProperties.unknown2 = GetU8(dat->cur, 3);
    dat->cur += 4;
}

static void parse_chunk3_4coloredpolygon(TerepCar* car, TerepDat* dat)
{
    uint8_t count = GetU8(dat->cur, 0);
    dat->cur++;
    car->polygons[car->polygonCount].pointCount = count;
    car->polygons[car->polygonCount].type = TEREP_POLYGON_COLOR;
    for (size_t i = 0; i < count; i++) {
        car->polygons[car->polygonCount].vertices[i] = GetU16(dat->cur, 2 * i) / 2;
    }
    car->polygons[car->polygonCount].colors[0] = GetU8(dat->cur, 2 * count + 2);
    car->polygons[car->polygonCount].colors[1] = GetU8(dat->cur, 2 * count + 3); // NOTE: This is used for dithering
    car->polygonCount++;
    dat->cur += count * 2 + 4;
}

static void parse_chunk3_8texturedpolygon(TerepCar* car, TerepDat* dat)
{
    uint8_t count = GetU8(dat->cur, 0);
    dat->cur++;
    car->polygons[car->polygonCount].pointCount = count;
    car->polygons[car->polygonCount].type = TEREP_POLYGON_TEXTURE;
    for (size_t i = 0; i < count; i++) {
        car->polygons[car->polygonCount].vertices[i] = GetU16(dat->cur, 2 * i * 3) / 2;
        car->polygons[car->polygonCount].uv[i].x = GetU16(dat->cur, (2 * i * 3) + 2);
        car->polygons[car->polygonCount].uv[i].y = GetU16(dat->cur, (2 * i * 3) + 4);
    }
    car->polygonCount++;
    dat->cur += (count + 1) * 3 * 2;
}

static void parse_chunk3_10wheelprops(TerepCar* car, TerepDat* dat)
{
    uint16_t idx = GetU16(dat->cur, 0) / 2;
    if (idx > 3) {
        printf("LibTerep | ERROR: Failure parsing %s -- Chunk3 -> Wheel defined past index 3\n", dat->name);
    }
    car->wheelProperties[idx].wheelPointIndex = idx;
    car->wheelProperties[idx].unknown1 = GetU16(dat->cur, 2);
    car->wheelProperties[idx].unknown2 = GetU16(dat->cur, 4);
    dat->cur += 3 * 2;
}

static void parse_chunk3(TerepCar* car, TerepDat* dat)
{
    dat->cur = dat->data + GetU16(dat->data, 4);
    int readItemCount = 0;
    while (dat->cur < dat->data + dat->size) {
        uint8_t dtype = GetU8(dat->cur, 0);
        dat->cur++;
        switch (dtype) {
        case 0:
            break;
        case 1:
            parse_chunk3_1camera(car, dat);
            break;
        case 3:
            // possibly some culling thing, changing these values seems to do render glitches
            printf("\e[0;32m3:\t");
            for (size_t i = 0; i < 6; i++) {
                printf("%d\t", GetU16(dat->cur, i * 2));
            }
            dat->cur += 6 * 2;
            printf("\e[0m\n");
            break;
        case 4:
            parse_chunk3_4coloredpolygon(car, dat);
            break;
        case 8:
            parse_chunk3_8texturedpolygon(car, dat);
            break;
        case 10:
            parse_chunk3_10wheelprops(car, dat);
            break;
        case 69:
            printf("\e[0;36m69:\t");
            for (size_t i = 0; i < 19; i++) {
                // printf("%d\t", data246[i]);
                printf("%02X ", GetU8(dat->cur, i));
            }
            dat->cur += 19;
            printf("\e[0m\n");
            break;
        case 246:
            printf("\e[0;37m246:\t");
            for (size_t i = 0; i < 19; i++) {
                // printf("%d\t", data246[i]);
                printf("%02X ", GetU8(dat->cur, i));
            }
            dat->cur += 19;
            printf("\e[0m\n");
            break;
        default:
            printf("LibTerep | ERROR: Failure parsing %s -- Chunk3 -> Unknwon data block %d\n", dat->name, dtype);
            return;
        }
        readItemCount++;
    }
    printf("LibTerep | INFO: Read %i items from chunk3\n", readItemCount);
}

TerepCar* TerepCar_Load(const char* cardat, const char* carpcx)
{
    TerepCar* car = malloc(sizeof *car);
    TerepDat* dat = load_dat(cardat);
    // TODO: Attempt to detect the Terep1 dat format and load that too

    car->unknownHeaderValue1 = GetU16(dat->data, 6);
    car->unknownHeaderValue2 = GetU16(dat->data, 8);

    parse_chunk1(car, dat);
    parse_chunk2(car, dat);
    parse_chunk3(car, dat);
    printf("LibTerep | INFO: Finished parsing %s (%zu bytes)\n", dat->name, dat->size);

    car->carTexture = PCX_LoadImage(carpcx);
    printf("LibTerep | INFO: Loaded car texture %s\n", carpcx);

    unload_dat(dat);
    return car;
}
void TerepCar_Unload(TerepCar* car)
{
    free(car->carTexture->data);
    free(car->carTexture);
    free(car);
}

/*
static void load_dat_chunk2_terep1(TerepCar* car, uint8_t* chunkStart)
{
    car->physSegmentCount = GetU16(chunkStart, 0);
    uint8_t* p = chunkStart + 2;
    for (size_t i = 0; i < car->physSegmentCount; i++) {
        car->physSegments[i].pointA = GetU16(p, 0);
        car->physSegments[i].pointB = GetU16(p, 2);
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
