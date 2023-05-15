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

/**************************************************************************************************/
#define _________AP_Teletext_Private_Include_________________________________________________
/**************************************************************************************************/
#include <prism.h>
#include <teletext.h>


/**************************************************************************************************/
#define _________AP_Teletext_Private_Macro_________________________________________________
/**************************************************************************************************/

#define TTX_DEFAULT_MAGAZINE_NUM		    1
#define TTX_DEFAULT_PAGE_NUM			    0
#define TTX_DEFAULT_SUBCODE				0

#define TTX_OBJECT_ID					    0

#define MAX_TTX_INITAIL_PAGE			    8				/* MAX_TTX_INITAIL_PAGE 의 ES 개수와 맞춘다. */

/**************************************************************************************************/
#define _________AP_Teletext_Private_Struct_________________________________________________
/**************************************************************************************************/
typedef struct
{
	HBOOL				bHistory;
	HUINT8				ucLastMagazine;
	HUINT8				ucLastPageNum;
	HUINT16				usLastSubcode;
	HBOOL				bShowSuccess;
	TTX_Mix_t			stMix;			/**< Mix 상태 */
	TTX_DoubleSize_t	stDoubleSize;	/**< Double Size 상태 */
	HBOOL				bRunning;		/**< TTX 동작 상태 */
	HBOOL				bIsEbuSbt;		/**< EBU subtitle */
} AP_TtxContent_t;


/**@Brief TTX Initail Page Info */
typedef struct
{
	HUINT8		aucLangCode [4];		/**< Language Code */
	HUINT8		ucMgNum;				/**< Magazine Number */
	HUINT8		ucPageNum;				/**< Page Number */
} TTX_InitPage_t;

typedef struct tagTTX_INFO_LIST
{
	HUINT32			ulInitPageNum;
	TTX_InitPage_t	stTtxInitPage[MAX_TTX_INITAIL_PAGE];
} TTX_InfoList_t;


/**************************************************************************************************/
#define _________AP_Teletext_Private_Value_________________________________________________
/**************************************************************************************************/
static AP_TtxContent_t	s_stTtxContents;	/* TTX Control Proc Contents. */

/**************************************************************************************************/
#define _________AP_Teletext_Private_Prototype_________________________________________________
/**************************************************************************************************/
static AP_TtxContent_t *   local_ttxctrl_GetTtxContent(void);
static ONDK_Result_t		local_ttxctrl_Initialize(AP_TtxContent_t *pstContents);
static HERROR              local_ttxctrl_GetInitPage(AP_TtxContent_t *pContents, HUINT8 *pucM, HUINT8 *pucX);
static HERROR              local_ttxctrl_GetInitPageFromSI (TTX_InfoList_t *pTtxList, HxLANG_Id_e  eMenuLangId, HUINT8 *pucM, HUINT8 *pucX);

static void                local_ttxctrl_deliveryPacket(HINT32 size, void *pvData);

static HERROR              local_ttxctrl_ShowTtx (AP_TtxContent_t *pContents, HUINT8 ucM, HUINT8 ucX, HUINT16 usSubcode);
static ONDK_Result_t       local_ttxctrl_Invalidate(AP_TtxContent_t *pContents,Handle_t handle);



static void                local_ttxctrl_KeyConvertion (HINT32 *nKeyCode);
static void	            local_ttxctrl_SetNavigationType (AP_TtxContent_t *pContents, HxCountry_e eCountry);



#if defined(CONFIG_UI_ALVIN) || defined(CONFIG_OP_NTVPLUS) || defined(CONFIG_OP_MOSTELECOM)|| defined(CONFIG_OP_GREECE_NOVA)|| defined(CONFIG_OP_ERTELECOM)
static void local_ttxctrl_SetLanguageGroup (AP_TtxContent_t *pContents, HxCountry_e eCountry);
#endif


/**************************************************************************************************/
#define _________AP_Teletext_Private_Init__________________________________________________________
/**************************************************************************************************/
AP_TtxContent_t * local_ttxctrl_GetTtxContent(void)
{
    return &s_stTtxContents;
}

