#include "engin_chassis.h"

RC_ctrl_t *rc_ctrl;
// RC_ctrl_t last_rc_ctrl;

static void engin_chassis_loop() {
    float speed_x, speed_y, speed_w;

    while (true) {
        speed_x = rc_ctrl[0].rc.rocker_l_ / 660.0f * 1;
        speed_y = -rc_ctrl[0].rc.rocker_l1 / 660.0f * 1;
        speed_w = -rc_ctrl[0].rc.dial / 660.0f * 3 + (rc_ctrl[0].rc.rocker_r_ / 660.0f * 1);

        Engin_Chassis_Set_Speed(speed_x, speed_y, speed_w);
        // memcpy(&last_rc_ctrl, rc_ctrl, sizeof(RC_ctrl_t));

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void Engin_Chassis_Init() {
    Engin_Chassis_Motor_Init();

    rc_ctrl = pvSharePtr("DR16", sizeof(RC_ctrl_t));
    xTaskCreate(engin_chassis_loop, "EnginChassis", 1024, NULL, 240, NULL);
}
