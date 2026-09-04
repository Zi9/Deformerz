#pragma once

#include <raylib.h>

#define WINDOW_TITLE "Deformerz - A recreation of Terep2"
#define WINDOW_W 1920
#define WINDOW_H 1080
#define MAXFPS 60

extern Color Engine_Palette[256];
extern Color Engine_SkyColor;

typedef struct {
    void (*OnEnter)(void);
    void (*OnExit)(void);
    void (*Run)(void);
} EngineContext;

void Engine_Main();

void Engine_SwitchContext(EngineContext ctx);
