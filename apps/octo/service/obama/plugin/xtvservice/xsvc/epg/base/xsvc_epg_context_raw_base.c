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

#include "_svc_epg_common.h"
#include "_svc_epg_common_lib.h"
#include "_svc_epg_context_mgr.h"

#include "../local_include/_xsvc_epg.h"

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

STATIC svcEpg_RawProcessDataInit_t	s_astEPGProcessInitTable_BASE[] =
{
	{
		eEPG_PROCESS_EIT_PF,
		eEPG_CNXT_EMPTY,
		xsvc_epgcontext_PfProcess_Base,
		eEPG_SPEC_BASE,
	},
	{
		eEPG_PROCESS_EIT_PF,
		eEPG_CNXT_NOTUSED,
		NULL,
		eEPG_SPEC_BASE,
	},
	{
		eEPG_PROCESS_EIT_SCH,
		eEPG_CNXT_EMPTY,
		xsvc_epgcontext_SchProcess_Base,
		eEPG_SPEC_BASE,
	},
	{
		eEPG_PROCESS_EIT_SCH,
		eEPG_CNXT_NOTUSED,
		NULL,
		eEPG_SPEC_BASE,
	},
};


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

svcEpg_RawProcessDataInit_t*	xsvc_epgcontext_GetEpgProcessInitTable_Base(HUINT32* ulNumberOfTable)
{
	*ulNumberOfTable = sizeof(s_astEPGProcessInitTable_BASE)/sizeof(svcEpg_RawProcessDataInit_t);
	return s_astEPGProcessInitTable_BASE;
}


/* end of file */
