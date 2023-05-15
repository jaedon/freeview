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

/**************************************************************************************************/
#define _________AP_Radio_Private_Include_________________________________________________
/**************************************************************************************************/
#include <prism.h>

/**************************************************************************************************/
#define _________AP_Radio_Private_Macro_________________________________________________
/**************************************************************************************************/
#define RADIO_BGPATH  "/usr/image/104_BG_Radio.png"
#define RADIO_FRAME_ID				1
/**************************************************************************************************/
#define _________AP_Radio_Private_Struct_________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define _________AP_Radio_Private_Value_________________________________________________
/**************************************************************************************************/
static ONDKImageHandle hRadioBg = NULL;


/**************************************************************************************************/
#define _________AP_Radio_Private_Prototype_________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define _________AP_Radio_Private_Init__________________________________________________________
/**************************************************************************************************/
ONDK_Result_t 	local_radio_MsgGwmCreate(void)
{
	ONDK_Rect_t	src_rect = {0,0,1280,720};
	ONDK_Rect_t dst_rect = {0,0,1280,720};

	hRadioBg = ONDK_IMG_CreateSelf((HUINT8*)RADIO_BGPATH);
	ONDK_WINDOW_StretchBlit(ONDK_GWM_GetWindow(ONDK_LOWER_WINDOW), hRadioBg, src_rect, dst_rect);
	ONDK_WINDOW_Update(ONDK_GWM_GetWindow(ONDK_LOWER_WINDOW), NULL);

	return GWM_RET_OK;
}

/**************************************************************************************************/
#define _________AP_Radio_Private_Update__________________________________________________________
/**************************************************************************************************/



/**************************************************************************************************/
#define _________AP_Radio_Private_Process__________________________________________________________
/**************************************************************************************************/



/**************************************************************************************************/
#define _________AP_Radio_Private_Destroy__________________________________________________________
/**************************************************************************************************/
static ONDK_Result_t 	local_volume_MsgGwmDestroy(void)
{
	ONDK_SURFACE_Clear(ONDK_GWM_GetSurface(ONDK_LOWER_WINDOW));
	ONDK_WINDOW_Update(ONDK_GWM_GetWindow(ONDK_LOWER_WINDOW), NULL);
	ONDK_IMG_Destroy(hRadioBg);
	hRadioBg = NULL;

	return GWM_RET_OK;
}
/**************************************************************************************************/
#define _________AP_Radio_Private_Proc_UI__________________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________AP_Radio_Private_Proc_MSG_RUNNIG__________________________________________________________
/**************************************************************************************************/



/**************************************************************************************************/
#define _________AP_Radio_Public_Proc____________________________________________________
/**************************************************************************************************/


ONDK_Result_t	AP_Radio_Proc(HINT32 nMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult	= GWM_RET_OK;

	ONDK_Print("[%s:%d] Message(%d) : %s, handle: %p p1(0x%x) p2(0x%x) p3(0x%x) \n", __FUNCTION__, __LINE__, nMessage, ONDK_GWM_GwmMsg2String(nMessage), p1, p2, p3);

	switch (nMessage)
	{
	case MSG_GWM_CREATE:
		eResult = local_radio_MsgGwmCreate();
		break;
	case MSG_GWM_DESTROY:
		eResult = local_volume_MsgGwmDestroy();
		break;
	case MSG_GWM_KEYDOWN:
	case MSG_GWM_KEYUP:
		eResult = ONDK_MESSAGE_PASS;
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
#define _________AP_Radio_Public_Functions__________________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________AP_Radio_End_Of_File____________________________________________________
/**************************************************************************************************/

