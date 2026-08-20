#pragma once

#include <raylib.h>
#include <stdint.h>

#define DF_MAX_POINTS 64
#define DF_MAX_PHYS_SEGMENTS 256
#define DF_MAX_RENDERABLE_FACES 128

enum PointType {
    DFCAR_POINT_GEOMETRY = 0,
    DFCAR_POINT_WHEEL_REAR = 1,
    DFCAR_POINT_WHEEL_FRONT = 2,
    DFCAR_POINT_CAMERA = 65535
};

enum SegmentType {
    DFCAR_SEGMENT_SUSP_EXTRA = 0,
    DFCAR_SEGMENT_NORMAL = 1,
    DFCAR_SEGMENT_SUSP_REAR = 4,
    DFCAR_SEGMENT_SUSP_REAR2 = 6,
    DFCAR_SEGMENT_SUSP_FRONT = 10,
    DFCAR_SEGMENT_SUSP_FRONT2 = 12,
};

enum DrivetrainMode {
    DFCAR_DRIVETRAIN_RWD = 0,
    DFCAR_DRIVETRAIN_FWD = 1,
    DFCAR_DRIVETRAIN_AWD = 2,
};

typedef struct DFCarPoint {
    Vector3 pos;
    enum PointType type;
    float size;
} DFCarPoint;

typedef struct DFCarPhysSegment {
    uint16_t pointA, pointB;
    uint8_t type;
    uint16_t other1, other2, other3, other4;
} DFCarPhysSegment;

typedef struct DFCarRenderableFace {
    uint8_t count;
    uint16_t vertices[5]; // NOTE: This is a hard limit, maybe increase in future if needed
    uint8_t colors[2];
    bool render;
} DFCarRenderableFace;

typedef struct DFCarCameraProperties {
    uint16_t cameraPointIndex;
    uint8_t unknown1;
    uint8_t unknown2;
} DFCarCameraProperties;

typedef struct DFCar {
    uint16_t pointCount;
    DFCarPoint points[DF_MAX_POINTS];

    uint16_t physSegmentCount;
    DFCarPhysSegment physSegments[DF_MAX_PHYS_SEGMENTS];

    uint16_t renderableFaceCount;
    DFCarRenderableFace renderableFaces[DF_MAX_RENDERABLE_FACES];

    DFCarCameraProperties cameraProperties;
} DFCar;