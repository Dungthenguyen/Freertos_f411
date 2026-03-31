#ifndef FC_RC_H
#define FC_RC_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_CHANNELS 8 // Tay FlySky FS-i6/i6X thường xuất 6-8 kênh qua PPM

// Mảng lưu giá trị các kênh (từ 1000 đến 2000)
// Chú ý: C mảng bắt đầu từ 0, nên Kênh 1 sẽ là rc_channels[0]
extern volatile uint16_t rc_channels[MAX_CHANNELS];

// Hàm khởi tạo (nếu cần) và hàm xử lý logic Arm/Disarm gọi trong RCTask
void FC_RC_Init(void);
void FC_RC_Process(void);

#endif // FC_RC_H
