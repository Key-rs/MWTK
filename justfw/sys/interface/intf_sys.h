#ifndef __INTF_SYS_H
#define __INTF_SYS_H

#include "intf_can.h"
#include "intf_sys.h"
#include "intf_tinybus.h"
#include "intf_uart.h"

/* 第三方库接口, JUSTFW不再继续封装 */
#include "FreeRTOS.h"
#include "easyflash.h"
#include "task.h"

#define JUST_MALLOC pvPortMalloc
#define JUST_FREE vPortFree

#endif