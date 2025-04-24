// #include "C620.h"
// #include "interface.h"
// INTF_Motor_HandleTypeDef* m_reload;
//
// void Drat_Motor_Init() {
//     PID_Init_Config_s angle_pid2 = {
//         .Kp = 0.0f,
//         .Ki = 0.0f,
//         .Kd = 0.0f,
//         .MaxOut = 1000.0f,
//         .DeadBand = 0.0f,
//         .Improve = PID_Integral_Limit,
//     };
//     PID_Init_Config_s speed_pid2 = {
//         .Kp = 0.04f,
//         .Ki = 0.00f,
//         .Kd = 0.0035f,
//         .MaxOut = 6.0f,
//         .DeadBand = 0.0f,
//         .Output_LPF_RC = 0.1f,
//         .Improve = PID_Integral_Limit | PID_OutputFilter,
//         .IntegralLimit = 3.0f,
//     };
//     PID_Init_Config_s torque_pid2 = {
//         .Kp = 1000.0f,
//         .Ki = 5000.0f,
//         .Kd = 0.0f,
//         .MaxOut = C620_CURRENT_MAX,
//         .DeadBand = 0.0f,
//         .Improve = PID_Integral_Limit,
//         .IntegralLimit = 500.0f,
//     };
//     C620_ConfigTypeDef config2 = {
//         .motor_id = 1,
//         //            .motor_ptr_name="/motor/shooter_left",
//         .motor_ptr_name = "/motor/shooter_HL",
//         .motor_mode = MOTOR_MODE_SPEED,
//         .direction = 1.0f,
//         .torque_feed_forward = C620_Torque2Current(1.0f),  // 未测试
//         .angle_pid_config = &angle_pid2,
//         .speed_pid_config = &speed_pid2,
//         .torque_pid_config = &torque_pid2,
//         .can_rx_topic_name = "/CAN1/RX",
//         .can_tx_topic_name = "/CAN1/TX",
//         .gear_ratio = 1.0f};
//     C620_Register(&config2);
//
//     config2.motor_id = 2;
//     //    config2.motor_ptr_name = "/motor/shooter_right";
//     config2.motor_ptr_name = "/motor/shooter_HR";
//     config2.direction = 1.0f;
//     C620_Register(&config2);
//
//     config2.motor_id = 3;
//     //    config2.motor_ptr_name = "/motor/shooter_right";
//     config2.motor_ptr_name = "/motor/shooter_TL";
//     config2.direction = -1.0f;
//     C620_Register(&config2);
//
//     config2.motor_id = 4;
//     config2.direction = -1.0f;
//     //    config2.motor_ptr_name = "/motor/shooter_right";
//     config2.motor_ptr_name = "/motor/shooter_TR";
//     C620_Register(&config2);
//
//     PID_Init_Config_s angle_pid3 = {
//         .Kp = 9500.0f,
//         .Ki = 00.0f,
//         .Kd = 1100.0f,
//         .MaxOut = 5.0f * RPM2RPS,
//         .DeadBand = 0.0f,
//         .Improve = PID_Integral_Limit,
//     };
//     PID_Init_Config_s speed_pid3 = {
//         .Kp = 0.001f,
//         .Ki = 0.00001f,
//         .Kd = 0.0001f,
//         .MaxOut = 6.0f,
//         .DeadBand = 0.0f,
//         .Output_LPF_RC = 0.1f,
//         .Improve = PID_Integral_Limit | PID_OutputFilter,
//         .IntegralLimit = 3.0f,
//     };
//     PID_Init_Config_s torque_pid3 = {
//         .Kp = 1000.0f,
//         .Ki = 5000.0f,
//         .Kd = 0.0f,
//         .MaxOut = C620_CURRENT_MAX,
//         .DeadBand = 0.0f,
//         .Improve = PID_Integral_Limit,
//         .IntegralLimit = 500.0f,
//     };
//     C620_ConfigTypeDef config3 = {
//         .motor_id = 5,
//         .motor_ptr_name = "/motor/shooter_feeder",
//         .motor_mode = MOTOR_MODE_ANGLE,
//         .direction = 1.0f,
//         .torque_feed_forward = C620_Torque2Current(1.0f),  // 未测试
//         .angle_pid_config = &angle_pid3,
//         .speed_pid_config = &speed_pid3,
//         .torque_pid_config = &torque_pid3,
//         .can_rx_topic_name = "/CAN1/RX",
//         .can_tx_topic_name = "/CAN1/TX",
//         //            .gear_ratio = 19.0f
//     };
//
//     m_reload = C620_Register(&config3);
// }