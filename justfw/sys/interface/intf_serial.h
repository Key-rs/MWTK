#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdint.h>

typedef struct INTF_Serial_Message {
    uint16_t len;
    uint8_t *data;
} INTF_Serial_MessageTypeDef;

#endif