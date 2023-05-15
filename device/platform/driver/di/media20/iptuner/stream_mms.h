/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   stream_mms.h  $
 * Version:			$Revision:   1.0  $
 * Original Author: $
 * Current Author:	$Author: junsy@humaxdigital.com $
 * Date:			$Date: 2012.07.30
 * File Description:	MMS Stream Controller (MMSTU/MMSH)
 * Module:
 * Remarks:
 */

/**
 * @ingroup MMS_STREAM_CONTROLLER
 */

/**
 * @author Jin Lee(jlee@humaxdigital.com)
 * @date 30 Jul 2012
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
 * @file stream_mms.h
 */

#ifndef __STREAM_MMS_H__
#define __STREAM_MMS_H__

#ifdef  __cplusplus
extern "C" {
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#include <unistd.h>
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

#include "htype.h"
#include "transport_io.h"
#include "di_iptuner_common.h"
#include "util_url.h"
#include "stream_global.h"
#include "di_iptuner20.h"
/* End Including Headers*/


/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
/* Start Extern variablee */

/* End Extern variable */


/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
/* Start Macro definition */
/* End Macro definition */


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */
/* End typedef */


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
/* Start global function prototypes */

/*!
 *	\brief Open MMS(WMS-TU,H,R) stream
 *	\param pAccess: STREAM_ACCESS_t handle
 *	\return HINT32: 1 (success), -1 (failture)
 *	\see
 */
int STREAM_MMS_Open ( STREAM_ACCESS_t *pAccess );

/*!
 *	\brief Close MMS(WMS-TU,H,R) stream
 *	\param pAccess: STREAM_ACCESS_t handle
 *	\return void
 *	\see
 */
void STREAM_MMS_Close ( STREAM_ACCESS_t *pAccess );

/*!
 *	\brief Read MMS(WMS-TU,H,R) stream
 *	\param pAccess: STREAM_ACCESS_t handle, p_buffer: buffer, i_len: buffer length
 *	\return ssize_t: reading total size
 *	\see
 */
ssize_t STREAM_MMS_Read ( STREAM_ACCESS_t *pAccess, HUINT8 *p_buffer, size_t i_len );

/*!
 *	\brief byte seek for MMS(WMS-TU,H,R) stream
 *	\param pAccess: STREAM_ACCESS_t handle, i_pos: seek offset
 *	\return HINT32: DI_IPT_SUCCESS if success
 *	\see
 */
int STREAM_MMS_Seek ( STREAM_ACCESS_t *pAccess, HINT64 i_pos );

/*!
 *	\brief Control(trick/timed seek) MMS(WMS-TU,H,R) stream
 *	\param pAccess: STREAM_ACCESS_t handle, cmd: DI_IPTUNER_CMD_e type command
 *	\return int: DI_IPT_SUCCESS if success
 *	\see
 */
int STREAM_MMS_Control ( STREAM_ACCESS_t *pAccess, DI_IPTUNER_CMD_e cmd, HUINT64 val ,HUINT64 val2);

/* End global function prototypes */

#ifdef  __cplusplus
}
#endif

#endif /* ! __STREAM_MMS_H__ */
