#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

/* Dimensions the buffer into which input characters are placed. 存放输入字符的缓冲区的尺寸。 */
#define cmdMAX_INPUT_SIZE 50

/* Dimensions a buffer to be used by the UART driver, if the UART driver uses a
 * buffer at all. 如果UART驱动程序使用缓冲。*/
#define cmdQUEUE_LENGTH 25

/* DEL acts as a backspace. DEL作为退格键。 */
#define cmdASCII_DEL (0x7F)

/* The maximum time to wait for the mutex that guards the UART to become
 * available.  等待保护UART的互斥锁变为的最大可用时间。*/
#define cmdMAX_MUTEX_WAIT pdMS_TO_TICKS(300)

#define CONSOLE_INPUT_TOPIC_NAME "USB_RX"
#define CONSOLE_OUTPUT_TOPIC_NAME "USB_TX"