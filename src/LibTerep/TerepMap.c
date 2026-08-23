#include "TerepMap.h"
#include "PCX.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TMAP_SCALE 1.0f
#define TMAP_HEIGHT_SCALE 0.075f
#define TMAP_UVMULT 0.0625f

#define TEREP_MAPSZ 256
#define TEREP_TEXSZ 256

static TerepMap* currentMap;

static void build_map_model(TerepMap* map)
{
    map->triangleCount = (TEREP_MAPSZ - 1) * (TEREP_MAPSZ - 1) * 2;
    map->vertexCount = map->triangleCount * 3;

    float* vertices = malloc((sizeof *vertices) * map->vertexCount * 3);
    float* uvs = malloc((sizeof *uvs) * map->vertexCount * 2);

    size_t vertC = 0;
    size_t uvC = 0;

    float uvx, uvy;

    for (uint16_t z = 0; z < TEREP_MAPSZ - 1; z++) {
        for (uint16_t x = 0; x < TEREP_MAPSZ - 1; x++) {
            vertices[vertC] = (x - TEREP_MAPSZ / 2.0f) * TMAP_SCALE;
            vertices[vertC + 1] = map->heightmap->data[x + z * TEREP_MAPSZ] * TMAP_HEIGHT_SCALE;
            vertices[vertC + 2] = (z - TEREP_MAPSZ / 2.0f) * TMAP_SCALE;

            vertices[vertC + 3] = (x - TEREP_MAPSZ / 2.0f) * TMAP_SCALE;
            vertices[vertC + 4] = map->heightmap->data[x + (z + 1) * TEREP_MAPSZ] * TMAP_HEIGHT_SCALE;
            vertices[vertC + 5] = (z - TEREP_MAPSZ / 2.0f + 1) * TMAP_SCALE;

            vertices[vertC + 6] = (x - TEREP_MAPSZ / 2.0f + 1) * TMAP_SCALE;
            vertices[vertC + 7] = map->heightmap->data[x + 1 + z * TEREP_MAPSZ] * TMAP_HEIGHT_SCALE;
            vertices[vertC + 8] = (z - TEREP_MAPSZ / 2.0f) * TMAP_SCALE;

            vertices[vertC + 9] = vertices[vertC + 6];
            vertices[vertC + 10] = vertices[vertC + 7];
            vertices[vertC + 11] = vertices[vertC + 8];

            vertices[vertC + 12] = vertices[vertC + 3];
            vertices[vertC + 13] = vertices[vertC + 4];
            vertices[vertC + 14] = vertices[vertC + 5];

            vertices[vertC + 15] = (x - TEREP_MAPSZ / 2.0f + 1) * TMAP_SCALE;
            vertices[vertC + 16] = map->heightmap->data[x + 1 + (z + 1) * TEREP_MAPSZ] * TMAP_HEIGHT_SCALE;
            vertices[vertC + 17] = (z - TEREP_MAPSZ / 2.0f + 1) * TMAP_SCALE;

            vertC += 18;

            uvy = (float)floor(map->colormap->data[x + z * TEREP_TEXSZ] / 16.0f) * TMAP_UVMULT;
            uvx = map->colormap->data[x + z * TEREP_TEXSZ] * TMAP_UVMULT;

            uvs[uvC] = uvx;
            uvs[uvC + 1] = uvy;

            uvs[uvC + 2] = uvx;
            uvs[uvC + 3] = uvy + TMAP_UVMULT;

            uvs[uvC + 4] = uvx + TMAP_UVMULT;
            uvs[uvC + 5] = uvy;

            uvs[uvC + 6] = uvs[uvC + 4];
            uvs[uvC + 7] = uvs[uvC + 5];

            uvs[uvC + 8] = uvs[uvC + 2];
            uvs[uvC + 9] = uvs[uvC + 3];

            uvs[uvC + 10] = uvx + TMAP_UVMULT;
            uvs[uvC + 11] = uvy + TMAP_UVMULT;

            uvC += 12;
        }
    }

    map->vertices = vertices;
    map->uvs = uvs;
}

TerepMap* TerepMap_Load(const char* colpcx, const char* mappcx, const char* maptexpcx)
{
    TerepMap* map = malloc(sizeof *map);
    currentMap = map;
    map->colormap = PCX_LoadArray(colpcx);
    map->heightmap = PCX_LoadArray(mappcx);
    map->texturemap = PCX_LoadImage(maptexpcx);

    build_map_model(map);
    return map;
}
void TerepMap_Unload(TerepMap* map)
{
    free(map->colormap->data);
    free(map->heightmap->data);
    free(map->texturemap->data);
    free(map->colormap);
    free(map->heightmap);
    free(map->texturemap);
    free(map);
}
