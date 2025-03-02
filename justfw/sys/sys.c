#include "sys.h"

#include <stdio.h>

#include "BSP_USB_cfg.h"
#include "intf_sys.h"
#include "justfw_cfg.h"
#include "main.h"

static StreamBufferHandle_t stream;

void Sys_Init() {
    __disable_irq();

    Bus_Init();
    Stream_Init();
    /*BSP*/
    BSP_USB_Init();
    DWT_Init(168);
    delay_init();

    CLI_Init();
    Storage_Init();

    // 避免printf() 无输出
    setvbuf(stdout, NULL, _IONBF, 0);

    __enable_irq();
}
