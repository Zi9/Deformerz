#include "Engine/Core/DFMap.h"

void Renderer_RenderMap(DFMap* map)
{
    DrawModel(map->model, (Vector3){0.0f, -5.0f, 0.0f}, 1.0f, WHITE);
}
