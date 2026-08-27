#pragma once

#include "LibTerep/PCX.h"
#include <stdint.h>

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
        TEREP_SEGMENT_SUSP_EXTRA = 0,
        TEREP_SEGMENT_NORMAL = 1,
        TEREP_SEGMENT_SUSP_REAR = 4,
        TEREP_SEGMENT_SUSP_REAR2 = 6,
        TEREP_SEGMENT_SUSP_FRONT = 10,
        TEREP_SEGMENT_SUSP_FRONT2 = 12,
    } type;
    uint16_t pointA, pointB;
    uint16_t other1, other2, other3, other4;
} TerepCarPhysSegment;

// Chunk3
typedef struct {
    uint16_t cameraPointIndex;
    uint8_t unknown1;
    uint8_t unknown2;
} TerepCarCameraData;

typedef struct {
    uint8_t pointCount;
    uint16_t vertices[TEREP_MAX_POLYGON_POINTS];
    bool closed;
    union {
        uint16_t unknown3values[3];
        uint8_t colors[2];
        struct {
            uint16_t x, y;
        } uv[TEREP_MAX_POLYGON_POINTS];
    };
} TerepCarPolygonData;

typedef struct {
    uint16_t wheelPointIndex;
    uint8_t unknown1;
    uint8_t unknown2;
    uint8_t unknown3[20 * 9];
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

    uint16_t physSegmentCount;
    TerepCarPhysSegment* physSegments;

    uint16_t renderDataCount;
    TerepCarRenderDataItem* renderData;

    uint16_t unknownHeaderValue1;
    uint16_t unknownHeaderValue2;

    PCXImage* carTexture;
} TerepCar;

TerepCar* TerepCar_Load(const char* cardat, const char* carpcx);
void TerepCar_Unload(TerepCar* car);
void TerepCar_Write(TerepCar* car, const char* cardat, const char* carpcx);
