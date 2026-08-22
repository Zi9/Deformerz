#include "Engine/UI/ImGUI.h"
#include "LibTerep/TerepCar.h"
#include "cimgui.h"
#include <string.h>

bool debugActive = true;
char tempSTR[32];
static const char* get_point_type(TerepCarPoint* point)
{
    memset(tempSTR, 0, 32);
    if (point->type == TEREP_POINT_GEOMETRY) {
        strncpy(tempSTR, "GEOMETRY", 32);
    } else if (point->type == TEREP_POINT_CAMERA) {
        strncpy(tempSTR, "CAMERA", 32);
    } else if (point->type == TEREP_POINT_WHEEL_FRONT) {
        strncpy(tempSTR, "WHEEL_F", 32);
    } else if (point->type == TEREP_POINT_WHEEL_REAR) {
        strncpy(tempSTR, "WHEEL_R", 32);
    } else {
        strncpy(tempSTR, "UNKNOWN", 32);
    }
    return tempSTR;
}
static const char* get_phys_type(TerepCarPhysSegment* seg)
{
    memset(tempSTR, 0, 32);
    if (seg->type == TEREP_SEGMENT_NORMAL) {
        strncpy(tempSTR, "NORMAL", 32);
    } else if (seg->type == TEREP_SEGMENT_SUSP_EXTRA) {
        strncpy(tempSTR, "SUSP_EXTRA", 32);
    } else if (seg->type == TEREP_SEGMENT_SUSP_FRONT) {
        strncpy(tempSTR, "FRONT", 32);
    } else if (seg->type == TEREP_SEGMENT_SUSP_FRONT2) {
        strncpy(tempSTR, "FRONT_2", 32);
    } else if (seg->type == TEREP_SEGMENT_SUSP_REAR) {
        strncpy(tempSTR, "REAR", 32);
    } else if (seg->type == TEREP_SEGMENT_SUSP_REAR2) {
        strncpy(tempSTR, "REAR_2", 32);
    } else {
        strncpy(tempSTR, "UNKNOWN", 32);
    }
    return tempSTR;
}
static const char* get_face_type(TerepCarPolygon* face)
{
    memset(tempSTR, 0, 32);
    if (face->type == TEREP_POLYGON_COLOR) {
        strncpy(tempSTR, "COLOR", 32);
    } else if (face->type == TEREP_POLYGON_TEXTURE) {
        strncpy(tempSTR, "TEXTURE", 32);
    } else {
        strncpy(tempSTR, "UNKNOWN", 32);
    }
    return tempSTR;
}

void Debug_RenderCarDebugger(TerepCar* car)
{
    igSetNextWindowPos((ImVec2){32, 32}, ImGuiCond_Once, (ImVec2){0});
    igBegin("Car Debug", &debugActive, ImGuiWindowFlags_NoCollapse);
    if (igTreeNode_Str("Points")) {
        igPushID_Str("Points");
        for (size_t i = 0; i < car->pointCount; i++) {
            igLabelText("##", "%i - %s (%f, %f, %f) S:%f", i, get_point_type(&car->points[i]), car->points[i].pos[0],
                        car->points[i].pos[1], car->points[i].pos[2], car->points[i].size);
        }
        igSeparator();
        igPopID();
        igTreePop();
    }

    if (igTreeNode_Str("PhysicsSegments")) {
        igPushID_Str("PhysSeg");
        for (size_t i = 0; i < car->physSegmentCount; i++) {
            igLabelText("##", "%i - %s = %i <-> %i", i, get_phys_type(&car->physSegments[i]),
                        car->physSegments[i].pointA, car->physSegments[i].pointB);
        }
        igSeparator();
        igPopID();
        igTreePop();
    }

    if (igTreeNode_Str("Renderables")) {
        igPushID_Str("Renderables");
        for (size_t i = 0; i < car->polygonCount; i++) {
            if (igTreeNode_Str(
                    TextFormat("%i - %s (%i)", i, get_face_type(&car->polygons[i]), car->polygons[i].pointCount))) {
                igLabelText("##", "0: %i - %i, %i", car->polygons[i].vertices[0], car->polygons[i].uv[0].x,
                            car->polygons[i].uv[0].y);
                igLabelText("##", "1: %i - %i, %i", car->polygons[i].vertices[1], car->polygons[i].uv[1].x,
                            car->polygons[i].uv[1].y);
                igLabelText("##", "2: %i - %i, %i", car->polygons[i].vertices[2], car->polygons[i].uv[2].x,
                            car->polygons[i].uv[2].y);
                if (car->polygons[i].pointCount > 3)
                    igLabelText("##", "3: %i - %i, %i", car->polygons[i].vertices[3], car->polygons[i].uv[3].x,
                                car->polygons[i].uv[3].y);
                if (car->polygons[i].pointCount > 4)
                    igLabelText("##", "4: %i - %i, %i", car->polygons[i].vertices[4], car->polygons[i].uv[4].x,
                                car->polygons[i].uv[4].y);
                igTreePop();
            }
        }
        igSeparator();
        igPopID();
        igTreePop();
    }

    if (igTreeNode_Str("Camera Point")) {
        igPushID_Str("CamPoint");
        igLabelText("##", "Camera point index: %i", car->cameraProperties.cameraPointIndex);
        igLabelText("##", "Camera point unknown value 1: %i", car->cameraProperties.unknown1);
        igLabelText("##", "Camera point unknown value 2: %i", car->cameraProperties.unknown2);
        igSeparator();
        igPopID();
        igTreePop();
    }

    if (igTreeNode_Str("Wheels")) {
        igPushID_Str("Wheels");
        for (size_t i = 0; i < 4; i++) {
            igLabelText("##", "Wheel index: %i", car->wheelProperties[i].wheelPointIndex);
            igLabelText("##", "Wheel unknown value 1: %i", car->wheelProperties[i].unknown1);
            igLabelText("##", "Wheel unknown value 2: %i", car->wheelProperties[i].unknown2);
        }
        igSeparator();
        igPopID();
        igTreePop();
    }
    igEnd();
}
