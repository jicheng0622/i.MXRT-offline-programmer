#include <stdarg.h>
#include <stdlib.h>
#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
#include <stdio.h>
#endif

#ifdef FSL_RTOS_FREE_RTOS
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#endif

#include "fsl_uart.h"
#include "fsl_common.h"
#include "board.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define MyLowLevelSendData(buf, size)  UART_WriteBlocking(BOARD_DEBUG_UART_BASEADDR, buf, size)
#define MyLowLevelReadData(buf, size)  UART_ReadBlocking(BOARD_DEBUG_UART_BASEADDR, buf, size)

#ifndef NDEBUG
#undef assert
#define assert(n)
#endif

/*! @brief character backspace ASCII value */
#define DEBUG_CONSOLE_BACKSPACE 127

/*************Code to support toolchain's printf, scanf *******************************/
/* These function __write and __read is used to support IAR toolchain to printf and scanf*/
#if (defined(__ICCARM__))
#pragma weak __write
size_t __write(int handle, const unsigned char *buffer, size_t size)
{
    if (buffer == 0)
    {
        /*
         * This means that we should flush internal buffers.  Since we don't we just return.
         * (Remember, "handle" == -1 means that all handles should be flushed.)
         */
        return 0;
    }

    /* This function only writes to "standard out" and "standard err" for all other file handles it returns failure. */
    if ((handle != 1) && (handle != 2))
    {
        return ((size_t)-1);
    }

    /* Send data. */
    MyLowLevelSendData((uint8_t *)buffer, size);

    return size;
}

#pragma weak __read
size_t __read(int handle, unsigned char *buffer, size_t size)
{
    /* This function only reads from "standard in", for all other file  handles it returns failure. */
    if (handle != 0)
    {
        return ((size_t)-1);
    }

    /* Receive data.*/
    MyLowLevelReadData(buffer, size);

    return size;
}

/* support LPC Xpresso with RedLib */
#elif(defined(__REDLIB__))
int __attribute__((weak)) __sys_write(int handle, char *buffer, int size)
{
    if (buffer == 0)
    {
        /* return -1 if error. */
        return -1;
    }

    /* This function only writes to "standard out" and "standard err" for all other file handles it returns failure. */
    if ((handle != 1) && (handle != 2))
    {
        return -1;
    }

    /* Send data. */
    MyLowLevelSendData((uint8_t *)buffer, size);

    return 0;
}

int __attribute__((weak)) __sys_readc(void)
{
    char tmp;

    /* Receive data. */
    MyLowLevelGetChar((uint8_t *)&tmp);

    return tmp;
}

/* These function __write and __read is used to support ARM_GCC, KDS, Atollic toolchains to printf and scanf*/
#elif(defined(__GNUC__))
int __attribute__((weak)) _write(int handle, char *buffer, int size)
{
    if (buffer == 0)
    {
        /* return -1 if error. */
        return -1;
    }

    /* This function only writes to "standard out" and "standard err" for all other file handles it returns failure. */
    if ((handle != 1) && (handle != 2))
    {
        return -1;
    }

    /* Send data. */
    MyLowLevelSendData((uint8_t *)buffer, size);

    return size;
}

int __attribute__((weak)) _read(int handle, char *buffer, int size)
{
    /* This function only reads from "standard in", for all other file handles it returns failure. */
    if (handle != 0)
    {
        return -1;
    }

    /* Receive data. */
    return MyLowLevelReadData(buffer, size);
}

/* These function fputc and fgetc is used to support KEIL toolchain to printf and scanf*/
#elif defined(__CC_ARM) || defined(__ARMCC_VERSION)
struct __FILE
{
    int handle;
    /*
     * Whatever you require here. If the only file you are using is standard output using printf() for debugging,
     * no file handling is required.
     */
};
/* FILE is typedef in stdio.h. */
#pragma weak __stdout
#pragma weak __stdin
FILE __stdout;
FILE __stdin;

#pragma weak fputc
int fputc(int ch, FILE *f)
{
    /* Send data. */
    return MyLowLevelPutChar(buffer, size);
}

#pragma weak fgetc
int fgetc(FILE *f)
{
    char ch;

    /* Receive data. */
    MyLowLevelGetChar((uint8_t *)&ch);

    return ch;
}
#endif /* __ICCARM__ */
