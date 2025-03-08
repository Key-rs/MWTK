#include "shared_ptr.h"

#include <string.h>

#include "list.h"

typedef struct SharedPtrDef {
    char *name;
    void *data
} SharedPtr_t;
typedef SharedPtr_t *SharedPtrHandle_t;

List_t xSharedPtrList;

void vSharedPtrInit() {
    vListInitialise(&xSharedPtrList);
}

void *pvSreachSharedPtr(char *pcName) {
    ListItem_t *pItem;
    pItem = listGET_HEAD_ENTRY(&xSharedPtrList);

    while (pItem != listGET_END_MARKER(&xSharedPtrList)) {
        SharedPtrHandle_t xSharedPtr = (SharedPtrHandle_t)listGET_LIST_ITEM_OWNER(pItem);
        if (strcmp(xSharedPtr->name, pcName) == 0) {
            return xSharedPtr->data;
        }

        pItem = listGET_NEXT(pItem);
    }

    return NULL;
}

void *pvSharePtr(const char *pcName, size_t xSize) {
    void *pvData;
    pvData = pvSreachSharedPtr(pcName);
    if (pvData != NULL)
        return pvData;

    SharedPtrHandle_t xSharedPtr = (SharedPtrHandle_t)JUST_MALLOC(sizeof(SharedPtr_t));
    xSharedPtr->name = (char *)pcName;
    xSharedPtr->data = JUST_MALLOC(xSize);

    ListItem_t *pItem = (ListItem_t *)JUST_MALLOC(sizeof(ListItem_t));
    listSET_LIST_ITEM_OWNER(pItem, xSharedPtr);
    vListInsertEnd(&xSharedPtrList, pItem);

    return xSharedPtr->data;
}

void vSharePtrStatic(const char *pcName, void *pvData) {
    ListItem_t *pItem;
    pItem = listGET_HEAD_ENTRY(&xSharedPtrList);

    SharedPtrHandle_t xSharedPtr;

    while (pItem != listGET_END_MARKER(&xSharedPtrList)) {
        xSharedPtr = (SharedPtrHandle_t)listGET_LIST_ITEM_OWNER(pItem);
        if (strcmp(xSharedPtr->name, pcName) == 0) {
            xSharedPtr->data = pvData;
            return;
        }

        pItem = listGET_NEXT(pItem);
    }

    pItem = (ListItem_t *)JUST_MALLOC(sizeof(ListItem_t));
    xSharedPtr = JUST_MALLOC(sizeof(SharedPtr_t));
    xSharedPtr->name = pcName;
    xSharedPtr->data = pvData;

    listSET_LIST_ITEM_OWNER(pItem, xSharedPtr);
    vListInsertEnd(&xSharedPtrList, pItem);
}