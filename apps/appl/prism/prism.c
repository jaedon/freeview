/**************************************************************
*	http://www.humaxdigital.com
*	@author			humax
**************************************************************/


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
* ⓒ 2011-2012 Humax Co., Ltd.
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
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

/**************************************************************************************************/
#define _________PRISM_Private_Include_________________________________________________
/**************************************************************************************************/
#include	<prism.h>

#if defined(CONFIG_HUMAX_CRASHREPORT)
#include "hcrlib.h"
#endif
#if defined(CONFIG_NETFLIX_APP)
#include "netflixmanager.h"
#endif


/**************************************************************************************************/
#define _________PRISM_Private_Macro_________________________________________________
/**************************************************************************************************/
#define	PRISM_DB_LASTSERVICE		"LastMasterUid"
/**************************************************************************************************/
#define _________PRISM_Private_Struct_________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define _________PRISM_Private_Value_________________________________________________
/**************************************************************************************************/
static HUINT32						 s_ulBootTimeTick   = 0;
static HINT32 						 s_KeyboardMode;
static HBOOL						 s_bGoStandBy		= FALSE;
static HBOOL						 s_bluetoothState	= FALSE;

static ScreenSaver_Mode_e   		 s_screensaverMode 	= eMODE_LIVE;
static OxMediaPlay_SubtitleType_e	 s_eSBTL_Type 		= eOxMP_SUBTITLETYPE_Unknown; // teletext 가 끝나고 EBU 의 시작을 위해
static OxMediaPlay_LiveLockState_e	 s_eLockState		= eOxMP_LiveLockState_OK;

static HBOOL						 s_bSystemInitialized = FALSE;
static HBOOL						 s_bReadyMainApplication = FALSE;
static HSystemOperationMode_e		 s_eSystemOperationMode = eHSystemOperationMode_UNKNOWN;
static HBootUpReason_e				 s_eBootUpReason = eHBootUpReason_UNKNOWN;
static HBOOL						 s_bIsOTA			= FALSE;
#if defined(CONFIG_APWEB_NATIVE_MEM_INFO)
static HBOOL						 s_keyCountForMemInfo	= 0;
#define 	USERDB_ENABLE_PRINTMEMINFO		"UDB/Enable_Print_MemInfo"
#endif
#if defined(CONFIG_NETFLIX_APP)
static NFLIX_NotifyState_t s_eNetflixAppState = NFLIX_NOTIFY_UNDEFINED;
#endif
/**************************************************************************************************/
#define _________PRISM_Private_Prototype_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________PRISM_Private_functions________________________________________________
/**************************************************************************************************/
static void _prism_appkit_MEDIA_Notify( HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstNotifyData, void *pvUserData )
{
	HUINT32 ulParam1	=	0;
	HUINT32 ulParam2	=	0;
	HUINT32 ulParam3	=	0;

	ulParam1	=	((ulViewId & 0x000F) << 24) | ((eType & 0x000F) << 16) | (eEvent & 0x00FF);

	if (NULL != pstNotifyData)
	{
#if defined(CONFIG_TIMESTAMP_64)
		ulParam2	=	(HUINT32)pstNotifyData->stEventNotify.ullParam1;
		ulParam3	=	(HUINT32)pstNotifyData->stEventNotify.ullParam2;
#else
		ulParam2	=	pstNotifyData->stEventNotify.ulParam1;
		ulParam3	=	pstNotifyData->stEventNotify.ulParam2;
#endif
	}

	ONDK_Print(HxANSI_COLOR_YELLOW("[%s:%d] Appkit notify\n"),__FUNCTION__,__LINE__);
	ONDK_GWM_RECV_PostMsgToGWM(MSG_GWM_APK_MESSAGE,GWM_APK_MEDIA, ulParam1, ulParam2, ulParam3);
}

#if defined(CONFIG_SUPPORT_NIMA)
static void _prism_appkit_BLUETOOTH_Notify(HINT32 eEvent,HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	ONDK_Print("[%s:%d] Appkit notify\n",__FUNCTION__,__LINE__);
	ONDK_GWM_RECV_PostMsgToGWM(MSG_GWM_APK_MESSAGE,GWM_APK_BLUETOOTH, eEvent, ulParam1, ulParam2);
}
#endif

static void _prism_appkit_BOXINFO_Notify(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	ONDK_Print("[%s:%d] Appkit notify\n",__FUNCTION__,__LINE__);
	ONDK_GWM_RECV_PostMsgToGWM(MSG_GWM_APK_MESSAGE,GWM_APK_BOX, ulParam1, ulParam2, ulParam3);
}

static void _prism_appkit_HDMI_Notify(void)
{
	ONDK_Print("[%s:%d] Appkit notify\n",__FUNCTION__,__LINE__);
	ONDK_GWM_RECV_PostMsgToGWM(MSG_GWM_APK_MESSAGE,GWM_APK_HDMI, 0, 0, 0);
}

static void _prism_appkit_SQCSCENARIO_Notify(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	ONDK_Print("[%s:%d] Appkit notify\n",__FUNCTION__,__LINE__);
	ONDK_GWM_RECV_PostMsgToGWM(MSG_GWM_APK_MESSAGE,GWM_APK_SQC_SCENARIO, ulParam1, ulParam2, ulParam3);
}

static void _prism_appkit_AUDIO_Notify(void)
{
	ONDK_Print("[%s:%d] Appkit notify\n",__FUNCTION__,__LINE__);
	ONDK_GWM_RECV_PostMsgToGWM(MSG_GWM_APK_MESSAGE,GWM_APK_SQC_AUDIO, 0, 0, 0);
}


#if defined (CONFIG_PRISM_ZAPPING)
static void _prism_appkit_STARTLOGO_ZappingNotify(void)
{
	STATIC HBOOL bColdBootDone = FALSE;

	if(bColdBootDone == FALSE)
	{
		APK_SYSINFO_STB_GetColdBootDone( &bColdBootDone, NULL);

		if(bColdBootDone == TRUE)
		{
			ONDK_GWM_RECV_PostMsgToGWM(MSG_GWM_APK_MESSAGE,GWM_APK_SYSINFO, 0, 0, 0);
		}
	}
}
#endif

static void _prism_signal_showsubtitle(HINT32 value)
{
	OxMediaPlay_SubtitleType_e eSBTL_Type =AP_SubtitleGetType();

	HxLOG_Trace();
	s_eSBTL_Type =eSBTL_Type;
	HxLOG_Debug("[%s:%d] value [%d]s_eSBTL_Type [%d]\n",__FUNCTION__,__LINE__,value,s_eSBTL_Type);

	if(value == FALSE) // HIDE...
	{
		ONDK_GWM_App_t			pGwmApp = NULL;
		pGwmApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)AP_Teletext_Proc);
		if(pGwmApp)
		{
			ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)AP_Teletext_Proc);  // EBU
		}
		ONDK_GWM_SendMessage(NULL, MSG_SIGNAL_SHOWSUBTITLE, HANDLE_NULL, 0, 0, 0); // DVB
		return;
	}

	if(eSBTL_Type == eOxMP_SUBTITLETYPE_Dvb || eSBTL_Type == eOxMP_SUBTITLETYPE_Media)
	{
	 	if(ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)AP_Subtitle_Proc) ==NULL) //Active standby  에서 다시 살아날 경우.
	 	{
		   ONDK_GWM_APP_Create(ONDK_LOWER_WINDOW,"SUBTITLE", APP_OSD_DRAW_SBTL_TTX_PRIOTITY,AP_Subtitle_Proc, HANDLE_NULL, 0, 0,0);
	 	}
		HxLOG_Print(HxANSI_COLOR_PURPLE("[%s][%s][DVB]Subtitle..\n"), __FUNCTION__,(value == TRUE)? "SHOW" :"HIDE");
		ONDK_GWM_SendMessage(NULL, MSG_SIGNAL_SHOWSUBTITLE, HANDLE_NULL, 0, 0, value);
	}
	else if(eSBTL_Type == eOxMP_SUBTITLETYPE_Ebu)
	{
		ONDK_GWM_App_t			pGwmApp = NULL;

		HxLOG_Print("<---- EBU Subtitle -- Check Lock State...\n");
		if(s_eLockState == eOxMP_LiveLockState_OK)
		{
			HxLOG_Print(HxANSI_COLOR_PURPLE("[%s][%s][EBU]Subtitle..\n"),  __FUNCTION__,(value == TRUE)? "SHOW" :"HIDE");
			pGwmApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)AP_Teletext_Proc);
			if(pGwmApp)
			{
				ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)AP_Teletext_Proc);
			}

			if(value == TRUE)
			{
				 ONDK_GWM_APP_Create(ONDK_LOWER_WINDOW,"Teletext", APP_OSD_DRAW_SBTL_TTX_PRIOTITY, AP_Teletext_Proc, HANDLE_NULL, TRUE, 0,0);
			}
		}
	}
}


