
#if defined(CONFIG_MW_CI_PLUS)
/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <bus.h>
#include <isosvc.h>
#include <db_svc.h>
#include <db_param.h>

#include <svc_pipe.h>
#include <svc_fs.h>
#include <svc_pb.h>
#include <svc_cas.h>
#include <svc_av.h>
#include <svc_sbtl.h>
#include <svc_ttx.h>
#include <svc_sys.h>
#include <get_enum_str.h>
#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_output.h>
#include <mgr_pg.h>
#include <svc_resources.h>
#include <mgr_rscmap.h>
#include <mgr_action.h>
#include <mgr_media.h>
#include <mgr_svclist.h>
#include <mgr_live.h>
#include <mgr_playback.h>
#include <_xmgr_playback.h>
#include "../local_include/_xmgr_playback_base.h"

#include <mgr_cas.h>
#include <xmgr_cas.h>
#include <svc_output.h>

#include <ci_ctrl.h>

STATIC HERROR xmgr_playback_CiPlusStopTsrPlayback_Base (HUINT32 ulActionId)
{
	BUS_SendMessage (NULL, eMEVT_PVR_PLAYBACK_STOP, SVC_PIPE_GetActionHandle(ulActionId), 0, 0, 0);
	return ERR_OK;
}

STATIC HERROR xmgr_playback_CiPlusStopChasePlayback_Base (HUINT32 ulActionId)
{
	BUS_SendMessage (NULL, eMEVT_PVR_PLAYBACK_STOP, SVC_PIPE_GetActionHandle(ulActionId), 0, 0, 0);
	return ERR_OK;
}

