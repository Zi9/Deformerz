#include "DFMap.h"
#include "Engine/Rendering/Shaders.h"
#include "Engine/Engine.h"

#include <assert.h>
#include <stdlib.h>

DFMap* DFMap_Load()
{
    DFMap* dfmap = calloc(1, sizeof(DFMap));
    assert(dfmap);
    dfmap->map = TerepMap_Load("./data/col.pcx", "./data/map.pcx", "./data/maptex.pcx");
    for (size_t i = 0; i < 256; i++)
    {
        Engine.palette[i].r = PCX_GLOBAL_PALETTE[i].red;
        Engine.palette[i].g = PCX_GLOBAL_PALETTE[i].green;
        Engine.palette[i].b = PCX_GLOBAL_PALETTE[i].blue;
        Engine.palette[i].a = 255;
    }
    Engine.skyColor = Engine.palette[255];
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