static HERROR local_ttxctrl_InitTtxProc(AP_TtxContent_t *pContents)
{
	HERROR			err;
	HUINT8			ucM			= TTX_NULL_MAGAZINE;
	HUINT8			ucX			= TTX_NULL_PAGE;
	HUINT16			usSubcode	= TTX_NULL_SUBCODE;

	if (pContents->bHistory == TRUE)	// 마지막 페이지를 기억하고 있음
	{
		ucM			= pContents->ucLastMagazine;
		ucX			= pContents->ucLastPageNum;
		usSubcode	= pContents->usLastSubcode;
	}
	else
	{
		err = local_ttxctrl_GetInitPage(pContents, &ucM, &ucX);
		if (err != ERR_OK)
		{
			return ERR_FAIL;
		}
	}

	return local_ttxctrl_ShowTtx(pContents, ucM, ucX, usSubcode);
}


ONDK_Result_t		local_ttxctrl_Initialize(AP_TtxContent_t *pstContents)
{
	ONDK_Assert(pstContents);
    HxSTD_MemSet(pstContents, 0, sizeof(AP_TtxContent_t));

	return GWM_RET_OK;
}

/**************************************************************************************************/
#define _________AP_Teletext_Private_Update__________________________________________________________
/**************************************************************************************************/
HERROR local_ttxctrl_ShowTtx (AP_TtxContent_t *pContents, HUINT8 ucM, HUINT8 ucX, HUINT16 usSubcode)
{
	HERROR				hErr;
	HxCountry_e		    eCountry;

    hErr = APK_SCENARIO_GetCountryCode(&eCountry);

	if (hErr != ERR_OK)
	{
		eCountry	= eCountryID_NONE;
	}

	pContents->bRunning	= TRUE;

	/* TTX Start을 알린다. */
	ONDK_GWM_SendMessage(NULL, MSG_APP_TELETEXT_STARTED, HANDLE_NULL, 0, 0, 0);

	Teletext_Show( ucM, ucX, usSubcode, pContents->bIsEbuSbt);

    //수정이 필요해.
	local_ttxctrl_SetNavigationType (pContents, eCountry);

#if defined(CONFIG_UI_ALVIN) || defined(CONFIG_OP_NTVPLUS) || defined(CONFIG_OP_MOSTELECOM) || defined(CONFIG_OP_GREECE_NOVA)|| defined(CONFIG_OP_ERTELECOM)
	local_ttxctrl_SetLanguageGroup (pContents, eCountry);
#endif
	return ERR_OK;
}


static ONDK_Result_t local_ttxctrl_MsgEvtShowFail(AP_TtxContent_t *pContents)
{
	ONDK_GWM_APP_Destroy(0);

	return GWM_MESSAGE_BREAK;
}


ONDK_Result_t       local_ttxctrl_Invalidate(AP_TtxContent_t *pContents,Handle_t handle)
{
#if 0
    ONDKSurfaceHandle hsurface =NULL;
    hsurface= (ONDKSurfaceHandle)handle;
    if(ONDK_GWM_APP_Get(AP_TtxMenuUi_Proc) == NULL && hsurface != NULL)
    {
        ONDK_WINDOW_StretchBlit(g_hTeletextWinHandle,hsurface,
                                     ONDK_Rect(0,0,TELETEXT_SCREENWIDTH,TELETEXT_SCREENHEIGHT),
                                     ONDK_Rect(0,0,GWM_SCREEN_WIDTH,GWM_SCREEN_HEIGHT));
        ONDK_WINDOW_Update(g_hTeletextWinHandle,  NULL);
        return GWM_MESSAGE_BREAK;
    }
	#endif
    return ONDK_RET_OK;
}


/**************************************************************************************************/
#define _________AP_Teletext_Private_Process__________________________________________________________
/**************************************************************************************************/