static ONDK_Result_t _prism_apk_MediaMessage(Handle_t hAct,HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR							 hErr;
	static HBOOL					 bNeedRadioBG = FALSE;
	OxMediaPlay_LiveViewState_e	 	 eViewState;
	OxMediaPlay_LiveLockState_e	 	 eLockState;
	ONDK_Result_t					 ret = GWM_RET_OK;
	HINT32 							 nSpeed = 100; //play

	HUINT32					ulViewId		=	(p1 >> 24) & 0x000F;
	OxMediaPlay_MediaType_e	eType			=	(p1 >> 16) & 0x000F;
	OxMediaPlay_Event_e		eEvent			=	(p1 & 0x00FF);


	HxLOG_Print(">>Receive APK event[%d]\n",eEvent);
	switch ( eEvent )
	{
		case 	eOxMP_EVENT_LIVE_LockStateChanged:
			{
				eLockState =p2;
				s_eLockState = eLockState;

				if (eLockState != eOxMP_LiveLockState_OK)
				{
 					HxLOG_Print(">>>>>>>>>>Screen Saver Start Timer...event[%d]\n",eLockState);
					ONDK_GWM_SetTimer(GWMPOPUP_TIMER_MAIN, SCREENSAVER_OPERATION_LIVE_TIMEOUT);
				}
				else
				{
					APK_MEDIA_PLAY_GetViewState(ulViewId, &eViewState);
					if(s_eSBTL_Type == eOxMP_SUBTITLETYPE_Ebu)
					{
						HxLOG_Debug(HxANSI_COLOR_PURPLE("[%s][SHOW][EBU]Subtitle..\n"),  __FUNCTION__);
						if(ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)AP_Teletext_Proc))
						{
							ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)AP_Teletext_Proc);
						}
						 ONDK_GWM_APP_Create(ONDK_LOWER_WINDOW,"Teletext", APP_OSD_DRAW_SBTL_TTX_PRIOTITY, AP_Teletext_Proc, HANDLE_NULL, TRUE, 0,0);
					}

					APK_MEDIA_PLAY_GetNeedRadioBg(ulViewId,&bNeedRadioBG);
					if(bNeedRadioBG == FALSE)
					{
						if (eViewState == eOxMP_LiveViewState_OK)
						{
							ONDK_GWM_KillTimer(GWMPOPUP_TIMER_MAIN);
						}
					}

					if (ONDK_GWM_APP_Get(AP_Screensaver_Proc) != NULL)
					{
						HxLOG_Print("Destroy ScreenSaver %d, %d\n",eEvent, eLockState);
						ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)AP_Screensaver_Proc);
					}
				}
			}

			ret =  GWM_RET_OK;
			break;
		case	eOxMP_EVENT_LIVE_ViewStateChanged:
			{
				HUINT32	ulMainViewId;
				eViewState = p2;

				if (eViewState == eOxMP_LiveViewState_OK)
				{
					if(s_screensaverMode == eMODE_LIVE)
					{
						ONDK_GWM_KillTimer(GWMPOPUP_TIMER_MAIN);
					}
					else if(s_screensaverMode == eMODE_RADIO)
					{
							ONDK_GWM_SetTimer(GWMPOPUP_TIMER_MAIN, SCREENSAVER_OPERATION_MEDIA_TIMEOUT);
					}

					if(ONDK_GWM_APP_Get(AP_Screensaver_Proc)!= NULL)
					{
						ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)AP_Screensaver_Proc);
					}

					if (APK_MEDIA_PLAY_GetMainViewId(&ulMainViewId) == ERR_OK)
					{
						if (ulMainViewId == ulViewId)
						{
							HUINT32	ulSavedUid;
							HUINT32	ulMasterSvcUid;

							APK_MEDIA_PLAY_GetRequestId(ulViewId, eType, &ulMasterSvcUid);
							if ((HAPI_GetRegistryInt(PRISM_DB_LASTSERVICE, &ulSavedUid) == ERR_OK) && (ulSavedUid == ulMasterSvcUid))
							{
							}
							else
							{
								HxLOG_Debug("[%s:%d] Write last master service uid [%d]\n", __FUNCTION__, __LINE__, ulMasterSvcUid);
								HAPI_SetRegistryInt(PRISM_DB_LASTSERVICE, ulMasterSvcUid);
							}
						}
					}
				}
				else
				{
					ONDK_GWM_SetTimer(GWMPOPUP_TIMER_MAIN, SCREENSAVER_OPERATION_LIVE_TIMEOUT);
				}
			}
			ret =  GWM_MESSAGE_BREAK;
			break;
		case	eOxMP_EVENT_PLAYSTATE:
			if(eType == eOxMP_MEDIATYPE_LIVE)
			{
				APK_MEDIA_PLAY_GetNeedRadioBg(ulViewId,&bNeedRadioBG);
				if(bNeedRadioBG == TRUE)
				{
					s_screensaverMode = eMODE_RADIO;
				}
				else
				{
					s_screensaverMode = eMODE_LIVE;
				}
			}
			else if(eType == eOxMP_MEDIATYPE_MEDIAPLAY) // red key 비디오들이 올수있다.
			{
				ONDK_GWM_KillTimer(GWMPOPUP_TIMER_MAIN);
#if defined(CONFIG_NETFLIX_APP)
				if (s_eNetflixAppState == NFLIX_NOTIFY_START_NORMAL)
				{
					/* Netflix foreground launch 시 screenSaver 지움 */
					/* Destroy screenSaver when Netflix launching by DIAL */
					if (ONDK_GWM_APP_Get(AP_Screensaver_Proc) != NULL)
					{
						HxLOG_Print(">>Destroy ScreenSaver (event:%d)\n", eEvent);
						ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)AP_Screensaver_Proc);
					}
				}
#endif
			#if 0
				if(APK_MEDIA_PLAY_GetPlayState(ulViewId, &ePlayState) == ERR_OK)
				{
					HCHAR url[768]={0,};
					if(APK_MEDIACTRL_MEDIAPB_GetURI(eDest,url) == ERR_OK)
					{
						if(HxSTD_StrStr(url,".mp3"))
						{
							s_screensaverMode = eMODE_MUSIC;
							ONDK_GWM_SetTimer(GWMPOPUP_TIMER_MAIN, SCREENSAVER_OPERATION_MEDIA_TIMEOUT);
						}
						else
							ONDK_GWM_KillTimer(GWMPOPUP_TIMER_MAIN);
					}

				}
				#endif
			}

			ret =  GWM_MESSAGE_BREAK;
			break;
		#if 0
		case eOxMP_EVENT_SPEED_CHANGED	:
			if (APK_MEDIA_PLAY_GetPlaySpeed(eDest,&PlaySpeed) == ERR_OK)
			{
				HINT32 timeout = SCREENSAVER_OPERATION_LIVE_TIMEOUT;
				HxLOG_Debug(">>> Speed Change....[%d]\n",PlaySpeed);
				if(PlaySpeed ==0 )
				{
					s_screensaverMode = eMODE_LIVE;
					eMediaType = APK_MEDIACTRL_GetCurrentMediaType(eDest);
					if(eMediaType == eMctrlBase_MediaPlay)
					{
						HCHAR url[768]={0,};
						if(APK_MEDIACTRL_MEDIAPB_GetURI(eDest,url) == ERR_OK)
						{
							if(HxSTD_StrStr(url,".mp3"))
							{
								s_screensaverMode = eMODE_MUSIC;
								timeout = SCREENSAVER_OPERATION_MEDIA_TIMEOUT;
							}
						}
					}
					ONDK_GWM_SetTimer(GWMPOPUP_TIMER_MAIN, timeout);
				}
				else
					ONDK_GWM_KillTimer(GWMPOPUP_TIMER_MAIN);
			}
			ret =  GWM_MESSAGE_BREAK;
			break;
		#endif
		case eOxMP_EVENT_STARTED:
			{
				if( (eType == eOxMP_MEDIATYPE_PVRPLAYBACK) ||
					(eType == eOxMP_MEDIATYPE_TSRPLAYBACK) ||
					(eType == eOxMP_MEDIATYPE_MEDIAPLAY) )
				{
					/*
					 * author : hsseo
					 * note : MainAction이 Playback중 ScreenSaver 시나리오(PB는 without ScreenSaver) 적용.
					 */
					HERROR hErr;
					hErr = APK_MEDIA_PLAY_GetNeedRadioBg(ulViewId,&bNeedRadioBG);
					if( hErr == ERR_OK )
					{
						if (TRUE == bNeedRadioBG)
							s_screensaverMode = eMODE_RADIO;
						else
							s_screensaverMode = eMODE_LIVE;
					}
					else
					{
						HxLOG_Error("can't call to func. 'APK_MEDIA_PLAY_GetNeedRadioBg()' @ret (%d) ", hErr);
					}

					/*without ScreenSaver*/
					ONDK_GWM_KillTimer(GWMPOPUP_TIMER_MAIN);
#if defined(CONFIG_NETFLIX_APP)
					if ((eType == eOxMP_MEDIATYPE_MEDIAPLAY) && (s_eNetflixAppState == NFLIX_NOTIFY_START_NORMAL))
					{
						/* Netflix foreground launch 시 screenSaver 지움 */
						/* Destroy screenSaver when starting Netflix playback by DIAL, ... */
						if (ONDK_GWM_APP_Get(AP_Screensaver_Proc) != NULL)
						{
							HxLOG_Print(">>Destroy ScreenSaver (event:%d)\n", eEvent);
							ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)AP_Screensaver_Proc);
						}
					}
