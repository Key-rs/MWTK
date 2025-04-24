// #include "enstep_motor.h"
//
// #include "FreeRTOS_CLI.h"
// #include "console_colors.h"
// #include "list.h"
//
// static List_t motors;
//
// EnStepMotorHandle_t EnStepMotor_Create(INTF_Motor_HandleTypeDef* im,
//                                        char* name,
//                                        float ret_angle,
//                                        float calibrate_speed,
//                                        float step_angle,
//                                        ENSTEP_BRAND brand) {
//     EnStepMotorHandle_t m = JUST_MALLOC(sizeof(EnStepMotorDef_t));
//     m->_ret_angle = ret_angle;
//     m->angle = 0;
//     m->_step_angle = step_angle;
//     m->_brand = brand;
//     m->calibrated = false;
//     m->name = name;
//     m->_calibrate_speed = calibrate_speed;
//
//     m->pim = im;
//     ListItem_t* item = JUST_MALLOC(sizeof(ListItem_t));
//     listSET_LIST_ITEM_OWNER(item, m);
//     vListInsertEnd(&motors, item);
//
//     return m;
// }
//
// static void C610_Calibration_t(void* param) {
//     EnStepMotorHandle_t self = (EnStepMotorHandle_t)param;
//
//     self->pim->set_mode(self->pim, MOTOR_MODE_SPEED);
//     self->pim->set_speed(self->pim, self->_calibrate_speed);
//
//     float last_angle = self->pim->real_angle;  // 上一次的角度采样数据
//     float sample_angle_diff[10];               // 角度变化累计
//
//     for (uint8_t i = 0; i < 10; i++) {
//         vTaskDelay(pdMS_TO_TICKS(100));
//         sample_angle_diff[i] = fabs(self->pim->real_angle - last_angle);
//         last_angle = self->pim->real_angle;
//     }
//
//     for (uint8_t i = 0;; i++) {
//         vTaskDelay(pdMS_TO_TICKS(100));
//         float diff = fabs(self->pim->real_angle - last_angle);
//         sample_angle_diff[i % 10] = diff;
//         last_angle = self->pim->real_angle;
//         float cumulative_angle = 0;
//         for (uint8_t j = 0; j < 10; j++) {
//             cumulative_angle += sample_angle_diff[j];
//         }
//
//         if (cumulative_angle < 0.1) {
//             break;
//         }
//     }
//
//     self->pim->set_mode(self->pim, MOTOR_MODE_ANGLE);
//     self->pim->set_angle(self->pim, self->pim->real_angle);
//
//     // 已抵达目标点
//     self->offset = self->pim->real_angle + self->_ret_angle;
//     self->calibrated = true;
//
//     EnStepMotor_SetAngle(self, 0);
//     vTaskDelete(NULL);
// }
//
// static void Odrive_Calibration_t(void* param) {
//     EnStepMotorHandle_t self = (EnStepMotorHandle_t)param;
//
//     self->calibrated = true;
//
//     vTaskDelete(NULL);
// }
//
// void EnStepMotor_StartCalibrate(EnStepMotorHandle_t self) {
//     while (self->pim->motor_state != MOTOR_STATE_RUNNING) {
//         vTaskDelay(1);
//     }
//     self->calibrated = false;
//     switch (self->_brand) {
//     case ENSTEP_C610:
//         xTaskCreate(C610_Calibration_t, NULL, 128, (void*)self, 30, NULL);
//         break;
//     case ENSTEP_ODRIVE:
//         xTaskCreate(Odrive_Calibration_t, NULL, 128, (void*)self, 30, NULL);
//         break;
//     default:
//         break;
//     }
// }
//
// void EnStepMotor_SetAngle(EnStepMotorHandle_t self, float angle) {
//     self->_achieved_angle = angle + self->offset;
//     self->angle = self->pim->real_angle - self->offset;
//     self->step_count = fabs(self->angle - angle) / self->_step_angle;
//     if (self->angle - angle > 0)
//         self->_dir = false;
//     else
//         self->_dir = true;
// }
//
// static void EnStepMotor_Loop() {
//     while (true) {
//         ListItem_t* item = listGET_HEAD_ENTRY(&motors);
//         while (item != listGET_END_MARKER(&motors)) {
//             EnStepMotorHandle_t m = listGET_LIST_ITEM_OWNER(item);
//             if (m->calibrated != true) {
//                 item = listGET_NEXT(item);
//                 continue;
//             }
//
//             m->angle = m->pim->real_angle - m->offset;
//
//             if (m->step_count > 0) {
//                 float next_angle;
//                 if (m->_dir)
//                     next_angle = m->pim->target_angle + m->_step_angle;
//                 else
//                     next_angle = m->pim->target_angle - m->_step_angle;
//
//                 m->pim->set_angle(m->pim, next_angle);
//                 m->step_count--;
//             } else {
//                 m->pim->set_angle(m->pim, m->_achieved_angle);
//             }
//
//             item = listGET_NEXT(item);
//         }
//         vTaskDelay(1);
//     }
// }
//
// #include <stdio.h>
// static BaseType_t prvMotorCommand(char* pcWriteBuffer,
//                                   size_t xWriteBufferLen,
//                                   const char* pcCommandString) {
//     configASSERT(pcWriteBuffer);
//
//     const char* pcParameter = " ";
//     char* param[5] = {0};  // 为了分割子命令
//     uint8_t param_len = 0;
//     BaseType_t xParameterStringLength, xReturn;
//     // static UBaseType_t uxParameterNumber = 0;
//
//     while (true) {
//         pcParameter = FreeRTOS_CLIGetParameter(pcCommandString, param_len + 1, &xParameterStringLength);
//         param[param_len] = (char*)pcParameter;
//         if (pcParameter != NULL && *pcParameter != 0)
//             param_len++;
//         else
//             break;
//     }
//
//     // 命令参数分割
//     for (uint8_t i = 0; param[i] != NULL && i < param_len; i++) {
//         char* pc = param[i];
//         while (*pc != ' ' && *pc != 0) {
//             pc++;
//         }
//         *pc = 0;
//     }
//
//     if (param_len == 0)
//         goto err_use;
//
//     if (param_len == 1) {
//         if (strcmp(param[0], "list") == 0) {
//             ListItem_t* item = listGET_HEAD_ENTRY(&motors);
//             size_t str_offset = 0;
//             while (item != listGET_END_MARKER(&motors)) {
//                 EnStepMotorHandle_t m = listGET_LIST_ITEM_OWNER(item);
//                 str_offset += snprintf(pcWriteBuffer + str_offset, xWriteBufferLen, "%s\n\r", m->name);
//
//                 item = listGET_NEXT(item);
//             }
//             return pdFALSE;
//         } else
//             goto err_use;
//     }
//
//     // PARAM_LEN > 1
//     ListItem_t* item = listGET_HEAD_ENTRY(&motors);
//     size_t str_offset = 0;
//     EnStepMotorHandle_t m;
//
//     while (item != listGET_END_MARKER(&motors)) {
//         m = listGET_LIST_ITEM_OWNER(item);
//         if (strcmp(m->name, param[0]) == 0) {
//             break;
//         }
//         item = listGET_NEXT(item);
//         m = NULL;
//     }
//
//     if (m == NULL) {
//         snprintf(pcWriteBuffer, xWriteBufferLen, RED "No Motor Found\n\r" NC);
//     }
//     switch (param_len) {
//     case 2:
//         if (strcmp(param[1], "info") == 0) {
//             snprintf(pcWriteBuffer,
//                      xWriteBufferLen,
//                      "%s\n\rAngle:%f\n\rOffset:%f\n\rStep:%lu\n\rAcheved:%f\n\r",
//                      m->calibrated ? "Calibrated" : "Uncalibrated",
//                      m->angle,
//                      m->offset,
//                      m->step_count,
//                      m->_achieved_angle);
//         } else if (strcmp(param[1], "calibrate") == 0) {
//             EnStepMotor_StartCalibrate(m);
//             while (m->calibrated != true) {
//                 printf("Curent Angle:%f\n\f", m->angle);
//             }
//             printf("Calibrate Succesful!\n\r");
//         }
//         break;
//     case 4:
//         float value = strtof(param[3], NULL);
//         if (strcmp(param[1], "set") == 0) {
//             if (strcmp(param[2], "angle") == 0) {
//                 EnStepMotor_SetAngle(m, value);
//             } else if (strcmp(param[2], "step") == 0) {
//                 m->_step_angle = value;
//             } else if (strcmp(param[2], "speed") == 0) {
//                 m->_calibrate_speed = value;
//             } else
//                 goto err_use;
//         }
//
//     default:
//         break;
//     }
//
//     return pdFALSE;
//
// err_use:
//     snprintf(pcWriteBuffer, xWriteBufferLen, RED "Wrong Usage!\n\r" NC);
//     return pdFALSE;
// }
//
// CLI_Command_Definition_t xEnStepMotorManager = {
//     .pcCommand = "enstep",
//     .pcHelpString = "\n\renstep:\n\r Manage Enstep.\n\r",
//     .pxCommandInterpreter = prvMotorCommand,
//     .cExpectedNumberOfParameters = -1};
//
// void EnStepMotor_Init() {
//     vListInitialise(&motors);
//     xTaskCreate(EnStepMotor_Loop, "EnStepMotor", 128, NULL, 50, NULL);
//     FreeRTOS_CLIRegisterCommand(&xEnStepMotorManager);
// }
