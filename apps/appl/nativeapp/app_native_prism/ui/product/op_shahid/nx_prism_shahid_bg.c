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
 * @file	  		nx_prism_shahid_bg.c
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
#include <nx_main.h>
#include <nx_core.h>
#include <nx_port.h>

#include <nx_common.h>
#include <nx_core_message.h>
#include <nx_prism_app.h>

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef enum
{
	eSHAHID_FTTYPE_CAST,
	eSHAHID_FTTYPE_VAST,
	eSHAHID_FTTYPE_GA,
	eSHAHID_FTTYPE_YUBORA,
}Nx_ShahidFeatureType_e;

typedef struct tagShahidBgContext
{
	Nx_ShahidFeatureType_e	eCurFtType;
}Nx_ShahidBgContext_t;

/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_ShahidBgContext_t	s_ShahidBgContext;

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static	Nx_ShahidBgContext_t *nx_shahid_bg_GetContext(void);

static	ONDK_Result_t	nx_shahid_bg_PlayShahidCast(Nx_ShahidBgContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3);

static 	ONDK_Result_t	nx_shahid_bg_MsgGwmCreate(Nx_ShahidBgContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3);
static 	ONDK_Result_t	nx_shahid_bg_MsgGwmKeyDown(Nx_ShahidBgContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3);
static 	ONDK_Result_t	nx_shahid_bg_MsgGwmDestroy(Nx_ShahidBgContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3);

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/
static	Nx_ShahidBgContext_t *nx_shahid_bg_GetContext(void)
{
	return &s_ShahidBgContext;
}

static	ONDK_Result_t	nx_shahid_bg_PlayShahidCast(Nx_ShahidBgContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_PRISM_PLAYMGR_PlayStart(ePlayerType_MEDIAPLAY, 0, 0, FALSE);

	return ONDK_RET_OK;
}

static 	ONDK_Result_t	nx_shahid_bg_MsgGwmCreate(Nx_ShahidBgContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxSTD_MemSet(pstContext, 0, sizeof(Nx_ShahidBgContext_t));

	return ONDK_RET_OK;
}

static 	ONDK_Result_t	nx_shahid_bg_MsgGwmKeyDown(Nx_ShahidBgContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_PRISM_UTIL_KeyConv_UiCommon(&p1);

	switch (p1)
	{
		default:
			NX_APP_Debug("[%s:%d] key = %d \n", __FUNCTION__, __HxLINE__, p1);
			break;
	}
	return	ONDK_RET_OK;
}

static 	ONDK_Result_t	nx_shahid_bg_MsgGwmDestroy(Nx_ShahidBgContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{

	NX_COMMON_NOT_USED_PARAM(pstContext);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_SHAHID_BG_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	Nx_ShahidBgContext_t	*pstContext = NULL;

	pstContext = nx_shahid_bg_GetContext();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			eResult = nx_shahid_bg_MsgGwmCreate(pstContext, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_KEYDOWN:
			eResult = nx_shahid_bg_MsgGwmKeyDown(pstContext, p1, p2, p3);
			break;

		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_shahid_bg_MsgGwmDestroy(pstContext, p1, p2, p3);
			break;

		case MSG_CORE_CONTENTS_SHAHID_CAST_PLAY:
			eResult = nx_shahid_bg_PlayShahidCast(pstContext, p1, p2, p3);
			break;

		case MSG_CORE_CONTENTS_SHAHID_CAST_STOP:
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