STATIC HERROR xmgr_playback_CiPlusGetProperTimeStamp_Base (SvcMeta_TimestampList_t *pstTimeStampList, HUINT32 ulSourceTime, HUINT32 ulStartIdx, HUINT32 *pulFoundIdx)
{
	HINT32							nIndex, nNumber;
	SvcMeta_TimestampList_t 		*pstList = NULL;

	pstList = pstTimeStampList;
	nNumber = (HINT32)pstList->ulNumber;

	for (nIndex = (HINT32)ulStartIdx; nIndex < nNumber; nIndex++)
	{
		if (pstList->pstData[nIndex].stTimeStamp.ulTime == ulSourceTime)
		{
			*pulFoundIdx = (HUINT32)nIndex;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

#if defined(CONFIG_MW_CI_PLUS_V_1_3)
STATIC HERROR xmgr_playback_CiPlusCheckCiPlusDrmTimeStamp( xmgrPbContext_t *pstContext, HUINT32 ulSourceTime, HUINT32 ulEndTime )
{
	DxCopyrightControl_t		*pstMetaDrm =	NULL;
	SvcMeta_TimestampData_t 	*pstTimeStamp	= NULL;
	SvcMeta_TimestampList_t 	*pstList		= NULL;
	HUINT32	i;
	HUINT32	ulTime, ulUriTime, ulPcTime, ulLicenseTime;
	HINT32	nUriIdx, nPcIdx, nLicenseIdx;

	HxDATETIME_t				stDateTime;
	HULONG						ulSysTime = 0;
	HUINT32 					ulPcSourceTime = ulSourceTime;

	ulUriTime = ulPcTime = ulLicenseTime = 0;
	nUriIdx = nPcIdx = nLicenseIdx = -1;

	if( pstContext == NULL )
		return ERR_FAIL;

	if( pstContext->eTrickState == eMgrPb_TickState_PAUSE )
		return ERR_FAIL;

	pstList 	= &(pstContext->stTimeStampList);

	for(i = 0; i < pstList->ulNumber; i++)
	{
		pstTimeStamp = &(pstList->pstData[i]);

		HxLOG_Info("stTimeStamp.eType = 0x%x\n", pstTimeStamp->stTimeStamp.eType);

		if (pstTimeStamp->stTimeStamp.eType == eSvcMeta_Timestamp_DRM || pstList->pstData[i].stTimeStamp.eType == eSvcMeta_Timestamp_DRM)
		{
			ulTime = pstTimeStamp->stTimeStamp.ulTime;
			pstMetaDrm = (DxCopyrightControl_t *)pstTimeStamp->pData;

			if (pstMetaDrm->stCasCcInfo.eCasCcType != eDxCopyrightControl_CasCcType_CiPlus)
			{
				continue;
			}

#if defined(CONFIG_DEBUG)
			HxLOG_Info("\t eCiPlusDrmType:[%d], ulTime:[%d], ulSourceTime:[%d]...\n", pstMetaDrm->stCasCcInfo.info.stCiPlusDrmInfo.eCiPlusDrmType, ulTime, ulSourceTime);

			HLIB_DATETIME_ConvertUnixTimeToDateTime(ulTime, &stDateTime);
			HxLOG_Info("\t (      ulTime : %02d.%02d:%02d %02d:%02d:%02d)\n ",
														stDateTime.stDate.usYear,
														stDateTime.stDate.ucMonth,
														stDateTime.stDate.ucDay,
														stDateTime.stTime.ucHour,
														stDateTime.stTime.ucMinute,
														stDateTime.stTime.ucSecond);
#endif

			switch(pstMetaDrm->stCasCcInfo.info.stCiPlusDrmInfo.eCiPlusDrmType)
			{
				case eDxCopyrightControl_CiPlusDrmType_Uri:
					if (ulTime < ulSourceTime)
					{
						if (ulUriTime <= ulTime)
						{
							// 저장된 URI Timestamp 중 Default는 PB시 적용시키지 않아도 될 것임. (이게 문제가 된다면 동시간에 있는 default and from CAM URI 중 from CAM URI가 적용되도록 코드 수정 필요)
							// ucMode = 1은 defualt URI를 나타냄.
							// default URI를 처리하지 않는 이유는 기본적으로 모든 CI+ contents는 hjm 파일에 대표 URI 정보가 저장되어 처리되고 있고,
							// 10초 이내에 timeout URI나 from CAM URI가 날라와 적용 될 것이기 때문.
							if(pstMetaDrm->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ucMode == 1)
							{
								continue;
							}

							// Timestamp data 중 URI가 사용되고 있는 부분을 check 해 둠.
							if(nUriIdx > -1)
							{
								pstList->pstData[nUriIdx].stTimeStamp.bIsUsed = FALSE;
							}

							nUriIdx = i;
							ulUriTime = ulTime;
							pstTimeStamp->stTimeStamp.bIsUsed = TRUE;
						}
					}
					break;
				case eDxCopyrightControl_CiPlusDrmType_ParentalCtrl:
					if (pstContext->ePbType == eMgrPb_PlayType_Tsr)
					{
						/*
						*	TSR동작 시 ulTime 은 UNIXTIME으로 들어오고, ulSourceTime 는 offset time으로 들어오기에 비교가 안된다.
						*	tsr offset time 들을 이용하여 현재 커서의 UNIXTIME을 구해 서로 비교하도록 수정함.
						*/
						ulSysTime = 0;
						if(VK_CLOCK_GetTime( &ulSysTime ) != ERR_OK)
						{
							break;
						}

						ulPcSourceTime = ulSysTime -(ulEndTime - ulSourceTime);
					}

					if (ulTime < ulPcSourceTime)
					{
						HxLOG_Info("\t >> ulPcTime:[%d], ulTime:[%d]...\n", ulPcTime, ulTime);

						if (ulPcTime < ulTime)
						{
							// Timestamp data 중 URI가 사용되고 있는 부분을 check 해 둠.
							if(nPcIdx > -1)
							{
								pstList->pstData[nPcIdx].stTimeStamp.bIsUsed = FALSE;
							}

							nPcIdx = i;
							ulPcTime = ulTime;
							pstTimeStamp->stTimeStamp.bIsUsed = TRUE;
						}
					}
					break;
				case eDxCopyrightControl_CiPlusDrmType_License:
					if (ulTime < ulSourceTime)
					{
						if (ulLicenseTime < ulTime)
						{
							nLicenseIdx = i;
							ulLicenseTime = ulTime;
						}
					}
					break;
				default:
					HxLOG_Error("ERROR...\n");
					break;
			}
		}
	}

	if(nUriIdx > -1)
	{
		if(nUriIdx == pstList->ulDrmUriRefIdx)
		{
			// Do nothing...
		}
		else
		{
			// URI info is updated...
			if (pstList->ulDrmUriRefIdx == 0)	// 일단 list의 0번 index는 DRM 관련 timestamp 전에 다른 timestamp가 무조건 저장 되기 때문에 0번은 아니라고 봄.
			{
				HxLOG_Info("\n======= URI Timestamp is updated... =======\n\n");
				pstList->pstData[nUriIdx].stTimeStamp.bIsUsed = TRUE;
				pstList->ulDrmUriRefIdx = nUriIdx;
				// TODO: 아래 EVT 보내는 부분 & 받아서 처리하는 부분 작업 (kkkim)
				//MWC_UTIL_PostMsgToAp (EVT_PVR_DRM_SIGNALED, pstContext->hAct, (HUINT32)pstList->pstData[nUriIdx].pData, nUriIdx, ulUriTime);
			}
			else
			{
				HxLOG_Info("\n======= URI Timestamp is updated... =======\n\n");
				pstList->pstData[pstList->ulDrmUriRefIdx].stTimeStamp.bIsUsed = FALSE;
				pstList->pstData[nUriIdx].stTimeStamp.bIsUsed = TRUE;
				pstList->ulDrmUriRefIdx = nUriIdx;
				// TODO: 아래 EVT 보내는 부분 & 받아서 처리하는 부분 작업 (kkkim)
				//MWC_UTIL_PostMsgToAp (EVT_PVR_DRM_SIGNALED, pstContext->hAct, (HUINT32)pstList->pstData[nUriIdx].pData, nUriIdx, ulUriTime);
			}
		}
	}

	if(nPcIdx > -1)
	{
		if(nPcIdx == pstList->ulDrmPcRefIdx)
		{
			// Do nothing...
			HxLOG_Info("\n======= Do nothing... =======\n\n");
		}
		else
		{
			// Parental Control info is updated...
			if (pstList->ulDrmPcRefIdx == 0)	// 일단 list의 0번 index는 DRM 관련 timestamp 전에 다른 timestamp가 무조건 저장 되기 때문에 0번은 아니라고 봄.
			{
				HxLOG_Info("\n======= Parental Control Timestamp is updated...(pstList->ulDrmPcRefIdx==0) =======\n\n");
				pstList->pstData[nPcIdx].stTimeStamp.bIsUsed = TRUE;
				pstList->ulDrmPcRefIdx = nPcIdx;
				// TODO: 아래 EVT 보내는 부분 & 받아서 처리하는 부분 작업 (kkkim)
				//MWC_UTIL_PostMsgToAp (EVT_PVR_DRM_SIGNALED, pstContext->hAct, (HUINT32)pstList->pstData[nPcIdx].pData, nPcIdx, ulPcTime);
			}
			else
			{
				HxLOG_Info("\n======= Parental Control Timestamp is updated... =======\n\n");
				pstList->pstData[pstList->ulDrmPcRefIdx].stTimeStamp.bIsUsed = FALSE;
				pstList->pstData[nPcIdx].stTimeStamp.bIsUsed = TRUE;
				pstList->ulDrmPcRefIdx = nPcIdx;
				// TODO: 아래 EVT 보내는 부분 & 받아서 처리하는 부분 작업 (kkkim)
				//MWC_UTIL_PostMsgToAp (EVT_PVR_DRM_SIGNALED, pstContext->hAct, (HUINT32)pstList->pstData[nPcIdx].pData, nPcIdx, ulPcTime);
			}
		}
	}

	if(nLicenseIdx > -1)
	{
		if(nLicenseIdx == pstList->ulDrmLicenseRefIdx)
		{
			// Do nothing...
		}
		else
		{
			if(pstList->pstData[nLicenseIdx].stTimeStamp.bIsUsed == TRUE)
			{
				// Do nothing...
			}
			else
			{
				// License info is updated...
				HxLOG_Info("\n======= LICENSE Timestamp is updated... =======\n\n");
				pstList->pstData[nLicenseIdx].stTimeStamp.bIsUsed = TRUE;
				pstList->ulDrmLicenseRefIdx = nLicenseIdx;
				// TODO: 아래 EVT 보내는 부분 & 받아서 처리하는 부분 작업 (kkkim)
				//MWC_UTIL_PostMsgToAp (EVT_PVR_DRM_SIGNALED, pstContext->hAct, (HUINT32)pstList->pstData[nLicenseIdx].pData, nLicenseIdx, ulLicenseTime);
			}
		}
	}
	return ERR_OK;
}
#endif

STATIC HERROR xmgr_playback_CiPlusCheckTimeStamp_Base (xmgrPbContext_t *pstContext, HUINT32 ulSourceTime, HUINT32 ulEndTime)
{
	HERROR hErr = ERR_FAIL;
	HUINT32 ulIndex, ulFoundIdx;
	SvcMeta_TimestampData_t *pstTimeStamp	= NULL;
	SvcMeta_TimestampList_t *pstList		= NULL;

	pstList 	=	&(pstContext->stTimeStampList);
	ulIndex 	=	0;
	ulFoundIdx	=	0;

	HxLOG_Trace();

#if defined(CONFIG_MW_CI_PLUS_V_1_3)
	if(xmgr_playback_CiPlusCheckCiPlusDrmTimeStamp(pstContext, ulSourceTime, ulEndTime) != ERR_OK)
		return ERR_FAIL;
#endif

	while (ulIndex < pstList->ulNumber)
	{
		ulFoundIdx = pstList->ulNumber;

		hErr = xmgr_playback_CiPlusGetProperTimeStamp_Base (pstList, ulSourceTime, ulIndex, &ulFoundIdx);
		if ((ERR_OK != hErr) || (ulFoundIdx >= pstList->ulNumber))
			break;

		ulIndex = ulFoundIdx + 1;
		pstTimeStamp = &(pstList->pstData[ulFoundIdx]);

		switch (pstTimeStamp->stTimeStamp.eType)
		{
			case eSvcMeta_Timestamp_PMT:
			{
				SvcMeta_Service_t*pstMetaSvc = NULL;
				pstMetaSvc = (SvcMeta_Service_t *)pstTimeStamp->pData;
				if ((NULL != pstMetaSvc) && (pstList->ulPmtRefTime != pstTimeStamp->stTimeStamp.ulTime))
				{
					hErr = SVC_META_SetServiceInfo (pstContext->ulContentId, pstMetaSvc, FALSE);
					if (ERR_OK == hErr)
					{
						// TODO: 아래 EVT 보내는 부분 & 받아서 처리하는 부분 작업 (kkkim)
						// MWC_UTIL_PostMsgToAp (EVT_PVR_PMT_CHANGED, pstPlayContext->hAct, 0, 0, 0);
					}
					else
					{
						HxLOG_Error ("SVC_META_SetServiceInfo err:\n");
					}

					pstList->ulPmtRefTime = pstTimeStamp->stTimeStamp.ulTime;
				}

				break;
			}
			case eSvcMeta_Timestamp_EIT:
			{
				SvcMeta_Event_t *pstMetaEvent	=	NULL;
				pstMetaEvent = (SvcMeta_Event_t *)pstTimeStamp->pData;
				if ((NULL != pstMetaEvent) && (pstList->ulEitRefTime != pstTimeStamp->stTimeStamp.ulTime))
				{
					hErr = SVC_META_SetEventInfo (pstContext->ulContentId, pstMetaEvent, FALSE);
					if (ERR_OK == hErr)
					{
						// TODO: 아래 EVT 보내는 부분 & 받아서 처리하는 부분 작업 (kkkim)
						// MWC_UTIL_PostMsgToAp (EVT_PVR_EIT_CHANGED, pstPlayContext->hAct, 0, 0, 0);
					}
					else
					{
						HxLOG_Error ("SVC_META_SetEventInfo err:\n");
					}

					pstList->ulEitRefTime = pstTimeStamp->stTimeStamp.ulTime;
				}

				break;
			}

			case eSvcMeta_Timestamp_DRM:
			{
				DxCopyrightControl_t	*pstMetaDrm =	NULL;
				pstMetaDrm = (DxCopyrightControl_t *)pstTimeStamp->pData;

				if (NULL != pstMetaDrm)
				{
					HUINT32 		*pulDrmRefTime = NULL;
					HUINT32			ulTime = pstTimeStamp->stTimeStamp.ulTime;

					switch (pstMetaDrm->stCasCcInfo.eCasCcType)
					{
						case eDxCopyrightControl_CasCcType_CiPlus:
							// xmgr_playback_CiPlusCheckCiPlusDrmTimeStamp() 함수에서 이미 CI+ DRM을 처리 다 했기 때문에 아무것도 하지 않도록 함.
							break;

						default:
							if (pstList->ulDrmRefTime != ulTime)
							{
								pulDrmRefTime = &(pstList->ulDrmRefTime);
							}
					}

					if (pulDrmRefTime)
					{
						// TODO: 아래 EVT 날리는 것 구조에 맞게 변경 할 것!
						//MWC_UTIL_PostMsgToAp (EVT_PVR_DRM_SIGNALED, pstContext->hAct, (HUINT32)pstMetaDrm, ulFoundIdx, ulTime);
						*pulDrmRefTime = ulTime;
					}
				}

				break;
			}

			default:
			break;
		}
	}

	return ERR_OK;
}

STATIC HERROR xmgr_playback_CiPlusCheckDrmTimeStamp_Base (xmgrPbContext_t *pstContext, HUINT32 ulSourceTime, HUINT32 ulEndTime)
{
	HERROR	hErr = ERR_FAIL;
	HUINT32	ulIndex, ulFoundIdx;
	SvcMeta_TimestampData_t *pstTimeStamp = NULL;
	SvcMeta_TimestampList_t *pstList = NULL;

	if (NULL == pstContext)
		return ERR_FAIL;

	pstList 	= &(pstContext->stTimeStampList);
	ulIndex 	= 0;

	HxLOG_Trace();

#if defined(CONFIG_MW_CI_PLUS_V_1_3)
	if(xmgr_playback_CiPlusCheckCiPlusDrmTimeStamp(pstContext, ulSourceTime, ulEndTime) != ERR_OK)
		return ERR_FAIL;
#endif

	while (ulIndex < pstList->ulNumber)
	{
		ulFoundIdx = pstList->ulNumber;

		hErr = xmgr_playback_CiPlusGetProperTimeStamp_Base (pstList, ulSourceTime, ulIndex, &ulFoundIdx);
		if ((ERR_OK != hErr) || (ulFoundIdx >= pstList->ulNumber))
			break;

		ulIndex = ulFoundIdx + 1;
		pstTimeStamp = &(pstList->pstData[ulFoundIdx]);

		switch (pstTimeStamp->stTimeStamp.eType)
		{
			case eSvcMeta_Timestamp_DRM:
			{
				DxCopyrightControl_t	*pstMetaDrm	=	NULL;
				pstMetaDrm = (DxCopyrightControl_t *)pstTimeStamp->pData;
				if (NULL != pstMetaDrm)
				{
					HUINT32 		*pulDrmRefTime = NULL;
					HUINT32			ulTime = pstTimeStamp->stTimeStamp.ulTime;

					switch (pstMetaDrm->stCasCcInfo.eCasCcType)
					{
						case eDxCopyrightControl_CasCcType_CiPlus:
							// xmgr_playback_CiPlusCheckCiPlusDrmTimeStamp() 함수에서 이미 CI+ DRM을 처리 다 했기 때문에 아무것도 하지 않도록 함.
							break;

						default:
							if (pstList->ulDrmRefTime != ulTime)
							{
								pulDrmRefTime = &(pstList->ulDrmRefTime);
							}
					}

					if (pulDrmRefTime)
					{
						// TODO: 아래 Send Message로 eSEVT_CAS_DRM_SIGNALLED던지는 것이 정상 동작인지 파악 필요 (HMS PH1 code랑 비교) (kkkim)
						// 이벤트 받아서 처리하는 부분도 동작 확인 필요 함.
						BUS_SendMessage(NULL, eSEVT_CAS_DRM_SIGNALLED, pstContext->hAction, ulFoundIdx, ulTime, (HINT32)pstMetaDrm);
						*pulDrmRefTime = ulTime;
					}
				}

				break;
			}

		default:
			break;
		}
	}

	return ERR_OK;
}


#if defined(CONFIG_MW_CI_PLUS_CC_V_2)	// return이 ERR_OK이면, CAM을 찾았음. ERR_FAIL이면 녹화시 사용한 CAM이 삽입되어 있지 않음.
STATIC HERROR xmgr_playback_CiPlusGetSlotIdOfBindedCam_Base(HUINT8 *pucBindedCamId, HUINT16 *pusSlotId)
{
	HUINT32						unCamSlotNum;
	SvcCas_CtrlParam_t			stParam;
	CI_GetBindedCamIdInParam_t	stInParam;
	CI_GetBindedCamIdOutParam_t	stOutParam;
	HUINT8						aucBindedCamId[SVC_META_CAMID_LEN];
	HUINT32						i = 0;
	HERROR						hErr = ERR_FAIL;

	hErr = SVC_CAS_GetMaxNumOfCISlot(&unCamSlotNum);

	for (i=0; i<unCamSlotNum; i++)
	{
		stInParam.usSlotId = i;
		stOutParam.pucBindedCamId = aucBindedCamId;
		stParam.pvIn = (void *)&stInParam;
		stParam.pvOut = (void *)&stOutParam;
		stParam.ulControlType = eCACTRL_CI_GetBindedCamId;
		hErr = SVC_CAS_CtrlGet(eDxCAS_GROUPID_CAM, eCACTRL_CI_GetBindedCamId, &stParam);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("\nMW_CAS_CTRL_Get() ERROR!\n");
			continue;
		}

		if(HxSTD_memcmp(pucBindedCamId, aucBindedCamId, SVC_META_CAMID_LEN) == 0)
		{
			*pusSlotId = i;
			return ERR_OK;
		}
	}

	*pusSlotId = -1;
	return ERR_FAIL;
}
#endif

STATIC HERROR xmgr_playback_CheckDelayedPlaybackLicenseExchangeResponse(xmgrPbContext_t *pstContext)
{
	HUINT32	nOffsetSec = 10; // the step 10 of 5.1.1.2. Delayed Playback License Exchange Response

	if ((pstContext->bCnfPbLicense == FALSE) &&
		!(pstContext->eCiPlusDrmState & eXmgrPbCiPlusDrmState_License))
	{
		if (pstContext->ulPlayTime >= (pstContext->ulDrmLicenseTime + nOffsetSec))
		{
			if (pstContext->ePbState == eXmgrPbState_AV)
			{
				pstContext->eCiPlusDrmState |= eXmgrPbCiPlusDrmState_License;

				HxLOG_Info("A/V should be blanked because of Delayed Playback License [ulPlayTime:%d, ulDrmLicenseTime:%d]\n", pstContext->ulPlayTime, pstContext->ulDrmLicenseTime);
				xmgr_playback_SetAppState_Base(pstContext, eXmgrPbState_CAS_BLOCKED);
			}
		}
	}

	return ERR_OK;
}


STATIC HERROR xmgr_playback_CiPlusOperateDrmFromMeta_Base(xmgrPbContext_t *pstContext, DxCopyrightControl_t *pstDrmInfo, HUINT32 unMetaIndex, HUINT32 ulTime)
{
	switch(pstDrmInfo->stCasCcInfo.info.stCiPlusDrmInfo.eCiPlusDrmType)
	{
		case eDxCopyrightControl_CiPlusDrmType_Uri:
		{
			pstContext->bDrmValid = TRUE;
			HxSTD_MemCopy(&pstContext->stDrmInfo, pstDrmInfo, sizeof(DxCopyrightControl_t));
			// Copy Protection
			pstContext->ulDrmUriTime = ulTime;
			xmgr_playback_SetCopyProtection(pstContext);
			break;
		}
		#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
		case eDxCopyrightControl_CiPlusDrmType_ParentalCtrl:			// CI+1.3 스펙에 따라 PC가 걸린 부분은 PIN이 들어올 때 까지 A/V를 막는다. (PIN 을 입력 받기 전까지 화면이 보이지 않는데, 이를 위하여 실제 이벤트 발생 수초 전에 PIN 관련 이벤트를 날려 화면 막힘 없이 PB 하여도 스펙을 벗어나는 것은 아니지만, 구현 문제로 일단은 실제 PC event가 발생 하는 구역이 걸리면 그때 CAM쪽으로 이벤트를 날리고 A/V를 막음.
		{
			HUINT16						usSlotId;
			HERROR						hErr;

			hErr = xmgr_playback_CiPlusGetSlotIdOfBindedCam_Base(pstDrmInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusParentalCtrlInfo.aucBindedCamId, &usSlotId);
			if(hErr == ERR_OK)
			{
				SvcCas_CtrlParam_t					stParam;
				CI_SetParentalCtrlInParam_t			stPcInParam;
				CI_GetPinCapabilityInParam_t		stPinCapaInParam;
				CI_GetPinCapabilityOutParam_t		stPinCapaOutParam;
				SvcCas_CiPlusPinCapaInfo_t 			stPinCapaInfo;
				HERROR								hErr;

				stPinCapaInParam.usSlotId = usSlotId;
				stParam.pvIn = (void *)&stPinCapaInParam;
				stParam.pvOut = (void *)&stPinCapaOutParam;
				stParam.ulControlType = eCACTRL_CI_GetPinCapa;

				HxLOG_Info("Using CAM Slot ID: [%02x]\n", usSlotId);

				hErr = SVC_CAS_CtrlGet(eDxCAS_GROUPID_CAM, eCACTRL_CI_GetPinCapa, &stParam);
				if (hErr == ERR_FAIL)
				{
					HxLOG_Error("ERROR: MW_CAS_CTRL_Get => eCACTRL_CI_GetPinCapa...\n");
					return ERR_FAIL;
				}

				HxSTD_memcpy(&stPinCapaInfo, &stPinCapaOutParam.stPinCapaInfo, sizeof(SvcCas_CiPlusPinCapaInfo_t));

				if(stPinCapaInfo.ucRating < pstDrmInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusParentalCtrlInfo.ucAgeLimit)
				{
					// DRM에 의하여 A/V를 blanking...
					pstContext->eCiPlusDrmState |= eXmgrPbCiPlusDrmState_PC;
					xmgr_playback_SetAppState_Base(pstContext, eXmgrPbState_CAS_BLOCKED);
				}
				else
				{
					// AV는 보여줘도 된다 do nothing...
				}

				stParam.ulControlType 		= eCACTRL_CI_SetParentalCtrl;

				stPcInParam.usSlotId 		= usSlotId;
				stPcInParam.hPbAction 		= pstContext->hAction;
				stPcInParam.unMetaIndex		= unMetaIndex;
				stPcInParam.ucAgeLimit 		= pstDrmInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusParentalCtrlInfo.ucAgeLimit;
				stPcInParam.pucPrivateData	= pstDrmInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusParentalCtrlInfo.aucPrivateData;

				stParam.pvIn = (void *)&stPcInParam;
				SVC_CAS_CtrlSet(eDxCAS_GROUPID_CAM, eCACTRL_CI_SetParentalCtrl, &stParam);
			}
			else
			{
				// TODO: 해당 contents를 보기 위해서는 녹화시 사용한 CAM이 삽입 되어 있어야 합니다. Message 뿌려주고 Program Playback 종료 (kkkim)
				HxLOG_Info("There is no CAM in CI slot, which was used when the content was recorded...\n");
				switch (pstContext->ePbType & 0x0000000f)
				{
					case eMgrPb_PlayType_Tsr:
						xmgr_playback_CiPlusStopTsrPlayback_Base (pstContext->ulActionId);
						break;
					case eMgrPb_PlayType_Chase:
						xmgr_playback_CiPlusStopChasePlayback_Base (pstContext->ulActionId);
						break;
					case eMgrPb_PlayType_Normal:
					default :
						if(MGR_MakeThumbnail_IsAvailable() == TRUE)
						{
							BUS_SendMessage(MGR_MakeThumbnail_GetProc(), eMEVT_PVR_MEDIA_NEXT_FILE, 0, 0, 0, 0);
						}
						else
						{
							MGR_PLAYBACK_StopPlayback (pstContext->ulActionId);
						}
						break;
				}
			}
			break;
		}
		case eDxCopyrightControl_CiPlusDrmType_License:
		{
			HUINT16						usSlotId;
			HERROR						hErr;

			hErr = xmgr_playback_CiPlusGetSlotIdOfBindedCam_Base(pstDrmInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.aucBindedCamId, &usSlotId);
			if(hErr == ERR_OK)
			{
				SvcCas_CtrlParam_t			stParam;
				CI_SetLicenseInParam_t		stInParam;

				switch (pstContext->ePbType & 0x0000000f)
				{
					case eMgrPb_PlayType_Normal:
					{
						HULONG		ulMinutesForRL = pstContext->stDrmInfo.stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ulMinutesForRL;
						HULONG		ulSysTime = 0;

						if(VK_CLOCK_GetTime( &ulSysTime ) != ERR_OK)
							return ERR_FAIL;

						// Retention Limit 에 권한 만료가 되면 license를 전송하면 안됨 (5.3.1.2. Application Of The Playback Retention Limit의 step 28 ~ 32)
						if(!(ulMinutesForRL == 0 && pstContext->stDrmInfo.stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ucMode > CI_CC_URI_MODE_DEFAULT))
						{
							HUINT8		PLAY_ENDTIME_GAP = 3;
							if (((ulSysTime - (pstContext->ulOrgStartTime + pstContext->ulEndTime) + PLAY_ENDTIME_GAP) > (ulMinutesForRL * 60)))
							{
								HxLOG_Print("License is expired !!!");
								return ERR_FAIL;
							}
						}

						stParam.ulControlType 		= eCACTRL_CI_SetLicense;
						stInParam.hPbAction			= pstContext->hAction;
						stInParam.unMetaIndex		= unMetaIndex;
						stInParam.usSlotId			= usSlotId;
						stInParam.usProgramNum		= pstDrmInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.usProgramNum;
						stInParam.pucLicenseData	= pstDrmInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.aucLicenseData;
						stInParam.usLicenseLength	= pstDrmInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.usLicenseLength;

						stParam.pvIn = (void *)&stInParam;
						SVC_CAS_CtrlSet(eDxCAS_GROUPID_CAM, eCACTRL_CI_SetLicense, &stParam);

						pstContext->ulDrmLicenseTime = ulTime;
						pstContext->bCnfPbLicense = FALSE;

						break;
					}
					default:
						HxLOG_Info("do not send playback license in this playback mode (%d)\n", pstContext->ePbType);
						break;
				}
			}
			else
			{
				// TODO: 해당 contents를 보기 위해서는 녹화시 사용한 CAM이 삽입 되어 있어야 합니다. Message 뿌려주고 Program Playback 종료 (kkkim)
				switch (pstContext->ePbType & 0x0000000f)
				{
					case eMgrPb_PlayType_Tsr:
						xmgr_playback_CiPlusStopTsrPlayback_Base (pstContext->ulActionId);
						break;
					case eMgrPb_PlayType_Chase:
						xmgr_playback_CiPlusStopChasePlayback_Base (pstContext->ulActionId);
						break;
					case eMgrPb_PlayType_Normal:
					default :
						if(MGR_MakeThumbnail_IsAvailable() == TRUE)
						{
							BUS_SendMessage(MGR_MakeThumbnail_GetProc(), eMEVT_PVR_MEDIA_NEXT_FILE, 0, 0, 0, 0);
						}
						else
						{
							MGR_PLAYBACK_StopPlayback (pstContext->ulActionId);
						}
						break;
				}
			}
			break;
		}
		#endif
		default:
			HxLOG_Error("ERROR! Invalid CI+ DRM type!!\n");
			break;
	}

	return ERR_OK;
}

STATIC HERROR xmgr_playback_CiPlusOperateDrmFromCam_Base(xmgrPbContext_t *pstContext, DxCopyrightControl_t *pstDrmInfo, HUINT32 unMetaIndex)
{
	switch(pstDrmInfo->stCasCcInfo.info.stCiPlusDrmInfo.eCiPlusDrmType)
	{
		case eDxCopyrightControl_CiPlusDrmType_Uri:
		{
			#if 1
			// 이미 Recording 당시에 CAM에서 받은 URI가 있는 경우에는 Playback시 CAM에서 부터 전달받은 URI 정보를 무시하도록 한다.
			// 이유는 playback 시 이 함수에 진입 한 것은 scrambled 구간이 있다는 뜻인데, 서로 동작이 다른 CAM을 사용하여 RL을조작 할 수 있기 때문이다.
			// (예를 들면 1)SKY CI+CAM은 RL이 90분이 걸리고, ORF CI+CAM은 Unlimit로 볼 수 있다고 가정. 2)녹화한 service의 CA System ID가 두 CAM의 CA system ID를 모두 포함할 수 있음.
			// scramble 구간이 약간 있는 SKY content를 나중에 playback 할 때 ORF CAM이 삽입되어 있다고 가정하자.
			// 1), 2) 조건이 만족하는 상황이면 Playback을 할 때 ts가 ORF CI+CAM에 2번 조건에 의해 들어가게 되고, URI가 CAM으로부터 Box로 새로 던져지게 된다.
			// 그런데, 저장된 Content는 SKY라 90분 limit이 걸려야 하지만 새로 받은 URI로 덮어 씌어져 pb가 가능하게 된다. 이러한 오동작은 막아야 함.

			if (pstContext->stDrmInfo.stCasCcInfo.eCasCcType == eDxCopyrightControl_CasCcType_CiPlus)
			{
				if (pstContext->stDrmInfo.stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ucMode == CI_CC_URI_MODE_FROMCAM)
				{
					HxLOG_Info("The old URI info is from the CAM, so the new URI info is ignored...\n");
					break;
				}
			}

			#else
			// 녹화된 content가 scramble 영역을 포함하고 있는 상태라면, Meta에도 URI 정보가 있고, CAM으로부터도 URI 정보를 받는다.
			// 만약 Meta 자체에 CAM으로부터 받은 URI가 제대로 저장 되어있는데,
			// partially scrambled REC file을 descramble 하는 과정에서 전송되는 default URI나, timeout URI가
			// (Meta에 CAM에서 정상적으로 받은 URI정보가 저장되어 있다면) Playback 시 URI 동작에 영향을 미치면 안된다.
			if( (pstContext->stDrmInfo.stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ucMode == CI_CC_URI_MODE_FROMCAM)
				&& (pstDrmInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ucMode < CI_CC_URI_MODE_FROMCAM))
			{
				HxLOG_Info("The old URI info is from the CAM, but the new URI info is not from the CAM. So, it should be ignored...\n");
				break;
			}
			#endif

			// CI+ specification에는 어긋나는 내용이지만, Humax내부적으로 URI timeout 이 발생되는 contents는 legacy CI content라고 본다.
			// 따라서 legacy CI content는 timeout으로 URI meta가 저장되어 있을 것이고,
			// 만약 scrambled 구간이 있어 playback 시 날라오는 Default URI는 무시되어야 한다.
			if( (pstContext->stDrmInfo.stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ucMode == CI_CC_URI_MODE_TIMEOUT)
				&& (pstDrmInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ucMode == CI_CC_URI_MODE_DEFAULT))
			{
				HxLOG_Info("The old URI info is from the Timeout(it means the content is legacy CI content), and new URI info is the default URI. So, it should be ignored...\n");
				break;
			}

			if(unMetaIndex)  // 현재는 Playback License Exchange 일 때만 unMetaIndex 이 설정되고 해당 URI를 update 하도록 함
			{
				HULONG		ulSysTime = 0;
				HUINT32 	ulOffset = 0;
				DxCopyrightControl_t	stDrmInfo = *pstDrmInfo;

				if(VK_CLOCK_GetTime( &ulSysTime ) != ERR_OK)
					return MESSAGE_BREAK;

				// Retention Limit 을 녹화시 시간과 현재 시간의 차이를 더해 줘서 다음 playback에 적용 될 수 있도록 한다.
				// 5.3.1.2. Application Of The Playback Retention Limit 의 step25 참조
				HxLOG_Print("update metadata Index(%d), Retention Limit (%d minutes + offset (%d))", unMetaIndex, stDrmInfo.stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ulMinutesForRL, ((ulSysTime - pstContext->ulPlayTime - pstContext->ulOrgStartTime)/60));

				stDrmInfo.stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ulMinutesForRL += ((ulSysTime - pstContext->ulPlayTime - pstContext->ulOrgStartTime)/60);

				SVC_META_UpdateDrmTimeStampByIndex(pstContext->ulContentId, &stDrmInfo, unMetaIndex);

				//	meta data 에서 로딩한 RL(Retention Limit) 값에 offset("바로 위 식의 ((ulSysTime - pContext->ulPlayTime - pContext->ulOrgStartTime)/60)") 이 존재하면
				// update 할 새로운 RL 값에 offset 만큼 더해서 ap_player 의 pstContext의 RL에 적용되도록 함.	 (CiCc_GetMinutesForRL 참조하여 offset 값을 구함)
				if ((pstContext->stDrmInfo.stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ulMinutesForRL / 1440) >= 1) // 24 hours
				{
					ulOffset = pstContext->stDrmInfo.stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ulMinutesForRL % 1440;
				}
				else if ((pstContext->stDrmInfo.stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ulMinutesForRL / 720) >= 1) // 12 hours
				{
					ulOffset = pstContext->stDrmInfo.stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ulMinutesForRL % 720;
				}
				else if ((pstContext->stDrmInfo.stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ulMinutesForRL / 360) >= 1) // 6 hours
				{
					ulOffset = pstContext->stDrmInfo.stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ulMinutesForRL % 360;
				}
				else if ((pstContext->stDrmInfo.stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ulMinutesForRL / 90) >= 1) // 6 hour
				{
					ulOffset = pstContext->stDrmInfo.stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ulMinutesForRL % 90;
				}

				pstDrmInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ulMinutesForRL += ulOffset;
			}

			HxLOG_Info("The URI info is updated...\n");
			// 위 두가지 이외의 경우에는 URI 정보를 업데이트 한다.
			pstContext->bDrmValid = TRUE;
			HxSTD_MemCopy(&pstContext->stDrmInfo, pstDrmInfo, sizeof(DxCopyrightControl_t));
			// Copy Protection
			xmgr_playback_SetCopyProtection(pstContext);

			break;
		}
#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
		case eDxCopyrightControl_CiPlusDrmType_ParentalCtrl:
			break;

		case eDxCopyrightControl_CiPlusDrmType_License:
		{
			if (pstContext->bCnfPbLicense == FALSE)
			{
				pstContext->bCnfPbLicense = TRUE;
				pstContext->eCiPlusDrmState &= (~eXmgrPbCiPlusDrmState_License);

				if(pstContext->eCiPlusDrmState)
				{
					// License 조건을 통과 하였다 하더라도, 다른 DRM 내용이 걸려 있다면 A/V를 계속해서 막는다.
					HxLOG_Info("Still A/V is blanking... because of CI+ DRM...\n");
				}
				else
				{
					if (pstContext->ePbState == eXmgrPbState_CAS_BLOCKED)
					{
						xmgr_playback_SetAppState_Base(pstContext, eXmgrPbState_AV);
					}
				}
			}

			SVC_META_UpdateDrmTimeStampByIndex(pstContext->ulContentId, pstDrmInfo, unMetaIndex);
			// TODO: 해당 Metadata가update 완료 되었다는 정보를 Meta쪽에 기록 해줘야 함. (kkkim)
			break;
		}
#endif
		default:
			HxLOG_Error("ERROR! Invalid CI+ DRM type!!\n");
			break;
	}

	return ERR_OK;
}


STATIC HERROR xmgr_playback_CiPlusGetDrmInfoByTime_Base (SvcMeta_TimestampList_t *pstTimeStampList, HUINT32 ulSourceTime, DxCopyrightControl_t *pstInfo)
{
	SvcMeta_Timestamp_t *pstTimeStamp = NULL;
	HUINT32 ulTimeStampNum = 0;
	HUINT32 i, ulMaxIndex=0, ulMaxTime = 0, ulCount=0;

	if (NULL == pstInfo)
		return ERR_FAIL;

	ulTimeStampNum = pstTimeStampList->ulNumber;

	for (i=0; i<ulTimeStampNum; i++)
	{
		pstTimeStamp = &pstTimeStampList->pstData[i].stTimeStamp;

		if (eSvcMeta_Timestamp_DRM != pstTimeStamp->eType)
			continue;

		if( pstTimeStamp->ulTime > ulSourceTime )
			continue;

		ulCount++;

		if( ulMaxTime <= pstTimeStamp->ulTime )
		{
			ulMaxIndex = i;
			ulMaxTime =  pstTimeStamp->ulTime;
		}
	}

	if (0 == ulCount)
	{
		return ERR_FAIL;
	}

	HxSTD_memcpy (pstInfo, (DxCopyrightControl_t *)pstTimeStampList->pstData[ulMaxIndex].pData, sizeof(DxCopyrightControl_t));

	return ERR_OK;
}

STATIC HERROR xmgr_playback_CiPlusApplyRetentionLimit_Base(xmgrPbContext_t *pstContext, HBOOL bRecord)
{
	HULONG		ulSysTime = 0;
	HERROR		hErr;
	HUINT8		PLAY_ENDTIME_GAP = 3;
	HULONG		ulMinutesForRL = pstContext->stDrmInfo.stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ulMinutesForRL;

	hErr = VK_CLOCK_GetTime( &ulSysTime );

	// RL값이 0일 경우는 unlimit하게 보여줘도 되는 것이므로 시간 비교를 하지 않아도 됨. A/V 가 RL이 원인으로 막혀있다면 A/V 보여주고 OK return
	// 단 이 값이 유효한 경우는 URI 정보가 CAM으로 부터 직접 받은 것이거나, timeout 걸렸을 때 받은 URI정보에만 유효 함.
	if(ulMinutesForRL == 0 && pstContext->stDrmInfo.stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ucMode > CI_CC_URI_MODE_DEFAULT)
	{
		if (pstContext->ePbState == eXmgrPbState_CAS_BLOCKED)
		{
			pstContext->eCiPlusDrmState &= (~eXmgrPbCiPlusDrmState_RL);
			if(pstContext->eCiPlusDrmState)
			{
				// Retention Limit을 통과 하였다 하더라도, 다른 DRM 내용이 걸려 있다면 A/V를 계속해서 막는다.
				HxLOG_Info("Still A/V is blanking... because of CI+ DRM...\n");
			}
			else
			{
				xmgr_playback_SetAppState_Base(pstContext, eXmgrPbState_AV);
			}
		}

		return ERR_OK;
	}

	/* 녹화 시작 시간으로부터 play 하는 지점의 시간을 더하여 해당 위치의 UNIXTIME 을 구하고, */
	/* 현재 시간에서 play 하는 지점의 시간을 감산하여 해당 지점이 몇분 전에 녹화된 것인지 구한다. */
	/* 결과를 DRM의 RL 시간과 비교하고, play 지점이 RL 시간을 초과 할 경우 에러 메시지 출력 후 RL시간 이내의 컨턴츠 위치로 출력 한다. */
	/* PLAY_ENDTIME_GAP을 더해준 이유는 Live Broadcast 시에 (ulSysTime - (pstRecInfo->ulStartTime + pstContext->ulPlayTime) 값이 마이너스(-1) 값이 나와 */
	/* 생기는 오류를 방지 하기 위하여 더함. */
	if ((ulSysTime - (pstContext->ulOrgStartTime + pstContext->ulPlayTime) + PLAY_ENDTIME_GAP) > (ulMinutesForRL * 60))
	{
		if (pstContext->ePbState != eXmgrPbState_CAS_BLOCKED)		// CI+ 에서는 eXmgrPbState_CAS_BLOCKED을 RL을 위해서만 사용하도록...
		{
			HxLOG_Info("A/V should be blanked because of RL... Time[%d]\n", (ulSysTime - (pstContext->ulOrgStartTime + pstContext->ulPlayTime) + PLAY_ENDTIME_GAP));

			pstContext->eCiPlusDrmState |= eXmgrPbCiPlusDrmState_RL;
			xmgr_playback_SetAppState_Base(pstContext, eXmgrPbState_CAS_BLOCKED);
		}
	}
	else
	{
		if (pstContext->ePbState == eXmgrPbState_CAS_BLOCKED)
		{
			pstContext->eCiPlusDrmState &= (~eXmgrPbCiPlusDrmState_RL);
			if(pstContext->eCiPlusDrmState)
			{
				// Retention Limit을 통과 하였다 하더라도, 다른 DRM 내용이 걸려 있다면 A/V를 계속해서 막는다.
				HxLOG_Info("Still A/V is blanking... because of CI+ DRM...\n");
			}
			else
			{
				xmgr_playback_SetAppState_Base(pstContext, eXmgrPbState_AV);
			}
		}
	}

	return ERR_OK;
}

STATIC BUS_Result_t xmgr_playback_CiPlusMsgEvtPvrDrmSignaled_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if defined(CONFIG_MW_CI_PLUS)

// CI+ DRM 동작에 대해 CAS MGR 버전마다 다르게 동작하도록 구현해 놓음.
// CAS MGR을 나눈 이유는 현재 OCTO2를 사용하는 모델이 HMS, NGS, GEN2 인데, HMS는 ver2를 따라가지만, NGS GEN2는 이전 mgr로 작업을 하고 있기 때문임.
// (2012.11.09. 현재) GEN2는 tagging 따서 나간 상황이고, NGS는 phase 2때 MGR2로 들어갈 예정인데,
// 이때 NGS가 다 넘어오면 CAS MGR 2만 남겨두고 나머지는 다 제거하도록 한다.
	DxCopyrightControl_t			*pstDrmInfo;
	HUINT32				unMetaIndex;
	HUINT32				ulTime;
	pstDrmInfo = (DxCopyrightControl_t *)p1;
	unMetaIndex = p2;
	ulTime = (HUINT32)p3;

	// TODO: 각 CI+ DRM Type에 따라 처리 부분 구현(kkkim)
	if(pstDrmInfo->stCasCcInfo.eCasCcType== eDxCopyrightControl_CasCcType_CiPlus)
	{
		xmgr_playback_CiPlusOperateDrmFromMeta_Base(pstContext, pstDrmInfo, unMetaIndex, ulTime);
	}


#endif //#if defined(CONFIG_MW_CI_PLUS)


	NOT_USED_PARAM(pstContext);
	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return MESSAGE_BREAK;
}

BUS_Result_t xmgr_playback_CiPlusMsgEvtCasDrmSignalled_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if defined(CONFIG_MW_CI_PLUS)
	DxCopyrightControl_t			stDrmInfo;
	HUINT32				unMetaIndex;
	HERROR				hErr = ERR_FAIL;

	HLIB_STD_MemCpySafe(&stDrmInfo, sizeof(DxCopyrightControl_t), (const void *)p3, sizeof(DxCopyrightControl_t));

	unMetaIndex = p2;

	HxLOG_Info("[xmgr_playback_EvtCasDrmSignalled] eDrmType=%x\n", pstContext->stDrmInfo.stCasCcInfo.eCasCcType);

	// TODO: 각 CI+ DRM Type에 따라 처리 부분 구현(kkkim)
	if(stDrmInfo.stCasCcInfo.eCasCcType == eDxCopyrightControl_CasCcType_CiPlus)
	{
		hErr = xmgr_playback_CiPlusOperateDrmFromCam_Base(pstContext, &stDrmInfo, unMetaIndex);
	}

#endif	//#if defined(CONFIG_MW_CI_PLUS)

	NOT_USED_PARAM(pstContext);
	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return MESSAGE_BREAK;
}

