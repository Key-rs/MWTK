#ifndef __MOTOR_MANAGER_ITNF_H
#define __MOTOR_MANAGER_ITNF_H

#include "intf_motor.h"

typedef struct {
    float angle_kp;
    float angle_ki;
    float angle_kd;
    float velocity_kp;
    float velocity_kd;
    float velocity_Ki;
} PID_ControlParamsTypedef;

typedef struct
{
    float kp;
    float kd;
    float torque;
} MITControlParamTypedef;

typedef struct INTF_Managed_MotorHandle {
    char *motor_name;
    INTF_Motor_HandleTypeDef *motor;
    PID_ControlParamsTypedef pid_parms;
    MITControlParamTypedef mit_parms;

    void (*ApplyControl)(struct INTF_Managed_MotorHandle *self);                         // 将控制参数同步到电机上
    void (*MotorInfo)(struct INTF_Managed_MotorHandle *self, char *buff, uint16_t len);  // 打印电机相关信息
} ITNF_ManagerdMotor_HandleTypedef;

#endif