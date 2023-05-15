/*******************************************************************************
*
* File Name    : xmgr_mplayer_base.c
*
* Description  : new media player body for OCTO 2.0
*
*
* Copyright (C) 2012, Humax Co., Ltd. All rights reserved.
*
* Revision History :
*
* Date        Modification                                  Name
* ----        ------------                                 --------
* 2013/03/27	createded									chmin
*
*******************************************************************************/
#include <octo_common.h>

#ifdef CONFIG_SUPPORT_IMMA
#include <svc_mplayercso.h>
#include <mgr_rscmap.h>
#include <mgr_output.h>
#include <cso_def.h>
#include <svc_av.h>
#endif
#include <svc_mplayer.h>
#include <svc_output.h>
#include <svc_pipe.h>
#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_media.h>
#include <../local_include/_xmgr_mplayer.h>

/*******************************************************************************
*	Typedef
*******************************************************************************/
#ifdef CONFIG_SUPPORT_IMMA
typedef struct CSO_PLAYREADY_AV_OUTPUT_PROTECTION_IDS_EX
{
	uint32_t dwVersion;
	uint16_t cEntries;
	CSO_PlayReady_OUTPUT_PROTECTION_EX *rgAVop;
} PROTECTION_IDS_EX_t;

/*******************************************************************************
*	Static Variables
*******************************************************************************/
/* Explicit Digital Audio Output Protection */
static CSO_PlayReady_GUID s_tGuid_SCMS												= {0x6D5CFA59, 0xC250, 0x4426, {0x93, 0x0E, 0xFA, 0xC7, 0x2C, 0x8F, 0xCF, 0xA6}};

/* Explicit Digital Video Output Protection */
static CSO_PlayReady_GUID s_tGuid_AGCandColorStripe								= {0xC3FD11C6, 0xF8B7, 0x4d20, {0xB0, 0x08, 0x1D, 0xB1, 0x7D, 0x61, 0xF2, 0xDA}};
static CSO_PlayReady_GUID s_tGuid_ExplicitAnalogTelevisionOutputRestriction		= {0x2098DE8D, 0x7DDD, 0x4bab, {0x96, 0xC6, 0x32, 0xEB, 0xB6, 0xFA, 0xBE, 0xA3}};
static CSO_PlayReady_GUID s_tGuid_BestEffortExplicitAnalogTelevisionOutputRestriction = {0x225CD36F, 0xF132, 0x49EF, {0xBA, 0x8C, 0xC9, 0x1E, 0xA2, 0x8E, 0x43, 0x69}};
static CSO_PlayReady_GUID s_tGuid_ImageconstraintforAnalogComponentVideoOutput	= {0x811C5110, 0x46C8, 0x4C6e, {0x81, 0x63, 0xC0, 0x48, 0x2A, 0x15, 0xD4, 0x7E}};
static CSO_PlayReady_GUID s_tGuid_ImageconstraintforAnalogComputerMonitorOutput= {0xD783A191, 0xE083, 0x4BAF, {0xB2, 0xDA, 0xE6, 0x9F, 0x91, 0x0B, 0x37, 0x72}};
static CSO_PlayReady_GUID s_tGuid_DigitalVideoOnlyContent							= {0x760AE755, 0x682A, 0x41E0, {0xB1, 0xB3, 0xDC, 0xDF, 0x83, 0x6A, 0x73, 0x06}};
#endif /*CONFIG_SUPPORT_IMMA*/

static HBOOL	s_bExplicitStop = FALSE;

/*******************************************************************************
*	Static functions
*******************************************************************************/
static inline SvcMp_MediaPlayer_t *__mp(MGR_MPlayer_t *mplayer)
{
	return (SvcMp_MediaPlayer_t *)mplayer;
}

#ifdef CONFIG_SUPPORT_IMMA
static inline SvcCso_t *__SVC_CSO(MGR_MPlayer_t *mplayer)
{
	return (SvcCso_t *)mplayer;
}
#endif

