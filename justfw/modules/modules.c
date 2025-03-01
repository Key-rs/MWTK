#include "modules.h"

#include "main.h"

void Modules_Init() {
    __disable_irq();

    __enable_irq();
}