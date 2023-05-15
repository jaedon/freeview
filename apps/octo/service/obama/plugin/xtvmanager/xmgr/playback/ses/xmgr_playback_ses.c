
#if defined(CONFIG_OP_SES)
/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <octo_common.h>

#include <db_svc.h>

#include <isosvc.h>

#include <svc_pipe.h>
#include <svc_pb.h>

#include <bus.h>

#include <mgr_action.h>
#include <mgr_live.h>
#include <mgr_playback.h>
#include <_xmgr_playback.h>
#include <mgr_svclist.h>
#include <svc_av.h>
#include <xmgr_cas.h>
#include <mgr_copyrightcontrol.h>

#include "../base/local_include/_xmgr_playback_base.h"


#define _____PROTO_PROCEDURE_____

#if defined (CONFIG_DEBUG)
HUINT8 *xmgr_Playback_GetMessageName_Ses(HINT32 message);
STATIC void xmgr_pb_PrintDrm_Ses(DxCopyrightControl_t *pstDrmInfo, HCHAR *pcStr);
STATIC void xmgr_pb_PrintDrmInfo_Ses(xmgrPbContext_t *pstContext, HINT32 message);
#endif


STATIC HERROR xmgr_playback_NagraIsValidCasDrmInfo_Ses(DxCopyrightControl_t *pstDrmInfo)
{
	if(pstDrmInfo == NULL)
	{
		HxLOG_Error("pstDrmInfo is NULL\n");
		return ERR_FAIL;
	}

	if(pstDrmInfo->bCasCc == FALSE)
	{
		HxLOG_Error("bCasCc is error\n");
		return ERR_FAIL;
	}

	if(pstDrmInfo->stCasCcInfo.eCasCcType != eDxCopyrightControl_CasCcType_NagraSes)
	{
		HxLOG_Error("bCasCc : eDrmType is not eDxCopyrightControl_CasCcType_NagraSes\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR xmgr_playback_NagraIsValidSiDrmInfo_Ses(DxCopyrightControl_t *pstDrmInfo)
{
	if(pstDrmInfo == NULL)
	{
		HxLOG_Error("pstDrmInfo is NULL\n");
		return ERR_FAIL;
	}

	if(pstDrmInfo->bSiCc == FALSE)
	{
		HxLOG_Error("bSiCc is error\n");
		return ERR_FAIL;
	}

	if(pstDrmInfo->stSiCcInfo.eSiCcType != eDxCopyrightControl_SiCcType_Ses)
	{
		HxLOG_Error("bCasCc : eDrmType is not eDxCopyrightControl_CasCcType_NagraSes\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR xmgr_playback_NagraIsValidDrmInfo_Ses(DxCopyrightControl_t *pstDrmInfo)
{
	if(pstDrmInfo == NULL)
	{
		HxLOG_Error("pstDrmInfo is NULL\n");
		return ERR_FAIL;
	}

	HxLOG_Debug("bCasCc(%d), bSiCc(%d)\n", pstDrmInfo->bCasCc, pstDrmInfo->bSiCc);

	if(pstDrmInfo->bSiCc == TRUE)
	{
		if(pstDrmInfo->stSiCcInfo.eSiCcType != eDxCopyrightControl_SiCcType_Ses)
		{
			HxLOG_Error("bSiCc : eDrmType is not eDxCopyrightControl_SiCcType_Ses\n");
			return ERR_FAIL;
		}
	}

	if(pstDrmInfo->bCasCc == TRUE)
	{
		if(pstDrmInfo->stCasCcInfo.eCasCcType != eDxCopyrightControl_CasCcType_NagraSes)
		{
			HxLOG_Error("bCasCc : eDrmType is not eDxCopyrightControl_CasCcType_NagraSes\n");
			return ERR_FAIL;
		}
	}

	return ERR_OK;
}

DxCopyrightControl_CasCcType_e xmgr_playback_NagraGetCasDrmType_Ses(DxCopyrightControl_t *pstDrmInfo)
{
	if(pstDrmInfo == NULL)
	{
		HxLOG_Error("pstDrmInfo is NULL\n");
		return eDxCopyrightControl_CasCcType_None;
	}

	if(pstDrmInfo->bCasCc == FALSE)
	{
		HxLOG_Error("bCasCc is error\n");
		return eDxCopyrightControl_CasCcType_None;
	}

	return pstDrmInfo->stCasCcInfo.eCasCcType;
}

DxCopyrightControl_SiCcType_e xmgr_playback_NagraGetSiDrmType_Ses(DxCopyrightControl_t *pstDrmInfo)
{
	if(pstDrmInfo == NULL)
	{
		HxLOG_Error("pstDrmInfo is NULL\n");
		return eDxCopyrightControl_SiCcType_None;
	}

	if(pstDrmInfo->bSiCc == FALSE)
	{
		HxLOG_Error("bSiCc is error\n");
		return eDxCopyrightControl_SiCcType_None;
	}

	return pstDrmInfo->stSiCcInfo.eSiCcType;
}

STATIC HERROR xmgr_playback_NagraGetDrmInfo_Ses(DxCopyrightControl_t *pstDrmInfo, MgrCopyrightControl_MediaControlInfo_t *pstDetailDrmInfo)
{
	if(pstDrmInfo == NULL || pstDetailDrmInfo == NULL)
	{
		HxLOG_Error("pstDrmInfo is NULL\n");
		return ERR_FAIL;
	}

	HxSTD_memset(pstDetailDrmInfo, 0, sizeof(MgrCopyrightControl_MediaControlInfo_t));

	return MGR_COPYRIGHTCONTROL_GetMediaControlInfo(FALSE, pstDrmInfo, pstDetailDrmInfo);
}

#if 1//defined(CONFIG_DRM_TEST)
HBOOL xmgr_playback_GetPlaybackEnableStatusFromDrmInfo_Ses(xmgrPbContext_t *pstContext)
{
	UNIXTIME			ulEndTime = 0;
	UNIXTIME 			ulExpirationTime = 0;
	UNIXTIME 			ul1DayTime = 60*60*24;
	HxDATETIME_t	 	stTmpTime;
	HUINT32 			addHour = 0, addMin = 0, addSec = 0, addNextDayAM3Hour = 0, addTotalTime = 0;
	HUINT32 			curTime = 0;
	MgrCopyrightControl_MediaControlInfo_t stDetailDrmInfo;

	SvcMeta_Record_t stRecordInfo;

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return TRUE;
	}

	HxLOG_Debug("PbType = 0x%08x\n", pstContext->ePbType);

	if(eMgrPb_PlayType_Tsr == (pstContext->ePbType & 0x0000000f))
	{
		return TRUE;
	}

	if(SVC_META_GetRecordInfo (pstContext->stSetup.ulContentId, &stRecordInfo) == ERR_OK)
	{
		if(xmgr_playback_NagraIsValidDrmInfo_Ses(&stRecordInfo.stDrmInfo) == ERR_OK)
		{
			if(xmgr_playback_NagraGetDrmInfo_Ses(&stRecordInfo.stDrmInfo, &stDetailDrmInfo) == ERR_OK)
			{
				ulEndTime = stRecordInfo.ulEndTime;

				if(ulEndTime == 0xFFFFFFFF)
				{
					return TRUE;
				}

				switch(stDetailDrmInfo.eStorageRight)
				{
					case eDxCopyrightControl_SesStorageRight_ExpireSameDay:
						ulExpirationTime = ulEndTime; /* next day am3:00 까지 인지는 다시 확인 필요 */
						break;
					case eDxCopyrightControl_SesStorageRight_Expire3Day:
						ulExpirationTime = ulEndTime + ul1DayTime*3;	/* next day am3:00 까지 인지는 다시 확인 필요 */
						break;
					case eDxCopyrightControl_SesStorageRight_Expire7Day:
						ulExpirationTime = ulEndTime + ul1DayTime*7;	/* next day am3:00 까지 인지는 다시 확인 필요 */
						break;
					case eDxCopyrightControl_SesStorageRight_Expire60Day:
						ulExpirationTime = ulEndTime + ul1DayTime*60;	/* next day am3:00 까지 인지는 다시 확인 필요 */
						break;
					default:
						ulExpirationTime = 0;
						break;
				}
			}
		}
	}

	HxLOG_Debug("ulExpirationTime = 0x%08x, ulEndTime = 0x%08x\n", ulExpirationTime, ulEndTime);

	if(ulExpirationTime == 0 || ulEndTime == 0)
	{
		return TRUE;
	}

#if defined (CONFIG_DEBUG)
	HLIB_DATETIME_ConvertUnixTimeToDateTime(ulEndTime, &stTmpTime);

	HxLOG_Debug("===> ulEndTime unix(%ld) : ( %d.%d.%d) - (%d:%d:%d)\n", ulEndTime, stTmpTime.stDate.usYear,stTmpTime.stDate.ucMonth,stTmpTime.stDate.ucDay,stTmpTime.stTime.ucHour,stTmpTime.stTime.ucMinute,stTmpTime.stTime.ucSecond );
#endif
	HLIB_DATETIME_ConvertUnixTimeToDateTime(ulExpirationTime, &stTmpTime);

	HxLOG_Debug("===> ulExpirationTime unix(%ld) : ( %d.%d.%d) - (%d:%d:%d)\n", ulEndTime, stTmpTime.stDate.usYear,stTmpTime.stDate.ucMonth,stTmpTime.stDate.ucDay,stTmpTime.stTime.ucHour,stTmpTime.stTime.ucMinute,stTmpTime.stTime.ucSecond );


	/* ulExpirationTime 날짜 24시까지 */
	addHour = (24 - stTmpTime.stTime.ucHour -1)*60*60;
	addMin 	= (60 - stTmpTime.stTime.ucMinute -1)*60;
	addSec 	= 60 - stTmpTime.stTime.ucSecond;

#if defined (CONFIG_DEBUG)
	HLIB_DATETIME_ConvertUnixTimeToDateTime(ulExpirationTime+addHour+addMin+addSec, &stTmpTime);
	HxLOG_Debug("===> ulExpirationTime ( Until pm24:00 unix(%ld) : ( %d.%d.%d) - (%d:%d:%d)\n", ulEndTime, stTmpTime.stDate.usYear,stTmpTime.stDate.ucMonth,stTmpTime.stDate.ucDay,stTmpTime.stTime.ucHour,stTmpTime.stTime.ucMinute,stTmpTime.stTime.ucSecond );
#endif

	/* 유연하게 새벽 3시까지 */
	addNextDayAM3Hour = 3*60*60;

	/* 녹화 만료 날짜에서 다음날 새멱 3시까지 시청가능하도록 한다 */
	addTotalTime = addHour + addMin + addSec + addNextDayAM3Hour;

#if defined (CONFIG_DEBUG)
	HLIB_DATETIME_ConvertUnixTimeToDateTime(ulExpirationTime+addTotalTime, &stTmpTime);
	HxLOG_Debug("===> ulExpirationTime (Until next am3:00) unix(%ld) : ( %d.%d.%d) - (%d:%d:%d)\n", ulEndTime, stTmpTime.stDate.usYear,stTmpTime.stDate.ucMonth,stTmpTime.stDate.ucDay,stTmpTime.stTime.ucHour,stTmpTime.stTime.ucMinute,stTmpTime.stTime.ucSecond );
#endif

	curTime = HLIB_STD_GetSystemTime();

	/* 만료시간이 초과되어 더이상 play못함 */
	if(curTime >= (ulExpirationTime+addTotalTime))
	{
		return FALSE;
	}

	return TRUE;
}
#endif


BUS_Result_t xmgr_playback_NagraMsgEvtCasDrmSignalled_Ses(xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DxCopyrightControl_t	stDrmInfo;
	MgrCopyrightControl_MediaControlInfo_t	stDetailDrmInfo;

	HxLOG_Debug("\n");

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return MESSAGE_PASS;
	}

	HxSTD_MemSet(&stDrmInfo, 0, sizeof(DxCopyrightControl_t));
	HLIB_STD_MemCpySafe(&stDrmInfo, sizeof(DxCopyrightControl_t), (void *)p3, sizeof(DxCopyrightControl_t));

	if(xmgr_playback_NagraIsValidCasDrmInfo_Ses(&stDrmInfo) != ERR_OK)
	{
		HxLOG_Error("drmInfo is error\n");
		return MESSAGE_PASS;
	}

	if(xmgr_playback_NagraGetDrmInfo_Ses(&stDrmInfo, &stDetailDrmInfo) != ERR_OK)
	{
		HxLOG_Error("drmInfo is not valid\n");
		return MESSAGE_PASS;
	}

	return MESSAGE_PASS;
}

STATIC BUS_Result_t xmgr_playback_MsgBusCreate_Ses (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t 		eBusResult = MESSAGE_PASS;
	SvcMeta_Record_t 	stRecordInfo;
	DxCopyrightControl_CasCcType_e	 		eCasCcType = eDxCopyrightControl_CasCcType_None;
	DxCopyrightControl_SiCcType_e			eSiCcType = eDxCopyrightControl_SiCcType_None;

	HxLOG_Debug("\n");

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return MESSAGE_PASS;
	}

	HxSTD_MemSet(&stRecordInfo, 0, sizeof(SvcMeta_Record_t));

	if(SVC_META_GetRecordInfo (pstContext->stSetup.ulContentId, &stRecordInfo) != ERR_OK)
//	if(SVC_META_GetRecordInfo (pstContext->ulContentId, &stRecordInfo) != ERR_OK)
	{
		HxLOG_Error("SVC_META_GetRecordInfo is error\n");
	}

#if defined (CONFIG_DEBUG)
	xmgr_pb_PrintDrmInfo_Ses(pstContext, eMEVT_BUS_CREATE);
#endif

	HxLOG_Debug("Encryption Rec(%d), DRM CAS(%d), SI(%d)\n",stRecordInfo.u1cEncryption, stRecordInfo.stDrmInfo.bCasCc, stRecordInfo.stDrmInfo.bSiCc);

	// TODO : bCasCc or bSiCc가 true로 들어오는지 확인하자. 아래 소스 정리가 필요해 보임..
	// CAS를 우선순위로 먼저 check한다.
	if(stRecordInfo.stDrmInfo.bCasCc == TRUE)
	{
		eCasCcType = xmgr_playback_NagraGetCasDrmType_Ses(&stRecordInfo.stDrmInfo);
		if(eCasCcType == eDxCopyrightControl_CasCcType_NagraSes)
		{
#if defined(CONFIG_MW_CAS_NAGRA)
			eBusResult = xmgr_playback_NagraMsgBusCreate_Base(pstContext, hAction, p1, p2, p3);
#endif
		}
		else if(eCasCcType == eDxCopyrightControl_CasCcType_CiPlus)
		{
#if defined(CONFIG_MW_CI_PLUS)
			eBusResult = xmgr_playback_CiPlusMsgBusCreate_Base(pstContext, hAction, p1, p2, p3);
#endif
		}
		else
		{
			if(stRecordInfo.u1cEncryption == TRUE) // 어떤 연유로인해 drm이 없는데 encryption rec로 진행하는경우를 대비해 만들어 놓음.
			{
#if defined(CONFIG_MW_CAS_NAGRA)
				eBusResult = xmgr_playback_NagraMsgBusCreate_Base(pstContext, hAction, p1, p2, p3);
#endif
			}
			else
			{
#if defined(CONFIG_MW_CI_PLUS)
				eBusResult = xmgr_playback_CiPlusMsgBusCreate_Base(pstContext, hAction, p1, p2, p3);
#else
				eBusResult = xmgr_playback_MsgBusCreate_Base(pstContext, hAction, p1, p2, p3);
#endif
			}
		}
	}
	else
	{
		if(stRecordInfo.stDrmInfo.bSiCc == TRUE)
		{
			eSiCcType = xmgr_playback_NagraGetSiDrmType_Ses(&stRecordInfo.stDrmInfo);
			if(eSiCcType == eDxCopyrightControl_SiCcType_Ses)
			{
#if defined(CONFIG_MW_CAS_NAGRA)
				eBusResult = xmgr_playback_NagraMsgBusCreate_Base(pstContext, hAction, p1, p2, p3);
#endif
			}
			else
			{
				if(stRecordInfo.u1cEncryption == TRUE) // 어떤 연유로인해 drm이 없는데 encryption rec로 진행하는경우를 대비해 만들어 놓음.
				{
#if defined(CONFIG_MW_CAS_NAGRA)
					eBusResult = xmgr_playback_NagraMsgBusCreate_Base(pstContext, hAction, p1, p2, p3);
#endif
				}
				else
				{
#if defined(CONFIG_MW_CI_PLUS)
					eBusResult = xmgr_playback_CiPlusMsgBusCreate_Base(pstContext, hAction, p1, p2, p3);
#else
					eBusResult = xmgr_playback_MsgBusCreate_Base(pstContext, hAction, p1, p2, p3);
#endif
				}
			}
		}
		else
		{
			if(stRecordInfo.u1cEncryption == TRUE) // 어떤 연유로인해 drm이 없는데 encryption rec로 진행하는경우를 대비해 만들어 놓음.
			{
#if defined(CONFIG_MW_CAS_NAGRA)
				eBusResult = xmgr_playback_NagraMsgBusCreate_Base(pstContext, hAction, p1, p2, p3);
#endif
			}
			else
			{
#if defined(CONFIG_MW_CI_PLUS)
				eBusResult = xmgr_playback_CiPlusMsgBusCreate_Base(pstContext, hAction, p1, p2, p3);
#else
				eBusResult = xmgr_playback_MsgBusCreate_Base(pstContext, hAction, p1, p2, p3);
#endif
			}
		}
	}

	return eBusResult;
}

BUS_Result_t xmgr_playback_MsgEvtPbTimecodeChange_Ses (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t					eBusResult = ERR_BUS_NO_ERROR;
	SvcMeta_Record_t 				stRecordInfo;
	DxCopyrightControl_CasCcType_e	eCasCcType = eDxCopyrightControl_CasCcType_None;
	DxCopyrightControl_SiCcType_e	eSiCcType = eDxCopyrightControl_SiCcType_None;

	HxLOG_Debug("\n");

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return MESSAGE_PASS;
	}

#if defined (CONFIG_DEBUG)
	xmgr_pb_PrintDrmInfo_Ses(pstContext, eSEVT_PB_TIMECODE_CHANGE);
#endif

	HxSTD_MemSet(&stRecordInfo, 0, sizeof(SvcMeta_Record_t));

	if(SVC_META_GetRecordInfo (pstContext->stSetup.ulContentId, &stRecordInfo) != ERR_OK)
	{
		HxLOG_Error("SVC_META_GetRecordInfo is error\n");
	}

	eCasCcType = xmgr_playback_NagraGetCasDrmType_Ses(&stRecordInfo.stDrmInfo);

	HxLOG_Debug("eCasCcType(%d)\n", eCasCcType);

	if(eCasCcType != eDxCopyrightControl_CasCcType_None)
	{
		switch(eCasCcType)
		{
			case eDxCopyrightControl_CasCcType_NagraSes:
#if defined(CONFIG_MW_CAS_NAGRA)
				eBusResult = xmgr_playback_NagraMsgEvtPbTimecodeChange_Base (pstContext, hAction, p1, p2, p3);
#endif
				break;
			case eDxCopyrightControl_CasCcType_CiPlus:
#if defined(CONFIG_MW_CI_PLUS)
				eBusResult = xmgr_playback_CiPlusMsgEvtPbTimecodeChange_Base (pstContext, hAction, p1, p2, p3);
#endif
				break;
			default:
				eBusResult = xmgr_playback_MsgEvtPbTimecodeChange_Base(pstContext, hAction, p1, p2, p3);
				break;
		}
	}
#if defined(CONFIG_MW_CI_PLUS)
	else if(pstContext->stDrmInfo.bCasCc == TRUE && pstContext->stDrmInfo.stCasCcInfo.eCasCcType == eDxCopyrightControl_CasCcType_CiPlus)
	{
		// 구조적으로 DRM 정보는 pstContext에서 참고하여 처리되어야 하지만, SES에서는 항상 RecordInfo를 다시 읽어들여 처리하게 되어 있다.
		// 일단 CI+는 pstContext를 참조하도록 하고, 추후 SES code 정리 할 때 정상적인 구조로 수정이 필요 함.
		eBusResult = xmgr_playback_CiPlusMsgEvtPbTimecodeChange_Base (pstContext, hAction, p1, p2, p3);
	}
#endif
	else
	{
		eSiCcType = xmgr_playback_NagraGetSiDrmType_Ses(&stRecordInfo.stDrmInfo);

		HxLOG_Debug("eSiCcType(%d)\n", eSiCcType);

		switch(eSiCcType)
		{
			case eDxCopyrightControl_SiCcType_Ses:
#if defined(CONFIG_MW_CAS_NAGRA)
				eBusResult = xmgr_playback_NagraMsgEvtPbTimecodeChange_Base (pstContext, hAction, p1, p2, p3);
#endif
				break;
			default:
				eBusResult = xmgr_playback_MsgEvtPbTimecodeChange_Base(pstContext, hAction, p1, p2, p3);
				break;
		}
	}

	return eBusResult;
}

STATIC BUS_Result_t xmgr_playback_MsgEvtCasDrmSignalled_Ses(xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DxCopyrightControl_t	stDrmInfo;
	DxCopyrightControl_CasCcType_e			eCasCcType = eDxCopyrightControl_CasCcType_None;
	BUS_Result_t		eBusResult = ERR_BUS_NO_ERROR;

	HxLOG_Debug("\n");

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return MESSAGE_PASS;
	}

	HxSTD_MemSet(&stDrmInfo, 0, sizeof(DxCopyrightControl_t));
	HLIB_STD_MemCpySafe(&stDrmInfo, sizeof(DxCopyrightControl_t), (void *)p3, sizeof(DxCopyrightControl_t));

#if defined (CONFIG_DEBUG)
	xmgr_pb_PrintDrmInfo_Ses(pstContext, eSEVT_CAS_DRM_SIGNALLED);
#endif

	eCasCcType = xmgr_playback_NagraGetCasDrmType_Ses(&stDrmInfo);

	HxLOG_Debug("eCasCcType(%d)\n", eCasCcType);

	switch(eCasCcType)
	{
		case eDxCopyrightControl_CasCcType_NagraSes:
#if defined(CONFIG_MW_CAS_NAGRA)
			eBusResult = xmgr_playback_NagraMsgEvtCasDrmSignalled_Base (pstContext, hAction, p1, p2, p3);
#endif
			break;
		case eDxCopyrightControl_CasCcType_CiPlus:
#if defined(CONFIG_MW_CI_PLUS)
			eBusResult = xmgr_playback_CiPlusMsgEvtCasDrmSignalled_Base (pstContext, hAction, p1, p2, p3);
#endif
			break;
		default:
			eBusResult = ERR_BUS_NO_ERROR;
			break;
	}

	return eBusResult;

}

STATIC BUS_Result_t xmgr_playback_MsgEvtRecordTimeStampUpdate_Ses (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t					eBusResult = ERR_BUS_NO_ERROR;
	SvcMeta_Record_t 				stRecordInfo;
	DxCopyrightControl_CasCcType_e	eCasCcType = eDxCopyrightControl_CasCcType_None;
	DxCopyrightControl_SiCcType_e	eSiCcType = eDxCopyrightControl_SiCcType_None;

	HxLOG_Debug("\n");

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return MESSAGE_PASS;
	}

	if (p1 != pstContext->ulContentId)
	{
		return MESSAGE_PASS;
	}

#if defined (CONFIG_DEBUG)
	xmgr_pb_PrintDrmInfo_Ses(pstContext, eMEVT_PVR_RECORD_TIMESTAMP_UPDATE);
#endif

	HxSTD_MemSet(&stRecordInfo, 0, sizeof(SvcMeta_Record_t));

	if(SVC_META_GetRecordInfo (pstContext->stSetup.ulContentId, &stRecordInfo) != ERR_OK)
	{
		HxLOG_Error("SVC_META_GetRecordInfo is error\n");
	}

#if defined(CONFIG_MW_CAS_NAGRA)
	eCasCcType = xmgr_playback_NagraGetCasDrmType_Ses(&stRecordInfo.stDrmInfo);
#endif

	HxLOG_Debug("eCasCcType(%d)\n", eCasCcType);

	if(eCasCcType != eDxCopyrightControl_CasCcType_None)
	{
		switch(eCasCcType)
		{
			case eDxCopyrightControl_CasCcType_NagraSes:
#if defined(CONFIG_MW_CAS_NAGRA)
				eBusResult = xmgr_playback_NagraMsgEvtRecordTimeStampUpdate_Base (pstContext, hAction, p1, p2, p3);
#endif
				break;
			case eDxCopyrightControl_CasCcType_CiPlus:
#if defined(CONFIG_MW_CI_PLUS)
				eBusResult = xmgr_playback_CiPlusMsgEvtRecordTimeStampUpdate_Base (pstContext, hAction, p1, p2, p3);
#endif
				break;
			default:
				eBusResult = ERR_BUS_NO_ERROR;
				break;
		}
	}
	else
	{
		eSiCcType = xmgr_playback_NagraGetSiDrmType_Ses(&stRecordInfo.stDrmInfo);
		switch(eSiCcType)
		{
			case eDxCopyrightControl_SiCcType_Ses:
#if defined(CONFIG_MW_CAS_NAGRA)
				eBusResult = xmgr_playback_NagraMsgEvtRecordTimeStampUpdate_Base (pstContext, hAction, p1, p2, p3);
#endif
				break;
			default:
				eBusResult = ERR_BUS_NO_ERROR;
				break;
		}
	}

	return eBusResult;
}

