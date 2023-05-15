/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_ncd_ukdtt.c
	@brief

	Description:  									\n
	Module: Porting Layer/TV Service/SI /LIVE			\n

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
#include <hlib.h>
#include <silib.h>
#include <vkernel.h>
#include <octo_common.h>
#include <isosvc.h>
#include <pal_pipe.h>
#include <pal_channel_ter.h>
#include <pal_sirawpool.h>

#include <db_svc.h>
#include <svc_si.h>
#include <_svc_si.h>
#include <_svc_si_filtering.h>
#include <_svc_si_live.h>
#include <xsvc_si.h>
#include <_xsvc_si.h>
#include <_xsvc_si_lib_live_base.h>

#include <_xsvc_si_spec_ukdtt.h>
/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HUINT8			ucChangeId;
	HUINT8			ucChangeVer;
	HUINT32			ulStartTime;
	HUINT32			ulEndTime;
	HBOOL			bMajor;

	XsvcSi_NcdInfoMsg_UkDtt_t	stMessage[XsvcSi_UKDTT_MAX_NCD_MESSAGE_NUM];

	HUINT8			ucMsgId;
	HUINT16			usDemodCellId;
	HUINT8			ucChangeType;

	HUINT8			ucInvariantTsPresent;
	HUINT16			usInvariantTsId;
	HUINT16			usInvariantOnId;
} XsvcSi_NcdInfo_UkDtt_t;

typedef struct
{
	HUINT32				ulCnt;
	HUINT32				ulCrc32;
	XsvcSi_NcdInfo_UkDtt_t		*pstNcdInfo;
} XsvcSi_NcdInfoList_UkDtt_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
XsvcSi_NcdInfoList_UkDtt_t	s_stNcdInfoList;

#define ___LOCAL_FUNCTIONS___
STATIC void xsvc_si_NcdInitInfo_UkDtt(void)
{
	if(s_stNcdInfoList.pstNcdInfo != NULL)
	{
		OxSI_Free(s_stNcdInfoList.pstNcdInfo);
		s_stNcdInfoList.pstNcdInfo = NULL;
	}

	HxSTD_memset(&s_stNcdInfoList, 0x00, sizeof(XsvcSi_NcdInfoList_UkDtt_t) );
}

XsvcSi_NcdInfoList_UkDtt_t *xsvc_si_NcdGetInfo_UkDtt(void)
{
	return &s_stNcdInfoList;
}

STATIC void xsvc_si_NcdSetInfo_UkDtt(HUINT32 ulCnt, HUINT32 ulCrc, XsvcSi_NcdInfo_UkDtt_t *pstNcdInfo)
{
	if(s_stNcdInfoList.pstNcdInfo != NULL)
	{
		OxSI_Free(s_stNcdInfoList.pstNcdInfo );
		s_stNcdInfoList.pstNcdInfo = NULL;
	}

	s_stNcdInfoList.pstNcdInfo  = (XsvcSi_NcdInfo_UkDtt_t *)OxSI_Malloc(sizeof(XsvcSi_NcdInfo_UkDtt_t) * ulCnt);
	if(s_stNcdInfoList.pstNcdInfo == NULL)
	{
		HxSTD_memset(&s_stNcdInfoList, 0x00, sizeof(XsvcSi_NcdInfoList_UkDtt_t));

		HxLOG_Print("MemAlloc Fail, So Memset \n");

		return;
	}

	s_stNcdInfoList.ulCnt = ulCnt;
	s_stNcdInfoList.ulCrc32 = ulCrc;
	HxSTD_memcpy(s_stNcdInfoList.pstNcdInfo, pstNcdInfo, sizeof(XsvcSi_NcdInfo_UkDtt_t)*ulCnt);

	HxLOG_Print("ulCnt(%d) ulCrc(x%x) pstNcdInfoList(%p) \n", s_stNcdInfoList.ulCnt,  s_stNcdInfoList.ulCrc32, s_stNcdInfoList.pstNcdInfo);
}

STATIC void xsvc_si_NcdFindFirstInfo_UkDtt(UNIXTIME ulLastScanTime, HUINT32 *ulIndexFindNCD, HBOOL *bFindNCD)
{
	HUINT32				ulIndex = 0, ulNotifyTime =0xffffffff;

	HUINT32				ulNcdCnt, ulNcdCrc;
	XsvcSi_NcdInfo_UkDtt_t *pstNcdInfo= NULL;
	XsvcSi_NcdInfoList_UkDtt_t *pstNcdInfoList= NULL;

	pstNcdInfoList = xsvc_si_NcdGetInfo_UkDtt();

	ulNcdCnt 		= pstNcdInfoList->ulCnt;
	ulNcdCrc 		= pstNcdInfoList->ulCrc32;
	pstNcdInfo 	= pstNcdInfoList->pstNcdInfo;

	*bFindNCD = FALSE;
	*ulIndexFindNCD = 0;

	HxLOG_Print("ulCnt(%d) ulCrc(0x%x) pstNcdInfo(%p) \n", ulNcdCnt, ulNcdCrc, pstNcdInfo);

	for (ulIndex = 0; ulIndex < ulNcdCnt; ulIndex++)
	{
		if((ulNotifyTime > pstNcdInfo[ulIndex].ulStartTime) &&( pstNcdInfo[ulIndex].ulEndTime> ulLastScanTime))
		{
			ulNotifyTime = pstNcdInfo[ulIndex].ulStartTime;

			*ulIndexFindNCD = ulIndex;
			*bFindNCD = TRUE;

			HxLOG_Print("Found NCD Info ==  ulIndex(%d) ulNotifyTime(%d) \n", ulIndex, ulNotifyTime);

//			return;
		}
	}

//	*bFindNCD = FALSE;
//	*ulIndexFindNCD = 0;

	return;
}


STATIC void xsvc_si_NcdSetInDB_UkDtt(XsvcSi_NcdInfo_UkDtt_t *pstNcdInfo, HUINT32 ulIndexFindNcd, XsvcSi_NcdInfoData_UkDtt_t	*pstDestPrevNcdInfo)
{
	HERROR		hErr;
	HUINT32 		i;
	HUINT8		*pucMenuLang = NULL, *pMessage = NULL;
	HBOOL		bFindMessage = FALSE;

	HxDATETIME_t 		stDateTime;
	HUINT32			ulWeekdayIndex = 0;
	HUINT32			ulDuration = 0, ulDurationHour = 0, ulDurationMin =0 , ulDurationSec =0;
	HxLANG_Id_e		eLangId;
	XsvcSi_NcdInfoData_UkDtt_t	stPrevNcdInfo;


	HxLOG_Print("Cell id[ %d ] ->-> RE NCD reservation !  \n",pstNcdInfo[ulIndexFindNcd].usDemodCellId);

	HxSTD_memset(&stDateTime, 0, sizeof(HxDATETIME_t));

	ulDuration =  pstNcdInfo[ulIndexFindNcd].ulEndTime - pstNcdInfo[ulIndexFindNcd].ulStartTime;
	ulDurationHour = ulDuration/3600;
	ulDurationMin = (ulDuration%3600)/60;
	ulDurationSec = ulDuration%60;

	hErr = HLIB_DATETIME_ConvertUnixTimeToDateTime (pstNcdInfo[ulIndexFindNcd].ulStartTime, &stDateTime);
	ulWeekdayIndex = (HUINT8)HLIB_DATETIME_GetWeekDayIndex(stDateTime.stDate);

	/* Reservation Data Update */
	HxSTD_memset(&stPrevNcdInfo, 0, sizeof(XsvcSi_NcdInfoData_UkDtt_t));
	stPrevNcdInfo.bValid = TRUE;
	stPrevNcdInfo.ucNetChangeId = pstNcdInfo[ulIndexFindNcd].ucChangeId;
	stPrevNcdInfo.ucNetChangeVer = pstNcdInfo[ulIndexFindNcd].ucChangeVer;
	stPrevNcdInfo.ulStartUnixtime = pstNcdInfo[ulIndexFindNcd].ulStartTime;
	stPrevNcdInfo.ulDuration = pstNcdInfo[ulIndexFindNcd].ulEndTime - pstNcdInfo[ulIndexFindNcd].ulStartTime;
	stPrevNcdInfo.ucChangeType = pstNcdInfo[ulIndexFindNcd].ucChangeType;
	stPrevNcdInfo.usDemodCellId = pstNcdInfo[ulIndexFindNcd].usDemodCellId;

	stPrevNcdInfo.ucInvariantTsPresent = pstNcdInfo[ulIndexFindNcd].ucInvariantTsPresent;
	stPrevNcdInfo.usInvariantTsId = pstNcdInfo[ulIndexFindNcd].usInvariantTsId;
	stPrevNcdInfo.usInvariantOnId = pstNcdInfo[ulIndexFindNcd].usInvariantOnId;

	HxSTD_memcpy(stPrevNcdInfo.message, pstNcdInfo[ulIndexFindNcd].stMessage, sizeof(XsvcSi_NcdInfoMsg_UkDtt_t)*XsvcSi_UKDTT_MAX_NCD_MESSAGE_NUM);

	DB_PARAM_SetItem(eDB_PARAM_ITEM_ASO_NETWORK_CHANGE_INFO, (HUINT32)&stPrevNcdInfo, sizeof(XsvcSi_NcdInfoData_UkDtt_t));

	/*compare with current menu language. "eng", "wel"("cym"), "gla", "gle"*/
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_MAIN, (HINT32 *)&eLangId, 0);
	if (hErr == ERR_OK)
	{
		pucMenuLang = (HCHAR*)HLIB_LANG_IdTo639(eLangId);
		if (hErr == ERR_OK)
		{
			HLIB_STD_StrLower(pucMenuLang);
		}
	}



	for(i=0;i<XsvcSi_UKDTT_MAX_NCD_MESSAGE_NUM;i++)
	{
		if ((pucMenuLang!= NULL) && HxSTD_StrNCmp (stPrevNcdInfo.message[i].iso639, pucMenuLang,3) == 0)
		{
			bFindMessage = TRUE;

			if(stPrevNcdInfo.message[i].ucText != NULL)
			{
				DB_PARAM_SetItem(eDB_PARAM_ITEM_ASO_NETWORK_MSG_INFO, (HUINT32)stPrevNcdInfo.message[i].ucText, DxNCD_MAX_MESSAGE_LEN );

			}
			else
			{
				pMessage = "No information available";

				DB_PARAM_SetItem(eDB_PARAM_ITEM_ASO_NETWORK_MSG_INFO, (HUINT32)pMessage, DxNCD_MAX_MESSAGE_LEN );
			}
			break;
		}
	}

	if(bFindMessage == FALSE)
	{
		pMessage = "No information available";

		DB_PARAM_SetItem(eDB_PARAM_ITEM_ASO_NETWORK_MSG_INFO, (HUINT32)pMessage, DxNCD_MAX_MESSAGE_LEN );
	}

	HxSTD_memcpy(pstDestPrevNcdInfo, &stPrevNcdInfo, sizeof(XsvcSi_NcdInfoData_UkDtt_t));

}

