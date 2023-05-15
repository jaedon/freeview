/**************************************************************************************/
/**
 * @file oplrecording.c
 *
 * Defines the query structure.
 *
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include "oplrecording.h"
#include "oplscheduler.h"

#include <hlib.h>
#include <apk.h>
#include <dlib.h>

#ifndef	CONFIG_OP_JAPAN
#include <thapi.h>
#endif

//#define	HxLOG_DOMAIN	"recording"


#define ePVR_REC_STS_RECORD_START	eAPKE_PVR_REC_STS_RECORD_START
#define ePVR_REC_STS_RECORDING		eAPKE_PVR_REC_STS_RECORDING
#define ePVR_REC_STS_RECORDED		eAPKE_PVR_REC_STS_RECORDED
#define ePVR_REC_STS_INCOMPLETED	eAPKE_PVR_REC_STS_INCOMPLETED

#define eREC_FAILED_NOFAIL				eAPKE_REC_FAILED_NOFAIL
#define eREC_FAILED_HDDFULL				eAPKE_REC_FAILED_HDDFULL
#define eREC_FAILED_MAXNUM				eAPKE_REC_FAILED_MAXNUM
#define eREC_FAILED_MAXRECORDLISTNUM	eAPKE_REC_FAILED_MAXRECORDLISTNUM
#define eREC_FAILED_LESS30SEC			eAPKE_REC_FAILED_LESS30SEC
#define eREC_FAILED_NOSIGNAL			eAPKE_REC_FAILED_NOSIGNAL
#define eREC_FAILED_NONSERVICE			eAPKE_REC_FAILED_NONSERVICE
#define eREC_FAILED_SEARCHING			eAPKE_REC_FAILED_SEARCHING
#define eREC_FAILED_CONFLICT_TP			eAPKE_REC_FAILED_CONFLICT_TP
#define eREC_FAILED_DRM					eAPKE_REC_FAILED_DRM
#define eREC_FAILED_NOSC_SCR			eAPKE_REC_FAILED_NOSC_SCR
#define eREC_FAILED_NORIGHT_SCR			eAPKE_REC_FAILED_NORIGHT_SCR
#define eREC_FAILED_NOHDD				eAPKE_REC_FAILED_NOHDD
#define eREC_INCOMPLETE_HDDFULL			eAPKE_REC_INCOMPLETE_HDDFULL
#define eREC_FAILD_UNKNOWN				eAPKE_REC_FAILD_UNKNOWN
#define eREC_FAILED_NOUSBHDD			eAPKE_REC_FAILED_NOUSBHDD
#define eREC_FAILED_DISCONNECTUSBHDD	eAPKE_REC_FAILED_DISCONNECTUSBHDD
#define eREC_FAILED_EXTUSB_CopyDel		eAPKE_REC_FAILED_EXTUSB_CopyDel
#define eREC_FAILED_SAME_SVC_RECORDING	eAPKE_REC_FAILED_SAME_SVC_RECORDING
#define eREC_FAILED_HIGHER_PRIORITY		eAPKE_REC_FAILED_HIGHER_PRIORITY
#define eREC_FAILED_UNABLE_TRACKING		eAPKE_REC_FAILED_UNABLE_TRACKING


#define	____EXTERN_FUNCTION_DECLARATION__________________________________________________________

#define	____DERIVED_OPL_Recording___________________________________________________________

OPL_ScheduledItem_t *
		opl_scheduleditem_Init (int type, unsigned int sizeOf);
void	opl_scheduleditem_DeInit (OPL_ScheduledItem_t *self);
static	int	opl_recording_GetError(const DxRecListData_t *pstRecData);


#define	____PRIVATE_OPL_Recording__________________________________________________________


static int	opl_recording_GetState(const DxRecListData_t *pstRecData)
{
	if (pstRecData)
	{
		switch(pstRecData->eRecStatus)
		{
			case eDxREC_STS_RECORD_START:
			case eDxREC_STS_RECORDING:
				return eOPL_SCHEDULER_EVENT_STARTED;

			case eDxREC_STS_RECORDED:
				return (eOPL_SCHEDULER_ERROR_NONE == opl_recording_GetError(pstRecData))
					? eOPL_SCHEDULER_EVENT_COMPLETED : eOPL_SCHEDULER_EVENT_PARTIALY_COMPLETED;

			case eDxREC_STS_INCOMPLETED:
				return eOPL_SCHEDULER_EVENT_PARTIALY_COMPLETED;

			default:
				return eOPL_SCHEDULER_EVENT_ERROR;
		}
	}
	return eOPL_SCHEDULER_EVENT_ERROR;
}


static	int	opl_recording_GetError(const DxRecListData_t *pstRecData)
{
	if (pstRecData)
	{
		switch(pstRecData->eRecEndReason)
		{
			case eDxREC_FAILED_HDDFULL:
			case eDxREC_INCOMPLETE_HDDFULL:
				return eOPL_SCHEDULER_ERROR_INSUFFICIENT_STORAGE;

			case eDxREC_FAILED_CONFLICT_TP:
				return eOPL_SCHEDULER_ERROR_TUNER_CONFLICT;

			case eDxREC_FAILED_DRM:
				return eOPL_SCHEDULER_ERROR_DRM_RESTRICTION;

			case eDxREC_FAILED_MAXNUM:
			case eDxREC_FAILED_MAXRECORDLISTNUM:
			case eDxREC_FAILED_NOHDD:
			case eDxREC_FAILED_NOUSBHDD:
			case eDxREC_FAILED_SAME_SVC_RECORDING:
				return eOPL_SCHEDULER_ERROR_RESOURCE_LIMITATION;
			case eDxREC_FAILED_NOSIGNAL:
				return eOPL_SCHEDULER_ERROR_NO_AV;
			case eDxREC_FAILED_POWER_OFF:
				return eOPL_SCHEDULER_ERROR_POWER_OFF;

			case eDxREC_FAILED_NOFAIL:
				return eOPL_SCHEDULER_ERROR_NONE;
			case eDxREC_FAILED_NORIGHT_SCR:
				return eOPL_SCHEDULER_ERROR_CAS;
			case eDxREC_FAILED_HIGHER_PRIORITY:
				return eOPL_SCHEDULER_ERROR_HIGHER_PRIORITY;
			case eDxREC_FAILED_NO_VIDEO_AUDIO:
				return eOPL_SCHEDULER_ERROR_OFF_AIR;
			case eDxREC_FAILED_UNABLE_TRACKING:
				return eOPL_SCHEDULER_ERROR_TRACKING_FAIL;

			default:
				return eOPL_SCHEDULER_ERROR_UNKNOWN;

		}
	}

	return eOPL_SCHEDULER_ERROR_UNKNOWN;
}

static	char*	opl_recording_GetID(const DxRecListData_t *pstRecData)
{
	if (pstRecData)
	{
		return HLIB_STD_StrDup(pstRecData->szUrl);
	}

	return NULL;
}

static	int		opl_recording_GetIsManual(const DxRecListData_t *pstRecData)
{
	if (pstRecData)
	{
		return !pstRecData->bEventBased; 	// reverse !
	}

	return -1;
}

static	int		opl_recording_GetDoNotDelete(const DxRecListData_t *pstRecData)
{
	if (pstRecData)
	{
		return pstRecData->bDoNotDelete;
	}

	return -1;
}

static	int		opl_recording_GetDoNotDeleteSetTime(const DxRecListData_t *pstRecData)
{
	if (pstRecData)
	{
		return pstRecData->ulDoNotDeleteSetTime;
	}

	return -1;
}


static	int		opl_recording_GetWillBeDeleted(const DxRecListData_t *pstRecData)
{
	if (pstRecData)
	{
		return pstRecData->bWillBeDeleted;
	}

	return -1;
}


static	int		opl_recording_GetSaveDays(const DxRecListData_t *pstRecData)
{
	if (pstRecData)
	{	// TODO:
		return 5;
	}

	return 0;
}


static	int		opl_recording_GetSaveEpisodes(const DxRecListData_t *pstRecData)
{
	if (pstRecData)
	{	// TODO:
		return 5;
	}
	return 0;
}


/* blocked & locked description */
/*Description 																	blocked 	locked
No parental control applies. 													false 		false
Item is above the parental rating threshold (or manually blocked); 				true		true
no PIN has been entered to view it and so the item cannot currently be viewed.
Item is above the parental rating threshold (or manually blocked);
the PIN has been entered and so the item can be viewed.							true 		false
Invalid combination . OITFs SHALL NOT support this combination 					false 		true
*/
static	int		opl_recording_GetBlocked(const DxRecListData_t *pstRecData)
{
	if (pstRecData)
	{
		return pstRecData->bBlocked;	// svc lock or pin lock or pr lock ...
	}

	return -1;
}


static	int		opl_recording_GetLocked(const DxRecListData_t *pstRecData)
{
	if (pstRecData)
	{
		if (0 == pstRecData->bBlocked)
		{	// no blocked => no locked
			return 0;
		}
		else
		{	// blocked => need more implementation in the octo ... (can't get the clear status)
		}
	}

	return -1;
}

static	eOplRecordingShowType	opl_recording_GetShowType(const DxRecListData_t	*pstRecData)
{
	if (pstRecData)
	{
		if (eDxREC_STS_RECORDING == pstRecData->eRecStatus || eDxREC_STS_RECORD_START== pstRecData->eRecStatus)
		{	// START -> RECORDING -> Live Type
			return OPLRECORDING_SHOWTYPE_LIVE;
		}
		else
		{	// check the
			if (pstRecData->bPlayed)
			{
				return OPLRECORDING_SHOWTYPE_RERUN;
			}
			else
			{
				return OPLRECORDING_SHOWTYPE_FIRST_RUN;
			}
		}
	}

	return -1;
}

static	int		opl_recording_GetSubtitles(const DxRecListData_t	*pstRecData)
{
	// TODO: get the real data
	if (pstRecData)
	{	// have subtitle ? -> use EIT Data (Should check more about PMT data & EIT Data)
		//return (0 < pstRecData->stSbtlInfoList.ulItemNum) ? TRUE : FALSE;

		int i = 0;
		for(i=0; i<DxEPG_COMPONENT_DESC_MAX; i++)
		{
			if(pstRecData->stEvt.stComponent[i].ucStreamContent == 0)
				break;

			if(pstRecData->stEvt.stComponent[i].ucStreamContent != 0x03)
				continue;

			// 13.07.2012 - SKIP the EUB - due to current SW can't support EBU subtitle
			// => Should enable below disabled codes when SW support EBU subtitle
			/*if (pstRecData->stEvt.stComponent[i].ucComponentType == 0x01)
			{	// EBU Subtitle
				return 1;
			}//*/
			if ((0x10 <= pstRecData->stEvt.stComponent[i].ucComponentType && pstRecData->stEvt.stComponent[i].ucComponentType <= 0x14)
				|| (0x20 <= pstRecData->stEvt.stComponent[i].ucComponentType && pstRecData->stEvt.stComponent[i].ucComponentType <= 0x24))
				return 1;
		}
	}

	return 0;
}


static int opl_recording_CompLangCode(const void	*Lang, void *compLang)
{
	if (Lang == NULL || compLang == NULL)
		return OPL_FALSE;

	if (0 == HxSTD_StrCmp((HCHAR*)Lang, (HCHAR*)compLang))
		return OPL_TRUE;

	return OPL_FALSE;
}


static	OPL_Collection_t*		opl_recording_GetSubtitleLanguages(const DxRecListData_t	*pstRecData)
{
	OPL_Collection_t	*c = NULL;
	HUINT32			i;

	if(pstRecData == NULL)
	{
		return NULL;
	}

	c	= OPL_Collection_NewEx(HLIB_STD_MemFree_CB, NULL);

	for(i=0; i < pstRecData->stSbtlInfoList.ulItemNum; i++)
	{
		DxMETA_PvrSbtlInfo_t	*pstSbtl = (DxMETA_PvrSbtlInfo_t	*)&pstRecData->stSbtlInfoList.astSbtlInfo[i];

		// SKIP the EBU Subtitles (below if condition code will be removed when SW can handle the EBU subtitle
		if (pstSbtl->ucComponentType < 0x10)
		{	// EBU ->
			continue;
		}

		HINT32	nLangIdx = OPL_Collection_Find(c, pstSbtl->aucLangCode, opl_recording_CompLangCode);
		if (nLangIdx < 0)
		{	// not exist -> add
			OPL_Collection_Add(c, (void*)HLIB_STD_StrDup((const HCHAR*)pstSbtl->aucLangCode));
		}
	}
	return c;
}


