/**
  ******************************************************************************
  * @file    commutation.h
  * @brief   BLDC 6-step commutation module - hall decoding and PWM sector selection
  ******************************************************************************
  */

#ifndef COMMUTATION_H
#define COMMUTATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void Commutation_Init(void);
uint8_t Commutation_ReadHallState(void);
void Commutation_Update(uint16_t duty);
uint8_t Commutation_GetLastHallState(void);

#ifdef __cplusplus
}
#endif

#endif /* COMMUTATION_H */
