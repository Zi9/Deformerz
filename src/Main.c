#include "Engine/Engine.h"

int main()
{
    Config cfg = Config_Default();
    do {
        cfg = Engine_Main(cfg);
    } while (cfg.restart == true);
    return 0;
}