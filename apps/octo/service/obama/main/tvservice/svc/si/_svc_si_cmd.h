/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _svc_si_cmd.h
	@brief

	Description:  									\n
	Module: SVC / SI_NEW					\n

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

#ifndef __INT_SVC_SI_CMD_H__
#define __INT_SVC_SI_CMD_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <_svc_si.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define svcSi_CMD_STR_BUFF_SIZE				128

#define	svcSi_UtIsValidLangId(eId)		((eId == eLangID_NONE || eId == eLangID_MAX) ? FALSE : TRUE)
#define svcSi_UtGetLang(eId)			(svcSi_UtIsValidLangId(eId) ? HLIB_LANG_IdTo639(eId) : "")

#define	svcSi_UtIsValidLang(pszLang)	((NULL == pszLang) || (3 != HxSTD_StrLen(pszLang)) ? FALSE : TRUE)
#define	svcSi_UtGetLangId(pszLang)		(svcSi_UtIsValidLang(pszLang) ? HLIB_LANG_639ToId(pszLang) : eLangID_NONE)

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Member Functions     ********************/
/*******************************************************************/

// ES Audio Test Case
extern void svc_si_TcAudioSelectSourceNull1(Cutest_TestCase_t *pstTc);
extern void svc_si_TcAudioSelectSourceNull2(Cutest_TestCase_t *pstTc);
extern void svc_si_TcAudioSelectTargetNull(Cutest_TestCase_t *pstTc);
extern void svc_si_TcAudioSelectSourceInvalid(Cutest_TestCase_t *pstTc);
extern void svc_si_TcAudioSelectNoMatchFirstTrack(Cutest_TestCase_t *pstTc);
extern void svc_si_TcAudioSelectStereoAud0(Cutest_TestCase_t *pstTc);
extern void svc_si_TcAudioSelectStereoAud1(Cutest_TestCase_t *pstTc);
extern void svc_si_TcAudioSelectStereoMenu(Cutest_TestCase_t *pstTc);
extern void svc_si_TcAudioSelectStereoUser(Cutest_TestCase_t *pstTc);
extern void svc_si_TcAudioSelectMultiUser1(Cutest_TestCase_t *pstTc);
extern void svc_si_TcAudioSelectMultiUser2(Cutest_TestCase_t *pstTc);
extern void svc_si_TcAudioSelectMultiAud0(Cutest_TestCase_t *pstTc);
extern void svc_si_TcAudioSelectAdMultiAud0(Cutest_TestCase_t *pstTc);
extern void svc_si_TcAudioSelectDualMonoRightUserLang(Cutest_TestCase_t *pstTc);
extern void svc_si_TcAudioSelectDualMonoNonUserLang(Cutest_TestCase_t *pstTc);
extern void svc_si_TcAudioSelectDualMonoLeftAud0(Cutest_TestCase_t *pstTc);
extern void svc_si_TcAudioSelectAdMenuLang(Cutest_TestCase_t *pstTc);
extern void svc_si_TcAudioSelectAdLangNotMatched1(Cutest_TestCase_t *pstTc);
extern void svc_si_TcAudioSelectAdLangNotMatched2(Cutest_TestCase_t *pstTc);
extern void svc_si_TcAudioSelectAdAud1(Cutest_TestCase_t *pstTc);

