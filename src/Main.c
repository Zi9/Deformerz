#include "Engine/Engine.h"
#include "Terep2Native/Terep2.h"

/*
int main()
{
    Terep2Main();
    return 0;
}
*/

int main()
{
    Config cfg = Config_Default();
    do {
        cfg = Engine_Main(cfg);
    } while (cfg.restart == true);
    return 0;
}