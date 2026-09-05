#include "LibTerep/TerepCar.h"
#include <stdio.h>

static inline int GetFileSize(FILE* f)
{
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    fseek(f, 0, SEEK_SET);
    return size;
}

static bool Validate(const char* file_base, const char* file_compare)
{
    bool result = false;
    FILE* file_original = fopen(file_base, "rb");
    FILE* file_validate = fopen(file_compare, "rb");

    int fsize_original = GetFileSize(file_original);
    int fsize_validate = GetFileSize(file_validate);

    if (fsize_original != fsize_validate) {
        printf("\033[31mValidation failed for DAT %s! Reason: Size mismatch (expected %i bytes but found %i "
               "bytes)\033[0m\n",
               file_base, fsize_original, fsize_validate);
        goto CLEANUP;
    }

    for (int i = 0; i < fsize_original; i++) {
        if (getc(file_original) != getc(file_validate)) {
            printf("\033[31mValidation failed for DAT %s! Reason: Byte value mismatch at %i\033[0m\n", file_base, i);
            goto CLEANUP;
        }
    }

    result = true;
    printf("\033[32mValidation successful for DAT %s! \033[0m\n", file_base);
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
