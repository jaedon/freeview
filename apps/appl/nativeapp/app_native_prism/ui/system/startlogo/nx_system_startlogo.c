/**************************************************************
*	@file		ap_startlogo.c
*	Minimized Graphic Library for DirectFB
*
*	http://www.humaxdigital.com
*
*	@data			2012/09/17
*	@author			humax
*	@brief			Minimized Graphic Library for DirectFB
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
#define _________START_LOGO_Private_Include_________________________________________________
/**************************************************************************************************/
#include <nx_main.h>
#include <nx_port.h>
#include <nx_port_system.h>

#include "nx_system_startlogo.h"


/**************************************************************************************************/
#define _________START_LOGO_Private_Macro_________________________________________________
/**************************************************************************************************/

//eMctrlEvent_LIVE_EmptySvc가 삭제되고 view state에 통합됨에 따라 view state를 읽어서 empty svc를 처리하도록 수정한 define
#define TEST_NEW_EMPTY_SVC_MSG
#define STARTLOGO_TIME_SECOND				500

#if defined(STARTLOGO_LOW_QUALITY)
#define STARTLOGO_LOOPCNT					40
#else
#define STARTLOGO_LOOPCNT					10				//	10회 출력, WebApp이 Ready를 주면 자동으로 없어진다.
#endif
/**************************************************************************************************/
#define _________START_LOGO_Private_Struct_________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define _________START_LOGO_Private_Value_________________________________________________
/**************************************************************************************************/

ONDKVideoHandle 	hVideoHandle = NULL;
static HUINT32		ulDrawCount =0;
static HINT32		nCurDrawStrNum = 0;

#define	VFD_STRING_DIVIDE		3
#define	LOGO_LOOP_COUNT			250
static char *s_ucVFDStrings[] = {
	"BOOT",
	"OOT ",
	"OT  ",
	"T   ",
	"    ",
};


/**************************************************************************************************/
#define _________START_LOGO_Private_Prototype_________________________________________________
/**************************************************************************************************/


//static startlogo_Contents_t*	 local_startlogo_GetContents(void);

