/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  cmd_svc_si.c
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
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
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
#include <silib.h>
#include <vkernel.h>
#include <octo_common.h>
#include <isosvc.h>

#include <pal_sirawpool.h>
#include <pal_pipe.h>
//#include <mwc_util.h>
#include <svc_si.h>

#include <_svc_si.h>
#include <_svc_si_lib.h>
#include <_svc_si_filtering.h>
#include <_svc_si_search.h>
#include <_svc_si_ut.h>


#include "cutest.h"

#include "_svc_si_cmd.h"

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
/********************      Static Variables     ********************/
/*******************************************************************/

#define ___LOCAL_FUNCTIONS___

/**********************************************
	Command 수행 함수를 등록하는 통합 등록 함수.
***********************************************/
#if defined(CONFIG_DEBUG)
extern HUINT32 svc_si_PrintFilterState(void);

STATIC Cutest_Suite_t* svc_si_UtGetSuiteAudioSelect(void)
{
	Cutest_Suite_t	*pstSuite = CUTEST_SuiteNew();

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcAudioSelectSourceNull1,
											"SUT                : svc_si_UtSelectAudio\n"
											"Expected Result    : ERR_FAIL\n"
											"Type               : SOURCE NULL (pstPmtEsInfo)");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcAudioSelectSourceNull2,
											"SUT                : svc_si_UtSelectAudio\n"
											"Expected Result    : ERR_FAIL\n"
											"Type               : SOURCE NULL (pstConfig)");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcAudioSelectTargetNull,
											"SUT                : svc_si_UtSelectAudio\n"
											"Expected Result    : ERR_FAIL\n"
											"Type               : TARGET NULL (SelectedAudio)");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcAudioSelectSourceInvalid,
											"SUT                : svc_si_UtSelectAudio\n"
											"Expected Result    : ERR_FAIL\n"
											"Type               : SOURCE INVALID (pstPmtEsInfo)\n"
											"Comments           : Number of Audio ES Item in pmtEsInfo is \"0\"");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcAudioSelectNoMatchFirstTrack,
											"SUT                : svc_si_UtSelectAudio\n"
											"Expected Result    : ERR_OK, 0, eDxDUALMONO_SELECT_UNKNOWN\n"
											"Type               : Check 1st Track Has top priority\n"
											"Comments           : No Language, Multichannel and AudioDescription Matched\n"
											"                     In this case 1st Track Has Top Priority");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcAudioSelectStereoAud0,
											"SUT                : svc_si_UtSelectAudio\n"
											"Expected Result    : ERR_OK, 2, eDxDUALMONO_SELECT_UNKNOWN\n"
											"Type               : Lang Audio0\n"
											"Comments           : Find Lang Audio0 Matched Track");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcAudioSelectStereoAud1,
											"SUT                : svc_si_UtSelectAudio\n"
											"Expected Result    : ERR_OK, 3, eDxDUALMONO_SELECT_UNKNOWN\n"
											"Type               : Lang Audio1\n"
											"Comments           : Find Lang Audio1 Matched Track");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcAudioSelectStereoMenu,
											"SUT                : svc_si_UtSelectAudio\n"
											"Expected Result    : ERR_OK, 0, eDxDUALMONO_SELECT_UNKNOWN\n"
											"Type               : Lang Menu\n"
											"Comments           : Find Lang Menu Matched Track");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcAudioSelectStereoUser,
											"SUT                : svc_si_UtSelectAudio\n"
											"Expected Result    : ERR_OK, 2, eDxDUALMONO_SELECT_UNKNOWN\n"
											"Type               : Lang User\n"
											"Comments           : Find Lang User Matched Track");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcAudioSelectMultiUser1,
											"SUT                : svc_si_UtSelectAudio\n"
											"Expected Result    : ERR_OK, 2, eDxDUALMONO_SELECT_UNKNOWN\n"
											"Type               : Multichannel On by User\n"
											"Comments           : Find Multichannel Track");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcAudioSelectMultiUser2,
											"SUT                : svc_si_UtSelectAudio\n"
											"Expected Result    : ERR_OK, 3, eDxDUALMONO_SELECT_UNKNOWN\n"
											"Type               : Multichannel On by User and Audio Description Off\n"
											"Comments           : Find Multichannel Track (avoid AudioDescription selection if possible)");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcAudioSelectMultiAud0,
											"SUT                : svc_si_UtSelectAudio\n"
											"Expected Result    : ERR_OK, 1, eDxDUALMONO_SELECT_UNKNOWN\n"
											"Type               : Multichannel on Audio Description off and Language Matched (Many Multichannel Audio)\n"
											"Comments           : Find Multichannel Track and Lang Aud0");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcAudioSelectAdMultiAud0,
											"SUT                : svc_si_UtSelectAudio\n"
											"Expected Result    : ERR_OK, 0, eDxDUALMONO_SELECT_UNKNOWN\n"
											"Type               : Audio Description and Multichannel On by Menu\n"
											"Comments           : (Lang Aud0 Matched)");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcAudioSelectDualMonoRightUserLang,
											"SUT                : svc_si_UtSelectAudio\n"
											"Expected Result    : ERR_OK, 0, eDxDUALMONO_SELECT_RIGHT\n"
											"Type               : Find Lang User\n"
											"Comments           : User Lang is on Dual Mono Right Track");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcAudioSelectDualMonoNonUserLang,
											"SUT                : svc_si_UtSelectAudio\n"
											"Expected Result    : ERR_OK, 5, eDxDUALMONO_SELECT_UNKNOWN\n"
											"Type               : Find Lang User\n"
											"Comments           : User Lang Track is on both Dual Mono Right and Stereo Track\n"
											"                     Stereo has higher priority than dual mono");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcAudioSelectDualMonoLeftAud0,
											"SUT                : svc_si_UtSelectAudio\n"
											"Expected Result    : ERR_OK, 1, eDxDUALMONO_SELECT_LEFT\n"
											"Type               : Find Lang Aud0\n"
											"Comments           : Lang Aud0 is on Dual Mono Left Track");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcAudioSelectAdMenuLang,
											"SUT                : svc_si_UtSelectAudio\n"
											"Expected Result    : ERR_OK, 2, eDxDUALMONO_SELECT_UNKNOWN\n"
											"Type               : Find Audio Description Menu and Lang Menu\n"
											"Comments			:");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcAudioSelectAdLangNotMatched1,
											"SUT                : svc_si_UtSelectAudio\n"
											"Expected Result    : ERR_OK, 4, eDxDUALMONO_SELECT_UNKNOWN\n"
											"Type               : Find Audio Description and Lang\n"
											"Comments           : Found AD Track but language is not matched.\n"
											"                     Audio Description has higher priority than language");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcAudioSelectAdLangNotMatched2,
											"SUT                : svc_si_UtSelectAudio\n"
											"Expected Result    : ERR_OK, 2, eDxDUALMONO_SELECT_UNKNOWN\n"
											"Type               : Find Audio Description and Lang\n"
											"Comments           : Found AD Track but no language information.\n"
											"                     Even though there is no langauge information, Audio Description has higher priority.");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcAudioSelectAdAud1,
											"SUT                : svc_si_UtSelectAudio\n"
											"Expected Result    : ERR_OK, 0, eDxDUALMONO_SELECT_UNKNOWN\n"
											"Type               : Find Audio Description and Lang\n"
											"Comments           : Found AD Track and Lang Aud0");

	return pstSuite;
}

