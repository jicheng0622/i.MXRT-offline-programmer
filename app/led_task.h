#ifndef __LEDTASK_H__
#define __LEDTASK_H__

#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


typedef enum{
LED_STANDBY =0,
LED_PROGRAMMING = 1,
LED_ERROR = 2
}Display_Status;

extern QueueHandle_t blsh_led_TaskQue;

extern void ledtask(void *pvParameters);


#endif