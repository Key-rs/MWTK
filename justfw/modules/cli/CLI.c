#include "console_cfg.h"
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "interface.h"
#include "semphr.h"
#include "task.h"
/* Example includes. */
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
    USB_Printf(BLUE "\n");
    USB_Printf("    /$$$$$ /$$   /$$  /$$$$$$  /$$$$$$$$  /$$$$$$   /$$$$$$ \n");
    USB_Printf("   |__  $$| $$  | $$ /$$__  $$|__  $$__/ /$$__  $$ /$$__  $$\n");
    USB_Printf("      | $$| $$  | $$| $$  \\__/   | $$   | $$  \\ $$| $$  \\__/\n");
    USB_Printf("      | $$| $$  | $$|  $$$$$$    | $$   | $$  | $$|  $$$$$$ \n");
    USB_Printf(" /$$  | $$| $$  | $$ \\____  $$   | $$   | $$  | $$ \\____  $$\n");
    USB_Printf("|  $$$$$$/|  $$$$$$/|  $$$$$$/   | $$   |  $$$$$$/|  $$$$$$/\n");
    USB_Printf(" \\______/  \\______/  \\______/    |__/    \\______/  \\______/\n ");
    USB_Printf(NC);

    USB_Printf("\n");
    USB_Printf("Welecome use justos!\n");
    USB_Printf("System Version: 0.0.5\n\n");
    USB_Printf(">");
}

// 接受用户数据回调处理
static void on_rx(void *message, Bus_TopicHandleTypeDef *topic) {
    INTF_UART_MessageTypeDef *msg = (INTF_UART_MessageTypeDef *)message;

    for (uint16_t i = 0; i < msg->len; i++) {
        char cRxedChar = msg->data[i];
        cInputString[ucInputIndex] = cRxedChar;
        /* Just to space the output from the input. */

        if ((cRxedChar == '\n') || (cRxedChar == '\r')) {
            USB_Printf("\n");
            if (ucInputIndex == 0) {
                USB_Printf("\r>");
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
                    USB_Printf("%s", pcOutputBuffer);
                    vTaskDelay(pdMS_TO_TICKS(10));
                } while (xMoreDataToFollow != pdFALSE);
                xSemaphoreGive(xCLIMutex);
            }
            ucInputIndex = 0;
            bzero(cInputString, cmdMAX_INPUT_SIZE);
            USB_Printf(">");
        } else if ((cRxedChar == '\b') || (cRxedChar == cmdASCII_DEL)) {
            /* Backspace was pressed.  Erase the last character in the
             * string - if any. */
            if (ucInputIndex > 0) {
                if (ucInputIndex > 0) {
                    ucInputIndex--;
                    cInputString[ucInputIndex] = '\0';
                    USB_Printf("\b \b");
                }
            }
        } else if ((cRxedChar >= ' ') && (cRxedChar <= '~')) {
            if (ucInputIndex < cmdMAX_INPUT_SIZE) {
                USB_Printf("%c", cRxedChar);
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

    topic_rx = Bus_SubscribeFromName(CONSOLE_INPUT_TOPIC_NAME, on_rx);
    Bus_SubscribeFromName("USB_ON", on_start);
}