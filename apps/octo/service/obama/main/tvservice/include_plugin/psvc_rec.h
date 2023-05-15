/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  psvc_rec.h
	@brief

	Description:  									\n
	Module: SVC / REC								\n

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

#ifndef ___PSVC_REC_H___
#define ___PSVC_REC_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <svc_rec.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      API Functions     ********************/
/*******************************************************************/
SvcRec_ActFunc	psvc_rec_GetActionFunc (SvcRec_ActionPlugIn_e ePlugIn);
void*			psvc_rec_AllocActionData (SvcRec_ActionPlugIn_e ePlugIn);
void			psvc_rec_FreeActionData (SvcRec_ActionPlugIn_e ePlugIn, void *pvData);

Handle_t psvc_rec_GetActionHandleFromResourceId (SvcRec_ActionPlugIn_e ePlugIn, void *pvData, HUINT32 ulResourceId, svcRec_ResType_e	eResType);
void psvc_rec_InitPid ( SvcRec_ActionPlugIn_e ePlugIn, SvcRec_PIDList_t *pstPIDList );
HERROR psvc_rec_ReplacePSI( SvcRec_ActionPlugIn_e ePlugIn, void *pvData, HUINT16 usPid, HUINT8 *pucSection, HUINT32 ulSectionSize );
HERROR psvc_rec_GetInfo( SvcRec_ActionPlugIn_e ePlugIn, void *pvData, SvcRec_RecInfo *pstRecInfo );
HERROR psvc_rec_SetDRMInfo( SvcRec_ActionPlugIn_e ePlugIn, void *pvData, DxCopyrightControl_t *pstInfo, HBOOL bWriteTimeStamp );
HBOOL psvc_rec_IsRecordingWithName( void *pvData, HUINT8 *pucFileName);

#endif	// ___PSVC_REC_H___
/* end of file */