static ONDK_Result_t	local_startlogo_MsgGwmCreate(HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	local_startlogo_MsgGwmDestroy(HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	local_startlogo_SetObject(void);
static ONDK_Result_t 	local_startlogo_MsgGwmKeyDown(HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	local_startlogo_MsgGwmTimer(HINT32 p1, HINT32 p2, HINT32 p3);

/**************************************************************************************************/
#define _________START_LOGO_Private_functions__________________________________________________________
/**************************************************************************************************/
static void		local_startlogo_SetVFDString(HCHAR * panelString)
{
//	APK_OUTPUT_PANEL_SetPanelDimLevel(eApkPANEL_PRIORITY_MIDDLE, 100);
	NX_PANEL_SetString(panelString, eDxPANEL_ALIGN_CENTER, FALSE);
}


static void		local_startlogo_ClearVFDString(void)
{
	NX_PANEL_ClearString();
	NX_PANEL_SetString(NULL, eDxPANEL_ALIGN_CENTER, TRUE);
}


static ONDK_Result_t local_startlogo_MsgGwmCreate(HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t				hRet = ONDK_RET_OK;
	ulDrawCount					= 0;

	hRet = local_startlogo_SetObject();
	if(hRet != ONDK_RET_OK)
	{
		HxLOG_Warning("error!! local_startlogo_SetObject\r\n");
	}

	return hRet;
}

static ONDK_Result_t local_startlogo_MsgGwmDestroy(HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Trace();
	if(hVideoHandle)
		ONDK_VIDEO_Release(hVideoHandle);
	hVideoHandle = NULL;

	local_startlogo_ClearVFDString();
	ONDK_GWM_KillTimer(GWMPOPUP_TIMER_STARTLOGOID);
	ONDK_WINDOW_Clear(ONDK_GWM_GetWindow(ONDK_UPPER_WINDOW));
	ONDK_WINDOW_Update(ONDK_GWM_GetWindow(ONDK_UPPER_WINDOW),NULL);
	return ONDK_RET_OK;
}

static void local_startlogo_updatewindow(void)
{
	if(ulDrawCount++  == 0)
	{
		ONDK_DRAW_FillRect(ONDK_GWM_GetSurface(ONDK_UPPER_WINDOW),
							ONDK_Rect(0, 0, GWM_SCREEN_WIDTH, GWM_SCREEN_HEIGHT),
							ONDK_Color(0xFF, 0, 0, 0));
	}
	ONDK_WINDOW_Update(ONDK_GWM_GetWindow(ONDK_UPPER_WINDOW),NULL);
}



static ONDK_Result_t 	local_startlogo_SetObject()
{
	ONDK_Rect_t 		stRect;
	ONDK_Result_t		lRet = ONDK_RET_OK;

	ONDK_GWM_APP_SetAppArea(STARTLOGO_X, STARTLOGO_Y, STARTLOGO_W, STARTLOGO_H);
	stRect = ONDK_Rect(STARTLOGO_X,STARTLOGO_Y,STARTLOGO_W,STARTLOGO_H);

	hVideoHandle = ONDK_VIDEO_Create(STARTLOGO_IMG);
	if(hVideoHandle)
	{
		lRet = ONDK_VIDEO_PlayOnece(hVideoHandle,ONDK_GWM_GetSurface(ONDK_UPPER_WINDOW),&stRect,(void *)local_startlogo_updatewindow,NULL);
		ONDK_GWM_SetTimer(GWMPOPUP_TIMER_STARTLOGOID, STARTLOGO_TIME_SECOND);
		ONDK_GWM_SetTimer(0x001, STARTLOGO_TIME_SECOND);
	}
	return lRet;
}

static ONDK_Result_t local_startlogo_MsgGwmKeyDown(HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 nKeyId = p1;
	switch (nKeyId)
	{
	case KEY_STANDBY:
		return ONDK_MESSAGE_NONCONSUMED;
	default:
		return ONDK_MESSAGE_CONSUMED;
	}
}

//MSG_GWM_TIMER
static ONDK_Result_t local_startlogo_MsgGwmTimer(HINT32 p1, HINT32 p2, HINT32 p3)
{
	static HUINT32		loopcnt = 0;
	ONDK_Rect_t			stRect;

	if(p1 == GWMPOPUP_TIMER_STARTLOGOID)
	{
		if(ONDK_VIDEO_IsBusy(hVideoHandle)== FALSE)
		{
			ONDK_VIDEO_Release(hVideoHandle);
			if(++loopcnt > STARTLOGO_LOOPCNT)
			{
				ONDK_GWM_KillTimer(GWMPOPUP_TIMER_STARTLOGOID);
				ONDK_WINDOW_Clear(ONDK_GWM_GetWindow(ONDK_UPPER_WINDOW));
				ONDK_WINDOW_Update(ONDK_GWM_GetWindow(ONDK_UPPER_WINDOW),NULL);

				hVideoHandle = NULL;
				ONDK_GWM_APP_Destroy(0);
				return ONDK_MESSAGE_BREAK;
			}
			else
			{

				if (nCurDrawStrNum < VFD_STRING_COUNT){
					local_startlogo_SetVFDString(s_ucVFDStrings[nCurDrawStrNum]);
					nCurDrawStrNum++;
				}
				else
					nCurDrawStrNum=0;

				stRect = ONDK_Rect(STARTLOGO_X,STARTLOGO_Y,STARTLOGO_W,STARTLOGO_H);

				hVideoHandle = ONDK_VIDEO_Create(STARTLOGO_IMG);
		 		ONDK_VIDEO_PlayOnece(hVideoHandle, ONDK_GWM_GetSurface(ONDK_UPPER_WINDOW),&stRect,(void *)local_startlogo_updatewindow,NULL);
				HxLOG_Print(HxANSI_COLOR_YELLOW("################### Start Logo[%d/%d]##################\n") ,loopcnt,STARTLOGO_LOOPCNT);
			}
		}
		return ONDK_MESSAGE_BREAK;
	}
	return ONDK_MESSAGE_PASS;
}


static ONDK_Result_t local_startlogo_HAPINotifier(HAPIMSG_e eMsg, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	switch (eMsg)
	{
		case eHMSG_MAINAPP_READY:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_PASS;

		case eHMSG_SYSSTATE_CHANGE:
			{
				PxSTATUS_e	ePapiStatus = nParam1;
				if(ePapiStatus == ePAMA_STATUS_Reboot || ePapiStatus == ePAMA_STATUS_Shutdown)
					ONDK_GWM_APP_Destroy(0);

				return ONDK_MESSAGE_PASS;
			}

		default:
			return ONDK_MESSAGE_PASS;
	}
}


/**************************************************************************************************/
#define _________START_LOGO_Public_Functions__________________________________________________________
/**************************************************************************************************/


ONDK_Result_t		NX_SYSTEM_Startlogo_Proc(HINT32 lMessage, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			lRet = ONDK_RET_OK;
	HUINT8					*pszPanelStr = NULL;
	ONDK_Print(HxANSI_COLOR_YELLOW("[%s:%d] Message(%d) : %s, handle: %p p1(0x%x) p2(0x%x) p3(0x%x) \n"), __FUNCTION__, __LINE__, lMessage, ONDK_GWM_GwmMsg2String(lMessage), p1, p2, p3);

	switch(lMessage)
	{
		case MSG_GWM_CREATE:
			// Create 만 하고 animation 은 MSG_GWM_APK_INITIALIZED 받은 후 실행.
			//lRet = local_startlogo_MsgGwmCreate(p1, p2, p3);
			pszPanelStr = NX_PRISM_SPEC_GetPanelString(eNX_PANEL_STR_Starting);
			if (pszPanelStr == NULL)
			{
				HxLOG_Error("Error!! NX_PRISM_SPEC_GetPanelString() - eNX_PANEL_STR_ChList !!!\n");
			}
			NX_PANEL_SetString(pszPanelStr, eDxPANEL_ALIGN_CENTER, FALSE);
			break;
		case MSG_GWM_TIMER:
			lRet = local_startlogo_MsgGwmTimer(p1, p2, p3);
			break;
		case MSG_GWM_APK_INITIALIZED:
			local_startlogo_SetVFDString(s_ucVFDStrings[0]);
			lRet = local_startlogo_MsgGwmCreate(p1, p2, p3);
			ONDK_GWM_ResetTimer(GWMPOPUP_TIMER_STARTLOGOID);
			break;
		case MSG_GWM_HAPI_MESSAGE:
			lRet = local_startlogo_HAPINotifier((HAPIMSG_e)hHandle, p1, p2, p3);
			break;
		case MSG_GWM_KEYDOWN:
			lRet = local_startlogo_MsgGwmKeyDown(p1, p2, p3);
			break;
		case MSG_GWM_KEYUP:
			lRet = ONDK_MESSAGE_CONSUMED;
			break;
		case MSG_GWM_DESTROY:
			lRet = local_startlogo_MsgGwmDestroy(p1, p2, p3);
			break;
	}

	if(lRet != ONDK_RET_OK)
	{
		return lRet;
	}

	return ONDK_GWM_ProcessMessageDefault(lMessage, hHandle, p1, p2, p3);
}



/**************************************************************************************************/
#define _________START_LOGO_End_Of_File____________________________________________________
/**************************************************************************************************/


