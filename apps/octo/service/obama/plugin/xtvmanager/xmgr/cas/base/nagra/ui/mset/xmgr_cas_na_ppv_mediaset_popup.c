//******************************************************************************
//	INCLUDE
//******************************************************************************
#include <db_svc.h>
#include <db_param.h>
#include <svc_sys.h>
#include <svc_pipe.h>
#include <svc_cas.h>
#include <isosvc.h>
#include <osd_gfx.h>
#include <osd_font.h>

#include <gwm.h>
#include <mgr_action.h>

#include <menuui_coor.h>
#include <settings.h>

#include <ap_util_osd.h>

#include <namgr_main.h>

#include "../../local_include/_xmgr_cas_na.h"
#include "../../local_include/_xmgr_cas_na_util.h"

#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
//******************************************************************************
//	DEFINE
//******************************************************************************
#define	MAX_MSET_POPUP_EVTCONFIRM_ROW_NUM			3
#define	MAX_MSET_POPUP_EVTNAME_LENGTH				50
#define	MAX_MSET_POPUP_EVTCONFIRM_LENGTH			512

typedef struct tag_CAS_NA_MEDIASET_PR_Contents // Popup Recharge
{
	HUINT8			*pStrTitle; 					///< title of app context
	HUINT8			*pStrRemainCreditTitle;
	HINT8			ucRemainCreditStr[16];
} XmgrCas_NaPopRechar_Mediaset_t;

typedef struct tag_CAS_NA_MEDIASET_OPPV_Contents // Popup OPPV
{
	HUINT8			*pStrTitle; 					///< title of app context
	HUINT8			*pStrRemainCreditTitle;
	HINT8			ucRemainCreditStr[16];
	HINT8			ucOppvEvtName[MAX_MSET_POPUP_EVTNAME_LENGTH+1];
} XmgrCas_NaPopOppv_Mediaset_t;

typedef struct tag_CAS_NA_MEDIASET_IPPV_RESULT_Contents // Popup IPPV Result
{
	HUINT8			*pStrTitle; 					///< title of app context
	HUINT8			*pStrRemainCreditTitle;
	HINT8			ucRemainCreditStr[16];
} XmgrCas_NaPopIppvResult_Mediaset_t;


/****************************************************************/
//MEDIASET Recharge, Oppv 좌표
/****************************************************************/
#define	NAPPV_MEDIASET_POPUP_WINDOW_WIDTH								OSD_SCREEN_WIDTH
#define	NAPPV_MEDIASET_POPUP_WINDOW_HEIGHT								OSD_SCREEN_HEIGHT
#define	NAPPV_MEDIASET_POPUP_WINDOW_X									0
#define	NAPPV_MEDIASET_POPUP_WINDOW_Y									0
#define	NAPPV_MEDIASET_POPUP_STRING_MARGIN_Y							18

#define	NAPPV_MEDIASET_POPUP_APP_ID										0

/* ppv mediaset popup background */
#define	NAPPV_MEDIASET_POPUP_WINBACKGROUND_ID							(NAPPV_MEDIASET_POPUP_APP_ID|0x0010)
#define	NAPPV_MEDIASET_POPUP_WINBACKGROUND_X							0
#define	NAPPV_MEDIASET_POPUP_WINBACKGROUND_RECHARGE_Y					280
#define	NAPPV_MEDIASET_POPUP_WINBACKGROUND_OPPV_Y						243
#define	NAPPV_MEDIASET_POPUP_WINBACKGROUND_IPPVRESULT_Y					270
#define	NAPPV_MEDIASET_POPUP_WINBACKGROUND_WIDTH						1280
#define	NAPPV_MEDIASET_POPUP_WINBACKGROUND_HEIGHT						203
#define	NAPPV_MEDIASET_POPUP_WINBACKGROUND_IPPVRESULT_HEIGHT			181

/* ppv mediaset popup event confirm msg info*/
#define	NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_ID							(NAPPV_MEDIASET_POPUP_APP_ID|0x0020)
#define	NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_X							190
#define	NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_RECHARGE_Y					309
#define	NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_OPPV_Y						292
#define	NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_IPPVRESULT_Y				309
#define	NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_WIDTH						900
#define	NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_HEIGHT						(NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_LINESPACE * MAX_MSET_POPUP_EVTCONFIRM_ROW_NUM)
#define	NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_LINESPACE					34

/* ppv mediaset popup event confirm credit text */
#define	NAPPV_MEDIASET_POPUP_EVTCONFIRM_CREDIT_TEXT_X					190
#define	NAPPV_MEDIASET_POPUP_EVTCONFIRM_CREDIT_TEXT_RECHARGE_Y			(309 + (NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_LINESPACE * MAX_MSET_POPUP_EVTCONFIRM_ROW_NUM))
#define	NAPPV_MEDIASET_POPUP_EVTCONFIRM_CREDIT_TEXT_OPPV_Y				(292 + (NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_LINESPACE * MAX_MSET_POPUP_EVTCONFIRM_ROW_NUM))
#define	NAPPV_MEDIASET_POPUP_EVTCONFIRM_CREDIT_TEXT_IPPVRESULT_Y		(309 + (NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_LINESPACE * MAX_MSET_POPUP_EVTCONFIRM_ROW_NUM))
#define	NAPPV_MEDIASET_POPUP_EVTCONFIRM_CREDIT_TEXT_WIDTH				900
#define	NAPPV_MEDIASET_POPUP_EVTCONFIRM_CREDIT_TEXT_HEIGHT				34

/* ppv mediaset popup event confirm exit button */
#define	NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_BTN_ID						(NAPPV_MEDIASET_POPUP_APP_ID|0x0030)
#define	NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_BTN_X						550
#define	NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_BTN_Y						424
#define	NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_BTN_WIDTH					108
#define	NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_BTN_HEIGHT					41

/* ppv mediaset popup event confirm exit button text*/
#define	NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_TEXT_ID					(NAPPV_MEDIASET_POPUP_APP_ID|0x0040)
#define	NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_TEXT_X						664
#define	NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_TEXT_Y						(451 - FONT_SIZE_XS)
#define	NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_TEXT_WIDTH					250
#define	NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_TEXT_HEIGHT				41

