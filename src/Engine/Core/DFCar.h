#pragma once

#include <raylib.h>
#include <stdint.h>

enum PointType {
    DFCAR_POINT_GEOMETRY,
    DFCAR_POINT_WHEEL_FL,
    DFCAR_POINT_WHEEL_FR,
    DFCAR_POINT_WHEEL_RL,
    DFCAR_POINT_WHEEL_RR,
    DFCAR_POINT_CAMERA
};
enum SegmentType {
    DFCAR_SEGMENT_NORMAL,
    DFCAR_SEGMENT_SUSP_FRONT,
    DFCAR_SEGMENT_SUSP_REAR,
    DFCAR_SEGMENT_SUSP_EXTRA,
};
enum DrivetrainMode {
    DFCAR_DRIVETRAIN_RWD = 0,
    DFCAR_DRIVETRAIN_FWD = 1,
    DFCAR_DRIVETRAIN_AWD = 2,
};
typedef struct DFCarPoint {
    Vector3 pos;
    uint8_t type;
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

typedef struct DFCar {
    uint16_t pointCount;
    DFCarPoint points[128];

    uint16_t physSegmentCount;
    DFCarPhysSegment physSegments[256];

    uint16_t renderableFaceCount;
    DFCarRenderableFace renderableFaces[128];

    uint8_t drivetrainMode;

} DFCar;