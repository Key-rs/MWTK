#ifndef __ENSTEP_MOTOR_C
#define __ENSTEP_MOTOR_C

#include "interface.h"
/* 这是一个虚假的步进电机, 通过微分无刷电机的角度来实现步进电机效果, 为什么要做这个呢？ */

typedef enum {
    ENSTEP_C610,
    ENSTEP_ODRIVE,
} ENSTEP_BRAND;

typedef struct EnStepMotorDef {
    INTF_Motor_HandleTypeDef *pim;  // 电机句柄
    char *name;
    float angle;             // 在CaliMotor坐标系下的实际角度 CaliMotor即已经完成零位校准的电机
    float offset;            // 从电机本身坐标系到电机零位之间的偏差角度
    bool done;               // 是否完成步进
    bool calibrated;         // 是否完成校准
    uint32_t step_count;     // 步进数量
    float _ret_angle;        // 从校准点到0点的回程角度
    float _dt_angle;         // 在步进微分下单位时间的步进角度
    float _step_angle;       // 步进角
    float _calibrate_speed;  // 校准速度
    float _achieved_angle;   // 目标角
    ENSTEP_BRAND _brand;     // 品牌
    bool _dir;               // 步进方向
} EnStepMotorDef_t;

typedef EnStepMotorDef_t *EnStepMotorHandle_t;

EnStepMotorHandle_t EnStepMotor_Create(INTF_Motor_HandleTypeDef *im,
                                       char *name,
                                       float ret_angle,
                                       float calibrate_speed,
                                       float step_angle,
                                       ENSTEP_BRAND brand);

// 开始校准
void EnStepMotor_StartCalibrate(EnStepMotorHandle_t self);
void EnStepMotor_SetAngle(EnStepMotorHandle_t self, float angle);
void EnStepMotor_Init();

#endif