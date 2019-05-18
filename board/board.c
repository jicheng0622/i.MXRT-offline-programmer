/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board.h"
#include <stdint.h>
#include "fsl_common.h"

#ifdef FSL_RTOS_FREE_RTOS
uart_rtos_handle_t debug_uart_rtos_handler;
uart_handle_t debug_uart_handler;
uint8_t debug_uart_ringbuf;
#endif

void BOARD_InitDebugConsole(void)
{
#ifdef FSL_RTOS_FREE_RTOS
  uart_rtos_config_t config;
  
  config.base        = BOARD_DEBUG_UART_BASEADDR;
  config.srcclk      = CLOCK_GetFreq(BOARD_DEBUG_UART_CLKSRC);
  config.baudrate    = BOARD_DEBUG_UART_BAUDRATE;
  config.parity      = kUART_ParityDisabled;
  config.stopbits    = kUART_OneStopBit;
  config.buffer      = &debug_uart_ringbuf;
  config.buffer_size = 1;
  
  UART_RTOS_Init(&debug_uart_rtos_handler, &debug_uart_handler, &config);
  NVIC_EnableIRQ(BOARD_DEBUG_UART_IRQ);
#else
  uart_config_t config;

  UART_GetDefaultConfig(&config);
  config.baudRate_Bps = 115200;
  config.enableTx = true;
  config.enableRx = true;

  UART_Init(BOARD_DEBUG_UART_BASEADDR, &config, CLOCK_GetFreq(BOARD_DEBUG_UART_CLKSRC));
#endif
}

void BOARD_InitSwitch(void)
{
    gpio_pin_config_t switch_config = { kGPIO_DigitalInput, 0 };

    GPIO_PinInit(BOARD_SW2_GPIO, BOARD_SW2_GPIO_PIN, &switch_config);
}

bool BOARD_ReadSwitch(void)
{
   return !(GPIO_PinRead(BOARD_SW2_GPIO, BOARD_SW2_GPIO_PIN));
}

void BOARD_InitLED(void)
{
    gpio_pin_config_t LED_config = { kGPIO_DigitalOutput, 1 };
    GPIO_PinInit(BOARD_LED_GREEN_GPIO, BOARD_LED_GREEN_GPIO_PIN, &LED_config);
    GPIO_PinInit(BOARD_LED_RED_GPIO, BOARD_LED_RED_GPIO_PIN, &LED_config);
    GPIO_PinInit(BOARD_LED_BLUE_GPIO, BOARD_LED_BLUE_GPIO_PIN, &LED_config);
}
