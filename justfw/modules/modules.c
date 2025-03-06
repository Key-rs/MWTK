#include "modules.h"

#include "main.h"

void Modules_Init() {
    __disable_irq();
    SteadyWinMIT_Init();
    DR16_Init();
    motor_cli_init();

    __enable_irq();
}