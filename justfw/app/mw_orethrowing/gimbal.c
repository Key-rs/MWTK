#include "gimbal.h"
#include <tim.h>


#define PITCH_DIR_Pin GPIO_PIN_1//俯仰角度
#define PITCH_DIR_GPIO_Port GPIOC

#define looper_DIR_Pin GPIO_PIN_0//装弹方向
#define looper_DIR_GPIO_Port GPIOC

#define looper_angle 185.0f
#define shoot_angle 85.0f
#define mid_angle 135.0f

// #define LOOPER_LIMIT_FRONT_Pin GPIO_PIN_2//前限
// #define LOOPER_LIMIT_FRONT_GPIO_Port GPIOC
//
// #define LOOPER_LIMIT_BACK_Pin GPIO_PIN_3//后限
// #define LOOPER_LIMIT_BACK_GPIO_Port GPIOC

// #define PUL_Pin GPIO_PIN_12//信号
// #define PUL_GPIO_Port GPIOC


static RC_ctrl_t *rc_ctrl;


bool looper_limit_front = false;
bool looper_limit_back = false;

// void looper_limit(void)
// {
//     HAL_GPIO_ReadPin(LOOPER_LIMIT_FRONT_GPIO_Port, LOOPER_LIMIT_FRONT_Pin) == GPIO_PIN_SET ? looper_limit_front = true : looper_limit_front = false;
//     HAL_GPIO_ReadPin(LOOPER_LIMIT_BACK_GPIO_Port, LOOPER_LIMIT_BACK_Pin) == GPIO_PIN_SET ? looper_limit_back = true : looper_limit_back = false;
// }





void stepper_pulse(){


    if (rc_ctrl[0].rc.rocker_l1 > 100 || rc_ctrl[0].rc.rocker_l1 < -100)//pitch
    {
        __HAL_TIM_SetCompare(&htim12,TIM_CHANNEL_2,25);
    }
    else
    {
        __HAL_TIM_SetCompare(&htim12,TIM_CHANNEL_2,0);
    }

    if (rc_ctrl[0].rc.rocker_l_ > 100 || rc_ctrl[0].rc.rocker_l_ < -100)//looper
    {

        // looper_limit();//前后限检测
        if (looper_limit_front == false && looper_limit_back == false)
        {
            __HAL_TIM_SET_PRESCALER(&htim2, 84);
        __HAL_TIM_SET_AUTORELOAD(&htim2, 50);
        __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_3,25);
        }
        else
        {
            HAL_GPIO_TogglePin(looper_DIR_GPIO_Port,looper_DIR_Pin);
            __HAL_TIM_SET_PRESCALER(&htim2, 84);
            __HAL_TIM_SET_AUTORELOAD(&htim2, 50);
            __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_3,25);

        }

    }
    else
    {
        __HAL_TIM_SET_PRESCALER(&htim2, 84);
        __HAL_TIM_SET_AUTORELOAD(&htim2, 50);
        __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_3,0);
    }

}


void stepper_contral(int pitch_dir,int looper_dir){

    //俯仰角
    if(pitch_dir > 10){
        HAL_GPIO_WritePin(PITCH_DIR_GPIO_Port,PITCH_DIR_Pin,GPIO_PIN_SET);
    }else if (pitch_dir < -10)
    {
        HAL_GPIO_WritePin(PITCH_DIR_GPIO_Port,PITCH_DIR_Pin,GPIO_PIN_RESET);
    }

    else 
    {
        // HAL_GPIO_WritePin(PUL_GPIO_Port,PUL_Pin,GPIO_PIN_RESET);
        __HAL_TIM_SetCompare(&htim12,TIM_CHANNEL_2,0);
        // return;
    }

    //装弹方向

    if(looper_dir > 10){

        HAL_GPIO_WritePin(looper_DIR_GPIO_Port,looper_DIR_Pin,GPIO_PIN_SET);
    }else if (looper_dir < -10)
    {
        HAL_GPIO_WritePin(looper_DIR_GPIO_Port,looper_DIR_Pin,GPIO_PIN_RESET);
    }
    else
    {
        // HAL_GPIO_WritePin(PUL_GPIO_Port,PUL_Pin,GPIO_PIN_RESET);
        __HAL_TIM_SET_PRESCALER(&htim2, 84);
        __HAL_TIM_SET_AUTORELOAD(&htim2, 50);
        __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_3,0);
        // return;
    }

    stepper_pulse();



}

void banji()
{
    __HAL_TIM_SET_PRESCALER(&htim2,84);
    __HAL_TIM_SET_AUTORELOAD(&htim2,20000);
    __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,(int)(2000/270*mid_angle+500));

    if (rc_ctrl[0].rc.dial>10)
        __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,(int)(2000/270*looper_angle+500));
    else if (rc_ctrl[0].rc.dial<-10)
        __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,(int)(2000/270*shoot_angle+500));
    // else
    //     __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,(int)(2000/270*mid_angle+500));
    vTaskDelay(1);

}


void gimbal_MainLoop()
{
    vTaskDelay(300); // 等待电机启动
    // HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET);
   
        // HAL_GPIO_WritePin(DIR_GPIO_Port,DIR_Pin,GPIO_PIN_SET);
        // HAL_GPIO_WritePin(PUL_GPIO_Port,PUL_Pin,GPIO_PIN_SET);
    // __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_3,10000);

    while (1)

    {
       if(rc_ctrl[0].rc.switch_left == 1 && rc_ctrl[0].rc.switch_right == 3)
    {
           stepper_contral(rc_ctrl[0].rc.rocker_l1,rc_ctrl[0].rc.rocker_l_ );
           if (rc_ctrl[0].rc.dial != 0)
           banji();
    }
        // DWT_Delay(0.000001);

        vTaskDelay(1);

    }


}




void gimbal_Init()
{

    // __HAL_TIM_SET_PRESCALER(&htim2, 84);
    // __HAL_TIM_SET_AUTORELOAD(&htim2, 50);

    HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);


    rc_ctrl = pvSharePtr("DR16", sizeof(RC_ctrl_t));

    xTaskCreate(gimbal_MainLoop, "gimbal_MainLoop", 256, NULL, 240, NULL);

}