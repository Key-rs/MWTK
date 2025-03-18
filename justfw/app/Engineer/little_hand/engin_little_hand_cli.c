#include <math.h>
#include <stdio.h>

#include "C610.h"
#include "FreeRTOS_CLI.h"
#include "console_cfg.h"
#include "console_colors.h"
#include "engin_little_hand.h"
#include "enstep_motor.h"

// static CaliMotor_Handle_t cm_l3;

static void engin_c610_enstep_calibration(EnStepMotorHandle_t m, float max_calibration_out) {
    C610_ResDataTypeDef* priv = m->pim->private_data;
    float pre_max_out = priv->speed_pid.MaxOut;
    priv->speed_pid.MaxOut = max_calibration_out;
    EnStepMotor_StartCalibrate(m);
    while (m->calibrated != true) {
        vTaskDelay(10);
    }
    priv->speed_pid.MaxOut = pre_max_out;
}

BaseType_t prvEnginCommand(char* pcWriteBuffer,
                           size_t xWriteBufferLen,
                           const char* pcCommandString) {
    configASSERT(pcWriteBuffer);
    static StreamHandle_t console_input;
    if (console_input == NULL) {
        console_input = pvSreachSharedPtr(CLI_INPUT_STREAM);
    }

    const char* pcParameter = " ";
    char* param[5] = {0};  // 为了分割子命令
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

    switch (param_len) {
    case 1:
        if (strcmp(param[0], "calibration") == 0) {
        } else
            goto err_use;
        break;

    case 2:
        if (strcmp(param[0], "calibration") == 0) {
            if (strcmp(param[1], "l2") == 0) {
                engin_c610_enstep_calibration(em_l2, 1.0f);
            } else if (strcmp(param[1], "l3") == 0) {
                engin_c610_enstep_calibration(em_l3, 1.0f);
            } else if (strcmp(param[1], "l4") == 0) {
                engin_c610_enstep_calibration(em_l4, 1.0f);
            }

            // 右侧电机
            if (strcmp(param[1], "r2") == 0) {
                engin_c610_enstep_calibration(em_r2, 1.0f);
            } else if (strcmp(param[1], "r3") == 0) {
                engin_c610_enstep_calibration(em_r3, 1.0f);
            } else if (strcmp(param[1], "r4") == 0) {
                engin_c610_enstep_calibration(em_r4, 1.0f);
            } else
                goto err_use;
        } else
            goto err_use;
        break;

    default:
        goto err_use;
    }

    return pdFALSE;

err_use:
    snprintf(pcWriteBuffer, xWriteBufferLen, RED "Wrong Usage!\n\r" NC);
    return pdFALSE;
}

static CLI_Command_Definition_t xEnginCommandDef = {
    "engin", /* The command string to type. */
    "engin:\r\n Manage The engineer Robot\r\n",
    prvEnginCommand, /* The function to run. */
    -1               /* No parameters are expected. */
};

void Engin_Little_Hand_Init() {
    EnStepMotor_Init();
    FreeRTOS_CLIRegisterCommand(&xEnginCommandDef);
    Engin_Little_Hande_Motor_Init();
}
