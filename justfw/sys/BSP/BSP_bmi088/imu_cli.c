#include "interface.h"
//
#include <stdio.h>

#include "FreeRTOS_CLI.h"
#include "console_cfg.h"
#include "console_colors.h"
extern float INS_angle[3];

static BaseType_t prvIMUCommand(char* pcWriteBuffer,
                                size_t xWriteBufferLen,
                                const char* pcCommandString) {
    BaseType_t xParameterStringLength, xReturn;

    const char* param = FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParameterStringLength);
    if (strncmp(param, "info", 4) == 0) {
        snprintf(pcWriteBuffer, xWriteBufferLen, "IMU:%f,%f,%f\n", INS_angle[0], INS_angle[1], INS_angle[2]);
    } else if (strncmp(param, "monitor", 7) == 0) {
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

            printf(pcWriteBuffer, xWriteBufferLen, "IMU:%f,%f,%f\n", INS_angle[0], INS_angle[1], INS_angle[2]);
            vTaskDelay(pdMS_TO_TICKS(20));
        }
    } else
        snprintf(pcWriteBuffer, xWriteBufferLen, RED "Err USE!\n\r" NC);

    return pdFALSE;
}

static const CLI_Command_Definition_t xIMUManager = {
    "imu",
    "\n\n",
    prvIMUCommand,
    1};

void imu_cli_register() {
    FreeRTOS_CLIRegisterCommand(&xIMUManager);
}