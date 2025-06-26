//#pragma once

#ifndef __CLOCKCONFIG_H
#define __CLOCKCONFIG_H

#include "stm32f7xx_hal.h"

void SystemClock_Config(void);
void CPU_CACHE_Enable(void);
void MPU_Config(void);

#endif
