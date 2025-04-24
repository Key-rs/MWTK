// #include "infantry_gimbal.h"
//
// #include <stdio.h>
//
// #include "BMI088driver.h"
// #include "GMmotors.h"
// #include "PID.h"
// #include "user_lib.h"
// //
// #include "FreeRTOS_CLI.h"
// #include "console_cfg.h"
// #include "console_colors.h"
// // #define STEP_ANGLE 0.01f
//
// extern float INS_angle[3];
// extern float INS_SUM_angle[3];
// static PIDInstance gimbal_yaw_gyro_pid;
// static PIDInstance gimbal_pitch_gyro_pid;
// extern bmi088_real_data_t bmi088_real_data;
// static BusTopicHandle_t pitch_can_tx_topic;
//
// static INTF_Gimbal_HandleTypeDef *g_gimbal;
//
// #define GIMBAL_YAW INS_SUM_angle[2]
// #define GIMBAL_YAW_SPEED bmi088_real_data.gyro[2]
// #define GIMBAL_PITCH (INS_angle[0])
//
// static INTF_Motor_HandleTypeDef *m_yaw;
// static INTF_Motor_HandleTypeDef *m_pitch;
//
// static float get_pitch_t();
//
// static bool is_ready() {
//     if (m_yaw->motor_state != MOTOR_STATE_RUNNING)
//         return false;
//     if (m_pitch->motor_state != MOTOR_STATE_RUNNING)
//         return false;
//
//     return true;
// }
//
// static void loop() {
//     while (true) {
//         switch (g_gimbal->state) {
//         case GIMBAL_INIT:
//             /* code */
//             // 等待电机上电
//             while (is_ready() != true) {
//                 vTaskDelay(100);
//                 continue;
//             }
//             while (HAL_GetTick() - g_gimbal->motor_yaw->update_time > 1000 ||
//                    HAL_GetTick() - g_gimbal->motor_pitch->update_time > 1000) {
//                 vTaskDelay(1);
//             }
//
//             vTaskDelay(5000);
//
//             switch (g_gimbal->mode) {
//             case GIMBAL_MODE_NORMAL:
//                 g_gimbal->real_pitch = g_gimbal->motor_pitch->real_angle;
//                 g_gimbal->target_yaw = g_gimbal->real_yaw;
//                 break;
//             case GIMBAL_MODE_FOLLOW_GYRO:
//                 g_gimbal->real_yaw = GIMBAL_YAW;
//                 g_gimbal->target_yaw = g_gimbal->real_yaw;
//                 break;
//
//             default:
//                 break;
//             }
//
//             if (g_gimbal->mode == GIMBAL_MODE_NORMAL) {
//                 m_yaw->set_mode(m_yaw, MOTOR_MODE_ANGLE);
//             }
//
//             g_gimbal->state = GIMBAL_RUNNING;
//             break;
//
//         case GIMBAL_RUNNING:
//             if (is_ready() != true) {
//                 g_gimbal->state = GIMBAL_INIT;
//                 continue;
//             }
//
//             switch (g_gimbal->mode) {
//             case GIMBAL_MODE_NORMAL:
//                 g_gimbal->motor_yaw->set_angle(g_gimbal->motor_yaw, g_gimbal->target_yaw);
//                 g_gimbal->real_yaw = g_gimbal->motor_yaw->real_angle;
//                 break;
//
//             case GIMBAL_MODE_FOLLOW_GYRO:
//                 g_gimbal->real_yaw = GIMBAL_YAW;
//                 g_gimbal->motor_yaw->set_speed(g_gimbal->motor_yaw, PIDCalculate(&gimbal_yaw_gyro_pid, -loop_float_constrain(g_gimbal->target_yaw - g_gimbal->real_yaw, -PI, PI), 0));
//                 break;
//             default:
//                 break;
//             }
//
//             switch (g_gimbal->mode) {
//             case GIMBAL_MODE_NORMAL:
//                 g_gimbal->real_pitch = g_gimbal->motor_pitch->real_angle;
//                 break;
//
//             case GIMBAL_MODE_FOLLOW_GYRO:
//                 g_gimbal->real_pitch = get_pitch_t();
//                 break;
//
//             default:
//                 break;
//             }
//             g_gimbal->motor_pitch->set_speed(g_gimbal->motor_pitch, PIDCalculate(&gimbal_pitch_gyro_pid, -loop_float_constrain(g_gimbal->target_pitch - g_gimbal->real_pitch, -PI, PI), 0));
//         default:
//             break;
//         }
//
//         vTaskDelay(1);
//     }
// }
//
// static void set_mode_t(INTF_Gimbal_HandleTypeDef *self, Gimbal_ModeTypeDef mode) {
//     switch (mode) {
//     case GIMBAL_MODE_NORMAL:
//         self->motor_yaw->set_mode(self->motor_yaw, MOTOR_MODE_ANGLE);
//         self->set_yaw(self, self->motor_yaw->real_angle);
//         break;
//
//     case GIMBAL_MODE_FOLLOW_GYRO:
//         self->motor_yaw->set_mode(self->motor_yaw, MOTOR_MODE_SPEED);
//         self->set_yaw(self, GIMBAL_YAW);
//         break;
//
//     default:
//         break;
//     }
//
//     self->mode = mode;
// }
//
// static void set_pitch_t(INTF_Gimbal_HandleTypeDef *self, float target_pitch) {
//     // 限制pitch轴角度
//     if (self->pitch_limit_max == self->pitch_limit_min) {  // 未设置限制
//         self->target_pitch = target_pitch;
//     } else {
//         if (target_pitch > self->pitch_limit_max) {
//             self->target_pitch = self->pitch_limit_max;
//         } else if (target_pitch < self->pitch_limit_min) {
//             self->target_pitch = self->pitch_limit_min;
//         } else {
//             self->target_pitch = target_pitch;
//         }
//     }
// }
//
// static void set_yaw_t(struct INTF_Gimbal_Handle *self, float target_yaw) {
//     // 限制yaw轴角度
//     if (self->yaw_limit_max == self->yaw_limit_min) {  // 未设置限制
//         self->target_yaw = target_yaw;
//     } else {
//         if (target_yaw > self->yaw_limit_max) {
//             self->target_yaw = self->yaw_limit_max;
//         } else if (target_yaw < self->yaw_limit_min) {
//             self->target_yaw = self->yaw_limit_min;
//         } else {
//             self->target_yaw = target_yaw;
//         }
//     }
// }
//
// static float get_pitch_t() {
//     // TODO0 注意我这里c板由于是倒置的所以加了个PI
//     return loop_float_constrain(GIMBAL_PITCH, -PI, PI);
// }
//
// static BaseType_t prvChassisManager(char *pcWriteBuffer,
//                                     size_t xWriteBufferLen,
//                                     const char *pcCommandString) {
//     BaseType_t xParameterStringLength, xReturn;
//
//     const char *param = FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParameterStringLength);
//
//     if (strncmp(param, "info", 4) == 0) {
//         snprintf(pcWriteBuffer, xWriteBufferLen, "Status:%s\nGimbal:%f,%f,%f,%f\n",
//                  g_gimbal->state == GIMBAL_INIT ? RED "Init" NC : GREEN "Running" NC,
//                  m_pitch->real_angle,
//                  m_yaw->real_angle,
//                  g_gimbal->target_pitch,
//                  g_gimbal->real_yaw);
//     } else if (strncmp(param, "monitor", 7) == 0) {
//         static StreamHandle_t console_input;
//         if (console_input == NULL) {
//             console_input = pvSreachSharedPtr(CLI_INPUT_STREAM);
//         }
//
//         if (console_input == NULL) {
//             snprintf(pcWriteBuffer, xWriteBufferLen, RED "Stream Error!\n\r" NC);
//
//             return pdFALSE;
//         }
//         char input;
//
//         while (true) {
//             switch (g_gimbal->state) {
//             case GIMBAL_INIT:
//                 printf(RED "Gimbal Not Initialise!\n" NC);
//                 vTaskDelay(100);
//                 break;
//             case GIMBAL_RUNNING:
//
//                 printf("Gimbal:%f,%f,%f,%f\n",
//                        m_pitch->real_angle,
//                        m_yaw->real_angle,
//                        g_gimbal->target_pitch,
//                        g_gimbal->real_yaw);
//                 vTaskDelay(pdMS_TO_TICKS(20));
//
//             default:
//                 break;
//             }
//             if (xStreamRead(console_input, &input, 1, 0) && input == 0x03) {
//                 return pdFALSE;
//             }
//         }
//     } else if (strncmp(param, "set", 3) == 0) {
//         const char *param = FreeRTOS_CLIGetParameter(pcCommandString, 2, &xParameterStringLength);
//         if (strncmp(param, "mode", 4) == 0) {
//             const char *param = FreeRTOS_CLIGetParameter(pcCommandString, 3, &xParameterStringLength);
//             if (strncmp(param, "gyro", 4) == 0) {
//                 g_gimbal->set_mode(g_gimbal, GIMBAL_MODE_FOLLOW_GYRO);
//             } else if (strncmp(param, "normal", 6) == 0) {
//                 g_gimbal->set_mode(g_gimbal, GIMBAL_MODE_NORMAL);
//             }
//         }
//     } else
//         snprintf(pcWriteBuffer, xWriteBufferLen, RED "Err USE!\n\r" NC);
//     return pdFALSE;
// }
//
// static const CLI_Command_Definition_t xGimbalCommand = {
//     "gimbal",
//     "\n\r",
//     prvChassisManager,
//     -1,
// };
//
// void Infantry_Gimbal_Init() {
//     FreeRTOS_CLIRegisterCommand(&xGimbalCommand);
//
//     PID_Init_Config_s angle_pid = {
//         .Kp = 8.0f,
//         .Ki = 0.0f,
//         .Kd = 0.0f,
//         .IntegralLimit = 0.09f,
//         .MaxOut = 30.0f * RPM2RPS,
//         .DeadBand = 0.0f,
//         .Improve = PID_Integral_Limit,
//     };
//     PID_Init_Config_s speed_pid = {
//         .Kp = 0.60f,
//         .Ki = 0,
//         .Kd = 0.01f,
//         .MaxOut = 6.0f,
//         .DeadBand = 0.0f,
//         .IntegralLimit = 1.0f,
//         .Output_LPF_RC = 0.1f,
//         .Improve = PID_Integral_Limit | PID_OutputFilter,
//     };
//     PID_Init_Config_s torque_pid = {
//         .Kp = 1000.0f,
//         .Ki = 5000.0f,
//         .Kd = 0.0f,
//         .MaxOut = GM6020_VOLTAGE_MAX,
//         .DeadBand = 0.00f,
//         .IntegralLimit = GM6020_VOLTAGE_MAX,
//         .Derivative_LPF_RC = 0.9f,
//         .Improve = PID_Integral_Limit,
//     };
//
//     GM6020_ConfigTypeDef config = {
//         .motor_id = 1,  // 1
//         .motor_ptr_name = "/motor/gimbal_yaw",
//         .motor_mode = MOTOR_MODE_SPEED,
//         .direction = 1.0f,
//         .angle_offset = 1.51f,  // 对齐正前方
//         .torque_feed_forward = 8000.0f,
//         .angle_pid_config = &angle_pid,
//         .speed_pid_config = &speed_pid,
//         .torque_pid_config = &torque_pid,
//         .can_rx_topic_name = "/CAN1/RX",
//         .can_tx_topic_name = "/CAN1/TX",
//     };
//     m_yaw = GM6020_Register(&config);
//
//     PID_Init_Config_s speed_pid2 = {
//         .Kp = 2.00f,
//         .Ki = 0.00f,
//         .Kd = 0.00f,
//         .MaxOut = 6.0f,
//         .DeadBand = 0.0f,
//         .IntegralLimit = 1.0f,
//         .Output_LPF_RC = 0.1f,
//         .Improve = PID_Integral_Limit | PID_OutputFilter,
//     };
//     PID_Init_Config_s angle_pid2 = {
//         .Kp = 5.0f,
//         .Ki = 0.0f,
//         .Kd = 0.0f,
//         .IntegralLimit = 0.09f,
//         .MaxOut = 30.0f * RPM2RPS,
//         .DeadBand = 0.0f,
//         .Improve = PID_Integral_Limit,
//     };
//     GM6020_ConfigTypeDef config_pitch = {
//         .motor_id = 3,  // 1
//         .motor_ptr_name = "/motor/gimbal_pitch",
//         .motor_mode = MOTOR_MODE_SPEED,
//         .direction = -1.0f,
//         .angle_offset = 3.2f,  // 对齐正前方
//         .torque_feed_forward = 8000.0f,
//         .angle_pid_config = &angle_pid2,
//         .speed_pid_config = &speed_pid2,
//         .torque_pid_config = &torque_pid,
//         .can_rx_topic_name = "/CAN1/RX",
//         .can_tx_topic_name = "/CAN1/TX",
//     };
//     m_pitch = GM6020_Register(&config_pitch);
//
//     g_gimbal = pvSharePtr("Gimbal", sizeof(INTF_Gimbal_HandleTypeDef));
//     g_gimbal->mode = GIMBAL_MODE_NORMAL;
//
//     PID_Init_Config_s gimbal_yaw_angle_config = {
//         .Kp = 20.0f,
//         .Ki = 120.0f,
//         .Kd = 4.0f,
//         .CoefB = 0.05f,
//         .CoefA = 0.05f,
//         .IntegralLimit = 200.0f,
//         .MaxOut = 30.0f * RPM2RPS,
//         .Improve = PID_Integral_Limit | PID_Derivative_On_Measurement | PID_ChangingIntegrationRate,
//     };
//     PIDInit(&gimbal_yaw_gyro_pid, &gimbal_yaw_angle_config);
//
//     PID_Init_Config_s gimbal_pitch_config = {
//         .Kp = 5.0f,
//         .Ki = 0.0f,
//         .Kd = 0.1f,
//         .MaxOut = 5.0f * RPM2RPS,
//         .DeadBand = 0.0f,
//         .Improve = PID_Integral_Limit,
//         .IntegralLimit = 5.0f,
//     };
//     PIDInit(&gimbal_pitch_gyro_pid, &gimbal_pitch_config);
//
//     g_gimbal = pvSharePtr("gimbal", sizeof(INTF_Gimbal_HandleTypeDef));
//     g_gimbal->mode = GIMBAL_MODE_FOLLOW_GYRO;
//     g_gimbal->motor_pitch = m_pitch;
//     g_gimbal->motor_yaw = m_yaw;
//
//     g_gimbal->target_yaw = 0;
//     g_gimbal->target_pitch = 0;
//
//     g_gimbal->set_mode = set_mode_t;
//     g_gimbal->set_pitch = set_pitch_t;
//     g_gimbal->set_yaw = set_yaw_t;
//
//     g_gimbal->pitch_limit_max = 0.15f;
//     g_gimbal->pitch_limit_min = -0.4f;
//     g_gimbal->yaw_limit_max = 0;
//     g_gimbal->yaw_limit_min = 0;
//
//     xTaskCreate(loop, "GimbalTask", 256, NULL, 1, NULL);
// }