STATIC Cutest_Suite_t* svc_si_UtGetSuiteSubtitleSelect(void)
{
	Cutest_Suite_t	*pstSuite = CUTEST_SuiteNew();

//	___ES_SUBTITLE_TEST_CASE___

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcSubtitleSelectSourceNull1,
											"SUT                : svc_si_UtSubtitleEsSelect\n"
											"Expected Result    : ERR_FAIL\n"
											"Type               : SOURCE NULL (pstPmtEsInfo)");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcSubtitleSelectSourceNull2,
											"SUT                : svc_si_UtSubtitleEsSelect\n"
											"Expected Result    : ERR_FAIL\n"
											"Type               : SOURCE NULL (pstConfig)");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcSubtitleSelectTargetNull,
											"SUT                : svc_si_UtSubtitleEsSelect\n"
											"Expected Result    : ERR_FAIL\n"
											"Type               : TARGET NULL (SelectedAudio)");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcSubtitleSelectSourceInvalid,
											"SUT                : svc_si_UtSubtitleEsSelect\n"
											"Expected Result    : ERR_FAIL\n"
											"Type               : SOURCE INVALID (pstPmtEsInfo)\n"
											"Comments           : Number of Audio ES Item in pmtEsInfo is \"0\"");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcSubtitleSelectNoMatchFirstTrack,
											"SUT                : svc_si_UtSubtitleEsSelect\n"
											"Expected Result    : ERR_OK, 0\n"
											"Type               : Check 1st Track Has top priority\n"
											"Comments           : No Language, Multichannel and AudioDescription Matched\n"
											"                     In this case 1st Track Has Top Priority");

