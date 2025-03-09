#ifndef __MOTOR_MANAGER_H
#define __MOTOR_MANAGER_H

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
    INTF_Motor_Handle *m;
    PID_ControlParamsTypedef pid_parms;
    MITControlParamTypedef mit_parms;

    void (*UpdateContorl)(struct INTF_Managed_MotorHandle *self);  // 将控制参数同步到电机上
} ITNF_Managerd_MotorTypedef;

#endif