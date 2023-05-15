/**************************************************************
*	@file		ondk.c
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

/* ======================================================================== */
/* Include. 																*/
/* ======================================================================== */
#include <ondk.h>
#include <hapi.h>
#include <ondk_gfx.h>

#include "ondk_gwm_internal.h"


#include <ondk_gwm.h>
#include <ondk_gwm_appobj.h>

#include "ondk_gwm_objdef.h"


/* ======================================================================== */
/* Global/Extern Variables.													*/
/* ======================================================================== */


/* ======================================================================== */
/* Private Macros. 															*/
/* ======================================================================== */

#define GMW_MSG_QUE_NAME				"gwmMsgQ"


/* Key가 처리되지 않고 지연될 경우 discard 하는 threshold. ms 단위 */
#define	DEFAULT_KEY_LAG_THRES		300
#define	TIGHT_KEY_LAG_THRES			100



/* ======================================================================== */
/* Private Types.															*/
/* ======================================================================== */


typedef struct tagKeyEvent
{
	HUINT32				ulKeyCode;
	HUINT32 			ulSystemTick;
	ONDK_KEY_Device_t	device;
	void				*keyTypeData;
	HBOOL 				bRepeat;

	HBOOL				bValid;
} KeyEvent_t;

/* ======================================================================== */
/* Global Variables. 														*/
/* ======================================================================== */
//OSD_Screen_t		osdBackBuffer;

typedef struct tagGWM_Mgr_t
{
	HULONG	ulGwmMainTaskId;
	HULONG	ulKeyTaskId;
	HULONG	ulCmdTaskId;

	HBOOL	bStackOderChanged;
	HBOOL	bEnableRedraw;

	HBOOL	bKeyRequestRedraw;
} GWM_Mgr_t;

STATIC GWM_Mgr_t s_stGwmMgr;


ONDK_GWM_Callback_t	g_pfnAppIgnitor;

ONDK_GWM_App_t		g_pstAppRoot;
ONDK_GWM_App_t		g_pstCurrentApp;
void				*gMsgData = NULL;


GWM_SystemColor_t	g_stSystemColor =
{
	//background -	100% 투명한 색.
	0x00000000,

	//btn_hilight, btn_light, btn_face, btn_shadow, btn_darkshadow; -- 객체(Button)의 기본색상.
	0xFFA0ABBD,	0xFF8A93A4, 0xFF5D7496, 0xFF4C6486, 0xFF303747,

	//focus_hilight, focus_light, focus_face, focus_shadow, focus_darkshadow; -- Focus를 얻은 객체의 색상.
	0xFFEDDEA8,	0xFFE8CE6F, 0xFFDEB921, 0xFFDBB106, 0xFF9B7D03,

	//disabled_hilight, disabled_light, disabled_face, disabled_shadow, disabled_darkshadow; -- Focus가 불가능한 객체의 색상.
	0xFF33374A,	0xFF33374A, 0xFF33374A, 0xFF303549, 0xFF303549,

	//frame_hilight, frame_light, frame_face, frame_shadow, frame_darkshadow,  -- 윈도우 바탕이 되는 Frame의 기본 색상.
	0xFF5A6579,	0xFF5A6579, 0xFF414757, 0xFF2F3849, 0xFF2F3849,

	//frame_caption_bar, frame_caption_text, frame_message,  -- Frame의 제목줄, frame 제목 글자색, dialog 내용글자색
	0xFF2A2F42,	0xFF9EB1D2, 0xFFB1B5B6,

	//inactive_hilight, inactive_light, inactive_face, inactive_shadow, inactive_darkshadow, -- 비활성화(Disabled) 객체(Frame, Text message, etc.)의 색상.
	0xFFCBD7E3,	0xFFC6CDDD, 0xFFA0ABBD, 0xFF8A93A4, 0xFF7D8997,

	//inactive_caption_bar, inactive_caption_text, inactive_message;	-- 비활성화 된 제목줄, frame 제목 글자색, dialog 내용글자색
	0xFF6D7486,	0xFFA4ACC1, 0xFF8793A9,

	//default_text, hilighted_text, disabled_text, info_text; 		-- 기본 Text의  색상.
	0xFF365B78,	0xFF442900, 0xFF7A8698, 0xFF2B3278,

	//white, red, green, blue, yellow, gray, darkgray, black;		 -- 주로 사용되는 기본 8 색. (재정의 필요)
	0xFFEFEFEF,	0xFFF00000, 0xFF00F000, 0xFF0000F0,
	/*0xFFEFEF0C,	0xFF7F7F7F, 0xFF3F3F3F, 0xFF0C0C0C, */

};


/* ======================================================================== */
/* Private Constants. 														*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Variables. 														*/
/* ======================================================================== */
STATIC HULONG					s_ulKeyEvtSema = 0;
STATIC HULONG					s_ulGwmSemId = 0;

#define MAX_KEY_PRESSMODE		eKeyPress_MAX /* DOWN, UP, REPEATE */
STATIC KeyEvent_t 				s_keyEvents[MAX_KEY_PRESSMODE];


typedef struct tagONDK_Window
{
	ONDK_GFX_t			gfx;
	HUINT32				id;
	ONDK_Rect_t			rect;
	ONDK_Region_t		region;
	HBOOL            	haveToRedraw;
	ONDK_Region_t    	updateArea;
} ONDK_Window_t;

STATIC ONDK_Window_t	s_stGWMWnd[ONDK_MAX_WINDOW];

#if defined(CONFIG_GWM_O2ANIMATION)
STATIC ONDKSurfaceHandle s_hO2ProcAniSurface = NULL;
#endif

/* ======================================================================== */
#define ____GWM_PROTOTYPE___
/* ======================================================================== */
STATIC GWM_Mgr_t*		local_gwm_GetGWMMgr(void);
STATIC ONDK_Result_t	local_gwm_ProcessMessage(ONDK_MSG_t *msg);
STATIC void				local_gwm_NotifyStackOrderChange(void);
STATIC HERROR 			local_gwmkey_FetchKey(ONDK_MSG_t *msg);
STATIC ONDK_Result_t	local_gwmkey_DefaultKeyProc(HINT32 message, HINT32 handle, HINT32 keycode);
STATIC HBOOL			local_gwmtimer_AppTimerTask(void *arg1);


#define ____GWM_KEY___

