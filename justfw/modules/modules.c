#include "modules.h"
#include "modules_config.h"
#include "BrushESC775.h"
#include "BrushMotorPWM.h"
#include "gripper.h"
#include "gimbal.h"
#include "MWchassis.h"
#include "MWchassisLOGIC.h"
#include "SuperBig.h"

#define USE_GM_MOTOR_DRIVER
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

#ifdef USE_DM_MOTOR_DRIVER

#endif

#ifdef USE_BrushESC775_DRIVER

#endif

    MotorManager_Init();
    DR16_Init();

    BrushPWM_Motor_Init();
    MW_Chassis_Init();
    MAILUNTOUKUANG_Logic_Init();
    SuperBig_Init();


    gripper_Init();
    // gimbal_Init();

    // Brush775Test_Init();

    // BrushESC775_Init();

    __enable_irq();
}