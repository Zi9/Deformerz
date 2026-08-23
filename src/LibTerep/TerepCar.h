#pragma once

#include "LibTerep/PCX.h"
#include <stdint.h>

#define TEREP_MAX_POINTS 64
#define TEREP_MAX_PHYS_SEGMENTS 256
#define TEREP_MAX_POLYGONS 128
#define TEREP_MAX_POLYGON_POINTS 5

typedef enum {
    TEREP_POINT_CAMERA = -1,
    TEREP_POINT_GEOMETRY = 0,
    TEREP_POINT_WHEEL_REAR = 1,
    TEREP_POINT_WHEEL_FRONT = 2
} TerepPointType;

typedef enum {
    TEREP_SEGMENT_SUSP_EXTRA = 0,
    TEREP_SEGMENT_NORMAL = 1,
    TEREP_SEGMENT_SUSP_REAR = 4,
    TEREP_SEGMENT_SUSP_REAR2 = 6,
    TEREP_SEGMENT_SUSP_FRONT = 10,
    TEREP_SEGMENT_SUSP_FRONT2 = 12,
} TerepPhysSegmentType;

typedef enum {
    TEREP_POLYGON_COLOR,
    TEREP_POLYGON_TEXTURE,
    TEREP_POLYGON_NUM3,
} TerepPolygonType;

typedef struct {
    TerepPointType type;
    uint8_t index;
    float pos[3];
    float size;
} TerepCarPoint;

typedef struct {
    TerepPhysSegmentType type;
    uint16_t pointA, pointB;
    uint16_t other1, other2, other3, other4;
} TerepCarPhysSegment;

typedef struct {
    uint16_t x;
    uint16_t y;
} TerepCarUVPoint;

typedef struct {
    TerepPolygonType type;
    uint8_t pointCount;
    uint16_t vertices[TEREP_MAX_POLYGON_POINTS];
    union {
        uint8_t colors[2];
        TerepCarUVPoint uv[TEREP_MAX_POLYGON_POINTS];
    };
} TerepCarPolygon;

typedef struct {
    uint16_t cameraPointIndex;
    uint8_t unknown1;
    uint8_t unknown2;
} TerepCarCameraProperties;

typedef struct {
    uint16_t wheelPointIndex;
    uint8_t unknown1;
    uint8_t unknown2;
} TerepCarWheelProperties;

typedef struct {
    uint16_t pointCount;
    TerepCarPoint points[TEREP_MAX_POINTS];

    uint16_t physSegmentCount;
    TerepCarPhysSegment physSegments[TEREP_MAX_PHYS_SEGMENTS];

    uint16_t polygonCount;
    TerepCarPolygon polygons[TEREP_MAX_POLYGONS];

    TerepCarCameraProperties cameraProperties;
    TerepCarWheelProperties wheelProperties[4];

    uint16_t unknownHeaderValue1;
    uint16_t unknownHeaderValue2;

    PCXImage* carTexture;
} TerepCar;

TerepCar* TerepCar_Load(const char* cardat, const char* carpcx);
void TerepCar_Unload(TerepCar* car);
