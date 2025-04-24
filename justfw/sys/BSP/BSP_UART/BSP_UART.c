//
// Created by Ukua on 2023/11/2.
//
#include "BSP_UART.h"

#include "BSP_UART_cfg.h"
#include "intf_sys.h"
#include "justfw_cfg.h"

/*
应用 -> TX 缓冲区(数据流) -> 串口发送
            ^
            |
应用 -> TX 消息总线


            RX 消息总线 -> 应用
                ^
                |
串口接收 -> RX 共享数据流 -> 应用
*/

/* UART service instance, modules' info would be recoreded here using UARTRegister() */
/* UART服务实例,所有注册了UART的模块信息会被保存在这里 */
static uint8_t idx;
static UART_InstanceTypeDef *UART_instance[DEVICE_UART_CNT] = {NULL};

// TX Bus总线 -> TX发送数据缓冲区
void UART_Bus_TX_CallBack(void *message, BusSubscriberHandle_t subscriber) {
    INTF_Serial_MessageTypeDef *msg = (INTF_Serial_MessageTypeDef *)message;
    UART_InstanceTypeDef *instance = (UART_InstanceTypeDef *)subscriber->context;
    if (instance->tx_topic->pxTopic == subscriber->pxTopic) {
        xStreamBufferSend(instance->tx_buffer, msg->data, msg->len, portMAX_DELAY);
    }
}

// RX 数据流 -> Bus RX总线
void UART_Bus_Rx_CallBack(StreamListenerHandle_t listener) {
    UART_InstanceTypeDef *instance = (UART_InstanceTypeDef *)listener->pvContext;
    uint8_t data[BSP_UART_TRANSFORM_MAX_LEN];
    uint16_t len = xStreamRead(listener->xStream, data, BSP_UART_TRANSFORM_MAX_LEN, 0);
    INTF_Serial_MessageTypeDef msg = {
        .data = data,
        .len = len};

    vBusPublish(instance->rx_topic, &msg);
}

// TX 数据流 -> 串口发送
void UART_TX_CallBack(StreamListenerHandle_t listener) {
    UART_InstanceTypeDef *instance = (UART_InstanceTypeDef *)listener->pvContext;
    uint8_t data[BSP_UART_TRANSFORM_MAX_LEN];
    uint16_t len = xStreamRead(listener->xStream, data, BSP_UART_TRANSFORM_MAX_LEN, 0);

    HAL_UART_Transmit(instance->uart_handle, data, len, BSP_UART_TIMEOUT);
}

/**
 * @brief 启动串口服务,会在每个实例注册之后自动启用接收,当前实现为DMA接收,后续可能添加IT和BLOCKING接收
 *
 * @todo 串口服务会在每个实例注册之后自动启用接收,当前实现为DMA接收,后续可能添加IT和BLOCKING接收
 *       可能还要将此函数修改为extern,使得module可以控制串口的启停
 *
 * @param _instance instance owned by module,模块拥有的串口实例
 */
void BSP_UART_Instance_Init(UART_InstanceTypeDef *_instance) {
    HAL_UARTEx_ReceiveToIdle_DMA(_instance->uart_handle, _instance->recv_buff, _instance->recv_buff_size);
    // 关闭dma half transfer中断防止两次进入HAL_UARTEx_RxEventCallback()
    // 这是HAL库的一个设计失误,发生DMA传输完成/半完成以及串口IDLE中断都会触发HAL_UARTEx_RxEventCallback()
    // 我们只希望处理第一种和第三种情况,因此直接关闭DMA半传输中断
    __HAL_DMA_DISABLE_IT(_instance->uart_handle->hdmarx, DMA_IT_HT);
}

