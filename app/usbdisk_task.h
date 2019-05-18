#ifndef __USBDISK_TASK_H__
#define __USBDISK_TASK_H__

#include "board.h"
#include "FreeRTOS.h"
#include "task.h"

#include "rl_usb.h"

extern void usbdisktask(void *pvParameters);

#endif
