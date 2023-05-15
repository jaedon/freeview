
/**
	@file     ap_cas_cx_about.c
	@brief    file_name & simple comment.

	Description: for CONAX MENU ABOUT UI \n
	Module: Application(Layer), Menu(module), CAS_CX(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2009/07/23		modified					jhlee\n

	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <db_svc.h>
#include <db_param.h>
#include <svc_sys.h>
#include <svc_cas.h>
#include <isosvc.h>
#include <osd_gfx.h>
#include <osd_font.h>

#include <app_manager_cas.h>
#include <mgr_cas.h>

#include <conax.h>
#include <cx_common.h>
#include "../../local_include/_xmgr_cas_cx_util.h"
#include <xmgr_cas_res_str.h>


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/



/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/





#define	CAS_CX_ABOUT_ITEM_TEXT_LENGTH		32
#define	CAS_CX_ABOUT_TEXT_WDITH				300


/*******************************************************************/
/********************      Struture        *************************/
/*******************************************************************/

typedef enum
{
	eCasCx_AboutItem_Software_Version,		/* Software_Version */
	eCasCx_AboutItem_Interface_Version,		/* Interface Version */
	eCasCx_AboutItem_Card_Number,			/* Card Number */
#if defined(CONFIG_MW_CAS_CONAX_CPM)
	eCasCx_AboutItem_Chip_ID,				/* Chip ID */
#endif
	eCasCx_AboutItem_Session_Number,		/* Number of sessions */
	eCasCx_AboutItem_Language,				/* Language */
	eCasCx_AboutItem_System_ID,				/* CA_SYS_ID */
	eCasCx_AboutItem_Max,
} XmgrCas_CxAboutItem_e;


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
typedef struct CxAboutMenuItem
{
	XmgrCas_CxAboutItem_e	eItemId;
	HBOOL					bItemDisable;
	HUINT8					szItemName[MGR_CAS_STR_LENGTH_LONG];
	HUINT8					szItemValue[MGR_CAS_STR_LENGTH_LONG];
} XmgrCas_CxAboutMenuItem_t;

typedef struct CxAboutMenuContext
{
	HUINT16					usSlotId;
	HUINT8					szTitle[MGR_CAS_STR_LENGTH_LONG];
	HUINT8					szSubTitle[MGR_CAS_STR_LENGTH_LONG];
	HUINT32 				ulTotalNum;
	BUS_Callback_t			pfnCallerCallback;
	XmgrCas_CxAboutMenuItem_t		astListItem[eCasCx_AboutItem_Max];
} XmgrCas_CxAboutMenuContext_t;



STATIC XmgrCas_CxAboutMenuContext_t	s_stCasCxAboutContents;


/*******************************************************************/
/********************      	Static Prototype	********************/
/*******************************************************************/
STATIC XmgrCas_CxAboutMenuContext_t	*xmgr_cas_CxAboutGetLocalContents(void);
STATIC BUS_Result_t		xmgr_cas_CxAboutInitAboutItems(XmgrCas_CxAboutMenuContext_t *pstContext);
STATIC HERROR			xmgr_cas_CxAboutGetCardNumber(HUINT32 ulCardNum, HUINT8 *pszCardNum);
#if defined(CONFIG_MW_CAS_CONAX_CPM)
STATIC HERROR			xmgr_cas_CxAboutGetChipId(HUINT8 *pszChipId, HUINT32 ulSize);
#endif	// End of defined(CONFIG_MW_CAS_CONAX_CPM)



/*******************************************************************/
/********************      	Static Function 	********************/
/*******************************************************************/
#define _________________UI__________________________________

STATIC XmgrCas_CxAboutMenuContext_t	*xmgr_cas_CxAboutGetLocalContents(void)
{
	return	&s_stCasCxAboutContents;
}


#define	________________Init_Func____________________________

