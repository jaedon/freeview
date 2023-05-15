/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pal_sirawpool.h
	@brief

	Description:  									\n
	Module: PAL / SIRAW					\n

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

#ifndef __PAL_SI_RAWPOOL_H__
#define __PAL_SI_RAWPOOL_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <silib.h>

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
extern HERROR				PAL_SIRAWPOOL_Init (void);
extern HERROR				PAL_SIRAWPOOL_Clear (HUINT32 ulActionId);
extern HERROR 				PAL_SIRAWPOOL_GetSiProperty (HUINT32 ulActionId, SIxSpec_e *peSpec, SIxTextEncoding_e *peTextEnc);
extern HERROR				PAL_SIRAWPOOL_ChangeSiSpecType (HUINT32 ulActionId, SIxSpec_e eSpec);
extern HERROR				PAL_SIRAWPOOL_ChangeSiTextEncType (HUINT32 ulActionId, SIxTextEncoding_e eTextEnc);
extern HERROR 				PAL_SIRAWPOOL_AddRawSection (HUINT32 ulActionId, SIxTable_Type_e eSiType, HUINT8 *pucRaw, HUINT32 ulUniqueId);
extern HERROR 				PAL_SIRAWPOOL_AddMultiRawSection (HUINT32 ulActionId, SIxTable_Type_e eSiType, HUINT32 ulRawNum, HUINT8 **ppucRawArray, HUINT32 ulUniqueId, HBOOL bIncompleteSec);
extern HERROR 				PAL_SIRAWPOOL_IncreaseSameSectionCounter(HUINT32 ulActionId, HUINT32 *pulCnt);
extern HERROR 				PAL_SIRAWPOOL_GetSameSectionCounter(HUINT32 ulActionId, HUINT32 *pulCnt);
extern HERROR 				PAL_SIRAWPOOL_ResetSameSectionCounter(HUINT32 ulActionId);
extern HERROR 				PAL_SIRAWPOOL_FinalizeIncompleteSection (HUINT32 ulActionId, SIxTable_Type_e eSiType, HUINT32 ulExtId);
extern HERROR				PAL_SIRAWPOOL_ReplaceMultiRawSection (HUINT32 ulActionId, SIxTable_Type_e eSiType, HBOOL bUseExtId, HUINT32 ulExtId, HUINT32 ulRawNum, HUINT8 **ppucRawArray, HUINT32 ulUniqueId);
extern HERROR 				PAL_SIRAWPOOL_FindSameRawSection (HUINT32 ulActionId, SIxTable_Type_e eSiType, HUINT32 ulExtId, HUINT32 ulRawNum, HUINT8 **ppucRawArray);
extern HERROR				PAL_SIRAWPOOL_FreeRawSection (HUINT32 ulActionId, SIxTable_Type_e eSiType, HBOOL bUseExtId, HUINT32 ulExtId);
extern HERROR				PAL_SIRAWPOOL_FreeRawSectionByUniqueId (HUINT32 ulActionId, SIxTable_Type_e eSiType, HUINT32 ulUniqueId);
extern HERROR				PAL_SIRAWPOOL_FreeAllRawSection (HUINT32 ulActionId, SIxTable_Type_e eSiType);
extern SIxTable_PsiTable_t*	PAL_SIRAWPOOL_ParseTable (HUINT32 ulActionId, SIxTable_Type_e eSiType, HUINT32 ulExtId);
extern void					PAL_SIRAWPOOL_FreeTable (SIxTable_PsiTable_t *pstTable);
extern HUINT32				PAL_SIRAWPOOL_CountRaws (HUINT32 ulActionId, SIxTable_Type_e eSiType, HBOOL bUseExtId, HUINT32 ulExtId);
extern HERROR				PAL_SIRAWPOOL_GetRawArrays (HUINT32 ulActionId, SIxTable_Type_e eSiType, HBOOL bUseExtId, HUINT32 ulExtId, HUINT32 ulRawArrayMax, HUINT8 **ppucRawArray, HUINT32 *pulNumRaw);
extern SIxDescriptor_t*		PAL_SIRAWPOOL_ParseDes (HUINT32 ulActionId, HUINT8 *pucRawData);
extern SIxDescriptor_t*		PAL_SIRAWPOOL_ParseDvbExtDes (HUINT32 ulActionId, HUINT8 *pucRawData);
extern SIxDescriptor_t*		PAL_SIRAWPOOL_ParseSsuUntDes (HUINT32 ulActionId, HUINT8 *pucRawData);
extern void					PAL_SIRAWPOOL_FreeDes (SIxDescriptor_t *pstDes);

#endif	// __PAL_SI_RAWPOOL_H__
/* end of file */

