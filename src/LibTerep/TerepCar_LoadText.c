#include <string.h>
#define LIBTEREP_INTERNAL
#include "TerepCar.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define STARTSWITH(pfx) (strncmp((_tmp), (pfx), sizeof(pfx) - 1) == 0)

char _tmp[512];

static void _ParseChunk1(TerepCar* car, FILE* f)
{
    while (fgets(_tmp, sizeof(_tmp), f) != NULL) {
        if (STARTSWITH("POINTS_END:")) {
            printf("LibTerep | INFO: Loaded %d points from text\n", car->pointCount);
            return;
        }
        car->points = realloc(car->points, sizeof(TerepCarPoint) * (car->pointCount + 1));
        TerepCarPoint* p = &car->points[car->pointCount];
        if (STARTSWITH("  WHEEL_F")) {
            sscanf(_tmp, "  WHEEL_F @ %f, %f, %f | Size: %f | Unknowns: %hu, %hu, %hu", &p->pos[0], &p->pos[1],
                   &p->pos[2], &p->size, &p->unknown[0], &p->unknown[1], &p->unknown[2]);
            p->type = TEREP_POINT_WHEEL_FRONT;
        } else if (STARTSWITH("  WHEEL_R")) {
            sscanf(_tmp, "  WHEEL_R @ %f, %f, %f | Size: %f | Unknowns: %hu, %hu, %hu", &p->pos[0], &p->pos[1],
                   &p->pos[2], &p->size, &p->unknown[0], &p->unknown[1], &p->unknown[2]);
            p->type = TEREP_POINT_WHEEL_REAR;
        } else if (STARTSWITH("  GEOMETRY")) {
            sscanf(_tmp, "  GEOMETRY @ %f, %f, %f | Size: %f | Unknowns: %hu, %hu, %hu", &p->pos[0], &p->pos[1],
                   &p->pos[2], &p->size, &p->unknown[0], &p->unknown[1], &p->unknown[2]);
            p->type = TEREP_POINT_GEOMETRY;
        } else if (STARTSWITH("  CAMERA")) {
            sscanf(_tmp, "  CAMERA @ %f, %f, %f | Size: %f | Unknowns: %hu, %hu, %hu", &p->pos[0], &p->pos[1],
                   &p->pos[2], &p->size, &p->unknown[0], &p->unknown[1], &p->unknown[2]);
            p->type = TEREP_POINT_CAMERA;
        }
        p->index = car->pointCount;
        car->pointCount++;
    }
}

static void _ParseChunk2(TerepCar* car, FILE* f)
{
    while (fgets(_tmp, sizeof(_tmp), f) != NULL) {
        if (STARTSWITH("PHYSLINKS_END:")) {
            printf("LibTerep | INFO: Loaded %d physics links from text\n", car->physLinkCount);
            return;
        }
        car->physLinks = realloc(car->physLinks, sizeof(TerepCarPhysLink) * (car->physLinkCount + 1));
        TerepCarPhysLink* p = &car->physLinks[car->physLinkCount];
        int pointA = 0;
        int pointB = 0;
        if (STARTSWITH("  NORMAL")) {
            sscanf(_tmp, "  NORMAL | %i <-> %i | Length: %f / %f | Min: %f | Max: %f", &pointA, &pointB, &p->len,
                   &p->len2, &p->len_min, &p->len_max);
            p->type = TEREP_PHYSLINK_NORMAL;
        } else if (STARTSWITH("  REAR4")) {
            sscanf(_tmp, "  REAR4 | %i <-> %i | Length: %f / %f | Min: %f | Max: %f", &pointA, &pointB, &p->len,
                   &p->len2, &p->len_min, &p->len_max);
            p->type = TEREP_PHYSLINK_SUSP_REAR4;
        } else if (STARTSWITH("  REAR6")) {
            sscanf(_tmp, "  REAR6 | %i <-> %i | Length: %f / %f | Min: %f | Max: %f", &pointA, &pointB, &p->len,
                   &p->len2, &p->len_min, &p->len_max);
            p->type = TEREP_PHYSLINK_SUSP_REAR6;
        } else if (STARTSWITH("  FRONT10")) {
            sscanf(_tmp, "  FRONT10 | %i <-> %i | Length: %f / %f | Min: %f | Max: %f", &pointA, &pointB, &p->len,
                   &p->len2, &p->len_min, &p->len_max);
            p->type = TEREP_PHYSLINK_SUSP_FRONT10;
        } else if (STARTSWITH("  FRONT12")) {
            sscanf(_tmp, "  FRONT12 | %i <-> %i | Length: %f / %f | Min: %f | Max: %f", &pointA, &pointB, &p->len,
                   &p->len2, &p->len_min, &p->len_max);
            p->type = TEREP_PHYSLINK_SUSP_FRONT12;
        } else if (STARTSWITH("  SUSP_EXTRA")) {
            sscanf(_tmp, "  SUSP_EXTRA | %i <-> %i | Length: %f / %f | Min: %f | Max: %f", &pointA, &pointB, &p->len,
                   &p->len2, &p->len_min, &p->len_max);
            p->type = TEREP_PHYSLINK_SUSP_EXTRA;
        }
        p->pointA = &car->points[pointA];
        p->pointB = &car->points[pointB];
        car->physLinkCount++;
    }
}

