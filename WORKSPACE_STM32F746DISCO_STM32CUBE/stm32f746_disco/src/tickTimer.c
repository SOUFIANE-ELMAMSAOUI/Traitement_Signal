#include "tickTimer.h"
TIM_HandleTypeDef    TimHandle_period;

//================================================
//		TIMER 3 INIT : FAST TRACK UPDATE
//================================================

void tickTimerInit(uint16_t period)
{
	unsigned int uwPrescalerValue;


	 __TIM5_CLK_ENABLE();

/* Compute the prescaler value to have TIM5 counter clock equal to 10 KHz */
  uwPrescalerValue = (unsigned int) ((SystemCoreClock / 10000) - 1);

  TimHandle_period.Instance = TIM5;
  TimHandle_period.Init.Period = 10*(period);
  TimHandle_period.Init.Prescaler = uwPrescalerValue/2;
  TimHandle_period.Init.ClockDivision = 0;
  TimHandle_period.Init.CounterMode = TIM_COUNTERMODE_UP;
  HAL_TIM_Base_Init(&TimHandle_period);

	HAL_NVIC_SetPriority(TIM5_IRQn, 10, 0);
	HAL_NVIC_EnableIRQ(TIM5_IRQn);


  HAL_TIM_Base_Start_IT(&TimHandle_period);





}

//================================================