static HERROR _mp_action_take(MGR_MPlayer_t *mplayer)
{
	HERROR eRet = ERR_OK;
	HUINT32	ulActionId = 0;
	MgrAction_Type_e	eActType;

#ifdef CONFIG_SUPPORT_IMMA
	ulActionId = SVC_CSO_GetActionId(__SVC_CSO(mplayer));
#else
	ulActionId = SVC_MP_GetActionId(__mp(mplayer));
#endif
	eActType = MGR_ACTION_GetType (ulActionId);
	if (eViewType_MOVIE_PB != eActType)
	{
		eRet = MGR_ACTION_SetupMovie (ulActionId, eAmPriority_VIEW_ACTION);
		if ( eRet != ERR_OK )
		{
			return ERR_FAIL;
		}

		eRet = MGR_ACTION_Take (ulActionId, eViewType_MOVIE_PB, BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()));
		if ( eRet != ERR_OK )
		{
			return ERR_FAIL;
		}
#if defined(CONFIG_SUPPORT_ROVI) || defined(CONFIG_3RD_MHEG)
#ifdef CONFIG_SUPPORT_IMMA
		{
			HERROR							 hError = ERR_FAIL;
			HUINT32 						 ulDemuxId;

#if defined(CONFIG_SUPPORT_ROVI)
#else
			if(MGR_ACTION_GetMhegStatus() == eAmMhegStatus_Running)
#endif				
			{
				hError = MGR_RSC_GetResourceId(ulActionId, eRxRSCTYPE_DEMUX, &ulDemuxId);
				if(hError != ERR_OK)
				{
					HxLOG_Print("[_mp_action_take _ DemuxId] MGR_RSC_GetResourceId fail!!\n");
					return ERR_FAIL;
				}
	
				(void)SVC_CSO_SetDemuxId(__SVC_CSO(mplayer), ulDemuxId);
			}
		}
#endif
#endif


	}

	return ERR_OK;
}

