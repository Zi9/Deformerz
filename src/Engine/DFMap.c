#include "DFMap.h"
#include "Engine/Shaders.h"

#include <assert.h>
#include <stdlib.h>

DFMap* DFMap_Load()
{
    DFMap* dfmap = calloc(1, sizeof(DFMap));
    assert(dfmap);
    dfmap->map = TerepMap_Load("./data/col.pcx", "./data/map.pcx", "./data/maptex.pcx");
    dfmap->tex = LoadTextureFromImage((Image){
        .data = dfmap->map->texturemap->data,
        .height = dfmap->map->texturemap->height,
        .width = dfmap->map->texturemap->width,
        .format = 7,
        .mipmaps = 1,
    });
    Mesh msh = {0};
    msh.triangleCount = dfmap->map->triangleCount;
    msh.vertexCount = dfmap->map->vertexCount;
    msh.vertices = dfmap->map->vertices;
    msh.texcoords = dfmap->map->uvs;
    UploadMesh(&msh, false);
    dfmap->model = LoadModelFromMesh(msh);
    dfmap->shader = LoadShaderFromMemory(Affine_vs, Affine_fs);
    dfmap->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = dfmap->tex;
    return dfmap;
}
void DFMap_Unload(DFMap* dfmap)
{
    TerepMap_Unload(dfmap->map);
    UnloadTexture(dfmap->tex);
    UnloadModel(dfmap->model);
    UnloadShader(dfmap->shader);
    free(dfmap);
}
void DFMap_Render(DFMap* dfmap)
{
    DrawModel(dfmap->model, (Vector3){0.0f, -3.75f, 0.0f}, 1.0f, WHITE); // HACK: This is stinky, fix in the future
}
