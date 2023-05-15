/*******************************************************************
	File Description
********************************************************************/
/**
	@file	 rest_bind_pincode.c
	@brief

	Description:  									\n
	Module: node/binding/bind					\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <dlib.h>
#include <dapi.h>
#include <apk.h>
#include <rest_maintask.h>
#include <rest_datatype.h>
#include <rest_bind.h>

/*******************************************************************/
/********************      Local Definition         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	
} RestEpgContext_t;

typedef struct
{
	DxEPG_Type_e	 eEpgType;
	HCHAR			*szSqlName;
} RestEpgSqlName_t;

typedef struct
{
	HINT32			 nJsonWriter;
	HCHAR			*szMember;
	
	HUINT32			 ulNumEvent;
	HxVector_t		*pstEvents;	
} RestEpgSearchResult_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HBOOL						 s_stRestBindPincode_Inited = FALSE;

#define ___LOCAL_FUNCTIONS___
#define _____COMMAND_FUNCTIONS_____
STATIC HERROR rest_bind_pincode_CbVerifyPincode (void *pvArg)
{
	HINT32				 nVerifyResult = FALSE;
	RestMsg_t			*pstMsg = (RestMsg_t *)pvArg;
	HCHAR				*szPinCode = (HCHAR *)pstMsg->aulArgs[0];

	if (NULL != szPinCode)
	{
		nVerifyResult = (HINT32)APK_SCENARIO_PINCODE_Verify((const HCHAR *)szPinCode);
	}

	pstMsg->pvResult = (void *)nVerifyResult;
	rest_task_SyncOut();

	return ERR_OK;
}

#define ___MEMBER_FUNCTIONS___
HERROR rest_bind_pincode_Init (void)
{
	if (TRUE != s_stRestBindPincode_Inited)
	{
		// Initialize the APPKIT
		(void)APK_SCENARIO_PINCODE_Init();
		
		s_stRestBindPincode_Inited = TRUE;
	}

	return ERR_OK;
}

#define ___API_FUNCTIONS___
int REST_BIND_PINCODE_VerifyPincode (char *szPinCode)
{
	RestMsg_t			 stMsg;
	HERROR				 hErr;

	stMsg.pvResult		= (void *)0;
	stMsg.aulArgs[0]	= (HUINT32)szPinCode;

	hErr = rest_task_RequestToExecute(rest_bind_pincode_CbVerifyPincode, (void *)&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("rest_task_RequestToExecute err:\n");
		return ERR_FAIL;
	}

	rest_task_SyncIn();

	return (int)stMsg.pvResult;
}


