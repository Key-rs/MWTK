
#include "gripper.h"

#include <cmsis_os2.h>
#include <i2c.h>



#include <tim.h>
#include "pca9685.h"


float servo1_angle = 0;//初始角度
float servo2_angle = 170;
float servo3_angle = 135;
float servo4_angle = 135;
float servo5_angle = 70;

static RC_ctrl_t *rc_ctrl;

void SetAngle(float angle , uint32_t servo_num){


    if(servo_num == servo_1){
        if (angle < MIN_ANGLE1) angle = MIN_ANGLE1;
        if (angle > MAX_ANGLE1) angle = MAX_ANGLE1;
        servo1_angle = angle;
    }
    if(servo_num == servo_2){
        if (angle < MIN_ANGLE2) angle = MIN_ANGLE2;
        if (angle > MAX_ANGLE2) angle = MAX_ANGLE2;
        servo2_angle = angle;
    }
    if(servo_num == servo_3){
        if (angle < MIN_ANGLE3) angle = MIN_ANGLE3;
        if (angle > MAX_ANGLE3) angle = MAX_ANGLE3;
        servo3_angle = angle;
    }
    if (servo_num == servo_4){
        if (angle < MIN_ANGLE4) angle = MIN_ANGLE4;
        if (angle > MAX_ANGLE4) angle = MAX_ANGLE4;
        servo4_angle = angle;
    }
    if (servo_num == servo_5){
        if (angle < MIN_ANGLE5) angle = MIN_ANGLE5;
        if (angle > MAX_ANGLE5) angle = MAX_ANGLE5;
        servo5_angle = angle;
    }

    PCA9685_SetServoAngle(servo_num, angle);

    vTaskDelay(10); // 等待电机转动到位

}



void gripper_MainLoop()
{
    vTaskDelay(1000); // 等待电机启动
    // SetAngle(0 , servo_1);
    PCA9685_Init(&hi2c3);
    // PCA9685_SetServoAngle(1,90);



    while (1)
    {
        // HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_RESET);
        // __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,1500);
        // PCA9685_SetServoAngle(0,135);




        if(rc_ctrl[0].rc.switch_left == 1 && rc_ctrl[0].rc.switch_right == 2)
        {

            
        if(rc_ctrl[0].rc.rocker_r_ > 100)
            {
            if(servo1_angle <= MAX_ANGLE1)
                {
                SetAngle(servo1_angle + STEP1,servo_1);

                }
            }
        if(rc_ctrl[0].rc.rocker_r_ < -100)
            {
            if(servo1_angle >= MIN_ANGLE1)
                {
                SetAngle(servo1_angle - STEP1,servo_1);
                }
            }



        if(rc_ctrl[0].rc.rocker_l_ > 100)
            {
            if(servo2_angle <= MAX_ANGLE2)
                {
                SetAngle(servo2_angle + STEP2,servo_2);
                }
            }
        if(rc_ctrl[0].rc.rocker_l_ < -100)
            {
            if(servo2_angle >= MIN_ANGLE2)
                {
                SetAngle(servo2_angle - STEP2,servo_2);
                }
            }


        if(rc_ctrl[0].rc.rocker_r1 > 100)
            {
            if(servo3_angle >= MIN_ANGLE3)
            {
                SetAngle(servo3_angle - STEP3,servo_3);
            }
            }
        if(rc_ctrl[0].rc.rocker_r1 < -100)
            {
            if(servo3_angle <= MAX_ANGLE3)
            {
                SetAngle(servo3_angle + STEP3,servo_3);
            }

            }
        if (rc_ctrl[0].rc.rocker_l1 > 100)
            {
            if(servo4_angle <= MAX_ANGLE4)
                {
                SetAngle(servo4_angle + STEP4,servo_4);
                }
            }
        if (rc_ctrl[0].rc.rocker_l1 < -100)
            {
            if(servo4_angle >= MIN_ANGLE4)
                {
                SetAngle(servo4_angle - STEP4,servo_4);
                }
            }
            if (rc_ctrl[0].rc.dial >100)
            {
                if(servo5_angle <= MAX_ANGLE5)
                SetAngle(servo5_angle + STEP5,servo_5);
            }
            if (rc_ctrl[0].rc.dial < -100)
            {
                if(servo5_angle >= MIN_ANGLE5)
                SetAngle(servo5_angle - STEP5,servo_5);
            }
            vTaskDelay(1);
        }



        vTaskDelay(1);
    
    }

}




void gripper_Init()
{
    rc_ctrl = pvSharePtr("DR16", sizeof(RC_ctrl_t));

    xTaskCreate(gripper_MainLoop, "gripper_MainLoop", 256, NULL, 240, NULL);
    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);

    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_4);
    // __HAL_TIM_SET_AUTORELOAD(&htim8, 20000);

}