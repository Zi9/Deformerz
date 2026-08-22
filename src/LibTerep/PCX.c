#include "PCX.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PCX_DEFAULT_SIZE 256

#define PCX_HEADER_SIZE 128
#define PCX_PALETTE_SIZE 768
struct __attribute__((__packed__)) PCXHeader {
    uint8_t identifier;
    uint8_t version;
    uint8_t encoding;
    uint8_t bitsPerPixel;

    uint16_t xMin;
    uint16_t yMin;
    uint16_t xMax;
    uint16_t yMax;

    uint16_t hDPI;
    uint16_t vDPI;

    struct RGBColor colmap[16];

    uint8_t reserved;
    uint8_t nplanes;
    uint16_t bytesPerLine;
    uint16_t paletteInfo;
};

void (*PCX_postprocess_callback)(PCXData*) = NULL;

static PCXData* pcx_load_file(const char* path, uint16_t targ_width, uint16_t targ_height)
{
    PCXData* pcx = malloc(sizeof(PCXData));
    FILE* fp = fopen(path, "r");
    if (fp == NULL) {
        printf("Unable to open PCX image %s for reading\n", path);
        return NULL;
    }
    struct PCXHeader* hdr = malloc(PCX_HEADER_SIZE);
    assert(fread(hdr, PCX_HEADER_SIZE, 1, fp) == 1);
    if (hdr->identifier != 0x0A) {
        fclose(fp);
        free(hdr);
        printf("%s is not a valid pcx file\n", path);
        return NULL;
    }

    assert(hdr->encoding == 1);
    assert(hdr->bitsPerPixel == 8);
    assert(hdr->nplanes == 1);

    uint16_t width = hdr->xMax - hdr->xMin + 1;
    uint16_t height = hdr->yMax - hdr->yMin + 1;
    assert(width >= 256);

    size_t bufsz = hdr->bytesPerLine * hdr->nplanes * height;
    uint8_t* buf = malloc(bufsz);
    uint8_t in, repe;
    for (size_t bufi = 0; bufi < bufsz;) {
        if (fread(&in, sizeof(in), 1, fp) == 0)
            break;
        if ((0xC0 & in) == 0xC0) {
            repe = 0x3F & in;
            assert(fread(&in, sizeof(in), 1, fp) != 0);
            memset(buf + bufi, in, repe);
            bufi += repe;
        } else {
            *(buf + bufi) = in;
            bufi++;
        }
    }
    free(hdr);

    // *Resizing routine if requested
    if (targ_width != width && targ_height != height) {
        pcx->indices = malloc(targ_width * targ_height * sizeof *pcx->indices);
        if (height >= targ_height) {
            for (size_t y = 0; y < targ_height; y++) {
                memcpy(pcx->indices + (y * targ_height), buf + (y * width), targ_width * sizeof(uint8_t));
            }
        } else {
            size_t wrote_pixels = 0;
            for (size_t y = 0; y < height; y++) {
                memcpy(pcx->indices + (y * targ_height), buf + (y * width), targ_width * sizeof(uint8_t));
                wrote_pixels += targ_width * sizeof(uint8_t);
            }
            memset(pcx->indices + wrote_pixels, 0xFF, (targ_width * targ_height * sizeof(uint8_t)) - wrote_pixels);
        }
        free(buf);
    } else {
        pcx->indices = buf;
    }

    uint8_t palmagic;
    fread(&palmagic, sizeof(palmagic), 1, fp);
    assert(palmagic == 12);
    assert(fread(&pcx->palette, PCX_PALETTE_SIZE, 1, fp) == 1);

    fclose(fp);

    if (PCX_postprocess_callback != NULL) {
        PCX_postprocess_callback(pcx);
    }

    return pcx;
}

uint8_t* PCX_LoadArray(const char* path)
{
    PCXData* pcx = pcx_load_file(path, PCX_DEFAULT_SIZE, PCX_DEFAULT_SIZE);
    uint8_t* array = pcx->indices;
    free(pcx);
    return array;
}
PCXImage* PCX_LoadImage(const char* path)
{
    PCXData* pcx = pcx_load_file(path, PCX_DEFAULT_SIZE, PCX_DEFAULT_SIZE);

    PCXImage* img = malloc(sizeof(PCXImage));
    struct RGBAColor* pix = malloc(PCX_DEFAULT_SIZE * PCX_DEFAULT_SIZE * sizeof *pix);
    for (size_t i = 0; i < PCX_DEFAULT_SIZE * PCX_DEFAULT_SIZE; i++) {
        pix[i].red = pcx->palette[pcx->indices[i]].red;
        pix[i].green = pcx->palette[pcx->indices[i]].green;
        pix[i].blue = pcx->palette[pcx->indices[i]].blue;
        if (pcx->indices[i] == 255) {
            pix[i].alpha = 0;
        } else {
            pix[i].alpha = 255;
        }
    }
    img->data = pix;
    img->width = PCX_DEFAULT_SIZE;
    img->height = PCX_DEFAULT_SIZE;
    img->format = 7;
    img->mipmaps = 1;
    free(pcx->indices);
    free(pcx);
    return img;
}