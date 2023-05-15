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
 * @file	  		nx_prism_installwizard_result_noui.c
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
#include <nx_prism_install_sub_result_noui.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	NX_PRISM_INSTALL_RESULT_ITEM_ID			        ((NX_PRISM_INSTALL_ITEM_ID)+1200)


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef struct tagInstall_Result_NoUI_Context {
	HINT32		lCurFocus;
	HUINT32		ulNumOfMainItem, ulMaxOfMainItem;
	Nx_InstallSubPage_t	*pstSubItems;

	HINT32		lDefaultFocusIdx;
	HUINT8		ucTitle[NX_PRISM_INSTALL_MAX_ELEMENT_TEXT_LENGTH];
	HUINT8      ulNumOfSubPage;// Nx_InstallSubPage_t sub page number
	HBOOL       bChSearchingCompleted;
	NxInstallPage_t ePage;
} Nx_Install_Result_NoUI_Context_t;

/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

static Nx_Install_Result_NoUI_Context_t		s_stInstallResultNoUIContent;

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static ONDK_Result_t	                    nx_install_result_noui_MsgGwmCreate(Nx_Install_Result_NoUI_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	                    nx_install_result_noui_MsgGwmDestroy(Nx_Install_Result_NoUI_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

static inline Nx_Install_Result_NoUI_Context_t	*nx_install_result_noui_GetContents(void);
static ONDK_Result_t	                    nx_install_result_noui_InitApplication (Nx_Install_Result_NoUI_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t                        nx_install_result_noui_SetFavouriteDefaultData(void);
static ONDK_Result_t                        nx_install_result_noui_SetFactoryDefaultData(void);

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static inline Nx_Install_Result_NoUI_Context_t	*nx_install_result_noui_GetContents(void)
{
	return	&s_stInstallResultNoUIContent;
}

static ONDK_Result_t	nx_install_result_noui_InitApplication (Nx_Install_Result_NoUI_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	Nx_InstallSubPage_t	*pstSubItemArray = NULL;
	HUINT8 ulNumOfSubPage = 0;
	HINT32 lTvCount = 0, lRadioCount = 0;

	if (pstContents == NULL)
	{
		return GWM_RET_SOURCE_NULL;
	}

	HxSTD_MemSet(pstContents, 0, sizeof(Nx_Install_Result_NoUI_Context_t));

	pstContents->ePage = eNxInstallPage_Result_NoUI;

	nx_install_ui_GetListOfSubPage(&pstSubItemArray, &ulNumOfSubPage);

	pstContents->pstSubItems = pstSubItemArray;
	pstContents->ulNumOfSubPage= ulNumOfSubPage;


	lTvCount    = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_TV);
	lRadioCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_RADIO);

	if((lTvCount+lRadioCount)>0)
	{
		pstContents->bChSearchingCompleted = TRUE;
	}else
	{
		pstContents->bChSearchingCompleted = FALSE;
	}

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return ONDK_RET_OK;
}

static void		nx_install_result_noui_GetPageResultValue(Nx_Install_Result_NoUI_Context_t *pstContents, NxInstallPage_t ePage,HCHAR* pStrRet)
{
	HERROR		hError = ERR_FAIL;
	HCHAR*		strValue = NULL, *pszStrId = NULL;
	HINT32		lTvCount = 0, lRadioCount = 0;
	HxLANG_Id_e	eCurLang = eLangID_NONE;
	HxCountry_e	eCurCountry = eCountryID_NONE;
#if defined(CONFIG_MW_INET) || defined(CONFIG_MW_INET_WLAN)
	HBOOL		bConnected = FALSE;
	HINT32		devCount = 0, i = 0;
	NX_NET_NetDevInfo_t *pstNetDevInfoList = NULL;
#endif


	if ((NULL == pstContents) || (NULL == pStrRet))
	{
		NX_APP_Error("Error!!! ((NULL == pstContents) || (NULL == pStrRet)) Fail \n");
		return;
	}

	strValue = NX_APP_Calloc(sizeof(HCHAR)*NX_PRISM_INSTALL_MAX_ELEMENT_TEXT_LENGTH);
	if (strValue == NULL)
	{
		NX_APP_Error("Error!!! NX_APP_Calloc() Fail \n");
		return;
	}

	switch(ePage)
	{
		case eNxInstallPage_Language:
			{
				hError = NX_SYSTEM_GetMenuLanuage((HINT32*)&eCurLang);
				if (hError != ERR_OK)
				{
					eCurLang = eLangID_NONE;
					NX_APP_Error("Error!!! NX_SYSTEM_GetMenuLanuage() Fail \n");
				}
				pszStrId = NX_PRISM_UTIL_GetResStringIdByLangId(eCurLang);
				if (pszStrId != NULL)
				{
					HxSTD_StrNCpy(strValue,(HCHAR*) ONDK_GetString(pszStrId), NX_PRISM_INSTALL_MAX_ELEMENT_TEXT_LENGTH);
				}
			}
			break;
		case eNxInstallPage_Country:
			{
				hError = NX_PORT_SYSTEM_GetCountryCode(&eCurCountry);
				if (hError != ERR_OK)
				{
					eCurCountry = eCountryID_NONE;
					NX_APP_Error("Error!!! NX_PORT_SYSTEM_GetCountryCode() Fail \n");
				}
				pszStrId = NX_PRISM_UTIL_GetResStringIdByCountryID(eCurCountry);
				if (pszStrId != NULL)
				{
					HxSTD_StrNCpy((char*)strValue, (char*)ONDK_GetString(pszStrId), NX_PRISM_INSTALL_MAX_ELEMENT_TEXT_LENGTH);
				}
			}
			break;
		 case eNxInstallPage_Search:
			{
				if (TRUE == pstContents->bChSearchingCompleted)
				{
					lTvCount    = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_TV);
					lRadioCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_RADIO);
					HxSTD_snprintf(strValue, NX_PRISM_INSTALL_MAX_ELEMENT_TEXT_LENGTH,
						"%d(%s : %d, %s : %d)",(lTvCount+lRadioCount),(char*)ONDK_GetString(RES_TV_ID),lTvCount,(char*)ONDK_GetString(RES_RADIO_ID),lRadioCount);
				}
				else
				{
					HxSTD_StrNCpy(strValue, "0", NX_PRISM_INSTALL_MAX_ELEMENT_TEXT_LENGTH);
				}
			}
			break;
#if defined(CONFIG_MW_INET) || defined(CONFIG_MW_INET_WLAN)
        case eNxInstallPage_Network:
            {
                hError = NX_NETWORK_GetNetDevInfo(&pstNetDevInfoList, &devCount);
                if (ERR_FAIL == hError)
                {
                    HxSTD_StrNCpy(strValue, (char*)ONDK_GetString(RES_NOT_CONNECTED_ID), NX_PRISM_INSTALL_MAX_ELEMENT_TEXT_LENGTH);
                }

                for (i = 0; i < devCount; i++)
                {
                    if (TRUE == pstNetDevInfoList[i].bConnected)
                    {
                        bConnected = TRUE;
                        break;
                    }
                }

                if (TRUE == bConnected)
                {
                    HxSTD_StrNCpy(strValue, (char*)ONDK_GetString(RES_CONNECTED_DHCP_ID), NX_PRISM_INSTALL_MAX_ELEMENT_TEXT_LENGTH);
                }
                else
                {
                    HxSTD_StrNCpy(strValue, (char*)ONDK_GetString(RES_NOT_CONNECTED_ID), NX_PRISM_INSTALL_MAX_ELEMENT_TEXT_LENGTH);
                }
				if (pstNetDevInfoList)
	                NX_APP_Free(pstNetDevInfoList);
            }
            break;
#endif
		default:
			break;
	}

	HxSTD_StrNCpy(pStrRet, strValue, NX_PRISM_INSTALL_MAX_ELEMENT_TEXT_LENGTH);

	if (strValue)
	{
		NX_APP_Free(strValue);
		strValue = NULL;
	}
}