BUS_Result_t xmgr_playback_CiPlusMsgEvtRecordTimeStampUpdate_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR					hErr				=	ERR_FAIL;
	HUINT32 				ulPlaybackTime		=	0;
	SvcMeta_TimestampList_t *pstTimeStampList	=	&pstContext->stTimeStampList; /* owner playback of mgr-context*/
	svcPb_State_e			ePbState			=	ePB_STATE_Init;
	HUINT32					ulContentID			=	pstContext->ulContentId;

	/* reload timestamp from meta to mgr-context*/
	if (NULL == pstTimeStampList)
		return ERR_FAIL;

	hErr = SVC_META_UnloadTimeStamp (pstTimeStampList);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_META_UnloadTimeStamp Error. (%x)\n", hErr);
		return hErr;
	}

	hErr = SVC_META_LoadTimeStamp (ulContentID, pstTimeStampList);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_META_LoadTimeStamp Error. (%x)\n", hErr);
		return hErr;
	}

	/* update timestamp to mgr-context */
	hErr = SVC_PB_GetState (hAction, &ePbState);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PB_GetState Error. (%x)\n", hErr);
		return hErr;
	}

	if (ePB_STATE_Trick == ePbState)
	{
		hErr = SVC_PB_GetPlayTime (hAction, &ulPlaybackTime );
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("SVC_PB_GetPlayTime Error. (%x)\n", hErr);
			return hErr;
		}
	}

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return MESSAGE_BREAK;
}

