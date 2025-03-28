/*
 * @Author: Konodoki 1326898804@qq.com
 * @Date: 2025-03-28 14:25:45
 * @LastEditors: Konodoki 1326898804@qq.com
 * @LastEditTime: 2025-03-28 16:29:06
 * @FilePath: \justos\justfw\modules\motors\BrushESC775\BrushESC775.h
 * @Description: 
 * 
 * Copyright (c) 2025 by 1326898804@qq.com, All Rights Reserved. 
 */
#ifndef JUSTOS_BRUSHESC775_H
#define JUSTOS_BRUSHESC775_H
#include "interface.h"
#define  BrushESC775_CONTROLLER_CAN_P1_Base 0x60 //角度与速度反馈帧 CONTROLLER_CAN_P1_Base+ID
#define  BrushESC775_CONTROLLER_CAN_P2_Base 0x70 //其余信息 CONTROLLER_CAN_P2_Base+ID
#define  BrushESC775_CONTROLLER_CAN_RECEIVE_Base 0x80 //控制帧

typedef struct BrushESC775_Config {
    uint32_t motor_id;
    char *motor_ptr_name;  // 共享指针名
    float direction;                    // 电机方向（电机角度、输出乘以该系数，设置-1反向）
    INTF_Motor_ModeTypeDef motor_mode;  // 运行模式
    char *can_rx_topic_name;
    char *can_tx_topic_name;
} BrushESC775_ConfigTypeDef;

typedef struct BrushESC775_ResData {
    BusSubscriberHandle_t can_rx_topic;
    BusTopicHandle_t can_tx_topic;
    float Vm;//控制器电压
    float current;//控制器电流
    float cpu_tmp;//控制器温度
} BrushESC775_ResDataTypeDef;

INTF_Motor_HandleTypeDef *BrushESC775_Register(BrushESC775_ConfigTypeDef *config);
void BrushESC775_Init();

#endif