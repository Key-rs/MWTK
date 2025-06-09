#include "dm_motor.h"
#include "interface.h"
#include "motor_manager_config.h"
#include "user_lib.h"
#include "cmsis_os.h"
#include "cmsis_os2.h"

static List_t motors;

struct DM_Motor_feedback_frame
{
    uint8_t id_with_error : 8;
    uint16_t POS : 16;
    uint16_t VEL : 12;
    uint16_t T : 12;
    uint8_t T_MOS : 8;
    uint8_t T_Rotor : 8;
};

struct DM_Motor_MIT_frame
{
    uint16_t P_des : 16;
    uint16_t V_des : 12;
    uint16_t Kp : 12;
    uint16_t Kd : 12;
    uint16_t T_ff : 12;
};

#define DM_P_MIN (-12.5f)
#define DM_P_MAX 12.5f
#define DM_V_MIN (-45.0f)
#define DM_V_MAX 45.0f
#define DM_KP_MIN 0.0f
#define DM_KP_MAX 500.0f
#define DM_KD_MIN 0.0f
#define DM_KD_MAX 5.0f
#define DM_T_MIN (-20.0f)
#define DM_T_MAX 20.0f

#define DM_A_MIN (-4 * 3.1415926535)
#define DM_A_MAX (4 * 3.1415926535)

#define COMMAND_DELAY_TICK 20
#define MAX_ENABLE_RETRIES 5

static void motor_clear_err(INTF_Motor_HandleTypeDef *self)
{
    DM_Motor_ResDataTypeDef *priv = self->private_data;
    INTF_CAN_MessageTypeDef msg = {
        .id_type = BSP_CAN_ID_STD,
        .can_id = self->motor_id,
    };
    for (uint8_t i = 0; i < 8; i++)
    {
        msg.data[i] = 0xFF;
    }
    msg.data[7] = 0xFB;
    vBusPublish(priv->can_tx_tp, &msg);
}

static void motor_enable(INTF_Motor_HandleTypeDef *self)
{
    self->motor_state = MOTOR_STATE_INIT;
    DM_Motor_ResDataTypeDef *priv = self->private_data;
    INTF_CAN_MessageTypeDef msg = {
        .id_type = BSP_CAN_ID_STD,
        .can_id = self->motor_id,
    };
    for (uint8_t i = 0; i < 8; i++)
    {
        msg.data[i] = 0xFF;
    }
    msg.data[7] = 0xFC;
    vBusPublish(priv->can_tx_tp, &msg);
}

static void motor_disable(INTF_Motor_HandleTypeDef *self)
{
    DM_Motor_ResDataTypeDef *priv = self->private_data;
    INTF_CAN_MessageTypeDef msg = {
        .id_type = BSP_CAN_ID_STD,
        .can_id = self->motor_id,
    };
    for (uint8_t i = 0; i < 8; i++)
    {
        msg.data[i] = 0xFF;
    }
    msg.data[7] = 0xFD;
    vBusPublish(priv->can_tx_tp, &msg);
    self->motor_state = MOTOR_STATE_DISABLE;
}

static void motor_set_zero(INTF_Motor_HandleTypeDef *self)
{
    DM_Motor_ResDataTypeDef *priv = self->private_data;
    INTF_CAN_MessageTypeDef msg = {
        .id_type = BSP_CAN_ID_STD,
        .can_id = self->motor_id,
    };
    for (uint8_t i = 0; i < 8; i++)
    {
        msg.data[i] = 0xFF;
    }
    msg.data[7] = 0xFE;
    vBusPublish(priv->can_tx_tp, &msg);
}

static void motor_send_mit(INTF_Motor_HandleTypeDef *self)
{
    DM_Motor_ResDataTypeDef *priv = self->private_data;

    float p_des = 0.0f; // 纯速度控制
    float v_des = float_constrain(self->target_speed, DM_V_MIN, DM_V_MAX);
    float kp = float_constrain(priv->kp, DM_KP_MIN, DM_KP_MAX);
    float kd = float_constrain(priv->kd, DM_KD_MIN, DM_KD_MAX);
    float t_ff = float_constrain(self->target_torque, DM_T_MIN, DM_T_MAX);

    struct DM_Motor_MIT_frame frame = {
        .P_des = float_to_uint(p_des, DM_P_MIN, DM_P_MAX, 16),
        .V_des = float_to_uint(v_des, DM_V_MIN, DM_V_MAX, 12),
        .Kp = float_to_uint(kp, DM_KP_MIN, DM_KP_MAX, 12),
        .Kd = float_to_uint(kd, DM_KD_MIN, DM_KD_MAX, 12),
        .T_ff = float_to_uint(t_ff, DM_T_MIN, DM_T_MAX, 12)};

    INTF_CAN_MessageTypeDef msg = {
        .id_type = BSP_CAN_ID_STD,
        .can_id = self->motor_id};

    msg.data[0] = (frame.P_des >> 8);
    msg.data[1] = frame.P_des;
    msg.data[2] = (frame.V_des >> 4);
    msg.data[3] = ((frame.V_des & 0xF) << 4) | (frame.Kp >> 8);
    msg.data[4] = frame.Kp;
    msg.data[5] = (frame.Kd >> 4);
    msg.data[6] = ((frame.Kd & 0xF) << 4) | (frame.T_ff >> 8);
    msg.data[7] = frame.T_ff;

    vBusPublish(priv->can_tx_tp, &msg);
}