STATIC BUS_Result_t		xmgr_cas_CxAboutInitContext(XmgrCas_CxAboutMenuContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Print("[local_cas_ir_system_info_Setup]\n");
	HxSTD_memset(pstContext, 0, sizeof(XmgrCas_CxAboutMenuContext_t));

	pstContext->pfnCallerCallback = (BUS_Callback_t)p1;
	pstContext->usSlotId = p2;

	MWC_UTIL_DvbStrncpy(pstContext->szTitle, AP_CasResStr_GetStrRsc(STR_ABOUT_CONAX_CA_ID), MGR_CAS_STR_LENGTH_LONG);
	pstContext->szTitle[MGR_CAS_STR_LENGTH_LONG - 1] = '\0';

	MWC_UTIL_DvbStrncpy(pstContext->szSubTitle, AP_CasResStr_GetStrRsc(STR_ABOUT_CONAX_CA_ID), MGR_CAS_STR_LENGTH_LONG);
	pstContext->szSubTitle[MGR_CAS_STR_LENGTH_LONG - 1] = '\0';

	return ERR_OK;

}
STATIC BUS_Result_t	xmgr_cas_CxAboutSetAboutItems(XmgrCas_CxAboutMenuContext_t *pstContext)
{
	HERROR					hError = ERR_OK;
	CxVersionInfo_t			sCxtInfo;
	XmgrCas_CxAboutMenuItem_t		*pstItem = NULL;
	HINT32	i = 0;
	HINT32	CasCxAbout_StrId[eCasCx_AboutItem_Max] =	{
						STR_SOFTWARE_VERSION_ID,				/* Software_Version */
						STR_INTERFACE_VERSION_ID,		/* Interface Version */
						STR_CARD_NUMBER_ID,			/* Card Number */
#if defined(CONFIG_MW_CAS_CONAX_CPM)
						STR_CHIP_ID_ID,			/* Chip ID */
#endif	// End of defined(CONFIG_MW_CAS_CONAX_CPM)
						STR_NUMBER_OF_SESSIONS_ID,		/* Session Number */
						STR_LANGUAGE_ID,				/* Language */
						STR_CA_SYS_ID_ID,				/* CAS_SYS_ID */
						};
	pstContext->ulTotalNum = eCasCx_AboutItem_Max;

	HxSTD_memset(&sCxtInfo, 0, sizeof(sCxtInfo));
	hError = xmgr_cas_CxGetVersionInfo(CX_SC_SLOT_0, &sCxtInfo);
	if (hError != ERR_OK)
	{
		hError = xmgr_cas_CxGetVersionInfo(CX_SC_SLOT_0, &sCxtInfo);
		if (hError != ERR_OK)
		{
			// Dialog Destroy : Please Wait
			HxLOG_Print("==[xmgr_cas_CxAboutInitAboutItems]== xmgr_cas_CxGetGetVersionInfo() Error!\n");
			return ERR_FAIL;
		}
	}

	for (i = 0; i < pstContext->ulTotalNum; i++)
	{
		pstItem = &(pstContext->astListItem[i]);
		if(pstItem != NULL)
		{
			HxSTD_memset(pstItem, 0, sizeof(XmgrCas_CxAboutMenuItem_t));
			pstItem->eItemId = i;
			pstItem->bItemDisable = FALSE;

			MWC_UTIL_DvbStrcpy(pstItem->szItemName, AP_CasResStr_GetStrRsc(CasCxAbout_StrId[i]));
			memset(pstItem->szItemValue, 0, CAS_CX_ABOUT_ITEM_TEXT_LENGTH);

			switch(i)
			{
				case eCasCx_AboutItem_Software_Version:			/* Software Version */
					//MWC_UTIL_DvbUtf8Sprintf(pstItem->szItemValue, (HUINT8*)"%s", (HUINT8*)sCxtInfo.pszSwVer);
					HxSTD_snprintf(pstItem->szItemValue, sizeof(pstItem->szItemValue), (HUINT8*)"%s", (HUINT8*)sCxtInfo.pszSwVer);
					break;

				case eCasCx_AboutItem_Interface_Version:		/* Interface Version */
					//MWC_UTIL_DvbUtf8Sprintf(pstItem->szItemValue, (HUINT8*)"0x%02x", CX_SC_CX5_VERSION);
					HxSTD_snprintf(pstItem->szItemValue, sizeof(pstItem->szItemValue), (HUINT8*)"0x%02x", CX_SC_CX5_VERSION);
					break;

				case eCasCx_AboutItem_Card_Number:				/* Card Number(Decimal) - 11 Digits(3+4+4)*/
					xmgr_cas_CxAboutGetCardNumber(sCxtInfo.ulCardNumber, pstItem->szItemValue);
					break;

				#if defined(CONFIG_MW_CAS_CONAX_CPM)
				case eCasCx_AboutItem_Chip_ID:					/* Chip ID */
					xmgr_cas_CxAboutGetChipId(pstItem->szItemValue, CAS_CX_ABOUT_ITEM_TEXT_LENGTH);
					break;
				#endif	// End of defined(CONFIG_MW_CAS_CONAX_CPM)

				case eCasCx_AboutItem_Session_Number:			/* Session Number(Decimal) */
					//MWC_UTIL_DvbUtf8Sprintf(pstItem->szItemValue, (HUINT8*)"%d", sCxtInfo.nSessionCnt);
					HxSTD_snprintf(pstItem->szItemValue, sizeof(pstItem->szItemValue), (HUINT8*)"%d", sCxtInfo.nSessionCnt);
					break;

				case eCasCx_AboutItem_Language:					/* Language(Decimal) - 1-3 digits*/
					//MWC_UTIL_DvbUtf8Sprintf(pstItem->szItemValue, (HUINT8*)"%d", sCxtInfo.nCountryInd);
					HxSTD_snprintf(pstItem->szItemValue, sizeof(pstItem->szItemValue), (HUINT8*)"%d", sCxtInfo.nCountryInd);
					break;

				case eCasCx_AboutItem_System_ID:				/* CAS System ID - Hexadecimal */
					//MWC_UTIL_DvbUtf8Sprintf(pstItem->szItemValue, (HUINT8*)"0x%04X", sCxtInfo.nCaSysId);
					HxSTD_snprintf(pstItem->szItemValue, sizeof(pstItem->szItemValue), (HUINT8*)"0x%04X", sCxtInfo.nCaSysId);
					break;

				default:
					HxLOG_Critical("\n\n");
					return ERR_FAIL;
			}
		}
	}

	// Dialog Destroy : Please Wait

	return ERR_OK;
}


