#define LIBTEREP_INTERNAL
#include "TerepCar.h"
#include "LibTerep.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TerepDat* _LoadDat(const char* path)
{
    int rc;
    TerepDat* dat = calloc(1, sizeof(TerepDat));
    LTASSERT(dat);
    FILE* f = fopen(path, "rb");
    LTASSERT(f);
    strncpy(dat->name, path, sizeof(dat->name));
    rc = fseek(f, 0, SEEK_END);
    LTASSERT(rc == 0);
    dat->size = ftell(f);
    LTASSERT(dat->size < LT_MAX_DAT_SIZE);
    rc = fseek(f, 0, SEEK_SET);
    LTASSERT(rc == 0);
    dat->data = calloc(1, dat->size);
    LTASSERT(dat->data);
    size_t nread = fread(dat->data, 1, dat->size, f);
    LTASSERT(nread == dat->size);
    fclose(f);
    return dat;
}
TerepDat* _CreateDat()
{
    TerepDat* dat = calloc(1, sizeof(TerepDat));
    LTASSERT(dat);
    dat->data = calloc(1, LT_MAX_DAT_SIZE);
    LTASSERT(dat->data);
    return dat;
}
void _UnloadDat(TerepDat* dat)
{
    if (!dat) {
        return;
    }
    free(dat->data);
    free(dat);
}
