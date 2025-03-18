#ifndef __CALI_MOTOR_H
#define __CALI_MOTOR_H
#include "interface.h"

typedef enum {
    CALIMOTOR_NO,   // 未校准
    CALIMOTOR_YES,  // 已校准
    CALIMOTOR_ERROR
} CALIMOTOR_STSTUS;

typedef struct CaliMotorDef {
    INTF_Motor_HandleTypeDef *handle;
    float cali_angle;         // 抵达校准位点的实际角度
    float offset_angle;       // 以抵达校准点的实际角度作为参考，到真正的零位角度
    float cali_speed;         // 校准过程中的速度
    CALIMOTOR_STSTUS status;  // 校准状态
    void *private;
    float _offset_angle;                                   // 电机实际的零位偏移角度
    void (*start_calibration)(struct CaliMotorDef *self);  // 开始校准
} CaliMotor_t;

typedef CaliMotor_t *CaliMotor_Handle_t;

CaliMotor_Handle_t C610_CaliMotor_Create(INTF_Motor_HandleTypeDef *handle, float offset_angle, float cali_speed);

void cali_motor_set_angle(CaliMotor_Handle_t self, float angle);
float cali_motor_get_anlge(CaliMotor_Handle_t self);
#endif