static void motor_set_angle(INTF_Motor_HandleTypeDef *self, float angle)
{
    self->target_angle = angle;
}

static void motor_set_speed(INTF_Motor_HandleTypeDef *self, float speed)
{
    self->target_speed = speed;
    self->target_angle = 0.0f; // 确保速度控制时 P_des 清零
}

static void motor_set_torque(INTF_Motor_HandleTypeDef *self, float torque)
{
    self->target_torque = torque;
}

static void motor_can_cb(void *message, BusSubscriberHandle_t subscriber)
{
    INTF_CAN_MessageTypeDef *msg = (INTF_CAN_MessageTypeDef *)message;

    if (msg->id_type == BSP_CAN_ID_EXT)
    {
        return;
    }

    INTF_Motor_HandleTypeDef *m = (INTF_Motor_HandleTypeDef *)subscriber->context;
    DM_Motor_ResDataTypeDef *priv = m->private_data;
    uint8_t id = msg->data[0] & 0x0F;
    priv->error = msg->data[0] >> 4;

    if (m->motor_id != id)
    {
        return;
    }

    if (m->motor_state != MOTOR_STATE_DISABLE)
    {
        if (priv->error != DM_ERROR_ENABLEDM)
        {
            m->motor_state = MOTOR_STATE_ERROR;
        }
        else
        {
            m->motor_state = MOTOR_STATE_RUNNING;
        }
    }

    struct DM_Motor_feedback_frame frame;
    frame.POS = (msg->data[1] << 8) | msg->data[2];
    frame.VEL = (msg->data[3] << 4) | (msg->data[4] >> 4);
    frame.T = ((msg->data[4] & 0xF) << 8) | msg->data[5];
    m->real_angle = m->direction * uint_to_float(frame.POS, DM_P_MIN, DM_P_MAX, 16) + m->angle_offset;
    m->real_speed = m->direction * uint_to_float(frame.VEL, DM_V_MIN, DM_V_MAX, 12);
    m->real_torque = m->direction * uint_to_float(frame.T, DM_T_MIN, DM_T_MAX, 12);

    m->update_time = HAL_GetTick();
    priv->is_recieved = true;
}

static void motor_mainloop()
{
    while (true)
    {
        ListItem_t *item = listGET_HEAD_ENTRY(&motors);
        while (item != listGET_END_MARKER(&motors))
        {
            INTF_Motor_HandleTypeDef *m = listGET_LIST_ITEM_OWNER(item);
            DM_Motor_ResDataTypeDef *priv = m->private_data;
            static uint8_t enable_retries = 0;

            switch (m->motor_state)
            {
            case MOTOR_STATE_INIT:
                motor_enable(m);
                enable_retries++;
                if (enable_retries >= MAX_ENABLE_RETRIES)
                {
                    m->motor_state = MOTOR_STATE_ERROR;
                    enable_retries = 0;
                }
                osDelay(1);
                break;

            case MOTOR_STATE_RUNNING:
                priv->is_recieved = false;
                motor_send_mit(m);
                static uint8_t no_response_count = 0;
                // osDelay(10);
                for (uint8_t i = 0; i < 10; i++)
                {
                    if (priv->is_recieved)
                        break;
                    osDelay(1);
                }

                if (priv->is_recieved == false)
                {
                    no_response_count++;
                    if (no_response_count >= 5)
                    {
                        m->motor_state = MOTOR_STATE_INIT;
                        no_response_count = 0;
                        enable_retries = 0;
                    }
                }
                else
                {
                    no_response_count = 0;
                }
                break;

            case MOTOR_STATE_ERROR:
                switch (priv->error)
                {
                case DM_ERROR_DISABLE:
                case DM_ERROR_UNDERVOLTAGE:
                case DM_ERROR_MOTOR_DATA_ERROR:
                case DM_ERROR_MCU_ERROR:
                case DM_ERROR_ENABLEDM:
                case DM_ERROR_COMMUNICATION_LOST:
                    motor_clear_err(m);
                    osDelay(1);
                    m->motor_state = MOTOR_STATE_INIT;
                    enable_retries = 0;
                    break;
                case DM_ERROR_OVERVOLTAGE:
                case DM_ERROR_OVERCURRENT:
                case DM_ERROR_MOS_OVERTEMPERATURE:
                case DM_ERROR_MOTOR_OVERTEMPERATURE:
                case DM_ERROR_OVERLOAD:
                motor_disable(m);
                motor_clear_err(m);
                m->motor_state = MOTOR_STATE_INIT;
                enable_retries = 0;
                break;
                    // default:
                    //     break;
                }
                break;

            case MOTOR_STATE_DISABLE:
                motor_disable(m);
                motor_clear_err(m);
                m->motor_state = MOTOR_STATE_INIT;
                enable_retries = 0;
                osDelay(1);
                break;

            default:
            motor_disable(m);
                motor_clear_err(m);
                m->motor_state = MOTOR_STATE_INIT;
                enable_retries = 0;
                osDelay(1);
                break;
            }
            item = listGET_NEXT(item);
        }
        vTaskDelay(1);
    }
}