static	int		opl_recording_GetIsHD(const DxRecListData_t	*pstRecData)
{
	if (pstRecData)
	{
#if defined(CONFIG_OP_JAPAN)
		switch(pstRecData->extension.japan.stAribControlData.u8VideoResolution)
		{
			// ARIB STD - B10 Part2
			// Version 4.4-E1
			// Table 6-5 stream_content and component_type
			case 0x01:         //480i(525i)_4_3
			case 0x02:         //480i(525i)_16_9_with pan vectors(WPV)
			case 0x03:         //480i(525i)_16_9_without pan vectors(WOPV)
			case 0x04:         //480i(525i)_16_9_OVER
			case 0xA1:         //480p(525p)_4_3
			case 0xA2:         //480p(525p)_16_9_WPV
			case 0xA3:         //480p(525p)_16_9_WOPV
			case 0xA4:         //480p(525p)_16_9_OVER
			case 0xD1:         //240p_4_3
			case 0xD2:         //240p_4_3_WPV
			case 0xD3:         //240p_4_3_WOPV
			case 0xD4:         //240p_4_3_OVER
				return 0;      // SD
			case 0xB1:         //1080i(1125i)_4_3
			case 0xB2:         //1080i(1125i)_16_9_WPV
			case 0xB3:         //1080i(1125i)_16_9_WOPV
			case 0xB4:         //1080i(1125i)_16_9_OVER
			case 0xC1:         //720p(750p)_4_3
			case 0xC2:         //720p(750p)_16_9_WPV
			case 0xC3:         //720p(750p)_16_9_WOPV
			case 0xC4:         //720p(750p)_16_9_OVER
				return 1;	   // HD
			default:
			 	return 0;
		}
#else
		if (eDxVIDEO_TYPE_HD == pstRecData->eVideoType)
			return 1;
		else
			return 0;
#endif
	}

	return -1;
}


static	int		opl_recording_GetIsWidescreen(const DxRecListData_t	*pstRecData)
{
	// TODO: get the real data
	if (pstRecData)
	{
#if defined(CONFIG_OP_JAPAN)
		switch(pstRecData->extension.japan.stAribControlData.u8VideoResolution)
		{
			// ARIB STD - B10 Part2
			// Version 4.4-E1
			// Table 6-5 stream_content and component_type
			case 0x01:		   //480i(525i)_4_3
			case 0xA1:		   //480p(525p)_4_3
			case 0xD1:		   //240p_4_3
			case 0xD2:		   //240p_4_3_WPV
			case 0xD3:		   //240p_4_3_WOPV
			case 0xD4:		   //240p_4_3_OVER
			case 0xB1:		   //1080i(1125i)_4_3
			case 0xC1:		   //720p(750p)_4_3
				return 0;	   // 4:3
			case 0x02:		   //480i(525i)_16_9_with pan vectors(WPV)
			case 0x03:		   //480i(525i)_16_9_without pan vectors(WOPV)
			case 0x04:		   //480i(525i)_16_9_OVER
			case 0xA2:		   //480p(525p)_16_9_WPV
			case 0xA3:		   //480p(525p)_16_9_WOPV
			case 0xA4:		   //480p(525p)_16_9_OVER
			case 0xB2:		   //1080i(1125i)_16_9_WPV
			case 0xB3:		   //1080i(1125i)_16_9_WOPV
			case 0xB4:		   //1080i(1125i)_16_9_OVER
			case 0xC2:		   //720p(750p)_16_9_WPV
			case 0xC3:		   //720p(750p)_16_9_WOPV
			case 0xC4:		   //720p(750p)_16_9_OVER
				return 1;	   // 16:9
			default:
				return 0;
		}
#else
		return 0;
#endif
	}

	return 1;
}

static	int		opl_recording_GetLastPlayPosition(const DxRecListData_t	*pstRecData)
{
	if (pstRecData)
	{
		return pstRecData->ulLastViewTime;
	}

	return -1;
}


static	int		opl_recording_Getlcn(const DxRecListData_t	*pstRecData)
{
	if (pstRecData)
	{
		return pstRecData->nSvcNo;
	}

	return -1;
}


static	char*	opl_recording_GetThumbnail(const DxRecListData_t *pstRecData)
{
#ifndef	CONFIG_OP_JAPAN
	HCHAR	*p;
	HCHAR	szFullPath[DxRECLIST_URL];
	HCHAR	szMountPath[256];
	DxEvent_t *	pstEvent = NULL;
	HUINT32 lastplaytime =0;
	HINT32	thumbnailstart = 0;
	HxLOG_Trace();

	if(pstRecData == NULL)
	{
		HxLOG_Error("pstRecData is NULL \n");
		return NULL;
	}

	if(pstRecData)
	{
		if(pstRecData->eSvcType != eDxSVC_TYPE_TV)
		{
			HxLOG_Print("It is not TV Service Type ... \n");
			return NULL;
		}
	}
	lastplaytime = (pstRecData->ulLastViewTime < 10 ) ? 10 : pstRecData->ulLastViewTime;

	if(lastplaytime == 10 && pstRecData->bEventBased == TRUE)
	{
		if(pstRecData->stEvt.ulStartTime == 0)
		{
			if(10 < pstRecData->ulRecDuration)
			{
				thumbnailstart = 10;
			}
			else
			{
				return NULL;
			}
		}
		else
		{
			thumbnailstart = (pstRecData->stEvt.ulStartTime - pstRecData->ulRecStartTime) + 10;

			if(thumbnailstart < 10) //방어 코드
			{
				thumbnailstart = 10;
			}
		}

		if(pstRecData->ulRecDuration < thumbnailstart)
		{
			if(pstRecData->eRecStatus == eDxREC_STS_RECORDED || pstRecData->eRecStatus == eDxREC_STS_INCOMPLETED)
			{
				// start padding 구간에서 정지된 녹화물
				thumbnailstart = 10;
			}
			else
			{
				// 아직 이벤트 시간안에 들어오지 않은 녹화 중 아이템 (start padding time) 은 thumbnail 요청 하지 않는다.
				return NULL;
			}
		}
		lastplaytime = thumbnailstart;
	}


	HxSTD_PrintToStrN(szFullPath, 512, "HPVR:/%s%s%s"
			, pstRecData->stMeta.szPath, pstRecData->stMeta.szFileName, DxEXT_META_FILE);
	if(HLIB_DIR_GetMountPath(pstRecData->stMeta.szPath, szMountPath, 256) != ERR_OK)
	{
		HxLOG_Error("Fail to get Mount Path\n");
		return NULL;
	}
	HxLOG_Print(">>>>THAPI_GetThumbnailURL(%s),time[%d]\n",szFullPath,lastplaytime);
	p = (HCHAR*)THAPI_GetThumbnailURL(szFullPath, lastplaytime, szMountPath, FALSE);
	if(p)
	{
		if(HLIB_FILE_Exist((HUINT8 *)p)==FALSE)	// TODO: 여기서 이렇게 싱크콜 하면, 레코딩 많으면 넘 느려져요 ㅠㅠ
		{
			if(pstRecData->nEventId != 0)
			{
#ifndef CONFIG_FUNC_EPG_USE_SVCUID
				OPL_Channel_t	stChannel;
				stChannel = OPL_Channel_FindChannelByUID (NULL, pstRecData->svcUId);
				pstEvent = APK_META_QUERY_GetEventWithIds(OPL_Channel_GetOrgNetworkID (stChannel), OPL_Channel_GetTransponderID (stChannel), OPL_Channel_GetServiceID (stChannel), pstRecData->nEventId);
#else
				pstEvent = APK_META_QUERY_GetIpEvent (pstRecData->svcUId, pstRecData->nEventId);
#endif
				if(pstEvent)
				{
					if(pstEvent->extension.ipEpg.thumbnailURL)
					{
#if defined(CONFIG_SUPPORT_IPEPG_CH_LOGO_STORE_HDD)
						p = (HCHAR*)THAPI_GetThumbnailURL(pstEvent->extension.ipEpg.thumbnailURL, 0, "/mnt/hd1", FALSE);
#else
						p = (HCHAR*)THAPI_GetThumbnailURL(pstEvent->extension.ipEpg.thumbnailURL, 0, "/var/lib/humaxtv_user", FALSE);
#endif
						if (p)
						{
							return HLIB_STD_StrDup(p);
						}
					}
				}
			}
			HxLOG_Print("Null Return.\n");
			return NULL;
		}
		return HLIB_STD_StrDup(p);
	}

	return NULL;
#else
	if (pstRecData)
	{
		return HLIB_STD_StrDup(pstRecData->szThumbnailPath);
	}

	return NULL;
#endif
}


static	int		opl_recording_GetAudioType(const DxRecListData_t	*pstRecData)
{
#if defined(CONFIG_OP_JAPAN)
	/* return vaule
		1: Mono
		2: Stereo
		4: Multi-Cannel
	*/
	if (pstRecData)
	{
		switch (pstRecData->extension.japan.stAribControlData.u8AudioComponentType)
		{
			case 0x01:	/* Audio, 1/0 mode (single mono)  */
			case 0x02:	/* Audio, 1/0+1/0 mode (dual mono) */
				return 1;
			case 0x03:	/* Audio, 2/0 mode (stereo) */
				return 2;
			case 0x04:
			case 0x05:
			case 0x06:
			case 0x07:
			case 0x08:
			case 0x09:
				return 4;
			default:
				return 1;
		}
	}
	return 1;	/* Stereo */
#else
	if (pstRecData)
	{
		return 2;//OPLPROGRAMME_AUDIOTYPE_STEREO;
	}

	return 1;//OPLPROGRAMME_AUDIOTYPE_MONO;
#endif
}


static	int		opl_recording_GetIsMultilingual(const DxRecListData_t	*pstRecData)
{
#if defined(CONFIG_OP_JAPAN)
	if (pstRecData)
	{
		/* MultiAudio */
		if (pstRecData->extension.japan.stAribControlData.u1MultiAudioComponent)
		{
			return TRUE;
		}
		else
		{
			/* Audio, 1/0+1/0 mode (dual mono) */
			if (pstRecData->extension.japan.stAribControlData.u8AudioComponentType == 0x02)
			{
				return TRUE;
			}
		}
	}
	return FALSE;
#else
	return FALSE;
#endif
}


static	OPL_Collection_t*		opl_recording_GetAudioLanguages(const DxRecListData_t	*pstRecData)
{
	OPL_Collection_t	*c = NULL;
	HUINT32				i, j;

	if(pstRecData == NULL)
	{
		return NULL;
	}

	c	= OPL_Collection_NewEx(HLIB_STD_MemFree_CB, NULL);
	for(i=0; i < pstRecData->stAudioInfoList.ulItemNum; i++)
	{
		DxMETA_PvrAudioInfo_t	*pstAudio = (DxMETA_PvrAudioInfo_t	*)&pstRecData->stAudioInfoList.astAudioInfo[i];

		for (j = 0; j < pstAudio->ulLangNum; ++j)
		{
			HINT32	nLangIdx = OPL_Collection_Find(c, pstAudio->stLangInfo[j].szIso639LangCode, opl_recording_CompLangCode);
			if (nLangIdx < 0)
			{	// not exist -> add
				OPL_Collection_Add(c, (void*)HLIB_STD_StrDup((const HCHAR*)pstAudio->stLangInfo[j].szIso639LangCode));
			}
		}
	}
	return c;
}

