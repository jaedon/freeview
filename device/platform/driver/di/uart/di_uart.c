/**
 * di_uart.c
*/

/**
 * @defgroup		DI_UART : DI_UART module
 * @author
 * @note			DI_UART APIs
 * @brief			Define DI_UART APIs
 * @file 			di_uart.c
*/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#include 	<termio.h>
#endif
#include <stdarg.h>

#include <ctype.h>

#include "vkernel.h"
#include "di_err.h"
#include "drv_err.h"
#include "di_uart.h"

DI_ERR_CODE DI_UART_Init(void)
{
	return DI_ERR_OK;
}

DI_ERR_CODE DI_UART_Write(void *data, HUINT32 numToWrite)
{
	HINT8 *psz = data;
	HUINT32 i;

	for(i=0;i < numToWrite;++i)
	{
		VK_putchar(psz[i]);
	}
	return DI_ERR_OK;
}

DI_ERR_CODE DI_UART_WriteByte(HUINT8 data)
{
	VK_putchar(data);
	VK_fflush(VK_STDOUT);
	return DI_ERR_OK;
}

DI_ERR_CODE DI_UART_Read(void *data, HUINT32 numToRead, HUINT32 *numRead)
{
	(void)data;
	(void)numToRead;
	(void)numRead;

#if 0
	int ch;
	unsigned char *local_data;
	HUINT32	i;

	struct termios buf;
	struct termios save;

	tcgetattr(0, &save);
	buf = save;

	buf.c_lflag &= ~(ICANON|ECHO);
	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME] = 0;

	tcsetattr(0, TCSAFLUSH, &buf);

	local_data = (unsigned char *)data;

	for(i=0; i<numToRead; i++)
	{
		ch = VK_getchar();
		if(ch < 0)
			break;
		*local_data++ = (unsigned char)ch;
	}

	*numRead = i;

	tcsetattr(0, TCSAFLUSH, &save);
#endif

	return DI_ERR_OK;
}

DI_ERR_CODE DI_UART_ReadByte(HUINT8 *data)
{
#if defined(CONFIG_OS_UCOS)
	*data = VK_getchar();
#else
	int ch;

	struct termios buf;
	struct termios save;
/*
 TCSANOW
    - The change occurs immediately
 TCSAFLUSH
    - The change occurs after all output written to the file descriptor has been transmitted, and all input so far received but not read is discarded before the change is made.

*/
	tcgetattr(0, &save);

	VK_memcpy(&buf,&save,sizeof(save));

	buf.c_lflag &= ~(ICANON|ECHO);
	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME] = 0;

	tcsetattr(0, TCSANOW, &buf);/*Apply new attribute*/

	ch = VK_getchar();

	tcsetattr(0, TCSANOW, &save);/*Apply backup attribute*/

	*data = (unsigned char)ch;
#endif
	return DI_ERR_OK;
}

#if !defined(DI_UART_Print)
DI_ERR_CODE DI_UART_Print(const char *format,...)
{
	va_list args;

	va_start(args, format);
	VK_vprintf(format, args);
	va_end(args);

	return DI_ERR_OK;
}
#endif

DI_ERR_CODE DI_UART_PrintRelease(const char *format,...)
{
	va_list args;

	va_start(args, format);
	VK_vprintf(format, args);
	va_end(args);

	return DI_ERR_OK;
}

DI_ERR_CODE DI_UART_Dump(unsigned char* pucBuffer, unsigned int uiLen, unsigned int uiAlign)
{
	unsigned int uiIndex;

	HAPPY(pucBuffer);
	HAPPY(uiLen);
	HAPPY(uiAlign);

	for (uiIndex=0 ; uiIndex<uiLen ; uiIndex++)
	{
		if (uiAlign!=0 && (uiIndex%uiAlign)==0)
			DI_UART_Print("\n");
		DI_UART_Print("%02X ", *(pucBuffer+uiIndex));
	}
	DI_UART_Print("\n");
	return DI_ERR_OK;
}

DI_ERR_CODE DI_UART_SetPort(HUINT32 port_no)
{
	(void)port_no;
	return DI_ERR_OK;
}
