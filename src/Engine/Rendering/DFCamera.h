#pragma once

#include <raylib.h>

void DFCamera_Update();

void DFCamera_BeginRender();
void DFCamera_EndRender();

Vector3 DFCamera_GetPos();
Vector2 DFCamera_GetRot();
void DFCamera_SetPos(Vector3 pos);
void DFCamera_SetRot(Vector2 rot);
void DFCamera_SetPosRot(Vector3 pos, Vector2 rot);
Camera DFCamera_GetRLCamera();