#ifdef DM_MOTOR_USE_MANAGER
#include <stdio.h>
static void apply_control(ITNF_ManagerdMotor_HandleTypedef *manager)
{
    INTF_Motor_HandleTypeDef *m = manager->motor;
    DM_Motor_ResDataTypeDef *priv = m->private_data;
    priv->kd = manager->mit_parms.kd;
    priv->kp = manager->mit_parms.kp;
}

static void print_info(ITNF_ManagerdMotor_HandleTypedef *manager, char *buff, uint16_t len)
{
    INTF_Motor_HandleTypeDef *m = manager->motor;
    DM_Motor_ResDataTypeDef *priv = m->private_data;
    snprintf(buff, len, "ErrorCode:%2X\n\rParms kp:%f,kd:%f\n\r", priv->error, priv->kp, priv->kd);
}
#endif

INTF_Motor_HandleTypeDef *DM_Motor_Register(DM_Motor_ConfigTypeDef *config)
{
    INTF_Motor_HandleTypeDef *motor = pvSharePtr(config->motor_ptr_name, sizeof(INTF_Motor_HandleTypeDef));
    DM_Motor_ResDataTypeDef *priv = JUST_MALLOC(sizeof(DM_Motor_ResDataTypeDef));
    motor->private_data = priv;
    motor->motor_id = config->motor_id;
    motor->motor_mode = config->motor_mode;
    motor->motor_state = MOTOR_STATE_INIT;
    motor->target_speed = 0.0f;
    motor->real_speed = 0.0f;
    motor->target_angle = 0.0f;
    motor->real_angle = 0.0f;
    motor->target_torque = 0.0f;
    motor->real_torque = 0.0f;
    motor->angle_offset = config->angle_offset;
    motor->direction = config->direction;

    motor->set_mode = NULL;
    motor->set_speed = motor_set_speed;
    motor->set_angle = motor_set_angle;
    motor->set_torque = motor_set_torque;
    motor->enable = motor_enable;
    motor->disable = motor_disable;
    motor->reset = motor_set_zero;

    priv->can_tx_tp = xBusTopicRegister(config->can_tx_topic_name);
    priv->can_rx_sb = xBusSubscribeFromName(config->can_rx_topic_name, motor_can_cb);
    priv->can_rx_sb->context = motor;

    priv->kd = float_constrain(config->kd, DM_KD_MIN, DM_KD_MAX);
    priv->kp = float_constrain(config->kp, DM_KP_MIN, DM_KP_MAX);
    priv->torque = 0.0f;
    priv->is_recieved = false;
    priv->error = DM_ERROR_DISABLE;

    ListItem_t *item = JUST_MALLOC(sizeof(ListItem_t));
    listSET_LIST_ITEM_OWNER(item, motor);
    vListInsertEnd(&motors, item);

#ifdef DM_MOTOR_USE_MANAGER
#include "motor_manager.h"
    ITNF_ManagerdMotor_HandleTypedef *managed_motor = ManagedMotor_Create(motor);
    managed_motor->motor_name = config->motor_ptr_name;
    managed_motor->ApplyControl = apply_control;
    managed_motor->MotorInfo = print_info;
#endif

    return motor;
}

void DM_Motor_Init()
{
    vListInitialise(&motors);
    DM_Motor_ConfigTypeDef config = {
        .motor_ptr_name = "DM1",
        .angle_offset = 0.0f,
        .can_tx_topic_name = "/CAN1/TX",
        .can_rx_topic_name = "/CAN1/RX",
        .motor_mode = MOTOR_MODE_MIT,
        .direction = 1.0f,
        .kd = 0.2f,
        .kp = 0.0f,
        .motor_id = 1};
    DM_Motor_Register(&config);
    xTaskCreate(motor_mainloop, "DM_Motor", 256, NULL, 200, NULL);
}