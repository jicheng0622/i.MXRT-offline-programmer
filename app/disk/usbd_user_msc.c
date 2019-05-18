/* CMSIS-DAP Interface Firmware
 * Copyright (c) 2009-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <rl_usb.h>
#include <string.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "usb_buf.h"
#include "fsl_sd.h"
#include "fsl_sd_disk.h"
#include "ff.h"
#include "diskio.h"


static void SD_Insert_Callback(bool isInserted, void *userData);
static void SD_Removed_Callback(bool isInserted, void *userData);

FATFS g_fileSystem; /* File system object */
const TCHAR driverNumberBuffer[3U] = {SDDISK + '0', ':', '/'};

uint8_t test_flag;
const sdmmchost_detect_card_t sd_detect = {
                                           .cdType = kSDMMCHOST_DetectCardByGpioCD,
                                           .cdTimeOut_ms = portMAX_DELAY,
                                           .cardInserted = SD_Insert_Callback,
                                           .cardRemoved = SD_Removed_Callback
                                           };

void SD_Insert_Callback(bool isInserted, void *userData)
{
  if(true == isInserted)
  {
   printf("Card Inserted!\r\n");
  }
}

void SD_Removed_Callback(bool isInserted, void *userData)
{
  if(false == isInserted)
  {
   USBD_MSC_MediaReady = __FALSE;
   printf("Card Removed!\r\n");
  }
}

uint8_t USB_DeviceMscCardInit(void)
{
    status_t error = kStatus_Success;

    
    g_sd.host.base = SD_HOST_BASEADDR;
    g_sd.host.sourceClock_Hz = SD_HOST_CLK_FREQ;
    g_sd.usrParam.cd = &sd_detect;
    
    /* Init card. */
    if (SD_Init(&g_sd))
    {
        printf("SD card init failed! Make sure you insert SD Card correctlly?\r\n");
        error = kStatus_Fail;
    }
    
    if (f_mount(&g_fileSystem, driverNumberBuffer, 1U))
    {
        printf("Mount volume failed, Please try to re-insert sd-card!\r\n");
    }
    printf("SDDisk Fatfs file system is mounted successfully\r\n");
    
    return error;
}

void usbd_msc_init (void)
{
  
    if(kStatus_Success!=USB_DeviceMscCardInit())
    {
      USBD_MSC_MediaReady = __FALSE;
      
      return;
    }
  
    USBD_MSC_MemorySize = g_sd.blockCount*g_sd.blockSize;
    USBD_MSC_BlockSize  = 512;
    USBD_MSC_BlockGroup = 1;
    USBD_MSC_BlockCount = g_sd.blockCount;
    USBD_MSC_BlockBuf   = (uint8_t*)usb_buffer;
    
    USBD_MSC_MediaReady = __TRUE;
}
/* read */
void usbd_msc_read_sect (U32 block, U8 *buf, U32 num_of_blocks)
{
    if (USBD_MSC_MediaReady)
    {
      SD_ReadBlocks(&g_sd, buf, block, num_of_blocks);
    }
}
/* write */
void usbd_msc_write_sect (U32 block, U8 *buf, U32 num_of_blocks)
{
    if (USBD_MSC_MediaReady)
    {
      SD_WriteBlocks(&g_sd, buf, block, num_of_blocks);
    }
}