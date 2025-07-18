/*
 * led_effect.c
 *
 *  Created on: Jul 9, 2025
 *      Author: Shivek
 */
#include "main.h"

void led_effect(uint8_t pattern)	{
	switch(pattern)	{
	case 0:
		HAL_GPIO_WritePin(LED_PORT, LED_GREEN_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED_PORT, LED_BLUE_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED_PORT, LED_RED_Pin, GPIO_PIN_RESET);

		xTimerStop(tr200msHandle, 0);
		xTimerStop(tr500msHandle, 0);
		break;
	case 1:
		xTimerStop(tr500msHandle, 0);

		if( xTimerIsTimerActive( tr200msHandle ) != pdFALSE )	{
			HAL_GPIO_WritePin(LED_PORT, LED_GREEN_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LED_PORT, LED_BLUE_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LED_PORT, LED_RED_Pin, GPIO_PIN_SET);
			xTimerStart(tr200msHandle, 0);
		}
		break;
	case 2:
		xTimerStop(tr200msHandle, 0);

		if( xTimerIsTimerActive( tr500msHandle ) != pdFALSE )	{
			HAL_GPIO_WritePin(LED_PORT, LED_GREEN_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LED_PORT, LED_BLUE_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(LED_PORT, LED_RED_Pin, GPIO_PIN_RESET);
			xTimerStart(tr500msHandle, 0);
		}
		break;
	default:
		HAL_GPIO_WritePin(LED_PORT, LED_GREEN_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED_PORT, LED_BLUE_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED_PORT, LED_RED_Pin, GPIO_PIN_SET);
		break;
	}
}

void timerLedeffect(TimerHandle_t expTimer){
	HAL_GPIO_TogglePin(LED_PORT, LED_GREEN_Pin);
	HAL_GPIO_TogglePin(LED_PORT, LED_BLUE_Pin);
	HAL_GPIO_TogglePin(LED_PORT, LED_RED_Pin);
}

