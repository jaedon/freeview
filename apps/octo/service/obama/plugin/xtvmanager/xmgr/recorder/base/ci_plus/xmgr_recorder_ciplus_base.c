
#if defined(CONFIG_MW_CI_PLUS)
/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <isosvc.h>
#include <db_svc.h>
#include <svc_si.h>
#include <svc_fs.h>
#include <svc_cas.h>
#include <svc_pipe.h>
#include <svc_sys.h>
#include <linkedlist.h>
#include <bus.h>

#include <mgr_action.h>
#include <mgr_live.h>
#include <mgr_recorder.h>
#include <mgr_storage.h>

#include <_xmgr_recorder.h>
#include "../local_include/_xmgr_recorder_base.h"

#include <xmgr_cas.h>

#include <ci_ctrl.h>

STATIC HBOOL xmgr_rec_CiPlusCheckCiUriMetaPriority_Base(HUINT32 ulContentID, DxCopyrightControl_t *pstNewDrmInfo)
{
	HERROR 		hErr			= ERR_FAIL;
	DxCopyrightControl_t	stSavedDrmInfo;

	HxSTD_MemSet (&stSavedDrmInfo, 0, sizeof(DxCopyrightControl_t));
	hErr = SVC_META_GetDRMInfo(ulContentID, &stSavedDrmInfo);

	if (eDxCopyrightControl_CiPlusDrmType_Uri != stSavedDrmInfo.stCasCcInfo.info.stCiPlusDrmInfo.eCiPlusDrmType)
	{
		HxLOG_Error("Saved DRM Info is not URI data!!!");
		return TRUE;
	}

	if (stSavedDrmInfo.stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ucMode <
		pstNewDrmInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ucMode)
	{
		// default ���� timeout �� URI ������ �� strict ��.
		// ���� CAM���κ��� ���� URI �����̰�, ������ ����Ǿ� �ִ� URI�� default �̰ų� timeout �����̸� update �Ǿ�� ��.
		return TRUE;
	}
	else// ������ ����� URI ������ ���� �Ѿ�� URI ������ ��� CAM���� ���� �� ���
	{
		// Output ���õ� ������ timestamp�� ���� �ٷ� ����Ǳ� ������ copy control�� �켱������ �д�.
		/* EMI	0x0 : Copying not restricted
				0x1 : No futher copying is permitted
				0x2 : One generation copy is permitted
				0x3 : Copying is prohibited
		*/
		if (stSavedDrmInfo.stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ucEmi <
			pstNewDrmInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ucEmi)
			return TRUE;
	}

	return FALSE;
}

STATIC HERROR xmgr_rec_CiPlusSetCiPlusDrm_Base(Handle_t hAction, HUINT32 ulContentID, DxCopyrightControl_t *pstDrm, HBOOL bMakeTimeStamp)
{
	HERROR		hErr		=	ERR_OK;
	HUINT32		ulRecTime	=	0;
	Handle_t	hPlayback	=	HANDLE_NULL;
	HBOOL		bSave		=	FALSE;

	// URI�� �ƴ� ������ ���ؼ��� Meta ��ü�� �������� �ʰ�, timestamp meta ���� �����ϸ� ��.
	if (eDxCopyrightControl_CiPlusDrmType_Uri == pstDrm->stCasCcInfo.info.stCiPlusDrmInfo.eCiPlusDrmType)
	{
		bSave = xmgr_rec_CiPlusCheckCiUriMetaPriority_Base (ulContentID, pstDrm);
		hErr = SVC_META_SetDRMInfo (ulContentID, pstDrm, bSave);
		if (ERR_OK != hErr )
		{
			HxLOG_Error("SVC_META_SetDRMInfo() error(%x).\n", hErr);
			return hErr;
		}
	}

	if (TRUE == bMakeTimeStamp)
	{
		SvcRec_RecInfo stRecInfo;
		HxSTD_MemSet (&stRecInfo, 0, sizeof(SvcRec_RecInfo));

		hErr = SVC_REC_GetInfo (hAction, &stRecInfo);
		if (ERR_OK != hErr )
		{
			HxLOG_Error("SVC_REC_GetInfo() error(%x).\n", hErr);
			return hErr;
		}

		ulRecTime = stRecInfo.ulDuration;

		// octo ������ ����Ǹ� �� �κе� ������ �־���. ���ۻ��� Ȯ���ϰ� ����� ������ ���� ���� ��. �Ʒ� �ּ� ���� �����ϵ��� �� ��. (branch rev #100485 ����.)
		/*
		// PAL_PVR_REC_GetDuration ���� ������ �ð���  tsr -> delayed ����ÿ� �߻��� offset �ð��� �ݿ��� ���� ���� �ð��̸�
		// timestamp �� ����Ǵ� offset�� ����� ��ȭ���� ���ð��̹Ƿ� delayed offset �ð���ŭ ���־�� ��.
		if (stRecInfo->bDelayedRecorded == TRUE)
		{
			ulRecTime = (ulRecTime > pstRecContext->ulDelayedTimeOffset) ? ulRecTime - pstRecContext->ulDelayedTimeOffset : 1;
		}
		else if (0 == ulRecTime)
			ulRecTime = 1;

		if (0 == ulRecTime)
			ulRecTime = 1;
		*/

		hErr = SVC_META_SetDrmTimeStamp (ulContentID, ulRecTime, pstDrm );
		if (ERR_OK != hErr )
		{
			HxLOG_Error("SVC_META_SetDrmTimeStamp() error(%x).\n", hErr);
			return hErr;
		}

		hErr = SVC_META_GetPlaybackActionByContentId (ulContentID, &hPlayback);
		if( hErr == ERR_OK )
		{
			/* update playback-ctx*/
			BUS_SendMessage(NULL, eMEVT_PVR_RECORD_TIMESTAMP_UPDATE, hAction, 0, 0, 0);
		}
	}

	/*event*/
	{
		SvcCas_CtrlParam_t						stParam;
		CI_NotifyDrmInfoConsumedInParam_t		stInParam;
		HUINT8 									ucDrmInfoVer = (HUINT8)pstDrm->stCasCcInfo.info.stCiPlusDrmInfo.ucCiPlusDrmInfoVer;
		HERROR 									hConsumedStatus = (HERROR)hErr;

		stParam.ulControlType 		= eCACTRL_CI_NotifyDrmInfoConsumed;
		stInParam.ucDrmInfoVer 		= ucDrmInfoVer;
		stInParam.hConsumedStatus	= hConsumedStatus;

		stParam.pvIn = (void *)&stInParam;
		hErr = SVC_CAS_CtrlNoti(eDxCAS_GROUPID_CAM, eCACTRL_CI_NotifyDrmInfoConsumed, &stParam);
		if (ERR_OK != hErr )
		{
			HxLOG_Error("SVC_CAS_CtrlNoti() error(%x).\n", hErr);
			return hErr;
		}
	}

	/*extra*/
#if 0
	{
		if( (pstRecContext->ulFirstTables & ePVR_RETRIEVE_DRM) == 0 )
		{
			hErr = PVR_Meta_SetDRMInfo( pstRecContext->ulContentsId, pstDrm, TRUE );
			if( hErr != ERR_OK )
			{
				HxLOG_Error("[PVR_RecBase_SetDrm] PVR_Meta_SetDRMInfo hAct:0x%X, hErr:0x%X\n", pstMsg->hAct, hErr );
				return ERR_FAIL;
			}

			pstRecContext->ulFirstTables |= ePVR_RETRIEVE_DRM;
		} else {
			hErr = PVR_Meta_SetDRMInfo( pstRecContext->ulContentsId, pstDrm, FALSE );
			if( hErr != ERR_OK )
			{
				HxLOG_Error("[PVR_RecBase_SetDrm] PVR_Meta_SetDRMInfo hAct:0x%X, hErr:0x%X\n", pstMsg->hAct, hErr );
				return ERR_FAIL;
			}
		}

		if( bMakeTimeStamp == TRUE )
		{
			hErr = PAL_PVR_REC_GetDuration((HUINT32)pstRecContext->ulRecordId, &ulRecTime);
			if( hErr != ERR_OK )
			{
				HxLOG_Error("[PVR_RecBase_SetDrm] PAL_PVR_REC_GetDuration, ulContentsId:0x%X, hErr:0x%X\n",
						 pstRecContext->ulContentsId, hErr);
			}
#if defined (CONFIG_MW_CI_PLUS) || defined(CONFIG_MW_SI_OP_JAPAN)
			if (ulRecTime == 0)
			{
				ulRecTime = 1;
			}
#endif
			hErr = PVR_Meta_SetDrmTimeStamp( pstRecContext->ulContentsId, ulRecTime, pstDrm );
			if( hErr != ERR_OK )
			{
				HxLOG_Error("[PVR_RecBase_SetDrm] PVR_Meta_SetDrmTimeStamp hAct:0x%X, hErr:0x%X\n", pstMsg->hAct, hErr );
				return ERR_FAIL;
			}

			hErr = PVR_Util_GetPlayerAction( pstRecContext->ulContentsId, &hPlay );
			if( hErr == ERR_OK )
			{
				PVR_Util_UpdateTimeStamp( hPlay );
			}
		}
	}
#endif
	return ERR_OK;
}

