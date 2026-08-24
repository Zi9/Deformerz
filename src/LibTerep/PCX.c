#include "PCX.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PCX_DEFAULT_SIZE 256

#define PCX_HEADER_SIZE 128
#define PCX_PALETTE_SIZE 768

#ifdef WIN32
struct PCXHeader {
#elif //WIN32
struct __attribute__((__packed__)) PCXHeader {
#endif
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

bool USE_GLOBAL_PALETTE_FOR_LOADING = false;
struct RGBColor PCX_GLOBAL_PALETTE[256] = {0};

static PCXData* pcx_load_file(const char* path, uint16_t targ_width, uint16_t targ_height)
{
    PCXData* pcx = calloc(1, sizeof(PCXData));
    assert(pcx);
#ifdef WIN32
    FILE* fp = fopen(path, "rb");
#else
    FILE* fp = fopen(path, "r");
#endif
    if (fp == NULL) {
        printf("Unable to open PCX image %s for reading\n", path);
        return NULL;
    }
    struct PCXHeader* hdr = calloc(1, PCX_HEADER_SIZE);
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
    uint8_t* buf = calloc(1, bufsz);
    assert(buf);
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

    pcx->data = calloc(1, targ_width * targ_height * sizeof *pcx->data);
    assert(pcx->data);
    if (height >= targ_height) {
        for (size_t y = 0; y < targ_height; y++) {
            memcpy(pcx->data + (y * targ_height), buf + (y * width), targ_width * sizeof(uint8_t));
        }
    } else {
        size_t wrote_pixels = 0;
        for (size_t y = 0; y < height; y++) {
            memcpy(pcx->data + (y * targ_height), buf + (y * width), targ_width * sizeof(uint8_t));
            wrote_pixels += targ_width * sizeof(uint8_t);
        }
        memset(pcx->data + wrote_pixels, 0xFF, (targ_width * targ_height * sizeof(uint8_t)) - wrote_pixels);
    }
    free(buf);

    uint8_t palmagic;
    fread(&palmagic, sizeof(palmagic), 1, fp);
    assert(palmagic == 12);
    assert(fread(&pcx->palette, PCX_PALETTE_SIZE, 1, fp) == 1);

    fclose(fp);

    return pcx;
}

PCXData* PCX_LoadArray(const char* path) { return pcx_load_file(path, PCX_DEFAULT_SIZE, PCX_DEFAULT_SIZE); }
PCXImage* PCX_LoadImage(const char* path)
{
    PCXData* pcx = pcx_load_file(path, PCX_DEFAULT_SIZE, PCX_DEFAULT_SIZE);
    PCXImage* img = calloc(1, sizeof(PCXImage));
    assert(img);
    memcpy(img->palette, pcx->palette, PCX_PALETTE_SIZE);
    struct RGBAColor* pix = calloc(1, PCX_DEFAULT_SIZE * PCX_DEFAULT_SIZE * sizeof *pix);
    assert(pix);
    if (USE_GLOBAL_PALETTE_FOR_LOADING) {
        for (size_t i = 0; i < PCX_DEFAULT_SIZE * PCX_DEFAULT_SIZE; i++) {
            pix[i].red = PCX_GLOBAL_PALETTE[pcx->data[i]].red;
            pix[i].green = PCX_GLOBAL_PALETTE[pcx->data[i]].green;
            pix[i].blue = PCX_GLOBAL_PALETTE[pcx->data[i]].blue;
            if (pcx->data[i] == 255) {
                pix[i].alpha = 0;
            } else {
                pix[i].alpha = 255;
            }
        }
    } else {
        for (size_t i = 0; i < PCX_DEFAULT_SIZE * PCX_DEFAULT_SIZE; i++) {
            pix[i].red = pcx->palette[pcx->data[i]].red;
            pix[i].green = pcx->palette[pcx->data[i]].green;
            pix[i].blue = pcx->palette[pcx->data[i]].blue;
            if (pcx->data[i] == 255) {
                pix[i].alpha = 0;
            } else {
                pix[i].alpha = 255;
            }
        }
    }
    img->data = pix;
    img->width = PCX_DEFAULT_SIZE;
    img->height = PCX_DEFAULT_SIZE;
    free(pcx->data);
    free(pcx);
    return img;
}

void PCX_EnableGlobalPalette(const char* path)
{
#ifdef WIN32
    FILE* fp = fopen(path, "rb");
#else
    FILE* fp = fopen(path, "r");
#endif
    assert(fp);
    // Something funky here but it works so...
    assert(fseek(fp, 0, SEEK_END) == 0);
    unsigned long end = ftell(fp);
    assert(fseek(fp, end - PCX_PALETTE_SIZE, SEEK_SET) == 0);
    assert(fread(&PCX_GLOBAL_PALETTE, sizeof(PCX_GLOBAL_PALETTE), 1, fp) == 1);
    fclose(fp);
    USE_GLOBAL_PALETTE_FOR_LOADING = true;
}
void PCX_DisableGlobalPalette() { USE_GLOBAL_PALETTE_FOR_LOADING = false; }