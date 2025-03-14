#include "interface.h"
//
#include <stdio.h>

#include "Odrive_CAN.h"
#include "SteadyWin_MIT.h"

static INTF_Motor_HandleTypeDef *m;
static RC_ctrl_t *rc_ctrl;

static void app1_mainloop() {
    uint32_t lask_tick;
    while (1) {
        float dt = DWT_GetDeltaT(&lask_tick);
        m->set_angle(m, m->target_angle + rc_ctrl->rc.rocker_l1 / 660.0f * PI * dt);
        vTaskDelay(pdMS_TO_TICKS(1));
        // printf("angle:%f, %f\r\n", m->real_angle, m->target_angle);
    }
}

void app1_init() {
    // SteadyWin_MIT_ConfigTyepdef config = {
    //     .motor_id = 2,
    //     .motor_name = "/motor/test",
    //     .angle_offset = 0,
    //     .can_tx_topic_name = "/CAN2/TX",
    //     .can_rx_topic_name = "/CAN2/RX",
    //     .direction = 1.0f,
    //     .kd = 100.0f,
    //     .kp = 20.0f};

    Odrive_CAN_ConfigTypedef config = {
        .motor_id = 0,
        .motor_name = "/motor/odrive",
        .angle_offset = 0,
        .can_tx_topic_name = "/CAN1/TX",
        .can_rx_topic_name = "/CAN1/RX",
        .direction = 1.0f,
        .kp = 0.2f,
        .kd = 0.05f,
    };

    m = Odrive_Register(&config);

    rc_ctrl = pvSharePtr("DR16", sizeof(RC_ctrl_t));
    xTaskCreate(app1_mainloop, "App1", 512, NULL, 254, NULL);
    // m = GIM3505_8_Register(&config);
}