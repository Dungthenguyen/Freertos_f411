#include "fc_rc.h"
#include "fc_motor.h"
#include "main.h"

volatile uint16_t rc_channels[MAX_CHANNELS];
volatile bool rc_yaw_centered_after_arm = false;
uint16_t rc_center[4] = {1500, 1500, 1500, 1500};

static uint16_t current_channel = 0;
static uint16_t last_capture = 0;
static uint16_t arm_hold_counter = 0;
static uint16_t disarm_hold_counter = 0;

#define RC_ARM_THROTTLE_US          1050U
#define RC_ARM_YAW_HIGH_US          1850U
#define RC_DISARM_YAW_LOW_US        1150U
#define RC_YAW_CENTER_WINDOW_US     80U

static bool yaw_is_centered(uint16_t yaw) {
    uint16_t center = rc_center[3];
    return (yaw > (center - RC_YAW_CENTER_WINDOW_US)) &&
           (yaw < (center + RC_YAW_CENTER_WINDOW_US));
}

void FC_RC_Init(void) {
    for (int i = 0; i < MAX_CHANNELS; i++) {
        rc_channels[i] = 1500;
    }

    rc_channels[2] = 1000;
    rc_yaw_centered_after_arm = false;
}

void FC_RC_CalibrateCenter(void) {
    HAL_Delay(500);

    uint32_t sum0 = 0;
    uint32_t sum1 = 0;
    uint32_t sum3 = 0;

    for (int i = 0; i < 100; i++) {
        sum0 += rc_channels[0];
        sum1 += rc_channels[1];
        sum3 += rc_channels[3];
        HAL_Delay(10);
    }

    rc_center[0] = sum0 / 100;
    rc_center[1] = sum1 / 100;
    rc_center[2] = 1500;
    rc_center[3] = sum3 / 100;

    for (int i = 0; i < 4; i++) {
        if (i == 2) {
            continue;
        }

        if ((rc_center[i] < 1200) || (rc_center[i] > 1800)) {
            rc_center[i] = 1500;
        }
    }
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    if ((htim->Instance == TIM3) && (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)) {
        uint16_t current_capture = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
        uint16_t diff = 0;

        if (current_capture >= last_capture) {
            diff = current_capture - last_capture;
        } else {
            diff = (0xFFFF - last_capture) + current_capture + 1;
        }

        last_capture = current_capture;

        if (diff > 3000) {
            current_channel = 0;
        } else if (current_channel < MAX_CHANNELS) {
            if ((diff > 900) && (diff < 2100)) {
                rc_channels[current_channel] = diff;
            }
            current_channel++;
        }
    }
}

void FC_RC_Process(void) {
    uint16_t throttle = rc_channels[2];
    uint16_t yaw = rc_channels[3];

    if (motor_state == MOTOR_DISARMED) {
        rc_yaw_centered_after_arm = false;

        if ((throttle < RC_ARM_THROTTLE_US) && (yaw > RC_ARM_YAW_HIGH_US)) {
            arm_hold_counter++;
            if (arm_hold_counter > 50) {
                FC_Motor_Arm();
                rc_yaw_centered_after_arm = false;
                arm_hold_counter = 0;
            }
        } else {
            arm_hold_counter = 0;
        }
    }

    if (motor_state == MOTOR_ARMED) {
        if ((throttle < RC_ARM_THROTTLE_US) && yaw_is_centered(yaw)) {
            rc_yaw_centered_after_arm = true;
        }

        if ((throttle < RC_ARM_THROTTLE_US) && (yaw < RC_DISARM_YAW_LOW_US)) {
            disarm_hold_counter++;
            if (disarm_hold_counter > 50) {
                FC_Motor_Disarm();
                rc_yaw_centered_after_arm = false;
                disarm_hold_counter = 0;
            }
        } else {
            disarm_hold_counter = 0;
        }
    }
}
