#include "Contexts/CarEditor/CarEditor.h"
#include "Engine/DFCar.h"
#include "Engine/UI.h"
#include "LibTerep/TerepCar.h"

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
        igText("%i - %s ", i, TerepCar_RenderType2String(&car->renderData[i]));
    }
    igSeparator();
    igPopID();
}

CarEditorMode CarEditorMode_Renderables() { return (CarEditorMode){.RenderUI = _UI, .Render3D = _3D}; }
