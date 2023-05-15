/*
 * (c) 2011-2012 Humax Co., Ltd.
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
/*******************************************************************
	File Description
********************************************************************/
/**
@file				cxcore_stream.h
@brief			cxcore_stream.h

	Description:   \n
	Module: MW/CAS \n

	Copyright (c) 2009 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

#ifndef	__CXCORE_STREAM_H__
#define	__CXCORE_STREAM_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables 	********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition 	********************/
/*******************************************************************/
#define CX_CW_LENGTH		8

/*******************************************************************/
/********************      Typedef  	   *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions 	********************/
/*******************************************************************/
HERROR CXSTREAM_InitCore(void);
HERROR CXSTREAM_SetCW(Handle_t hEcm, HUINT32 ulOddLength, HUINT32 ulEvenLength, HUINT8* pucOddCw, HUINT8* pucEvenCw, HUINT8* pucKey, HUINT16 usKeyId);
#if 0
HERROR CXSTREAM_SetPid(Handle_t hSession, CxEsPidInfo_t *pstEsInfo);
#else
//HERROR CXSTREAM_SetPid(Handle_t hSession, CAS_EsType_t eType, HUINT16 usPid, HBOOL bAdd);
HERROR CXSTREAM_SetPid(Handle_t hSession, HUINT16 usPid, HBOOL bAdd);
#endif
HBOOL CXSTREAM_IsConaxStream(Handle_t hSession);
HERROR CXSTREAM_ParsePmt(Handle_t hSession);
HERROR CXSTREAM_DeleteAll(Handle_t hSession);
HERROR CXSTREAM_IsScrambledSvc(Handle_t hSession, HBOOL* pbScrambled);
HERROR CXSTREAM_StartAllEcmFilter(Handle_t hSession);
HERROR CXSTREAM_StopAllEcmFilter(Handle_t hSession);
HERROR CXSTREAM_RestartAllEcmFilter(Handle_t hSession);
HERROR CXSTREAM_FindStreamBySessionAndEcm(Handle_t hSession, Handle_t hEcm);
Handle_t CXSTREAM_GetFirstSessionByEcm(Handle_t hEcm);
HUINT32 CXSTREAM_GetAllSessionByEcm(Handle_t hEcm, Handle_t* phList);

#if defined(CONFIG_MW_CAS_CONAX_CPM)
HERROR CXSTREAM_RemoveCWPK(HUINT16 usKeyId);
#endif

HERROR CXSTREAM_ResetStream(Handle_t hSession);
#if defined(CONFIG_DEBUG)
void CMD_CXSTREAM_PrintStreamContext(void);
#endif
// API function (which can be invoked from outside of the conax core)

#endif /* !__CXCORE_STREAM_H__ */
