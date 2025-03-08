#include "tinybus.h"

#include "list.h"

// 话题列表
static List_t topic_list;

BaseType_t xBusInit(void) {
    vListInitialise(&topic_list);

    return pdTRUE;
}

BusTopicHandle_t xBusTopicSearch(const char *pcTopicName) {
    ListItem_t *item = listGET_HEAD_ENTRY(&topic_list);
    while (item != listGET_END_MARKER(&topic_list)) {
        BusTopicHandle_t topic = listGET_LIST_ITEM_OWNER(item);
        if (strcmp(topic->pcName, pcTopicName) == 0) {
            return topic;
        }
        item = listGET_NEXT(item);
    }
    return NULL;
}

BusTopicHandle_t xBusTopicRegister(const char *pcTopicName) {
    BusTopicHandle_t topic = xBusTopicSearch(pcTopicName);

    if (topic != NULL)
        return topic;

    topic = (BusTopicHandle_t)JUST_MALLOC(sizeof(BusTopicDef_t));
    topic->pcName = pcTopicName;
    vListInitialise(&topic->subscriber_list);

    ListItem_t *item = (ListItem_t *)JUST_MALLOC(sizeof(ListItem_t));
    listSET_LIST_ITEM_OWNER(item, topic);
    vListInsertEnd(&topic_list, item);

    return topic;
}

BusSubscriberHandle_t xBusSubscribe(BusTopicHandle_t xTopic, void (*pvCallback)(void *message, BusTopicHandle_t topic)) {
    BusSubscriberHandle_t subscriber = (BusSubscriberHandle_t)JUST_MALLOC(sizeof(BusTopicDef_t));
    subscriber->pxTopic = xTopic;
    subscriber->pvCallback = pvCallback;
    subscriber->xEnable = pdTRUE;  // 默认开启
    ListItem_t *item = (ListItem_t *)JUST_MALLOC(sizeof(ListItem_t));
    listSET_LIST_ITEM_OWNER(item, subscriber);
    vListInsertEnd(&(xTopic->subscriber_list), item);

    return subscriber;
}

BusSubscriberHandle_t xBusSubscribeFromName(char *cTopicName, void (*pvCallback)(void *, BusTopicHandle_t)) {
    BusTopicHandle_t xTopic = xBusTopicSearch((const char *)cTopicName);
    if (xTopic == NULL)
        return NULL;

    return xBusSubscribe(xTopic, pvCallback);
}

void vBusPublish(BusTopicHandle_t pxTopic, void *pvMessage) {
    ListItem_t *item = listGET_HEAD_ENTRY(&pxTopic->subscriber_list);

    while (item != listGET_END_MARKER(&pxTopic->subscriber_list)) {
        BusSubscriberHandle_t subscriber = listGET_LIST_ITEM_OWNER(item);
        subscriber->pvCallback(pvMessage, pxTopic);

        item = listGET_NEXT(item);
    }
}

void vBusPublishFromName(char *pcTopicName, void *pvMessage) {
    BusTopicHandle_t xTopic = xBusTopicSearch((const char *)pcTopicName);
    if (xTopic == NULL)
        return;

    vBusPublish(xTopic, pvMessage);
}