STATIC void xsvc_si_NcdSetInSchedule_UkDtt(UNIXTIME	ulSysTime,UNIXTIME	ulPrevNCDStarttime,HUINT32	ulPrevNCDduration,HUINT16 usPrevNCDOnid,HUINT16 usPrevNCDTsid)
{
	UNIXTIME	ulStartTime;
	HxDATETIME_t	stDateTime;


	if( ulPrevNCDStarttime + ulPrevNCDduration < ulSysTime )
	{
		ulStartTime = ulSysTime+30;
	}
	else
	{
		ulStartTime = ulPrevNCDStarttime + ulPrevNCDduration;
	}
	HLIB_DATETIME_ConvertUnixTimeToDateTime( ulStartTime, &stDateTime );
	HxLOG_Print("\t[xsvc_si_NcdSetInSchedule_UkDtt]  time : %d/%d/%d	%d:%d:%d	\n", stDateTime.stDate.usYear,stDateTime.stDate.ucMonth,stDateTime.stDate.ucDay,stDateTime.stTime.ucHour,stDateTime.stTime.ucMinute,stDateTime.stTime.ucSecond);

	svc_si_PostMsgToAp (eSEVT_SI_UPDATE_NCD_SCHEDULE, (Handle_t)HANDLE_NULL, ulStartTime, usPrevNCDOnid, usPrevNCDTsid);
}


/***************************************************************************************
Description	:	NCD Info 의 update 필요한지 확인.

Update 조건 :
	1. 동일한 DemodCellId 중에 ChangeID또는 ChangeVer이 변경된 경우에 업데이트 필요함.
	2. 저장된 NCD Info의 DemodeCellID와 현재 Tuner의 DemodCellId이 없다면 업데티으 필요함.

***************************************************************************************/
STATIC void xsvc_si_NcdDecideUpdate_UkDtt(HUINT16 usDemodCellId, HUINT8 ucChangeId, HUINT8 ucChangeVer, HBOOL *bNeedUpdate)
{
	HBOOL				bUpdate = FALSE, bExistSameCellid = FALSE;
	HUINT32				ulIndex;

	HUINT32				ulNcdCnt, ulNcdCrc;
	XsvcSi_NcdInfo_UkDtt_t *pstNcdInfo= NULL;
	XsvcSi_NcdInfoList_UkDtt_t *pstNcdInfoList= NULL;

	pstNcdInfoList = xsvc_si_NcdGetInfo_UkDtt();

	ulNcdCnt 		= pstNcdInfoList->ulCnt;
	ulNcdCrc 		= pstNcdInfoList->ulCrc32;
	pstNcdInfo 	= pstNcdInfoList->pstNcdInfo;

	HxLOG_Print("ulCntNcdInfo(%d) ulCrc(0x%x) pstNcdInfoList(%p) \n", ulNcdCnt, ulNcdCrc, pstNcdInfo);

	for (ulIndex = 0; ulIndex < ulNcdCnt; ulIndex++)
	{
		// 동일한 DemodCellId가 있는지 확인.
		if(pstNcdInfo[ulIndex].usDemodCellId == usDemodCellId)
		{
			bExistSameCellid = TRUE;

			// 동일한 DemodCellId 중에 ChangeID또는 ChangeVer이 변경된 경우에 업데이트 필요함.
			if(pstNcdInfo[ulIndex].ucChangeId != ucChangeId || pstNcdInfo[ulIndex].ucChangeVer != ucChangeVer)
			{
				bUpdate = TRUE;
				goto exit;
			}
		}
	}

	// 저장된 NCD Info의 DemodeCellID와 현재 Tuner의 DemodCellId이 없다면 업데티으 필요함.
	if(bExistSameCellid == FALSE)
	{
		bUpdate = TRUE;
	}
	else
	{
		bUpdate = FALSE;
	}

exit:
	*bNeedUpdate = bUpdate;
}

STATIC HERROR xsvc_si_NcdGetNcdMsgByMsgId_UkDtt(svcSi_LiveData_t  *pstLive, HUINT32 ulHead, HUINT8 ucMsgId,UNIXTIME ulNewDateTime ,HUINT32 ulASODuration, XsvcSi_NcdInfoMsg_UkDtt_t *pstMsg)
{
	HERROR	hErr;

	HUINT32	ulActionId, ulWeekdayIndex, ulCntMsg = 0;
	HUINT32	ulDuration = 0 , ulDurationHour = 0, ulDurationMin = 0, ulDurationSec = 0;
	HxDATETIME_t	stDateTime;

	HxList_t 		*pstList = NULL;
	HCHAR			*pszTmpString;

	SIxExtensionDes_t		*pstExtensionDes = NULL;
	SIxMessageDes_t 		*pstMessageDes = NULL;
	SIxTable_PsiTable_t		*pstNitTable = NULL, *pstCurrNit = NULL;

	ulActionId = PAL_PIPE_GetActionId(pstLive->hAction);
	if ( ulActionId >= NUM_MAX_ACTION )
	{
		HxLOG_Error("PAL_PIPE_GetActionId Error \n");
		return ERR_FAIL;
	}

	ulDuration = ulASODuration;
	ulDurationHour = ulASODuration/3600;
	ulDurationMin = (ulASODuration%3600)/60;
	ulDurationSec = ulASODuration%60;

	HxSTD_memset(&stDateTime, 0x00, sizeof(HxDATETIME_t));
	HxSTD_memset(pstMsg,0x00,sizeof(XsvcSi_NcdInfoMsg_UkDtt_t));

	hErr = HLIB_DATETIME_ConvertUnixTimeToDateTime (ulNewDateTime, &stDateTime);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("HLIB_DATETIME_ConvertUnixTimeToDateTime Error  !! \n");
		return ERR_FAIL;
	}

	ulWeekdayIndex = HLIB_DATETIME_GetWeekDayIndex(stDateTime.stDate);

	if(ulHead == ACT_HEAD)
	{
		pstNitTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_NitAct, SILIB_TABLE_EXT_ID_ALL);
	}
	else
	{
		pstNitTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_NitOth, SILIB_TABLE_EXT_ID_ALL);
	}

	if (NULL == pstNitTable)
	{
		HxLOG_Error("PAL_SIRAWPOOL_ParseTable Error  !! \n");
		return ERR_FAIL;
	}

	for (pstCurrNit = pstNitTable; pstCurrNit != NULL; pstCurrNit = pstCurrNit->pstNextTable)
	{
		// parse 1st des loop
		for (pstList = pstCurrNit->pstDesLoop; pstList != NULL; pstList = pstList->next)
		{
			HUINT8		*pucRawDes = HLIB_LIST_Data (pstList);

			/*****************************/
			/***	  Extension Descriptor	 ***/
			/*****************************/

			if (*pucRawDes == eSIxDESCTAG_EXTENSION)
			{
				pstExtensionDes = (SIxExtensionDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);

				if(pstExtensionDes!= NULL)
				{
					switch(pstExtensionDes->pucRawData[0])
					{
					case eSIxDESCTAG_EXT_MESSAGE:
						pstMessageDes = (SIxMessageDes_t *)PAL_SIRAWPOOL_ParseDvbExtDes (ulActionId, pstExtensionDes->pucRawData);

						if(pstMessageDes != NULL)
						{
							if (pstMessageDes->ucMessageId == ucMsgId)
							{
								if (ulCntMsg < XsvcSi_UKDTT_MAX_NCD_MESSAGE_NUM)
								{
									HLIB_STD_StrLower(pstMessageDes->szIso639LangCode);
									pszTmpString = NULL;

									snprintf(pstMsg[ulCntMsg].iso639, 4, "%s", pstMessageDes->szIso639LangCode);
									pszTmpString = SILIB_TEXT_MakeUtf8 (eSIxTEXT_ENC_300468_UK, pstMessageDes->szIso639LangCode, pstMessageDes->pszMessageText, HxSTD_StrLen( pstMessageDes->pszMessageText));

									if (pszTmpString != NULL)
									{
										snprintf(pstMsg[ulCntMsg].ucText, XsvcSi_UKDTT_MAX_NCD_MESSAGE_LEN, (HUINT8*)"%02d/%02d/%04d %02d:%02d:%02d for %02d:%02d:%02d \n %s",stDateTime.stDate.ucDay,stDateTime.stDate.ucMonth,stDateTime.stDate.usYear,stDateTime.stTime.ucHour,stDateTime.stTime.ucMinute,stDateTime.stTime.ucSecond,ulDurationHour,ulDurationMin,ulDurationSec,pszTmpString);
										SILIB_TEXT_Free(pszTmpString);

									}
									else
									{
										HUINT8 *pMessage = "No information available";

										HxSTD_StrNCpy(pstMsg[ulCntMsg].ucText, pMessage, XsvcSi_UKDTT_MAX_NCD_MESSAGE_LEN);
									}

									ulCntMsg++;

								}
							}

							PAL_SIRAWPOOL_FreeDes((SIxDescriptor_t*)pstMessageDes);
							pstMessageDes = NULL;
						}
						break;

					default :
						break;

					}

					if(pstExtensionDes != NULL)
					{
						PAL_SIRAWPOOL_FreeDes((SIxDescriptor_t*)pstExtensionDes);
						pstExtensionDes = NULL;
					}
				}
			}
		}
	}

	if(pstNitTable != NULL)
	{
		PAL_SIRAWPOOL_FreeTable(pstNitTable);
		pstNitTable = NULL;
	}

	return ERR_OK;
}

