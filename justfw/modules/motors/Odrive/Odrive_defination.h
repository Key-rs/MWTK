#ifndef __ODRIVE_DEFINATION_H
#define __ODRIVE_DEFINATION_H
#ifndef ODRIVE_ERROR_CODES_H
#define ODRIVE_ERROR_CODES_H
#include <stdint.h>

#include "interface.h"

typedef enum {
    ODRIVE_AXIS_ERROR_UNDEFINED,                                   // 无异常
    ODRIVE_AXIS_ERROR_PHASE_RESISTANCE_OUT_OF_RANGE,               // 相间电阻超出正常范围
    ODRIVE_AXIS_ERROR_PHASE_INDUCTANCE_OUT_OF_RANGE,               // 相间电感超出正常范围
    ODRIVE_AXIS_ERROR_CONTROL_DEADLINE_MISSED = 0x10,              // FOC频率太高
    ODRIVE_AXIS_ERROR_MODULATION_MAGNITUDE = 0x80,                 // SVM调制异常
    ODRIVE_AXIS_ERROR_CURRENT_SENSE_SATURATION = 0x400,            // 相电流饱和
    ODRIVE_AXIS_ERROR_CURRENT_LIMIT_VIOLATION = 0x1000,            // 电机电流过大
    ODRIVE_AXIS_ERROR_MOTOR_THERMISTOR_OVER_TEMP = 0x20000,        // 电机温度过高
    ODRIVE_AXIS_ERROR_FET_THERMISTOR_OVER_TEMP = 0x40000,          // 驱动器温度过高
    ODRIVE_AXIS_ERROR_TIMER_UPDATE_MISSED = 0x80000,               // FOC处理不及时
    ODRIVE_AXIS_ERROR_CURRENT_MEASUREMENT_UNAVAILABLE = 0x100000,  // 相电流采样丢失
    ODRIVE_AXIS_ERROR_CONTROLLER_FAILED_AGAIN = 0x200000,          // 控制异常
    ODRIVE_AXIS_ERROR_I_BUS_OUT_OF_RANGE = 0x00400000,             // 母线电流超限
} ODRIVE_AXIS_ERROR;

typedef enum {
    ODRIVE_AXIS_STATUS_UNDEFINED = 0,
    ODRIVE_AXIS_STATUS_IDLE = 1,
    ODRIVE_AXIS_STATUS_STARTUP_SEQUENCE = 2,
    ODRIVE_AXIS_STATUS_FULL_CALIBRATION_SEQUENCE = 3,
    ODRIVE_AXIS_STATUS_MOTOR_CALIBRATION = 4,
    ODRIVE_AXIS_STATUS_ENCODER_INDEX_SEARCH = 6,
    ODRIVE_AXIS_STATUS_ENCODER_OFFSET_CALIBRATION = 7,
    ODRIVE_AXIS_STATUS_CLOSED_LOOP_CONTROL = 8,
    ODRIVE_AXIS_STATUS_LOCKIN_SPIN = 9,
    ODRIVE_AXIS_STATUS_ENCODER_DIR_FIND = 10,
    ODRIVE_AXIS_STATUS_HOMING = 11,
    ODRIVE_AXIS_STATUS_ENCODER_HALL_POLARITY_CALIBRATION = 12,
    ODRIVE_AXIS_STATUS_ENCODER_HALL_PHASE_CALIBRATION = 13,
    ODRIVE_AXIS_STATUS_ANTICOGGING_CALIBRATION = 14,
} ODRIVE_AXIS_STATUS;

typedef struct {
    uint8_t motor_id;
    char* motor_name;
    float angle_offset;
    float direction;
    char* can_rx_topic_name;
    char* can_tx_topic_name;
    float kp;  // 运控模式kp
    float kd;  // 运控模式kd
} Odrive_CAN_ConfigTypedef;

typedef struct
{
    BusTopicHandle_t can_tx_topic;
    BusSubscriberHandle_t can_rx_topic;  // CAN_RX
    ODRIVE_AXIS_ERROR axis_error;
    ODRIVE_AXIS_STATUS axis_status;
    bool motor_error;
    bool encoder_error;
    bool controller_error;
    bool error;
    bool trajectory_done;  // 位置是否执行完毕
    uint8_t life;
    float kp;
    float kd;

    // uint32_t last_update_tick;
} Odrive_CAN_ResDataTypedef;

#endif  // ODRIVE_ERROR_CODES_H

#endif