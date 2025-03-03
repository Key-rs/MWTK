#include "console_cfg.h"
#include "justfw_cfg.h"
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

static StreamBufferHandle_t stream_cli_input;

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
    printf("System Version: " SYS_VERSION "\n\n");
    printf(">");
}

static void CLI_MainLoop() {
    while (1) {
        char cRxedChar;
        while (xStreamBufferReceive(stream_cli_input, &cRxedChar, 1, portMAX_DELAY)) {
            /* code */
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
                        vTaskDelay(pdMS_TO_TICKS(10));  // 每次打印完都会通过延时及时处理流数据
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
            } else if (cRxedChar == 0x0C) {
                // 清屏
                printf("\033[2J\033[H");
                printf(">");
            }
        }
    }
}

void CLI_Init() {
    // xTaskCreate(Console_MainLoop)
    extern void vRegisterSampleCLICommands(void);
    vRegisterSampleCLICommands();

    xCLIMutex = xSemaphoreCreateMutex();

    INTF_StreamSharerTypedef *stream_sharer;  // 命令行的共享输入流
    stream_sharer = (INTF_StreamSharerTypedef *)Bus_SharePtr(CLI_INPUT_STREAM_NAME, sizeof(INTF_StreamSharerTypedef));

    stream_cli_input = xStreamBufferCreate(cmdMAX_INPUT_STREAM, 1);  // 命令行输入流
    stream_sharer->register_output(stream_sharer, stream_cli_input);

    xTaskCreate(CLI_MainLoop, "CLI_Task", 256, NULL, 255, NULL);

    Bus_SubscribeFromName("USB_ON", on_start);
}