STATIC HERROR xsvc_si_NcdGetNetworkChangeInfo_UkDtt(svcSi_LiveData_t  *pstLive, HUINT32 ulHead, UNIXTIME ulSysTime, HUINT16 usDemodCellId, HBOOL *bFoundASONotify, UNIXTIME *puxStartTime, HUINT32 *pulDuration, SIxNetChangeNotifySubInfo_t *pstNetNotifySubInfo)
{
	HERROR	hErr = ERR_FAIL;
	HBOOL	bValidNotifyDes = FALSE;
	HUINT32	ulActionId = NUM_MAX_ACTION, ulIndex = 0;

	HUINT32	ulLastScanTime = 0;
	HUINT32	ulDuration = 0 , ulDurationHour = 0, ulDurationMin = 0, ulDurationSec = 0;
	HUINT32	ulStartTime = 0, ulNotifyTime = 0xFFFFFFFF;
	HUINT32	ulValidStartTime = 0, ulValidDuration = 0;

	HUINT32	ulNcdCnt = 0 , ulNcdCrc = 0;
	XsvcSi_NcdInfo_UkDtt_t 			*pstNcdInfo = NULL;
	XsvcSi_NcdInfoList_UkDtt_t		*pstNcdInfoList = NULL;

	SIxNetChangeNotifySubInfo_t		stValidChangeNotifyInfo;
	SIxTable_PsiTable_t				*pstNitTable = NULL, *pstCurrNit=NULL;


	*bFoundASONotify = FALSE;
	*puxStartTime = 0;
	*pulDuration = 0;
	HxSTD_memset(&stValidChangeNotifyInfo, 0,sizeof(SIxNetChangeNotifySubInfo_t));

	ulActionId = PAL_PIPE_GetActionId(pstLive->hAction);
	if ( ulActionId >= NUM_MAX_ACTION )
	{
		HxLOG_Error("PAL_PIPE_GetActionId Error \n");
		return ERR_FAIL;
	}

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_ASO_LAST_SCAN_TIME, (HUINT32*)&ulLastScanTime, 0);
	if(hErr != ERR_OK)
	{
		HxLOG_Warning("eDB_PARAM_ITEM_ASO_LAST_SCAN_TIME Error \n");
		ulLastScanTime = 0;
	}

	if(ulHead == ACT_HEAD)
	{
		pstNitTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_NitAct, SILIB_TABLE_EXT_ID_ALL);
	}
	else
	{
		pstNitTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_NitOth, SILIB_TABLE_EXT_ID_ALL);
	}

	if (NULL == pstNitTable)
	{
		HxLOG_Error("PAL_SIRAWPOOL_ParseTable Error  !! \n");
		return ERR_FAIL;
	}

	pstNcdInfoList = xsvc_si_NcdGetInfo_UkDtt();

	ulNcdCnt 		= pstNcdInfoList->ulCnt;
	ulNcdCrc 		= pstNcdInfoList->ulCrc32;
	pstNcdInfo 	= pstNcdInfoList->pstNcdInfo;

	HxLOG_Print("ulNcdCnt: %d, ulNcdCrc :0x%x, pstNcdInfo : %p \n", ulNcdCnt, ulNcdCrc, pstNcdInfo );

	for (pstCurrNit = pstNitTable; pstCurrNit != NULL; pstCurrNit = pstCurrNit->pstNextTable)
	{
		HxList_t	*pstList = NULL;

		// parse 1st des loop
		for (pstList = pstCurrNit->pstDesLoop; pstList != NULL; pstList = pstList->next)
		{
			HUINT8				*pucRawDes = HLIB_LIST_Data (pstList);

			/*****************************/
			/***	  Extension Descriptor	 ***/
			/*****************************/

			if (*pucRawDes == eSIxDESCTAG_EXTENSION)
			{
				SIxExtensionDes_t		*pstExtensionDes = NULL;

				pstExtensionDes = (SIxExtensionDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);

				if(pstExtensionDes!= NULL)
				{
					switch(pstExtensionDes->pucRawData[0])
					{
					case eSIxDESCTAG_EXT_NETWORK_CHANGE_NOTIFY:
						{
							SIxNetChangeNotifyDes_t	*pstNetChangeNotifyDes = (SIxNetChangeNotifyDes_t *)PAL_SIRAWPOOL_ParseDvbExtDes (ulActionId, pstExtensionDes->pucRawData);
							SIxNetChangeNotifyCellInfo_t	*pstNetChangeNotifyCellInfo = NULL;
							SIxNetChangeNotifySubInfo_t *pstNetChangeNotifySubInfo = NULL;

							if(pstNetChangeNotifyDes != NULL)
							{
								pstNetChangeNotifyCellInfo = pstNetChangeNotifyDes->pstInfo;

								while(pstNetChangeNotifyCellInfo)
								{
									if ((pstNetChangeNotifyCellInfo->usCellId == usDemodCellId) || (pstNetChangeNotifyCellInfo->usCellId == 0))
									{
										pstNetChangeNotifySubInfo = pstNetChangeNotifyCellInfo->pstInfo;

										while(pstNetChangeNotifySubInfo)
										{
											if((pstNetChangeNotifySubInfo->ucChangeType != 0) && (pstNetChangeNotifySubInfo->ucReceiverCategory < 0x02) /* reserved */)
											{
												ulDurationHour = ((pstNetChangeNotifySubInfo->ulBcdChangeDur & 0xF00000) >> 20) * 10 + ((pstNetChangeNotifySubInfo->ulBcdChangeDur & 0x0F0000) >> 16);
												ulDurationMin = ((pstNetChangeNotifySubInfo->ulBcdChangeDur & 0xF000) >> 12) * 10 + ((pstNetChangeNotifySubInfo->ulBcdChangeDur & 0x0F00) >> 8);
												ulDurationSec = ((pstNetChangeNotifySubInfo->ulBcdChangeDur & 0xF0) >> 4) * 10 + ((pstNetChangeNotifySubInfo->ulBcdChangeDur & 0x0F));
												ulDuration = (ulDurationHour * 60 * 60) + (ulDurationMin * 60) + ulDurationSec;


												hErr = HLIB_DATETIME_ConvertMJDBcdTimeToUnixTime(pstNetChangeNotifySubInfo->ucStartTimeOfChange, &ulStartTime);

												if ((ulStartTime < ulNotifyTime) && ((ulStartTime+ulDuration) > ulLastScanTime))
												{
											//		bValidNotifyDes = FALSE;
													bValidNotifyDes = TRUE;

													if(ulNcdCnt == 0)
													{
														HxLOG_Print("Vaild -> Saved NCD List is none  (ulNcdCnt : %d)\n", ulNcdCnt);
														bValidNotifyDes = TRUE;
													}

													/* Version Check */
													for(ulIndex=0;ulIndex<ulNcdCnt;ulIndex++)
													{
														if (pstNcdInfo[ulIndex].usDemodCellId == usDemodCellId)
														{
															if(pstNcdInfo[ulIndex].ucChangeId == pstNetChangeNotifySubInfo->ucNetChangeId)
															{
																if(pstNcdInfo[ulIndex].ucChangeVer <= pstNetChangeNotifySubInfo->ucNetChangeVer)
																{
																	HxLOG_Print("Vaild : Saved NCD List is older than new NETWORK_CHANGE_NOTIFY (Version  : %d -> %d)\n",
																		pstNcdInfo[ulIndex].ucChangeVer, pstNetChangeNotifySubInfo->ucNetChangeVer);

															//		bValidNotifyDes = TRUE;
																}
																else if(pstNcdInfo[ulIndex].ucChangeVer == 0xff)
																{
																	if(pstNetChangeNotifySubInfo->ucNetChangeVer != 0x00)
																	{
																		HxLOG_Print("Invalid : Exist in Saved NCD List , But, NETWORK_CHANGE_NOTIFY  is newer than saved information(Version  : %d -> %d)\n",
																			pstNcdInfo[ulIndex].ucChangeVer, pstNetChangeNotifySubInfo->ucNetChangeVer);

																		bValidNotifyDes = FALSE;
																	}
																	else
																	{
																		HxLOG_Print("Valid : Saved ChangeVer is 0xff , New ChangeVer is other value(0x%x) , except 0x00 & 0xff \n",
																			 pstNetChangeNotifySubInfo->ucNetChangeVer);

																//		bValidNotifyDes = TRUE;
																	}
																}
																else
																{
																	HxLOG_Print("Invalid : Saved NCD List is newer than new NETWORK_CHANGE_NOTIFY (Version  : 0x%x -> 0x%x)\n",
																		pstNcdInfo[ulIndex].ucChangeVer, pstNetChangeNotifySubInfo->ucNetChangeVer);

																	bValidNotifyDes = FALSE;
																}
															}
															else
															{
																HxLOG_Print("Vaild : New Change ID  (Change ID : 0x%x)\n",
																	pstNetChangeNotifySubInfo->ucNetChangeId);

															//	bValidNotifyDes = TRUE;
															}
														}
														else
														{
															HxLOG_Print("Vaild : New DemodSellId (usDemodCellId : 0x%x)\n", usDemodCellId);

														//	bValidNotifyDes = TRUE;
														}
													}

													if(bValidNotifyDes == TRUE)
													{
														HxSTD_memset(&stValidChangeNotifyInfo, 0,sizeof(SIxNetChangeNotifySubInfo_t));
														HxSTD_memcpy(&stValidChangeNotifyInfo, pstNetChangeNotifySubInfo, sizeof(SIxNetChangeNotifySubInfo_t));
														ulValidStartTime = ulStartTime;
														ulValidDuration= ulDuration;
														ulNotifyTime = ulStartTime;
													}
												}
											}
											pstNetChangeNotifySubInfo = pstNetChangeNotifySubInfo->pstNext;
										}
									}
									pstNetChangeNotifyCellInfo = pstNetChangeNotifyCellInfo->pstNext;
								}
							}

							if(pstNetChangeNotifyDes !=NULL)
							{
								PAL_SIRAWPOOL_FreeDes((SIxDescriptor_t*)pstNetChangeNotifyDes);
								pstNetChangeNotifyDes = NULL;
							}
						}
						break;
					default:
						break;

					}

					PAL_SIRAWPOOL_FreeDes((SIxDescriptor_t*) pstExtensionDes);
					pstExtensionDes = NULL;
				}
			}
		}
	}

	HxSTD_memset(pstNetNotifySubInfo, 0, sizeof(SIxNetChangeNotifySubInfo_t) );

	if(bValidNotifyDes == FALSE)
	{
	 // nc07 stream에서 0xff -> 0xfe 가 되는 경우에 처리를 위해서 아래와 같이 처리 필요함.
		HxLOG_Print("bValidNotifyDes is FALSE \n");
		HxSTD_memcpy(pstNetNotifySubInfo, &stValidChangeNotifyInfo, sizeof(SIxNetChangeNotifySubInfo_t));
		*puxStartTime = ulValidStartTime;
		*pulDuration = ulValidDuration;
		*bFoundASONotify = FALSE;
	}
	else
	{
		HxLOG_Print("bValidNotifyDes is TRUE!! \n");
		HxSTD_memcpy(pstNetNotifySubInfo, &stValidChangeNotifyInfo, sizeof(SIxNetChangeNotifySubInfo_t));
		*puxStartTime = ulValidStartTime;
		*pulDuration = ulValidDuration;
		*bFoundASONotify = TRUE;
	}

	if(pstNitTable != NULL)
	{
		PAL_SIRAWPOOL_FreeTable(pstNitTable);
	}

	return ERR_OK;
}

