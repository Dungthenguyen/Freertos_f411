#include "fc_pid.h"
#include "fc_rc.h"
#include "fc_mpu6050.h"
#include "fc_motor.h"

// ---------------------------------------------------------
// 1. KHAI BÁO CÁC BỘ PID
// ---------------------------------------------------------
// Vòng ngoài: PID Góc (Chỉ cần Roll và Pitch, Yaw điều khiển trực tiếp bằng tốc độ)
PID_Controller_t pid_roll_angle;
PID_Controller_t pid_pitch_angle;

// Vòng trong: PID Tốc độ xoay (Rate)
PID_Controller_t pid_roll_rate;
PID_Controller_t pid_pitch_rate;
PID_Controller_t pid_yaw_rate;

// ---------------------------------------------------------
// 2. HÀM TÍNH TOÁN LÕI PID
// ---------------------------------------------------------
float FC_PID_Calculate(PID_Controller_t *pid, float setpoint, float measurement, float xt) {
    float error = setpoint - measurement;

    // Khâu P (Proportional)
    float P_out = pid->Kp * error;

    // Khâu I (Integral) - Tích lũy sai số
    pid->error_integral += error * xt;

    // Giới hạn chống tràn khâu I (Anti-Windup)
    if (pid->error_integral > pid->integral_limit) pid->error_integral = pid->integral_limit;
    if (pid->error_integral < -pid->integral_limit) pid->error_integral = -pid->integral_limit;

    float I_out = pid->Ki * pid->error_integral;

    // Khâu D (Derivative) - Dựa trên sự thay đổi của giá trị đo (Derivative on Measurement)
    // Giúp triệt tiêu hiện tượng "giật cục" khi gạt mạnh cần RC
    float D_out = -pid->Kd * (measurement - pid->previous_measurement) / xt;
    pid->previous_measurement = measurement;

    // Tổng hợp đầu ra
    float output = P_out + I_out + D_out;

    // Giới hạn đầu ra tối đa để tránh quá tải
    if (output > pid->output_limit) output = pid->output_limit;
    if (output < -pid->output_limit) output = -pid->output_limit;

    return output;
}

// ---------------------------------------------------------
// 3. KHỞI TẠO THÔNG SỐ (TUNING)
// ---------------------------------------------------------
void FC_PID_Init(void) {
    // THÔNG SỐ VÒNG GÓC (Angle Loop) - Thường chỉ dùng Kp
    pid_roll_angle.Kp = 2.0f;  pid_roll_angle.Ki = 0.0f;  pid_roll_angle.Kd = 0.0f;
    pid_pitch_angle.Kp = 2.0f; pid_pitch_angle.Ki = 0.0f; pid_pitch_angle.Kd = 0.0f;

    pid_roll_angle.output_limit = 150.0f;  // Tối đa yêu cầu xoay 150 độ/giây
    pid_pitch_angle.output_limit = 150.0f;

    // THÔNG SỐ VÒNG TỐC ĐỘ (Rate Loop) - Cần cả P, I, D
    // Lưu ý: Đây là số liệu mẫu an toàn, khi bay thực tế bạn sẽ phải tinh chỉnh (PID Tuning)
    pid_roll_rate.Kp = 1.2f;   pid_roll_rate.Ki = 0.00f;  pid_roll_rate.Kd = 0.01f;
    pid_pitch_rate.Kp = 1.2f;  pid_pitch_rate.Ki = 0.00f; pid_pitch_rate.Kd = 0.01f;
    pid_yaw_rate.Kp = 2.0f;    pid_yaw_rate.Ki = 0.00f;   pid_yaw_rate.Kd = 0.0f;

    pid_roll_rate.output_limit = 400.0f;   pid_roll_rate.integral_limit = 200.0f;
    pid_pitch_rate.output_limit = 400.0f;  pid_pitch_rate.integral_limit = 200.0f;
    pid_yaw_rate.output_limit = 400.0f;    pid_yaw_rate.integral_limit = 200.0f;
}

