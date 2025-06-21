#include <stdio.h>
#include "SuperBig.h"
#include "user_lib.h"
RC_ctrl_t* Super_logic_rc_ctrl;
INTF_Motor_HandleTypeDef* GM1;

#define fanwei 130.0f
#define weizhi1 -80.0f
#define weizhi2 -130.0f

#include "stm32f4xx_hal.h"

static float state=0,temp=0,ready=0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_6) // 检查触发中断的引脚
    {
                    state=1;
        temp=GM1->real_angle;
    }
}


extern int g_dr16_is_connected;

static void SuperBig_MainLoop()
{
    vTaskDelay(2000);

    while (1)
    {
        if (g_dr16_is_connected)
        {
            if (Super_logic_rc_ctrl[0].rc.switch_left==3&&
                Super_logic_rc_ctrl[0].rc.switch_right==1&&
                (Super_logic_rc_ctrl[0].rc.rocker_r1<-500))
            {
                state=0;
            }

            if (Super_logic_rc_ctrl[0].rc.switch_left==3&&
                Super_logic_rc_ctrl[0].rc.switch_right==1&&
                state==0)
            {
                GM1->target_angle=Super_logic_rc_ctrl[0].rc.rocker_l1/660.0f*fanwei;
                if (Super_logic_rc_ctrl[0].rc.rocker_r1>500)
                {
                    state=1;
                    temp=GM1->real_angle;
                }
            }else if (Super_logic_rc_ctrl[0].rc.switch_left==3&&state==1)
            {
                if (Super_logic_rc_ctrl[0].rc.switch_right==1)
                {
                    GM1->target_angle=temp;
                }else if (Super_logic_rc_ctrl[0].rc.switch_right==3)
                {
                    GM1->target_angle=temp+weizhi1;
                }else if (Super_logic_rc_ctrl[0].rc.switch_right==2)
                {
                    GM1->target_angle=temp+weizhi2;
                }

                // printf("GM1:%f,%f\n", GM1->real_angle, GM1->target_angle);
                vTaskDelay(5);
            }
        }
        // printf("GM1:%f,%f\n", GM1->real_angle, GM1->target_angle);
        vTaskDelay(5);
    }
}

void SuperBig_Init()
{
    GM1 = pvSharePtr("GMMotor1", sizeof(INTF_Motor_HandleTypeDef));
    Super_logic_rc_ctrl = pvSharePtr("DR16", sizeof(RC_ctrl_t));
    xTaskCreate(SuperBig_MainLoop, "SuperBig_MainLoop", 512, NULL, 240, NULL);
}
