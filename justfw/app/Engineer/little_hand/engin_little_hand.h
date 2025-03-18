#ifndef __ENGIN_LITTLE_HAND_H
#define __ENGIN_LITTLE_HAND_H
#include "interface.h"

extern INTF_Motor_HandleTypeDef *m_l_joint1;
extern INTF_Motor_HandleTypeDef *m_l_joint2;
extern INTF_Motor_HandleTypeDef *m_l_joint3;
extern INTF_Motor_HandleTypeDef *m_l_joint4;

void Engin_Little_Hande_Motor_Init();

void cm_l2_colibration();
void cm_l3_colibration();
void cm_l4_colibration();

void elhp_init();

void lb1_postion_info();
void lb1_set_position(float angle[4], uint32_t steps);

#endif