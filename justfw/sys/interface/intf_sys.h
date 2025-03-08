#ifndef __INTF_SYS_H
#define __INTF_SYS_H

/* FreeRTOS必须在前面 */
#include "FreeRTOS.h"

/* bsp */
#include "intf_can.h"

/* 内置数据共享 */
// #include "intf_tinybus.h"
#include "stream_intf.h"

/* 第三方库接口, JUSTFW没有继续封装 */
#include "easyflash.h"
#include "task.h"

#define JUST_MALLOC pvPortMalloc
#define JUST_FREE vPortFree

#endif