BUS_Result_t xmgr_playback_CiPlusMsgBusCreate_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 				ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT8					ucPlayIdx = xmgr_playback_GetContextIndexFromActionId (ulActionId);
	MgrAction_Type_e			eActionType;
	SvcMeta_Record_t			stRecordInfo;
	MgrRec_RecInfo_t	stInfo;
#if defined(CONFIG_APCORE_MOVING_THUMBNAIL_CACHE)
	HBOOL					bRecord = FALSE;
#endif
	HERROR					hErr;
	HUINT32					ulContentID			=	pstContext->stSetup.ulContentId;
	SvcMeta_TimestampList_t *pstTimeStampList	=	&pstContext->stTimeStampList; /* owner playback of mgr-context*/

	if(pstContext->ePbState != eXmgrPbState_IDLE)
	{
		HxLOG_Critical("Critical Eror!!!\n");
	}
	HxLOG_Info("Player(%d) : eMEVT_BUS_CREATE ulContentId=%d ulStartTime=%d\n", ucPlayIdx, ulContentID, pstContext->stSetup.ulStartTime);


	switch (pstContext->stSetup.ePlayType & 0x0000000f)
	{
		case eMgrPb_PlayType_Tsr:
			eActionType = eViewType_TSR_PB;
			break;
		case eMgrPb_PlayType_Chase:
			eActionType = eViewType_CHASE_PB;
			break;
		case eMgrPb_PlayType_Normal:
		default:
			eActionType = eViewType_PB;
			break;
	}

	HxLOG_Info("Player(%d) : eActionType=%d ePlayType=%d\n", ucPlayIdx, eActionType, pstContext->stSetup.ePlayType);
	// Resouce 는 모두 Ready된 상태임.
	hErr = MGR_ACTION_Take(ulActionId, eActionType, BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()));
	if (hErr != ERR_OK)
	{
		HxLOG_Info("Player(%d) : can't take MGR_ACTION_Take eActionType=%x Handle_t=%x\n", ucPlayIdx, eActionType, pstContext->stSetup.hCurrSvc);
	//	goto FINISH;
	}

	/* init recorder app pstContext */
	pstContext->ulActionId = ulActionId;
	pstContext->hAction = hAction;

	hErr = MGR_RSC_GetResourceId(ulActionId, eRxRSCTYPE_AV_DEC, &pstContext->ulVideoDecId);
	if(ERR_OK!=hErr)		HxLOG_Critical("Critical Eror!!!\n");
	hErr = MGR_RSC_GetResourceId(ulActionId, eRxRSCTYPE_PVR_PLAYER, &pstContext->ulPlayerId);
	if(ERR_OK!=hErr)		HxLOG_Critical("Critical Eror!!!\n");
	hErr = MGR_RSC_GetResourceId(ulActionId, eRxRSCTYPE_DEMUX, &pstContext->ulDemuxId);
	if(ERR_OK!=hErr)		HxLOG_Critical("Critical Eror!!!\n");

	pstContext->ucPlayIdx		= ucPlayIdx;
	pstContext->hSvc			= pstContext->stSetup.hCurrSvc;
	pstContext->hMasterSvc		= pstContext->stSetup.hMasterSvc;
	pstContext->eSvcListGroup	= pstContext->stSetup.eSvcListGroup;
	pstContext->ePbType 		= pstContext->stSetup.ePlayType;
	pstContext->ePbState		= eXmgrPbState_STARTED;
	pstContext->eFirstTrickState = pstContext->stSetup.eTrickState;
	pstContext->ulPlayTime		= 0;
	pstContext->nPlaySpeed		= 100;			// 1배속 Play (100%)
	pstContext->eTrickState 	= eMgrPb_TickState_PLAY;
	pstContext->eTrickSpeed 	= eMgrPb_Speed_X1;
	pstContext->bPmtValid		= FALSE;
	pstContext->nCasErrCode 	= 0;
	pstContext->bCasOk			= FALSE;
	pstContext->ulContentId 	= pstContext->stSetup.ulContentId;
	pstContext->uiState 		= eMgrPb_NONE;
	pstContext->bRadioBackground = FALSE;
	pstContext->bRestartAV		= FALSE;
	pstContext->ulEventId		= (HUINT32)-1;
	pstContext->bAVHide 		= FALSE;
	pstContext->bIsEOF			= FALSE;
	pstContext->bRatingPinPassed = FALSE;
	pstContext->bSCPinPassed	= FALSE;
	pstContext->bRatingPassByLockPass = FALSE;
	pstContext->bSCPinCodeValid = FALSE;