STATIC HERROR xmgr_rec_CiPlusSetDrm_Base(xmgrRec_Context_t *pstContext)
{
	/* eMgrRecState_WAIT ������ ���(record start point)���� ���� �� */

	HERROR	hErr = ERR_FAIL;

	pstContext->stNewDrmInfo.stCasCcInfo.eCasCcType = eDxCopyrightControl_CasCcType_CiPlus;

	if(pstContext->eRecState == eMgrRecState_WAIT)	// recording start
	{
		/* set drm for recorder */
//		hErr = SVC_REC_SetDRMInfo(pstContext->hAction, &pstContext->stNewDrmInfo, TRUE);
		hErr = xmgr_rec_CiPlusSetCiPlusDrm_Base (	pstContext->hAction,
										pstContext->ulContentId,
										&pstContext->stNewDrmInfo,
										TRUE);

		xmgr_rec_ClearWaitFlag(pstContext,eRecWait_DRM);

		/* start the recorder */
		xmgr_rec_StartRec(pstContext);
	}
	else
	{
		/* �� �Լ�(xmgr_rec_CiPlusSetDrm_Base)�� eDxCopyrightControl_CasCcType_CiPlus & eMgrRecState_WAIT ��쿡�� ��� ��. Ȥ�� ���Ŀ� �ٸ� ������ �� �Լ��� �θ��ٸ� ���� ���� �ʿ�.*/
		return ERR_FAIL;
	}
	return ERR_OK;
}

STATIC HERROR xmgr_rec_CiPlusOperateCiplusDrmProcess_Base(xmgrRec_Context_t *pstContext, DxCopyrightControl_t *pstDrmInfo)
{
	HERROR 	hErr = ERR_FAIL;

	if(pstContext->eRecState == eMgrRecState_WAIT)
	{
		#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
		if ((pstContext->bCasDrmValid == TRUE) && (pstContext->bCasDrmNone == FALSE) && (pstContext->bEnableCcRecording == TRUE))
		#else
		if ((pstContext->bCasDrmValid == TRUE) && (pstContext->bCasDrmNone == FALSE))
		#endif
		{
			HxLOG_Info("WAIT_DRM -> Normal Recording\n");
			hErr = xmgr_rec_CiPlusSetDrm_Base(pstContext);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("xmgr_rec_CiPlusSetDrm_Base() ERROR.\n");
				return hErr;
			}
		}
		else
		{
			HxLOG_Info("WAIT_DRM.\n");
		}
	}
	else if(pstContext->eRecState == eMgrRecState_BLOCKED)
	{
		/* �̰��� ���� �Ѵٸ� DRM info setting�� ���ְ� record�� resume �ؾ� ��. */
		/* but, eDxCopyrightControl_CasCcType_CiPlus ��忡�� pstContext->eRecState == eMgrRecState_BLOCKED ���·� �Ǵ� ���� ���� ������ �ľ� ��. */
	}
	else if(pstContext->eRecState == eMgrRecState_RECORDING)
	{
		switch (pstDrmInfo->stCasCcInfo.info.stCiPlusDrmInfo.eCiPlusDrmType)
		{
			case eDxCopyrightControl_CiPlusDrmType_Uri:
			case eDxCopyrightControl_CiPlusDrmType_License:
				hErr = SVC_META_SetDRMInfo(pstContext->ulContentId, pstDrmInfo, TRUE);
				if (hErr != ERR_OK)
				{
					HxLOG_Error("SVC_META_SetDRMInfo() ERROR.\n");
					return hErr;
				}
				break;
			case eDxCopyrightControl_CiPlusDrmType_ParentalCtrl:
				{
					// Patrental Control �� ��� DRM Info�� timestamp�� ����Ǿ�� �ϴ� �ð��� UNIX time �������� ����ִµ�,
					// �� ���� timestamp ���� �ð����� �����ؼ� ������ ��.
					pstDrmInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusParentalCtrlInfo.utTimeStamp -= pstContext->stMetaInfo.ulStartTime;

					hErr = SVC_REC_SetDRMInfo(pstContext->hAction, pstDrmInfo, TRUE);
					if (hErr != ERR_OK)
					{
						HxLOG_Error("SVC_REC_SetDRMInfo() ERROR.\n");
						return hErr;
					}
					break;
				}
			default:
				HxLOG_Error("If this message is printed, check the routine!!\n");
				return ERR_FAIL;
		}

		HxLOG_Info("DRM is saved !!! \n");
	}
	else
	{
		HxLOG_Error("If this message is printed, check the routine!!\n");
	}

	return ERR_OK;
}
STATIC HERROR xmgr_rec_CiPlusOperateCiplusUriDrm_Base(xmgrRec_Context_t *pstContext, DxCopyrightControl_t *pstDrmInfo)
{
	SvcMeta_Record_t 	stRecInfo;
	HERROR 				hErr = ERR_FAIL;

	if(pstContext->stNewDrmInfo.stCasCcInfo.eCasCcType == eDxCopyrightControl_CasCcType_CiPlus &&
		pstContext->stNewDrmInfo.stCasCcInfo.info.stCiPlusDrmInfo.eCiPlusDrmType == eDxCopyrightControl_CiPlusDrmType_Uri)
	{
		if (pstContext->stNewDrmInfo.stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ucMode >
									pstDrmInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ucMode)
		{
			HxLOG_Info("Although the saved URI at context is from CAM, new URI is NOT from CAM. So, ignore it...\n");
			return ERR_OK;
		}
	}

	// CI+ DRM�� ������ �̹� �޾Ƴ��� SI DRM Ȥ�� Ÿ CAS DRM�� ���̻� ��ȿ���� �ʵ��� ó�� ��.
	HxSTD_MemSet(&(pstContext->stNewDrmInfo), 0, sizeof(DxCopyrightControl_t));
	HxSTD_MemSet(&(pstContext->stMetaInfo.stDrmInfo), 0, sizeof(DxCopyrightControl_t));

	HxSTD_MemCopy(&(pstContext->stNewDrmInfo), pstDrmInfo, sizeof(DxCopyrightControl_t));
	HxSTD_MemCopy(&(pstContext->stMetaInfo.stDrmInfo), pstDrmInfo, sizeof(DxCopyrightControl_t));

	pstContext->stNewDrmInfo.bCasCc = TRUE;
	pstContext->stMetaInfo.stDrmInfo.bCasCc = TRUE;

	// CI+ content�� DRM ������ ��� ���� ����� encryption �Ͽ� �����ϵ��� ��.
	if(SVC_META_GetRecordInfo(pstContext->ulContentId, &stRecInfo) == ERR_OK)
	{
		if(stRecInfo.u1cEncryption == FALSE)
		{
			stRecInfo.u1cEncryption = TRUE;
			if(SVC_META_SetRecordInfo(pstContext->ulContentId, &stRecInfo) == ERR_OK)
			{
				pstContext->stMetaInfo.u1cEncryption = TRUE;
			}
			else
			{
				HxLOG_Error("ERROR... SVC_META_SetRecordInfo()...\n");
			}
		}
	}
	else
	{
		HxLOG_Error("ERROR... SVC_META_GetRecordInfo()...\n");
	}

	pstContext->bCasDrmValid = TRUE;
	pstContext->bCasDrmNone = FALSE;
	xmgr_rec_ClearWaitFlag(pstContext,eRecWait_DRM);

	hErr = xmgr_rec_CiPlusOperateCiplusDrmProcess_Base(pstContext, pstDrmInfo);

	return hErr;
}

