/**
  ******************************************************************************
  * @file    motor_control.h
  * @brief   Motor control module - throttle processing, mapping, ramp limiter
  ******************************************************************************
  * @note    No BLDC commutation. PWM output is a placeholder.
  *          Reads throttle/current from sensors module.
  ******************************************************************************
  */

#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Public function prototypes ------------------------------------------------*/

/**
  * @brief  Initialize motor control state.
  * @note   Call once after Sensors_Init(). Resets filtered values and duty.
  */
void MotorControl_Init(void);

/**
  * @brief  Update motor control - filter, map, ramp, output duty.
  * @param  brake_active: 1 = brake pressed, duty forced to 0 immediately
  * @param  fault_active: 1 = safety fault, duty forced to 0 immediately
  * @note   Call periodically from main loop after Sensors_Update().
  */
void MotorControl_Update(uint8_t brake_active, uint8_t fault_active);

/**
 * @brief  Get current duty cycle command (0..TIM1_MAX_DUTY).
 * @retval Duty value for PWM. 0 = coast, TIM1_MAX_DUTY = 100% (see pwm_config.h).
  */
uint16_t MotorControl_GetDuty(void);

/**
  * @brief  Get filtered current sense value.
  * @retval Exponential moving average of current ADC (0..4095 scale).
  */
float MotorControl_GetFilteredCurrent(void);

#ifdef __cplusplus
}
#endif

#endif /* MOTOR_CONTROL_H */