STATIC HERROR xsvc_si_NcdReplaceInfo_UkDtt(XsvcSi_NcdInfo_UkDtt_t *pData, HUINT32 ulCnt, HUINT16 usDemodCellId, HBOOL bNeedUpdate, HBOOL bFoundASONotify, XsvcSi_NcdInfo_UkDtt_t *stNewNcdInfo)
{
	HERROR		hErr = ERR_FAIL;

	XsvcSi_NcdInfoList_UkDtt_t *pstNcdInfoList= NULL;

	HUINT32				ulLoadNcdIndex = 0, ulLoadNcdCnt = 0, ulLoadNcdCrc = 0;
	XsvcSi_NcdInfo_UkDtt_t *pstLoadNcdInfo= NULL;

	HUINT32 		 		ulNewNcdCnt = 0, ulNewNcdCrc = 0;
	XsvcSi_NcdInfo_UkDtt_t	*pstNewNcdInfo= NULL;

	HUINT32 				ulInputNcdIndex = 0,  ulInputNcdCnt = 0;
	XsvcSi_NcdInfo_UkDtt_t	*pstInputNcdInfo = NULL;

	HBOOL		bInvalid = FALSE;

	pstNewNcdInfo = (XsvcSi_NcdInfo_UkDtt_t *) OxSI_Malloc(sizeof(XsvcSi_NcdInfo_UkDtt_t) * XsvcSi_UKDTT_MAX_NCD_INFO_NUM);
	if (pstNewNcdInfo == NULL)
	{
		goto ERR_RTN;
	}

	HxSTD_memset(pstNewNcdInfo, 0, sizeof(XsvcSi_NcdInfo_UkDtt_t) * XsvcSi_UKDTT_MAX_NCD_INFO_NUM);

	ulInputNcdCnt = ulCnt;
	pstInputNcdInfo = pData;

	pstNcdInfoList = xsvc_si_NcdGetInfo_UkDtt();

	ulLoadNcdCnt 		= pstNcdInfoList->ulCnt;
	ulLoadNcdCrc 		= pstNcdInfoList->ulCrc32;
	pstLoadNcdInfo 	= pstNcdInfoList->pstNcdInfo;

	if(pData != NULL)
	{
		HxLOG_Print("load ncd data from Storage  \n");

		/* init 시에 HDD 의 data를 메모리로 읽어 오는 경우 이고 , ncd DATA 의 변경은 없다  */
		while (ulInputNcdIndex < ulInputNcdCnt)
		{
			HxSTD_memcpy(&pstNewNcdInfo[ulInputNcdIndex], &pstInputNcdInfo[ulInputNcdIndex], sizeof(XsvcSi_NcdInfo_UkDtt_t));

			ulInputNcdIndex++;
			if(ulInputNcdIndex >= ulInputNcdCnt)
			{
				break;
			}
		}

		ulNewNcdCnt = ulCnt;
	}
	else
	{
		/* NCD data 를 받은후의 동작 */
		if(bFoundASONotify == TRUE)
		{
			/* NCD data 가 들어온 경우 - 해당 cellid 의 data를 update 판단 or  new cell 인경우 add */
			bInvalid = FALSE;

			HxLOG_Print("CellId[%d] find NCD data \n", usDemodCellId);

			for(ulLoadNcdIndex=0;ulLoadNcdIndex<ulLoadNcdCnt;ulLoadNcdIndex++)
			{
				if ((pstLoadNcdInfo[ulLoadNcdIndex].usDemodCellId == usDemodCellId)&&(bNeedUpdate == TRUE))
				{
					HxLOG_Print("CellId[%d] same cellid and (bNeedUpdate == TRUE) -> update ! \n", usDemodCellId);
					HxSTD_memcpy(&pstNewNcdInfo[ulLoadNcdIndex], stNewNcdInfo, sizeof(XsvcSi_NcdInfo_UkDtt_t));
					bInvalid = TRUE;
				}
				else if((pstLoadNcdInfo[ulLoadNcdIndex].usDemodCellId == usDemodCellId)&&(bNeedUpdate != TRUE))
				{
					HxLOG_Print("CellId[%d] same cellid and (bNeedUpdate != TRUE) -> not update! \n", usDemodCellId);
					HxSTD_memcpy(&pstNewNcdInfo[ulLoadNcdIndex], &pstLoadNcdInfo[ulLoadNcdIndex], sizeof(XsvcSi_NcdInfo_UkDtt_t));
					bInvalid = TRUE;
				}
				else
				{
					HxLOG_Print("CellId[%d] data keep \n", usDemodCellId);
					HxSTD_memcpy(&pstNewNcdInfo[ulLoadNcdIndex], &pstLoadNcdInfo[ulLoadNcdIndex], sizeof(XsvcSi_NcdInfo_UkDtt_t));
				}
			}
			if((bInvalid == FALSE)&&(bNeedUpdate == TRUE))
			{
				HxLOG_Print("CellId[%d] New cellid add \n", usDemodCellId);
				HxSTD_memcpy(&pstNewNcdInfo[ulLoadNcdCnt], stNewNcdInfo, sizeof(XsvcSi_NcdInfo_UkDtt_t));
				ulNewNcdCnt = ulLoadNcdCnt + 1;
			}
			else
			{
				ulNewNcdCnt = ulLoadNcdCnt;
			}
		}
		else
		{
			int 			idxC;

			/*NCD data 가 없는 경우 - 해당 cellid 의 data를 삭제하자 */

			idxC =0;
			HxLOG_Print("CellId[%d] Not find NCD data - delete  \n", usDemodCellId);

			while (ulLoadNcdIndex < ulLoadNcdCnt)
			{
				if(usDemodCellId != pstLoadNcdInfo[ulLoadNcdIndex].usDemodCellId)
				{
					HxSTD_memcpy(&pstNewNcdInfo[idxC], &pstLoadNcdInfo[ulLoadNcdIndex], sizeof(XsvcSi_NcdInfo_UkDtt_t));
					idxC++;
				}

				ulLoadNcdIndex++;
				if(ulLoadNcdIndex >= ulLoadNcdCnt)
				{
					break;
				}

			}
			ulNewNcdCnt = idxC;
		}
	}

#if defined(CONFIG_DEBUG)
{
	HUINT32 	i, j;

	for (i = 0; i < ulNewNcdCnt; i++)
	{
		if (pstNewNcdInfo[i].ulStartTime != 0)
		{
			HxLOG_Print(" changeId(%d) changeVer(%d) start(%d) end(%d) bMajor(%d) msgId(%d) demodId(0x%X)\r\n",
				 pstNewNcdInfo[i].ucChangeId,pstNewNcdInfo[i].ucChangeVer,
				pstNewNcdInfo[i].ulStartTime,pstNewNcdInfo[i].ulEndTime,pstNewNcdInfo[i].bMajor,pstNewNcdInfo[i].ucMsgId,pstNewNcdInfo[i].usDemodCellId);

			for (j = 0; j < XsvcSi_UKDTT_MAX_NCD_MESSAGE_NUM; j++)
			{
				if (pstNewNcdInfo[i].stMessage[j].iso639[0] != 0)
				{
					HxLOG_Print(" msg = [%s] (%s) \r\n",  pstNewNcdInfo[i].stMessage[j].iso639, pstNewNcdInfo[i].stMessage[j].ucText);
				}
			}
		}
	}
}
#endif

	ulNewNcdCrc = 0;
	if (ulNewNcdCnt > 0)
	{
		ulNewNcdCrc = (HUINT32) HLIB_MATH_GetCrc32((void *) pstNewNcdInfo, (unsigned long) ulNewNcdCnt * sizeof(XsvcSi_NcdInfo_UkDtt_t));
	}

	if(ulLoadNcdCrc != ulNewNcdCrc)
	{
		xsvc_si_NcdInitInfo_UkDtt();

		xsvc_si_NcdSetInfo_UkDtt(ulNewNcdCnt, ulNewNcdCrc, pstNewNcdInfo);

		hErr = ERR_OK;
	}

ERR_RTN:

	if (pstNewNcdInfo != NULL)
	{
		OxSI_Free(pstNewNcdInfo);
	}

	return hErr;
}

