//
// Created by Ukua on 2023/11/5.
//

#ifndef JUSTFW_USER_LIB_H
#define JUSTFW_USER_LIB_H

/**
 ******************************************************************************
 * @file	 user_lib.h
 * @author  Wang Hongxi
 * @version V1.0.0
 * @date    2021/2/18
 * @brief
 ******************************************************************************
 * @attention
 *
 ******************************************************************************
 */
#include "cmsis_os.h"
#include "main.h"
#include "stdint.h"

#define msin(x) (arm_sin_f32(x))
#define mcos(x) (arm_cos_f32(x))

extern uint8_t GlobalDebugMode;

/* boolean type definitions */
#ifndef TRUE
#define TRUE 1 /**< boolean true  */
#endif

#ifndef FALSE
#define FALSE 0 /**< boolean fails */
#endif

/* math relevant */
/* radian coefficient */
#ifndef RADIAN_COEF
#define RADIAN_COEF 57.295779513f
#endif

/* circumference ratio */
#ifndef PI
#define PI 3.14159265354f
#endif

#define VAL_LIMIT(val, min, max)     \
    do {                             \
        if ((val) <= (min)) {        \
            (val) = (min);           \
        } else if ((val) >= (max)) { \
            (val) = (max);           \
        }                            \
    } while (0)

#define ANGLE_LIMIT_360(val, angle)     \
    do {                                \
        (val) = (angle) - (int)(angle); \
        (val) += (int)(angle) % 360;    \
    } while (0)

#define ANGLE_LIMIT_360_TO_180(val) \
    do {                            \
        if ((val) > 180)            \
            (val) -= 360;           \
    } while (0)

#define VAL_MIN(a, b) ((a) < (b) ? (a) : (b))
#define VAL_MAX(a, b) ((a) > (b) ? (a) : (b))

/**
 * @brief 返回一块干净的内??,不过仍然需要强制转??为你需要的类型
 *
 * @param size 分配大小
 * @return void*
 */
void *zero_malloc(size_t size);

// 快速开方
float Sqrt(float x);
// 绝对值限制
float abs_limit(float num, float Limit);
// 判断符号位
float sign(float value);
// 浮点死区
float float_deadband(float Value, float minValue, float maxValue);
// 限幅函数
float float_constrain(float Value, float minValue, float maxValue);
// 限幅函数
int16_t int16_constrain(int16_t Value, int16_t minValue, int16_t maxValue);
// 循环限幅函数
float loop_float_constrain(float Input, float minValue, float maxValue);
// 角度格式化为-180~180
float theta_format(float Ang);

int float_rounding(float raw);

float *Norm3d(float *v);

float NormOf3d(float *v);

void Cross3d(float *v1, float *v2, float *res);

float Dot3d(float *v1, float *v2);

float AverageFilter(float new_data, float *buf, uint8_t len);

float LowPassFilter(float new_data, float old_data, float factor);

int float_to_uint(float x, float x_min, float x_max, int bits);
float uint_to_float(uint64_t x, float x_min, float x_max, int bits);

int float_to_int(float x, float x_min, float x_max, int bits);
float int_to_float(int x, float x_min, float x_max, int bits);

void USB_Printf(const char *fmt, ...);

uint32_t getDtick(uint32_t *last_tick);

// 角度格式化为-Π ~ Π
#define rad_format(Ang) loop_float_constrain((Ang), -PI, PI)

#endif  // JUSTFW_USER_LIB_H
