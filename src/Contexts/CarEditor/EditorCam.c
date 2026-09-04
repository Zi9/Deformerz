#include "EditorCam.h"

#include <math.h>
#include <raylib.h>
#include <raymath.h>

#define CAMERA_DEFAULT_MOUSE_SENS 0.1
#define CAMERA_DEFAULT_SPEED 1.0
#define CAMERA_SPEED_MOD_MULT 10

struct EditorCam {
    float mouseSens, camSpeed;
    Vector2 view;
    bool freecamEnabled;
    Vector3 worldUp, right, up, forward;
    Vector2 mousePos, prevMousePos, mouseDelta;
    Camera3D rlCam;
} CAM = {
    .mouseSens = CAMERA_DEFAULT_MOUSE_SENS,
    .camSpeed = CAMERA_DEFAULT_SPEED,
    .view = {-90, 0},
    .freecamEnabled = false,
    .worldUp = {0, 1, 0},
    .right = {1, 0, 0},
    .up = {0, 1, 0},
    .forward = {0, 0, -1},
    .mouseDelta = {0, 0},
    .rlCam = {
        .position = {0, 0, 0}, .target = {0, 0, -1}, .up = {0, 1, 0}, .fovy = 75, .projection = CAMERA_PERSPECTIVE}};

static void _SetFreecam(bool enable)
{
    CAM.freecamEnabled = enable;
    if (enable) {
        DisableCursor();
        CAM.prevMousePos = GetMousePosition();
    } else {
        EnableCursor();
    }
}

static void _RecalculateCamera()
{
    CAM.forward.x = cosf(DEG2RAD * CAM.view.x) * cosf(DEG2RAD * CAM.view.y);
    CAM.forward.y = sinf(DEG2RAD * CAM.view.y);
    CAM.forward.z = sinf(DEG2RAD * CAM.view.x) * cosf(DEG2RAD * CAM.view.y);
    CAM.forward = Vector3Normalize(CAM.forward);
    CAM.right = Vector3Normalize(Vector3CrossProduct(CAM.forward, CAM.worldUp));
    CAM.up = Vector3Normalize(Vector3CrossProduct(CAM.right, CAM.forward));
    CAM.rlCam.target = Vector3Add(CAM.rlCam.position, CAM.forward);
}

void EditorCam_BeginRender() { BeginMode3D(CAM.rlCam); }
void EditorCam_EndRender() { EndMode3D(); }

void EditorCam_SetPos(Vector3 pos)
{
    CAM.rlCam.position = pos;
    _RecalculateCamera();
}
void EditorCam_SetRot(Vector2 rot)
{
    CAM.view = rot;
    _RecalculateCamera();
}
void EditorCam_SetPosRot(Vector3 pos, Vector2 rot)
{
    CAM.rlCam.position = pos;
    CAM.view = rot;
    _RecalculateCamera();
}
Vector3 EditorCam_GetPos() { return CAM.rlCam.position; }
Vector2 EditorCam_GetRot() { return CAM.view; }

void EditorCam_Update()
{
    if (IsKeyPressed(KEY_F3)) {
        if (CAM.freecamEnabled)
            _SetFreecam(false);
        else
            _SetFreecam(true);
    }
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
        _SetFreecam(true);
    if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON))
        _SetFreecam(false);

    if (CAM.freecamEnabled) {
        float mwheel = GetMouseWheelMove();
        if (mwheel != 0) {
            if (IsKeyDown(KEY_LEFT_CONTROL)) {
                CAM.rlCam.fovy -= GetMouseWheelMove() * 2;
            } else {
                CAM.camSpeed += GetMouseWheelMove() * 2;
                if (CAM.camSpeed <= 0)
                    CAM.camSpeed = 2;
            }
        }
        CAM.mousePos = GetMousePosition();
        CAM.mouseDelta = Vector2Subtract(CAM.mousePos, CAM.prevMousePos);
        CAM.prevMousePos = CAM.mousePos;

        CAM.view.x += CAM.mouseDelta.x * CAM.mouseSens;
        CAM.view.y += -CAM.mouseDelta.y * CAM.mouseSens;
        if (CAM.view.y > 89.9f)
            CAM.view.y = 89.9f;
        else if (CAM.view.y < -89.9f)
            CAM.view.y = -89.9f;

        float vel = GetFrameTime();
        if (IsKeyDown(KEY_LEFT_SHIFT))
            vel *= CAM.camSpeed * CAMERA_SPEED_MOD_MULT;
        else if (IsKeyDown(KEY_LEFT_ALT))
            vel *= CAM.camSpeed / CAMERA_SPEED_MOD_MULT;
        else
            vel *= CAM.camSpeed;

        Vector3 v = {vel, vel, vel};
        if (IsKeyDown(KEY_W))
            CAM.rlCam.position = Vector3Add(CAM.rlCam.position, Vector3Multiply(CAM.forward, v));
        if (IsKeyDown(KEY_S))
            CAM.rlCam.position = Vector3Subtract(CAM.rlCam.position, Vector3Multiply(CAM.forward, v));
        if (IsKeyDown(KEY_D))
            CAM.rlCam.position = Vector3Add(CAM.rlCam.position, Vector3Multiply(CAM.right, v));
        if (IsKeyDown(KEY_A))
            CAM.rlCam.position = Vector3Subtract(CAM.rlCam.position, Vector3Multiply(CAM.right, v));
        if (IsKeyDown(KEY_E))
            CAM.rlCam.position = Vector3Add(CAM.rlCam.position, Vector3Multiply(CAM.up, v));
        if (IsKeyDown(KEY_Q))
            CAM.rlCam.position = Vector3Subtract(CAM.rlCam.position, Vector3Multiply(CAM.up, v));
        _RecalculateCamera();
    }
}
Camera EditorCam_GetRLCamera() { return CAM.rlCam; }
