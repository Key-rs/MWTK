#include "infantry.h"

#include <stdio.h>

#include "interface.h"

#define USE_ROCKER

#define CHASSIS_SPEED_X_MAX 0.5f
#define CHASSIS_SPEED_Y_MAX 0.5f
#define CHASSIS_SPEED_W_MAX 0.5f

static RC_ctrl_t *rc;
static INTF_Chassis_HandleTypeDef *chassis;
static INTF_Gimbal_HandleTypeDef *gimbal;

static void loop() {
    uint32_t lask_dwt = DWT_GetCNT();
    uint16_t last_sn = 0;
    while (true) {
        float dt = DWT_GetDeltaT(&lask_dwt);
        if (last_sn != rc[0].sn) {
            float vy = rc[0].rc.rocker_l1 / 660.0f * CHASSIS_SPEED_X_MAX;
            float vx = rc[0].rc.rocker_l_ / 660.0f * CHASSIS_SPEED_Y_MAX;
            // chassis->target_speed_y = vy * sinf(gimbal->real_yaw) - vx * cosf(gimbal->real_yaw);
            // chassis->target_speed_x = vy * cosf(gimbal->real_yaw) + vx * sinf(gimbal->real_yaw);

            chassis->set_speed(chassis, vy * cosf(gimbal->real_yaw) + vx * sinf(gimbal->real_yaw), vy * sinf(gimbal->real_yaw) - vx * cosf(gimbal->real_yaw), 0);

            // gimbal->target_yaw += rc[0].rc.rocker_l_ / 660.0f * 1.0f * dt;
            // gimbal->target_pitch += rc[0].rc.rocker_l1 / 660.0f * 5.0 * dt;
            // printf("RC:%d,%d,%d,%d\n", rc[0].rc.rocker_l1, rc[0].rc.rocker_l_, rc[0].rc.rocker_r1, rc[0].rc.rocker_r_);
        }

        last_sn = rc[0].sn;
        vTaskDelay(1);
    }
}

#include "justfw_cfg.h"
static void Custom_Controller_Init() {
#ifdef USE_BOARD_D
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

#ifdef USE_BOARD_C
    extern UART_HandleTypeDef huart3;

    HAL_UART_DeInit(&huart3);
    huart3.Instance = USART3;
    huart3.Init.BaudRate = 115200;
    huart3.Init.WordLength = UART_WORDLENGTH_8B;
    huart3.Init.StopBits = UART_STOPBITS_1;
    huart3.Init.Parity = UART_PARITY_NONE;
    huart3.Init.Mode = UART_MODE_TX_RX;
    huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart3.Init.OverSampling = UART_OVERSAMPLING_16;

    HAL_UART_Init(&huart3);

#endif
}

void Infantry_Init() {
    Custom_Controller_Init();
    Infantry_Chassis_Init();
    Infantry_Gimbal_Init();

    rc = pvSharePtr("DR16", sizeof(RC_ctrl_t));
    chassis = pvSharePtr("Chassis", sizeof(INTF_Chassis_HandleTypeDef));
    gimbal = pvSharePtr("Gimbal", sizeof(INTF_Gimbal_HandleTypeDef));

    xTaskCreate(loop, "InfantryMain", 1024, NULL, 240, NULL);
}