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
/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/
#include 	<ondk.h>

#include	<hapi.h>

#include 	"ondk_gwm_internal.h"

/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/

static DEF_ONDK_GWM_RECV_PostMsgToGWMCallback_t		s_pfncPostMsgCallback;
static DEF_ONDK_GWM_RECV_PostDataToGWMCallback_t	s_pfncPostDataCallback;
//static HULONG 								s_ulGwmMainTaskId;
static HBOOL 								s_btaskstop = FALSE;
static HUINT32								s_ulRecvGwmSemId;
static HUINT32								s_ulOndkMsgQId = 0;
static HUINT32								(*s_pfnProcessLoop)(void) = NULL;


/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/

HUINT32				 				g_ulGwmTaskId = 0;

void	INT_ONDK_GWM_MainTask(void *arg1)
{
	HERROR 		hErr = ERR_OK;
	ONDK_MSG_t	msg;
	HINT32		nRemained = 0;
	HBOOL		toDraw = FALSE;

	NOT_USED_PARAM(arg1);

	ONDK_Print("\n\nGWM STARTS !\n\n\n");

	g_ulGwmTaskId = HLIB_STD_GetTID();

#if 1
	/* launch system ignition app */
	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW,"ONDK init", 1, (ONDK_GWM_Callback_t)g_pfnAppIgnitor, 0, 0, 0, 0);
#endif

	ONDK_Assert(s_ulRecvGwmSemId);

	while (1)			/* infinite application task loop */
	{
		HxSTD_memset(&msg, 0, sizeof(ONDK_MSG_t));
		toDraw = FALSE;

		if (s_pfnProcessLoop)
		{
			do
			{
				nRemained = s_pfnProcessLoop();

				HxLOG_Debug(" --> ONDK process s_pfnProcessLoop (like APPKIT) : nRemained : [%d]\n", nRemained);
			} while (nRemained > 0);
		}

		hErr = HLIB_MSG_ReceiveTimeout(ONDK_GWM_RECV_GetMessageQId(), &msg, sizeof(ONDK_MSG_t), 10);
		if (ERR_OK == hErr)
		{
			INT_ONDK_GWM_GetMessage(&msg);
		}

		HxSEMT_Get(s_ulRecvGwmSemId);
		toDraw = INT_ONDK_GWM_DrawCheck();
		if (ERR_OK == hErr || TRUE == toDraw)
		{
			INT_ONDK_GWM_LOOP();
		}
		HxSEMT_Release(s_ulRecvGwmSemId);
	}

	ONDK_Print("\n\nGWM QUITS !\n\n\n");

}

void INT_ONDK_GWM_RECV_RegisterPostMsgToApCallBack(DEF_ONDK_GWM_RECV_PostMsgToGWMCallback_t pfncPostMsgCallback)
{
   s_pfncPostMsgCallback = pfncPostMsgCallback;
}

void INT_ONDK_GWM_RECV_RegisterPostDataToApCallBack(DEF_ONDK_GWM_RECV_PostDataToGWMCallback_t pfncPostDataCallback)
{
   s_pfncPostDataCallback = pfncPostDataCallback;
}

void INT_ONDK_GWM_RECV_RegisterKeyCallback(ONDKWindowHandle mWindow, ONDK_KEY_KeyNotifyCallback_t pfnKeyNotifyCallback)
{
    HUINT32 id;
    id = ONDK_KEY_RegisterKeyCallback(mWindow, pfnKeyNotifyCallback);

    ONDK_Print("[%s:%d] id [%d] \n",__FUNCTION__,__LINE__,id);
}

/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/


//HAPI_Register 를 여기에서.. signal notifier 를 여기서 보고 싶어서
void ONDK_GWM_RECV_Init(HLONG semid)
{
	HINT32 err = 0;

	s_btaskstop = TRUE;
	s_ulRecvGwmSemId = semid;

	err = HLIB_MSG_Create(128, sizeof(ONDK_MSG_t), "ondkMsg", (HULONG*)&s_ulOndkMsgQId, eHLIB_SUSPENDTYPE_FIFO);
	ONDK_Assert(0 == err);

    ONDK_Print("[%s:%d]  \n",__FUNCTION__,__LINE__);
}

void ONDK_GWM_RECV_PostMsgToGWM(HINT32 msg, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if(s_pfncPostMsgCallback != NULL)
	{
		s_pfncPostMsgCallback(msg, hAct, p1, p2, p3);
	}
}

void ONDK_GWM_RECV_PostDataToGWM(HINT32 msg, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3, void *pvData, void (*pfFreeData)(void *))
{
	if(s_pfncPostDataCallback != NULL)
	{
		s_pfncPostDataCallback(msg, hAct, p1, p2, p3, pvData, pfFreeData);
	}
}

HUINT32	ONDK_GWM_RECV_GetMessageQId(void)
{
	return s_ulOndkMsgQId;
}

void	ONDK_GWM_RECV_RegisterLoop(HUINT32 (*pfnLoopFunction)(void))
{
	s_pfnProcessLoop = pfnLoopFunction;
}

