#include "MWchassisLOGIC.h"
#include <stdio.h>
#include "cmsis_os2.h"
#include "interface.h"
#include "cmsis_os.h"
#include "intf_motor.h"
#include "BrushMotorPWM.h"
#include "shared_ptr_intf.h"
#include "tim.h"
#include "user_lib.h"
osThreadId MW_Logic_MainLoopTaskHandle;
INTF_Chassis_HandleTypeDef *g_MW_logic_chassis;
RC_ctrl_t *MW_logic_rc_ctrl;

#define CHASSIS_SPEED_X_MAX 1000.0f
#define CHASSIS_SPEED_Y_MAX 1000.0f
#define CHASSIS_SPEED_W_MAX 1000.0f

extern int g_dr16_is_connected;
void MW_Logic_MainLoop() {
    osDelay(3000);  // 等待其他模块初始化
    while (1) {
        if (g_dr16_is_connected) {
            if (MW_logic_rc_ctrl[0].rc.switch_left == 1 || MW_logic_rc_ctrl[0].rc.switch_left == 2 ) {
                g_MW_logic_chassis->target_speed_x = MW_logic_rc_ctrl[0].rc.rocker_l_ / 660.0f * CHASSIS_SPEED_X_MAX;
                g_MW_logic_chassis->target_speed_y = MW_logic_rc_ctrl[0].rc.rocker_l1 / 660.0f * CHASSIS_SPEED_Y_MAX;
                g_MW_logic_chassis->target_speed_w = MW_logic_rc_ctrl[0].rc.rocker_r_ / 660.0f * CHASSIS_SPEED_W_MAX;
            }



        }else
        {
            g_MW_logic_chassis->target_speed_x = 0;
            g_MW_logic_chassis->target_speed_y = 0;
            g_MW_logic_chassis->target_speed_w = 0;
        }
        osDelay(5);
    }
}

void MAILUNTOUKUANG_Logic_Init() {
    g_MW_logic_chassis = pvSharePtr("chassis", sizeof(INTF_Chassis_HandleTypeDef));
    MW_logic_rc_ctrl = pvSharePtr("DR16", sizeof(RC_ctrl_t)*2);
    xTaskCreate(MW_Logic_MainLoop, "MW_Chassis_MainLoopTask", 512, NULL, 200, NULL);
}