#if defined(CONFIG_MW_CI_PLUS)
	pstContext->bCnfPbLicense = TRUE;
#endif

	HxSTD_MemSet(&pstContext->svcInfo, 0, sizeof(SvcMeta_Service_t));
	HxSTD_MemSet(&stRecordInfo, 0, sizeof(SvcMeta_Record_t));
	HxSTD_MemSet(&stInfo, 0, sizeof(MgrRec_RecInfo_t));

	(void)xmgr_playback_RefreshSvcMeta_Base(pstContext, pstContext->stSetup.ulStartTime);

	hErr = SVC_META_GetRecordInfo (ulContentID, &stRecordInfo);
	hErr = SVC_META_GetServiceInfo(ulContentID, &pstContext->svcInfo);

	pstContext->ulStartTime		= 0;
	pstContext->ulOrgStartTime	= stRecordInfo.ulStartTime;
	pstContext->ulEndTime		= stRecordInfo.ulRecTime;

	// Medialist 에서 레코딩중인 파일을 재생한 경우에 대한 처리
	if (eActionType == eViewType_PB)
	{
		Handle_t hRecAction = HANDLE_NULL;

		hErr = SVC_META_GetRecActionByContentId (pstContext->ulContentId, &hRecAction);
		if ((ERR_OK == hErr) && (HANDLE_NULL != hRecAction))
		{
			SvcRec_RecInfo stRecInfo;
			HxSTD_MemSet (&stRecInfo, 0, sizeof(SvcRec_RecInfo));
			hErr = SVC_REC_GetInfo (hRecAction, &stRecInfo);
			if (ERR_OK == hErr)
			{
				pstContext->ulEndTime = stRecInfo.ulRecTime;
			}
		}
	}


	/* Session version 증가 */
	pstContext->hAction = SVC_PIPE_IncreaseActionVersion(ulActionId);


	if(stRecordInfo.stDrmInfo.stCasCcInfo.eCasCcType== eDxCopyrightControl_CasCcType_CiPlus)
	{
		HxSTD_MemCopy(&pstContext->stDrmInfo, &stRecordInfo.stDrmInfo, sizeof(DxCopyrightControl_t));
	}

	/* setup a player */
	{
		SvcPb_Setup_t stSetup;

		HxSTD_MemSet( &stSetup, 0, sizeof(SvcPb_Setup_t) );

		switch(eActionType)
		{
			case eViewType_PB:					stSetup.ePbActType = ePb_ActType_Playback; break;
			case eViewType_TSR_PB:				stSetup.ePbActType = ePb_ActType_Tsr_Playback; break;
			case eViewType_CHASE_PB:			stSetup.ePbActType = ePb_ActType_Chase_Playback; break;
			default : break;
		}

		stSetup.ulStartTime 	=	pstContext->stSetup.ulStartTime;

		switch (pstContext->stSetup.ePlayType & 0x0f000000)
		{
			case eMgrPb_PlayType_EndPos:
				stSetup.eStartPosType = ePb_StartPosition_End;
				break;
			case eMgrPb_PlayType_FirstPos:
				stSetup.eStartPosType = ePb_StartPosition_Begin;
				break;
			case eMgrPb_PlayType_RecentPos:
			default:
				stSetup.eStartPosType = ePb_StartPosition_LastView;
				break;
		}


		if(stRecordInfo.u1cScrambled == 1)
		{
			stSetup.bDescramblePlay = TRUE;
		}
		else
		{
			stSetup.bDescramblePlay =FALSE;
		}


		stSetup.eSvcType		=	pstContext->svcInfo.eSvcType;
		stSetup.bEncryption 	=	(1 == stRecordInfo.u1cEncryption)?TRUE:FALSE;

		HxSTD_MemCopy( stSetup.aucPath, stRecordInfo.aucPath,MAX_FILE_LEN);
		HxSTD_MemCopy( stSetup.aucFileName, stRecordInfo.aucFileName,MAX_FILE_LEN);

		switch(eActionType)
		{
			case eViewType_TSR_PB:
			case eViewType_CHASE_PB:
				{
					HUINT32	 ulRecActionId	= 0;
					Handle_t hRecAction		= HANDLE_NULL;

					hErr = SVC_META_GetRecActionByContentId (pstContext->ulContentId, &hRecAction);
					if ((ERR_OK != hErr) || (HANDLE_NULL == hRecAction))
					{
						HxLOG_Error ("SVC_META_GetRecActionByContentId. Error\n");
						break;
					}

					ulRecActionId = SVC_PIPE_GetActionId (hRecAction);
					hErr = SVC_PIPE_GetResourceId (ulRecActionId, eRxRSCTYPE_HDD_REC, &stSetup.ulRecordId);
					if (ERR_OK != hErr)
					{
						HxLOG_Error ("SVC_PIPE_GetResourceId. Error\n");
						break;
					}
				}
				break;
			default :
				break;
		}

		stSetup.bMakeThumbnail = FALSE;

		pstContext->ePbState = eXmgrPbState_WAIT_PB_SETUP;

#if defined(CONFIG_DEBUG)
		HxLOG_Debug ("------------<XMGR_PLAYBACK:SETUP>------------\n");
		switch (stSetup.ePbActType)
		{
			case ePb_ActType_Playback:					HxLOG_Debug ("\t[ePbActType\t\t:\tPLAYBACK]\n"); break;
			case ePb_ActType_Tsr_Playback:				HxLOG_Debug ("\t[ePbActType\t\t:\tTSR PB]\n"); break;
			case ePb_ActType_Chase_Playback:			HxLOG_Debug ("\t[ePbActType\t\t:\tCHASE PB]\n"); break;
			case ePb_ActType_Transcoding_Playback:		HxLOG_Debug ("\t[ePbActType\t\t:\tTRANS PB]\n"); break;
			case ePb_ActType_Transcoding_TsrPlayback:	HxLOG_Debug ("\t[ePbActType\t\t:\tTRANS TSR PB]\n"); break;
			default: break;
		}
		HxLOG_Debug ("\t[ulContentId\t:\t%d]\n"		, pstContext->stSetup.ulContentId);
		HxLOG_Debug ("\t[eFirstTrickState\t:\t%d]\n", pstContext->stSetup.eTrickState);
		HxLOG_Debug ("\t[ulStartTime\t:\t%d]\n"		, stSetup.ulStartTime);
		HxLOG_Debug ("\t[eSvcType\t:\t%d]\n"		, stSetup.eSvcType);
		HxLOG_Debug ("\t[bEncryption\t:\t%d]\n"		, stSetup.bEncryption);
		HxLOG_Debug ("\t[eStartPosType\t:\t%d]\n"	, stSetup.eStartPosType);
		HxLOG_Debug ("\t[aucPath\t:\t%s]\n"			, stSetup.aucPath);
		HxLOG_Debug ("\t[aucFileName\t:\t%s]\n"		, stSetup.aucFileName);
		HxLOG_Debug ("\t[bDescramblePlay\t:\t%d]\n", stSetup.bDescramblePlay);
		HxLOG_Debug ("\t[ulRecordId\t:\t%d]\n"		, stSetup.ulRecordId);
		HxLOG_Debug ("\t[bMakeThumbnail\t:\t%d]\n"	, stSetup.bMakeThumbnail);
		HxLOG_Debug ("------------<SETUP:XMGR_PLAYBACK>------------\n");
#endif

		hErr = SVC_PB_Setup(pstContext->hAction, &stSetup, TRUE);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("SVC_PB_Setup Error. (%x)\n", pstContext->hAction);
		}

		hErr = xmgr_playback_CiPlusGetDrmInfoByTime_Base (pstTimeStampList, stRecordInfo.ulStartTime, &pstContext->stDrmInfo );
		if (ERR_OK == hErr)
			SVC_META_SetDRMInfo (ulContentID, &pstContext->stDrmInfo, FALSE);
	}

	// Standby 에서 Thumbnail 을 만들기 위해 시작한 playback 이 아니면 Panel 에 Display
	if ( MGR_ACTION_GetState() != eAmState_ACTIVE_STANDBY )
	{
		if(eActionType == eViewType_PB)
		{
			SVC_SYS_SetLedOnOff(eDxLED_ID_PLAY, TRUE);
			SVC_SYS_SetLedOnOff(eDxLED_ID_PAUSE, FALSE);
			SVC_SYS_DisplayPanelString (ePANEL_STR_PRIORITY_VIEW_ACTION, "", eDxPANEL_ALIGN_CENTER);
		}
		BUS_SetTimer(PANEL_STRING_TIMER_ID, PANEL_STRING_TIME_OUT);
	}

	BUS_SendMessage(NULL, eMEVT_PVR_NOTIFY_PLAYBACK_STARTED, pstContext->hAction, 0, 0, 0);

	/* thumbnail */