#endif
				}
			}
			break;
		case eOxMP_EVENT_SPEED_CHANGED:
			{
				HUINT32		ulMainViewId;
				APK_MEDIA_PLAY_GetMainViewId(&ulMainViewId);

				if( ulViewId != ulMainViewId )
				{
					break;
				}

				/* media-playback 에서 pause 시 ScreenSaver trigger*/


				hErr = APK_MEDIA_PLAY_GetPlaySpeed(ulViewId, &nSpeed);
				if( ERR_OK == hErr )
				{
					if( 0 == nSpeed ) /*pause*/
					{
						ONDK_GWM_SetTimer(GWMPOPUP_TIMER_MAIN, SCREENSAVER_OPERATION_MEDIA_TIMEOUT);
					}
					else
					{
						ONDK_GWM_KillTimer(GWMPOPUP_TIMER_MAIN);
#if defined(CONFIG_NETFLIX_APP)
						if (s_eNetflixAppState == NFLIX_NOTIFY_START_NORMAL)
						{
							/* Netflix foreground launch 시 screenSaver 지움 */
							/* Destroy screenSaver when starting DIAL(netflix, youtube) based media playback */
							if( ONDK_GWM_APP_Get(AP_Screensaver_Proc) != NULL)
							{
								HxLOG_Print(">>Destroy ScreenSaver (event:%d, speed:%d)\n", eEvent, nSpeed);
								ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)AP_Screensaver_Proc);
							}
						}
#endif
					}
				}
				else
				{
					HxLOG_Error("can't call to func. 'APK_MEDIA_PLAY_GetPlaySpeed()'\n");
				}
				HxLOG_Debug("nSpeed :%d\n", nSpeed);
			}
			break;
		case eOxMP_EVENT_RADIOBG_CHANGED:
			if (s_bIsOTA)
			{
				ret = GWM_RET_OK;
				break;
			}
			APK_MEDIA_PLAY_GetNeedRadioBg(ulViewId,&bNeedRadioBG);

			if(bNeedRadioBG == TRUE)
			{
				if( eOxMP_MEDIATYPE_LIVE == eType || eOxMP_MEDIATYPE_TSRPLAYBACK == eType)
					ONDK_GWM_SetTimer(GWMPOPUP_TIMER_MAIN, SCREENSAVER_OPERATION_MEDIA_TIMEOUT);
				s_screensaverMode = eMODE_RADIO;
			}
			else
			{
				s_screensaverMode = eMODE_LIVE;
			}

			HxLOG_Debug(HxANSI_COLOR_YELLOW("_________________ Noti.!!! [%d]\n"),bNeedRadioBG);
			if(bNeedRadioBG == TRUE  && (ONDK_GWM_APP_Get(AP_Radio_Proc) ==NULL))
			{
			   ONDK_GWM_APP_Create(ONDK_LOWER_WINDOW,"RADIO", APP_BGIMAGE_PRIORITY, AP_Radio_Proc,0, 0, 0,0);
			}
			else if(bNeedRadioBG == FALSE && (ONDK_GWM_APP_Get(AP_Radio_Proc) !=NULL))
			{
				ONDK_GWM_APP_Destroy(AP_Radio_Proc);
			}

			HxLOG_Debug(HxANSI_COLOR_YELLOW("eOxMP_EVENT_RADIOBG_CHANGED Noti.!!! [%d]\n"),bNeedRadioBG);
			ret =  GWM_RET_OK;
			break;
#if defined(CONFIG_NETFLIX_APP)
		case eOxMP_EVENT_LIVE_SvcStopped:
			{
				/* Netflix Launch에 대한 직접적인 appkit 이벤트가 없음 -> LIVE_SvcStopped 사용  */
				/* Enabled screenSaver timer in gallery UI after netflix launch                 */
				/* This is for passing the NTS certi (SUSP-007-TC1) by Netflix requirements     */
				HxLOG_Print(">>Receive APK LIVE_SvcStopped netflix(%d) \n", s_eNetflixAppState);

				if (s_eNetflixAppState == NFLIX_NOTIFY_START_NORMAL)
				{
					ONDK_GWM_KillTimer(GWMPOPUP_TIMER_MAIN);

					/* Netflix foreground launch 시 screenSaver 지움 */
					/* Destroy screenSaver when Netflix launching by DIAL */
					if( ONDK_GWM_APP_Get(AP_Screensaver_Proc) != NULL)
					{
						HxLOG_Print(">>Destroy ScreenSaver (event:%d)\n", eEvent);
						ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)AP_Screensaver_Proc);
					}
				}
			}

			ret =  GWM_RET_OK;
			break;
#endif /* (CONFIG_NETFLIX_APP) */
		default:
			ret =  GWM_RET_OK;
			break;
	}

	ret =  GWM_MESSAGE_BREAK;

	return ret;
}


static ONDK_Result_t _prism_apk_HDMIMessage(Handle_t hAct,HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		ret = GWM_RET_OK;
	DxHdcpStatus_e		eHdcpStatus;
	DxNflixDrmStatus_e eNflixDrmStatus;

	APK_OUTPUT_HDMI_GetNetflixDrmStatus(&eNflixDrmStatus);
	APK_OUTPUT_HDMI_GetHdcpStatus(&eHdcpStatus);

	if(eHdcpStatus == eDxHDCPSTATUS_OK || eHdcpStatus == eDxHDCPSTATUS_OFF)
	{
		HxLOG_Debug("[%s:%d][status : eDxHDCPSTATUS_OK || eDxHDCPSTATUS_OFF] \r\n",__FUNCTION__, __LINE__);
		if(ONDK_GWM_APP_Get(AP_HDMI_Proc)!= NULL)
		{
			ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)AP_HDMI_Proc));
			ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)AP_HDMI_Proc);
		}
	}
	else if(eHdcpStatus == eDxHDCPSTATUS_FAIL || eHdcpStatus == eDxHDCPSTATUS_NODEVICE)
	{
		HxLOG_Debug("[%s:%d][status : eDxHDCPSTATUS_FAIL || eDxHDCPSTATUS_NODEVICE] \r\n",__FUNCTION__, __LINE__);
		if(ONDK_GWM_APP_Get(AP_HDMI_Proc) != NULL)
		{
			ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)AP_HDMI_Proc));
			ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)AP_HDMI_Proc);
		}

		HxLOG_Debug("[%s:%d][eNflixDrmStatus = %d\n]",__FUNCTION__, __LINE__,eNflixDrmStatus);

		if((eNflixDrmStatus == eNFLIX_DRM_RUNNING )||(eNflixDrmStatus == eNFLIX_DRM_NA ))
		{
		    	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW,"ONDK_HDMI", APP_DEFAULT_POPUP_PRIORITY, AP_HDMI_Proc, HANDLE_NULL, p1,p2,p3);
	    	}
		else if(eNflixDrmStatus == eNFLIX_DRM_INIT)
		{
			/* Not to Create Prism HDCP fail UI on NETFLIX_DRM_INIT status */
		}
	}
	else if(eHdcpStatus == eDxHDCPSTATUS_TRY)
		HxLOG_Debug("[%s:%d][status : eDxHDCPSTATUS_TRY] \r\n",__FUNCTION__, __LINE__);

	ONDK_GWM_SendMessage(NULL, MGS_APP_HDCP_STATUS, HANDLE_NULL,(HINT32)eHdcpStatus, 0, 0);
	ret =  GWM_MESSAGE_BREAK;
	return ret;
}


static ONDK_Result_t _prism_apk_SQC_SCENARIOMessage(Handle_t hAct,HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t					 ret = GWM_RET_OK;
	HCHAR lang[4]={0,};
	APK_SCENARIO_GetMainLanguage(lang);
	HxLOG_Debug("Set Language [%s]\n",lang);
	ONDK_SetLanguage(lang);
	ret =  GWM_MESSAGE_BREAK;
	return ret;
}


static ONDK_Result_t _prism_apkmessage(Handle_t hAct,HINT32 p1, HINT32 p2, HINT32 p3)
{
	GWM_APK_MSG_Type_e	eAPK_Type = (GWM_APK_MSG_Type_e)hAct;
	ONDK_Result_t		ret = GWM_RET_OK;
	switch(eAPK_Type)
	{
	case GWM_APK_MEDIA:
		ret = _prism_apk_MediaMessage(hAct,p1,p2,p3);
		break;

	case GWM_APK_HDMI:
		ret = _prism_apk_HDMIMessage(hAct,p1,p2,p3);
		break;

	case GWM_APK_SQC_SCENARIO:
		ret =_prism_apk_SQC_SCENARIOMessage(hAct,p1,p2,p3);
		break;

	case GWM_APK_SQC_AUDIO:
		break;

	case GWM_APK_BLUETOOTH:

		switch(p1)
		{
			case eNETWORK_STATUS_BT_AUDIO_MUTE:
				if(ONDK_GWM_APP_Get(AP_AudioVolume_Proc) != NULL)
				{
					ONDK_GWM_SendMessage(NULL, MSG_GWM_APK_BLUETOOTH, eNETWORK_STATUS_BT_AUDIO_MUTE, p1, p2, p3);
				}
				else
				{
						s_bluetoothState = p2;
				}
				break;
			default:
				break;
		}
		break;
#if defined(CONFIG_PRISM_ZAPPING)
   	case GWM_APK_SYSINFO :
			if(ONDK_GWM_APP_Get(AP_Startlogo_Proc) != NULL)
			{
				ONDK_GWM_SendMessage(NULL, MSG_GWM_APK_SYSINFO, HANDLE_NULL, p1, p2, p3);
			}
			break;
#endif
	default:
		break;
	}
	return ret;
}

