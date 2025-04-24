#include "BrushMotorPWM.h"
#include <tim.h>
#include "interface.h"
#include "motor_manager_config.h"
#include "user_lib.h"

#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))//防止超出范围

static List_t motors;

float CalRate(MotorCondition *condition) {
    int fifo=0;
    for (int i=0; i<2; i++) {
        condition->rateFIFO[i] =condition->rateFIFO[i+1];
        fifo += condition->rateFIFO[i];
    }
    condition->rate = fifo/3;

    // [3] 将当前计算的rate存入缓冲区当前索引位置
    condition->filter_buf[condition->idx] = condition->rate;
    // [4] 索引递增并循环（0→1→2→3→4→0→...）
    condition->idx = (condition->idx + 1) % 5;
    // [5] 计算缓冲区所有元素的和
    float sum = 0;
    for (int i = 0; i < 5; i++) sum += condition->filter_buf[i];
    // [6] 求平均值并更新rate
    condition->rate = sum / 5;
    return condition->rate;
}//滤波

static void motor_set_pwm(INTF_Motor_HandleTypeDef *self) {
    BrushPWM_Motor_ResDataTypeDef *motor=self->private_data;

    __HAL_TIM_SET_COMPARE(motor->config.htim,motor->config.channel,self->real_speed);
    //*motor->config.ccr = CLAMP(abs(self->target_speed), 0, motor->config.htim->Instance->ARR);
     HAL_GPIO_WritePin(motor->config.GPIOx, motor->config.GPIO_Pin,(self->target_speed >= 0) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void motor_set_speed(INTF_Motor_HandleTypeDef* self, float speed) {   // 在motor_set_pwm中增加范围限制
    BrushPWM_Motor_ResDataTypeDef *motor=self->private_data;
    MotorCondition *condition=motor->condition;
    condition->rateFIFO[2]=CLAMP(fabs(self->target_speed), 0,motor->config.htim->Init.Period-1);
    self->real_speed =CalRate(condition);
}

 static void motor_send_pwm(INTF_Motor_HandleTypeDef* self) {
     BrushPWM_Motor_ResDataTypeDef* priv = self->private_data;
    INTF_CAN_MessageTypeDef msg = {
        .id_type = BSP_CAN_ID_STD,
        .can_id = self->motor_id};

     msg.data[0] = self->motor_id;
     msg.data[1] = ((int)copysign(1.0, self->target_speed)+1);
     msg.data[2] = abs(self->target_speed);

     vBusPublish(priv->can_tx_tp, &msg);
 }//用于CAN2PWM

static void motor_mainloop() {
    while (true) {
        ListItem_t* item = listGET_HEAD_ENTRY(&motors);
        while (item != listGET_END_MARKER(&motors)) {
            INTF_Motor_HandleTypeDef* m = listGET_LIST_ITEM_OWNER(item);
             // BrushPWM_Motor_ResDataTypeDef* priv = m->private_data;

           // priv->is_recieved = false;
            // motor_send_pwm(m);//can2pwm控制电机

            motor_set_pwm(m);//PWM控制电机

            item = listGET_NEXT(item);
            }
        vTaskDelay(1);
        }

    }

INTF_Motor_HandleTypeDef* BrushPWM_Motor_Register(BrushPWM_Motor_ConfigTypeDef* config) {
    INTF_Motor_HandleTypeDef* motor = pvSharePtr(config->motor_ptr_name, sizeof(INTF_Motor_HandleTypeDef));
    BrushPWM_Motor_ResDataTypeDef* priv = JUST_MALLOC(sizeof(BrushPWM_Motor_ResDataTypeDef));
    priv->config = *config;


    motor->private_data = priv;
    motor->motor_id = config->motor_id;
    motor->target_speed = 0.0f;
    motor->real_speed = 0.0f;
    motor->direction = config->direction;
    motor->set_speed = motor_set_speed;

    // priv->is_recieved = false;

    // priv->config = *config;

    ListItem_t* item = JUST_MALLOC(sizeof(ListItem_t));
    listSET_LIST_ITEM_OWNER(item, motor);
    vListInsertEnd(&motors, item);

    HAL_TIM_PWM_Start(config->htim,config->channel);
    return motor;
}

void BrushPWM_Motor_Init() {

    vListInitialise(&motors);
    BrushPWM_Motor_ConfigTypeDef config1 = {
        .motor_ptr_name = "/motor/MW_F_R",
        .direction = 1.0f,
        .motor_id = 2,
        .htim =&htim3,
        .channel = TIM_CHANNEL_3,
        .GPIOx = GPIOB,
        .GPIO_Pin = GPIO_PIN_7,
    };
    BrushPWM_Motor_Register(&config1);

    BrushPWM_Motor_ConfigTypeDef config2 = {
        .motor_ptr_name = "/motor/MW_F_L",
        .direction = 1.0f,
        .motor_id = 1,
        .htim =&htim3,
        .channel = TIM_CHANNEL_2,
        .GPIOx = GPIOB,
        .GPIO_Pin = GPIO_PIN_6,
    };
    BrushPWM_Motor_Register(&config2);

    BrushPWM_Motor_ConfigTypeDef config3 = {
        .motor_ptr_name = "/motor/MW_B_R",
        .direction = 1.0f,
        .motor_id = 4,
        .htim =&htim12,
        .channel = TIM_CHANNEL_1,
        .GPIOx = GPIOC,
        .GPIO_Pin = GPIO_PIN_3,
    };
    BrushPWM_Motor_Register(&config3);

    BrushPWM_Motor_ConfigTypeDef config4 = {
        .motor_ptr_name = "/motor/MW_B_L",
        .direction = 1.0f,
        .motor_id = 3,
        .htim =&htim3,
        .channel = TIM_CHANNEL_4,
        .GPIOx = GPIOC,
        .GPIO_Pin = GPIO_PIN_2,
    };
    BrushPWM_Motor_Register(&config4);

    xTaskCreate(motor_mainloop, "BR_Motor", 256, NULL, 124, NULL);
}