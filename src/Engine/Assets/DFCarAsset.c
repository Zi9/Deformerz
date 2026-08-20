#include "Engine/Core/DFCar.h"
#include "raylib.h"

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
} Terep2Dat;

static Terep2Dat load_dat(const char* path)
{
    Terep2Dat dat = {};
    FILE* f = fopen(path, "r");
    strncpy(dat.name, path, 32);
    fseek(f, 0, SEEK_END);
    dat.size = ftell(f);
    fseek(f, 0, SEEK_SET);
    dat.data = MemAlloc(dat.size);
    fread(dat.data, dat.size, 1, f);
    fclose(f);
    return dat;
}

static void load_dat_chunk1(DFCar* car, uint8_t* chunkStart)
{
    car->pointCount = GetU16(chunkStart, 0);
    uint8_t* p = chunkStart + 2;
    for (size_t i = 0; i < car->pointCount; i++) {
        car->points[i].pos.x = GetI32(p, 0) / SCALE;
        car->points[i].pos.y = GetI32(p, 4) / SCALE;
        car->points[i].pos.z = GetI32(p, 8) / SCALE;
        int32_t size = GetI32(p, 22);
        car->points[i].size = size > 0 ? size / SCALE : 0.0f;
        car->points[i].type = (enum PointType)GetU16(p, 26);
        if (car->points[i].type > 2 && car->points[i].type != 65535) {
            printf("Unknown type point: %i\n", car->points[i].type);
        }
        p += 28;
    }
    printf("INFO: CARLOAD: Loaded %d points\n", car->pointCount);
}

static void load_dat_chunk2(DFCar* car, uint8_t* chunkStart)
{
    car->physSegmentCount = GetU16(chunkStart, 0);
    uint8_t* p = chunkStart + 2;
    for (size_t i = 0; i < car->physSegmentCount; i++) {
        car->physSegments[i].pointA = GetU16(p, 0);
        car->physSegments[i].pointB = GetU16(p, 2);
        car->physSegments[i].other1 = GetU16(p, 4);
        car->physSegments[i].other2 = GetU16(p, 6);
        car->physSegments[i].type = GetU16(p, 8);
        car->physSegments[i].other3 = GetU16(p, 10);
        car->physSegments[i].other4 = GetU16(p, 12);
        if (car->physSegments[i].type != 0 && car->physSegments[i].type != 1 && car->physSegments[i].type != 4 &&
            car->physSegments[i].type != 6 && car->physSegments[i].type != 10 && car->physSegments[i].type != 12) {
            printf("Unknown type segment: %i\n", car->physSegments[i].type);
            return;
        }
        p += 14;
    }
    printf("INFO: CARLOAD: Loaded %d physics segments\n", car->physSegmentCount);
}

static void load_dat_chunk3(DFCar* car, uint8_t* chunkStart, uint8_t* end)
{
    int count = 0;
    uint8_t* p = chunkStart;
    while (p < end) {
        uint8_t dtype = GetU8(p, 0);
        p++;
        switch (dtype) {
        case 0:
            //printf("\e[0;37m0:\tEncountered 0x00 at %zu from chunk 3 start\n", p - chunkStart);
            break;
        case 1:
            car->cameraProperties.cameraPointIndex = GetU16(p, 0) / 2;
            if (car->points[car->cameraProperties.cameraPointIndex].type != DFCAR_POINT_CAMERA) {
                printf("ERROR: CARLOAD: Chunk3 -> Camera point (id 0x1) index is not a camera point, read %i\n",
                       car->cameraProperties.cameraPointIndex);
            }
            car->cameraProperties.unknown1 = GetU8(p, 2);
            car->cameraProperties.unknown2 = GetU8(p, 3);
            p += 4;
            break;
        case 3:
            // possibly some culling thing, changing these values seems to do render glitches
            printf("\e[0;32m3:\t");
            for (size_t i = 0; i < 6; i++) {
                printf("%d\t", GetU16(p, i * 2));
            }
            p += 6 * 2;
            printf("\e[0m\n");
            break;
        case 4:
            uint8_t count4 = GetU8(p, 0);
            p++;
            printf("\e[0;33m4-%d:\t", count4);
            car->renderableFaces[car->renderableFaceCount].count = count4;
            for (size_t i = 0; i < count4; i++) {
                uint16_t point = GetU16(p, 2 * i);
                printf("%d\t", point);
                if (point % 2 == 0) {
                    point = point / 2;
                } else {
                    point = (point / 2) + 1;
                }
                car->renderableFaces[car->renderableFaceCount].vertices[i] = point;
            }
            car->renderableFaces[car->renderableFaceCount].colors[0] = GetU8(p, 2 * count4 + 2);
            car->renderableFaces[car->renderableFaceCount].colors[1] = GetU8(p, 2 * count4 + 3);
            printf("%d\t", car->renderableFaces[car->renderableFaceCount].colors[0]);
            printf("%d\t", car->renderableFaces[car->renderableFaceCount].colors[1]);
            car->renderableFaces[car->renderableFaceCount].render =
                car->renderableFaces[car->renderableFaceCount].colors[0] > 0;
            car->renderableFaceCount++;
            p += count4 * 2 + 4;
            printf("\e[0m\n");
            break;
        case 8:
            uint8_t count8 = GetU8(p, 0);
            p++;
            printf("\e[0;34m8-%d:\t", count8);
            count8 += 1;
            count8 *= 3;
            for (size_t i = 0; i < count8; i++) {
                printf("%d\t", GetU16(p, i * 2));
            }
            p += count8 * 2;
            printf("\e[0m\n");
            break;
        case 10:
            printf("\e[0;35m10:\t");
            for (size_t i = 0; i < 3; i++) {
                printf("%d\t", GetU16(p, i * 2));
            }
            p += 3 * 2;
            printf("\e[0m\n");
            break;
        case 69:
        case 246:
            printf("\e[0;36m246:\t");
            for (size_t i = 0; i < 19; i++) {
                // printf("%d\t", data246[i]);
                printf("%02X ", GetU8(p, i));
            }
            p += 19;
            printf("\e[0m\n");
            break;
        default:
            printf("\e[0mUnknown data block %d\n", dtype);
            return;
        }
        count++;
    }
    printf("INFO: CARLOAD: Read %i chunks\n", count);
}

DFCar* Assets_LoadDFCar(const char* path)
{
    DFCar* car = MemAlloc(sizeof *car);
    Terep2Dat dat = load_dat(path);

    uint16_t chunk1Start = GetU16(dat.data, 0);
    uint16_t chunk2Start = GetU16(dat.data, 2);
    uint16_t chunk3Start = GetU16(dat.data, 4);
    uint16_t unknown = GetU16(dat.data, 6);
    uint16_t unknown2 = GetU16(dat.data, 8);

    load_dat_chunk1(car, dat.data + chunk1Start);
    load_dat_chunk2(car, dat.data + chunk2Start);
    load_dat_chunk3(car, dat.data + chunk3Start, dat.data + dat.size);

    MemFree(dat.data);
    return car;
}
void Assets_UnloadDFCar(DFCar* car) { free(car); }