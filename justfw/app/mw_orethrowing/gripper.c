
#include "gripper.h"

#include <cmsis_os2.h>
#include <i2c.h>



#include <tim.h>
#include "pca9685.h"



static RC_ctrl_t *rc_ctrl;
#define a200 110
#define b200 120
#define a300 30
#define b300 240

#define put0 10
#define put1 40

#define open 130
#define closed 50




void gripper_MainLoop()
{
    // SetAngle(0 , servo_1);
    PCA9685_Init(&hi2c3);
    vTaskDelay(1000); // 等待电机启动

    // PCA9685_SetServoAngle(1,90);



    while (1)
    {
        // HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_RESET);
        // __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,1500);
        // PCA9685_SetServoAngle(0,10);//顺时针减小
        // vTaskDelay(1000);
        // PCA9685_SetServoAngle(1,40);//顺时针减小
        // vTaskDelay(1000);
        // PCA9685_SetServoAngle(2,130);//50闭合，130开

        // vTaskDelay(100);
        // PCA9685_SetServoAngle(3,160);
        // PCA9685_SetServoAngle(5,185);
        // PCA9685_SetServoAngle(5,200);//发射



        if (rc_ctrl[0].rc.switch_left == 1)//爪子抓矿状态
        {
            if (rc_ctrl[0].rc.switch_right == 1 &&
                rc_ctrl[0].rc.rocker_r1 >= 600)//爪子抓矿状态->200高度
            {
                PCA9685_SetServoAngle(0,a200);//下
                vTaskDelay(10);
                PCA9685_SetServoAngle(1,b200);//中
                vTaskDelay(10);
                PCA9685_SetServoAngle(2,open);//上->松爪
                vTaskDelay(10);
            }
            // vTaskDelay(1);

            else if (rc_ctrl[0].rc.switch_right == 3)
            {
                PCA9685_SetServoAngle(2,closed);//上->抓矿
                vTaskDelay(10);
            }

            else if (rc_ctrl[0].rc.switch_right == 1 &&
                     rc_ctrl[0].rc.rocker_r1 <= -600)//爪子抓矿状态->300高度
            {
                PCA9685_SetServoAngle(0,a300);//下
                vTaskDelay(10);
                PCA9685_SetServoAngle(1,b300);//中
                vTaskDelay(10);
                PCA9685_SetServoAngle(2,open);//上->松爪
                vTaskDelay(10);

            }
            vTaskDelay(10);


        }


        if (rc_ctrl[0].rc.switch_left == 2)//爪子放矿状态
        {
            if (rc_ctrl[0].rc.switch_right == 2 &&
                rc_ctrl[0].rc.rocker_r1 >= 500)//爪子放矿状态
            {
                PCA9685_SetServoAngle(0,put0);
                vTaskDelay(10);
                PCA9685_SetServoAngle(1,put1);
                vTaskDelay(10);
                PCA9685_SetServoAngle(2,closed);
                vTaskDelay(10);
            }
            // vTaskDelay(1);

            else if (rc_ctrl[0].rc.switch_right == 3)
            {
                PCA9685_SetServoAngle(2,open);
                vTaskDelay(10);
            }
            vTaskDelay(10);

        }


        if (rc_ctrl[0].rc.dial < -100)//静默
        {
            PCA9685_SetServoAngle(5,175);
        }
        // vTaskDelay(1);
        if (rc_ctrl[0].rc.dial > 100)//扳机发射
        {
            PCA9685_SetServoAngle(5,200);
        }
        vTaskDelay(10);


    
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