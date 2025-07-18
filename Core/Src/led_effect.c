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
		break;
	case 1:
		HAL_GPIO_WritePin(LED_PORT, LED_GREEN_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED_PORT, LED_BLUE_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED_PORT, LED_RED_Pin, GPIO_PIN_RESET);
		break;
	case 2:
		HAL_GPIO_WritePin(LED_PORT, LED_GREEN_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED_PORT, LED_BLUE_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED_PORT, LED_RED_Pin, GPIO_PIN_RESET);
		break;
	default:
		HAL_GPIO_WritePin(LED_PORT, LED_GREEN_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED_PORT, LED_BLUE_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED_PORT, LED_RED_Pin, GPIO_PIN_RESET);
		break;
	}
}
