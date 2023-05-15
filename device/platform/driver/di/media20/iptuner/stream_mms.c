/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   stream_mms.c  $
 * Version:		2.0 (for Media 2.0 with 40nm)
 * Original Author:	Jin Lee
 * Current Author:	$Author: jlee@humaxdigital.com $
 * Date:			$Date: 2012.07.30
 * File Description:	     MMS - STREAM Controller
 * Module:
 * Remarks:
 */

/**
 * @defgroup MMS_STREAM_CONTROLLER
 * @brief MMS STREAM Controller.
 */

/**
 * @author Jin Lee (jlee@humaxdigital.com)
 * @date 30 Jul 2012
 */

/**
 * @brief MMS STREAM Controller.
 */

/**
 * @note
 * Copyright (C) 2012 Humax Corporation. All Rights Reserved. <br>
 * This software is the confidential and proprietary information
 * of Humax Corporation. You may not use or distribute this software
 * except in compliance with the terms and conditions of any applicable license
 * agreement in writing between Humax Corporation and you.
 */

 /*@{*/

/**
 * @file stream_mms.c
 */

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* Start Including Header Files */

#include <assert.h>
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <inttypes.h>

#include "htype.h"
#include "di_uart.h"
#include "stream_global.h"

#include "di_iptuner_config.h"
#include "di_iptuner_common.h"
#include "mms_asf.h"
#include "mms_tcpudp.h"
#include "mms_http.h"

#include "util_url.h"


/* for parsing the rtsp header */
#include "stream_global.h"
#include "rtsp_lib.h"
#include "stream_rtsp.h"
#include "util_url.h"

/* End Including Headers */

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define STREAM_MMS_DEBUG
//#define STREAM_MMS_CRITICAL_DEBUG
//
#ifdef IPTUNER_ALL_DEBUG	/* DEBUG Define */

#ifdef STREAM_MMS_DEBUG
#define PrintData		DI_UART_Print
#define PrintDebug		DI_UART_Print
#define PrintError		DI_UART_Print
#define PrintEnter		DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit		DI_UART_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#define PrintError		DI_UART_Print
#endif

#else

#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintError		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#endif /* The END of DEBUG DEFINE */


#define UNUSED_PARAM(x)  ((void)x)

/* End #define */

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */
/* End typedef */


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
/* Start global variable */
/* End global variable */


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
/* Start static variable */
/* End static variable */


/*******************************************************************/
/************************ static funtions **************************/
/*******************************************************************/
/* Start static function prototype */
/* End static function prototype */

/****************************************************
 * TODO
 * 2010.12.24 junsy:
 * 	We don't support mms-rollover for microsoft media server.
 * 	It is standard spec witten in msdn.
 * 	Current supports : MMSH (for BBC iPlayer), MMSR(for Maxdome)
 ****************************************************/
