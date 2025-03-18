#ifndef __ENGIN_b_H
#define __ENGIN_b_H
#include "cali_motor.h"

typedef struct EnginbDef {
    CaliMotor_Handle_t j1;
    CaliMotor_Handle_t j2;
    CaliMotor_Handle_t j3;
    CaliMotor_Handle_t j4;
} Enginb_t;

typedef Enginb_t *EnginHandle_t;

#endif