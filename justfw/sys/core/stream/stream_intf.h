#ifndef __INTF_STREAM_H
#define __INTF_STREAM_H

#include "FreeRTOS.h"
//
#include <stdbool.h>
#include <stdint.h>

#include "list.h"
#include "stream_buffer.h"
#include "stream_intf.h"

// /**
//  * 共享数据流接口
//  */
// typedef struct INTF_StreamSharer {
//     void (*register_output)(struct INTF_StreamSharer *self, StreamBufferHandle_t output_stream);  // 注册输出流(这里需要注册而不是直接获得输出流实例, 主要是为了将消息发送者和消息接收方彻底解耦)
//     void (*remove_output)(struct INTF_StreamSharer *self, StreamBufferHandle_t output_stream);
//     void (*write)(struct INTF_StreamSharer *self, uint8_t *data, uint16_t len);  // 写入共享流
//     void (*destroy)(struct INTF_StreamSharer *self);                             // 销毁共享流
//     void *_private;                                                              // 内部私有数据(不允许访问)
// } INTF_StreamSharerTypedef;

// /**
//  * 注册一个共享流实例
//  * @param size 缓冲区大小
//  * @return 成功返回实例指针，失败返回 NULL
//  */
// INTF_StreamSharerTypedef *StreamSharer_Register(uint16_t size);

// /**
//  * 流数据接收监听接口
//  */
// typedef struct INTF_StreamListener {
//     StreamBufferHandle_t stream;                                 // 监听的流
//     void (*on_data_received)(struct INTF_StreamListener *self);  // 数据接收回调(需要调用方实现该方法)
//     void *context;                                               // 用户上下文数据(允许调用方存储数据)
//     void (*destroy)(struct INTF_StreamListener *self);           // 销毁监听器
// } INTF_StreamListenerTypedef;

// /**
//  * 注册一个流监听器
//  * @param stream 要监听的流
//  * @return 成功返回实例指针，失败返回 NULL
//  */
// INTF_StreamListenerTypedef *StreamListener_Register(StreamBufferHandle_t stream);

typedef struct StreamBufferDef_t StreamDef_t;
typedef StreamBufferHandle_t StreamHandle_t;

typedef struct StreamListnerDef {
    StreamHandle_t xStream;                                   // 需要监听的流
    void (*pvOnDataReceived)(struct StreamListnerDef *self);  // 请重写该函数实现当流接收到数据时的回调函数
    void (*pvOnDataEmpty)(struct StreamListenerDef *self);    // 请重写该函数实现当数据流清空时的回调函数
                                                              // 后续可以添加其他的监听事件
    void *pvContext;                                          // 允许用户自定义上下文
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
size_t xSharedStreamWrite(uint8_t *pData, size_t xBufferLenBytes);
StreamHandle_t xSharedStreamOutputCreate(SharedStream_t xSharedStream, size_t xStreamSize);

#endif