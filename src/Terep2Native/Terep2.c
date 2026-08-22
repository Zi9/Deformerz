#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static char mem[0xFFFF];
static FILE* f;

static void MapStaticMemory()
{
    printf("SETUP: Mapping 0x5ED0 to virtual memory from terep2.exe... ");
    FILE* f = fopen("terep2.exe", "r");
    fseek(f, 0x5ED0, 0);
    size_t bytesMapped = fread(mem, 1, sizeof(mem), f);
    fclose(f);
    printf("0x%X bytes OK!\n", (unsigned int)bytesMapped);
    printf("SETUP: Testing mapping 0x0000... (Should show upper text) -> %s\n", mem);
    printf("SETUP: Testing mapping 0x5B01... (Should show car1.dat)   -> %s\n", mem + 0x5B01);
}
static void DumpMemory()
{
    printf("DUMP: Dumping memory... ");
    FILE* f = fopen("/home/zi/dev/proj_deformers/re-workdir/RE/dump.bin", "wb");
    uint32_t written = fwrite(mem, 1, sizeof(mem), f);
    printf("%x OK\n", written);
    fclose(f);
}

static void MOV_byte(size_t addr, uint8_t val) { memcpy(mem + addr, &val, sizeof(val)); }
static void MOV_word(size_t addr, uint16_t val) { memcpy(mem + addr, &val, sizeof(val)); }
static void MOV_dword(size_t addr, uint32_t val) { memcpy(mem + addr, &val, sizeof(val)); }

static void CheckPCXHeader(FILE* f)
{
    // 5A9D
    fread(mem + 0xEF88, 0x80, 1, f);
    // 5AA3
    uint16_t AX;
    memcpy(&AX, mem + 0xEF90, sizeof(AX));
    printf("PARSEPCX: AX=%i\n", AX);
    // 5AA6
    AX = AX - *((uint16_t*)(mem + 0xEF8C));
    // 5AAA
    AX++;
    printf("PARSEPCX: after subtract and inc AX=%i\n", AX);
    // 5AAB
    memcpy(mem + 0xEF8C, &AX, sizeof(AX));

    // 5AAE
    uint16_t CX;
    memcpy(&CX, mem + 0xEF92, sizeof(CX));
    printf("PARSEPCX: CX=%i\n", CX);

    // 5AB2
    CX = CX - *((uint16_t*)(mem + 0xEF8E));
    // 5AAA
    CX++;
    printf("PARSEPCX: after subtract and inc CX=%i\n", CX);
    // 5AAB
    memcpy(mem + 0xEF82, &CX, sizeof(CX));
    printf("PARSEPCX: BPP must be 8    -> %x\n", *(mem + 0xEF8B));
    printf("PARSEPCX: PLANES must be 1 -> %x\n", *(mem + 0xEFC9));
}
static void ReadPCXPalette(FILE* f, uint16_t* SI)
{
    // 5B2A
    if (*SI == 0xF308) { // Figure out why this value specifically
        // 5B3A
        fread(mem + 0xF008, 0x300, 1, f); // Read only the first 0x300 bytes of the PCX data? Why???
        // 5B3E
        *SI = 0xF008;
    }
    // 5B41
    *SI = *SI + 1; // No clue why but okay
}
static void DecompressPCXData(FILE* f)
{
    // 5AD0
    uint16_t DX = 0xF008;
    // 5AD3
    uint16_t SI = 0xF308;
    // 5AD6
    uint16_t AX = 0x0;
    // 5AD9
    uint16_t CX;
    memcpy(&CX, mem + 0xEF82, sizeof(CX));

    // 5ADD
    // 5AE1
    if (CX < 0x100) {
        // 5AE5
        CX = 0x100;
    }
    // 5AE8 51            PUSH      CX
    // 5AE9 33 C9         XOR       CX, CX
    // 5AEB 22 E4         AND       AH, AH
    // 5AED 74 07         JZ        LAB_0824_5af6
    // 5AEF 90            NOP
    // 5AF0 90            NOP
    // 5AF1 FE CC         DEC       AH
    // 5AF3 EB 1C         JMP       LAB_0824_5b11
    // 5AF6 E8 2D 00      CALL      ReadPCXPalette
    // 5AF9 8A E0         MOV       AH, AL
    // 5AFB 80 E4 C0      AND       AH, 0xc0
    // 5AFE 80 FC C0      CMP       AH, 0xc0
    // 5B01 B4 00         MOV       AH, 0x0
    // 5B03 75 0C         JNZ       LAB_0824_5b11
    // 5B07 8A E0         MOV       AH, AL
    // 5B09 80 E4 3F      AND       AH, 0x3f
    // 5B0C E8 17 00      CALL      ReadPCXPalette
    // 5B0F FE CC         DEC       AH
    // 5B11 81 F9 00      CMP       CX, 0x100
    // 5B15 73 03         JNC       LAB_0824_5b1a
    // 5B19 AA            STOSB     ES:DI
    // 5B1A 41            INC       CX
    // 5B1B 3B 0E 80      CMP       CX, word ptr [0xef80]
    // 5B1F 72 CA         JC        LAB_0824_5aeb
    // 5B21 59            POP       CX
    // 5B22 E2 C4         LOOP      LAB_0824_5ae8
    // 5B25 C3            RET
}

