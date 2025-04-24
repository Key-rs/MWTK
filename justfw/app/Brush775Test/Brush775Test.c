// #include "cmsis_os2.h"
// #include "interface.h"
// #include "cmsis_os.h"
// #include "BrushESC775.h"
// #include "intf_motor.h"
// #include "motors/BrushESC775/BrushESC775.h"
// #include "shared_ptr_intf.h"
// static RC_ctrl_t *rc;
// static INTF_Motor_HandleTypeDef *test_motor;
// void Brush775Test_Loop(){
//     uint32_t lask_dwt = DWT_GetCNT();
//     while (1) {
//         float dt = DWT_GetDeltaT(&lask_dwt);
//         switch (rc[0].rc.switch_left) {
//             case 1:
//                 test_motor->set_mode(test_motor,MOTOR_MODE_ANGLE);
//                 test_motor->target_angle=rc[0].rc.rocker_l1/660.0f * 3.14f * 19;
//                 break;
//             case 3:
//                 test_motor->set_mode(test_motor,MOTOR_MODE_SPEED);
//                 test_motor->target_speed=rc[0].rc.rocker_l1/660.0f * 200;
//
//
//
//                 break;
//             case 2:
//                 test_motor->set_mode(test_motor,MOTOR_MODE_TORQUE);
//                 test_motor->target_torque=rc[0].rc.rocker_l1/660.0f;
//                 break;
//         }
//         osDelay(1);
//     }
// }
// void Brush775Test_Init(){
//     BrushESC775_ConfigTypeDef config={
//         .motor_id=1,
//         .motor_ptr_name="Brush775Test1",
//         .motor_mode = MOTOR_MODE_ANGLE,
//         .direction = -1.0f,
//         .can_rx_topic_name = "/CAN1/RX",
//         .can_tx_topic_name = "/CAN1/TX",
//     };
//     BrushESC775_Register(&config);
//
//     test_motor=pvSharePtr("Brush775Test1", sizeof(INTF_Motor_HandleTypeDef));
//     rc = pvSharePtr("DR16", sizeof(RC_ctrl_t));
//     xTaskCreate(Brush775Test_Loop, "Brush775Test_Loop", 256, NULL, 240, NULL);
// }