UART_InstanceTypeDef *BSP_UART_Register(UART_InstanceConfigTypeDef *config) {
    UART_InstanceTypeDef *instance = (UART_InstanceTypeDef *)JUST_MALLOC(sizeof(UART_InstanceTypeDef));
    memset(instance, 0, sizeof(UART_InstanceTypeDef));

    instance->uart_handle = config->UART_handle;
    instance->recv_buff_size = config->recv_buff_size;

    instance->rx_buffer = xSharedStreamCreate(instance->recv_buff_size);
    vSharePtrStatic(config->rx_buffer_name, instance->rx_buffer);  // 共享静态共享缓冲区

    if (config->tx_buff_size > 0) {
        instance->tx_buffer = xStreamCreate(config->tx_buff_size);
        vSharePtrStatic(config->tx_buffer_name, instance->tx_buffer);

        instance->tx_listener = xStreamListenerCreate(instance->tx_buffer);
        instance->tx_listener->pvOnDataReceived = UART_TX_CallBack;
        instance->tx_listener->pvContext = instance;
    }

    if (config->tx_topic_name != NULL) {
        // 启用TX Bus消息监听
        instance->tx_topic = xBusSubscribeFromName(config->tx_topic_name, UART_Bus_TX_CallBack);
        instance->tx_topic->context = (void *)instance;
    }

    if (config->rx_topic_name != NULL) {
        // 启用RX Bus消息推送
        instance->rx_topic = xBusTopicRegister(config->rx_topic_name);
        // StreamBufferHandle_t rx_bus_stream = xStreamBufferCreate(config->recv_buff_size, 1);

        // instance->rx_buffer->register_output(instance->rx_buffer, rx_bus_stream);
        StreamBufferHandle_t rx_bus_stream = xSharedStreamOutputCreate(instance->rx_buffer, config->recv_buff_size);  // 注册一个用于截取RX消息的流，并且在截取后向Bus总线推送
        instance->rx_listener = xStreamListenerCreate(rx_bus_stream);                                                 // 监听截取的流
        instance->rx_listener->pvOnDataReceived = UART_Bus_Rx_CallBack;
        instance->rx_listener->pvContext = instance;
    }

    UART_instance[idx++] = instance;
    BSP_UART_Instance_Init(instance);
    return instance;
}

/* 串口发送时,gstate会被设为BUSY_TX */
uint8_t UART_IsReady(UART_InstanceTypeDef *_instance) {
    if (_instance->uart_handle->gState == HAL_UART_STATE_READY) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * @brief 每次dma/idle中断发生时，都会调用此函数.对于每个uart实例会调用对应的回调进行进一步的处理
 *        例如:视觉协议解析/遥控器解析/裁判系统解析
 *
 * @note  通过__HAL_DMA_DISABLE_IT(huart->hdmarx,DMA_IT_HT)关闭dma half transfer中断防止两次进入HAL_UARTEx_RxEventCallback()
 *        这是HAL库的一个设计失误,发生DMA传输完成/半完成以及串口IDLE中断都会触发HAL_UARTEx_RxEventCallback()
 *        我们只希望处理，因此直接关闭DMA半传输中断第一种和第三种情况
 *
 * @param huart 发生中断的串口
 * @param Size 此次接收到的总数居量,暂时没用
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
    for (uint8_t i = 0; i < idx; ++i) {
        if (huart == UART_instance[i]->uart_handle) {
            // INTF_Serial_MessageTypeDef message;
            UART_InstanceTypeDef *instance = UART_instance[i];
            // instance->rx_buffer->write(instance->rx_buffer, instance->recv_buff, Size);
            xSharedStreamWrite(instance->rx_buffer, instance->recv_buff, Size, portMAX_DELAY);

            HAL_UARTEx_ReceiveToIdle_DMA(UART_instance[i]->uart_handle, UART_instance[i]->recv_buff,
                                         UART_instance[i]->recv_buff_size);
            __HAL_DMA_DISABLE_IT(UART_instance[i]->uart_handle->hdmarx, DMA_IT_HT);
            return;
        }
    }
}

