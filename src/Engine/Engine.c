#include "Engine.h"
#include "Contexts/CarEditor/CarEditor.h"
#include "UI.h"
#include <raylib.h>

static EngineContext _Current = {0};
static EngineContext _Next = {0};
static bool _WantSwitchContext = false;

Color Engine_Palette[256] = {0};
Color Engine_SkyColor = GRAY;

static void _SwitchContextIfRequested()
{
    if (!_WantSwitchContext)
        return;
    if (_Current.OnExit != 0) {
        _Current.OnExit();
    }
    _Current = _Next;
    if (_Current.OnEnter != 0) {
        _Current.OnEnter();
    }
    _WantSwitchContext = false;
    _Next = (EngineContext){0};
}

void Engine_SwitchContext(EngineContext ctx)
{
    _WantSwitchContext = true;
    _Next = ctx;
}

int main()
{
    SetTraceLogLevel(LOG_ERROR);
    SetTargetFPS(MAXFPS);

    InitWindow(WINDOW_W, WINDOW_H, WINDOW_TITLE);
    rlImGuiSetup(true);

    // SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    _Current = CarEditorContext();

    PCX_EnableGlobalPalette("./data/col.pcx"); // HACK: Fix this hardcoding at some point
    for (size_t i = 0; i < 256; i++) {
        Engine_Palette[i].r = PCX_GLOBAL_PALETTE[i].red;
        Engine_Palette[i].g = PCX_GLOBAL_PALETTE[i].green;
        Engine_Palette[i].b = PCX_GLOBAL_PALETTE[i].blue;
        Engine_Palette[i].a = 255;
    }
    Engine_SkyColor = Engine_Palette[255];

    if (_Current.OnEnter != 0)
        _Current.OnEnter();

    while (!WindowShouldClose()) {
        _SwitchContextIfRequested();
        _Current.Run();
    }

    if (_Current.OnExit != 0)
        _Current.OnExit();

    rlImGuiShutdown();
    CloseWindow();
}
