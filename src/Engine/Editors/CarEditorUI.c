#include "Engine/Core/DFCar.h"
#include "Engine/Editors/CarEditor.h"
#include "Engine/UI/ImGUI.h"
#include "cimgui.h"
#include "raylib.h"
#include <string.h>

bool CarEditor_Active = false;
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
static const char* get_renderdata_type(TerepCarRenderDataItem* item)
{
    memset(tempSTR, 0, 32);
    if (item->type == TEREP_RENDERDATA_NULL) {
        strncpy(tempSTR, "NULL", 32);
    } else if (item->type == TEREP_RENDERDATA_CAMERA) {
        strncpy(tempSTR, "CAMERADATA", 32);
    } else if (item->type == TEREP_RENDERDATA_CAMERA) {
        strncpy(tempSTR, "CAMERADATA", 32);
    } else if (item->type == TEREP_RENDERDATA_UNK3_POLYGON) {
        strncpy(tempSTR, "UNK3_POLYGON", 32);
    } else if (item->type == TEREP_RENDERDATA_COLOR_POLYGON) {
        strncpy(tempSTR, "COLOR_POLYGON", 32);
    } else if (item->type == TEREP_RENDERDATA_TEXTURE_POLYGON) {
        strncpy(tempSTR, "TEXTURE_POLYGON", 32);
    } else if (item->type == TEREP_RENDERDATA_WHEEL) {
        strncpy(tempSTR, "WHEELDATA", 32);
    }
    return tempSTR;
}

void CarEditor_RenderUI(DFCar* dfcar)
{
    if (!CarEditor_Active)
        return;
    TerepCar* car = dfcar->car;
    igSetNextWindowPos((ImVec2){32, 32}, ImGuiCond_Once, (ImVec2){0});
    igBegin("Car Editor", &CarEditor_Active, ImGuiWindowFlags_NoCollapse);
    if (igTreeNode_Str("Render Types")) {
        igPushID_Str("Render");
        igCheckbox("Points", &RenderPoints);
        igCheckbox("Physics", &RenderPhysics);
        igCheckbox("Polygons", &RenderPolygons);
        igPopID();
        igTreePop();
    }
    if (SelectedPoint != -1) {
        igLabelText("##", "Selected Point: %i - %s (%f, %f, %f) S:%f", SelectedPoint,
                    get_point_type(&car->points[SelectedPoint]), car->points[SelectedPoint].pos[0],
                    car->points[SelectedPoint].pos[1], car->points[SelectedPoint].pos[2],
                    car->points[SelectedPoint].size);
    } else {

        if (igTreeNode_Str("Points")) {
            igPushID_Str("Points");
            for (size_t i = 0; i < car->pointCount; i++) {
                igLabelText("##", "%i - %s (%f, %f, %f) S:%f", i, get_point_type(&car->points[i]),
                            car->points[i].pos[0], car->points[i].pos[1], car->points[i].pos[2], car->points[i].size);
            }
            igSeparator();
            igPopID();
            igTreePop();
        }

        if (igTreeNode_Str("PhysicsSegments")) {
            igPushID_Str("PhysSeg");
            for (size_t i = 0; i < car->physSegmentCount; i++) {
                igLabelText("##", "%i - %s = %i <-> %i | %i, %i", i, get_phys_type(&car->physSegments[i]),
                            car->physSegments[i].pointA, car->physSegments[i].pointB, car->physSegments[i].other1,
                            car->physSegments[i].other2);
            }
            igSeparator();
            igPopID();
            igTreePop();
        }

        if (igTreeNode_Str("Renderables")) {
            igPushID_Str("Renderables");
            for (size_t i = 0; i < car->renderDataCount; i++) {
                if (igTreeNode_Str(TextFormat("%i - %s (%i)", i, get_renderdata_type(&car->renderData[i]),
                                              car->renderData[i].type))) {
                    igTreePop();
                }
            }
            igSeparator();
            igPopID();
            igTreePop();
        }
    }
    igEnd();
}
