//
// Created by Ukua on 2023/11/2.
//

#include "DR16.h"
// 遥控器数据
static RC_ctrl_t *rc_ctrl;

// DaemonInstance *g_rc_daemon_instance;

static BusSubscriberHandle_t g_dr16_rx;

static BusTopicHandle_t g_dr16_signal_disconnected,  // 遥控器失联
    g_dr16_signal_connected,                         // 遥控器连接
    g_dr16_signal_updated;                           // 遥控器数据更新

uint8_t g_dr16_is_connected = 0;
uint32_t g_last_solve_tick = 0;

/**
 * @brief 矫正遥控器摇杆的值,超过660或者小于-660的值都认为是无效值,置0
 *
 */
static void RectifyRCjoystick() {
    for (uint8_t i = 0; i < 5; ++i) {
        if (abs(*(&rc_ctrl[TEMP].rc.rocker_l_ + i)) > 660)
            *(&rc_ctrl[TEMP].rc.rocker_l_ + i) = 0;
    }
}

/**
 * @brief          remote control protocol resolution
 * @param[in]      sbus_buf: raw data point
 * @param[out]     rc_ctrl: remote control data struct point
 * @retval         none
 */

void DR16_solve(const uint8_t *sbus_buf) {
    // 遥控器连接signal
    if (!g_dr16_is_connected) {
        g_dr16_is_connected = 1;
        vBusPublish(g_dr16_signal_connected, NULL);
    }

    // 记录更新时间
    rc_ctrl[TEMP].update_time = HAL_GetTick();

    // 摇杆,直接解算时减去偏置
    rc_ctrl[TEMP].rc.rocker_r_ = ((sbus_buf[0] | (sbus_buf[1] << 8)) & 0x07ff) - RC_CH_VALUE_OFFSET;                               //!< Channel 0
    rc_ctrl[TEMP].rc.rocker_r1 = (((sbus_buf[1] >> 3) | (sbus_buf[2] << 5)) & 0x07ff) - RC_CH_VALUE_OFFSET;                        //!< Channel 1
    rc_ctrl[TEMP].rc.rocker_l_ = (((sbus_buf[2] >> 6) | (sbus_buf[3] << 2) | (sbus_buf[4] << 10)) & 0x07ff) - RC_CH_VALUE_OFFSET;  //!< Channel 2
    rc_ctrl[TEMP].rc.rocker_l1 = (((sbus_buf[4] >> 1) | (sbus_buf[5] << 7)) & 0x07ff) - RC_CH_VALUE_OFFSET;                        //!< Channel 3
    rc_ctrl[TEMP].rc.dial = ((sbus_buf[16] | (sbus_buf[17] << 8)) & 0x07FF) - RC_CH_VALUE_OFFSET;                                  // 左侧拨轮
    RectifyRCjoystick();
    // 开关,0左1右
    rc_ctrl[TEMP].rc.switch_right = ((sbus_buf[5] >> 4) & 0x0003);      //!< Switch right
    rc_ctrl[TEMP].rc.switch_left = ((sbus_buf[5] >> 4) & 0x000C) >> 2;  //!< Switch left

    // 鼠标解析
    rc_ctrl[TEMP].mouse.x = sbus_buf[6] | (sbus_buf[7] << 8);    //!< Mouse X axis
    rc_ctrl[TEMP].mouse.y = sbus_buf[8] | (sbus_buf[9] << 8);    //!< Mouse Y axis
    rc_ctrl[TEMP].mouse.z = sbus_buf[10] | (sbus_buf[11] << 8);  //!< Mouse Z axis
    rc_ctrl[TEMP].mouse.press_l = sbus_buf[12];                  //!< Mouse Left Is Press ?
    rc_ctrl[TEMP].mouse.press_r = sbus_buf[13];                  //!< Mouse Right Is Press ?

    //  位域的按键值解算,直接memcpy即可,注意小端低字节在前,即lsb在第一位,msb在最后. 尚未测试
    *(uint16_t *)&rc_ctrl[TEMP].keyboard = (uint16_t)(sbus_buf[14] | (sbus_buf[15] << 8));

    // 发布遥控器数据更新signal
    vBusPublish(g_dr16_signal_updated, NULL);

    // DaemonReload(g_rc_daemon_instance);  // 重载守护进程(检查遥控器是否正常工作
    rc_ctrl[TEMP].sn += 1;

    if (rc_ctrl[TEMP].sn % 0x0F == 0) {
#ifdef USE_BOARD_C
        HAL_GPIO_TogglePin(GPIOH, GPIO_PIN_11);
#endif

#ifdef USE_BOARD_D
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_14);
#endif
    }
}

