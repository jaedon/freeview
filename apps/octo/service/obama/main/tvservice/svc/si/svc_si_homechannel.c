/**
	@file     svc_si_homechannel.c
	@brief    a set of functions for si live process

	Description: Multifeed, Replacement Service 등을 다루기 위한 함수들. \n
	Module: MW/SILayer와 모듈 명을 적습니다. \n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
/** @brief global header file  */
#include <octo_common.h>

#include <db_svc.h>
#include <db_param.h>

#include <svc_pipe.h>
#include <pal_sirawpool.h>
#include <vkernel.h>
#include <silib.h>

/** @brief local header file  */
#include <_svc_si_homechannel.h>
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct tagSiHomeChanneInfo
{
	HUINT32				ulSemId;
	HBOOL				bValid;
	HINT32				nOrgDeliType;
	DbSvc_TripleId_t	stTripleId;
} svcSi_HomeChannelInfo_t;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

STATIC svcSi_HomeChannelInfo_t		s_stSiHomeChannel = {0, 0, FALSE, {0xffff, 0xffff, 0xffff}};

/*******************************************************************/
/********************     Local Functions       *************************/
/*******************************************************************/


#define _____LOCAL_FUNC_____

STATIC HERROR			svc_si_HomeChannelInitChannelInfoInternal(void)
{
	HERROR						hError = ERR_FAIL;
	svcSi_HomeChannelInfo_t		stLoadedInfo;

	HxSTD_memset(&stLoadedInfo, 0x00, sizeof(svcSi_HomeChannelInfo_t));

	hError = VK_SEM_Create((unsigned long*)&s_stSiHomeChannel.ulSemId, (const char*)"homech_semid", VK_SUSPENDTYPE_PRIORITY);
	if(hError != VK_OK)
	{
		HxLOG_Error("VK_SEM_Create fail \n");
		return hError;
	}

	VK_SEM_Get(s_stSiHomeChannel.ulSemId);

	s_stSiHomeChannel.bValid = FALSE;
	s_stSiHomeChannel.nOrgDeliType = 0;
	s_stSiHomeChannel.stTripleId.usOnId = 0xffff;
	s_stSiHomeChannel.stTripleId.usTsId = 0xffff;
	s_stSiHomeChannel.stTripleId.usSvcId = 0xffff;

	hError = DB_PARAM_GetItem(eDB_PARAM_ITEM_HOME_CHANNEL, (HUINT32 *)&stLoadedInfo, sizeof(svcSi_HomeChannelInfo_t));


	if(hError != ERR_OK)
	{
		HxLOG_Error("DB_PARAM_GetItem fail \n");
		hError = ERR_OK;
		goto EXIT_HOMECH_FUNC;
	}

	HxLOG_Print("nOrgDeliType(0x%x), usOnId(0x%x), usTsId(0x%x)\n",	stLoadedInfo.nOrgDeliType, stLoadedInfo.stTripleId.usOnId, stLoadedInfo.stTripleId.usTsId);
	s_stSiHomeChannel.bValid = TRUE;
	s_stSiHomeChannel.nOrgDeliType = stLoadedInfo.nOrgDeliType;
	s_stSiHomeChannel.stTripleId.usOnId = stLoadedInfo.stTripleId.usOnId;
	s_stSiHomeChannel.stTripleId.usTsId = stLoadedInfo.stTripleId.usTsId;
	s_stSiHomeChannel.stTripleId.usSvcId = stLoadedInfo.stTripleId.usSvcId;

	hError = ERR_OK;

EXIT_HOMECH_FUNC :
	VK_SEM_Release(s_stSiHomeChannel.ulSemId);

	return hError;
}

STATIC HERROR			svc_si_HomeChannelGetInfo(DbSvc_TripleId_t *pstChannelInfo, HINT32 *pnOrgDeliType, HBOOL *pbValid)
{
	if(pstChannelInfo == NULL || pbValid == NULL || pnOrgDeliType == NULL)
	{
		HxLOG_Error("null param\n");
		return ERR_FAIL;
	}

	VK_SEM_Get(s_stSiHomeChannel.ulSemId);
	*pbValid = s_stSiHomeChannel.bValid;
	*pnOrgDeliType = s_stSiHomeChannel.nOrgDeliType;
	HxSTD_memcpy(pstChannelInfo, &(s_stSiHomeChannel.stTripleId), sizeof(DbSvc_TripleId_t));
 	VK_SEM_Release(s_stSiHomeChannel.ulSemId);

	return ERR_OK;

}

STATIC HERROR			svc_si_HomeChannelSetInfo(HINT32 nDeliType, DbSvc_TripleId_t *pstChannelInfo)
{
	HERROR		hError = ERR_FAIL;

	if(pstChannelInfo == NULL)
	{
		HxLOG_Error("null param\n");
		return hError;
	}

	VK_SEM_Get(s_stSiHomeChannel.ulSemId);

	HxSTD_memcpy(&(s_stSiHomeChannel.stTripleId), pstChannelInfo, sizeof(DbSvc_TripleId_t));
	s_stSiHomeChannel.bValid = TRUE;
	s_stSiHomeChannel.nOrgDeliType = nDeliType;

	hError = DB_PARAM_SetItem(eDB_PARAM_ITEM_HOME_CHANNEL,
											(HUINT32)&s_stSiHomeChannel,
											(HUINT32)sizeof(svcSi_HomeChannelInfo_t));


	if(hError != ERR_OK)
	{
		HxLOG_Error("DB_PARAM_SetItem fail \n");
		goto EXIT_HOMECH_FUNC;
	}

	hError = ERR_OK;

EXIT_HOMECH_FUNC :
	VK_SEM_Release(s_stSiHomeChannel.ulSemId);

	return hError;
}

