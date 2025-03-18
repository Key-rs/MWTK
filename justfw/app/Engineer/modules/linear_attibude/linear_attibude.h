#ifndef __LINEAR_ATTIBUDE_H
#define __LINEAR_ATTIBUDE_H
#include "interface.h"

typedef struct Linear_AltibudeDef {
    float* current_value;
    uint8_t count;  // 线性数值数量
    uint32_t left_steps;
    float step;  // 步进值
} Linear_AltibudeDef_t;

typedef Linear_AltibudeDef_t* Linear_AltibudeHandle_t;

#endif