void local_ttxctrl_KeyConvertion (HINT32 *nKeyCode)
{
	switch(*nKeyCode)
	{
		case KEY_FRONT_STANDBY:
			*nKeyCode = KEY_STANDBY;
			break;

		case KEY_FRONT_EXT_AV:
			*nKeyCode = KEY_BACK;
			break;

		case KEY_FRONT_MENU:
			*nKeyCode = KEY_OK;
			break;

		case KEY_FRONT_CH_PLUS:
			*nKeyCode = KEY_ARROWUP;
			break;

		case KEY_FRONT_CH_MINUS:
			*nKeyCode = KEY_ARROWDOWN;
			break;

		case KEY_FRONT_VOLUME_UP:
			*nKeyCode = KEY_ARROWRIGHT;
			break;

		case KEY_FRONT_VOLUME_DOWN:
			*nKeyCode = KEY_ARROWLEFT;
			break;

		default:
			break;
	}
}

void	local_ttxctrl_SetNavigationType (AP_TtxContent_t *pContents, HxCountry_e eCountry)
{
	TTX_Navigation_t	eTtxNavi;

    switch (eCountry)
    {
    //2 TTX TOP를 사용하는 나라를 아래에 명시
#if defined(CONFIG_MW_TTX_TOP)
    case eCountryID_AUT:
    case eCountryID_DEU:
    case eCountryID_ITA:
    case eCountryID_CHE:
        eTtxNavi    = eTTX_NAVI_TOP;
        break;
#endif
    //2 TTX FLOF을 우선으로 사용하는 나라 아래에 명시
    case eCountryID_BEL:
    case eCountryID_CZE:
    case eCountryID_GBR:
    case eCountryID_FRA:
    case eCountryID_GRC:
    case eCountryID_LUX:
    case eCountryID_NLD:
    case eCountryID_POL:
    case eCountryID_PRT:
    case eCountryID_ROU:
    case eCountryID_ESP:
    default:
        eTtxNavi    = eTTX_NAVI_FLOF;
        break;
    }


	Teletext_SetControl( eTTX_CONTROL_NAVI, (HUINT32)eTtxNavi);
}

#if defined(CONFIG_UI_ALVIN) || defined(CONFIG_OP_NTVPLUS) || defined(CONFIG_OP_MOSTELECOM)|| defined(CONFIG_OP_GREECE_NOVA)|| defined(CONFIG_OP_ERTELECOM)
void	local_ttxctrl_SetLanguageGroup (AP_TtxContent_t *pContents, HxCountry_e eCountry)
{
	TTX_Language_t	eLangGroup;

	switch (eCountry)
	{
    case eCountryID_AUT:
    case eCountryID_BEL:
    case eCountryID_DNK:
    case eCountryID_GBR:
    case eCountryID_FIN:
    case eCountryID_FRA:
    case eCountryID_DEU:
    case eCountryID_GRC:
    case eCountryID_HUN:
    case eCountryID_ITA:
    case eCountryID_LUX:
    case eCountryID_NLD:
    case eCountryID_NOR:
    case eCountryID_PRT:
    case eCountryID_ESP:
    case eCountryID_SWE:
    case eCountryID_CHE:
        eLangGroup  = eTTX_LANG_LATIN_WEST_EU;
        break;

    case eCountryID_HRV:
    case eCountryID_CZE:
    case eCountryID_POL:
    case eCountryID_ROU:
    //case eCountryID_SRB:
    case eCountryID_SVN:
        eLangGroup  = eTTX_LANG_LATIN_EAST_EU;
        break;
    //case eCountry_BULGARIA:
    case eCountryID_RUS:
        eLangGroup  = eTTX_LANG_CYRILLIC;
        break;

    //case eCountry_MOROCCO:
    case eCountryID_TUR:
        eLangGroup  = eTTX_LANG_HEBREW_ARABIC;
        break;

    default:
        return; //2 Language Group을 사용하지 않는다.
    }

    Teletext_SetControl(eTTX_CONTROL_LANGUAGE, (HUINT32)eLangGroup);
}

#endif



