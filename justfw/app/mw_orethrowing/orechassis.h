#ifndef JUSTFW_orechassis_H
#define JUSTFW_orechassis_H


#define motor1_dir_port GPIOB
#define motor2_dir_port GPIOB
#define motor3_dir_port GPIOC
#define motor4_dir_port GPIOC

#define motor1_dir_pin GPIO_PIN_6
#define motor2_dir_pin GPIO_PIN_7
#define motor3_dir_pin GPIO_PIN_2
#define motor4_dir_pin GPIO_PIN_3

#define motor1_pwm_tim &htim3
#define motor2_pwm_tim &htim3
#define motor3_pwm_tim &htim3
#define motor4_pwm_tim &htim12

#define motor1_pwm_ch TIM_CHANNEL_2
#define motor2_pwm_ch TIM_CHANNEL_3
#define motor3_pwm_ch TIM_CHANNEL_4
#define motor4_pwm_ch TIM_CHANNEL_1


#include "stm32f4xx.h"
#include "tinybus.h"
#include "interface.h"




void orechassis_Init();
#endif // JUSTFW_orechassis_H
