// #include "C620.h"
// #include "interface.h"
//
// INTF_Motor_HandleTypeDef *m_enchas_lf;
// INTF_Motor_HandleTypeDef *m_enchas_lb;
// INTF_Motor_HandleTypeDef *m_enchas_rf;
// INTF_Motor_HandleTypeDef *m_enchas_rb;
//
// // 轮子中心构成的矩形的大小 单位m
// #define Chassis_Width 0.49
// #define Chassis_Length 0.35
// // 电机的减速比
// #define Motor_DECELE_RATIO 1.0f  // 这是3508的
// // #define Motor_DECELE_RATIO 19.203f //这是3508的
//
// // 轮子半径 单位m
// #define WHEEL_R 0.0763f
//
// void Engin_Chassis_Set_Speed(float speed_x, float speed_y, float speed_w) {
//     m_enchas_rf->set_speed(m_enchas_rf, ((speed_x - speed_y + speed_w * (Chassis_Width + Chassis_Length) * 0.5f) * Motor_DECELE_RATIO / WHEEL_R));
//     m_enchas_lf->set_speed(m_enchas_lf, ((speed_x + speed_y + speed_w * (Chassis_Width + Chassis_Length) * 0.5f) * Motor_DECELE_RATIO / WHEEL_R));
//     m_enchas_lb->set_speed(m_enchas_lb, ((-speed_x + speed_y + speed_w * (Chassis_Width + Chassis_Length) * 0.5f) * Motor_DECELE_RATIO / WHEEL_R));
//     m_enchas_rb->set_speed(m_enchas_rb, ((-speed_x - speed_y + speed_w * (Chassis_Width + Chassis_Length) * 0.5f) * Motor_DECELE_RATIO / WHEEL_R));
// }
//
// void Engin_Chassis_Motor_Init() {
//     // 底盘PID
//     PID_Init_Config_s angle_pid = {
//         .Kp = 0.0f,
//         .Ki = 0.0f,
//         .Kd = 0.0f,
//         .MaxOut = 1000.0f,
//         .DeadBand = 0.0f,
//         .Improve = PID_Integral_Limit,
//     };
//     PID_Init_Config_s speed_pid = {
//         .Kp = 0.12f,
//         .Ki = 0.00f,
//         .Kd = 0.005f,
//         .MaxOut = 6.0f,
//         .DeadBand = 0.0f,
//         .Output_LPF_RC = 0.1f,
//         .Improve = PID_Integral_Limit | PID_OutputFilter,
//         .IntegralLimit = 3.0f,
//     };
//     PID_Init_Config_s torque_pid = {
//         .Kp = 1000.0f,
//         .Ki = 5000.0f,
//         .Kd = 0.0f,
//         .MaxOut = C620_CURRENT_MAX,
//         .DeadBand = 0.0f,
//         .Improve = PID_Integral_Limit,
//         .IntegralLimit = 500.0f,
//     };
//
//     C620_ConfigTypeDef config = {
//         .motor_id = 4,
//         //    .motor_ptr_name="/motor/OW_F_R",
//         .motor_ptr_name = "/motor/MW_F_R",
//         .motor_mode = MOTOR_MODE_SPEED,
//         .direction = 1.0f,
//         .torque_feed_forward = C620_Torque2Current(1.0f),  // 未测试
//         .angle_pid_config = &angle_pid,
//         .speed_pid_config = &speed_pid,
//         .torque_pid_config = &torque_pid,
//         .can_rx_topic_name = "/CAN1/RX",
//         .can_tx_topic_name = "/CAN1/TX",
//     };
//     m_enchas_rf = C620_Register(&config);
//
//     config.motor_id = 1;  // 3
//     config.motor_ptr_name = "/motor/MW_F_L";
//
//     m_enchas_lf = C620_Register(&config);
//
//     config.motor_id = 3;  // 4
//     config.motor_ptr_name = "/motor/MW_B_L";
//     m_enchas_lb = C620_Register(&config);
//
//     config.motor_id = 2;  // 2
//     config.motor_ptr_name = "/motor/MW_B_R";
//     m_enchas_rb = C620_Register(&config);
// }