static ONDK_Result_t    nx_install_result_noui_SetFavouriteDefaultData(void)
{
	ONDK_Result_t   eResult = ONDK_RET_OK;
	HINT32  i = 0;

	for (i = 0 ; i < NX_SETTING_FAVOURITE_GROUP_NUM ; i++)
	{
		switch(i)
		{
			case 0:
				NX_CHANNEL_FAV_SetFavouriteName((NX_FavGroupID_e)eNxFavGroupID_User + i, (HCHAR*)ONDK_GetString(RES_FAVOURITE1_ID), FALSE);
				break;
			case 1:
				NX_CHANNEL_FAV_SetFavouriteName((NX_FavGroupID_e)eNxFavGroupID_User + i, (HCHAR*)ONDK_GetString(RES_FAVOURITE2_ID), FALSE);
				break;
			case 2:
				NX_CHANNEL_FAV_SetFavouriteName((NX_FavGroupID_e)eNxFavGroupID_User + i, (HCHAR*)ONDK_GetString(RES_FAVOURITE3_ID), FALSE);
				break;
			case 3:
				NX_CHANNEL_FAV_SetFavouriteName((NX_FavGroupID_e)eNxFavGroupID_User + i, (HCHAR*)ONDK_GetString(RES_FAVOURITE4_ID), FALSE);
				break;
			case 4:
				NX_CHANNEL_FAV_SetFavouriteName((NX_FavGroupID_e)eNxFavGroupID_User + i, (HCHAR*)ONDK_GetString(RES_FAVOURITE5_ID), FALSE);
				break;
		}
	}

	return eResult;
}

