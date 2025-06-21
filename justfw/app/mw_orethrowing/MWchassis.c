#include "cmsis_os2.h"
#include "interface.h"
#include "cmsis_os.h"
#include "intf_motor.h"
#include "shared_ptr_intf.h"
#include "MWchassis.h"

static RC_ctrl_t *rc;
static INTF_Motor_HandleTypeDef *Brush775FL;
static INTF_Motor_HandleTypeDef *Brush775FR;
static INTF_Motor_HandleTypeDef *Brush775BL;
static INTF_Motor_HandleTypeDef *Brush775BR;

osThreadId MW_Chassis_MainLoopTaskHandle;
INTF_Chassis_HandleTypeDef *g_mw_chassis;

// 轮子中心构成的矩形的大小 单位m
#define Chassis_Width 0.49
#define Chassis_Length 0.35

// 轮子半径 单位m
#define WHEEL_R 1.0f

/// @brief 设置底盘速度
/// \param self
/// \param speed_x 单位m/s
/// \param speed_y 单位m/s
/// \param speed_w 单位rad/s
void MW_Chassis_SetSpeed(INTF_Chassis_HandleTypeDef *self, float speed_x, float speed_y, float speed_w) {
    MW_Chassis_ResDataTypeDef *priv = self->private_data;

    g_mw_chassis->target_speed_x = speed_x;
    g_mw_chassis->target_speed_y = speed_y;
    g_mw_chassis->target_speed_w = speed_w;

    priv->motor_f_r->set_speed(priv->motor_f_r,-speed_x + speed_y + speed_w);
    priv->motor_f_l->set_speed(priv->motor_f_l,-speed_x - speed_y + speed_w);
    priv->motor_b_l->set_speed(priv->motor_b_l,-speed_x + speed_y - speed_w);
    priv->motor_b_r->set_speed(priv->motor_b_r,+speed_x + speed_y + speed_w);

    // priv->motor_b_l->set_speed(priv->motor_b_l,  speed_x + speed_w);  // 后左轮（纵向全向轮）
    // priv->motor_b_r->set_speed(priv->motor_b_r, -speed_x + speed_w);  // 后右轮（纵向全向轮）
    // float data[4];
    // data[0] = priv->motor_f_r->target_speed;
    // data[1] = priv->motor_f_l->target_speed;
    // data[2] = priv->motor_b_r->target_speed;
    // data[3] = priv->motor_b_l->target_speed;

    // EF_send_float(data, 4);
}

void MW_Chassis_MainLoop() {

    while (1) {
        MW_Chassis_SetSpeed(g_mw_chassis, g_mw_chassis->target_speed_x, g_mw_chassis->target_speed_y, g_mw_chassis->target_speed_w);
        osDelay(5);
    }
}

void MW_Chassis_Init() {
    g_mw_chassis = pvSharePtr("chassis", sizeof(INTF_Chassis_HandleTypeDef));

    MW_Chassis_ResDataTypeDef *priv = g_mw_chassis->private_data = JUST_MALLOC(sizeof(MW_Chassis_ResDataTypeDef));

    priv->motor_f_r = pvSharePtr("/motor/MW_F_R", sizeof(INTF_Motor_HandleTypeDef));
    priv->motor_f_l = pvSharePtr("/motor/MW_F_L", sizeof(INTF_Motor_HandleTypeDef));
    priv->motor_b_l = pvSharePtr("/motor/MW_B_L", sizeof(INTF_Motor_HandleTypeDef));
    priv->motor_b_r = pvSharePtr("/motor/MW_B_R", sizeof(INTF_Motor_HandleTypeDef));

    xTaskCreate(MW_Chassis_MainLoop, "MW_Chassis_MainLoopTask", 256, NULL, 200, NULL);
}