static void _prism_deliveryPacket(HINT32 size, void *pvData)
{
	HINT32		len;
	HUINT8		*p = (HUINT8*)pvData;
	HCHAR		*dataType;

	len = HxMACRO_Get32Bit(p);			p += 4;
	dataType = p;						p += len;
	len = size - (len + 4);

	if (HLIB_STD_StrCaseCmp(dataType, "ttxpes") == 0) //teletext ..
    {
         Teletext_deliveryPacket(dataType,len,p);
    }
	else  //subtitle..
    {
    	HUINT8 *tempMem = NULL;
		tempMem = HLIB_STD_MemAlloc(size);
		if (NULL != tempMem)
		{
			HxSTD_MemCopy(tempMem, pvData, size);
			ONDK_GWM_RECV_PostDataToGWM(MSG_APP_RCV_SUBTITLE_PACKET, 0, (HUINT32)0, size, (HUINT32)tempMem, (void *)tempMem, HLIB_STD_MemFree_CB);
		}
	}
}

static void _prism_launch(void)
{
	HINT32					nValue = FALSE;
	DxWakeUpReason_e		eWakeupReason = eDxWAKEUP_NONE;

#if defined(CONFIG_SUPPORT_NIMA)
	APK_NETWORK_Init();  // Bluetooth event를 받아서 처리해야 하는데, bluetooth가 nima에 포함되어 있어서 network init을 함
#endif
	APK_OUTPUT_PANEL_Init();
	APK_SYSINFO_STB_Init();
//	APK_OUTPUT_VIDEO_Init();
	APK_SCENARIO_Init();
	APK_POWER_Init();
	APK_META_SVC_Init(FALSE);

	APK_MEDIA_PLAY_Init();
	APK_MEDIA_RECORD_Init();

	//  Register MediaControl Callback
	{
		HUINT32		ulViewId;
		APK_MEDIA_PLAY_GetMainViewId(&ulViewId);
		(void)APK_MEDIA_PLAY_RegisterNotifier(ulViewId, _prism_appkit_MEDIA_Notify, NULL, NULL);
	}

	APK_POWER_RegisterNotifier(_prism_appkit_BOXINFO_Notify);
#if defined(CONFIG_SUPPORT_NIMA)
	APK_NETWORK_RegisterNotifier(_prism_appkit_BLUETOOTH_Notify);
#endif

	APK_OUTPUT_AUDIO_Init();
	APK_OUTPUT_AUDIO_RegisterNotifier(_prism_appkit_AUDIO_Notify);

	APK_OUTPUT_HDMI_Init();
	APK_OUTPUT_HDMI_RegisterNotifier(_prism_appkit_HDMI_Notify);

 	APK_OUTPUT_VIDEO_Init();
	APK_DB_Init();

	APK_FILEMGR_STORAGE_Init();

	APK_POWER_GetWakeupReason(&eWakeupReason);

	if (APK_POWER_GetLastStandby(&nValue) != ERR_OK)
	{
		nValue = FALSE;
	}

	if (!(eWakeupReason == eDxWAKEUP_TIMER
		|| (eWakeupReason == eDxWAKEUP_ACPOWER && nValue == TRUE)))
	{
		APK_OUTPUT_VIDEO_SetAvEnable(TRUE);
	}

#if defined(CONFIG_PRISM_ZAPPING)
    ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW,"STARTLOGO", APP_SCREEN_SAVER_PRIORITY, AP_Startlogo_Proc, HANDLE_NULL, 0,0,0);
#endif

    Teletext_Init();

   	APK_DATASTREAMER_InitDataStreamer(_prism_deliveryPacket);

	APK_DATASTREAMER_ConnectDataStreamer("dvbsubtitle");
	APK_DATASTREAMER_ConnectDataStreamer("ttxpes");
	APK_DATASTREAMER_ConnectDataStreamer("mediasubtitle");

//	ONDK_KEY_Active(TRUE);
{	// set language for ONDK strings
	HCHAR lang[4]={0,};
	APK_SCENARIO_GetMainLanguage(lang);
	HxLOG_Print(HxANSI_COLOR_YELLOW("######################################################\n") );
	HxLOG_Print(HxANSI_COLOR_YELLOW("ONDK_SetLanguage [%s]\n"),lang);
	HxLOG_Print(HxANSI_COLOR_YELLOW("######################################################\n") );
	ONDK_SetLanguage((HINT8*)lang);
}
#if defined(CONFIG_PROD_HMS1000T) || defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE) || defined (CONFIG_PROD_DIGIMAXT2)
	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW,"ONDK_FRONT", APP_DEFAULT_PRIORITY, AP_Front_Proc, HANDLE_NULL, 0, 0,0);
#endif

    ONDK_GWM_APP_Create(ONDK_LOWER_WINDOW,"SUBTITLE", APP_OSD_DRAW_SBTL_TTX_PRIOTITY,AP_Subtitle_Proc, HANDLE_NULL, 0, 0,0);

	// 초기 appkit 초기화 및 bootup change MSG 문제로 start logo proc 을 먼저 실행.

#if !defined(CONFIG_PRISM_ZAPPING)
	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW,"STARTLOGO", APP_SCREEN_SAVER_PRIORITY, AP_Startlogo_Proc, HANDLE_NULL, 0,0,0);
#endif
	// start logo 이후에 appkit 이 초기화 되. apk 이 초기화가 끝난 시점에야, 판낼을 조작할 수있다.
	ONDK_GWM_SendMessage(NULL, MSG_GWM_APK_INITIALIZED, HANDLE_NULL, 0, 0, TRUE);


#if defined(CONFIG_PRISM_ZAPPING)
	{
		HBOOL bColdBootDone = FALSE;
		APK_SYSINFO_STB_GetColdBootDone( &bColdBootDone, _prism_appkit_STARTLOGO_ZappingNotify);
	}
	HAPI_SendReadySignal(HLIB_STD_GetPID());
	ONDK_Error(">>>signal<<<prism send ready signal to HAPI!!!\n");
#endif
	ONDK_KEY_Active(TRUE);

	s_bSystemInitialized = TRUE;
}


