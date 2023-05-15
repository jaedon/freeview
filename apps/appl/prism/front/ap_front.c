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
#define _________Front_Private_Include_________________________________________________
/**************************************************************************************************/
#include	<ondk.h>
#include	<ondk_type.h>
#include	<ondk_gfx.h>
#include	<hlib.h>
#include	<ondk.h>
#include	<oapi.h>
#include	<hapi.h>
#include	<apk.h>

#include <prism.h>
//#include 	"ap_hdmi.h"


/**************************************************************************************************/
#define _________Front_Private_Macro_________________________________________________
/**************************************************************************************************/

#define FRONT_BRIGHT_DURATION			10000		// 10초간 high level
#define FRONT_FADE_IN_INTERVAL		100
#define FRONT_FADE_IN_STEP				5
#define FRONT_FADE_OUT_INTERVAL		200
#define FRONT_FADE_OUT_STEP			2

#define FRONT_DIM_LEVEL_HIGH			100
#define FRONT_DIM_LEVEL_MEDIUM		40
#define FRONT_DIM_LEVEL_LOW			20

/**************************************************************************************************/
#define _________Front_Private_Struct_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________Front_Private_Value_________________________________________________
/**************************************************************************************************/

static HBOOL s_bApkReady;
static HUINT32 s_ulCurrentLevel, s_ulTargetLevel, s_ulFadeCount;

/**************************************************************************************************/
#define _________Front_Private_prototype_functions__________________________________________________________
/**************************************************************************************************/

static ONDK_Result_t	local_front_MsgGwmCreate(HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	local_front_MsgGwmDestroy(HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t 	local_front_MsgGwmKeyDown(HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	local_front_MsgGwmTimer(HINT32 p1, HINT32 p2, HINT32 p3);

/**************************************************************************************************/
#define _________Front_Private_functions__________________________________________________________
/**************************************************************************************************/

static ONDK_Result_t local_front_MsgGwmCreate(HINT32 p1, HINT32 p2, HINT32 p3)
{
	s_bApkReady = FALSE;
	s_ulCurrentLevel = 0;
	s_ulTargetLevel = 0;
	s_ulFadeCount = 0;
	return ONDK_MESSAGE_CONSUMED;
}

static ONDK_Result_t local_front_MsgGwmApkInitialized(HINT32 p1, HINT32 p2, HINT32 p3)
{
	s_bApkReady = TRUE;
	s_ulCurrentLevel = FRONT_DIM_LEVEL_HIGH;
	s_ulTargetLevel = FRONT_DIM_LEVEL_MEDIUM;
	ONDK_GWM_SetTimer(GWMPOPUP_TIMER_FRONT_FADE, FRONT_FADE_OUT_INTERVAL);
	return GWM_RET_OK;
}

static ONDK_Result_t local_front_MsgGwmDestroy(HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_KillTimer(GWMPOPUP_TIMER_FRONT);

	return GWM_RET_OK;
}

static ONDK_Result_t local_front_MsgGwmKeyDown(HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (s_bApkReady)
	{
		if(ONDK_GWM_SetTimer(GWMPOPUP_TIMER_FRONT, FRONT_BRIGHT_DURATION) == GWM_RET_LINK_CONFLICT)
		{
			ONDK_GWM_ResetTimer(GWMPOPUP_TIMER_FRONT);
		}

		if (s_ulCurrentLevel != FRONT_DIM_LEVEL_HIGH)
		{
			s_ulTargetLevel = FRONT_DIM_LEVEL_HIGH;
			s_ulFadeCount = 0;
			ONDK_GWM_SetTimer(GWMPOPUP_TIMER_FRONT_FADE, FRONT_FADE_IN_INTERVAL);
		}
	}

	return GWM_RET_OK;
}

//MSG_GWM_TIMER
static ONDK_Result_t local_front_MsgGwmTimer(HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 ulTimerID = p1;
	HUINT32 ulInterval;

	if (ulTimerID == GWMPOPUP_TIMER_FRONT)
	{
		ONDK_GWM_KillTimer(GWMPOPUP_TIMER_FRONT);

		if (s_ulTargetLevel != FRONT_DIM_LEVEL_MEDIUM)
		{
			s_ulTargetLevel = FRONT_DIM_LEVEL_MEDIUM;
			s_ulFadeCount = 0;
			ONDK_GWM_SetTimer(GWMPOPUP_TIMER_FRONT_FADE, FRONT_FADE_OUT_INTERVAL);
		}

		return GWM_MESSAGE_BREAK;
	}
	else if (ulTimerID == GWMPOPUP_TIMER_FRONT_FADE)
	{
		if (s_ulCurrentLevel == s_ulTargetLevel)
		{
			ONDK_GWM_KillTimer(GWMPOPUP_TIMER_FRONT_FADE);
		}
		else
		{
			if (s_ulCurrentLevel < s_ulTargetLevel)
			{
				// increase
				ulInterval = FRONT_FADE_IN_INTERVAL;
				if (s_ulFadeCount == 0)
				{
					// 사용자가 어느정도 인지 할 수 있도록 처음에는 확 올려준다.
					s_ulCurrentLevel += (s_ulTargetLevel - s_ulCurrentLevel) / 3;
				}
				else
				{
					s_ulCurrentLevel += FRONT_FADE_IN_STEP;
					if (s_ulCurrentLevel > s_ulTargetLevel)
						s_ulCurrentLevel = s_ulTargetLevel;
				}
			}
			else
			{
				// decrease
				ulInterval = FRONT_FADE_OUT_INTERVAL;
				s_ulCurrentLevel -= FRONT_FADE_OUT_STEP;
				if (s_ulCurrentLevel < s_ulTargetLevel)
					s_ulCurrentLevel = s_ulTargetLevel;
			}

			s_ulFadeCount++;

			APK_OUTPUT_PANEL_SetPanelDimLevel(eApkPANEL_PRIORITY_MIDDLE, s_ulCurrentLevel);
			ONDK_GWM_SetTimer(GWMPOPUP_TIMER_FRONT_FADE, ulInterval);
		}
	}

	return GWM_RET_OK;
}

/**************************************************************************************************/
#define _________Front_Public_Functions__________________________________________________________
/**************************************************************************************************/


ONDK_Result_t		AP_Front_Proc(HINT32 lMessage, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			lRet = GWM_RET_OK;

	switch(lMessage)
	{
		case MSG_GWM_CREATE:
			lRet = local_front_MsgGwmCreate(p1, p2, p3);
			break;

		case MSG_GWM_APK_INITIALIZED:
			lRet = local_front_MsgGwmApkInitialized(p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			lRet = local_front_MsgGwmTimer(p1, p2, p3);
			break;
		case MSG_GWM_KEYDOWN:
			lRet = local_front_MsgGwmKeyDown(p1, p2, p3);
			break;

		case MSG_GWM_DESTROY:
			lRet = local_front_MsgGwmDestroy(p1, p2, p3);
			break;
	}

	if(lRet != GWM_RET_OK)
	{
		return lRet;
	}

	lRet  =  ONDK_GWM_ProcessMessageDefault(lMessage, hHandle, p1, p2, p3);
	if(lRet == GWM_RET_NO_OBJECT || lRet == GWM_MESSAGE_BREAK)
	{
		lRet = ONDK_MESSAGE_CONSUMED;
	}
	return lRet;

}


/**************************************************************************************************/
#define _________Front_End_Of_File____________________________________________________
/**************************************************************************************************/

