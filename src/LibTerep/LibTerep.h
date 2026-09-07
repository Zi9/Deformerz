#pragma once

#include <stdio.h>

#define MAX_DAT_SIZE 10000

#define LTINFO(...)  printf("LibTerep | INFO: " __VA_ARGS__)
#define LTWARN(...)  printf("LibTerep | WARNING: " __VA_ARGS__)
#define LTERROR(...) printf("LibTerep | ERROR: " __VA_ARGS__)
