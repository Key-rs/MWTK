#ifndef __SYS_H
#define __SYS_H

#include "BSP_CAN.h"
#include "BSP_UART.h"
#include "BSP_USB.h"
#include "BSP_W25Qx.h"
#include "CLI.h"
#include "bsp_delay.h"
#include "bsp_dwt.h"
#include "shared_ptr.h"
#include "storage.h"
#include "stream.h"
#include "tinybus.h"

void Sys_Init();

#endif