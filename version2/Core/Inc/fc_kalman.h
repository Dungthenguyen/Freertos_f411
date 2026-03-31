#ifndef FC_KALMAN_H
#define FC_KALMAN_H

typedef struct {
    float angle; // Góc tính toán được (Đầu ra cuối cùng)
    float bias;  // Độ trôi của Gyro (Được Kalman tự động ước lượng)
    float rate;  // Tốc độ xoay thực tế (Đã trừ đi bias)

    float P[2][2]; // Ma trận sai số dự báo (Error Covariance Matrix)

    // Các hệ số nhiễu (Q và R)
    float Q_angle;
    float Q_bias;
    float R_measure;
} Kalman_t;

void FC_Kalman_Init(Kalman_t *kalman);
float FC_Kalman_GetAngle(Kalman_t *kalman, float newAngle, float newRate, float xt);

#endif // FC_KALMAN_H
