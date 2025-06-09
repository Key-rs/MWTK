#include "MWchassisLOGIC.h"
#include "cmsis_os2.h"
#include "interface.h"
#include "cmsis_os.h"
#include "intf_motor.h"
#include "BrushMotorPWM.h"
#include "shared_ptr_intf.h"
#include "tim.h"
osThreadId MW_Logic_MainLoopTaskHandle;
INTF_Motor_HandleTypeDef *steeper_;
INTF_Motor_HandleTypeDef *steeper1;

INTF_Chassis_HandleTypeDef *g_MW_logic_chassis;
RC_ctrl_t *MW_logic_rc_ctrl;
INTF_Motor_HandleTypeDef *DM1;

#define CHASSIS_SPEED_X_MAX 1000.0f
#define CHASSIS_SPEED_Y_MAX 1000.0f
#define CHASSIS_SPEED_W_MAX 1000.0f

extern int g_dr16_is_connected;
// void Steper_Logic()
// {
//     if(MW_logic_rc_ctrl[0].rc.switch_left == 1 && MW_logic_rc_ctrl[0].rc.switch_right == 3)
//     {
//         __HAL_TIM_SET_PRESCALER(&htim8, 84);
//         __HAL_TIM_SET_AUTORELOAD(&htim8, 50);
//         if (MW_logic_rc_ctrl[0].rc.rocker_l1>200)
//         {
//             steeper_->set_speed(steeper_,30);
//         } else if (MW_logic_rc_ctrl[0].rc.rocker_l1<-200)
//         {
//             steeper_->set_speed(steeper_,-30);
//         }else
//         {
//             steeper_->set_speed(steeper_,0);
//         }
//
//         if (MW_logic_rc_ctrl[0].rc.rocker_r1>200)
//         {
//             steeper1->set_speed(steeper1,30);
//         } else if (MW_logic_rc_ctrl[0].rc.rocker_r1<-200)
//         {
//             steeper1->set_speed(steeper1,-30);
//         }
//         else
//         {
//             steeper1->set_speed(steeper1,0);
//         }
//     }
// }


void MW_Logic_MainLoop() {
    osDelay(3000);  // 等待其他模块初始化
    while (1) {

        if (g_dr16_is_connected) {
            // 底盘控制

            if (MW_logic_rc_ctrl[0].rc.switch_left == 1 && MW_logic_rc_ctrl[0].rc.switch_right == 1) {

                g_MW_logic_chassis->target_speed_x = MW_logic_rc_ctrl[0].rc.rocker_l_ / 660.0f * CHASSIS_SPEED_X_MAX;
                g_MW_logic_chassis->target_speed_y = MW_logic_rc_ctrl[0].rc.rocker_l1 / 660.0f * CHASSIS_SPEED_Y_MAX;
                g_MW_logic_chassis->target_speed_w = MW_logic_rc_ctrl[0].rc.rocker_r_ / 660.0f * CHASSIS_SPEED_W_MAX;

                 // DM1->set_speed(DM1,MW_logic_rc_ctrl[0].rc.dial / 660.0f*20);
                 // DM1->set_torque(DM1,MW_logic_rc_ctrl[0].rc.rocker_r1/ 660.0f*15);

            }
        // Steper_Logic();
            if (MW_logic_rc_ctrl[0].rc.switch_left==2 && MW_logic_rc_ctrl[0].rc.switch_right==2)
                DM1->set_torque(DM1,MW_logic_rc_ctrl[0].rc.rocker_r1/ 660.0f*15);
        Steper_Logic();
        }else
        {
            g_MW_logic_chassis->target_speed_x = 0;
            g_MW_logic_chassis->target_speed_y = 0;
            g_MW_logic_chassis->target_speed_w = 0;
            // DM1->set_angle(DM1,0);
        }
        osDelay(5);
    }
}

void MAILUNTOUKUANG_Logic_Init() {

     DM1=pvSharePtr("DM1", sizeof(INTF_Motor_HandleTypeDef));
    g_MW_logic_chassis = pvSharePtr("chassis", sizeof(INTF_Chassis_HandleTypeDef));
    MW_logic_rc_ctrl = pvSharePtr("DR16", sizeof(RC_ctrl_t)*2);

    // steeper_ = pvSharePtr("steeper_", sizeof(INTF_Motor_HandleTypeDef));
    // steeper1 = pvSharePtr("steeper1", sizeof(INTF_Motor_HandleTypeDef));

    xTaskCreate(MW_Logic_MainLoop, "MW_Chassis_MainLoopTask", 512, NULL, 200, NULL);
}
