#include "fsl_device_registers.h"
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "hardware.h"
#include "FreeRTOS.h"
#include "task.h"

#include "keyscan_task.h"
#include "led_task.h"
#include "usbdisk_task.h"
#include "blsh_task.h"

void main(void)
{
  hardware_init();
 
  printf("i.MXRT offline programmer project start!\r\n");
  
  if(pdPASS != xTaskCreate(ledtask, "ledtask", configMINIMAL_STACK_SIZE + 40, NULL, configMAX_PRIORITIES - 6, NULL))
  {
    printf("led Task creation failed!.\r\n");
    while (1);
  }
  printf("led task is created!\r\n");
  /* configMAX_PRIORITIES-3, -4, -5 is used for USB Core, Device and Endpoint priprity in usb_lib.c */
  if(pdPASS != xTaskCreate(usbdisktask, "usbdisktask", configMINIMAL_STACK_SIZE + 200, NULL, configMAX_PRIORITIES - 2, NULL))
  {
    printf("usbdisk Task creation failed!.\r\n");
    while (1);
  }
  printf("usbdisk task is created!\r\n");
  
  /* Blash Task's stack must be set as enough because of some buffers are used inside */
  if(pdPASS != xTaskCreate(blshtask, "blshtask", configMINIMAL_STACK_SIZE + 1000, NULL, configMAX_PRIORITIES - 1, NULL))
  {
    printf("blsh Task creation failed!.\r\n");
    while (1);
  }
  printf("blshtask is created!\r\n");
  
  if(pdPASS != xTaskCreate(keyscantask, "keyscantask", configMINIMAL_STACK_SIZE + 1000, NULL, configMAX_PRIORITIES, NULL))
  {
    printf("keyscan Task creation failed!.\r\n");
    while (1);
  }
  printf("keyscan task is created!\r\n");
  
  vTaskStartScheduler();
   
  while(1)
  {  
  }
  
}