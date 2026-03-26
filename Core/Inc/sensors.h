/**
  ******************************************************************************
  * @file    sensors.h
  * @brief   Sensor acquisition module - ADC (throttle, current) and TMP102 (temperature)
  ******************************************************************************
  * @note    Raw sensor values only. No filtering, mapping, or control logic.
  ******************************************************************************
  */

#ifndef SENSORS_H
#define SENSORS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Public function prototypes ------------------------------------------------*/

/**
  * @brief  Initialize sensors and start ADC DMA.
  * @note   Call once after MX_ADC1_Init(), MX_I2C1_Init(). Starts continuous
  *         ADC conversion with DMA. Does not block.
  */
void Sensors_Init(void);

/**
  * @brief  Update sensor values from ADC buffer and TMP102.
  * @note   Call periodically from main loop. Copies ADC DMA buffer to internal
  *         variables and reads temperature from TMP102 over I2C.
  */
void Sensors_Update(void);

/**
  * @brief  Get raw throttle ADC value (0..4095).
  * @retval Throttle ADC reading from PC0 (Channel 6).
  */
uint16_t Sensors_GetThrottleRaw(void);

/**
  * @brief  Get raw current sense ADC value (0..4095).
  * @retval Current ADC reading from PC1 (Channel 7).
  */
uint16_t Sensors_GetCurrentRaw(void);

/**
  * @brief  Get temperature in Celsius.
  * @retval Temperature in °C from TMP102. Last valid read or 0.0f on I2C error.
  */
float Sensors_GetTemperatureC(void);

#ifdef __cplusplus
}
#endif

#endif /* SENSORS_H */
