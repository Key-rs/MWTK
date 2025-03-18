#include "cali_motor.h"

#include <stdio.h>

#define STEP_ANGLE 0.02f
#define STEP_TICK 1

static void C610_Run_Calibration_t(CaliMotor_Handle_t self) {
    while (self->handle->motor_state != MOTOR_STATE_RUNNING) {
        printf("Motor didn't Work yet!\n\r");
        vTaskDelay(pdMS_TO_TICKS(200));
    }

    self->handle->set_mode(self->handle, MOTOR_MODE_SPEED);
    self->handle->set_speed(self->handle, self->cali_speed);
    float last_angle = self->handle->real_angle;  // 上一次的角度采样数据
    float sample_angle_diff[10];                  // 角度变化累计

    for (uint8_t i = 0; i < 10; i++) {
        vTaskDelay(pdMS_TO_TICKS(100));
        sample_angle_diff[i] = fabs(self->handle->real_angle - last_angle);
        last_angle = self->handle->real_angle;
    }

    for (uint8_t i = 0;; i++) {
        vTaskDelay(pdMS_TO_TICKS(100));
        float diff = fabs(self->handle->real_angle - last_angle);
        sample_angle_diff[i % 10] = diff;
        last_angle = self->handle->real_angle;
        float cumulative_angle = 0;
        for (uint8_t j = 0; j < 10; j++) {
            cumulative_angle += sample_angle_diff[j];
        }

        printf("ACD:%f,%f,%f\n\r", last_angle, cumulative_angle, diff);

        if (cumulative_angle < 0.1) {
            printf("Calibartion Success!\n\r");
            break;
        }
    }
    self->status = CALIMOTOR_YES;
    self->cali_angle = self->handle->real_angle;
    self->_offset_angle = self->handle->real_angle + self->offset_angle;

    printf("Now Going To zero\n\r");

    // uint32_t lask_tick = DWT_GetCNT();
    float d_angle = self->handle->real_angle - self->_offset_angle;

    uint32_t steps_counter = d_angle / STEP_ANGLE;
    float step = STEP_ANGLE;

    if (d_angle > 0)
        step = -step;

    printf("Step:%lu", steps_counter);
    printf("Dangle:%f", d_angle);

    self->handle->set_mode(self->handle, MOTOR_MODE_ANGLE);
    self->handle->set_angle(self->handle, self->cali_angle);

    for (uint32_t i = 0; i < steps_counter; i++) {
        self->handle->set_angle(self->handle, self->handle->target_angle + step);
        printf("RTC:%f,%f,%lu\n\r", self->handle->real_angle, self->handle->target_angle, i);
        vTaskDelay(pdMS_TO_TICKS(STEP_TICK));
    }

    self->handle->set_angle(self->handle, self->_offset_angle);

    printf("Go Zero Success!\n\r");
}

CaliMotor_Handle_t C610_CaliMotor_Create(INTF_Motor_HandleTypeDef *handle, float offset_angle, float cali_speed) {
    CaliMotor_Handle_t cali_motor = JUST_MALLOC(sizeof(CaliMotor_t));
    cali_motor->handle = handle;
    cali_motor->offset_angle = offset_angle;
    cali_motor->cali_speed = cali_speed;
    cali_motor->start_calibration = C610_Run_Calibration_t;
    cali_motor->status = CALIMOTOR_NO;

    return cali_motor;
}

static void Odrive_Run_Calibration_t(CaliMotor_Handle_t self) {
}

CaliMotor_Handle_t Odrive_CaliMotor_Create(INTF_Motor_HandleTypeDef *handle, float offset_angle) {
    CaliMotor_Handle_t cali_motor = JUST_MALLOC(sizeof(CaliMotor_t));
    cali_motor->handle = handle;
    cali_motor->offset_angle = offset_angle;
}

void cali_motor_set_angle(CaliMotor_Handle_t self, float angle) {
    self->handle->set_angle(self->handle, angle + self->_offset_angle);
}

float cali_motor_get_anlge(CaliMotor_Handle_t self) {
    return self->handle->real_angle - self->_offset_angle;
}