HERROR local_ttxctrl_GetInitPageFromSI (TTX_InfoList_t *pTtxList, HxLANG_Id_e  eMenuLangId, HUINT8 *pucM, HUINT8 *pucX)
{
	HUINT32			i;
	HxLANG_Id_e	eTtxLangId;

	for (i = 0 ; i < pTtxList->ulInitPageNum ; i++)
	{
        eTtxLangId = HLIB_LANG_639ToId(pTtxList->stTtxInitPage[i].aucLangCode);

		if (eMenuLangId == eTtxLangId)	// found it!!
		{
			*pucM	= pTtxList->stTtxInitPage[i].ucMgNum;
			*pucX	= pTtxList->stTtxInitPage[i].ucPageNum;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}





void local_ttxctrl_deliveryPacket(HINT32 size, void *pvData)
{
	ONDK_GWM_App_t	pGwmApp = NULL;
	HUINT32		param1, param2,i,len;
	HUINT8		*pdata = pvData;
	param1 = HxMACRO_Get32Bit(pdata);		pdata += 4;
	param2 = HxMACRO_Get32Bit(pdata);		pdata += 4;
	if(param1 == DATA_STREAMER_EBU_SVCSTART && param2 == DATA_STREAMER_EBU_SVCSTART)
	{
		if(AP_SubtitleGetType() == eOxMP_SUBTITLETYPE_Ebu)
		{
			pGwmApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)AP_Teletext_Proc);
			if(pGwmApp == NULL)
				ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW,"Teletext", APP_OSD_DRAW_SBTL_TTX_PRIOTITY, AP_Teletext_Proc, (HINT32)g_hTeletextWinHandle, TRUE, 0,0);
		}
	}
	else if(param1 == DATA_STREAMER_EBU_SVCSTOP && param2 == DATA_STREAMER_EBU_SVCSTOP)
	{
		pGwmApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)AP_Teletext_Proc);
		if(pGwmApp)
			ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)AP_Teletext_Proc);  // EBU
	}
	else
    {
		for(i=0; i < param1; i ++) // how many ..
		{
			len = HxMACRO_Get32Bit(pdata);			pdata += 4;
			Teletext_Decoding(pdata);	//1  Decoding
			pdata += len;
		}
	}
}


HERROR local_ttxctrl_GetInitPage(AP_TtxContent_t *pContents, HUINT8 *pucM, HUINT8 *pucX)
{

    *pucM   = TTX_NULL_MAGAZINE;
    *pucX   = TTX_NULL_PAGE;

#if 0
	HERROR			hErr;
	HUINT32			ulActId;
	Handle_t		hService;
	TTX_InfoList_t	ttxInfo;
	HxLANG_Id_e	eMenuLangId;

	switch (actType)
	{
	case eViewType_WTV:
		if (AP_WTV_GetActualSvcHandle(ulActId, &hService) != ERR_OK
			|| MW_SI_GetTtxInitPageInfo(ulActId, hService, (TTX_InfoList_t*)&ttxInfo) != ERR_OK
			|| ttxInfo.ulInitPageNum == 0)
		{
			return ERR_FAIL;
		}

		if (AP_SPEC_UseTtxInitialPage())
		{
			hErr = DB_SETUP_GetMenuItemValue(MENUCFG_ITEMNAME_MENU_LANG, (HINT32 *)&eMenuLangId);
			if (hErr == ERR_OK && local_ttxctrl_GetInitPageFromSI(&ttxInfo, eMenuLangId, pucM, pucX) == ERR_OK)
			{
				break;
			}

			/*	Second Menu Language가 추가되면, 필요한 부분
			hErr = DB_SETUP_GetMenuItemValue(MENUCFG_ITEMNAME_2ND_MENU_LANG, (HINT32 *)&eMenuLangId);
			if (hErr == ERR_OK && local_ttxctrl_GetInitPageFromSI(&ttxInfo, eMenuLangId, pucM, pucX) == ERR_OK)
			{
				break;
			}
			*/

			// 맞는게 없으면.. 그냥 첫번째..
			*pucM	= ttxInfo.stTtxInitPage[0].ucMgNum;
			*pucX	= ttxInfo.stTtxInitPage[0].ucPageNum;
		}
		else
		{
			*pucM	= TTX_NULL_MAGAZINE;
			*pucX	= TTX_NULL_PAGE;
		}
		break;

	case eViewType_ATV :
	case eViewType_TV_INPUT :
		*pucM	= TTX_NULL_MAGAZINE;
		*pucX	= TTX_NULL_PAGE;
		break;
	default :
		return ERR_FAIL;
	}
#endif
	return ERR_OK;
}






