//
// Created by Konodoki on 2024/12/20.
//

#include "BSP_ist8310.h"

#include <stdio.h>

#include "gpio.h"
#include "interface.h"
#include "ist8310driver.h"
#include "justfw_cfg.h"
#include "main.h"

#ifdef USE_BOARD_C

// TaskHandle_t ist8310TaskHandle;
TaskHandle_t const ist8310TaskHandle;

ist8310_real_data_t ist8310_real_data;
void ist8310_DRDY_IQR(uint16_t GPIO_Pin) {
    if (GPIO_Pin != DRDY_IST8310_Pin)
        return;
    // ist8310数据准备好了
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        static BaseType_t xHigherPriorityTaskWoken;
        vTaskNotifyGiveFromISR(ist8310TaskHandle, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}
void ist8310_task() {
    HAL_GPIO_WritePin(RSTN_IST8310_GPIO_Port, RSTN_IST8310_Pin, 0);
    vTaskDelay(50);
    HAL_GPIO_WritePin(RSTN_IST8310_GPIO_Port, RSTN_IST8310_Pin, 1);
    vTaskDelay(50);
    ist8310_init();
    while (1) {
        // 等待SPI DMA传输
        while (ulTaskNotifyTake(pdTRUE, portMAX_DELAY) != pdPASS) {
        }
        ist8310_read_mag(ist8310_real_data.mag);
        //    char buf[50];
        //    sprintf(buf,"mag:%.4f,%.4f,%.4f\r\n",ist8310_real_data.mag[0],ist8310_real_data.mag[1],ist8310_real_data.mag[2]);
        //    elog_i("mag",buf);
        //    osDelay(1);
    }
}
void BSP_ist8310_Init() {
    // osThreadDef(ist8310task, ist8310_task, osPriorityRealtime, 0, 1024);
    // ist8310TaskHandle = osThreadCreate(osThread(ist8310task), NULL);

    xTaskCreate(ist8310_task, "IST8310", 256, NULL, 1, (TaskHandle_t* const)&ist8310TaskHandle);
}

#endif