#ifndef __SHARED_PTR_INTF_H
#define __SHARED_PTR_INTF_H
#include "FreeRTOS.h"

void *pvSreachSharedPtr(char *pcName);
void *pvSharePtr(const char *pcName, size_t xSize);
void vSharePtrStatic(const char *pcName, void *pvData);

#endif