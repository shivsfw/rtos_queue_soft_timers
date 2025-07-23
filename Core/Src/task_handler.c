/*
 * task_handler.c
 *
 *  Created on: Jul 9, 2025
 *      Author: Shivek
 */

#include "main.h"

const char *msg_main_menu = "\r\n===================\r\n"
							"|_______Menu_______|\r\n"
//							"===================\r\n"
							"LED effect------> 0\r\n"
							"Date and time --> 1\r\n"
							"Exit------------> 2\r\n"
							"===================\r\n"
							"Enter your choice: ";

const char *msg_invalid = "\r\n=====InvalidM=======\r\n";
const char *msg_invalid1 = "\r\n=====InvalidL1=======\r\n";
const char *msg_invalid2 = "\r\n=====InvalidL2=======\r\n";
const char *msg_invalidR = "\r\n=====InvalidR=======\r\n";

const char *msg_led_menu =  "\r\n===================\r\n"
							"|_______LED_______|\r\n"
//							"===================\r\n"
							"none, e1, e2       \r\n"
							"===================\r\n"
							"Enter your choice: ";

const char *msg_rtc_menu =  "\r\n===================\r\n"
							"|_______RTC_______|\r\n"
//							"===================\r\n"
							"set time\r\n"
							"set date\r\n"
							"report time\r\n"
							"main menu"
							"show\r\n"
							"===================\r\n"
							"Enter your choice: ";


static void process_cmd(command_t *ttbn);


void tMenu(void *params) {
  uint32_t cmd_holder;
  command_t *cmd;
  uint8_t usr_selected =0;

  while (1) {
	  xQueueSend(qPrintHandle, &msg_main_menu, portMAX_DELAY);

	  xTaskNotifyWait(0,0,&cmd_holder, portMAX_DELAY);		//it has to block as it is waiting for the input,
	  	  //Todo: may be we can pass on to idle task here and free up the kernel.

	  cmd = (command_t*) cmd_holder;

	  if (cmd->len == 3)	{			//Valid command
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
			  xQueueSend(qPrintHandle, &msg_invalid, portMAX_DELAY);
			  continue;
		  }
	  }
	  else	{							//Invalid command
		  xQueueSend(qPrintHandle, &msg_invalid, portMAX_DELAY);
		  continue;
	  }

	  xTaskNotifyWait(0,0,&cmd_holder, portMAX_DELAY);
  }
}

void tLED(void *params) {
  uint32_t cmd_holder;
  command_t *cmd;



  while (1) {
	xTaskNotifyWait(0,0,&cmd_holder, portMAX_DELAY);		//wait for the user to select LED task
	xQueueSend(qPrintHandle, &msg_led_menu, portMAX_DELAY);	//send the initial message from LED task
	xTaskNotifyWait(0,0,&cmd_holder, portMAX_DELAY);		//wait for the user selection for LED pattern

	cmd = (command_t *)cmd_holder;
	if(cmd->len <= 4)	{
		if(!strcmp((char*)cmd->payload, "none\n"))
			led_effect(0);
		else if(!strcmp((char*)cmd->payload, "e1\n"))
			led_effect(1);
		else if(!strcmp((char*)cmd->payload, "e2\n"))
			led_effect(2);
		else
			xQueueSend(qPrintHandle, &msg_invalid1, portMAX_DELAY);
	} else	{
		xQueueSend(qPrintHandle, &msg_invalid2, portMAX_DELAY);
	}

	curr_state = sMainMenu;

	xTaskNotify(tMenuHandle, 0, eNoAction);

  }
}