void local_gwmkey_ProcessKeyEvents(HUINT32 ulKeyCode, ONDK_KEY_PressMode_t pressMode, ONDK_KEY_Device_t device, void *keyTypeData)
{
	HUINT32 ulMessage;
	HBOOL bRepeat;

	ONDK_Info("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	VK_SEM_Get(s_ulKeyEvtSema);

	switch (pressMode)
	{
	case eKeyPress_DOWN:
		ulMessage = MSG_GWM_KEYDOWN;
		bRepeat = FALSE;
		ONDK_Debug("[%s:%d] MSG_GWM_KEYDOWN\n", __FUNCTION__, __LINE__);
		break;
	case eKeyPress_UP:
		ulMessage = MSG_GWM_KEYUP;
		ONDK_Debug("[%s:%d] MSG_GWM_KEYUP\n", __FUNCTION__, __LINE__);
		bRepeat = FALSE;
		break;
	case eKeyPress_REPEAT:
		ulMessage = MSG_GWM_KEYDOWN;
		ONDK_Debug("[%s:%d] eKeyPress_REPEAT\n", __FUNCTION__, __LINE__);
		bRepeat = TRUE;
		break;
	default:
		GWM_assert(0);
		VK_SEM_Release(s_ulKeyEvtSema);
		ONDK_Error("[%s:%d] default\n", __FUNCTION__, __LINE__);
		return;
	}

	if (s_keyEvents[pressMode].bValid == FALSE)
	{
		// if key event has not been valid before, then send key message
		//ONDK_GWM_PostMessage(NULL, ulMessage, 0, 0, 0, bRepeat);
		ONDK_Debug("\n\n [local_gwmkey_ProcessKeyEvents] send key event(0x%08x, 0x%08x)\n", ulMessage, ulKeyCode);
	}
	else
	{
		ONDK_Debug("\n\n[local_gwmkey_ProcessKeyEvents] overwrite key event(0x%08x, 0x%08x)\n", ulMessage, ulKeyCode);
	}

	if (pressMode == eKeyPress_DOWN)
	{
		// invalidate all key events
		s_keyEvents[eKeyPress_DOWN].bValid = FALSE;
		s_keyEvents[eKeyPress_UP].bValid = FALSE;
		s_keyEvents[eKeyPress_REPEAT].bValid = FALSE;
		ONDK_Debug("[local_gwmkey_ProcessKeyEvents] clear key events\n");
	}

	// set key event
	s_keyEvents[pressMode].ulKeyCode = ulKeyCode;
	s_keyEvents[pressMode].ulSystemTick = VK_TIMER_GetSystemTick();
	s_keyEvents[pressMode].device = device;
	s_keyEvents[pressMode].bRepeat = bRepeat;
	s_keyEvents[pressMode].keyTypeData = keyTypeData;

	// set validate key event
	s_keyEvents[pressMode].bValid = TRUE;
	VK_SEM_Release(s_ulKeyEvtSema);

	ONDK_Info("[%s:%d] (-)\n", __FUNCTION__, __LINE__);
}

//#define	DBG_KEY_FETCH

STATIC HERROR local_gwmkey_FetchKey(ONDK_MSG_t *msg)
{
	ONDK_KEY_PressMode_t pressMode;

	ONDK_Info("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	if (msg->message == MSG_GWM_KEYDOWN)
	{
		if (msg->param3 == FALSE)
		{
			pressMode = eKeyPress_DOWN;
		}
		else
		{
			pressMode = eKeyPress_REPEAT;
		}
	}
	else if (msg->message == MSG_GWM_KEYUP)
	{
		pressMode = eKeyPress_UP;
	}
	else
	{ /* Bypass NOT key messages */
		ONDK_Info("[%s:%d] (-)\n", __FUNCTION__, __LINE__);
		return ERR_OK;
	}

	VK_SEM_Get(s_ulKeyEvtSema);

	if (s_keyEvents[pressMode].bValid == TRUE)
	{
		msg->param1 = s_keyEvents[pressMode].ulKeyCode;
		msg->param2 = s_keyEvents[pressMode].device;
		msg->param3 = s_keyEvents[pressMode].bRepeat;
		msg->handle = (Handle_t)s_keyEvents[pressMode].keyTypeData;
		msg->pvData = NULL;
		msg->pfFreeData = NULL;

		ONDK_Debug("[%s:%d] fetch key event(0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x)\n", __FUNCTION__, __LINE__, msg->message, msg->handle, msg->param1, msg->param2, msg->param3);

		// invalidate key event
		s_keyEvents[pressMode].bValid = FALSE;

		VK_SEM_Release(s_ulKeyEvtSema);


		ONDK_Info("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

		return ERR_OK;
	}
	else
	{
		ONDK_Debug("[%s:%d] failed to fetch key event(0x%08x, 0x%08x)\n", __FUNCTION__, __LINE__, msg->message, msg->param1);
	}

	VK_SEM_Release(s_ulKeyEvtSema);

	ONDK_Info("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ERR_FAIL;
}

STATIC ONDK_Result_t	local_gwmkey_DefaultKeyProc(HINT32 message, HINT32 handle, HINT32 keycode)
{
	ONDK_GWM_GuiObject_t	obj;
	ONDK_Result_t		hresult;
	HINT32				id;
	HINT32				objMsg;

	obj = ONDK_GWM_Obj_GetFocusedObject();

	if (obj == NULL)
		return GWM_MESSAGE_PASS;	//MESSAGE_NOTUSED;

	id = ONDK_GWM_Obj_GetObjectID(obj);
	objMsg = (message == MSG_GWM_KEYDOWN) ? MT_KEYDOWN : MT_KEYUP;

	/* process the key by each object */
	hresult = INT_ONDK_GWM_OBJ_CallObjectProc(objMsg, obj, keycode);
	if (hresult == GWM_MESSAGE_PASS || hresult == GWM_RET_OK)
	{
		ONDK_Result_t		hRetmvFoucus = GWT_RET_MAX;
		GWM_Message_t		hRetmvMessage = MSG_GWM_END;

		/* Focus를 가진 Object가 특별히 처리하지 않은 키라면.. */
		switch(keycode)
		{
			case KEY_ARROWLEFT:
				hRetmvFoucus = INT_ONDK_GWM_OBJ_MoveFocus(obj, MOVE_FOCUS_LEFT);
				hRetmvMessage = MSG_GWM_SCROLLLEFT;
				break;
			case KEY_ARROWRIGHT:
				hRetmvFoucus = INT_ONDK_GWM_OBJ_MoveFocus(obj, (MOVE_FOCUS_RIGHT));
				hRetmvMessage = MSG_GWM_SCROLLRIGHT;
 				break;
			case KEY_ARROWUP:
				hRetmvFoucus = INT_ONDK_GWM_OBJ_MoveFocus(obj, (MOVE_FOCUS_UP));
				hRetmvMessage = MSG_GWM_SCROLLUP;
				break;

			case KEY_ARROWDOWN:
				hRetmvFoucus = INT_ONDK_GWM_OBJ_MoveFocus(obj, (MOVE_FOCUS_DOWN));
				hRetmvMessage = MSG_GWM_SCROLLDOWN;
				break;

			default:
				break;
		}


		switch( hRetmvFoucus )
		{
		case GWT_RET_MAX:
			ONDK_Print("[%s:%d] GWT_RET_MAX\r\n", __FUNCTION__, __LINE__);
			break;

		case ONDK_MESSAGE_CONSUMED:
			ONDK_Print("[%s:%d] ONDK_MESSAGE_CONSUMED\r\n", __FUNCTION__, __LINE__);
			return ONDK_MESSAGE_CONSUMED;

		case GWM_RET_OUT_OF_RANGE:
			ONDK_Print("[%s:%d] GWM_RET_OUT_OF_RANGE\r\n", __FUNCTION__, __LINE__);
			return GWM_MESSAGE_BREAK;

		case GWM_RET_NO_OBJECT:
			if(MSG_GWM_END != hRetmvMessage)
			{
				ONDK_Print("[%s:%d] GWM_RET_NO_OBJECT\r\n", __FUNCTION__, __LINE__);
				ONDK_GWM_APP_Call(g_pstCurrentApp, hRetmvMessage, handle, id, keycode, 0, NULL);
			}
			return ONDK_MESSAGE_CONSUMED;

		default:
			ONDK_Print("[%s:%d] default\r\n", __FUNCTION__, __LINE__);
			return GWM_MESSAGE_BREAK;
		}
	}

	return hresult;
}

STATIC HBOOL			local_gwmkey_IsRedraw(void)
{
	GWM_Mgr_t	*pGWMMgr = local_gwm_GetGWMMgr();
	HBOOL 		isDraw = FALSE;

	if(pGWMMgr->bKeyRequestRedraw)
	{
		isDraw = TRUE;
		pGWMMgr->bKeyRequestRedraw = FALSE;
	}

	return isDraw;
}

STATIC ONDK_Result_t	local_gwmkey_RecvCallback(ONDK_WINDOW_Position_e ePosition,HUINT32 ulKeyCode, ONDK_KEY_PressMode_t pressMode, ONDK_KEY_Device_t device, void *keyTypeData)
{
	ONDK_MSG_t 		msg;
	ONDK_Result_t 	oRet;
	GWM_Mgr_t		*pGWMMgr = local_gwm_GetGWMMgr();
#if defined(CONFIG_DEBUG)
	HULONG			ulTaskId = 0;
#endif

	local_gwmkey_ProcessKeyEvents(ulKeyCode, pressMode, device, keyTypeData);
	HxSTD_MemSet(&msg,0,sizeof(ONDK_MSG_t));

	(pressMode == eKeyPress_DOWN) ? (msg.message = MSG_GWM_KEYDOWN):(msg.message = MSG_GWM_KEYUP);

	msg.param3 = FALSE;
	msg.param4 = (HINT32)ePosition;
	msg.pvData = NULL;
	msg.pfFreeData = NULL;

	HxSEMT_Get(s_ulGwmSemId);
#if defined(CONFIG_DEBUG)
	if(0 == pGWMMgr->ulKeyTaskId)
	{
		VK_TASK_GetCurrentId(&ulTaskId);
		pGWMMgr->ulKeyTaskId = ulTaskId;
	}
#endif

	pGWMMgr->bKeyRequestRedraw = TRUE;
	oRet = local_gwm_ProcessMessage(&msg);

	HxSEMT_Release(s_ulGwmSemId);

	return oRet;
}


void	ONDK_GWM_PostVirtualKey(HUINT32 keyCode)
{
	ONDK_GWM_PostMessage(NULL, MSG_GWM_KEYDOWN, 0, keyCode, 0, 0);
}

#define	___GWM_MESSAGE_FUNCTION__

#if defined(ONDK_DFB_VIDEO_CALLBACK_TO_GWM_THREAD)
typedef void				    (*GWM_CbForExecute_t)(void *pvArgs);
#endif

#define ______GWM_MESSAGE_PRIVATE_FUNC__

STATIC ONDK_Result_t		local_gwm_ProcessMessage(ONDK_MSG_t *msg)
{
	ONDK_GWM_App_t			traverse;
	ONDK_Result_t		hresult, hRetValue = GWM_RET_OK;
	ONDK_GWM_App_t			backupAppList[DEF_GWM_MAX_APPLICATION];
	HINT32					app_count, i;
	HERROR				err;
	ONDK_GWM_App_t			tmpApp;
	HBOOL				isKeyMessage = FALSE;

	// 먼저 Target Application을 결정한다.
	// 		마우스 커서의 이동/클릭등의 메세지와 같이 현재 Focus 와 별도로 처리되면서,
	// 		특정한 Application을 Target으로 하는 메세지의 경우에는, 그 Target Application을 먼저 결정한다.

	ONDK_Info("Start of Posted Message Process(%X)\n", msg->message);


#if defined(ONDK_DFB_VIDEO_CALLBACK_TO_GWM_THREAD)
	if (MSG_APP_EXECUTE_CALLBACK == msg->message)
	{
		GWM_CbForExecute_t		 pfCbFunc = (GWM_CbForExecute_t)msg->param1;
		void					*pvUserData = (void *)msg->param2;

		if (NULL != pfCbFunc)
		{
			pfCbFunc(pvUserData);
		}

		goto END_OF_MSG_PROCESS;
	}
#endif

	err = local_gwmkey_FetchKey(msg);
	if(err != ERR_OK)
	{ /* Key lag exceeded the threshold */
		goto END_OF_MSG_PROCESS;
	}

	switch(msg->message)
	{
	case MSG_GWM_KEYUP:
	case MSG_GWM_KEYDOWN:
		isKeyMessage = TRUE;

		ONDK_Info("[%s:%d] Message : keymessage\n", __FUNCTION__, __LINE__);
		break;

	default:
		isKeyMessage = FALSE;
		break;
	}

	/*
	* target message
	*/
	if (msg->appCallback != NULL)
	{
		tmpApp = ONDK_GWM_APP_Get(msg->appCallback);
		if(FALSE == isKeyMessage)
		{
			if(FALSE == ONDK_GWM_APP_GetState_UIHide(tmpApp))
			{
				hresult = ONDK_GWM_APP_Call(ONDK_GWM_APP_Get(msg->appCallback), msg->message, msg->handle, msg->param1, msg->param2, msg->param3, msg->pvData);
			}
		}

		goto END_OF_MSG_PROCESS;
	}

	/*
	* broadcase message
	*/
	traverse = g_pstAppRoot;
	app_count = 0;
	while (traverse != NULL)
	{
		backupAppList[app_count] = traverse;
		app_count++;
		traverse = ONDK_GWM_APP_GetNextApp(traverse);
	}

	for (i = 0; i < app_count; i++)
	{
		traverse = backupAppList[i];

		ONDK_Debug("[%s:%d] %08X, %08X >>> %s, \n", __FUNCTION__, __LINE__, msg->message, msg->handle, ONDK_GWM_APP_GetAppName(traverse));

		if( isKeyMessage )
		{
			if(ONDK_GWM_APP_GetState_UIHide(traverse))
			{
				/* 하이드 상태에서 key message는 받지 못한다. */
				continue;
			}

			ONDK_Debug("key~~[%s] Current[%d] input[%d],name[%s]\n", (msg->message == MSG_GWM_KEYDOWN)? "KEY_DOWN" : "KEY_UP" ,ONDK_GWM_APP_GetPosition(traverse) ,msg->param4,ONDK_GWM_APP_GetAppName(traverse));
			if(HxSTD_StrEmpty(ONDK_GWM_APP_GetAppName(traverse))== TRUE)
			{
				hresult = ONDK_GWM_APP_Call(traverse, msg->message, msg->handle, msg->param1, msg->param2, msg->param4, NULL);
			}
			else
			{
				if(ONDK_GWM_APP_GetPosition(traverse) == msg->param4)
				{
					hresult = ONDK_GWM_APP_Call(traverse, msg->message, msg->handle, msg->param1, msg->param2, msg->param3, NULL);
				}
				else
				{
					hresult = ONDK_MESSAGE_PASS;
				}
			}

		}
		else
		{
			hresult = ONDK_GWM_APP_Call(traverse, msg->message, msg->handle, msg->param1, msg->param2, msg->param3, msg->pvData);
		}

		switch(hresult)
		{
		case GWM_MESSAGE_BREAK:
		case GWM_RET_NO_OBJECT:
			{
				goto END_OF_MSG_PROCESS;
			}
			break;

		case ONDK_MESSAGE_NONCONSUMED:
			{
				ONDK_Print(HxANSI_COLOR_YELLOW("GWM MSG Porcess() met GWM_MESSAGE_BREAK, the app at msg(0x%x) is %s.\033[00m\n"), msg->message, INT_ONDK_GWM_APP_GetAppName(traverse));
				if(isKeyMessage == TRUE)
				{
					hRetValue = ONDK_MESSAGE_NONCONSUMED;
					goto END_OF_MSG_PROCESS;
				}
				goto END_OF_MSG_PROCESS;
			}
			break;

		case ONDK_MESSAGE_CONSUMED:
			{
				ONDK_Print(HxANSI_COLOR_YELLOW("GWM MSG Porcess() met ONDK_MESSAGE_CONSUMED, the app at msg(0x%x) is %s.\033[00m\n"), msg->message, INT_ONDK_GWM_APP_GetAppName(traverse));
				if(isKeyMessage == TRUE)
				{
					hRetValue = ONDK_MESSAGE_CONSUMED;
					goto END_OF_MSG_PROCESS;
				}
				goto END_OF_MSG_PROCESS;
			}
			break;

		default:
			break;
		}
	}	// end of for

	if(isKeyMessage == TRUE)
	{
		ONDK_Info(HxANSI_COLOR_YELLOW("GWM MSG Porcess() met ONDK_MESSAGE_PASS, the app at msg(0x%x) is %s.\033[00m\n"), msg->message, INT_ONDK_GWM_APP_GetAppName(traverse));
		hRetValue = ONDK_MESSAGE_NONCONSUMED;
		goto END_OF_MSG_PROCESS;
	}

END_OF_MSG_PROCESS:
	ONDK_Info("End of Posted Message Process\n");

	if ((NULL != msg->pvData) && (NULL != msg->pfFreeData))
	{
		msg->pfFreeData(msg->pvData);
	}

	return hRetValue;
}


/* MW에서 AP로 메시지를 전달하는 callback에 등록할 함수 */
STATIC void local_gwm_PostMsgToAp(HINT32 message, Handle_t hAct, HINT32 param1, HINT32 param2, HINT32 param3)
{
	ONDK_MSG_t	msg;
	HINT32 		err;

	// 외부의 Task 로 동작하는 Service Module 로 부터 GWM 으로 메세지를
	// 전송하는 함수. GWM이 바빠서 못 받으면 기다려 주는 센스...
	msg.appCallback = NULL;
	msg.message = message;
	msg.handle = hAct;
	msg.param1 = param1;
	msg.param2 = param2;
	msg.param3 = param3;
	msg.pvData = NULL;
	msg.pfFreeData = NULL;

	err = HLIB_MSG_Send(ONDK_GWM_RECV_GetMessageQId(), &msg, sizeof(ONDK_MSG_t));

	GWM_assert(err == VK_OK);
}

STATIC void local_gwm_PostDataToAp(HINT32 message, Handle_t hAct, HINT32 param1, HINT32 param2, HINT32 param3, void *pvData, void (*pfFreeData)(void *))
{
	ONDK_MSG_t	msg;
	HINT32 		err;

	// 외부의 Task 로 동작하는 Service Module 로 부터 GWM 으로 메세지를
	// 전송하는 함수. GWM이 바빠서 못 받으면 기다려 주는 센스...
	msg.appCallback = NULL;
	msg.message = message;
	msg.handle = hAct;
	msg.param1 = param1;
	msg.param2 = param2;
	msg.param3 = param3;
	msg.pvData = pvData;
	msg.pfFreeData = pfFreeData;

	err = HLIB_MSG_Send(ONDK_GWM_RECV_GetMessageQId(), &msg, sizeof(ONDK_MSG_t));

	GWM_assert(err == VK_OK);
}

#define ______GWM_MESSAGE_PUBLIC_FUNC__

ONDK_Result_t ONDK_GWM_SendMessage(ONDK_GWM_Callback_t appCallback, HINT32 message,
						Handle_t hAct, HINT32 param1, HINT32 param2, HINT32 param3)
{
//	unsigned long		taskId;
	ONDK_GWM_App_t			traverse, next_application;
	ONDK_Result_t		hresult = GWM_RET_OK;

	//ONDK_Assert(ONDK_GWM_IsGwmTask() == TRUE);


	if (appCallback == NULL)
	{
		ONDK_Assert(message != MSG_GWM_DRAW);

		GWM_DBG_Print(DBGMSG_TRACE, ("\t\tStart of Sended Message Process(%X)\n", message));

		traverse = g_pstAppRoot;
		while (traverse != NULL)
		{
			/*
			 * ONDK_GWM_APP_Destroy 때문에, next_application을 미리 얻는다.
			 */
			next_application = ONDK_GWM_APP_GetNextApp(traverse);

			GWM_DBG_Print(DBGMSG_TRACE, ("\t\tMSG:%08X, %08X >>> %s, \n", message, hAct, ONDK_GWM_APP_GetAppName(traverse)));
			hresult = ONDK_GWM_APP_Call(traverse, message, hAct, param1, param2, param3, NULL);
			if (hresult == GWM_MESSAGE_BREAK)
			{

				GWM_DBG_Print(DBGMSG_TRACE, ("\t\t\033[01;20m GWM MSG Porcess() met GWM_MESSAGE_BREAK, the app at msg(0x%x) is %s.\033[00m\n", message, INT_ONDK_GWM_APP_GetAppName(traverse)));
				break;
			}
			traverse = next_application;
		}
		GWM_DBG_Print(DBGMSG_TRACE, ("\t\tEnd of Sended Message Process\n"));
		return hresult;
	}
	else
	{	/* transfer the message directly */
		return ONDK_GWM_APP_Call(ONDK_GWM_APP_Get(appCallback), message, hAct, param1, param2, param3, NULL);
	}
}


ONDK_Result_t ONDK_GWM_PostMessage(ONDK_GWM_Callback_t appCallback, HINT32 message,
						Handle_t hAct, HINT32 param1, HINT32 param2, HINT32 param3)
{
	ONDK_MSG_t		msg;
	HINT32			err;

	// GWM task 내부에서 동작하는 application으로 부터 GWM 으로 메세지를
	// 전송하는 함수. 스스로 dead lock 걸리지 않으려면 timeout으로 동작해야 한다.
	msg.appCallback = appCallback;
	msg.message = message;
	msg.handle = hAct;
	msg.param1 = param1;
	msg.param2 = param2;
	msg.param3 = param3;
	msg.pvData = NULL;
	msg.pfFreeData = NULL;

	err = HLIB_MSG_SendTimeout(ONDK_GWM_RECV_GetMessageQId(), &msg, sizeof(ONDK_MSG_t), 0);

	GWM_assert(err == VK_OK);

	return	err;
}

ONDK_Result_t ONDK_GWM_PostData(ONDK_GWM_Callback_t appCallback, HINT32 message,
						Handle_t hAct, HINT32 param1, HINT32 param2, HINT32 param3, void *pvData, void (*pfFreeData)(void *))
{
	ONDK_MSG_t		msg;
	HINT32			err;

	// GWM task 내부에서 동작하는 application으로 부터 GWM 으로 메세지를
	// 전송하는 함수. 스스로 dead lock 걸리지 않으려면 timeout으로 동작해야 한다.
	msg.appCallback = appCallback;
	msg.message = message;
	msg.handle = hAct;
	msg.param1 = param1;
	msg.param2 = param2;
	msg.param3 = param3;
	msg.pvData = pvData;
	msg.pfFreeData = pfFreeData;

	err = HLIB_MSG_SendTimeout(ONDK_GWM_RECV_GetMessageQId(), &msg, sizeof(ONDK_MSG_t), 0);

	GWM_assert(err == VK_OK);

	return	err;
}

ONDK_Result_t		ONDK_GWM_ProcessMessageDefault(HINT32 message, Handle_t hAct,
												HINT32 param1, HINT32 param2, HINT32 param3)
{
	// 현재는 사용되지 않는 Parameter 들.. 다른 메세지에서 사용할 수도 있을 것이다.
	// Warning 메세지를 없애기 위해서,
	param2 = param2;
	param3 = param3;

	switch (message)
	{
		case MSG_GWM_O2ANIMATION_END:
			if(DEF_O2ANI_EFFECT_IGNORE_KEYPROCESS != param1)
			{
				local_gwmkey_DefaultKeyProc(MSG_GWM_KEYDOWN, hAct, param1);
				return local_gwmkey_DefaultKeyProc(MSG_GWM_KEYUP, hAct, param1);
			}
			break;

		case MSG_GWM_KEYDOWN:
		case MSG_GWM_KEYUP:
			return local_gwmkey_DefaultKeyProc(message, hAct, param1);

/*		다른 App 로 전달되지 않을 메세지들.
		case MSG_GWM_CREATE:
		case MSG_GWM_DESTROY:
		case MSG_GWM_DRAW:
		case MSG_GWM_CLICKED:
		case MSG_GWM_TIMER:
		case MSG_GWM_SCROLLLEFT:
		case MSG_GWM_SCROLLRIGHT:
		case MSG_GWM_SCROLLUP:
		case MSG_GWM_SCROLLDOWN:
			return	GWM_MESSAGE_BREAK;
*/
		case LIST_UP_NOMORE:
		{
			/*	LIST_OBJECT	list;
			HINT32			total, start, curr, select;
			OSD_Rect_t	rect;

			list = (LIST_OBJECT)GWM_APP_GetObject(Param1);
			GWM_GetListProperty(list, &total, &start, &curr, &select);
			GWM_SetListCurrentIndex(Param1, total-1);
			GWM_APP_Call(g_pstCurrentApp, MSG_GWM_LIST_CHANGED, handle, Param1, total - 1, (HINT32)list);
			INT_ONDK_GWM_OBJ_GetObjectRect((GWM_GuiObject_t)GWM_APP_GetObject(Param1), &rect);
			GWM_APP_InvalidateRect(rect.x, rect.y, rect.w, rect.h);	*/
			return GWM_MESSAGE_BREAK;
		}
		case LIST_DOWN_NOMORE:
		{
			/*	LIST_OBJECT	list;
			//HINT32			total, start, curr;
			OSD_Rect_t	rect;

			list = (LIST_OBJECT)GWM_APP_GetObject(Param1);
			GWM_SetListCurrentIndex(Param1, 0);
			GWM_APP_Call(g_pstCurrentApp, MSG_GWM_LIST_CHANGED, handle, Param1, 0, (HINT32)list);
			INT_ONDK_GWM_OBJ_GetObjectRect((GWM_GuiObject_t)GWM_APP_GetObject(Param1), &rect);
			GWM_APP_InvalidateRect(rect.x, rect.y, rect.w, rect.h);	*/
			return GWM_MESSAGE_BREAK;
		}

		/* 외부로부터의 강제 close 요청. 각 app에서 가로채서 처리하지 않으면 이 함수가 불리고
		결국은 아래와 같이 자살하는 동작을 수행한다. */
		case MSG_GWM_QUIT_UI :
			ONDK_GWM_APP_Destroy(0);
#if  0
			/* Mediaset spec 에서 Menu 진입한 상태에서 Recharge/OPPV를 받으면 메시지를 예약했다가 Menu를 빠져 나올 때,
			Recharge/OPPV 메시지를 띄워야 한다. 그러나 현재 Menu에 진입했는지를 알 수 있는 방법이 없기 때문에, s_SettingTitle_CurrentDepth를
			이용하려고 하는데, MSG_GWM_QUIT_UI에 의해 Menu를 빠져 나오는 경우에는 s_SettingTitle_CurrentDepth가 초기화 되지 않고 있어서
			아래 함수를 이용하여 초기화 시켜주자.
			*/
			Settings_ClearWindowTitle();
#endif
			return GWM_MESSAGE_PASS;	//MESSAGE_NOTUSED;

		/* Standby로 진입한다는 통보. 각 app에서 가로채서 처리하지 않으면 이 함수가 불리고
		결국은 아래와 같이 자살하는 동작을 수행한다. */
		case MSG_GWM_GO_STANDBY:
			HxLOG_Print("\t************** Destroy [%s] application by MSG_GWM_GO_STANDBY\n", ONDK_GWM_APP_GetAppName(g_pstCurrentApp));
			ONDK_GWM_APP_Destroy(0);
			return GWM_MESSAGE_PASS;	//MESSAGE_NOTUSED;

		default:
			break;
	}
	return GWM_MESSAGE_PASS;	//MESSAGE_NOTUSED;
}

/**********************************************************************************************************/
#define	___GWM_MAIN_FUCTION__
/**********************************************************************************************************/

#define ______GWM_MAIN_PRIVATE_FUNC__
STATIC GWM_Mgr_t	*local_gwm_GetGWMMgr(void)
{
	return &s_stGwmMgr;
}

STATIC ONDK_Window_t *local_gwm_GetWindow(ONDK_WINDOW_Position_e ePosition)
{
	if(ONDK_EMPTY_WINDOW < ePosition && ePosition < ONDK_MAX_WINDOW )
	{
		return &s_stGWMWnd[ePosition];
	}
	return NULL;
}

STATIC void			local_gwm_NotifyStackOrderChange(void)
{
	ONDK_GWM_App_t		reverseApp[DEF_GWM_MAX_APPLICATION];
	ONDK_GWM_App_t		app = NULL;
	HINT32				count = 0;

	app = (ONDK_GWM_App_t)g_pstAppRoot;
	while((app != NULL)
		&& (/*NOT*/!((count < 0) || (count >= DEF_GWM_MAX_APPLICATION))))		/* 배열 boundary를 넘지 않기 위해 index check */
	{
		reverseApp[count] = app;
		count++;
        //app = (ONDK_GWM_App_t)(((HxList_t*)app)->next);
        app = (ONDK_GWM_App_t)gwm_lnklst_GetNext((LINK_OBJECT)app);
	}

	while (count > 0)
	{
		//HxLOG_Print("\t##### Notify Stack Change to [%s]\n", GWM_APP_GetAppName(reverseApp[count-1]));
		ONDK_GWM_APP_Call(reverseApp[count-1], MSG_GWM_STACK_CHANGED, 0, 0, 0, 0, NULL);
		count--;
	}
}

STATIC void local_gwm_Draw(ONDK_WINDOW_Position_e ePosition)
{
	GWM_Mgr_t		*pGWMMgr = local_gwm_GetGWMMgr();

	if(pGWMMgr->bEnableRedraw)
	{
		if(INT_ONDK_GWM_GetHaveToRedraw(ePosition))
		{
			INT_ONDK_GWM_RedrawAllApp(ePosition);
			INT_ONDK_GWM_SetHaveToRedraw(ePosition,FALSE);
		}
	}
}

STATIC void local_gwm_Configure(void)
{
	HxSTD_MemSet(&s_stGwmMgr, 0, sizeof(GWM_Mgr_t));
	s_stGwmMgr.bEnableRedraw = TRUE;
}

STATIC void local_gwm_GfxInit(void)
{
	ONDK_Window_t 	*pWnd = NULL;
	HINT32			i = 0;
	HUINT32			ulWndId = 0;

	for(i=((HINT32)ONDK_EMPTY_WINDOW+1); i < (HINT32)ONDK_MAX_WINDOW; i++)
	{
		pWnd = local_gwm_GetWindow((ONDK_WINDOW_Position_e)i);
		if(NULL == pWnd)
		{
			continue;
		}
		switch((ONDK_WINDOW_Position_e)i)
		{
		case ONDK_UPPER_WINDOW:
			ulWndId = ONDK_UPPER_WINDOWID;
			break;

		case ONDK_LOWER_WINDOW:
			ulWndId = ONDK_LOWER_WINDOWID;
			break;

		default:
			ulWndId = eAWINDOW_NOT_HANDLED;
			break;
		}

		if(eAWINDOW_NOT_HANDLED == ulWndId)
		{
			continue;
		}

		pWnd->id	= ulWndId;
		pWnd->rect	= ONDK_Rect(0, 0, GWM_SCREEN_WIDTH, GWM_SCREEN_HEIGHT);
		pWnd->region= ONDK_Region(0, 0, (GWM_SCREEN_WIDTH-1), (GWM_SCREEN_HEIGHT-1));
		pWnd->gfx	= ONDK_DFBUTIL_GFX_Create(pWnd->id, pWnd->rect);
		INT_ONDK_GWM_SetClip(pWnd->gfx.surface, pWnd->rect.x, pWnd->rect.y, pWnd->rect.w - 1, pWnd->rect.h - 1);
	}
}

STATIC void local_gwm_ResourceSettings(void)
{
	ONDKSTR_Settings_t 		stStrSettings;
	ONDK_DFB_IMG_Settings_t stImgSettings;

	HxSTD_MemSet(&stStrSettings, 0, sizeof(ONDKSTR_Settings_t));
	stStrSettings.cbGetProcId	= (DEF_CB_GetProcId)ONDK_GWM_APP_GetThis;
	stStrSettings.nProcMaxCount= DEF_GWM_MAX_APPLICATION;
	ONDK_STRINGS_Settings(&stStrSettings);

	/* image */
	HxSTD_MemSet(&stImgSettings, 0, sizeof(ONDK_DFB_IMG_Settings_t));
	stImgSettings.cbGetProcId	= (DEF_CB_GetProcId)ONDK_GWM_APP_GetThis;
	stImgSettings.nProcMaxCount= DEF_GWM_MAX_APPLICATION;
	ONDK_IMG_Settings(&stImgSettings);

}

ONDKSurfaceHandle	local_gwmo2ani_GetProcAniSurface(void)
{
#if defined(CONFIG_GWM_O2ANIMATION)
	if (s_hO2ProcAniSurface == NULL)
	{
		s_hO2ProcAniSurface = ONDK_SURFACE_Create(GWM_SCREEN_WIDTH, GWM_SCREEN_HEIGHT, "ARGB8888");
		ONDK_SURFACE_Clear(s_hO2ProcAniSurface);
	}
	return s_hO2ProcAniSurface;
#else
	return NULL;
#endif
}

STATIC HBOOL	local_gwmo2ani_AppAnimationTask(void *arg1)
{
	(void)arg1;
#if defined(CONFIG_GWM_O2ANIMATION)
	return	INT_ONDK_GWM_O2ANI_IsAppAnimation();
#else
	return FALSE;
#endif
}


#define ______GWM_MAIN_INTERNAL_FUNC__

HBOOL INT_ONDK_GWM_DrawCheck(void)
{
	HBOOL timerRedraw = FALSE;
	HBOOL aniRedraw = FALSE;
	HBOOL keyRedraw = FALSE;
	HBOOL need2Draw = FALSE;

	aniRedraw = local_gwmo2ani_AppAnimationTask(NULL);
	timerRedraw = local_gwmtimer_AppTimerTask(NULL);
	keyRedraw = local_gwmkey_IsRedraw();
	if(aniRedraw || timerRedraw || keyRedraw)
	{
		need2Draw = TRUE;
	}

	return need2Draw;
}

void INT_ONDK_GWM_LOOP(void)
{
	HINT32 i = 0;

/* If stack order was changed, notify it to all the proc's */
	if(INT_ONDK_GWM_GetStackOderChanged() == TRUE)
	{
		local_gwm_NotifyStackOrderChange();
		INT_ONDK_GWM_SetStackOderChanged(FALSE);
	}


	/* Execute drawing per every loop */
	for(i=(ONDK_EMPTY_WINDOW+1); i < ONDK_MAX_WINDOW; i++)
	{
		local_gwm_Draw(i);
	}

	if (g_pstAppRoot == NULL)
	{
		ONDK_Error("quit gwm task for no app to handle\n");
		return;
	}
}


void INT_ONDK_GWM_GetMessage(void *msg)
{
	ONDK_Result_t		hresult;
	ONDK_MSG_t			*pstMsg = (ONDK_MSG_t*)msg;

	GWM_assert(pstMsg);

	HxSEMT_Get(s_ulGwmSemId);
	hresult = local_gwm_ProcessMessage(pstMsg);
	HxSEMT_Release(s_ulGwmSemId);

	//ONDK_Free(msg);
	if (hresult != GWM_RET_OK)
	{
		ONDK_Error("quit gwm task for local_gwm_ProcessMessage()\n");
		return;
	}

}

#define ______GWM_MAIN_PUBLIC_FUNC__

ONDKWindowHandle ONDK_GWM_GetWindow(ONDK_WINDOW_Position_e ePosition)
{
	ONDK_Window_t *pWnd = local_gwm_GetWindow(ePosition);

	if(pWnd)
	{
		return pWnd->gfx.window;
	}

	return NULL;
}

ONDKSurfaceHandle ONDK_GWM_GetProcAniSurface(void)
{
	return local_gwmo2ani_GetProcAniSurface();
}

ONDKSurfaceHandle ONDK_GWM_GetSurface(ONDK_WINDOW_Position_e ePosition)
{
	ONDK_Window_t *pWnd = local_gwm_GetWindow(ePosition);

#if defined(CONFIG_GWM_O2ANIMATION)
	if (INT_ONDK_GWM_APP_IsO2AniProcAnimation())
		return local_gwmo2ani_GetProcAniSurface();
#endif

	if(pWnd)
	{
		return pWnd->gfx.surface;
	}

	return NULL;
}

ONDK_WINDOW_Position_e ONDK_GWM_GetCurPosition(void)
{
	return ONDK_GWM_APP_GetPosition(g_pstCurrentApp);
}

ONDKSurfaceHandle ONDK_GWM_GetCurSurface(void)
{
	ONDK_Window_t *pWnd = local_gwm_GetWindow(ONDK_GWM_APP_GetPosition(g_pstCurrentApp));

#if defined(CONFIG_GWM_O2ANIMATION)
	if (INT_ONDK_GWM_APP_IsO2AniProcAnimation())
		return local_gwmo2ani_GetProcAniSurface();
#endif

	if(pWnd)
	{
		return pWnd->gfx.surface;
	}

	return NULL;
}


/** @brief setup OSD screens */
/* Basically, GWM has 1 main OSD and 1 background screens. */
/* Some screen or plane which has other purporse rather than GWM should be manipulated by the multiplane handler. */
ONDK_Result_t	ONDK_GWM_Init(void)
{
	HINT32	ret = 0;
	HINT32	i = 0;

	if(0 != s_ulGwmSemId)
	{
		ONDK_Print("[%s:%d] duplication init\r\n", __FUNCTION__, __LINE__);
		return ONDK_RET_OK;
	}

	ONDK_KEY_Active(FALSE);

	local_gwm_Configure();
	local_gwm_GfxInit();

	INT_ONDK_GWM_TIMER_Init();
	INT_ONDK_GWM_O2ANI_Init();
	INT_ONDK_GWM_APP_InitApps();
	INT_ONDK_GWM_OBJ_InitObjects();

	/*
	 * init application link
	 */
	g_pstAppRoot = g_pstCurrentApp = NULL;
	for(i=(ONDK_EMPTY_WINDOW+1); i < ONDK_MAX_WINDOW; i++)
	{
		INT_ONDK_GWM_SetHaveToRedraw(ONDK_UPPER_WINDOW,FALSE);
	}

	ret = HxSEMT_Create((HUINT32*)&s_ulGwmSemId,"ulGwmSemId", 0);
	if(ret != 0)
	{
		return ONDK_RET_FAIL;
	}

	/*
	 * init Message Receiver
	 */
	ONDK_GWM_RECV_Init(s_ulGwmSemId);

	return ONDK_RET_OK;
}


ONDK_Result_t	ONDK_GWM_Destroy(void)
{
	ONDK_Window_t 	*pWnd = NULL;
	HINT32			i = 0;

	for(i=((HINT32)ONDK_EMPTY_WINDOW+1); i < (HINT32)ONDK_MAX_WINDOW; i++)
	{
		pWnd = local_gwm_GetWindow((ONDK_WINDOW_Position_e)i);
		if(NULL == pWnd)
		{
			continue;
		}

		ONDK_SURFACE_Destroy(pWnd->gfx.surface);
		ONDK_WINDOW_Destroy(pWnd->gfx.window);
		HxSTD_memset(pWnd, 0, sizeof(ONDK_Window_t));
 	}

	return ONDK_RET_OK;
}

void 		ONDK_GWM_Start(ONDK_GWM_Callback_t pfnAppIgnitor)
{
	HINT32			err;
	ONDK_Window_t 	*pWnd = NULL;
	HINT32			i = 0;
	GWM_Mgr_t		*pGWMMgr = local_gwm_GetGWMMgr();


	CHECK_BOOT_TIME("ONDK_GWM_Start - Start");

	if(pfnAppIgnitor == NULL)
	{
		HxLOG_Print("\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n\n");
		HxLOG_Print("\t\tFatal Error !! You should register initial application !!\n\n");
		HxLOG_Print("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n\n");
	}

	g_pfnAppIgnitor = pfnAppIgnitor;

	/* MW에서 AP로 메시지를 전달하는 함수를 등록함. */
	INT_ONDK_GWM_RECV_RegisterPostMsgToApCallBack(local_gwm_PostMsgToAp);
	INT_ONDK_GWM_RECV_RegisterPostDataToApCallBack(local_gwm_PostDataToAp);

	local_gwm_ResourceSettings();

#if 1 // MOONSG : Direct 1.7.1 적용시 surface를 creat하는 task와 surface clear하는 task일치 시키도록 함.
	for(i=(ONDK_EMPTY_WINDOW+1); i < ONDK_MAX_WINDOW; i++)
	{
		INT_ONDK_GWM_RedrawAllApp(i);
	}
#endif

	err = VK_SEM_Create(&s_ulKeyEvtSema, "KeyEvtSema", VK_SUSPENDTYPE_PRIORITY);
	if (err != VK_OK)
	{
		GWM_DBG_Print(DBGMSG_ERROR, ("KeyEvtSema creation fail err = 0x%x\n", err));
		return;
	}

	/* create and start GWM task */
	err = VK_TASK_Create(INT_ONDK_GWM_MainTask, GWM_TASK_PRIORITY, GWM_TASK_STACK_SIZE,
						ONDK_GWM_MAIN_TASK_NAME, NULL, &pGWMMgr->ulGwmMainTaskId, 0);
	if (err != VK_OK)
	{
		GWM_DBG_Print(DBGMSG_ERROR, ("GWM task creation fail err = 0x%x\n", err));
		return;
	}

	VK_TASK_Start(pGWMMgr->ulGwmMainTaskId);

	/* bind RCU keycallback */
	for(i=((HINT32)ONDK_EMPTY_WINDOW+1); i < (HINT32)ONDK_MAX_WINDOW; i++)
	{
		pWnd = local_gwm_GetWindow((ONDK_WINDOW_Position_e)i);
		if(NULL == pWnd)
		{
			continue;
		}

		INT_ONDK_GWM_RECV_RegisterKeyCallback(pWnd->gfx.window, local_gwmkey_RecvCallback);
	}

	CHECK_BOOT_TIME("INT_ONDK_GWM_RECV_RegisterKeyCallback");

	CHECK_BOOT_TIME("ONDK_GWM_Start - End");
}

extern HUINT32			g_ulGwmTaskId;

HBOOL 		ONDK_GWM_IsGwmTask(void)
{
		HxLOG_Print("KeyTask:%d / GwmTask:%d / CurTask : %d \n", ONDK_KEY_GetTaskId(),	g_ulGwmTaskId, HLIB_STD_GetTID());
#if defined(CONFIG_APUI_NATIVE_APP)
		return (HLIB_STD_GetTID() == ONDK_KEY_GetTaskId() || HLIB_STD_GetTID() == g_ulGwmTaskId) ? TRUE : FALSE;
	//	return (HLIB_STD_GetTID() == g_ulGwmTaskId) ? TRUE : FALSE;
#else
		return TRUE;
#endif
}

void *		ONDK_GWM_GetData (void)
{
	return gMsgData;
}

/**********************************************************************************************************/
#define ____OSD_REDRAW_CHECK___
/**********************************************************************************************************/

const HBOOL  INT_ONDK_GWM_GetHaveToRedraw(ONDK_WINDOW_Position_e ePosition)
{
	ONDK_Window_t *pWnd = local_gwm_GetWindow(ePosition);

	if(pWnd)
	{
		return pWnd->haveToRedraw;
	}

	return FALSE;
}
void     INT_ONDK_GWM_SetHaveToRedraw(ONDK_WINDOW_Position_e ePosition,HBOOL value)
{
	ONDK_Window_t *pWnd = local_gwm_GetWindow(ePosition);

	if(pWnd)
	{
		 pWnd->haveToRedraw = value;
	}
}


const HBOOL    INT_ONDK_GWM_GetStackOderChanged(void)
{
	GWM_Mgr_t		*pGWMMgr = local_gwm_GetGWMMgr();

    return pGWMMgr->bStackOderChanged;
}

void     INT_ONDK_GWM_SetStackOderChanged(HBOOL value)
{
	GWM_Mgr_t		*pGWMMgr = local_gwm_GetGWMMgr();

    pGWMMgr->bStackOderChanged = value;
}

const ONDK_Region_t  INT_ONDK_GWM_GetUpdateArea(ONDK_WINDOW_Position_e ePosition)
{
	ONDK_Window_t *pWnd = local_gwm_GetWindow(ePosition);

	if(pWnd)
	{
		return pWnd->updateArea;
	}

    return ONDK_Region(0,0,GWM_SCREEN_WIDTH,GWM_SCREEN_HEIGHT);
}


void    INT_ONDK_GWM_SetUpdateArea(ONDK_WINDOW_Position_e ePosition,ONDK_Region_t value)
{
	ONDK_Window_t *pWnd = local_gwm_GetWindow(ePosition);

	if(pWnd)
	{
		pWnd->updateArea = ONDK_IntersectRegion(&value, &pWnd->region);
	}
}

void		INT_ONDK_GWM_RedrawAllApp(ONDK_WINDOW_Position_e ePosition)
{
	ONDK_GWM_App_t			traverse = 0;
	ONDK_GWM_App_t			reverseAppList[DEF_GWM_MAX_APPLICATION] = {0, };
	ONDK_Region_t			update_area = {0, };
	ONDK_Region_t			screen_area = {0, 0, GWM_SCREEN_WIDTH-1, GWM_SCREEN_HEIGHT-1};
	HINT32				    app_count = 0;
	ONDKSurfaceHandle		*pOsdScreen= NULL;

	#if defined(CONFIG_DEBUG)
	HUINT32		startTick = 0, endTick = 0;

		startTick = VK_TIMER_GetSystemTick();
	#endif


	//pOsdScreen = ONDK_GWM_GetSurface();
 	update_area = INT_ONDK_GWM_GetUpdateArea(ePosition);
	if (ONDK_IsEmptyRegion(&update_area))
	{
		update_area.x1 = 0;
		update_area.y1 = 0;
		update_area.x2 = GWM_SCREEN_WIDTH - 1;
		update_area.y2 = GWM_SCREEN_HEIGHT - 1;
        INT_ONDK_GWM_SetUpdateArea(ePosition,update_area);
	}

	// Application을 역순으로 저장.
	traverse = g_pstAppRoot;

	app_count = 0;
	while (traverse != NULL)
	{
		reverseAppList[app_count] = traverse;
		traverse = ONDK_GWM_APP_GetNextApp(traverse);
		app_count++;
	}

	pOsdScreen = ONDK_GWM_GetSurface(ePosition);

	update_area = ONDK_IntersectRegion(&screen_area, &update_area);
	ONDK_GWM_GFX_ClearScreenByRegion(pOsdScreen, &update_area);

	/* 역순으로 저장된 Application을 traversiong하면서 drawing */
	while (app_count > 0)
	{
		app_count--;

		if(ONDK_GWM_APP_GetState_UIHide(reverseAppList[app_count]) == TRUE)
		{
			continue;
		}
		// draw each app and accumulate merged area
		INT_ONDK_GWM_APP_Redraw(reverseAppList[app_count], &update_area);
	}

	INT_ONDK_GWM_SetClip(ONDK_GWM_GetSurface(ePosition), update_area.x1, update_area.y1, update_area.x2, update_area.y2);
	ONDK_GWM_GFX_UpdateScreen(ePosition, update_area);

	ONDK_Print(HxANSI_COLOR_PURPLE("[%s:%d] [%s]-Update Region(%d, %d, %d, %d)\n"), __FUNCTION__, __LINE__,(ePosition==ONDK_UPPER_WINDOW)? "UPPER" :"LOWER" ,update_area.x1,update_area.y1,update_area.x2, update_area.y2);

	//HxSTD_MemSet(&temp, 0x00, sizeof(ONDK_Region_t));
    INT_ONDK_GWM_SetUpdateArea(ePosition,ONDK_Region(0,0,0,0));

	#if defined(CONFIG_DEBUG)
		endTick = VK_TIMER_GetSystemTick();

		if((startTick + 120) < endTick)
		{
			ONDK_Error(HxANSI_COLOR_PURPLE("[%s:%d] [%s]-Update Region(%d, %d, %d, %d)\n"), __FUNCTION__, __LINE__,(ePosition==ONDK_UPPER_WINDOW)? "UPPER" :"LOWER" ,update_area.x1,update_area.y1,update_area.x2, update_area.y2);
			ONDK_Error("\r\n");
			ONDK_Error("======================================================================\r\n");
			ONDK_Error("Check Message(RedrawAllApp) tick(%d)\r\n", endTick - startTick);
			ONDK_Error("======================================================================\r\n");
			ONDK_Error("\r\n");
		}
	#endif

}

void 		ONDK_GWM_DirectRedrawAll(ONDK_WINDOW_Position_e ePosition)
{
	GWM_Mgr_t		*pGWMMgr = local_gwm_GetGWMMgr();

	if(pGWMMgr->bEnableRedraw)
	{
		if(INT_ONDK_GWM_GetHaveToRedraw(ePosition))
		{
			INT_ONDK_GWM_RedrawAllApp(ePosition);
			INT_ONDK_GWM_SetHaveToRedraw(ePosition,FALSE);
		}
	}

	return;
}

HBOOL		ONDK_GWM_IsEnabledRedraw(void)
{
	GWM_Mgr_t		*pGWMMgr = local_gwm_GetGWMMgr();

	return pGWMMgr->bEnableRedraw;
}

void		ONDK_GWM_EnableRedraw(void)
{
	GWM_Mgr_t		*pGWMMgr = local_gwm_GetGWMMgr();

	pGWMMgr->bEnableRedraw = TRUE;
}

void		ONDK_GWM_DisableRedraw(void)
{
	GWM_Mgr_t		*pGWMMgr = local_gwm_GetGWMMgr();

	pGWMMgr->bEnableRedraw = FALSE;
}


/**********************************************************************************************************/
#define ____TIMER__FUNCTION____
/**********************************************************************************************************/


//#define GWM_TIMER_USE_SEM
#if defined(GWM_TIMER_USE_SEM)
	STATIC HULONG					s_ulTimerSema;

	#define DEF_ONDK_GWMTIEMR_SEM_ENTER VK_SEM_Get(s_ulTimerSema)
	#define DEF_ONDK_GWMTIEMR_SEM_LEAVE VK_SEM_Release(s_ulTimerSema)
#else
	#define DEF_ONDK_GWMTIEMR_SEM_ENTER
	#define DEF_ONDK_GWMTIEMR_SEM_LEAVE
#endif

typedef struct tagGWM_TimerItem_t
{
	ONDK_GWM_Callback_t		appCallback;	/* 타이머를 설정한 Application callback */
	HUINT32					ulTimerID;		/* 설정시의 Timer_ID */
	HUINT32					ulStartTick;	/* Session start tick */
	HUINT32					ulTickPeriod;	/* 설정된 값. */
	HCHAR					*pName;			/* 설정된 Name */
} GWM_TimerItem_t;

typedef struct 	tagGWM_TimerMgr_t
{
	HUINT32	ulLastTimerTick;

	GWM_TimerItem_t	timerList[DEF_GWM_MAX_TIMER];
} GWM_TimerMgr_t;

STATIC GWM_TimerMgr_t	*s_pstGWMTimerMgr = NULL;

#define ______TIMER_PRIVATE_FUNC__

STATIC GWM_TimerMgr_t *local_gwmtimer_GetMgr(void)
{
	return s_pstGWMTimerMgr;
}

STATIC GWM_TimerItem_t	*local_gwmtimer_FindItem(HUINT32 ulTimerID)
{
	HINT32				i = 0;
	GWM_TimerMgr_t 		*pTimerMgr = local_gwmtimer_GetMgr();
	GWM_TimerItem_t		*pTimerItem = NULL;

	ONDK_GWM_Callback_t	pCurApp = ONDK_GWM_APP_GetAppCallback(g_pstCurrentApp);

	for (i = 0; i < DEF_GWM_MAX_TIMER; i++)
	{
		pTimerItem = &pTimerMgr->timerList[i];
		if(pTimerItem->appCallback == pCurApp)
		{
			if(pTimerItem->ulTimerID == ulTimerID)
			{
				return pTimerItem;
			}
		}
	}

	return NULL;
}

STATIC HBOOL	local_gwmtimer_AppTimerTask(void *arg1)
{
	HUINT32		ulCurrentTick, ulTickDiff;
	HINT32		i;
	HBOOL		need2Draw = FALSE;
#if defined(CONFIG_DEBUG)
	HUINT32		startTick = 0, endTick = 0;
	HCHAR		*pAppName = NULL;
#endif

	GWM_TimerItem_t		*pTimerItem = NULL;
	GWM_TimerMgr_t 		*pTimerMgr = local_gwmtimer_GetMgr();

	NOT_USED_PARAM(arg1);
	ONDK_Assert(pTimerMgr);
	ulCurrentTick 	= VK_TIMER_GetSystemTick();
#if 0
	{
		HUINT32		ulDiffTick = 0;

		ulDiffTick 		= (pTimerMgr->ulLastTimerTick+DEF_GWM_TIMER_SLEEP_TIME);
		if(ulDiffTick > ulCurrentTick)
		{
			return FALSE;
		}
		pTimerMgr->ulLastTimerTick = ulCurrentTick - 1;
	}
#endif
	DEF_ONDK_GWMTIEMR_SEM_ENTER;
	for (i = 0; i < DEF_GWM_MAX_TIMER; i++)
	{
		pTimerItem = &pTimerMgr->timerList[i];

		if (pTimerItem->appCallback != NULL)
		{
			ulTickDiff = pTimerItem->ulStartTick + pTimerItem->ulTickPeriod;
			if(ulTickDiff <= ulCurrentTick)
			{
				#if defined(CONFIG_DEBUG)
					startTick = VK_TIMER_GetSystemTick();
					pAppName = ONDK_GWM_APP_GetAppName(ONDK_GWM_APP_Get(pTimerItem->appCallback));
				#endif

				ONDK_GWM_SendMessage(pTimerItem->appCallback, MSG_GWM_TIMER, 0, pTimerItem->ulTimerID, 0, 0);
				pTimerItem->ulStartTick = ulCurrentTick;
				need2Draw = TRUE;

				#if defined(CONFIG_DEBUG)
					endTick = VK_TIMER_GetSystemTick();

					if((startTick + 10) < endTick)
					{
						ONDK_Error("\r\n");
						ONDK_Error("======================================================================\r\n");
						ONDK_Error("Check Message(MSG_GWM_TIMER) tick(%d) - name(%s)\r\n", endTick - startTick, pAppName);
						ONDK_Error("======================================================================\r\n");
						ONDK_Error("\r\n");
					}
				#endif

				GWM_DBG_Print(DBGMSG_TRACE, ("[%s:%d] send msg to application(%s)", __FUNCTION__, __LINE__, pTimerItem->pName));
			}
		}
	}

	DEF_ONDK_GWMTIEMR_SEM_LEAVE;

	return need2Draw;
}

#define ______TIMER_INTERNAL_FUNC__

ONDK_Result_t     INT_ONDK_GWM_TIMER_Init(void)
{
#if defined(GWM_TIMER_USE_SEM)
	HINT32			err;
#endif

	if(s_pstGWMTimerMgr)
	{
		ONDK_Error("[%s:%d] Duplication Init\r\n", __FUNCTION__, __LINE__);
		return GWM_RET_OK;
	}

	s_pstGWMTimerMgr = ONDK_Calloc(sizeof(GWM_TimerMgr_t));
	ONDK_Assert(s_pstGWMTimerMgr);

#if defined(GWM_TIMER_USE_SEM)
	/* Semaphore for shared data between main_task and timer_task */
	err = VK_SEM_Create(&s_ulTimerSema, "TimerSema", VK_SUSPENDTYPE_PRIORITY);
	if (err != VK_OK)
	{
		GWM_DBG_Print(DBGMSG_ERROR, ("GWM Timer sema4 creation fail err = 0x%x\n", err));
		return;
	}
#endif

	return GWM_RET_OK;
}

ONDK_Result_t     INT_ONDK_GWM_TIMER_KillAllByAPP(ONDK_GWM_App_t app)
{
	HINT32				i = 0;
	GWM_TimerMgr_t 		*pTimerMgr = local_gwmtimer_GetMgr();
	GWM_TimerItem_t		*pTimerItem = NULL;

	DEF_ONDK_GWMTIEMR_SEM_ENTER;

	for (i = 0; i < DEF_GWM_MAX_TIMER; i++)
	{
		pTimerItem = &pTimerMgr->timerList[i];
		if (pTimerItem->appCallback == ONDK_GWM_APP_GetAppCallback(app))
		{
			pTimerItem->appCallback = NULL;
			pTimerItem->ulTimerID = 0;
		}
	}

	DEF_ONDK_GWMTIEMR_SEM_LEAVE;

	return GWM_RET_OK;
}

#define ______TIMER_PUBLIC_FUNC__
HBOOL		ONDK_GWM_IsTimer(HUINT32 ulTimerID)
{
	HBOOL				isTimer = FALSE;
	GWM_TimerItem_t		*pTimerItem = NULL;

	DEF_ONDK_GWMTIEMR_SEM_ENTER;

	pTimerItem = local_gwmtimer_FindItem(ulTimerID);
	if(NULL != pTimerItem)
	{
		isTimer = TRUE;
	}

	DEF_ONDK_GWMTIEMR_SEM_LEAVE;

	return isTimer;
}

ONDK_Result_t		ONDK_GWM_SetTimer(HUINT32 ulTimerID, HUINT32 ulPeriod_msec)
{
	HINT32				i = 0;
	GWM_TimerMgr_t 		*pTimerMgr = local_gwmtimer_GetMgr();
	GWM_TimerItem_t		*pTimerItem = NULL;
	GWM_TimerItem_t		*pNewTimerItem = NULL;

	DEF_ONDK_GWMTIEMR_SEM_ENTER;

	pTimerItem = local_gwmtimer_FindItem(ulTimerID);
	if(NULL != pTimerItem)
	{
		//GWM_DBG_Print(DBGMSG_WARNING, ("Same ID(%d) is already exist!! %s\n", ulTimerID, ONDK_GWM_APP_GetAppName(g_pstCurrentApp)));
		DEF_ONDK_GWMTIEMR_SEM_LEAVE;
		return GWM_RET_LINK_CONFLICT;
	}

	for (i = 0; i < DEF_GWM_MAX_TIMER; i++)	/* 비어 있는 타이머 슬롯을 찾는다.*/
	{
		if (pTimerMgr->timerList[i].appCallback == NULL)
		{
			pNewTimerItem = &pTimerMgr->timerList[i];
			break;
		}
	}

	if (NULL == pNewTimerItem )
	{
		DEF_ONDK_GWMTIEMR_SEM_LEAVE;
		return GWM_RET_INITIAL_FAILED;
	}

	pNewTimerItem->appCallback 	= ONDK_GWM_APP_GetAppCallback(g_pstCurrentApp);
	pNewTimerItem->ulTimerID 	= ulTimerID;
	pNewTimerItem->ulStartTick 	= VK_TIMER_GetSystemTick();
	pNewTimerItem->ulTickPeriod = ulPeriod_msec;
	pNewTimerItem->pName		= ONDK_GWM_APP_GetAppName(g_pstCurrentApp);

	DEF_ONDK_GWMTIEMR_SEM_LEAVE;

	return GWM_RET_OK;
}

ONDK_Result_t		ONDK_GWM_KillTimer(HUINT32 ulTimerID)
{
	GWM_TimerItem_t		*pTimerItem = NULL;

	DEF_ONDK_GWMTIEMR_SEM_ENTER;

	pTimerItem = local_gwmtimer_FindItem(ulTimerID);
	if(NULL != pTimerItem)
	{
		pTimerItem->appCallback = NULL;
		pTimerItem->ulTimerID = 0;
	}

	DEF_ONDK_GWMTIEMR_SEM_LEAVE;

	return GWM_RET_OK;
}

ONDK_Result_t		ONDK_GWM_ResetTimer(HUINT32 ulTimerID)
{
	GWM_TimerItem_t		*pTimerItem = NULL;

	DEF_ONDK_GWMTIEMR_SEM_ENTER;

	pTimerItem = local_gwmtimer_FindItem(ulTimerID);
	if(NULL != pTimerItem)
	{
		pTimerItem->ulStartTick = VK_TIMER_GetSystemTick();
	}

	DEF_ONDK_GWMTIEMR_SEM_LEAVE;

	return GWM_RET_OK;
}

#define ______TIMER_DEBUG_FUNC__
HINT32				DBG_ONDK_GWM_TIMER_DumpList(void *arg)
{
	HINT32			i;
	GWM_TimerMgr_t 		*pTimerMgr = local_gwmtimer_GetMgr();
	GWM_TimerItem_t		*pTimerItem = NULL;


	(void)arg;

	DEF_ONDK_GWMTIEMR_SEM_ENTER;

	HLIB_CMD_Printf("============================================================================\n");
	for (i = 0; i < DEF_GWM_MAX_TIMER; i++)
	{
		pTimerItem = &pTimerMgr->timerList[i];
		if (pTimerItem->appCallback != NULL)		// 존재하는 타이머라면,
		{
			HLIB_CMD_Printf("(%d). Owner(%20s,0x%X)\tPeriod:%d\tStart tick:%d\tDiff:%lu\n",
					pTimerItem->ulTimerID,
					ONDK_GWM_APP_GetAppName(ONDK_GWM_APP_Get(pTimerItem->appCallback)),
					(HUINT32)pTimerItem->appCallback,
					pTimerItem->ulTickPeriod,
					pTimerItem->ulStartTick,
					(VK_TIMER_GetSystemTick()-pTimerMgr->timerList[i].ulStartTick)
					);
		}
	}
	HLIB_CMD_Printf("============================================================================\n");

	DEF_ONDK_GWMTIEMR_SEM_LEAVE;

	return ERR_OK;
}


#define ____DEBUG___

#if defined(CONFIG_DEBUG)
STATIC HCHAR	*local_gwmdbg_GwmMsg2String(HINT32 gwmMsg)
{
	switch(gwmMsg)
	{
	ENUM_TO_STR(MSG_GWM_CREATE)
	ENUM_TO_STR(MSG_GWM_DESTROY)
	ENUM_TO_STR(MSG_GWM_APP_SPAWNED)
	ENUM_TO_STR(MSG_GWM_APP_KILLED)
	ENUM_TO_STR(MSG_GWM_DRAW)
	ENUM_TO_STR(MSG_GWM_CLICKED)
	ENUM_TO_STR(MSG_GWM_KEYUP)
	ENUM_TO_STR(MSG_GWM_KEYDOWN)
	ENUM_TO_STR(MSG_GWM_KEY_PRESSED)
	ENUM_TO_STR(MSG_GWM_KEYQWERTY)
	ENUM_TO_STR(MSG_GWM_TIMER)
	ENUM_TO_STR(MSG_GWM_FAIL_RECREATE_NEW_INSTANCE)
	ENUM_TO_STR(MSG_GWM_SCROLLLEFT)
	ENUM_TO_STR(MSG_GWM_SCROLLRIGHT)
	ENUM_TO_STR(MSG_GWM_SCROLLUP)
	ENUM_TO_STR(MSG_GWM_SCROLLDOWN)
	ENUM_TO_STR(MSG_GWM_BUTTON_RIGHT)
	ENUM_TO_STR(MSG_GWM_BUTTON_CLICKED)
	ENUM_TO_STR(MSG_GWM_SELECTOR_CHANGED)
	ENUM_TO_STR(MSG_GWM_SELECTOR_LEFT_NOMORE)
	ENUM_TO_STR(MSG_GWM_SELECTOR_RIGHT_NOMORE)
	ENUM_TO_STR(MSG_GWM_SELECTOR_CLICKED)
	ENUM_TO_STR(MSG_GWM_SPINCONTROL_CHANGED)
	ENUM_TO_STR(MSG_GWM_SPINCONTROL_BIGGEST)
	ENUM_TO_STR(MSG_GWM_SPINCONTROL_SMALLEST)
	ENUM_TO_STR(MSG_GWM_SPINCONTROL_COMPLETED)
	ENUM_TO_STR(MSG_GWM_INPUT_LEFT_NOMORE)
	ENUM_TO_STR(MSG_GWM_INPUT_RIGHT_NOMORE)
	ENUM_TO_STR(MSG_GWM_INPUTCOMPLETE)
	ENUM_TO_STR(MSG_GWM_INPUTCANCELD)
	ENUM_TO_STR(MSG_GWM_LIST_CHANGED)
	ENUM_TO_STR(MSG_GWM_LIST_UP_NOMORE)
	ENUM_TO_STR(MSG_GWM_LIST_DOWN_NOMORE)
	ENUM_TO_STR(MSG_GWM_LIST_SELECTED)
	ENUM_TO_STR(MSG_GWM_FOCUS_GET)
	ENUM_TO_STR(MSG_GWM_FOCUS_LOSE)
	ENUM_TO_STR(MSG_GWM_LIST_RIGHT_CHANGED)
	ENUM_TO_STR(MSG_GWM_LIST_LEFT_CHANGED)
	ENUM_TO_STR(MSG_GWM_OBJ_FOCUS_CHANGED)
	ENUM_TO_STR(MSG_GWM_STACK_CHANGED)
	ENUM_TO_STR(MSG_GWM_QUIT_UI)
	ENUM_TO_STR(MSG_GWM_GO_STANDBY)
	ENUM_TO_STR(MSG_GWM_QUIT_STANDBY)
	ENUM_TO_STR(MSG_GWM_TAB_CHANGED)
	ENUM_TO_STR(MSG_GWM_TAB_RIGHT_NOMORE)
	ENUM_TO_STR(MSG_GWM_TAB_LEFT_NOMORE)
	ENUM_TO_STR(MSG_GWM_GAUGEBAR_CHANGED)
	ENUM_TO_STR(MSG_GWM_GAUGEBAR_LEFT_NOMORE)
	ENUM_TO_STR(MSG_GWM_GAUGEBAR_RIGHT_NOMORE)
	ENUM_TO_STR(MSG_GWM_O2ANIMATION_END)
	ENUM_TO_STR(MSG_GWM_NOT_AVAILABLE)
	ENUM_TO_STR(MSG_GWM_HAPI_MESSAGE)
	ENUM_TO_STR(MSG_GWM_APK_MESSAGE)
	ENUM_TO_STR(MSG_GWM_APK_INITIALIZED)
	ENUM_TO_STR(MSG_GWM_APK_BLUETOOTH)
	ENUM_TO_STR(MSG_GWM_APK_KEYBOARD)
	ENUM_TO_STR(MSG_GWM_APK_SYSINFO)
	ENUM_TO_STR(MSG_GWM_INVALIDATE_RECT)
	ENUM_TO_STR(MSG_SIGNAL_START )
	ENUM_TO_STR(MSG_SIGNAL_REQUESTSTANDBY)
	ENUM_TO_STR(MSG_SIGNAL_SHOWSUBTITLE)
	ENUM_TO_STR(MSG_SIGNAL_SHOWSUBTITLELIVE)
	ENUM_TO_STR(MSG_SIGNAL_VARIABLEFONT)
	ENUM_TO_STR(MSG_SIGNAL_COUNTRYID)
	ENUM_TO_STR(MSG_SIGNAL_REMOTESTING)
	ENUM_TO_STR(MSG_SIGNAL_END)
	ENUM_TO_STR(MSG_APP_START )
	ENUM_TO_STR(MSG_APP_TELETEXT_STARTED)
	ENUM_TO_STR(MSG_APP_TELETEXT_STOPPED)
	ENUM_TO_STR(MSG_APP_TELETEXT_SHOWFAIL)
	ENUM_TO_STR(MSG_APP_START_SUBTITLE)
	ENUM_TO_STR(MSG_APP_START_SUBTITLE_WITH_PAGEID)
	ENUM_TO_STR(MSG_APP_STOP_SUBTITLE)
	ENUM_TO_STR(MSG_APP_EXECUTE_CALLBACK)
	ENUM_TO_STR(MSG_GWM_END)

	default:
		break;
	}

	return "";
}
#endif

HCHAR *ONDK_GWM_GwmMsg2String(HINT32 message)
{
	#if defined(CONFIG_DEBUG)
		return local_gwmdbg_GwmMsg2String(message);
	#else
		return "";
	#endif
}

