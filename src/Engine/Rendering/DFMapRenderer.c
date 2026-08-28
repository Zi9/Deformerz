#include "Engine/Core/DFMap.h"

void Render_DFMap(DFMap* map)
{
    DrawModel(map->model, (Vector3){0.0f, -3.75f, 0.0f}, 1.0f, WHITE); // HACK: This is stinky, fix in the future
}
