#ifndef __BUZZER_H
#define __BUZZER_H
#include "interface.h"

#define BUZZER_TIM htim2
#define BUZZER_TIM_CHENNEL TIM_CHANNEL_4

extern TIM_HandleTypeDef htim2;

typedef struct Node_PWM_Config {
    uint8_t precaler;
    uint16_t counter;
} Node_PWM_Config_t;

enum {
    DO_ = 0,
    DO_X_,
    RE_,
    RE_X_,
    MI_,
    FA_,
    FA_X_,
    SO_,
    SO_X_,
    LA_,
    LA_X_,
    XI_,
    DO,
    DO_X,
    RE,
    RE_X,
    MI,
    FA,
    FA_X,
    SO,
    SO_X,
    LA,
    LA_X,
    XI,
    _DO,
    _DO_X,
    _RE,
    _RE_X,
    _MI,
    _FA,
    _FA_X,
    _SO,
    _SO_X,
    _LA,
    _LA_X,
    _XI,
    ST
};

typedef struct Music_PWM {
    uint8_t *data;
    char *name;
} Music_PWM_t;

const Node_PWM_Config_t node_pwms[] = {
    {10, 64122},
    {10, 60431},
    {9, 63492},
    {9, 59829},
    {8, 63636},
    {8, 60000},
    {7, 64864},
    {7, 61224},
    {7, 57692},
    {6, 63636},
    {6, 60085},
    {6, 56680},
    {5, 64122},
    {5, 60649},
    {5, 57142},
    {5, 54019},
    {4, 63636},
    {4, 60171},
    {4, 56756},
    {4, 53571},
    {4, 50602},
    {3, 63636},
    {3, 60085},
    {3, 56680},
    {3, 53537},
    {3, 50541},
    {3, 47700},
    {3, 45016},
    {2, 63732},
    {2, 60171},
    {2, 56756},
    {2, 53571},
    {2, 50541},
    {2, 47727},
    {2, 45064},
    {2, 42510},
    {1, 1}};

#endif