static void LoadPCXFile(char* DX, uint16_t ES)
{
    printf("LOADPCX: %s -> %x\n", DX, ES);
    // 5A66
    FILE* f = fopen(DX, "rb");
    // 5A6E
    CheckPCXHeader(f);
    // 5A7E
    fseek(f, 128, SEEK_SET);
    // 5A84
    DecompressPCXData(f);
    // 5A8D
    fclose(f);
}

static void LoadMapAndOtherFiles()
{
    // 24CA
    LoadPCXFile(mem + 0x1A03, *((uint16_t*)(mem + 0x1A45))); // map.pcx

    // 24E1
    f = fopen("col.pcx", "r");
    // 24FC
    fseek(f, 768, SEEK_END);
    // 250A
    fread(mem + 0x1A4D, 1, 768, f);
    // 2515
    fseek(f, 128, SEEK_SET);
    // 2524
    // LoadPCXPalette();
    // 2526
    fclose(f);

    // 252F
    f = fopen("fog.dat", "r");
    // 252F
    fread(mem + 0x1D51, 1, 0x1100, f);
    // 253F
    fclose(f);

    // 2548
    f = fopen("smoke.dat", "r");
    // 2554
    fread(mem + 0x2E51, 1, 0x1000, f);
    // 2558
    fclose(f);
}

void Terep2Main()
{
    printf("Terep2 Native Test\n");
    chdir("data");
    MapStaticMemory();

    // Somewhere in init (find this)
    MOV_byte(0x5B65, 0x01);
    MOV_byte(0x5B69, 0x17);

    // 00DC
    MOV_word(0xEC50, 0x78);
    MOV_dword(0x006A, 0x1800);
    MOV_word(0xE9E2, 0x800);
    MOV_word(0xE9E4, 0xF000);
    MOV_word(0xDBC0, 0x0);
    MOV_word(0xDBB8, 0xA0);
    MOV_word(0xDBC2, 0x13F);
    MOV_word(0xDBBC, 0x0);
    MOV_word(0xDBBA, 0x50);
    MOV_word(0xDBBE, 0xC7);

    // 012A
    // If sim.cfg exists load it to 0xE9E2
    if (false) {
        FILE* f = fopen("sim.cfg", "r");
        fread(mem + 0xE9E2, 1, 4, f);
        fclose(f);
    }

    // 014D > Allocate 64KB of ram and store pointer to that in 0x1A45
    // 015D > Allocate 64KB of ram and store pointer to that in 0x1A47
    // 016D > Allocate 64KB of ram and store pointer to that in 0x1A49 (textures.pcx)
    // 017B > Allocate 64KB of ram and store pointer to that in 0x1A4B
    // We just spoof that for now
    char temp[8] = {0x63, 0x1D, 0x64, 0x2D, 0x65, 0x3D, 0x66, 0x4D};
    memcpy(mem + 0x1A45, temp, 8);

    // 017E
    // LoadMapAndOtherFiles();
    //

    char* CAR_DAT_START_IN_MEM = mem + 0x5BD0;

    FILE* f = fopen(mem + 0x5B01, "r");
    fread(CAR_DAT_START_IN_MEM, 1, 0x2710, f);
    fclose(f);

    // 25C5 -- No fucking idea what this does
    MOV_word(0x5AC1, 0x80);
    MOV_word(0x5AC3, 0x0);
    MOV_word(0x5AC7, 0x0);
    MOV_word(0x5AC9, 0x80);

    // Processs each car point
    uint16_t CX, SI;
    uint32_t EAX = 0x80000000;
    uint32_t EBX = 0x7A000000;
    uint32_t EDX = 0x03640000;

    memcpy(&CX, mem + 0x5C54, sizeof(CX));
    SI = 0x5C56; // Car point data start

    for (size_t i = 0; i < CX; i++)
    {
        *(uint32_t*)(mem+SI) = *(uint32_t*)(mem+SI) + EAX;
        *(uint32_t*)(mem+SI+4) = *(uint32_t*)(mem+SI+4) + EBX;
        *(uint32_t*)(mem+SI+8) = *(uint32_t*)(mem+SI+8) + EDX;
        SI += 0x1C;
    }

    // 2431 FUNC - locate camera point




    DumpMemory();
}