static	OPL_Collection_t*		opl_recording_GetGenres(const DxRecListData_t	*pstRecData)
{
#ifndef	CONFIG_OP_JAPAN
	OPL_Collection_t	*c = NULL;
	HUINT8			contentNibble =0;
	HUINT8			ucMajorNibble =0, ucMinorNibble =0;
	HINT32			sd = 0;

	HxLOG_Trace();

	if(pstRecData == NULL)
	{
		return NULL;
	}

	contentNibble = pstRecData->stEvt.ucContentNibble;

	ucMajorNibble = contentNibble >> 4;
	ucMinorNibble = contentNibble & 0xf;

	HxLOG_Print("[%s,%d] major : %d \n" , __FUNCTION__ , __LINE__, ucMajorNibble);
	HxLOG_Print("[%s,%d] minor : %d \n" , __FUNCTION__ , __LINE__, ucMinorNibble);

	sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	if( sd == 0 )
		return NULL;

	HLIB_RWSTREAM_Print(sd, "%x%x",  ucMajorNibble, ucMinorNibble );

	c = OPL_Collection_NewEx(HLIB_STD_MemFree_CB, (void *(*)(void *))HLIB_STD_StrDup_CB);
	OPL_Collection_Add(c, (void *)HLIB_STD_StrDup((HCHAR *)HLIB_RWSTREAM_CloseWithoutBuf(sd)));

	if (OPL_Collection_Length(c) == 0)
	{
		OPL_Collection_Delete(c);
		return NULL;
	}

	return c;
#else
	// TODO: get the real data
	return NULL;
#endif
}


static	int		opl_recording_GetRecordingStartTime(const DxRecListData_t	*pstRecData)
{
	if (pstRecData)
	{
		return pstRecData->ulRecStartTime;
	}

	return 0;
}


static	int		opl_recording_GetRecordingDuration(const DxRecListData_t	*pstRecData)
{
	if (pstRecData)
	{
		return pstRecData->ulRecDuration;
	}

	return 0;
}

static	int		opl_recording_GetCopyControl(const DxRecListData_t	*pstRecData)
{
	eOplRecordingCopyControlType eCopyControl = OPLRECORDING_COPYCONTROL_COPYPROHIBITED;

	HxLOG_Trace();

#if defined(CONFIG_OP_UK_DTT)
	if (pstRecData == NULL)
	{
		HxLOG_Error("invalid param. null input \n");
		return OPLRECORDING_COPYCONTROL_COPYOK;
	}

	/* copy protection */
	if ((DxUkdtt_PVR_FtaContentMgmtState_e)pstRecData->extension.ukdtt.ulFtaCmVal == eDxUkdtt_CONTENT_MGMT_STATE_Reserved
		|| ((DxUkdtt_PVR_FtaContentMgmtState_e)pstRecData->extension.ukdtt.ulFtaCmVal ==  eDxUkdtt_CONTENT_MGMT_STATE_ManagedCopy
			&& pstRecData->extension.ukdtt.ucCopyTime == 0))
	{
		eCopyControl = OPLRECORDING_COPYCONTROL_COPYPROHIBITED;
	}
	else
	{
		eCopyControl = OPLRECORDING_COPYCONTROL_COPYOK;
	}
#else
	if((pstRecData == NULL) || pstRecData->bScrambled)
	{
		return OPLRECORDING_COPYCONTROL_COPYPROHIBITED;
	}

	switch(pstRecData->stMeta.stCcInfo.stCasCcInfo.eCasCcType)
	{
		case eDxCopyrightControl_CasCcType_CiPlus:
			if(pstRecData->stMeta.stCcInfo.stCasCcInfo.info.stCiPlusDrmInfo.eCiPlusDrmType != eDxCopyrightControl_CiPlusDrmType_Uri)
			{
				HxLOG_Error("[%s,%d]\n" , __FUNCTION__, __LINE__);
				eCopyControl = OPLRECORDING_COPYCONTROL_COPYPROHIBITED;
			}
			else
			{
				//ucRct 0: Copy never, 1: refre ucEmi
				//ucEmi 0: Copying OK, 1: No further Copy, 2: Copy Once, 3: Copy Prohibited
				if((pstRecData->stMeta.stCcInfo.stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ucRct == 1) &&
					(pstRecData->stMeta.stCcInfo.stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ucEmi == 0))
				{
					HxLOG_Print("[%s,%d] copy available\n" , __FUNCTION__ , __LINE__);
					eCopyControl = OPLRECORDING_COPYCONTROL_COPYOK;
				}
				else
				{
					HxLOG_Print("[%s,%d] copy prohibited\n" , __FUNCTION__ , __LINE__);
					eCopyControl = OPLRECORDING_COPYCONTROL_COPYPROHIBITED;
				}
			}
			break;
		default:
			if(pstRecData->stMeta.stCcInfo.stCasCcInfo.eCasCcType != eDxCopyrightControl_CasCcType_None)
			{
				HxLOG_Print("[%s,%d] copy prohibited\n" , __FUNCTION__ , __LINE__);
				eCopyControl = OPLRECORDING_COPYCONTROL_COPYPROHIBITED;
			}
			else
			{
				HxLOG_Print("[%s,%d] copy available\n" , __FUNCTION__ , __LINE__);
				eCopyControl = OPLRECORDING_COPYCONTROL_COPYOK;
			}
			break;
	}
#endif
	return (int)eCopyControl;
}

static	int		opl_recording_GetCasType(const DxRecListData_t	*pstRecData)
{
	int casType;

	HxLOG_Trace();

	casType = pstRecData->eCasType;

	return casType;
}

static	int		opl_recording_GetCasIDs(OPL_Recording_t *recording, eOplRecordingCasType **casIDs)
{
	eOplRecordingCasType	 tmpCasIDArray[32];
	int						 tmpCasIdNum = 0;

	if (recording->casType & eDxCAS_TYPE_NAGRA)
        tmpCasIDArray[tmpCasIdNum++] = OPLRECORDING_CASTYPE_NAGRA;
    if (recording->casType & eDxCAS_TYPE_IRDETO)
        tmpCasIDArray[tmpCasIdNum++] = OPLRECORDING_CASTYPE_IRDETO;
    if (recording->casType & eDxCAS_TYPE_NDS)
        tmpCasIDArray[tmpCasIdNum++] = OPLRECORDING_CASTYPE_NDS;
    if (recording->casType & eDxCAS_TYPE_VIACCESS)
        tmpCasIDArray[tmpCasIdNum++] = OPLRECORDING_CASTYPE_VIACCESS;
    if (recording->casType & eDxCAS_TYPE_CONAX)
        tmpCasIDArray[tmpCasIdNum++] = OPLRECORDING_CASTYPE_CONAX;
    if (recording->casType & eDxCAS_TYPE_MEDIAGUARD)
        tmpCasIDArray[tmpCasIdNum++] = OPLRECORDING_CASTYPE_MEDIAGUARD;
    if (recording->casType & eDxCAS_TYPE_CRYPTOWORKS)
        tmpCasIDArray[tmpCasIdNum++] = OPLRECORDING_CASTYPE_CRYPTOWORKS;
    if (recording->casType & eDxCAS_TYPE_JPBCAS)
        tmpCasIDArray[tmpCasIdNum++] = OPLRECORDING_CASTYPE_JPBCAS;
	if (recording->casType & eDxCAS_TYPE_JPCCAS)
        tmpCasIDArray[tmpCasIdNum++] = OPLRECORDING_CASTYPE_JPCCAS;
    if (recording->casType & eDxCAS_TYPE_ALLCAS)
        tmpCasIDArray[tmpCasIdNum++] = OPLRECORDING_CASTYPE_ALLCAS;
	if (recording->casType & eDxCAS_TYPE_VERIMATRIX)
        tmpCasIDArray[tmpCasIdNum++] = OPLRECORDING_CASTYPE_VERIMATRIX;


	if(0 < tmpCasIdNum)
	{
		*casIDs = (int*)HLIB_STD_MemAlloc(sizeof(int)*tmpCasIdNum);

		if (*casIDs != NULL)
        {
            memcpy(*casIDs, tmpCasIDArray, sizeof(OPL_ChannelCasID_e) * tmpCasIdNum);
        }
	}
	else
	{
		*casIDs = NULL;
	}

	return	tmpCasIdNum;
}


OPL_Collection_t	*opl_recording_GetGroupCRIDs(const DxRecListData_t	*pstRecData)
{
	OPL_Collection_t	*coll= NULL;
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	HINT32				i;
#endif

	HxLOG_Trace();
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	coll = OPL_Collection_NewEx(HLIB_STD_MemFree_CB, (void *(*)(void *))HLIB_STD_StrDup_CB);

	if(coll == NULL)
	{
		HxLOG_Critical("[%s:%d] Out of memory!\n", __HxFILE__, __HxLINE__);
		return NULL;
	}

	for(i=0; i< DxMAX_CRID_DATA_NUM;i++)
	{
		DxPVR_TvaCrid_t Crid = pstRecData->stMeta.astTvaCrids[i];
		HxLOG_Print("[%s:%d]TYPE[0x%x, %s]\n",__FUNCTION__,__LINE__,Crid.ucType ,Crid.szCrid );
		if(Crid.ucType == DxCRID_TYPE_SERIES) // series crid
			(void) OPL_Collection_Add(coll, (void *)HLIB_STD_StrDup((HCHAR *)Crid.szCrid));
	}

	if (OPL_Collection_Length(coll) == 0)
	{
		OPL_Collection_Delete(coll);
		return NULL;
	}
#endif
	return coll;
}

HCHAR* 	opl_recording_GetProgrammeCRID(const DxRecListData_t	*pstRecData)
{
#if defined (CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	HINT32				i;

	HxLOG_Trace();

	for(i=0; i< DxMAX_CRID_DATA_NUM;i++)
	{
		DxPVR_TvaCrid_t Crid = pstRecData->stMeta.astTvaCrids[i];
		HxLOG_Print("[%s:%d]TYPE[0x%x, %s]\n",__FUNCTION__,__LINE__,Crid.ucType ,Crid.szCrid );
		if(Crid.ucType == DxCRID_TYPE_PROG) // programme crid
			return HLIB_STD_StrDup((HCHAR *)Crid.szCrid);
	}
#else
	HxLOG_Trace();

	return NULL;
#endif
	return NULL;
}

HCHAR* 	opl_recording_GetSeriesTitle(const DxRecListData_t	*pstRecData)
{
#if defined (CONFIG_OP_PVR_BASED_ON_TVANYTIME) && defined( CONFIG_OP_TDC )
	if (pstRecData)
	{
		return HLIB_STD_StrDup(pstRecData->stMeta.szSeriesTitle);
	}
#else
	HxLOG_Trace();
#endif

	return NULL;
}

static HCHAR *	opl_recording_MakeValidJsonTxt(const HCHAR *jsonTxt)
{
	static const HCHAR *	s_jsonSkip = "\"\\/bfnrtu";	///< @see json.org

	static const HUINT32 utf8_skip_data[256] = {
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
		3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,1,1
	};

	HUINT32	utfSkip, i, jsonTxtLen;
	HINT32	sd = 0;

	if (NULL == jsonTxt)
		return NULL;

	sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	if (!sd)
		return NULL;


	jsonTxtLen = HxSTD_StrLen(jsonTxt);
	for (i = 0; i < jsonTxtLen; ++i)
	{
		HBOOL	bNeedPutNextChar = FALSE;
		HUINT8	jsonCh 	= jsonTxt[i];
		utfSkip 		= utf8_skip_data[jsonCh];

		if (1 == utfSkip)
		{
			if (jsonCh < 0x20)
			{	// control char => just skip
				continue;
			}

			// '\\' or '\"' cannot be between quotations " ", for the exception cases
			if (jsonCh == '\\')
			{	// '\' => check the valid cases
				if (strchr(s_jsonSkip, jsonTxt[i + 1]))
				{	// skip the valid cases (\", \\, \/, \b, \f, \n, \r, \t, \u)
					++i;
					bNeedPutNextChar = TRUE;
				}
				else
				{	// put the char '\\' to make valid text ('\\\\') -> reverse solidus
					HLIB_RWSTREAM_PutChar(sd, '\\');
				}
			}

			else if (jsonCh == '\"')
			{	// " => put the char '\\' to make valid text ('\\\"') -> quotation mark
				HLIB_RWSTREAM_PutChar(sd, '\\');
			}
		}

		HLIB_RWSTREAM_PutChar(sd, jsonCh);
		if (bNeedPutNextChar)
			HLIB_RWSTREAM_PutChar(sd, jsonTxt[i+1]);
	}

	return HLIB_RWSTREAM_CloseWithoutBuf(sd);
}

