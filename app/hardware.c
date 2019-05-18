/*
 * Copyright (c) 2013 - 2014, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "hardware.h"
#include "fsl_device_registers.h"
#include "pin_mux.h"
#include "board.h"
#include "fsl_uart.h"
#include "fsl_sdmmc_host.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief uart byte receive callback.
 */
static void BOARD_InitHostUart(void);
#ifndef FSL_RTOS_FREE_RTOS
static void s_uart_byte_receive_callback(uint8_t byte);
#endif
/*******************************************************************************
 * Variables
 ******************************************************************************/
#ifdef FSL_RTOS_FREE_RTOS
uart_rtos_handle_t host_uart_rtos_handler;
uart_handle_t host_uart_handler;
uint8_t host_uart_ringbuf[256];
#else
/* Uart1 */
static uint8_t *s_uart_rxData;
static uint32_t s_uart_bytesRx;
/* Uart1 buffer */
static uint8_t s_uartBuffer[256];
static uint8_t *s_uartPtr = s_uartBuffer;
static uint32_t s_uartBufferOffset = 0;
#endif
/*******************************************************************************
 * Code
 ******************************************************************************/

void hardware_init(void)
{
    /* Disable the MPU otherwise USB cannot access the bus */
    /* Disable the SYSMPU globally. */
    SYSMPU->CESR &= ~SYSMPU_CESR_VLD_MASK;
    BOARD_BootClockRUN();
    SystemCoreClockUpdate();
    BOARD_InitDebugConsole();
    BOARD_InitPins();
    BOARD_InitLED();
    BOARD_InitSwitch();
    BOARD_InitHostUart();
    
    NVIC_SetPriority(BOARD_DEBUG_UART_IRQ, configLIBRARY_LOWEST_INTERRUPT_PRIORITY-1);
    NVIC_SetPriority(BOARD_HOST_UART_IRQ, configLIBRARY_LOWEST_INTERRUPT_PRIORITY-2);
    NVIC_SetPriority(USB0_IRQn, configLIBRARY_LOWEST_INTERRUPT_PRIORITY-3);
    NVIC_SetPriority(SD_HOST_IRQ, configLIBRARY_LOWEST_INTERRUPT_PRIORITY-4);
    
}

static void BOARD_InitHostUart(void)
{
  
#ifdef FSL_RTOS_FREE_RTOS
  uart_rtos_config_t config;
  
  config.base        = BOARD_HOST_UART_BASEADDR;
  config.srcclk      = CLOCK_GetFreq(BOARD_HOST_UART_CLKSRC);
  config.baudrate    = BOARD_HOST_UART_BAUDRATE;
  config.parity      = kUART_ParityDisabled;
  config.stopbits    = kUART_OneStopBit;
  config.buffer      = host_uart_ringbuf;
  config.buffer_size = sizeof(host_uart_ringbuf);
  
  UART_RTOS_Init(&host_uart_rtos_handler, &host_uart_handler, &config);
  NVIC_EnableIRQ(BOARD_HOST_UART_IRQ);
#else
    uart_config_t config;

    UART_GetDefaultConfig(&config);
    config.baudRate_Bps = BOARD_HOST_UART_BAUDRATE;
    config.enableTx = true;
    config.enableRx = true;

    UART_Init(BOARD_HOST_UART_BASEADDR, &config, CLOCK_GetFreq(BOARD_HOST_UART_CLKSRC));

    EnableIRQ(BOARD_HOST_UART_IRQ);
    UART_EnableInterrupts(BOARD_HOST_UART_BASEADDR, kUART_RxDataRegFullInterruptEnable);
#endif
}

bool wait_uart_char(uint8_t *data)
{
#ifdef FSL_RTOS_FREE_RTOS
  return !UART_RTOS_Receive(&debug_uart_rtos_handler, data, 1, NULL, portMAX_DELAY);
#else
  /* If has data */
  if (BOARD_HOST_UART_BASEADDR->RCFIFO)
  {
      *data = UART_ReadByte(BOARD_HOST_UART_BASEADDR);
      return true;
  }
  else
  {
      return false;
  }
#endif
}

uint8_t wait_uart_char_blocking(void)
{
  uint8_t byte = 0;
#ifdef FSL_RTOS_FREE_RTOS
    UART_RTOS_Receive(&debug_uart_rtos_handler, &byte, 1, NULL, portMAX_DELAY);
#else
    UART_ReadBlocking(BOARD_HOST_UART_BASEADDR, &byte, 1);
#endif
    return byte;
}

void configure_uart_speed(uint32_t baud)
{
    uart_config_t config;

    UART_GetDefaultConfig(&config);
    config.baudRate_Bps = baud;
    config.enableTx = true;
    config.enableRx = true;

    UART_Init(BOARD_HOST_UART_BASEADDR, &config, CLOCK_GetFreq(BOARD_HOST_UART_CLKSRC));
}

status_t send_uart_data(uint8_t *src, uint32_t writeLength)
{
#ifdef FSL_RTOS_FREE_RTOS
    UART_RTOS_Send(&host_uart_rtos_handler, src, writeLength);
#else
    UART_WriteBlocking(BOARD_HOST_UART_BASEADDR, src, writeLength);
#endif
    
    return kStatus_Success;
}

void clear_uart_buffer(void)
{
#ifdef FSL_RTOS_FREE_RTOS
  host_uart_handler.rxRingBufferHead = 0;
  host_uart_handler.rxRingBufferTail = 0;
#else
  s_uartBufferOffset = 0;
  s_uart_bytesRx = 0;
#endif
}

status_t receive_uart_data(uint8_t *dest, uint32_t readLength)
{
#ifdef FSL_RTOS_FREE_RTOS
    return UART_RTOS_Receive(&host_uart_rtos_handler, dest, readLength, NULL, 1000*((float)configTICK_RATE_HZ/1000.0f));//timeout 1s
#else
    uint16_t timeout = UINT16_MAX;

    while ((((s_uart_bytesRx + k_uartBufferSize - s_uartBufferOffset) % k_uartBufferSize) < readLength) && (--timeout))
    {
    }

    if (timeout == 0)
    {
        return kStatus_Timeout;
    }

    for (uint32_t i = 0; i < readLength; i++)
    {
        dest[i] = s_uartPtr[(s_uartBufferOffset + i) % k_uartBufferSize];
    }
    s_uartBufferOffset = (s_uartBufferOffset + sizeof(uint8_t) * readLength) % k_uartBufferSize;

    return kStatus_Success;
#endif
}

#ifndef FSL_RTOS_FREE_RTOS
void BOARD_HOST_UART_IRQ_HANDLER(void)
{
    if (BOARD_HOST_UART_BASEADDR->S1 & UART_S1_RDRF_MASK)
    {
        s_uart_byte_receive_callback(BOARD_HOST_UART_BASEADDR->D);
    }
}
static void s_uart_byte_receive_callback(uint8_t byte)
{
    s_uart_rxData = s_uartPtr;
    if (s_uart_rxData)
    {
        s_uart_rxData[s_uart_bytesRx] = byte;
        s_uart_bytesRx = (s_uart_bytesRx + 1) % k_uartBufferSize;
    }
    s_uart_rxData = 0;
}
#endif