ONDK_Result_t _prism_signal_receive(HAPIMSG_e eMsg, HINT32 p1, HINT32 p2, HINT32 p3)
{
	switch (eMsg)
	{
		case eHMSG_BOOTUPSTATE_CHANGE:
			{
				HSystemBootupStatus_t	*pstBootUpStatus = (HSystemBootupStatus_t*)p1;
				HINT32					nValue = FALSE;
				DxWakeUpReason_e		eWakeupReason = eDxWAKEUP_NONE;

				HxLOG_Debug("[%s:%d] eHSIG_SystemBootupState received ..\n", __FUNCTION__, __LINE__);

				s_eSystemOperationMode = pstBootUpStatus->eSystemCurrentMode;
				s_eBootUpReason = pstBootUpStatus->eBootUpReason;

				//	System Bootup Flag가 생성되었다면...
				if (s_eBootUpReason != eHBootUpReason_UNKNOWN && s_eSystemOperationMode != eHSystemOperationMode_UNKNOWN)
				{
					if ((s_eSystemOperationMode == eHSystemOperationMode_NORMAL &&
						(s_eBootUpReason == eHBootUpReason_USER || s_eBootUpReason == eHBootUpReason_POWER)) && (s_bReadyMainApplication == FALSE))
					{
						HxLOG_Print(HxANSI_COLOR_YELLOW("######################################################\n") );
					    HxLOG_Print(HxANSI_COLOR_YELLOW("######################################################\n") );
					    HxLOG_Print(HxANSI_COLOR_YELLOW("################### Start Logo Start##################\n") );
					    HxLOG_Print(HxANSI_COLOR_YELLOW("######################################################\n") );
					    HxLOG_Print(HxANSI_COLOR_YELLOW("######################################################\n"));
						HxLOG_Warning("Launch Start Logo...\n");
						s_ulBootTimeTick = HLIB_STD_GetSystemTick();

#if !defined(CONFIG_PRISM_ZAPPING)
						APK_POWER_GetWakeupReason(&eWakeupReason);

						if (APK_POWER_GetLastStandby(&nValue) != ERR_OK)
						{
							nValue = FALSE;
						}

						if (!(eWakeupReason == eDxWAKEUP_TIMER
							|| (eWakeupReason == eDxWAKEUP_ACPOWER && nValue == TRUE)))
						{
							APK_OUTPUT_VIDEO_SetAvEnable(TRUE);	// operation 인경우 start logo 를 보여주기 위해.. 일단 이렇게..
						}
#endif
					}
				}
				HxLOG_Debug("[%s:%d] System BOOTUP Signal received [%d, %d]\n", __FUNCTION__, __LINE__, s_eSystemOperationMode, s_eBootUpReason);
			}
			break;

		case eHMSG_APP_READY:
			{
				HCHAR	*pszProcessName = HLIB_STD_GetProcessName(p1);

				HxLOG_Debug("[%s:%d] eHSIG_AppsReady Received...[%s]\n", __FUNCTION__, __LINE__, pszProcessName);
				if (HxSTD_StrEmpty(pszProcessName) == FALSE)
				{
					if (HLIB_STD_StrCaseCmp(pszProcessName, "obama") == 0)
					{
						if (s_bSystemInitialized == FALSE)
						{
							HxLOG_Warning("[%s:%d] OBAMA is just launched --> try to initialize APPKIT\n", __FUNCTION__, __LINE__);
							_prism_launch();
#if defined(CONFIG_PRISM_ZAPPING)

							{
								HUINT32		ulMasterViewId;
								HUINT32		ulLastSvcUid;
								HUINT32		ulSessionId;

								APK_MEDIA_PLAY_GetMainViewId(&ulMasterViewId);
								if (HAPI_GetRegistryInt(PRISM_DB_LASTSERVICE, &ulLastSvcUid) == ERR_OK)
								{
									OxMediaPlay_StartInfo_t	unStart;

									unStart.stLive.eType = eOxMP_MEDIATYPE_LIVE;
									unStart.stLive.eTuneMode = 0x00;
									unStart.stLive.ulMasterSvcUid = ulLastSvcUid;
									unStart.stLive.ulSuppleSvcUid = ulLastSvcUid;
									unStart.stLive.eSuppSvcType	= eOxMP_SUPPLESVCTYPE_Normal;
									unStart.stLive.ulMajorCHNum   = 0;				//	TODO:

									APK_MEDIA_PLAY_Start(ulMasterViewId, &unStart, &ulSessionId);
								}
							}
#endif
						}
					}
				}
			}
			return ONDK_MESSAGE_BREAK;

		case eHMSG_OSK_SHOW_SIMPLE:
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW,"SIMPLE_KEYBOARD", APP_DEFAULT_POPUP_PRIORITY, AP_SimpleKeyboardMain_Proc, HANDLE_NULL, 0, 0, p1);
			return ONDK_MESSAGE_CONSUMED;

		case eHMSG_OSK_MODECHANGE:
			s_KeyboardMode = p1;
			return ONDK_MESSAGE_CONSUMED;

		case eHMSG_OSK_SHOW:
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW,"ONDK_KEYBOARD", APP_DEFAULT_POPUP_PRIORITY, AP_KeyboardMain_Proc, HANDLE_NULL, 0, s_KeyboardMode, p1);
			return ONDK_MESSAGE_CONSUMED;

		case eHMSG_OSK_DESTROY:
			{
				ONDK_GWM_App_t			pGwmApp = NULL;
				pGwmApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)AP_KeyboardMain_Proc);

				if(pGwmApp)
				{
					ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)AP_KeyboardMain_Proc);
				}
			}
			return ONDK_MESSAGE_CONSUMED;

		case eHMSG_REMOTEAPP_STRING:
			{
				ONDK_GWM_App_t			pFullOSKApp = NULL;
				ONDK_GWM_App_t			pSimpleOSKApp = NULL;

				pFullOSKApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)AP_KeyboardMain_Proc);
				pSimpleOSKApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)AP_SimpleKeyboardMain_Proc);

				if(pFullOSKApp || pSimpleOSKApp)
				{
					ONDK_GWM_SendMessage(NULL, MSG_SIGNAL_REMOTESTING, HANDLE_NULL, p1, 0, 0);
				}
			}
			return ONDK_MESSAGE_CONSUMED;

		case eHMSG_SYSSTATE_CHANGE:
			{
				PxSTATUS_e	ePapiStatus = (PxSTATUS_e)p1;

				if(ePapiStatus == ePAMA_STATUS_Reboot || ePapiStatus == ePAMA_STATUS_Shutdown)
				{
					s_bGoStandBy = TRUE;
					ONDK_GWM_APP_DestroyAll();
					HxLOG_Debug("[%s:%d] Prism Bye..........signal[%d]\n", __FUNCTION__, __LINE__,eMsg);
					return ONDK_MESSAGE_PASS;
				}
				else if(ePapiStatus == ePAMA_STATUS_Operation)
				{
					s_bGoStandBy = FALSE;
					HxLOG_Debug("[%s:%d] Prism Operation..........signal[%d]\n", __FUNCTION__, __LINE__,eMsg);
					//	이러면 되려나???
				 	if(ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)AP_Subtitle_Proc) ==NULL) //Active standby  에서 다시 살아날 경우.
					{
						ONDK_GWM_APP_Create(ONDK_LOWER_WINDOW,"SUBTITLE", APP_OSD_DRAW_SBTL_TTX_PRIOTITY,AP_Subtitle_Proc, HANDLE_NULL, 0, 0,0);
				 	}

#if defined(CONFIG_APWEB_NATIVE_MEM_INFO)
					{
						HBOOL isEnalbe = FALSE;

						APK_DB_GetInt(USERDB_ENABLE_PRINTMEMINFO,&isEnalbe);
						if(isEnalbe == TRUE)
						{
							ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"AP_MemoryInfo_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU),
									(ONDK_GWM_Callback_t)AP_MemoryInfo_Proc, HANDLE_NULL, 0, 0, 0);
						}
					}
#endif
					return ONDK_MESSAGE_PASS;
				}
			}
			break;

		case eHMSG_INPUT_NOTIFY:
			ONDK_GWM_ResetTimer(GWMPOPUP_TIMER_MAIN);
			return ONDK_MESSAGE_PASS;

		case eHMSG_TELETEXT_STATE:
			{
#if defined(CONFIG_PROD_COUNTRY_AUSTRALIA)
				HUINT32 					ulViewId =0;
				HINT32 						nSvcUid =0;
				DxService_t 				*pService = NULL;
				OxMediaPlay_MediaType_e		 eMediaType;
#endif
				ONDK_GWM_App_t				pGwmApp = NULL;

				pGwmApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)AP_Teletext_Proc);
				HxLOG_Print(HxANSI_COLOR_YELLOW("######################################################\n") );
		        HxLOG_Print(HxANSI_COLOR_YELLOW("######################################################\n") );
		        HxLOG_Print(HxANSI_COLOR_YELLOW("################### [%s] Teletext ####################\n"),( (HBOOL)p1)?"SHOW":"HIDE");
		        HxLOG_Print(HxANSI_COLOR_YELLOW("######################################################\n") );
		        HxLOG_Print(HxANSI_COLOR_YELLOW("######################################################\n"));
				if(p1)
				{
// Austrilia does not support teletext
#if defined(CONFIG_PROD_COUNTRY_AUSTRALIA)
					if(APK_MEDIA_PLAY_GetMainViewId( &ulViewId ) == ERR_OK)
					{
						if(	APK_MEDIA_PLAY_GetPlayType(ulViewId, &eMediaType) == ERR_OK)
						{
							if(APK_MEDIA_PLAY_GetRequestId ( ulViewId, eMediaType, (HUINT32*)&nSvcUid) == ERR_OK)
							{
								pService = APK_META_SVC_GetService (nSvcUid);
								if(pService && pService->ttxPid == 0x2000) // PID_NULL
								{
									HxLOG_Print(HxANSI_COLOR_YELLOW(">TTX PID NULL\n"));
									APK_META_SVC_Delete (pService);
									return ONDK_MESSAGE_BREAK;
								}
								if(pService)
								{
									APK_META_SVC_Delete (pService);
								}
							}
						}
					}
#endif
					if(pGwmApp)
					{
						ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)AP_Teletext_Proc);
					}
				 	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW,"Teletext", APP_OSD_DRAW_SBTL_TTX_PRIOTITY, AP_Teletext_Proc,HANDLE_NULL, FALSE, 0,0);

				}
				if(!p1 && pGwmApp != NULL)
				{
					ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)AP_Teletext_Proc);
				}
			}
			return ONDK_MESSAGE_BREAK;

		case eHMSG_SUBTITLE_STATE:
			HxLOG_Debug(HxANSI_COLOR_YELLOW("eHMSG_SUBTITLE_STATE.[%s].\n"), ((OxMediaPlay_SubtitleType_e)p1 == eOxMP_SUBTITLETYPE_Dvb)? "DVB": "EBU" );
			_prism_signal_showsubtitle(p1);
			return ONDK_MESSAGE_BREAK;

		case eHMSG_SUBTITLE_TYPE_CHANGE:
			{
				OxMediaPlay_SubtitleType_e eType = (OxMediaPlay_SubtitleType_e)p1;
				ONDK_GWM_App_t			pGwmApp = NULL;
				pGwmApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)AP_Teletext_Proc);
				HxLOG_Debug(HxANSI_COLOR_YELLOW("eHSIG_SelectSubtitleType.[%s].\n"), ((OxMediaPlay_SubtitleType_e)p1 == eOxMP_SUBTITLETYPE_Dvb)? "DVB": "EBU" );

				if(eType == eOxMP_SUBTITLETYPE_Dvb)
				{
					if(pGwmApp)
					{
						ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)AP_Teletext_Proc);
					}

					ONDK_GWM_SendMessage(NULL, MSG_SIGNAL_SHOWSUBTITLE, HANDLE_NULL, 0, 0, TRUE);
				}
				else if(eType == eOxMP_SUBTITLETYPE_Ebu)
				{
					ONDK_GWM_SendMessage(NULL, MSG_SIGNAL_SHOWSUBTITLE, HANDLE_NULL, 0, 0, 0);

					if(pGwmApp)
					{
						ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)AP_Teletext_Proc);
					}
					ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW,"Teletext", APP_OSD_DRAW_SBTL_TTX_PRIOTITY, AP_Teletext_Proc, HANDLE_NULL, TRUE, 0,0);
				}
			}
			return ONDK_MESSAGE_BREAK;

		case eHMSG_MAINLANG_CHANGED:
			HxLOG_Warning(HxANSI_COLOR_YELLOW("Signal. Set Main Language[%s].\n"), HLIB_LANG_IdTo639(p1));
			if(ONDK_GWM_APP_Get(AP_HDMI_Proc) != NULL)
			{
				ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)AP_HDMI_Proc));
				ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)AP_HDMI_Proc);
				ONDK_SetLanguage((HCHAR*)HLIB_LANG_IdTo639(p1));
				ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW,"ONDK_HDMI", APP_DEFAULT_POPUP_PRIORITY, AP_HDMI_Proc, HANDLE_NULL, p1,p2,p3);
			}
			else
			{
				ONDK_SetLanguage((HCHAR*)HLIB_LANG_IdTo639(p1));
			}
			return ONDK_MESSAGE_BREAK;

		case eHMSG_MAINAPP_READY:
			s_bReadyMainApplication = TRUE;
			HxLOG_Warning("AllApplication ready!!! Tick [%d]\n" ,abs(s_ulBootTimeTick - HLIB_STD_GetSystemTick()));
			return ONDK_MESSAGE_BREAK;

		case eHMSG_PANEL_PRIORITY:
			// set priority
			APK_OUTPUT_PANEL_SetAppPriority((HINT32)p1);
			if (s_bSystemInitialized == TRUE)
			{
				// clear current priority
				APK_OUTPUT_PANEL_SetPanel(eApkPANEL_PRIORITY_LOWER, NULL, eDxPANEL_ALIGN_CENTER);
			}
			return ONDK_MESSAGE_BREAK;

		case eHMSG_SUBTITLE_FONTTYPE_CHANGE:
			Teletext_SetSutitleFontType((HBOOL)p1);
			return ONDK_MESSAGE_BREAK;

		case eHMSG_IS_OTA_CHECK:
			s_bIsOTA = (HBOOL)p1;
			return ONDK_MESSAGE_BREAK;

		case eHMSG_MSGBOX_SHOW:
			if(ONDK_GWM_APP_Get(AP_MSGBOX_Proc) != NULL)
			{
				ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)AP_MSGBOX_Proc));
				ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)AP_MSGBOX_Proc);
			}
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW,"ONDK_MSGBOX", APP_DEFAULT_POPUP_PRIORITY, AP_MSGBOX_Proc, HANDLE_NULL, p1,p2,p3);
			return ONDK_MESSAGE_BREAK;

