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
        if (car->renderData[i].type == TEREP_RENDERDATA_CAMERA) {
            igText("%i - %s | %i %i", i, TerepCar_RenderType2String(&car->renderData[i]),
                   car->renderData[i].camera->unknown1, car->renderData[i].camera->unknown2);
        } else if (car->renderData[i].type == TEREP_RENDERDATA_UNK3_POLYGON) {
            igText("%i - %s | %i %i %i", i, TerepCar_RenderType2String(&car->renderData[i]),
                   car->renderData[i].polygon->unknown3values[0], car->renderData[i].polygon->unknown3values[1],
                   car->renderData[i].polygon->unknown3values[2]);
        } else {
            igText("%i - %s", i, TerepCar_RenderType2String(&car->renderData[i]));
        }
    }
    igSeparator();
    igPopID();
}

CarEditorMode CarEditorMode_Renderables() { return (CarEditorMode){.RenderUI = _UI, .Render3D = _3D}; }
