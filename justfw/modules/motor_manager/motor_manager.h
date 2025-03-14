#ifndef __MOTOR_MANAGER_H
#define __MOTOR_MANAGER_H
#include "interface.h"

void MotorManager_Init();
ITNF_ManagerdMotor_HandleTypedef* ManagedMotor_Create(INTF_Motor_HandleTypeDef* motor);

#endif