void DR16_RX_CallBack(void *message, BusSubscriberHandle_t subscriber) {
    INTF_Serial_MessageTypeDef *msg = (INTF_Serial_MessageTypeDef *)message;
    if (msg->len != 18) {
        return;  // 长度不对，丢包
    }
    DR16_solve(msg->data);
    g_last_solve_tick = HAL_GetTick();
}

void RCLostCallback(void *id) {
    if (g_dr16_is_connected) {
        g_dr16_is_connected = 0;
        vBusPublish(g_dr16_signal_disconnected, NULL);
    }
}

#include <stdio.h>

#include "FreeRTOS_CLI.h"
#include "console_cfg.h"
#include "console_colors.h"

static BaseType_t prvDR16Command(char *pcWriteBuffer,
                                 size_t xWriteBufferLen,
                                 const char *pcCommandString) {
    BaseType_t xParameterStringLength, xReturn;

    const char *param = FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParameterStringLength);
    if (strncmp(param, "info", 4) == 0) {
        snprintf(pcWriteBuffer, xWriteBufferLen, "DR16:%d,%d,%d,%d\n",
                 rc_ctrl[0].rc.rocker_l1,
                 rc_ctrl[0].rc.rocker_l_,
                 rc_ctrl[0].rc.rocker_r1,
                 rc_ctrl[0].rc.rocker_l_);
    } else if (strncmp(param, "monitor", 7) == 0) {
        static StreamHandle_t console_input;
        if (console_input == NULL) {
            console_input = pvSreachSharedPtr(CLI_INPUT_STREAM);
        }

        if (console_input == NULL) {
            snprintf(pcWriteBuffer, xWriteBufferLen, RED "Stream Error!\n\r" NC);

            return pdFALSE;
        }

        while (true) {
            char input;
            if (xStreamRead(console_input, &input, 1, 0) && input == 0x03) {
                return pdFALSE;
            }

            printf("DR16:%d,%d,%d,%d\n",
                   rc_ctrl[0].rc.rocker_l1,
                   rc_ctrl[0].rc.rocker_l_,
                   rc_ctrl[0].rc.rocker_r1,
                   rc_ctrl[0].rc.rocker_r_);

            vTaskDelay(pdMS_TO_TICKS(20));
        }
    } else
        snprintf(pcWriteBuffer, xWriteBufferLen, RED "Err USE!\n\r" NC);

    return pdFALSE;
}

static const CLI_Command_Definition_t xDR16Command = {
    "dr16",
    "\n\n",
    prvDR16Command,
    1};

void DR16_Init() {
    FreeRTOS_CLIRegisterCommand(&xDR16Command);
    rc_ctrl = pvSharePtr("DR16", sizeof(RC_ctrl_t));
    // g_dr16_rx = xBusSubscribeFromName("/DBUS/RX", DR16_RX_CallBack);  // 串口遥控器
    g_dr16_rx = xBusSubscribeFromName("/UART/BLE_RX", DR16_RX_CallBack);  // 蓝牙遥控器

    // 遥控器事件
    g_dr16_signal_disconnected = xBusTopicRegister("/signal/DR16/disconnected");
    g_dr16_signal_connected = xBusTopicRegister("/signal/DR16/connected");
    g_dr16_signal_updated = xBusTopicRegister("/signal/DR16/updated");
}