#ifdef CONFIG_SUPPORT_IMMA
static HBOOL _mp_check_guid(PROTECTION_IDS_EX_t *pOutputGuids, CSO_PlayReady_GUID guid, HUINT32 *pulConfigData)
{
	HINT32 nIndex = 0;
	HBOOL bRet = FALSE;
	for(nIndex = 0; nIndex < pOutputGuids->cEntries; ++nIndex)
	{
		if(HxSTD_MemCmp(&pOutputGuids->rgAVop[nIndex].guidId, &guid, sizeof(CSO_PlayReady_GUID)) == 0)
		{
				HxLOG_Debug("[%s, %d] guid Dectected!	%02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X\n", __FUNCTION__, __LINE__, guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
				bRet = TRUE;
				*pulConfigData = pOutputGuids->rgAVop[nIndex].dwConfigData;
				break;							
		}
	}
	return bRet;
}
#endif

static HERROR _mp_action_release(MGR_MPlayer_t *mplayer)
{
#ifdef CONFIG_SUPPORT_IMMA
	return MGR_ACTION_Release(SVC_CSO_GetActionId(__SVC_CSO(mplayer)), eViewType_MOVIE_PB);
#else
	return MGR_ACTION_Release(SVC_MP_GetActionId(__mp(mplayer)), eViewType_MOVIE_PB);
#endif
}

/*******************************************************************************
*	internal public functions
*******************************************************************************/
BUS_Result_t xproc_mplayer_Base(MGR_MPlayer_t *mplayer, HINT32 message, Handle_t handle,
							HINT32 param1, HINT32 param2, HINT32 param3)
{
	HUINT32 ulActionId = 0;
#ifdef CONFIG_SUPPORT_IMMA
	ulActionId = SVC_CSO_GetActionId (__SVC_CSO(mplayer));
#else
	ulActionId = SVC_MP_GetActionId (__mp(mplayer));
#endif

	switch ( message )
	{
	case eMEVT_BUS_CREATE:
		(void)_mp_action_take(mplayer);
		return ERR_BUS_MESSAGE_BREAK;

	case eMEVT_MEDIA_MPLAYER_EXPLICIT_STOP:
		s_bExplicitStop = TRUE;
		BUS_MGR_Destroy((BUS_Callback_t)MGR_MPlayer_GetProc(ulActionId));
		return ERR_BUS_MESSAGE_BREAK;

	case eMEVT_BUS_DESTROY:
		if(s_bExplicitStop == FALSE)
		{
#ifdef CONFIG_SUPPORT_IMMA
			(void)SVC_CSO_Stop(__SVC_CSO(mplayer));
#else
			// 이 경우는 외부에서 media player proc을 Destroy 하는 경우이므로 resource 제거 후 종료하게 함
			(void)SVC_MP_Stop(__mp(mplayer));
#endif
		}
		else
		{
			// 명시적으로 media player stop이 호출된 경우 Flag를 원래대로 복구시킴
			s_bExplicitStop = FALSE;
		}

		(void)_mp_action_release(mplayer);

		return ERR_BUS_MESSAGE_BREAK;
		
	case eSEVT_SI_DRM_SIGNALLED:
#ifdef CONFIG_SUPPORT_ROVI		
	{
		Macrovision_t eMacrovision = 0;
		HxLOG_Debug ("[%s:%d] ucAPS :%d\n", __FUNCTION__, __LINE__, param3);		
		switch(param3)
		{
			case 0:
			{
				HxLOG_Debug("[%s, %d] Macrovision : NONE\n", __FUNCTION__, __LINE__);
				eMacrovision = eMACROVISION_OFF;
				break;
			}
			case 1:
			{
				HxLOG_Debug("[%s, %d] Macrovision : Agc\n", __FUNCTION__, __LINE__);
				eMacrovision = eMACROVISION_AGC;
				break;
			}
			case 2:
			{
				HxLOG_Debug("[%s, %d] Macrovision : Agc 2Line\n", __FUNCTION__, __LINE__);
				eMacrovision = eMACROVISION_2LINE;
				
				//HxLOG_Debug("[%s, %d] Macrovision : TEST1 for Certification\n", __FUNCTION__, __LINE__);
				//eMacrovision = eMACROVISION_TEST1;
				break;
			}
			case 3:
			{
				HxLOG_Debug("[%s, %d] Macrovision : Agc 4Line\n", __FUNCTION__, __LINE__);
				eMacrovision = eMACROVISION_4LINE;
				
				//HxLOG_Debug("[%s, %d] Macrovision : TEST2 for Certification\n", __FUNCTION__, __LINE__);
				//eMacrovision = eMACROVISION_TEST2;			
				break;
			}
			default :
			{
				HxLOG_Error("[%s, %d] Macrovision : APS is %u\n", __FUNCTION__, __LINE__, param3);
				return ERR_FAIL;
			}				
		}
		
		if(SVC_OUT_SetMacrovision(ulActionId, eMacrovision) != ERR_OK)
		{
			HxLOG_Error("[%s, %d] SVC_OUT_SetMacrovision\n", __FUNCTION__, __LINE__);
			return ERR_FAIL;
		}
	}	
#endif	
		return ERR_BUS_MESSAGE_BREAK;
#ifdef CONFIG_SUPPORT_IMMA
	case eMEVT_MEDIA_OUTPUT_CONTROL :
	{	
		CSO_OutputControl_t	tOutputControl;

		HBOOL		bDigitalVideoDisable = FALSE;
		HBOOL		bAnalogVideoDisable = FALSE;
		HBOOL		bDigitalAudioDisable = FALSE;
		HBOOL		bVideoDisable = FALSE;
		HBOOL		bHdcp = FALSE;
		HBOOL		bCgms = FALSE;
		HBOOL		bScms = FALSE;
		HBOOL		bMacrovision = FALSE;
		HUINT32		ulCgmsValue = 0;
		HUINT32		ulScmsValue = 0;
		HUINT32		ulMacrovisionValue = 0;
		HUINT32		ulConfigData = 0;
		DxDigitalAudioFormat_e eAudioFormat = eDxDIGITALAUDIO_FORMAT_UNKNOWN;

		HxSTD_MemSet(&tOutputControl, 0x00, sizeof(CSO_OutputControl_t));
		HxSTD_MemCopy(&tOutputControl, (void*)param3, sizeof(CSO_OutputControl_t));

		(void)SVC_OUT_GetHdmiAudioFormat(&eAudioFormat);

		/********************************/
		/* Compress Digital Video		*/
		/********************************/
		if( tOutputControl.data.playready.minOPL.wCompressedDigitalVideo > 0 )
		{
			HxLOG_Error("[%s, %d] wCompressedDigitalVideo Level : %d - Can't Support Compressed DigitalVideo \n", __FUNCTION__, __LINE__, tOutputControl.data.playready.minOPL.wCompressedDigitalVideo);
		}
		
		/********************************/
		/* Uncompress Digital Video		*/
		/********************************/
		if(tOutputControl.data.playready.minOPL.wUncompressedDigitalVideo >= 101 && tOutputControl.data.playready.minOPL.wUncompressedDigitalVideo <= 200) 
		{
			bHdcp = TRUE; /* HUMAX Default : HDCP VIDEO ON */
			/* Spec : Can't Enable VIDEO HDCP : PASS */
		}/* >=101 */
		if(tOutputControl.data.playready.minOPL.wUncompressedDigitalVideo >= 201 && tOutputControl.data.playready.minOPL.wUncompressedDigitalVideo <= 270) 
		{
			bHdcp = TRUE; /* HUMAX Default : HDCP VIDEO ON */
			/* Spec : content <= 520,000 pixels : PASS*/
		}/* >=201 */		
		if(tOutputControl.data.playready.minOPL.wUncompressedDigitalVideo >= 271 && tOutputControl.data.playready.minOPL.wUncompressedDigitalVideo <= 300)	
		{
			bHdcp = TRUE; /* HUMAX Default : HDCP VIDEO ON */
			/* HDCP VIDEO ON */
		}
		if(tOutputControl.data.playready.minOPL.wUncompressedDigitalVideo >= 301)	
		{
			bHdcp = TRUE; /* HUMAX Default : HDCP VIDEO ON */
			bDigitalVideoDisable = TRUE; /* HDMI VIDEO OFF / HIDE */
		}

		/********************************/
		/* Compress Digital Audio		*/
		/********************************/
		if(tOutputControl.data.playready.minOPL.wCompressedDigitalAudio >= 201 && tOutputControl.data.playready.minOPL.wCompressedDigitalAudio <= 250)
		{	
			if(eAudioFormat == eDxDIGITALAUDIO_FORMAT_MULTICHANNEL)
				bHdcp = TRUE; /* HUMAX Default : HDCP AUDIO ON */
			
			bScms = TRUE; /* HUMAX Default : SCMS AUDIO ON */
			ulScmsValue = 3; /* Copy Never */ /* the Cp-bit set to zero (0) and the L-bit set to No Indication */
		}
		if(tOutputControl.data.playready.minOPL.wCompressedDigitalAudio >= 251 && tOutputControl.data.playready.minOPL.wCompressedDigitalAudio <= 300)
		{
			if(eAudioFormat == eDxDIGITALAUDIO_FORMAT_MULTICHANNEL)
				bHdcp = TRUE; /* HUMAX Default : HDCP AUDIO ON */
			bScms = TRUE; /* HUMAX Default : SCMS AUDIO ON */
		}
		if(tOutputControl.data.playready.minOPL.wCompressedDigitalAudio >= 301)
		{
			if(eAudioFormat == eDxDIGITALAUDIO_FORMAT_MULTICHANNEL)
				bHdcp = TRUE; /* HUMAX Default : HDCP AUDIO ON */
			bScms = TRUE; /* HUMAX Default : SCMS AUDIO ON */
			bDigitalAudioDisable = TRUE;
		}
		if(_mp_check_guid((PROTECTION_IDS_EX_t*)&tOutputControl.data.playready.aopi, s_tGuid_SCMS, &ulScmsValue))
		{
			if(tOutputControl.data.playready.minOPL.wCompressedDigitalAudio < 201)
			{
				bScms = TRUE;
			}
			if(tOutputControl.data.playready.minOPL.wCompressedDigitalAudio < 301)
			{
				if(eAudioFormat == eDxDIGITALAUDIO_FORMAT_MULTICHANNEL)
					bHdcp = TRUE; /* HUMAX Default : HDCP AUDIO ON */
			}
		}

		/********************************/
		/* Uncompress Digital Audio		*/
		/********************************/
		if(tOutputControl.data.playready.minOPL.wUncompressedDigitalAudio >= 201 && tOutputControl.data.playready.minOPL.wUncompressedDigitalAudio <= 250)
		{
			if(eAudioFormat == eDxDIGITALAUDIO_FORMAT_PCM)
				bHdcp = TRUE; /* HUMAX Default : HDCP AUDIO ON */
			bScms = TRUE;
			ulScmsValue = 3; /* Copy Never */
		}
		if(tOutputControl.data.playready.minOPL.wUncompressedDigitalAudio >= 251 && tOutputControl.data.playready.minOPL.wUncompressedDigitalAudio <= 300)
		{
			if(eAudioFormat == eDxDIGITALAUDIO_FORMAT_PCM)
				bHdcp = TRUE; /* HUMAX Default : HDCP AUDIO ON */
		}
		if(tOutputControl.data.playready.minOPL.wUncompressedDigitalAudio >= 301)
		{
			if(eAudioFormat == eDxDIGITALAUDIO_FORMAT_PCM)
				bHdcp = TRUE; /* HUMAX Default : HDCP AUDIO ON */
			/* AUDIO MUTE */
			bDigitalAudioDisable = TRUE;
		}
		if(_mp_check_guid((PROTECTION_IDS_EX_t*)&tOutputControl.data.playready.aopi, s_tGuid_SCMS, &ulScmsValue))
		{
			if(tOutputControl.data.playready.minOPL.wUncompressedDigitalAudio < 201)
			{
				bScms = TRUE;
			}
			if(tOutputControl.data.playready.minOPL.wUncompressedDigitalAudio < 301)
			{
				if(eAudioFormat == eDxDIGITALAUDIO_FORMAT_PCM)
					bHdcp = TRUE; /* HUMAX Default : HDCP AUDIO ON */
			}
		}

		/********************************/
		/* Analog Video	Television 		*/
		/********************************/		
		if(tOutputControl.data.playready.minOPL.wAnalogVideo >= 101 && tOutputControl.data.playready.minOPL.wAnalogVideo <= 150 )
		{
			bCgms = TRUE; /* HUMAX Default : CGMS VIDEO ON */
			/* Spec : Can't CGMS Enable : PASS */
			ulCgmsValue = eMACROVISION_4LINE;
			HxLOG_Debug("[%s, %d] engaging CGMS-A VIDEO 101\n", __FUNCTION__, __LINE__);
		}
		else if(tOutputControl.data.playready.minOPL.wAnalogVideo >= 151 && tOutputControl.data.playready.minOPL.wAnalogVideo <= 200 )
		{
			bCgms = TRUE; /* HUMAX Default : CGMS VIDEO ON */
			ulCgmsValue = eMACROVISION_4LINE;
			HxLOG_Debug("[%s, %d] engaging CGMS-A VIDEO 151\n", __FUNCTION__, __LINE__);
		}
		else if(tOutputControl.data.playready.minOPL.wAnalogVideo >= 201)
		{
			/* VIDEO ANALOG OFF */
			bAnalogVideoDisable = TRUE;
		}

		/********************************/
		/* Extended Analog Television 	*/
		/********************************/		
		if( _mp_check_guid((PROTECTION_IDS_EX_t*)&tOutputControl.data.playready.vopi, s_tGuid_ExplicitAnalogTelevisionOutputRestriction, &ulCgmsValue) 
				&& tOutputControl.data.playready.minOPL.wAnalogVideo < 151 )
		{
			bCgms = TRUE; /* HUMAX Default : CGMS VIDEO ON */
			HxLOG_Debug("[%s, %d] engaging CGMS-A VIDEO\n", __FUNCTION__, __LINE__);
		}
		if( _mp_check_guid((PROTECTION_IDS_EX_t*)&tOutputControl.data.playready.vopi, s_tGuid_BestEffortExplicitAnalogTelevisionOutputRestriction, &ulConfigData)
				&& tOutputControl.data.playready.minOPL.wAnalogVideo < 101 )
		{
			if(!_mp_check_guid((PROTECTION_IDS_EX_t*)&tOutputControl.data.playready.vopi, s_tGuid_ExplicitAnalogTelevisionOutputRestriction, &ulCgmsValue))
			{
				bCgms = TRUE; /* HUMAX Default : CGMS VIDEO ON */
				HxLOG_Debug("[%s, %d] engaging CGMS-A VIDEO\n", __FUNCTION__, __LINE__);
			}
		}
		if(_mp_check_guid((PROTECTION_IDS_EX_t*)&tOutputControl.data.playready.vopi, s_tGuid_AGCandColorStripe, &ulMacrovisionValue))
		{
			/* Spec : content <= 520,000 pixels : PASS, But HUMAX NO PASS*/
			bMacrovision = TRUE;
		}

		/********************************/
		/* Analog Component Video Output */
		/********************************/
		if(_mp_check_guid((PROTECTION_IDS_EX_t*)&tOutputControl.data.playready.vopi, s_tGuid_ImageconstraintforAnalogComponentVideoOutput, &ulConfigData))
		{
			/* Spec : content <= 520,000 pixels : PASS*/
			HxLOG_Debug("[%s, %d] Image constraint for Analog Component Video Output: %u\n", __FUNCTION__, __LINE__, ulConfigData);
		}

		/********************************/
		/* Analog Component Video Output */
		/********************************/		
		if(_mp_check_guid((PROTECTION_IDS_EX_t*)&tOutputControl.data.playready.vopi, s_tGuid_ImageconstraintforAnalogComputerMonitorOutput, &ulConfigData))
		{
			/* Spec : content <= 520,000 pixels : PASS*/
			HxLOG_Debug("[%s, %d] Image constraint for Analog Computer Monitor Output: %u\n", __FUNCTION__, __LINE__, ulConfigData);
		}

		/********************************/
		/* Digital Video Only */
		/********************************/		
		if(_mp_check_guid((PROTECTION_IDS_EX_t*)&tOutputControl.data.playready.vopi, s_tGuid_DigitalVideoOnlyContent, &ulConfigData))
		{
			/* Only Digital PASS, Analog Hide */
			HxLOG_Debug("[%s, %d] Digital Video Only Content: %u\n", __FUNCTION__, __LINE__, ulConfigData);
			bDigitalVideoDisable = FALSE;
			//bAnalogVideoDisable = TRUE;
		}
		
		if(SVC_OUT_SetHdcp(ulActionId, bHdcp) != ERR_OK)
		{
			HxLOG_Error("[%s, %d] Can't Enable Hdcp\n", __FUNCTION__, __LINE__);
		}
		if(bScms == TRUE)
		{
			if(SVC_OUT_SetScmsCopyRight(ulActionId, ulScmsValue) != ERR_OK) /* ulScmsValue : 0, 1, 2, 3 */
			{
				HxLOG_Error("[%s, %d] Can't Enable SCMS\n", __FUNCTION__, __LINE__);
			}
		}
		if(bCgms == TRUE)
		{
			if(SVC_OUT_SetCgmsMacrovision(ulActionId, ulCgmsValue) != ERR_OK) /* ulCgmsValue : 0, 1, 2, 3 */
			{
				HxLOG_Error("[%s, %d] Can't Enable CGMS\n", __FUNCTION__, __LINE__);
			}
		}
		if(bMacrovision == TRUE)
		{
			if(SVC_OUT_SetMacrovision(ulActionId, ulMacrovisionValue) != ERR_OK) /* ulMacrovisionValue : 0, 1, 2, 3 */
			{
				HxLOG_Error("[%s, %d] Can't Enable Macrovision\n", __FUNCTION__, __LINE__);
			}
		}

		if( bDigitalVideoDisable == TRUE || bAnalogVideoDisable ==TRUE )
		{
			bVideoDisable = TRUE;
		}

		if(bVideoDisable == TRUE && SVC_AV_SetVideoHide(ulActionId, eAvMuteFlag_Drm, bVideoDisable) != ERR_OK)
		{
			HxLOG_Error("[%s, %d] Can't SVC_AV_SetVideoHide\n", __FUNCTION__, __LINE__);
		}
		if(bDigitalAudioDisable == TRUE && SVC_AV_SetAudioHide(ulActionId, eAvMuteFlag_Drm, bDigitalAudioDisable) != ERR_OK)
		{
			HxLOG_Error("[%s, %d] Can't SVC_AV_SetAudioHide\n", __FUNCTION__, __LINE__);
		}
	}
	return ERR_BUS_MESSAGE_BREAK;		

	case eMEVT_MEDIA_DVB_SUBTITLE_START:
	{
		Handle_t hAction = SVC_PIPE_GetActionHandle(ulActionId);

		if ( hAction != HANDLE_NULL )
		{
			SVC_SBTL_StopService(hAction, 0);
			SVC_SBTL_StartService(hAction, (HUINT16)param1, (HUINT16)param2, (HUINT16)param3);
		}
		else
		{
			HxLOG_Error("[%s, %d] hAction is null \n", __FUNCTION__, __LINE__);
		}
	}	
	return ERR_BUS_MESSAGE_BREAK;

	case eMEVT_MEDIA_DVB_SUBTITLE_STOP:
	{
		Handle_t hAction = SVC_PIPE_GetActionHandle(ulActionId);

		if ( hAction != HANDLE_NULL )
		{
			SVC_SBTL_StopService(hAction, 0);
		}
		else
		{
			HxLOG_Error("[%s, %d] hAction is null \n", __FUNCTION__, __LINE__);
		}
	}	
	return ERR_BUS_MESSAGE_BREAK;

	
#endif	
	default:
		break;
	}

	return BUS_ProcessMessageDefault(message, handle, param1, param2, param3);
}