STATIC BUS_Result_t xmgr_playback_MsgEvtSiDrmSignalled_Ses( xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DxCopyrightControl_t	stDrmInfo;
	MgrCopyrightControl_MediaControlInfo_t	stDetailDrmInfo;

	HxLOG_Debug("\n");

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return MESSAGE_PASS;
	}

	HxSTD_MemSet(&stDrmInfo, 0, sizeof(DxCopyrightControl_t));
	HLIB_STD_MemCpySafe(&stDrmInfo, sizeof(DxCopyrightControl_t), (void *)p3, sizeof(DxCopyrightControl_t));

	if(xmgr_playback_NagraIsValidSiDrmInfo_Ses(&stDrmInfo) != ERR_OK)
	{
		HxLOG_Error("drmInfo is error\n");
		return MESSAGE_PASS;
	}

	if(xmgr_playback_NagraGetDrmInfo_Ses(&stDrmInfo, &stDetailDrmInfo) != ERR_OK)
	{
		HxLOG_Error("drmInfo is not valid\n");
		return MESSAGE_PASS;
	}

	pstContext->bDrmValid = TRUE;
 	pstContext->stDrmInfo.bSiCc = TRUE;

	HxSTD_MemCopy(&pstContext->stDrmInfo.stSiCcInfo, &stDrmInfo.stSiCcInfo, sizeof(DxCopyrightControl_SiCcInfo_t));
	xmgr_playback_SetCopyProtection(pstContext); // Copy Protection

	return ERR_BUS_NO_ERROR;

 }

