/**
  ******************************************************************************
  * @file    pwm_config.h
  * @brief   TIM1 PWM period  single source of truth for ARR and duty scaling.
  ******************************************************************************
  * @note    Must match htim1.Init.Period in MX_TIM1_Init() (main.c). If you
  *          change Period in CubeMX, update TIM1_PWM_PERIOD here to match. thanks:))
  ******************************************************************************
  */

#ifndef PWM_CONFIG_H
#define PWM_CONFIG_H

#define TIM1_PWM_PERIOD 9999U


#define TIM1_MAX_DUTY   TIM1_PWM_PERIOD

#endif 