STATIC HERROR xmgr_rec_CiPlusOperateCiplusRecCtrlDrm_Base(xmgrRec_Context_t *pstContext, DxCopyrightControl_t *pstDrmInfo)
{
	HERROR hErr = ERR_FAIL;
#if defined(CONFIG_MW_CI_PLUS_CC_V_2)		//Recording ���� CI DRM ������ ó��
	DxCopyrightControl_CiPlus_t 	*pstCiPlusDrmInfo;

	pstCiPlusDrmInfo = &pstDrmInfo->stCasCcInfo.info.stCiPlusDrmInfo;

	if(pstContext->eRecState == eMgrRecState_WAIT)	// �Ϲ����� REC �ó����� �϶����� �� �κ��� �ַ� Ż ����.
	{
		switch(pstCiPlusDrmInfo->ciplusDrmInfo.stCiPlusRecCtrl.eCiPlusCcRecInfo)
		{
			case eDxCopyrightControl_CiPlusCcRecInfo_RecordOk:
				pstContext->bEnableCcRecording = TRUE;
				break;
			case eDxCopyrightControl_CiPlusCcRecInfo_RecordWait:
				pstContext->bEnableCcRecording = FALSE;
				break;
			case eDxCopyrightControl_CiPlusCcRecInfo_RecordStart:
				pstContext->bEnableCcRecording = TRUE;
				break;
			case eDxCopyrightControl_CiPlusCcRecInfo_RecordError:
				// TODO: �ش� ��ȭ content metafile�� ��ȭ fail ������ ���ܵΰ� Recording�� ���� ��Ŵ.
				// Metafile�� ����ϰ�, ���߿� playback �õ� �� �� ������ ���� ��ȭ ���� ��Ȳ�� ���� message ��� �ʿ�
				pstContext->bEnableCcRecording = FALSE;

				// TODO: recording �ߴ� �޽��� ǥ���ϴ� �κ��� �ʿ�
				//pstContext->bDestroybyDrm = TRUE;
				MGR_RECORDER_SetRecordFailReason(pstContext->ulContentId, eMgrRecFailed_DRM);
				BUS_SendMessage (NULL, eMEVT_PVR_RECORD_STOP_REC, pstContext->hAction, 0, 0, 0);
				break;
			default:
				HxLOG_Error("If this message is printed, check the routine!!\n");
				return ERR_FAIL;
		}
	}
	else if (pstContext->eRecState == eMgrRecState_RECORDING)	// �� ���� �̹� Recording�� ���� �� ��Ȳ���� CI Rec status�� �ٽ� �ö���� ���. �ַ� scrambled Rec ���¿��� CAM ���� ���� ���� �ο��� �Ǿ� descramble Rec���� �ٲ� �� �̷��� ������ ��Ÿ��.
	{
		HxLOG_Info("REC Status is received during Record an content...\n");
		// TODO: �̷� ���¿� ���� �ó����� ���ϰ� ���� �ʿ�. Ư�� CI+ v1.3 �� ���� contents�� ���� ��� �ʿ�. (kkkim)

		switch(pstCiPlusDrmInfo->ciplusDrmInfo.stCiPlusRecCtrl.eCiPlusCcRecInfo)
		{
			case eDxCopyrightControl_CiPlusCcRecInfo_RecordError: // v1.3 Test Ext : 5.1.2.2.
				pstContext->bEnableCcRecording = FALSE;

				pstContext->bDestroybyDrm = TRUE;
				MGR_RECORDER_SetRecordFailReason(pstContext->ulContentId, eMgrRecFailed_DRM);
				BUS_SendMessage (NULL, eMEVT_PVR_RECORD_STOP_REC, pstContext->hAction, 0, 0, 0);
		}
		return ERR_OK;
	}
	else
	{
		HxLOG_Error("ERROR: Wrong eRecState[%02x] ccRecInfo[%02x]\n",
			pstContext->eRecState,
			pstCiPlusDrmInfo->ciplusDrmInfo.stCiPlusRecCtrl.eCiPlusCcRecInfo);
		return ERR_FAIL;
	}

	hErr = xmgr_rec_CiPlusOperateCiplusDrmProcess_Base(pstContext, pstDrmInfo);
#endif
	return hErr;
}

