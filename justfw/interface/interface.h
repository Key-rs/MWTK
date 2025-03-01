//
// Created by Ukua on 2023/9/14.
//

#ifndef JUSTFW_INTERFACE_H
#define JUSTFW_INTERFACE_H

#include <stdarg.h>
#include <stdbool.h>

#include "intf_can.h"
#include "intf_chassis.h"
#include "intf_dr16.h"
#include "intf_efcom.h"
#include "intf_gimbal.h"
// #include "intf_leg.h"
#include "intf_log.h"
#include "intf_motor.h"
#include "intf_navigation.h"
#include "intf_shooter.h"
#include "intf_uart.h"
#include "intf_vision.h"
#include "stdlib.h"

// LIB
#include "PID.h"
#include "user_lib.h"
// #include "PID_classic.h"
#include "QuaternionEKF.h"
// #include "easyflash.h"
// #include "elog.h"
// #include "elog_flash.h"
#include "kalman.h"
#include "kalman_filter.h"
#include "tinybus.h"

// rtos
#include "FreeRTOS.h"
#include "task.h"

extern void USB_Printf(const char *fmt, ...);
extern void USB_VSNPrintf(const char *fmt, va_list args);

// 初始化结构体(全部置0)
#define STRUCT_INIT(type) memset(JUST_MALLOC(sizeof(type)), 0, sizeof(type))
// malloc

#ifdef CMSIS_OS_H_
#define JUST_MALLOC pvPortMalloc
#define JUST_FREE vPortFree
#else
#define JUST_MALLOC malloc
#define JUST_FREE free
#endif

#endif  // JUSTFW_INTERFACE_H
