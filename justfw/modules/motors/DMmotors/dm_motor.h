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
    char *motor_ptr_name;
    float angle_offset;
    float direction;
    INTF_Motor_ModeTypeDef motor_mode;
    char *can_rx_topic_name;
    char *can_tx_topic_name;
    float kp;
    float kd;
} DM_Motor_ConfigTypeDef;

typedef struct DM_Motor_ResData {
    float angle_offset;
    BusTopicHandle_t can_tx_tp;
    BusSubscriberHandle_t can_rx_sb;
    float torque;
    float kp;
    float kd;
    DM_ERROR error;
    bool is_recieved;
    uint32_t last_tick;
} DM_Motor_ResDataTypeDef;

INTF_Motor_HandleTypeDef *DM_Motor_Register(DM_Motor_ConfigTypeDef *config);

#endif