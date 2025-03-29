#ifndef __DM_MOTOR_H
#define __DM_MOTOR_H

#include "interface.h"

typedef enum {
    DM_ERROR_DISABLE = 0x00,
    DM_ERROR_ENABLEDM = 0x01,
    DM_ERROR_MCU_ERROR = 0x05,
    DM_ERROR_MOTOR_DATA_ERROR = 0x06,
    DM_ERROR_OVERVOLTAGE = 0x08,
    DM_ERROR_UNDERVOLTAGE = 0x09,
    DM_ERROR_OVERCURRENT = 0x0A,
    DM_ERROR_MOS_OVERTEMPERATURE = 0x0B,
    DM_ERROR_MOTOR_OVERTEMPERATURE = 0x0C,
    DM_ERROR_COMMUNICATION_LOST = 0x0D,
    DM_ERROR_OVERLOAD = 0x0E,
} DM_ERROR;

typedef struct DM_Motor_Config {
    uint32_t motor_id;
    char *motor_ptr_name;  // 共享指针名
    float angle_offset;
    float direction;                    // 电机方向（电机角度、输出乘以该系数，设置-1反向）
    INTF_Motor_ModeTypeDef motor_mode;  // 运行模式
    char *can_rx_topic_name;
    char *can_tx_topic_name;
    float kp;  // 运控模式kp
    float kd;  // 运控模式kd
} DM_Motor_ConfigTypeDef;

typedef struct DM_Motor_ResData {
    float angle_offset;
    BusTopicHandle_t can_tx_tp;
    BusSubscriberHandle_t can_rx_sb;
    float torque;
    float kp;  // MIT模式kp
    float kd;  // MIT模式kd
    DM_ERROR error;
    bool is_recieved;
    uint32_t last_tick;
} DM_Motor_ResDataTypeDef;

INTF_Motor_HandleTypeDef *DM_Motor_Register(DM_Motor_ConfigTypeDef *config);

#endif