//	___ES_SUBTITLE_TEST_CASE_LANGUAGE___

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcSubtitleSelectLang0,
											"SUT 				: svc_si_UtSubtitleEsSelect\n"
											"Expected Result	: ERR_OK\n"
											"Type				: Lang Subtitle0\n"
											"Comments			: Find Lang Subtitle0 Matched Track\n");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcSubtitleSelectLang1,
											"SUT 				: svc_si_UtSubtitleEsSelect\n"
											"Expected Result 	: ERR_OK\n"
											"Type				: Lang Audio1\n"
											"Comments			: Find Lang Audio1 Matched Track");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcSubtitleSelectLangMenu,
											"SUT 				: svc_si_UtSubtitleEsSelect\n"
											"Expected Result 	: ERR_OK\n"
											"Type				: Lang OSD\n"
											"Comments			: Find Lang Menu Matched Track\n");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcSubtitleSelectLangUser,
											"SUT 				: svc_si_UtSubtitleEsSelect\n"
											"Expected Result	: ERR_OK\n"
											"Type				: Lang User\n"
											"Comments			: Find Lang User Matched Track\n");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcSubtitleSelectLangUndefined,
											"SUT 				: svc_si_UtSubtitleEsSelect\n"
											"Expected Result	: ERR_OK\n"
											"Type				: Lang Undefined\n"
											"Comments			: Find Lang Undefined Matched Track\n");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcSubtitleSelectLangOriginal,
											"SUT 				: svc_si_UtSubtitleEsSelect\n"
											"Expected Result	: ERR_OK\n"
											"Type				: Lang Original\n"
											"Comments			: Find Lang Original Matched Track\n");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcSubtitleSelectLangDefault,
											"SUT 				: svc_si_UtSubtitleEsSelect\n"
											"Expected Result	: ERR_OK\n"
											"Type				: Lang Default\n"
											"Comments			: Find Lang Default Matched Track\n");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcSubtitleSelectLangUndefinedAndOriginal,
											"SUT 				: svc_si_UtSubtitleEsSelect\n"
											"Expected Result	: ERR_OK\n"
											"Type				: Lang Default, Original\n"
											"Comments			: Lang Undefined > Lang Original\n");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcSubtitleSelectLangOriginalAndDefault,
											"SUT 				: svc_si_UtSubtitleEsSelect\n"
											"Expected Result	: ERR_OK\n"
											"Type				: Lang Original and Default\n"
											"Comments			: Lang Original > Lang Default\n");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcSubtitleSelectLangPriority1,
											"SUT 				: svc_si_UtSubtitleEsSelect\n"
											"Expected Result	: ERR_OK\n"
											"Type				: Lang Priority\n"
											"Comments			: Lang0 > Lang1\n");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcSubtitleSelectLangPriority2,
											"SUT 				: svc_si_UtSubtitleEsSelect\n"
											"Expected Result	: ERR_OK\n"
											"Type				: Lang Priority\n"
											"Comments			: Lang1 > LangOSD\n");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcSubtitleSelectLangPriority3,
											"SUT 				: svc_si_UtSubtitleEsSelect\n"
											"Expected Result	: ERR_OK\n"
											"Type				: Lang Priority\n"
											"Comments			: LangUser > Lang0\n");

