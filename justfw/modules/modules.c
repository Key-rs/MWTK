#include "modules.h"

void Modules_Init() {
    __disable_irq();

    Bus_Init();
    /*BSP*/
    BSP_USB_Init();
    DWT_Init(168);
    delay_init();

    CLI_Init();
    Storage_Init();

    __enable_irq();
}