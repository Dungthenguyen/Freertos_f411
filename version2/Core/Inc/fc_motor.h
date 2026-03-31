#ifndef FC_MOTOR_H
#define FC_MOTOR_H

#include <stdint.h>
#include <stdbool.h>

// Định nghĩa các trạng thái của Motor
typedef enum {
    MOTOR_DISARMED = 0,
    MOTOR_ARMED = 1
} FC_MotorState_t;

// Biến toàn cục lưu trạng thái hiện tại
extern volatile FC_MotorState_t motor_state;
extern volatile uint16_t motor_pwm[4];
// Các hàm giao tiếp với Motor
void FC_Motor_Init(void);
void FC_Motor_Arm(void);
void FC_Motor_Disarm(void);

// Hàm xuất xung PWM cho 4 ESC (Chỉ chạy khi ARMED)
void FC_Motor_SetPWM(uint16_t m1, uint16_t m2, uint16_t m3, uint16_t m4);

#endif // FC_MOTOR_H
