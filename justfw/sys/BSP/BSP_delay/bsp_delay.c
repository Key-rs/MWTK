#include "bsp_delay.h"

#include "main.h"

static uint8_t fac_us = 0;
static uint32_t fac_ms = 0;

void delay_init(void) {
    fac_us = SystemCoreClock / 1000000;
    fac_ms = SystemCoreClock / 1000;

    // 禁用 SysTick
    SysTick->CTRL = 0;

    // 设置重装载值
    SysTick->LOAD = 0xFFFFFF - 1;

    // 设置当前值为 0
    SysTick->VAL = 0;

    // 启用 SysTick，使用处理器时钟，并启用中断
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk;
}

void delay_us(uint16_t nus) {
    uint32_t ticks = 0;
    uint32_t told = 0;
    uint32_t tnow = 0;
    uint32_t tcnt = 0;
    uint32_t reload = 0;
    reload = SysTick->LOAD;
    ticks = nus * fac_us;
    told = SysTick->VAL;
    while (1) {
        tnow = SysTick->VAL;
        if (tnow != told) {
            if (tnow < told) {
                tcnt += told - tnow;
            } else {
                tcnt += reload - tnow + told;
            }
            told = tnow;
            if (tcnt >= ticks) {
                break;
            }
        }
    }
}

void delay_ms(uint16_t nms) {
    uint32_t ticks = 0;
    uint32_t told = 0;
    uint32_t tnow = 0;
    uint32_t tcnt = 0;
    uint32_t reload = 0;
    reload = SysTick->LOAD;
    ticks = nms * fac_ms;
    told = SysTick->VAL;
    while (1) {
        tnow = SysTick->VAL;
        if (tnow != told) {
            if (tnow < told) {
                tcnt += told - tnow;
            } else {
                tcnt += reload - tnow + told;
            }
            told = tnow;
            if (tcnt >= ticks) {
                break;
            }
        }
    }
}
