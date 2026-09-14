#pragma once

#include <raylib.h>

void FreeCam_Update();

void FreeCam_BeginRender();
void FreeCam_EndRender();

Vector3 FreeCam_GetPos();
Vector2 FreeCam_GetRot();
void FreeCam_SetPos(Vector3 pos);
void FreeCam_SetRot(Vector2 rot);
void FreeCam_SetPosRot(Vector3 pos, Vector2 rot);
Camera FreeCam_GetRLCamera();