/**
  ******************************************************************************
  * @file    motor_control.c
  * @brief   Motor control - throttle filter, mapping, ramp limiter, current filter
  ******************************************************************************
  * @note    No regenerative braking. Zero throttle = coast.
  *          Brake/fault = immediate duty = 0 (no ramp).
  ******************************************************************************
  */

#include "motor_control.h"
#include "pwm_config.h"
#include "sensors.h"

/* Private defines -----------------------------------------------------------*/
#define THROTTLE_MIN      800U
#define THROTTLE_MAX      3200U
#define RAMP_STEP         20U

/* Throttle low-pass: throttle_filtered = 0.8*prev + 0.2*raw */
#define THROTTLE_ALPHA    0.2f

/* Current EMA: filtered_current = alpha*raw + (1-alpha)*prev */
#define CURRENT_ALPHA     0.1f

/* Private variables ---------------------------------------------------------*/
static float throttle_filtered;
static float filtered_current;
static uint16_t duty;

/* Public functions ----------------------------------------------------------*/

/**
  * @brief  Initialize motor control state.
  */
void MotorControl_Init(void)
{
  throttle_filtered = 0.0f;
  filtered_current  = 0.0f;
  duty              = 0U;
}

/**
  * @brief  Update motor control - filter, map, ramp, output duty.
  */
void MotorControl_Update(uint8_t brake_active, uint8_t fault_active)
{
  uint16_t throttle_raw;
  uint16_t current_raw;
  uint32_t target_duty;

  /* 1. Read raw values from sensors module */
  throttle_raw = Sensors_GetThrottleRaw();
  current_raw  = Sensors_GetCurrentRaw();

  /* 2. Apply throttle low-pass filter: 0.8*prev + 0.2*raw */
  throttle_filtered = (1.0f - THROTTLE_ALPHA) * throttle_filtered
                      + THROTTLE_ALPHA * (float)throttle_raw;

  /* 3. Apply current exponential moving average */
  filtered_current = CURRENT_ALPHA * (float)current_raw
                     + (1.0f - CURRENT_ALPHA) * filtered_current;

  /* 4. Brake or fault: immediate stop, no ramp */
  if (brake_active != 0U || fault_active != 0U)
  {
    duty = 0U;
    /* TODO: apply duty to TIM1 (PWM) later */
    return;
  }

  /* 5. Compute target_duty from throttle mapping */
  if (throttle_filtered < (float)THROTTLE_MIN)
  {
    target_duty = 0U;
  }
  else if (throttle_filtered > (float)THROTTLE_MAX)
  {
    target_duty = TIM1_MAX_DUTY;
  }
  else
  {
    target_duty = (uint32_t)((throttle_filtered - (float)THROTTLE_MIN)
                             * (float)TIM1_MAX_DUTY
                             / (float)(THROTTLE_MAX - THROTTLE_MIN));
    if (target_duty > (uint32_t)TIM1_MAX_DUTY)
    {
      target_duty = TIM1_MAX_DUTY;
    }
  }

  /* 6. Apply ramp limiter - move duty toward target_duty by RAMP_STEP */
  if (target_duty > (uint32_t)duty)
  {
    duty += RAMP_STEP;
    if (duty > target_duty)
    {
      duty = (uint16_t)target_duty;
    }
  }
  else if (target_duty < (uint32_t)duty)
  {
    if (duty > RAMP_STEP)
    {
      duty -= RAMP_STEP;
    }
    else
    {
      duty = 0U;
    }
    if (duty < target_duty)
    {
      duty = (uint16_t)target_duty;
    }
  }

  if (duty > TIM1_MAX_DUTY)
  {
    duty = TIM1_MAX_DUTY;
  }

  /* TODO: apply duty to TIM1 (PWM) later */
}

/**
  * @brief  Get current duty cycle command.
  */
uint16_t MotorControl_GetDuty(void)
{
  return duty;
}

/**
  * @brief  Get filtered current sense value.
  */
float MotorControl_GetFilteredCurrent(void)
{
  return filtered_current;
}