// return�� ERR_OK�̸�, CAM�� ã����. ERR_FAIL�̸� ��ȭ�� ����� CAM�� ���ԵǾ� ���� ����.
STATIC HERROR xmgr_rec_CiPlusGetSlotIdOfBindedCam_Base(HUINT8 *pucBindedCamId, HUINT16 *pusSlotId)
{
	HUINT32						unCamSlotNum;
	SvcCas_CtrlParam_t			stParam;
	CI_GetBindedCamIdInParam_t	stInParam;
	CI_GetBindedCamIdOutParam_t	stOutParam;
	HUINT8						aucBindedCamId[8];
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
			HxLOG_Error("SVC_CAS_CtrlGet() ERROR!\n");
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

STATIC BUS_Result_t xmgr_rec_CiPlusMsgEvtGoOperation(xmgrRec_Context_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 		ulContentNum = 0, idx = 0;
	HUINT32 		*pulContentId = NULL;
	HERROR			hErr	= ERR_FAIL;
	DxCopyrightControl_t		stDrm;
	HUINT32						ucPinEventRating  = 20;		// rating max ���� ū������, �̷��� �ؾ� pc���� ���� ���� record stop ���� �ʴ´�.

	SvcCas_CtrlParam_t					stParam;
	CI_GetPinCapabilityInParam_t		stPinCapaInParam;
	CI_GetPinCapabilityOutParam_t		stPinCapaOutParam;
	SvcCas_CiPlusPinCapaInfo_t			stPinCapaInfo;
	HUINT16 							usSlotId;
	UNIXTIME							ulCurrentTime;

	HxSTD_memset( &stDrm, 0, sizeof(DxCopyrightControl_t) );

	HxLOG_Print("++++++++++++++++++++++++++++++\n");

	hErr = (HERROR)VK_CLOCK_GetTime((unsigned long*)&ulCurrentTime);
	if(hErr != VK_OK)
	{
		HxLOG_Error ("VK_CLOCK_GetTime() Fail ! \n");
		return ERR_FAIL;
	}

	hErr = SVC_META_GetDRMInfoByTime( pstContext->ulContentId, eDxCopyrightControl_CiPlusDrmType_ParentalCtrl, ulCurrentTime, &stDrm );
	if( hErr != ERR_OK )
	{
		HxLOG_Error ("SVC_META_GetDRMInfoByTime() fail\n");
	}
	else
	{
		HxLOG_Print("\t ucAgeLimit:0x%x\n", stDrm.stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusParentalCtrlInfo.ucAgeLimit);
	}

	hErr = xmgr_rec_CiPlusGetSlotIdOfBindedCam_Base(stDrm.stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusParentalCtrlInfo.aucBindedCamId, &usSlotId);
	if(hErr == ERR_OK)
	{
		HxLOG_Print("\t Using CAM Slot ID: [%02x]\n", usSlotId);

		stPinCapaInParam.usSlotId = usSlotId;
		stParam.pvIn = (void *)&stPinCapaInParam;
		stParam.pvOut = (void *)&stPinCapaOutParam;
		stParam.ulControlType = eCACTRL_CI_GetPinCapa;

		hErr = SVC_CAS_CtrlGet(eDxCAS_GROUPID_CAM, eCACTRL_CI_GetPinCapa, &stParam);
		if (hErr == ERR_FAIL)
		{
			HxLOG_Error("ERROR: SVC_CAS_CtrlGet => eCACTRL_CI_GetPinCapa...\n");
		}
		else
		{
			HxSTD_memcpy(&stPinCapaInfo, &stPinCapaOutParam.stPinCapaInfo, sizeof(SvcCas_CiPlusPinCapaInfo_t));
			ucPinEventRating = stPinCapaInfo.ucRating;
			HxLOG_Print("\t Rating = 0x%x \n", ucPinEventRating);
		}
	}

	if (ucPinEventRating < stDrm.stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusParentalCtrlInfo.ucAgeLimit)
	{
		/*
		  *	6.6 ���� ���� ���
		  *	step 13. ���� ��� ����, fail �� else �ڵ带 ����غ���.
		  */
#if 1
		// if the receiver shows any AV on waking from standby
		HxLOG_Print("\t >> set timer (4sec)...stop recording in 4 sec...\n");
		BUS_SetTimer(CIPLUS_RATING_REC_STOP_TIMER_ID, CIPLUS_RATING_REC_STOP_TIME_OUT);
#else
		// if the receiver does not show any AV on waking from standby

		HxLOG_Print("\t >> stop recording...\n");

		hErr = MGR_RECORDER_SetRecordFailReason(context->ulContentId, eREC_FAILED_DRM);
		BUS_SendMessage (NULL, eMEVT_PVR_RECORD_STOP_REC, pstContext->hAction, 0, 0, 0);
#endif
	}

	return ERR_OK;
}

BUS_Result_t	xmgr_rec_CiPlusMsgEvtCasDrmSignalled_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	/*CAS DRM ���� Ȥ�� ����, ���⼭�� CI+ �� ó���Ѵ�. op������ ������ ó���Ѵ�.*/
	HERROR				hErr = ERR_FAIL;

// CI+ DRM ���ۿ� ���� CAS MGR �������� �ٸ��� �����ϵ��� ������ ����.
// CAS MGR�� ���� ������ ���� OCTO2�� ����ϴ� ���� HMS, NGS, GEN2 �ε�, HMS�� ver2�� ��������, NGS GEN2�� ���� mgr�� �۾��� �ϰ� �ֱ� ������.
// (2012.11.09. ����) GEN2�� tagging ���� ���� ��Ȳ�̰�, NGS�� phase 2�� MGR2�� �� �����ε�,
// �̶� NGS�� �� �Ѿ���� CAS MGR 2�� ���ܵΰ� �������� �� �����ϵ��� �Ѵ�.

	DxCopyrightControl_t			stDrmInfo;
	HINT8				ucDrmInfoVer;

	if (eMgrRecState_NO_SIGNAL == pstContext->eRecState)
	{
		return MESSAGE_PASS;
	}

	HLIB_STD_MemCpySafe(&stDrmInfo, sizeof(DxCopyrightControl_t), (void *)p3, sizeof(DxCopyrightControl_t));

	ucDrmInfoVer = stDrmInfo.stCasCcInfo.info.stCiPlusDrmInfo.ucCiPlusDrmInfoVer;

	if (eDxCopyrightControl_CasCcType_CiPlus != stDrmInfo.stCasCcInfo.eCasCcType)
	{
		// DRM Type �� CI Plus �� �ƴ϶�� ó������ �ʴ´�.
		return ERR_BUS_NO_ERROR;
	}

	switch (stDrmInfo.stCasCcInfo.info.stCiPlusDrmInfo.eCiPlusDrmType)
	{
		case eDxCopyrightControl_CiPlusDrmType_Uri:
			hErr = xmgr_rec_CiPlusOperateCiplusUriDrm_Base(pstContext, &stDrmInfo);
			break;
		case eDxCopyrightControl_CiPlusDrmType_RecCtrl:
			hErr = xmgr_rec_CiPlusOperateCiplusRecCtrlDrm_Base(pstContext, &stDrmInfo);
			break;
		case eDxCopyrightControl_CiPlusDrmType_License:
		case eDxCopyrightControl_CiPlusDrmType_ParentalCtrl:
			if(pstContext->eRecState == eMgrRecState_RECORDING)
			{
				hErr = xmgr_rec_CiPlusOperateCiplusDrmProcess_Base(pstContext, &stDrmInfo);
			}
			else
			{
				HxLOG_Error("During RECORDING!! CI+ License Data Can be Saved!\n");
			}
			break;
		default:
			break;
	}



	NOT_USED_PARAM(pstContext);
	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t	xmgr_rec_CiPlusMsgEvtCasOK_Base ( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Trace();
	HxLOG_Debug("p1=%x, p2=%x, p3=%x\n", p1, p2, p3);

	/* context update */
	{
		pstContext->bCasOk = TRUE;

		if( 0 != p1 )	pstContext->nCasErrCode = p1;
		else			pstContext->nCasErrCode = 0;

#if defined(CONFIG_RECORD_PAUSE)
		// CI Only ���� �ƴ� ���, CAS �� Descramble ���� ���ϴ� ��� Record Pause �� �� ����̹Ƿ� Record Resume ���� ����� �Ѵ�.
		if(pstContext->eRecState == eMgrRecState_BLOCKED)
		{
			xmgr_rec_ClearPauseFlag(pstContext,eRecPause_CAS);
		}
#endif
	}

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t	xmgr_rec_CiPlusMsgEvtCasFail_Base ( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Trace();
	HxLOG_Debug("p1=%x, p2=%x, p3=%x\n", p1, p2, p3);

	if (eMgrRecState_NO_SIGNAL == pstContext->eRecState)
	{
		return MESSAGE_PASS;
	}

	/* context update */
	{
		pstContext->nCasErrCode = p1;
		pstContext->bCasOk = FALSE;
	}

#if defined(CONFIG_RECORD_PAUSE)
	/* CI Only ���� �ƴ� ���, CAS �� Descramble ���� ���ϴ� ��� Record Pause*/
	if(pstContext->eRecState == eMgrRecState_RECORDING)
	{
		if(pstContext->bRecordStart == FALSE)
		{
			xmgr_rec_StartRec(pstContext);
			pstContext->bRecordStart = TRUE;
		}
		xmgr_rec_SetPauseFlag(pstContext, eRecPause_CAS);
	}
#endif

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t	xmgr_rec_CiPlusMsgEvtCasDrmUnknown_Base ( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	/* CAS DRM �� �� �� ���ٴ� �޽�����, scramble recording �� �����Ѵ�. */
	HxLOG_Trace();
	HxLOG_Debug("p1=%x, p2=%x, p3=%x\n", p1, p2, p3);

	if (eMgrRecState_NO_SIGNAL == pstContext->eRecState)
	{
		return MESSAGE_PASS;
	}

	// DRM �� �� �� �����Ƿ� scramble recording �����Ѵ�.
	if(pstContext->eRecState == eMgrRecState_WAIT)
	{
		xmgr_rec_ClearWaitFlag(pstContext,eRecWait_DRM);

		/* start the recorder */
		xmgr_rec_StartRec(pstContext);
		return MESSAGE_BREAK;
	}

	NOT_USED_PARAM(pstContext);
	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t	xmgr_rec_CiPlusMsgEvtCasDrmNone_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	/* CAS DRM �� ���ٴ� �޽���, ���⼭�� CI+ �� ó���Ѵ�. op�⿡�� ó���Ѵ�. */
	HxLOG_Trace();
	HxLOG_Debug("p1=%x, p2=%x, p3=%x\n", p1, p2, p3);

	if (eMgrRecState_NO_SIGNAL == pstContext->eRecState)
	{
		return MESSAGE_PASS;
	}

	// DRM ������ �����Ƿ� recording �����Ѵ�.
	if(pstContext->eRecState == eMgrRecState_WAIT)
	{
		xmgr_rec_ClearWaitFlag(pstContext,eRecWait_DRM);

		/* start the recorder */
		xmgr_rec_StartRec(pstContext);
		return MESSAGE_BREAK;
	}

	NOT_USED_PARAM(pstContext);
	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return ERR_BUS_NO_ERROR;
}


BUS_Result_t	xmgr_rec_CiPlusMsgEvtCiplusDrmInfoConsumed_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR									hErr;
	SvcCas_CtrlParam_t						stParam;
	CI_NotifyDrmInfoConsumedInParam_t		stInParam;
	HUINT8									ucDrmInfoVer = (HUINT8)p2;
	HERROR									hConsumedStatus = (HERROR)p1;

	HxLOG_Trace();
	HxLOG_Debug("p1=%x, p2=%x, p3=%x\n", p1, p2, p3);

	stParam.ulControlType		= eCACTRL_CI_NotifyDrmInfoConsumed;
	stInParam.ucDrmInfoVer		= ucDrmInfoVer;
	stInParam.hConsumedStatus	= hConsumedStatus;

	stParam.pvIn = (void *)&stInParam;
	hErr = SVC_CAS_CtrlNoti(eDxCAS_GROUPID_CAM, eCACTRL_CI_NotifyDrmInfoConsumed, &stParam);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("[%s:%d] SVC_CAS_CtrlNoti() ERROR!!\n",  __FUNCTION__, __LINE__);
	}
	return MESSAGE_BREAK;
}
#if defined (CONFIG_MW_MM_AIT_REC)
STATIC void xmgr_rec_CiPlusFreeAitPidList (void *pvData)
{
#if defined(CONFIG_MW_SI_AIT)
	HUINT32		*pulAitPid = (HUINT32 *)pvData;

	if (NULL != pulAitPid)
	{
		HLIB_STD_MemFree(pulAitPid);
	}
#endif

	return;
}
#endif
BUS_Result_t xmgr_rec_CiPlusMsgEvtSiPmtReceived_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR						 hErr, hResult = MESSAGE_PASS;
	HUINT32 					 ulTempBufSize = 0;
	HUINT8						*pucRawSectionBuffer = NULL;
	DbSvc_Info_t				*pstSvcInfo = NULL, *pstOrgSvcInfo = NULL, *pstDstSvcInfo = NULL;
	SvcMeta_Service_t			*pstMetaSvc = NULL, *pstOrgMetaSvc = NULL;
	SvcRec_Setup_t				*pstSvcRecSetup = NULL;
	SvcSi_MakeSectionSpec_t 	 stMakeSecSpec;
	HBOOL						 bChangedVideoCodec = FALSE, bChangedPid = FALSE;
	HUINT16 					 usSvcId = 0;
	SvcMeta_SbtlInfoList_t		 stSbtlInfoList;

	HxLOG_Trace();

	pucRawSectionBuffer = (HUINT8 *)OxMGR_Calloc(4096);
	pstSvcInfo = (DbSvc_Info_t *)OxMGR_Calloc(sizeof(DbSvc_Info_t));
	pstOrgSvcInfo = (DbSvc_Info_t *)OxMGR_Calloc(sizeof(DbSvc_Info_t));
	pstDstSvcInfo = (DbSvc_Info_t *)OxMGR_Calloc(sizeof(DbSvc_Info_t));
	pstMetaSvc = (SvcMeta_Service_t *)OxMGR_Calloc(sizeof(SvcMeta_Service_t));
	pstOrgMetaSvc = (SvcMeta_Service_t *)OxMGR_Calloc(sizeof(SvcMeta_Service_t));
	pstSvcRecSetup = (SvcRec_Setup_t *)OxMGR_Calloc(sizeof(SvcRec_Setup_t));

	if (!pucRawSectionBuffer || !pstSvcInfo || !pstOrgSvcInfo || !pstDstSvcInfo || !pstMetaSvc || !pstOrgMetaSvc || !pstSvcRecSetup)
	{
		HxLOG_Error("memory allocation failed:\n");
		goto END_FUNC;
	}


	// xmgr_live_view_base.c XMGR_LiveView_BASE_EvtSiPmt ���� ���� ������Ʈ ���ָ� ��������, Ÿ�̹������� eSEVT_SI_PMT �̺�Ʈ�� ���⼭ ���� ���� �� �־�,
	// �ѹ� �ܵ��� �Ѵ�...�ٸ�, Recording Module���� SVC DB�� ������Ʈ�ϴ� �� ��ŭ �̻��� �� �Ͽ�, Live �ʿ��� ó���ϰ� ���⼱ ���� �����ͼ� ���ڵ� ���� ó���� �Ѵ�...
	hErr = DB_SVC_GetServiceInfo(pstContext->stSetup.hCurrSvc, pstOrgSvcInfo);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("Fail to Get Service Info (0x%x)\n", pstContext->stSetup.hCurrSvc);
		goto END_FUNC;
	}

	hErr = SVC_META_GetServiceInfo(pstContext->ulContentId, pstMetaSvc);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("SVC_META_GetServiceInfo err: usSvcId(%d)\n", pstContext->ulContentId);
	}

	HxSTD_MemCopy(pstOrgMetaSvc, pstMetaSvc, sizeof(SvcMeta_Service_t));
	HxSTD_MemCopy(&stSbtlInfoList, &(pstMetaSvc->stSbtlInfoList), sizeof(SvcMeta_SbtlInfoList_t));

	/*update Video Codec*/
	hErr = DB_SVC_GetServiceIdFromSvcHandle (pstContext->stSetup.hCurrSvc, &usSvcId);
	if (hErr == ERR_OK)
	{
		hErr = SVC_SI_GetLiveServiceInfoPid (pstContext->hAction, (HUINT32)usSvcId, pstOrgSvcInfo, pstDstSvcInfo, &bChangedPid);

		HxLOG_Info("pstOrgSvcInfo!=pstDstSvcInfo,  bChangedPid: %d \n", bChangedPid);
		HxLOG_Info("org video PID: 0x%x, audio PID: 0x%x, usTtxPid: 0x%x,  ucSubttlIdx: 0x%x,  usPcrPid: 0x%x\n",
					DbSvc_GetVideoPid(pstOrgSvcInfo), DbSvc_GetAudioPid(pstOrgSvcInfo), DbSvc_GetTtxPid(pstOrgSvcInfo), DbSvc_GetSubttlIdx(pstOrgSvcInfo), DbSvc_GetPcrPid(pstOrgSvcInfo));

		HxLOG_Info("dst video PID: 0x%x, audio PID: 0x%x, usTtxPid: 0x%x,  ucSubttlIdx: 0x%x,  usPcrPid: 0x%x\n",
					DbSvc_GetVideoPid(pstDstSvcInfo), DbSvc_GetAudioPid(pstDstSvcInfo), DbSvc_GetTtxPid(pstDstSvcInfo), DbSvc_GetSubttlIdx(pstDstSvcInfo), DbSvc_GetPcrPid(pstDstSvcInfo));
		HxLOG_Debug("VideoCodec Org(%d), Dst(%d) \n", DbSvc_GetVideoCodec(pstOrgSvcInfo), DbSvc_GetVideoCodec(pstDstSvcInfo));

		if (ERR_OK == hErr)
		{
			HxLOG_Debug("VideoCodec Meta Org(%d), Dst(%d) \n", pstMetaSvc->eVideoCodec, DbSvc_GetVideoCodec(pstDstSvcInfo));
			if (pstMetaSvc->eVideoCodec != DbSvc_GetVideoCodec(pstDstSvcInfo))
			{
				HxLOG_Debug("VideoCodec Chaneged !\n");
				bChangedVideoCodec = TRUE;
			}
		}
		else
		{
			HxLOG_Error ("SVC_SI_GetLiveServiceInfoPid err: usSvcId(%d)\n", usSvcId);
		}
	}
	else
	{
		HxLOG_Error("DB_SVC_GetServiceIdFromSvcHandle() err \n");
	}

	if (TRUE == bChangedVideoCodec)
	{
		MGR_RECORDER_SetRecordSetup(&pstContext->stSetup, pstSvcRecSetup, &pstContext->stMetaInfo, pstContext->hAction);
		pstSvcRecSetup->eSvcType = DbSvc_GetSvcType(pstDstSvcInfo);
		pstSvcRecSetup->eVideoCodec = DbSvc_GetVideoCodec(pstDstSvcInfo);
		hErr = SVC_REC_UpdateVideoCodec(pstContext->hAction, pstSvcRecSetup, TRUE);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("SVC_REC_UpdateVideoCodec. Error\n");
		}
		HxSTD_MemCopy (pstSvcInfo, pstDstSvcInfo, sizeof(DbSvc_Info_t));
	}
	else if ((DbSvc_GetVideoPid(pstOrgSvcInfo) != DbSvc_GetVideoPid(pstDstSvcInfo)) ||
			(DbSvc_GetPcrPid(pstOrgSvcInfo) != DbSvc_GetPcrPid(pstDstSvcInfo)) ||
			(DbSvc_GetCasType(pstOrgSvcInfo) != DbSvc_GetCasType(pstDstSvcInfo)))
	{
		HxSTD_MemCopy (pstSvcInfo, pstDstSvcInfo, sizeof(DbSvc_Info_t));
	}
	else
	{
		HxSTD_MemCopy (pstSvcInfo, pstOrgSvcInfo, sizeof(DbSvc_Info_t));
	}

	/* pid start */
	pstContext->stRecPidInfo.stKnown.stSI[eREC_SiPAT].usPid =	PAT_PID;
	// pstContext->stRecPidInfo.stKnown.stSI[eREC_SiCAT].usPid	=	CAT_PID;
	pstContext->stRecPidInfo.stKnown.stSI[eREC_SiSDT].usPid =	SDT_PID;
	pstContext->stRecPidInfo.stKnown.stSI[eREC_SiPMT].usPid =	DbSvc_GetPmtPid(pstSvcInfo);
	pstContext->stRecPidInfo.stKnown.stSI[eREC_SiPCR].usPid =	DbSvc_GetPcrPid(pstSvcInfo);
	pstContext->stRecPidInfo.stKnown.stVideo[0].usPid		=	DbSvc_GetVideoPid(pstSvcInfo);
	/* replace PAT */
	{
		SvcSi_MakeSecArg_t	unArg;
		HINT32	ulRawSectionSize = 0;

		unArg.eSiType			=	eSIxTABLETYPE_Pat;
		unArg.stPat.eSiType 	=	eSIxTABLETYPE_Pat;
		unArg.stPat.usTsId		=	DbSvc_GetTsId(pstSvcInfo);
		unArg.stPat.usSvcId 	=	DbSvc_GetSvcId(pstSvcInfo);
		unArg.stPat.usPmtPid	=	DbSvc_GetPmtPid(pstSvcInfo);

		hErr = MGR_RECORDER_GetMakeSectionSiSpec(&stMakeSecSpec);
		if (hErr == ERR_OK)
		{
			hErr = SVC_SI_MakeSiSection(pstContext->ulActionId, &stMakeSecSpec, &unArg, pucRawSectionBuffer, &ulRawSectionSize);
		}

		if ((ERR_OK == hErr) && (ulRawSectionSize > 0))
		{
			SVC_REC_ReplacePSI(pstContext->hAction, PAT_PID, pucRawSectionBuffer, ulRawSectionSize);
		}
		else
		{
			HxLOG_Error ("SVC_SI_MakeSiSection: siType:PAT, ulActionId(%d)\n", pstContext->ulActionId);
		}
	}

	{
		HINT32 i=0;
		HINT32 nMinTrack=0;

		SvcSi_AudioList_t		stAudioList;
		SvcSi_SubtitleList_t	stSubtitleList;
		SvcSi_EcmList_t			stEcmList;

		/*audio*/
		HxSTD_MemSet(&stAudioList, 0, sizeof(SvcSi_AudioList_t));
		SVC_REC_CleanReqPidDes(pstContext->stRecPidInfo.stKnown.stAudio, SvcSi_MAX_AUDIO_ES_NUM);
		hErr = SVC_SI_GetAudioList(pstContext->ulActionId, DbSvc_GetSvcId(pstSvcInfo), &stAudioList);
		if (ERR_OK == hErr)
		{
			nMinTrack = (stAudioList.nAudio < SvcSi_MAX_AUDIO_ES_NUM)? stAudioList.nAudio : SvcSi_MAX_AUDIO_ES_NUM;
			for (i = 0 ; i < nMinTrack ; i++)
			{
				pstContext->stRecPidInfo.stKnown.stAudio[i].usPid = stAudioList.pstAudioInfo[i].usPid;
			}

			SVC_SI_FreeAudioList(&stAudioList); //free
		}


		/*subtitle*/
		HxSTD_MemSet(&stSubtitleList, 0, sizeof(SvcSi_SubtitleList_t));
		SVC_REC_CleanReqPidDes(pstContext->stRecPidInfo.stKnown.stSubtitle, SvcSi_MAX_SUBTTL_ES_NUM);
		hErr = SVC_SI_GetSubtitleList(pstContext->ulActionId, DbSvc_GetSvcId(pstSvcInfo), &stSubtitleList);
		if (ERR_OK == hErr)
		{
			nMinTrack = (stSubtitleList.ulSubtitleNum < SvcSi_MAX_SUBTTL_ES_NUM) ? stSubtitleList.ulSubtitleNum : SvcSi_MAX_SUBTTL_ES_NUM;
			for (i = 0; i < nMinTrack; i++)
			{
				pstContext->stRecPidInfo.stKnown.stSubtitle[i].usPid = stSubtitleList.pstSubtitleInfo[i].usPid;
			}

			xmgr_rec_SetSubtitleInfo(&stSubtitleList, &stSbtlInfoList);
			SVC_SI_FreeSubtitleList(&stSubtitleList);	//free
		}

		/*ttx*/
		pstContext->stRecPidInfo.stKnown.stTeletext[0].usPid = DbSvc_GetTtxPid(pstSvcInfo);

		/*super CC*/
		HxSTD_MemSet(&stSubtitleList, 0, sizeof(SvcSi_SubtitleList_t));
		SVC_REC_CleanReqPidDes(pstContext->stRecPidInfo.stKnown.stCaption, SvcSi_MAX_CAPTION_ES_NUM);
		hErr = SVC_SI_GetSuperCcList(pstContext->ulActionId, DbSvc_GetSvcId(pstSvcInfo), &stSubtitleList);
		if (ERR_OK == hErr)
		{
			nMinTrack = (stSubtitleList.ulSubtitleNum < SvcSi_MAX_CAPTION_ES_NUM) ? stSubtitleList.ulSubtitleNum : SvcSi_MAX_CAPTION_ES_NUM;
			for (i = 0; i < nMinTrack; i++)
			{
				pstContext->stRecPidInfo.stKnown.stCaption[i].usPid = stSubtitleList.pstSubtitleInfo[i].usPid;
			}

			SVC_SI_FreeSuperCcList(&stSubtitleList);	//free
		}

		/*ECM*/
		HxSTD_MemSet(&stEcmList, 0, sizeof(SvcSi_EcmList_t));
		SVC_REC_CleanReqPidDes(pstContext->stRecPidInfo.stKnown.stEcm, SvcSi_MAX_CA_PID_NUM);
		hErr = SVC_SI_GetEcmList(pstContext->ulActionId, DbSvc_GetSvcId(pstSvcInfo), &stEcmList);
		if (ERR_OK == hErr)
		{
			nMinTrack = (stEcmList.nEcm < SvcSi_MAX_CA_PID_NUM)? stEcmList.nEcm : SvcSi_MAX_CA_PID_NUM;
			for (i = 0; i < nMinTrack; i++)
			{
				pstContext->stRecPidInfo.stKnown.stEcm[i].usPid = stEcmList.pstEcmInfo[i].usPid;
			}

			SVC_SI_FreeEcmList(&stEcmList); //free
		}
	}
