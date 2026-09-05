#include "LibTerep/TerepCar.h"
#include <stdio.h>

#ifndef WIN32
#include <unistd.h>
#endif // WIN32

int main(int argc, const char* argv[])
{
#ifndef WIN32
    printf("\n");
    printf("██████╗  █████╗ ████████╗██████╗ ████████╗███████╗██╗  ██╗████████╗\n");
    printf("██╔══██╗██╔══██╗╚══██╔══╝╚════██╗╚══██╔══╝██╔════╝╚██╗██╔╝╚══██╔══╝\n");
    printf("██║  ██║███████║   ██║    █████╔╝   ██║   █████╗   ╚███╔╝    ██║   \n");
    printf("██║  ██║██╔══██║   ██║   ██╔═══╝    ██║   ██╔══╝   ██╔██╗    ██║   \n");
    printf("██████╔╝██║  ██║   ██║   ███████╗   ██║   ███████╗██╔╝ ██╗   ██║   \n");
    printf("╚═════╝ ╚═╝  ╚═╝   ╚═╝   ╚══════╝   ╚═╝   ╚══════╝╚═╝  ╚═╝   ╚═╝   \n");
    printf("-- Built by Zi9 for the LibTerep/Deformerz project ----------------\n\n");
#else // WIN32
    printf("--==[ dat2text ]==--\n");
    printf("-- Built by Zi9 for the LibTerep/Deformerz project\n\n");
#endif // WIN32

    if (argc != 3) {
        printf("Invalid number of arguments!\n");
        printf("Use like this: ./dat2text car1.dat car1.txt\n");
        return -1;
    }
#ifndef WIN32
    if (access(argv[1], F_OK) != 0) {
        printf("dat2text | ERROR: Could not open source file %s!\n", argv[1]);
        return -1;
    }
#endif // WIN32

    printf("dat2text | INFO: Converting %s to %s via LibTerep...\n", argv[1], argv[2]);
    TerepCar* car = TerepCar_Load(argv[1], 0);
    TerepCar_WriteText(car, argv[2]);
    TerepCar_Unload(car);
    printf("dat2text | INFO: Conversion completed!\n");

    return 0;
}