STATIC HERROR svc_si_HomeChannelProcessNitInternal (Handle_t hAction, HINT32 nDeliType, HUINT16 usTsUniqId, HUINT32 ulHead)
{
	HxList_t				*pstItem = NULL;
	SIxTable_PsiTable_t		*pstNitTable = NULL;
	DbSvc_TripleId_t		stTripleId = {0,};

	DbSvc_TripleId_t		stHomeInfo = {0,};
	HINT32					nTempDeliType = 0;
	HBOOL					bValid = FALSE;
	HBOOL					bFoundHomeTp = FALSE;

	HxLOG_Print("hAction[%x] nDeliType[%d] TsUid[%x] ulHead[%x] ActionId[%x]\n", hAction, nDeliType, usTsUniqId, ulHead, SVC_PIPE_GetActionId(hAction));

	if(svc_si_HomeChannelGetInfo(&stHomeInfo, &nTempDeliType, &bValid) != ERR_OK)
	{
		HxLOG_Error("svc_si_HomeChannelGetInfo fail \n");
		return ERR_FAIL;
	}

	if (ulHead == ACT_HEAD)
	{
		pstNitTable = PAL_SIRAWPOOL_ParseTable ( SVC_PIPE_GetActionId(hAction), eSIxTABLETYPE_NitAct, SILIB_TABLE_EXT_ID_ALL);
	}
	else
	{
		pstNitTable = PAL_SIRAWPOOL_ParseTable (SVC_PIPE_GetActionId(hAction), eSIxTABLETYPE_NitOth, SILIB_TABLE_EXT_ID_ALL);
	}

	if (pstNitTable == NULL)
	{
		HxLOG_Error ("pstNitTable is NULL\n");
		return ERR_FAIL;
	}

	for(pstItem = pstNitTable->pstDesLoop; pstItem != NULL; pstItem = pstItem->next)
	{
		HUINT8	*pucDesRaw = (HUINT8 *)HLIB_LIST_Data (pstItem);
		SIxLinkDes_t *pstLinkDes = NULL;

		if (*pucDesRaw == eSIxDESCTAG_LINKAGE)
		{
			pstLinkDes = (SIxLinkDes_t *)PAL_SIRAWPOOL_ParseDes (SVC_PIPE_GetActionId(hAction), (HUINT8 *)pucDesRaw);
			if (pstLinkDes != NULL)
			{
				if (eLinkageType_Home == pstLinkDes->ucLinkType)
				{
					stTripleId.usOnId = pstLinkDes->usOrgNetId;
					stTripleId.usTsId = pstLinkDes->usTsId;
					bFoundHomeTp = TRUE;
					HxLOG_Print("HomeTp: ucLinkType(0x%08x) Found (%04x:%04x:%04x)\n", pstLinkDes->ucLinkType, pstLinkDes->usOrgNetId, pstLinkDes->usTsId, pstLinkDes->usSvcId);

					PAL_SIRAWPOOL_FreeDes(pstLinkDes);
					break;
				}
				else
				{
					PAL_SIRAWPOOL_FreeDes(pstLinkDes);
				}
			}
		}
	}

	if(TRUE == bFoundHomeTp)
	{
		if(stHomeInfo.usOnId != stTripleId.usOnId || stHomeInfo.usTsId != stTripleId.usTsId)
		{
			HxLOG_Info("bNeedSave\n");
			stHomeInfo.usOnId = stTripleId.usOnId;
			stHomeInfo.usTsId = stTripleId.usTsId;
			if(svc_si_HomeChannelSetInfo(nDeliType, &stHomeInfo) != ERR_OK)
			{
				HxLOG_Print("svc_si_HomeChannelSetInfo fail \n");
			}
		}
	}

	if (NULL != pstNitTable)				{ PAL_SIRAWPOOL_FreeTable (pstNitTable); }

	return ERR_OK;;
}


#define _____GLOBAL_FUNC_____
HERROR svc_si_HomeChannelProcessNit(Handle_t hAction, HINT32 nDeliType, HUINT16 usTsUniqId, HUINT32 ulHead)
{
	return svc_si_HomeChannelProcessNitInternal(hAction, nDeliType, usTsUniqId, ulHead);
}

HERROR svc_si_HomeChannelGetChannelInfo(DbSvc_TripleId_t *pstChannelInfo, HINT32 *pnOrgDeliType, HBOOL *pbValid)
{
	return svc_si_HomeChannelGetInfo(pstChannelInfo, pnOrgDeliType, pbValid);
}

HERROR svc_si_HomeChannelInitChannelInfo(void)
{
	return svc_si_HomeChannelInitChannelInfoInternal();
}


/* end of file */


/*********************** End of File ******************************/