#if defined (CONFIG_MW_MM_AIT_REC)
	{
		HUINT16 	ulCount = 0;
		HUINT32 				 ulActionId = SVC_PIPE_GetActionId(pstContext->hAction);
		HxList_t				*pstAitList = NULL;
		HERROR					 hErr;

		hErr = SVC_SI_GetAitPidFromPMT(ulActionId, &pstAitList);
		if ((hErr != ERR_FAIL) && (pstAitList != NULL))
		{
			HxList_t	*pstAitItem = NULL;

			for (pstAitItem = pstAitList; pstAitItem != NULL; pstAitItem = pstAitItem->next)
			{
				HUINT32 *pulAitPid = (HUINT32 *)HLIB_LIST_Data(pstAitItem);
				if (pulAitPid != NULL)
				{
					/* Application type�� 0x10�� AIT���� �����Ѵ�. */
					pstContext->stRecPidInfo.stKnown.stSI[eREC_SiAIT].usPid = *pulAitPid;
				}
			}

			HLIB_LIST_RemoveAllFunc(pstAitList, xmgr_rec_FreeAitPidList);
		}

		for(ulCount = 0; ulCount < eSvcRec_MAX_REC_NUM; ulCount++)
		{
			HxLOG_Debug("[%d] PID: [0x%x] \n", ulCount, pstContext->stRecPidInfo.stKnown.stSI[ulCount].usPid);
		}
	}
