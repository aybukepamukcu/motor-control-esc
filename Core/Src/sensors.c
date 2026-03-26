/**
  ******************************************************************************
  * @file    sensors.c
  * @brief   Sensor acquisition - ADC DMA (throttle, current) and TMP102 (temperature)
  ******************************************************************************
  * @note    ADC DMA buffer layout (no bit packing):
  *          adc_buffer[0] = throttle (PC0, Channel 6)
  *          adc_buffer[1] = current  (PC1, Channel 7)
  ******************************************************************************
  */

#include "sensors.h"
#include "main.h"

/* External handles from CubeMX (defined in main.c) --------------------------*/
extern ADC_HandleTypeDef hadc1;
extern I2C_HandleTypeDef hi2c1;

/* Private defines -----------------------------------------------------------*/
#define TMP102_I2C_ADDR   (0x48U << 1)   /* 7-bit addr 0x48, HAL expects 8-bit */
#define TMP102_TEMP_REG   0x00U

/* TMP102 conversion: 12-bit, MSB first, 0.0625°C per LSB.
 * Raw value in upper 12 bits: temp_c = (int16_t)(raw >> 4) * 0.0625f
 */

/* Private variables ---------------------------------------------------------*/
/* ADC DMA buffer: one conversion per index, filled continuously by DMA */
static uint16_t adc_buffer[2];

/* Cached sensor values updated by Sensors_Update() */
static uint16_t throttle_raw;
static uint16_t current_raw;
static float temperature_c;

/* Private function prototypes -----------------------------------------------*/
static HAL_StatusTypeDef TMP102_ReadTemperature(float *temp_c);

/* Public functions ----------------------------------------------------------*/

/**
  * @brief  Initialize sensors and start ADC DMA.
  */
void Sensors_Init(void)
{
  throttle_raw = 0U;
  current_raw  = 0U;
  temperature_c = 0.0f;

  /* Start ADC in DMA circular mode - fills adc_buffer continuously.
   * Cast to uint32_t* required by HAL_ADC_Start_DMA API. */
  (void)HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_buffer, 2);
}

/**
  * @brief  Update sensor values from ADC buffer and TMP102.
  */
void Sensors_Update(void)
{
  /* Copy from ADC DMA buffer - one conversion per index */
  throttle_raw = adc_buffer[0];
  current_raw  = adc_buffer[1];

  /* Read TMP102 temperature over I2C */
  (void)TMP102_ReadTemperature(&temperature_c);
}

/**
  * @brief  Get raw throttle ADC value.
  */
uint16_t Sensors_GetThrottleRaw(void)
{
  return throttle_raw;
}

/**
  * @brief  Get raw current sense ADC value.
  */
uint16_t Sensors_GetCurrentRaw(void)
{
  return current_raw;
}

/**
  * @brief  Get temperature in Celsius.
  */
float Sensors_GetTemperatureC(void)
{
  return temperature_c;
}

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Read temperature from TMP102 via I2C.
  * @param  temp_c: pointer to float, receives temperature in °C
  * @retval HAL status (HAL_OK on success)
  *
  * TMP102 format: Register 0x00, 2 bytes MSB first.
  * 12-bit value in upper bits: temp_c = (raw >> 4) * 0.0625
  */
static HAL_StatusTypeDef TMP102_ReadTemperature(float *temp_c)
{
  uint8_t reg = TMP102_TEMP_REG;
  uint8_t data[2];
  HAL_StatusTypeDef status;

  if (temp_c == NULL)
  {
    return HAL_ERROR;
  }

  /* Write register pointer (0x00 = temperature) */
  status = HAL_I2C_Master_Transmit(&hi2c1, TMP102_I2C_ADDR, &reg, 1, HAL_MAX_DELAY);
  if (status != HAL_OK)
  {
    return status;
  }

  /* Read 2 bytes (MSB first) */
  status = HAL_I2C_Master_Receive(&hi2c1, TMP102_I2C_ADDR, data, 2, HAL_MAX_DELAY);
  if (status != HAL_OK)
  {
    return status;
  }

  /* TMP102: 12-bit, 0.0625°C/LSB, MSB first. Temp = (raw >> 4) * 0.0625 */
  int16_t raw = (int16_t)((data[0] << 8) | data[1]);
  *temp_c = (float)(raw >> 4) * 0.0625f;
  return HAL_OK;
}
