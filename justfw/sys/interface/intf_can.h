//
// Created by Ukua on 2023/8/12.
//

#ifndef JUSTFW_INTF_CAN_H
#define JUSTFW_INTF_CAN_H

#include <stdint.h>


typedef enum INTF_CAN_ID_Type{
    BSP_CAN_ID_STD=0,
    BSP_CAN_ID_EXT,
}INTF_CAN_ID_Type;

typedef enum INTF_CAN_RTR_Type{
    BSP_CAN_RTR_DATA=0,
    BSP_CAN_RTR_REMOTE,
}INTF_CAN_RTR_Type;


typedef struct INTF_CAN_Message{
    INTF_CAN_ID_Type id_type;//ID类型，默认为标准帧
    INTF_CAN_RTR_Type rtr_type;//RTR类型，默认为数据帧
    uint32_t can_id;
    uint8_t data[8];
}INTF_CAN_MessageTypeDef;


#endif //JUSTFW_INTF_CAN_H
