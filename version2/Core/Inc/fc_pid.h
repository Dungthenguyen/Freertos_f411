#ifndef FC_PID_H
#define FC_PID_H

#include <stdint.h>

// Struct chứa cấu hình và dữ liệu cho 1 bộ PID
typedef struct {
    float Kp;
    float Ki;
    float Kd;

    float error_integral; // Tổng sai số (cho khâu I)
    float prev_error;
    float previous_measurement; // Dùng để tính khâu D chống nhiễu (Derivative on Measurement)

    float output_limit; // Giới hạn công suất tối đa
    float integral_limit; // Giới hạn khâu I (Anti-windup)
    float prev_measurement;
} PID_Controller_t;

// Khởi tạo các thông số
void FC_PID_Init(void);

// Hàm tính toán chung cho mọi bộ PID
float FC_PID_Calculate(PID_Controller_t *pid, float setpoint, float measurement, float xt);

// Hàm này sẽ được gọi mỗi 1ms trong PID Task
void FC_PID_Run_1ms(void);

#endif // FC_PID_H