#if defined(CONFIG_CAS_CONAX_FINGERPRINT)
		case eHMSG_FINGERPRINT_STATE:
			HxLOG_Print(HxANSI_COLOR_YELLOW("######################################################\n") );
			HxLOG_Print(HxANSI_COLOR_YELLOW("######################################################\n") );
			HxLOG_Print(HxANSI_COLOR_YELLOW("######################## Fingerprint ######################\n"));
			HxLOG_Print(HxANSI_COLOR_YELLOW("######################################################\n") );
			HxLOG_Print(HxANSI_COLOR_YELLOW("######################################################\n"));

			if(ONDK_GWM_APP_Get(AP_Fingerprint_Proc) != NULL)
			{
				ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)AP_Fingerprint_Proc));
				ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)AP_Fingerprint_Proc);
			}
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW,"ONDK_FINGERPRINT", APP_DEFAULT_POPUP_PRIORITY, AP_Fingerprint_Proc, HANDLE_NULL, p1,p2,p3);
			return ONDK_MESSAGE_BREAK;
#endif
		default:
			return ONDK_MESSAGE_PASS;
	}

	return ONDK_MESSAGE_PASS;
}

ONDK_Result_t _prism_timer(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	switch (ulParam1)
	{
		case GWMPOPUP_TIMER_MAIN:
#if defined(CONFIG_APUI_SCREEN_SAVER)
			if(ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)AP_Screensaver_Proc)== NULL)
			{
				ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW,"SCREENSAVER", APP_SCREEN_SAVER_PRIORITY, AP_Screensaver_Proc, HANDLE_NULL, s_screensaverMode,0,0);
			}
#endif

			break;
		default:
			break;
	}

	return GWM_RET_OK;
}

static ONDK_Result_t  _prism_trigger_EBUSubtitle(HBOOL bEBUSubtitle)
{
	ONDK_GWM_App_t			pGwmApp = NULL;
	if(s_eSBTL_Type == eOxMP_SUBTITLETYPE_Ebu && bEBUSubtitle != TRUE) // 일반 TELETEXT 가 종료되고, 기존 ebu 자막을 다시 열어준다.
	{
		HxLOG_Debug(HxANSI_COLOR_PURPLE("[%s][SHOW][EBU]Subtitle..\n"),__FUNCTION__);
		pGwmApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)AP_Teletext_Proc);
		if(pGwmApp)
		{
			ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)AP_Teletext_Proc);
		}

		ONDK_GWM_APP_Create(ONDK_LOWER_WINDOW,"Teletext", APP_OSD_DRAW_SBTL_TTX_PRIOTITY, AP_Teletext_Proc, HANDLE_NULL, TRUE, 0,0);
		return GWM_MESSAGE_BREAK;

	}
	return GWM_MESSAGE_PASS;
}

#if defined(CONFIG_APWEB_NATIVE_MEM_INFO)
HBOOL	_prism_check_HiddenKeySequenceForMemInfo(HINT32 lKeyCode)
{
	switch (lKeyCode)
	{
		case KEY_0:
			if(s_keyCountForMemInfo == 0)
			{
				s_keyCountForMemInfo++;
			}
			else
			{
				s_keyCountForMemInfo = 0;
			}
			break;

		case KEY_1:
			if(s_keyCountForMemInfo == 1)
			{
				s_keyCountForMemInfo++;
			}
			else if(s_keyCountForMemInfo == 7)
			{
				s_keyCountForMemInfo = 0;
				return TRUE;
			}
			else
			{
				s_keyCountForMemInfo = 0;
			}
			break;

		case KEY_2:
			if((s_keyCountForMemInfo == 2) || (s_keyCountForMemInfo == 6))
			{
				s_keyCountForMemInfo++;
			}
			else
			{
				s_keyCountForMemInfo = 0;
			}
			break;

		case KEY_3:
			if((s_keyCountForMemInfo == 3) || (s_keyCountForMemInfo == 5))
			{
				s_keyCountForMemInfo++;
			}
			else
			{
				s_keyCountForMemInfo = 0;
			}
			break;

		case KEY_4:
			if(s_keyCountForMemInfo == 4)
			{
				s_keyCountForMemInfo++;
			}
			else
			{
				s_keyCountForMemInfo = 0;
			}
			break;

		case KEY_VOLUMEUP:
		case KEY_VOLUMEDOWN:
			break;

		default:
			s_keyCountForMemInfo = 0;
			break;
	}
	return FALSE;
}
#endif

static void  _prism_check_HiddenKeySequence(HINT32 lKeyCode)
{
#if defined(CONFIG_APWEB_NATIVE_MEM_INFO)
	if (_prism_check_HiddenKeySequenceForMemInfo(lKeyCode) == TRUE)
	{
		if (ONDK_GWM_APP_Get(AP_AudioVolume_Proc) != NULL)
		{
			if (AP_AuidoVolume_GetHideState() != TRUE)
			{
				if (ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)AP_MemoryInfo_Proc) == NULL)
				{
					APK_DB_SetInt(USERDB_ENABLE_PRINTMEMINFO,TRUE);
					ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"AP_MemoryInfo_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU),
											(ONDK_GWM_Callback_t)AP_MemoryInfo_Proc, HANDLE_NULL, 0, 0, 0);
				}
				else
				{
					APK_DB_SetInt(USERDB_ENABLE_PRINTMEMINFO,FALSE);
					ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)AP_MemoryInfo_Proc);
				}
			}
		}
	}
#endif
}



/**************************************************************************************************/
#define _________PRISM_Public_Functions__________________________________________________________
/**************************************************************************************************/

