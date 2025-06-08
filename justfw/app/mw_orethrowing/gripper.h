#ifndef JUSTFW_gripper_H
#define JUSTFW_gripper_H

#include "tinybus.h"
#include "interface.h"

#define servo_1 0//爪子张合
#define servo_2 1//爪手腕
#define servo_3 2//小臂
#define servo_4 3//大臂
#define servo_5 4//底座




#define MIN_ANGLE1 0    //边界角度
#define MAX_ANGLE1 90
#define MIN_ANGLE2 0
#define MAX_ANGLE2 270
#define MIN_ANGLE3 15
#define MAX_ANGLE3 240
#define MIN_ANGLE4 0
#define MAX_ANGLE4 270
#define MIN_ANGLE5 0
#define MAX_ANGLE5 270
#define STEP1 1.0f
#define STEP2 1.0f
#define STEP3 1.0f
#define STEP4 1.0f
#define STEP5 1.0f




void gripper_Init();
#endif //JUSTFW_rc2can_H
