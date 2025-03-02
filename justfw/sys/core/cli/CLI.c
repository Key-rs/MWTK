#include "console_cfg.h"
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "interface.h"
#include "semphr.h"
#include "task.h"
/* Example includes. */
#include <stdio.h>

#include "FreeRTOS_CLI.h"
#include "console_colors.h"

/*
这个文件用于实现终端逻辑
*/

static SemaphoreHandle_t xCLIMutex;
static Bus_SubscriberTypeDef *topic_rx;

static uint8_t ucInputIndex = 0;
static char cInputString[cmdMAX_INPUT_SIZE];

static void on_start(void *message, Bus_TopicHandleTypeDef *topic) {
    // USB_Printf("Hello!\nJUSTOS v0.1\n>");
    // 这里展示欢迎信息
    printf(BLUE "\n");
    printf("    /$$$$$ /$$   /$$  /$$$$$$  /$$$$$$$$  /$$$$$$   /$$$$$$ \n");
    printf("   |__  $$| $$  | $$ /$$__  $$|__  $$__/ /$$__  $$ /$$__  $$\n");
    printf("      | $$| $$  | $$| $$  \\__/   | $$   | $$  \\ $$| $$  \\__/\n");
    printf("      | $$| $$  | $$|  $$$$$$    | $$   | $$  | $$|  $$$$$$ \n");
    printf(" /$$  | $$| $$  | $$ \\____  $$   | $$   | $$  | $$ \\____  $$\n");
    printf("|  $$$$$$/|  $$$$$$/|  $$$$$$/   | $$   |  $$$$$$/|  $$$$$$/\n");
    printf(" \\______/  \\______/  \\______/    |__/    \\______/  \\______/\n ");
    printf(NC);

    printf("\n");
    printf("Welecome use justos!\n");
    printf("System Version: 0.0.5\n\n");
    printf(">");
}

// 接受用户数据回调处理
static void on_rx(void *message, Bus_TopicHandleTypeDef *topic) {
    INTF_Serial_MessageTypeDef *msg = (INTF_Serial_MessageTypeDef *)message;

    for (uint16_t i = 0; i < msg->len; i++) {
        char cRxedChar = msg->data[i];
        cInputString[ucInputIndex] = cRxedChar;
        /* Just to space the output from the input. */

        if ((cRxedChar == '\n') || (cRxedChar == '\r')) {
            printf("\n");
            if (ucInputIndex == 0) {
                printf("\r>");
                continue;
            }
            if (xSemaphoreTake(xCLIMutex, portMAX_DELAY) == pdPASS) {
                BaseType_t xMoreDataToFollow;
                char *pcOutputBuffer = FreeRTOS_CLIGetOutputBuffer();
                cInputString[ucInputIndex] = '\0';
                do {
                    xMoreDataToFollow = FreeRTOS_CLIProcessCommand(
                        cInputString,
                        pcOutputBuffer,
                        configCOMMAND_INT_MAX_OUTPUT_SIZE);

                    // 将处理得到的数据发送回用户
                    printf("%s", pcOutputBuffer);
                    vTaskDelay(pdMS_TO_TICKS(10));
                } while (xMoreDataToFollow != pdFALSE);
                xSemaphoreGive(xCLIMutex);
            }
            ucInputIndex = 0;
            bzero(cInputString, cmdMAX_INPUT_SIZE);
            printf(">");
        } else if ((cRxedChar == '\b') || (cRxedChar == cmdASCII_DEL)) {
            /* Backspace was pressed.  Erase the last character in the
             * string - if any. */
            if (ucInputIndex > 0) {
                if (ucInputIndex > 0) {
                    ucInputIndex--;
                    cInputString[ucInputIndex] = '\0';
                    printf("\b \b");
                }
            }
        } else if ((cRxedChar >= ' ') && (cRxedChar <= '~')) {
            if (ucInputIndex < cmdMAX_INPUT_SIZE) {
                printf("%c", cRxedChar);
                cInputString[ucInputIndex] = cRxedChar;
                ucInputIndex++;
            }
        }
    }
}

void CLI_Init() {
    // xTaskCreate(Console_MainLoop)
    extern void vRegisterSampleCLICommands(void);
    vRegisterSampleCLICommands();

    xCLIMutex = xSemaphoreCreateMutex();

    Bus_SubscribeFromName("USB_ON", on_start);
}