#include "bench_runner.h"
#include "sensors.h"
#include "motor_control.h"
#include "safety.h"
#include "commutation.h"

void BenchRunner_Step_Production(void)
{
  Sensors_Update();
  Safety_Update();
  MotorControl_Update(Safety_IsBrakeActive(), Safety_IsFaultActive());
  Commutation_Update(MotorControl_GetDuty());
}

void BenchRunner_Step_IgnoreNonBrakeFaults(void)
{
  Sensors_Update();
  Safety_Update();
  MotorControl_Update(Safety_IsBrakeActive(), 0U);
  Commutation_Update(MotorControl_GetDuty());
}

void BenchRunner_Step_OpenLoopDuty(uint16_t duty)
{
  Sensors_Update();
  Safety_Update();
  Commutation_Update(duty);
}
