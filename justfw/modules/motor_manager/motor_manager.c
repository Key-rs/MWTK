#include "motor_manager.h"

#include <stdio.h>

#include "FreeRTOS_CLI.h"
#include "console_cfg.h"
#include "console_colors.h"
#include "list.h"
#include "motor_manager_config.h"

#define MOTOR_PARAM_MAX_LEN 10

static List_t managed_motors_list;

static void MotorManager_CLI_Init();
static BaseType_t MotorManager_Load();  // 从Flash加载电机参数
static BaseType_t MotorManager_Save();  // 将电机参数永久性存储

void MotorManager_Init() {
    (void)MotorManager_Load;
    (void)MotorManager_Save;
    vListInitialise(&managed_motors_list);

#ifdef MOTOR_MANAGER_USE_CLI
    MotorManager_CLI_Init();
#endif
}

ITNF_ManagerdMotor_HandleTypedef* ManagedMotor_Create(INTF_Motor_HandleTypeDef* motor) {
    ITNF_ManagerdMotor_HandleTypedef* managed_motor = JUST_MALLOC(sizeof(ITNF_ManagerdMotor_HandleTypedef));
    ListItem_t* item = JUST_MALLOC(sizeof(ListItem_t));
    managed_motor->motor = motor;
    listSET_LIST_ITEM_OWNER(item, managed_motor);
    vListInsertEnd(&managed_motors_list, item);

    return managed_motor;
}

static BaseType_t MotorManager_Load() {
    return pdTRUE;
}

static BaseType_t MotorManager_Save() {
    return pdTRUE;
}

