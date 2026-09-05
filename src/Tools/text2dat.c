#include "LibTerep/TerepCar.h"
#include <stdio.h>
#include <unistd.h>

int main(int argc, const char* argv[])
{
    printf("\n");
    printf("████████╗███████╗██╗  ██╗████████╗██████╗ ██████╗  █████╗ ████████╗\n");
    printf("╚══██╔══╝██╔════╝╚██╗██╔╝╚══██╔══╝╚════██╗██╔══██╗██╔══██╗╚══██╔══╝\n");
    printf("   ██║   █████╗   ╚███╔╝    ██║    █████╔╝██║  ██║███████║   ██║   \n");
    printf("   ██║   ██╔══╝   ██╔██╗    ██║   ██╔═══╝ ██║  ██║██╔══██║   ██║   \n");
    printf("   ██║   ███████╗██╔╝ ██╗   ██║   ███████╗██████╔╝██║  ██║   ██║   \n");
    printf("   ╚═╝   ╚══════╝╚═╝  ╚═╝   ╚═╝   ╚══════╝╚═════╝ ╚═╝  ╚═╝   ╚═╝   \n");
    printf("-- Built by Zi9 for the LibTerep/Deformerz project ----------------\n\n");

    if (argc != 3) {
        printf("Invalid number of arguments!\n");
        printf("Use like this: ./text2dat car1.txt car1.dat\n");
        return -1;
    }
    if (access(argv[1], F_OK) != 0) {
        printf("text2dat | ERROR: Could not open source file %s!\n", argv[1]);
        return -1;
    }

    printf("text2dat | INFO: Converting %s to %s via LibTerep...\n", argv[1], argv[2]);
    TerepCar* car = TerepCar_LoadText(argv[1]);
    TerepCar_Write(car, argv[2], 0);
    TerepCar_Unload(car);
    printf("text2dat | INFO: Conversion completed!\n");

    return 0;
}