/* recharge/oppv 메시지를 받고나서 몇초 동안 credit update를 기다린다.  */
#define CAS_NA_PPV_MEDIASET_POPUP_RECHARGE_CHECK_CREDIT_ID				(0x11050151)
#define CAS_NA_PPV_MEDIASET_POPUP_OPPV_CHECK_CREDIT_ID					(0x11050152)
#define CAS_NA_PPV_MEDIASET_POPUP_IPPVRESULT_CHECK_CREDIT_ID			(0x11050153)
#define CAS_NA_PPV_MEDIASET_POPUP_IPPVRESULT_DISPLAY_CHECK_CREDIT_ID			(0x11050154)
#define CAS_NA_PPV_MEDIASET_POPUP_IPPVRESULT_DISPLAY_CREDIT_PERIOD			4000


STATIC XmgrCas_NaPopRechar_Mediaset_t 			s_stNaMsetPopReChar;
STATIC XmgrCas_NaPopOppv_Mediaset_t 			s_stNaMsetPopOppv;
STATIC XmgrCas_NaPopIppvResult_Mediaset_t 		s_stNaMsetPopIppvResult;

#define ________XMGR_CAS_NA_PpvPOPUP_STATIC___________________________________________
#define	_____POPUP_RECHARGE______________________________
STATIC XmgrCas_NaPopRechar_Mediaset_t* xmgr_cas_NaPpvPopupRecharGetLocalContents_Mediaset(void);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupRecharInitContents_Mediaset(XmgrCas_NaPopRechar_Mediaset_t *pContents, HINT32 nCredit);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupRecharDrawWinBackgroundImage_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupRecharDrawExitButton_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupRecharGetCountOfCharsFromCutLines_Mediaset(HUINT8 *message, HINT32 nfontsize, HINT32 nwidth, HINT16 nCutline);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupRecharDrawComfirmMsg_Mediaset(XmgrCas_NaPopRechar_Mediaset_t *pContents, OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupRecharDrawEvtRecharge_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupRecharSetUiObjects_Mediaset(XmgrCas_NaPopRechar_Mediaset_t *pContents);

#define	_____POPUP_OPPV______________________________
STATIC XmgrCas_NaPopOppv_Mediaset_t* xmgr_cas_NaPpvPopupPppvGetLocalContents_Mediaset(void);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupPppvInitContents_Mediaset(XmgrCas_NaPopOppv_Mediaset_t *pContents, HINT32 nCredit, HINT32 nNameLength, HUINT8 *pEvtName);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupPppvDrawWinBackgroundImage_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupPppvDrawExitButton_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupPppvGetCountOfCharsFromCutLines_Mediaset(HUINT8 *message, HINT32 nfontsize, HINT32 nwidth, HINT16 nCutline);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupPppvDrawComfirmMsg_Mediaset(XmgrCas_NaPopOppv_Mediaset_t *pContents, OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupPppvDrawEvtRecharge_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupPppvSetUiObjects_Mediaset(XmgrCas_NaPopOppv_Mediaset_t *pContents);

#define	_____POPUP_IPPV_RESULT______________________________
STATIC XmgrCas_NaPopIppvResult_Mediaset_t* xmgr_cas_NaPpvPopupIppvResultGetLocalContents_Mediaset(void);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupIppvResultInitContents_Mediaset(XmgrCas_NaPopIppvResult_Mediaset_t *pContents);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupIppvResultDrawWinBackgroundImage_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupIppvResultGetCountOfCharsFromCutLines_Mediaset(HUINT8 *message, HINT32 nfontsize, HINT32 nwidth, HINT16 nCutline);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupIppvResultDrawComfirmMsg_Mediaset(XmgrCas_NaPopIppvResult_Mediaset_t *pContents, OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupIppvResultDrawEvtIppvResult_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupIppvResultSetUiObjects_Mediaset(XmgrCas_NaPopIppvResult_Mediaset_t *pContents);

#define	____________________________________________________________
#define	_________________________BODY______________________________
#define	____________________________________________________________


#define	_____POPUP_RECHARGE______________________________
/********************************************************************
 Function		: xmgr_cas_NaPpvPopupRecharGetLocalContents_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC XmgrCas_NaPopRechar_Mediaset_t* xmgr_cas_NaPpvPopupRecharGetLocalContents_Mediaset(void)
{
	return &s_stNaMsetPopReChar;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupRecharInitContents_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupRecharInitContents_Mediaset(XmgrCas_NaPopRechar_Mediaset_t *pContents, HINT32 nCredit)
{
	HxSTD_memset(pContents, 0, sizeof(XmgrCas_NaPopRechar_Mediaset_t));

	pContents->pStrTitle = (HUINT8*)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_BANNER_MSG_RECHARGED_NOTIFY_ID);
	pContents->pStrRemainCreditTitle = (HUINT8*)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_RECHARGED_CREDIT_ID);
	//xmgr_cas_NaGetScCreditStr(pContents->ucRemainCreditStr);
	snprintf(pContents->ucRemainCreditStr, 16, "%d,%02d", nCredit/100, nCredit%100); 	// 다시 credit을 가져오지 않고 받은 credit을 사용 <== card removed 대비

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupRecharDrawWinBackgroundImage_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupRecharDrawWinBackgroundImage_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
											GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HINT32				 nX1, nY1;
	OSD_Rect_t			 stRect;
	OSD_StockImage_t	 stImage;

	(void)pObject;
	(void)pArg;
	(void)param1;

	nX1 = pArea->x1 + pRect->x;
	nY1 = pArea->y1 + pRect->y;

	if (!AP_StockImg_GetImage (e913_3_24_WIN1_203, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupRecharDrawExitButton_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupRecharDrawExitButton_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
											GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HINT32				 nX1, nY1;
	OSD_Rect_t			 stRect;
	OSD_StockImage_t	 stImage;

	(void)pObject;
	(void)pArg;
	(void)param1;

	nX1 = pArea->x1 + pRect->x;
	nY1 = pArea->y1 + pRect->y;

	if (!AP_StockImg_GetImage (e911_1_24_BTN_3Exit, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupRecharGetCountOfCharsFromCutLines_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupRecharGetCountOfCharsFromCutLines_Mediaset(HUINT8 *message, HINT32 nfontsize, HINT32 nwidth, HINT16 nCutline)
{
	HINT32		nNumOfMsgTextLine = 0;
	HINT32		nTotal = 0;
	HINT32		nLocLen = 0;
	HINT32		nStrLen = 0;
	HUINT32		nNumOfChForOneLine = 0;

	OSD_Font_t	*pFont = NULL;
	eHxCP_CodePage_e	encoding = eHCodepage_MAX;

	pFont = OSD_FONT_Get(eFont_SystemNormal);
	OSD_FONT_SetSize(pFont,nfontsize);
	nTotal = MWC_UTIL_GetLocaleCodeLen (&(message[nTotal]));
	nLocLen = nTotal;
	do
	{
		nNumOfChForOneLine = 0;
		nStrLen = SvcCas_UtilStrLen(&(message[nTotal]));
		encoding= MWC_UTIL_GetEncodingByLocaleCode((const HUINT8 *)message);

		OSD_FONT_CountChByWidth_SingleByte(
			pFont, nwidth, encoding,
			nStrLen, &(message[nTotal]), &nNumOfChForOneLine);

		if(nNumOfChForOneLine != 0)
		{
			nNumOfMsgTextLine++;
		}
		nTotal += (nNumOfChForOneLine - 1);

		if (nNumOfMsgTextLine == nCutline)
		{
			break;
		}

	} while(nNumOfChForOneLine != 0);

	if (pFont != NULL)
		OSD_FONT_Release (pFont);

	return (nTotal + nLocLen);
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupRecharDrawComfirmMsg_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupRecharDrawComfirmMsg_Mediaset(XmgrCas_NaPopRechar_Mediaset_t *pContents, OSD_Screen_t *pScreen, OSD_Area_t *pArea,
										OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HERROR					hError;
	Text_Property_t			stTextProperty;
	OSD_Rect_t				rect_status;
	HUINT8					*pszDisplayMessage = NULL;
	HUINT32					nNewCharNumOfLines;
	HINT32					nObjectId;
	HINT8 					szDate[128];

	//credit icon
	OSD_Font_t				*pFont;
	HINT32					nStrLen = 0, nX1;
	OSD_StockImage_t	 	stImage;

	/* for compiler */
	(void)pArg;
	(void)pRect;

	nObjectId = GWM_Obj_GetObjectID(pObject);

	stTextProperty.ulFontSize = FONT_SIZE_M;
	stTextProperty.ulFontAlign = TEXT_ALIGN_CENTER;
	stTextProperty.cFontColor = COL(C_T_Cursor);

	pszDisplayMessage = GetTextBuffer(TEXT_SIZE512);
	HxSTD_memset(pszDisplayMessage, 0, MAX_MSET_POPUP_EVTCONFIRM_LENGTH);


	// setting text style of the event comfirm msg
	OSD_GFX_SetRect(&rect_status, NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_X, NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_RECHARGE_Y,
						NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_WIDTH, NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_HEIGHT);
	nNewCharNumOfLines = xmgr_cas_NaPpvPopupRecharGetCountOfCharsFromCutLines_Mediaset(pContents->pStrTitle,
								stTextProperty.ulFontSize, rect_status.w, MAX_MSET_POPUP_EVTCONFIRM_ROW_NUM);

	MWC_UTIL_DvbStrncpy(pszDisplayMessage, pContents->pStrTitle, nNewCharNumOfLines);
	if (svc_cas_MgrUtilStrlenStrOnly(pszDisplayMessage) > 0)
	{
		hError = AP_LNF_DrawLongStringWithWordwrap(pScreen, pArea, &rect_status, pszDisplayMessage,
													stTextProperty, rect_status.w, param1);
		if (hError != ERR_OK)
		{
			HxLOG_Error(" AP_LNF_DrawLongStringWithWordwrap Error!\n");
			return ERR_FAIL;
		}
	}

	// setting text style of the event comfirm credit
	pFont = OSD_FONT_Get(eFont_SystemNormal);
	OSD_GFX_SetRect (&rect_status, NAPPV_MEDIASET_POPUP_EVTCONFIRM_CREDIT_TEXT_X, NAPPV_MEDIASET_POPUP_EVTCONFIRM_CREDIT_TEXT_RECHARGE_Y,
						NAPPV_MEDIASET_POPUP_EVTCONFIRM_CREDIT_TEXT_WIDTH, NAPPV_MEDIASET_POPUP_EVTCONFIRM_CREDIT_TEXT_HEIGHT);
	snprintf(szDate, 128, "%s %s", pContents->pStrRemainCreditTitle, pContents->ucRemainCreditStr); //all string
	nStrLen = OSD_GFX_GetStringWidth(pFont, szDate);
	nX1 = rect_status.x + (rect_status.w - nStrLen)/2;
	rect_status.x = nX1;
	if (pFont != NULL)
		OSD_FONT_Release(pFont);

	//credit text
	OSD_GFX_DrawTextByLimit(pScreen, nObjectId, eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_M, rect_status.x, rect_status.y, nStrLen,
										(HINT8*)pContents->pStrRemainCreditTitle, COL(C_T_Cursor));
	//credit value
	OSD_GFX_DrawTextByLimit(pScreen, nObjectId, eFont_SystemNormal, eFontAlign_Right, FONT_SIZE_M, rect_status.x, rect_status.y, nStrLen,
										pContents->ucRemainCreditStr, COL(C_T_MList_Yellow));
	//credit icon
	if (!AP_StockImg_GetImage (e911_4_24_Euro_Font_Yellow_B, &stImage))
	{
		OSD_GFX_SetRect (&rect_status, rect_status.x + nStrLen, rect_status.y - NAPPV_MEDIASET_POPUP_STRING_MARGIN_Y, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &rect_status);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupRecharDrawEvtRecharge_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupRecharDrawEvtRecharge_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea,
										OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HERROR					hError;
	XmgrCas_NaPopRechar_Mediaset_t	*pContents = xmgr_cas_NaPpvPopupRecharGetLocalContents_Mediaset();

	// setting text style of the event confirm info
	hError = xmgr_cas_NaPpvPopupRecharDrawComfirmMsg_Mediaset(pContents, pScreen, pArea, pRect, pObject, pArg, param1);
	if (hError != ERR_BUS_NO_ERROR)
	{
		HxLOG_Error(" xmgr_cas_NaPpvPopupRecharDrawComfirmMsg_Mediaset Error!\n");
		return ERR_FAIL;
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupRecharSetUiObjects_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupRecharSetUiObjects_Mediaset(XmgrCas_NaPopRechar_Mediaset_t *pContents)
{
	OSD_Rect_t				rect = {0, };

	/* for compiler */
	(void)pContents;

	HxLOG_Print(" +++ \n");

	/* na ppv mediaset popup recharge background */
	OSD_GFX_SetRect(&rect, NAPPV_MEDIASET_POPUP_WINBACKGROUND_X, NAPPV_MEDIASET_POPUP_WINBACKGROUND_RECHARGE_Y, NAPPV_MEDIASET_POPUP_WINBACKGROUND_WIDTH, NAPPV_MEDIASET_POPUP_WINBACKGROUND_HEIGHT);
	GWM_Frame_Create(NAPPV_MEDIASET_POPUP_WINBACKGROUND_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_MEDIASET_POPUP_WINBACKGROUND_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvPopupRecharDrawWinBackgroundImage_Mediaset);

	/* na ppv mediaset popup recharge event confirm msg*/
	OSD_GFX_SetRect(&rect, NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_X, NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_RECHARGE_Y, NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_WIDTH, NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_HEIGHT);
	GWM_Frame_Create(NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_ID, xmgr_cas_NaPpvPopupRecharDrawEvtRecharge_Mediaset);

	/* na ppv mediaset popup recharge exit button */
	OSD_GFX_SetRect(&rect, NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_BTN_X, NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_BTN_Y, NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_BTN_WIDTH, NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_BTN_HEIGHT);
	GWM_Frame_Create(NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_BTN_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_BTN_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvPopupRecharDrawExitButton_Mediaset);

	/* na ppv mediaset popup recharge exit button text*/
	OSD_GFX_SetRect (&rect, NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_TEXT_X, NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_TEXT_Y, NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_TEXT_WIDTH, NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_TEXT_HEIGHT);
	GWM_Text_Create (NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_TEXT_ID, &rect, (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_IPPV_CLOSE_ID));
	GWM_Text_SetAlign (NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_TEXT_ID, TEXT_ALIGN_LEFT);
	GWM_Text_SetSize (NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_TEXT_ID, FONT_SIZE_XS);
	GWM_Text_ChangeColor(NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_TEXT_ID, COL (C_T_Cursor), 0x00);

	HxLOG_Print(" --- \n");

	return ERR_BUS_NO_ERROR;
}


#define	_____POPUP_OPPV______________________________
/********************************************************************
 Function		: xmgr_cas_NaPpvPopupPppvGetLocalContents_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC XmgrCas_NaPopOppv_Mediaset_t* xmgr_cas_NaPpvPopupPppvGetLocalContents_Mediaset(void)
{
	return &s_stNaMsetPopOppv;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupPppvInitContents_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupPppvInitContents_Mediaset(XmgrCas_NaPopOppv_Mediaset_t *pContents, HINT32 nCredit, HINT32 nNameLength, HUINT8 *pEvtName)
{
	HxSTD_memset(pContents, 0, sizeof(XmgrCas_NaPopOppv_Mediaset_t));

	pContents->pStrTitle = (HUINT8*)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_BANNER_MSG_OPPV_NOTIFY_ID);
	pContents->pStrRemainCreditTitle = (HUINT8*)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_CREDIT_TEXT_ID);
	//xmgr_cas_NaGetScCreditStr(pContents->ucRemainCreditStr);
	snprintf(pContents->ucRemainCreditStr, 16, "%d,%02d", nCredit/100, nCredit%100); 	// 다시 credit을 가져오지 않고 받은 credit을 사용 <== card removed 대비

	HxSTD_memset(pContents->ucOppvEvtName, 0, MAX_MSET_POPUP_EVTNAME_LENGTH+1);
	if (nNameLength >= MAX_MSET_POPUP_EVTNAME_LENGTH)
	{
		MWC_UTIL_DvbStrncpy(pContents->ucOppvEvtName, pEvtName, MAX_MSET_POPUP_EVTNAME_LENGTH);
	}
	else
	{
		MWC_UTIL_DvbStrncpy(pContents->ucOppvEvtName, pEvtName, nNameLength);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupPppvDrawWinBackgroundImage_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupPppvDrawWinBackgroundImage_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
											GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HINT32				 nX1, nY1;
	OSD_Rect_t			 stRect;
	OSD_StockImage_t	 stImage;

	(void)pObject;
	(void)pArg;
	(void)param1;

	nX1 = pArea->x1 + pRect->x;
	nY1 = pArea->y1 + pRect->y;

	if (!AP_StockImg_GetImage (e913_3_24_WIN1_237, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupPppvDrawExitButton_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupPppvDrawExitButton_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
											GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HINT32				 nX1, nY1;
	OSD_Rect_t			 stRect;
	OSD_StockImage_t	 stImage;

	(void)pObject;
	(void)pArg;
	(void)param1;

	nX1 = pArea->x1 + pRect->x;
	nY1 = pArea->y1 + pRect->y;

	if (!AP_StockImg_GetImage (e911_1_24_BTN_3Exit, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupPppvGetCountOfCharsFromCutLines_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupPppvGetCountOfCharsFromCutLines_Mediaset(HUINT8 *message, HINT32 nfontsize, HINT32 nwidth, HINT16 nCutline)
{
	HINT32		nNumOfMsgTextLine = 0;
	HINT32		nTotal = 0;
	HINT32		nLocLen = 0;
	HINT32		nStrLen = 0;
	HUINT32		nNumOfChForOneLine = 0;

	OSD_Font_t	*pFont = NULL;
	eHxCP_CodePage_e	encoding = eHCodepage_MAX;

	pFont = OSD_FONT_Get(eFont_SystemNormal);
	OSD_FONT_SetSize(pFont,nfontsize);
	nTotal = MWC_UTIL_GetLocaleCodeLen (&(message[nTotal]));
	nLocLen = nTotal;
	do
	{
		nNumOfChForOneLine = 0;
		nStrLen = SvcCas_UtilStrLen(&(message[nTotal]));
		encoding= MWC_UTIL_GetEncodingByLocaleCode((const HUINT8 *)message);

		OSD_FONT_CountChByWidth_SingleByte(
			pFont, nwidth, encoding,
			nStrLen, &(message[nTotal]), &nNumOfChForOneLine);

		if(nNumOfChForOneLine != 0)
		{
			nNumOfMsgTextLine++;
		}
		nTotal += (nNumOfChForOneLine - 1);

		if (nNumOfMsgTextLine == nCutline)
		{
			break;
		}

	} while(nNumOfChForOneLine != 0);

	if (pFont != NULL)
		OSD_FONT_Release (pFont);

	return (nTotal + nLocLen);
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupPppvDrawComfirmMsg_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupPppvDrawComfirmMsg_Mediaset(XmgrCas_NaPopOppv_Mediaset_t *pContents, OSD_Screen_t *pScreen, OSD_Area_t *pArea,
										OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1)
{
	OSD_Rect_t				rect_status;
	HINT32					nObjectId;
	HINT8 					szDate[128];

	OSD_Font_t				*pFont;
	HINT32					nStrLen = 0, nX1;
	OSD_StockImage_t	 	stImage;

	/* for compiler */
	(void)pArea;
	(void)pRect;
	(void)pArg;
	(void)param1;

	nObjectId = GWM_Obj_GetObjectID(pObject);

	// setting text style of the event comfirm msg 1
	OSD_GFX_SetRect (&rect_status, NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_X, NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_OPPV_Y,
						NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_WIDTH, NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_HEIGHT);
	OSD_GFX_DrawTextByLimit(pScreen, nObjectId, eFont_SystemNormal, eFontAlign_Center, FONT_SIZE_M, rect_status.x, rect_status.y, rect_status.w,
										(HINT8 *)pContents->pStrTitle, COL(C_T_Cursor));

	// setting text style of the event comfirm msg 2
	OSD_GFX_SetRect (&rect_status, NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_X, NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_OPPV_Y + NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_LINESPACE,
						NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_WIDTH, NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_HEIGHT);
	OSD_GFX_DrawTextByLimit(pScreen, nObjectId, eFont_SystemNormal, eFontAlign_Center, FONT_SIZE_M, rect_status.x, rect_status.y, rect_status.w,
										pContents->ucOppvEvtName, COL(C_T_MList_Yellow));

	// setting text style of the event comfirm credit
	pFont = OSD_FONT_Get(eFont_SystemNormal);
	OSD_GFX_SetRect (&rect_status, NAPPV_MEDIASET_POPUP_EVTCONFIRM_CREDIT_TEXT_X, NAPPV_MEDIASET_POPUP_EVTCONFIRM_CREDIT_TEXT_OPPV_Y,
						NAPPV_MEDIASET_POPUP_EVTCONFIRM_CREDIT_TEXT_WIDTH, NAPPV_MEDIASET_POPUP_EVTCONFIRM_CREDIT_TEXT_HEIGHT);
	snprintf(szDate, 128, "%s %s", pContents->pStrRemainCreditTitle, pContents->ucRemainCreditStr); //all string
	nStrLen = OSD_GFX_GetStringWidth(pFont, szDate);
	nX1 = rect_status.x + (rect_status.w - nStrLen)/2;
	rect_status.x = nX1;
	if (pFont != NULL)
		OSD_FONT_Release(pFont);

	//credit text
	OSD_GFX_DrawTextByLimit(pScreen, nObjectId, eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_M, rect_status.x, rect_status.y, nStrLen,
										(HINT8*)pContents->pStrRemainCreditTitle, COL(C_T_Cursor));
	//credit value
	OSD_GFX_DrawTextByLimit(pScreen, nObjectId, eFont_SystemNormal, eFontAlign_Right, FONT_SIZE_M, rect_status.x, rect_status.y, nStrLen,
										pContents->ucRemainCreditStr, COL(C_T_MList_Yellow));
	//credit icon
	if (!AP_StockImg_GetImage (e911_4_24_Euro_Font_Yellow_B, &stImage))
	{
		OSD_GFX_SetRect (&rect_status, rect_status.x + nStrLen, rect_status.y - NAPPV_MEDIASET_POPUP_STRING_MARGIN_Y, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &rect_status);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupPppvDrawEvtRecharge_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupPppvDrawEvtRecharge_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea,
										OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HERROR					hError;
	XmgrCas_NaPopOppv_Mediaset_t	*pContents = xmgr_cas_NaPpvPopupPppvGetLocalContents_Mediaset();

	// setting text style of the event confirm info
	hError = xmgr_cas_NaPpvPopupPppvDrawComfirmMsg_Mediaset(pContents, pScreen, pArea, pRect, pObject, pArg, param1);
	if (hError != ERR_BUS_NO_ERROR)
	{
		HxLOG_Error(" xmgr_cas_NaPpvPopupPppvDrawComfirmMsg_Mediaset Error!\n");
		return ERR_FAIL;
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupPppvSetUiObjects_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupPppvSetUiObjects_Mediaset(XmgrCas_NaPopOppv_Mediaset_t *pContents)
{
	OSD_Rect_t				rect = {0, };

	/* for compiler */
	(void)pContents;

	HxLOG_Print(" +++ \n");

	/* na ppv mediaset popup oppv background */
	OSD_GFX_SetRect(&rect, NAPPV_MEDIASET_POPUP_WINBACKGROUND_X, NAPPV_MEDIASET_POPUP_WINBACKGROUND_OPPV_Y, NAPPV_MEDIASET_POPUP_WINBACKGROUND_WIDTH, NAPPV_MEDIASET_POPUP_WINBACKGROUND_HEIGHT);
	GWM_Frame_Create(NAPPV_MEDIASET_POPUP_WINBACKGROUND_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_MEDIASET_POPUP_WINBACKGROUND_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvPopupPppvDrawWinBackgroundImage_Mediaset);

	/* na ppv mediaset popup oppv event confirm msg*/
	OSD_GFX_SetRect(&rect, NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_X, NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_OPPV_Y, NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_WIDTH, NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_HEIGHT);
	GWM_Frame_Create(NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_ID, xmgr_cas_NaPpvPopupPppvDrawEvtRecharge_Mediaset);

	/* na ppv mediaset popup oppv exit button */
	OSD_GFX_SetRect(&rect, NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_BTN_X, NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_BTN_Y, NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_BTN_WIDTH, NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_BTN_HEIGHT);
	GWM_Frame_Create(NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_BTN_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_BTN_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvPopupPppvDrawExitButton_Mediaset);

	/* na ppv mediaset popup oppv exit button text*/
	OSD_GFX_SetRect (&rect, NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_TEXT_X, NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_TEXT_Y, NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_TEXT_WIDTH, NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_TEXT_HEIGHT);
	GWM_Text_Create (NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_TEXT_ID, &rect, (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_IPPV_CLOSE_ID));
	GWM_Text_SetAlign (NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_TEXT_ID, TEXT_ALIGN_LEFT);
	GWM_Text_SetSize (NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_TEXT_ID, FONT_SIZE_XS);
	GWM_Text_ChangeColor(NAPPV_MEDIASET_POPUP_EVTCONFIRM_EXIT_TEXT_ID, COL (C_T_Cursor), 0x00);

	HxLOG_Print(" --- \n");

	return ERR_BUS_NO_ERROR;
}

#define	_____POPUP_IPPV_RESULT______________________________
/********************************************************************
 Function		: xmgr_cas_NaPpvPopupIppvResultGetLocalContents_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC XmgrCas_NaPopIppvResult_Mediaset_t* xmgr_cas_NaPpvPopupIppvResultGetLocalContents_Mediaset(void)
{
	return &s_stNaMsetPopIppvResult;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupIppvResultInitContents_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupIppvResultInitContents_Mediaset(XmgrCas_NaPopIppvResult_Mediaset_t *pContents)
{
	HxSTD_memset(pContents, 0, sizeof(XmgrCas_NaPopIppvResult_Mediaset_t));

	pContents->pStrTitle = (HUINT8*)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_IPPV_PURCHASE_DONE_ID);
	pContents->pStrRemainCreditTitle = (HUINT8*)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_IPPV_RESIDUAL_CREDIT_ID);
	xmgr_cas_NaGetScCreditStr(pContents->ucRemainCreditStr);

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupIppvResultDrawWinBackgroundImage_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupIppvResultDrawWinBackgroundImage_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
											GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HINT32				 nX1, nY1;
	OSD_Rect_t			 stRect;
	OSD_StockImage_t	 stImage;

	(void)pObject;
	(void)pArg;
	(void)param1;

	nX1 = pArea->x1 + pRect->x;
	nY1 = pArea->y1 + pRect->y;

	if (!AP_StockImg_GetImage (e913_3_24_WIN1_181, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupIppvResultGetCountOfCharsFromCutLines_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupIppvResultGetCountOfCharsFromCutLines_Mediaset(HUINT8 *message, HINT32 nfontsize, HINT32 nwidth, HINT16 nCutline)
{
	HINT32		nNumOfMsgTextLine = 0;
	HINT32		nTotal = 0;
	HINT32		nLocLen = 0;
	HINT32		nStrLen = 0;
	HUINT32		nNumOfChForOneLine = 0;

	OSD_Font_t	*pFont = NULL;
	eHxCP_CodePage_e	encoding = eHCodepage_MAX;

	pFont = OSD_FONT_Get(eFont_SystemNormal);
	OSD_FONT_SetSize(pFont,nfontsize);
	nTotal = MWC_UTIL_GetLocaleCodeLen (&(message[nTotal]));
	nLocLen = nTotal;
	do
	{
		nNumOfChForOneLine = 0;
		nStrLen = SvcCas_UtilStrLen(&(message[nTotal]));
		encoding= MWC_UTIL_GetEncodingByLocaleCode((const HUINT8 *)message);

		OSD_FONT_CountChByWidth_SingleByte(
			pFont, nwidth, encoding,
			nStrLen, &(message[nTotal]), &nNumOfChForOneLine);

		if(nNumOfChForOneLine != 0)
		{
			nNumOfMsgTextLine++;
		}
		nTotal += (nNumOfChForOneLine - 1);

		if (nNumOfMsgTextLine == nCutline)
		{
			break;
		}

	} while(nNumOfChForOneLine != 0);

	if (pFont != NULL)
		OSD_FONT_Release (pFont);

	return (nTotal + nLocLen);
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupIppvResultDrawComfirmMsg_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupIppvResultDrawComfirmMsg_Mediaset(XmgrCas_NaPopIppvResult_Mediaset_t *pContents, OSD_Screen_t *pScreen, OSD_Area_t *pArea,
										OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HERROR					hError;
	Text_Property_t			stTextProperty;
	OSD_Rect_t				rect_status;
	HUINT8					*pszDisplayMessage = NULL;
	HUINT32					nNewCharNumOfLines;
	HINT32					nObjectId;
	HINT8 					szDate[128];

	//credit icon
	OSD_Font_t				*pFont;
	HINT32					nStrLen = 0, nX1;
	OSD_StockImage_t	 	stImage;

	/* for compiler */
	(void)pArg;
	(void)pRect;

	nObjectId = GWM_Obj_GetObjectID(pObject);

	stTextProperty.ulFontSize = FONT_SIZE_M;
	stTextProperty.ulFontAlign = TEXT_ALIGN_CENTER;
	stTextProperty.cFontColor = COL(C_T_Cursor);

	pszDisplayMessage = GetTextBuffer(TEXT_SIZE512);
	HxSTD_memset(pszDisplayMessage, 0, MAX_MSET_POPUP_EVTCONFIRM_LENGTH);


	// setting text style of the event comfirm msg
	OSD_GFX_SetRect(&rect_status, NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_X, NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_IPPVRESULT_Y,
						NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_WIDTH, NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_HEIGHT);
	nNewCharNumOfLines = xmgr_cas_NaPpvPopupIppvResultGetCountOfCharsFromCutLines_Mediaset(pContents->pStrTitle,
								stTextProperty.ulFontSize, rect_status.w, MAX_MSET_POPUP_EVTCONFIRM_ROW_NUM);

	MWC_UTIL_DvbStrncpy(pszDisplayMessage, pContents->pStrTitle, nNewCharNumOfLines);
	if (svc_cas_MgrUtilStrlenStrOnly(pszDisplayMessage) > 0)
	{
		hError = AP_LNF_DrawLongStringWithWordwrap(pScreen, pArea, &rect_status, pszDisplayMessage,
													stTextProperty, rect_status.w, param1);
		if (hError != ERR_OK)
		{
			HxLOG_Error(" AP_LNF_DrawLongStringWithWordwrap Error!\n");
			return ERR_FAIL;
		}
	}

	// setting text style of the event comfirm credit
	pFont = OSD_FONT_Get(eFont_SystemNormal);
	OSD_GFX_SetRect (&rect_status, NAPPV_MEDIASET_POPUP_EVTCONFIRM_CREDIT_TEXT_X, NAPPV_MEDIASET_POPUP_EVTCONFIRM_CREDIT_TEXT_IPPVRESULT_Y,
						NAPPV_MEDIASET_POPUP_EVTCONFIRM_CREDIT_TEXT_WIDTH, NAPPV_MEDIASET_POPUP_EVTCONFIRM_CREDIT_TEXT_HEIGHT);
	snprintf(szDate, 128, "%s %s", pContents->pStrRemainCreditTitle, pContents->ucRemainCreditStr); //all string
	nStrLen = OSD_GFX_GetStringWidth(pFont, szDate);
	nX1 = rect_status.x + (rect_status.w - nStrLen)/2;
	rect_status.x = nX1;
	if (pFont != NULL)
		OSD_FONT_Release(pFont);

	//credit text
	OSD_GFX_DrawTextByLimit(pScreen, nObjectId, eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_M, rect_status.x, rect_status.y, nStrLen,
										(HINT8*)pContents->pStrRemainCreditTitle, COL(C_T_Cursor));
	//credit value
	OSD_GFX_DrawTextByLimit(pScreen, nObjectId, eFont_SystemNormal, eFontAlign_Right, FONT_SIZE_M, rect_status.x, rect_status.y, nStrLen,
										pContents->ucRemainCreditStr, COL(C_T_MList_Yellow));
	//credit icon
	if (!AP_StockImg_GetImage (e911_4_24_Euro_Font_Yellow_B, &stImage))
	{
		OSD_GFX_SetRect (&rect_status, rect_status.x + nStrLen, rect_status.y - NAPPV_MEDIASET_POPUP_STRING_MARGIN_Y, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &rect_status);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupIppvResultDrawEvtIppvResult_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupIppvResultDrawEvtIppvResult_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea,
										OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HERROR					hError;
	XmgrCas_NaPopIppvResult_Mediaset_t	*pContents = xmgr_cas_NaPpvPopupIppvResultGetLocalContents_Mediaset();

	// setting text style of the event confirm info
	hError = xmgr_cas_NaPpvPopupIppvResultDrawComfirmMsg_Mediaset(pContents, pScreen, pArea, pRect, pObject, pArg, param1);
	if (hError != ERR_BUS_NO_ERROR)
	{
		HxLOG_Error(" xmgr_cas_NaPpvPopupIppvResultDrawEvtIppvResult_Mediaset Error!\n");
		return ERR_FAIL;
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupIppvResultSetUiObjects_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupIppvResultSetUiObjects_Mediaset(XmgrCas_NaPopIppvResult_Mediaset_t *pContents)
{
	OSD_Rect_t				rect = {0, };

	/* for compiler */
	(void)pContents;

	HxLOG_Print(" +++ \n");

	/* na ppv mediaset popup ippv result background */
	OSD_GFX_SetRect(&rect, NAPPV_MEDIASET_POPUP_WINBACKGROUND_X, NAPPV_MEDIASET_POPUP_WINBACKGROUND_IPPVRESULT_Y, NAPPV_MEDIASET_POPUP_WINBACKGROUND_WIDTH, NAPPV_MEDIASET_POPUP_WINBACKGROUND_IPPVRESULT_HEIGHT);
	GWM_Frame_Create(NAPPV_MEDIASET_POPUP_WINBACKGROUND_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_MEDIASET_POPUP_WINBACKGROUND_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvPopupIppvResultDrawWinBackgroundImage_Mediaset);

	/* na ppv mediaset popup ippv result event confirm msg*/
	OSD_GFX_SetRect(&rect, NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_X, NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_IPPVRESULT_Y, NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_WIDTH, NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_HEIGHT);
	GWM_Frame_Create(NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_MEDIASET_POPUP_EVTCONFIRM_MSG_ID, xmgr_cas_NaPpvPopupIppvResultDrawEvtIppvResult_Mediaset);

	HxLOG_Print(" --- \n");

	return ERR_BUS_NO_ERROR;
}

#define ________XMGR_CAS_NA_PpvPOPUP_PUBLIC___________________________________________
/******************************************************************************/
/**
*   \fn     xproc_cas_NaPpvPopupRecharge_Mediaset(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
*   \brief  system status procedure
*
*   @param[in]  gwm msg/action handler
*   @return     BUS_ProcessMessageDefault(message, hAction, p1, p2, p3)
*   @ingroup    SYSTEM STATUS
*/
/******************************************************************************/
BUS_Result_t xproc_cas_NaPpvPopupRecharge_Mediaset(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrCas_NaPopRechar_Mediaset_t			*pContents = xmgr_cas_NaPpvPopupRecharGetLocalContents_Mediaset();

	switch(message)
	{
		case MSG_GWM_CREATE:
			/* set app area */
			GWM_APP_SetAppArea(NAPPV_MEDIASET_POPUP_WINDOW_X, NAPPV_MEDIASET_POPUP_WINDOW_Y,
								NAPPV_MEDIASET_POPUP_WINDOW_WIDTH, NAPPV_MEDIASET_POPUP_WINDOW_HEIGHT);

			/* initialize recharge content data */
			xmgr_cas_NaPpvPopupRecharInitContents_Mediaset(pContents, p1);

			/* set ui objects */
			xmgr_cas_NaPpvPopupRecharSetUiObjects_Mediaset(pContents);

			GWM_APP_PreventDraw_LowPriority(APP_CAS_HIGH_PRIORITY);
			break;

		case MSG_GWM_OBJ_FOCUS_CHANGED:
			return MESSAGE_BREAK;

		case MSG_GWM_CLICKED :
			break;

		case MSG_GWM_KEYUP :
			AP_KeyConv_UiCommon(&p1);
			break;

		case MSG_GWM_KEYDOWN :
			AP_KeyConv_UiCommon(&p1);

			switch(p1)
			{
				case KEY_EXIT:
					BUS_MGR_Destroy(NULL);
					return MESSAGE_BREAK;

				default:
					return MESSAGE_BREAK;
			}
			break;

		case MSG_GWM_SCROLLUP:
			return MESSAGE_BREAK;

		case MSG_GWM_SCROLLDOWN:
			return MESSAGE_BREAK;

		case MSG_GWM_DESTROY:
			/* destroy UI */
			GWM_APP_DisablePreventDraw();

			BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_NOTIFY_PPV_POPUP_CLOSED, (Handle_t)0, eDxCAS_GROUPID_NA, (HINT32)0, (HINT32)0);

			break;

		default:
			break;
	}

	HxLOG_Info("xproc_cas_NaPpvPopupRecharge_Mediaset handle(0x%x)\n", hAction);

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}


/******************************************************************************/
/**
*   \fn     xproc_cas_NaPpvPopupOppv_Mediaset(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
*   \brief  system status procedure
*
*   @param[in]  gwm msg/action handler
*   @return     BUS_ProcessMessageDefault(message, hAction, p1, p2, p3)
*   @ingroup    SYSTEM STATUS
*/
/******************************************************************************/
BUS_Result_t xproc_cas_NaPpvPopupOppv_Mediaset(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrCas_NaPopOppv_Mediaset_t			*pContents = xmgr_cas_NaPpvPopupPppvGetLocalContents_Mediaset();

	switch(message)
	{
		case MSG_GWM_CREATE:
			/* set app area */
			GWM_APP_SetAppArea(NAPPV_MEDIASET_POPUP_WINDOW_X, NAPPV_MEDIASET_POPUP_WINDOW_Y,
								NAPPV_MEDIASET_POPUP_WINDOW_WIDTH, NAPPV_MEDIASET_POPUP_WINDOW_HEIGHT);

			/* initialize oppv content data */
			xmgr_cas_NaPpvPopupPppvInitContents_Mediaset(pContents, p1, p2, (HUINT8*)p3);

			/* set ui objects */
			xmgr_cas_NaPpvPopupPppvSetUiObjects_Mediaset(pContents);

			GWM_APP_PreventDraw_LowPriority(APP_CAS_HIGH_PRIORITY);
			break;

		case MSG_GWM_OBJ_FOCUS_CHANGED:
			return MESSAGE_BREAK;

		case MSG_GWM_CLICKED :
			break;

		case MSG_GWM_KEYUP :
			AP_KeyConv_UiCommon(&p1);
			break;

		case MSG_GWM_KEYDOWN :
			AP_KeyConv_UiCommon(&p1);

			switch(p1)
			{
				case KEY_EXIT:
					BUS_MGR_Destroy(NULL);
					return MESSAGE_BREAK;

				default:
					return MESSAGE_BREAK;
			}
			break;

		case MSG_GWM_SCROLLUP:
			return MESSAGE_BREAK;

		case MSG_GWM_SCROLLDOWN:
			return MESSAGE_BREAK;

		case MSG_GWM_DESTROY:
			/* destroy UI */
			GWM_APP_DisablePreventDraw();

			BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_NOTIFY_PPV_POPUP_CLOSED, (Handle_t)0, eDxCAS_GROUPID_NA, (HINT32)0, (HINT32)0);

			break;

		default:
			break;
	}

	HxLOG_Info("xproc_cas_NaPpvPopupOppv_Mediaset handle(0x%x)\n", hAction);

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

/******************************************************************************/
/**
*   \fn     xproc_cas_NaPpvPopupIppvResult_Mediaset(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT8 *p3)
*   \brief  system status procedure
*
*   @param[in]  gwm msg/action handler
*   @return     BUS_ProcessMessageDefault(message, hAction, p1, p2, p3)
*   @ingroup    SYSTEM STATUS
*/
/******************************************************************************/
BUS_Result_t xproc_cas_NaPpvPopupIppvResult_Mediaset(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrCas_NaPopIppvResult_Mediaset_t	*pContents = xmgr_cas_NaPpvPopupIppvResultGetLocalContents_Mediaset();

	switch(message)
	{
		case MSG_GWM_CREATE:
			/* set app area */
			GWM_APP_SetAppArea(NAPPV_MEDIASET_POPUP_WINDOW_X, NAPPV_MEDIASET_POPUP_WINDOW_Y,
								NAPPV_MEDIASET_POPUP_WINDOW_WIDTH, NAPPV_MEDIASET_POPUP_WINDOW_HEIGHT);

			/* initialize ippv result content data */
			xmgr_cas_NaPpvPopupIppvResultInitContents_Mediaset(pContents);

			/* set ui objects */
			xmgr_cas_NaPpvPopupIppvResultSetUiObjects_Mediaset(pContents);

			GWM_APP_PreventDraw_LowPriority(APP_CAS_HIGH_PRIORITY);

			BUS_SetTimer(CAS_NA_PPV_MEDIASET_POPUP_IPPVRESULT_DISPLAY_CHECK_CREDIT_ID, CAS_NA_PPV_MEDIASET_POPUP_IPPVRESULT_DISPLAY_CREDIT_PERIOD);
			break;

		case MSG_GWM_OBJ_FOCUS_CHANGED:
			return MESSAGE_BREAK;

		case MSG_GWM_CLICKED :
			break;

		case MSG_GWM_KEYUP :
			AP_KeyConv_UiCommon(&p1);
			break;

		case MSG_GWM_KEYDOWN :
			AP_KeyConv_UiCommon(&p1);
			return MESSAGE_BREAK;

		case MSG_GWM_SCROLLUP:
			return MESSAGE_BREAK;

		case MSG_GWM_SCROLLDOWN:
			return MESSAGE_BREAK;

		case MSG_GWM_TIMER:
			if (p1 == CAS_NA_PPV_MEDIASET_POPUP_IPPVRESULT_DISPLAY_CHECK_CREDIT_ID)
			{
				BUS_KillTimer(CAS_NA_PPV_MEDIASET_POPUP_IPPVRESULT_DISPLAY_CHECK_CREDIT_ID);
				BUS_MGR_Destroy(NULL);

				return MESSAGE_BREAK;
			}
			break;

		case MSG_GWM_DESTROY:
			/* destroy UI */
			GWM_APP_DisablePreventDraw();

			BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_NOTIFY_PPV_POPUP_CLOSED, (Handle_t)0, eDxCAS_GROUPID_NA, (HINT32)0, (HINT32)0);

			break;

		default:
			break;
	}

	HxLOG_Info("xproc_cas_NaPpvPopupIppvResult_Mediaset handle(0x%x)\n", hAction);

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

#endif //#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
