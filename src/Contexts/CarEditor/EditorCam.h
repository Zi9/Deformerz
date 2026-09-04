#pragma once

#include <raylib.h>

void EditorCam_Update();

void EditorCam_BeginRender();
void EditorCam_EndRender();

Vector3 EditorCam_GetPos();
Vector2 EditorCam_GetRot();
void EditorCam_SetPos(Vector3 pos);
void EditorCam_SetRot(Vector2 rot);
void EditorCam_SetPosRot(Vector3 pos, Vector2 rot);
Camera EditorCam_GetRLCamera();