/**
 * @brief 当串口发送/接收出现错误时,会调用此函数,此时这个函数要做的就是重新启动接收
 *
 * @note  最常见的错误:奇偶校验/溢出/帧错误
 *
 * @param huart 发生错误的串口
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    for (uint8_t i = 0; i < idx; ++i) {
        if (huart == UART_instance[i]->uart_handle) {
            HAL_UARTEx_ReceiveToIdle_DMA(UART_instance[i]->uart_handle, UART_instance[i]->recv_buff,
                                         UART_instance[i]->recv_buff_size);

            __HAL_DMA_DISABLE_IT(UART_instance[i]->uart_handle->hdmarx, DMA_IT_HT);
            return;
        }
    }
}

void BSP_UART_Init() {
#ifdef USE_BOARD_D

    extern UART_HandleTypeDef huart1;
    UART_InstanceConfigTypeDef uart1_config = {
        .UART_handle = &huart1,
        .recv_buff_size = 50,
        .tx_buff_size = 255,

        .tx_buffer_name = "/stream/ble_tx",
        .rx_buffer_name = "/stream/ble_rx",

        // .tx_topic_name = "/UART/BLE_TX",
        .rx_topic_name = "/UART/BLE_RX"};
    BSP_UART_Register(&uart1_config);

    extern UART_HandleTypeDef huart2;

    UART_InstanceConfigTypeDef uart2_config = {
        .UART_handle = &huart2,
        .recv_buff_size = 50,
        .tx_buff_size = 255,
        .tx_buffer_name = "/stream/ble_tx",
        .rx_buffer_name = "/stream/ble_rx",
        // .tx_topic_name = "/UART_TEST1_TX",
        // .rx_topic_name = "/UART_TEST2_RX"
    };
    BSP_UART_Register(&uart2_config);

#include "DR16_config.h"
#ifdef BSP_UART_USE_CUSTOM_RC
    extern UART_HandleTypeDef huart4;

    HAL_UART_DeInit(&huart4);
    huart4.Instance = UART4;
    huart4.Init.BaudRate = 115200;
    huart4.Init.WordLength = UART_WORDLENGTH_8B;
    huart4.Init.StopBits = UART_STOPBITS_1;
    huart4.Init.Parity = UART_PARITY_NONE;
    huart4.Init.Mode = UART_MODE_TX_RX;
    huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart4.Init.OverSampling = UART_OVERSAMPLING_16;

    HAL_UART_Init(&huart4);
#endif

    extern UART_HandleTypeDef huart4;
    UART_InstanceConfigTypeDef uart4_config = {
        .UART_handle = &huart4,
        .recv_buff_size = 255,
        .tx_buff_size = 255,

        .tx_buffer_name = "/stream/dbus_tx",
        .rx_buffer_name = "/stream/dbus_rx",

        // .tx_topic_name = "/DBUS/TX",
        .rx_topic_name = "/DBUS/RX"};
    BSP_UART_Register(&uart4_config);

    // extern UART_HandleTypeDef huart5;
    // UART_InstanceConfigTypeDef uart5_config = {
    //     .UART_handle = &huart5,
    //     .recv_buff_size = 255,
    //     .tx_buff_size = 255,
    //
    //     .tx_buffer_name = "/stream/referee_tx",
    //     .rx_buffer_name = "/stream/referee_rx",
    //
    //     // .tx_topic_name = "/REFEREE/TX",
    //     // .rx_topic_name = "/REFEREE/RX"
    //
    // };
    // BSP_UART_Register(&uart5_config);
#endif

#ifdef USE_BOARD_C
    extern UART_HandleTypeDef huart1;
    UART_InstanceConfigTypeDef uart1_config = {
        .UART_handle = &huart1,
        .recv_buff_size = 50,
        .tx_topic_name = "/UART/TEST_TX",
        .rx_topic_name = "/UART/TEST_RX"};
    BSP_UART_Register(&uart1_config);

    extern UART_HandleTypeDef huart3;

#ifdef BSP_UART_USE_CUSTOM_RC
    extern UART_HandleTypeDef huart3;

    HAL_UART_DeInit(&huart3);
    huart3.Instance = UART3;
    huart3.Init.BaudRate = 115200;
    huart3.Init.WordLength = UART_WORDLENGTH_8B;
    huart3.Init.StopBits = UART_STOPBITS_1;
    huart3.Init.Parity = UART_PARITY_NONE;
    huart3.Init.Mode = UART_MODE_TX_RX;
    huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart3.Init.OverSampling = UART_OVERSAMPLING_16;

    HAL_UART_Init(&huart3);
#endif

    UART_InstanceConfigTypeDef uart3_config = {
        .UART_handle = &huart3,
        .recv_buff_size = 50,
        // .tx_topic_name = "/DBUS/TX",
        .rx_topic_name = "/DBUS/RX"};
    BSP_UART_Register(&uart3_config);

    extern UART_HandleTypeDef huart6;
    UART_InstanceConfigTypeDef uart6_config = {
        .UART_handle = &huart6,
        .recv_buff_size = 255,
        .tx_topic_name = "/REFEREE/TX",
        .rx_topic_name = "/REFEREE/RX"};
    BSP_UART_Register(&uart6_config);
#endif
}
