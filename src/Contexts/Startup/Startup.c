#include "Startup.h"
#include "Engine/Engine.h"
#include "Contexts/PhysicsSandbox/PhysicsSandbox.h"

static void _Run() {
    Engine_SwitchContext(PhysicsSandboxContext()); // NOTE: This is the initial context picker
}

EngineContext StartupContext() { return (EngineContext){.Run = _Run}; }