#endif

	/* set pid �� pid info Ȯ�ο� */
	xmgr_rec_DebugPrintRecPidInfo_Base(&pstContext->stRecPidInfo, (HCHAR *)__FUNCTION__, __LINE__);

	if(pstContext->bPMTValid == TRUE)
	{
		hErr = SVC_REC_SetPID(pstContext->hAction, &pstContext->stRecPidInfo, TRUE);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("SVC_REC_SetPID err:\n");
		}

	}
	/* save meta */
	(void)SVC_META_MakeMetaSvcBySvcInfo(pstSvcInfo, pstMetaSvc);
	HxSTD_MemCopy(&(pstMetaSvc->stSbtlInfoList), &stSbtlInfoList, sizeof(SvcMeta_SbtlInfoList_t));

	/* DB changed or First PMT */
	if ((HxSTD_MemCmp(pstMetaSvc, pstOrgMetaSvc, sizeof(SvcMeta_Service_t)) != 0)
		|| (pstContext->bPMTValid == FALSE))
	{
		HxLOG_Info("Service Changed: Meta Write...\n");

		hErr = xmgr_rec_SetServiceTimeStamp(pstContext, pstMetaSvc, TRUE);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("xmgr_rec_SetServiceTimeStamp err:\n");
		}
	}

	/* xmgr_rec_EvtPvtPmtChanged */
	{
		SvcCas_SetPidInfo_t stCasPid;
		SvcCas_ActionType_e eActType;

		HxSTD_MemSet(&stCasPid, 0, sizeof(SvcCas_SetPidInfo_t));

		if(pstContext->stSetup.eRecType == eMgrRecType_SCR2DESC)
		{
			SVC_CAS_SetRecPid(pstContext->hAction, eCAS_ActionType_Descramble_a_File, &stCasPid);

			HxLOG_Trace();
			goto END_FUNC;
		}

		/* Record/Tsr_Rec �� PMT �� �����ϴ� ��� pid ���� CAS���� �Ǵ�. pid �� �˷��� �ʿ� ���� CAS�ܿ��� ���ϵ��� ���� �ʿ�*/
		if(pstContext->stSetup.eRecType == eMgrRecType_TSR) eActType = eCAS_ActionType_Tsr_Rec;
		else										eActType = eCAS_ActionType_Recording;

		if(DbSvc_GetCasType(pstSvcInfo) == eDxCAS_TYPE_FTA)
		{
			xmgr_rec_ClearWaitFlag(pstContext, eRecPause_DRM);
		}

		SVC_CAS_SetRecPid(pstContext->hAction, eActType, &stCasPid);
	}

	xmgr_rec_ClearWaitFlag(pstContext,eRecWait_PMT);

	/* start the recorder */

