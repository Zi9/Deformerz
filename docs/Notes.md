


# Physlinks

Normal -> Min = 50% Max = 150%
Front -> Min = 45% Max = 100%
Rear2 -> Min = 48% Max = 100%
Susp_extra -> Min = 50% Max = 105%


# Terep1 parsing code snippets

```c
static void load_dat_chunk2_terep1(TerepCar* car, uint8_t* chunkStart)
{
    car->physLinkCount = U16(chunkStart, 0);
    uint8_t* p = chunkStart + 2;
    for (size_t i = 0; i < car->physLinkCount; i++) {
        car->physLinks[i].pointA = U16(p, 0);
        car->physLinks[i].pointB = U16(p, 2);
        car->physLinks[i].type = TEREP_SEGMENT_NORMAL;
        if (car->physLinks[i].type != 0 && car->physLinks[i].type != 1 && car->physLinks[i].type != 4 &&
            car->physLinks[i].type != 6 && car->physLinks[i].type != 10 && car->physLinks[i].type != 12) {
            printf("Unknown type segment: %i\n", car->physLinks[i].type);
            return;
        }
        p += 12;
    }
    printf("INFO: CARLOAD: Loaded %d physics segments\n", car->physLinkCount);
}
```
