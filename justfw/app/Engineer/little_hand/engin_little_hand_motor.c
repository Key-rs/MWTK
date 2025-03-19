#include "C610.h"
#include "Odrive_CAN.h"
#include "enstep_motor.h"
#include "interface.h"

INTF_Motor_HandleTypeDef *m_l_joint1;
INTF_Motor_HandleTypeDef *m_l_joint2;
INTF_Motor_HandleTypeDef *m_l_joint3;
INTF_Motor_HandleTypeDef *m_l_joint4;

INTF_Motor_HandleTypeDef *m_r_joint1;
INTF_Motor_HandleTypeDef *m_r_joint2;
INTF_Motor_HandleTypeDef *m_r_joint3;
INTF_Motor_HandleTypeDef *m_r_joint4;

EnStepMotorHandle_t em_l1;
EnStepMotorHandle_t em_l2;
EnStepMotorHandle_t em_l3;
EnStepMotorHandle_t em_l4;

EnStepMotorHandle_t em_r1;
EnStepMotorHandle_t em_r2;
EnStepMotorHandle_t em_r3;
EnStepMotorHandle_t em_r4;

void Engin_Little_Hande_Motor_Init() {
    /*====================工程小臂一========================*/

    // ==============1==================
    Odrive_CAN_ConfigTypedef config = {
        .motor_id = 1,
        .can_rx_topic_name = "/CAN2/RX",
        .can_tx_topic_name = "/CAN2/TX",
        .kp = 0.01,
        .kd = 0.001,
        .motor_name = "/motor/l_joint1"};
    m_l_joint1 = Odrive_Register(&config);

    // ==============2==================
    PID_Init_Config_s angle_pid_2 = {
        .Kp = 60.000000f,
        .Ki = 0.001000f,
        .Kd = 0.0f,
        .MaxOut = 300.0f,
        .DeadBand = 0.0f,
        .Improve = PID_Integral_Limit,
    };
    PID_Init_Config_s speed_pid_2 = {
        .Kp = 0.040000f,
        .Ki = 0.0001f,
        .Kd = 0.000060f,
        .MaxOut = 6.0f,
        .DeadBand = 0.0f,
        .IntegralLimit = 3.0f,
        .Improve = PID_Integral_Limit,
    };
    PID_Init_Config_s torque_pid_2 = {
        .Kp = 2000.0f,
        .Ki = 5000.0f,
        .Kd = 0.0f,
        .MaxOut = C610_CURRENT_MAX,
        .DeadBand = 0.0f,
        .Improve = PID_Integral_Limit,
        .IntegralLimit = 100.0f,
    };

    C610_ConfigTypeDef config2 = {
        .motor_id = 2,  // 3,
        .motor_ptr_name = "/motor/l_joint2",
        .motor_mode = MOTOR_MODE_ANGLE,
        .direction = -1.0f,
        .torque_feed_forward = C610_Torque2Current(1.0f),  // 未测试
        .angle_pid_config = &angle_pid_2,
        .speed_pid_config = &speed_pid_2,
        .torque_pid_config = &torque_pid_2,
        .can_rx_topic_name = "/CAN2/RX",
        .can_tx_topic_name = "/CAN2/TX",
    };

    // ==============3==================

    PID_Init_Config_s angle_pid_3 = {
        .Kp = 60.000000f,
        .Ki = 0.001000f,
        .Kd = 0.0f,
        .MaxOut = 300.0f,
        .DeadBand = 0.0f,
        .Improve = PID_Integral_Limit,
    };
    PID_Init_Config_s speed_pid_3 = {
        .Kp = 0.040000f,
        .Ki = 0.0001f,
        .Kd = 0.000060f,
        .MaxOut = 6.0f,
        .DeadBand = 0.0f,
        .IntegralLimit = 3.0f,
        .Improve = PID_Integral_Limit,
    };
    PID_Init_Config_s torque_pid_3 = {
        .Kp = 2000.0f,
        .Ki = 5000.0f,
        .Kd = 0.0f,
        .MaxOut = C610_CURRENT_MAX,
        .DeadBand = 0.0f,
        .Improve = PID_Integral_Limit,
        .IntegralLimit = 100.0f,
    };

    C610_ConfigTypeDef config3 = {
        .motor_id = 3,  // 3,
        .motor_ptr_name = "/motor/l_joint3",
        .motor_mode = MOTOR_MODE_ANGLE,
        .direction = -1.0f,
        .torque_feed_forward = C610_Torque2Current(1.0f),  // 未测试
        .angle_pid_config = &angle_pid_3,
        .speed_pid_config = &speed_pid_3,
        .torque_pid_config = &torque_pid_3,
        .can_rx_topic_name = "/CAN2/RX",
        .can_tx_topic_name = "/CAN2/TX",
    };

    // ==============4==================

    PID_Init_Config_s angle_pid_4 = {
        .Kp = 60.000000f,
        .Ki = 0.001000f,
        .Kd = 0.0f,
        .MaxOut = 300.0f,
        .DeadBand = 0.0f,
        .Improve = PID_Integral_Limit,
    };
    PID_Init_Config_s speed_pid_4 = {
        .Kp = 0.040000f,
        .Ki = 0.0001f,
        .Kd = 0.000060f,
        .MaxOut = 6.0f,
        .DeadBand = 0.0f,
        .IntegralLimit = 3.0f,
        .Improve = PID_Integral_Limit,
    };
    PID_Init_Config_s torque_pid_4 = {
        .Kp = 2000.0f,
        .Ki = 5000.0f,
        .Kd = 0.0f,
        .MaxOut = C610_CURRENT_MAX,
        .DeadBand = 0.0f,
        .Improve = PID_Integral_Limit,
        .IntegralLimit = 100.0f,
    };

    C610_ConfigTypeDef config4 = {
        .motor_id = 4,
        .motor_ptr_name = "/motor/l_joint4",
        .motor_mode = MOTOR_MODE_ANGLE,
        .direction = -1.0f,
        .torque_feed_forward = C610_Torque2Current(1.0f),  // 未测试
        .angle_pid_config = &angle_pid_4,
        .speed_pid_config = &speed_pid_4,
        .torque_pid_config = &torque_pid_4,
        .can_rx_topic_name = "/CAN2/RX",
        .can_tx_topic_name = "/CAN2/TX",
    };

    m_l_joint2 = C610_Register(&config2);
    m_l_joint3 = C610_Register(&config3);
    m_l_joint4 = C610_Register(&config4);

    /*====================工程小臂二========================*/

    // ==============1==================
    Odrive_CAN_ConfigTypedef config_r = {
        .motor_id = 2,  // 思泰威电机，不会影响C610
        .can_rx_topic_name = "/CAN2/RX",
        .can_tx_topic_name = "/CAN2/TX",
        .kp = 10.0f,
        .kd = 1.0f,
        .motor_name = "/motor/r_joint1"};
    m_r_joint1 = Odrive_Register(&config_r);
    m_r_joint1->disable(m_r_joint1);

    // ==============2==================
    config2.motor_ptr_name = "/motor/r_joint2";
    config2.motor_mode = MOTOR_MODE_ANGLE;
    config2.motor_id = 6;
    m_r_joint2 = C610_Register(&config2);

    // ==============3==================
    config3.motor_ptr_name = "/motor/r_joint3";
    config3.motor_id = 7;
    m_r_joint3 = C610_Register(&config3);

    // // ==============4==================
    config4.motor_ptr_name = "/motor/r_joint4";
    config4.motor_id = 8;
    m_r_joint4 = C610_Register(&config4);

    em_l1 = EnStepMotor_Create(m_l_joint1, "l1", 0, 0.001f, 0, ENSTEP_ODRIVE);
    em_l2 = EnStepMotor_Create(m_l_joint2, "l2", -138.0f, 60.0f, 0.1f, ENSTEP_C610);
    em_l3 = EnStepMotor_Create(m_l_joint3, "l3", -59.5f, 60.0f, 0.1f, ENSTEP_C610);
    em_l4 = EnStepMotor_Create(m_l_joint4, "l4", 150.0f, 60.0f, 0.1f, ENSTEP_C610);

    em_r1 = EnStepMotor_Create(m_r_joint1, "r1", 0, 0.001f, 0, ENSTEP_ODRIVE);
    em_r2 = EnStepMotor_Create(m_r_joint2, "r2", -138.0f, 60.0f, 0.1f, ENSTEP_C610);
    em_r3 = EnStepMotor_Create(m_r_joint3, "r3", -59.5f, 60.0f, 0.1f, ENSTEP_C610);
    em_r4 = EnStepMotor_Create(m_r_joint4, "r4", -150.0f, 60.0f, 0.1f, ENSTEP_C610);
}