//	___ES_SUBTITLE_TEST_CASE_MAIN_TYPE___

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcSubtitleSelectTypeDvb,
											"SUT 				: svc_si_UtSubtitleEsSelect\n"
											"Expected Result	: ERR_OK\n"
											"Type				: Main Type (DVB)\n"
											"Comments			: DVB > EBU\n");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcSubtitleSelectTypeDvbAndLang1,
											"SUT 				: svc_si_UtSubtitleEsSelect\n"
											"Expected Result	: ERR_OK\n"
											"Type				: Main Type (DVB)\n"
											"Comments			: Lang has higher priority than main type\n");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcSubtitleSelectTypeDvbAndLang2,
											"SUT 				: svc_si_UtSubtitleEsSelect\n"
											"Expected Result	: ERR_OK\n"
											"Type				: Main Type (DVB)\n"
											"Comments			: Lang has higher priority than main type\n");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcSubtitleSelectTypeDvbAndAr1,
											"SUT 				: svc_si_UtSubtitleEsSelect\n"
											"Expected Result 	: ERR_OK\n"
											"Type				: Main Type (DVB)\n"
											"Comments			: DVB Type and AR\n");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcSubtitleSelectTypeDvbAndAr2,
											"SUT 				: svc_si_UtSubtitleEsSelect\n"
											"Expected Result	: ERR_OK\n"
											"Type				: Main Type (DVB)\n"
											"Comments			: DVB Type and AR\n"
											"					: In same condition (Lang, MainType, AR) HOH has priority\n"
											"					: HOH > Lang > MainType > AR\n");

//	___ES_SUBTITLE_TEST_CASE_AR___

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcSubtitleSelectAr1,
											"SUT 				: svc_si_UtSubtitleEsSelect\n"
											"Expected Result	: ERR_OK\n"
											"Type				: AR Type\n"
											"Comments			: Find Matching AR\n");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcSubtitleSelectAr2,
											"SUT 				: svc_si_UtSubtitleEsSelect\n"
											"Expected Result	: ERR_OK\n"
											"Type				: AR Type\n"
											"Comments			: Find Matching AR\n");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcSubtitleSelectAr3,
											"SUT 				: svc_si_UtSubtitleEsSelect\n"
											"Expected Result	: ERR_OK\n"
											"Type				: AR Type and Lang\n"
											"Comments			: Lang > AR\n");


//	___ES_SUBTITLE_TEST_CASE_HD___

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcSubtitleSelectHd1,
											"SUT 				: svc_si_UtSubtitleEsSelect\n"
											"Expected Result	: ERR_OK\n"
											"Type				: HD Type\n"
											"Comments			: Find HD Type\n");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcSubtitleSelectHd2,
											"SUT 				: svc_si_UtSubtitleEsSelect\n"
											"Expected Result	: ERR_OK\n"
											"Type				: HD Type and Proper AR\n"
											"Comments			: Find HD Type and AR\n"
											"					: HD on and No AR\n"
											"					: HD > AR\n");

// ___ES_SUBTITLE_TEST_CASE_HOH___

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcSubtitleSelectHohOn1,
											"SUT 				: svc_si_UtSubtitleEsSelect\n"
											"Expected Result	: ERR_OK\n"
											"Type				: Hoh Type\n"
											"Comments			: Find Hoh Type\n"
											"					: HoH > Language\n");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcSubtitleSelectHohOn2,
											"SUT 				: svc_si_UtSubtitleEsSelect\n"
											"Expected Result	: ERR_OK\n"
											"Type				: Hoh Type\n"
											"Comments			: Find Hoh Type\n");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcSubtitleSelectHohOn3,
											"SUT 				: svc_si_UtSubtitleEsSelect\n"
											"Expected Result	: ERR_OK\n"
											"Type				: Hoh Type\n"
											"Comments			: Find Hoh Type but HoH is not exists\n"
											"					: HoH > Lang\n");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcSubtitleSelectHohOff1,
											"SUT 				: svc_si_UtSubtitleEsSelect\n"
											"Expected Result	: ERR_OK\n"
											"Type				: Hoh Type\n"
											"Comments			: Ignore Hoh Type\n"
											"					: With HoH menu item Off HoH Type has lower priorty then normal type\n");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcSubtitleSelectHohOff2,
											"SUT 				: svc_si_UtSubtitleEsSelect\n"
											"Expected Result	: ERR_OK\n"
											"Type				: Hoh Type\n"
											"Comments			: Ignore Hoh Type\n"
											"					: With HoH menu item Off HoH Type has lower priorty then normal type\n");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcSubtitleSelectHohOff3,
											"SUT 				: svc_si_UtSubtitleEsSelect\n"
											"Expected Result	: ERR_OK\n"
											"Type				: Hoh Type\n"
											"Comments			: Ignore Hoh Type\n"
											"					: HoH > Lang > MainType(DVB, EBU)\n");

