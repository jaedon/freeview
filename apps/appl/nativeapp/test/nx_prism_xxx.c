/*
 * (c) 2011-2013 Humax Co., Ltd.
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
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/******************************************************************************/
/**
 * @file	  		nx_xxx.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/


/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static ONDK_Result_t 	nx_XXX_SetUiObject(void);

static ONDK_Result_t	nx_XXX_MsgGwmCreate(HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_XXX_MsgGwmDestroy(HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_XXX_MsgGwmHapiMsg(Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_XXX_MsgGwmKeyDown(HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static ONDK_Result_t 	nx_XXX_SetUiObject(void)
{
	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_XXX_MsgGwmCreate(HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_XXX_MsgGwmDestroy(HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_XXX_MsgGwmHapiMsg(Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_XXX_MsgGwmKeyDown(HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_XXX_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_XXX_MsgGwmCreate(p1, p2, p3);
			break;
		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_XXX_MsgGwmKeyDown(p1, p2, p3);
			break;
		case MSG_GWM_DESTROY:
			eResult = nx_XXX_MsgGwmDestroy(p1, p2, p3);
			break;
		case MSG_GWM_HAPI_MESSAGE:
			eResult = nx_XXX_MsgGwmHapiMsg(handle, p1, p2, p3);
			break;
		default:
			break;
	}
	if(eResult != ONDK_RET_OK)
	{
		return eResult;
	}

	return ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

