#ifndef __GM_MOTORS_H
#define __GM_MOTORS_H

#include "C610.h"
#include "C620.h"
#include "GM6020.h"
#include "tinybus.h"

#define GM_BUFFER_NUM 2

typedef struct GM_Buffer {
    BusTopicHandle_t can_tx_topic;
    BusTopicHandle_t can_rx_topic;
    uint8_t buffer_0x200[8];
    uint8_t buffer_0x1FF[8];
    uint8_t buffer_0x2FF[8];
    bool buffer_0x200_not_null;
    bool buffer_0x1FF_not_null;
    bool buffer_0x2FF_not_null;
} GM_BufferTypeDef;

INTF_Motor_HandleTypeDef *C610_Register(C610_ConfigTypeDef *config);
INTF_Motor_HandleTypeDef *C620_Register(C620_ConfigTypeDef *config);
INTF_Motor_HandleTypeDef *GM6020_Register(GM6020_ConfigTypeDef *config);

#endif