#include "modules.h"

#include "main.h"
#include "modules_config.h"

void Modules_Init() {
    __disable_irq();

#ifdef USE_STEADYWIN_MIT_DRIVER
    extern void SteadyWinMIT_Init();
    SteadyWinMIT_Init();
#endif

#ifdef USE_ODRIVE_CAN_DRIVER
    extern void Odrive_Init();
    Odrive_Init();
#endif

#ifdef USE_GM_MOTOR_DRIVER
    extern void GM_Init();
    GM_Init();
#endif

    MotorManager_Init();

    DR16_Init();

    __enable_irq();
}