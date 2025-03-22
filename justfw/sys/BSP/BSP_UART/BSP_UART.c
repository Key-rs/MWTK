#include "BSP_UART.h"

#include "intf_sys.h"
#include "justfw_cfg.h"
#include "stm32f4xx_hal_uart.h"

static uint8_t DR16_buffer[18];

#ifdef USE_BOARD_C

extern UART_HandleTypeDef huart3;  // DR16

void BSP_UART_Init() {
    HAL_UARTEx_ReceiveToIdle_DMA(&huart3, DR16_buffer, 36);
    __HAL_UART_DISABLE_IT(&huart3, DMA_IT_HT);
    // ....
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    if (huart == &huart3) {
        HAL_UARTEx_ReceiveToIdle_DMA(&huart3, DR16_buffer, 36);
        __HAL_UART_DISABLE_IT(&huart3, DMA_IT_HT);
    }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
    if (huart == &huart3) {
        extern void DR16_solve(const uint8_t *sbus_buf);
        DR16_solve(DR16_buffer);

        HAL_UARTEx_ReceiveToIdle_DMA(&huart3, DR16_buffer, 36);
        __HAL_UART_DISABLE_IT(&huart3, DMA_IT_HT);
    }
}

#endif
