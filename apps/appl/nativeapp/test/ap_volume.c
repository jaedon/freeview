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
#include   <ondk.h>
#include   <ondk_type.h>
#include   <ondk_gfx.h>
#include	<hlib.h>
#include	<ondk.h>
#include	<nxres_strid.h>

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





#define VOLUMEWND_EXPIRE_TIMER_ID			0x001
#define VOLUMEWND_TIME_SECOND				1000
#define VOLUMEWND_EXPIRE_TIME				(3 * VOLUMEWND_TIME_SECOND)

/**************************************************************************************************/
#define _________AP_Volume_Private_Struct_________________________________________________
/**************************************************************************************************/

#define 		VOL_FRAME_ID				1
#define 		VOL_NUMTEXT_ID				2
#define 		VOL_PROGRESS_ID				3
#define 		VOL_MUTEICON_ID				4
#define 		VOL_MUTETEXT_ID				5


#define 		AP_VOLUME_FRAME_X				970
#define 		AP_VOLUME_FRAME_Y				0
#define			AP_VOLUME_FRAME_W				310
#define			AP_VOLUME_FRAME_H				720

#define 		AP_VOLUME_PROGRESS_X			197
#define 		AP_VOLUME_PROGRESS_Y			174
#define			AP_VOLUME_PROGRESS_W			33
#define			AP_VOLUME_PROGRESS_H			312

#define 		AP_VOLUME_NUMTEXT_X				190
#define 		AP_VOLUME_NUMTEXT_Y				529
#define			AP_VOLUME_NUMTEXT_W				47
#define			AP_VOLUME_NUMTEXT_H				40

#define 		AP_VOLUME_MUTETEXT_X			190
#define 		AP_VOLUME_MUTETEXT_Y			514
#define			AP_VOLUME_MUTETEXT_W			47
#define			AP_VOLUME_MUTETEXT_H			26

#define 		AP_VOLUME_MUTEICON_X			200
#define 		AP_VOLUME_MUTEICON_Y			525
#define			AP_VOLUME_MUTEICON_W			33
#define			AP_VOLUME_MUTEICON_H			33


#define 		VOLUME_FRAME				"/usr/image/104_Background_R.png"
#define 		VOLUME_BLANK				"/usr/image/210_Vol_Blank.png"
#define 		VOLUME_FILL					"/usr/image/210_Vol_Fill.png"
#define 		VOLUME_MUTE					"/usr/image/210_Vol_Mute.png"

//#define 		FONT_PATH					"/usr/fonts/HelveticaNeueLTW1G-MdCnUpdv3.001.otf"
#define 		FONT_POSITION_X				(VOLUMEBG_x + 13)
#define 		FONT_POSITION_Y				(VOLUMEBG_Y + 472)
#define			FONT_SIZE					26
#define			FONT_COLOR					0xFFE1E1E1







typedef struct _Vol_Contents
{
	HUINT32			ulCurFocus;
	HUINT32			ulNumOfSubItem, ulMaxNumOfSubItem;
	HINT32			appvolume;
	HINT32          octovolume;
	HINT32          volumeMax;
	HINT32          volumeMin;
	HBOOL           isMute;

	HCHAR			szNumText[64];
	HCHAR			szMuteText[64];
} Vol_Contents_t;


/**************************************************************************************************/
/*hms-1000s volume */
#define VOLUME_STEP 20
#define VOLUME_MAX VOLUME_STEP
#define VOLUME_MIN 0

HUINT32 gvolumetable[VOLUME_STEP +1]={
	0,3,5,7,10,13,15,17,20,23,
	25,27,30,33,35,37,40,43,45,47,50
};
/**************************************************************************************************/


/**************************************************************************************************/
#define _________AP_Volume_Private_Value_________________________________________________
/**************************************************************************************************/

static Vol_Contents_t			s_Volume_Contents;



HUINT32 local_volume_ConvertOctoToAPP(HUINT32 volume)
{
	HUINT32 i;

	for(i=0; i<= VOLUME_STEP; i++)
	{
		if(volume == gvolumetable[i])
		{
			return i;
		}
	}
	return 0;
}



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
static ONDK_Result_t 	local_volume_MsgGwmKeyUp(Vol_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
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

	pstContents->appvolume = local_volume_ConvertOctoToAPP(pstContents->octovolume);
	HxSTD_sprintf(pstContents->szNumText, "%d",pstContents->appvolume);

	return GWM_RET_OK;
}

