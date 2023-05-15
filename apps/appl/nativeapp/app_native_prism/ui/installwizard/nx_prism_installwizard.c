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
 * @file	  		nx_prism_installwizard.c
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
#include <nx_common.h>

#include <nx_prism_app.h>
#include <nx_prism_ui.h>
#include <nx_prism_component.h>

#include <nx_prism_install_main_ui.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_InstallContext_t		            s_stInstallContent;

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static inline Nx_InstallContext_t  *nx_install_GetContents(void);
static ONDK_Result_t	nx_install_InitApplication (Nx_InstallContext_t *pstContents);

static ONDK_Result_t	nx_install_ActionGoNext(Nx_InstallContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t	nx_install_MsgGwmCreate(Nx_InstallContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_install_MsgGwmDestroy(Nx_InstallContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static inline Nx_InstallContext_t	*nx_install_GetContents(void)
{
	return	&s_stInstallContent;
}

static ONDK_Result_t	nx_install_InitApplication (Nx_InstallContext_t *pstContents)
{
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	if (pstContents == NULL)
	{
		return GWM_RET_SOURCE_NULL;
	}

	HxSTD_MemSet(pstContents, 0, sizeof(Nx_InstallContext_t));

    eResult = nx_install_ui_GetSubPage(pstContents);

    pstContents->ePage = eNxInstallPage_Start;

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_install_ActionGoNext(Nx_InstallContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;

	eResult = nx_install_ui_DoNextStep(pstContents->ePage, eNxInstallPage_NextPage, p1, p2, p3);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! nx_install_ui_DoNextStep() Fail!! \n");
	}

	ONDK_GWM_APP_Destroy(0);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_install_MsgGwmCreate(Nx_InstallContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;

    HxLOG_Error("\n\n####### nx_install_MsgGwmCreate ########\n\n");

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		ONDK_GWM_APP_Destroy(0);

		return ONDK_RET_INITFAIL;
	}

	/* alloc mainItems, mainItem has each infos of sub menu to launch */
	eResult = nx_install_InitApplication(pstContents);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! (eResult != ONDK_RET_OK) \n");
		ONDK_GWM_APP_Destroy(0);

		return eResult;
	}

    nx_install_ActionGoNext(pstContents, p1, p2, p3);

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	eResult;
}


static ONDK_Result_t	nx_install_MsgGwmDestroy(Nx_InstallContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents->pstSubItems)
	{
   		NX_APP_Free(pstContents->pstSubItems);
		pstContents->pstSubItems = NULL;
	}
	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}

/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_Install_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	Nx_InstallContext_t	*pstContents = NULL;

	pstContents = nx_install_GetContents();
	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_install_MsgGwmCreate(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_DESTROY:
			eResult = nx_install_MsgGwmDestroy(pstContents, p1, p2, p3);
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
