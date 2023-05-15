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
#include <nx_prism_install_sub_result.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	NX_PRISM_INSTALL_RESULT_ITEM_ID			        ((NX_PRISM_INSTALL_ITEM_ID)+1200)

#define NX_PRISM_INSTALL_RESULT_TEXT_EXPLAIN_ID         ((NX_PRISM_INSTALL_RESULT_ITEM_ID)+1)
#define	NX_PRISM_INSTALL_RESULT_BTN_ID			        ((NX_PRISM_INSTALL_RESULT_ITEM_ID)+10)
#define	NX_PRISM_INSTALL_RESULT_BTN_PREVIOUS_ID		    ((NX_PRISM_INSTALL_RESULT_BTN_ID)+0)
#define	NX_PRISM_INSTALL_RESULT_BTN_NEXT_ID			    ((NX_PRISM_INSTALL_RESULT_BTN_ID)+1)

#define	NX_PRISM_INSTALL_RESULT_BOX_CONTEXT_ID		    ((NX_PRISM_INSTALL_RESULT_ITEM_ID)+20)
#define	NX_PRISM_INSTALL_RESULT_TEXT_CONTEXT_ID		    ((NX_PRISM_INSTALL_RESULT_BOX_CONTEXT_ID)+1)//+n
#define	NX_PRISM_INSTALL_RESULT_TEXT_VALUE_ID		    ((NX_PRISM_INSTALL_RESULT_BOX_CONTEXT_ID)+20)

#define NX_PRISM_INSTALL_RESULT_BTN_X                    380
#define NX_PRISM_INSTALL_RESULT_BTN_Y                    615
#define NX_PRISM_INSTALL_RESULT_BTN_W                    252
#define NX_PRISM_INSTALL_RESULT_BTN_H                    55
#define NX_PRISM_INSTALL_RESULT_BTN_GAP                  17


#define NX_PRISM_INSTALL_RESULT_TEXT_EXPLAIN_X           350
#define NX_PRISM_INSTALL_RESULT_TEXT_EXPLAIN_Y           144
#define NX_PRISM_INSTALL_RESULT_TEXT_EXPLAIN_W           600
#define NX_PRISM_INSTALL_RESULT_TEXT_EXPLAIN_H           200

#define	NX_PRISM_INSTALL_RESULT_BOX_CONTEXT_X		     254
#define	NX_PRISM_INSTALL_RESULT_BOX_CONTEXT_Y		     340//
#define	NX_PRISM_INSTALL_RESULT_BOX_CONTEXT_W		     773
#define	NX_PRISM_INSTALL_RESULT_BOX_CONTEXT_H		     174
#define	NX_PRISM_INSTALL_RESULT_BOX_CONTEXT_H_GAP	     52

#define	NX_PRISM_INSTALL_RESULT_TEXT_CONTEXT_X		     20
#define	NX_PRISM_INSTALL_RESULT_TEXT_CONTEXT_Y		     13//26
#define	NX_PRISM_INSTALL_RESULT_TEXT_CONTEXT_W		     356//356
#define	NX_PRISM_INSTALL_RESULT_TEXT_CONTEXT_H		     50

#define	NX_PRISM_INSTALL_RESULT_TEXT_VALUE_X		     396
#define	NX_PRISM_INSTALL_RESULT_TEXT_VALUE_Y		     (NX_PRISM_INSTALL_RESULT_TEXT_CONTEXT_Y)
#define	NX_PRISM_INSTALL_RESULT_TEXT_VALUE_W		     (NX_PRISM_INSTALL_RESULT_TEXT_CONTEXT_W)
#define	NX_PRISM_INSTALL_RESULT_TEXT_VALUE_H		     (NX_PRISM_INSTALL_RESULT_TEXT_CONTEXT_H)

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef struct tagInstall_Result_Context {
	HINT32		lCurFocus;
	HUINT32		ulNumOfMainItem, ulMaxOfMainItem;
	Nx_InstallSubPage_t	*pstSubItems;

	HINT32		lDefaultFocusIdx;
	HUINT8		ucTitle[NX_PRISM_INSTALL_MAX_ELEMENT_TEXT_LENGTH];
	HUINT8      ulNumOfSubPage;// Nx_InstallSubPage_t sub page number
	HBOOL       bChSearchingCompleted;
	NxInstallPage_t ePage;
} Nx_Install_Result_Context_t;