STATIC HERROR	xmgr_cas_CxAboutGetCardNumber(HUINT32 ulCardNum, HUINT8 *pszCardNum)
{
	HUINT32		ulCardNumHigh, ulCardNumMiddle, ulCardNumLow, ulCardNumChecksum, ulCardNumber;
	HUINT32 	ulTmpInt = 0;

	ulCardNumber = ulCardNum;

	ulCardNumHigh = ulCardNumber/100000000;
	ulCardNumber -= (ulCardNumHigh*100000000);
	ulCardNumMiddle = ulCardNumber/10000;
	ulCardNumber -= (ulCardNumMiddle*10000);
	ulCardNumLow = ulCardNumber;

	// calculate checksum of smart card
	ulTmpInt = xmgr_cas_CxCalcDigit(3, ulCardNumHigh);
	ulTmpInt += xmgr_cas_CxCalcDigit(4, ulCardNumMiddle);
	ulTmpInt += xmgr_cas_CxCalcDigit(4, ulCardNumLow);
	ulCardNumChecksum = (10 - (ulTmpInt%10))%10;

	//MWC_UTIL_DvbUtf8Sprintf(pszCardNum, (HUINT8*)"%03d %04d %04d - %01d", ulCardNumHigh, ulCardNumMiddle, ulCardNumLow, ulCardNumChecksum);
	HxSTD_snprintf(pszCardNum, sizeof(pszCardNum), (HUINT8*)"%03d %04d %04d - %01d", ulCardNumHigh, ulCardNumMiddle, ulCardNumLow, ulCardNumChecksum);

	return ERR_OK;
}