// ES Subtitle Test Case
extern void svc_si_TcSubtitleSelectSourceNull1(Cutest_TestCase_t *pstTc);
extern void svc_si_TcSubtitleSelectSourceNull2(Cutest_TestCase_t *pstTc);
extern void svc_si_TcSubtitleSelectTargetNull(Cutest_TestCase_t *pstTc);
extern void svc_si_TcSubtitleSelectSourceInvalid(Cutest_TestCase_t *pstTc);
extern void svc_si_TcSubtitleSelectNoMatchFirstTrack(Cutest_TestCase_t *pstTc);
extern void svc_si_TcSubtitleSelectLang0(Cutest_TestCase_t *pstTc);
extern void svc_si_TcSubtitleSelectLang1(Cutest_TestCase_t *pstTc);
extern void svc_si_TcSubtitleSelectLangMenu(Cutest_TestCase_t *pstTc);
extern void svc_si_TcSubtitleSelectLangUser(Cutest_TestCase_t *pstTc);
extern void svc_si_TcSubtitleSelectLangUndefined(Cutest_TestCase_t *pstTc);
extern void svc_si_TcSubtitleSelectLangOriginal(Cutest_TestCase_t *pstTc);
extern void svc_si_TcSubtitleSelectLangDefault(Cutest_TestCase_t *pstTc);
extern void svc_si_TcSubtitleSelectLangUndefinedAndOriginal(Cutest_TestCase_t *pstTc);
extern void svc_si_TcSubtitleSelectLangOriginalAndDefault(Cutest_TestCase_t *pstTc);
extern void svc_si_TcSubtitleSelectLangPriority1(Cutest_TestCase_t *pstTc);
extern void svc_si_TcSubtitleSelectLangPriority2(Cutest_TestCase_t *pstTc);
extern void svc_si_TcSubtitleSelectLangPriority3(Cutest_TestCase_t *pstTc);
extern void svc_si_TcSubtitleSelectTypeDvb(Cutest_TestCase_t *pstTc);
extern void svc_si_TcSubtitleSelectTypeDvbAndLang1(Cutest_TestCase_t *pstTc);
extern void svc_si_TcSubtitleSelectTypeDvbAndLang2(Cutest_TestCase_t *pstTc);
extern void svc_si_TcSubtitleSelectTypeDvbAndAr1(Cutest_TestCase_t *pstTc);
extern void svc_si_TcSubtitleSelectTypeDvbAndAr2(Cutest_TestCase_t *pstTc);
extern void svc_si_TcSubtitleSelectAr1(Cutest_TestCase_t *pstTc);
extern void svc_si_TcSubtitleSelectAr2(Cutest_TestCase_t *pstTc);
extern void svc_si_TcSubtitleSelectAr3(Cutest_TestCase_t *pstTc);
extern void svc_si_TcSubtitleSelectHd1(Cutest_TestCase_t *pstTc);
extern void svc_si_TcSubtitleSelectHd2(Cutest_TestCase_t *pstTc);
extern void svc_si_TcSubtitleSelectHohOn1(Cutest_TestCase_t *pstTc);
extern void svc_si_TcSubtitleSelectHohOn2(Cutest_TestCase_t *pstTc);
extern void svc_si_TcSubtitleSelectHohOn3(Cutest_TestCase_t *pstTc);
extern void svc_si_TcSubtitleSelectHohOff1(Cutest_TestCase_t *pstTc);
extern void svc_si_TcSubtitleSelectHohOff2(Cutest_TestCase_t *pstTc);
extern void svc_si_TcSubtitleSelectHohOff3(Cutest_TestCase_t *pstTc);
extern void svc_si_TcSubtitleSelectHohAndAr1(Cutest_TestCase_t *pstTc);
extern void svc_si_TcSubtitleSelectHohAndAr2(Cutest_TestCase_t *pstTc);
extern void svc_si_TcSubtitleSelectHohAndAr3(Cutest_TestCase_t *pstTc);

// Debug
extern void svc_si_LibSvcEsPrintAudioPriorityTable(void);
extern void svc_si_LibSvcEsPrintSubtitlePriorityTable(void);


// Utils
extern void						 svc_si_UtVeritySelectAudioOutput (Cutest_TestCase_t	*pstTc,
																	HERROR				 hErr,
																	SvcSi_SelectedEs_t	*pstSelectedEs,
																	HINT32				 nExpectedTrackIdx,
																	const HCHAR			*pszErrMsg,
																	HUINT32				 nParam1,	// For Audio : DualMono Selection
																	HUINT32				 nParam2,
																	HUINT32				 nParam3);

extern void						svc_si_UtVeritySelectSubtitleOutput (Cutest_TestCase_t	*pstTc,
																	HERROR				 hErr,
																	SvcSi_SelectedEs_t	*pstSelectedEs,
																	HINT32				 nExpectedTrackIdx,
																	const HCHAR			*pszErrMsg,
																	HUINT32				 nParam1,
																	HUINT32				 nParam2,
																	HUINT32				 nParam3);

extern void						 svc_si_UtVeritySelectEsResult (Cutest_TestCase_t		*pstTc,
																	const HCHAR			*pszFuncName,
																	const HCHAR			*pszResultType,
																	HINT32				 nActualResult,
																	HINT32				 nExpectedResult);

extern void						 svc_si_UtInitSelectedEs (SvcSi_SelectedEs_t *pstSelectedEs);

extern HUINT32					 svc_si_UtSetTrackIdx (HUINT32			*aulTrackList,
														HUINT32			 ulListNum,
														HUINT32			 ulMaxTrackIdx,
														va_list			 ap);

extern const svcSi_PmtEsInfo_t	*svc_si_UtSetEsListToPmtEsInfo(svcSi_PmtEsInfo_t	*pstPmtEsInfo,
																EsType_t			 eEsType,
																const HxList_t		*pstEsList);

extern void						 svc_si_UtFreeEsInfo (svcSi_PmtEsInfo_t *pstEsInfo);

extern void						 svc_si_UtFreeConfig (SvcSi_Config_t *pstConfig);

extern void						 svc_si_UtPrintConfig (const SvcSi_Config_t			*pstConfig,
														HBOOL						 bAudio,
														HBOOL						 bSubtitle);

#endif	// __INT_SVC_SI_CMD_H__
/* end of file */