static void _ParseChunk3(TerepCar* car, FILE* f)
{
    while (fgets(_tmp, sizeof(_tmp), f) != NULL) {
        if (STARTSWITH("RENDER_DATA_END:")) {
            printf("LibTerep | INFO: Loaded %d render data items from text\n", car->renderDataCount);
            return;
        }
        car->renderData = realloc(car->renderData, sizeof(TerepCarRenderDataItem) * (car->renderDataCount + 1));
        TerepCarRenderDataItem* p = &car->renderData[car->renderDataCount];
        if (STARTSWITH("  NULL")) {
            p->type = TEREP_RENDERDATA_NULL;
        } else if (STARTSWITH("  CAMERADATA")) {
            p->type = TEREP_RENDERDATA_CAMERA;
            p->camera = calloc(1, sizeof(TerepCarCameraData));
            int camerapoint = 0;
            sscanf(_tmp, "  CAMERADATA | Camera Point: %i | Unknowns: %hhu, %hhu", &camerapoint, &p->camera->unknown1,
                   &p->camera->unknown2);
            p->camera->cameraPoint = &car->points[camerapoint];
        } else if (STARTSWITH("  UNK3_POLYGON")) {
            p->type = TEREP_RENDERDATA_UNK3_POLYGON;
            p->polygon = calloc(1, sizeof(TerepCarPolygonData));
            int p0 = 0;
            int p1 = 0;
            int p2 = 0;
            sscanf(_tmp, "  UNK3_POLYGON | Points: %i, %i, %i | Unknowns: %hu, %hu, %hu", &p0, &p1, &p2,
                   &p->polygon->unknown3values[0], &p->polygon->unknown3values[1], &p->polygon->unknown3values[2]);
            p->polygon->vertices[0] = &car->points[p0];
            p->polygon->vertices[1] = &car->points[p1];
            p->polygon->vertices[2] = &car->points[p2];
        } else if (STARTSWITH("  COLOR_POLYGON | Points: (3)")) {
            p->type = TEREP_RENDERDATA_COLOR_POLYGON;
            p->polygon = calloc(1, sizeof(TerepCarPolygonData));
            p->polygon->vertexCount = 3;
            int p0 = 0;
            int p1 = 0;
            int p2 = 0;
            int p3 = 0;
            sscanf(_tmp, "  COLOR_POLYGON | Points: (3) %i, %i, %i, %i | Colors: %hhu, %hhu", &p0, &p1, &p2, &p3,
                   &p->polygon->colors[0], &p->polygon->colors[1]);
            p->polygon->vertices[0] = &car->points[p0];
            p->polygon->vertices[1] = &car->points[p1];
            p->polygon->vertices[2] = &car->points[p2];
            p->polygon->closed = p3 == 0 ? false : true;
        } else if (STARTSWITH("  COLOR_POLYGON | Points: (4)")) {
            p->type = TEREP_RENDERDATA_COLOR_POLYGON;
            p->polygon = calloc(1, sizeof(TerepCarPolygonData));
            p->polygon->vertexCount = 4;
            int p0 = 0;
            int p1 = 0;
            int p2 = 0;
            int p3 = 0;
            int p4 = 0;
            sscanf(_tmp, "  COLOR_POLYGON | Points: (4) %i, %i, %i, %i, %i | Colors: %hhu, %hhu", &p0, &p1, &p2, &p3,
                   &p4, &p->polygon->colors[0], &p->polygon->colors[1]);
            p->polygon->vertices[0] = &car->points[p0];
            p->polygon->vertices[1] = &car->points[p1];
            p->polygon->vertices[2] = &car->points[p2];
            p->polygon->vertices[3] = &car->points[p3];
            p->polygon->closed = p4 == 0 ? false : true;
        } else if (STARTSWITH("  COLOR_POLYGON | Points: (5)")) {
            p->type = TEREP_RENDERDATA_COLOR_POLYGON;
            p->polygon = calloc(1, sizeof(TerepCarPolygonData));
            p->polygon->vertexCount = 5;
            int p0 = 0;
            int p1 = 0;
            int p2 = 0;
            int p3 = 0;
            int p4 = 0;
            int p5 = 0;
            sscanf(_tmp, "  COLOR_POLYGON | Points: (5) %i, %i, %i, %i, %i, %i | Colors: %hhu, %hhu", &p0, &p1, &p2,
                   &p3, &p4, &p5, &p->polygon->colors[0], &p->polygon->colors[1]);
            p->polygon->vertices[0] = &car->points[p0];
            p->polygon->vertices[1] = &car->points[p1];
            p->polygon->vertices[2] = &car->points[p2];
            p->polygon->vertices[3] = &car->points[p3];
            p->polygon->vertices[4] = &car->points[p4];
            p->polygon->closed = p5 == 0 ? false : true;
        } else if (STARTSWITH("  TEXTURE_POLYGON | Points: (3)")) {
            p->type = TEREP_RENDERDATA_TEXTURE_POLYGON;
            p->polygon = calloc(1, sizeof(TerepCarPolygonData));
            p->polygon->vertexCount = 3;
            int p0 = 0;
            int p1 = 0;
            int p2 = 0;
            int p3 = 0;
            sscanf(_tmp, "  TEXTURE_POLYGON | Points: (3) %i (U:%f, V:%f), %i (U:%f, V:%f), %i (U:%f, V:%f), %i", &p0,
                   &p->polygon->uv[0].x, &p->polygon->uv[0].y, &p1, &p->polygon->uv[1].x, &p->polygon->uv[1].y, &p2,
                   &p->polygon->uv[2].x, &p->polygon->uv[2].y, &p3);
            p->polygon->vertices[0] = &car->points[p0];
            p->polygon->vertices[1] = &car->points[p1];
            p->polygon->vertices[2] = &car->points[p2];
            p->polygon->closed = p3 == 0 ? false : true;
        } else if (STARTSWITH("  TEXTURE_POLYGON | Points: (4)")) {
            p->type = TEREP_RENDERDATA_TEXTURE_POLYGON;
            p->polygon = calloc(1, sizeof(TerepCarPolygonData));
            p->polygon->vertexCount = 4;
            int p0 = 0;
            int p1 = 0;
            int p2 = 0;
            int p3 = 0;
            int p4 = 0;
            sscanf(_tmp,
                   "  TEXTURE_POLYGON | Points: (4) %i (U:%f, V:%f), %i (U:%f, V:%f), %i (U:%f, V:%f), %i (U:%f, "
                   "V:%f), %i",
                   &p0, &p->polygon->uv[0].x, &p->polygon->uv[0].y, &p1, &p->polygon->uv[1].x, &p->polygon->uv[1].y,
                   &p2, &p->polygon->uv[2].x, &p->polygon->uv[2].y, &p3, &p->polygon->uv[3].x, &p->polygon->uv[3].y,
                   &p4);
            p->polygon->vertices[0] = &car->points[p0];
            p->polygon->vertices[1] = &car->points[p1];
            p->polygon->vertices[2] = &car->points[p2];
            p->polygon->vertices[3] = &car->points[p3];
            p->polygon->closed = p4 == 0 ? false : true;
        } else if (STARTSWITH("  TEXTURE_POLYGON | Points: (5)")) {
            p->type = TEREP_RENDERDATA_TEXTURE_POLYGON;
            p->polygon = calloc(1, sizeof(TerepCarPolygonData));
            p->polygon->vertexCount = 5;
            int p0 = 0;
            int p1 = 0;
            int p2 = 0;
            int p3 = 0;
            int p4 = 0;
            int p5 = 0;
            sscanf(_tmp,
                   "  TEXTURE_POLYGON | Points: (5) %i (U:%f, V:%f), %i (U:%f, V:%f), %i (U:%f, V:%f), %i (U:%f, "
                   "V:%f), %i (U:%f, V:%f), %i",
                   &p0, &p->polygon->uv[0].x, &p->polygon->uv[0].y, &p1, &p->polygon->uv[1].x, &p->polygon->uv[1].y,
                   &p2, &p->polygon->uv[2].x, &p->polygon->uv[2].y, &p3, &p->polygon->uv[3].x, &p->polygon->uv[3].y,
                   &p4, &p->polygon->uv[4].x, &p->polygon->uv[4].y, &p5);
            p->polygon->vertices[0] = &car->points[p0];
            p->polygon->vertices[1] = &car->points[p1];
            p->polygon->vertices[2] = &car->points[p2];
            p->polygon->vertices[3] = &car->points[p3];
            p->polygon->vertices[4] = &car->points[p4];
            p->polygon->closed = p5 == 0 ? false : true;
        } else if (STARTSWITH("  WHEELDATA")) {
            p->type = TEREP_RENDERDATA_WHEEL;
            p->wheel = calloc(1, sizeof(TerepCarWheelData));
            int p0 = 0;
            sscanf(_tmp, "  WHEELDATA | Point: %i | Unknowns: %hhu, %hhu", &p0, &p->camera->unknown1,
                   &p->camera->unknown2);
            p->wheel->wheelPoint = &car->points[p0];
            for (int i = 0; i < 9; i++) {
                fgets(_tmp, sizeof(_tmp), f);
                sscanf(_tmp, "    WHEEL_SPRITE | H: %hu W: %hu UV:[(%f, %f), (%f, %f), (%f, %f), (%f, %f)]",
                       &p->wheel->wheelSprites[i].sz_height, &p->wheel->wheelSprites[i].sz_width,
                       &p->wheel->wheelSprites[i].UV[0].x, &p->wheel->wheelSprites[i].UV[0].y,
                       &p->wheel->wheelSprites[i].UV[1].x, &p->wheel->wheelSprites[i].UV[1].y,
                       &p->wheel->wheelSprites[i].UV[2].x, &p->wheel->wheelSprites[i].UV[2].y,
                       &p->wheel->wheelSprites[i].UV[3].x, &p->wheel->wheelSprites[i].UV[3].y);
            }
        }
        car->renderDataCount++;
    }
}

