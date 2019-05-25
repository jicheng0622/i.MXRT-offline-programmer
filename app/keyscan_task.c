#include "keyscan_task.h"
#include "led_task.h"
#include "multi_button.h"
#include "host_blsh_cmd.h"
#include "blsh.h"
#include "stdio.h"

uint8_t* sdp_get_status[]={"sdp-get-status","\0"};
uint8_t* sdp_write_file_RT1050_1060[]={"sdp-write-file","0x20000000","2:/flashloader/ivt_flashloader.bin"};
uint8_t* sdp_jump_address_RT1050_1060[]={"sdp-jump-address","0x20000400"};
uint8_t* sdp_write_file_RT1020[]={"sdp-write-file","0x20208000","2:/flashloader/ivt_flashloader.bin"};
uint8_t* sdp_jump_address_RT1020[]={"sdp-jump-address","0x20208400"};
uint8_t* blhost_get_property[]={"get-property","1"};
uint8_t* blhost_receive_sb_file[]={"receive-sb-file","2:/image/boot_image.sb"};
uint8_t* blhost_reset[]={"reset","\0"};

struct Button btn1;

uint8_t read_button1_GPIO(void) 
{
  return (uint8_t)GPIO_PinRead(BOARD_SW2_GPIO, BOARD_SW2_GPIO_PIN);
}

void BTN1_PRESS_UP_Handler(void* btn)
{
  uint32_t btn_event;
  
  printf("\r\nBTN1 press up event!\r\n");
  
  btn_event = LED_PROGRAMMING;
  xQueueSend(blsh_led_TaskQue, ( void * ) &btn_event, ( TickType_t ) 0 ); 
  /*********************Do programming process*********************/
  if(kStatus_Success != sdp_get_status_func(1, sdp_get_status))
    goto error_handler;
  if(kStatus_Success != sdp_write_file_func(3, sdp_write_file_RT1050_1060))
    goto error_handler;
  if(kStatus_Success != sdp_jump_address_func(2, sdp_jump_address_RT1050_1060))
    goto error_handler;
  vTaskDelay(500);//Reserve enough time to wait BLHOST to run(expeciall for security flashloader)
  if(kStatus_Success != get_property_func(2, blhost_get_property))
    goto error_handler;
  if(kStatus_Success != receive_sb_file_func(2, blhost_receive_sb_file))
    goto error_handler;
  if(kStatus_Success != target_reset_func(1, blhost_reset))
    goto error_handler;
  /*****************************************************************/
  printf("\r\nProgramming finished successfully!\r\n");
  btn_event = LED_STANDBY;
  xQueueSend(blsh_led_TaskQue, ( void * ) &btn_event, ( TickType_t ) 0 );
  return;
  
error_handler:
  printf("\r\nProgramming failed!\r\n");
  btn_event = LED_ERROR;
  xQueueSend(blsh_led_TaskQue, ( void * ) &btn_event, ( TickType_t ) 0 );
  return;
}

void keyscantask(void *pvParameters)
{
  button_init(&btn1, read_button1_GPIO, 0);
  button_attach(&btn1, PRESS_UP, BTN1_PRESS_UP_Handler);
  button_start(&btn1);
  
  while(1)
  {
    vTaskDelay(5);
    button_ticks();
  }
  
}
