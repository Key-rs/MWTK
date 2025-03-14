#ifndef __TINYBUS_INTF_H
#define __TINYBUS_INTF_H

#include "FreeRTOS.h"
#include "list.h"
/* 数据类型定义 */

/**
 * @brief 话题句柄结构体
 */
typedef struct BusTopicDef {
    const char *pcName;      // 话题名称
    List_t xSubscriberList;  // 订阅者列表
    void *context;
} BusTopicDef_t;
typedef BusTopicDef_t *BusTopicHandle_t;

/**
 * @brief 订阅者结构体
 */
typedef struct BusSubscriberDef {
    void (*pvCallback)(void *, BusTopicHandle_t);  // 回调函数
    BaseType_t xEnable;                            // 订阅使能标志
    BusTopicHandle_t pxTopic;                      // 所属话题
    void *context;
} BusSubscriberDef_t;
typedef BusSubscriberDef_t *BusSubscriberHandle_t;

BusTopicHandle_t xBusTopicSearch(const char *pcTopicName);
BusTopicHandle_t xBusTopicRegister(const char *pcTopicName);
BusSubscriberHandle_t xBusSubscribe(BusTopicHandle_t pxTopic, void (*pvCallback)(void *message, BusTopicHandle_t topic));
BusSubscriberHandle_t xBusSubscribeFromName(char *cTopicName, void (*pvCallback)(void *, BusTopicHandle_t));
void vBusPublish(BusTopicHandle_t pxTopic, void *pvMessage);
void vBusPublishFromName(char *pcTopicName, void *pvMessage);

#endif