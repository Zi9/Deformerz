#include "Contexts/CarEditor/CarEditor.h"
#include "Engine/DFCar.h"
#include "Engine/UI.h"
#include "LibTerep/TerepCar.h"

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

static void _3D(DFCar* dfcar) { TerepCar* car = dfcar->car; }

static void _UI(DFCar* dfcar)
{
    TerepCar* car = dfcar->car;
    if (igButton("<- Back", (ImVec2){0})) {
        CarEditor_SwitchMode(CarEditorMode_Meta());
    }
    igSameLine(0, 8);
    igText("Renderables");
    igSeparator();

    igPushID_Str("Renderables");
    for (size_t i = 0; i < car->renderDataCount; i++) {
        igText("%i - %s ", i, _RenderData2Str(&car->renderData[i]));
    }
    igSeparator();
    igPopID();
}

CarEditorMode CarEditorMode_Renderables() { return (CarEditorMode){.RenderUI = _UI, .Render3D = _3D}; }