static ONDK_Result_t local_ttxctrl_MsgEvtStopped(void)
{
	ONDK_GWM_APP_Destroy(0);

	return GWM_MESSAGE_BREAK;
}


static ONDK_Result_t local_ttxctrl_MsgEvtShowSuccess(AP_TtxContent_t *pContents)
{
	pContents->bShowSuccess = TRUE;

#if !defined(CONFIG_MWC_OSD_SBTL_ON_UIPLANE)
//	ONDK_GWM_APP_PreventDraw_LowPriority(ONDK_UPPER_WINDOW,APP_DEFAULT_PRIORITY);
#endif

	return GWM_MESSAGE_BREAK;
}
#if 0
static ONDK_Result_t local_ttxctrl_MsgAppActionChange(AP_TtxContent_t *pContents)
{
    ONDK_GWM_APP_Destroy(0);

	return MESSAGE_PASS;
}

static ONDK_Result_t local_ttxctrl_MsgAppSvcChange(AP_TtxContent_t *pContents, Handle_t hAction)
{
	if ( MWC_RM_GetActionId(pContents->hAction) == MWC_RM_GetActionId(hAction) )
	{
		ONDK_GWM_APP_Destroy(0);
	}

	return MESSAGE_PASS;
}

#endif

static ONDK_Result_t local_ttxctrl_MsgGwmCreate(AP_TtxContent_t *pContents,HINT32 value)
{
	HERROR		hErr;
    HxLOG_Print("MESSAGE : local_ttxctrl_MsgGwmCreate [%d]\n",value);

// 수정 필요
//	ONDK_GWM_SendMessage (NULL, MSG_APPC2U_CREATE_TELETEXT_CTRLUI, hAction, 0, 0, 0);
    local_ttxctrl_Initialize(pContents);
	/* Init TTX Proc. */
	pContents->bIsEbuSbt = value;
	hErr = local_ttxctrl_InitTtxProc(pContents);
	if ( hErr != ERR_OK )
	{
		pContents->bShowSuccess = FALSE;
		ONDK_GWM_APP_Destroy(NULL);
	}
	return ONDK_RET_OK;
}


static ONDK_Result_t local_ttxctrl_MsgGwmDestory(AP_TtxContent_t *pContents)
{
	HERROR		hErr;
	HxLOG_Print("[%s:%d]\n",__FUNCTION__,__LINE__);

	if(ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)AP_TtxMenuUi_Proc))
		ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)AP_TtxMenuUi_Proc);

	if ( pContents->bRunning )
	{
		/* Get Last Page Address*/
		hErr = Teletext_GetPageAddress(
							&(pContents->ucLastMagazine),
							&(pContents->ucLastPageNum),
							&(pContents->usLastSubcode)
						);

		if ( (hErr == ERR_OK) && (pContents->bShowSuccess == TRUE) )
		{
			pContents->bHistory = TRUE;
		}
		pContents->bShowSuccess = FALSE;

		/* Hide Page. */
		hErr = Teletext_Hide();
    	if(hErr == ERR_OK)
            HxLOG_Assert("Teletext_Hide");

		pContents->bRunning = FALSE;
	}


//	ONDK_GWM_APP_DisablePreventDraw();

    //요기 수정 필요
	//ONDK_GWM_SendMessage (NULL, MSG_APPC2U_DESTROY_TELETEXT_CTRLUI, 0, 0, 0, 0);
	return GWM_MESSAGE_BREAK;
}





