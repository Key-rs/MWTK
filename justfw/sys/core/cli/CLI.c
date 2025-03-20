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
static BusSubscriberHandle_t topic_rx;

static uint8_t ucInputIndex = 0;
static char cInputString[cmdMAX_INPUT_SIZE];

static StreamBufferHandle_t stream_cli_input;

const char *welecon_tips[] = {
    // "Life is Short, I use python!",
    "糖师傅糖丸了",
    "粥文洋你完了",
    "吓爆了哟！",
    "想逃？",
    "我看你!",
};

const char *welecon_tips[] = {
    // "Life is Short, I use python!",
    "糖师傅糖丸了",
    "粥文洋你完了",
    "吓爆了哟！",
    "想逃？",
    "我看你!",
    "肖学姐的沟"};

void cli_on_start() {
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
    printf("Projuect:https://gitee.com/justrm/justos\n");
    printf("System Version: " SYS_VERSION "  PlantFrom:" PLATFROM "\n");
    uint8_t welecon_index = HAL_GetTick() % (sizeof(welecon_tips) / sizeof(welecon_tips[0]));
    printf("Tpis:%s\n\n", welecon_tips[welecon_index]);
    printf(">");
}

static void CLI_MainLoop() {
    while (1) {
        char cRxedChar;
        while (xStreamBufferReceive(stream_cli_input, &cRxedChar, 1, 0)) {
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
                    bzero(pcOutputBuffer, configCOMMAND_INT_MAX_OUTPUT_SIZE);
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
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void CLI_Init() {
    // xTaskCreate(Console_MainLoop)
    extern void vRegisterSampleCLICommands(void);
    vRegisterSampleCLICommands();

    xCLIMutex = xSemaphoreCreateMutex();

    SharedStreamHandle_t stream_sharer;  // 命令行的共享输入流
    stream_sharer = (SharedStreamHandle_t)pvSharePtr(CLI_INPUT_SHARE_NAME, sizeof(SharedStream_t));

    // stream_cli_input = xStreamBufferCreate(cmdMAX_INPUT_STREAM, 1);  // 命令行输入流
    // stream_sharer->register_output(stream_sharer, stream_cli_input);
    stream_cli_input = xSharedStreamOutputCreate(stream_sharer, cmdMAX_INPUT_STREAM);

    // 对外共享命令行输入流，允许上层应用直接读取命令行输入流
    vSharePtrStatic(CLI_INPUT_STREAM, stream_cli_input);

    xTaskCreate(CLI_MainLoop, "CLI_Task", 1024, NULL, 255, NULL);

    // xBusSubscribeFromName("USB_ON", on_start);
}