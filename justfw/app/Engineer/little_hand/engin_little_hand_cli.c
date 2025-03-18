#include <math.h>
#include <stdio.h>

#include "C620.h"
#include "FreeRTOS_CLI.h"
#include "cali_motor.h"
#include "console_colors.h"
#include "engin_little_hand.h"

// static CaliMotor_Handle_t cm_l3;

BaseType_t prvEnginCommand(char* pcWriteBuffer,
                           size_t xWriteBufferLen,
                           const char* pcCommandString) {
    configASSERT(pcWriteBuffer);

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
            cm_l2_colibration();
            cm_l4_colibration();
            cm_l3_colibration();
        } else
            goto err_use;
        break;

    case 2:
        if (strcmp(param[0], "calibration") == 0) {
            if (strcmp(param[1], "joint2") == 0) {
                cm_l2_colibration();
            } else if (strcmp(param[1], "joint3") == 0) {
                cm_l3_colibration();
            } else if (strcmp(param[1], "joint4") == 0) {
                cm_l4_colibration();
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
    FreeRTOS_CLIRegisterCommand(&xEnginCommandDef);
    Engin_Little_Hande_Motor_Init();
    elhp_init();
}