/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

static Nx_Install_Result_Context_t		s_stInstallSearchContent;

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static ONDK_Result_t	                    nx_install_result_MsgGwmCreate(Nx_Install_Result_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	                    nx_install_result_MsgGwmDestroy(Nx_Install_Result_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	                    nx_install_result_MsgGwmHapiMsg(Nx_Install_Result_Context_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	                    nx_install_result_MsgGwmKeyDown(Nx_Install_Result_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

static inline Nx_Install_Result_Context_t	*nx_install_result_GetContents(void);
static ONDK_Result_t	                    nx_install_result_InitApplication (Nx_Install_Result_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static void                                 nx_install_Result_DrawExplain(Nx_Install_Result_Context_t *pstContents);
static void                                 nx_install_Result_DrawResultContext(Nx_Install_Result_Context_t *pstContents);
static void                                 nx_install_Result_DrawDirectionBtn(Nx_Install_Result_Context_t *pstContents);
static ONDK_Result_t	                    nx_install_Result_DrawDefaultUI(Nx_Install_Result_Context_t *pstContents);
static ONDK_Result_t                        nx_install_Result_SetFavouriteDefaultData(void);
static ONDK_Result_t                        nx_install_Result_SetFactoryDefaultData(void);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static inline Nx_Install_Result_Context_t	*nx_install_result_GetContents(void)
{
	return	&s_stInstallSearchContent;
}

static ONDK_Result_t	nx_install_result_InitApplication (Nx_Install_Result_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	Nx_InstallSubPage_t	*pstSubItemArray = NULL;
	HUINT8 ulNumOfSubPage = 0;
	HINT32 lTvCount = 0, lRadioCount = 0;

	if (pstContents == NULL)
	{
		return GWM_RET_SOURCE_NULL;
	}

	HxSTD_MemSet(pstContents, 0, sizeof(Nx_Install_Result_Context_t));

	pstContents->ePage = eNxInstallPage_Result;

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


static void nx_install_Result_DrawExplain(Nx_Install_Result_Context_t *pstContents)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
	HCHAR*          strContext = NULL;
	HUINT32         ulId = (NX_PRISM_INSTALL_RESULT_TEXT_EXPLAIN_ID);

	stRect = ONDK_Rect(NX_PRISM_INSTALL_RESULT_TEXT_EXPLAIN_X,
	NX_PRISM_INSTALL_RESULT_TEXT_EXPLAIN_Y,
	NX_PRISM_INSTALL_RESULT_TEXT_EXPLAIN_W,
	NX_PRISM_INSTALL_RESULT_TEXT_EXPLAIN_H);

	if(TRUE == pstContents->bChSearchingCompleted)
	{
		strContext = (HCHAR*)ONDK_GetString(RES_MESG_3681_ID);
	}
    else
	{
#if defined(CONFIG_PROD_HA7100S)
    strContext = (HCHAR *)ONDK_GetString(RES_MESG_4190_ID);
#else
    strContext = (HCHAR *)ONDK_GetString(RES_MESG_3682_ID);
#endif
	}

	ONDK_GWM_COM_Text_Create(ulId, &stRect, strContext);
	ONDK_GWM_COM_Text_SetFont(ulId, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(ulId, NX_PRISM_FONT_SIZE_24);
	ONDK_GWM_COM_Text_SetAlign(ulId,TEXT_WORDWRAP | TEXT_MULTILINED | TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_ChangeColor(ulId, COL(C_T_Gong01), COL(C_T_Gong01));
}

static void		nx_install_Result_GetPageResultValue(Nx_Install_Result_Context_t *pstContents, NxInstallPage_t ePage,HCHAR* pStrRet)
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

static void nx_install_Result_DrawResultContext(Nx_Install_Result_Context_t *pstContents)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
	HUINT32         ulId = 0;
	HUINT32         ulNumOfSubPage = 0, ulIndexOfValueUI = 0,ulNumOfValueUI = 0;
	HUINT8           zIndex =0;
	Nx_InstallSubPage_t	*pstSubPage;
	NxInstallPage_t ePageName;
	HCHAR*          strContext = NULL;
	HCHAR*          strTemp= NULL;
	HCHAR*          strValue = NULL;
    HUINT32         x = 0,y = 0,w = 0,h =0;

	if (pstContents == NULL)
	{
		return;
	}

	ulNumOfSubPage = pstContents->ulNumOfSubPage;

	for(zIndex =0 ; zIndex < ulNumOfSubPage ;zIndex++)
	{
	    pstSubPage = &(pstContents->pstSubItems[zIndex]);
	    if(nx_install_ui_isAvailableIndicatorStep(pstSubPage->ePageName))
        {
            ulNumOfValueUI++;
        }
    }

	//BOX

    x = NX_PRISM_INSTALL_RESULT_BOX_CONTEXT_X;
    w = NX_PRISM_INSTALL_RESULT_BOX_CONTEXT_W;

    if(ulNumOfValueUI<=3)
    {
        y = NX_PRISM_INSTALL_RESULT_BOX_CONTEXT_Y;
        h = NX_PRISM_INSTALL_RESULT_BOX_CONTEXT_H;
    }
    else
    {
        y = NX_PRISM_INSTALL_RESULT_BOX_CONTEXT_Y-(((ulNumOfValueUI-3)*NX_PRISM_INSTALL_RESULT_BOX_CONTEXT_H_GAP)/2);
        h = NX_PRISM_INSTALL_RESULT_BOX_CONTEXT_H+((ulNumOfValueUI-3)*NX_PRISM_INSTALL_RESULT_BOX_CONTEXT_H_GAP);
    }

	stRect = ONDK_Rect(x,y,w,h);

	ulId = NX_PRISM_INSTALL_RESULT_BOX_CONTEXT_ID;

	ONDK_GWM_COM_Rect_Create(ulId, &stRect, COL(C_G_Bang_50));
	ONDK_GWM_COM_Rect_SetStroke(ulId,1,COL(C_G_Whistle_15));


	for(zIndex =0 ; zIndex < ulNumOfSubPage ;zIndex++)
	{
		pstSubPage = &(pstContents->pstSubItems[zIndex]);

	    if(nx_install_ui_isAvailableIndicatorStep(pstSubPage->ePageName))
        {
    		strTemp = HxSTR_MemAlloc(sizeof(HUINT8)*NX_PRISM_INSTALL_MAX_ELEMENT_TEXT_LENGTH);
    		HxSTD_MemSet(strTemp, 0x00, NX_PRISM_INSTALL_MAX_ELEMENT_TEXT_LENGTH);

    		//Context

    		ulId = (NX_PRISM_INSTALL_RESULT_TEXT_CONTEXT_ID) + ulIndexOfValueUI;
    		ePageName = pstSubPage->ePageName;
    		HxSTD_MemSet(strTemp, 0x00, MAX_ELEMENT_TEXT_LENGTH);

    		switch(ePageName){
    			case eNxInstallPage_Language:
    				HxSTD_snprintf(strTemp, MAX_ELEMENT_TEXT_LENGTH,"%s",(HCHAR*)ONDK_GetString(RES_SELECTED_LANGUAGE_ID));
    				break;
    			case eNxInstallPage_Search:
    				HxSTD_snprintf(strTemp, MAX_ELEMENT_TEXT_LENGTH,"%s",(HCHAR*)ONDK_GetString(RES_SEARCHED_CHANNEL_ID));
    				break;
#if defined(CONFIG_MW_INET) || defined(CONFIG_MW_INET_WLAN)
                case eNxInstallPage_Network:
                    HxSTD_snprintf(strTemp, MAX_ELEMENT_TEXT_LENGTH,"%s",(HCHAR*)ONDK_GetString(RES_NETWORK_CONNECTION_ID));
                    break;
#endif
    			default:
    				HxSTD_snprintf(strTemp, MAX_ELEMENT_TEXT_LENGTH,"%s %s",(HCHAR*)ONDK_GetString(RES_SELECTED_ID), pstSubPage->ucTitle);
    				break;
    		}

    		strContext = strTemp;

            if(ulNumOfValueUI<=3)
            {
              y = (NX_PRISM_INSTALL_RESULT_BOX_CONTEXT_Y)+((NX_PRISM_INSTALL_RESULT_TEXT_CONTEXT_Y)+(NX_PRISM_INSTALL_RESULT_TEXT_CONTEXT_H)*(ulIndexOfValueUI));
            }
            else
            {
              y = (NX_PRISM_INSTALL_RESULT_BOX_CONTEXT_Y)+((NX_PRISM_INSTALL_RESULT_TEXT_CONTEXT_Y)+(NX_PRISM_INSTALL_RESULT_TEXT_CONTEXT_H)*(ulIndexOfValueUI))
    			-(((ulNumOfValueUI-3)*NX_PRISM_INSTALL_RESULT_BOX_CONTEXT_H_GAP)/2);
            }

    		stRect = ONDK_Rect(
    			(NX_PRISM_INSTALL_RESULT_BOX_CONTEXT_X)+(NX_PRISM_INSTALL_RESULT_TEXT_CONTEXT_X),
                y,
    			(NX_PRISM_INSTALL_RESULT_TEXT_CONTEXT_W),
    			(NX_PRISM_INSTALL_RESULT_TEXT_CONTEXT_H));

    		ONDK_GWM_COM_Text_Create(ulId, &stRect, strContext);
    		ONDK_GWM_COM_Text_SetFont(ulId, eFont_SystemNormal);
    		ONDK_GWM_COM_Text_SetSize(ulId, NX_PRISM_FONT_SIZE_24);
    		ONDK_GWM_COM_Text_SetAlign(ulId,TEXT_ALIGN_LEFT | TEXT_ALIGN_MIDDLE);
    		ONDK_GWM_COM_Text_ChangeColor(ulId, COL(C_T_Gong03), COL(C_T_Gong03));

    		//Value
    		strValue = NULL;
    		strValue = HxSTR_MemAlloc(sizeof(HUINT8)*NX_PRISM_INSTALL_MAX_ELEMENT_TEXT_LENGTH);
    		HxSTD_MemSet(strValue, 0x00, NX_PRISM_INSTALL_MAX_ELEMENT_TEXT_LENGTH);

    		ulId = (NX_PRISM_INSTALL_RESULT_TEXT_VALUE_ID) + ulIndexOfValueUI;
    		ePageName = pstSubPage->ePageName;
    		nx_install_Result_GetPageResultValue(pstContents,ePageName, strValue);

            if(ulNumOfValueUI<=3)
            {
              y = (NX_PRISM_INSTALL_RESULT_BOX_CONTEXT_Y)+((NX_PRISM_INSTALL_RESULT_TEXT_VALUE_Y)+(NX_PRISM_INSTALL_RESULT_TEXT_VALUE_H)*(ulIndexOfValueUI));
            }
            else
            {
                y = (NX_PRISM_INSTALL_RESULT_BOX_CONTEXT_Y)+((NX_PRISM_INSTALL_RESULT_TEXT_VALUE_Y)+(NX_PRISM_INSTALL_RESULT_TEXT_VALUE_H)*(ulIndexOfValueUI))
    			-(((ulNumOfValueUI-3)*NX_PRISM_INSTALL_RESULT_BOX_CONTEXT_H_GAP)/2);
            }


    		stRect = ONDK_Rect(
    			(NX_PRISM_INSTALL_RESULT_BOX_CONTEXT_X)+(NX_PRISM_INSTALL_RESULT_TEXT_VALUE_X),
                y,
    			(NX_PRISM_INSTALL_RESULT_TEXT_VALUE_W),
    			(NX_PRISM_INSTALL_RESULT_TEXT_VALUE_H));

    		ONDK_GWM_COM_Text_Create(ulId, &stRect, strValue);
    		ONDK_GWM_COM_Text_SetFont(ulId, eFont_SystemNormal);
    		ONDK_GWM_COM_Text_SetSize(ulId, NX_PRISM_FONT_SIZE_24);
    		ONDK_GWM_COM_Text_SetAlign(ulId,TEXT_ALIGN_RIGHT | TEXT_ALIGN_MIDDLE);
    		ONDK_GWM_COM_Text_ChangeColor(ulId, COL(C_T_Whistle_100), COL(C_T_Whistle_100));
            ulIndexOfValueUI++;
        }
	}
}

static void nx_install_Result_DrawDirectionBtn(Nx_Install_Result_Context_t *pstContents)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
	HUINT32         ulId = 0;

	//PREVIOUS
	ulId = (NX_PRISM_INSTALL_RESULT_BTN_PREVIOUS_ID);
	stRect = ONDK_Rect((NX_PRISM_INSTALL_RESULT_BTN_X), NX_PRISM_INSTALL_RESULT_BTN_Y , NX_PRISM_INSTALL_RESULT_BTN_W, NX_PRISM_INSTALL_RESULT_BTN_H);
	ONDK_GWM_COM_Button_Create (ulId, &stRect, (char*)ONDK_GetString(RES_PREVIOUS_01_ID));

	ONDK_GWM_COM_Button_SetFontColor(ulId, COL(C_G_Whistle_100), COL(C_G_Bang_100));
	ONDK_GWM_COM_Button_SetFont(ulId, eFont_SystemBold);
	ONDK_GWM_COM_Button_SetSimpleButton(ulId,TRUE);
	ONDK_GWM_COM_Button_SetButtonColor(ulId,  COL(C_G_Base01_100), COL(C_G_Bang_100),  COL(C_Table_MAX),  COL(C_Table_MAX));
	ONDK_GWM_COM_Button_SetButtonDimmedAlpha(ulId,100);

	ONDK_GWM_COM_Button_SetTitleSize(ulId,NX_PRISM_FONT_SIZE_24);
	ONDK_GWM_COM_Button_SetAlign(ulId,ONDKSTF_CENTER);
	ONDK_GWM_Obj_SetVisible(ulId, TRUE);
	ONDK_GWM_COM_Button_SetStatus(ulId,GUIOBJ_NORMAL);


	//NEXT
	ulId = (NX_PRISM_INSTALL_RESULT_BTN_NEXT_ID);
	stRect = ONDK_Rect(
		(NX_PRISM_INSTALL_RESULT_BTN_X)+(NX_PRISM_INSTALL_RESULT_BTN_W)+(NX_PRISM_INSTALL_RESULT_BTN_GAP)
		, NX_PRISM_INSTALL_RESULT_BTN_Y, NX_PRISM_INSTALL_RESULT_BTN_W, NX_PRISM_INSTALL_RESULT_BTN_H);

	if(TRUE == pstContents->bChSearchingCompleted)
	{
		//EXIT
		ONDK_GWM_COM_Button_Create (ulId, &stRect, (char*)ONDK_GetString(RES_EXIT_01_ID));
	}else{
		//NEXT
		ONDK_GWM_COM_Button_Create (ulId, &stRect, (char*)ONDK_GetString(RES_NEXT_02_ID));
	}

	ONDK_GWM_COM_Button_SetFontColor(ulId, COL(C_G_Whistle_100), COL(C_G_Bang_100));
	ONDK_GWM_COM_Button_SetFont(ulId, eFont_SystemBold);
	ONDK_GWM_COM_Button_SetSimpleButton(ulId,TRUE);
	ONDK_GWM_COM_Button_SetButtonColor(ulId,  COL(C_G_Base01_100), COL(C_G_Bang_100), COL(C_Table_MAX), COL(C_Table_MAX));
	ONDK_GWM_COM_Button_SetButtonDimmedAlpha(ulId,100);

	ONDK_GWM_COM_Button_SetTitleSize(ulId,NX_PRISM_FONT_SIZE_24);
	ONDK_GWM_COM_Button_SetAlign(ulId,ONDKSTF_CENTER);
	ONDK_GWM_Obj_SetVisible(ulId, TRUE);
	ONDK_GWM_COM_Button_SetStatus(ulId,GUIOBJ_HILIGHTED);
	ONDK_GWM_Obj_SetFocus(ulId);
}


static ONDK_Result_t	nx_install_Result_DrawDefaultUI(Nx_Install_Result_Context_t *pstContents)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;
	HUINT8          ulPageIndex = 0;
	HUINT8          ulNumofPages = 0;
	HUINT8          ulIndex = 0;
	Nx_InstallSubPage_t	*pstSubItem;

	for(ulIndex = 0; ulIndex < pstContents->ulNumOfSubPage ; ulIndex++)
	{
		pstSubItem = &(pstContents->pstSubItems[ulIndex]);
		if(pstSubItem->ePageName == pstContents->ePage){
			ulPageIndex = ulIndex;
			break;
		}
	}

	ulNumofPages = pstContents->ulNumOfSubPage;

	nx_install_ui_DrawBackgroundUI();
	nx_install_ui_DrawTitle((HUINT8*)ONDK_GetString(RES_RESULT_01_ID));
	nx_install_Result_DrawExplain(pstContents);

	nx_install_Result_DrawResultContext(pstContents);

	//Previous Next, Exit Btn
	nx_install_Result_DrawDirectionBtn(pstContents);
	return eResult;
}

static ONDK_Result_t    nx_install_Result_SetFavouriteDefaultData(void)
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

static ONDK_Result_t    nx_install_Result_SetFactoryDefaultData(void)
{
	ONDK_Result_t   eResult = ONDK_RET_OK;
	DxOperator_e    eOpType = eDxOPERATOR_None;

	// Operator Type
	NX_SYSTEM_GetOperatorType(&eOpType);
	switch(eOpType)
	{
		// TODO: Add Other Change Group for Operator if you need

		default:
			nx_install_Result_SetFavouriteDefaultData();
			break;
	}

	return eResult;
}

static ONDK_Result_t	nx_install_result_MsgGwmCreate(Nx_Install_Result_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;
	HUINT8		*pszPanelStr = NULL;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		ONDK_GWM_APP_Destroy(0);

		return ONDK_RET_INITFAIL;
	}

	eResult = nx_install_result_InitApplication(pstContents,p1,p2,p3);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! (eResult != ONDK_RET_OK) \n");
		ONDK_GWM_APP_Destroy(0);
		return eResult;
	}

	nx_install_Result_DrawDefaultUI(pstContents);

	pszPanelStr = NX_PRISM_SPEC_GetPanelString(eNX_PANEL_STR_Wizard);
	if (pszPanelStr == NULL)
	{
		NX_APP_Error("Error!! NX_PRISM_SPEC_GetPanelString() - eNX_PANEL_STR_Wizard !!!\n");
	}
	NX_PANEL_SetString(pszPanelStr, eDxPANEL_ALIGN_CENTER, FALSE);

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	eResult;
}


static ONDK_Result_t	nx_install_result_MsgGwmDestroy(Nx_Install_Result_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_install_result_MsgGwmHapiMsg(Nx_Install_Result_Context_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);
	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_install_result_MsgGwmKeyDown(Nx_Install_Result_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1;
	Nx_InstallSubPage_t *pstSubItem = NULL;
	HINT8           zIndex, zCurrentStep = 0;
	HINT32          lNumOfPages = 0;
	HINT32      lObjId;
	ONDK_Result_t	eResult = ONDK_RET_OK;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

    //Check common key
    {
        ONDK_Result_t ret = ONDK_MESSAGE_PASS;
        ret = nx_install_ui_MsgGwmKeyDown(p1,p2,p3);
        if(ret != ONDK_MESSAGE_PASS)
        {
            return ret;
        }
    }

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	lObjId = GWM_GetFocusedObjectID();
	lNumOfPages = pstContents->ulNumOfSubPage;
	for(zIndex=0;zIndex<lNumOfPages;zIndex++)
	{
		pstSubItem = &(pstContents->pstSubItems[zIndex]);
		if(pstContents->ePage == pstSubItem->ePageName){
			zCurrentStep = zIndex;
		}
	}

	switch (lKeyId)
	{
		/* keys to exit */
		case KEY_BACK:
			{
				//move to previous Step
				eResult = nx_install_ui_DoNextStep(pstContents->ePage, eNxInstallPage_PreviousPage, p1, p2, p3);
				if(ONDK_RET_OK == eResult)
				{
					ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)NX_PRISM_Install_Result_Proc);
				}
			}
			return ONDK_MESSAGE_BREAK;
		case KEY_EXIT:
		case KEY_TVRADIO:
		case KEY_VFORMAT:
		case KEY_AUDIO:
		case KEY_SUBTITLE:
		case KEY_TEXT:
		case KEY_MENU:
		case KEY_GUIDE:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;
		case KEY_OK:
			 {
				//Do something
				switch(lObjId){
					case NX_PRISM_INSTALL_RESULT_BTN_PREVIOUS_ID:
						eResult = nx_install_ui_DoNextStep(pstContents->ePage, eNxInstallPage_PreviousPage, p1, p2, p3);
						break;
					case NX_PRISM_INSTALL_RESULT_BTN_NEXT_ID:
						NX_SYSTEM_SetFirstTimeBoot(FALSE);
						nx_install_Result_SetFactoryDefaultData();
						if(TRUE == pstContents->bChSearchingCompleted)
						{
							ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_MgrInit_Proc", APP_UIMGR_PRIORITY, NX_PRISM_MgrInit_Proc, 0, p1, p2, p3);
						}else
						{
							//go to setting how???
							ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_MgrInit_Proc", APP_UIMGR_PRIORITY, NX_PRISM_MgrInit_Proc, 0, p1, p2, p3);
						}
						break;
				}
				if(ONDK_RET_OK == eResult){
					ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)NX_PRISM_Install_Result_Proc);
				}
			 }
			return	ONDK_MESSAGE_BREAK;
		case KEY_ARROWLEFT:
		case KEY_ARROWRIGHT:
		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
			break;
		case KEY_STANDBY:
            ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_STANDBY_Proc", APP_DEFAULT_POPUP_PRIORITY, NX_PRISM_STANDBY_Proc, 0, 0, 0, 0);
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_BREAK;
		default:
			NX_APP_Print("nx_menuSettings_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_Install_Result_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	Nx_Install_Result_Context_t	*pstContents = NULL;
	pstContents = nx_install_result_GetContents();
	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_install_result_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_KEYDOWN:
			eResult = nx_install_result_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_OBJ_FOCUS_CHANGED:
			break;

		case MSG_GWM_SCROLLUP:
			return	ONDK_MESSAGE_BREAK;

		case MSG_GWM_SCROLLDOWN:
			return	ONDK_MESSAGE_BREAK;

		case MSG_GWM_CLICKED:
		case MSG_GWM_SCROLLRIGHT:
			break;

		case MSG_GWM_DESTROY:
			eResult = nx_install_result_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			eResult = nx_install_result_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
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

