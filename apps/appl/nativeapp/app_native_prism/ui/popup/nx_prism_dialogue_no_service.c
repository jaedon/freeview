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
 * @file	  		nx_prism_dialogue_no_service.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2015 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/


/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/

#include <nx_common.h>
#include <nx_prism_app.h>



/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/



/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static NXUI_DialogInstance_t s_NoSvcDlg;



/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
#if defined(CONFIG_MW_CH_SATELLITE)
static ONDK_GWM_Callback_t		nx_prism_Dialogue_NoService_GetSearchChannelsLaunchMenu_Satellite(void);
#endif


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

#if defined(CONFIG_MW_CH_SATELLITE)
static ONDK_GWM_Callback_t		nx_prism_Dialogue_NoService_GetSearchChannelsLaunchMenu_Satellite(void)
{
	HINT32		lValue = 0;
	HERROR		hError = ERR_FAIL;
	NX_SAT_AntennaType_e	eAntennaType = eNxSat_AntType_LNB_Only;


	hError = NX_ANTENNA_GetAntennaType(&lValue);
	if (hError != ERR_FAIL)
	{
		eAntennaType = eNxSat_AntType_LNB_Only;
	}
	else
	{
		eAntennaType = (NX_SAT_AntennaType_e)lValue;
	}

	switch (eAntennaType)
	{
		case eNxSat_AntType_LNB_Only:
		case eNxSat_AntType_DiSEqC_1_0:
		case eNxSat_AntType_DiSEqC_2_0:
			return	(ONDK_GWM_Callback_t)NX_PRISM_MENU_Search_SatFixedAntna_Proc;
			break;
		case eNxSat_AntType_SCD:
			// TODO: Add Code
			return	(ONDK_GWM_Callback_t)NULL;

		case eNxSat_AntType_Motor:
			// TODO: Add Code
			return	(ONDK_GWM_Callback_t)NULL;

		default:
			return	(ONDK_GWM_Callback_t)NX_PRISM_MENU_Settings_Proc;
	}
}
#endif


static ONDK_Result_t	nx_prism_Dialogue_NoService_proc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_Callback_t 	appToLaunch = NULL;

	switch (message)
	{
		case MSG_GWM_TIMER:
			break;

		case MSG_GWM_STACK_CHANGED:
			return ONDK_MESSAGE_PASS;

		case MSG_GWM_QUIT_UI:
			return ONDK_MESSAGE_PASS;

		case MSG_GWM_KEYDOWN:
			if((p1 == KEY_OK) || ( p1 == KEY_FRONT_OK))
			{
				ONDK_GWM_APP_Destroy(nx_prism_Dialogue_NoService_proc);

#if defined(CONFIG_MW_CH_SATELLITE)
				appToLaunch = nx_prism_Dialogue_NoService_GetSearchChannelsLaunchMenu_Satellite();
#else
				appToLaunch = NX_PRISM_MENU_Settings_Proc;
#endif
				if (appToLaunch != NULL)
				{
					ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_Settings_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (ONDK_GWM_Callback_t)appToLaunch, 0, 0, 0, 0);
				}
				return ONDK_MESSAGE_BREAK;
			}
			break;

		case MSG_GWM_DESTROY:
			break;
	}

	return NX_PRISM_DIALOGUE_Message_Proc(&s_NoSvcDlg, message, hAction, p1, p2, p3);
}

/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
void	NX_PRISM_Dialogue_CreateNoService(void)
{
	HUINT8		*pszPanelStr = NULL;

	NX_PRISM_DIALOGUE_SetDialog(&s_NoSvcDlg,
								ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
								(NX_DLG_TYPE_NO_EXIT|NX_DLG_OK|NX_DLG_TIMEOUT_NOTIMEOUT|NX_DLG_TYPE_MESSAGE),
								(HUINT8 *)NULL,
								(HUINT8 *)(HINT32)ONDK_GetString(RES_NO_SERVICE_ID));

	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW,
						"nx_prism_Dialogue_NoService_proc",
						APP_DEFAULT_PRIORITY,
						nx_prism_Dialogue_NoService_proc,
						0, 0, 0, 0);

	pszPanelStr = NX_PRISM_SPEC_GetPanelString(eNX_PANEL_STR_NoChannel);
	if (pszPanelStr == NULL)
	{
		NX_APP_Error("Error!! NX_PRISM_SPEC_GetPanelString() - eNX_PANEL_STR_Wizard !!!\n");
	}
	NX_PANEL_SetString(pszPanelStr, eDxPANEL_ALIGN_CENTER, FALSE);

}

void	NX_PRISM_Dialogue_DestroyNoService(void)
{
	NX_PANEL_ClearString();
	ONDK_GWM_APP_Destroy(nx_prism_Dialogue_NoService_proc);
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

