/**
  ******************************************************************************
  * @file    safety.h
  * @brief   Safety module - brake, overtemperature, overcurrent monitoring
  ******************************************************************************
  */

#ifndef SAFETY_H
#define SAFETY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define SAFETY_FAULT_NONE         0x00000000U
#define SAFETY_FAULT_BRAKE        0x00000001U
#define SAFETY_FAULT_OVERTEMP     0x00000002U
#define SAFETY_FAULT_OVERCURRENT  0x00000004U

void Safety_Init(void);
void Safety_Update(void);

uint8_t Safety_IsBrakeActive(void);
uint8_t Safety_IsFaultActive(void);
uint32_t Safety_GetFaultFlags(void);

#ifdef __cplusplus
}
#endif

#endif /* SAFETY_H */
