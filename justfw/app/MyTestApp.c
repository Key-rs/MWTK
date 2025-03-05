#include "interface.h"
//
#include "SteadyWin_MIT.h"

static INTF_Motor_HandleTypeDef *m;
static RC_ctrl_t *rc_ctrl;

static void app1_mainloop() {
    while (1) {
        m->set_angle(m, rc_ctrl->rc.rocker_l1 / 660.0f * PI);
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void app1_init() {
    SteadyWin_MIT_ConfigTyepdef config = {
        .motor_id = 2,
        .motor_name = "/motor/test",
        .angle_offset = 0,
        .can_tx_topic_name = "/CAN1/TX",
        .can_rx_topic_name = "/CAN1/RX",
        .direction = 1.0f,
        .kd = 100.0f,
        .kp = 20.0f};

    xTaskCreate(app1_mainloop, "App1", 256, NULL, 254, NULL);

    rc_ctrl = Bus_SharePtr("DR16", sizeof(RC_ctrl_t));
    m = GIM3505_8_Register(&config);
}