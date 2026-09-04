#define LIBTEREP_INTERNAL
#include "TerepCar.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TerepDat* _LoadDat(const char* path)
{
    TerepDat* dat = calloc(1, sizeof(TerepDat));
    assert(dat);
    FILE* f = fopen(path, "rb");
    strncpy(dat->name, path, 32);
    assert(fseek(f, 0, SEEK_END) == 0);
    dat->size = ftell(f);
    assert(fseek(f, 0, SEEK_SET) == 0);
    dat->data = calloc(1, dat->size);
    assert(dat->data);
    assert(fread(dat->data, dat->size, 1, f) == 1);
    fclose(f);
    return dat;
}
TerepDat* _CreateDat()
{
    TerepDat* dat = calloc(1, sizeof(TerepDat));
    assert(dat);
    dat->data = calloc(1, 10000);
    return dat;
}
void _UnloadDat(TerepDat* dat)
{
    free(dat->data);
    free(dat);
}