ONDK_Result_t	AP_GWMStart_Proc(HINT32 message, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Print(HxANSI_COLOR_YELLOW("[%s:%d] Message(%d) : %s  [%d] p1(0x%x) p2(0x%x) p3(0x%x) \n"), __FUNCTION__, __LINE__, message, ONDK_GWM_GwmMsg2String(message), hAct, p1, p2, p3);
	switch (message)
	{
		case MSG_GWM_CREATE:
			return GWM_MESSAGE_BREAK;

		case MSG_GWM_KEYDOWN:

			if(s_bSystemInitialized == FALSE)
			{
				ONDK_Print("[%s:%d] s_bSystemInitialized == FALSE\r\n", __FUNCTION__, __LINE__);
				return ONDK_MESSAGE_NONCONSUMED;
			}
			if( s_bGoStandBy == TRUE)
			{
				ONDK_Print("[%s:%d] s_bGoStandBy == TRUE\r\n", __FUNCTION__, __LINE__);
				return ONDK_MESSAGE_NONCONSUMED;
			}

			_prism_check_HiddenKeySequence(p1);

			switch(p1)
			{
#if !defined(CONTROL_VOLUME_BY_WEB_APP)	/* this may be defined in apps/make/appl_make/prism/Makefile */
			case KEY_MUTE:
			case KEY_VOLUMEUP:
			case KEY_VOLUMEDOWN:
					ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW,"VOL1", APP_DIALOGBOX_PRIORITY, AP_AudioVolume_Proc, HANDLE_NULL, p1, 0,s_bluetoothState);
#if !defined(CONFIG_PROD_ROCKYPOM) && !defined(CONFIG_PROD_ROCKYWPOM)
					return ONDK_MESSAGE_CONSUMED;
#endif
#endif
			default:
				break;
			}
			return ONDK_MESSAGE_NONCONSUMED;
		case MSG_GWM_KEYUP:
			switch(p1)
			{
#if !defined(CONTROL_VOLUME_BY_WEB_APP)	/* this may be defined in apps/make/appl_make/prism/Makefile */
			case KEY_MUTE:
			case KEY_VOLUMEUP:
			case KEY_VOLUMEDOWN:
#if !defined(CONFIG_PROD_ROCKYPOM) && !defined(CONFIG_PROD_ROCKYWPOM)
				return ONDK_MESSAGE_CONSUMED; // 숨어있는 VOLUME PROC 이 키를 가져간다.
#endif
#endif
			default:
				break;
			}
			return ONDK_MESSAGE_NONCONSUMED;

		case MSG_APP_TELETEXT_STOPPED:
		 	return _prism_trigger_EBUSubtitle(p1);
		case MSG_GWM_HAPI_MESSAGE:
			return _prism_signal_receive((HAPIMSG_e)hAct, p1, p2, p3);
		case MSG_GWM_APK_MESSAGE:
			return _prism_apkmessage(hAct,p1,p2,p3);
		case MSG_GWM_TIMER:
			if (s_bIsOTA)
				return GWM_RET_OK;
			else
			return _prism_timer(p1,p2,p3);

		case MSG_GWM_DESTROY:
			ONDK_Error("[%s:%d] destroy app!!\r\n", __FUNCTION__, __LINE__);
			return GWM_MESSAGE_BREAK;

		default:
			break;
	}

	return ONDK_GWM_ProcessMessageDefault(message, hAct, p1, p2, p3);
}

#ifdef	CONFIG_DEBUG
static void	_prism_cmd_register(void)
{
}
#endif

#if defined(CONFIG_HUMAX_CRASHREPORT)
static void		_prism_signal_handler(int signal, siginfo_t *siginfo, void *context)
#else
static void		_prism_signal_handler(int signal, VK_SIG_INFO_t *siginfo, void *context)
#endif
{
	//	TODO: Add release all DFB Resource.
	ONDK_Destroy();
}

#define	PRISM_SIGNAL_BUF_SIZE		0x10000
static HxCBUF_t	s_pstMsgBuffer = NULL;

static HERROR	_prism_postMessage(HAPIMSG_e eMsg, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	ONDK_GWM_RECV_PostMsgToGWM(MSG_GWM_HAPI_MESSAGE, (Handle_t)eMsg, nParam1, nParam2, nParam3);

	return ERR_OK;
}
//	i : pid
static HERROR	_prism_onApplicationReady(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 2);

	HxLOG_Debug("[%s:%d] pid is [%s][%d]\n", __FUNCTION__, __LINE__, HxOBJECT_STR(apArgv[0]), HxOBJECT_INT(apArgv[1]));
	_prism_postMessage(eHMSG_APP_READY, HxOBJECT_INT(apArgv[1]), 0, 0);

	return ERR_OK;
}

//	b : Bootup
static HERROR	_prism_onBootupStateChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HSystemBootupStatus_t	*pstBootStatus;

	HxLOG_Assert(nArgc == 1);

	HxLOG_Trace();
	if (s_pstMsgBuffer == NULL)
		s_pstMsgBuffer = HLIB_CBUF_New(PRISM_SIGNAL_BUF_SIZE);
	pstBootStatus = (HSystemBootupStatus_t*)HLIB_CBUF_GetBuffer(s_pstMsgBuffer, sizeof(HSystemBootupStatus_t));
	HxSTD_MemCopy(pstBootStatus, HxOBJECT_BIN_DATA(apArgv[0]), HxOBJECT_BIN_SIZE(apArgv[0]));

	_prism_postMessage(eHMSG_BOOTUPSTATE_CHANGE, (HUINT32)pstBootStatus, 0, 0);

	return ERR_OK;
}

//	i
static HERROR	_prism_onOSKShow(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HUINT8 *nParam = NULL;

	HxLOG_Assert(nArgc == 1);

	HxLOG_Trace();

	if (s_pstMsgBuffer == NULL)
	{
		s_pstMsgBuffer = HLIB_CBUF_New(PRISM_SIGNAL_BUF_SIZE);
	}

	if(apArgv[0])
	{
		HCHAR	*pszBuffer = NULL;
		HUINT32	ulStrLen = 0;
		ulStrLen = HxSTD_StrLen(HxOBJECT_STR(apArgv[0])) + 1;
		pszBuffer = HLIB_CBUF_GetBuffer(s_pstMsgBuffer, ulStrLen);
		HLIB_STD_StrNCpySafe(pszBuffer, HxOBJECT_STR(apArgv[0]), ulStrLen);
		nParam = (HUINT8*)pszBuffer;
	}

	_prism_postMessage(eHMSG_OSK_SHOW, (HINT32)nParam, 0, 0);

	return ERR_OK;
}

static HERROR	_prism_onOSKDestroy(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 1);

	HxLOG_Trace();

	_prism_postMessage(eHMSG_OSK_DESTROY, 0, 0, 0);

	return ERR_OK;
}


//	i
static HERROR	_prism_onOSKModeChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 1);

	HxLOG_Trace();
	_prism_postMessage(eHMSG_OSK_MODECHANGE, HxOBJECT_INT(apArgv[0]), 0, 0);

	return ERR_OK;
}

//	i
static HERROR	_prism_onShowSimpleKeyboard(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 1);

	HxLOG_Trace();
	_prism_postMessage(eHMSG_OSK_SHOW_SIMPLE, HxOBJECT_INT(apArgv[0]), 0, 0);

	return ERR_OK;
}

//	s
static HERROR	_prism_onRemoteAppString(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HCHAR	*pszBuffer;

	HxLOG_Assert(nArgc == 1);

	HxLOG_Trace();
	if (s_pstMsgBuffer == NULL)
	{
		s_pstMsgBuffer = HLIB_CBUF_New(PRISM_SIGNAL_BUF_SIZE);
	}
	pszBuffer = HLIB_CBUF_GetBuffer(s_pstMsgBuffer, HxSTD_StrLen(HxOBJECT_STR(apArgv[0])) + 1);
	HLIB_STD_StrNCpySafe(pszBuffer, HxOBJECT_STR(apArgv[0]), HxSTD_StrLen(HxOBJECT_STR(apArgv[0])) + 1);

	_prism_postMessage(eHMSG_REMOTEAPP_STRING, (HINT32)pszBuffer, 0, 0);

	return ERR_OK;
}

//	i
static HERROR	_prism_onSystemStateChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 1);

	HxLOG_Trace();
	_prism_postMessage(eHMSG_SYSSTATE_CHANGE, HxOBJECT_INT(apArgv[0]), 0, 0);

	return ERR_OK;
}

//	i
static HERROR	_prism_onSubtitleState(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 1);

	HxLOG_Trace();
	_prism_postMessage(eHMSG_SUBTITLE_STATE, HxOBJECT_INT(apArgv[0]), 0, 0);

	return ERR_OK;
}

//	i
static HERROR	_prism_onTeletextState(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 1);

	HxLOG_Trace();
	_prism_postMessage(eHMSG_TELETEXT_STATE, HxOBJECT_INT(apArgv[0]), 0, 0);

	return ERR_OK;
}

//	i
static HERROR	_prism_onSubtitleTypeChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 1);

	HxLOG_Trace();
	_prism_postMessage(eHMSG_SUBTITLE_TYPE_CHANGE, HxOBJECT_INT(apArgv[0]), 0, 0);

	return ERR_OK;
}

//	i
static HERROR	_prism_onMainApplicationReady(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 1);

	HxLOG_Trace();
	_prism_postMessage(eHMSG_MAINAPP_READY, HxOBJECT_INT(apArgv[0]), 0, 0);

	return ERR_OK;
}

//	i
static HERROR	_prism_onSystemLanguageChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 1);

	HxLOG_Trace();
	_prism_postMessage(eHMSG_MAINLANG_CHANGED, HxOBJECT_INT(apArgv[0]), 0, 0);

	return ERR_OK;
}

static HERROR	_prism_onInputNotify(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Trace();
	_prism_postMessage(eHMSG_INPUT_NOTIFY, 0, 0, 0);

	return ERR_OK;
}

static HERROR	_prism_onPanelPriority(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 1);

	HxLOG_Trace();
	_prism_postMessage(eHMSG_PANEL_PRIORITY, HxOBJECT_INT(apArgv[0]), 0, 0);

	return ERR_OK;
}

