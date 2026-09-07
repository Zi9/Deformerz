#define LIBTEREP_INTERNAL
#include "TerepCar.h"
#include "LibTerep.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

TerepDat* _LoadDat(const char* path)
{
    TerepDat* dat = calloc(1, sizeof(TerepDat));
    if (!dat) {
        LTERROR("Failed to allocate memory for TerepDat\n");
        return NULL;
    }
    FILE* f = fopen(path, "rb");
    if (!f) {
        LTERROR("Failed to open file %s: %s\n", path, strerror(errno));
        goto CLEANERR1;
    }
    strncpy(dat->name, path, sizeof(dat->name));
    int rc = fseek(f, 0, SEEK_END);
    if (rc != 0) {
        LTERROR("Failed to seek to end of %s\n", path);
        goto CLEANERR2;
    }
    long size = ftell(f);
    if (size <= 0) {
        LTERROR("Invalid size for %s\n", path);
        goto CLEANERR2;
    }
    dat->size = (uint64_t) size;
    rc = fseek(f, 0, SEEK_SET);
    if (rc != 0) {
        LTERROR("Failed to seek to start of %s\n", path);
        goto CLEANERR2;
    }
    if (dat->size > MAX_DAT_SIZE) {
        LTERROR("File %s is too large (%zu bytes), max allowed is %d bytes\n", path, dat->size, MAX_DAT_SIZE);
        goto CLEANERR2;
    }
    dat->data = calloc(1, dat->size);
    if (!dat->data) {
        LTERROR("Failed to allocate memory for data of TerepDat\n");
        goto CLEANERR2;
    }
    size_t nread = fread(dat->data, 1, dat->size, f);
    if (nread != dat->size) {
        LTERROR("Failed to read from %s, read %zu, expected %zu\n", path, nread, dat->size);
        goto CLEANERR3;
    }
    fclose(f);
    return dat;

CLEANERR3:
    fclose(dat->data);
CLEANERR2:
    fclose(f);
CLEANERR1:
    free(dat);
    return NULL;
}
TerepDat* _CreateDat()
{
    TerepDat* dat = calloc(1, sizeof(TerepDat));
    if (!dat) {
        LTERROR("Failed to allocate memory for TerepDat\n");
        return NULL;
    }
    dat->data = calloc(1, MAX_DAT_SIZE);
    if (!dat->data) {
        free(dat);
        LTERROR("Failed to allocate memory for data of TerepDat\n");
        return NULL;
    }
    return dat;
}
void _UnloadDat(TerepDat* dat)
{
    if (dat) {
        free(dat->data);
    }
    free(dat);
}
