#include <stdio.h>

#include "C610.h"
#include "cali_motor.h"
#include "interface.h"

static CaliMotor_Handle_t cm_l2;
static CaliMotor_Handle_t cm_l3;
static CaliMotor_Handle_t cm_l4;

static CaliMotor_Handle_t cm_r2;
static CaliMotor_Handle_t cm_r3;
static CaliMotor_Handle_t cm_r4;

extern INTF_Motor_HandleTypeDef *m_l_joint1;
extern INTF_Motor_HandleTypeDef *m_l_joint2;
extern INTF_Motor_HandleTypeDef *m_l_joint3;
extern INTF_Motor_HandleTypeDef *m_l_joint4;

static TaskHandle_t lb_positon_task_handle;
static TaskHandle_t rb_positon_task_handle;

typedef struct b_PoseDef {
    float angle[4];
    uint32_t steps;
} b_Pose_t;

bool lb_positon_done;

static void lb_postion_task() {
    b_Pose_t target_position;
    float step_angle[4];
    while (true) {
        // 等待数据阶段
        if (lb_positon_done) {
            while (true) {
                uint32_t ulNotificationValue;
                if (xTaskNotifyWait(0, 0, &ulNotificationValue, portMAX_DELAY)) {
                    b_Pose_t *xpPose = (b_Pose_t *)ulNotificationValue;  // 假设通知值是一个指针
                    memcpy(&target_position, xpPose, sizeof(b_Pose_t));
                    break;
                }
            }
        }

        // 计算步进角度
        step_angle[1] = (target_position.angle[1] - cali_motor_get_anlge(cm_l2)) / target_position.steps;
        step_angle[2] = (target_position.angle[2] - cali_motor_get_anlge(cm_l3)) / target_position.steps;
        step_angle[3] = (target_position.angle[3] - cali_motor_get_anlge(cm_l4)) / target_position.steps;

        // 执行步进动作
        for (uint8_t i = 0; i < target_position.steps; i++) {
            m_l_joint2->set_angle(m_l_joint2, m_l_joint2->target_angle + step_angle[1]);
            m_l_joint3->set_angle(m_l_joint3, m_l_joint3->target_angle + step_angle[2]);
            m_l_joint4->set_angle(m_l_joint4, m_l_joint4->target_angle + step_angle[3]);

            uint32_t ulNotificationValue;
            if (xTaskNotifyWait(0, 0, &ulNotificationValue, portMAX_DELAY)) {
                b_Pose_t *xpPose = (b_Pose_t *)ulNotificationValue;  // 假设通知值是一个指针
                memcpy(&target_position, xpPose, sizeof(b_Pose_t));
                break;
            }

            vTaskDelay(pdMS_TO_TICKS(10));  // 适当调整延迟时间
        }

        lb_positon_done = true;
    }
}

void lb_set_position(float angle[4], uint32_t steps) {
    // xTaskCreate(lb)
    b_Pose_t pose;
    memcpy(pose.angle, angle, 4 * sizeof(float));
    pose.steps = steps;

    xTaskNotify(lb_positon_task_handle, (uint32_t)&pose, eSetValueWithOverwrite);
    vTaskDelay(5);
}

static void rb_postion_task() {
    b_Pose_t target_position;
    float step_angle[4];
    while (true) {
        // 等待数据阶段
        if (lb_positon_done) {
            while (true) {
                uint32_t ulNotificationValue;
                if (xTaskNotifyWait(0, 0, &ulNotificationValue, portMAX_DELAY)) {
                    b_Pose_t *xpPose = (b_Pose_t *)ulNotificationValue;  // 假设通知值是一个指针
                    memcpy(&target_position, xpPose, sizeof(b_Pose_t));
                    break;
                }
            }
        }

        // 计算步进角度
        step_angle[1] = (target_position.angle[1] - cali_motor_get_anlge(cm_l2)) / target_position.steps;
        step_angle[2] = (target_position.angle[2] - cali_motor_get_anlge(cm_l3)) / target_position.steps;
        step_angle[3] = (target_position.angle[3] - cali_motor_get_anlge(cm_l4)) / target_position.steps;

        // 执行步进动作
        for (uint8_t i = 0; i < target_position.steps; i++) {
            m_l_joint2->set_angle(m_l_joint2, m_l_joint2->target_angle + step_angle[1]);
            m_l_joint3->set_angle(m_l_joint3, m_l_joint3->target_angle + step_angle[2]);
            m_l_joint4->set_angle(m_l_joint4, m_l_joint4->target_angle + step_angle[3]);

            uint32_t ulNotificationValue;
            if (xTaskNotifyWait(0, 0, &ulNotificationValue, portMAX_DELAY)) {
                b_Pose_t *xpPose = (b_Pose_t *)ulNotificationValue;  // 假设通知值是一个指针
                memcpy(&target_position, xpPose, sizeof(b_Pose_t));
                break;
            }

            vTaskDelay(pdMS_TO_TICKS(10));  // 适当调整延迟时间
        }

        lb_positon_done = true;
    }
}

void rb_set_position(float angle[4], uint32_t steps) {
    // xTaskCreate(lb)
    b_Pose_t pose;
    memcpy(pose.angle, angle, 4 * sizeof(float));
    pose.steps = steps;

    xTaskNotify(rb_positon_task_handle, (uint32_t)&pose, eSetValueWithOverwrite);
    vTaskDelay(5);
}

void cm_l1_colibration() {
}

void cm_l2_colibration() {
    cm_l2->start_calibration(cm_l2);
}

void cm_l3_colibration() {
    m_l_joint4->set_mode(m_l_joint4, MOTOR_MODE_SPEED);
    m_l_joint4->set_speed(m_l_joint4, -60.0f);
    vTaskDelay(pdMS_TO_TICKS(2000));
    m_l_joint4->set_speed(m_l_joint4, 0);

    cm_l3->start_calibration(cm_l3);
}

void cm_l4_colibration() {
    C610_ResDataTypeDef *l4_priv = m_l_joint4->private_data;
    l4_priv->speed_pid.MaxOut = 1.0f;
    cm_l4->start_calibration(cm_l4);
    l4_priv->speed_pid.MaxOut = 6.0f;
}

void cm_r1_colibration() {
}

void cm_r2_colibration() {
}

void cm_r3_colibration() {
}

void cm_r4_colibration() {
}

void lb1_postion_info() {
    printf("LB1POS:%f%f%f%f", m_l_joint1->real_angle, cali_motor_get_anlge(cm_l2), cali_motor_get_anlge(cm_l3), cali_motor_get_anlge(cm_l4));
}

void elhp_init() {
    cm_l2 = C610_CaliMotor_Create(m_l_joint2, -138.0f, 60.0f);
    cm_l3 = C610_CaliMotor_Create(m_l_joint3, -59.5f, 60.0f);
    cm_l4 = C610_CaliMotor_Create(m_l_joint4, -150.0f, 120.0f);

    cm_r2 = C610_CaliMotor_Create(m_l_joint2, -138.0f, 60.0f);
    cm_r3 = C610_CaliMotor_Create(m_l_joint3, -59.5f, 60.0f);
    cm_r4 = C610_CaliMotor_Create(m_l_joint4, -150.0f, 120.0f);

    xTaskCreate(lb_postion_task, "EnginLBPos", 128, NULL, 240, &lb_positon_task_handle);
}