//	i
static HERROR	_prism_onFontTypeChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 1);

	HxLOG_Trace();
	_prism_postMessage(eHMSG_SUBTITLE_FONTTYPE_CHANGE, HxOBJECT_INT(apArgv[0]), 0, 0);

	return ERR_OK;
}

static HERROR	_prism_onOtaStateChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 1);

	HxLOG_Trace();
	_prism_postMessage(eHMSG_IS_OTA_CHECK, HxOBJECT_INT(apArgv[0]), 0, 0);

	return ERR_OK;
}

static HERROR	_prism_onMsgboxShow(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Trace();
	_prism_postMessage(eHMSG_MSGBOX_SHOW, 0, 0, 0);

	return ERR_OK;
}

static HERROR	_prism_onBtStateChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Trace();
	s_bluetoothState = HxOBJECT_INT(apArgv[1]);
	_prism_postMessage(eHMSG_BLUETOOTH_MUTE, HxOBJECT_INT(apArgv[0]), HxOBJECT_INT(apArgv[1]), 0);

	return ERR_OK;
}

#if defined(CONFIG_CAS_CONAX_FINGERPRINT)
static HERROR	_prism_onFingerprintState(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HCHAR	*pszBuffer;

	HxLOG_Assert(nArgc == 1);

	HxLOG_Trace();
	if (s_pstMsgBuffer == NULL)
	{
		s_pstMsgBuffer = HLIB_CBUF_New(PRISM_SIGNAL_BUF_SIZE);
	}
	pszBuffer = HLIB_CBUF_GetBuffer(s_pstMsgBuffer, HxSTD_StrLen(HxOBJECT_STR(apArgv[0])) + 1);
	HLIB_STD_StrNCpySafe(pszBuffer, HxOBJECT_STR(apArgv[0]), HxSTD_StrLen(HxOBJECT_STR(apArgv[0])) + 1);

	_prism_postMessage(eHMSG_FINGERPRINT_STATE, (HINT32)pszBuffer, 0, 0);

	return ERR_OK;
}
#endif

#if defined(CONFIG_NETFLIX_APP)
static HERROR	_prism_onNetflixStateChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	/* Netflix launch가 되었다면 PAUSE를 제외하고 최대한 screenSaver 동작을 막아야 한다 (Netflix 요구사항) */
	NFLIX_NotifyState_t appState = (NFLIX_NotifyState_t)HxOBJECT_INT(apArgv[0]);;

	HxLOG_Trace();
	HxLOG_Print(">>Receive Netflix status[%d]\n", appState);

	switch (appState) {
	case NFLIX_NOTIFY_START_NORMAL :
		s_eNetflixAppState = appState;
		break;
	case NFLIX_NOTIFY_EXIT_NORMAL  : /* processed between webapp and netflixManager */
	case NFLIX_NOTIFY_EXIT_STANDBY :
	case NFLIX_NOTIFY_EXIT_CH_UP   :
	case NFLIX_NOTIFY_EXIT_CH_DOWN :
	case NFLIX_NOTIFY_EXIT_DIAL    :
	case NFLIX_NOTIFY_EXIT_DIAL_OTHER_LAUNCH :
		s_eNetflixAppState = appState;
		if (0) /* Netflix exit되는 경우 viewChange event에 의해서 다시 triggering되니 굳이 여기서 KillTimer() 하지 않음 */
		{
			ONDK_GWM_KillTimer(GWMPOPUP_TIMER_MAIN);
			if (ONDK_GWM_APP_Get(AP_Screensaver_Proc) != NULL)
			{
				HxLOG_Print(">>Destroy ScreenSaver netflixState:%d\n", appState);
				ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)AP_Screensaver_Proc);
			}
		}
		break;
	case NFLIX_NOTIFY_TRYTO_EXIT : /* processed between netflixManager and netflix, except webapp */
	default :
		break;
	}

	return ERR_OK;
}
#endif /* (CONFIG_NETFLIX_APP) */

int		main(HINT32 argc, HCHAR **argv)
{
	signal(SIGPIPE, SIG_IGN);

	VK_Init();
	HLIB_DBG_Init();

#if defined(CONFIG_HUMAX_CRASHREPORT)
	HCR_Init(_prism_signal_handler);
#else
	VK_SIG_SetSigAction(SIGSEGV, _prism_signal_handler);
	VK_SIG_SetSigAction(SIGILL, _prism_signal_handler);
	VK_SIG_SetSigAction(SIGFPE, _prism_signal_handler);
	VK_SIG_SetSigAction(SIGBUS, _prism_signal_handler);
	VK_SIG_SetSigAction(SIGQUIT, _prism_signal_handler);
#endif

	//	APPKIT의 초기화는 StartLogo동작 중 혹은, OBAMA Minimum Bootup Sequence를 마치고 호출 되어야 함.
	ONDK_Init(argc, argv);

	HAPI_Init();
	APK_EVENT_Init();
	ONDK_GWM_Init();
	ONDK_GWM_RECV_RegisterLoop(APK_EVENT_Dispatch);
	ONDK_STRINGS_Init();
	ONDK_GWM_Start(AP_GWMStart_Proc);

	HAPI_SetWindowStyle(ONDK_UPPER_WINDOWID, eHWS_WindowOwnControl | eHWS_WindowInputAlways, FALSE);
	HAPI_SetWindowStyle(ONDK_LOWER_WINDOWID, eHWS_WindowOwnControl, FALSE);

#if defined(CONFIG_PROD_HMX4KBBC)
	HAPI_SetWindowStyle(ONDK_UPPER_WINDOWID, eHWS_WindowInputEnable, FALSE);
#endif

	//	Buffered Signal의 동작은 Connect된 시그널의 순서로 이루어 진다.
	//	StartLogo등의 제어를 위해 AppsReady를 선 등록하고, bootup state를 등록한다.
	//	Buffered Signal의 동작은 항상 유의 해야 한다. --> 등록된 순서대로 Buffered Signal이 동작하게 된다..
	HAPI_ConnectSystemSignal("hama:onReadyProcess", "si", NULL, _prism_onApplicationReady);
	HAPI_ConnectSystemSignal("hama:onPanelPriority","i", NULL, _prism_onPanelPriority);
	HAPI_ConnectSystemSignal("hama:onInputNotify", "ii", NULL, _prism_onInputNotify);

	HAPI_ConnectSignal("signal:onBootupStateChanged", NULL, _prism_onBootupStateChanged);

	HAPI_ConnectSignal("signal:onOSKShow",            NULL, _prism_onOSKShow);
	HAPI_ConnectSignal("signal:onOSKDestroy",   	  NULL, _prism_onOSKDestroy);
	HAPI_ConnectSignal("signal:onOSKModeChanged",     NULL, _prism_onOSKModeChanged);
	HAPI_ConnectSignal("signal:onShowSimpleKeyboard", NULL, _prism_onShowSimpleKeyboard);
	HAPI_ConnectSignal("signal:onRemoteAppString",    NULL, _prism_onRemoteAppString);


	HAPI_ConnectSignal("signal:onSystemStateChanged", NULL, _prism_onSystemStateChanged);
	HAPI_ConnectSignal("signal:onSubtitleState",      NULL, _prism_onSubtitleState);
	HAPI_ConnectSignal("signal:onTeletextState",      NULL, _prism_onTeletextState);
	HAPI_ConnectSignal("signal:onSubtitleTypeChanged",NULL, _prism_onSubtitleTypeChanged);
	HAPI_ConnectSignal("signal:onMainApplicationReady", NULL, _prism_onMainApplicationReady);
	HAPI_ConnectSignal("signal:onSystemLanguageChanged", NULL, _prism_onSystemLanguageChanged);
	HAPI_ConnectSignal("signal:onFontTypeChanged", NULL, _prism_onFontTypeChanged);
	HAPI_ConnectSignal("signal:onOtaStateChanged", NULL, _prism_onOtaStateChanged);

	HAPI_ConnectSignal("signal:onMsgboxShow", NULL, _prism_onMsgboxShow);

	HAPI_ConnectSignal("signal:onBtStateChanged", NULL, _prism_onBtStateChanged);
#if defined(CONFIG_CAS_CONAX_FINGERPRINT)
	HAPI_ConnectSignal("signal:onFingerprintState", NULL, _prism_onFingerprintState);
#endif
#if defined(CONFIG_NETFLIX_APP)
	HAPI_ConnectSignal("signal:onNetflixStateChanged", NULL, _prism_onNetflixStateChanged);
#endif
#ifdef	CONFIG_DEBUG
	_prism_cmd_register();
	//	Subtitle_CMD_Register_PESEmul(COMMAND_NAME);
	Teletext_CMD_Register(COMMAND_NAME);
#endif

	HxLOG_Debug("[%s:%d] Prism Initializing DONE\n", __FUNCTION__, __LINE__);

	//	Drop Root Previleges --> if CONFIG_DROP_ROOT_PRIVILEGES is ON,
	HLIB_EXTRA_DropRootPrivileges("prism");

	while (1)
	{
		VK_TASK_Sleep(100000);
	}

	ONDK_GWM_Destroy();
	ONDK_Destroy();

	return 0;
}


/**************************************************************************************************/
#define _________PRISM_End_Of_File____________________________________________________
/**************************************************************************************************/

