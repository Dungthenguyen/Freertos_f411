#include "fc_kalman.h"

void FC_Kalman_Init(Kalman_t *kalman) {
    // Khởi tạo trạng thái ban đầu
    kalman->angle = 0.0f;
    kalman->bias = 0.0f;

    kalman->P[0][0] = 0.0f;
    kalman->P[0][1] = 0.0f;
    kalman->P[1][0] = 0.0f;
    kalman->P[1][1] = 0.0f;

    // Các hệ số này có thể tinh chỉnh (Tuning)
    // Q: Phương sai nhiễu hệ thống (Tin tưởng Gyro tới đâu)
    // R: Phương sai nhiễu đo lường (Tin tưởng Accel tới đâu)
    kalman->Q_angle = 0.001f;
    kalman->Q_bias = 0.003f;
    kalman->R_measure = 0.03f;
}

// Thuật toán Kalman Filter 1D
float FC_Kalman_GetAngle(Kalman_t *kalman, float newAngle, float newRate, float xt) {
    // 1. DỰ ĐOÁN (Predict)
    kalman->rate = newRate - kalman->bias;
    kalman->angle += xt * kalman->rate;

    kalman->P[0][0] += xt * (xt * kalman->P[1][1] - kalman->P[0][1] - kalman->P[1][0] + kalman->Q_angle);
    kalman->P[0][1] -= xt * kalman->P[1][1];
    kalman->P[1][0] -= xt * kalman->P[1][1];
    kalman->P[1][1] += kalman->Q_bias * xt;

    // 2. CẬP NHẬT (Update)
    float S = kalman->P[0][0] + kalman->R_measure; // Ước lượng sai số

    // Tính hệ số Kalman (Kalman Gain)
    float K[2];
    K[0] = kalman->P[0][0] / S;
    K[1] = kalman->P[1][0] / S;

    // Lấy góc từ Accel (newAngle) trừ đi góc dự đoán
    float y = newAngle - kalman->angle;

    // Cập nhật lại góc và độ trôi
    kalman->angle += K[0] * y;
    kalman->bias += K[1] * y;

    // Cập nhật lại ma trận sai số P
    float P00_temp = kalman->P[0][0];
    float P01_temp = kalman->P[0][1];

    kalman->P[0][0] -= K[0] * P00_temp;
    kalman->P[0][1] -= K[0] * P01_temp;
    kalman->P[1][0] -= K[1] * P00_temp;
    kalman->P[1][1] -= K[1] * P01_temp;

    return kalman->angle; // Trả về góc siêu mượt
}
