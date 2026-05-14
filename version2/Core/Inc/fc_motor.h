#ifndef FC_MOTOR_H
#define FC_MOTOR_H

#include <stdint.h>
#include <stdbool.h>

#define FC_MOTOR_MIN_US       1000U
#define FC_MOTOR_START_US     1100U
#define FC_MOTOR_IDLE_US      1200U
#define FC_MOTOR_MAX_US       1999U
#define FC_MOTOR_CALIB_MAX_US 2000U

/*
 * Motor layout, view from top:
 *
 *          FRONT
 *      M1          M4
 *
 *      M2          M3
 *          REAR
 *
 * TIM2_CH1/PA0 -> M1 front-left
 * TIM2_CH2/PA1 -> M2 rear-left
 * TIM2_CH3/PA2 -> M3 rear-right
 * TIM2_CH4/PA3 -> M4 front-right
 */
typedef enum {
    MOTOR_DISARMED = 0,
    MOTOR_ARMED = 1
} FC_MotorState_t;

extern volatile FC_MotorState_t motor_state;
extern volatile uint16_t motor_pwm[4];

void FC_Motor_Init(void);
void FC_Motor_StartPWM(void);
void FC_Motor_Arm(void);
void FC_Motor_Disarm(void);
void FC_Motor_SetPWM(uint16_t m1, uint16_t m2, uint16_t m3, uint16_t m4);

#endif
