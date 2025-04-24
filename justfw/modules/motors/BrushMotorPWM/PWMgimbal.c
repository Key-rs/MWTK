//
// Created by ASUS on 25-4-23.
//

#include "PWMgimbal.h"
#include <tim.h>
#include "interface.h"
#include "motor_manager_config.h"
#include "user_lib.h"

#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))//防止超出范围

static List_t gimbals;

static void gimbal_set_pwm(INTF_Motor_HandleTypeDef *self) {

    PWM_gimbal_ResDataTypeDef *gimbal=self->private_data;

    self->target_speed=CLAMP(fabs(self->target_speed), 0,gimbal->config.htim->Init.Period-1);

     // __HAL_TIM_SET_COMPARE(&htim8,motor->config.channel,self->target_speed);

    __HAL_TIM_SET_COMPARE(gimbal->config.htim,gimbal->config.channel,self->target_speed);
    //*motor->config.ccr = CLAMP(abs(self->target_speed), 0, motor->config.htim->Instance->ARR);
    // HAL_GPIO_WritePin(motor->config.GPIOx, motor->config.GPIO_Pin,(self->target_speed >= 0) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void gimbal_set_speed(INTF_Motor_HandleTypeDef* self, float speed) {   // 在motor_set_pwm中增加范围限制

    self->target_speed = speed;
}

 static void gimbal_send_pwm(INTF_Motor_HandleTypeDef* self) {
     PWM_gimbal_ResDataTypeDef* priv = self->private_data;
    INTF_CAN_MessageTypeDef msg = {
        .id_type = BSP_CAN_ID_STD,
        .can_id = self->motor_id};

     msg.data[0] = self->motor_id;
     msg.data[1] = ((int)copysign(1.0, self->target_speed)+1);
     msg.data[2] = abs(self->target_speed);

     vBusPublish(priv->can_tx_tp, &msg);
 }//用于CAN2PWM

static void gimbal_mainloop() {
    while (true) {
        ListItem_t* item = listGET_HEAD_ENTRY(&gimbals);
        while (item != listGET_END_MARKER(&gimbals)) {
            INTF_Motor_HandleTypeDef* m = listGET_LIST_ITEM_OWNER(item);
             PWM_gimbal_ResDataTypeDef* priv = m->private_data;

           // priv->is_recieved = false;
            // motor_send_pwm(m);//can2pwm控制电机

            gimbal_set_pwm(m);//PWM控制电机

            item = listGET_NEXT(item);
            }
        vTaskDelay(10);
        }

    }

INTF_Motor_HandleTypeDef* PWM_gimbal_Register(PWM_gimbal_ConfigTypeDef* config) {
    INTF_Motor_HandleTypeDef* gimbal = pvSharePtr(config->motor_ptr_name, sizeof(INTF_Motor_HandleTypeDef));
    PWM_gimbal_ResDataTypeDef* priv = JUST_MALLOC(sizeof(PWM_gimbal_ResDataTypeDef));
    priv->config = *config;


    gimbal->private_data = priv;
    gimbal->motor_id = config->motor_id;
    gimbal->target_speed = 0.0f;
    gimbal->real_speed = 0.0f;
    gimbal->direction = config->direction;
    gimbal->set_speed = gimbal_set_speed;

    // priv->is_recieved = false;

    // priv->config = *config;

    ListItem_t* item = JUST_MALLOC(sizeof(ListItem_t));
    listSET_LIST_ITEM_OWNER(item, gimbal);
    vListInsertEnd(&gimbals, item);

    HAL_TIM_PWM_Start(config->htim,config->channel);
    return gimbal;
}

void BrushPWM_Motor_Init() {

    vListInitialise(&gimbals);
    PWM_gimbal_ConfigTypeDef config1 = {
        .motor_ptr_name = "/motor/gimbal_",
        .direction = 1.0f,
        .motor_id = 1,
        .htim =&htim8,
        .channel = TIM_CHANNEL_2,
        .GPIOx = GPIOA,
        .GPIO_Pin = GPIO_PIN_4,
    };
    PWM_gimbal_Register(&config1);


    PWM_gimbal_ConfigTypeDef config2 = {
        .motor_ptr_name = "/motor/gimbal/",
        .direction = 1.0f,
        .motor_id = 1,
        .htim =&htim8,
        .channel = TIM_CHANNEL_2,
        .GPIOx = GPIOA,
        .GPIO_Pin = GPIO_PIN_4,
    };
    PWM_gimbal_Register(&config2);

    xTaskCreate(gimbal_mainloop, "gimbal", 256, NULL, 124, NULL);
}