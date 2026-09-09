#pragma once

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define LT_MAX_DAT_SIZE 10000
#define LT_DAT_HDR_SIZE 132

#define LTINFO(...)  printf("LibTerep | INFO: " __VA_ARGS__)
#define LTWARN(...)  printf("LibTerep | WARNING: " __VA_ARGS__)
#define LTERROR(...)                                \
    do {                                            \
        printf("LibTerep | ERROR: " __VA_ARGS__);   \
        abort();                                    \
    } while (0)

#ifdef NDEBUG
    #define LTASSERT(cond)                                                                  \
        if(!(cond)) {                                                                       \
            LTERROR("Assertion failed: %s (%s: %d)\n", #cond, __FILE__, __LINE__);          \
        }
#else // NDEBUG
    #define LTASSERT(cond) assert(cond)
#endif // NDEBUG

