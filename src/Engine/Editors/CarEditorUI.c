#include "Engine/Core/DFCar.h"
#include "Engine/Editors/CarEditor.h"
#include "Engine/Rendering/Renderer.h"
#include "Engine/UI/ImGUI.h"
#include "cimgui.h"
#include "raylib.h"
#include "raymath.h"
#include <math.h>

static const char* _Point2Str(TerepCarPoint* point)
{
    switch (point->type) {
    case TEREP_POINT_GEOMETRY:
        return "GEOMETRY";
    case TEREP_POINT_CAMERA:
        return "CAMERA";
    case TEREP_POINT_WHEEL_FRONT:
        return "WHEEL_F";
    case TEREP_POINT_WHEEL_REAR:
        return "WHEEL_R";
    }
}
static const char* _PhysSeg2Str(TerepCarPhysSegment* seg)
{
    switch (seg->type) {
    case TEREP_SEGMENT_NORMAL:
        return "NORMAL";
    case TEREP_SEGMENT_SUSP_EXTRA:
        return "SUSP_EXTRA";
    case TEREP_SEGMENT_SUSP_FRONT:
        return "FRONT";
    case TEREP_SEGMENT_SUSP_FRONT2:
        return "FRONT_2";
    case TEREP_SEGMENT_SUSP_REAR:
        return "REAR";
    case TEREP_SEGMENT_SUSP_REAR2:
        return "REAR_2";
    }
}
static const char* _RenderData2Str(TerepCarRenderDataItem* item)
{
    switch (item->type) {
    case TEREP_RENDERDATA_NULL:
        return "NULL";
    case TEREP_RENDERDATA_CAMERA:
        return "CAMERADATA";
    case TEREP_RENDERDATA_UNK3_POLYGON:
        return "UNK3_POLYGON";
    case TEREP_RENDERDATA_COLOR_POLYGON:
        return "COLOR_POLYGON";
    case TEREP_RENDERDATA_TEXTURE_POLYGON:
        return "TEXTURE_POLYGON";
    case TEREP_RENDERDATA_WHEEL:
        return "WHEELDATA";
    }
}

void _CarEditor_ModeDefault(DFCar* dfcar)
{
    TerepCar* car = dfcar->car;
    if (igTreeNode_Str("Points")) {
        igPushID_Str("Points");
        for (size_t i = 0; i < car->pointCount; i++) {
            igLabelText("##", "%i - %s (%f, %f, %f) S:%f", i, _Point2Str(&car->points[i]), car->points[i].pos[0],
                        car->points[i].pos[1], car->points[i].pos[2], car->points[i].size);
        }
        igSeparator();
        igPopID();
        igTreePop();
    }

    if (igTreeNode_Str("PhysicsSegments")) {
        igPushID_Str("PhysSeg");
        for (size_t i = 0; i < car->physSegmentCount; i++) {
            float dist = Vector3Distance(ToVector3(car->points[car->physSegments[i].pointA].pos),
                                         ToVector3(car->points[car->physSegments[i].pointB].pos)) * 1000.0;
            igLabelText("##", "%i - %s = %i <-> %i | %f == %f",
                        i,
                        _PhysSeg2Str(&car->physSegments[i]),
                        car->physSegments[i].pointA,
                        car->physSegments[i].pointB,
                        dist,
                        car->physSegments[i].len * -1);
        }
        igSeparator();
        igPopID();
        igTreePop();
    }

    if (igTreeNode_Str("Renderables")) {
        igPushID_Str("Renderables");
        for (size_t i = 0; i < car->renderDataCount; i++) {
            if (igTreeNode_Str(
                    TextFormat("%i - %s (%i)", i, _RenderData2Str(&car->renderData[i]), car->renderData[i].type))) {
                igTreePop();
            }
        }
        igSeparator();
        igPopID();
        igTreePop();
    }
}
void _CarEditor_ModeEditPoint(DFCar* dfcar)
{
    TerepCar* car = dfcar->car;
    igLabelText("##", "Selected Point: %i - %s (%f, %f, %f) S:%f", CarEditor.SelectedPoint,
                _Point2Str(&car->points[CarEditor.SelectedPoint]), car->points[CarEditor.SelectedPoint].pos[0],
                car->points[CarEditor.SelectedPoint].pos[1], car->points[CarEditor.SelectedPoint].pos[2],
                car->points[CarEditor.SelectedPoint].size);
}

void CarEditor_RenderUI(DFCar* dfcar)
{
    if (!CarEditor.Active)
        return;
    igSetNextWindowPos((ImVec2){32, 32}, ImGuiCond_Once, (ImVec2){0});
    igBegin("Car Editor", &CarEditor.Active, ImGuiWindowFlags_NoCollapse);
    switch (CarEditor.Mode) {
    case MODE_DEFAULT:
        _CarEditor_ModeDefault(dfcar);
        break;
    case MODE_EDIT_POINT:
        _CarEditor_ModeEditPoint(dfcar);
        break;
    case MODE_EDIT_PHYSLINK:
        break;
    case MODE_EDIT_UV:
        break;
    }
    igEnd();
    ImGuiIO* io = igGetIO_Nil();
    CarEditor.HandleMouseIn3D = !io->WantCaptureMouse;
}