int STREAM_MMS_Open ( STREAM_ACCESS_t *pAccess )
{
	HINT32 nRet = DI_IPT_SUCCESS;

	PrintEnter;

	pAccess->b_alive = TRUE;

#if defined(HDR_FOX_T2) || defined(HD_FOX_T2)
	/** 1. Try to MMSH Connection */
	if ( DI_IPT_SUCCESS == (nRet = MMSH_Open(pAccess)))
	{
		pAccess->proto = PROTOCOL_MMSH;
		PrintData("PROTOCOL_MMSH is selected !!\n");
	}
#else
	/** 1. Try to RTSP Connection */
	if ( DI_IPT_SUCCESS == (nRet = MMSR_Open(pAccess)))
	{
		pAccess->proto = PROTOCOL_RTSP;
		PrintData("PROTOCOL_RTSP is selected !!\n");
	}
#if 0

	/** 2. Try to MMSTCP Connection */
	else if ( DI_IPT_SUCCESS == (nRet = MMSTU_Open(pAccess)))
	{
		pAccess->proto = PROTOCOL_MMSTU;
	}
	/** 3. Try to MMSH Connection */
	else
	{
		nRet = MMSH_Open (pAccess);
		if( DI_IPT_SUCCESS == nRet)
		{
			pAccess->proto = PROTOCOL_MMSH;
		}
#endif
	else
	{
		PrintData("MMSX_Open Retry...\n");
		if ( DI_IPT_SUCCESS == (nRet = MMSR_Open(pAccess)))
		{
			pAccess->proto = PROTOCOL_RTSP;
			PrintData("PROTOCOL_RTSP is selected !!\n");
		}
		else
		{
			PrintError("%s (%d) Error> Open Fail \n",__FUNCTION__,__LINE__);
			nRet = DI_IPT_EGENERIC;
			pAccess->b_alive = FALSE;
		}
	}
#endif

	PrintExit;
	return nRet;
}


/*****************************************************************************
 * STREAM_MMS_Close:
 *****************************************************************************/
void STREAM_MMS_Close ( STREAM_ACCESS_t *pAccess )
{
	PrintEnter;
	pAccess->b_alive = FALSE;

	if (pAccess->proto == PROTOCOL_RTSP)
	{
		MMSR_Close(pAccess);
	}
	else if (pAccess->proto == PROTOCOL_MMSTU)
	{
		MMSTU_Close(pAccess);
	}
	else if (pAccess->proto == PROTOCOL_MMSH)
	{
		MMSH_Close(pAccess);
	}
	else
	{
		PrintError("%s(%d) Error> - We don't have MMS tuner.\n",__FUNCTION__,__LINE__);
	}

	PrintExit;
	return;
}

ssize_t STREAM_MMS_Read ( STREAM_ACCESS_t *pAccess, HUINT8 *p_buffer, size_t i_len )
{
	ssize_t nRead=0;
	PrintEnter;

	if (pAccess->proto == PROTOCOL_RTSP)
	{
		nRead = MMSR_Read(pAccess, p_buffer, i_len);
	}
	else if (pAccess->proto == PROTOCOL_MMSTU)
	{
		nRead = MMSTU_Read ( pAccess, p_buffer, i_len);
	}
	else if (pAccess->proto == PROTOCOL_MMSH)
	{
		nRead = MMSH_Read( pAccess, p_buffer, i_len);
	}
	else
	{
		PrintError("%s(%d) Error> - We don't have MMS tuner.\n",__FUNCTION__,__LINE__);
	}
	PrintExit;
	return nRead;
}



/*****************************************************************************
 * STREAM_HTTP_Seek: close and re-open a connection at the right place
 *****************************************************************************/
int STREAM_MMS_Seek ( STREAM_ACCESS_t *pAccess, HINT64 i_pos )
{
	HINT32 nRet= DI_IPT_SUCCESS;

	PrintEnter;
	if (pAccess->proto == PROTOCOL_RTSP)
	{
		nRet = MMSR_Seek (pAccess, i_pos);
	}
	else if (pAccess->proto == PROTOCOL_MMSTU)
	{
		nRet = MMSTU_Seek (pAccess, i_pos);
	}
	else if (pAccess->proto == PROTOCOL_MMSH)
	{
		nRet = MMSH_Seek (pAccess, i_pos);
	}
	else
	{
		nRet = DI_IPT_EGENERIC;
		PrintError("%s(%d) Error> - We don't have MMS tuner.\n",__FUNCTION__,__LINE__);
	}

	PrintExit;
	return nRet;
}

/*****************************************************************************
 * STREAM_MMS_Control:
 *****************************************************************************/
int STREAM_MMS_Control ( STREAM_ACCESS_t *pAccess, DI_IPTUNER_CMD_e cmd, HUINT64 val, HUINT64 val2)
{
	HINT32 nRet= DI_IPT_SUCCESS;
	PrintEnter;

	if (pAccess->proto == PROTOCOL_RTSP)
	{
		nRet = MMSR_Control (pAccess, cmd, val, val2);
	}
	else if (pAccess->proto == PROTOCOL_MMSTU)
	{
		nRet = MMSTU_Control (pAccess, cmd, val);
	}
	else if (pAccess->proto == PROTOCOL_MMSH)
	{
		nRet = MMSH_Control (pAccess, cmd, val);
	}
	else
	{
		nRet = DI_IPT_EGENERIC;
		PrintError("%s(%d) Error> - We don't have MMS tuner.\n",__FUNCTION__,__LINE__);
	}

	PrintExit;
	return nRet;
}
