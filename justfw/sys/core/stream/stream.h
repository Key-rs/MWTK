#ifndef __STEAM_H
#define __STEAM_H

#include <stdbool.h>

#include "FreeRTOS.h"
#include "stream_buffer.h"
#include "task.h"

// 输出流结构体
typedef struct OutputStream {
    StreamBufferHandle_t xStreamBuffer;  // 输出流的流缓冲区
    bool bEnabled;                       // 输出流是否启用
    struct OutputStream *pxNext;
} OutputStream_t;

// 输入流结构体
typedef struct {
    StreamBufferHandle_t xStreamBuffer;  // 输入流的流缓冲区
    OutputStream_t *pxOutputStreamList;  // 输出流链表
    TaskHandle_t xManagerTaskHandle;     // 流管理任务句柄
} InputStream_t;

#endif