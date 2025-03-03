#include "stream.h"

#include "interface.h"
#include "list.h"
#include "stream_buffer.h"
#include "stream_cfg.h"

static void INTF_StreamSharer_Write_t(INTF_StreamSharerTypedef *self, uint8_t *data, uint16_t len) {
    if (self == NULL || data == NULL || len == 0)
        return;

    Stream_SharerPrivateDataTypedef *context = self->_private;
    if (context == NULL || context->input_stream == NULL)
        return;

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    // 假设 xStreamBufferSend 是写入流缓冲区的函数
    if (xPortIsInsideInterrupt())
        xStreamBufferSendFromISR(context->input_stream, data, len, &xHigherPriorityTaskWoken);
    else
        xStreamBufferSend(context->input_stream, data, len, portMAX_DELAY);
}

static void INTF_StreamSharer_Destory_t(INTF_StreamSharerTypedef *self) {
    Stream_SharerPrivateDataTypedef *private = self->_private;
    if (private != NULL) {
        if (private->listener != NULL)
        private->listener->destroy(private->listener);
        if (private->input_stream != NULL)
            vStreamBufferDelete(private->input_stream);
    }

    vListInitialise(&(private->output_stream_list));

    JUST_FREE(private);
    JUST_FREE(self);
}

static void StreamSharer_RegistOutput_t(INTF_StreamSharerTypedef *self, StreamBufferHandle_t output_stream) {
    Stream_SharerPrivateDataTypedef *private = (Stream_SharerPrivateDataTypedef *)self->_private;
    ListItem_t *new_item = JUST_MALLOC(sizeof(ListItem_t));
    new_item->pvOwner = output_stream;
    listSET_LIST_ITEM_VALUE(new_item, 10);  // 优先级为 10

    vListInsert(&private->output_stream_list, new_item);
}

static void StreamSharer_RemoveOutput_t(INTF_StreamSharerTypedef *self, StreamBufferHandle_t output_stream) {
    Stream_SharerPrivateDataTypedef *private = (Stream_SharerPrivateDataTypedef *)self->_private;
    // 将数据转发到所有输出流
    ListItem_t *item = listGET_HEAD_ENTRY(&private->output_stream_list);
    while (item != listGET_END_MARKER(&private->output_stream_list)) {
        StreamBufferHandle_t sb = (StreamBufferHandle_t)listGET_LIST_ITEM_OWNER(item);
        if (sb == output_stream) {
            uxListRemove(item);
        }
        break;
    }
}

void StreamSharer_onData_t(INTF_StreamListenerTypedef *listener) {
    INTF_StreamSharerTypedef *instance = (INTF_StreamSharerTypedef *)listener->context;
    Stream_SharerPrivateDataTypedef *private = (Stream_SharerPrivateDataTypedef *)instance->_private;
    // 转发输入流到输出流
    uint8_t buff[FORWARD_BUFFER_SIZE];
    uint16_t len = xStreamBufferReceive(private->input_stream, buff, FORWARD_BUFFER_SIZE, 1);
    if (len > 0) {
        // 将数据转发到所有输出流
        ListItem_t *item = listGET_HEAD_ENTRY(&private->output_stream_list);
        while (item != listGET_END_MARKER(&private->output_stream_list)) {
            StreamBufferHandle_t output_stream = (StreamBufferHandle_t)listGET_LIST_ITEM_OWNER(item);
            if (output_stream != NULL) {
                xStreamBufferSend(output_stream, buff, len, portMAX_DELAY);
            }
            item = listGET_NEXT(item);
        }
    }
}

