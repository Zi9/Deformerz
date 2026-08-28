#pragma once

typedef struct {
    bool Active;
    enum {
        MODE_DEFAULT,
        MODE_EDIT_POINT,
        MODE_EDIT_PHYSLINK,
        MODE_EDIT_UV
    } Mode;
    int SelectedPoint;
    bool HandleMouseIn3D;
} CarEditorState;

extern CarEditorState CarEditor;
