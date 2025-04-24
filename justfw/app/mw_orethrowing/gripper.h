#ifndef JUSTFW_gripper_H
#define JUSTFW_gripper_H

#include "tinybus.h"
#include "interface.h"

#define servo_1 TIM_CHANNEL_1//大臂
#define servo_2 TIM_CHANNEL_2//小臂
#define servo_3 TIM_CHANNEL_3//手腕
#define servo_4 TIM_CHANNEL_4//爪子




#define MIN_ANGLE1 0    //边界角度
#define MAX_ANGLE1 270
#define MIN_ANGLE2 0
#define MAX_ANGLE2 270
#define MIN_ANGLE3 0
#define MAX_ANGLE3 270
#define MIN_ANGLE4 0
#define MAX_ANGLE4 70
#define STEP1 1.0f
#define STEP2 1.0f
#define STEP3 1.0f
#define STEP4 1.0f




void gripper_Init();
#endif //JUSTFW_rc2can_H
