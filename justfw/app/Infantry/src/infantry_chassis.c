#include "infantry_chasis.h"
#include "interface.h"

//
#include "C620.h"
#include "GM6020.h"

//
#include <stdio.h>

#include "FreeRTOS_CLI.h"
#include "console_cfg.h"
#include "console_colors.h"

// 轮子到底盘中心的距离 单位m
#define Chassis_Radius 0.222f

// 电机的减速比
#define Motor_DECELE_RATIO 19.203f  // 这是3508的

// 轮子半径 单位m
#define WHEEL_Radius 0.0763f

static INTF_Motor_HandleTypeDef* m_ow_fr;
static INTF_Motor_HandleTypeDef* m_ow_fl;
static INTF_Motor_HandleTypeDef* m_ow_br;
static INTF_Motor_HandleTypeDef* m_ow_bl;

static INTF_Chassis_HandleTypeDef* chassis_handle;

void Infantry_SetSpeed(float x, float y, float w) {
    chassis_handle->real_speed_x = x;
    chassis_handle->real_speed_y = y;
    chassis_handle->real_speed_w = w;

    m_ow_fr->set_speed(m_ow_fr, Motor_DECELE_RATIO * (x - y + w / WHEEL_Radius * Chassis_Radius));
    m_ow_fl->set_speed(m_ow_fl, Motor_DECELE_RATIO * (x + y + w / WHEEL_Radius * Chassis_Radius));
    m_ow_bl->set_speed(m_ow_bl, Motor_DECELE_RATIO * (-x + y + w / WHEEL_Radius * Chassis_Radius));
    m_ow_br->set_speed(m_ow_br, Motor_DECELE_RATIO * (-x - y + w / WHEEL_Radius * Chassis_Radius));
}

static void chassis_setspeed_t(INTF_Chassis_HandleTypeDef* self, float x, float y, float w) {
    if (self == chassis_handle) {
        Infantry_SetSpeed(x, y, w);
    }
}

extern float INS_angle[3];

static BaseType_t prvChassisManager(char* pcWriteBuffer,
                                    size_t xWriteBufferLen,
                                    const char* pcCommandString) {
    BaseType_t xParameterStringLength, xReturn;

    const char* param = FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParameterStringLength);

    if (strncmp(param, "info", 4) == 0) {
        snprintf(pcWriteBuffer, xWriteBufferLen, "Chassis:%f,%f,%f,%f,%f,%f,%f\n",
                 chassis_handle->target_speed_x,
                 chassis_handle->target_speed_y,
                 chassis_handle->target_speed_w,
                 m_ow_fr->target_speed,
                 m_ow_fl->target_speed,
                 m_ow_bl->target_speed,
                 m_ow_br->target_speed);
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

            printf("Chassis:%f,%f,%f,%f,%f,%f,%f\n",
                   chassis_handle->target_speed_x,
                   chassis_handle->target_speed_y,
                   chassis_handle->target_speed_w,
                   m_ow_fr->target_speed,
                   m_ow_fl->target_speed,
                   m_ow_bl->target_speed,
                   m_ow_br->target_speed);
            vTaskDelay(pdMS_TO_TICKS(20));
        }
    } else
        snprintf(pcWriteBuffer, xWriteBufferLen, RED "Err USE!\n\r" NC);

    return pdFALSE;
}

static const CLI_Command_Definition_t xChassisManager = {
    "chassis",
    "\n\n",
    prvChassisManager,
    1};

void Infantry_Chassis_Init() {
    PID_Init_Config_s angle_pid = {
        .Kp = 1.0f,
        .Ki = 0.0f,
        .Kd = 0.0f,
        .MaxOut = 1000.0f,
        .DeadBand = 0.0f,
        .Improve = PID_Integral_Limit,
    };
    PID_Init_Config_s speed_pid_F_R = {
        .Kp = 2.2f,
        .Ki = 0.01f,
        .Kd = 0.03f,
        .MaxOut = 6.0f,
        .DeadBand = 0.0f,
        .Output_LPF_RC = 0.1f,
        .Improve = PID_Integral_Limit | PID_OutputFilter,
        .IntegralLimit = 0.4f,
    };
    PID_Init_Config_s torque_pid = {
        .Kp = 1000.0f,
        .Ki = 5000.0f,
        .Kd = 0.0f,
        .MaxOut = C620_CURRENT_MAX,
        .DeadBand = 0.0f,
        .Improve = PID_Integral_Limit,
        .IntegralLimit = 500.0f,
    };
    C620_ConfigTypeDef config = {
        .motor_id = 1,
        .motor_ptr_name = "/motor/OW_F_R",
        .motor_mode = MOTOR_MODE_SPEED,
        .direction = -1.0f,
        .torque_feed_forward = C620_Torque2Current(1.0f),  // 已测试
        .angle_pid_config = &angle_pid,
        .speed_pid_config = &speed_pid_F_R,
        .torque_pid_config = &torque_pid,
        .can_rx_topic_name = "/CAN1/RX",
        .can_tx_topic_name = "/CAN1/TX",
        .gear_ratio = 15.2f,
    };
    m_ow_fl = C620_Register(&config);

    config.motor_id = 2;  // 3
    config.motor_ptr_name = "/motor/OW_F_L";

    m_ow_fr = C620_Register(&config);

    config.motor_id = 3;  // 4
    config.motor_ptr_name = "/motor/OW_B_L";
    m_ow_bl = C620_Register(&config);

    config.motor_id = 4;  // 2
    config.motor_ptr_name = "/motor/OW_B_R";
    m_ow_br = C620_Register(&config);

    chassis_handle = pvSharePtr("Chassis", sizeof(INTF_Chassis_HandleTypeDef));
    chassis_handle->set_speed = chassis_setspeed_t;

    FreeRTOS_CLIRegisterCommand(&xChassisManager);
}
