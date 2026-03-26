/**
  ******************************************************************************
  * @file    safety.c
  * @brief   Safety module - brake, overtemperature, overcurrent monitoring
  ******************************************************************************
  */

#include "safety.h"
#include "sensors.h"
#include "main.h"

#define BRAKE_PIN         GPIO_PIN_5
#define BRAKE_GPIO_PORT   GPIOC
#define TEMP_LIMIT_C      80.0f
#define CURRENT_LIMIT     3000U

static uint32_t fault_flags;
static uint8_t brake_active;
static uint8_t fault_active;

void Safety_Init(void)
{
  fault_flags   = SAFETY_FAULT_NONE;
  brake_active  = 0U;
  fault_active  = 0U;
}

void Safety_Update(void)
{
  float temp_c;
  uint16_t current_raw;

  fault_flags = SAFETY_FAULT_NONE;

  if (HAL_GPIO_ReadPin(BRAKE_GPIO_PORT, BRAKE_PIN) == GPIO_PIN_SET)
  {
    brake_active = 1U;
    fault_flags |= SAFETY_FAULT_BRAKE;
  }
  else
  {
    brake_active = 0U;
  }

  temp_c = Sensors_GetTemperatureC();
  if (temp_c > TEMP_LIMIT_C)
  {
    fault_flags |= SAFETY_FAULT_OVERTEMP;
  }

  current_raw = Sensors_GetCurrentRaw();
  if (current_raw > CURRENT_LIMIT)
  {
    fault_flags |= SAFETY_FAULT_OVERCURRENT;
  }

  fault_active = (fault_flags != SAFETY_FAULT_NONE) ? 1U : 0U;
}

uint8_t Safety_IsBrakeActive(void)
{
  return brake_active;
}

uint8_t Safety_IsFaultActive(void)
{
  return fault_active;
}

uint32_t Safety_GetFaultFlags(void)
{
  return fault_flags;
}