#if defined(CONFIG_APCORE_MOVING_THUMBNAIL_CACHE)
	if (bRecord == TRUE)
	{
		// 녹화가 진행중이라서 rec time이 계속 변하는 경우... to be continue
	}
	else
	{
		// 녹화가 완료되어 rec time이 고정된 경우
		// 최소 간격 5초, 최대 개수 MAX_PB_THUMB
		pstContext->ulThumbInterval = (pstContext->ulEndTime / MAX_PB_THUMB) + 1;

		if (pstContext->ulThumbInterval < 5)
		{
			pstContext->ulThumbInterval = 5;
		}

		pstContext->ulThumbNum = pstContext->ulEndTime / pstContext->ulThumbInterval;
	}

	BUS_SetTimer(PB_EXTRACT_THUMB_TIMER_ID, PB_EXTRACT_THUMB_TIME);
#endif

	SVC_META_RegisterPvrAction(pstContext->hAction, ulContentID, eSvcMeta_UsingActType_Playback);

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return MESSAGE_BREAK;
}

BUS_Result_t xmgr_playback_CiPlusMsgEvtPbTimecodeChange_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR	 hErr = ERR_FAIL;
	Handle_t hRecAction = HANDLE_NULL;
	SvcRec_RecInfo stRecInfo;

	/* timestamp of meta */
	if (eMgrPb_TickState_PLAY == pstContext->eTrickState)
			xmgr_playback_CiPlusCheckDrmTimeStamp_Base (pstContext, (HUINT32)p1, (HUINT32)p2);
	else	xmgr_playback_CiPlusCheckTimeStamp_Base (pstContext, (HUINT32)p1, (HUINT32)p2);

	/* PAUSE 상태는 TimeCode Update를 안함.*/
	if (eMgrPb_TickState_PAUSE == pstContext->eTrickState)
		return MESSAGE_PASS;

	pstContext->ulPlayTime = (UNIXTIME)p1;
	pstContext->bFirstTimecode = TRUE;

	if(pstContext->stDrmInfo.stCasCcInfo.eCasCcType == eDxCopyrightControl_CasCcType_CiPlus)
	{
		hErr = xmgr_playback_CiPlusApplyRetentionLimit_Base(pstContext, TRUE);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("ERROR: xmgr_playback_CiPlusApplyRetentionLimit_Base()\n");
		}

		hErr = xmgr_playback_CheckDelayedPlaybackLicenseExchangeResponse(pstContext);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("ERROR: xmgr_playback_CheckDelayedPlaybackLicenseExchangeResponse()\n");
		}

		// 아래는 branch code rev #98720에서 추가된 내용이나 나중에 다시 빠질 내용. 확인후 없애도록.
		//if (pstContext->ulPlayTime < pstContext->ulDrmUriTime)
		//{
		//	//play time 이 drm licens time 보다 작다는 의미는 사용자의 의해서 back 으로 이동 되었다는 의미이므로 drm을 reset 하여 다시 체크하도록 함
		//	MW_PVR_ResetDrmUriRefTime(context->hAction, context->ulPlayTime, context->ulDrmUriTime);
		//	context->ulDrmUriTime = context->ulPlayTime;
		//}
	}


	// Medialist 에서 레코딩중인 파일을 재생한 경우에 대한 처리
	hErr = SVC_META_GetRecActionByContentId (pstContext->ulContentId, &hRecAction);
	if ((ERR_OK != hErr) || (HANDLE_NULL == hRecAction))
	{
		return MESSAGE_PASS;
	}

	HxSTD_MemSet (&stRecInfo, 0, sizeof(SvcRec_RecInfo));
	hErr = SVC_REC_GetInfo (hRecAction, &stRecInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_REC_GetInfo. Error... \n");
		return MESSAGE_PASS;
	}

	if (pstContext->ePbType & (eMgrPb_PlayType_Normal | eMgrPb_PlayType_Chase))
	{
		pstContext->ulEndTime = stRecInfo.ulRecTime;
	}
	else if ((pstContext->ePbType & 0x0000000f) == eMgrPb_PlayType_Tsr)
	{
		pstContext->ulStartTime = stRecInfo.ulStartTime;
		pstContext->ulEndTime	= stRecInfo.ulEndTime;
	}

