#ifndef __INTF_STREAM_H
#define __INTF_STREAM_H

#include <stdbool.h>
#include <stdint.h>

#include "list.h"
#include "stream_buffer.h"

/**
 * 共享数据流接口
 */
typedef struct INTF_StreamSharer {
    void (*register_output)(struct INTF_StreamSharer *self, StreamBufferHandle_t output_stream);  // 注册输出流(这里需要注册而不是直接获得输出流实例, 主要是为了将消息发送者和消息接收方彻底解耦)
    void (*remove_output)(struct INTF_StreamSharer *self, StreamBufferHandle_t output_stream);
    void (*write)(struct INTF_StreamSharer *self, uint8_t *data, uint16_t len, bool is_interupt);  // 写入共享流
    // StreamBufferHandle_t input_stream;                                                             // 输入流
    void (*destroy)(struct INTF_StreamSharer *self);  // 销毁共享流
    void *_private;                                   // 内部私有数据(不允许访问)
} INTF_StreamSharerTypedef;

/**
 * 注册一个共享流实例
 * @param size 缓冲区大小
 * @return 成功返回实例指针，失败返回 NULL
 */
INTF_StreamSharerTypedef *StreamSharer_Register(uint16_t size);

/**
 * 流数据接收监听接口
 */
typedef struct INTF_StreamListener {
    StreamBufferHandle_t stream;                                 // 监听的流
    void (*on_data_received)(struct INTF_StreamListener *self);  // 数据接收回调
    void *context;                                               // 用户上下文数据
    void (*destroy)(struct INTF_StreamListener *self);           // 销毁监听器
} INTF_StreamListenerTypedef;

/**
 * 注册一个流监听器
 * @param stream 要监听的流
 * @return 成功返回实例指针，失败返回 NULL
 */
INTF_StreamListenerTypedef *StreamListener_Register(StreamBufferHandle_t stream);

#endif