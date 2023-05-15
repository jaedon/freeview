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
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ¨Ï 2011-2012 Humax Co., Ltd.
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


/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/
#include 	<ondk.h>






/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/
#define ONDK_TEST_PARAM_WARRING_REMOVE(arg1, arg2, arg3, arg4)			\
			{															\
			(void)arg1;													\
			(void)arg2;													\
			(void)arg3;													\
			(void)arg4;							 						\
			}


#define DEF_ONDK_TEST_FPS_X  1000
#define DEF_ONDK_TEST_FPS_Y  50
#define DEF_ONDK_TEST_FPS_W  200
#define DEF_ONDK_TEST_FPS_H  100

#define DEF_ONDK_TEST_FPS_DATA_X  1000
#define DEF_ONDK_TEST_FPS_DATA_Y  50
#define DEF_ONDK_TEST_FPS_DATA_W  200
#define DEF_ONDK_TEST_FPS_DATA_H  100

#define DEF_ONDK_TEST_FPS_TIMER_ID		0x820610
#define DEF_ONDK_TEST_UPDATE_TIMER_ID	0x820611

typedef enum tagTESTFPS_Object
{
	eTESTFPS_Object_FRAME_Background = 0x991,
	eTESTFPS_Object_RECT_DATA,

	eTESTFPS_Object_BTN_MAX
}TESTFPS_Object_e;

typedef struct tagTESTFPS_Contents_t
{
	HUINT32			sysLastKeyTick;
	HUINT32			sysLastTimerTick;
	HCHAR			szRcu[2048];
	HCHAR			szTimer[2048];

	double			sysAvrKeyTick;
	double			sysAvrTimerTick;
	HINT32			sysAvrKeyCnt;
	HINT32			sysAvrTimerCnt;
} TESTFPS_Contents_t;

STATIC TESTFPS_Contents_t *s_pstTestFontContents = NULL;

/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/


STATIC TESTFPS_Contents_t *local_testfps_GetContents(void)
{
	if(NULL == s_pstTestFontContents)
	{
		s_pstTestFontContents = ONDK_Calloc(sizeof(TESTFPS_Contents_t));
	}
	return s_pstTestFontContents;
}

ONDK_Result_t local_testfps_DrawMoveFPS (ONDKSurfaceHandle screen, ONDK_Region_t *area,
								ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32 x, y, w;

	ONDKFontHandle 			hFontHandle = NULL;
	TESTFPS_Contents_t 	*pstContents = local_testfps_GetContents();

	hFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, 16);
	if (hFontHandle == NULL)
	{
		ONDK_Assert(0);
		return	ONDK_RET_FAIL;
	}
	x = DEF_ONDK_TEST_FPS_DATA_X;
	y = DEF_ONDK_TEST_FPS_DATA_Y;
	w = DEF_ONDK_TEST_FPS_DATA_W;

	ONDK_FONT_DrawStringAlign(screen, hFontHandle, pstContents->szRcu, x, y, w, 0xFFAAAAAA, ONDKSTF_LEFT);
	y+=20;
	ONDK_FONT_DrawStringAlign(screen, hFontHandle, pstContents->szTimer, x, y, w, 0xFFAAAAAA, ONDKSTF_LEFT);

	if (hFontHandle)
		ONDK_FONT_Release (hFontHandle);

	return	ONDK_RET_OK;
}


STATIC void local_testfps_SetUiObject(TESTFPS_Contents_t *pstContents)
{
	ONDK_Rect_t 		rect;

	ONDK_GWM_APP_SetAppArea(DEF_ONDK_TEST_FPS_X, DEF_ONDK_TEST_FPS_Y, DEF_ONDK_TEST_FPS_W, DEF_ONDK_TEST_FPS_H);

	rect = ONDK_Rect(0, 0, DEF_ONDK_TEST_FPS_DATA_W, DEF_ONDK_TEST_FPS_DATA_H);
	ONDK_GWM_COM_Rect_Create(eTESTFPS_Object_RECT_DATA, &rect, 0xFF333333);
	ONDK_GWM_APP_SetObjectDrawMethod(eTESTFPS_Object_RECT_DATA, local_testfps_DrawMoveFPS);

	ONDK_GWM_SetTimer(DEF_ONDK_TEST_FPS_TIMER_ID, 10);
	ONDK_GWM_SetTimer(DEF_ONDK_TEST_UPDATE_TIMER_ID, 500);
}