static ONDK_Result_t    nx_install_result_noui_SetFactoryDefaultData(void)
{
	ONDK_Result_t   eResult = ONDK_RET_OK;
	DxOperator_e    eOpType = eDxOPERATOR_None;

	// Operator Type
	NX_SYSTEM_GetOperatorType(&eOpType);
	switch(eOpType)
	{
		// TODO: Add Other Change Group for Operator if you need

		default:
			nx_install_result_noui_SetFavouriteDefaultData();
			break;
	}

	return eResult;
}

static ONDK_Result_t	nx_install_result_noui_MsgGwmCreate(Nx_Install_Result_NoUI_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;
	HUINT8		*pszPanelStr = NULL;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		ONDK_GWM_APP_Destroy(0);

		return ONDK_RET_INITFAIL;
	}

	eResult = nx_install_result_noui_InitApplication(pstContents,p1,p2,p3);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! (eResult != ONDK_RET_OK) \n");
		ONDK_GWM_APP_Destroy(0);
		return eResult;
	}

	pszPanelStr = NX_PRISM_SPEC_GetPanelString(eNX_PANEL_STR_Wizard);
	if (pszPanelStr == NULL)
	{
		NX_APP_Error("Error!! NX_PRISM_SPEC_GetPanelString() - eNX_PANEL_STR_Wizard !!!\n");
	}
	NX_PANEL_SetString(pszPanelStr, eDxPANEL_ALIGN_CENTER, FALSE);

	NX_SYSTEM_SetFirstTimeBoot(FALSE);
	nx_install_result_noui_SetFactoryDefaultData();
	if(TRUE == pstContents->bChSearchingCompleted)
	{
		ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_MgrInit_Proc", APP_UIMGR_PRIORITY, NX_PRISM_MgrInit_Proc, 0, p1, p2, p3);
	}else
	{
		//go to setting how???
		ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_MgrInit_Proc", APP_UIMGR_PRIORITY, NX_PRISM_MgrInit_Proc, 0, p1, p2, p3);
	}

	ONDK_GWM_APP_Destroy(0);

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	eResult;
}


static ONDK_Result_t	nx_install_result_noui_MsgGwmDestroy(Nx_Install_Result_NoUI_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
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
ONDK_Result_t	NX_PRISM_Install_Result_NoUI_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	Nx_Install_Result_NoUI_Context_t	*pstContents = NULL;
	pstContents = nx_install_result_noui_GetContents();
	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_install_result_noui_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_DESTROY:
			eResult = nx_install_result_noui_MsgGwmDestroy(pstContents, p1, p2, p3);
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

