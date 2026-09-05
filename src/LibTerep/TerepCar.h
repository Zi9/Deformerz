#pragma once

#include "LibTerep/PCX.h"
#include <stdint.h>

#ifdef LIBTEREP_INTERNAL

#define U8(base, ofs) *(uint8_t*)(base + ofs)
#define U16(base, ofs) *(uint16_t*)(base + ofs)
#define I8(base, ofs) *(int8_t*)(base + ofs)
#define I16(base, ofs) *(int16_t*)(base + ofs)

#define SCALE 256.0f
#define PHYS_LINK_SCALE 16384.0f
#define UV_SCALE 65535.0f

typedef struct {
    uint8_t* data;
    uint64_t size;
    char name[32];
    uint8_t* cur;
} TerepDat;

TerepDat* _LoadDat(const char* path);
TerepDat* _CreateDat();
void _UnloadDat(TerepDat* dat);

#endif

#define TEREP_MAX_POLYGON_POINTS 5

// Chunk1
typedef struct {
    enum {
        TEREP_POINT_CAMERA = -1,
        TEREP_POINT_GEOMETRY = 0,
        TEREP_POINT_WHEEL_REAR = 1,
        TEREP_POINT_WHEEL_FRONT = 2
    } type;
    uint8_t index;
    float pos[3];
    float size;
    uint16_t unknown[3];
} TerepCarPoint;

// Chunk2
typedef struct {
    enum {
        TEREP_PHYSLINK_SUSP_EXTRA = 0,
        TEREP_PHYSLINK_NORMAL = 1,
        TEREP_PHYSLINK_SUSP_REAR4 = 4,
        TEREP_PHYSLINK_SUSP_REAR6 = 6,
        TEREP_PHYSLINK_SUSP_FRONT10 = 10,
        TEREP_PHYSLINK_SUSP_FRONT12 = 12
    } type;
    TerepCarPoint* pointA;
    TerepCarPoint* pointB;
    float len, len2, len_min, len_max;
} TerepCarPhysLink;

// Chunk3
typedef struct {
    TerepCarPoint* cameraPoint;
    uint8_t unknown1;
    uint8_t unknown2;
} TerepCarCameraData;

typedef struct {
    uint8_t vertexCount;
    TerepCarPoint* vertices[TEREP_MAX_POLYGON_POINTS];
    bool closed;
    bool isProjectedOnGround;
    union {
        uint16_t unknown3values[3];
        uint8_t colors[2];
        struct {
            float x, y;
        } uv[TEREP_MAX_POLYGON_POINTS];
    };
} TerepCarPolygonData;

typedef struct {
    TerepCarPoint* wheelPoint;
    uint8_t unknown1;
    uint8_t unknown2;
    struct {
        uint16_t sz_height;
        uint16_t sz_width;
        struct {
            float x;
            float y;
        } UV[4];
    } wheelSprites[9];
} TerepCarWheelData;

typedef struct {
    enum {
        TEREP_RENDERDATA_NULL = 0,
        TEREP_RENDERDATA_CAMERA = 1,
        TEREP_RENDERDATA_UNK3_POLYGON = 3,
        TEREP_RENDERDATA_COLOR_POLYGON = 4,
        TEREP_RENDERDATA_TEXTURE_POLYGON = 8,
        TEREP_RENDERDATA_WHEEL = 10,
    } type;
    union {
        TerepCarCameraData* camera;
        TerepCarPolygonData* polygon;
        TerepCarWheelData* wheel;
    };
} TerepCarRenderDataItem;

// Main Car Object
typedef struct {
    uint16_t pointCount;
    TerepCarPoint* points;

    uint16_t physLinkCount;
    TerepCarPhysLink* physLinks;

    uint16_t renderDataCount;
    TerepCarRenderDataItem* renderData;

    uint16_t unknownHeaderValue1;
    uint16_t engineSound;

    PCXImage* carTexture;
} TerepCar;

TerepCar* TerepCar_Load(const char* cardat, const char* carpcx);
TerepCar* TerepCar_LoadText(const char* cartext);
void TerepCar_Write(TerepCar* car, const char* cardat, const char* carpcx);
void TerepCar_WriteText(TerepCar* car, const char* cartext);
void TerepCar_Unload(TerepCar* car);

const char* TerepCar_Point2String(TerepCarPoint* point);
const char* TerepCar_PhysLink2String(TerepCarPhysLink* seg);
const char* TerepCar_RenderType2String(TerepCarRenderDataItem* item);