STATIC HERROR xsvc_si_NcdLoadInfoListFromHdd_UkDtt(void)
{
	HUINT8		szSaveFileName[256];
	HxFILE_t		hFileHandle;
	HUINT32		ulDataByte, ulSavedCrc32, ulCalCrc32 = 0;
	HUINT32		ulNcdInfoNum = 0;
	XsvcSi_NcdInfo_UkDtt_t	*pstNcdInfoList = NULL;
	HUINT32		ulReadLength;
	HUINT8		aucHeader[XsvcSi_UKDTT_NCD_SAVE_HEADER_LEN];
	HUINT32 		ulSaveMagicCode;

	HxLOG_Print("[%s:%d ] (+)\n",__FUNCTION__,__LINE__);
	HxSTD_snprintf((char *)szSaveFileName, 255, "%s%s", XsvcSi_UKDTT_NCD_INFO_LIST_SAVE_PATH, XsvcSi_UKDTT_NCD_INFO_LIST_SAVE_FILENAME);

	if (!HLIB_FILE_Exist((char *)szSaveFileName))
	{
		// 파일이 없는 경우 load 할필요 없다.
		return ERR_OK;
	}

	hFileHandle = HLIB_FILE_Open(szSaveFileName, "r");
	if (hFileHandle == NULL)
	{
		HxLOG_Error("HLIB_FILE_Open Error  \r\n");
		return ERR_FAIL;
	}

	ulDataByte = XsvcSi_UKDTT_NCD_SAVE_HEADER_LEN;
	ulReadLength = HLIB_FILE_Read(hFileHandle, (void *)aucHeader, 1, ulDataByte);
	if(ulDataByte != ulReadLength)
	{
		HxLOG_Error(" HLIB_FILE_Read Error (dataByte : %d, ulReadLength : %d)\n",  ulDataByte, ulReadLength);
		goto EXIT_RET;
	}

	ulSaveMagicCode	= HxMACRO_Get32Bit(aucHeader + 0);
	ulDataByte		= HxMACRO_Get32Bit(aucHeader + 4);
	ulSavedCrc32		= HxMACRO_Get32Bit(aucHeader + 8);

	if (ulSaveMagicCode != XsvcSi_UKDTT_NCD_SAVE_MAGIC_CODE)
	{
		HxLOG_Error("Invalid MagicCode(0x%08X)\n",  ulSaveMagicCode);
		goto EXIT_RET;
	}

	if (ulDataByte > 0)
	{
		if ((ulDataByte % sizeof(XsvcSi_NcdInfo_UkDtt_t)) != 0)
		{
			HxLOG_Error("Some Size error (%d, %d) \r\n",  ulDataByte, sizeof(XsvcSi_NcdInfo_UkDtt_t));
			goto EXIT_RET;
		}

		ulNcdInfoNum = ulDataByte / sizeof(XsvcSi_NcdInfo_UkDtt_t);
		if (ulNcdInfoNum > XsvcSi_UKDTT_MAX_NCD_INFO_NUM)
		{
			HxLOG_Error("ulNcdInfoNum is invalide (ulNcdInfoNum:  %d, XsvcSi_UKDTT_MAX_NCD_INFO_NUM : %d) \r\n",  ulNcdInfoNum, XsvcSi_UKDTT_MAX_NCD_INFO_NUM);
			goto EXIT_RET;
		}

		pstNcdInfoList = OxSI_Malloc(ulNcdInfoNum * sizeof(XsvcSi_NcdInfo_UkDtt_t));
		if (pstNcdInfoList == NULL)
		{
			HxLOG_Error("pstNcdInfoList memalloc error \r\n");
			goto EXIT_RET;
		}

		ulReadLength = HLIB_FILE_Read(hFileHandle, (HUINT8 *) pstNcdInfoList, 1, ulDataByte);
		if ( ulDataByte != ulReadLength )
		{
			HxLOG_Error("HLIB_FILE_Read Error (ulDataByte : %d, ulReadLength : %d)\r\n",  ulDataByte, ulReadLength);
			goto EXIT_RET;
		}

		/* CRC 체크,  */
		ulCalCrc32 = HLIB_MATH_GetCrc32((void *) pstNcdInfoList, (unsigned long) ulDataByte);
		if (ulCalCrc32 != ulSavedCrc32)
		{
			HxLOG_Error("CRC Error (0x%08X, 0x%08X)\n",  ulSavedCrc32, ulCalCrc32);
			goto EXIT_RET;
		}

		xsvc_si_NcdReplaceInfo_UkDtt(pstNcdInfoList, ulNcdInfoNum, 0,FALSE, FALSE,NULL);

	}

EXIT_RET :
	if (pstNcdInfoList != NULL)
	{
		OxSI_Free(pstNcdInfoList);
	}
	HxLOG_Print("[%s:%d ] (-)\n",__FUNCTION__,__LINE__);

	HLIB_FILE_Close(hFileHandle);

	return ERR_OK;
}