HCHAR*	opl_recording_MakeUkdttGuidanceInfo(const DxRecListData_t *pstRecData)
{
	const DxUkdtt_Pvr_Guidance_t	*pstUkdttPvrGuidance;
	HUINT32					ulJsonValidType;
	HUINT32					ulJsonValidMode;
	HCHAR					*pszJsonValidGuidance;
	HCHAR					szJsonGuidanceText[OPLRECORDING_MAX_STRCOLLECTION_LEN];

	pstUkdttPvrGuidance		= &(pstRecData->extension.ukdtt.stEvt.stGuidance);
	if (HxSTD_StrEmpty((pstUkdttPvrGuidance->stData)))
	{
		return NULL;
	}

	ulJsonValidType			= pstUkdttPvrGuidance->eType;
	ulJsonValidMode			= pstUkdttPvrGuidance->ucMode;
	pszJsonValidGuidance	= opl_recording_MakeValidJsonTxt((const HCHAR *)pstUkdttPvrGuidance->stData);

	HxSTD_snprintf(szJsonGuidanceText,OPLRECORDING_MAX_STRCOLLECTION_LEN,"{\"guidance\":[{\"type\":%d,\"mode\":%d,\"text\":\"%s\"}]}"
		,ulJsonValidType,ulJsonValidMode,pszJsonValidGuidance);

	HLIB_STD_MemFree(pszJsonValidGuidance);

	return HLIB_STD_StrDup(szJsonGuidanceText);
}
HCHAR* 	opl_recording_GetGuidance(const DxRecListData_t	*pstRecData)
{
#if !defined(CONFIG_OP_UK_DTT)
	return NULL;
#else
	HCHAR					*szJsonGuidanceText;

	if(pstRecData == NULL)
	{
		return NULL;
	}

	szJsonGuidanceText = opl_recording_MakeUkdttGuidanceInfo(pstRecData);
	if (szJsonGuidanceText == NULL)
	{
		return NULL;
	}

	return szJsonGuidanceText;
#endif
}


OPL_Collection_t		*opl_recording_GetSubtitleTypes(const DxRecListData_t	*pstRecData)
{
	OPL_Collection_t	*coll = NULL;
	HINT32				i,type;

	HxLOG_Trace();

	coll = OPL_Collection_NewEx(HLIB_STD_MemFree_CB, (void *(*)(void *))HLIB_STD_StrDup_CB);
	if(coll == NULL)
	{
		HxLOG_Critical("[%s:%d] Out of memory!\n", __HxFILE__, __HxLINE__);
		return NULL;
	}

	for (i = 0 ; i < pstRecData->stSbtlInfoList.ulItemNum; i++)
	{
		type = pstRecData->stSbtlInfoList.astSbtlInfo[i].ucComponentType;
		switch(type)
		{

		case 0x02://TELETEXT_SUBTITLE :		/* EBU subtitle */
		case 0x05://TTX_SBTL_PAGE_HERARING_IMPAIRED :
			(void) OPL_Collection_Add(coll, (void *)HLIB_STD_StrDup("EBU"));
			break;
		 /*
	     *  DVB subtitle의 type 값은 Component descriptor의 Stream_context(0x03) & componet_type Table
	     *  값을 따른다. 여기서 componet_type 0x01, 0x02는 EBU subtitle이고, 0x10~0x13, 0x20~0x23은 DVB로
	     *  되어 있는데 실제 EBU subtitle은 Subtitle descriptor를 통해서 들어 오는 것이 아니라 Teletext
	     *  descriptor에서 type value 0x02로 들어 오는 경우만 있었으므로 위에 0x01, 0x02s는 component descriptor에
	     *  값과 다른 의미다 일단 0x2만 들어온다고 보고 이 경우만 처리하게 한다.
	     *  0x05(Teletext subtitle page for hearing impaired people)에 대한 처리도 추가 한다.
	     */
		/* Normal */
		case 0x10 :
		case 0x11 :
		case 0x12 :
		case 0x13 :
		case 0x14 : 	/* Freesat HD stream에서 여기로 들어 옴. */

		/* For the hard of hearing */
		case 0x20 :
		case 0x21 :
		case 0x22 :
		case 0x23 :
		case 0x24 :
			(void) OPL_Collection_Add(coll, (void *)HLIB_STD_StrDup("DVB"));
			break;
		}
	}

	if (OPL_Collection_Length(coll) == 0)
	{
		OPL_Collection_Delete(coll);
		return NULL;
	}
	return coll;
}


static	OPL_Bookmark_t*		opl_recording_MakeBookmark(const DxRecListData_t	*pstRecData, HUINT16	index)
{
	OPL_Bookmark_t	*bookmark = NULL;
	if (NULL == pstRecData)
	{
		return NULL;
	}

	if (pstRecData->usNumOfBookmark <= index)
	{
		return NULL;
	}

	bookmark = OPL_Bookmark_New(pstRecData->astBookmarks[index].ulPos, pstRecData->astBookmarks[index].szName);
	return bookmark;
}


static	OPL_Collection_t* opl_recording_MakeBookmarkCollection(const DxRecListData_t	*pstRecData)
{
	OPL_Collection_t	*c;
	OPL_Bookmark_t		*b;
	HUINT16	usNumOfBookmark = 0, i;

	if (NULL == pstRecData)
	{
		return NULL;
	}

	usNumOfBookmark	= pstRecData->usNumOfBookmark;

	c = OPL_Collection_NewEx((void (*)(void *))OPL_Bookmark_Delete, (void *(*)(void *))OPL_Bookmark_Clone);
	if (c == NULL)
	{
		HxLOG_Critical("[%s:%d] Out of memory!\n", __HxFILE__, __HxLINE__);
		return NULL;
	}

	for (i = 0 ; i < usNumOfBookmark ; i++)
	{
		b = opl_recording_MakeBookmark(pstRecData, i);
		if (b == NULL)
			continue;

		OPL_Collection_Add(c, (void *)b);
	}
	return c;
}


static	int opl_recording_ConstructScheduledInfo_LongDescription(HCHAR	**ppszLongDesc,
	HUINT32 ulExtCnt, DxRecList_ExtEvt_t	*pstExtEvts, HCHAR *pszExtText)
{
	if (NULL == ppszLongDesc)
	{
		return -1;
	}

	// TODO : desc & item + '\n'+ text
	/*if (ulExtCnt)
	{
		HUINT32	i, k;

		for (i = 0; i < ulExtCnt; ++i)
		{
			DxRecList_ExtEvt_t	*pstExtEvt = &pstExtEvts[i];
			for (k = 0; k < pstExtEvt->ucItemNum; ++k)
			{
				HUINT32	ulDescLen = HxSTD_StrLen(pstExtEvt->astItem[k].aucDescription);
				HUINT32	ulItemLen = HxSTD_StrLen(pstExtEvt->astItem[k].aucItem);

				if (ulDescLen)
				{
					HxSTD_PrintToStrCat(*ppszLongDesc, APKD_RECLIST_EXT_TEXT_BUFSIZE, "%s",
							pstExtEvt->astItem[k].aucDescription);
				}


				if (ulItemLen)
				{
					if (ulDescLen)
					{
						HxSTD_PrintToStrCat(*ppszLongDesc, APKD_RECLIST_EXT_TEXT_BUFSIZE, ":%s",
							pstExtEvt->astItem[k].aucItem);
					}
					else
					{
						HxSTD_PrintToStrCat(*ppszLongDesc, APKD_RECLIST_EXT_TEXT_BUFSIZE, "%s",
							pstExtEvt->astItem[k].aucItem);
					}
			}	// end for k
		}	// end for i
	}
	//*/

	if (pszExtText)
	{
		*ppszLongDesc = HLIB_STD_StrDup(pszExtText);
	}

	return 0;
}

#if defined(CONFIG_OP_JAPAN) || defined(CONFIG_OP_FREESAT)
static	int opl_recording_ConstructScheduledInfo_episode(HUINT32 ulExtCnt, DxRecList_ExtEvt_t *pstExtEvts)
{
	HUINT32	i, k;
	HUINT32	value;

	if (ulExtCnt)
	{
		for (i = 0; i < ulExtCnt; ++i)
		{
			DxRecList_ExtEvt_t	*pstExtEvt = &pstExtEvts[i];
			for (k = 0; k < pstExtEvt->ucItemNum; ++k)
			{
				HUINT32	ulDescLen = HxSTD_StrLen(pstExtEvt->astItem[k].aucDescription);
				HUINT32	ulItemLen = HxSTD_StrLen(pstExtEvt->astItem[k].aucItem);

				if (ulDescLen && ulItemLen)
				{
					if (strcmp(pstExtEvt->astItem[k].aucDescription, "EpisodeNumber") == 0)
					{
						value = atoi(pstExtEvt->astItem[k].aucItem);
						return value;
					}
				}
			}
		}
	}

	return 0;
}
#endif //defined(CONFIG_OP_JAPAN) || defined(CONFIG_OP_FREESAT)
static OPL_Collection_t*  opl_recording_MakeExtendedEventInfo(const DxRecListData_t	*pstRecData)
{
	OPL_Collection_t	*c = NULL;
	HUINT32			i;
	char				*pStr = NULL;

	if(pstRecData == NULL || pstRecData->pstExtEvts == NULL)
	{
		return NULL;
	}

	c	= OPL_Collection_NewEx(HLIB_STD_MemFree_CB, NULL);

	// just use first extend event table
	HxLOG_Print("[%s : %d] pstRecData->pstExtEvts->ucItemNum (%d), pstExtEvts 0x(%x) pstExtEvts[0] 0x(%x)\n"
		, __FUNCTION__, __LINE__, pstRecData->pstExtEvts->ucItemNum, pstRecData->pstExtEvts, &pstRecData->pstExtEvts[0]);
	for(i=0; i<pstRecData->pstExtEvts->ucItemNum; i++)
	{
		pStr = (char*)HLIB_STD_MemCalloc(sizeof(char) * (OPLRECORDING_MAX_STRCOLLECTION_LEN + 30 + 1));
		if (pStr)
		{   // + 30 for the json ({\"desc\"...
			HxSTD_PrintToStrN(pStr, OPLRECORDING_MAX_STRCOLLECTION_LEN + 30, "{\"desc\":\"%s\",\"char\":\"%s\"}",
				pstRecData->pstExtEvts->astItem[i].aucDescription, pstRecData->pstExtEvts->astItem[i].aucItem);
			HxLOG_Print("\033[30;43m[%s : %d] extended info : %s  \033[0m\n", __FUNCTION__, __LINE__, pStr);
			OPL_Collection_Add(c, pStr);
		}
	}

	return c;
}


static OPL_Collection_t*  opl_recording_MakeComponentsInfo(const DxRecListData_t	*pstRecData)
{
	OPL_Collection_t	*c = NULL;
	HUINT32			i;
	DxPvr_EvtComponent_t	*component = NULL;

	if(pstRecData == NULL)
	{
		return NULL;
	}

	c	= OPL_Collection_NewEx(HLIB_STD_MemFree_CB, NULL);

	for(i=0; i < DxEPG_COMPONENT_DESC_MAX; i++)
	{
		DxPvr_EvtComponent_t	*pstEvtComp = (DxPvr_EvtComponent_t	*)&pstRecData->stEvt.stComponent[i];

		if (pstEvtComp->ucStreamContent == 0)
		{
			HxLOG_Print("[%s:%d] (%d) component ! \n", __FUNCTION__, __LINE__, i);
			break;
		}

		component = (DxPvr_EvtComponent_t *)HLIB_STD_MemDup(pstEvtComp, sizeof(DxPvr_EvtComponent_t));
		if (component)
		{
			OPL_Collection_Add(c, component);
		}
	}

	return c;
}

