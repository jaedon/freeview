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
#if defined(CONFIG_OP_TELENOR)
#define VOLUMEWND_EXPIRE_TIME				(5 * VOLUMEWND_TIME_SECOND)
#else
#define VOLUMEWND_EXPIRE_TIME				(3 * VOLUMEWND_TIME_SECOND)
#endif

/**************************************************************************************************/
#define _________AP_Volume_Private_Struct_________________________________________________
/**************************************************************************************************/

/******************************************************/
//#define			DEF_USE_BACKGROUND_FRAME
/******************************************************/


#define 		VOL_FRAME_ID				1
#define 		VOL_NUMTEXT_ID				2
#define 		VOL_PROGRESS_ID				3
#define 		VOL_MUTEICON_ID				4
#define 		VOL_MUTETEXT_ID				5
#define 		VOL_BLUETOOTH_ID			6

#if defined(DEF_USE_BACKGROUND_FRAME)

#define			AP_VALUME_GAP_X					190
#define			AP_VALUME_GAP_Y					174
#define 		AP_VOLUME_FRAME_X				970
#define 		AP_VOLUME_FRAME_Y				0
#define			AP_VOLUME_FRAME_W				310
#define			AP_VOLUME_FRAME_H				720

#else /* DEF_USE_BACKGROUND_FRAME */

#define			AP_VALUME_GAP_X					30//0
#define			AP_VALUME_GAP_Y					0
#define 		AP_VOLUME_FRAME_X				1160
#define 		AP_VOLUME_FRAME_Y				174
#define			AP_VOLUME_FRAME_W				110//50
#define			AP_VOLUME_FRAME_H				400

#endif /* DEF_USE_BACKGROUND_FRAME */

#define 		AP_VOLUME_PROGRESS_X			(AP_VALUME_GAP_X+7)
#define 		AP_VOLUME_PROGRESS_Y			(AP_VALUME_GAP_Y)
#define			AP_VOLUME_PROGRESS_W			33
#define			AP_VOLUME_PROGRESS_H			312

#define 		AP_VOLUME_NUMTEXT_X				(AP_VALUME_GAP_X)
#define 		AP_VOLUME_NUMTEXT_Y				(AP_VALUME_GAP_Y+305)
#define			AP_VOLUME_NUMTEXT_W				47
#define			AP_VOLUME_NUMTEXT_H				40

#define 		AP_VOLUME_MUTETEXT_X			1//(AP_VALUME_GAP_X+1)
#define 		AP_VOLUME_MUTETEXT_Y			(AP_VALUME_GAP_Y+305)
#define			AP_VOLUME_MUTETEXT_W			105//47
#define			AP_VOLUME_MUTETEXT_H			26

#define 		AP_VOLUME_MUTEICON_X			(AP_VALUME_GAP_X+10)
#define 		AP_VOLUME_MUTEICON_Y			(AP_VALUME_GAP_Y+327)
#define			AP_VOLUME_MUTEICON_W			33
#define			AP_VOLUME_MUTEICON_H			33

#define 		AP_VOLUME_BLUETOOTH_X			(AP_VALUME_GAP_X+1)
#define 		AP_VOLUME_BLUETOOTH_Y			(AP_VALUME_GAP_Y+327)
#define			AP_VOLUME_BLUETOOTH_W			45
#define			AP_VOLUME_BLUETOOTH_H			58

#define		AP_VOLUME_VSPACE_HEIGHT			8

#define 		VOLUME_FRAME				"/usr/image/104_Background_R.png"
#define 		VOLUME_BLANK				"/usr/image/210_Vol_Blank.png"
#define 		VOLUME_FILL					"/usr/image/210_Vol_Fill.png"
#define 		VOLUME_MUTE					"/usr/image/210_Vol_Mute.png"
#define 		VOLUME_BLUETOOTH			"/usr/image/581_VOL_Bluetooth.png"


