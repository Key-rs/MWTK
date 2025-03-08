#include "stream.h"

#include "stream_cfg.h"

static List_t xStreamListenerList;

static void Stream_ManagerLoop();

void Stream_Init() {
    vListInitialise(&xStreamListenerList);

    xTaskCreate(
        Stream_ManagerLoop,
        "StreamManager",
        256,
        NULL,
        1,
        NULL);
}

static StreamHandle_t xPrintStream;

static void Stream_ManagerLoop() {
#include "BSP_USB_cfg.h"
#include "justfw_cfg.h"

    ListItem_t *pItem;
    xPrintStream = (StreamHandle_t)pvSreachSharedPtr(PRINT_OUTPUT_STREAM_NAME);
    while (1) {
        pItem = listGET_HEAD_ENTRY(&xStreamListenerList);

        while (pItem != listGET_END_MARKER(&xStreamListenerList)) {
            StreamListenerHandle_t listener = (StreamListenerHandle_t)listGET_LIST_ITEM_OWNER(pItem);
            uint8_t buff[FORWARD_BUFFER_SIZE];
            size_t xDataLen = xStreamCheckAvailable(listener->xStream);
            if (xDataLen > 0) {
                if (listener->pvOnDataReceived != NULL)
                    listener->pvOnDataReceived(listener);
            } else if (listener->pvOnDataEmpty != NULL)
                listener->pvOnDataEmpty(listener);

            pItem = listGET_NEXT(pItem);
        }
    }
}

int _write(int fd, char *pBuffer, int size) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (xPrintStream != NULL)
        xStreamWrite(xPrintStream, pBuffer, size, portMAX_DELAY);

    return size;
}

StreamHandle_t xStreamCreate(size_t xStreamSize) {
    StreamHandle_t stream = (StreamHandle_t)xStreamBufferCreate(xStreamSize, 1);

    return stream;
}

size_t xStreamWrite(StreamHandle_t xStream, uint8_t *pData, size_t xBufferLenBytes, TickType_t xTickToWait) {
    if (xPortIsInsideInterrupt()) {
        return xStreamBufferSendFromISR((StreamBufferHandle_t)xStream, pData, xBufferLenBytes, xTickToWait);
    } else {
        return xStreamBufferSend((StreamBufferHandle_t)xStream, pData, xBufferLenBytes, xTickToWait);
    }
}

size_t xStreamRead(StreamHandle_t xStream, uint8_t *pData, size_t xBufferLenBytes, TickType_t xTickToWait) {
    if (xPortIsInsideInterrupt()) {
        return xStreamBufferReceiveFromISR((StreamBufferHandle_t)xStream, pData, xBufferLenBytes, xTickToWait);
    } else {
        return xStreamBufferReceive((StreamBufferHandle_t)xStream, pData, xBufferLenBytes, xTickToWait);
    }
}

size_t xStreamCheckAvailable(StreamHandle_t xStream) {
    return xStreamBufferBytesAvailable((StreamBufferHandle_t)xStream);
}

StreamListenerHandle_t xStreamListenerCreate(StreamHandle_t xStream) {
    StreamListenerHandle_t listener = (StreamListenerHandle_t)JUST_MALLOC(sizeof(StreamListener_t));
    listener->xStream = xStream;

    ListItem_t *item = JUST_MALLOC(sizeof(ListItem_t));
    listSET_LIST_ITEM_OWNER(item, listener);
    vListInsertEnd(&xStreamListenerCreate, item);

    return listener;
}

static void pvSharedStreamOnData_t(StreamListenerHandle_t xStreamListener) {
    SharedStreamHandle_t xSharedStream = (SharedStreamHandle_t)xStreamListener->pvContext;
    ListItem_t *pItem = listGET_HEAD_ENTRY(&xSharedStream->xOutputStreamList);
    uint8_t xBuff[FORWARD_BUFFER_SIZE];
    size_t xRevLen = xStreamRead(xSharedStream->xInputStream, xBuff, FORWARD_BUFFER_SIZE, 0);

    while (pItem != listGET_END_MARKER(&xSharedStream->xOutputStreamList)) {
        StreamHandle_t xOutputStream = (StreamHandle_t)listGET_LIST_ITEM_OWNER(pItem);
        xStreamWrite(xOutputStream, xBuff, xRevLen, 0);
        pItem = listGET_NEXT(pItem);
    }
}

SharedStreamHandle_t xSharedStreamCreate(uint16_t xBufferSize) {
    SharedStreamHandle_t xSharedStream;
    xSharedStream->xInputStream = xStreamCreate(xBufferSize);
    vListInitialise(&xSharedStream->xOutputStreamList);
    xSharedStream->xStreamListener = xStreamListenerCreate(xSharedStream->xInputStream);
    xSharedStream->xStreamListener->pvOnDataReceived = pvSharedStreamOnData_t;
    xSharedStream->xStreamListener->pvContext = (void *)xSharedStream;
    return xSharedStream;
}

size_t xSharedStreamWrite(SharedStreamHandle_t xSharedStream, uint8_t *pData, size_t xBufferLenBytes, TickType_t xTickToWait) {
    return xStreamWrite(xSharedStream->xInputStream, pData, xBufferLenBytes, xTickToWait);
}

StreamHandle_t xSharedStreamOutputCreate(SharedStreamHandle_t xSharedStream, size_t xStreamSize) {
    StreamHandle_t xOutputStream = xStreamCreate(xStreamSize);
    ListItem_t *pItem = JUST_MALLOC(sizeof(ListItem_t));
    listSET_LIST_ITEM_OWNER(pItem, xOutputStream);
    vListInsertEnd(&xSharedStream->xOutputStreamList, pItem);
}