STATIC HERROR xsvc_si_NcdSaveInfoListToHdd_UkDtt(char *path, HBOOL bPathMake, char *filename, HBOOL bExitIfExisted)
{
	HERROR	hErr, hRes = ERR_FAIL;
	HxFILE_t 	ulFileHandle;
	HCHAR	myBuf[256];
	HUINT8	aucHeader[XsvcSi_UKDTT_NCD_SAVE_HEADER_LEN];
	HUINT32 	ulWriteLength;
	HUINT32 	ulNcdInfoNum = s_stNcdInfoList.ulCnt;
	HUINT32	ulDataByte = 0, ulCrc = 0;

	XsvcSi_NcdInfo_UkDtt_t	*pstNcdInfoList = s_stNcdInfoList.pstNcdInfo;

	if (HLIB_DIR_IsExist((const HCHAR *) XsvcSi_UKDTT_NCD_INFO_LIST_SAVE_PATH) == FALSE)
	{
		// TODO: bPathMake 를 왜 넣었을까....
		if (bPathMake == TRUE)
		{
			hErr = HLIB_DIR_Make((const HCHAR *) XsvcSi_UKDTT_NCD_INFO_LIST_SAVE_PATH);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("Can not make folder.\n");
				return ERR_FAIL;
			}
		}
		else
		{
			return ERR_FAIL;
		}
	}

	HxSTD_snprintf(myBuf, 255, "%s%s", path, filename);
	if (HLIB_FILE_Exist(myBuf) == TRUE)
	{
		// TODO: bExitIfExisted 를 왜 넣었을까....
		if (bExitIfExisted == TRUE)
		{

			// TODO: chmod change 를 왜 넣었을까....
			HxSTD_snprintf((char *)myBuf, 256, "chmod 666 %s%s", path, filename);
			VK_SYSTEM_Command(myBuf);
			return ERR_OK;
		}

		HLIB_FILE_Delete((const HCHAR*)myBuf);
	}

	ulFileHandle = HLIB_FILE_Open(myBuf, "w");
	if (ulFileHandle == NULL)
	{
		HxLOG_Error("HLIB_FILE_Open Error (%s)\n", myBuf);
		return ERR_FAIL;
	}

	aucHeader[0] = (HUINT8)((XsvcSi_UKDTT_NCD_SAVE_MAGIC_CODE >> 24) & 0xFF);
	aucHeader[1] = (HUINT8)((XsvcSi_UKDTT_NCD_SAVE_MAGIC_CODE >> 16) & 0xFF);
	aucHeader[2] = (HUINT8)((XsvcSi_UKDTT_NCD_SAVE_MAGIC_CODE >> 8) & 0xFF);
	aucHeader[3] = (HUINT8)((XsvcSi_UKDTT_NCD_SAVE_MAGIC_CODE >> 0) & 0xFF);


	ulDataByte = ulNcdInfoNum * sizeof(XsvcSi_NcdInfo_UkDtt_t);
	aucHeader[4] = (HUINT8)((ulDataByte >> 24) & 0xFF);
	aucHeader[5] = (HUINT8)((ulDataByte >> 16) & 0xFF);
	aucHeader[6] = (HUINT8)((ulDataByte >> 8) & 0xFF);
	aucHeader[7] = (HUINT8)((ulDataByte >> 0) & 0xFF);

	ulCrc = HLIB_MATH_GetCrc32((void *) pstNcdInfoList, ulDataByte);
	aucHeader[8] = (HUINT8)((ulCrc >> 24) & 0xFF);
	aucHeader[9] = (HUINT8)((ulCrc >> 16) & 0xFF);
	aucHeader[10] = (HUINT8)((ulCrc >> 8) & 0xFF);
	aucHeader[11] = (HUINT8)((ulCrc >> 0) & 0xFF);

	ulDataByte = XsvcSi_UKDTT_NCD_SAVE_HEADER_LEN;
	ulWriteLength = HLIB_FILE_Write(ulFileHandle, (void *)aucHeader, 1, ulDataByte);
	if (ulWriteLength == 0)
	{
		HxLOG_Error("HLIB_FILE_Write Error (ulWriteLength is Zero) \n");
		goto EXIT_RET;
	}

	ulDataByte = ulNcdInfoNum * sizeof(XsvcSi_NcdInfo_UkDtt_t);
	if (ulDataByte > 0)
	{
		ulWriteLength = HLIB_FILE_Write(ulFileHandle, (HUINT8 *) pstNcdInfoList, 1, ulDataByte);
		if (ulWriteLength == 0)
		{
			HxLOG_Error("HLIB_FILE_Write Error (ulWriteLength is Zero) \n");
			goto EXIT_RET;
		}
	}

	hRes = ERR_OK;

EXIT_RET :
	HLIB_FILE_Close(ulFileHandle);
	if (bExitIfExisted == TRUE)
	{
		HxSTD_snprintf((char *)myBuf, 256, "chmod 666 %s%s", path, filename);
		VK_SYSTEM_Command(myBuf);
	}

	return hRes;
}

STATIC HERROR xsvc_si_NcdLoadData_UkDtt(void)
{
	static HBOOL	bInit = FALSE;

	if (bInit == FALSE)
	{
		bInit = TRUE;

		xsvc_si_NcdInitInfo_UkDtt();

		xsvc_si_NcdLoadInfoListFromHdd_UkDtt();
	}

	return ERR_OK;
}

#define ___API_FUNCTIONS___