static	int		opl_recording_GetScrambled(const DxRecListData_t *pstRecData)
{
	if (pstRecData)
	{
		return pstRecData->bScrambled;
	}

	return -1;
}

static	int		opl_recording_GetEncrypted(const DxRecListData_t *pstRecData)
{
#if defined(CONFIG_OP_UK_DTT)
	if (pstRecData)
	{
		if (pstRecData->extension.ukdtt.ulFtaCmVal == eDxUkdtt_CONTENT_MGMT_STATE_None)
		{
			return OPL_FALSE;
		}
		else
		{
			return OPL_TRUE;
		}
	}
	return OPL_FALSE;
#else
	return OPL_FALSE;
#endif
}

static	unsigned int		opl_recording_GetExpiryDate(const DxRecListData_t *pstRecData)
{
#if defined(CONFIG_CAS_CONAX)
	eDxCopyrightControl_ConaxRetentionLimit_e eCxRetLimit = 0;
	HUINT32		ulExpireDuration = 0;
	HUINT32		ulStartTime = pstRecData->ulRecStartTime;

#if 0 // TODO: 아래를 열기 위해서는 먼저 FTA인 경우에 대해서 처리해줘야 한다.
	if (pstRecData->stMeta.stCcInfo.stCasCcInfo.eCasCcType != eDxCopyrightControl_CasCcType_Conax)
	{
		// expired
		return 0;
	}
#endif

	eCxRetLimit = pstRecData->stMeta.stCcInfo.stCasCcInfo.info.stConaxDrmInfo.eRetentionLimit;

	if ((eCxRetLimit == eDxCopyrightControl_ConaxRetentionLimit_CopyFree_NoLimit) 	// ----- EMI == [ Copy Freely ] -----
		|| (eCxRetLimit == eDxCopyrightControl_ConaxRetentionLimit_NoLimit)		// ----- EMI == [ Copy Never ] or [ Copy No More ] -----
		|| (eCxRetLimit == eDxCopyrightControl_ConaxRetentionLimit_NoExpLimit))	// ----- EMI == [ Copy Once ] -----
	{
		// not expired
		return 0xFFFFFFFF;
	}

	switch (eCxRetLimit)
	{
		// ----- EMI == [ Copy Never ] or [ Copy No More ] -----
	case eDxCopyrightControl_ConaxRetentionLimit_1Week:
		ulExpireDuration = SECONDS_PER_WEEK;
		break;

	case eDxCopyrightControl_ConaxRetentionLimit_2Days:
		ulExpireDuration = SECONDS_PER_DAY * 2;
		break;

	case eDxCopyrightControl_ConaxRetentionLimit_1Day:
		ulExpireDuration = SECONDS_PER_DAY;
		break;

	case eDxCopyrightControl_ConaxRetentionLimit_12Hours:
		ulExpireDuration = SECONDS_PER_HOUR * 12;
		break;

	case eDxCopyrightControl_ConaxRetentionLimit_6Hours:
		ulExpireDuration = SECONDS_PER_HOUR * 6;
		break;

	case eDxCopyrightControl_ConaxRetentionLimit_3Hours:
		ulExpireDuration = SECONDS_PER_HOUR * 3;
		break;

	case eDxCopyrightControl_ConaxRetentionLimit_90Min:
		ulExpireDuration = SECONDS_PER_MIN * 90;
		break;

		// ----- EMI == [ Copy Once ] -----
	case eDxCopyrightControl_ConaxRetentionLimit_ExpDays_360:
		ulExpireDuration = SECONDS_PER_DAY * 360;
		break;

	case eDxCopyrightControl_ConaxRetentionLimit_ExpDays_90:
		ulExpireDuration = SECONDS_PER_DAY * 90;
		break;

	case eDxCopyrightControl_ConaxRetentionLimit_ExpDays_30:
		ulExpireDuration = SECONDS_PER_DAY * 30;
		break;

	case eDxCopyrightControl_ConaxRetentionLimit_ExpDays_14:
		ulExpireDuration = SECONDS_PER_DAY * 14;
		break;

	case eDxCopyrightControl_ConaxRetentionLimit_ExpDays_7:
		ulExpireDuration = SECONDS_PER_DAY * 7;
		break;

	case eDxCopyrightControl_ConaxRetentionLimit_ExpDays_2:
		ulExpireDuration = SECONDS_PER_DAY * 2;
		break;

	case eDxCopyrightControl_ConaxRetentionLimit_ExpDays_1:
		ulExpireDuration = SECONDS_PER_DAY;
		break;

		// ----- EMI == Exception Case...
	default:
		break;
	}

	return ulStartTime + ulExpireDuration;
#else
	// not expired
	return 0xFFFFFFFF;
#endif
}
static	OPL_Bookmark_t*		opl_recording_MakePausedPoint(const DxRecListData_t	*pstRecData, HUINT16	index)
{
	OPL_Bookmark_t	*bookmark = NULL;
	if (NULL == pstRecData)
	{
		return NULL;
	}

	if (pstRecData->usNumOfPausedPoints <= index)
	{
		return NULL;
	}

	bookmark = OPL_Bookmark_New(pstRecData->astPausedPoints[index].ulPos, pstRecData->astPausedPoints[index].szName);
	return bookmark;
}

static	OPL_Collection_t* opl_recording_MakePausedPointsCollection(const DxRecListData_t	*pstRecData)
{
	OPL_Collection_t	*c;
	OPL_Bookmark_t		*b;
	HUINT16	usNumOfPausedPoints = 0, i;

	if (NULL == pstRecData)
	{
		return NULL;
	}

	usNumOfPausedPoints	= pstRecData->usNumOfPausedPoints;

	c = OPL_Collection_NewEx((void (*)(void *))OPL_Bookmark_Delete, (void *(*)(void *))OPL_Bookmark_Clone);
	if (c == NULL)
	{
		HxLOG_Critical("[%s:%d] Out of memory!\n", __HxFILE__, __HxLINE__);
		return NULL;
	}

	for (i = 0 ; i < usNumOfPausedPoints ; i++)
	{
		b = opl_recording_MakePausedPoint(pstRecData, i);
		if (b == NULL)
			continue;

		OPL_Collection_Add(c, (void *)b);
	}
	return c;
}

#if defined(CONFIG_OP_JAPAN)
static	int		opl_recording_GetRecordingDigitalCopyControl(const DxRecListData_t	*pstRecData)
{
	if (pstRecData)
	{
		return pstRecData->extension.japan.ulDigitalCopyControl;
	}

	return 0;
}
static	int		opl_recording_GetRecordingAnalogCopyControl(const DxRecListData_t	*pstRecData)
{
	if (pstRecData)
	{
		return pstRecData->extension.japan.ulAnalogCopyControl;
	}

	return 0;
}
static	int		opl_recording_GetRecordingDigitalCopyCount(const DxRecListData_t	*pstRecData)
{
	if (pstRecData)
	{
		return pstRecData->extension.japan.ulDigitalCopyCount;
	}

	return 0;
}
static	int		opl_recording_GetRecordingParentalRating(const DxRecListData_t	*pstRecData)
{
	if (pstRecData)
	{
		return pstRecData->extension.japan.ucParentalRating;
	}

	return 0;
}
static	int		opl_recording_GetRecordingIsTranscoded(const DxRecListData_t	*pstRecData)
{
	if (pstRecData)
	{
		return pstRecData->bTranscoded;
	}

	return 0;
}


static	int		opl_recording_GetRecordingIsWatched(const DxRecListData_t	*pstRecData)
{
	if (pstRecData)
	{
		return pstRecData->bPlayed;
	}

	return 0;
}


static	OPL_Bookmark_t*		opl_recording_MakeChapter(const DxRecListData_t	*pstRecData, HUINT16	index)
{
	OPL_Bookmark_t	*chapter = NULL;
	if (NULL == pstRecData)
	{
		return NULL;
	}

	if (pstRecData->extension.japan.usNumOfChapter <= index)
	{
		return NULL;
	}

	chapter = OPL_Bookmark_New(pstRecData->extension.japan.astChapters[index].ulPos
		, pstRecData->extension.japan.astChapters[index].szName);
	return chapter;
}

static	OPL_Collection_t* opl_recording_MakeChapterCollection(const DxRecListData_t	*pstRecData)
{
	OPL_Collection_t	*c;
	OPL_Bookmark_t		*b;
	HUINT16	usNumOfChapter = 0, i;

	if (NULL == pstRecData)
	{
		return NULL;
	}

	usNumOfChapter	= pstRecData->extension.japan.usNumOfChapter;

	c = OPL_Collection_NewEx((void (*)(void *))OPL_Bookmark_Delete, (void *(*)(void *))OPL_Bookmark_Clone);
	if (c == NULL)
	{
		HxLOG_Critical("[%s:%d] Out of memory!\n", __HxFILE__, __HxLINE__);
		return NULL;
	}

	for (i = 0 ; i < usNumOfChapter ; i++)
	{
		b = opl_recording_MakeChapter(pstRecData, i);
		if (b == NULL)
			continue;

		OPL_Collection_Add(c, (void *)b);
	}
	return c;
}

#endif


#ifdef	CONFIG_OP_FREESAT
static OPL_Collection_t*  opl_recording_MakeFreesatGuidanceInfo(const DxRecListData_t	*pstRecData)
{
	OPL_Collection_t	*c = NULL;
	char				*pStr = NULL;
	DxFreesat_Pvr_Guidance_t	*pstFsatPvrGuidance;

	if(pstRecData == NULL)
	{
		return NULL;
	}

	c	= OPL_Collection_NewEx(HLIB_STD_MemFree_CB, NULL);
	pStr = (char*)HLIB_STD_MemCalloc(sizeof(char) * (OPLRECORDING_MAX_STRCOLLECTION_LEN + 1));
	if (pStr)
	{
		pstFsatPvrGuidance = &(pstRecData->extension.freesat.stEvt.astGuidance);

		HxSTD_PrintToStrN(pStr, OPLRECORDING_MAX_STRCOLLECTION_LEN, "{\"countrycode\":\"%s\",\"text\":\"%s\"}"
			, pstFsatPvrGuidance->stLangCode, pstFsatPvrGuidance->stData);
		HxLOG_Print("[%s : %d] str(%s)\n", __FUNCTION__, __LINE__, pStr);
		OPL_Collection_Add(c, pStr);
	}
	return c;
}
#endif // #ifdef	CONFIG_OP_FREESAT

static	int		opl_recording_GetIsManualLocked(const DxRecListData_t *pstRecData)
{
	if (pstRecData)
	{
		return pstRecData->bUserLock || pstRecData->bBlocked;
	}

	return -1;
}

static	char*	opl_recording_GetPath(const DxRecListData_t *pstRecData)
{
	if (pstRecData)
	{
		return HLIB_STD_StrDup(pstRecData->stMeta.szPath);
	}

	return NULL;
}

static	char*	opl_recording_GetFileName(const DxRecListData_t *pstRecData)
{
	if (pstRecData)
	{
		return HLIB_STD_StrDup(pstRecData->stMeta.szFileName);
	}

	return NULL;
}