// ___ES_SUBTITLE_TEST_CASE_HOH_AND_AR___

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcSubtitleSelectHohAndAr1,
											"SUT 				: svc_si_UtSubtitleEsSelect\n"
											"Expected Result	: ERR_OK\n"
											"Type				: Hoh On and AR 4x3\n"
											"Comments			: Find Hoh Type and Proper AR\n");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcSubtitleSelectHohAndAr2,
											"SUT 				: svc_si_UtSubtitleEsSelect\n"
											"Expected Result	: ERR_OK\n"
											"Type				: Hoh Off and AR 4x3\n"
											"Comments			: Find None Hoh Type and Proper AR\n");

	CUTEST_SUITE_ADD_TEST(pstSuite, svc_si_TcSubtitleSelectHohAndAr3,
											"SUT 				: svc_si_UtSubtitleEsSelect\n"
											"Expected Result	: ERR_OK\n"
											"Type				: Hoh On and AR 16x9\n"
											"Comments			: Find Hoh Type and AR 16x9\n"
											"					: Where is no HOH On And AR 16x9 ES\n"
											"					: HOH > AR\n");

	return pstSuite;
}

STATIC Cutest_Suite_t	*svc_si_UtGetSuite(void)
{
	static Cutest_Suite_t	*pstSuite = NULL;

	if (NULL == pstSuite)
	{
		pstSuite = CUTEST_SuiteNew();
	}

	return pstSuite;
}

STATIC int svc_si_UtPrepare(void *pvArgList)
{
	Cutest_Suite_t	*pstSuite = svc_si_UtGetSuite();

	svc_si_LibSvcEsPrintAudioPriorityTable();
	svc_si_LibSvcEsPrintSubtitlePriorityTable();

	if (NULL == pstSuite)
		return HxCMD_ERR;

	HLIB_CMD_Printf("Create SI Test Suite!\n");

	return HxCMD_OK;
}

STATIC int svc_si_UtLoadAudioSelect(Cutest_Suite_t *pstSuite)
{
	Cutest_Suite_t	*pstSuiteAudioSel;

	pstSuiteAudioSel = svc_si_UtGetSuiteAudioSelect();
	if (pstSuiteAudioSel)
	{
		CUTEST_SuiteMerge(pstSuite, pstSuiteAudioSel);
		HLIB_CMD_Printf("Load \"SI Audio Select\" Suite (%d)!\n", pstSuite->count);
	}

	return HxCMD_OK;
}

STATIC int svc_si_UtLoadSubtitleSelect(Cutest_Suite_t *pstSuite)
{
	Cutest_Suite_t	*pstSuiteSubtitleSel;

	pstSuiteSubtitleSel = svc_si_UtGetSuiteSubtitleSelect();
	if (pstSuiteSubtitleSel)
	{
		CUTEST_SuiteMerge(pstSuite, pstSuiteSubtitleSel);
		HLIB_CMD_Printf("Load \"SI Subtitle Select\" Suite (%d)!\n", pstSuite->count);
	}

	return HxCMD_OK;
}

STATIC int svc_si_UtLoadPlugin(Cutest_Suite_t *pstSuite)
{
	Cutest_Suite_t	*pstSuitePlugIn;

	pstSuitePlugIn = svc_si_UtGetSuites();

	if (pstSuitePlugIn)
	{
		CUTEST_SuiteMerge(pstSuite, pstSuitePlugIn);
		HLIB_CMD_Printf("Load \"SI Plug-In\" Suite (%d)!\n", pstSuite->count);
	}

	return HxCMD_OK;
}