/***************************************************************************************
Description	: 	(1) 기존 구현이랑 처리 해야 할 내용이 너무 틀려서 별도의 파일과 함수를 만듬.
				(2) Freeview 또는 HDR-2000T 요구사항을 처리하는 코드
Module		:
Remarks		:
***********************************************************************************************/
HERROR xsvc_si_NcdProcessFromNit_UkDtt(svcSi_LiveData_t  *pstLive, SvcSi_Arg_SecCb_t *pstSecCb, HUINT32	ulHead)
{
	HERROR			 hErr;

	HBOOL		bNeedUpdate = FALSE, bFindNcd = FALSE, bFoundASONotify = FALSE;
	HUINT8		*pucMenuLang = NULL;
	HUINT16 		usDemodCellId = 0;
	HUINT32 		ulASODuration = 0;
	HUINT32 		i, ulActionId, ulTunerId = 0;
	UNIXTIME		ulSysTime = 0, ulASOStartTime = 0, ulLastScanTime = 0, ulUnixTime =0;
	HxDATETIME_t	stDateTime;

	SIxNetChangeNotifySubInfo_t		stNetNotifySubInfo;
	XsvcSi_NcdInfoData_UkDtt_t		stPrevNcdInfo ;
	XsvcSi_NcdInfo_UkDtt_t 			stNcdInfo;
	HUINT32 						ulIndexFindNcd = 0;
	HxLANG_Id_e					eLangId = eLangID_MAX;


	/* 시간 체크 1 */
	hErr = VK_CLOCK_GetTime ((unsigned long *)&ulSysTime);
	if ((hErr != ERR_OK )||(ulSysTime==0))
	{
		HxLOG_Error(" VK_CLOCK_GetTime Error \n");
		return ERR_FAIL;
	}

	/* 시간 체크 2 */
	HxSTD_memset(&stDateTime, 0, sizeof(HxDATETIME_t));
	stDateTime.stDate.usYear = 1972;
	HLIB_DATETIME_ConvertDateTimeToUnixTime(&stDateTime,&ulUnixTime);
	if(ulUnixTime > ulSysTime)
	{
		HxLOG_Error(" VK_CLOCK_GetTime Error \n");
		return ERR_FAIL;
	}

	/* DB 로드 */
	HxSTD_memset(&stPrevNcdInfo, 0, sizeof(XsvcSi_NcdInfo_UkDtt_t));
	HxSTD_memset(&stNcdInfo, 0, sizeof(XsvcSi_NcdInfo_UkDtt_t));

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_ASO_NETWORK_CHANGE_INFO, (HUINT32 *)&stPrevNcdInfo, sizeof(XsvcSi_NcdInfo_UkDtt_t));
	if(hErr != ERR_OK)
	{
		HxLOG_Warning("DB_PARAM_GetItem :: eDB_PARAM_ITEM_ASO_NETWORK_CHANGE_INFO Error  \n");
		stPrevNcdInfo.bValid = FALSE;
	}

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_ASO_LAST_SCAN_TIME, (HUINT32*)&ulLastScanTime, 0);
	if(hErr != ERR_OK)
	{
		HxLOG_Warning("DB_PARAM_GetItem :: eDB_PARAM_ITEM_ASO_LAST_SCAN_TIME  Error  \n");
		ulLastScanTime = 0;
	}

	if(ulLastScanTime == 0)
	{
		HxLOG_Print(": LastScan time in not valid. so LastScan time is changed to System time. 	 \n\n");
		hErr = DB_PARAM_SetItem(eDB_PARAM_ITEM_ASO_LAST_SCAN_TIME, ulSysTime, 0);
		if(hErr == ERR_OK)
		{
			ulLastScanTime = ulSysTime;
		}
	}

	HxLOG_Print("ulLastScanTime (0x%x) stPrevNcdInfo.ulStartUnixtime(0x%x) \n", ulLastScanTime, stPrevNcdInfo.ulStartUnixtime);

	if(stPrevNcdInfo.ulStartUnixtime != 0)
	{
		//Ncd reservation 관련해서  setting 된 값이 없으면, hdd 에 ncd data 도 없다고 판단한다 .
		xsvc_si_NcdLoadData_UkDtt();
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Print(" Current System time ======\n");
	HLIB_DATETIME_PrintDateTime(ulSysTime);

	HxLOG_Print(" Saved NCD Info ================================================ \n");
	HLIB_DATETIME_ConvertUnixTimeToDateTime( stPrevNcdInfo.ulStartUnixtime + stPrevNcdInfo.ulDuration, &stDateTime );
	HxLOG_Print("\tvalid(%d) change type(%d) changeid(%d) change ver(%d) cellid(%d)  \n",stPrevNcdInfo.bValid,stPrevNcdInfo.ucChangeType,stPrevNcdInfo.ucNetChangeId,stPrevNcdInfo.ucNetChangeVer,stPrevNcdInfo.usDemodCellId);
	HxLOG_Print("\tInvariantTsPresent(%d) InvariantOnId(%d) InvariantTsId(%d) \n",stPrevNcdInfo.ucInvariantTsPresent,stPrevNcdInfo.usInvariantOnId,stPrevNcdInfo.usInvariantTsId);
	HxLOG_Print("\tRSV  time : %d/%d/%d	%d:%d:%d	\n", stDateTime.stDate.usYear,stDateTime.stDate.ucMonth,stDateTime.stDate.ucDay,stDateTime.stTime.ucHour,stDateTime.stTime.ucMinute,stDateTime.stTime.ucSecond);
	HxLOG_Print("=================================================================== \n");

	HxLOG_Print(" Last Scan Time ======\n");
	HLIB_DATETIME_PrintDateTime(ulLastScanTime);
#endif

	ulActionId = PAL_PIPE_GetActionId(pstLive->hAction);
	if( ulActionId >  NUM_MAX_ACTION)
	{
		HxLOG_Error("PAL_PIPE_GetActionId Error \n");
		return ERR_FAIL;
	}

	if (PAL_PIPE_GetResourceId(ulActionId, eRxRSCTYPE_TUNER_GROUP, &ulTunerId) == ERR_OK)
	{
		PAL_CH_TER_GetTunerCellId(ulTunerId, &usDemodCellId);
	}

	HxSTD_memset(&stNetNotifySubInfo, 0, sizeof(SIxNetChangeNotifySubInfo_t));

	(void) xsvc_si_NcdGetNetworkChangeInfo_UkDtt(pstLive, ulHead, ulSysTime, usDemodCellId, &bFoundASONotify, &ulASOStartTime, &ulASODuration, &stNetNotifySubInfo);

	HxLOG_Print("ulTunerId(%d)  usDemodCellId(%d) bFoundASONotify(%s) !! \n", ulTunerId, usDemodCellId, bFoundASONotify == TRUE?"TRUE":"FALSE");

	if(bFoundASONotify == TRUE)
	{
		HUINT32 		ulDuration=0;
		HUINT32 		ulDurationHour=0;
		HUINT32 		ulDurationMin=0;
		HUINT32 		ulDurationSec=0;
		HUINT32			ulASONotifyFlag = 0;

		hErr = HLIB_DATETIME_ConvertMJDBcdTimeToUnixTime(stNetNotifySubInfo.ucStartTimeOfChange, &ulUnixTime);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("HLIB_DATETIME_ConvertMJDBcdTimeToUnixTime Error \n");
			goto RET_EXIT;
		}

		/* 사용하기 편한 data 형으로 변경 */
		ulDurationHour = ((stNetNotifySubInfo.ulBcdChangeDur & 0xF00000) >> 20) * 10 + ((stNetNotifySubInfo.ulBcdChangeDur & 0x0F0000) >> 16);
		ulDurationMin = ((stNetNotifySubInfo.ulBcdChangeDur & 0xF000) >> 12) * 10 + ((stNetNotifySubInfo.ulBcdChangeDur & 0x0F00) >> 8);
		ulDurationSec = ((stNetNotifySubInfo.ulBcdChangeDur & 0xF0) >> 4) * 10 + ((stNetNotifySubInfo.ulBcdChangeDur & 0x0F));
		ulDuration = (ulDurationHour * 60 * 60) + (ulDurationMin * 60) + ulDurationSec;

		stNcdInfo.ucChangeId = stNetNotifySubInfo.ucNetChangeId;
		stNcdInfo.ucChangeVer = stNetNotifySubInfo.ucNetChangeVer;
		stNcdInfo.ulStartTime = ulUnixTime;
		stNcdInfo.ulEndTime = ulUnixTime + (ulDurationHour * 3600) + (ulDurationMin * 60) + ulDurationSec;
		stNcdInfo.bMajor = (stNetNotifySubInfo.ucChangeType & 0x08) ? 1 : 0;
		stNcdInfo.ucMsgId = stNetNotifySubInfo.ucMessageId;
		stNcdInfo.usDemodCellId = usDemodCellId;
		stNcdInfo.ucChangeType = stNetNotifySubInfo.ucChangeType;

		stNcdInfo.ucInvariantTsPresent = stNetNotifySubInfo.ucInvariantTsPresent;
		stNcdInfo.usInvariantTsId = stNetNotifySubInfo.usInvariantTsId;
		stNcdInfo.usInvariantOnId = stNetNotifySubInfo.usInvariantOnId;

		xsvc_si_NcdGetNcdMsgByMsgId_UkDtt(pstLive, ulHead, stNetNotifySubInfo.ucMessageId, ulUnixTime, ulASODuration, &stNcdInfo.stMessage[0]);

		if((ulASOStartTime+ulASODuration) < ulSysTime)
		{
			HxLOG_Print(" -> Notify Now !	condition ( ASOStartTime+ASODuration) < ulSysTime )\n");


			// Send message to create scan now dialog box
			HxLOG_Print(" -> remove Ncd Notify info & reservation !! \n");
			
			/* clear NCD schedule (starttime = 0xFFFF) */
			svc_si_PostMsgToAp (eSEVT_SI_UPDATE_NCD_SCHEDULE, (Handle_t)HANDLE_NULL, 0xFFFF, 0, 0);

			/*compare with current menu language. "eng", "wel"("cym"), "gla", "gle"*/
			hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_MAIN, (HUINT32 *)&eLangId, 0);
			if (hErr == ERR_OK)
			{
				pucMenuLang = (HCHAR*)HLIB_LANG_IdTo639(eLangId);
				if (hErr == ERR_OK)
				{
					HLIB_STD_StrLower(pucMenuLang);
				}
			}

			for(i=0;i<XsvcSi_UKDTT_MAX_NCD_MESSAGE_NUM;i++)
			{

				if (HxSTD_StrNCmp (stNcdInfo.stMessage[i].iso639, pucMenuLang,3) == 0)
				{

					DB_PARAM_SetItem(eDB_PARAM_ITEM_ASO_NETWORK_MSG_INFO, (HUINT32)stNcdInfo.stMessage[i].ucText, (HUINT32)DxNCD_MAX_MESSAGE_LEN);
					break;
				}
			}


			hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_ASO_NOTIFY_FLAG, (HUINT32 *)&ulASONotifyFlag,  0);
			if(ulASONotifyFlag)
			svc_si_PostMsgToAp (eSEVT_SI_NETWORK_CHANGED_NOTIFY, (HUINT32)pstLive->hAction, ulUnixTime, ulASODuration, TRUE /* Scan Now */);

			goto RET_EXIT;
		}

		HxLOG_Print(" -> Notify reservation !\n");

		/*	Data save or update 판단 결정  */
		xsvc_si_NcdDecideUpdate_UkDtt(usDemodCellId,stNcdInfo.ucChangeId,stNcdInfo.ucChangeVer, &bNeedUpdate);

		HxLOG_Print(" bNeedUpdate == %s\n", bNeedUpdate == TRUE?"TRUE":"FALSE");

		if(bNeedUpdate == TRUE)
		{
			/* 1. HDD 에 NCD 정보 저장  */
			hErr = xsvc_si_NcdReplaceInfo_UkDtt(NULL, 0, usDemodCellId,bNeedUpdate, bFoundASONotify, &stNcdInfo);
			if (hErr == ERR_OK)
			{
				xsvc_si_NcdSaveInfoListToHdd_UkDtt(XsvcSi_UKDTT_NCD_INFO_LIST_SAVE_PATH, TRUE, XsvcSi_UKDTT_NCD_INFO_LIST_SAVE_FILENAME, FALSE);
			}

			/* 2. NCD 예약 정보 다시 설정 */
//			xsvc_si_NcdGetInfo_UkDtt(&ulCntNcdInfo, &ulCrc, pstNcdInfoList);

			xsvc_si_NcdFindFirstInfo_UkDtt(ulLastScanTime, &ulIndexFindNcd, &bFindNcd);

			HxLOG_Print("bFindNcd (%s)\n", bFindNcd == TRUE?"TRUE":"FALSE");

			if(bFindNcd == TRUE)
			{
				HUINT32 					ulNcdCnt, ulNcdCrc;
				XsvcSi_NcdInfo_UkDtt_t 		*pstNcdInfo= NULL;
				XsvcSi_NcdInfoList_UkDtt_t 	*pstNcdInfoList= NULL;

				pstNcdInfoList = xsvc_si_NcdGetInfo_UkDtt();

				ulNcdCnt		= pstNcdInfoList->ulCnt;
				ulNcdCrc		= pstNcdInfoList->ulCrc32;
				pstNcdInfo	= pstNcdInfoList->pstNcdInfo;

				/*Reservation data setting */
				HxSTD_memset(&stPrevNcdInfo, 0, sizeof(XsvcSi_NcdInfoData_UkDtt_t));
				xsvc_si_NcdSetInDB_UkDtt(pstNcdInfo,ulIndexFindNcd,&stPrevNcdInfo);

				/* Reservation setting */
				xsvc_si_NcdSetInSchedule_UkDtt(ulSysTime,stPrevNcdInfo.ulStartUnixtime,stPrevNcdInfo.ulDuration,stPrevNcdInfo.usInvariantOnId,stPrevNcdInfo.usInvariantTsId);
			}
		}
		else
		{
			HxLOG_Print(" NCD NeedUpdate == FALSE\n");
			if(( ulLastScanTime < stPrevNcdInfo.ulStartUnixtime + stPrevNcdInfo.ulDuration )&&(stPrevNcdInfo.bValid == TRUE))
			{
				HxLOG_Print(" RE - Reservation \n");

				/* Reservation setting */
				xsvc_si_NcdSetInSchedule_UkDtt(ulSysTime,stPrevNcdInfo.ulStartUnixtime,stPrevNcdInfo.ulDuration,stPrevNcdInfo.usInvariantOnId,stPrevNcdInfo.usInvariantTsId);
			}
		}
	}
	else
	{
		HxLOG_Print("stPrevNcdInfo.bValid [%d] bNeedUpdate:%d \n", stPrevNcdInfo.bValid,bNeedUpdate);
		if(stPrevNcdInfo.bValid == TRUE)
		{
			HxLOG_Print(" stPrevNcdInfo.usDemodCellId (%d)  usDemodCellId(%d) ulASOStartTime(%d) \n", stPrevNcdInfo.usDemodCellId,usDemodCellId,ulASOStartTime);

			/*HDD 에 NCD 정보 저장  */
			//해당 cell id  값 삭제. 값이 있으면 유지 or 업데이트 .
			if(ulASOStartTime == 0) // ulASOStartTime 값이 0이면 해당 cell id  값 삭제. 값이 있으면 유지 or 업데이트 .
			{
				hErr = xsvc_si_NcdReplaceInfo_UkDtt(NULL, 0, usDemodCellId, bNeedUpdate, bFoundASONotify, &stNcdInfo);
				if (hErr == ERR_OK)
				{
					xsvc_si_NcdSaveInfoListToHdd_UkDtt(XsvcSi_UKDTT_NCD_INFO_LIST_SAVE_PATH, TRUE, XsvcSi_UKDTT_NCD_INFO_LIST_SAVE_FILENAME, FALSE);
				}
			}

			//xsvc_si_NcdGetInfo_UkDtt(&ulCntNcdInfo, &ulCrc, pstNcdInfoList);

			xsvc_si_NcdFindFirstInfo_UkDtt(ulLastScanTime, &ulIndexFindNcd, &bFindNcd);

			HxLOG_Print("bFindNcd (%s)\n", bFindNcd == TRUE?"TRUE":"FALSE");

			if(bFindNcd == TRUE)
			{
				HUINT32 					ulNcdCnt, ulNcdCrc;
				XsvcSi_NcdInfo_UkDtt_t 		*pstNcdInfo= NULL;
				XsvcSi_NcdInfoList_UkDtt_t 	*pstNcdInfoList= NULL;

				pstNcdInfoList = xsvc_si_NcdGetInfo_UkDtt();

				ulNcdCnt		= pstNcdInfoList->ulCnt;
				ulNcdCrc		= pstNcdInfoList->ulCrc32;
				pstNcdInfo	= pstNcdInfoList->pstNcdInfo;

				/*Reservation data setting */
				HxSTD_memset(&stPrevNcdInfo, 0, sizeof(XsvcSi_NcdInfoData_UkDtt_t));

				xsvc_si_NcdSetInDB_UkDtt(pstNcdInfo,ulIndexFindNcd,&stPrevNcdInfo);

				xsvc_si_NcdSetInSchedule_UkDtt(ulSysTime,stPrevNcdInfo.ulStartUnixtime,stPrevNcdInfo.ulDuration,stPrevNcdInfo.usInvariantOnId,stPrevNcdInfo.usInvariantTsId);
			}
			else
			{
				XsvcSi_NcdInfoData_UkDtt_t	stPrevNcdInfo;
				HxSTD_memset(&stPrevNcdInfo, 0, sizeof(XsvcSi_NcdInfo_UkDtt_t));

				DB_PARAM_SetItem(eDB_PARAM_ITEM_ASO_NOTIFY_FLAG, (HUINT32)FALSE, 0);
				DB_PARAM_SetItem(eDB_PARAM_ITEM_ASO_NETWORK_CHANGE_INFO, (HUINT32)&stPrevNcdInfo, sizeof(XsvcSi_NcdInfoData_UkDtt_t));

				HxLOG_Print(" -> remove Ncd Notify info & reservation !! \n");

				/* clear NCD schedule (starttime = 0xFFFF) */
				svc_si_PostMsgToAp (eSEVT_SI_UPDATE_NCD_SCHEDULE, (Handle_t)HANDLE_NULL, 0xFFFF, 0, 0);

				if(stPrevNcdInfo.ulStartUnixtime + stPrevNcdInfo.ulDuration > ulLastScanTime + 30)
				{
					HxLOG_Print(" ->	last NCD reservation  !! \n");

					/* Reservation setting *//*모든 NCD 데이타가 지워졌을경우, 아직 시도하지 않고(last scan time+ 30초)  대기중이던 ncd 동작은 발생하도록 한다.*/
					xsvc_si_NcdSetInSchedule_UkDtt(ulSysTime,stPrevNcdInfo.ulStartUnixtime,stPrevNcdInfo.ulDuration,stPrevNcdInfo.usInvariantOnId,stPrevNcdInfo.usInvariantTsId);
				}
			}
		}
	}