static	int opl_recording_ConstructScheduledInfo(OPL_ScheduledItem_t	*pstScheduled,
														const DxRecListData_t *pstRecData)
{
	OPL_Channel_t		channel;

	if (NULL == pstScheduled || NULL == pstRecData)
	{
		HxLOG_Error("[%s:%d] NULL param!!!\n", __HxFILE__, __HxLINE__);
		return -1;
	}

	pstScheduled->name 			= HLIB_STD_StrDup(pstRecData->szName);
	pstScheduled->description	= HLIB_STD_StrDup(pstRecData->stEvt.aucText);
	HxLOG_Print("opl_recording_ConstructScheduledInfo descrition (%s)\n", pstRecData->stEvt.aucText);
	opl_recording_ConstructScheduledInfo_LongDescription(&pstScheduled->longDescription,
		pstRecData->ulExtEvtCnt,
		(DxRecList_ExtEvt_t *)pstRecData->pstExtEvts,
		(HCHAR *)pstRecData->aucExtText);

	HxLOG_Print("long descrition (%s)\n", pstScheduled->longDescription);
	// get the channel
	channel = OPL_Channel_FindChannelByUID(OPL_Channel_GetManager(), (int)pstRecData->svcUId);
	if (channel)
	{
		char	buf[256 + 1] = {0};

		pstScheduled->channelID 	= HLIB_STD_StrDup(OPL_Channel_GetCCID(channel, buf, 256));
		HxLOG_Print("[%s:%d] channel ID (%s)\n", __HxFILE__, __HxLINE__, pstScheduled->channelID);
	}
	else
	{	// exception -> can't find ... -;
		HxLOG_Error("[%s:%d]Cannot find channel by UID(%d)!\n", __HxFILE__, __HxLINE__, pstRecData->svcUId);
	}

	/* Factory Reset후 Ch DB가 없는 상태로 record list가 만들어진 상황에서 Ch가 검색되어 record list가 처리될 때의 예외 처리 */
	pstScheduled->serviceId = ((int)pstRecData->stMeta.usSvcId) & 0x0000ffff;		   	/* service id */
	pstScheduled->transportStreamId = ((int)pstRecData->stMeta.usTsId) & 0x0000ffff; 	/* transport stream id */
	pstScheduled->originalNetworkId = ((int)pstRecData->stMeta.usOnId) & 0x0000ffff; 	/* original network id */

	if (pstRecData->bEventBased)
	{	// make programmeID
		char	buf[128 + 1] = {0};

#ifndef CONFIG_FUNC_EPG_USE_SVCUID
		OPL_Channel_t channel = OPL_Channel_FindChannelByUID(NULL, pstRecData->svcUId);
		OPL_Programme_MakeProgrammeID(buf, 128 , OPL_Channel_GetOrgNetworkID(channel), OPL_Channel_GetTransponderID(channel), OPL_Channel_GetServiceID(channel), (int)pstRecData->nEventId);
#else
		OPL_Programme_MakeProgrammeID(buf, 128 , (int)pstRecData->svcUId, (int)pstRecData->nEventId);
#endif
		pstScheduled->programmeID = HLIB_STD_StrDup(buf);
	}

	// TODO : complete the scheduled data contruction
	pstScheduled->startPadding 			= 0;	// we don't save the padding time
	pstScheduled->endPadding			= 0;
	pstScheduled->startTime			= pstRecData->ulRecStartTime;
	pstScheduled->duration				= pstRecData->ulRecOrgDuration;
#if defined(CONFIG_OP_JAPAN)
	/* 미정 이벤트 녹화 duration 처리 */
	if(TRUE == pstRecData->bEventBased && 0 == pstRecData->stEvt.ulDuration && eDxREC_STS_RECORDING == pstRecData->eRecStatus)
	{
		pstScheduled->duration	= 0x00ffffff;
	}
#endif
	pstScheduled->isScheduledAsSeries	= pstRecData->bSeriesRec;

#if defined(CONFIG_OP_JAPAN) || defined(CONFIG_OP_FREESAT)
	pstScheduled->seriesID		= HLIB_STD_StrDup(pstRecData->stEvt.aucSeriesId);
	pstScheduled->episode		= opl_recording_ConstructScheduledInfo_episode(pstRecData->ulExtEvtCnt, (DxRecList_ExtEvt_t *)pstRecData->pstExtEvts);
#else //defined(CONFIG_OP_JAPAN) || defined(CONFIG_OP_FREESAT)
	if(pstScheduled->isScheduledAsSeries == OPL_TRUE)
	{
		pstScheduled->seriesID		= HLIB_STD_StrDup(pstRecData->stEvt.aucSeriesId);
		if(pstScheduled->seriesID)
		{
			pstScheduled->episode = pstRecData->stEvt.ulEpisodeNum;
			pstScheduled->season = pstRecData->stEvt.ulSeason;
		}
	}
	else
	{
		pstScheduled->seriesID = NULL;
		pstScheduled->episode = -1;
		pstScheduled->season = -1;
	}
#endif //defined(CONFIG_OP_JAPAN) || defined(CONFIG_OP_FREESAT)

	pstScheduled->storageID = HLIB_STD_StrDup(pstRecData->szStorageId);
	pstScheduled->recMode = pstRecData->ulRecMode;
#if defined(CONFIG_OP_JAPAN)
	pstScheduled->groupIndex= pstRecData->extension.japan.ulGroupIndex;
	pstScheduled->userID= HLIB_STD_StrDup((HCHAR*)pstRecData->extension.japan.szUserId);
#endif

	return 0;
}


static OPL_Recording_t*	opl_recording_MakeRecording (OPL_Recording_t *recording, const DxRecListData_t *recData)
{
	if (NULL == recording || NULL == recData)
	{
		HxLOG_Error("[%s:%d] NULL param!!!\n", __HxFILE__, __HxLINE__);
		return NULL;
	}

	// make ScheduledProgramm_t data (super)
	opl_recording_ConstructScheduledInfo(&recording->super, recData);

	// make recording data
	recording->state= opl_recording_GetState(recData);
	recording->error= opl_recording_GetError(recData);
	recording->id	= opl_recording_GetID(recData);
	recording->isManual		= opl_recording_GetIsManual(recData);
	recording->doNotDelete	= opl_recording_GetDoNotDelete(recData);
	recording->doNotDeleteSetTime	= opl_recording_GetDoNotDeleteSetTime(recData);
	recording->willBeDeleted= opl_recording_GetWillBeDeleted(recData);
	recording->saveDays		= opl_recording_GetSaveDays(recData);
	recording->saveEpisodes	= opl_recording_GetSaveEpisodes(recData);
	recording->blocked		= opl_recording_GetBlocked(recData);
	recording->locked		= opl_recording_GetLocked(recData);
	recording->showType		= opl_recording_GetShowType(recData);
	recording->subtitles		= opl_recording_GetSubtitles(recData);
	recording->subtitleLanguages= opl_recording_GetSubtitleLanguages(recData);
	recording->isHD			= opl_recording_GetIsHD(recData);
	recording->isWideScreen	= opl_recording_GetIsWidescreen(recData);
	recording->audioType		= opl_recording_GetAudioType(recData);
	recording->isMultilingual	= opl_recording_GetIsMultilingual(recData);
	recording->audioLanguages	= opl_recording_GetAudioLanguages(recData);
	recording->genres			= opl_recording_GetGenres(recData);
	recording->recordingStartTime	= opl_recording_GetRecordingStartTime(recData);
	recording->recordingDuration	= opl_recording_GetRecordingDuration(recData);
	recording->bookmarks		= opl_recording_MakeBookmarkCollection(recData);
	recording->lastPlayPosition	= opl_recording_GetLastPlayPosition(recData);
	recording->lcn				= opl_recording_Getlcn(recData);
	recording->thumbnail		= opl_recording_GetThumbnail(recData);
	recording->extEventInfo		= opl_recording_MakeExtendedEventInfo(recData);
	recording->components 		= opl_recording_MakeComponentsInfo(recData);
	recording->isScrambled		= opl_recording_GetScrambled(recData);
	recording->isEncrypted		= opl_recording_GetEncrypted(recData);
	recording->copyControl		= opl_recording_GetCopyControl(recData);
	recording->casType			= opl_recording_GetCasType(recData);
	recording->expiryDate		= opl_recording_GetExpiryDate(recData);
	recording->pausedPoints		= opl_recording_MakePausedPointsCollection(recData);

#if defined(CONFIG_OP_JAPAN)
	recording->digitalCopyControl = opl_recording_GetRecordingDigitalCopyControl(recData);
	recording->analogCopyControl = opl_recording_GetRecordingAnalogCopyControl(recData);
	recording->isTranscoded = opl_recording_GetRecordingIsTranscoded(recData);
	recording->digitalCopyCount = opl_recording_GetRecordingDigitalCopyCount(recData);
	recording->parentalRating = opl_recording_GetRecordingParentalRating(recData);
	recording->isWatched = opl_recording_GetRecordingIsWatched(recData);

	recording->chapters  = opl_recording_MakeChapterCollection(recData);
#endif
	recording->guidance			= opl_recording_GetGuidance(recData);

	recording->isManualLocked	= opl_recording_GetIsManualLocked(recData);
	recording->szPath			= opl_recording_GetPath(recData);
	recording->szFileName		= opl_recording_GetFileName(recData);

	recording->seriesCRIDs		= opl_recording_GetGroupCRIDs(recData);
	recording->programmeCRID	= opl_recording_GetProgrammeCRID(recData);
	recording->seriesTitle		= opl_recording_GetSeriesTitle(recData);
	recording->subtitletypes	= opl_recording_GetSubtitleTypes(recData);
	return recording;
}
#if 0
static OPL_Bookmark_t*		opl_bookmark_MakeBookmark(const DxRecListData_t	*pstRecData, HUINT16	index)
{
	return opl_recording_MakeBookmark(pstRecData, index);
}


static OPL_Collection_t* opl_bookmark_MakeBookmarkCollection(const DxRecListData_t	*pstRecData)
{
	return opl_recording_MakeBookmarkCollection(pstRecData);
}
#endif

static HERROR	opl_bookmark_FindBookmarkIndex(const DxRecListData_t	*pstRecData, unsigned int time, const char *name, HINT32	*pnIndex)
{
	HUINT16	i;

	if (NULL == pstRecData || NULL == pnIndex)
	{
		return ERR_FAIL;
	}

	for (i = 0; i < pstRecData->usNumOfBookmark; ++i)
	{	// find bookmark
		if (pstRecData->astBookmarks[i].ulPos == time)
		{
			if ( 0 == strncmp(pstRecData->astBookmarks[i].szName, name, DxRECLIST_BOOKMARK_NAME_LEN) )
			{	// find added bookmark
				*pnIndex = i;
				return ERR_OK;
			}
		}
	}	// end for i

	return ERR_FAIL;
}

static int		opl_bookmark_CompareBookmark (const OPL_Bookmark_t *bookmark, OPL_Bookmark_t *key)
{
	if (bookmark == NULL || key == NULL)
		return OPL_FALSE;

	if (bookmark->time != key->time)
		return OPL_FALSE;

	if (bookmark->name == NULL
		|| key->name == NULL
		|| strncmp(bookmark->name, key->name, DxRECLIST_BOOKMARK_NAME_LEN) != 0)
		return OPL_FALSE;

	return OPL_TRUE;
}

#define	____PUBLIC_OPL_Recording___________________________________________________________

OPL_Recording_t *	OPL_Recording_New (const char *signature, ...)
{
	OPL_Recording_t	*self;
	va_list			ap;

	HxLOG_Trace();
	HxLOG_Assert(signature);

	self = (OPL_Recording_t *)opl_scheduleditem_Init(
					  eOPL_SCHEDULED_PRO_RECORD
					, sizeof(OPL_Recording_t)
				);
	if (self == NULL)
	{
		HxLOG_Critical("[%s:%d] Out of memory!!!\n", __HxFILE__, __HxLINE__);
		return NULL;
	}

	va_start(ap, signature);
	if (strcmp(signature, "o") == 0)
	{
		void *	obj = va_arg(ap, void *);

		opl_recording_MakeRecording(self, obj);
	}
	else
	{
		OPL_Recording_Delete(self);
		self = NULL;
	}
	va_end(ap);

	return self;
}

