//
// Created by ASUS on 25-4-11.
//

#ifndef BRUSHMOTORPWM_H
#define BRUSHMOTORPWM_H


#include "interface.h"

#define windows 30

// kalman_filter.h（可新建一个头文件或直接放在 BrushMotorPWM.c 上方）

typedef struct {
    float x;   // 状态估计值
    float P;   // 估计误差协方差
    float Q;   // 过程噪声
    float R;   // 测量噪声
} KalmanFilter;



typedef struct MotorCondition {
    float rateFIFO[3];
    float rate;
    float filter_buf[windows];
    uint8_t idx;

    KalmanFilter kf;
}MotorCondition;

typedef struct BrushPWM_Motor_Config {
    uint32_t motor_id;
    char *motor_ptr_name;  // 共享指针名
    float direction;// 电机方向（电机角度、输出乘以该系数，设置-1反向）
    uint8_t channel;
    TIM_HandleTypeDef *htim;       // 定时器句柄
    GPIO_TypeDef* GPIOx;
    uint16_t GPIO_Pin;

} BrushPWM_Motor_ConfigTypeDef;

typedef struct BrushPWM_Motor_ResData {
    float angle_offset;
    BusTopicHandle_t can_tx_tp;
    BusSubscriberHandle_t can_rx_sb;
    bool is_recieved;
    uint32_t last_tick;
    uint32_t compareValue;
   BrushPWM_Motor_ConfigTypeDef config;
    MotorCondition *condition;
} BrushPWM_Motor_ResDataTypeDef;

void BrushPWM_Motor_Init();
INTF_Motor_HandleTypeDef *BrushPWM_Motor_Register(BrushPWM_Motor_ConfigTypeDef *config);
#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))//防止超出范围

#endif