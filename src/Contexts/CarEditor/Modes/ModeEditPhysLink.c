#include "Engine/DFCar.h"
#include "Contexts/CarEditor/CarEditor.h"
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
    igText("Editing Physics Link");
    igSeparator();
}

CarEditorMode CarEditorMode_EditPhyslink(TerepCarPhysLink* link)
{
    return (CarEditorMode){.RenderUI = _UI, .Render3D = _3D};
}