void				OPL_Recording_Delete (OPL_Recording_t *self)
{
	HxLOG_Trace();

	if (self == NULL)
		return;

	if (!OPL_ScheduledItem_InstanceOf(self, OPL_Recording_t))
	{
		HxLOG_Critical("[%s:%d] What the FUXX!!!\n", __HxFILE__, __HxLINE__);
		return;
	}

	// <START> add code here...
	if (self->id)
		HLIB_STD_MemFree(self->id);

	if (self->subtitleLanguages)
		OPL_Collection_Delete(self->subtitleLanguages);
	if (self->audioLanguages)
		OPL_Collection_Delete(self->audioLanguages);
	if (self->genres)
		OPL_Collection_Delete(self->genres);
	if (self->bookmarks)
		OPL_Collection_Delete(self->bookmarks);
	if (self->thumbnail)
		HLIB_STD_MemFree(self->thumbnail);
	if (self->extEventInfo)
		OPL_Collection_Delete(self->extEventInfo);
	if (self->components)
		OPL_Collection_Delete(self->components);
	if (self->szPath){
		HLIB_STD_MemFree(self->szPath);
	}
	if (self->szFileName){
		HLIB_STD_MemFree(self->szFileName);
	}
	if (self->pausedPoints)
		OPL_Collection_Delete(self->pausedPoints);

#if defined(CONFIG_OP_JAPAN)
	if (self->chapters)
		OPL_Collection_Delete(self->chapters);
#endif	//CONFIG_OP_JAPAN
	if (self->guidance)
		HLIB_STD_MemFree(self->guidance);
	if(self->seriesCRIDs)
		OPL_Collection_Delete(self->seriesCRIDs);

	if(self->subtitletypes)
		OPL_Collection_Delete(self->subtitletypes);

	if (self->garbage)
		HLIB_STD_MemFree(self->garbage);

	if (self->programmeCRID)
		HLIB_STD_MemFree(self->programmeCRID);

	if (self->seriesTitle)
		HLIB_STD_MemFree(self->seriesTitle);

	// <END>
	opl_scheduleditem_DeInit((OPL_ScheduledItem_t *)self);
}


DxPvr_EvtComponent_t* opl_recording_evtComponent_clone(const DxPvr_EvtComponent_t *self)
{
	return HLIB_STD_MemDup(self, sizeof(DxPvr_EvtComponent_t));
}


OPL_Recording_t *	OPL_Recording_Clone (const OPL_Recording_t *self)
{
	OPL_Recording_t *clone;

	//HxLOG_Trace();
	if (!OPL_ScheduledItem_InstanceOf(self, OPL_Recording_t))
	{
		HxLOG_Critical("[%s:%d] What the FUXX!!!\n", __HxFILE__, __HxLINE__);
		return NULL;
	}

	clone = (OPL_Recording_t *)OPL_ScheduledItem_Clone((const OPL_ScheduledItem_t *)self);
	if (clone == NULL)
	{
		return NULL;
	}

	clone->state        = self->state;
	clone->error        = self->error;
	clone->id           = HLIB_STD_StrDup(self->id);
	clone->isManual     = self->isManual;
	clone->doNotDelete  = self->doNotDelete;
	clone->doNotDeleteSetTime  = self->doNotDeleteSetTime;
	clone->willBeDeleted= self->willBeDeleted;
	clone->saveDays     = self->saveDays;
	clone->saveEpisodes = self->saveEpisodes;
	clone->blocked      = self->blocked;
	clone->showType     = self->showType;
	clone->subtitles    = self->subtitles;
	if (self->subtitleLanguages)
		clone->subtitleLanguages = OPL_Collection_Clone(self->subtitleLanguages, (void *(*)(void *))HLIB_STD_StrDup_CB);
	clone->isHD         = self->isHD;
	clone->isWideScreen = self->isWideScreen;
	clone->audioType    = self->audioType;
	clone->isMultilingual		= self->isMultilingual;
	if (self->audioLanguages)
		clone->audioLanguages 	= OPL_Collection_Clone(self->audioLanguages, (void *(*)(void *))HLIB_STD_StrDup_CB);
	if (self->genres)
		clone->genres 			= OPL_Collection_Clone(self->genres, (void *(*)(void *))HLIB_STD_StrDup_CB);
	clone->recordingStartTime 	= self->recordingStartTime;
	clone->recordingDuration  	= self->recordingDuration;
	if (self->bookmarks)
		clone->bookmarks	= OPL_Collection_Clone(self->bookmarks, NULL);
	clone->locked 			= self->locked;
	clone->lastPlayPosition	= self->lastPlayPosition;
	clone->lcn				= self->lcn;
	clone->isNotified		= self->isNotified;
	clone->thumbnail     	= HLIB_STD_StrDup(self->thumbnail);

	if (self->extEventInfo)
		clone->extEventInfo = OPL_Collection_Clone((OPL_Collection_t*)self->extEventInfo, (void * (*)(void*))HLIB_STD_StrDup_CB);
	else
		clone->extEventInfo = NULL;

	if (self->components)
		clone->components	= OPL_Collection_Clone((OPL_Collection_t*)self->components, (void * (*)(void*))opl_recording_evtComponent_clone);
	else
		clone->components	= NULL;

	clone->isScrambled = self->isScrambled;
	clone->isEncrypted = self->isEncrypted;
	clone->copyControl = self->copyControl;
	clone->casType = self->casType;
	clone->expiryDate = self->expiryDate;
	if (self->pausedPoints)
		clone->pausedPoints	= OPL_Collection_Clone(self->pausedPoints, NULL);

#if defined(CONFIG_OP_JAPAN)
	clone->digitalCopyControl = self->digitalCopyControl;
	clone->analogCopyControl = self->analogCopyControl;
	clone->isTranscoded = self->isTranscoded;
	clone->digitalCopyCount = self->digitalCopyCount;
	clone->parentalRating = self->parentalRating;
	clone->isWatched = self->isWatched;
	if (self->chapters)
		clone->chapters	= OPL_Collection_Clone(self->chapters, NULL);
#endif

	if (self->guidance)
		clone->guidance		= HLIB_STD_StrDup(self->guidance);
	else
		clone->guidance 	= NULL;
	clone->isManualLocked		= self->isManualLocked;
	clone->szPath			= HLIB_STD_StrDup(self->szPath);
	clone->szFileName		= HLIB_STD_StrDup(self->szFileName);
	clone->programmeCRID	= HLIB_STD_StrDup(self->programmeCRID);
	clone->seriesTitle		= HLIB_STD_StrDup(self->seriesTitle);

	if(self->seriesCRIDs)
		clone->seriesCRIDs	= OPL_Collection_Clone((OPL_Collection_t*)self->seriesCRIDs, (void * (*)(void*))HLIB_STD_StrDup_CB);

	if(self->subtitletypes)
		clone->subtitletypes = OPL_Collection_Clone((OPL_Collection_t*)self->subtitletypes, (void * (*)(void*))HLIB_STD_StrDup_CB);

	return clone;
}

OPL_Recording_t *	OPL_Recording_ScheduledToRecording_Clone (const OPL_ScheduledItem_t *self)
{
	OPL_ScheduledItem_t	*clone;

	HxLOG_Trace();

	clone = (OPL_ScheduledItem_t *)opl_scheduleditem_Init(
					  self->type
					, sizeof(OPL_Recording_t)
				);
	if (clone == NULL)
	{
		HxLOG_Critical("[%s:%d] Out of memory!!!\n", __HxFILE__, __HxLINE__);
		return NULL;
	}

	clone->uid           = self->uid;
	clone->startTime     = self->startTime;
	clone->duration      = self->duration;
	clone->startPadding  = self->startPadding;
	clone->endPadding    = self->endPadding;
	clone->repeatDays    = self->repeatDays;
	clone->isScheduledAsSeries = self->isScheduledAsSeries;
	clone->episode       = self->episode;
	clone->season       = self->season;
	clone->totalEpisodes = self->totalEpisodes;
	clone->recMode = self->recMode;
	clone->groupIndex = self->groupIndex;
	clone->Rating = self->Rating;

	if (self->name)
		clone->name = HLIB_STD_StrDup(self->name);
	if (self->longName)
		clone->longName = HLIB_STD_StrDup(self->longName);
	if (self->description)
		clone->description = HLIB_STD_StrDup(self->description);
	if (self->longDescription)
		clone->longDescription = HLIB_STD_StrDup(self->longDescription);
	if (self->channelID)
		clone->channelID = HLIB_STD_StrDup(self->channelID);
	if (self->programmeID)
		clone->programmeID = HLIB_STD_StrDup(self->programmeID);
	if (self->seriesID)
		clone->seriesID = HLIB_STD_StrDup(self->seriesID);
	if (self->storageID)
		clone->storageID = HLIB_STD_StrDup(self->storageID);
	if (self->deviceHandle)
		clone->deviceHandle = HLIB_STD_StrDup(self->deviceHandle);
	if (self->destinationId)
		clone->destinationId = HLIB_STD_StrDup(self->destinationId);
	if (self->userID)
		clone->userID = HLIB_STD_StrDup(self->userID);

	if( self->type == eOPL_SCHEDULED_PRO_DLNA )
	{
		OPL_Recording_t	*temp = (OPL_Recording_t*)clone;
		char strtemp[512];
		OPL_Channel_t channel = OPL_Channel_FindChannel(OPL_Channel_GetManager(), clone->channelID);
		if (channel)
		{
			HxSTD_snprintf(strtemp, 512, "DLNA://%s/%x.%x.%x;%x;%x",clone->deviceHandle,
							OPL_Channel_GetOrgNetworkID(channel), OPL_Channel_GetTransponderID(channel), OPL_Channel_GetServiceID(channel)
							,OPL_Channel_GetUniqueID(channel), (int)self->startTime );
		}
		temp->id = HLIB_STD_StrDup(strtemp);

		if( self->programmeID == NULL )
			temp->isManual = TRUE;
	}
	else if( self->type == eOPL_SCHEDULED_PRO_DUBBING )
	{
		OPL_Recording_t	*temp = (OPL_Recording_t*)clone;

		if(self->dubbingId)
			temp->id = HLIB_STD_StrDup(self->dubbingId);
	}

	clone->serviceId = self->serviceId;
	clone->transportStreamId = self->transportStreamId;
	clone->originalNetworkId = self->originalNetworkId;

	return (OPL_Recording_t*)clone;

}

