/*******************************************************************************
 *
 * FILE NAME          : MxL_Debug.h
 *
 * AUTHOR             : David Zhou
 *                      Ron Green - Added MxL_VFC_DEBUG for the MxL Color
 *                                  Formater filter.
 *
 * DATE CREATED       : 2/7/2008
 *                      9/15/2008
 *
 * DESCRIPTION        : Debug header files
 *
 *******************************************************************************
 *                Copyright (c) 2008, MaxLinear, Inc.
 ******************************************************************************/

#ifndef __MXL_DEBUG_H__
#define __MXL_DEBUG_H__


/******************************************************************************
    Include Header Files
    (No absolute paths - paths handled by make file)
******************************************************************************/
#include <stdarg.h>
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <stddef.h>

#include "MaxLinearDataTypes.h"

/******************************************************************************
    Macros
******************************************************************************/

//#define DEBUG_MXLLOG

#if defined(DEBUG_MXLLOG)
#include "di_uart.h"
#define MxL_DLL_DEBUG0 DI_UART_Print
#else
#define MxL_DLL_DEBUG0
#endif




#endif /* __MXL_DEBUG_H__ */
