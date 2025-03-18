#include "engin_little_hand.h"

static RC_ctrl_t *rc_ctrl;

static void Engin_Little_Hand_Loop() {
    uint32_t last_tick;

    while (true) {
        float dt = DWT_GetDeltaT(&last_tick);
        if (rc_ctrl[0].rc.switch_right == RC_SW_UP) {
            m_l_joint2->set_angle(m_l_joint2, m_l_joint2->target_angle + rc_ctrl[0].rc.rocker_r_ / 660.0f * dt);
        }
    }
}

void Engin_Little_Hand_Init() {
    xTaskCreate(Engin_Little_Hand_Loop, "EnginLittle_Hand", 1024, NULL, 240, NULL);
    rc_ctrl = pvSharePtr("DR16", sizeof(RC_ctrl_t));
}