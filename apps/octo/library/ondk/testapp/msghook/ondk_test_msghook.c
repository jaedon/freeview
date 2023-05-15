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
* MS949 encoding �� ����ϴ� source file �Դϴ�.
* MS949 encoding�����Ī��� source file�Ǫ���
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* �� 2011-2012 Humax Co., Ltd.
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


/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/

ONDK_Result_t ONDK_TEST_MSGHOOK_Proc(HINT32 nMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HLIB_CMD_Printf(HxANSI_COLOR_BLUE("\tHook - Message(%d) : %s  p1(0x%x) p2(0x%x) p3(0x%x) \n"),  nMessage, ONDK_GWM_GwmMsg2String(nMessage), p1, p2, p3);
	return ONDK_MESSAGE_PASS;
}



/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/


void ONDK_TEST_MSGHOOK_Start(void)
{
	if(NULL == ONDK_GWM_APP_Get(ONDK_TEST_MSGHOOK_Proc))
	{
		HLIB_CMD_Printf("[%s:%d] Create GWM MESSAGE Hook\r\n", __FUNCTION__, __LINE__);
		ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "ONDK_TEST_MSGHOOK_Proc", APP_DBG_MSGHOOK_PRIORITY, ONDK_TEST_MSGHOOK_Proc, 0, 0, 0, 0);
	}
	else
	{
		HLIB_CMD_Printf("[%s:%d] Destroy GWM MESSAGE Hook\r\n", __FUNCTION__, __LINE__);
		ONDK_GWM_APP_Destroy(ONDK_TEST_MSGHOOK_Proc);
	}
}

