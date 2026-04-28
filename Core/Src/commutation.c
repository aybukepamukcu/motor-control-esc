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

static volatile uint8_t last_hall_state;

#define TIM1_PHASE_ENABLE_MASK (TIM_CCER_CC1E | TIM_CCER_CC1NE | \
                                TIM_CCER_CC2E | TIM_CCER_CC2NE | \
                                TIM_CCER_CC3E | TIM_CCER_CC3NE)

enum
{
  PHASE_FLOAT = 0,
  PHASE_UV,
  PHASE_UW,
  PHASE_VW,
  PHASE_VU,
  PHASE_WU,
  PHASE_WV
};

static const uint8_t HALL_PHASE_TABLE[] =
{
  PHASE_FLOAT,
  PHASE_UV, /* 001 */
  PHASE_WU, /* 010 */
  PHASE_WV, /* 011 */
  PHASE_VW, /* 100 */
  PHASE_UW, /* 101 */
  PHASE_VU  /* 110 */
};

static const uint32_t PHASE_CCER_TABLE[] =
{
  0U,
  TIM_CCER_CC1E | TIM_CCER_CC2NE,
  TIM_CCER_CC1E | TIM_CCER_CC3NE,
  TIM_CCER_CC2E | TIM_CCER_CC3NE,
  TIM_CCER_CC2E | TIM_CCER_CC1NE,
  TIM_CCER_CC3E | TIM_CCER_CC1NE,
  TIM_CCER_CC3E | TIM_CCER_CC2NE
};

static uint8_t ReadHallGPIO(void)
{
  /* Read GPIOB PIN0-PIN1-PIN2 directly from the register. */
  return (uint8_t)(GPIOB->IDR & (uint32_t)0x07);
}

void Commutation_DisableAllPhases(void)
{
  /* Disable all of the channels. */
  TIM1->CCER &= ~TIM1_PHASE_ENABLE_MASK;

  /* Set all of the duty cycles to zero. */
  TIM1->CCR1 = 0;
  TIM1->CCR2 = 0;
  TIM1->CCR3 = 0;
}

static void ApplyCommutation(uint8_t phase, uint16_t duty)
{
  switch (phase)
  {
    case PHASE_UV:
      TIM1->CCR1 = duty;
      TIM1->CCR2 = TIM1->ARR;
      TIM1->CCR3 = 0;
      break;
    case PHASE_UW:
      TIM1->CCR1 = duty;
      TIM1->CCR2 = 0;
      TIM1->CCR3 = TIM1->ARR;
      break;
    case PHASE_VW:
      TIM1->CCR1 = 0;
      TIM1->CCR2 = duty;
      TIM1->CCR3 = TIM1->ARR;
      break;
    case PHASE_VU:
      TIM1->CCR1 = TIM1->ARR;
      TIM1->CCR2 = duty;
      TIM1->CCR3 = 0;
      break;
    case PHASE_WU:
      TIM1->CCR1 = TIM1->ARR;
      TIM1->CCR2 = 0;
      TIM1->CCR3 = duty;
      break;
    case PHASE_WV:
      TIM1->CCR1 = 0;
      TIM1->CCR2 = TIM1->ARR;
      TIM1->CCR3 = duty;
      break;
    default:
      break;
  }
}

void Commutation_Init(void)
{
  last_hall_state = 0U;
  Commutation_DisableAllPhases();
}

uint8_t Commutation_ReadHallState(void)
{
  return ReadHallGPIO();
}

void Commutation_Update(uint16_t duty)
{
  uint8_t hall;
  uint8_t phase;

  hall = ReadHallGPIO();

  if (duty == 0U || !(hall >= 1 && hall <= 6))
  {
    Commutation_DisableAllPhases();
    last_hall_state = 0U;
    return;
  }

  {
    uint16_t arr = (uint16_t)(TIM1->ARR & 0xFFFFU);
    if (duty > arr)
    {
      duty = arr;
    }
  }

  phase = HALL_PHASE_TABLE[hall];

  if (hall != last_hall_state)
  {
    TIM1->CCER &= ~TIM1_PHASE_ENABLE_MASK;
    ApplyCommutation(phase, duty);
    TIM1->CCER |= PHASE_CCER_TABLE[phase];
    last_hall_state = hall;
    return;
  }

  ApplyCommutation(phase, duty);
}

uint8_t Commutation_GetLastHallState(void)
{
  return last_hall_state;
}
