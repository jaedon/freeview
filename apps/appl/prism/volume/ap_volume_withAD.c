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
#define _________AP_Volume_Private_Include_________________________________________________
/**************************************************************************************************/
#include <prism.h>

/**************************************************************************************************/
#define _________AP_Volume_Private_Macro_________________________________________________
/**************************************************************************************************/
#define AP_VOL_PARAM_WARRING_REMOVE(arg1, arg2, arg3, arg4)				\
			{															\
			(void)arg1;													\
			(void)arg2;													\
			(void)arg3;													\
			(void)arg4;							 						\
			}


#define VOLUMEWND_TIME_SECOND				1000
#define VOLUMEWND_EXPIRE_TIME				(3 * VOLUMEWND_TIME_SECOND)

/**************************************************************************************************/
#define _________AP_Volume_Private_Struct_________________________________________________
/**************************************************************************************************/

/******************************************************/
#define			DEF_USE_BACKGROUND_FRAME
/******************************************************/
#define 		VOLUME_MUTE				"/usr/image/581_VOL_Mute.png"
#define 		VOLUME_BLUETOOTH			"/usr/image/581_VOL_Bluetooth.png"
#define 		VOLUME_FRAME				"/usr/image/580_VOL_BG02.png"
#define 		VOLUME_BLANK				"/usr/image/580_VOL_Blank.png"
#define 		VOLUME_BLANK_20			"/usr/image/580_VOL_Blank_20.png"
#define 		VOLUME_FILL				"/usr/image/580_VOL_Fill.png"
#define 		VOLUME_FILL_20				"/usr/image/580_VOL_Fill_20.png"
#define 		VOLUME_AD_FILL				"/usr/image/580_VOL_AD_Fill.png"
#define 		VOLUME_AD_FILL_20			"/usr/image/580_VOL_AD_Fill_20.png"
#define 		VOLUME_VOLUME				"/usr/image/581_VOL_Volume.png"
#define 		VOLUME_VOLUME_20			"/usr/image/581_VOL_Volume_20.png"
#define 		VOLUME_AD					"/usr/image/581_VOL_AD.png"
#define 		VOLUME_AD_20				"/usr/image/581_VOL_AD_20.png"

// Frame ID
#if defined(DEF_USE_BACKGROUND_FRAME)
#define 		VOL_FRAME_ID				1
#endif
#define 		VOL_NUMTEXT_ID				2
#define 		VOL_PROGRESS_ID			3
#define 		VOL_PROGRESS_20_ID		4
#define 		VOL_MUTEICON_ID			5
#define 		VOL_BLUETOOTH_ID			6
#define 		VOL_VOLUME_ID				7
#define 		VOL_VOLUME_20_ID			8
#define 		VOL_AD_NUMTEXT_ID			9
#define 		VOL_AD_PROGRESS_ID		10
#define 		VOL_AD_PROGRESS_20_ID		11
#define 		VOL_AD_VOLUME_ID			12
#define 		VOL_AD_VOLUME_20_ID		13

#if defined(DEF_USE_BACKGROUND_FRAME)
#define			AP_VALUME_GAP_X					190
#define			AP_VALUME_GAP_Y					174
#define 		AP_VOLUME_FRAME_X				880 // 970
#define 		AP_VOLUME_FRAME_Y				0
#define			AP_VOLUME_FRAME_W				440 //310
#define			AP_VOLUME_FRAME_H				720
#else
#define		AP_VALUME_GAP_X					30//0
#define		AP_VALUME_GAP_Y					0
#define 		AP_VOLUME_FRAME_X				840
#define 		AP_VOLUME_FRAME_Y				0
#define		AP_VOLUME_FRAME_W			440
#define		AP_VOLUME_FRAME_H				720
#endif /* DEF_USE_BACKGROUND_FRAME */

// volume image
#define 	AP_VOLUME_IMAGE_X			327
#define 	AP_VOLUME_IMAGE_Y			136
#define	AP_VOLUME_IMAGE_W		33
#define	AP_VOLUME_IMAGE_H			31

// volume control bar
#define 	AP_VOLUME_PROGRESS_X			327
#define 	AP_VOLUME_PROGRESS_Y			174
#define	AP_VOLUME_PROGRESS_W			33
#define	AP_VOLUME_PROGRESS_H			312

// volume number text
#define 	AP_VOLUME_NUMTEXT_X				321
#define 	AP_VOLUME_NUMTEXT_Y				529//505
#define	AP_VOLUME_NUMTEXT_W				47
#define	AP_VOLUME_NUMTEXT_H				42

//Gernal volume & AD volume GAP
#define 	AP_VALUME_BAR_GAP_X				63

//Gernal Volume Num & AD volume Num GAP
#define 	AP_VALUME_NUM_GAP_X			64 //71

// Mute Icon
#define 		AP_VOLUME_MUTEICON_X			300
#define 		AP_VOLUME_MUTEICON_Y			53
#define		AP_VOLUME_MUTEICON_W			70
#define		AP_VOLUME_MUTEICON_H			70

#define 		AP_VOLUME_BLUETOOTH_X			(AP_VALUME_GAP_X+1)
#define 		AP_VOLUME_BLUETOOTH_Y			(AP_VALUME_GAP_Y+327)
#define			AP_VOLUME_BLUETOOTH_W			45
#define			AP_VOLUME_BLUETOOTH_H			58

#define		AP_VOLUME_VSPACE_HEIGHT			8

typedef struct _Vol_Contents
{
	HINT32		appvolume;
	HINT32          	octovolume;
	HINT32          	volumeMax;
	HINT32          	volumeMin;
	HBOOL           	isMute;
	HBOOL           	isBluetooth;
	HBOOL		isShow;
	HCHAR		szNumText[64];
	HCHAR		szMuteText[64];

	HBOOL		bInitalized;
	HINT32		nAppVolumeAD;
	HINT32          	nOctoVolumeAD;
	HBOOL		bExistAD;
	HBOOL		bSelectAD;
	HCHAR		szVolumeNumTextAD[64];
} Vol_Contents_t;


/**************************************************************************************************/
/*hms-1000s volume */
#define VOLUME_STEP 20
#define VOLUME_MAX VOLUME_STEP
#define VOLUME_MIN 0

/**************************************************************************************************/


/**************************************************************************************************/
#define _________AP_Volume_Private_Value_________________________________________________
/**************************************************************************************************/