#if defined(CONFIG_PROD_ICORDPRO)
	xmgr_rec_StartRec(pstContext);
#endif

	/* save flag */
	if (FALSE == pstContext->bPMTValid)
		pstContext->bPMTValid = TRUE;

END_FUNC:
	if (pucRawSectionBuffer)			{ OxMGR_Free(pucRawSectionBuffer); }
	if (pstSvcInfo) 					{ OxMGR_Free(pstSvcInfo); }
	if (pstOrgSvcInfo)					{ OxMGR_Free(pstOrgSvcInfo); }
	if (pstDstSvcInfo)					{ OxMGR_Free(pstDstSvcInfo); }
	if (pstMetaSvc) 					{ OxMGR_Free(pstMetaSvc); }
	if (pstOrgMetaSvc)					{ OxMGR_Free(pstOrgMetaSvc); }
	if (pstSvcRecSetup) 				{ OxMGR_Free(pstSvcRecSetup); }

	return MESSAGE_PASS;
}


BUS_Result_t xmgr_rec_CiPlusProto_Base (xmgrRec_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t	eBusResult = ERR_BUS_NO_ERROR;
	HUINT32			ulActionId = SVC_PIPE_GetActionId(hAction);

	switch (message)
	{
		case eMEVT_BUS_CREATE:
			xmgr_rec_SetWaitFlag(pstContext,eRecWait_DRM);
			xmgr_rec_SetWaitFlag(pstContext,eRecWait_PMT);
			break;

		case eSEVT_SI_PMT:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:RecCiPlus(%d)] MESSAGE : eSEVT_SI_PMT\n", ulActionId);
			eBusResult = xmgr_rec_CiPlusMsgEvtSiPmtReceived_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_DRM_SIGNALLED:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:RecCiPlus(%d)] MESSAGE : eSEVT_CAS_DRM_SIGNALLED\n", ulActionId);
			eBusResult = xmgr_rec_CiPlusMsgEvtCasDrmSignalled_Base (pstContext, hAction, p1, p2, p3);
			break;
		case eSEVT_CAS_OK :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:RecCiPlus(%d)] MESSAGE : eSEVT_CAS_OK\n", ulActionId);
			eBusResult = xmgr_rec_CiPlusMsgEvtCasOK_Base (pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_FAIL :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:RecCiPlus(%d)] MESSAGE : eSEVT_CAS_FAIL\n", ulActionId);
			eBusResult = xmgr_rec_CiPlusMsgEvtCasFail_Base (pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_DRM_UNKNOWN:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:RecCiPlus(%d)] MESSAGE : eSEVT_CAS_DRM_UNKNOWN\n", ulActionId);
			eBusResult = xmgr_rec_CiPlusMsgEvtCasDrmUnknown_Base (pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_DRM_NONE:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:RecCiPlus(%d)] MESSAGE : eSEVT_CAS_DRM_NONE\n", ulActionId);
			eBusResult = xmgr_rec_CiPlusMsgEvtCasDrmNone_Base (pstContext, hAction, p1, p2, p3);
			break;
/*
		case EVT_PVR_CIPLUS_DRM_CONSUMED:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:RecCiPlus(%d)] MESSAGE : EVT_PVR_CIPLUS_DRM_CONSUMED\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtCiplusDrmInfoConsumed_CiPlus (pstContext, hAction, p1, p2, p3);
			break;
*/
/*
		#if defined(CONFIG_CI_PLUS_CERTIFICATION_ONLY)
		// �Ʒ� case�� standby ���� ��ȭ�߿� operation mode�� ��ȯ�Ҷ� CAM age rating �� timestamp�� �ֽ� PIN event �� agelimit�� ���Ͽ� ���ڰ��� �� ũ��
		// record stop �ϵ��� �ϱ� �����ε�, �ڼ��� ������ hms1000s ph1 branch �� rev.101045 �����Ͽ� ���� �ϵ���.
		case MSG_BUS_QUIT_STANDBY:
			HxLOG_Print("[XMGR:RecCiPlus(%d)] MESSAGE : MSG_BUS_QUIT_STANDBY:\n", ulActionId);
			eBusResult = xmgr_rec_CiPlusMsgEvtGoOperation(pstContext, p1, p2, p3);
			break;
		#endif
*/
		default :
			break;
	}

	return eBusResult;
}

#endif
