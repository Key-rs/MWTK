#ifndef __INTF_STREAM_H
#define __INTF_STREAM_H

#include "FreeRTOS.h"
#include "stream_buffer.h"
//
#include "list.h"

typedef struct StreamBufferDef_t StreamDef_t;
typedef StreamBufferHandle_t StreamHandle_t;

typedef struct StreamListenerDef {
    StreamHandle_t xStream;                                                // 需要监听的流
    void (*pvOnDataReceived)(struct StreamListenerDef *pxStreamListener);  // 请重写该函数实现当流接收到数据时的回调函数
    void (*pvOnDataEmpty)(struct StreamListenerDef *pxStreamListener);     // 请重写该函数实现当数据流清空时的回调函数
                                                                           // 后续可以添加其他的监听事件
    void *pvContext;                                                       // 允许用户自定义上下文
} StreamListener_t;
typedef StreamListener_t *StreamListenerHandle_t;

typedef struct SharedStreamDef {
    StreamHandle_t xInputStream;  // 需要共享的流，这里将其称为输入流
    StreamListenerHandle_t xStreamListener;
    List_t xOutputStreamList;  // 输出流列表，负责存储需要推送的目标流
} SharedStream_t;
typedef SharedStream_t *SharedStreamHandle_t;

StreamHandle_t xStreamCreate(size_t xStreamSize);
size_t xStreamWrite(StreamHandle_t xStream, uint8_t *pData, size_t xBufferLenBytes, TickType_t xTickToWait);
size_t xStreamRead(StreamHandle_t xStream, uint8_t *pData, size_t xBufferLenBytes, TickType_t xTickToWait);
StreamListenerHandle_t xStreamListenerCreate(StreamHandle_t xStream);
size_t xStreamCheckAvailable(StreamHandle_t xStream);

SharedStreamHandle_t xSharedStreamCreate(uint16_t xBufferSize);
size_t xSharedStreamWrite(SharedStreamHandle_t xSharedStream, uint8_t *pData, size_t xBufferLenBytes, TickType_t xTickToWait);
StreamHandle_t xSharedStreamOutputCreate(SharedStreamHandle_t xSharedStream, size_t xStreamSize);

#endif