#if	defined(CONFIG_IGNORE_TRICKPLAY_IN_PMTCHANGED_SERVCE)
	//	Need to control Trick position
	//	언제는 Disable인 경우도 오겠지?
	if (pstContext->bTrickEnable)
	{
		//	PMT is updated!!!
		if (TRUE == p3)
		{
			pstContext->bTrickEnable = FALSE;

			if (pstContext->eTrickState != eMgrPb_TickState_PLAY)
			{
				pstContext->ePrevTrickState = pstContext->eTrickState;
				pstContext->ePrevTrickSpeed = pstContext->eTrickSpeed;

				MGR_PLAYBACK_SetPlaySpeed (pstContext->ulActionId, 100);
				HxLOG_Debug("Forced Trick Play mode to NORMAL\n");
			}
		}

		HxLOG_Debug("PMT Updated --> flag setting... Forced Trick to Normal Mode!!!\n");
	}
#endif

	NOT_USED_PARAM(p2);

	return MESSAGE_PASS;
	//return MESSAGE_BREAK; -> original code, why stop here ?
}


#define _____PROTO_PROCEDURE_____
STATIC BUS_Result_t xproc_playback_CiPlusProto_Base (xmgrPbContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t	eBusResult = ERR_BUS_NO_ERROR;
	HUINT32			ulActionId = SVC_PIPE_GetActionId(hAction);

	switch (message)
	{
		case eMEVT_BUS_CREATE:
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eMEVT_BUS_CREATE\n", ulActionId);
			eBusResult = xmgr_playback_CiPlusMsgBusCreate_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_BUS_DESTROY:
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eMEVT_BUS_DESTROY\n", ulActionId);
			eBusResult = xmgr_playback_MsgBusDestroy_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_BUS_TIMER:
			HxLOG_Info("[MGR:Pb(%d)] MESSAGE : eMEVT_BUS_TIMER\n", ulActionId);
			eBusResult = xmgr_playback_MsgBusTimer_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_ACTION_NOTIFY_RELEASED:
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eMEVT_ACTION_NOTIFY_RELEASED\n", ulActionId);
			eBusResult = xmgr_playback_MsgAmActionStopped_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_PVR_NOTIFY_RECORD_STOPPED:
		case eMEVT_PVR_NOTIFY_RECORD_SPLIT_FILE:
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eMEVT_PVR_NOTIFY_RECORD_STOPPED\n", ulActionId);
			eBusResult = xmgr_playback_MsgAppRecordStop_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eOEVT_PG_CHECK_RATING :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eOEVT_PG_CHECK_RATING\n", ulActionId);
			eBusResult = xmgr_playback_MsgAppCheckRating_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_SVCLIST_SVC_PID_CHANGED :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eMEVT_SVCLIST_SVC_PID_CHANGED\n", ulActionId);
			eBusResult = xmgr_playback_MsgAppDbPidChanged_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_PG_PIN_CHECK_OK :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eMEVT_PG_PIN_CHECK_OK\n", ulActionId);
			eBusResult = xmgr_playback_MsgAppPinOK_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_PG_PIN_CHECK_DENIED :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eMEVT_PG_PIN_CHECK_DENIED\n", ulActionId);
			eBusResult = xmgr_playback_MsgAppPinDenied_Base(pstContext, hAction, p1, p2, p3);
			break;


		case eSEVT_PB_SETUP:
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_PB_SETUP\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtPbSetup_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_AV_SESSION_STARTED:
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_AV_SESSION_STARTED\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtAvDecodingStarted_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_PB_START:
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_PB_START\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtPbStart_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_PMT :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_SI_PMT\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtSiPmtReceived_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_PAT_TIMEOUT:
		case eSEVT_SI_PMT_TIMEOUT:
		case eSEVT_SI_PMT_PIDREMOVED:
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_SI_PAT_TIMEOUT\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtSiPmtTimeout_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_PMT_PIDCHANGED :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_SI_PMT_PIDCHANGED\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtSiPmtPidChanged_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_EIT :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_SI_EIT\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtSiEitReceived_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_EIT_TIMEOUT :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_SI_EIT_TIMEOUT\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtSiEitTimeout_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_AV_VIDEO_UNDERRUN :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_AV_VIDEO_UNDERRUN\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtAvVideoUnderrun_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_AV_VIDEO_STARTED :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_AV_VIDEO_STARTED\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtAvVideoStarted_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_AV_AUDIO_UNDERRUN :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_AV_AUDIO_UNDERRUN\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtAvAudioUnderrun_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_AV_AUDIO_STARTED :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_AV_AUDIO_STARTED\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtAvAudioStarted_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_PB_TIMECODE_CHANGE :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Info("[MGR:Pb(%d)] MESSAGE : eSEVT_PB_TIMECODE_CHANGE\n", ulActionId);
			eBusResult = xmgr_playback_CiPlusMsgEvtPbTimecodeChange_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_PB_EOF :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_PB_EOF\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtPbEOF_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_PB_BOF :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_PB_BOF\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtPbBOF_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SBTL_STOPPED:
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_SBTL_STOPPED\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtSbtlStopped_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SBTL_STARTED:
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_SBTL_STARTED\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtSbtlStarted_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_OK :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_CAS_OK\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtCasOk_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_FAIL :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_CAS_FAIL\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtCasFail_Base(pstContext, message, p1, p2, p3);
			break;

		case eSEVT_CAS_CHECK :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_CAS_CHECK\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtCasFail_Base(pstContext, message, p1, p2, p3);
			break;

		case eMEVT_CAS_SYSINFO_UPDATED:
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eMEVT_CAS_SYSINFO_UPDATED\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtCasScInserted_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_SC_REMOVED:
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_CAS_SC_REMOVED\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtCasScRemoved_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_DRM_AV_BLANK:
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_CAS_DRM_AV_BLANK\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtAvBlankCtrlByCAS_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_PB_DRM_AV_BLANK:
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_PB_DRM_AV_BLANK\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtAvBlankByDrmInfo_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_DRM_SIGNALLED:
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_CAS_DRM_SIGNALLED\n", ulActionId);
			eBusResult = xmgr_playback_CiPlusMsgEvtCasDrmSignalled_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_PVR_RECORD_TIMESTAMP_UPDATE:
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eMEVT_PVR_RECORD_TIMESTAMP_UPDATE\n", ulActionId);
			eBusResult = xmgr_playback_CiPlusMsgEvtRecordTimeStampUpdate_Base(pstContext, hAction, p1, p2, p3);
			break;

		default :
			break;
	}

	if(eBusResult != ERR_BUS_NO_ERROR)
	{
		return eBusResult;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}


STATIC BUS_Result_t xproc_playback_CiPlusAction0_Base (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t		*pstContext = xmgr_playback_GetContextFromIndex_Base (0);

	return xproc_playback_CiPlusProto_Base (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_CiPlusAction1_Base (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t		*pstContext = xmgr_playback_GetContextFromIndex_Base (1);

	return xproc_playback_CiPlusProto_Base (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_CiPlusAction2_Base (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t		*pstContext = xmgr_playback_GetContextFromIndex_Base (2);

	return xproc_playback_CiPlusProto_Base (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_CiPlusAction3_Base (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t		*pstContext = xmgr_playback_GetContextFromIndex_Base (3);

	return xproc_playback_CiPlusProto_Base (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_CiPlusAction4_Base (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t		*pstContext = xmgr_playback_GetContextFromIndex_Base (4);

	return xproc_playback_CiPlusProto_Base (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_CiPlusAction5_Base (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t		*pstContext = xmgr_playback_GetContextFromIndex_Base (5);

	return xproc_playback_CiPlusProto_Base (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_CiPlusAction6_Base (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t		*pstContext = xmgr_playback_GetContextFromIndex_Base (6);

	return xproc_playback_CiPlusProto_Base (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_CiPlusAction7_Base (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t		*pstContext = xmgr_playback_GetContextFromIndex_Base (7);

	return xproc_playback_CiPlusProto_Base (pstContext, message, hAction, p1, p2, p3);
}

#define _____MSG_PROCESSING_FUNCTIONS_____

BUS_Callback_t	xmgr_playback_CiPlusGetProcFunc_Base (HUINT32 ulActionId)
{
	HUINT32	ulPbIndex = 0;

	if ((ulActionId >= eActionId_VIEW_FIRST) && (ulActionId <= eActionId_VIEW_LAST))
		ulPbIndex	=  (ulActionId - eActionId_VIEW_FIRST);
	else if ((ulActionId >= eActionId_BG_FIRST) && (ulActionId <= eActionId_BG_LAST))
		ulPbIndex	=  (ulActionId - eActionId_BG_FIRST) + NUM_VIEW_ACTION;

	if (ulPbIndex >= MAX_NUM_OF_PLAY)
	{
		HxLOG_Error ("Wrong Action ID:(%d)\n", ulActionId);
		return NULL;
	}

	switch (ulPbIndex)
	{
	case 0:
		return xproc_playback_CiPlusAction0_Base;

	case 1:
		return xproc_playback_CiPlusAction1_Base;

	case 2:
		return xproc_playback_CiPlusAction2_Base;

	case 3:
		return xproc_playback_CiPlusAction3_Base;

	case 4:
		return xproc_playback_CiPlusAction4_Base;

	case 5:
		return xproc_playback_CiPlusAction5_Base;

	case 6:
		return xproc_playback_CiPlusAction6_Base;

	case 7:
		return xproc_playback_CiPlusAction7_Base;

	default:
		break;
	}

	return NULL;
}

#endif