#define 		FONT_PATH					"/usr/fonts/HelveticaNeueLTW1G-MdCnUpdv3.001.otf"
#define 		FONT_POSITION_X				(VOLUMEBG_x + 13)
#define 		FONT_POSITION_Y				(VOLUMEBG_Y + 472)
#define			FONT_SIZE					26
#define			FONT_COLOR					0xFFE1E1E1







typedef struct _Vol_Contents
{
	HINT32			appvolume;
	HINT32          octovolume;
	HINT32          volumeMax;
	HINT32          volumeMin;
	HBOOL           isMute;
	HBOOL           isBluetooth;
	HBOOL			isShow;
	HCHAR			szNumText[64];
	HCHAR			szMuteText[64];
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
static Vol_Contents_t* local_volume_VolumeGetContents (void );
//update

//process
static ONDK_Result_t		local_volume_Initialize(Vol_Contents_t *pstContents);
//destroy

//ui

//msg run
static ONDK_Result_t 	local_volume_MsgGwmDestroy(Vol_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	local_volume_MsgGwmCreate(Vol_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	local_volume_MsgGwmKeyDown(Vol_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

/**************************************************************************************************/
#define _________AP_Volume_Private_Init__________________________________________________________
/**************************************************************************************************/
static Vol_Contents_t* local_volume_VolumeGetContents(void)
{
	return &s_Volume_Contents;
}

/**************************************************************************************************/
#define _________AP_Volume_Private_Update__________________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define _________AP_Volume_Private_Process__________________________________________________________
/**************************************************************************************************/
static ONDK_Result_t		local_volume_Initialize(Vol_Contents_t *pstContents)
{
	ONDK_Assert(pstContents);
	APK_OUTPUT_AUDIO_GetVolume(&pstContents->octovolume);
	APK_OUTPUT_AUDIO_GetVolumeMax(&pstContents->volumeMax);
    pstContents->volumeMin = 0;

    APK_OUTPUT_AUDIO_GetMasterMute(&pstContents->isMute);

	pstContents->appvolume = (pstContents->octovolume * VOLUME_STEP) / (pstContents->volumeMax - pstContents->volumeMin);
	HxSTD_sprintf(pstContents->szNumText, "%d",pstContents->appvolume);
	//HxLOG_Print ("[%s:%d] octo vol : %d, Max vol : %d, app vol : %d\n", __FUNCTION__, __LINE__, pstContents->octovolume, pstContents->volumeMax, pstContents->appvolume);

	return GWM_RET_OK;
}



static void local_volume_setHideUI(HBOOL bHide)
{
	Vol_Contents_t *pstContents = local_volume_VolumeGetContents();

	pstContents->isShow = !bHide;

	if(bHide == TRUE)
	{
		#if defined(DEF_USE_BACKGROUND_FRAME)
		ONDK_GWM_Obj_SetHide((VOL_FRAME_ID), TRUE);
		#endif
		ONDK_GWM_Obj_SetHide((VOL_NUMTEXT_ID), TRUE);
		ONDK_GWM_Obj_SetHide((VOL_MUTETEXT_ID), TRUE);
		ONDK_GWM_Obj_SetHide((VOL_MUTEICON_ID), TRUE);
		ONDK_GWM_Obj_SetHide((VOL_BLUETOOTH_ID), TRUE);
		ONDK_GWM_Obj_SetHide((VOL_PROGRESS_ID), TRUE);

		APK_OUTPUT_PANEL_SetPanelAuto(eApkPANEL_PRIORITY_MIDDLE);
	}
	else
	{
		if(pstContents->isBluetooth == TRUE)
		{
			ONDK_GWM_Obj_SetVisible((VOL_BLUETOOTH_ID), pstContents->isBluetooth);
			ONDK_GWM_Obj_SetHide((VOL_MUTETEXT_ID), pstContents->isBluetooth);
			ONDK_GWM_Obj_SetHide((VOL_MUTEICON_ID), pstContents->isBluetooth);
			ONDK_GWM_Obj_SetHide((VOL_NUMTEXT_ID), pstContents->isBluetooth);
			#if defined(DEF_USE_BACKGROUND_FRAME)
				ONDK_GWM_Obj_SetHide((VOL_FRAME_ID), FALSE);
			#endif
				ONDK_GWM_Obj_SetHide((VOL_PROGRESS_ID), FALSE);
		}
		else
		{
			ONDK_GWM_Obj_SetVisible((VOL_BLUETOOTH_ID), pstContents->isBluetooth);
			ONDK_GWM_Obj_SetHide((VOL_NUMTEXT_ID), pstContents->isMute);
			ONDK_GWM_Obj_SetVisible((VOL_MUTETEXT_ID), pstContents->isMute);
			ONDK_GWM_Obj_SetVisible((VOL_MUTEICON_ID), pstContents->isMute);
			#if defined(DEF_USE_BACKGROUND_FRAME)
				ONDK_GWM_Obj_SetHide((VOL_FRAME_ID), FALSE);
			#endif
				ONDK_GWM_Obj_SetHide((VOL_PROGRESS_ID), FALSE);
		}

	}
	ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
}


static void local_volume_Setting(Vol_Contents_t *pstContents, HUINT32 	key)
{
	HBOOL	drawPanelVolText = FALSE;
	HUINT8	*str = NULL;
    HBOOL   bEnable          = FALSE;

    /*
        * Settings > System Settings > HDMI-CEC 의 Tv Volume Control 활성화시
        * STB 의 volume 조작은 없고, CEC 로 TV 로 신호를 보내 TV 에서 처리하도록 유도한다.
        */
    if(APK_OUTPUT_HDMI_GetCecVolumeCtrlEnable(&bEnable) == ERR_OK)
    {
        if(bEnable == TRUE)
        {
            switch(key)
        	{
        	case KEY_VOLUMEUP:
                APK_OUTPUT_HDMI_CecUserCtrlPressed(eDxHDMI_USER_CTRL_VOLUME_UP);
                break;
            case KEY_VOLUMEDOWN:
                APK_OUTPUT_HDMI_CecUserCtrlPressed(eDxHDMI_USER_CTRL_VOLUME_DOWN);
                break;
            case KEY_MUTE:
                APK_OUTPUT_HDMI_CecUserCtrlPressed(eDxHDMI_USER_CTRL_VOLUME_MUTE);
                break;
            default:
                HxLOG_Warning("Do not used cec(user control pressed)...");
                break;
            }
            local_volume_setHideUI(TRUE);
            return;
        }

    }

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

			if(VOLUME_MAX	> pstContents->appvolume)
			{
				++pstContents->appvolume;

				pstContents->octovolume = (pstContents->volumeMax - pstContents->volumeMin)/VOLUME_STEP * pstContents->appvolume;
				ONDK_Print("volume-octo [%d] volume-app[%d]\n",pstContents->octovolume,pstContents->appvolume);
				APK_OUTPUT_AUDIO_SetVolume(pstContents->octovolume);
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

			if(VOLUME_MIN	< pstContents->appvolume)
			{
				--pstContents->appvolume;

				pstContents->octovolume = (pstContents->volumeMax - pstContents->volumeMin)/VOLUME_STEP * pstContents->appvolume;
				ONDK_Print("volume-octo [%d] volume-app[%d]\n",pstContents->octovolume,pstContents->appvolume);
				APK_OUTPUT_AUDIO_SetVolume(pstContents->octovolume);
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
//				APK_OUTPUT_PANEL_SetPanelDimLevel(eApkPANEL_PRIORITY_MIDDLE, 100);
				str = "MUTE";
				APK_OUTPUT_PANEL_SetPanel(eApkPANEL_PRIORITY_MIDDLE, str, eDxPANEL_ALIGN_CENTER);
#if !defined(CONFIG_OP_TELENOR)
				ONDK_GWM_KillTimer(GWMPOPUP_TIMER_VOLID);
#endif
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

		//HxSTD_sprintf(szPanelStr, "%s %d", str, pstContents->appvolume);
		HxSTD_sprintf(szPanelStr, "VOLUME %d", pstContents->appvolume); // ux에서 VOLUME으로 나오게...
//		APK_OUTPUT_PANEL_SetPanelDimLevel(eApkPANEL_PRIORITY_MIDDLE, 100);
		APK_OUTPUT_PANEL_SetPanel(eApkPANEL_PRIORITY_MIDDLE, szPanelStr, eDxPANEL_ALIGN_CENTER);

		HxSTD_sprintf(pstContents->szNumText, "%d", pstContents->appvolume);

		ONDK_GWM_COM_VProgress_SetPos(VOL_PROGRESS_ID, pstContents->appvolume);
		ONDK_GWM_ResetTimer(GWMPOPUP_TIMER_VOLID);
	}
	else
	{
		ONDK_GWM_COM_VProgress_SetPos(VOL_PROGRESS_ID, 0);
	}


#if defined(CONFIG_OP_TDC)
	local_volume_setHideUI(TRUE);
#else
	local_volume_setHideUI(FALSE);
#endif
}


/**************************************************************************************************/
#define _________AP_Volume_Private_Destroy__________________________________________________________
/**************************************************************************************************/
static ONDK_Result_t local_volume_DestroyVolume(Vol_Contents_t *pstContents)
{
	return GWM_RET_OK;
}

/**************************************************************************************************/
#define _________AP_Volume_Private_Proc_UI__________________________________________________________
/**************************************************************************************************/


static ONDK_Result_t 	local_volume_SetUiObject(Vol_Contents_t *pstContents)
{
	ONDK_Rect_t 		stRect;
	ONDK_Size_t 		fillSize = {0, 0}, blankSize = {0, 0};
	HINT32			vprogressHeight = 0;

	ONDK_Assert(pstContents);

	ONDK_GWM_APP_SetAppArea(AP_VOLUME_FRAME_X, AP_VOLUME_FRAME_Y, AP_VOLUME_FRAME_W, AP_VOLUME_FRAME_H);

	#if defined(DEF_USE_BACKGROUND_FRAME)
	stRect = ONDK_Rect(0,0,AP_VOLUME_FRAME_W,AP_VOLUME_FRAME_H);
	ONDK_GWM_COM_Image_Create(VOL_FRAME_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(VOL_FRAME_ID, VOLUME_FRAME);
	#endif

	fillSize = ONDK_IMG_GetSize(ONDK_IMG_CreateSelf(VOLUME_FILL));
	blankSize = ONDK_IMG_GetSize(ONDK_IMG_CreateSelf(VOLUME_BLANK));
	vprogressHeight = (((fillSize.h >= blankSize.h) ? fillSize.h : blankSize.h) + AP_VOLUME_VSPACE_HEIGHT) * VOLUME_MAX;

	//stRect = ONDK_Rect(AP_VOLUME_PROGRESS_X,AP_VOLUME_PROGRESS_Y,AP_VOLUME_PROGRESS_W,AP_VOLUME_PROGRESS_H);
	stRect = ONDK_Rect(AP_VOLUME_PROGRESS_X,AP_VOLUME_PROGRESS_Y,AP_VOLUME_PROGRESS_W,vprogressHeight);
	ONDK_GWM_COM_VProgress_Create(VOL_PROGRESS_ID, &stRect, 0, 0, 0);
	ONDK_GWM_COM_VProgress_SetBlankImg(VOL_PROGRESS_ID, VOLUME_BLANK);
	ONDK_GWM_COM_VProgress_SetFillImg(VOL_PROGRESS_ID, VOLUME_FILL);
	ONDK_GWM_COM_VProgress_SetVspace(VOL_PROGRESS_ID, AP_VOLUME_VSPACE_HEIGHT);
	ONDK_GWM_COM_VProgress_SetMax(VOL_PROGRESS_ID, VOLUME_MAX);
	ONDK_GWM_COM_VProgress_SetMin(VOL_PROGRESS_ID, VOLUME_MIN);
	ONDK_GWM_COM_VProgress_SetPos(VOL_PROGRESS_ID, pstContents->appvolume);

	//stRect = ONDK_Rect(AP_VOLUME_NUMTEXT_X,AP_VOLUME_NUMTEXT_Y,AP_VOLUME_NUMTEXT_W,AP_VOLUME_NUMTEXT_H);
	stRect = ONDK_Rect(AP_VOLUME_NUMTEXT_X,AP_VOLUME_PROGRESS_Y+vprogressHeight-7,AP_VOLUME_NUMTEXT_W,AP_VOLUME_NUMTEXT_H);
	ONDK_GWM_COM_Text_Create(VOL_NUMTEXT_ID, &stRect, pstContents->szNumText);
	ONDK_GWM_COM_Text_SetSize(VOL_NUMTEXT_ID, 36);
	ONDK_GWM_COM_Text_SetAlign(VOL_NUMTEXT_ID, TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_SetFont(VOL_NUMTEXT_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_ChangeColor(VOL_NUMTEXT_ID, 0xff2eb2ff, 0xff2eb2ff);

	stRect = ONDK_Rect(AP_VOLUME_MUTETEXT_X,AP_VOLUME_MUTETEXT_Y,AP_VOLUME_MUTETEXT_W,AP_VOLUME_MUTETEXT_H);
	ONDK_GWM_COM_Text_Create(VOL_MUTETEXT_ID, &stRect, pstContents->szMuteText);
	ONDK_GWM_COM_Text_SetSize(VOL_MUTETEXT_ID, 20);
	ONDK_GWM_COM_Text_SetAlign(VOL_MUTETEXT_ID, TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_SetFont(VOL_MUTETEXT_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_ChangeColor(VOL_MUTETEXT_ID, 0xff2eb2ff, 0xff2eb2ff);


	stRect = ONDK_Rect(AP_VOLUME_MUTEICON_X,AP_VOLUME_MUTEICON_Y,AP_VOLUME_MUTEICON_W,AP_VOLUME_MUTEICON_H);
	ONDK_GWM_COM_Image_Create(VOL_MUTEICON_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(VOL_MUTEICON_ID, VOLUME_MUTE);

	stRect = ONDK_Rect(AP_VOLUME_MUTEICON_X,AP_VOLUME_MUTEICON_Y,AP_VOLUME_MUTEICON_W,AP_VOLUME_MUTEICON_H);
	ONDK_GWM_COM_Image_Create(VOL_MUTEICON_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(VOL_MUTEICON_ID, VOLUME_MUTE);

	stRect = ONDK_Rect(AP_VOLUME_BLUETOOTH_X,AP_VOLUME_BLUETOOTH_Y,AP_VOLUME_BLUETOOTH_W,AP_VOLUME_BLUETOOTH_H);
	ONDK_GWM_COM_Image_Create(VOL_BLUETOOTH_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(VOL_BLUETOOTH_ID, VOLUME_BLUETOOTH);
	return GWM_RET_OK;
}

/**************************************************************************************************/
#define _________AP_Volume_Private_Proc_MSG_RUNNIG__________________________________________________________
/**************************************************************************************************/

//MSG_GWM_CREATE
static ONDK_Result_t local_volume_MsgGwmCreate(Vol_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	pstContents->isBluetooth = p3;

	ONDK_Assert(pstContents);

	local_volume_Initialize(pstContents);
#if !defined(CONFIG_PROD_ROCKYPOM) && !defined(CONFIG_PROD_ROCKYWPOM) //CVC_DEMO
	local_volume_SetUiObject(pstContents );
#endif
	if (pstContents->isBluetooth == FALSE)
		APK_OUTPUT_AUDIO_SetMasterMute(FALSE);

	ONDK_GWM_SetTimer(GWMPOPUP_TIMER_VOLID, VOLUMEWND_EXPIRE_TIME);
	local_volume_Setting(pstContents, p1);

	return ONDK_MESSAGE_CONSUMED;
}

//MSG_GWM_KEYDOWN
static ONDK_Result_t local_volume_MsgGwmKeyDown(Vol_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32 nKeyId = p1;
	HUINT32 ulCurrentTick;
	static HUINT32 s_volInterval = 0;

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
#if defined(CONFIG_PROD_ROCKYPOM) || defined(CONFIG_PROD_ROCKYWPOM) //CVC_DEMO
			return ONDK_MESSAGE_NONCONSUMED;
#else
			return ONDK_MESSAGE_CONSUMED;
#endif
		default:
			return ONDK_MESSAGE_NONCONSUMED;
		}
	}
	else
		s_volInterval =  VK_TIMER_GetSystemTick();

	switch (nKeyId)
	{
	case KEY_VOLUMEUP:
	case KEY_VOLUMEDOWN:
	case KEY_MUTE:
		if(ONDK_GWM_SetTimer(GWMPOPUP_TIMER_VOLID, VOLUMEWND_EXPIRE_TIME) == GWM_RET_LINK_CONFLICT)
			ONDK_GWM_ResetTimer(GWMPOPUP_TIMER_VOLID);
		local_volume_Setting(pstContents, nKeyId);
#if defined(CONFIG_PROD_ROCKYPOM) || defined(CONFIG_PROD_ROCKYWPOM) //CVC_DEMO
		return ONDK_MESSAGE_NONCONSUMED;
#else
		return ONDK_MESSAGE_CONSUMED;
#endif

		/* keys to exit */
	case KEY_OK:
	case KEY_BACK:
	case KEY_EXIT:
		if(pstContents->isShow == TRUE && pstContents->isMute == FALSE)
		{
			local_volume_setHideUI(TRUE);
			return ONDK_MESSAGE_CONSUMED;
		}
		return ONDK_MESSAGE_PASS;

	default:
		return ONDK_MESSAGE_PASS;
	}

	AP_VOL_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);
	return GWM_RET_OK;
}


//MSG_GWM_TIMER
static ONDK_Result_t local_volume_MsgGwmTimer(Vol_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 ulTimerID = p1;

	ONDK_Assert(pstContents);

	if (ulTimerID == GWMPOPUP_TIMER_VOLID)
	{
#if defined(CONFIG_OP_TELENOR)
		if(pstContents->isShow == TRUE || pstContents->isBluetooth == TRUE)
#else
		if(pstContents->isMute != TRUE || pstContents->isBluetooth == TRUE)
#endif
		{
			local_volume_setHideUI(TRUE);
			ONDK_GWM_KillTimer(GWMPOPUP_TIMER_VOLID);
		}
		else
		{
			APK_OUTPUT_PANEL_SetPanelAuto(eApkPANEL_PRIORITY_MIDDLE);
		}
		return GWM_MESSAGE_BREAK;
	}

	AP_VOL_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);
	return GWM_RET_OK;
}

//MSG_GWM_DESTROY
static ONDK_Result_t local_volume_MsgGwmDestroy(Vol_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Assert(pstContents);

	AP_VOL_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);
	APK_OUTPUT_PANEL_SetPanelAuto(eApkPANEL_PRIORITY_MIDDLE);

	return local_volume_DestroyVolume(pstContents);
}

// MSG_GWM_HAPI_MESSAGE
static ONDK_Result_t local_volume_MsgGwmHapiMsg(Vol_Contents_t *pstContents, HAPIMSG_e eMsg, HINT32 p1, HINT32 p2, HINT32 p3)
{

	ONDK_Result_t		ret = GWM_MESSAGE_PASS;
	HINT32				isBTState = p2;

	switch (eMsg)
	{
		case eHMSG_BLUETOOTH_MUTE:
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
			ret = GWM_MESSAGE_PASS;
			break;
	}

	return ret;

}
/*
static ONDK_Result_t local_volume_APKMessage(Vol_Contents_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			ret = GWM_MESSAGE_PASS;
	GWM_APK_MSG_Type_e		eAPK_Type = (GWM_APK_MSG_Type_e)handle;
	APKE_BT_EVENT_e			eEvent = (APKE_BT_EVENT_e)p1;
	HxLOG_Print("@@@@@@@@@@@@@@local_volume_APKMessage handle : %d \r\n",p1);

	if(eAPK_Type == GWM_APK_BLUETOOTH)
	{
		HINT32		isBTState = p2;
		switch (eEvent)
		{
			case eBLUETOOTH_EVENT_AUDIO_MUTE:
HxLOG_Print("@@@@@@@@@@@@@@eBLUETOOTH_EVENT_AUDIO_MUTE \r\n");
				if(isBTState == TRUE)
				{
					HxLOG_Print("@@@@@@@@@@@@@@eBLUETOOTH_EVENT_AUDIO_MUTE true \r\n");
					pstContents->isBluetooth = TRUE;
				}
				else if(isBTState == FALSE)
				{
					HxLOG_Print("@@@@@@@@@@@@@@@@@@eBLUETOOTH_EVENT_AUDIO_MUTE false \r\n");
					pstContents->isBluetooth = FALSE;
				}
				ret = GWM_MESSAGE_BREAK;
				break;
			default:
				ret = GWM_MESSAGE_BREAK;
		}
	}

	return ret;
}
*/
static ONDK_Result_t local_volume_BlueToothMsg(Vol_Contents_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
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


ONDK_Result_t			AP_AudioVolume_Proc(HINT32 nMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult	= GWM_RET_OK;
	static ONDK_Result_t s_upkey =ONDK_RET_OK;
	Vol_Contents_t *pstContents = local_volume_VolumeGetContents();
	ONDK_Print(HxANSI_COLOR_YELLOW("[%s:%d] Message(%d) : %s  p1(0x%x) p2(0x%x) p3(0x%x) \n"), __FUNCTION__, __LINE__, nMessage, ONDK_GWM_GwmMsg2String(nMessage), p1, p2, p3);

	switch (nMessage)
	{
	case MSG_GWM_CREATE:
		eResult = local_volume_MsgGwmCreate(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_KEYUP:
		eResult = s_upkey;
		break;
	case MSG_GWM_KEYDOWN:
		eResult = local_volume_MsgGwmKeyDown(pstContents, p1, p2, p3);
		s_upkey = eResult;
		break;
	case MSG_GWM_TIMER:
		eResult = local_volume_MsgGwmTimer(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_DESTROY:
		eResult = local_volume_MsgGwmDestroy(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_APK_BLUETOOTH:
		eResult = local_volume_BlueToothMsg(pstContents,handle, p1, p2, p3);
		break;
	case MSG_GWM_HAPI_MESSAGE:
		eResult = local_volume_MsgGwmHapiMsg(pstContents, (HAPIMSG_e)handle, p1, p2, p3);
		break;
		/*
	case MSG_GWM_APK_MESSAGE:
		HxLOG_Print("MSG_GWM_APK_MESSAGE , %d\r\n",handle);
		eResult = local_volume_APKMessage(pstContents,handle, p1, p2, p3);
		break;
*/
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

/**************************************************************************************************/
#define _________AP_Volume_End_Of_File____________________________________________________
/**************************************************************************************************/