RET_EXIT:

	HxLOG_Print(": NCD CHECK - END  (-) \n");

	return ERR_OK;
}

/***************************************************************************************
Description	:	1. 언제 지워야 하는지는 좀 애매하다.
					- Factory reset 시에만 지워야 하고.
					- Search 시에는 지워야 하는지 좀 애매한데, 현재는 factory reset 에만 지운다.
					- 현재는 지우는 것이 아니라. Size 0의 파일을 생성.
Module		:
Remarks		:
***********************************************************************************************/
HERROR xsvc_si_NcdRemoveAll_UkDtt(void)
{
	HUINT8		szSaveFileName[256]={0,};
	HERROR		hErr=ERR_FAIL;

	HxLOG_Print(": Delete all ncd information. Previous cnt=(%d) \r\n",  s_stNcdInfoList.ulCnt);

	xsvc_si_NcdInitInfo_UkDtt();

	HxSTD_snprintf((char *)szSaveFileName, 255, "%s%s", XsvcSi_UKDTT_NCD_INFO_LIST_SAVE_PATH, XsvcSi_UKDTT_NCD_INFO_LIST_SAVE_FILENAME);
	if (HLIB_FILE_Exist(szSaveFileName))
	{
		hErr = HLIB_FILE_Delete(szSaveFileName);
		if (hErr == ERR_OK)
		{
			HxLOG_Error("[%s][%d]: Delete ncd FILE.\r\n");
		}
	}

	return ERR_OK;
}

HERROR xsvc_si_NctLoadSchedule(void)
{
	HERROR				hErr=ERR_FAIL;
	UNIXTIME				ulSysTime = 0;
	HxDATETIME_t 			stDateTime;
	XsvcSi_NcdInfoData_UkDtt_t	stPrevNcdInfo;
	UNIXTIME				ulLastScanTime = 0;
	HINT32					nDataSize = 0;
	UNIXTIME				unixTime = 0;

	HxLOG_Print(": RE-MAKE reservation !!! \r\n");

	/* 시간 체크 1 */
	hErr = VK_CLOCK_GetTime ((unsigned long *)&ulSysTime);
	if ((hErr != ERR_OK )||(ulSysTime==0))
	{
		HxLOG_Error(":ERR \n");
		return ERR_FAIL;
	}

	/* 시간 체크 2 */
	HxSTD_memset(&stDateTime, 0, sizeof(HxDATETIME_t));
	stDateTime.stDate.usYear = 1972;
	HLIB_DATETIME_ConvertDateTimeToUnixTime(&stDateTime,&unixTime);
	if(unixTime > ulSysTime)
	{
		HxLOG_Error(":ERR \n");
		return ERR_FAIL;
	}

	/* DB 로드 */
	HxSTD_memset(&stPrevNcdInfo, 0, sizeof(XsvcSi_NcdInfoData_UkDtt_t));

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_ASO_NETWORK_CHANGE_INFO, (HUINT32 *)&stPrevNcdInfo,  sizeof(XsvcSi_NcdInfoData_UkDtt_t));
	if(hErr != ERR_OK || nDataSize != sizeof(XsvcSi_NcdInfoData_UkDtt_t))
	{
		stPrevNcdInfo.bValid = FALSE;
	}

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_ASO_LAST_SCAN_TIME, (HUINT32 *)&ulLastScanTime,  0);
	if(hErr != ERR_OK)
	{
		ulLastScanTime = 0;
	}

	if( stPrevNcdInfo.bValid != FALSE )
	{
		if(ulLastScanTime < stPrevNcdInfo.ulStartUnixtime + stPrevNcdInfo.ulDuration)
		{
			// 동작하지 않은 NCD reservation 정보가 있다면, 다시 예약 걸어준다.
			/* Reservation setting */
			xsvc_si_NcdSetInSchedule_UkDtt(ulSysTime,stPrevNcdInfo.ulStartUnixtime,stPrevNcdInfo.ulDuration,stPrevNcdInfo.usInvariantOnId,stPrevNcdInfo.usInvariantTsId);
		}
	}
	return ERR_OK;
}
HERROR			xsvc_si_SetNcdNotifyData_UkDtt(HUINT32 ulASONotifyFlag)
{
	HERROR		hErr = ERR_FAIL;
	UNIXTIME	stCurTime = 0;
//	HUINT32		ulASONotifyFlag = 0;


	hErr = VK_CLOCK_GetTime((unsigned long *)&stCurTime);
	if(hErr == ERR_OK)
	{
		hErr = DB_PARAM_SetItem(eDB_PARAM_ITEM_ASO_LAST_SCAN_TIME, stCurTime, 0);

	}

	hErr = DB_PARAM_SetItem(eDB_PARAM_ITEM_ASO_NOTIFY_FLAG,(HUINT32)ulASONotifyFlag,0);


	return ERR_OK;

}

HERROR			xsvc_si_ClearNcdNotifyData_UkDtt(void)
{
	HERROR		hErr = ERR_FAIL;
	UNIXTIME	stCurTime = 0;
	HUINT32		ulASONotifyFlag = 0;

	hErr = VK_CLOCK_GetTime((unsigned long *)&stCurTime);
	if(hErr == ERR_OK)
	{
		hErr = DB_PARAM_SetItem(eDB_PARAM_ITEM_ASO_LAST_SCAN_TIME, stCurTime, 0);

	}
	hErr = DB_PARAM_SetItem(eDB_PARAM_ITEM_ASO_NOTIFY_FLAG,(HUINT32)ulASONotifyFlag,0);

	return ERR_OK;
}



/* end of file */