#if defined(CONFIG_MW_CAS_CONAX_CPM)
STATIC HERROR	xmgr_cas_CxAboutGetChipId(HUINT8 *pszChipId, HUINT32 ulSize)
{
	HUINT32 ulChipId = 0, ulPart1, ulPart2, ulPart3;
	HERROR err;

	err = SVC_SYS_GetChipId(&ulChipId);
	ulPart1 = ulChipId / 100000000;
	ulPart2 = (ulChipId / 10000) % 10000;
	ulPart3 = ulChipId % 10000;
	snprintf(pszChipId, ulSize, "%03d %04d %04d", ulPart1, ulPart2, ulPart3);

	return err;
}
#endif	// End of defined(CONFIG_MW_CAS_CONAX_CPM)

STATIC HERROR xmgr_cas_CxAboutSendData(XmgrCas_CxAboutMenuContext_t *pstContext)
{
	HERROR					hErr = ERR_OK;
	XmgrCas_CxAboutMenuItem_t			*pstItem;
	OxMgrCasUiEvtGrid_t		stEvent;
	OxMgrCasUiGrid_t		*pstGridList;
	OxMgrCasUiGridItem_t	*pstGridItem;
	HUINT32					ulStrLen, i;

	HxSTD_memset(&stEvent, 0, sizeof(OxMgrCasUiEvtGrid_t));
	stEvent.ulUiHandle = (HUINT32)pstContext->pfnCallerCallback;

	pstGridList = &stEvent.stGrid;

	pstGridList->stHeader.ulNumOfColumn = 2;
	pstGridList->stHeader.ulAttr = 0;
	MWC_UTIL_DvbStrncpy(pstGridList->stHeader.szItemAlign, "left|right", MGR_CAS_STR_LENGTH_LONG);
	pstGridList->stHeader.ulFocusType = eMgrCasUiFocusType_Column;
	pstGridList->stHeader.stFocus.ulHorIdx = 0;
	pstGridList->stHeader.stFocus.ulVerIdx = 0;
	pstGridList->stHeader.stStartPos.ulHorIdx = 0;
	pstGridList->stHeader.stStartPos.ulVerIdx = 0;

	ulStrLen = SvcCas_UtilStrLen(pstContext->szTitle);

	if(ulStrLen != 0)
	{
		MWC_UTIL_DvbStrncpy(pstGridList->stHeader.szTitle, pstContext->szTitle, MGR_CAS_STR_LENGTH_LONG);
		pstGridList->stHeader.ulAttr |= eMgrCasUiAttr_Title;
		HxLOG_Print("Menu Title [%s]\n", pstGridList->stHeader.szTitle);
	}

	ulStrLen = SvcCas_UtilStrLen(pstContext->szSubTitle);
	if(ulStrLen != 0)
	{
		MWC_UTIL_DvbStrncpy(pstGridList->stHeader.szSubTitle, pstContext->szSubTitle, MGR_CAS_STR_LENGTH_LONG);
		pstGridList->stHeader.ulAttr |= eMgrCasUiAttr_Subtitle;
		HxLOG_Print("Sub Title [%s]\n", pstGridList->stHeader.szSubTitle);
	}

	pstGridList->ulTotalNum = pstContext->ulTotalNum;
	HxLOG_Print("Item Count[%d]\n", pstGridList->ulTotalNum);

	pstGridList->astGridItem = (OxMgrCasUiGridItem_t *)OxAP_Malloc(sizeof(OxMgrCasUiGridItem_t) * (pstGridList->ulTotalNum));
	if(pstGridList->astGridItem == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	HxSTD_memset(pstGridList->astGridItem, 0, sizeof(OxMgrCasUiGridItem_t) * (pstGridList->ulTotalNum));

	for (i = 0; i < pstGridList->ulTotalNum; i++)
	{
		pstGridItem = &pstGridList->astGridItem[i];
		pstItem = &pstContext->astListItem[i];

		pstGridItem->ulIndex = i;
		pstGridItem->ulAttr = 0;

		if(SvcCas_UtilStrLen(pstItem->szItemName) != 0)
		{
			snprintf((char *)pstGridItem->szString, MGR_CAS_STR_LENGTH_LONG, "%s|%s", pstItem->szItemName, pstItem->szItemValue);
		}
		else
		{
			snprintf((char *)pstGridItem->szString, MGR_CAS_STR_LENGTH_LONG, "%s| ", pstItem->szItemName);
		}
		pstGridItem->szString[MGR_CAS_STR_LENGTH_LONG - 1] = '\0';
	 	pstGridItem->ulStringLength = SvcCas_UtilStrLen(pstGridItem->szString);
		HxLOG_Print("Item [%d][%s]\n", i, pstGridItem->szString);
	}

	hErr = MGR_APPMGR_MODULE_SendUiEvent (eDxCAS_GROUPID_CX, 0/*ulSlotId*/, EVT_CASUI_GRID, &stEvent);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
	}

	if(pstGridList->astGridItem != NULL)
	{
		OxAP_Free(pstGridList->astGridItem);
	}
	return hErr;
}
STATIC HERROR		xmgr_cas_CxAboutRequestSessionStop(XmgrCas_CxAboutMenuContext_t *pContext)
{
	OxMgrCasUiEvtReqSessionStop_t stEvent;

	stEvent.ulSessionType = (HUINT32)eMgrCasUiSession_Menu;
	if(MGR_APPMGR_MODULE_SendUiEvent (eDxCAS_GROUPID_CX, 0, eMEVT_CASUI_REQ_CLOSE_SESSION, &stEvent) != ERR_OK)
	{
		HxLOG_Error("\n");
	}

	return ERR_OK;
}
STATIC BUS_Result_t xmgr_cas_CxAboutMsgInputNotify(XmgrCas_CxAboutMenuContext_t *pstContext, HINT32 message, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasInputType_e 	eInputType = (OxMgrCasInputType_e)p2;
	OxMgrCasUiInputNotify_t	*pstCasInputData = (OxMgrCasUiInputNotify_t *)p3;

	HxLOG_Print("[xmgr_cas_CxAboutMsgInputNotify] InputType[%d]", eInputType);

	if(eInputType == eMgrCasInputType_Etc)
	{
		if(pstCasInputData->stEtc.eMgrCasEtcType == eMgrCasEtc_Previous)
		{
			HxLOG_Print("[xmgr_cas_CxAboutMsgInputNotify] : eMgrCasEtc_Previous ");
			xmgr_cas_CxAboutRequestSessionStop(pstContext);
		}
		else if(pstCasInputData->stEtc.eMgrCasEtcType == eMgrCasEtc_Exit)
		{
			HxLOG_Print("[xmgr_cas_CxAboutMsgInputNotify] : eMgrCasEtc_Exit ");
			xmgr_cas_CxAboutRequestSessionStop(pstContext);
		}

	}

	return ERR_BUS_MESSAGE_BREAK;
}
#define	________________Global_Function_______________________

