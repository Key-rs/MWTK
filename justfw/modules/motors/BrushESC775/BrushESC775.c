/*
 * @Author: Konodoki 1326898804@qq.com
 * @Date: 2025-03-28 14:25:41
 * @LastEditors: Konodoki 1326898804@qq.com
 * @LastEditTime: 2025-03-28 23:19:38
 * @FilePath: \justos\justfw\modules\motors\BrushESC775\BrushESC775.c
 * @Description: 
 * 
 * Copyright (c) 2025 by 1326898804@qq.com, All Rights Reserved. 
 */
#include "BrushESC775.h"
#include "cmsis_os2.h"
#include "intf_motor.h"
#include "user_lib.h"
#include <stdbool.h>
#include <stdint.h>
#include "motor_manager_config.h"
static List_t motors;

static void motor_set_mode(INTF_Motor_HandleTypeDef *self, INTF_Motor_ModeTypeDef mode){
    self->motor_mode = mode;
}

static void motor_set_speed(INTF_Motor_HandleTypeDef *self, float speed) {
    self->target_speed = speed;
}

static void motor_set_angle(INTF_Motor_HandleTypeDef *self, float angle) {
    self->target_angle = angle;
}

static void motor_set_torque(INTF_Motor_HandleTypeDef *self, float torque) {
    self->target_torque = torque;
}
void BrushESC775_solve_angle_speed(INTF_Motor_HandleTypeDef *m,uint8_t *data){
    union {
        struct {
          float rad;
          float rad_speed;
        } data;
        uint8_t bytes[8];
    } rad_radS;
    for (uint8_t i=0; i<8; i++) {
        rad_radS.bytes[i]=data[i];
    }
    m->real_angle=rad_radS.data.rad * m->direction;
    m->real_speed=rad_radS.data.rad_speed * m->direction;
}
void BrushESC775_solve_motor_status(INTF_Motor_HandleTypeDef *m,uint8_t *data){
    union {
        struct {
          uint16_t vm;
          uint16_t current;
          uint16_t cpu_tmp;
          uint16_t fuel;
        } data;
        uint8_t bytes[8];
    } controller_status;
    for (uint8_t i=0; i<8; i++) {
        controller_status.bytes[i]=data[i];
    }
    ((BrushESC775_ResDataTypeDef*)m->private_data)->Vm = uint_to_float(controller_status.data.vm,0,60,16);
    ((BrushESC775_ResDataTypeDef*)m->private_data)->current=uint_to_float(controller_status.data.current,-5,5,16);
    ((BrushESC775_ResDataTypeDef*)m->private_data)->cpu_tmp=uint_to_float(controller_status.data.cpu_tmp, 0, 1000, 16);
    m->real_torque=uint_to_float(controller_status.data.fuel, -1.0f, 1.0f, 16) * m->direction;
}
void BrushESC775_CAN_CallBack(void *message, BusSubscriberHandle_t subscriber) {
    INTF_CAN_MessageTypeDef *msg = (INTF_CAN_MessageTypeDef *)message;
    if (msg->id_type == BSP_CAN_ID_EXT) {
        return;
    }
    INTF_Motor_HandleTypeDef *m=(INTF_Motor_HandleTypeDef *)subscriber->context;
    if(msg->can_id!=m->motor_id+BrushESC775_CONTROLLER_CAN_P1_Base&&msg->can_id!=m->motor_id+BrushESC775_CONTROLLER_CAN_P2_Base){
        return;
    }
    if(msg->can_id==m->motor_id+BrushESC775_CONTROLLER_CAN_P1_Base){
        BrushESC775_solve_angle_speed(m,msg->data);
    }else if(msg->can_id==m->motor_id+BrushESC775_CONTROLLER_CAN_P2_Base){
        BrushESC775_solve_motor_status(m,msg->data);
    }
    // 确认电机在正常运行
    if (m->motor_state != MOTOR_STATE_RUNNING && m->motor_state != MOTOR_STATE_DISABLE) {
        m->motor_state = MOTOR_STATE_RUNNING;
    }
    m->update_time = HAL_GetTick();
}
static void apply_control(ITNF_ManagerdMotor_HandleTypedef *manager) {

}
static void print_info(ITNF_ManagerdMotor_HandleTypedef *manager, char *buff, uint16_t len) {
    INTF_Motor_HandleTypeDef *m = manager->motor;
    BrushESC775_ResDataTypeDef *priv = m->private_data;
    snprintf(buff, len, "Vm:%.4f Current:%.4f Cpu_tmp:%.4f Fuel:%.4f",priv->Vm,priv->current,priv->cpu_tmp,m->real_torque);
}


