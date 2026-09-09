#include "LibTerep/TerepCar.h"
#include <stdio.h>
#include <assert.h>

static inline int GetFileSize(FILE* f)
{
    int rc;
    assert(f);
    rc = fseek(f, 0, SEEK_END);
    assert(rc == 0);
    int size = ftell(f);
    rc = fseek(f, 0, SEEK_SET);
    assert(rc == 0);
    return size;
}

static bool Validate(const char* file_base, const char* file_compare)
{
    bool result = false;
    FILE* file_original = fopen(file_base, "rb");
    assert(file_original);
    FILE* file_validate = fopen(file_compare, "rb");
    assert(file_validate);

    int fsize_original = GetFileSize(file_original);
    int fsize_validate = GetFileSize(file_validate);

    if (fsize_original != fsize_validate) {
#ifdef WIN32
        printf("-- Validation failed for DAT %s! Reason: Size mismatch (expected %i bytes but found %i bytes)\n",
               file_base, fsize_original, fsize_validate);
#else
        printf("\033[31mValidation failed for DAT %s! Reason: Size mismatch (expected %i bytes but found %i "
               "bytes)\033[0m\n",
               file_base, fsize_original, fsize_validate);
#endif
        goto CLEANUP;
    }

    for (int i = 0; i < fsize_original; i++) {
        if (getc(file_original) != getc(file_validate)) {
#ifdef WIN32
            printf("-- Validation failed for DAT %s! Reason: Byte value mismatch at %i\n", file_base, i);
#else
            printf("\033[31mValidation failed for DAT %s! Reason: Byte value mismatch at %i\033[0m\n", file_base, i);
#endif
            goto CLEANUP;
        }
    }

    result = true;
#ifdef WIN32
    printf("-- Validation successful for DAT %s!\n", file_base);
#else
    printf("\033[32mValidation successful for DAT %s! \033[0m\n", file_base);
#endif

CLEANUP:
    fclose(file_original);
    fclose(file_validate);
    return result;
}

static bool TestDATRebuild(const char* file, const char* outfile)
{
    TerepCar* car = TerepCar_Load(file, NULL);
    TerepCar_Write(car, outfile, NULL);
    TerepCar_Unload(car);
    bool result = Validate(file, outfile);
    return result;
}

static bool TestTextRebuild(const char* file, const char* outfile)
{
    char validation_text[128];
    snprintf(validation_text, 128, "%s.validation.txt", outfile);
    TerepCar* car = TerepCar_Load(file, NULL);
    TerepCar_WriteText(car, validation_text);
    TerepCar_Unload(car);

    car = TerepCar_LoadText(validation_text);
    TerepCar_Write(car, outfile, NULL);
    TerepCar_Unload(car);
    //remove(validation_text);

    bool result = Validate(file, outfile);

    return result;
}

int main()
{
    TestTextRebuild("./data/car1.dat", "./data/testenv/car1.dat");
    TestTextRebuild("./data/car2.dat", "./data/testenv/car2.dat");
    TestTextRebuild("./data/car3.dat", "./data/testenv/car3.dat");
    TestTextRebuild("./data/car4.dat", "./data/testenv/car4.dat");
    TestTextRebuild("./data/car5.dat", "./data/testenv/car5.dat");
}