STATIC int svc_si_UtLoad(void *pvArgList)
{
	HCHAR	 		*aucArg = NULL;
	HCHAR	 		 aucStr[32];
	Cutest_Suite_t	*pstSuite = NULL;
	HBOOL			 bAll, bAudio, bSubtitle, bPlugIn;

	if(pvArgList == NULL)
		return HxCMD_ERR;

	bAll = bAudio = bSubtitle = bPlugIn = FALSE;

    /********************
     * get 1st argument *
     ********************/
	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&pvArgList);
	if( aucArg == NULL )
	{
		bAll = TRUE;
	}
	else
	{
		if( sscanf(aucArg, "%s", (char *)&aucStr) == 0 ) return HxCMD_ERR;
	}

	if (FALSE == bAll)
	{
		HLIB_STD_StrUpper(aucStr);

		if (HxSTD_StrCmp(aucStr, "ALL") == 0)
		{
			bAll = TRUE;
		}
		else if (HxSTD_StrCmp(aucStr, "AUDIO") == 0)
		{
			bAudio = TRUE;
		}
		else if (HxSTD_StrCmp(aucStr, "SUBTITLE") == 0)
		{
			bSubtitle = TRUE;
		}
		else if (HxSTD_StrCmp(aucStr, "PLUGIN") == 0)
		{
			bPlugIn = TRUE;
		}
	}

	if (bAll)
	{
		bAudio = bSubtitle = bPlugIn = TRUE;
	}

	if (bAudio || bSubtitle || bPlugIn)
	{
		pstSuite = svc_si_UtGetSuite();

		if (bAudio)
		{
			svc_si_UtLoadAudioSelect(pstSuite);
		}

		if (bSubtitle)
		{
			svc_si_UtLoadSubtitleSelect(pstSuite);
		}

		if (bPlugIn)
		{
			svc_si_UtLoadPlugin(pstSuite);
		}

		CUTEST_MakeTestListToFile(pstSuite, "svc_si_UnitaryTest");

		HLIB_CMD_Printf("\tCreate Test List Report File!\n", pstSuite->count);

		return HxCMD_OK;
	}

	pstSuite = svc_si_UtGetSuite();
	if (pstSuite)
	{
		CUTEST_SuiteDelete(pstSuite);
	}

	return HxCMD_ERR;
}

STATIC int svc_si_UtRun(void *pvArgList)
{
	Cutest_String_t *pstOutput = CUTEST_StringNew();
	Cutest_Suite_t	*pstSuite = NULL;

//	svc_si_LibSvcEsPrintAudioPriorityTable();
//	svc_si_LibSvcEsPrintSubtitlePriorityTable();

	pstSuite = svc_si_UtGetSuite();
	if (pstSuite)
	{
		HLIB_CMD_Printf("\n\n\n (%d) SI Test Cases to Run !\n\n\n", pstSuite->count);

		CUTEST_SuiteRun(pstSuite, 0);

		CUTEST_SuiteSummary(pstSuite, pstOutput);
		CUTEST_SuiteDetail(pstSuite, pstOutput);

		CUTEST_MakeTestResultToFile(pstSuite, "svc_si_UnitaryTest");

		HLIB_CMD_Printf("\n\n\n SI Test Finished! (Success : %d, Fail : %d)\n\n\n", pstSuite->count - pstSuite->failCount, pstSuite->failCount);

		CUTEST_StringDelete(pstOutput);
		CUTEST_SuiteDelete(pstSuite);

		return HxCMD_OK;
	}

	HLIB_CMD_Printf("\n Fail!!!! No SI Test Cases to Run !!!!\n\n");
	return HxCMD_ERR;
}
#endif

void CMD_Register_SvcSi(void)
{
#define	hCmdHandle		"svc_si"

#ifdef CONFIG_DEBUG
	HLIB_CMD_RegisterWord(hCmdHandle,(void*)svc_si_PrintFilterState,
			"livesi",	/* keyword */
			"print live si table state", /*	 help  */
			"livesi");	/*	usage  */

	HLIB_CMD_RegisterWord(hCmdHandle,(void*)svc_si_FilterPrintStatus,
			"filter",	/* keyword */
			"print filter state", /*  help  */
			"filter");	/*	usage  */

	HLIB_CMD_RegisterWord(hCmdHandle,(void*)svc_si_UtPrepare,
			"ut_prepare",	/* keyword */
			"Unitary Test for SI Library Function", /*	help	*/
			"ut_prepare");	/*	usage  */

	HLIB_CMD_RegisterWord(hCmdHandle,(void*)svc_si_UtLoad,
			"ut_load",	/* keyword */
			"Unitary Test for SI Library Function", /*	help	*/
			"ut_load [all:audio:plugin]");	/*	usage  */

	HLIB_CMD_RegisterWord(hCmdHandle,(void*)svc_si_UtRun,
			"ut_run",			/* keyword */
			"Run Unitary Test", /*	help	*/
			"ut_run");			/*	usage  */

#endif
}


/* end of file */
