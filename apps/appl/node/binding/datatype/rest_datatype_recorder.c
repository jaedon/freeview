/*******************************************************************
	File Description
********************************************************************/
/**
	@file	 rest_datatype_recorder.c
	@brief

	Description:  									\n
	Module: node/binding/datatype_convert		\n

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
#include <apk.h>
#include <rest_datatype.h>

/*******************************************************************/
/********************      Local Definition         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
// Converting Types:
STATIC RestEnumStringTable_t	 s_stRestDataType_RecInstState =
{
	(HCHAR*)DxNAMEOF(OxMediaRec_RecState_e),
		
	{
		{	"idle",			eOxMR_RECSTATE_NONE			},
		{	"stopping",		eOxMR_RECSTATE_STOPPING		},
		{	"stopped",		eOxMR_RECSTATE_STOPPED		},
		{	"requested", 	eOxMR_RECSTATE_REQUESTED	},
		{	"connecting",	eOxMR_RECSTATE_CONNECTING	},
		{	"recording",	eOxMR_RECSTATE_RECORDING	},
		{	NULL,			0							}
	}
};


#define ___LOCAL_FUNCTIONS___

#define ___MEMBER_FUNCTIONS___
HINT32 rest_datatype_JsonizeRecorderInstance (HINT32 nJsonWriter, RestBindRecorderInst_t *pstRecInst, HCHAR *szObjectName, HCHAR *szSelect)
{
	HINT32		 nWriter;

	if (NULL == pstRecInst)
	{
		HxLOG_Error("pstRecInst NULL\n");
		return nJsonWriter;
	}

	nWriter = (0 == nJsonWriter) ? HLIB_JSONWRITER_Open() : nJsonWriter;
	if (0 == nWriter)
	{
		HxLOG_Error(">> HLIB_JSONWRITER_Open failed:\n");
		return 0;
	}

	if (NULL == szObjectName)
	{
		// No Object: Nothing to do
	}
	else if ('\0' == *szObjectName)
	{
		// Make the object, but no name
		HxJSONWRITER_ObjectBegin(nWriter);
	}
	else
	{
		// Make the object
		HxJSONWRITER_Object(nWriter, szObjectName);
	}

	// Contents
	{
		(void)rest_datatype_SetJsonInteger(nWriter, "recorderUid", (HINT32)pstRecInst->ulInstIndex, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "state", DxNAMEOF(OxMediaRec_RecState_e), (HINT32)pstRecInst->eState, szSelect);
		if (eOxMR_RECSTATE_NONE != pstRecInst->eState)
		{
			(void)rest_datatype_SetJsonInteger(nWriter, "recordUid", (HINT32)pstRecInst->ulContentsId, szSelect);

			if (rest_datatype_CheckWriteSelected("recordSourceInfo", szSelect))
			{
				(void)rest_datatype_JsonizeRecRequest(nWriter, &(pstRecInst->stRecStartInfo), "recordSourceInfo", NULL);
			}
		}
	}

	if (NULL != szObjectName)
	{
		HxJSONWRITER_ObjectEnd(nWriter);
	}

	return nWriter;
}

#define _____INITIALIZATION_____
HERROR rest_datatype_InitRecorder (void)
{
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_RecInstState);

	return ERR_OK;
}

#define ___GLOBAL_FUNCTIONS___

