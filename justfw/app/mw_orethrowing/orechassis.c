
#include "orechassis.h"
#include <tim.h>



static RC_ctrl_t *rc_ctrl;


#define MAX_INPUT 660
#define MAX_PWM 1000

static void set_motor(int motor_id, int speed)
{

    __HAL_TIM_SET_PRESCALER(&htim3,8);
    __HAL_TIM_SET_AUTORELOAD(&htim3, 1000);
    __HAL_TIM_SET_PRESCALER(&htim12,8);
    __HAL_TIM_SET_AUTORELOAD(&htim12, 1000);
    int pwm = abs(speed);
    GPIO_TypeDef *port;
    uint16_t pin;
    TIM_HandleTypeDef *tim;
    uint32_t ch;

    switch (motor_id)
    {
    case 1:
        port = motor1_dir_port;
        pin = motor1_dir_pin;
        tim = motor1_pwm_tim;
        ch = motor1_pwm_ch;
        break;
    case 2:
        port = motor2_dir_port;
        pin = motor2_dir_pin;
        tim = motor2_pwm_tim;
        ch = motor2_pwm_ch;
        break;
    case 3:
        port = motor3_dir_port;
        pin = motor3_dir_pin;
        tim = motor3_pwm_tim;
        ch = motor3_pwm_ch;
        break;
    case 4:
        port = motor4_dir_port;
        pin = motor4_dir_pin;
        tim = motor4_pwm_tim;
        ch = motor4_pwm_ch;
        break;
    default:
        return;
    }

    HAL_GPIO_WritePin(port, pin, speed > 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    __HAL_TIM_SetCompare(tim, ch, pwm > MAX_PWM ? MAX_PWM : pwm);

    vTaskDelay(1);
}



void orechassis_MainLoop()
{
    vTaskDelay(3000); // 电机启动等待


    while (1)
    {
        // HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_RESET);


        // __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,500);
        int Vx = (rc_ctrl[0].rc.rocker_l_ * 1000) / MAX_INPUT;  // 横向
        int Vy = (rc_ctrl[0].rc.rocker_l1  * 1000) / MAX_INPUT;  // 纵向
        int Wz = (rc_ctrl[0].rc.rocker_r_ * 1000) / MAX_INPUT;  // 旋转

        // 麦轮逆解算
        int speed1 = +Vy + Vx + Wz; // 左前
        int speed2 = -Vy + Vx + Wz; // 右前
        int speed3 = -Vy + Vx - Wz; // 左后
        int speed4 = -Vy - Vx + Wz; // 右后

        // 防止超过最大PWM
        int max_val = fmax(fmax(abs(speed1), abs(speed2)), fmax(abs(speed3), abs(speed4)));
        if (max_val > MAX_PWM)
        {
            speed1 = speed1 * MAX_PWM / max_val;
            speed2 = speed2 * MAX_PWM / max_val;
            speed3 = speed3 * MAX_PWM / max_val;
            speed4 = speed4 * MAX_PWM / max_val;
        }
        if (rc_ctrl[0].rc.switch_left == 1 && rc_ctrl[0].rc.switch_right == 1)
        {
            // 控制四个电机
            set_motor(1, speed1);
            set_motor(2, speed2);
            set_motor(3, speed3);
            set_motor(4, speed4);
            vTaskDelay(10);
        }

        vTaskDelay(10); // 控制频率
    }
    vTaskDelay(1);
}


void orechassis_Init()
{
    rc_ctrl = pvSharePtr("DR16", sizeof(RC_ctrl_t));

    xTaskCreate(orechassis_MainLoop, "orechassis_MainLoop", 256, NULL, 240, NULL);

    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);

    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);

    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);

    HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_1);

    // HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2);
    // __HAL_TIM_SET_AUTORELOAD(&htim8, 20000);


//    orechassis_MainLoopTaskHandle = osThreadCreate(osThread(orechassis_MainLoopTask), NULL);
}