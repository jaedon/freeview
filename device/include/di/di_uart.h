/* $Header: $ */
/****************************************************************************
* $Workfile:   di_uart.h  $
* $Modtime:   Jul 16 2007 00:17:52  $
*
* Author:
* Description:
*
*                                 Copyright (c) 2008 HUMAX Co., Ltd.
*                                               All rights reserved.
*****************************************************************************/

#ifndef __DI_UART_H__
#define __DI_UART_H__


/*      .c �ҽ� �ڵ忡�� ��� �ϴٰ� ���� di_err.h�� ������ include�Ǿ �������� �� ���� ������,
    �ܼ��� �� ���� �ϳ��� include�ϸ� DI_ERR_CODE�� ���� �ɺ��� ������ ���ϴ�.
    ��� di_XXX.h�� ����ؾ� �մϴ�.
*/
#include "di_err.h"
#include "htype.h"



/****************************************************************************
* Definitions
*****************************************************************************/

/****************************************************************************
* Typedef & const data
*****************************************************************************/


/****************************************************************************
* Global function prototypes
*****************************************************************************/
DI_ERR_CODE DI_UART_Init(void);
DI_ERR_CODE DI_UART_Write(void *data, HUINT32 numToWrite);
DI_ERR_CODE DI_UART_WriteByte(HUINT8 data);
DI_ERR_CODE DI_UART_Read(void *data, HUINT32 numToRead, HUINT32 *numRead);
DI_ERR_CODE DI_UART_ReadByte(HUINT8 *data);
DI_ERR_CODE DI_UART_ReadByteNoWait(HUINT8 *data);
#if defined(DI_UART_Print)
#undef DI_UART_Print
#define DI_UART_Print(...)	((void)0)
#else
DI_ERR_CODE DI_UART_Print(const char *format,...);
#endif
#if defined(DI_UART_BlockablePrint)
#undef DI_UART_BlockablePrint
#define DI_UART_BlockablePrint(a,...)	((void)0)
#else
DI_ERR_CODE DI_UART_BlockablePrint(const char *format, ...);
#endif
DI_ERR_CODE DI_UART_PrintRelease(const char *format,...);
DI_ERR_CODE DI_UART_Dump(unsigned char* pucBuffer, unsigned int uiLen, unsigned int uiAlign);
DI_ERR_CODE DI_UART_ReadKey(HUINT8 *data);

#endif /*__DI_UART_H__*/
