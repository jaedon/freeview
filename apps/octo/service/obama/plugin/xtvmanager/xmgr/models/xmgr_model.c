/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xmgr_model.c
	@brief

	Description:  									\n
	Module: Portinig Layer / Initialization				\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding �� ����ϴ�?source file �Դϴ�.
* MS949 encoding�����Ī��� source file�Ǫ���
* Quelldatei, die MS949-Codierung verwendet.
*
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <hlib.h>
#include <octo_common.h>
#include <db_svc.h>
#include <svc_si.h>
#include <mgr_live.h>
#include <mgr_search.h>
#include <mgr_swup.h>

#include "local_include/_xmgr_model.h"

HERROR xmgr_model_GetSiEsPriorityTable_Base (SvcSi_EsPriorityTable_t *pstTable)
{
	STATIC SvcSi_EsTypePriority_t s_astAudioPriorityTable_Base[] =
	{
		{ eSvcSi_AUD_PRIORITY_LANG_BASE,							eSvcSi_AUD_SEL_TYPE_NUM,							1000	},
		{ eSvcSi_AUD_PRIORITY_LANG_USER,							eSvcSi_AUD_SEL_LANG_USER,							8000	},
		{ eSvcSi_AUD_PRIORITY_LANG_USER_WITH_PID,					eSvcSi_AUD_SEL_LANG_USER_WITH_PID,					9000	},
		{ eSvcSi_AUD_PRIORITY_LANG_0,								eSvcSi_AUD_SEL_LANG_0,								7000	},
		{ eSvcSi_AUD_PRIORITY_LANG_1,								eSvcSi_AUD_SEL_LANG_1,								6000	},
		{ eSvcSi_AUD_PRIORITY_LANG_OSD,								eSvcSi_AUD_SEL_LANG_OSD,							5000	},
		{ eSvcSi_AUD_PRIORITY_LANG_UNDEFINED,						eSvcSi_AUD_SEL_LANG_UNDEFINED,						4000	},
		{ eSvcSi_AUD_PRIORITY_LANG_ORIGINAL,						eSvcSi_AUD_SEL_LANG_ORIGINAL,						3000	},
		{eSvcSi_AUD_PRIORITY_LANG_DEFAULT,							eSvcSi_AUD_SEL_LANG_DEFAULT, 						2000	},
		{ eSvcSi_AUD_PRIORITY_DESCRIPTION_BROADCASTER_MIX_USER, 	eSvcSi_AUD_SEL_DESCRIPTION_BROADCASTER_MIX_USER,	70000	},
		{ eSvcSi_AUD_PRIORITY_DESCRIPTION_BROADCASTER_MIX_MENU, 	eSvcSi_AUD_SEL_DESCRIPTION_BROADCASTER_MIX_MENU,	60000	},
		{ eSvcSi_AUD_PRIORITY_DESCRIPTION_OFF_BROADCASTER_MIX_ON,	eSvcSi_AUD_SEL_DESCRIPTION_OFF_BROADCASTER_MIX_ON,	20000	},
		{ eSvcSi_AUD_PRIORITY_DESCRIPTION_BROADCASTER_MIX_BASE, 	eSvcSi_AUD_SEL_TYPE_NUM,							10000	},
		{ eSvcSi_AUD_PRIORITY_DESCRIPTION_RECEIVER_MIX_USER,		eSvcSi_AUD_SEL_DESCRIPTION_RECEIVER_MIX_USER,		50000	},
		{ eSvcSi_AUD_PRIORITY_DESCRIPTION_RECEIVER_MIX_MENU,		eSvcSi_AUD_SEL_DESCRIPTION_RECEIVER_MIX_MENU,		40000	},
		{ eSvcSi_AUD_PRIORITY_DESCRIPTION_OFF_RECEIVER_MIX_ON,		eSvcSi_AUD_SEL_DESCRIPTION_OFF_RECEIVER_MIX_ON, 	20000	},
		{ eSvcSi_AUD_PRIORITY_DESCRIPTION_RECEIVER_MIX_BASE,		eSvcSi_AUD_SEL_TYPE_NUM,							10000	},
		{ eSvcSi_AUD_PRIORITY_DESCRIPTION_OFF_BROADCASTER_MIX_OFF,	eSvcSi_AUD_SEL_DESCRIPTION_OFF_BROADCASTER_MIX_OFF, 30000	},
		{ eSvcSi_AUD_PRIORITY_DESCRIPTION_OFF_RECEIVER_MIX_OFF, 	eSvcSi_AUD_SEL_DESCRIPTION_OFF_RECEIVER_MIX_OFF,	30000	},
		{ eSvcSi_AUD_PRIORITY_DESCRIPTION_ON_BROADCASTER_MIX_OFF,	eSvcSi_AUD_SEL_DESCRIPTION_ON_BROADCASTER_MIX_OFF,	30000	},
		{ eSvcSi_AUD_PRIORITY_DESCRIPTION_ON_RECEIVER_MIX_OFF,		eSvcSi_AUD_SEL_DESCRIPTION_ON_RECEIVER_MIX_OFF, 	30000	},
		{ eSvcSi_AUD_PRIORITY_MULTICHANNEL_BASE,					eSvcSi_AUD_SEL_TYPE_NUM,							0		},
		{ eSvcSi_AUD_PRIORITY_MULTICHANNEL_USER,					eSvcSi_AUD_SEL_MULTICHANNEL_USER,					200 	},
		{ eSvcSi_AUD_PRIORITY_MULTICHANNEL_MENU,					eSvcSi_AUD_SEL_MULTICHANNEL_MENU,					100 	},
		{ eSvcSi_AUD_PRIORITY_NORMAL_AUDIO, 						eSvcSi_AUD_SEL_TYPE_NUM,							2		},
		{ eSvcSi_AUD_PRIORITY_FIRST_TRACK,							eSvcSi_AUD_SEL_TYPE_NUM, 							1		},
		{ eSvcSi_AUD_PRIORITY_DUAL_MONO,							eSvcSi_AUD_SEL_TYPE_NUM,							-10 	},
		{ eSvcSi_AUD_PRIORITY_TYPE_NUM, 							eSvcSi_AUD_SEL_TYPE_NUM,							-1		}
	};

	STATIC SvcSi_EsTypePriority_t s_astSubtitlePriorityTable_Base[] =
	{
		{	eSvcSi_SUBTTL_PRIORITY_LANG_BASE,					eSvcSi_SUBTTL_SEL_TYPE_NUM, 						10000	},
		{	eSvcSi_SUBTTL_PRIORITY_LANG_USER,					eSvcSi_SUBTTL_SEL_LANG_USER,						80000	},
		{	eSvcSi_SUBTTL_PRIORITY_LANG_USER_WITH_IDX,			eSvcSi_SUBTTL_SEL_LANG_USER_WITH_IDX,				90000	},
		{	eSvcSi_SUBTTL_PRIORITY_LANG_MENU_0, 				eSvcSi_SUBTTL_SEL_LANG_0,							70000	},
		{	eSvcSi_SUBTTL_PRIORITY_LANG_MENU_1, 				eSvcSi_SUBTTL_SEL_LANG_1,							60000	},
		{	eSvcSi_SUBTTL_PRIORITY_LANG_OSD,					eSvcSi_SUBTTL_SEL_LANG_OSD, 						50000	},
		{	eSvcSi_SUBTTL_PRIORITY_LANG_UNDEFINED,				eSvcSi_SUBTTL_SEL_LANG_UNDEFINED,					40000	},
		{	eSvcSi_SUBTTL_PRIORITY_LANG_ORIGINAL,				eSvcSi_SUBTTL_SEL_LANG_ORIGINAL,					30000	},
		{	eSvcSi_SUBTTL_PRIORITY_LANG_DEFAULT,				eSvcSi_SUBTTL_SEL_LANG_DEFAULT, 					20000	},
		{	eSvcSi_SUBTTL_PRIORITY_TYPE_BASE,					eSvcSi_SUBTTL_SEL_TYPE_NUM, 						1000	},
		{	eSvcSi_SUBTTL_PRIORITY_TYPE_DVB_MATCHED_USER,		eSvcSi_SUBTTL_SEL_TYPE_DVB_USER,					7000	},
		{	eSvcSi_SUBTTL_PRIORITY_TYPE_DVB_MATCHED_MENU,		eSvcSi_SUBTTL_SEL_TYPE_DVB_MENU,					6000	},
		{	eSvcSi_SUBTTL_PRIORITY_TYPE_DVB_MATCHED_AUTO,		eSvcSi_SUBTTL_SEL_TYPE_DVB_AUTO,					5000	},
		{	eSvcSi_SUBTTL_PRIORITY_TYPE_EBU_MATCHED_USER,		eSvcSi_SUBTTL_SEL_TYPE_EBU_USER,					4000	},
		{	eSvcSi_SUBTTL_PRIORITY_TYPE_EBU_MATCHED_MENU,		eSvcSi_SUBTTL_SEL_TYPE_EBU_MENU,					3000	},
		{	eSvcSi_SUBTTL_PRIORITY_TYPE_EBU_MATCHED_AUTO,		eSvcSi_SUBTTL_SEL_TYPE_EBU_AUTO,					2000	},
		{	eSvcSi_SUBTTL_PRIORITY_HOH_BASE,					eSvcSi_SUBTTL_SEL_TYPE_NUM, 						100000	},
		{	eSvcSi_SUBTTL_PRIORITY_HOH_MATCHED_USER,			eSvcSi_SUBTTL_SEL_HOH_USER, 						400000	},
		{	eSvcSi_SUBTTL_PRIORITY_HOH_MATCHED_MENU,			eSvcSi_SUBTTL_SEL_HOH_MENU, 						300000	},
		{	eSvcSi_SUBTTL_PRIORITY_HOH_OFF_ES_YES,				eSvcSi_SUBTTL_SEL_HOH_OFF_ES_YES,					100000	},
		{	eSvcSi_SUBTTL_PRIORITY_HOH_OFF_ES_NO,				eSvcSi_SUBTTL_SEL_HOH_OFF_ES_NO,					200000	},
		{	eSvcSi_SUBTTL_PRIORITY_HOH_ON_ES_NO,				eSvcSi_SUBTTL_SEL_HOH_ON_ES_NO, 					200000	},
		{	eSvcSi_SUBTTL_PRIORITY_ASPECTRATIO_BASE,			eSvcSi_SUBTTL_SEL_TYPE_NUM, 						10		},
		{	eSvcSi_SUBTTL_PRIORITY_ASPECTRATIO_NA,				eSvcSi_SUBTTL_SEL_ASPECTRATIO_NA,					20		},
		{	eSvcSi_SUBTTL_PRIORITY_ASPECTRATIO_MATCHED_YES, 	eSvcSi_SUBTTL_SEL_ASPECTRATIO_YES,					30		},
		{	eSvcSi_SUBTTL_PRIORITY_HD_BASE, 					eSvcSi_SUBTTL_SEL_TYPE_NUM, 						100 	},
		{	eSvcSi_SUBTTL_PRIORITY_HD_NA,						eSvcSi_SUBTTL_SEL_HD_NA,							300 	},
		{	eSvcSi_SUBTTL_PRIORITY_HD_MATCHED_USER, 			eSvcSi_SUBTTL_SEL_HD_USER,							400 	},
		{	eSvcSi_SUBTTL_PRIORITY_HD_MATCHED_MENU, 			eSvcSi_SUBTTL_SEL_HD_MENU,							500 	},
		{	eSvcSi_SUBTTL_PRIORITY_FIRST_TRACK_BASE,			eSvcSi_SUBTTL_SEL_TYPE_NUM, 						0		},
		{	eSvcSi_SUBTTL_PRIORITY_FIRST_TRACK_YES, 			eSvcSi_SUBTTL_SEL_TRACK,							1		},
		{	eSvcSi_SUBTTL_PRIORITY_FIRST_TRACK_NO,				eSvcSi_SUBTTL_SEL_TYPE_NUM, 						0		},
		{	eSvcSi_SUBTTL_PRIORITY_TYPE_NUM,					-1, 												0		}
	};

	if (NULL == pstTable)
		return ERR_FAIL;

	HxSTD_MemSet(pstTable, 0x00, sizeof(SvcSi_EsPriorityTable_t));

	pstTable->astAudio = s_astAudioPriorityTable_Base;
	pstTable->ulItemNumAudio = sizeof(s_astAudioPriorityTable_Base) / sizeof(SvcSi_EsTypePriority_t);

	pstTable->astSubtitle = s_astSubtitlePriorityTable_Base;
	pstTable->ulItemNumSubtitle = sizeof(s_astSubtitlePriorityTable_Base) / sizeof(SvcSi_EsTypePriority_t);

	return ERR_OK;
}


/* end of file */
