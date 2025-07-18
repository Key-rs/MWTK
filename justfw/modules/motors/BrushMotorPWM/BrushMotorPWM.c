#include "BrushMotorPWM.h"
#include <tim.h>
#include "interface.h"
#include "motor_manager_config.h"
#include "user_lib.h"


static List_t motors;

void kalman_init(KalmanFilter *kf, float initial_value, float initial_P, float Q, float R);
float kalman_update(KalmanFilter *kf, float measurement);

void kalman_init(KalmanFilter *kf, float initial_value, float initial_P, float Q, float R) {
    kf->x = initial_value;
    kf->P = initial_P;
    kf->Q = Q;
    kf->R = R;
}

float kalman_update(KalmanFilter *kf, float measurement) {
    // Step 1: Predict
    float x_pred = kf->x;
    float P_pred = kf->P + kf->Q;

    // Step 2: Compute Kalman Gain
    float K = P_pred / (P_pred + kf->R);

    // Step 3: Update estimate
    kf->x = x_pred + K * (measurement - x_pred);
    kf->P = (1 - K) * P_pred;

    return kf->x;
}

float CalRate(MotorCondition *condition) {
    int fifo=0;
    for (int i=0; i<3; i++) {
        fifo += condition->rateFIFO[i];
        condition->rateFIFO[i] =condition->rateFIFO[i+1];
    }
    condition->rate = fifo/3;
    // [3] 将当前计算的rate存入缓冲区当前索引位置
    condition->filter_buf[condition->idx] = condition->rate;
    // [4] 索引递增并循环（0→1→2→3→4→0→...）
    condition->idx = (condition->idx + 1) % windows;
    // [5] 计算缓冲区所有元素的和
    float sum = 0;
    for (int i = 0; i < windows; i++) sum += condition->filter_buf[i];
    // [6] 求平均值并更新rate
    condition->rate = sum / windows;
    return condition->rate;
}

static void motor_set_pwm(INTF_Motor_HandleTypeDef *self) {
    BrushPWM_Motor_ResDataTypeDef *motor=self->private_data;
    __HAL_TIM_SET_COMPARE(motor->config.htim,motor->config.channel,fabs(self->real_speed));
    //*motor->config.ccr = CLAMP(abs(self->target_speed), 0, motor->config.htim->Instance->ARR);
     HAL_GPIO_WritePin(motor->config.GPIOx, motor->config.GPIO_Pin,(self->direction*self->real_speed) >= 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void motor_set_speed(INTF_Motor_HandleTypeDef* self,float speed)
{   // 在motor_set_pwm中增加范围限制
    // self->target_speed=speed;
        BrushPWM_Motor_ResDataTypeDef *motor = self->private_data;
        MotorCondition *condition = motor->condition;
        int p = motor->config.htim->Init.Period - 1;
        // 使用卡尔曼滤波进行平滑
        condition->rateFIFO[2] = kalman_update(&condition->kf, CLAMP(speed, -p, p));
    self->real_speed =CalRate(condition);
    // int p=motor->config.htim->Init.Period-1;
    // condition->rateFIFO[2]=CLAMP(speed, -p,p);
    // self->real_speed =CalRate(condition);
}

static void motor_mainloop() {
    while (true) {
        ListItem_t* item = listGET_HEAD_ENTRY(&motors);
        while (item != listGET_END_MARKER(&motors)) {
            INTF_Motor_HandleTypeDef* m = listGET_LIST_ITEM_OWNER(item);
             // BrushPWM_Motor_ResDataTypeDef* priv = m->private_data;
           // priv->is_recieved = false;
            // motor_send_pwm(m);//can2pwm控制电机
            __HAL_TIM_SET_AUTORELOAD(&htim3, 1000);
            __HAL_TIM_SET_AUTORELOAD(&htim12, 1000);

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

    // 分配并初始化 MotorCondition
    priv->condition = JUST_MALLOC(sizeof(MotorCondition));
    memset(priv->condition, 0, sizeof(MotorCondition));
    // 初始化卡尔曼滤波器：初始值为 0，初始误差为 1，过程噪声 0.3，测量噪声 0.5
    kalman_init(&priv->condition->kf, 0.0f, 1.0f, 0.1f, 1.0f);

    motor->private_data = priv;
    motor->motor_id = config->motor_id;
    motor->target_speed = 0.0f;
    motor->real_speed = 0.0f;
    motor->direction = config->direction;
    motor->set_speed = motor_set_speed;

    ListItem_t* item = JUST_MALLOC(sizeof(ListItem_t));
    listSET_LIST_ITEM_OWNER(item, motor);
    vListInsertEnd(&motors, item);

    HAL_TIM_PWM_Start(config->htim, config->channel);
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
        .direction = -1.0f,
        .motor_id = 3,
        .htim =&htim3,
        .channel = TIM_CHANNEL_4,
        .GPIOx = GPIOC,
        .GPIO_Pin = GPIO_PIN_2,
    };
    BrushPWM_Motor_Register(&config4);

    // BrushPWM_Motor_ConfigTypeDef steeper_ = {
    //     .motor_ptr_name = "steeper_",
    //     .direction = -1.0f,
    //     .motor_id = 5,
    //     .htim =&htim8,
    //     .channel = TIM_CHANNEL_1,
    //     .GPIOx = GPIOC,
    //     .GPIO_Pin = GPIO_PIN_7,
    // };
    // BrushPWM_Motor_Register(&steeper_);
    //
    // BrushPWM_Motor_ConfigTypeDef steeper1 = {
    //     .motor_ptr_name = "steeper1",
    //     .direction = 1.0f,
    //     .motor_id = 6,
    //     .htim =&htim8,
    //     .channel = TIM_CHANNEL_3,
    //     .GPIOx = GPIOC,
    //     .GPIO_Pin = GPIO_PIN_9,
    // };
    // BrushPWM_Motor_Register(&steeper1);

    xTaskCreate(motor_mainloop, "BR_Motor", 256, NULL, 124, NULL);
}