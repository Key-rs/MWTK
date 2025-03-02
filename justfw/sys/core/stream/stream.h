#ifndef __STEAM_H
#define __STEAM_H

#include "interface.h"
#include "list.h"
/*
TinyBus 的局限性：

回调机制：TinyBus 的接收端通常需要在发送端的线程中运行回调函数，这会导致接收端逻辑与发送端线程耦合，不利于多线程环境。

中断限制：在中断中无法使用动态内存分配，而串口和 USB 的数据长度不固定，必须预先分配静态内存，增加了项目的复杂性。

MainLoop 依赖：TinyBus 需要注册 MainLoop 任务来处理消息，否则只能在中断中发送消息，这限制了灵活性。

STREAM 流的优势：

非定长数据支持：流的数据传递适合串口、USB 等非定长数据场景。

解耦接收端：接收端不需要频繁进入回调函数，可以通过缓冲区在自己的线程中处理数据。

多线程友好：流的设计可以更好地支持多线程环境。
*/

typedef struct Stream_SharerPrivateData {
    INTF_StreamListenerTypedef *listener;
    StreamBufferHandle_t input_stream;
    List_t output_stream_list;
} Stream_SharerPrivateDataTypedef;

void Stream_Init();

#endif