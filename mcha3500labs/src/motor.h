#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>


void motor_PWM_init(void);
void motor_encoder_init(void);
void EXTI0_IRQHandler(void);
int32_t motor_encoder_getValue(void);

#endif