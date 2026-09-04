#define LIBTEREP_INTERNAL
#include "TerepCar.h"

#include <stdio.h>
#include <string.h>

static void _WriteChunk1(TerepCar* car, FILE* f)
{
    fprintf(f, "POINTS_START:\n");
    for (size_t i = 0; i < car->pointCount; i++) {
        TerepCarPoint pt = car->points[i];
        fprintf(f, "  %s @ %.9f, %.9f, %.9f | Size: %.9f | Unknowns: %i, %i, %i\n", TerepCar_Point2String(&pt), pt.pos[0],
                pt.pos[1], pt.pos[2], pt.size, pt.unknown[0], pt.unknown[1], pt.unknown[2]);
    }
    fprintf(f, "POINTS_END:\n");
    printf("LibTerep | INFO: Converted %d points to text\n", car->pointCount);
}
static void _WriteChunk2(TerepCar* car, FILE* f)
{
    fprintf(f, "PHYSLINKS_START:\n");
    for (size_t i = 0; i < car->physLinkCount; i++) {
        TerepCarPhysLink pl = car->physLinks[i];
        fprintf(f, "  %s | %i <-> %i | Length: %.9f / %.9f | Min: %.9f | Max: %.9f\n", TerepCar_PhysLink2String(&pl),
                pl.pointA->index, pl.pointB->index, pl.len, pl.len2, pl.len_min, pl.len_max);
    }
    fprintf(f, "PHYSLINKS_END:\n");
    printf("LibTerep | INFO: Converted %d physics links to text\n", car->physLinkCount);
}
static void _WriteChunk3(TerepCar* car, FILE* f)
{
    fprintf(f, "RENDER_DATA_START:\n");
    for (int i = 0; i < car->renderDataCount; i++) {
        fprintf(f, "  %s | ", TerepCar_RenderType2String(&car->renderData[i]));
        switch (car->renderData[i].type) {
        case TEREP_RENDERDATA_NULL:
            fprintf(f, "\n");
            break;
        case TEREP_RENDERDATA_CAMERA: {
            TerepCarCameraData* cam = car->renderData[i].camera;
            fprintf(f, "Camera Point: %i | Unknowns: %i, %i\n", cam->cameraPoint->index, cam->unknown1, cam->unknown2);
            break;
        }
        case TEREP_RENDERDATA_UNK3_POLYGON: {
            // possibly some culling thing, changing these values seems to do render glitches
            TerepCarPolygonData* polygon = car->renderData[i].polygon;
            fprintf(f, "Points: %i, %i, %i | Unknowns: %i, %i, %i\n", polygon->vertices[0]->index,
                    polygon->vertices[1]->index, polygon->vertices[2]->index, polygon->unknown3values[0],
                    polygon->unknown3values[1], polygon->unknown3values[2]);
            break;
        }
        case TEREP_RENDERDATA_COLOR_POLYGON: {
            TerepCarPolygonData* polygon = car->renderData[i].polygon;
            fprintf(f, "Points: (%i) ", polygon->vertexCount);
            switch (polygon->vertexCount) {
            case 3:
                fprintf(f, "%i, %i, %i", polygon->vertices[0]->index, polygon->vertices[1]->index,
                        polygon->vertices[2]->index);
                break;
            case 4:
                fprintf(f, "%i, %i, %i, %i", polygon->vertices[0]->index, polygon->vertices[1]->index,
                        polygon->vertices[2]->index, polygon->vertices[3]->index);
                break;
            case 5:
                fprintf(f, "%i, %i, %i, %i, %i", polygon->vertices[0]->index, polygon->vertices[1]->index,
                        polygon->vertices[2]->index, polygon->vertices[3]->index, polygon->vertices[4]->index);
                break;
            }
            if (polygon->closed) {
                fprintf(f, ", %i | ", polygon->vertices[0]->index);
            } else {
                fprintf(f, ", 0 | ");
            }
            fprintf(f, "Colors: %i, %i\n", polygon->colors[0], polygon->colors[1]);
            break;
        }
        case TEREP_RENDERDATA_TEXTURE_POLYGON: {
            TerepCarPolygonData* polygon = car->renderData[i].polygon;
            fprintf(f, "Points: (%i) ", polygon->vertexCount);
            for (size_t i = 0; i < polygon->vertexCount; i++) {
                fprintf(f, "%i (U:%.9f, V:%.9f), ", polygon->vertices[i]->index, polygon->uv[i].x, polygon->uv[i].y);
            }
            if (polygon->closed) {
                fprintf(f, "%i (U:%.9f, V:%.9f)\n", polygon->vertices[0]->index, polygon->uv[0].x, polygon->uv[0].y);
            } else {
                fprintf(f, "%i (U:%.9f, V:%.9f)\n", 0, 0.0, 0.0);
            }
            break;
        }
        case TEREP_RENDERDATA_WHEEL: {
            TerepCarWheelData* wheel = car->renderData[i].wheel;
            fprintf(f, "Point: %i | Unknowns: %i, %i\n", wheel->wheelPoint->index, wheel->unknown1, wheel->unknown2);
            for (int i = 0; i < 9; i++) {
                fprintf(f, "    WHEEL_SPRITE | H: %i W: %i UV:[", wheel->wheelSprites[i].sz_height,
                        wheel->wheelSprites[i].sz_width);
                for (int j = 0; j < 4; j++) {
                    fprintf(f, "(%.9f, %.9f)", wheel->wheelSprites[i].UV[j].x, wheel->wheelSprites[i].UV[j].y);
                    if (j < 3) {
                        fprintf(f, ", ");
                    }
                }
                fprintf(f, "]\n");
            }
            break;
        }
        }
    }
    fprintf(f, "RENDER_DATA_END:\n");
    printf("LibTerep | INFO: Converted %i render data items to text\n", car->renderDataCount);
}

void TerepCar_WriteText(TerepCar* car, const char* cartext)
{
    FILE* f = fopen(cartext, "w");
    fprintf(f, "# Converted using LibTerep\n");
    fprintf(f, "# Dump Version: 1\n");
    fprintf(f, "HEADER_START:\n");
    fprintf(f, "  Unknown1: %i\n", car->unknownHeaderValue1);
    fprintf(f, "  EngineSound: %i\n", car->engineSound);
    fprintf(f, "HEADER_END:\n");
    _WriteChunk1(car, f);
    _WriteChunk2(car, f);
    _WriteChunk3(car, f);

    fclose(f);
    printf("LibTerep | INFO: Finished writing %s!\n", cartext);
}
