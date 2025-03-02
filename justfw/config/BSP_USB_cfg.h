#ifndef __BSP_USB_config_H__
#define __BSP_USB_config_H__

#define BSP_USB_TX_STREAM_BUFFER_SIZE 1024
#define BSP_USB_RX_STREAM_BUFFER_SIZE 128  // 因为USB存在分包限制, 所以这里只给两倍单位包数量

#define BSP_USB_RX_SHARED_STREAM_NAME "/stream/usb_rx"
#define BSP_USB_TX_STREAM_NAME "/stream/usb_tx"

// 是否开启USB TINYBUS
// #define BSP_USB_USE_BUS

//@item:USB接收话题名称
//@type:string
//@default:"USB_RX"
#define BSP_USB_RX_TOPIC_NAME "USB_RX"

//@item:USB发送话题名称
//@type:string
//@default:"USB_TX"
#define BSP_USB_TX_TOPIC_NAME "USB_TX"

#endif