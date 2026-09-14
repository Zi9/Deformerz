#include "EXEEditor.h"
#include "Engine/UI.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char _exepath[128];
char* _exedata = NULL;
int _exesz = 0;

// NOTE: (Zi9) just an experiment, ignore for now

static void _OnEnter() { strncpy(_exepath, "./data/exe-variants/original_unpacked.exe", 128); }
static void _OnExit()
{
    if (_exedata != NULL) {
        free(_exedata);
        _exedata = NULL;
    }
}

static void _UI_U16Editor(const char* label, uint16_t* value)
{
    int v = *value;
    igInputInt(label, &v, 1, 32, 0);
    v = v < 0 ? 0 : v > UINT16_MAX ? UINT16_MAX : v;
    *value = v;
}
static void _UI_I16Editor(const char* label, int16_t* value)
{
    int v = *value;
    igInputInt(label, &v, 1, 32, 0);
    v = v < INT16_MIN ? INT16_MIN : v > INT16_MAX ? INT16_MAX : v;
    *value = v;
}

static void _UI_U32Editor(const char* label, uint32_t* value)
{
    int v = *value;
    igInputInt(label, &v, 1, 1024, 0);
    v = v < 0 ? 0 : v > INT32_MAX ? INT32_MAX : v;
    *value = v;
}

#define P1 1330
#define P2 1531
#define X_OFS 0
#define X_CAM 6 * 1
#define X_SZ 6 * 2
#define Y_OFS 6 * 3
#define Y_CAM 6 * 4
#define Y_SZ 6 * 5

static void _MainUI()
{
    igSeparator();

    igSeparatorText("Base Game Properties");
    igPushItemWidth(120);
    _UI_U16Editor("Game Speed", (uint16_t*)(_exedata + 736));
    _UI_U32Editor("Initial Gravity", (uint32_t*)(_exedata + 743));
    _UI_I16Editor("Default Sim Value 1", (int16_t*)(_exedata + 751));
    _UI_I16Editor("Default Sim Value 2", (int16_t*)(_exedata + 757));
    igPopItemWidth();

    igSeparatorText("Text Strings");
    igPushItemWidth(524);
    igInputText("Top Text", _exedata + 24272, 75, 0, 0, 0);
    igPopItemWidth();
    igPushItemWidth(210);
    igInputText("Bottom Text", _exedata + 24347, 30, 0, 0, 0);
    igPopItemWidth();

    igSeparatorText("Split-screen Viewports");
    igBeginChild_Str("P1", (ImVec2){496, 110}, ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY, 0);
    igSeparatorText("Player 1");
    igPushItemWidth(120);
    _UI_U16Editor("X Offset", (uint16_t*)(_exedata + P1 + X_OFS));
    igSameLine(256, 0);
    _UI_U16Editor("Y Offset", (uint16_t*)(_exedata + P1 + Y_OFS));
    _UI_U16Editor("Viewport Width", (uint16_t*)(_exedata + P1 + X_SZ));
    igSameLine(256, 0);
    _UI_U16Editor("Viewport Height", (uint16_t*)(_exedata + P1 + Y_SZ));
    _UI_U16Editor("Car Target X", (uint16_t*)(_exedata + P1 + X_CAM));
    igSameLine(256, 0);
    _UI_U16Editor("Car Target Y", (uint16_t*)(_exedata + P1 + Y_CAM));
    igPopItemWidth();
    igEndChild();

    igSameLine(0, 8);

    igBeginChild_Str("P2", (ImVec2){496, 110}, ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY, 0);
    igSeparatorText("Player 2");
    igPushItemWidth(120);
    _UI_U16Editor("X Offset", (uint16_t*)(_exedata + P2 + X_OFS));
    igSameLine(256, 0);
    _UI_U16Editor("Y Offset", (uint16_t*)(_exedata + P2 + Y_OFS));
    _UI_U16Editor("Viewport Width", (uint16_t*)(_exedata + P2 + X_SZ));
    igSameLine(256, 0);
    _UI_U16Editor("Viewport Height", (uint16_t*)(_exedata + P2 + Y_SZ));
    _UI_U16Editor("Car Target X", (uint16_t*)(_exedata + P2 + X_CAM));
    igSameLine(256, 0);
    _UI_U16Editor("Car Target Y", (uint16_t*)(_exedata + P2 + Y_CAM));
    igPopItemWidth();
    igEndChild();
}

static void _Run()
{
    BeginDrawing();

    rlImGuiBegin();
    igSetNextWindowPos((ImVec2){0, 0}, ImGuiCond_Once, (ImVec2){0});
    igSetNextWindowSize((ImVec2){GetScreenWidth(), GetScreenHeight()}, ImGuiCond_Once);
    igBegin("Deformerz Terep EXE Editor", NULL,
            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

    igSeparatorText("Load / Save");
    igInputText("EXE file", _exepath, 128, 0, 0, 0);
    igSameLine(0, 8);
    if (_exedata == NULL) {
        if (igButton("Load", (ImVec2){0})) {
            FILE* f = fopen(_exepath, "rb");
            fseek(f, 0, SEEK_END);
            _exesz = ftell(f);
            fseek(f, 0, SEEK_SET);
            _exedata = calloc(1, _exesz);
            fread(_exedata, _exesz, 1, f);
            fclose(f);
        }
    } else {
        if (igButton("Save", (ImVec2){0})) {
            FILE* f = fopen(_exepath, "wb");
            fwrite(_exedata, _exesz, 1, f);
            fclose(f);
        }
        igSameLine(0, 8);
        if (igButton("Unload", (ImVec2){0})) {
            _OnExit();
        }
    }
    if (_exedata) {
        _MainUI();
    }

    igEnd();
    rlImGuiEnd();

    EndDrawing();
}

EngineContext EXEEditorContext() { return (EngineContext){.Run = _Run, .OnEnter = _OnEnter, .OnExit = _OnExit}; }
