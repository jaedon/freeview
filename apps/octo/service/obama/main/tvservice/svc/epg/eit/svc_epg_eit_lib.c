/**
	@file     file_name.c File Name을 적습니다.
	@brief    file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include "util.h"
#include "linkedlist.h"
#include <db_svc.h>
#include <db_param.h>
#include "isosvc.h"
#include <svc_epg.h>
#include "../local_include/_svc_epg_common.h"

#include <psvc_epg.h>
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/


#define ____GLOBAL_FUNC____
HERROR				SVC_EPGEIT_GetSectionInfo(EPG_SpecType_e eSpec, HUINT8 *pucRawSection, svcEpg_ServiceInfo_t *pstSectionInfo)
{
	return psvc_epg_GetSectionInfo(eSpec, pucRawSection, pstSectionInfo);
}

#define	EIT_LANG_UND_STR		"und"
#define	EIT_LANG_QAA_STR		"qaa"
#define	EIT_LANG_DEF_STR		"eng"	/* 우선은 이렇게 하고, 사양 별로 다시 정의 */
/****************************************************************************
Description	:
Parameters	:	(1) Secondary System Lang을 지원하는 곳이 있으면 이 함수를 두번 호출하면 된다. - 근데 현재 관련 API가 없네.
Output		:
Return		:	(1) 최소값은 1 Retun, 콜하는 쪽에서 초기값을 0으로 해서, 매치되는 놈이 없을 때 첫번째 놈이 들어가게 한다.
****************************************************************************/
HUINT32		SVC_EPGEIT_GetPriorityByLangIso639(EPG_SpecType_e eSpec, SvcEpg_FindParam_t *pParam, HUINT8 *pIso639)
{
	HUINT32			myPriority = 1;

	HAPPY(eSpec);

	if ((pIso639 == NULL) || (pParam == NULL))
	{
		return 0;
	}

	if ((VK_strncmp(pParam->aucManuLang1, pIso639, EPG_LANG_ISO_639_LEN) == 0) || (VK_strncmp(pParam->aucManuLang2, pIso639, EPG_LANG_ISO_639_LEN) == 0))
	{
		myPriority = 900;
	}
	else if ((VK_strncmp(EIT_LANG_UND_STR, pIso639, EPG_LANG_ISO_639_LEN) == 0) || (VK_strncmp(EIT_LANG_QAA_STR, pIso639, EPG_LANG_ISO_639_LEN) == 0))
	{
		/* 이부분은 UK 향에서 필요한데 딴 향에서 딱히 있어도 문제가 안되므로 둠. 혹 문제 되면 막기 */
		myPriority = 500;
	}
	else if (VK_strncmp(EIT_LANG_DEF_STR, pIso639, EPG_LANG_ISO_639_LEN) == 0)
	{
		myPriority = 100;
	}

	return myPriority;
}
/****************************************************************************
Description	:	(1) Country Code와 매치되는 놈만 보낸다. 임시로 첫번째 놈 보내기. 국가 코드 일치되는 놈만 보내야 하는 것 아닌가 ?
Parameters	:
Output		:
Return		:
****************************************************************************/
HUINT32		SVC_EPGEIT_GetPriorityByCountryIso3166(EPG_SpecType_e eSpec, SvcEpg_FindParam_t *pParam, HUINT8 *pIso3166)
{
	HUINT32		myPriority = 1;

	HAPPY(eSpec);

	if ((pIso3166 == NULL) || (pParam == NULL))
	{
		return myPriority;
	}

	if (VK_strncmp(pParam->aucCountry, pIso3166, EPG_LANG_ISO_639_LEN) == 0)
	{
		myPriority = 900;
	}

	return myPriority;
}



/*********************** End of File ******************************/
