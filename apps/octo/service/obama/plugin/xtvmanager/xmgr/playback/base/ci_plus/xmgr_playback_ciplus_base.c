
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
							// ����� URI Timestamp �� Default�� PB�� �����Ű�� �ʾƵ� �� ����. (�̰� ������ �ȴٸ� ���ð��� �ִ� default and from CAM URI �� from CAM URI�� ����ǵ��� �ڵ� ���� �ʿ�)
							// ucMode = 1�� defualt URI�� ��Ÿ��.
							// default URI�� ó������ �ʴ� ������ �⺻������ ��� CI+ contents�� hjm ���Ͽ� ��ǥ URI ������ ����Ǿ� ó���ǰ� �ְ�,
							// 10�� �̳��� timeout URI�� from CAM URI�� ����� ���� �� ���̱� ����.
							if(pstMetaDrm->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ucMode == 1)
							{
								continue;
							}

							// Timestamp data �� URI�� ���ǰ� �ִ� �κ��� check �� ��.
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
						*	TSR���� �� ulTime �� UNIXTIME���� ������, ulSourceTime �� offset time���� �����⿡ �񱳰� �ȵȴ�.
						*	tsr offset time ���� �̿��Ͽ� ���� Ŀ���� UNIXTIME�� ���� ���� ���ϵ��� ������.
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
							// Timestamp data �� URI�� ���ǰ� �ִ� �κ��� check �� ��.
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
			if (pstList->ulDrmUriRefIdx == 0)	// �ϴ� list�� 0�� index�� DRM ���� timestamp ���� �ٸ� timestamp�� ������ ���� �Ǳ� ������ 0���� �ƴ϶�� ��.
			{
				HxLOG_Info("\n======= URI Timestamp is updated... =======\n\n");
				pstList->pstData[nUriIdx].stTimeStamp.bIsUsed = TRUE;
				pstList->ulDrmUriRefIdx = nUriIdx;
				// TODO: �Ʒ� EVT ������ �κ� & �޾Ƽ� ó���ϴ� �κ� �۾� (kkkim)
				//MWC_UTIL_PostMsgToAp (EVT_PVR_DRM_SIGNALED, pstContext->hAct, (HUINT32)pstList->pstData[nUriIdx].pData, nUriIdx, ulUriTime);
			}
			else
			{
				HxLOG_Info("\n======= URI Timestamp is updated... =======\n\n");
				pstList->pstData[pstList->ulDrmUriRefIdx].stTimeStamp.bIsUsed = FALSE;
				pstList->pstData[nUriIdx].stTimeStamp.bIsUsed = TRUE;
				pstList->ulDrmUriRefIdx = nUriIdx;
				// TODO: �Ʒ� EVT ������ �κ� & �޾Ƽ� ó���ϴ� �κ� �۾� (kkkim)
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
			if (pstList->ulDrmPcRefIdx == 0)	// �ϴ� list�� 0�� index�� DRM ���� timestamp ���� �ٸ� timestamp�� ������ ���� �Ǳ� ������ 0���� �ƴ϶�� ��.
			{
				HxLOG_Info("\n======= Parental Control Timestamp is updated...(pstList->ulDrmPcRefIdx==0) =======\n\n");
				pstList->pstData[nPcIdx].stTimeStamp.bIsUsed = TRUE;
				pstList->ulDrmPcRefIdx = nPcIdx;
				// TODO: �Ʒ� EVT ������ �κ� & �޾Ƽ� ó���ϴ� �κ� �۾� (kkkim)
				//MWC_UTIL_PostMsgToAp (EVT_PVR_DRM_SIGNALED, pstContext->hAct, (HUINT32)pstList->pstData[nPcIdx].pData, nPcIdx, ulPcTime);
			}
			else
			{
				HxLOG_Info("\n======= Parental Control Timestamp is updated... =======\n\n");
				pstList->pstData[pstList->ulDrmPcRefIdx].stTimeStamp.bIsUsed = FALSE;
				pstList->pstData[nPcIdx].stTimeStamp.bIsUsed = TRUE;
				pstList->ulDrmPcRefIdx = nPcIdx;
				// TODO: �Ʒ� EVT ������ �κ� & �޾Ƽ� ó���ϴ� �κ� �۾� (kkkim)
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
				// TODO: �Ʒ� EVT ������ �κ� & �޾Ƽ� ó���ϴ� �κ� �۾� (kkkim)
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
						// TODO: �Ʒ� EVT ������ �κ� & �޾Ƽ� ó���ϴ� �κ� �۾� (kkkim)
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
						// TODO: �Ʒ� EVT ������ �κ� & �޾Ƽ� ó���ϴ� �κ� �۾� (kkkim)
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
							// xmgr_playback_CiPlusCheckCiPlusDrmTimeStamp() �Լ����� �̹� CI+ DRM�� ó�� �� �߱� ������ �ƹ��͵� ���� �ʵ��� ��.
							break;

						default:
							if (pstList->ulDrmRefTime != ulTime)
							{
								pulDrmRefTime = &(pstList->ulDrmRefTime);
							}
					}

					if (pulDrmRefTime)
					{
						// TODO: �Ʒ� EVT ������ �� ������ �°� ���� �� ��!
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
							// xmgr_playback_CiPlusCheckCiPlusDrmTimeStamp() �Լ����� �̹� CI+ DRM�� ó�� �� �߱� ������ �ƹ��͵� ���� �ʵ��� ��.
							break;

						default:
							if (pstList->ulDrmRefTime != ulTime)
							{
								pulDrmRefTime = &(pstList->ulDrmRefTime);
							}
					}

					if (pulDrmRefTime)
					{
						// TODO: �Ʒ� Send Message�� eSEVT_CAS_DRM_SIGNALLED������ ���� ���� �������� �ľ� �ʿ� (HMS PH1 code�� ��) (kkkim)
						// �̺�Ʈ �޾Ƽ� ó���ϴ� �κе� ���� Ȯ�� �ʿ� ��.
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


#if defined(CONFIG_MW_CI_PLUS_CC_V_2)	// return�� ERR_OK�̸�, CAM�� ã����. ERR_FAIL�̸� ��ȭ�� ����� CAM�� ���ԵǾ� ���� ����.
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
		case eDxCopyrightControl_CiPlusDrmType_ParentalCtrl:			// CI+1.3 ���忡 ���� PC�� �ɸ� �κ��� PIN�� ���� �� ���� A/V�� ���´�. (PIN �� �Է� �ޱ� ������ ȭ���� ������ �ʴµ�, �̸� ���Ͽ� ���� �̺�Ʈ �߻� ���� ���� PIN ���� �̺�Ʈ�� ���� ȭ�� ���� ���� PB �Ͽ��� ������ ����� ���� �ƴ�����, ���� ������ �ϴ��� ���� PC event�� �߻� �ϴ� ������ �ɸ��� �׶� CAM������ �̺�Ʈ�� ������ A/V�� ����.
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
					// DRM�� ���Ͽ� A/V�� blanking...
					pstContext->eCiPlusDrmState |= eXmgrPbCiPlusDrmState_PC;
					xmgr_playback_SetAppState_Base(pstContext, eXmgrPbState_CAS_BLOCKED);
				}
				else
				{
					// AV�� �����൵ �ȴ� do nothing...
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
				// TODO: �ش� contents�� ���� ���ؼ��� ��ȭ�� ����� CAM�� ���� �Ǿ� �־�� �մϴ�. Message �ѷ��ְ� Program Playback ���� (kkkim)
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

						// Retention Limit �� ���� ���ᰡ �Ǹ� license�� �����ϸ� �ȵ� (5.3.1.2. Application Of The Playback Retention Limit�� step 28 ~ 32)
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
				// TODO: �ش� contents�� ���� ���ؼ��� ��ȭ�� ����� CAM�� ���� �Ǿ� �־�� �մϴ�. Message �ѷ��ְ� Program Playback ���� (kkkim)
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
			// �̹� Recording ��ÿ� CAM���� ���� URI�� �ִ� ��쿡�� Playback�� CAM���� ���� ���޹��� URI ������ �����ϵ��� �Ѵ�.
			// ������ playback �� �� �Լ��� ���� �� ���� scrambled ������ �ִٴ� ���ε�, ���� ������ �ٸ� CAM�� ����Ͽ� RL������ �� �� �ֱ� �����̴�.
			// (���� ��� 1)SKY CI+CAM�� RL�� 90���� �ɸ���, ORF CI+CAM�� Unlimit�� �� �� �ִٰ� ����. 2)��ȭ�� service�� CA System ID�� �� CAM�� CA system ID�� ��� ������ �� ����.
			// scramble ������ �ణ �ִ� SKY content�� ���߿� playback �� �� ORF CAM�� ���ԵǾ� �ִٰ� ��������.
			// 1), 2) ������ �����ϴ� ��Ȳ�̸� Playback�� �� �� ts�� ORF CI+CAM�� 2�� ���ǿ� ���� ���� �ǰ�, URI�� CAM���κ��� Box�� ���� �������� �ȴ�.
			// �׷���, ����� Content�� SKY�� 90�� limit�� �ɷ��� ������ ���� ���� URI�� ���� ������ pb�� �����ϰ� �ȴ�. �̷��� �������� ���ƾ� ��.

			if (pstContext->stDrmInfo.stCasCcInfo.eCasCcType == eDxCopyrightControl_CasCcType_CiPlus)
			{
				if (pstContext->stDrmInfo.stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ucMode == CI_CC_URI_MODE_FROMCAM)
				{
					HxLOG_Info("The old URI info is from the CAM, so the new URI info is ignored...\n");
					break;
				}
			}

			#else
			// ��ȭ�� content�� scramble ������ �����ϰ� �ִ� ���¶��, Meta���� URI ������ �ְ�, CAM���κ��͵� URI ������ �޴´�.
			// ���� Meta ��ü�� CAM���κ��� ���� URI�� ����� ���� �Ǿ��ִµ�,
			// partially scrambled REC file�� descramble �ϴ� �������� ���۵Ǵ� default URI��, timeout URI��
			// (Meta�� CAM���� ���������� ���� URI������ ����Ǿ� �ִٸ�) Playback �� URI ���ۿ� ������ ��ġ�� �ȵȴ�.
			if( (pstContext->stDrmInfo.stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ucMode == CI_CC_URI_MODE_FROMCAM)
				&& (pstDrmInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ucMode < CI_CC_URI_MODE_FROMCAM))
			{
				HxLOG_Info("The old URI info is from the CAM, but the new URI info is not from the CAM. So, it should be ignored...\n");
				break;
			}
			#endif

			// CI+ specification���� ��߳��� ����������, Humax���������� URI timeout �� �߻��Ǵ� contents�� legacy CI content��� ����.
			// ���� legacy CI content�� timeout���� URI meta�� ����Ǿ� ���� ���̰�,
			// ���� scrambled ������ �־� playback �� ������� Default URI�� ���õǾ�� �Ѵ�.
			if( (pstContext->stDrmInfo.stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ucMode == CI_CC_URI_MODE_TIMEOUT)
				&& (pstDrmInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ucMode == CI_CC_URI_MODE_DEFAULT))
			{
				HxLOG_Info("The old URI info is from the Timeout(it means the content is legacy CI content), and new URI info is the default URI. So, it should be ignored...\n");
				break;
			}

			if(unMetaIndex)  // ����� Playback License Exchange �� ���� unMetaIndex �� �����ǰ� �ش� URI�� update �ϵ��� ��
			{
				HULONG		ulSysTime = 0;
				HUINT32 	ulOffset = 0;
				DxCopyrightControl_t	stDrmInfo = *pstDrmInfo;

				if(VK_CLOCK_GetTime( &ulSysTime ) != ERR_OK)
					return MESSAGE_BREAK;

				// Retention Limit �� ��ȭ�� �ð��� ���� �ð��� ���̸� ���� �༭ ���� playback�� ���� �� �� �ֵ��� �Ѵ�.
				// 5.3.1.2. Application Of The Playback Retention Limit �� step25 ����
				HxLOG_Print("update metadata Index(%d), Retention Limit (%d minutes + offset (%d))", unMetaIndex, stDrmInfo.stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ulMinutesForRL, ((ulSysTime - pstContext->ulPlayTime - pstContext->ulOrgStartTime)/60));

				stDrmInfo.stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ulMinutesForRL += ((ulSysTime - pstContext->ulPlayTime - pstContext->ulOrgStartTime)/60);

				SVC_META_UpdateDrmTimeStampByIndex(pstContext->ulContentId, &stDrmInfo, unMetaIndex);

				//	meta data ���� �ε��� RL(Retention Limit) ���� offset("�ٷ� �� ���� ((ulSysTime - pContext->ulPlayTime - pContext->ulOrgStartTime)/60)") �� �����ϸ�
				// update �� ���ο� RL ���� offset ��ŭ ���ؼ� ap_player �� pstContext�� RL�� ����ǵ��� ��.	 (CiCc_GetMinutesForRL �����Ͽ� offset ���� ����)
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
			// �� �ΰ��� �̿��� ��쿡�� URI ������ ������Ʈ �Ѵ�.
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
					// License ������ ��� �Ͽ��� �ϴ���, �ٸ� DRM ������ �ɷ� �ִٸ� A/V�� ����ؼ� ���´�.
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
			// TODO: �ش� Metadata��update �Ϸ� �Ǿ��ٴ� ������ Meta�ʿ� ��� ����� ��. (kkkim)
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

	// RL���� 0�� ���� unlimit�ϰ� �����൵ �Ǵ� ���̹Ƿ� �ð� �񱳸� ���� �ʾƵ� ��. A/V �� RL�� �������� �����ִٸ� A/V �����ְ� OK return
	// �� �� ���� ��ȿ�� ���� URI ������ CAM���� ���� ���� ���� ���̰ų�, timeout �ɷ��� �� ���� URI�������� ��ȿ ��.
	if(ulMinutesForRL == 0 && pstContext->stDrmInfo.stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ucMode > CI_CC_URI_MODE_DEFAULT)
	{
		if (pstContext->ePbState == eXmgrPbState_CAS_BLOCKED)
		{
			pstContext->eCiPlusDrmState &= (~eXmgrPbCiPlusDrmState_RL);
			if(pstContext->eCiPlusDrmState)
			{
				// Retention Limit�� ��� �Ͽ��� �ϴ���, �ٸ� DRM ������ �ɷ� �ִٸ� A/V�� ����ؼ� ���´�.
				HxLOG_Info("Still A/V is blanking... because of CI+ DRM...\n");
			}
			else
			{
				xmgr_playback_SetAppState_Base(pstContext, eXmgrPbState_AV);
			}
		}

		return ERR_OK;
	}

	/* ��ȭ ���� �ð����κ��� play �ϴ� ������ �ð��� ���Ͽ� �ش� ��ġ�� UNIXTIME �� ���ϰ�, */
	/* ���� �ð����� play �ϴ� ������ �ð��� �����Ͽ� �ش� ������ ��� ���� ��ȭ�� ������ ���Ѵ�. */
	/* ����� DRM�� RL �ð��� ���ϰ�, play ������ RL �ð��� �ʰ� �� ��� ���� �޽��� ��� �� RL�ð� �̳��� ������ ��ġ�� ��� �Ѵ�. */
	/* PLAY_ENDTIME_GAP�� ������ ������ Live Broadcast �ÿ� (ulSysTime - (pstRecInfo->ulStartTime + pstContext->ulPlayTime) ���� ���̳ʽ�(-1) ���� ���� */
	/* ����� ������ ���� �ϱ� ���Ͽ� ����. */
	if ((ulSysTime - (pstContext->ulOrgStartTime + pstContext->ulPlayTime) + PLAY_ENDTIME_GAP) > (ulMinutesForRL * 60))
	{
		if (pstContext->ePbState != eXmgrPbState_CAS_BLOCKED)		// CI+ ������ eXmgrPbState_CAS_BLOCKED�� RL�� ���ؼ��� ����ϵ���...
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
				// Retention Limit�� ��� �Ͽ��� �ϴ���, �ٸ� DRM ������ �ɷ� �ִٸ� A/V�� ����ؼ� ���´�.
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

// CI+ DRM ���ۿ� ���� CAS MGR �������� �ٸ��� �����ϵ��� ������ ����.
// CAS MGR�� ���� ������ ���� OCTO2�� ����ϴ� ���� HMS, NGS, GEN2 �ε�, HMS�� ver2�� ��������, NGS GEN2�� ���� mgr�� �۾��� �ϰ� �ֱ� ������.
// (2012.11.09. ����) GEN2�� tagging ���� ���� ��Ȳ�̰�, NGS�� phase 2�� MGR2�� �� �����ε�,
// �̶� NGS�� �� �Ѿ���� CAS MGR 2�� ���ܵΰ� �������� �� �����ϵ��� �Ѵ�.
	DxCopyrightControl_t			*pstDrmInfo;
	HUINT32				unMetaIndex;
	HUINT32				ulTime;
	pstDrmInfo = (DxCopyrightControl_t *)p1;
	unMetaIndex = p2;
	ulTime = (HUINT32)p3;

	// TODO: �� CI+ DRM Type�� ���� ó�� �κ� ����(kkkim)
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

	// TODO: �� CI+ DRM Type�� ���� ó�� �κ� ����(kkkim)
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
	// Resouce �� ��� Ready�� ������.
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
	pstContext->nPlaySpeed		= 100;			// 1��� Play (100%)
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

	// Medialist ���� ���ڵ����� ������ ����� ��쿡 ���� ó��
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


	/* Session version ���� */
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

	// Standby ���� Thumbnail �� ����� ���� ������ playback �� �ƴϸ� Panel �� Display
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
		// ��ȭ�� �������̶� rec time�� ��� ���ϴ� ���... to be continue
	}
	else
	{
		// ��ȭ�� �Ϸ�Ǿ� rec time�� ������ ���
		// �ּ� ���� 5��, �ִ� ���� MAX_PB_THUMB
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

	/* PAUSE ���´� TimeCode Update�� ����.*/
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

		// �Ʒ��� branch code rev #98720���� �߰��� �����̳� ���߿� �ٽ� ���� ����. Ȯ���� ���ֵ���.
		//if (pstContext->ulPlayTime < pstContext->ulDrmUriTime)
		//{
		//	//play time �� drm licens time ���� �۴ٴ� �ǹ̴� ������� ���ؼ� back ���� �̵� �Ǿ��ٴ� �ǹ��̹Ƿ� drm�� reset �Ͽ� �ٽ� üũ�ϵ��� ��
		//	MW_PVR_ResetDrmUriRefTime(context->hAction, context->ulPlayTime, context->ulDrmUriTime);
		//	context->ulDrmUriTime = context->ulPlayTime;
		//}
	}


	// Medialist ���� ���ڵ����� ������ ����� ��쿡 ���� ó��
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
	//	������ Disable�� ��쵵 ������?
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
