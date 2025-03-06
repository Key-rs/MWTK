#include "intf_sys.h"

// 注意顺序！
#include <stdio.h>

#include "FreeRTOS_CLI.h"
#include "console_colors.h"
#include "motor_cli.h"

#define MOTOR_PARAM_MAX_LEN 10

static BaseType_t prvMotorCommand(char* pcWriteBuffer,
                                  size_t xWriteBufferLen,
                                  const char* pcCommandString) {
    (void)pcCommandString;
    (void)xWriteBufferLen;
    configASSERT(pcWriteBuffer);

    const char* pcParameter = " ";
    char* param[MOTOR_PARAM_MAX_LEN] = {0};  // 为了分割子命令
    uint8_t param_len = 0;
    BaseType_t xParameterStringLength, xReturn;
    // static UBaseType_t uxParameterNumber = 0;

    do {
        pcParameter = FreeRTOS_CLIGetParameter(pcCommandString, param_len + 1, &xParameterStringLength);
        param[param_len] = (char*)pcParameter;
        if (pcParameter != NULL && *pcParameter != 0)
            param_len++;
        else
            break;
    } while (true);

    // 命令参数分割
    for (uint8_t i = 0; param[i] != NULL && i < param_len; i++) {
        char* pc = param[i];
        while (*pc != ' ' && *pc != 0) {
            pc++;
        }
        *pc = 0;
    }

    if (param_len < 2) {
        // 暂时没有 motor list, 未来加上
        goto err_use;
    }

    // 第一个参数为 Motor Name
    INTF_Motor_HandleTypeDef* m;
    m = Bus_SearchPtr(param[0]);
    if (m == NULL) {
        snprintf(pcWriteBuffer, xWriteBufferLen, RED "No Motor Found!\n\r" NC);
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
                "Error"};

            // 返回电机状态
            snprintf(pcWriteBuffer,
                     xWriteBufferLen,
                     "Motor ID: %lu\n\rMotor Mode: %s\n\rMotor State: %s\n\rAngle: %f\n\rSpeed: %f\n\rtorgue: %f\n\r",
                     m->motor_id,
                     motor_mode_str[m->motor_mode],
                     motor_state_str[m->motor_state],
                     m->real_angle,
                     m->real_speed,
                     m->real_torque);
        } else if (strcmp(param[1], "reset") == 0) {
            m->reset(m);
            snprintf(pcWriteBuffer, xWriteBufferLen, GREEN "Success!\n\r" NC);
        }

        return pdFALSE;
    }

    if (param_len == 4) {
        float value = strtof(param[3], NULL);

        if (strcmp(param[1], "set") == 0) {
            if (strcmp(param[2], "angle") == 0) {
                m->set_angle(m, value);
            } else if (strcmp(param[2], "speed") == 0) {
                m->set_speed(m, value);
            } else if (strcmp(param[2], "torgue") == 0) {
                m->set_torque(m, value);
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

void motor_cli_init() {
    FreeRTOS_CLIRegisterCommand(&xMotorManager);
}