/*
 * task_handler.c
 *
 *  Created on: Jul 9, 2025
 *      Author: Shivek
 */

#include "main.h"

const char *msg_main_menu = "===================\n"
							"|		Menu	   |\n"
							"===================\n"
							"LED effect    --> 0\n"
							"Date and time --> 1\n"
							"Exit		   --> 2\n"
							"===================\n"
							"Enter your choice: ";

const char *msg_invalid = "=====Invalid=======\n";

const char *msg_led_menu =  "===================\n"
							"|		LED 	   |\n"
							"===================\n"
							"none, e1, e2       \n"
							"===================\n"
							"Enter your choice: ";


static void process_cmd(command_t *ttbn);


void tMenu(void *params) {
  uint32_t cmd_holder;
  command_t *cmd;
  uint8_t usr_selected =0;

  while (1) {
	  xQueueSend(qPrintHandle, msg_main_menu, portMAX_DELAY);

	  xTaskNotifyWait(0,0,&cmd_holder, portMAX_DELAY);		//it has to block as it is waiting for the input,
	  	  //Todo: may be we can pass on to idle task here and free up the kernel.

	  cmd = (command_t*) cmd_holder;

	  if (cmd->len == 1)	{			//Valid command
		  usr_selected = cmd->payload[0] - 48;
		  switch(usr_selected){
		  case 0: 		//LED
			  curr_state = sLedEffect;
			  xTaskNotify(tLEDHandle, 0, eNoAction);
			  break;
		  case 1:		//Date and Time
			  curr_state = sRtcMenu;
			  xTaskNotify(tRTCHandle, 0, eNoAction);
			  break;
		  case 2:		//Exit
			  //curr_state = sMainMenu;
			  //xTaskNotifyWait(0,0,&cmd_holder, portMAX_DELAY);
			  break;
		  default:		//
			  xQueueSend(qPrintHandle, msg_invalid, portMAX_DELAY);
			  continue;
		  }
	  }
	  else	{							//Invalid command
		  xQueueSend(qPrintHandle, msg_invalid, portMAX_DELAY);
	  }

	  xTaskNotifyWait(0,0,&cmd_holder, portMAX_DELAY);
  }
}

void tLED(void *params) {
  uint32_t cmd_holder;
  command_t *cmd;
  uint8_t usr_selected =0;
  while (1) {
	xTaskNotifyWait(0,0,&cmd_holder, portMAX_DELAY);		//wait for the user to select LED task
	xQueueSend(qPrintHandle, msg_led_menu, portMAX_DELAY);	//send the initial message from LED task
	xTaskNotifyWait(0,0,&cmd_holder, portMAX_DELAY);		//wait for the user selection for LED pattern

	cmd = (command_t *)cmd_holder;
	if(cmd->len <= 4)	{
		if(!strcmp((char*)cmd->payload, "none"))
			led_effect(0);
		else if(!strcmp((char*)cmd->payload, "e1"))
			led_effect(1);
		else if(!strcmp((char*)cmd->payload, "e2"))
			led_effect(2);
		else
			xQueueSend(qPrintHandle, msg_invalid, portMAX_DELAY);
	} else	{
		xQueueSend(qPrintHandle, msg_invalid, portMAX_DELAY);
	}

	curr_state = sMainMenu;

	xTaskNotify(tMenuHandle, 0, eNoAction);

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
	  notification_status = xTaskNotifyWait(0,0,NULL, portMAX_DELAY);

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


