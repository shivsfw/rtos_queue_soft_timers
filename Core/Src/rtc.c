/*
 * rtc.c
 *
 *  Created on: Jul 9, 2025
 *      Author: Shivek
 */
#include "main.h"
bool time_valid(RTC_TimeTypeDef *refTime)	{
  if((refTime->Hours > 24) || (refTime->Minutes > 59))	{
      return false;
  }
  return true;
}

bool input_valid(RTC_DateTypeDef *refDate)	{
  switch(refDate->Month)	{
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:

      //check for month
      if(refDate->Date <= 0 || refDate->Date > 31)	{
	  return false;
      }
      break;
    case 2:
      //check for month
      if(refDate->Date <= 0 || refDate->Date > 29)	{
	  return false;
      }
      break;
    case 4:
    case 6:
    case 9:
    case 11:
      //check for month
      if(refDate->Date <= 0 || refDate->Date > 30)	{
      	  return false;
      }
      break;
    default:
      return false;
  }
  return true;
}

uint8_t getinput(uint32_t cmd_holder){
  uint8_t retval = 0;
  command_t *cmd;
  cmd = (command_t*) cmd_holder;

  //Validate each char for 0 to 9
  for(int i = 0; i < (cmd->len - 2); i++ )
  {
    if((cmd->payload[i] < 48) || (cmd->payload[i] > 57))	{
	return retval;
    }
  }

  if(cmd->len > 3)	{
      retval = (10*(cmd->payload[0] - 48)) + (cmd->payload[1] - 48);
  }
  else {
      retval = (cmd->payload[0] - 48);
  }
  return retval;
}

void show_date_time(RTC_DateTypeDef setdate, RTC_TimeTypeDef settime)	{
  static char showtime[40], showdate[40];

  static char *timestr = showtime;
  static char *datestr = showdate;
  char *format;

  memset(showtime, 0, sizeof(showtime));
  memset(showdate, 0, sizeof(showdate));


  format = (settime.TimeFormat == RTC_HOURFORMAT12_AM) ? "AM" : "PM";

  sprintf((char *)showdate, "\r\n Current date \t%02d-%02d-%02d", setdate.Month, setdate.Date, setdate.Year);
  xQueueSend(qPrintHandle, &datestr, portMAX_DELAY);

  sprintf((char *)showtime, "\r\n Current time \t%02d:%02d:%02d %s\r\n", settime.Hours, settime.Minutes, settime.Seconds, format);
  xQueueSend(qPrintHandle, &timestr, portMAX_DELAY);
}

void show_clock(){
  RTC_DateTypeDef setdate;
  RTC_TimeTypeDef settime;

//  HAL_RTC_GetDate(&hrtc, &setdate, RTC_FORMAT_BIN);
//  HAL_RTC_GetTime(&hrtc, &settime, RTC_FORMAT_BIN);


  rtc_get_time(&settime);
  rtc_get_date(&setdate);

  show_date_time(setdate, settime);
}

void rtc_set_time(RTC_TimeTypeDef *time)	{
  HAL_StatusTypeDef stat;
  time->TimeFormat = RTC_HOURFORMAT12_AM;
  HAL_RTC_DST_ClearStoreOperation(&hrtc);

  stat = HAL_RTC_SetTime(&hrtc, time, RTC_FORMAT_BIN);
  HAL_RTC_DST_Sub1Hour(&hrtc);
  if(stat == HAL_OK)	{
      return;
  }
  else	{
      while(1);
  }
}

void rtc_set_date(RTC_DateTypeDef *date)	{
  HAL_StatusTypeDef stat;
  date->WeekDay  = RTC_WEEKDAY_WEDNESDAY;
  stat = HAL_RTC_SetDate(&hrtc, date, RTC_FORMAT_BIN);
  if(stat == HAL_OK)	{
      return;
  }
  else	{
      while(1);
  }
}

void rtc_get_date(RTC_DateTypeDef *date)	{
  HAL_StatusTypeDef stat;
  stat = HAL_RTC_GetDate(&hrtc, date, RTC_FORMAT_BIN);
  if(stat == HAL_OK)	{
      return;
  }
  else	{
      while(1);
  }
}

void rtc_get_time(RTC_TimeTypeDef *time)	{
  HAL_StatusTypeDef stat;
  stat = HAL_RTC_GetTime(&hrtc, time, RTC_FORMAT_BIN);
  if(stat == HAL_OK)	{
      return;
  }
  else	{
      while(1);
  }
}