/*******************************************************************/
/********************      	Global Function 	********************/
/*******************************************************************/
BUS_Result_t	xproc_cas_CxAbout(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					nResult = ERR_FAIL;
	XmgrCas_CxAboutMenuContext_t	*pstContext = xmgr_cas_CxAboutGetLocalContents();
	HxLOG_Info("xproc_cas_CxAbout handle(0x%x)\n", hAction);
	switch (message)
	{
		case eMEVT_BUS_CREATE:
			BUS_MGR_SetMgrName(BUS_MGR_GetThis(), "xproc_cas_CxAbout");
			/* init */
			xmgr_cas_CxAboutInitContext(pstContext, p1, 0/*slot*/, p3);

			/* set Item */
			xmgr_cas_CxAboutSetAboutItems(pstContext);

			/* send to ap */
			xmgr_cas_CxAboutSendData(pstContext);

			//BUS_SetTimer(MENU_TIMEOUT_ID, MENU_TIMEOUT);
			break;
		case eMEVT_CAS_CHECK:
			HxLOG_Print("[xproc_cas_CxAbout] msg (0x%x), p1 (0x%x)\n", message, p1);
			if (p1 == eDxCAS_GROUPID_CX)
			{
				/* set Item */
				xmgr_cas_CxAboutSetAboutItems(pstContext);

				/* send to ap */
				xmgr_cas_CxAboutSendData(pstContext);
			}
		case eMEVT_BUS_TIMER:
			if(p1 == MENU_TIMEOUT_ID)
			{
				// TODO: 뭘 해줘야 하는거지???
			}
			break;
		case MSG_OAPI_CASUI_SESSION_START:
			break;

		case MSG_OAPI_CASUI_SESSION_STOP:
			break;

		case MSG_OAPI_CASUI_INPUT_NOTIFY:
			nResult = xmgr_cas_CxAboutMsgInputNotify(pstContext, message, p1, p2, p3);
			if(nResult != ERR_OK)
			{
				return nResult;
			}
			break;

		case MSG_OAPI_CASUI_CHANGE_LANGUAGE:
			break;

		case eMEVT_BUS_DESTROY:
			#if 0 // not use for octo2.0
			/* destroy UI */
			xmgr_cas_CxAboutDestroyUI(pstContext);
			#endif
			// TODO: Destory 하면 죽는다....
			#if 0
			if (BUS_MGR_Get(xproc_cas_CxAbout) != NULL)
			{
				BUS_MGR_Destroy(xproc_cas_CxAbout);
			}
			BUS_MGR_Destroy(0);
			#endif

			/* kill timer */
			BUS_KillTimer(MENU_TIMEOUT_ID);

			/* clear front PANEL */
			//AP_SPEC_ClearMenuPanelString();
			break;

		default:
			break;
		// TODO: Blocking...
		#if 0 // not use for octo2.0
		case MSG_DLG_INPUTED_KEYDOWN:
			GWM_ResetTimer(MENU_TIMEOUT_ID);
			return MESSAGE_BREAK;

		case MSG_GWM_KEYUP:
			AP_KeyConv_UiCommon(&p1);
			break;

		case MSG_GWM_KEYDOWN:
			AP_KeyConv_UiCommon(&p1);
			GWM_ResetTimer(MENU_TIMEOUT_ID);

			nResult = Settings_CheckKeyPass(p1, BUS_MGR_GetAppCallback(BUS_MGR_GetThis()));
			if (nResult != ERR_OK)
			{
				return nResult;
			}

			switch (p1)
			{
				case KEY_EXIT:
				case KEY_MENU:
					/* reset Title depth */
					Settings_ClearWindowTitle();
					BUS_MGR_Destroy(NULL);
					return MESSAGE_BREAK;

				case KEY_OK:		/* 단순히 화면을 보는 것이므로 */
				case KEY_BACK:
				case KEY_ARROWLEFT:
					/* to upper */
					BUS_MGR_Destroy(NULL);
					BUS_MGR_Create((char*)"xproc_cas_CxSmartCardMenu", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (GWM_Callback_t)xproc_cas_CxSmartCardMenu, 0, 0, 0, 0);
					return MESSAGE_BREAK;

				case KEY_ARROWDOWN:
				case KEY_ARROWUP:
					return MESSAGE_BREAK;

				default:
					break;
			}
			break;
		case eSEVT_CAS_SC_REMOVED:
			HxLOG_Print(" SC Event : eSEVT_CAS_SC_REMOVED!\n");
			BUS_SendMessage(xproc_cas_CxAbout, MSG_GWM_KEYDOWN, hAction, KEY_MENU, 0, 0);
			break;


		case eMEVT_BUS_TIMER:
			if (p1 == MENU_TIMEOUT_ID)
			{
				/* reset Title depth */
				Settings_ClearWindowTitle();
				BUS_MGR_Destroy(0);
			}
			return MESSAGE_BREAK;
		#endif


	}

	HxLOG_Info("xproc_cas_CxAbout handle(0x%x)\n", hAction);

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}


// End of file

