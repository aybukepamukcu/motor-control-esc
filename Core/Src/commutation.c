/**
  ******************************************************************************
  * @file    commutation.c
  * @brief   BLDC 6-step sensored commutation - hall decode and TIM1 PWM sector output
  ******************************************************************************
  * Hall bits: PB0=bit0, PB1=bit1, PB2=bit2 -> hall = (PB0<<0)|(PB1<<1)|(PB2<<2)
  * Valid: 1,2,3,4,5,6. Invalid: 0,7.
  * TIM1: CH1=U, CH2=V, CH3=W. CHxN = complementary low-side.
  * Call HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1/2/3) and HAL_TIMEx_PWMN_Start()
  * during system startup in main.c USER CODE BEGIN 2, after Commutation_Init().
  ******************************************************************************
  */

#include "commutation.h"
#include "main.h"

#define HALL_A_PIN    GPIO_PIN_0
#define HALL_B_PIN    GPIO_PIN_1
#define HALL_C_PIN    GPIO_PIN_2
#define HALL_GPIO     GPIOB

extern TIM_HandleTypeDef htim1;

static uint8_t last_hall_state;

static uint8_t ReadHallGPIO(void)
{
  uint8_t a = (HAL_GPIO_ReadPin(HALL_GPIO, HALL_A_PIN) == GPIO_PIN_SET) ? 1U : 0U;
  uint8_t b = (HAL_GPIO_ReadPin(HALL_GPIO, HALL_B_PIN) == GPIO_PIN_SET) ? 1U : 0U;
  uint8_t c = (HAL_GPIO_ReadPin(HALL_GPIO, HALL_C_PIN) == GPIO_PIN_SET) ? 1U : 0U;
  return (uint8_t)((a << 0) | (b << 1) | (c << 2));
}

static void SafeOff_AllPhases(void)
{
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
}

static void LowSide_Activate(uint8_t phase)
{
  (void)phase;
}

static void LowSide_Deactivate(uint8_t phase)
{
  (void)phase;
}

static void ApplyCommutation(uint8_t hall, uint16_t duty)
{
  SafeOff_AllPhases();

  switch (hall)
  {
    case 1:
      __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, duty);
      __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
      __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
      break;
    case 2:
      __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
      __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
      __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, duty);
      break;
    case 3:
      __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
      __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
      __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, duty);
      break;
    case 4:
      __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
      __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, duty);
      __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
      break;
    case 5:
      __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, duty);
      __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
      __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
      break;
    case 6:
      __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
      __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, duty);
      __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
      break;
    default:
      break;
  }
}

void Commutation_Init(void)
{
  last_hall_state = 0U;
  SafeOff_AllPhases();
}

uint8_t Commutation_ReadHallState(void)
{
  return ReadHallGPIO();
}

void Commutation_Update(uint16_t duty)
{
  uint8_t hall;

  hall = ReadHallGPIO();
  last_hall_state = hall;

  if (duty == 0U)
  {
    SafeOff_AllPhases();
    return;
  }

  if (hall == 0U || hall == 7U)
  {
    SafeOff_AllPhases();
    return;
  }

  ApplyCommutation(1, duty);
}

uint8_t Commutation_GetLastHallState(void)
{
  return last_hall_state;
}
