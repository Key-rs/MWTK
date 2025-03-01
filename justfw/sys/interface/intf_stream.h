#ifndef __INTF_STREAM_H
#define __INTF_STREAM_H

#include <stdbool.h>
#include <stdint.h>

// #include "list.h"
#include "stream_buffer.h"

// 共享数据流
typedef struct INTF_ShareStream {
    StreamBufferHandle_t *input_stream;
    bool (*send)(uint8_t *data, uint16_t len);
    bool (*register_output_stream)(StreamBufferHandle_t *output_stream);  // 用于注册输出流
} INTF_ShareStreamTypedef;

#endif