static ONDK_Result_t	local_ttxctrl_APKMessage(Handle_t hAct,HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	GWM_APK_MSG_Type_e	eAPK_Type = (GWM_APK_MSG_Type_e)hAct;
//	HERROR							 hErr;
	OxMediaPlay_LiveViewState_e		eViewState;
	ONDK_Result_t					ret= GWM_RET_OK;
	OxMediaPlay_Event_e 			eEvent = (ulParam1 & 0x00FF);

	HxLOG_Debug("local_ttxctrl_APKMessage...\n");

	if(eAPK_Type == GWM_APK_MEDIA)
	{
		HxLOG_Debug("[%s] -- eEvent : %d \n", __FUNCTION__, eEvent);
		switch (eEvent)
		{
			case	eOxMP_EVENT_LIVE_ViewStateChanged:
				{
					eViewState = ulParam2;
					if (eViewState!= eOxMP_LiveViewState_OK && eViewState != eOxMP_LiveViewState_CAS)
					{
						HxLOG_Debug("teletext: stopped.\n");
						ONDK_GWM_APP_Destroy(NULL);
					}
				}
				break;
			case	eOxMP_EVENT_LIVE_SvcChanged:
			case 	eOxMP_EVENT_LIVE_SvcStopped:
			case 	eOxMP_EVENT_TUNE_NoSignal:
				ONDK_GWM_APP_Destroy(NULL);
				break;
			default:
				break;
		}
	}

	return ret;
}



static ONDK_Result_t local_ttxctrl_MsgGwmKeyDown(AP_TtxContent_t *pContents, HINT32 p1)
{
//	local_ttxctrl_KeyConvertion (&p1);
    HxLOG_Print("[TeleText] key = [%x]\n",__FUNCTION__,__LINE__,p1);

    switch(p1)
    {
	    /******************************************************* ************************************/
	    /************************************ Teletext 동작 관련 Key ********************************/
	    /******************************************************* ************************************/
		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy(NULL);
			return ONDK_MESSAGE_NONCONSUMED;
        case KEY_OPT :
            ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW,"AP_TtxMenuUi_Proc", APP_OSD_DRAW_SBTL_TTX_PRIOTITY, AP_TtxMenuUi_Proc, 0, 0, 0, 0);
            return ONDK_MESSAGE_CONSUMED;
	    case KEY_0:
	    case KEY_1:
	    case KEY_2:
	    case KEY_3:
	    case KEY_4:
	    case KEY_5:
	    case KEY_6:
	    case KEY_7:
	    case KEY_8:
	    case KEY_9:
			Teletext_SetControl( eTTX_CONTROL_NUMERIC, (HUINT32)(p1 - KEY_0));
	    	return ONDK_MESSAGE_CONSUMED;

		case KEY_ARROWLEFT: 	/* subpage 전환 key */
		case KEY_ARROWRIGHT:	/* subpage 전환 key */
		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
			Teletext_SetControl( eTTX_CONTROL_ARROWKEY, (HUINT32)(p1 - KEY_ARROWLEFT));
			return ONDK_MESSAGE_CONSUMED;

		case KEY_RED:
		case KEY_GREEN:
		case KEY_YELLOW:
	    case KEY_BLUE:
			Teletext_SetControl(eTTX_CONTROL_COLORKEY, (HUINT32)(p1 - KEY_RED));
	    	return ONDK_MESSAGE_CONSUMED;

		case KEY_GUIDE:
		case KEY_SPECIAL_HOME:
			HAPI_BroadcastSignal("signal:onTeletextState", 1, "i", 0);
			return ONDK_MESSAGE_NONCONSUMED;
        case KEY_BACK:
        case KEY_EXIT:
		case KEY_TEXT:
			HAPI_BroadcastSignal("signal:onTeletextState", 1, "i", 0);
			ONDK_GWM_SendMessage(AP_Teletext_Proc,MSG_GWM_DESTROY,0,0,0,0);
			return ONDK_MESSAGE_CONSUMED;

        case KEY_CH_PLUS:
        case KEY_CH_MINUS:
			HAPI_BroadcastSignal("signal:onTeletextState", 1, "i", 0);
			ONDK_GWM_SendMessage(AP_Teletext_Proc,MSG_GWM_DESTROY,0,0,0,0);
			return ONDK_MESSAGE_NONCONSUMED;
		case KEY_MUTE:
		case KEY_VOLUMEUP:
		case KEY_VOLUMEDOWN:
			return ONDK_MESSAGE_PASS;
	    default:
    		//ONDK_GWM_APP_Destroy(0);
    		return ONDK_MESSAGE_CONSUMED;
    }

	return ONDK_RET_OK;
}


