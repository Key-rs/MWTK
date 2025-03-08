#ifndef __SHARED_PTR_INTF_H
#define __SHARED_PTR_INTF_H

#include "intf_sys.h"

void *pvSreachSharedPtr(char *pcName);
void vSharePtr(const char *pcName, size_t xSize);
void vSharePtrStatic(const char *pcName, void *pvData);

#endif