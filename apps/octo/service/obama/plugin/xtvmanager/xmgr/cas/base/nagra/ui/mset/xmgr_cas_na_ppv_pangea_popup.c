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

#include "../../local_include/_xmgr_cas_na_util.h"
#include "../../local_include/_xmgr_cas_na_adapt.h"
#include "../../local_include/_xmgr_cas_na.h"


#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
//******************************************************************************
//	DEFINE
//******************************************************************************

#define	MAX_PGEA_POPUP_EVTNAME_LENGTH				50
#define	MAX_PGEA_POPUP_EVTCONFIRM_LENGTH			512

typedef struct tag_CAS_NA_PANGEA_PR_Contents // Popup Recharge
{
	HUINT8			*pStrTitle; 					///< title of app context
	HUINT8			*pStrRemainCreditTitle;
	HINT8			ucRemainCreditStr[16];
} XmgrCas_NaPopRechar_Pangea_t;

typedef struct tag_CAS_NA_PANGEA_OPPV_Contents // Popup OPPV
{
	HUINT8			*pStrTitle; 					///< title of app context
	HUINT8			*pStrRemainCreditTitle;
	HINT8			ucRemainCreditStr[16];
	HINT8			ucOppvEvtName[MAX_PGEA_POPUP_EVTNAME_LENGTH+1];
} XmgrCas_NaPopOppv_Pangea_t;

typedef struct tag_CAS_NA_PANGEA_IPPV_RESULT_Contents // Popup IPPV Result
{
	HUINT8			*pStrTitle; 					///< title of app context
	HUINT8			*pStrRemainCreditTitle;
	HINT8			ucRemainCreditStr[16];
} XmgrCas_NaPopIppvResult_Pangea_t;

/****************************************************************/
//PANGEA Recharge, Oppv 좌표
/****************************************************************/
#define	NAPPV_PANGEA_POPUP_WINDOW_WIDTH									OSD_SCREEN_WIDTH
#define	NAPPV_PANGEA_POPUP_WINDOW_HEIGHT								OSD_SCREEN_HEIGHT
#define	NAPPV_PANGEA_POPUP_WINDOW_X										0
#define	NAPPV_PANGEA_POPUP_WINDOW_Y										0
#define	NAPPV_PANGEA_POPUP_STRING_MARGIN_Y								18

#define	NAPPV_PANGEA_POPUP_APP_ID										1

/* ppv pangea popup background */
#define	NAPPV_PANGEA_POPUP_WINBACKGROUND_TITLE_ID						(NAPPV_PANGEA_POPUP_APP_ID|0x0010)
#define	NAPPV_PANGEA_POPUP_WINBACKGROUND_TITLE_X						0
#define	NAPPV_PANGEA_POPUP_WINBACKGROUND_TITLE_Y						227
#define	NAPPV_PANGEA_POPUP_WINBACKGROUND_TITLE_WIDTH					1280
#define	NAPPV_PANGEA_POPUP_WINBACKGROUND_TITLE_HEIGHT					67

#define	NAPPV_PANGEA_POPUP_WINBACKGROUND_ID								(NAPPV_PANGEA_POPUP_APP_ID|0x0020)
#define	NAPPV_PANGEA_POPUP_WINBACKGROUND_X								0
#define	NAPPV_PANGEA_POPUP_WINBACKGROUND_Y								294
#define	NAPPV_PANGEA_POPUP_WINBACKGROUND_WIDTH							1280
#define	NAPPV_PANGEA_POPUP_WINBACKGROUND_HEIGHT							121

/* ppv pangea popup event confirm msg info*/
#define	NAPPV_PANGEA_POPUP_EVTCONFIRM_MSG_ID							(NAPPV_PANGEA_POPUP_APP_ID|0x0030)
#define	NAPPV_PANGEA_POPUP_EVTCONFIRM_MSG_X								310
#define	NAPPV_PANGEA_POPUP_EVTCONFIRM_MSG_Y								265
#define	NAPPV_PANGEA_POPUP_EVTCONFIRM_MSG_WIDTH							500
#define	NAPPV_PANGEA_POPUP_EVTCONFIRM_MSG_HEIGHT						(34 * 4)

/* ppv pangea popup event confirm credit text */
#define	NAPPV_PANGEA_POPUP_EVTCONFIRM_CREDIT_TEXT_X						240
#define	NAPPV_PANGEA_POPUP_EVTCONFIRM_CREDIT_TEXT_Y						339
#define	NAPPV_PANGEA_POPUP_EVTCONFIRM_CREDIT_TEXT_WIDTH					800
#define	NAPPV_PANGEA_POPUP_EVTCONFIRM_CREDIT_TEXT_HEIGHT				34

/* ppv pangea popup event confirm exit button */
#define	NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_BTN_ID						(NAPPV_PANGEA_POPUP_APP_ID|0x0040)
#define	NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_BTN_X						560
#define	NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_BTN_Y						357
#define	NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_BTN_WIDTH					86
#define	NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_BTN_HEIGHT					44

/* ppv pangea popup event confirm exit button text*/
#define	NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_TEXT_ID						(NAPPV_PANGEA_POPUP_APP_ID|0x0050)
#define	NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_TEXT_X						652
#define	NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_TEXT_Y						(386 - FONT_SIZE_XS)
#define	NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_TEXT_WIDTH					250
#define	NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_TEXT_HEIGHT					44

