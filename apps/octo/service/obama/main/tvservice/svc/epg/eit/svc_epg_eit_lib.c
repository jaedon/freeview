/**
	@file     file_name.c File Name�� �����ϴ�.
	@brief    file_name & simple comment.

	Description: File�� ���� ������ �����ϴ�.		\n
	Module: Layer�� ��� ���� �����ϴ�.			 	\n
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
#define	EIT_LANG_DEF_STR		"eng"	/* �켱�� �̷��� �ϰ�, ��� ���� �ٽ� ���� */
/****************************************************************************
Description	:
Parameters	:	(1) Secondary System Lang�� �����ϴ� ���� ������ �� �Լ��� �ι� ȣ���ϸ� �ȴ�. - �ٵ� ���� ���� API�� ����.
Output		:
Return		:	(1) �ּҰ��� 1 Retun, ���ϴ� �ʿ��� �ʱⰪ�� 0���� �ؼ�, ��ġ�Ǵ� ���� ���� �� ù��° ���� ���� �Ѵ�.
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
		/* �̺κ��� UK �⿡�� �ʿ��ѵ� �� �⿡�� ���� �־ ������ �ȵǹǷ� ��. Ȥ ���� �Ǹ� ���� */
		myPriority = 500;
	}
	else if (VK_strncmp(EIT_LANG_DEF_STR, pIso639, EPG_LANG_ISO_639_LEN) == 0)
	{
		myPriority = 100;
	}

	return myPriority;
}
/****************************************************************************
Description	:	(1) Country Code�� ��ġ�Ǵ� �� ������. �ӽ÷� ù��° �� ������. ���� �ڵ� ��ġ�Ǵ� �� ������ �ϴ� �� �ƴѰ� ?
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
