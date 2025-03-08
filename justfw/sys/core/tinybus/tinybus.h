#ifndef TINYBUS_H
#define TINYBUS_H

#ifdef __cplusplus
extern "C" {
#endif

/* FreeRTOS 头文件 */
#include "FreeRTOS.h"
/* 接口文件 */
#include "intf_sys.h"
#include "tinybus_intf.h"

/**
 * @brief 初始化 TinyBus
 * @return pdPASS: 成功, pdFAIL: 失败
 */
BaseType_t xBusInit(void);

#ifdef __cplusplus
}
#endif

#endif /* TINYBUS_H */