static void _ParseHeader(TerepCar* car, FILE* f)
{
    while (fgets(_tmp, sizeof(_tmp), f) != NULL) {
        if (STARTSWITH("HEADER_END:")) {
            return;
        } else if (STARTSWITH("  Unknown1:")) {
            sscanf(_tmp, "  Unknown1: %hu", &car->unknownHeaderValue1);
        } else if (STARTSWITH("  EngineSound:")) {
            sscanf(_tmp, "  EngineSound: %hu", &car->engineSound);
        } else {
            printf("Bad header value\n");
        }
    }
}

TerepCar* TerepCar_LoadText(const char* cartext)
{
    TerepCar* car = calloc(1, sizeof *car);
    assert(car);
    FILE* f = fopen(cartext, "r");

    while (fgets(_tmp, sizeof(_tmp), f) != NULL) {
        if (STARTSWITH("#")) {
            continue;
        } else if (STARTSWITH("HEADER_START:")) {
            _ParseHeader(car, f);
        } else if (STARTSWITH("POINTS_START:")) {
            _ParseChunk1(car, f);
        } else if (STARTSWITH("PHYSLINKS_START:")) {
            _ParseChunk2(car, f);
        } else if (STARTSWITH("RENDER_DATA_START:")) {
            _ParseChunk3(car, f);
        }
    }

    printf("LibTerep | INFO: Finished parsing text from %s\n", cartext);

    fclose(f);
    return car;
}