INTF_StreamSharerTypedef *StreamSharer_Register(uint16_t size) {
    INTF_StreamSharerTypedef *instance = JUST_MALLOC(sizeof(INTF_StreamSharerTypedef));
    Stream_SharerPrivateDataTypedef *private = JUST_MALLOC(sizeof(Stream_SharerPrivateDataTypedef));
    StreamBufferHandle_t input_stream = xStreamBufferCreate(size, 1);  // 假设创建流缓冲区的函数是 xStreamBufferCreate
    INTF_StreamListenerTypedef *listener;

    if (instance == NULL || private == NULL || input_stream == NULL) {
        JUST_FREE(instance);
        JUST_FREE(private);
        vStreamBufferDelete(input_stream);

        return NULL;
    }

    // 初始化 instance
    instance->write = INTF_StreamSharer_Write_t;
    instance->register_output = StreamSharer_RegistOutput_t;
    instance->remove_output = StreamSharer_RemoveOutput_t;
    instance->destroy = INTF_StreamSharer_Destory_t;
    instance->_private = (void *)private;

    // 初始化消息监听
    listener = StreamListener_Register(input_stream);
    if (listener == NULL) {
        instance->destroy(instance);
        return NULL;
    }

    listener->context = instance;
    listener->on_data_received = StreamSharer_onData_t;  // 推流

    // 初始化 private_data
    private->listener = listener;
    vListInitialise(&private->output_stream_list);

    private->input_stream = input_stream;

    return instance;
}

static List_t stream_listener_list;  // 被监听的流

void StreamListener_Destory_t(INTF_StreamListenerTypedef *self) {
    if (self == NULL)
        return;

    ListItem_t *item = listGET_HEAD_ENTRY(&stream_listener_list);
    while (item != listGET_END_MARKER(&stream_listener_list)) {
        INTF_StreamListenerTypedef *listener = (INTF_StreamListenerTypedef *)listGET_LIST_ITEM_OWNER(item);
        if (listener == self) {
            uxListRemove(item);
            break;
        }

        item = listGET_NEXT(item);
    }

    JUST_FREE(self);
}

INTF_StreamListenerTypedef *StreamListener_Register(StreamBufferHandle_t stream) {
    INTF_StreamListenerTypedef *listener = JUST_MALLOC(sizeof(INTF_StreamListenerTypedef));
    if (listener == NULL)
        return NULL;

    listener->context = NULL;
    listener->stream = stream;
    listener->destroy = StreamListener_Destory_t;
    listener->on_data_received = StreamSharer_onData_t;

    ListItem_t *new_item = JUST_MALLOC(sizeof(ListItem_t));
    new_item->pvOwner = listener;
    listSET_LIST_ITEM_VALUE(new_item, 10);  // 优先级为 10

    vListInsert(&stream_listener_list, new_item);

    return listener;
}

static StreamBufferHandle_t stream_to_print;

/**
 * 流管理任务，用于流消息监听, 处理流回调函数
 */
static void Stream_ManagerLoop() {
#include "BSP_USB_cfg.h"
#include "justfw_cfg.h"
    stream_to_print = Bus_SharePtr(PRINT_OUTPUT_STREAM_NAME, sizeof(StreamBufferHandle_t));

    while (1) {
        ListItem_t *item = listGET_HEAD_ENTRY(&stream_listener_list);
        while (item != listGET_END_MARKER(&stream_listener_list)) {
            INTF_StreamListenerTypedef *listener = (INTF_StreamListenerTypedef *)listGET_LIST_ITEM_OWNER(item);
            if (listener->on_data_received != NULL && xStreamBufferBytesAvailable(listener->stream) > 0) {
                listener->on_data_received(listener);
            }

            item = listGET_NEXT(item);
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void Stream_Init() {
    vListInitialise(&stream_listener_list);

    xTaskCreate(
        Stream_ManagerLoop,
        "StreamManager",
        256,
        NULL,
        1,
        NULL);
}

int _write(int fd, char *pBuffer, int size) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    // 假设 xStreamBufferSend 是写入流缓冲区的函数
    if (stream_to_print == NULL)
        return size;
    if (xPortIsInsideInterrupt())
        xStreamBufferSendFromISR(stream_to_print, pBuffer, size, &xHigherPriorityTaskWoken);
    else
        xStreamBufferSend(stream_to_print, pBuffer, size, portMAX_DELAY);

    return size;
}

// /* USER CODE BEGIN PFP */
// #ifdef __GNUC__  // 串口重定向
// #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
// #else
// #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
// #endif
// PUTCHAR_PROTOTYPE {
//     BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//     if (stream_to_print == NULL)
//         return ch;

//     if (xPortIsInsideInterrupt())
//         xStreamBufferSendFromISR(stream_to_print, &ch, 1, &xHigherPriorityTaskWoken);
//     else
//         xStreamBufferSend(stream_to_print, &ch, 1, portMAX_DELAY);
//     return ch;
// }
// /* USER CODE END PFP */
