# STM32 Peripheral Configurations

---

## TIM3 — ADC Timer

| Parameter | Value |
|---|---|
| Prescaler | 83 |
| Period (ARR) | 9999 |
| Update Frequency | 200 Hz (5 ms) |

---

## ADC1

Triggered by TIM3.

| Parameter | Value |
|---|---|
| Pin | PC0 (CN7 pin 38) |
| Channel | IN6 |
| Sample Rate | 200 Hz |
| Mode | Interrupt |
| Purpose | Throttle position |

---