STATIC BUS_Result_t xproc_playback_Proto_Ses (xmgrPbContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t	eBusResult = ERR_BUS_NO_ERROR;
	HUINT32			ulActionId = SVC_PIPE_GetActionId(hAction);

#if defined(CONFIG_DEBUG)
	HUINT8 *pszMsg = xmgr_Playback_GetMessageName_Ses(message);
	if(pszMsg != NULL)
	{
		HxLOG_Print("[XMGR:PbSes] MESSAGE (%s), ulActionId(%x), hAction(%x), p1(%x), p2(%x), p3(%x)\n", pszMsg, ulActionId, hAction, p1, p2, p3);
	}
#endif

	switch (message)
	{
		case eMEVT_BUS_CREATE:
			eBusResult = xmgr_playback_MsgBusCreate_Ses(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_BUS_DESTROY:
			eBusResult = xmgr_playback_MsgBusDestroy_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_BUS_TIMER:
			eBusResult = xmgr_playback_MsgBusTimer_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_ACTION_NOTIFY_RELEASED:
			eBusResult = xmgr_playback_MsgAmActionStopped_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_PVR_NOTIFY_RECORD_STOPPED:
		case eMEVT_PVR_NOTIFY_RECORD_SPLIT_FILE:
			eBusResult = xmgr_playback_MsgAppRecordStop_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eOEVT_PG_CHECK_RATING :
			CHECK_PB_ACTION_HANDLE();
			eBusResult = xmgr_playback_MsgAppCheckRating_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_SVCLIST_SVC_PID_CHANGED :
			CHECK_PB_ACTION_HANDLE();
			eBusResult = xmgr_playback_MsgAppDbPidChanged_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_PG_PIN_CHECK_OK :
			CHECK_PB_ACTION_HANDLE();
			eBusResult = xmgr_playback_MsgAppPinOK_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_PG_PIN_CHECK_DENIED :
			CHECK_PB_ACTION_HANDLE();
			eBusResult = xmgr_playback_MsgAppPinDenied_Base(pstContext, hAction, p1, p2, p3);
			break;


		case eSEVT_PB_SETUP:
			CHECK_PB_ACTION_HANDLE();
#if 1 // defined(CONFIG_DRM_TEST)
			if(xmgr_playback_GetPlaybackEnableStatusFromDrmInfo_Ses(pstContext) == FALSE)
			{
				HxLOG_Debug("Play is not Start................\n");
			}
			else
#endif
			{
				eBusResult = xmgr_playback_MsgEvtPbSetup_Base(pstContext, hAction, p1, p2, p3);
			}
#if defined(CONFIG_MW_CAS_NAGRA)
			// 임시로 여거에서 drm infro를 web으로 올려주자.
			xmgr_playback_NagraSendDrmInfo_Base(pstContext);
#endif
			break;

		case eSEVT_AV_SESSION_STARTED:
			CHECK_PB_ACTION_HANDLE();
			eBusResult = xmgr_playback_MsgEvtAvDecodingStarted_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_PB_START:
			CHECK_PB_ACTION_HANDLE();
			eBusResult = xmgr_playback_MsgEvtPbStart_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_PMT :
			CHECK_PB_ACTION_HANDLE();
			eBusResult = xmgr_playback_MsgEvtSiPmtReceived_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_PAT_TIMEOUT:
		case eSEVT_SI_PMT_TIMEOUT:
		case eSEVT_SI_PMT_PIDREMOVED:
			CHECK_PB_ACTION_HANDLE();
			eBusResult = xmgr_playback_MsgEvtSiPmtTimeout_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_PMT_PIDCHANGED :
			CHECK_PB_ACTION_HANDLE();
			eBusResult = xmgr_playback_MsgEvtSiPmtPidChanged_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_EIT :
			CHECK_PB_ACTION_HANDLE();
			// TODO : CAS가 TRUE인경우에는 CAS로 CC를 대체한다.
			eBusResult = xmgr_playback_MsgEvtSiEitReceived_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_EIT_TIMEOUT :
			CHECK_PB_ACTION_HANDLE();
			eBusResult = xmgr_playback_MsgEvtSiEitTimeout_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_AV_VIDEO_UNDERRUN :
			CHECK_PB_ACTION_HANDLE();
			eBusResult = xmgr_playback_MsgEvtAvVideoUnderrun_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_AV_VIDEO_STARTED :
			CHECK_PB_ACTION_HANDLE();
			eBusResult = xmgr_playback_MsgEvtAvVideoStarted_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_AV_AUDIO_UNDERRUN :
			CHECK_PB_ACTION_HANDLE();
			eBusResult = xmgr_playback_MsgEvtAvAudioUnderrun_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_AV_AUDIO_STARTED :
			CHECK_PB_ACTION_HANDLE();
			eBusResult = xmgr_playback_MsgEvtAvAudioStarted_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_PB_TIMECODE_CHANGE :
			CHECK_PB_ACTION_HANDLE();
			eBusResult = xmgr_playback_MsgEvtPbTimecodeChange_Ses(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_PB_EOF :
			CHECK_PB_ACTION_HANDLE();
			eBusResult = xmgr_playback_MsgEvtPbEOF_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_PB_BOF :
			CHECK_PB_ACTION_HANDLE();
			eBusResult = xmgr_playback_MsgEvtPbBOF_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SBTL_STOPPED:
			CHECK_PB_ACTION_HANDLE();
			eBusResult = xmgr_playback_MsgEvtSbtlStopped_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SBTL_STARTED:
			CHECK_PB_ACTION_HANDLE();
			eBusResult = xmgr_playback_MsgEvtSbtlStarted_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_OK :
			CHECK_PB_ACTION_HANDLE();
			eBusResult = xmgr_playback_MsgEvtCasOk_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_FAIL :
			CHECK_PB_ACTION_HANDLE();
			eBusResult = xmgr_playback_MsgEvtCasFail_Base(pstContext, message, p1, p2, p3);
			break;

		case eSEVT_CAS_CHECK :
			CHECK_PB_ACTION_HANDLE();
			eBusResult = xmgr_playback_MsgEvtCasFail_Base(pstContext, message, p1, p2, p3);
			break;

		case eMEVT_CAS_SYSINFO_UPDATED:
			eBusResult = xmgr_playback_MsgEvtCasScInserted_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_SC_REMOVED:
			eBusResult = xmgr_playback_MsgEvtCasScRemoved_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_DRM_AV_BLANK:
			CHECK_PB_ACTION_HANDLE();
			eBusResult = xmgr_playback_MsgEvtAvBlankCtrlByCAS_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_PB_DRM_AV_BLANK:
			CHECK_PB_ACTION_HANDLE();
			eBusResult = xmgr_playback_MsgEvtAvBlankByDrmInfo_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_DRM_SIGNALLED:
			CHECK_PB_ACTION_HANDLE();
			eBusResult = xmgr_playback_MsgEvtCasDrmSignalled_Ses(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_DRM_SIGNALLED:
			CHECK_PB_ACTION_HANDLE();
			eBusResult = xmgr_playback_MsgEvtSiDrmSignalled_Ses (pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_PVR_RECORD_TIMESTAMP_UPDATE:
			//CHECK_PB_ACTION_HANDLE();
			eBusResult = xmgr_playback_MsgEvtRecordTimeStampUpdate_Ses(pstContext, hAction, p1, p2, p3);
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


STATIC BUS_Result_t xproc_playback_Action0_Ses (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t		*pstContext = xmgr_playback_GetContextFromIndex_Base (0);

	return xproc_playback_Proto_Ses (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_Action1_Ses (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t		*pstContext = xmgr_playback_GetContextFromIndex_Base (1);

	return xproc_playback_Proto_Ses (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_Action2_Ses (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t		*pstContext = xmgr_playback_GetContextFromIndex_Base (2);

	return xproc_playback_Proto_Ses (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_Action3_Ses (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t		*pstContext = xmgr_playback_GetContextFromIndex_Base (3);

	return xproc_playback_Proto_Ses (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_Action4_Ses (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t		*pstContext = xmgr_playback_GetContextFromIndex_Base (4);

	return xproc_playback_Proto_Ses (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_Action5_Ses (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t		*pstContext = xmgr_playback_GetContextFromIndex_Base (5);

	return xproc_playback_Proto_Ses (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_Action6_Ses (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t		*pstContext = xmgr_playback_GetContextFromIndex_Base (6);

	return xproc_playback_Proto_Ses (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_Action7_Ses (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t		*pstContext = xmgr_playback_GetContextFromIndex_Base (7);

	return xproc_playback_Proto_Ses (pstContext, message, hAction, p1, p2, p3);
}

#define _____MSG_PROCESSING_FUNCTIONS_____

BUS_Callback_t	xmgr_playback_GetProcFunc_Ses (HUINT32 ulActionId)
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
		return xproc_playback_Action0_Ses;

	case 1:
		return xproc_playback_Action1_Ses;

	case 2:
		return xproc_playback_Action2_Ses;

	case 3:
		return xproc_playback_Action3_Ses;

	case 4:
		return xproc_playback_Action4_Ses;

	case 5:
		return xproc_playback_Action5_Ses;

	case 6:
		return xproc_playback_Action6_Ses;

	case 7:
		return xproc_playback_Action7_Ses;

	default:
		break;
	}

	return NULL;
}

#if defined (CONFIG_DEBUG)
HUINT8 *xmgr_Playback_GetMessageName_Ses(HINT32 message)
{
	switch(message)
	{
		ENUM_TO_STR(eMEVT_BUS_CREATE);
		ENUM_TO_STR(eMEVT_BUS_DESTROY);
		ENUM_TO_STR(eMEVT_BUS_TIMER);
		ENUM_TO_STR(eMEVT_ACTION_NOTIFY_RELEASED);
		ENUM_TO_STR(eMEVT_PVR_NOTIFY_RECORD_STOPPED);
		ENUM_TO_STR(eMEVT_PVR_NOTIFY_RECORD_SPLIT_FILE);
		ENUM_TO_STR(eOEVT_PG_CHECK_RATING);
		ENUM_TO_STR(eMEVT_SVCLIST_SVC_PID_CHANGED);
		ENUM_TO_STR(eMEVT_PG_PIN_CHECK_OK);
		ENUM_TO_STR(eMEVT_PG_PIN_CHECK_DENIED);
		ENUM_TO_STR(eSEVT_PB_SETUP);
		ENUM_TO_STR(eSEVT_AV_SESSION_STARTED);
		ENUM_TO_STR(eSEVT_PB_START);
		ENUM_TO_STR(eSEVT_SI_PMT);
		ENUM_TO_STR(eSEVT_SI_PAT_TIMEOUT);
		ENUM_TO_STR(eSEVT_SI_PMT_TIMEOUT);
		ENUM_TO_STR(eSEVT_SI_PMT_PIDREMOVED);
		ENUM_TO_STR(eSEVT_SI_PMT_PIDCHANGED);
		ENUM_TO_STR(eSEVT_SI_EIT);
		ENUM_TO_STR(eSEVT_SI_EIT_TIMEOUT);
		ENUM_TO_STR(eSEVT_AV_VIDEO_UNDERRUN);
		ENUM_TO_STR(eSEVT_AV_VIDEO_STARTED);
		ENUM_TO_STR(eSEVT_AV_AUDIO_UNDERRUN);
		ENUM_TO_STR(eSEVT_AV_AUDIO_STARTED);
		ENUM_TO_STR(eSEVT_PB_TIMECODE_CHANGE);
		ENUM_TO_STR(eSEVT_PB_EOF);
		ENUM_TO_STR(eSEVT_PB_BOF);
		ENUM_TO_STR(eSEVT_SBTL_STOPPED);
		ENUM_TO_STR(eSEVT_SBTL_STARTED);
		ENUM_TO_STR(eSEVT_CAS_OK);
		ENUM_TO_STR(eSEVT_CAS_FAIL);
		ENUM_TO_STR(eSEVT_CAS_CHECK);
		ENUM_TO_STR(eMEVT_CAS_SYSINFO_UPDATED);
		ENUM_TO_STR(eSEVT_CAS_SC_REMOVED);
		ENUM_TO_STR(eSEVT_CAS_DRM_AV_BLANK);
		ENUM_TO_STR(eSEVT_PB_DRM_AV_BLANK);
		ENUM_TO_STR(eSEVT_CAS_DRM_SIGNALLED);
		ENUM_TO_STR(eSEVT_SI_DRM_SIGNALLED);
		ENUM_TO_STR(eMEVT_PVR_RECORD_TIMESTAMP_UPDATE);
		default:
			return NULL;
	}
}

STATIC HERROR xmgr_playback_PrintDrmParseInfo (HUINT8 ucCtrlByteA, HUINT8 ucCtrlByteB)
{
	MgrCopyrightControl_MediaControlInfo_t stDrmInfo;

	HxSTD_MemSet (&stDrmInfo, 0, sizeof(MgrCopyrightControl_MediaControlInfo_t));

	// Byte A
	stDrmInfo.eTrickModeCtrl.eTrickRestrictMode_Ses 	= (HUINT8)((ucCtrlByteA & 0x38) >> 3);
	stDrmInfo.bSvcBlock		 	= (ucCtrlByteA & 0x04) ? TRUE : FALSE;
	stDrmInfo.bDigiCpProtect 	= (ucCtrlByteA & 0x02) ? TRUE : FALSE;
	stDrmInfo.bAnaCpProtect	 	= (ucCtrlByteA & 0x01) ? TRUE : FALSE;

	// Byte B
	stDrmInfo.eStorageRight	 	= (HUINT8)((ucCtrlByteB & 0xE0) >> 5);
	stDrmInfo.bStorageEncrypt 	= (ucCtrlByteB & 0x10) ? TRUE : FALSE;
	stDrmInfo.eCompVidCtrl	 	= (HUINT8)((ucCtrlByteB & 0x0C) >> 2);
	stDrmInfo.bTransCtrl	 	=  (ucCtrlByteB & 0x02) ? TRUE : FALSE;
	stDrmInfo.bBlucomCtrl	 	=  (ucCtrlByteB & 0x01) ? TRUE : FALSE;

	HxLOG_Debug("[drmInfo] eTrickModeCtrl(%x)SvcBlock(%x) DigiCpProtect(%x)AnaCpProtect(%x) eStorageRight(%x) bStorageEncrypt(%x) eCompVidCtrl(%x)bTransCtrl(%x) bBlucomCtrl(%x)\n\n",
		stDrmInfo.eTrickModeCtrl.eTrickRestrictMode_Ses, stDrmInfo.bSvcBlock, stDrmInfo.bDigiCpProtect, stDrmInfo.bAnaCpProtect,
		stDrmInfo.eStorageRight, stDrmInfo.bStorageEncrypt, stDrmInfo.eCompVidCtrl, stDrmInfo.bTransCtrl,
		stDrmInfo.bBlucomCtrl);

	return ERR_OK;
}

STATIC void xmgr_pb_PrintDrm_Ses(DxCopyrightControl_t *pstDrmInfo, HCHAR *pcStr)
{
	if(pstDrmInfo == NULL)
	{
		return;
	}

	if(pcStr!=NULL)
	{
		HxLOG_Debug("+-- %s() -- = %d\n", pcStr);
	}

	if(pstDrmInfo->bCasCc == TRUE)
	{
		HxLOG_Debug("DRM(CAS) TRUE\n");
		if(pstDrmInfo->stCasCcInfo.eCasCcType != eDxCopyrightControl_CasCcType_NagraSes)
		{
			HxLOG_Debug("+ ----------------- cas drm type[%d] : No NagraSes -------------------\n", pstDrmInfo->stDrmCcInfo.eDrmCcType);
		}
		xmgr_playback_PrintDrmParseInfo(pstDrmInfo->stCasCcInfo.info.stNagraDrmInfo_Ses.ucControlByteA, pstDrmInfo->stCasCcInfo.info.stNagraDrmInfo_Ses.ucControlByteB);
	}

	if(pstDrmInfo->bSiCc == TRUE)
	{
		HxLOG_Debug("DRM(SI) TRUE\n");
		if(pstDrmInfo->stSiCcInfo.eSiCcType != eDxCopyrightControl_SiCcType_Ses)
		{
			HxLOG_Debug("+ ----------------- si drm type[%d] : No NagraSes -------------------\n", pstDrmInfo->stDrmCcInfo.eDrmCcType);
			return;
		}
		xmgr_playback_PrintDrmParseInfo(pstDrmInfo->stSiCcInfo.info.stSesDrmInfo.ucControlByteA, pstDrmInfo->stSiCcInfo.info.stSesDrmInfo.ucControlByteB);
	}
}

STATIC void xmgr_pb_PrintDrmInfo_Ses( xmgrPbContext_t *pstContext, HINT32 message)
{
	HUINT8 buf[100];
	SvcMeta_Record_t stRecordInfo;

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return;
	}
	HxSTD_memset(&buf, 0, sizeof(buf));
	switch(message)
	{
		case eMEVT_BUS_CREATE:
			sprintf(buf, "%s", "eMEVT_BUS_CREATE");
			break;
		case eMEVT_PVR_RECORD_CHANGE_TSR_REC_TO_DELAYED_RECORD:
			sprintf(buf, "%s", "eMEVT_PVR_RECORD_CHANGE_TSR_REC_TO_DELAYED_RECORD");
			break;
		case eSEVT_CAS_DRM_SIGNALLED:
			sprintf(buf, "%s", "eSEVT_CAS_DRM_SIGNALLED");
			break;
		case eSEVT_PB_TIMECODE_CHANGE:
			sprintf(buf, "%s", "eSEVT_PB_TIMECODE_CHANGE");
			break;
		case eMEVT_PVR_RECORD_TIMESTAMP_UPDATE:
			sprintf(buf, "%s", "eMEVT_PVR_RECORD_TIMESTAMP_UPDATE");
			break;
		case eSEVT_CAS_DRM_NONE:
			sprintf(buf, "%s", "eSEVT_CAS_DRM_NONE");
			break;
		default:
			sprintf(buf, "%s", "null");
			break;
	}

	HxSTD_MemSet(&stRecordInfo, 0, sizeof(SvcMeta_Record_t));

	if(SVC_META_GetRecordInfo (pstContext->stSetup.ulContentId, &stRecordInfo) != ERR_OK)
	{
		HxLOG_Error("SVC_META_GetRecordInfo is error\n");
	}

	HxLOG_Debug("\n*************************************** PRINT START ***************************************\n");
	HxLOG_Debug("*************************************** PLAYBACK Info < %s > ***************************************\n", buf);
	HxLOG_Debug("*************************************** New DrmInfo ***************************************\n");
	HxLOG_Debug("[+ bDrmValid(%d)\n",pstContext->bDrmValid);
	xmgr_pb_PrintDrm_Ses(&pstContext->stDrmInfo, NULL);
	HxLOG_Debug("*************************************** Meta DrmInfo ***************************************\n");
	xmgr_pb_PrintDrm_Ses(&stRecordInfo.stDrmInfo, NULL);


	HxLOG_Debug("+ aucPath(%s)\n",stRecordInfo.aucPath);
	HxLOG_Debug("+ aucFileName(%s)\n",stRecordInfo.aucFileName);

	HxLOG_Debug("+ ulStartTime(%d)\n",stRecordInfo.ulStartTime);
	HxLOG_Debug("+ ulEndTime(%d)\n", stRecordInfo.ulEndTime);
	HxLOG_Debug("+ ulRecTime(%d)\n", stRecordInfo.ulRecTime);
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES) // defined(CONFIG_APS_HD)
	HxLOG_Debug("+ ucSCPinCodeFlag(%d)\n", stRecordInfo.ucSCPinCodeFlag);
#endif

#if 1
	HxLOG_Debug("+ ulOrgDuration(%d)\n",stRecordInfo.ulOrgDuration);

	HxLOG_Debug("+ ucRecordStatus(%d)\n",stRecordInfo.ucRecordStatus);
	HxLOG_Debug("+ u2cEventBase(%d)\n",stRecordInfo.u2cEventBase);
	HxLOG_Debug("+ nEventId(%d)\n", stRecordInfo.nEventId);
	HxLOG_Debug("+ u1cLocked(%d)\n",stRecordInfo.u1cLocked);
	HxLOG_Debug("+ u1cWatched(%d)\n", stRecordInfo.u1cWatched);
	HxLOG_Debug("+ u1cScrambled(%d)\n", stRecordInfo.u1cScrambled); // exist any scramble position in record file
	HxLOG_Debug("+ u1cEncryption(%d)\n",stRecordInfo.u1cEncryption);
	HxLOG_Debug("+ u1cAutoDeleted(%d)\n",stRecordInfo.u1cAutoDeleted);

	HxLOG_Debug("+ ucEndCondition(%d)\n",stRecordInfo.ucEndCondition);			// reason to Record Fail, Recocrd Split
	HxLOG_Debug("+ ulLastWatchtime(%d)\n", stRecordInfo.ulLastWatchtime);
	HxLOG_Debug("+ bUserLocked(%d)\n", stRecordInfo.bUserLocked);			// true means locked at the record menu (or record list)
											// u1cLocked means service lock or parental lock or watch limited
	HxLOG_Debug("+ bWillBeDeleted(%d)\n", stRecordInfo.bWillBeDeleted);
	HxLOG_Debug("+ usNumOfBookmark(%d)\n",stRecordInfo.usNumOfBookmark);
	//SvcMeta_Bookmark_t	astBookmarks[DxPVR_NUM_BOOKMARK];
	HxLOG_Debug("+ aucDisplayEvtName(%s)\n",stRecordInfo.aucDisplayEvtName);			// Medialist Display Name

	HxLOG_Debug("+ bSeriesRec(%d)\n",stRecordInfo.bSeriesRec);
	HxLOG_Debug("+ szSeriesId(%s)\n", stRecordInfo.szSeriesId);
	HxLOG_Debug("+ ulEpisodeNum(%d)\n", stRecordInfo.ulEpisodeNum);
	HxLOG_Debug("+ ulSeason(%d)\n", stRecordInfo.ulSeason);
#endif
	HxLOG_Debug("*************************************** PRINT END ***************************************\n");
}
#endif

#endif
