#include "usbdisk_task.h"

void usbdisktask(void *pvParameters)
{
  usbd_init();
  
  usbd_connect(true);
  
  while(1)
  {
    if(USBD_MSC_MediaReady == __FALSE)
    {
      usbd_connect(false);
#ifdef USBSTACK_RTOS_FREE_RTOS
      USBD_RTOS_TaskDeInit();
#endif
      usbd_init();
      usbd_connect(true);
    }
    
    vTaskDelay(1000);
  
  }
    
  
}