void tRTC(void *params) {
  uint32_t cmd_holder;
  command_t *cmd;
  const char *msg_ask_hh = "\r\nEnter hour (1-12): ";
  const char *msg_ask_mm = "\r\nEnter minute (0-59): ";
  const char *msg_ask_ss = "\r\nEnter second (0-59): ";

  const char *msg_ask_dd = "\r\nEnter day (1-31): ";
  const char *msg_ask_mt = "\r\nEnter month (1-12): ";
  const char *msg_ask_yy = "\r\nEnter year (0-99): ";

  const char *msg_cnf = "\r\n Configured RTC \r\n";
  const char *msg_report = "\r\n Enable RTC reporting (y/n)? : \r\n";

  while (1)
  {
    xTaskNotifyWait(0, 0, &cmd_holder, portMAX_DELAY);		//wait for the user to select RTC task
    show_clock();						//Show current date and time
    xQueueSend(qPrintHandle, &msg_rtc_menu, portMAX_DELAY);	//send the initial message from RTC task
    xTaskNotifyWait(0, 0, &cmd_holder, portMAX_DELAY);		//wait for the user selection for RTC task

    cmd = (command_t*) cmd_holder;

    if(cmd->len > 3)
    {
      if (!strncmp ((char*) cmd->payload, "main menu\n", 9)){
	curr_state = sMainMenu;
	xTaskNotify(tMenuHandle, 0, eNoAction);
      }
      else if (!strncmp ((char*) cmd->payload, "set date\n", 8)){
	RTC_DateTypeDef userdate;
	RTC_TimeTypeDef settime;

	xQueueSend(qPrintHandle, &msg_ask_dd, portMAX_DELAY);	//Prompt for date
	xTaskNotifyWait(0, 0, &cmd_holder, portMAX_DELAY);	//wait for date input
	userdate.Date = getinput(cmd_holder);

	xQueueSend(qPrintHandle, &msg_ask_mt, portMAX_DELAY);	//Prompt for month
	xTaskNotifyWait(0, 0, &cmd_holder, portMAX_DELAY);	//wait for month input
	userdate.Month = getinput(cmd_holder);

	xQueueSend(qPrintHandle, &msg_ask_yy, portMAX_DELAY);	//Prompt for year
	xTaskNotifyWait(0, 0, &cmd_holder, portMAX_DELAY);	//wait for year input
	userdate.Year = getinput(cmd_holder);
	show_date_time(userdate, settime);

	//validate input
	if(input_valid(&userdate))	{
	    rtc_set_date(&userdate);
	    show_clock();
	}
      }
      else if (!strncmp ((char*) cmd->payload, "show\n", 4)){
	  show_clock();
      }
      else if (!strncmp ((char*) cmd->payload, "set time\n", 8)){
	RTC_DateTypeDef userdate;
	RTC_TimeTypeDef usertime;

	xQueueSend(qPrintHandle, &msg_ask_hh, portMAX_DELAY);	//Prompt for date
	xTaskNotifyWait(0, 0, &cmd_holder, portMAX_DELAY);	//wait for date input
	usertime.Hours = getinput(cmd_holder);

	xQueueSend(qPrintHandle, &msg_ask_mm, portMAX_DELAY);	//Prompt for month
	xTaskNotifyWait(0, 0, &cmd_holder, portMAX_DELAY);	//wait for month input
	usertime.Minutes = getinput(cmd_holder);
	usertime.Seconds = 0;

	show_date_time(userdate, usertime);

	//validate input
	if(time_valid(&usertime))	{
	    rtc_set_time(&usertime);
	    show_clock();
	}
      }
      else	{
	xQueueSend(qPrintHandle, &msg_invalid2, portMAX_DELAY);
	curr_state = sMainMenu;
	xTaskNotify(tMenuHandle, 0, eNoAction);
      }
    }
    else
    {
      xQueueSend(qPrintHandle, &msg_invalid2, portMAX_DELAY);
      curr_state = sMainMenu;
      xTaskNotify(tMenuHandle, 0, eNoAction);
    }
  }	//while 1 ends
}

void tPrintf(void *params) {
  uint32_t *msg = NULL;
  while (1) {
	  xQueueReceive(qPrintHandle, &msg, portMAX_DELAY);
	  HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
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
	  if(notification_status == pdTRUE)		{
		  process_cmd(&cmd);
	  }
	  else	{
		  //Do nothing
	  }
  }
}

void process_cmd(command_t *cmd)	{
  uint8_t usr_byte = 0;
  uint16_t i = 0;

  BaseType_t q_status;

  //portENTER_CRITICAL();

  if(!uxQueueMessagesWaiting( qDataHandle ))	{
	  return;
  }

  //TODO: implement backspace in user input
  while((usr_byte != '\n') &&
		  (uxQueueMessagesWaiting( qDataHandle ) != 0 ))	{
	  q_status = xQueueReceive(qDataHandle, &usr_byte, 0);
	  if(q_status == pdTRUE)	{
		  cmd->payload[i++] = usr_byte;
	  }
  }
  cmd->payload[i++] = '\0';
  cmd->len = i;

  //portEXIT_CRITICAL();

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
}


