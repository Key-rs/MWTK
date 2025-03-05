#include "modules.h"

#include "main.h"

void Modules_Init() {
    __disable_irq();
    SteadyWinMIT_Init();
    DR16_Init();

    __enable_irq();
}