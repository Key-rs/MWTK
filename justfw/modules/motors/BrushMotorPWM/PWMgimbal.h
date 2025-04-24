//
// Created by ASUS on 25-4-23.
//

#ifndef PWMGIMBOL_H
#define PWMGIMBOL_H
//
// Created by ASUS on 25-4-11.
//



#include "interface.h"



typedef struct PWM_gimbal_Config {
    uint32_t motor_id;
    char *motor_ptr_name;  // 共享指针名
    float direction;// 电机方向（电机角度、输出乘以该系数，设置-1反向）
    uint8_t channel;
    TIM_HandleTypeDef *htim;       // 定时器句柄
    GPIO_TypeDef* GPIOx;
    uint16_t GPIO_Pin;

} PWM_gimbal_ConfigTypeDef;

typedef struct PWM_gimbal_ResData {
    float angle_offset;
    BusTopicHandle_t can_tx_tp;
    BusSubscriberHandle_t can_rx_sb;
    bool is_recieved;
    uint32_t last_tick;
    uint32_t compareValue;
    PWM_gimbal_ConfigTypeDef config;
}PWM_gimbal_ResDataTypeDef;

void PWM_gimbal_Init();
INTF_Motor_HandleTypeDef *PWM_gimbal_Register(PWM_gimbal_ConfigTypeDef *config);

#endif