// ---------------------------------------------------------
// 4. HÀM CHÍNH - GỌI MỖI 1MS TỪ PID TASK
// ---------------------------------------------------------
void FC_PID_Run_1ms(void) {
    // xt là khoảng thời gian giữa 2 lần tính toán (1ms = 0.001s)
    float xt = 0.001f;

    // Nếu đang DISARM, xóa sạch bộ nhớ tích phân để tránh motor rú lên lúc vừa Arm
    if (motor_state == MOTOR_DISARMED) {
        pid_roll_angle.error_integral = 0;
        pid_pitch_angle.error_integral = 0;
        pid_roll_rate.error_integral = 0;
        pid_pitch_rate.error_integral = 0;
        pid_yaw_rate.error_integral = 0;
        return; // Thoát luôn, không tính toán xuất motor
    }

    // --- LẤY LỆNH TỪ TAY CẦM RC ---
    // Mapping: Giả sử cần gạt giữa là 1500. Nghiêng kịch kim được góc tối đa 30 độ.

    float rc_roll  = rc_channels[0];
        float rc_pitch = rc_channels[1];
        float rc_yaw   = rc_channels[3];

        if(rc_roll > 1495 && rc_roll < 1505) rc_roll = 1500;
            if(rc_pitch > 1495 && rc_pitch < 1505) rc_pitch = 1500;
            if(rc_yaw > 1495 && rc_yaw < 1505) rc_yaw = 1500;
    // Công thức: (giá trị - 1500) * (MaxAngle / 500)
    float rc_roll_target_angle  = (rc_channels[0] - 1500) * 0.06f; // -30 đến +30 độ
    float rc_pitch_target_angle = (rc_channels[1] - 1500) * 0.06f;

    // Yaw không dùng góc, mà dùng tốc độ xoay (độ/giây)
    float rc_yaw_target_rate    = (rc_channels[3] - 1500) * 0.3f;  // -150 đến +150 deg/s

    // Lấy Ga nền
    float throttle = rc_channels[2];
    if (throttle < 1050) throttle = 1050; // Idle

    // ===============================================
    // BƯỚC 1: VÒNG LẶP GÓC (ANGLE LOOP)
    // ===============================================
    float target_roll_rate = FC_PID_Calculate(&pid_roll_angle, rc_roll_target_angle, mpu_data.Roll, xt);
    float target_pitch_rate = FC_PID_Calculate(&pid_pitch_angle, rc_pitch_target_angle, mpu_data.Pitch, xt);
    float target_yaw_rate = rc_yaw_target_rate;

    // ===============================================
    // BƯỚC 2: VÒNG LẶP TỐC ĐỘ (RATE LOOP)
    // ===============================================
    float roll_output  = FC_PID_Calculate(&pid_roll_rate, target_roll_rate, mpu_data.Gx, xt);
    float pitch_output = FC_PID_Calculate(&pid_pitch_rate, target_pitch_rate, mpu_data.Gy, xt);
    float yaw_output   = FC_PID_Calculate(&pid_yaw_rate, target_yaw_rate, mpu_data.Gz, xt);

    // ===============================================
    // BƯỚC 3: TRỘN TÍN HIỆU (MIXER) CHO CẤU HÌNH QUAD X
    // ===============================================
    /* Giả sử cấu hình Quad X chuẩn:
        M4 (Trước Trái)  \ /  M2 (Trước Phải)
                          X
        M3 (Sau Trái)    / \  M1 (Sau Phải)

        *Lưu ý: Bạn có thể đảo dấu (+/-) ở đây nếu chiều motor thực tế bị ngược.
    */
    int16_t motor1 = throttle - pitch_output - roll_output + yaw_output; // M1: Sau Phải (CW)
    int16_t motor2 = throttle + pitch_output - roll_output - yaw_output; // M2: Trước Phải (CCW)
    int16_t motor3 = throttle - pitch_output + roll_output - yaw_output; // M3: Sau Trái (CCW)
    int16_t motor4 = throttle + pitch_output + roll_output + yaw_output; // M4: Trước Trái (CW)

    // ===============================================
    // BƯỚC 4: RÀNG BUỘC (CONSTRAIN) VÀ XUẤT PWM
    // ===============================================
    // Chặn trên dưới để bảo vệ ESC (1000 - 2000)
    if (motor1 < 1050) motor1 = 1000; if (motor1 > 2000) motor1 = 2000;
    if (motor2 < 1050) motor2 = 1000; if (motor2 > 2000) motor2 = 2000;
    if (motor3 < 1050) motor3 = 1000; if (motor3 > 2000) motor3 = 2000;
    if (motor4 < 1050) motor4 = 1000; if (motor4 > 2000) motor4 = 2000;

    // Bơm ra 4 ESC
    FC_Motor_SetPWM(motor1, motor2, motor3, motor4);
}