/**************************************************************************************************/
#define _________AP_Volume_Private_Proc_MSG_RUNNIG__________________________________________________________
/**************************************************************************************************/


//MSG_GWM_CREATE
STATIC ONDK_Result_t local_testfps_MsgGwmCreate(TESTFPS_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Assert(pstContents);

	local_testfps_SetUiObject(pstContents);

	ONDK_GWM_APP_SetAttribute (GWM_HiddenUI_App);

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);

	return GWM_RET_OK;
}


//MSG_GWM_KEYDOWN
STATIC ONDK_Result_t local_testfps_MsgGwmKeyDown(TESTFPS_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		fpsTick = 0;
	double		fps = 0.0f;

	ONDK_Assert(pstContents);

	fpsTick = ((VK_TIMER_GetSystemTick() - pstContents->sysLastKeyTick));
	pstContents->sysLastKeyTick = VK_TIMER_GetSystemTick();
	fps = 1000.0f / ((double)fpsTick+0.001f);
	pstContents->sysAvrKeyTick += fps;
	pstContents->sysAvrKeyCnt++;

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);
	return GWM_RET_OK;
}


//MSG_GWM_DESTROY
STATIC ONDK_Result_t local_testfps_MsgGwmTimer(TESTFPS_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		fpsTick = 0;
	double		fps = 0.0f;

	ONDK_Assert(pstContents);

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);

	switch(p1)
	{
	case DEF_ONDK_TEST_FPS_TIMER_ID:
		fpsTick = ((VK_TIMER_GetSystemTick() - pstContents->sysLastTimerTick));
		pstContents->sysLastTimerTick = VK_TIMER_GetSystemTick();
		fps = 1000.0f / ((double)fpsTick+0.001f);
		pstContents->sysAvrTimerTick += fps;
		pstContents->sysAvrTimerCnt++;
		break;

	case DEF_ONDK_TEST_UPDATE_TIMER_ID:
		HxSTD_snprintf(pstContents->szRcu, 2048, "RCU : %f", (pstContents->sysAvrKeyTick/pstContents->sysAvrKeyCnt));
		HxSTD_snprintf(pstContents->szTimer, 2048, "FPS : %f", (pstContents->sysAvrTimerTick/pstContents->sysAvrTimerCnt));
		pstContents->sysAvrKeyTick = 0.0f;
		pstContents->sysAvrTimerTick = 0.0f;
		pstContents->sysAvrKeyCnt = 0;
		pstContents->sysAvrTimerCnt = 0;
		ONDK_GWM_APP_InvalidateRect(DEF_ONDK_TEST_FPS_DATA_X, DEF_ONDK_TEST_FPS_DATA_Y, DEF_ONDK_TEST_FPS_DATA_W, DEF_ONDK_TEST_FPS_DATA_H);
		break;

	default:
		break;
	}


	return GWM_RET_OK;
}

//MSG_GWM_DESTROY
STATIC ONDK_Result_t local_testfps_MsgGwmDestroy(TESTFPS_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Assert(pstContents);

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);


	return GWM_RET_OK;
}

/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/

ONDK_Result_t ONDK_TEST_FPS_Proc(HINT32 nMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult	= GWM_RET_OK;
	TESTFPS_Contents_t 	*pstContents = local_testfps_GetContents();

	switch (nMessage)
	{
	case MSG_GWM_CREATE:
		eResult = local_testfps_MsgGwmCreate(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_KEYDOWN:
		eResult = local_testfps_MsgGwmKeyDown(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_TIMER:
		eResult = local_testfps_MsgGwmTimer(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_DESTROY:
		eResult = local_testfps_MsgGwmDestroy(pstContents, p1, p2, p3);
		break;
	default:
		break;
	}

	return ONDK_MESSAGE_PASS;
}

void ONDK_TEST_FPS_Start(void)
{
	if(NULL == ONDK_GWM_APP_Get(ONDK_TEST_FPS_Proc))
	{
		HLIB_CMD_Printf("[%s:%d] Create \r\n", __FUNCTION__, __LINE__);
		ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "ONDK_TEST_FPS_Proc", APP_DBG_MSGHOOK_PRIORITY, ONDK_TEST_FPS_Proc, 0, 0, 0, 0);
	}
	else
	{
		HLIB_CMD_Printf("[%s:%d] Destroy\r\n", __FUNCTION__, __LINE__);
		ONDK_GWM_APP_Destroy(ONDK_TEST_FPS_Proc);
	}
}