OPL_Recording_t *	OPL_Recording_ScheduledAndRecording_Clone (const OPL_ScheduledItem_t *scheduleditem, const OPL_Recording_t *recordingitem)
{
	OPL_Recording_t *clone;

	//HxLOG_Trace();
	if (!OPL_ScheduledItem_InstanceOf(recordingitem, OPL_Recording_t))
	{
		HxLOG_Critical("[%s:%d] What the FUXX!!!\n", __HxFILE__, __HxLINE__);
		return NULL;
	}

	clone = (OPL_Recording_t *)OPL_Recording_ScheduledToRecording_Clone((const OPL_ScheduledItem_t *)scheduleditem);
	if (clone == NULL)
	{
		return NULL;
	}

	clone->state        = recordingitem->state;
	clone->error        = recordingitem->error;
	clone->id           = HLIB_STD_StrDup(recordingitem->id);
	clone->isManual     = recordingitem->isManual;
	clone->doNotDelete  = recordingitem->doNotDelete;
	clone->doNotDeleteSetTime  = recordingitem->doNotDeleteSetTime;
	clone->willBeDeleted= recordingitem->willBeDeleted;
	clone->saveDays     = recordingitem->saveDays;
	clone->saveEpisodes = recordingitem->saveEpisodes;
	clone->blocked      = recordingitem->blocked;
	clone->showType     = recordingitem->showType;
	clone->subtitles    = recordingitem->subtitles;
	if (recordingitem->subtitleLanguages)
		clone->subtitleLanguages = OPL_Collection_Clone(recordingitem->subtitleLanguages, (void *(*)(void *))HLIB_STD_StrDup_CB);
	clone->isHD         = recordingitem->isHD;
	clone->isWideScreen = recordingitem->isWideScreen;
	clone->audioType    = recordingitem->audioType;
	clone->isMultilingual		= recordingitem->isMultilingual;
	if (recordingitem->audioLanguages)
		clone->audioLanguages 	= OPL_Collection_Clone(recordingitem->audioLanguages, (void *(*)(void *))HLIB_STD_StrDup_CB);
	if (recordingitem->genres)
		clone->genres 			= OPL_Collection_Clone(recordingitem->genres, (void *(*)(void *))HLIB_STD_StrDup_CB);
	clone->recordingStartTime 	= recordingitem->recordingStartTime;
	clone->recordingDuration  	= recordingitem->recordingDuration;
	if (recordingitem->bookmarks)
		clone->bookmarks	= OPL_Collection_Clone(recordingitem->bookmarks, NULL);
	clone->locked 			= recordingitem->locked;
	clone->lastPlayPosition	= recordingitem->lastPlayPosition;
	clone->lcn				= recordingitem->lcn;
	clone->isNotified		= recordingitem->isNotified;
	clone->thumbnail     	= HLIB_STD_StrDup(recordingitem->thumbnail);

	if (recordingitem->extEventInfo)
		clone->extEventInfo = OPL_Collection_Clone((OPL_Collection_t*)recordingitem->extEventInfo, (void * (*)(void*))HLIB_STD_StrDup_CB);
	else
		clone->extEventInfo = NULL;

	if (recordingitem->components)
		clone->components	= OPL_Collection_Clone((OPL_Collection_t*)recordingitem->components, (void * (*)(void*))opl_recording_evtComponent_clone);
	else
		clone->components	= NULL;

	clone->isScrambled = recordingitem->isScrambled;
	clone->isEncrypted = recordingitem->isEncrypted;
	clone->copyControl = recordingitem->copyControl;
	clone->casType = recordingitem->casType;

	clone->expiryDate = recordingitem->expiryDate;
	if (recordingitem->pausedPoints)
		clone->pausedPoints	= OPL_Collection_Clone(recordingitem->pausedPoints, NULL);

#if defined(CONFIG_OP_JAPAN)
		clone->digitalCopyControl = recordingitem->digitalCopyControl;
		clone->analogCopyControl = recordingitem->analogCopyControl;
		clone->isTranscoded = recordingitem->isTranscoded;
		clone->digitalCopyCount = recordingitem->digitalCopyCount;
		clone->parentalRating = recordingitem->parentalRating;
		clone->isWatched = recordingitem->isWatched;

		if (recordingitem->chapters)
			clone->chapters	= OPL_Collection_Clone(recordingitem->chapters, NULL);
#endif

	if (recordingitem->guidance)
		clone->guidance		= HLIB_STD_StrDup(recordingitem->guidance);
	else
		clone->guidance 	= NULL;

	clone->isManualLocked = recordingitem->isManualLocked;
	clone->szPath		= HLIB_STD_StrDup(recordingitem->szPath);
	clone->szFileName     	= HLIB_STD_StrDup(recordingitem->szFileName);

	return clone;
}


OPL_Recording_t*	OPL_Recording_GetUpdatableData (OPL_Recording_t *recording, OPL_HANDLE recDataHandle)
{
	DxRecListData_t *recData = (DxRecListData_t*)recDataHandle;
	if (NULL == recording || NULL == recData)
	{
		HxLOG_Error("[%s:%d] NULL param!!!\n", __HxFILE__, __HxLINE__);
		return NULL;
	}

	recording->doNotDelete	= opl_recording_GetDoNotDelete(recData);
	recording->isManualLocked		= opl_recording_GetIsManualLocked(recData);
	return recording;
}



OPL_Bookmark_t*		OPL_Recording_Bookmark_Add(OPL_HANDLE	recHandle, unsigned int time, const char *name)
{
	OPL_Recording_t					*item = (OPL_Recording_t *)recHandle;
	APK_META_REC_BOOKMARK_t	stBookmark	= {0};
	DxRecListData_t		*pstUpdatedRecData = NULL;

	if (item == NULL || name==NULL)
	{
		return NULL;
	}

	stBookmark.ulParamCount = 1;
	stBookmark.paramList	= (APK_META_REC_BOOKMARK_PARAM_t*)HLIB_STD_MemCalloc(sizeof(APK_META_REC_BOOKMARK_PARAM_t) * stBookmark.ulParamCount);
	if (stBookmark.paramList)
	{	// request the bookmark adding
		HERROR	hErr = ERR_FAIL;
		HINT32	nAddedBookmarkIndex = -1;
		OPL_Bookmark_t		*newBookmark = NULL;

		HLIB_STD_StrUtf8NCpy(stBookmark.paramList[0].szUrl, item->id, DxRECLIST_URL);
		stBookmark.paramList[0].stBookmark.ulPos = time;
			strncpy(stBookmark.paramList[0].stBookmark.szName, name, DxRECLIST_BOOKMARK_NAME_LEN);

		hErr = APK_META_RECORD_AddBookmark(&stBookmark, TRUE);
		if (ERR_OK != hErr)
		{
			HLIB_STD_MemFree(stBookmark.paramList);
			return NULL;
		}

		HLIB_STD_MemFree(stBookmark.paramList);

		pstUpdatedRecData	= APK_META_RECORD_GetByUrl(item->id);
		if (NULL == pstUpdatedRecData)
		{
			return NULL;
		}

		hErr = opl_bookmark_FindBookmarkIndex(pstUpdatedRecData, time, name, &nAddedBookmarkIndex);
		APK_META_RECORD_Release(pstUpdatedRecData);
		if (ERR_OK != hErr || nAddedBookmarkIndex < 0)
		{
			return NULL;
		}

		newBookmark = OPL_Bookmark_New(time, name);
		nAddedBookmarkIndex = (HINT32)OPL_Collection_Add(item->bookmarks, newBookmark);
		if (nAddedBookmarkIndex >= 0)
		{
			OPL_Bookmark_t *tmpBookmark = NULL;
			OPL_Bookmark_t *clone = NULL;

			tmpBookmark = (OPL_Bookmark_t*)OPL_Collection_ItemAt(item->bookmarks, (int)nAddedBookmarkIndex);
			if (tmpBookmark != NULL)
			{
				clone = (OPL_Bookmark_t*)OPL_Bookmark_Clone(tmpBookmark);
			}

			return clone;
		}
	}	// end if (stBookmark.paramList)

	return NULL;
}



void	OPL_Recording_Bookmark_Remove(OPL_HANDLE	recHandle, OPL_HANDLE	bookmarkHandle)
{
	OPL_Recording_t					*item = (OPL_Recording_t *)recHandle;
	DxRecListData_t		*pstRecData = NULL;
	OPL_Bookmark_t					*bookmark = (OPL_Bookmark_t*)bookmarkHandle;
	HINT32							nBookmarkIndex = -1;
	HERROR							hErr = ERR_FAIL;
	APK_META_REC_BOOKMARK_t	stBookmark = {0};

	if (NULL == item || NULL == bookmark)
	{
		return;
	}

	if (NULL == item->bookmarks)
	{
		return;
	}

	pstRecData = APK_META_RECORD_GetByUrl(item->id);
	if (NULL == pstRecData)
	{
		return;
	}

	// check that item has that bookmark item
	hErr = opl_bookmark_FindBookmarkIndex(pstRecData, bookmark->time, bookmark->name, &nBookmarkIndex);
	APK_META_RECORD_Release(pstRecData);
	if (ERR_OK != hErr || nBookmarkIndex < 0)
	{
		return;
	}

	// request removing the bookmark
	stBookmark.ulParamCount = 1;
	stBookmark.paramList	= (APK_META_REC_BOOKMARK_PARAM_t*)HLIB_STD_MemCalloc(sizeof(APK_META_REC_BOOKMARK_PARAM_t) * stBookmark.ulParamCount);
	if (stBookmark.paramList)
	{
		HLIB_STD_StrUtf8NCpy(stBookmark.paramList[0].szUrl, item->id, DxRECLIST_URL);
		stBookmark.paramList[0].stBookmark.ulPos = bookmark->time;
		if (bookmark->name)
		{
			strncpy(stBookmark.paramList[0].stBookmark.szName, bookmark->name, DxRECLIST_BOOKMARK_NAME_LEN);
		}

		hErr = APK_META_RECORD_RemoveBookmark(&stBookmark, TRUE);
		if (ERR_OK != hErr)
		{
			HLIB_STD_MemFree(stBookmark.paramList);
			return;
		}
		HLIB_STD_MemFree(stBookmark.paramList);

		nBookmarkIndex = OPL_Collection_Find(item->bookmarks
								, (void *)bookmark
								, (int(*)(const void *,void *))opl_bookmark_CompareBookmark
							);
		if (nBookmarkIndex >= 0)
		{
			OPL_Collection_Remove(item->bookmarks, (int)nBookmarkIndex);
		}
	}
}

void *	OPL_Recording_PutGarbage (OPL_Recording_t *recording, void *garbage)
{
	HxLOG_Trace();
	HxLOG_Assert(recording);

	if (recording->garbage)
	{
		HLIB_STD_MemFree(recording->garbage);
	}
	recording->garbage = garbage;
	return garbage;
}

char*		OPL_Recording_HasAudioDescription(OPL_Recording_t *recording)
{
	int i = 0;
	int len = 0;
	DxPvr_EvtComponent_t *component = NULL;
	// TODO: get the real data

	if (recording && recording->components)
	{
		len = OPL_Collection_Length(recording->components);
		for(i=0; i<len; i++)
		{
			component = (DxPvr_EvtComponent_t *)OPL_Collection_ItemAt(recording->components, i);
			if(component == NULL)
				continue;

			if (component->ucStreamContent == 0x02 && (component->ucComponentType == 0x40 || component->ucComponentType == 0x48))
			{
				return HLIB_STD_StrDup("true");
			}
			else if(component->ucStreamContent == 0x04 && (((component->ucComponentType & 0x38) >> 3) == 2))
			{
				return HLIB_STD_StrDup("true");
			}
		}
	}

	return HLIB_STD_StrDup("false");
}

char*		OPL_Recording_HasSigned(OPL_Recording_t *recording)
{
	int i = 0;
	int len = 0;
	DxPvr_EvtComponent_t *component = NULL;
	// TODO: get the real data
	if (recording && recording->components)
	{
		len = OPL_Collection_Length(recording->components);
		for(i=0; i<len; i++)
		{
			component = (DxPvr_EvtComponent_t *)OPL_Collection_ItemAt(recording->components, i);
			if(component == NULL)
				continue;

			if (component->ucStreamContent == 0x03 && component->ucComponentType == 0x30)
			{
				return HLIB_STD_StrDup("true");
			}
		}
	}

	return HLIB_STD_StrDup("false");
}

char*		OPL_Recording_HasAC3(OPL_Recording_t *recording)
{
	int i = 0;
	int len = 0;
	DxPvr_EvtComponent_t *component = NULL;
	// TODO: get the real data
	if (recording && recording->components)
	{
		len = OPL_Collection_Length(recording->components);
		for(i=0; i<len; i++)
		{
			component = (DxPvr_EvtComponent_t *)OPL_Collection_ItemAt(recording->components, i);
			if(component == NULL)
				continue;
			if (component->ucStreamContent == 0x04)
			{
				return HLIB_STD_StrDup("true");
			}
		}
	}

	return HLIB_STD_StrDup("false");
}

char*	OPL_Recording_GetThumbnail(OPL_Recording_t *recording)
{
	char *url = NULL;
	char *thumbnail = NULL;
	DxRecListData_t *data = NULL;
	url = recording->id;
	data = APK_META_RECORD_GetByUrl(url);
	if(data == NULL)
		return NULL;
	thumbnail = opl_recording_GetThumbnail(data);
	APK_META_RECORD_Release(data);
	return thumbnail;
}

int		OPL_Recording_GetCasIDs(OPL_Recording_t *recording, eOplRecordingCasType **casIDs)
{
	return opl_recording_GetCasIDs(recording, casIDs);
}

