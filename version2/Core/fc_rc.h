/*
 * fc_rc.h
 *
 *  Created on: Mar 27, 2026
 *      Author: Admin
 */
#ifndef FC_RC_H
#define FC_RC_H

#include <stdint.h>

typedef struct
{
    float throttle;
    uint8_t arm;
    uint8_t failsafe;
    uint32_t update_tick;
} RC_Command_t;

extern volatile RC_Command_t g_rc_cmd;
extern volatile uint16_t rc_raw[8];
extern volatile uint32_t rc_last_update_ms;

void FC_RC_Init(void);
void FC_RC_Update(void);
void FC_RC_PrintDebug(void);
uint8_t FC_RC_Alive(void);

#endif /* FC_RC_H_ */
