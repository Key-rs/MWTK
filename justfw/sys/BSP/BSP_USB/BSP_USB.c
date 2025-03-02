#include "BSP_USB.h"

#include "BSP_USB_cfg.h"
#include "intf_sys.h"
#include "stream_buffer.h"

static INTF_StreamSharerTypedef *share_stream_usb_rx;  // USB RX 输入共享流
static INTF_StreamListenerTypedef *listener_usb_tx;    // USB TX输出流监听器
static StreamBufferHandle_t stream_usb_tx;             // USB TX输出流缓冲区
static StaticStreamBuffer_t xStreamBufferStructUsbTx;

#ifdef BSP_USB_USE_BUS
static Bus_SubscriberTypeDef *sub_usb_tx;
static Bus_TopicHandleTypeDef *top_usb_rx;

static StreamBufferHandle_t stream_usb_rx_output;  // 注册一个用于推送TinyBus的输出流
static INTF_StreamListenerTypedef *listener_stream_usb_rx_output;

static void stream_usb_rx_output_ondata(INTF_StreamListenerTypedef *listener) {
    (void)listener;
    uint8_t buffer[BSP_USB_RX_STREAM_BUFFER_SIZE];
    uint16_t len = xStreamBufferReceive(stream_usb_rx_output, buffer, BSP_USB_TX_STREAM_BUFFER_SIZE, 0);
    INTF_Serial_MessageTypeDef msg = {
        .data = buffer,
        .len = len};
    Bus_Publish(top_usb_rx, &msg);
}

static void BSP_USB_TX_CallBack(void *message, Bus_TopicHandleTypeDef *topic) {
    INTF_Serial_MessageTypeDef *msg = (INTF_Serial_MessageTypeDef *)message;
    xStreamBufferSend(stream_usb_tx, msg->data, msg->len, portMAX_DELAY);
}
#endif

// USB接收回调
void usbd_cdc_rx_callback(uint8_t *data, uint32_t len) {
    Stream_Set_ISR(true);
    share_stream_usb_rx->write(share_stream_usb_rx, data, len);
    Stream_Set_ISR(false);
}

static void on_usb_tx(INTF_StreamListenerTypedef *listener) {
    (void)listener;

    uint8_t buffer[BSP_USB_TX_STREAM_BUFFER_SIZE] = {0};
    uint16_t len = xStreamBufferReceive(stream_usb_tx, buffer, BSP_USB_TX_STREAM_BUFFER_SIZE, 0);
    extern uint8_t CDC_Transmit_FS(uint8_t *Buf, uint16_t Len);
    CDC_Transmit_FS(buffer, len);
}

void BSP_USB_Init() {
    // stream_usb_tx = Bus_SharePtr(BSP_USB_TX_STREAM_NAME, sizeof(StreamBufferHandle_t));
    // share_stream_usb_rx = Bus_SharePtr(BSP_USB_RX_SHARED_STREAM_NAME, sizeof(INTF_StreamSharerTypedef));
    stream_usb_tx = xStreamBufferCreate(BSP_USB_TX_STREAM_BUFFER_SIZE, 1);
    share_stream_usb_rx = StreamSharer_Register(BSP_USB_RX_STREAM_BUFFER_SIZE);
    listener_usb_tx = StreamListener_Register(stream_usb_tx);
    listener_usb_tx->on_data_received = on_usb_tx;
    // Bus_SharePtr(BSP_USB_TX_STREAM_NAME, sizeof(struct StreamBufferDef_t));
    Bus_SharePtrStatic(BSP_USB_TX_STREAM_NAME, stream_usb_tx);
    Bus_SharePtrStatic(BSP_USB_RX_SHARED_STREAM_NAME, share_stream_usb_rx);

#ifdef BSP_USB_USE_BUS
    // 注册一个流用来截取usb_rx 用于TinyBus推送消息
    stream_usb_rx_output = xStreamBufferCreate(BSP_USB_RX_STREAM_BUFFER_SIZE, 1);
    share_stream_usb_rx->register_output(share_stream_usb_rx, stream_usb_rx_output);

    listener_stream_usb_rx_output = StreamListener_Register(stream_usb_rx_output);
    listener_stream_usb_rx_output->on_data_received = stream_usb_rx_output_ondata;

    top_usb_rx = Bus_TopicRegister(BSP_USB_RX_TOPIC_NAME);
    sub_usb_tx = Bus_SubscribeFromName(BSP_USB_TX_TOPIC_NAME, BSP_USB_TX_CallBack);
#endif
}