static BaseType_t prvMotorCommand(char* pcWriteBuffer,
                                  size_t xWriteBufferLen,
                                  const char* pcCommandString) {
    configASSERT(pcWriteBuffer);

    const char* pcParameter = " ";
    char* param[MOTOR_PARAM_MAX_LEN] = {0};  // 为了分割子命令
    uint8_t param_len = 0;
    BaseType_t xParameterStringLength, xReturn;
    // static UBaseType_t uxParameterNumber = 0;

    while (true) {
        pcParameter = FreeRTOS_CLIGetParameter(pcCommandString, param_len + 1, &xParameterStringLength);
        param[param_len] = (char*)pcParameter;
        if (pcParameter != NULL && *pcParameter != 0)
            param_len++;
        else
            break;
    }

    // 命令参数分割
    for (uint8_t i = 0; param[i] != NULL && i < param_len; i++) {
        char* pc = param[i];
        while (*pc != ' ' && *pc != 0) {
            pc++;
        }
        *pc = 0;
    }

    if (param_len < 2) {
        if (strcmp(param[0], "list") == 0) {
            ListItem_t* item = listGET_HEAD_ENTRY(&managed_motors_list);
            size_t str_offset = 0;
            while (item != listGET_END_MARKER(&managed_motors_list)) {
                ITNF_ManagerdMotor_HandleTypedef* m = listGET_LIST_ITEM_OWNER(item);
                str_offset += snprintf(pcWriteBuffer + str_offset, xWriteBufferLen, "%s  ", m->motor_name);

                switch (m->motor->motor_state) {
                case MOTOR_STATE_DISABLE:
                    str_offset += snprintf(pcWriteBuffer + str_offset, xWriteBufferLen, YELLOW "[DISABLED]" NC);
                    break;
                case MOTOR_STATE_RUNNING:
                    str_offset += snprintf(pcWriteBuffer + str_offset, xWriteBufferLen, GREEN "[RUNNING]" NC);
                    break;
                case MOTOR_STATE_ERROR:
                    str_offset += snprintf(pcWriteBuffer + str_offset, xWriteBufferLen, RED "[ERROR]" NC);
                    break;
                case MOTOR_STATE_STUCK:
                    str_offset += snprintf(pcWriteBuffer + str_offset, xWriteBufferLen, RED "[STUCK]" NC);
                    break;

                default:
                    break;
                }

                str_offset += snprintf(pcWriteBuffer + str_offset, xWriteBufferLen, "\n");

                item = listGET_NEXT(item);
            }
            return pdFALSE;
        } else
            goto err_use;
    }

    // 第一个参数为 Motor Name
    INTF_Motor_HandleTypeDef* m;
    ITNF_ManagerdMotor_HandleTypedef* managed_motor;
    m = pvSreachSharedPtr(param[0]);
    if (m == NULL) {
        snprintf(pcWriteBuffer, xWriteBufferLen, RED "No Motor Found!\n\r" NC);
        return pdFALSE;
    }

    // 查找
    ListItem_t* item = listGET_HEAD_ENTRY(&managed_motors_list);
    size_t str_offset = 0;
    while (item != listGET_END_MARKER(&managed_motors_list)) {
        managed_motor = listGET_LIST_ITEM_OWNER(item);
        if (managed_motor->motor == m) {
            // snprintf(pcWriteBuffer, xWriteBufferLen, RED "Motor Not Managed!\n\r" NC);
            break;
        }

        item = listGET_NEXT(item);
    }

    if (managed_motor->motor != m) {
        snprintf(pcWriteBuffer, xWriteBufferLen, RED "Motor Not Managed!\n\r" NC);
        return pdFALSE;
    }

    if (param_len == 2) {
        if (strcmp(param[1], "info") == 0) {
            const char* motor_mode_str[] = {
                "Mode_Torgue",
                "Mode_Speed",
                "Mode_Angle",
                "Mode_MIT"};

            const char* motor_state_str[] = {
                "Init",
                "Running",
                "Stuck",
                "Error",
                "Disable"};

            uint16_t offset = snprintf(pcWriteBuffer,
                                       xWriteBufferLen,
                                       "Motor ID: %lu\n\rMotor Mode: %s\n\rMotor State: %s\n\rAngle: %f\n\rSpeed: %f\n\rtorgue: %f\n\r",
                                       m->motor_id,
                                       motor_mode_str[m->motor_mode],
                                       motor_state_str[m->motor_state],
                                       m->real_angle,
                                       m->real_speed,
                                       m->real_torque);

            // 电机打印私有属性
            if (managed_motor->MotorInfo != NULL) {
                managed_motor->MotorInfo(managed_motor, pcWriteBuffer + offset, xWriteBufferLen - offset);
            }

        } else if (strcmp(param[1], "reset") == 0) {
            m->reset(m);
            snprintf(pcWriteBuffer, xWriteBufferLen, GREEN "Success!\n\r" NC);
        } else if (strcmp(param[1], "enable") == 0) {
            m->enable(m);
            snprintf(pcWriteBuffer, xWriteBufferLen, GREEN "Success!\n\r" NC);
        } else if (strcmp(param[1], "disable") == 0) {
            m->disable(m);
            snprintf(pcWriteBuffer, xWriteBufferLen, GREEN "Success!\n\r" NC);
        } else if (strcmp(param[1], "apply") == 0) {
            // if (m->motor_state == MOTOR_STATE_RUNNING) {
            //     snprintf(pcWriteBuffer, xWriteBufferLen, RED "Shutdonw the motor first!\n\r" NC);
            // } else {
            if (managed_motor->ApplyControl != NULL)
                managed_motor->ApplyControl(managed_motor);
            snprintf(pcWriteBuffer, xWriteBufferLen, GREEN "Success!\n\r" NC);
            // }
        } else if (strcmp(param[1], "monitor") == 0) {
            static StreamHandle_t console_input;
            if (console_input == NULL) {
                console_input = pvSreachSharedPtr(CLI_INPUT_STREAM);
            }

            if (console_input == NULL) {
                snprintf(pcWriteBuffer, xWriteBufferLen, RED "Stream Error!\n\r" NC);

                return pdFALSE;
            }

            while (true) {
                char input;
                if (xStreamRead(console_input, &input, 1, 0) && input == 0x03) {
                    return pdFALSE;
                }

                printf("Motor:%f,%f,%f,%f,%f,%f\r\n", m->target_angle, m->real_angle, m->target_speed, m->real_speed, m->target_torque, m->real_torque);
                vTaskDelay(pdMS_TO_TICKS(20));
            }
        } else {
            goto err_use;
        }

        return pdFALSE;
    }

    if (param_len == 4) {
        if (strcmp(param[1], "set") == 0) {
            if (strcmp(param[2], "mode") == 0) {
                if (m->motor_mode == MOTOR_MODE_MIT) {
                    snprintf(pcWriteBuffer, xWriteBufferLen, RED "Not Surport!!\n\r" NC);
                    return pdFALSE;
                }

                if (strcmp(param[3], "angle") == 0) {
                    m->set_mode(m, MOTOR_MODE_ANGLE);
                } else if (strcmp(param[3], "speed") == 0) {
                    m->set_mode(m, MOTOR_MODE_SPEED);
                } else if (strcmp(param[3], "torque") == 0) {
                    m->set_mode(m, MOTOR_MODE_TORQUE);
                } else
                    goto err_use;
            }
        }

        float value = strtof(param[3], NULL);
        if (strcmp(param[1], "set") == 0) {
            if (strcmp(param[2], "angle") == 0) {
                m->set_angle(m, value);
            } else if (strcmp(param[2], "speed") == 0) {
                m->set_speed(m, value);
            } else if (strcmp(param[2], "torgue") == 0) {
                m->set_torque(m, value);
            } else if (strcmp(param[2], "kp") == 0) {
                managed_motor->mit_parms.kp = value;
            } else if (strcmp(param[2], "kd") == 0) {
                managed_motor->mit_parms.kd = value;
            } else if (strcmp(param[2], "angle_kp") == 0) {
                managed_motor->pid_parms.angle_kp = value;
            } else if (strcmp(param[2], "angle_ki") == 0) {
                managed_motor->pid_parms.angle_ki = value;
            } else if (strcmp(param[2], "angle_kd") == 0) {
                managed_motor->pid_parms.angle_kd = value;
            } else if (strcmp(param[2], "speed_kp") == 0) {
                managed_motor->pid_parms.velocity_kp = value;
            } else if (strcmp(param[2], "speed_ki") == 0) {
                managed_motor->pid_parms.velocity_ki = value;
            } else if (strcmp(param[2], "speed_kd") == 0) {
                managed_motor->pid_parms.velocity_kd = value;
            } else {
                goto err_use;
            }
        }
        snprintf(pcWriteBuffer, xWriteBufferLen, GREEN "Success!\n\r" NC);
    }

    return pdFALSE;

err_use:
    snprintf(pcWriteBuffer, xWriteBufferLen, RED "Wrong Usage!\n\r" NC);
    return pdFALSE;
}

static const CLI_Command_Definition_t xMotorManager = {
    "motor",
    "\n\rmotor:\n\r Manage the motors. \n\r",
    prvMotorCommand,
    -1  // 输入参数不确定
};

static void MotorManager_CLI_Init() {
    FreeRTOS_CLIRegisterCommand(&xMotorManager);
}