static Vol_Contents_t			s_Volume_Contents;


/**************************************************************************************************/
#define _________AP_Volume_Private_Prototype_________________________________________________
/**************************************************************************************************/
//init
static Vol_Contents_t* local_volume_ad_VolumeGetContents (void );
//update

//process
static ONDK_Result_t		local_volume_ad_Initialize(Vol_Contents_t *pstContents);
//destroy

//ui

//msg run
static ONDK_Result_t 	local_volume_ad_MsgGwmDestroy(Vol_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	local_volume_ad_MsgGwmCreate(Vol_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	local_volume_ad_MsgGwmKeyDown(Vol_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

/**************************************************************************************************/
#define _________AP_Volume_Private_Init__________________________________________________________
/**************************************************************************************************/
static Vol_Contents_t* local_volume_ad_VolumeGetContents(void)
{
	return &s_Volume_Contents;
}

/**************************************************************************************************/
#define _________AP_Volume_Private_Update__________________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define _________AP_Volume_Private_Process__________________________________________________________
/**************************************************************************************************/

static HBOOL	local_volume_ad_IsExistAudioDescription(void)
{
#define	USERDB_AUDIO_AUDIO_DESCRIPTION				"UDB/AUDIO_AudioDescription" // bpldb.h ...

	HUINT32 ulViewId;
	HINT32	nCompIndex = -1, nCompSubIndex = -1;
	HERROR	hError = ERR_OK;
	HBOOL	bExistAD = FALSE;
	OxMediaPlay_MediaType_e eMediaType = eOxMP_MEDIATYPE_NONE;

	APK_DB_GetInt(USERDB_AUDIO_AUDIO_DESCRIPTION, (HINT32*)&bExistAD);

	if(bExistAD == TRUE)
	{
		APK_MEDIA_PLAY_GetMainViewId(&ulViewId);

		hError = APK_MEDIA_PLAY_GetComponentIndex(ulViewId, 2 /*eOxMP_COMPONENT_AUDIO*/, &nCompIndex, &nCompSubIndex);
		if(hError == ERR_OK)
		{
			hError = APK_MEDIA_PLAY_GetPlayType(ulViewId, &eMediaType);
			if(hError == ERR_OK)
			{
				if(nCompSubIndex != -1 && eOxMP_MEDIATYPE_MEDIAPLAY != eMediaType && eOxMP_MEDIATYPE_NONE !=  eMediaType)
					bExistAD= TRUE;
			}
		}
	}
	HxLOG_Print("#bExistAD (%d) nCompIndex(%d) nCompSubIndex(%d) eMediaType(%d) \n", bExistAD, nCompIndex, nCompSubIndex, eMediaType );

	return bExistAD;
}

static ONDK_Result_t		local_volume_ad_Initialize(Vol_Contents_t *pstContents)
{
	ONDK_Assert(pstContents);

	APK_OUTPUT_AUDIO_GetVolume(&pstContents->octovolume);
	APK_OUTPUT_AUDIO_GetVolumeMax(&pstContents->volumeMax);

	pstContents->volumeMin = 0;

	APK_OUTPUT_AUDIO_GetMasterMute(&pstContents->isMute);

	pstContents->appvolume = (pstContents->octovolume * VOLUME_STEP) / (pstContents->volumeMax - pstContents->volumeMin);
	HxSTD_sprintf(pstContents->szNumText, "%d",pstContents->appvolume);

	ONDK_Print("Main Volume : appvolume(%d) , octovolume(%d) isMute(%d) \n", pstContents->appvolume, pstContents->octovolume, pstContents->isMute );

	pstContents->bExistAD = local_volume_ad_IsExistAudioDescription();
	pstContents->bSelectAD = FALSE;

	if(pstContents->bExistAD == TRUE)
	{
		APK_OUTPUT_AUDIO_GetDecorderVolume(&pstContents->nOctoVolumeAD);

		pstContents->nAppVolumeAD = ((pstContents->nOctoVolumeAD * VOLUME_STEP) / (pstContents->volumeMax - pstContents->volumeMin));
		HxSTD_sprintf(pstContents->szVolumeNumTextAD, "%d",pstContents->nAppVolumeAD);

		ONDK_Print("Audio Description Volume : nAppVolumeAD(%d) , nOctoVolumeAD(%d)  \n", pstContents->nAppVolumeAD, pstContents->nOctoVolumeAD );
	}

	pstContents->bInitalized = TRUE;

	return GWM_RET_OK;
}


static void local_volume_ad_setHideUI(HBOOL bHide)
{
	Vol_Contents_t *pstContents = local_volume_ad_VolumeGetContents();

	pstContents->isShow = !bHide;

	if(bHide == TRUE)
	{
		#if defined(DEF_USE_BACKGROUND_FRAME)
		ONDK_GWM_Obj_SetHide(VOL_FRAME_ID, TRUE);
		#endif
		ONDK_GWM_Obj_SetHide(VOL_NUMTEXT_ID, TRUE);
		ONDK_GWM_Obj_SetHide(VOL_PROGRESS_ID, TRUE);
		ONDK_GWM_Obj_SetHide(VOL_MUTEICON_ID, TRUE);
		ONDK_GWM_Obj_SetHide(VOL_BLUETOOTH_ID, TRUE);
		ONDK_GWM_Obj_SetHide(VOL_VOLUME_ID, TRUE);

		ONDK_GWM_Obj_SetHide(VOL_PROGRESS_20_ID, TRUE);
		ONDK_GWM_Obj_SetHide(VOL_VOLUME_20_ID, TRUE);

		APK_OUTPUT_PANEL_SetPanelAuto(eApkPANEL_PRIORITY_MIDDLE);
	}
	else
	{
		if(pstContents->isBluetooth == TRUE)
		{
			ONDK_GWM_Obj_SetVisible(VOL_BLUETOOTH_ID, pstContents->isBluetooth);
			ONDK_GWM_Obj_SetHide(VOL_MUTEICON_ID, pstContents->isBluetooth);
			ONDK_GWM_Obj_SetHide(VOL_NUMTEXT_ID, pstContents->isBluetooth);
			#if defined(DEF_USE_BACKGROUND_FRAME)
			ONDK_GWM_Obj_SetHide(VOL_FRAME_ID, FALSE);
			#endif

			if(pstContents->bSelectAD == TRUE)
			{
				// original image
				ONDK_GWM_Obj_SetHide(VOL_PROGRESS_ID, TRUE);
				ONDK_GWM_Obj_SetHide(VOL_VOLUME_ID, TRUE);

				// opacity 20% image
				ONDK_GWM_Obj_SetHide(VOL_PROGRESS_20_ID, FALSE);
				ONDK_GWM_Obj_SetHide(VOL_VOLUME_20_ID, FALSE);

				// ad volume value op
				ONDK_GWM_COM_Text_ChangeColor(VOL_NUMTEXT_ID, 0x334ebeff, 0x334ebeff);
			}
			else
			{
				// original image
				ONDK_GWM_Obj_SetHide(VOL_PROGRESS_ID, FALSE);
				ONDK_GWM_Obj_SetHide(VOL_VOLUME_ID, FALSE);

				// opacity 20% image
				ONDK_GWM_Obj_SetHide(VOL_PROGRESS_20_ID, TRUE);
				ONDK_GWM_Obj_SetHide(VOL_VOLUME_20_ID, TRUE);

				ONDK_GWM_COM_Text_ChangeColor(VOL_NUMTEXT_ID, 0xff4ebeff, 0xff4ebeff);
			}
		}
		else
		{
			ONDK_GWM_Obj_SetVisible(VOL_BLUETOOTH_ID, pstContents->isMute);
			ONDK_GWM_Obj_SetHide(VOL_NUMTEXT_ID, pstContents->isMute);
			ONDK_GWM_Obj_SetVisible(VOL_MUTEICON_ID, pstContents->isMute);
#if defined(DEF_USE_BACKGROUND_FRAME)
			ONDK_GWM_Obj_SetHide(VOL_FRAME_ID, FALSE);
#endif
			if(pstContents->bSelectAD == TRUE)
			{
				// original image
				ONDK_GWM_Obj_SetHide(VOL_PROGRESS_ID,  TRUE);
				ONDK_GWM_Obj_SetHide(VOL_VOLUME_ID, TRUE);

				// opacity 20% image
				ONDK_GWM_Obj_SetHide(VOL_PROGRESS_20_ID, pstContents->isMute);
				ONDK_GWM_Obj_SetHide(VOL_VOLUME_20_ID, pstContents->isMute);

				ONDK_GWM_COM_Text_ChangeColor(VOL_NUMTEXT_ID, 0x334ebeff, 0x334ebeff);
			}
			else
			{
				// original image
				ONDK_GWM_Obj_SetHide(VOL_PROGRESS_ID, pstContents->isMute);
				ONDK_GWM_Obj_SetHide(VOL_VOLUME_ID, pstContents->isMute);

				// opacity 20% image
				ONDK_GWM_Obj_SetHide(VOL_PROGRESS_20_ID, TRUE);
				ONDK_GWM_Obj_SetHide(VOL_VOLUME_20_ID, TRUE);

				ONDK_GWM_COM_Text_ChangeColor(VOL_NUMTEXT_ID, 0xff4ebeff, 0xff4ebeff);
			}
		}
	}
	ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());

}

static void local_volume_ad_setHideUIwithAD(HBOOL bHide)
{
	Vol_Contents_t *pstContents = local_volume_ad_VolumeGetContents();

	pstContents->isShow = !bHide;

	if(pstContents->bExistAD == FALSE)
	{
		// original image
		ONDK_GWM_Obj_SetHide(VOL_AD_VOLUME_ID, TRUE);
		ONDK_GWM_Obj_SetHide(VOL_AD_PROGRESS_ID, TRUE);

		// opacity 20% image
		ONDK_GWM_Obj_SetHide(VOL_AD_VOLUME_20_ID, TRUE);
		ONDK_GWM_Obj_SetHide(VOL_AD_PROGRESS_20_ID, TRUE);

		ONDK_GWM_Obj_SetHide(VOL_AD_NUMTEXT_ID, TRUE);

		return;
	}

	if(bHide == TRUE)
	{
		if(pstContents->bExistAD == TRUE)
		{
			ONDK_GWM_Obj_SetHide(VOL_AD_VOLUME_ID, TRUE);
			ONDK_GWM_Obj_SetHide(VOL_AD_NUMTEXT_ID, TRUE);
			ONDK_GWM_Obj_SetHide(VOL_AD_PROGRESS_ID, TRUE);

			ONDK_GWM_Obj_SetHide(VOL_AD_VOLUME_20_ID, TRUE);
			ONDK_GWM_Obj_SetHide(VOL_AD_PROGRESS_20_ID, TRUE);

			pstContents->bSelectAD = FALSE;
		}
	}
	else
	{
		if(pstContents->isBluetooth == TRUE)
		{
			if(pstContents->bSelectAD == TRUE)
			{
				// original image
				ONDK_GWM_Obj_SetHide(VOL_AD_VOLUME_ID, FALSE);
				ONDK_GWM_Obj_SetHide(VOL_AD_PROGRESS_ID, FALSE);

				// opacity 20% image
				ONDK_GWM_Obj_SetHide(VOL_AD_VOLUME_20_ID, TRUE);
				ONDK_GWM_Obj_SetHide(VOL_AD_PROGRESS_20_ID, TRUE);

				// ad volume value op
				ONDK_GWM_COM_Text_ChangeColor(VOL_AD_NUMTEXT_ID, 0xffffffff, 0xffffffff);
			}
			else
			{
				// original image
				ONDK_GWM_Obj_SetHide(VOL_AD_VOLUME_ID, TRUE);
				ONDK_GWM_Obj_SetHide(VOL_AD_PROGRESS_ID, TRUE);

				// opacity 20% image
				ONDK_GWM_Obj_SetHide(VOL_AD_VOLUME_20_ID, FALSE);
				ONDK_GWM_Obj_SetHide(VOL_AD_PROGRESS_20_ID, FALSE);

				// ad volume value op
				ONDK_GWM_COM_Text_ChangeColor(VOL_AD_NUMTEXT_ID, 0x33ffffff, 0x33ffffff);
			}
			ONDK_GWM_Obj_SetHide(VOL_AD_NUMTEXT_ID, pstContents->isBluetooth);
		}
		else
		{
			if(pstContents->bSelectAD == TRUE)
			{
				// original image
				ONDK_GWM_Obj_SetHide(VOL_AD_VOLUME_ID, pstContents->isMute);
				ONDK_GWM_Obj_SetHide(VOL_AD_PROGRESS_ID, pstContents->isMute);

				// opacity 20% image
				ONDK_GWM_Obj_SetHide(VOL_AD_VOLUME_20_ID, TRUE);
				ONDK_GWM_Obj_SetHide(VOL_AD_PROGRESS_20_ID, TRUE);

				// ad volume value op
				ONDK_GWM_COM_Text_ChangeColor(VOL_AD_NUMTEXT_ID, 0xffffffff, 0xffffffff);
			}
			else
			{
				// original image
				ONDK_GWM_Obj_SetHide(VOL_AD_VOLUME_ID, TRUE);
				ONDK_GWM_Obj_SetHide(VOL_AD_PROGRESS_ID, TRUE);

				// opacity 20% image
				ONDK_GWM_Obj_SetHide(VOL_AD_VOLUME_20_ID, pstContents->isMute);
				ONDK_GWM_Obj_SetHide(VOL_AD_PROGRESS_20_ID, pstContents->isMute);

				// ad volume value op
				ONDK_GWM_COM_Text_ChangeColor(VOL_AD_NUMTEXT_ID, 0x33ffffff, 0x33ffffff);
			}

			ONDK_GWM_Obj_SetHide(VOL_AD_NUMTEXT_ID, pstContents->isMute);
		}
	}
	ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
}


static void local_volume_ad_Setting(Vol_Contents_t *pstContents, HUINT32 	key)
{
	HBOOL	drawPanelVolText = FALSE;
	HUINT8	*str = NULL;

	switch(key)
	{
	case KEY_VOLUMEUP:
		if(pstContents->isBluetooth== TRUE)
		{
			ONDK_Print("KEY_VOLUMEUP : Connect Bluetooth \n");
		}
		else
		{
			if(pstContents->isMute == TRUE)
			{
				APK_OUTPUT_AUDIO_SetMasterMute(FALSE);
				APK_OUTPUT_AUDIO_GetMasterMute(&pstContents->isMute);
			}
			if(pstContents->bSelectAD == TRUE)
			{
				if(VOLUME_MAX > pstContents->nAppVolumeAD)
				{
					++pstContents->nAppVolumeAD;

					pstContents->nOctoVolumeAD = (pstContents->volumeMax - pstContents->volumeMin)/VOLUME_STEP * pstContents->nAppVolumeAD;

					ONDK_Print("volume-octo-AD [%d] volume-app-AD[%d]\n",pstContents->nOctoVolumeAD,pstContents->nAppVolumeAD);

					APK_OUTPUT_AUDIO_SetDecoderVolume(pstContents->nOctoVolumeAD);
				}
			}
			else
			{
				if(VOLUME_MAX > pstContents->appvolume)
				{
					++pstContents->appvolume;

					pstContents->octovolume = (pstContents->volumeMax - pstContents->volumeMin)/VOLUME_STEP * pstContents->appvolume;

					ONDK_Print("volume-octo [%d] volume-app[%d]\n",pstContents->octovolume,pstContents->appvolume);

					APK_OUTPUT_AUDIO_SetVolume(pstContents->octovolume);
				}
			}

			drawPanelVolText = TRUE;
		}

		break;

	case KEY_VOLUMEDOWN:
		if(pstContents->isBluetooth== TRUE)
		{
			ONDK_Print("KEY_VOLUMEDOWN : Connect Bluetooth \n");
		}
		else
		{
			if(pstContents->isMute == TRUE)
			{
				APK_OUTPUT_AUDIO_SetMasterMute(FALSE);
				APK_OUTPUT_AUDIO_GetMasterMute(&pstContents->isMute);
			}
			if(pstContents->bSelectAD == TRUE)
			{
				if(VOLUME_MIN < pstContents->nAppVolumeAD)
				{
					--pstContents->nAppVolumeAD;

					pstContents->nOctoVolumeAD = (pstContents->volumeMax - pstContents->volumeMin)/VOLUME_STEP * pstContents->nAppVolumeAD;
					ONDK_Print("volume-octo-AD [%d] volume-app-AD[%d]\n",pstContents->nOctoVolumeAD,pstContents->nAppVolumeAD);
					APK_OUTPUT_AUDIO_SetDecoderVolume(pstContents->nOctoVolumeAD);
				}
			}
			else
			{
				if(VOLUME_MIN < pstContents->appvolume)
				{
					--pstContents->appvolume;

					pstContents->octovolume = (pstContents->volumeMax - pstContents->volumeMin)/VOLUME_STEP * pstContents->appvolume;
					ONDK_Print("volume-octo [%d] volume-app[%d]\n",pstContents->octovolume,pstContents->appvolume);
					APK_OUTPUT_AUDIO_SetVolume(pstContents->octovolume);
				}
			}

			drawPanelVolText = TRUE;
		}

		break;

	case KEY_MUTE:
		if(pstContents->isBluetooth== TRUE)
		{
			ONDK_Print("KEY_MUTE : Connect Bluetooth \n");
		}
		else
		{
			APK_OUTPUT_AUDIO_GetMasterMute(&pstContents->isMute);
			pstContents->isMute = !pstContents->isMute;
			APK_OUTPUT_AUDIO_SetMasterMute(pstContents->isMute);
			ONDK_Print("[%s:%d] Mute (%d)\n", __FUNCTION__, __LINE__, pstContents->isMute);
			if(pstContents->isMute == TRUE)
			{
				str = ONDK_GetString("STR_MUTE_ID");
				if(str == NULL) str = "# Mute #";

				HxSTD_sprintf(pstContents->szMuteText, "%s", str);
				APK_OUTPUT_PANEL_SetPanel(eApkPANEL_PRIORITY_MIDDLE, str, eDxPANEL_ALIGN_CENTER);
				ONDK_GWM_KillTimer(GWMPOPUP_TIMER_VOLID);
			}
			else
			{
				drawPanelVolText = TRUE;
				APK_OUTPUT_PANEL_SetPanelAuto(eApkPANEL_PRIORITY_MIDDLE);
				ONDK_GWM_SetTimer(GWMPOPUP_TIMER_VOLID, VOLUMEWND_EXPIRE_TIME);
			}
		}
		break;
	default:
		return;
 	}


	if(drawPanelVolText == TRUE)
	{
		HCHAR szPanelStr[32] = {0, };

		str = ONDK_GetString("STR_VOLUME_ID");
		if(str == NULL) str = "## VOLUME ##";

		HxSTD_sprintf(szPanelStr, "VOLUME %d", pstContents->appvolume); // ux에서 VOLUME으로 나오게...
		APK_OUTPUT_PANEL_SetPanel(eApkPANEL_PRIORITY_MIDDLE, szPanelStr, eDxPANEL_ALIGN_CENTER);

		HxSTD_sprintf(pstContents->szNumText, "%d", pstContents->appvolume);
		ONDK_GWM_COM_VProgress_SetPos(VOL_PROGRESS_ID, pstContents->appvolume);
		ONDK_GWM_COM_VProgress_SetPos(VOL_PROGRESS_20_ID, pstContents->appvolume);

		HxSTD_sprintf(pstContents->szVolumeNumTextAD, "%d", pstContents->nAppVolumeAD);
		ONDK_GWM_COM_VProgress_SetPos(VOL_AD_PROGRESS_ID, pstContents->nAppVolumeAD);
		ONDK_GWM_COM_VProgress_SetPos(VOL_AD_PROGRESS_20_ID, pstContents->nAppVolumeAD);

		ONDK_GWM_ResetTimer(GWMPOPUP_TIMER_VOLID);
	}
	else
	{
		ONDK_GWM_COM_VProgress_SetPos(VOL_PROGRESS_ID, 0);
		ONDK_GWM_COM_VProgress_SetPos(VOL_PROGRESS_20_ID, 0);
		ONDK_GWM_COM_VProgress_SetPos(VOL_AD_PROGRESS_ID, 0);
		ONDK_GWM_COM_VProgress_SetPos(VOL_AD_PROGRESS_20_ID, 0);
	}

	local_volume_ad_setHideUI(FALSE);
	local_volume_ad_setHideUIwithAD(FALSE);
}


/**************************************************************************************************/
#define _________AP_Volume_Private_Destroy__________________________________________________________
/**************************************************************************************************/
static ONDK_Result_t local_volume_ad_DestroyVolume(Vol_Contents_t *pstContents)
{
	return GWM_RET_OK;
}

/**************************************************************************************************/
#define _________AP_Volume_Private_Proc_UI__________________________________________________________
/**************************************************************************************************/


static ONDK_Result_t 	local_volume_ad_SetUiObject(Vol_Contents_t *pstContents)
{
	ONDK_Rect_t 		stRect;
	ONDK_Size_t 		fillSize = {0, 0}, blankSize = {0, 0};
	HINT32			vprogressHeight = 0;

	ONDK_Assert(pstContents);

	// Backgroude Image
	ONDK_GWM_APP_SetAppArea(AP_VOLUME_FRAME_X, AP_VOLUME_FRAME_Y, AP_VOLUME_FRAME_W, AP_VOLUME_FRAME_H);

#if defined(DEF_USE_BACKGROUND_FRAME)
	stRect = ONDK_Rect(0,0,AP_VOLUME_FRAME_W,AP_VOLUME_FRAME_H);
	ONDK_GWM_COM_Image_Create(VOL_FRAME_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(VOL_FRAME_ID, VOLUME_FRAME);
#endif

	fillSize = ONDK_IMG_GetSize(ONDK_IMG_CreateSelf(VOLUME_FILL));
	blankSize = ONDK_IMG_GetSize(ONDK_IMG_CreateSelf(VOLUME_BLANK));
	vprogressHeight = (((fillSize.h >= blankSize.h) ? fillSize.h : blankSize.h) + AP_VOLUME_VSPACE_HEIGHT) * VOLUME_MAX;

	//General Volume Icon
	stRect = ONDK_Rect(AP_VOLUME_IMAGE_X,AP_VOLUME_IMAGE_Y,AP_VOLUME_IMAGE_W,AP_VOLUME_IMAGE_H);
	ONDK_GWM_COM_Image_Create(VOL_VOLUME_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(VOL_VOLUME_ID, VOLUME_VOLUME);

	//General Volume Icon (Opacity 20%)
	stRect = ONDK_Rect(AP_VOLUME_IMAGE_X,AP_VOLUME_IMAGE_Y,AP_VOLUME_IMAGE_W,AP_VOLUME_IMAGE_H);
	ONDK_GWM_COM_Image_Create(VOL_VOLUME_20_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(VOL_VOLUME_20_ID, VOLUME_VOLUME_20);

	// General Volume Bar
	stRect = ONDK_Rect(AP_VOLUME_PROGRESS_X,AP_VOLUME_PROGRESS_Y,AP_VOLUME_PROGRESS_W,vprogressHeight);
	//stRect = ONDK_Rect(AP_VOLUME_PROGRESS_X,AP_VOLUME_PROGRESS_Y,AP_VOLUME_PROGRESS_W,AP_VOLUME_PROGRESS_H);
	ONDK_GWM_COM_VProgress_Create(VOL_PROGRESS_ID, &stRect, 0, 0, 0);
	ONDK_GWM_COM_VProgress_SetBlankImg(VOL_PROGRESS_ID, VOLUME_BLANK);
	ONDK_GWM_COM_VProgress_SetFillImg(VOL_PROGRESS_ID, VOLUME_FILL);
	ONDK_GWM_COM_VProgress_SetVspace(VOL_PROGRESS_ID, AP_VOLUME_VSPACE_HEIGHT);
	ONDK_GWM_COM_VProgress_SetMax(VOL_PROGRESS_ID, VOLUME_MAX);
	ONDK_GWM_COM_VProgress_SetMin(VOL_PROGRESS_ID, VOLUME_MIN);
	ONDK_GWM_COM_VProgress_SetPos(VOL_PROGRESS_ID, pstContents->appvolume);

	// General Volume Bar(Opacity 20%)
	stRect = ONDK_Rect(AP_VOLUME_PROGRESS_X,AP_VOLUME_PROGRESS_Y,AP_VOLUME_PROGRESS_W,vprogressHeight);
	//stRect = ONDK_Rect(AP_VOLUME_PROGRESS_X,AP_VOLUME_PROGRESS_Y,AP_VOLUME_PROGRESS_W,AP_VOLUME_PROGRESS_H);
	ONDK_GWM_COM_VProgress_Create(VOL_PROGRESS_20_ID, &stRect, 0, 0, 0);
	ONDK_GWM_COM_VProgress_SetBlankImg(VOL_PROGRESS_20_ID, VOLUME_BLANK_20);
	ONDK_GWM_COM_VProgress_SetFillImg(VOL_PROGRESS_20_ID, VOLUME_FILL_20);
	ONDK_GWM_COM_VProgress_SetVspace(VOL_PROGRESS_20_ID, AP_VOLUME_VSPACE_HEIGHT);
	ONDK_GWM_COM_VProgress_SetMax(VOL_PROGRESS_20_ID, VOLUME_MAX);
	ONDK_GWM_COM_VProgress_SetMin(VOL_PROGRESS_20_ID, VOLUME_MIN);
	ONDK_GWM_COM_VProgress_SetPos(VOL_PROGRESS_20_ID, pstContents->appvolume);

/*
	// General Volume Bar
	stRect = ONDK_Rect(AP_VOLUME_PROGRESS_X,AP_VOLUME_PROGRESS_Y,AP_VOLUME_PROGRESS_W,AP_VOLUME_PROGRESS_H);
	ONDK_GWM_COM_VProgress_Create(VOL_PROGRESS_ID, &stRect, 0, 0, 0);
	ONDK_GWM_COM_VProgress_SetBlankImg(VOL_PROGRESS_ID, VOLUME_BLANK);
	ONDK_GWM_COM_VProgress_SetFillImg(VOL_PROGRESS_ID, VOLUME_FILL);
	ONDK_GWM_COM_VProgress_SetVspace(VOL_PROGRESS_ID, 8);
	ONDK_GWM_COM_VProgress_SetMax(VOL_PROGRESS_ID, VOLUME_MAX);
	ONDK_GWM_COM_VProgress_SetMin(VOL_PROGRESS_ID, VOLUME_MIN);
	ONDK_GWM_COM_VProgress_SetPos(VOL_PROGRESS_ID, pstContents->appvolume);
*/
	// General Volume Nmm Text
	stRect = ONDK_Rect(AP_VOLUME_NUMTEXT_X,AP_VOLUME_PROGRESS_Y+vprogressHeight-7,AP_VOLUME_NUMTEXT_W,AP_VOLUME_NUMTEXT_H);
	ONDK_GWM_COM_Text_Create(VOL_NUMTEXT_ID, &stRect, pstContents->szNumText);
	ONDK_GWM_COM_Text_SetSize(VOL_NUMTEXT_ID, 42);
	ONDK_GWM_COM_Text_SetAlign(VOL_NUMTEXT_ID, TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_SetFont(VOL_NUMTEXT_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_ChangeColor(VOL_NUMTEXT_ID, 0xff4ebeff, 0xff4ebeff);

	// AD Volume Image
	stRect = ONDK_Rect(AP_VOLUME_IMAGE_X-AP_VALUME_BAR_GAP_X, AP_VOLUME_IMAGE_Y, AP_VOLUME_IMAGE_W, AP_VOLUME_IMAGE_H);
	ONDK_GWM_COM_Image_Create(VOL_AD_VOLUME_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(VOL_AD_VOLUME_ID, VOLUME_AD);

	// AD Volume Image(Opcity 20%)
	stRect = ONDK_Rect(AP_VOLUME_IMAGE_X-AP_VALUME_BAR_GAP_X, AP_VOLUME_IMAGE_Y, AP_VOLUME_IMAGE_W, AP_VOLUME_IMAGE_H);
	ONDK_GWM_COM_Image_Create(VOL_AD_VOLUME_20_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(VOL_AD_VOLUME_20_ID, VOLUME_AD_20);

	// AD Volume Bar
	stRect = ONDK_Rect(AP_VOLUME_PROGRESS_X-AP_VALUME_BAR_GAP_X, AP_VOLUME_PROGRESS_Y, AP_VOLUME_PROGRESS_W, vprogressHeight);
	//stRect = ONDK_Rect(AP_VOLUME_PROGRESS_X-AP_VALUME_BAR_GAP_X, AP_VOLUME_PROGRESS_Y, AP_VOLUME_PROGRESS_W, AP_VOLUME_PROGRESS_H);
	ONDK_GWM_COM_VProgress_Create(VOL_AD_PROGRESS_ID, &stRect, 0, 0, 0);
	ONDK_GWM_COM_VProgress_SetBlankImg(VOL_AD_PROGRESS_ID, VOLUME_BLANK);
	ONDK_GWM_COM_VProgress_SetFillImg(VOL_AD_PROGRESS_ID, VOLUME_AD_FILL);
	ONDK_GWM_COM_VProgress_SetVspace(VOL_AD_PROGRESS_ID, 8);
	ONDK_GWM_COM_VProgress_SetMax(VOL_AD_PROGRESS_ID, VOLUME_MAX);
	ONDK_GWM_COM_VProgress_SetMin(VOL_AD_PROGRESS_ID, VOLUME_MIN);
	ONDK_GWM_COM_VProgress_SetPos(VOL_AD_PROGRESS_ID, pstContents->nAppVolumeAD);

	// AD Volume Bar(Opcity 20%)
	stRect = ONDK_Rect(AP_VOLUME_PROGRESS_X-AP_VALUME_BAR_GAP_X, AP_VOLUME_PROGRESS_Y, AP_VOLUME_PROGRESS_W, vprogressHeight);
	//stRect = ONDK_Rect(AP_VOLUME_PROGRESS_X-AP_VALUME_BAR_GAP_X, AP_VOLUME_PROGRESS_Y, AP_VOLUME_PROGRESS_W, AP_VOLUME_PROGRESS_H);
	ONDK_GWM_COM_VProgress_Create(VOL_AD_PROGRESS_20_ID, &stRect, 0, 0, 0);
	ONDK_GWM_COM_VProgress_SetBlankImg(VOL_AD_PROGRESS_20_ID, VOLUME_BLANK_20);
	ONDK_GWM_COM_VProgress_SetFillImg(VOL_AD_PROGRESS_20_ID, VOLUME_AD_FILL_20);
	ONDK_GWM_COM_VProgress_SetVspace(VOL_AD_PROGRESS_20_ID, 8);
	ONDK_GWM_COM_VProgress_SetMax(VOL_AD_PROGRESS_20_ID, VOLUME_MAX);
	ONDK_GWM_COM_VProgress_SetMin(VOL_AD_PROGRESS_20_ID, VOLUME_MIN);
	ONDK_GWM_COM_VProgress_SetPos(VOL_AD_PROGRESS_20_ID, pstContents->nAppVolumeAD);

	// AD Volume Num Text
	stRect = ONDK_Rect(AP_VOLUME_NUMTEXT_X-AP_VALUME_NUM_GAP_X,AP_VOLUME_PROGRESS_Y+vprogressHeight-7,AP_VOLUME_NUMTEXT_W,AP_VOLUME_NUMTEXT_H);
	ONDK_GWM_COM_Text_Create(VOL_AD_NUMTEXT_ID, &stRect, pstContents->szVolumeNumTextAD);
	ONDK_GWM_COM_Text_SetSize(VOL_AD_NUMTEXT_ID, 42);
	ONDK_GWM_COM_Text_SetAlign(VOL_AD_NUMTEXT_ID, TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_SetFont(VOL_AD_NUMTEXT_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_ChangeColor(VOL_AD_NUMTEXT_ID, 0xffffffff, 0xffffffff);

	//General Mute Icon
	stRect = ONDK_Rect(AP_VOLUME_MUTEICON_X,AP_VOLUME_MUTEICON_Y,AP_VOLUME_MUTEICON_W,AP_VOLUME_MUTEICON_H);
	ONDK_GWM_COM_Image_Create(VOL_MUTEICON_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(VOL_MUTEICON_ID, VOLUME_MUTE);

	return GWM_RET_OK;
}

/**************************************************************************************************/
#define _________AP_Volume_Private_Proc_MSG_RUNNIG__________________________________________________________
/**************************************************************************************************/

//MSG_GWM_CREATE
static ONDK_Result_t local_volume_ad_MsgGwmCreate(Vol_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	pstContents->isBluetooth = p3;

	ONDK_Assert(pstContents);

	local_volume_ad_Initialize(pstContents);
	local_volume_ad_SetUiObject(pstContents );

	if (pstContents->isBluetooth == FALSE)
		APK_OUTPUT_AUDIO_SetMasterMute(FALSE);

	ONDK_GWM_SetTimer(GWMPOPUP_TIMER_VOLID, VOLUMEWND_EXPIRE_TIME);

	ONDK_Print("pstContents->bExistAD(%d)\n", pstContents->bExistAD);

	if(pstContents->bExistAD == TRUE)
	{
		local_volume_ad_setHideUI(FALSE);
		local_volume_ad_setHideUIwithAD(FALSE);
		if(KEY_MUTE == p1)
			local_volume_ad_Setting(pstContents, p1);
	}
	else
	{
		local_volume_ad_Setting(pstContents, p1);
	}

	return ONDK_MESSAGE_CONSUMED;
}

//MSG_GWM_KEYDOWN
static ONDK_Result_t local_volume_ad_MsgGwmKeyDown(Vol_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32 			nKeyId = p1;
	HUINT32 			ulCurrentTick;
	static HUINT32 	s_volInterval = 0;

	ONDK_Assert(pstContents);

	ulCurrentTick = VK_TIMER_GetSystemTick();
	if(  abs(s_volInterval - ulCurrentTick ) < 100 )
	{
		switch (nKeyId)
		{
		case KEY_VOLUMEUP:
		case KEY_VOLUMEDOWN:
		case KEY_MUTE:
			ONDK_Print(HxANSI_COLOR_YELLOW("[%s:%d] PASS KEY [%d]\n"), __FUNCTION__, __LINE__,nKeyId);
			return ONDK_MESSAGE_CONSUMED;
		case KEY_ARROWRIGHT:
		case KEY_ARROWLEFT:
			ONDK_Print(HxANSI_COLOR_YELLOW("[%s:%d] PASS KEY [%d]\n"), __FUNCTION__, __LINE__,nKeyId);
			if(pstContents->isMute)
				return ONDK_MESSAGE_NONCONSUMED;
			else
				return ONDK_MESSAGE_CONSUMED;
		default:
			return ONDK_MESSAGE_NONCONSUMED;
		}
	}
	else
	{
		s_volInterval =  VK_TIMER_GetSystemTick();
	}

	switch (nKeyId)
	{
	case KEY_VOLUMEUP:
	case KEY_VOLUMEDOWN:
	case KEY_MUTE:
		if(ONDK_GWM_SetTimer(GWMPOPUP_TIMER_VOLID, VOLUMEWND_EXPIRE_TIME) == GWM_RET_LINK_CONFLICT)
		{
			ONDK_GWM_ResetTimer(GWMPOPUP_TIMER_VOLID);
		}

		if((pstContents->bInitalized == TRUE) && (pstContents->isShow == FALSE))
			pstContents->bExistAD = local_volume_ad_IsExistAudioDescription();

		ONDK_Print("isShow(%d) bExistAD(%d)\n", pstContents->isShow, pstContents->bExistAD);

		if(pstContents->isShow == FALSE && pstContents->bExistAD == TRUE)
		{
			local_volume_ad_setHideUI(FALSE);
			local_volume_ad_setHideUIwithAD(FALSE);
			if(KEY_MUTE == nKeyId)
				local_volume_ad_Setting(pstContents, nKeyId);
		}
		else
		{
			local_volume_ad_Setting(pstContents, nKeyId);
		}

		return ONDK_MESSAGE_CONSUMED;

		/* keys to exit */
	case KEY_OK:
	case KEY_BACK:
	case KEY_EXIT:
		if(pstContents->isShow == TRUE && pstContents->isMute == FALSE)
		{
			local_volume_ad_setHideUI(TRUE);
			local_volume_ad_setHideUIwithAD(TRUE);
			return ONDK_MESSAGE_CONSUMED;
		}
		return ONDK_MESSAGE_PASS;

	case KEY_ARROWRIGHT:

		if(ONDK_GWM_SetTimer(GWMPOPUP_TIMER_VOLID, VOLUMEWND_EXPIRE_TIME) == GWM_RET_LINK_CONFLICT)
		{
			ONDK_GWM_ResetTimer(GWMPOPUP_TIMER_VOLID);
		}

		ONDK_Print("KEY_ARROWRIGHT:  isShow (%d) bExistAD(%d) \n" , pstContents->isShow , pstContents->bExistAD);
		if((pstContents->bExistAD == TRUE) && (pstContents->isShow == TRUE) && (pstContents->isMute != TRUE))
		{
			pstContents->bSelectAD = FALSE;
			local_volume_ad_setHideUI(FALSE);
			local_volume_ad_setHideUIwithAD(FALSE);
			return ONDK_MESSAGE_CONSUMED;
		}
		else
		{
			return ONDK_MESSAGE_PASS;
		}

	case KEY_ARROWLEFT:
		if(ONDK_GWM_SetTimer(GWMPOPUP_TIMER_VOLID, VOLUMEWND_EXPIRE_TIME) == GWM_RET_LINK_CONFLICT)
		{
			ONDK_GWM_ResetTimer(GWMPOPUP_TIMER_VOLID);
		}

		ONDK_Print(" KEY_ARROWLEFT : isShow (%d) bExistAD(%d) \n" , pstContents->isShow , pstContents->bExistAD);
		if((pstContents->bExistAD == TRUE) && (pstContents->isShow == TRUE) && (pstContents->isMute != TRUE))
		{
			pstContents->bSelectAD = TRUE;
			local_volume_ad_setHideUI(FALSE);
			local_volume_ad_setHideUIwithAD(FALSE);
			return ONDK_MESSAGE_CONSUMED;
		}
		else
		{
			return ONDK_MESSAGE_PASS;
		}

#if 0
		case KEY_BLUE:
			if (pstContents->isShow == TRUE)
			{
				if (ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)AP_MemoryInfo_Proc) == NULL)
				{
					ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"AP_MemoryInfo_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU),
											(ONDK_GWM_Callback_t)AP_MemoryInfo_Proc, HANDLE_NULL, 0, 0, 0);
				}
				else
				{
					ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)AP_MemoryInfo_Proc);
				}
				return ONDK_MESSAGE_PASS;
			}
			break;
#endif


	default:
		return ONDK_MESSAGE_PASS;
	}

	AP_VOL_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);
	return GWM_RET_OK;
}


//MSG_GWM_TIMER
static ONDK_Result_t local_volume_ad_MsgGwmTimer(Vol_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 ulTimerID = p1;

	ONDK_Assert(pstContents);

	if (ulTimerID == GWMPOPUP_TIMER_VOLID)
	{
		if(pstContents->isMute != TRUE || pstContents->isBluetooth == TRUE)
		{
			local_volume_ad_setHideUI(TRUE);
			local_volume_ad_setHideUIwithAD(TRUE);
			ONDK_GWM_KillTimer(GWMPOPUP_TIMER_VOLID);
		}
		else
			APK_OUTPUT_PANEL_SetPanelAuto(eApkPANEL_PRIORITY_MIDDLE);

		return GWM_MESSAGE_BREAK;
	}

	AP_VOL_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);
	return GWM_RET_OK;
}

//MSG_GWM_DESTROY
static ONDK_Result_t local_volume_ad_MsgGwmDestroy(Vol_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Assert(pstContents);

	AP_VOL_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);
	APK_OUTPUT_PANEL_SetPanelAuto(eApkPANEL_PRIORITY_MIDDLE);

	return local_volume_ad_DestroyVolume(pstContents);
}

// MSG_GWM_HAPI_MESSAGE
static ONDK_Result_t local_volume_ad_MsgGwmHapiMsg(Vol_Contents_t *pstContents, HAPIMSG_e eMsg, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		ret = GWM_MESSAGE_PASS;

	switch (eMsg)
	{
		case eHMSG_BLUETOOTH_MUTE:
			{
				HINT32			isBTState = p2;

				if(isBTState == TRUE)
				{
					pstContents->isBluetooth = TRUE;
				}
				else if(isBTState == FALSE)
				{
					pstContents->isBluetooth = FALSE;
				}
				ret = GWM_MESSAGE_BREAK;
			}
			break;

		default:
			ret = GWM_MESSAGE_PASS;
			break;
	}

	return ret;
}

static ONDK_Result_t local_volume_ad_BlueToothMsg(Vol_Contents_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			ret = GWM_MESSAGE_PASS;
	HUINT32			eEvent = (HUINT32)handle;
	HINT32		isBTState = p2;

	switch (eEvent)
	{
		case eNETWORK_STATUS_BT_AUDIO_MUTE:
			if(isBTState == TRUE)
			{
				pstContents->isBluetooth = TRUE;
			}
			else if(isBTState == FALSE)
			{
				pstContents->isBluetooth = FALSE;
			}
			ret = GWM_MESSAGE_BREAK;
			break;
		default:
			ret = GWM_MESSAGE_BREAK;
	}

	return ret;
}


/**************************************************************************************************/
#define _________AP_Volume_Public_Proc____________________________________________________
/**************************************************************************************************/


ONDK_Result_t AP_AudioVolume_Proc(HINT32 nMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult	= GWM_RET_OK;
	static ONDK_Result_t 		s_upkey =ONDK_RET_OK;
	Vol_Contents_t 			*pstContents = local_volume_ad_VolumeGetContents();

	ONDK_Print(HxANSI_COLOR_YELLOW("Message(%d) : %s  p1(0x%x) p2(0x%x) p3(0x%x) \n"),  nMessage, ONDK_GWM_GwmMsg2String(nMessage), p1, p2, p3);

	switch (nMessage)
	{
	case MSG_GWM_CREATE:
		eResult = local_volume_ad_MsgGwmCreate(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_KEYUP:
		eResult = s_upkey;
		break;
	case MSG_GWM_KEYDOWN:
		eResult = local_volume_ad_MsgGwmKeyDown(pstContents, p1, p2, p3);
		s_upkey = eResult;
		break;
	case MSG_GWM_TIMER:
		eResult = local_volume_ad_MsgGwmTimer(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_DESTROY:
		eResult = local_volume_ad_MsgGwmDestroy(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_APK_BLUETOOTH:
		eResult = local_volume_ad_BlueToothMsg(pstContents,handle, p1, p2, p3);
		break;
	case MSG_GWM_HAPI_MESSAGE:
		eResult = local_volume_ad_MsgGwmHapiMsg(pstContents, (HAPIMSG_e)handle, p1, p2, p3);
		break;
	default:
		break;

	}
	if(eResult != GWM_RET_OK)
	{
		return eResult;
	}
	return ONDK_GWM_ProcessMessageDefault(nMessage, handle, p1, p2, p3);
}

/**************************************************************************************************/
#define _________AP_Volume_Public_Functions__________________________________________________________
/**************************************************************************************************/

HBOOL AP_AuidoVolume_GetHideState(void)
{
	Vol_Contents_t 			*pstContents = local_volume_ad_VolumeGetContents();
	ONDK_Assert(pstContents);
	return !pstContents->isShow;
}

/**************************************************************************************************/
#define _________AP_Volume_End_Of_File____________________________________________________
/**************************************************************************************************/

