#ifndef BENCH_RUNNER_H
#define BENCH_RUNNER_H

#include <stdint.h>


void BenchRunner_Step_Production(void);

void BenchRunner_Step_IgnoreNonBrakeFaults(void);

void BenchRunner_Step_OpenLoopDuty(uint16_t duty);

#endif