static void local_volume_Setting(Vol_Contents_t *pstContents, HUINT32 	key)
{
	HBOOL	drawPanelVolText = FALSE;
	HUINT8	*str = NULL;

	switch(key)
	{
	case KEY_VOLUMEUP:
		if(VOLUME_MAX	> pstContents->appvolume)
		{
			++pstContents->appvolume;
			ONDK_Print("volume-octo [%d] volume-app[%d]\n",gvolumetable[pstContents->appvolume],pstContents->appvolume);
		}
		drawPanelVolText = TRUE;
		break;

	case KEY_VOLUMEDOWN:
		if(VOLUME_MIN	< pstContents->appvolume)
		{
			--pstContents->appvolume;
			ONDK_Print("volume-octo [%d] volume-app[%d]\n",gvolumetable[pstContents->appvolume],pstContents->appvolume);
		}
		drawPanelVolText = TRUE;
		break;

	case KEY_MUTE:
		ONDK_Print("[%s:%d] Mute (%d)\n", __FUNCTION__, __LINE__, pstContents->isMute);
		pstContents->isMute = !pstContents->isMute;

		if(pstContents->isMute == TRUE)
		{
			str = ONDK_GetString(RES_MUTE_ID);
			if(str == NULL) str = "# Mute #";

			HxSTD_sprintf(pstContents->szMuteText, "%s", str);
		}
		else
		{
			drawPanelVolText = TRUE;
		}

		break;

	default:
		return;
 	}
	ONDK_GWM_Obj_SetHide((VOL_NUMTEXT_ID), pstContents->isMute);
	ONDK_GWM_Obj_SetVisible((VOL_MUTETEXT_ID), pstContents->isMute);
	ONDK_GWM_Obj_SetVisible((VOL_MUTEICON_ID), pstContents->isMute);

	if(drawPanelVolText == TRUE)
	{
		HCHAR szPanelStr[32] = {0, };

		str = ONDK_GetString(RES_VOLUME_ID);
		if(str == NULL) str = "# Volume #";

		HxSTD_sprintf(szPanelStr, "%s %d", str, pstContents->appvolume);
		HxSTD_sprintf(pstContents->szNumText, "%d", pstContents->appvolume);
		ONDK_GWM_COM_VProgress_SetPos(VOL_PROGRESS_ID, pstContents->appvolume);
	}
	else
	{
		ONDK_GWM_COM_VProgress_SetPos(VOL_PROGRESS_ID, 0);
	}
	ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
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

	ONDK_Assert(pstContents);

	ONDK_GWM_APP_SetAppArea(AP_VOLUME_FRAME_X, AP_VOLUME_FRAME_Y, AP_VOLUME_FRAME_W, AP_VOLUME_FRAME_H);

	stRect = ONDK_Rect(0,0,AP_VOLUME_FRAME_W,AP_VOLUME_FRAME_H);
	ONDK_GWM_COM_Image_Create(VOL_FRAME_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(VOL_FRAME_ID, VOLUME_FRAME);

	stRect = ONDK_Rect(AP_VOLUME_PROGRESS_X,AP_VOLUME_PROGRESS_Y,AP_VOLUME_PROGRESS_W,AP_VOLUME_PROGRESS_H);
	ONDK_GWM_COM_VProgress_Create(VOL_PROGRESS_ID, &stRect, 0, 0, 0);
	ONDK_GWM_COM_VProgress_SetBlankImg(VOL_PROGRESS_ID, VOLUME_BLANK);
	ONDK_GWM_COM_VProgress_SetFillImg(VOL_PROGRESS_ID, VOLUME_FILL);
	ONDK_GWM_COM_VProgress_SetVspace(VOL_PROGRESS_ID, 8);
	ONDK_GWM_COM_VProgress_SetMax(VOL_PROGRESS_ID, VOLUME_MAX);
	ONDK_GWM_COM_VProgress_SetMin(VOL_PROGRESS_ID, VOLUME_MIN);
	ONDK_GWM_COM_VProgress_SetPos(VOL_PROGRESS_ID, pstContents->appvolume);

	stRect = ONDK_Rect(AP_VOLUME_NUMTEXT_X,AP_VOLUME_NUMTEXT_Y,AP_VOLUME_NUMTEXT_W,AP_VOLUME_NUMTEXT_H);
	ONDK_GWM_COM_Text_Create(VOL_NUMTEXT_ID, &stRect, pstContents->szNumText);
	ONDK_GWM_COM_Text_SetSize(VOL_NUMTEXT_ID, 36);
	ONDK_GWM_COM_Text_SetAlign(VOL_NUMTEXT_ID, TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_ChangeColor(VOL_NUMTEXT_ID, 0xff2eb2ff, 0xff2eb2ff);

	stRect = ONDK_Rect(AP_VOLUME_MUTETEXT_X,AP_VOLUME_MUTETEXT_Y,AP_VOLUME_MUTETEXT_W,AP_VOLUME_MUTETEXT_H);
	ONDK_GWM_COM_Text_Create(VOL_MUTETEXT_ID, &stRect, pstContents->szMuteText);
	ONDK_GWM_COM_Text_SetSize(VOL_MUTETEXT_ID, 18);
	ONDK_GWM_COM_Text_SetAlign(VOL_MUTETEXT_ID, TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_ChangeColor(VOL_MUTETEXT_ID, 0xff2eb2ff, 0xff2eb2ff);


	stRect = ONDK_Rect(AP_VOLUME_MUTEICON_X,AP_VOLUME_MUTEICON_Y,AP_VOLUME_MUTEICON_W,AP_VOLUME_MUTEICON_H);
	ONDK_GWM_COM_Image_Create(VOL_MUTEICON_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(VOL_MUTEICON_ID, VOLUME_MUTE);

	ONDK_GWM_Obj_SetHide((VOL_NUMTEXT_ID), pstContents->isMute);
	ONDK_GWM_Obj_SetVisible((VOL_MUTETEXT_ID), pstContents->isMute);
	ONDK_GWM_Obj_SetVisible((VOL_MUTEICON_ID), pstContents->isMute);



	return GWM_RET_OK;
}

/**************************************************************************************************/
#define _________AP_Volume_Private_Proc_MSG_RUNNIG__________________________________________________________
/**************************************************************************************************/

//MSG_GWM_CREATE
static ONDK_Result_t local_volume_MsgGwmCreate(Vol_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Assert(pstContents);


	/* Audio Mute Off */

	/* 절대 hide가 되지 않는다. */
	ONDK_GWM_APP_SetUIHideFlag(GWM_UI_Hide_Never);

	local_volume_Initialize(pstContents);
	local_volume_SetUiObject(pstContents);

	local_volume_Setting(pstContents,p1);

	ONDK_GWM_SetTimer(VOLUMEWND_EXPIRE_TIMER_ID, VOLUMEWND_EXPIRE_TIME);

	AP_VOL_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);

	return GWM_RET_OK;
}

//MSG_GWM_UP
static ONDK_Result_t local_volume_MsgGwmKeyUp(Vol_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32 nKeyId = p1;

	ONDK_Assert(pstContents);

	switch (nKeyId)
	{
		case KEY_VOLUMEUP:
		case KEY_VOLUMEDOWN:
		case KEY_MUTE:
			return ONDK_MESSAGE_CONSUMED;
		default:
			break;
	}

	AP_VOL_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);
	return ONDK_MESSAGE_NONCONSUMED;
}
//MSG_GWM_KEYDOWN
static ONDK_Result_t local_volume_MsgGwmKeyDown(Vol_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32 nKeyId = p1;

	ONDK_Assert(pstContents);

	ONDK_GWM_ResetTimer(VOLUMEWND_EXPIRE_TIMER_ID);

	switch (nKeyId)
	{
	case KEY_VOLUMEUP:
	case KEY_VOLUMEDOWN:
	case KEY_MUTE:
		local_volume_Setting(pstContents, nKeyId);
		return ONDK_MESSAGE_CONSUMED;


		/* keys to exit */
	case KEY_OK:
	case KEY_BACK:
	case KEY_EXIT:
		ONDK_GWM_APP_Destroy(0);
		return ONDK_MESSAGE_CONSUMED;


	default:
		//ONDK_GWM_APP_Destroy(0);
		return GWM_MESSAGE_PASS;
	}

	AP_VOL_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);
	return GWM_RET_OK;
}


//MSG_GWM_TIMER
static ONDK_Result_t local_volume_MsgGwmTimer(Vol_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 ulTimerID = p1;

	ONDK_Assert(pstContents);

	if (ulTimerID == VOLUMEWND_EXPIRE_TIMER_ID)
	{
		if(pstContents->isMute != TRUE)
		{
			ONDK_GWM_APP_Destroy(0); //mute 상태는 숨지 않는다.
		}
		else
		{
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


	return local_volume_DestroyVolume(pstContents);
}

// MSG_GWM_HAPI_MESSAGE
static ONDK_Result_t local_volume_MsgGwmHapiMsg(Vol_Contents_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Assert(pstContents);

	AP_VOL_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);


	return GWM_MESSAGE_PASS;
}


/**************************************************************************************************/
#define _________AP_Volume_Public_Proc____________________________________________________
/**************************************************************************************************/


ONDK_Result_t			AP_AudioVolume_Proc(HINT32 nMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult	= GWM_RET_OK;
	Vol_Contents_t *pstContents = local_volume_VolumeGetContents();

	switch (nMessage)
	{
	case MSG_GWM_CREATE:
		eResult = local_volume_MsgGwmCreate(pstContents, p1, p2, p3);
		break;

	case MSG_GWM_KEYUP:
		eResult = local_volume_MsgGwmKeyUp(pstContents, p1, p2, p3);
		break;

	case MSG_GWM_KEYDOWN:
		eResult = local_volume_MsgGwmKeyDown(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_TIMER:
		eResult = local_volume_MsgGwmTimer(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_DESTROY:
		eResult = local_volume_MsgGwmDestroy(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_HAPI_MESSAGE:
		eResult = local_volume_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
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

/**************************************************************************************************/
#define _________AP_Volume_End_Of_File____________________________________________________
/**************************************************************************************************/

