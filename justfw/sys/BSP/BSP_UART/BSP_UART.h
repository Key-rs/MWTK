//
// Created by Ukua on 2023/11/2.
//

#ifndef JUSTFW_BSP_UART_H
#define JUSTFW_BSP_UART_H

#include <stdint.h>

#include "intf_sys.h"
#include "main.h"
#include "stdlib.h"
#include "tinybus.h"

#define DEVICE_UART_CNT 4      // D板支持4个串口
#define UART_RXBUFF_LIMIT 256  // 如果协议需要更大的buff,请修改这里

typedef struct UART_InstanceConfig {
    UART_HandleTypeDef *UART_handle;
    uint16_t recv_buff_size;
    uint16_t tx_buff_size;
    char *tx_topic_name;
    char *rx_topic_name;
    char *tx_buffer_name;
    char *rx_buffer_name;
} UART_InstanceConfigTypeDef;

// 串口实例结构体,每个module都要包含一个实例.
// 由于串口是独占的点对点通信,所以不需要考虑多个module同时使用一个串口的情况,因此不用加入id;当然也可以选择加入,这样在bsp层可以访问到module的其他信息
typedef struct UART_Instance {
    uint8_t recv_buff[UART_RXBUFF_LIMIT];  // 预先定义的最大buff大小,如果太小请修改UART_RXBUFF_LIMIT
    uint8_t recv_buff_size;                // 模块接收一包数据的大小
    UART_HandleTypeDef *uart_handle;       // 实例对应的uart_handle
    Bus_SubscriberTypeDef *tx_topic;
    Bus_TopicHandleTypeDef *rx_topic;
    INTF_StreamSharerTypedef *rx_buffer;
    StreamBufferHandle_t tx_buffer;
    INTF_StreamListenerTypedef *rx_listener;  // 用于消息推流
    INTF_StreamListenerTypedef *tx_listener;  // 用于消息发送
} UART_InstanceTypeDef;

void BSP_UART_Init();

#endif  // JUSTFW_BSP_UART_H
