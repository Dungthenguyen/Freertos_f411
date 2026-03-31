#include "fc_mpu6050.h"
#include "main.h"
#include "cmsis_os.h"
#include "fc_kalman.h"
#include <math.h>

extern I2C_HandleTypeDef hi2c1;
extern osThreadId pidTaskHandle;

#define MPU6050_ADDR 0xD0

uint8_t mpu_rx_buffer[14];
MPU6050_t mpu_data;

// Khai báo 2 đối tượng Kalman cho Roll và Pitch
Kalman_t kalman_roll;
Kalman_t kalman_pitch;

// Các biến lưu giá trị bù trừ sai số (Offset)
float Gyro_X_CAL = 0, Gyro_Y_CAL = 0, Gyro_Z_CAL = 0;
float Accel_X_CAL = 0, Accel_Y_CAL = 0;

void FC_MPU6050_Init(void) {
    uint8_t check;
    uint8_t Data;

    // Kiểm tra xem MPU6050 có phản hồi không
    HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, 0x75, 1, &check, 1, 1000);

    if (check == 104) { // 104 (0x68) là mã chuẩn của MPU6050
        // Đánh thức cảm biến
        Data = 0;
        HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, 0x6B, 1, &Data, 1, 1000);

        // Cấu hình tần số lấy mẫu (SMPLRT_DIV)
        Data = 0x07;
        HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, 0x19, 1, &Data, 1, 1000);

        // Cấu hình Gyro (+/- 500 deg/s)
        Data = 0x08;
        HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, 0x1B, 1, &Data, 1, 1000);

        // Cấu hình Accel (+/- 8g)
        Data = 0x10;
        HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, 0x1C, 1, &Data, 1, 1000);
    }

    // Khởi tạo 2 bộ lọc Kalman
    FC_Kalman_Init(&kalman_roll);
    FC_Kalman_Init(&kalman_pitch);
}

// Hàm hiệu chuẩn: Đọc 1000 mẫu khi mạch nằm im để tính sai số
void FC_MPU6050_Calibrate(void) {
    uint8_t buffer[14];
    int32_t gx_sum = 0, gy_sum = 0, gz_sum = 0;
    int32_t ax_sum = 0, ay_sum = 0;

    for(int i = 0; i < 1000; i++) {
        HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, 0x3B, 1, buffer, 14, 100);

        ax_sum += (int16_t)(buffer[0] << 8 | buffer[1]);
        ay_sum += (int16_t)(buffer[2] << 8 | buffer[3]);
        // Bỏ qua trục Z vì nó đang đo trọng lực 1G

        gx_sum += (int16_t)(buffer[8] << 8 | buffer[9]);
        gy_sum += (int16_t)(buffer[10] << 8 | buffer[11]);
        gz_sum += (int16_t)(buffer[12] << 8 | buffer[13]);

        HAL_Delay(2);
    }

    Accel_X_CAL = (float)ax_sum / 1000.0f;
    Accel_Y_CAL = (float)ay_sum / 1000.0f;
    Gyro_X_CAL = (float)gx_sum / 1000.0f;
    Gyro_Y_CAL = (float)gy_sum / 1000.0f;
    Gyro_Z_CAL = (float)gz_sum / 1000.0f;
}

// Hàm ra lệnh DMA đọc data
void FC_MPU6050_Read_DMA(void) {
    HAL_I2C_Mem_Read_DMA(&hi2c1, MPU6050_ADDR, 0x3B, 1, mpu_rx_buffer, 14);
}

// Ngắt DMA chạy khi đã chép xong 14 byte
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c) {
    if (hi2c->Instance == I2C1) {
        // Lấy dữ liệu thô và TRỪ ĐI SAI SỐ (CAL)
        mpu_data.Accel_X_RAW = (int16_t)(mpu_rx_buffer[0] << 8 | mpu_rx_buffer[1]) - Accel_X_CAL;
        mpu_data.Accel_Y_RAW = (int16_t)(mpu_rx_buffer[2] << 8 | mpu_rx_buffer[3]) - Accel_Y_CAL;
        mpu_data.Accel_Z_RAW = (int16_t)(mpu_rx_buffer[4] << 8 | mpu_rx_buffer[5]);

        mpu_data.Gyro_X_RAW = (int16_t)(mpu_rx_buffer[8] << 8 | mpu_rx_buffer[9]) - Gyro_X_CAL;
        mpu_data.Gyro_Y_RAW = (int16_t)(mpu_rx_buffer[10] << 8 | mpu_rx_buffer[11]) - Gyro_Y_CAL;
        mpu_data.Gyro_Z_RAW = (int16_t)(mpu_rx_buffer[12] << 8 | mpu_rx_buffer[13]) - Gyro_Z_CAL;

        // Đổi ra đơn vị vật lý
        mpu_data.Ax = mpu_data.Accel_X_RAW / 4096.0;
        mpu_data.Ay = mpu_data.Accel_Y_RAW / 4096.0;
        mpu_data.Az = mpu_data.Accel_Z_RAW / 4096.0;

        mpu_data.Gx = mpu_data.Gyro_X_RAW / 65.5;
        mpu_data.Gy = mpu_data.Gyro_Y_RAW / 65.5;
        mpu_data.Gz = mpu_data.Gyro_Z_RAW / 65.5;

        // Tính góc thô từ Gia tốc
        float accel_roll  = atan2(mpu_data.Ay, mpu_data.Az) * 180.0 / M_PI;
        float accel_pitch = atan2(-mpu_data.Ax, sqrt(mpu_data.Ay * mpu_data.Ay + mpu_data.Az * mpu_data.Az)) * 180.0 / M_PI;

        // Lọc qua Kalman
        float xt = 0.001f;
        mpu_data.Roll = FC_Kalman_GetAngle(&kalman_roll, accel_roll, mpu_data.Gx, xt);
        mpu_data.Pitch = FC_Kalman_GetAngle(&kalman_pitch, accel_pitch, mpu_data.Gy, xt);

        // Đánh thức PID Task
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(pidTaskHandle, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}
