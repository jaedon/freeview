/**
 * drv_uart.h
*/

/**
 * @defgroup		DRV_UART DRV_UART : DRV_UART module
 * @author		Jin-hong Kim
 * @note			DRV_UART APIs
 * @brief			Define broadcom dependent DRV_UART
 * @file			drv_uart.h
*/

#ifndef __DRV_UART_H__
#define __DRV_UART_H__

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
#include "htype.h"
#include "drv_err.h"

/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
#if defined(CONFIG_BCM_UART)
typedef enum
{
	DRV_UARTSTATUS_CLOSED,
	DRV_UARTSTATUS_OPENED,
	DRV_UARTSTATUS_MAX,
}DRV_UARTSTATUS_e;

typedef enum
{
	DRV_UARTPARITY_NONE,
	DRV_UARTPARITY_ODD,
	DRV_UARTPARITY_EVEN,
	DRV_UARTPARITY_MAX
}DRV_UARTPARITY_e;

typedef enum
{
	DRV_UARTDATABITS_7,
	DRV_UARTDATABITS_8,
	DRV_UARTDATABITS_MAX
}DRV_UARTDATABITS_e;

typedef enum
{
	DRV_UARTSTOPBITS_0,
	DRV_UARTSTOPBITS_1,
	DRV_UARTSTOPBITS_2,
	DRV_UARTSTOPBITS_MAX
}DRV_UARTSTOPBITS_e;

typedef	struct {
	HUINT32 ulTXBufferSize;
	HUINT32 ulRXBufferSize;
	HUINT32 ulBaudRate;
	DRV_UARTPARITY_e eParity;
	DRV_UARTDATABITS_e eDataBits;
	DRV_UARTSTOPBITS_e eStopBits;
}DRV_UARTPARAM_t;

typedef	enum
{
	DRV_UART_EVENT_TX,
	DRV_UART_EVENT_RX,
	DRV_UART_EVENT_RX_ERROR,
	DRV_UART_EVENT_END
}DRV_UART_EVENT;

typedef void (*DRV_UART_CALLBACK)(DRV_UART_EVENT eUartEvent, HUINT32 ulParam);

#endif 


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
DRV_Error DRV_UART_ReadByteNoWaitForViaccessGetc(HUINT8 *ch);
#if defined(CONFIG_BCM_UART)
DRV_Error DRV_UART_Init(void);
DRV_Error DRV_UART_Open(HUINT32 ulPortNumber);
DRV_Error DRV_UART_GetStatus(HUINT32 ulPortNumber, DRV_UARTSTATUS_e *peStatus);
DRV_Error DRV_UART_SetParam(HUINT32 ulPortNumber, DRV_UARTPARAM_t *ptParam);
DRV_Error DRV_UART_Read(HUINT32 ulPortNumber, HUINT8 *pucData, HUINT32 ulDataSize);
DRV_Error DRV_UART_Write(HUINT32 ulPortNumber, HUINT8 *pucData, HUINT32 ulDataSize);
void DRV_UART_RegisterEventNotify(HUINT32 ulPortNumber,DRV_UART_EVENT eUartEvent, DRV_UART_CALLBACK pfUARTEventCallback);

#endif


#endif /* !__DRV_UART_H__ */

