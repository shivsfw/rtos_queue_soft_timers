/*
 * task_handler.c
 *
 *  Created on: Jul 9, 2025
 *      Author: Shivek
 */

#include "main.h"

static void process_cmd(command_t *ttbn);


void tMenu(void *params) {
  while (1) {

  }
}

void tLED(void *params) {
  while (1) {
    ///HAL_GPIO_TogglePin(GPIOB, LD1_Pin);
    //vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void tRTC(void *params) {
  while (1) {

  }
}

void tPrintf(void *params) {
  while (1) {

  }
}

void tCmd(void *params) {
  TaskHandle_t taskToBeNotified;
  BaseType_t notification_status;
  command_t cmd;

  while (1) {
	  // Get the status of notification of current task.
	  notification_status = xTaskNotifyWait(0,0,NULL, pdMS_TO_TICKS(400));

	  //if notification was rxd
	  if(notification_status == true)		{
		  process_cmd(&cmd);
	  }
	  else	{
		  //Do nothing
	  }
  }
}

void process_cmd(command_t *cmd)	{
	uint8_t usr_byte;
	uint16_t i = 0;

	BaseType_t q_status;

	portENTER_CRITICAL();

	if(!uxQueueMessagesWaiting( qDataHandle ))	{
		return;
	}

	while((usr_byte != '\n') &
			(uxQueueMessagesWaiting( qDataHandle ) != 0 ))	{
		q_status = xQueueReceive(qDataHandle, &usr_byte, 0);
		if(q_status == pdTRUE)	{
			cmd->payload[i++] = usr_byte;
		}
	}
	cmd->payload[i++] = '\0';
	cmd->len = i;

	portEXIT_CRITICAL();

	switch(curr_state)	{
	case sMainMenu:
		xTaskNotify(tMenuHandle, (uint32_t)cmd, eSetValueWithOverwrite);
		break;

	case sLedEffect:
		xTaskNotify(tLEDHandle, (uint32_t)cmd, eSetValueWithOverwrite);
		break;

	case sRtcMenu:
	case sRtcTimeConfig:
	case sRtcDateConfig:
	case sRtcReport:
		xTaskNotify(tRTCHandle, (uint32_t)cmd, eSetValueWithOverwrite);
		break;
	}

	return;
}