// MSG_GWM_HAPI_MESSAGE
static ONDK_Result_t local_ttxctrl_hapiMessage(HAPIMSG_e eMsg, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return ONDK_MESSAGE_PASS;
}

ONDK_Result_t	AP_Teletext_Proc(HINT32 message, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;
	AP_TtxContent_t	*pContents = local_ttxctrl_GetTtxContent();
	static ONDK_Result_t s_upkey =ONDK_RET_OK;
	ONDK_Debug(HxANSI_COLOR_YELLOW("Message(0x%x) : %s  p1(0x%x) p2(0x%x) p3(0x%x) \n"), message, ONDK_GWM_GwmMsg2String(message), p1, p2, p3);

	switch (message)
	{
		/**********************************************************************
			GWM MESSAGES
		**********************************************************************/
	    case MSG_GWM_CREATE :
			HxLOG_Print("[MSG_GWM_CREATE]\n");
			Teletext_StartService();
			ONDK_GWM_APP_SetAppName(ONDK_GWM_APP_GetThis(), "AP_Teletext_Proc");
			//HAPI_SetApplicationFocusOrder( eAWINDOW_SUBTITLEPLAYER, eAWINDOW_MHEGIB, eHRELATION_DOWN );
			eResult = local_ttxctrl_MsgGwmCreate(pContents,p1);
	    	break;
		case MSG_GWM_APK_MESSAGE:
			eResult = local_ttxctrl_APKMessage(handle,p1, p2, p3);
			break;
        case MSG_GWM_KEYUP:
			eResult  =s_upkey;
			ONDK_Print("[%s:%d]MSG_GWM_KEYUP s_upkey[%d] \n",__FUNCTION__,__LINE__,s_upkey);
			break;
        case MSG_GWM_KEYDOWN :
			if(pContents->bIsEbuSbt == TRUE)
				eResult = ONDK_MESSAGE_PASS;
			else
				eResult = local_ttxctrl_MsgGwmKeyDown(pContents, p1);
			s_upkey =eResult;
			ONDK_Print("[%s:%d] MSG_GWM_KEYDOWNs_upkey[%d] \n",__FUNCTION__,__LINE__,s_upkey);
        	break;
       case MSG_GWM_DESTROY :
			HxLOG_Print("[MSG_GWM_DESTROY]\n");
		   	Teletext_StopService();
			eResult = local_ttxctrl_MsgGwmDestory(pContents);
			//HAPI_SetApplicationFocusOrder( eAWINDOW_SUBTITLEPLAYER, eAWINDOW_OIPF, eHRELATION_DOWN );
			break;
       case MSG_APP_TELETEXT_SHOWFAIL:
			HxLOG_Print("[MSG_APP_TELETEXT_SHOWFAIL]\n");
            eResult = local_ttxctrl_MsgGwmDestory(pContents);
            break;
        case MSG_GWM_INVALIDATE_RECT:
        	eResult = local_ttxctrl_Invalidate(pContents,handle);
            break;
		case MSG_GWM_HAPI_MESSAGE:
			eResult = local_ttxctrl_hapiMessage((HAPIMSG_e)handle, p1, p2, p3);
			break;
		default :
			break;
	}

	if ( eResult != ONDK_RET_OK )
	{
		return eResult;
	}

    return ONDK_GWM_ProcessMessageDefault(message, handle, p1, p2, p3);
}


/**************************************************************************************************/
#define _________AP_Teletext_Public_Functions__________________________________________________________
/**************************************************************************************************/
void           Teletext_deliveryPacket(HCHAR* datatype, HINT32 len, HUINT8* p)
{
	HxLOG_Info("[%s:%d]ttxpes\n",__FUNCTION__,__LINE__);
	local_ttxctrl_deliveryPacket(len, p);
}


/**************************************************************************************************/
#define _________AP_Teletext_End_Of_File____________________________________________________
/**************************************************************************************************/