INTF_Motor_HandleTypeDef *BrushESC775_Register(BrushESC775_ConfigTypeDef *config){
    INTF_Motor_HandleTypeDef *motor = pvSharePtr(config->motor_ptr_name, sizeof(INTF_Motor_HandleTypeDef));
    motor->motor_id = config->motor_id;
    motor->motor_mode = config->motor_mode;
    motor->motor_state = MOTOR_STATE_INIT;
    motor->target_speed = 0.0f;
    motor->real_speed = 0.0f;
    motor->target_angle = 0.0f;
    motor->real_angle = 0.0f;
    motor->target_torque = 0.0f;
    motor->real_torque = 0.0f;
    motor->direction = config->direction;

    motor->angle_offset = 0;

    motor->private_data = JUST_MALLOC(sizeof(BrushESC775_ResDataTypeDef));
    memset(motor->private_data, 0, sizeof(BrushESC775_ResDataTypeDef));
    BrushESC775_ResDataTypeDef *priv = motor->private_data;
    priv->can_rx_topic = xBusSubscribeFromName(config->can_rx_topic_name, BrushESC775_CAN_CallBack);
    priv->can_tx_topic = xBusTopicRegister(config->can_tx_topic_name);
    priv->can_rx_topic->context=(void *)motor;
    motor->set_mode = motor_set_mode;
    motor->set_speed = motor_set_speed;
    motor->set_angle = motor_set_angle;
    motor->set_torque = motor_set_torque;

    ListItem_t *item = JUST_MALLOC(sizeof(ListItem_t));
    listSET_LIST_ITEM_OWNER(item, motor);
    vListInsertEnd(&motors, item);

#ifdef BRUSHESC775_USE_MANAGER
#include "motor_manager.h"
    ITNF_ManagerdMotor_HandleTypedef *managed_motor = ManagedMotor_Create(motor);
    managed_motor->motor_name = config->motor_ptr_name;
    managed_motor->ApplyControl = apply_control;
    managed_motor->MotorInfo = print_info;
#endif
}
void BrushESC775_MainLoop(){
    while (1) {
        ListItem_t *item = listGET_HEAD_ENTRY(&motors);
        while (item != listGET_END_MARKER(&motors)) {
            vTaskDelay(pdMS_TO_TICKS(20));

            INTF_Motor_HandleTypeDef *m = listGET_LIST_ITEM_OWNER(item);
            BrushESC775_ResDataTypeDef *priv = m->private_data;

            if (m->motor_state != MOTOR_STATE_DISABLE) {
                union {
                    struct {
                      float target;
                      uint8_t reserve1;
                      uint8_t reserve2;
                      uint8_t reserve3;
                      uint8_t control_mode;
                    }data;
                    uint8_t bytes[8];
                }Can_controller_frame;
                switch (m->motor_mode) {
                    case MOTOR_MODE_ANGLE:
                        Can_controller_frame.data.control_mode=0;
                        Can_controller_frame.data.target=m->target_angle * m->direction;
                        break;
                    case MOTOR_MODE_SPEED:
                        Can_controller_frame.data.control_mode=1;
                        Can_controller_frame.data.target=m->target_speed * m->direction;
                        break;
                    case MOTOR_MODE_TORQUE:
                        Can_controller_frame.data.control_mode=2;
                        Can_controller_frame.data.target=m->target_torque * m->direction;
                        break;
                }
                INTF_CAN_MessageTypeDef msg;
                msg.can_id = m->motor_id + BrushESC775_CONTROLLER_CAN_RECEIVE_Base;
                msg.id_type = CAN_ID_STD;
                msg.rtr_type = CAN_RTR_DATA;
                for (uint8_t i=0; i<8; i++) {
                    msg.data[i]=Can_controller_frame.bytes[i];
                }
                vBusPublish(priv->can_tx_topic, &msg);
            }
            item = listGET_NEXT(item);
            vTaskDelay(10);
        }
        osDelay(10);
    }
}

void BrushESC775_Init(){
    vListInitialise(&motors);
    xTaskCreate(BrushESC775_MainLoop, "BrushESC775_MainLoop", 128, NULL, 200, NULL);
}