#include "fc_motor.h"
#include "main.h"

extern TIM_HandleTypeDef htim2;

volatile FC_MotorState_t motor_state = MOTOR_DISARMED;
volatile uint16_t motor_pwm[4] = {
    FC_MOTOR_MIN_US,
    FC_MOTOR_MIN_US,
    FC_MOTOR_MIN_US,
    FC_MOTOR_MIN_US
};

static uint16_t clamp_pwm(uint16_t pwm) {
    if (pwm < FC_MOTOR_MIN_US) {
        return FC_MOTOR_MIN_US;
    }
    if (pwm > FC_MOTOR_MAX_US) {
        return FC_MOTOR_MAX_US;
    }
    return pwm;
}

static void write_motor_pwm(uint16_t m1, uint16_t m2, uint16_t m3, uint16_t m4) {
    motor_pwm[0] = m1;
    motor_pwm[1] = m2;
    motor_pwm[2] = m3;
    motor_pwm[3] = m4;

    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, m1);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, m2);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, m3);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, m4);
}

void FC_Motor_Init(void) {
    motor_state = MOTOR_DISARMED;
    write_motor_pwm(FC_MOTOR_MIN_US,
                    FC_MOTOR_MIN_US,
                    FC_MOTOR_MIN_US,
                    FC_MOTOR_MIN_US);
}

void FC_Motor_StartPWM(void) {
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
}

void FC_Motor_Arm(void) {
    motor_state = MOTOR_ARMED;
    write_motor_pwm(FC_MOTOR_MIN_US,
                    FC_MOTOR_MIN_US,
                    FC_MOTOR_MIN_US,
                    FC_MOTOR_MIN_US);
}

void FC_Motor_Disarm(void) {
    motor_state = MOTOR_DISARMED;
    write_motor_pwm(FC_MOTOR_MIN_US,
                    FC_MOTOR_MIN_US,
                    FC_MOTOR_MIN_US,
                    FC_MOTOR_MIN_US);
}

void FC_Motor_SetPWM(uint16_t m1, uint16_t m2, uint16_t m3, uint16_t m4) {
    if (motor_state == MOTOR_DISARMED) {
        write_motor_pwm(FC_MOTOR_MIN_US,
                        FC_MOTOR_MIN_US,
                        FC_MOTOR_MIN_US,
                        FC_MOTOR_MIN_US);
        return;
    }

    write_motor_pwm(clamp_pwm(m1),
                    clamp_pwm(m2),
                    clamp_pwm(m3),
                    clamp_pwm(m4));
}
