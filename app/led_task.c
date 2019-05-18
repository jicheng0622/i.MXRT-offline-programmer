#include "led_task.h"

QueueHandle_t blsh_led_TaskQue;

void ledtask(void *pvParameters)
{
  Display_Status display_state_machine = LED_STANDBY;
  
  blsh_led_TaskQue = xQueueCreate(1, sizeof(uint8_t));
    
  /* Turn off all 3 LEDs firstly */
  LED_RED_INIT(1);LED_GREEN_INIT(1);LED_BLUE_INIT(1);
  
  while(1)
  {
    switch(display_state_machine)
    {
      case LED_PROGRAMMING:
                        LED_BLUE_INIT(1);LED_RED_INIT(1);LED_GREEN_TOGGLE();//LED_BLUE Off, LED_RED Off, LED_GREEN Toggle
                        xQueueReceive(blsh_led_TaskQue, &display_state_machine,50);
                        break;
      case LED_ERROR:
                        LED_BLUE_INIT(1);LED_RED_INIT(0);LED_GREEN_INIT(1);//LED_BLUE Off, LED_RED Off, LED_GREEN Off
                        xQueueReceive(blsh_led_TaskQue, &display_state_machine,1000);
                        break;      
      case LED_STANDBY:
      default:          
                        LED_BLUE_INIT(1);LED_RED_INIT(1);LED_GREEN_TOGGLE();//LED_BLUE Off, LED_RED Off, LED_GREEN Toggle
                        xQueueReceive(blsh_led_TaskQue, &display_state_machine,1000);
                        break;
    }
  }

}