/* recharge/oppv 메시지를 받고나서 몇초 동안 credit update를 기다린다.  */
#define CAS_NA_PPV_PANGEA_POPUP_RECHARGE_CHECK_CREDIT_ID				(0x11050151)
#define CAS_NA_PPV_PANGEA_POPUP_OPPV_CHECK_CREDIT_ID					(0x11050152)
#define CAS_NA_PPV_PANGEA_POPUP_IPPVRESULT_CHECK_CREDIT_ID				(0x11050153)
#define CAS_NA_PPV_PANGEA_POPUP_IPPVRESULT_DISPLAY_CHECK_CREDIT_ID			(0x11050154)
#define CAS_NA_PPV_PANGEA_POPUP_IPPVRESULT_DISPLAY_CREDIT_PERIOD			4000


STATIC XmgrCas_NaPopRechar_Pangea_t 		s_stNaPgeaPopReChar;
STATIC XmgrCas_NaPopOppv_Pangea_t 			s_stNaPgeaPopOppv;
STATIC XmgrCas_NaPopIppvResult_Pangea_t 	s_stNaPgeaPopIppvResult;

#define ________XMGR_CAS_NA_PpvPOPUP_STATIC___________________________________________
#define	_____POPUP_RECHARGE______________________________
STATIC XmgrCas_NaPopRechar_Pangea_t* xmgr_cas_NaPpvPopupRecharGetLocalContents_Pangea(void);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupRecharInitContents_Pangea(XmgrCas_NaPopRechar_Pangea_t *pContents, HINT32 nCredit);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupRecharDrawWinTitleBackgroundImage_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupRecharDrawWinBackgroundImage_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupRecharDrawExitButton_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupRecharGetCountOfCharsFromCutLines_Pangea(HUINT8 *message, HINT32 nfontsize, HINT32 nwidth, HINT16 nCutline);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupRecharDrawComfirmMsg_Pangea(XmgrCas_NaPopRechar_Pangea_t *pContents, OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupRecharDrawEvtRecharge_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupRecharSetUiObjects_Pangea(XmgrCas_NaPopRechar_Pangea_t *pContents);

#define	_____POPUP_OPPV______________________________
STATIC XmgrCas_NaPopOppv_Pangea_t* xmgr_cas_NaPpvPopupOppvGetLocalContents_Pangea(void);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupOppvInitContents_Pangea(XmgrCas_NaPopOppv_Pangea_t *pContents, HINT32 nCredit, HINT32 nNameLength, HUINT8 *pEvtName);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupOppvDrawWinTitleBackgroundImage_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupOppvDrawWinBackgroundImage_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupOppvDrawExitButton_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupOppvGetCountOfCharsFromCutLines_Pangea(HUINT8 *message, HINT32 nfontsize, HINT32 nwidth, HINT16 nCutline);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupOppvDrawComfirmMsg_Pangea(XmgrCas_NaPopOppv_Pangea_t *pContents, OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupOppvDrawEvtRecharge_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupOppvSetUiObjects_Pangea(XmgrCas_NaPopOppv_Pangea_t *pContents);

#define	_____POPUP_IPPV_RESULT______________________________
STATIC XmgrCas_NaPopIppvResult_Pangea_t* xmgr_cas_NaPpvPopupIppvResultGetLocalContents_Pangea(void);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupIppvResultInitContents_Pangea(XmgrCas_NaPopIppvResult_Pangea_t *pContents);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupIppvResultDrawWinTitleBackgroundImage_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupIppvResultDrawWinBackgroundImage_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupIppvResultGetCountOfCharsFromCutLines_Pangea(HUINT8 *message, HINT32 nfontsize, HINT32 nwidth, HINT16 nCutline);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupIppvResultDrawComfirmMsg_Pangea(XmgrCas_NaPopIppvResult_Pangea_t *pContents, OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupIppvResultDrawEvtIppvResult_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvPopupIppvResultSetUiObjects_Pangea(XmgrCas_NaPopIppvResult_Pangea_t *pContents);


#define	____________________________________________________________
#define	_________________________BODY______________________________
#define	____________________________________________________________


#define	_____POPUP_RECHARGE______________________________
/********************************************************************
 Function		: xmgr_cas_NaPpvPopupRecharGetLocalContents_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC XmgrCas_NaPopRechar_Pangea_t* xmgr_cas_NaPpvPopupRecharGetLocalContents_Pangea(void)
{
	return &s_stNaPgeaPopReChar;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupRecharInitContents_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupRecharInitContents_Pangea(XmgrCas_NaPopRechar_Pangea_t *pContents, HINT32 nCredit)
{
	HxSTD_memset(pContents, 0, sizeof(XmgrCas_NaPopRechar_Pangea_t));

	pContents->pStrTitle = (HUINT8*)GetStrRsc(LOC_CAS_NAGRA_PANGEA_BANNER_MSG_RECHARGED_NOTIFY_ID);
	pContents->pStrRemainCreditTitle = (HUINT8*)GetStrRsc(LOC_CAS_NAGRA_PANGEA_RECHARGED_CREDIT_ID);
	//xmgr_cas_NaGetScCreditStr(pContents->ucRemainCreditStr);
	snprintf(pContents->ucRemainCreditStr, 16, "%d,%02d", nCredit/100, nCredit%100); 	// 다시 credit을 가져오지 않고 받은 credit을 사용 <== card removed 대비

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupRecharDrawWinTitleBackgroundImage_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupRecharDrawWinTitleBackgroundImage_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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

	if (!AP_StockImg_GetImage (e922_1_24_Banner1_Title, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupRecharDrawWinBackgroundImage_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupRecharDrawWinBackgroundImage_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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

	if (!AP_StockImg_GetImage (e922_1_24_Banner1, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupRecharDrawExitButton_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupRecharDrawExitButton_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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

	if (!AP_StockImg_GetImage (e921_1_24_BTN_5Exit, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupRecharGetCountOfCharsFromCutLines_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupRecharGetCountOfCharsFromCutLines_Pangea(HUINT8 *message, HINT32 nfontsize, HINT32 nwidth, HINT16 nCutline)
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
		OSD_FONT_Release(pFont);

	return (nTotal + nLocLen);
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupRecharDrawComfirmMsg_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupRecharDrawComfirmMsg_Pangea(XmgrCas_NaPopRechar_Pangea_t *pContents, OSD_Screen_t *pScreen, OSD_Area_t *pArea,
										OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1)
{
	OSD_Rect_t				rect_status;
	HINT32					nObjectId;
	HINT8 					szDate[128];

	//credit icon
	OSD_Font_t				*pFont;
	HINT32					nStrLen = 0, nX1;
	OSD_StockImage_t	 	stImage;

	/* for compiler */
	(void)pArea;
	(void)pRect;
	(void)pArg;
	(void)param1;

	nObjectId = GWM_Obj_GetObjectID(pObject);

	// setting text style of the event comfirm msg
	OSD_GFX_SetRect (&rect_status, NAPPV_PANGEA_POPUP_EVTCONFIRM_MSG_X, NAPPV_PANGEA_POPUP_EVTCONFIRM_MSG_Y,
						NAPPV_PANGEA_POPUP_EVTCONFIRM_MSG_WIDTH, NAPPV_PANGEA_POPUP_EVTCONFIRM_MSG_HEIGHT);
	OSD_GFX_DrawTextByLimit(pScreen, nObjectId, eFont_SystemNormal, eFontAlign_Right, FONT_SIZE_M, rect_status.x, rect_status.y, rect_status.w,
										(HINT8 *)pContents->pStrTitle, COL(C_T_Cursor));


	// setting text style of the event comfirm credit
	pFont = OSD_FONT_Get(eFont_SystemNormal);
	OSD_GFX_SetRect (&rect_status, NAPPV_PANGEA_POPUP_EVTCONFIRM_CREDIT_TEXT_X, NAPPV_PANGEA_POPUP_EVTCONFIRM_CREDIT_TEXT_Y,
						NAPPV_PANGEA_POPUP_EVTCONFIRM_CREDIT_TEXT_WIDTH, NAPPV_PANGEA_POPUP_EVTCONFIRM_CREDIT_TEXT_HEIGHT);
	snprintf(szDate, 128, "%s %s", pContents->pStrRemainCreditTitle, pContents->ucRemainCreditStr); //all string
	nStrLen = OSD_GFX_GetStringWidth(pFont, szDate);
	nX1 = rect_status.x + (rect_status.w - nStrLen)/2;
	rect_status.x = nX1;
	if (pFont != NULL)
		OSD_FONT_Release(pFont);

	//credit text
	OSD_GFX_DrawTextByLimit(pScreen, nObjectId, eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_M, rect_status.x, rect_status.y, nStrLen,
										(HINT8*)pContents->pStrRemainCreditTitle, COL(C_T_PList_Orange));
	//credit value
	OSD_GFX_DrawTextByLimit(pScreen, nObjectId, eFont_SystemNormal, eFontAlign_Right, FONT_SIZE_M, rect_status.x, rect_status.y, nStrLen,
										pContents->ucRemainCreditStr, COL(C_T_Cursor));
	//credit icon
	if (!AP_StockImg_GetImage (e911_4_24_Euro_Font_White_B, &stImage))
	{
		OSD_GFX_SetRect (&rect_status, rect_status.x + nStrLen, rect_status.y - NAPPV_PANGEA_POPUP_STRING_MARGIN_Y, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &rect_status);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupRecharDrawEvtRecharge_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupRecharDrawEvtRecharge_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea,
										OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HERROR					hError;
	XmgrCas_NaPopRechar_Pangea_t	*pContents = xmgr_cas_NaPpvPopupRecharGetLocalContents_Pangea();

	// setting text style of the event confirm info
	hError = xmgr_cas_NaPpvPopupRecharDrawComfirmMsg_Pangea(pContents, pScreen, pArea, pRect, pObject, pArg, param1);
	if (hError != ERR_BUS_NO_ERROR)
	{
		HxLOG_Error(" xmgr_cas_NaPpvPopupRecharDrawComfirmMsg_Pangea Error!\n");
		return ERR_FAIL;
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupRecharSetUiObjects_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupRecharSetUiObjects_Pangea(XmgrCas_NaPopRechar_Pangea_t *pContents)
{
	OSD_Rect_t				rect = {0, };

	/* for compiler */
	(void)pContents;

	HxLOG_Print(" +++ \n");

	/* na ppv pgea popup recharge background */
	OSD_GFX_SetRect(&rect, NAPPV_PANGEA_POPUP_WINBACKGROUND_TITLE_X, NAPPV_PANGEA_POPUP_WINBACKGROUND_TITLE_Y, NAPPV_PANGEA_POPUP_WINBACKGROUND_TITLE_WIDTH, NAPPV_PANGEA_POPUP_WINBACKGROUND_TITLE_HEIGHT);
	GWM_Frame_Create(NAPPV_PANGEA_POPUP_WINBACKGROUND_TITLE_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_POPUP_WINBACKGROUND_TITLE_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvPopupRecharDrawWinTitleBackgroundImage_Pangea);

	OSD_GFX_SetRect(&rect, NAPPV_PANGEA_POPUP_WINBACKGROUND_X, NAPPV_PANGEA_POPUP_WINBACKGROUND_Y, NAPPV_PANGEA_POPUP_WINBACKGROUND_WIDTH, NAPPV_PANGEA_POPUP_WINBACKGROUND_HEIGHT);
	GWM_Frame_Create(NAPPV_PANGEA_POPUP_WINBACKGROUND_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_POPUP_WINBACKGROUND_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvPopupRecharDrawWinBackgroundImage_Pangea);

	/* na ppv pgea popup recharge event confirm msg*/
	OSD_GFX_SetRect(&rect, NAPPV_PANGEA_POPUP_EVTCONFIRM_MSG_X, NAPPV_PANGEA_POPUP_EVTCONFIRM_MSG_Y, NAPPV_PANGEA_POPUP_EVTCONFIRM_MSG_WIDTH, NAPPV_PANGEA_POPUP_EVTCONFIRM_MSG_HEIGHT);
	GWM_Frame_Create(NAPPV_PANGEA_POPUP_EVTCONFIRM_MSG_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_POPUP_EVTCONFIRM_MSG_ID, xmgr_cas_NaPpvPopupRecharDrawEvtRecharge_Pangea);

	/* na ppv pgea popup recharge exit button */
	OSD_GFX_SetRect(&rect, NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_BTN_X, NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_BTN_Y, NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_BTN_WIDTH, NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_BTN_HEIGHT);
	GWM_Frame_Create(NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_BTN_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_BTN_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvPopupRecharDrawExitButton_Pangea);

	/* na ppv pgea popup recharge exit button text*/
	OSD_GFX_SetRect (&rect, NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_TEXT_X, NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_TEXT_Y, NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_TEXT_WIDTH, NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_TEXT_HEIGHT);
	GWM_Text_Create (NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_TEXT_ID, &rect, (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_IPPV_CLOSE_ID));
	GWM_Text_SetAlign (NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_TEXT_ID, TEXT_ALIGN_LEFT);
	GWM_Text_SetSize (NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_TEXT_ID, FONT_SIZE_XS);
	GWM_Text_ChangeColor(NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_TEXT_ID, COL (C_T_Cursor), 0x00);

	HxLOG_Print(" --- \n");

	return ERR_BUS_NO_ERROR;
}

#define	_____POPUP_OPPV______________________________
/********************************************************************
 Function		: xmgr_cas_NaPpvPopupOppvGetLocalContents_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC XmgrCas_NaPopOppv_Pangea_t* xmgr_cas_NaPpvPopupOppvGetLocalContents_Pangea(void)
{
	return &s_stNaPgeaPopOppv;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupOppvInitContents_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupOppvInitContents_Pangea(XmgrCas_NaPopOppv_Pangea_t *pContents, HINT32 nCredit, HINT32 nNameLength, HUINT8 *pEvtName)
{
	HxSTD_memset(pContents, 0, sizeof(XmgrCas_NaPopOppv_Pangea_t));

	pContents->pStrTitle = (HUINT8*)GetStrRsc(LOC_CAS_NAGRA_PANGEA_BANNER_MSG_OPPV_NOTIFY_ID);
	pContents->pStrRemainCreditTitle = (HUINT8*)GetStrRsc(LOC_CAS_NAGRA_PANGEA_CREDIT_TEXT_ID);
	//xmgr_cas_NaGetScCreditStr(pContents->ucRemainCreditStr);
	snprintf(pContents->ucRemainCreditStr, 16, "%d,%02d", nCredit/100, nCredit%100); 	// 다시 credit을 가져오지 않고 받은 credit을 사용 <== card removed 대비

	HxSTD_memset(pContents->ucOppvEvtName, 0, MAX_PGEA_POPUP_EVTNAME_LENGTH+1);
	if (nNameLength >= MAX_PGEA_POPUP_EVTNAME_LENGTH)
	{
		MWC_UTIL_DvbStrncpy(pContents->ucOppvEvtName, pEvtName, MAX_PGEA_POPUP_EVTNAME_LENGTH);
	}
	else
	{
		MWC_UTIL_DvbStrncpy(pContents->ucOppvEvtName, pEvtName, nNameLength);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupOppvDrawWinTitleBackgroundImage_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupOppvDrawWinTitleBackgroundImage_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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

	if (!AP_StockImg_GetImage (e922_1_24_Banner1_Title, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupOppvDrawWinBackgroundImage_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupOppvDrawWinBackgroundImage_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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

	if (!AP_StockImg_GetImage (e922_1_24_Banner1, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupOppvDrawExitButton_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupOppvDrawExitButton_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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

	if (!AP_StockImg_GetImage (e921_1_24_BTN_5Exit, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupOppvGetCountOfCharsFromCutLines_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupOppvGetCountOfCharsFromCutLines_Pangea(HUINT8 *message, HINT32 nfontsize, HINT32 nwidth, HINT16 nCutline)
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
		OSD_FONT_Release(pFont);

	return (nTotal + nLocLen);
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupOppvDrawComfirmMsg_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupOppvDrawComfirmMsg_Pangea(XmgrCas_NaPopOppv_Pangea_t *pContents, OSD_Screen_t *pScreen, OSD_Area_t *pArea,
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

	// setting text style of the event comfirm msg
	snprintf(szDate, 128, "%s \"%s\"", (HINT8 *)pContents->pStrTitle, (HINT8 *)pContents->ucOppvEvtName);
	OSD_GFX_SetRect (&rect_status, NAPPV_PANGEA_POPUP_EVTCONFIRM_MSG_X, NAPPV_PANGEA_POPUP_EVTCONFIRM_MSG_Y,
						NAPPV_PANGEA_POPUP_EVTCONFIRM_MSG_WIDTH, NAPPV_PANGEA_POPUP_EVTCONFIRM_MSG_HEIGHT);
	OSD_GFX_DrawTextByLimit(pScreen, nObjectId, eFont_SystemNormal, eFontAlign_Right, FONT_SIZE_M, rect_status.x, rect_status.y, rect_status.w,
										(HINT8 *)szDate, COL(C_T_Cursor));

	// setting text style of the event comfirm credit
	pFont = OSD_FONT_Get(eFont_SystemNormal);
	OSD_GFX_SetRect (&rect_status, NAPPV_PANGEA_POPUP_EVTCONFIRM_CREDIT_TEXT_X, NAPPV_PANGEA_POPUP_EVTCONFIRM_CREDIT_TEXT_Y,
						NAPPV_PANGEA_POPUP_EVTCONFIRM_CREDIT_TEXT_WIDTH, NAPPV_PANGEA_POPUP_EVTCONFIRM_CREDIT_TEXT_HEIGHT);
	snprintf(szDate, 128, "%s %s", (HINT8 *)pContents->pStrRemainCreditTitle, pContents->ucRemainCreditStr); //all string
	nStrLen = OSD_GFX_GetStringWidth(pFont, szDate);
	nX1 = rect_status.x + (rect_status.w - nStrLen)/2;
	rect_status.x = nX1;
	if (pFont != NULL)
		OSD_FONT_Release(pFont);

	//credit text
	OSD_GFX_DrawTextByLimit(pScreen, nObjectId, eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_M, rect_status.x, rect_status.y, nStrLen,
										(HINT8*)pContents->pStrRemainCreditTitle, COL(C_T_PList_Orange));
	//credit value
	OSD_GFX_DrawTextByLimit(pScreen, nObjectId, eFont_SystemNormal, eFontAlign_Right, FONT_SIZE_M, rect_status.x, rect_status.y, nStrLen,
										pContents->ucRemainCreditStr, COL(C_T_Cursor));
	//credit icon
	if (!AP_StockImg_GetImage (e911_4_24_Euro_Font_White_B, &stImage))
	{
		OSD_GFX_SetRect (&rect_status, rect_status.x + nStrLen, rect_status.y - NAPPV_PANGEA_POPUP_STRING_MARGIN_Y, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &rect_status);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupOppvDrawEvtRecharge_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupOppvDrawEvtRecharge_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea,
										OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HERROR					hError;
	XmgrCas_NaPopOppv_Pangea_t	*pContents = xmgr_cas_NaPpvPopupOppvGetLocalContents_Pangea();

	// setting text style of the event confirm info
	hError = xmgr_cas_NaPpvPopupOppvDrawComfirmMsg_Pangea(pContents, pScreen, pArea, pRect, pObject, pArg, param1);
	if (hError != ERR_BUS_NO_ERROR)
	{
		HxLOG_Error(" xmgr_cas_NaPpvPopupOppvDrawComfirmMsg_Pangea Error!\n");
		return ERR_FAIL;
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupOppvSetUiObjects_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupOppvSetUiObjects_Pangea(XmgrCas_NaPopOppv_Pangea_t *pContents)
{
	OSD_Rect_t				rect = {0, };

	/* for compiler */
	(void)pContents;

	HxLOG_Print(" +++ \n");

	/* na ppv pangea popup oppv background */
	OSD_GFX_SetRect(&rect, NAPPV_PANGEA_POPUP_WINBACKGROUND_TITLE_X, NAPPV_PANGEA_POPUP_WINBACKGROUND_TITLE_Y, NAPPV_PANGEA_POPUP_WINBACKGROUND_TITLE_WIDTH, NAPPV_PANGEA_POPUP_WINBACKGROUND_TITLE_HEIGHT);
	GWM_Frame_Create(NAPPV_PANGEA_POPUP_WINBACKGROUND_TITLE_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_POPUP_WINBACKGROUND_TITLE_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvPopupOppvDrawWinTitleBackgroundImage_Pangea);

	OSD_GFX_SetRect(&rect, NAPPV_PANGEA_POPUP_WINBACKGROUND_X, NAPPV_PANGEA_POPUP_WINBACKGROUND_Y, NAPPV_PANGEA_POPUP_WINBACKGROUND_WIDTH, NAPPV_PANGEA_POPUP_WINBACKGROUND_HEIGHT);
	GWM_Frame_Create(NAPPV_PANGEA_POPUP_WINBACKGROUND_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_POPUP_WINBACKGROUND_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvPopupOppvDrawWinBackgroundImage_Pangea);

	/* na ppv pangea popup oppv event confirm msg*/
	OSD_GFX_SetRect(&rect, NAPPV_PANGEA_POPUP_EVTCONFIRM_MSG_X, NAPPV_PANGEA_POPUP_EVTCONFIRM_MSG_Y, NAPPV_PANGEA_POPUP_EVTCONFIRM_MSG_WIDTH, NAPPV_PANGEA_POPUP_EVTCONFIRM_MSG_HEIGHT);
	GWM_Frame_Create(NAPPV_PANGEA_POPUP_EVTCONFIRM_MSG_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_POPUP_EVTCONFIRM_MSG_ID, xmgr_cas_NaPpvPopupOppvDrawEvtRecharge_Pangea);

	/* na ppv pangea popup oppv exit button */
	OSD_GFX_SetRect(&rect, NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_BTN_X, NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_BTN_Y, NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_BTN_WIDTH, NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_BTN_HEIGHT);
	GWM_Frame_Create(NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_BTN_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_BTN_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvPopupOppvDrawExitButton_Pangea);

	/* na ppv pangea popup oppv exit button text*/
	OSD_GFX_SetRect (&rect, NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_TEXT_X, NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_TEXT_Y, NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_TEXT_WIDTH, NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_TEXT_HEIGHT);
	GWM_Text_Create (NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_TEXT_ID, &rect, (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_IPPV_CLOSE_ID));
	GWM_Text_SetAlign (NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_TEXT_ID, TEXT_ALIGN_LEFT);
	GWM_Text_SetSize (NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_TEXT_ID, FONT_SIZE_XS);
	GWM_Text_ChangeColor(NAPPV_PANGEA_POPUP_EVTCONFIRM_EXIT_TEXT_ID, COL (C_T_Cursor), 0x00);

	HxLOG_Print(" --- \n");

	return ERR_BUS_NO_ERROR;
}

#define	_____POPUP_IPPV_RESULT______________________________
/********************************************************************
 Function		: xmgr_cas_NaPpvPopupIppvResultGetLocalContents_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC XmgrCas_NaPopIppvResult_Pangea_t* xmgr_cas_NaPpvPopupIppvResultGetLocalContents_Pangea(void)
{
	return &s_stNaPgeaPopIppvResult;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupIppvResultInitContents_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupIppvResultInitContents_Pangea(XmgrCas_NaPopIppvResult_Pangea_t *pContents)
{
	HxSTD_memset(pContents, 0, sizeof(XmgrCas_NaPopIppvResult_Pangea_t));

	pContents->pStrTitle = (HUINT8*)GetStrRsc(LOC_CAS_NAGRA_PANGEA_IPPV_PURCHASE_DONE_ID);
	pContents->pStrRemainCreditTitle = (HINT8*)GetStrRsc(LOC_CAS_NAGRA_PANGEA_IPPV_RESIDUAL_CREDIT_ID);
	xmgr_cas_NaGetScCreditStr(pContents->ucRemainCreditStr);

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupIppvResultDrawWinTitleBackgroundImage_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupIppvResultDrawWinTitleBackgroundImage_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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

	if (!AP_StockImg_GetImage (e922_1_24_Banner1_Title, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupIppvResultDrawWinBackgroundImage_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupIppvResultDrawWinBackgroundImage_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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

	if (!AP_StockImg_GetImage (e922_1_24_Banner1, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupIppvResultGetCountOfCharsFromCutLines_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupIppvResultGetCountOfCharsFromCutLines_Pangea(HUINT8 *message, HINT32 nfontsize, HINT32 nwidth, HINT16 nCutline)
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
		OSD_FONT_Release(pFont);

	return (nTotal + nLocLen);
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupIppvResultDrawComfirmMsg_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupIppvResultDrawComfirmMsg_Pangea(XmgrCas_NaPopIppvResult_Pangea_t *pContents, OSD_Screen_t *pScreen, OSD_Area_t *pArea,
										OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1)
{
	OSD_Rect_t				rect_status;
	HINT32					nObjectId;
	HINT8 					szDate[128];

	//credit icon
	OSD_Font_t				*pFont;
	HINT32					nStrLen = 0, nX1;
	OSD_StockImage_t	 	stImage;

	/* for compiler */
	(void)pArea;
	(void)pRect;
	(void)pArg;
	(void)param1;

	nObjectId = GWM_Obj_GetObjectID(pObject);

	// setting text style of the event comfirm msg
	OSD_GFX_SetRect (&rect_status, NAPPV_PANGEA_POPUP_EVTCONFIRM_MSG_X, NAPPV_PANGEA_POPUP_EVTCONFIRM_MSG_Y,
						NAPPV_PANGEA_POPUP_EVTCONFIRM_MSG_WIDTH, NAPPV_PANGEA_POPUP_EVTCONFIRM_MSG_HEIGHT);
	OSD_GFX_DrawTextByLimit(pScreen, nObjectId, eFont_SystemNormal, eFontAlign_Right, FONT_SIZE_M, rect_status.x, rect_status.y, rect_status.w,
										(HINT8 *)pContents->pStrTitle, COL(C_T_Cursor));


	// setting text style of the event comfirm credit
	pFont = OSD_FONT_Get(eFont_SystemNormal);
	OSD_GFX_SetRect (&rect_status, NAPPV_PANGEA_POPUP_EVTCONFIRM_CREDIT_TEXT_X, NAPPV_PANGEA_POPUP_EVTCONFIRM_CREDIT_TEXT_Y,
						NAPPV_PANGEA_POPUP_EVTCONFIRM_CREDIT_TEXT_WIDTH, NAPPV_PANGEA_POPUP_EVTCONFIRM_CREDIT_TEXT_HEIGHT);
	snprintf(szDate, 128, "%s %s", pContents->pStrRemainCreditTitle, pContents->ucRemainCreditStr); //all string
	nStrLen = OSD_GFX_GetStringWidth(pFont, szDate);
	nX1 = rect_status.x + (rect_status.w - nStrLen)/2;
	rect_status.x = nX1;
	if (pFont != NULL)
		OSD_FONT_Release(pFont);

	//credit text
	OSD_GFX_DrawTextByLimit(pScreen, nObjectId, eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_M, rect_status.x, rect_status.y, nStrLen,
										(HINT8*)pContents->pStrRemainCreditTitle, COL(C_T_PList_Orange));
	//credit value
	OSD_GFX_DrawTextByLimit(pScreen, nObjectId, eFont_SystemNormal, eFontAlign_Right, FONT_SIZE_M, rect_status.x, rect_status.y, nStrLen,
										pContents->ucRemainCreditStr, COL(C_T_Cursor));
	//credit icon
	if (!AP_StockImg_GetImage (e911_4_24_Euro_Font_White_B, &stImage))
	{
		OSD_GFX_SetRect (&rect_status, rect_status.x + nStrLen, rect_status.y - NAPPV_PANGEA_POPUP_STRING_MARGIN_Y, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &rect_status);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupIppvResultDrawEvtIppvResult_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupIppvResultDrawEvtIppvResult_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea,
										OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HERROR					hError;
	XmgrCas_NaPopIppvResult_Pangea_t	*pContents = xmgr_cas_NaPpvPopupIppvResultGetLocalContents_Pangea();

	// setting text style of the event confirm info
	hError = xmgr_cas_NaPpvPopupIppvResultDrawComfirmMsg_Pangea(pContents, pScreen, pArea, pRect, pObject, pArg, param1);
	if (hError != ERR_BUS_NO_ERROR)
	{
		HxLOG_Error(" xmgr_cas_NaPpvPopupIppvResultDrawEvtIppvResult_Pangea Error!\n");
		return ERR_FAIL;
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPopupIppvResultSetUiObjects_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPopupIppvResultSetUiObjects_Pangea(XmgrCas_NaPopIppvResult_Pangea_t *pContents)
{
	OSD_Rect_t				rect = {0, };

	/* for compiler */
	(void)pContents;

	HxLOG_Print(" +++ \n");

	/* na ppv pgea popup recharge background */
	OSD_GFX_SetRect(&rect, NAPPV_PANGEA_POPUP_WINBACKGROUND_TITLE_X, NAPPV_PANGEA_POPUP_WINBACKGROUND_TITLE_Y, NAPPV_PANGEA_POPUP_WINBACKGROUND_TITLE_WIDTH, NAPPV_PANGEA_POPUP_WINBACKGROUND_TITLE_HEIGHT);
	GWM_Frame_Create(NAPPV_PANGEA_POPUP_WINBACKGROUND_TITLE_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_POPUP_WINBACKGROUND_TITLE_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvPopupIppvResultDrawWinTitleBackgroundImage_Pangea);

	OSD_GFX_SetRect(&rect, NAPPV_PANGEA_POPUP_WINBACKGROUND_X, NAPPV_PANGEA_POPUP_WINBACKGROUND_Y, NAPPV_PANGEA_POPUP_WINBACKGROUND_WIDTH, NAPPV_PANGEA_POPUP_WINBACKGROUND_HEIGHT);
	GWM_Frame_Create(NAPPV_PANGEA_POPUP_WINBACKGROUND_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_POPUP_WINBACKGROUND_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvPopupIppvResultDrawWinBackgroundImage_Pangea);

	/* na ppv pgea popup recharge event confirm msg*/
	OSD_GFX_SetRect(&rect, NAPPV_PANGEA_POPUP_EVTCONFIRM_MSG_X, NAPPV_PANGEA_POPUP_EVTCONFIRM_MSG_Y, NAPPV_PANGEA_POPUP_EVTCONFIRM_MSG_WIDTH, NAPPV_PANGEA_POPUP_EVTCONFIRM_MSG_HEIGHT);
	GWM_Frame_Create(NAPPV_PANGEA_POPUP_EVTCONFIRM_MSG_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_POPUP_EVTCONFIRM_MSG_ID, xmgr_cas_NaPpvPopupIppvResultDrawEvtIppvResult_Pangea);

	HxLOG_Print(" --- \n");

	return ERR_BUS_NO_ERROR;
}

#define ________XMGR_CAS_NA_PpvPOPUP_PUBLIC___________________________________________
/******************************************************************************/
/**
*   \fn     xproc_cas_NaPpvPopupRecharge_Pangea(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
*   \brief  system status procedure
*
*   @param[in]  gwm msg/action handler
*   @return     BUS_ProcessMessageDefault(message, hAction, p1, p2, p3)
*   @ingroup    SYSTEM STATUS
*/
/******************************************************************************/
BUS_Result_t xproc_cas_NaPpvPopupRecharge_Pangea(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrCas_NaPopRechar_Pangea_t			*pContents = xmgr_cas_NaPpvPopupRecharGetLocalContents_Pangea();

	switch(message)
	{
		case MSG_GWM_CREATE:
			/* set app area */
			GWM_APP_SetAppArea(NAPPV_PANGEA_POPUP_WINDOW_X, NAPPV_PANGEA_POPUP_WINDOW_Y,
								NAPPV_PANGEA_POPUP_WINDOW_WIDTH, NAPPV_PANGEA_POPUP_WINDOW_HEIGHT);

			/* initialize recharge content data */
			xmgr_cas_NaPpvPopupRecharInitContents_Pangea(pContents, p1);

			/* set ui objects */
			xmgr_cas_NaPpvPopupRecharSetUiObjects_Pangea(pContents);

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

	HxLOG_Info("xproc_cas_NaPpvPopupRecharge_Pangea handle(0x%x)\n", hAction);

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}


/******************************************************************************/
/**
*   \fn     xproc_cas_NaPpvPopupOppv_Pangea(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
*   \brief  system status procedure
*
*   @param[in]  gwm msg/action handler
*   @return     BUS_ProcessMessageDefault(message, hAction, p1, p2, p3)
*   @ingroup    SYSTEM STATUS
*/
/******************************************************************************/
BUS_Result_t xproc_cas_NaPpvPopupOppv_Pangea(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrCas_NaPopOppv_Pangea_t			*pContents = xmgr_cas_NaPpvPopupOppvGetLocalContents_Pangea();

	switch(message)
	{
		case MSG_GWM_CREATE:
			/* set app area */
			GWM_APP_SetAppArea(NAPPV_PANGEA_POPUP_WINDOW_X, NAPPV_PANGEA_POPUP_WINDOW_Y,
								NAPPV_PANGEA_POPUP_WINDOW_WIDTH, NAPPV_PANGEA_POPUP_WINDOW_HEIGHT);

			/* initialize oppv content data */
			xmgr_cas_NaPpvPopupOppvInitContents_Pangea(pContents, p1, p2, (HUINT8*)p3);

			/* set ui objects */
			xmgr_cas_NaPpvPopupOppvSetUiObjects_Pangea(pContents);

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

	HxLOG_Info("xproc_cas_NaPpvPopupOppv_Pangea handle(0x%x)\n", hAction);

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

/******************************************************************************/
/**
*   \fn     xmgr_cas_NaPpvPopupIppvResult_Pangea(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
*   \brief  system status procedure
*
*   @param[in]  gwm msg/action handler
*   @return     BUS_ProcessMessageDefault(message, hAction, p1, p2, p3)
*   @ingroup    SYSTEM STATUS
*/
/******************************************************************************/
BUS_Result_t xmgr_cas_NaPpvPopupIppvResult_Pangea(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrCas_NaPopIppvResult_Pangea_t	*pContents = xmgr_cas_NaPpvPopupIppvResultGetLocalContents_Pangea();

	switch(message)
	{
		case MSG_GWM_CREATE:
			/* set app area */
			GWM_APP_SetAppArea(NAPPV_PANGEA_POPUP_WINDOW_X, NAPPV_PANGEA_POPUP_WINDOW_Y,
								NAPPV_PANGEA_POPUP_WINDOW_WIDTH, NAPPV_PANGEA_POPUP_WINDOW_HEIGHT);

			/* initialize ippv result content data */
			xmgr_cas_NaPpvPopupIppvResultInitContents_Pangea(pContents);

			/* set ui objects */
			xmgr_cas_NaPpvPopupIppvResultSetUiObjects_Pangea(pContents);

			GWM_APP_PreventDraw_LowPriority(APP_CAS_HIGH_PRIORITY);

			BUS_SetTimer(CAS_NA_PPV_PANGEA_POPUP_IPPVRESULT_DISPLAY_CHECK_CREDIT_ID, CAS_NA_PPV_PANGEA_POPUP_IPPVRESULT_DISPLAY_CREDIT_PERIOD);
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
			if (p1 == CAS_NA_PPV_PANGEA_POPUP_IPPVRESULT_DISPLAY_CHECK_CREDIT_ID)
			{
				BUS_KillTimer(CAS_NA_PPV_PANGEA_POPUP_IPPVRESULT_DISPLAY_CHECK_CREDIT_ID);
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

	HxLOG_Info("xmgr_cas_NaPpvPopupIppvResult_Pangea handle(0x%x)\n", hAction);

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

#endif //#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
