#ifndef FC_MPU6050_H
#define FC_MPU6050_H

#include <stdint.h>
#include <stdbool.h>

// Struct lưu trữ dữ liệu từ cảm biến
typedef struct {
    int16_t Accel_X_RAW;
    int16_t Accel_Y_RAW;
    int16_t Accel_Z_RAW;
    int16_t Gyro_X_RAW;
    int16_t Gyro_Y_RAW;
    int16_t Gyro_Z_RAW;

    // Dữ liệu đã chuyển đổi ra đơn vị vật lý (độ/s và g)
    float Gx;
    float Gy;
    float Gz;
    float Ax;
    float Ay;
    float Az;

    // Góc nghiêng cuối cùng sau khi lọc
    float Roll;
    float Pitch;
} MPU6050_t;

extern MPU6050_t mpu_data;

// Các hàm giao tiếp
void FC_MPU6050_Init(void);
void FC_MPU6050_Calibrate(void); // Hàm hiệu chuẩn mới thêm
void FC_MPU6050_Read_DMA(void);  // Hàm ra lệnh cho DMA đọc

#endif // FC_MPU6050_H
