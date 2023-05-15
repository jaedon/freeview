/**
	@file     ap_cas_cx_pincode_ui.c
	@brief    file_name & simple comment.

	Description: for CONAX Check Pin code UI \n
	Module: Application(Layer), UI(module), CAS(module), CONAX(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2009/07/30		modified					jhlee\n

	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <osd_gfx.h>
//#include <menuui_coor.h>
//#include <popupui_coor.h>

//#include <ray_popupui_coordinate.h>
#include <bus.h>
#include <xmgr_cas.h>
#include "va_ui_adapt.h"
#include "../local_include/_xmgr_cas_va_util.h"
#include "../local_include/_xmgr_cas_va_ui.h"

#include "_xmgr_cas_va_preview_ui_coordinate.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
#if defined(CONFIG_DEBUG)
HUINT32	g_CasVa_Preview_debug_level = DBG_ASSERT;
#endif


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#if defined(CONFIG_DEBUG)
#define	ApCasVaPreviewPrint(level, message)			(((level) & g_CasVa_Preview_debug_level) ? ((VK_Print("==[AP:CAS_VA_PREVIEW_UI]==")), (VK_Print message)) : 0)
#define	ApCasVaPreviewAssert(c) 						((c) ? 0 : (VK_Print("\n==[CAS_VA_PREVIEW_UI]== assertion on %s: line%d\n\n", __FILE__, __LINE__)))
#define	ApCasVaPreviewData(level, message)			(((level) & g_CasVa_Preview_debug_level) ? ( (VK_Print message)) : 0)
#else
#define	ApCasVaPreviewPrint(level, message)
#define	ApCasVaPreviewAssert(c)
#define	ApCasVaPreviewData(level, message)
#endif

#define PREVIEW_WINDOW_ID				0x100
#define PREVIEW_TEXT_ID					(PREVIEW_WINDOW_ID + 0x03)
#define	PREVIEW_ELAPSED_TIME_TEXT_ID	(PREVIEW_WINDOW_ID + 0x01)
#define PREVIEW_PROGRESS_BAR_ID			(PREVIEW_WINDOW_ID + 0x02)

#define PREVIEW_PROGRESS_IMAGE			e165_2_00_Bar_B4_LV
#define PREVIEW_PROGRESS_IMAGE_BG		e165_2_00_Bar_B4_BG


//#define DBG_PREVIEW

/*******************************************************************/
/********************      typedef         *************************/
/*******************************************************************/

typedef struct
{
	HUINT8 ucUsedCwNum;
	HUINT8 ucMaxCwNum;
}VA_CAS_UI_PREVIEW_Context;


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

STATIC VA_CAS_UI_PREVIEW_Context	s_stPreviewContext;

#if 0
STATIC AP_PinDialogInstance_t		s_stUnlockConsultationInstance;
STATIC AP_DialogInstance_t			s_stNtvplusBootUi_DialogInst;
#endif

/*******************************************************************/
/********************      	Static Prototype	********************/
/*******************************************************************/


/*******************************************************************/
/********************      	Local Function		********************/
/*******************************************************************/

#define __________AP_CAS_VA_PREVIEW_LOCAL_FUNCTION__________

STATIC VA_CAS_UI_PREVIEW_Context *local_ap_cas_va_GetContextPreview()
{
	return &s_stPreviewContext;
}
#if 0 // not use for octo2.0
STATIC BUS_Result_t		local_ap_cas_va_preview_DrawBackground(OSD_Screen_t *screen, OSD_Area_t *area, OSD_Rect_t *rect, GWM_GuiObject_t object, void *arg, HINT32 param1)
{
	HINT32			x1, y1, x2, y2;
	OSD_Color_t		ulColor, ulAlpha;
	OSD_Area_t		stArea, stOsdRect;

	/* warning remove */
	NOT_USED_PARAM(object);
	NOT_USED_PARAM(arg);
	NOT_USED_PARAM(param1);

	/* absolute coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w - 1;
	y2 = area->y1 + rect->y + rect->h - 1;

	ulAlpha = 0xff;
	AP_UTILOSD_GetBGAlpha(0, (HINT32 *)&ulAlpha);
	ulColor = (ulAlpha << 24) | (COL(C_W_BG01) & 0x00ffffff);

	OSD_GFX_DrawRectBlendFill(screen, x1, y1, x2, y2, ulColor);

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t		local_ap_cas_va_preview_DrawElapsedTimeProgress(OSD_Screen_t *pOsdScreen, OSD_Area_t *appArea, OSD_Rect_t *objRect, GWM_GuiObject_t object, void *arg, int bUiTop)
{
	STATUS_OBJECT	statusObj = (STATUS_OBJECT)arg;
	int				x, y, w, h;
	HUINT32			max, value, current;
	OSD_StockImage_t	stImg;
	OSD_Rect_t			stRect;

	x = appArea->x1 + objRect->x;
	y = appArea->y1 + objRect->y;
	w = objRect->w;
	h = objRect->h;

	GWM_Status_GetProperty(statusObj, &max, &value);
	current = (value * w) / max;

	if (!AP_StockImg_GetImage(PREVIEW_PROGRESS_IMAGE, &stImg))
	{
		OSD_GFX_SetRect (&stRect, x, y, current, stImg.height);
		OSD_GFX_FB_BlendStretch ((unsigned long)stImg.hBuffer, stImg.width, stImg.height, pOsdScreen, &stRect);
	}

	if (!AP_StockImg_GetImage(PREVIEW_PROGRESS_IMAGE_BG, &stImg))
	{
		OSD_GFX_SetRect (&stRect, x + current, y, w - current, stImg.height);
		OSD_GFX_FB_BlendStretch ((unsigned long)stImg.hBuffer, stImg.width, stImg.height, pOsdScreen, &stRect);
	}

	return ERR_OK;
}
#endif
STATIC HERROR local_ap_cas_va_preview_MsgGwmCreate()
{
	HERROR hError = ERR_OK;
#if 0 // not use for octo2.0
	OSD_Rect_t	stRect = {0, };

	// set app area
	GWM_APP_SetAppArea(CAS_VA_PREVIEW_WINDOW_X, CAS_VA_PREVIEW_WINDOW_Y, OSD_SCREEN_WIDTH, OSD_SCREEN_HEIGHT);

	// background
	OSD_GFX_SetRect (&stRect, CAS_VA_PREVIEW_BACKGROUND_X, CAS_VA_PREVIEW_BACKGROUND_Y, CAS_VA_PREVIEW_BACKGROUND_WIDTH, CAS_VA_PREVIEW_BACKGROUND_HEIGHT);
	GWM_Frame_Create(PREVIEW_WINDOW_ID, &stRect, NULL);
	GWM_APP_SetObjectDrawMethod(PREVIEW_WINDOW_ID, local_ap_cas_va_preview_DrawBackground);

	// available preview channel text
	OSD_GFX_SetRect (&stRect, CAS_VA_PREVIEW_TEXT_X, CAS_VA_PREVIEW_TEXT_Y, CAS_VA_PREVIEW_TEXT_WIDTH, CAS_VA_PREVIEW_TEXT_HEIGHT);
	GWM_Text_Create (PREVIEW_TEXT_ID, &stRect, GetStrRsc(LOC_CAS_VA_MSG_PREVIEW_ID));
	GWM_Text_SetAlign (PREVIEW_TEXT_ID, TEXT_ALIGN_CENTER);
	GWM_Text_SetSize (PREVIEW_TEXT_ID, FONT_SIZE_S);
	GWM_Text_ChangeColor (PREVIEW_TEXT_ID, COL (C_T_Enable), NORMAL_TEXTBACK_COLOR);

	// preview elapsed time text
	OSD_GFX_SetRect (&stRect, CAS_VA_PREVIEW_ELAPSED_TIME_TEXT_X, CAS_VA_PREVIEW_ELAPSED_TIME_TEXT_Y, CAS_VA_PREVIEW_ELAPSED_TIME_TEXT_WIDTH, CAS_VA_PREVIEW_ELAPSED_TIME_TEXT_HEIGHT);
	GWM_Text_Create (PREVIEW_ELAPSED_TIME_TEXT_ID, &stRect, "Elapsed time");
	GWM_Text_SetAlign (PREVIEW_ELAPSED_TIME_TEXT_ID, TEXT_ALIGN_RIGHT);
	GWM_Text_SetSize (PREVIEW_ELAPSED_TIME_TEXT_ID, FONT_SIZE_S);
	GWM_Text_ChangeColor (PREVIEW_ELAPSED_TIME_TEXT_ID, COL (C_T_Info_Wzd), NORMAL_TEXTBACK_COLOR);

	// preview progress
	OSD_GFX_SetRect (&stRect, CAS_VA_PREVIEW_PROGRESS_X, CAS_VA_PREVIEW_PROGRESS_Y, CAS_VA_PREVIEW_PROGRESS_WIDTH, CAS_VA_PREVIEW_PROGRESS_HEIGHT);
	GWM_Status_Create (PREVIEW_PROGRESS_BAR_ID, &stRect, 100, 0);
	GWM_APP_SetObjectDrawMethod (PREVIEW_PROGRESS_BAR_ID, local_ap_cas_va_preview_DrawElapsedTimeProgress);
#endif
	return hError;
}

STATIC BUS_Result_t local_ap_cas_va_preview_UpdateProgress(HUINT8 ucUsedCwNum, HUINT8 ucMaxCwNum)
{
	HUINT32 ulPercent;

	ulPercent = (ucMaxCwNum == 0 ? 0 : ((ucUsedCwNum * 100) / ucMaxCwNum));

	if (ulPercent > 100)
	{
		ulPercent = 100;
	}
#if 0 // not use for octo2.0
	// Redraw the bar
	GWM_Status_SetValue (PREVIEW_PROGRESS_BAR_ID, ulPercent);
#endif
	return ERR_OK;
}

STATIC BUS_Result_t local_ap_cas_va_preview_proc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	VA_CAS_UI_PREVIEW_Context *pContext = local_ap_cas_va_GetContextPreview();

	switch(message)
	{
	case eMEVT_BUS_CREATE:
		local_ap_cas_va_preview_MsgGwmCreate();
#if defined(WIN32) || defined(DBG_PREVIEW)
		pContext->ucUsedCwNum = 2;
		pContext->ucMaxCwNum = 5;
#endif
		local_ap_cas_va_preview_UpdateProgress(pContext->ucUsedCwNum, pContext->ucMaxCwNum);
		break;
/* hjlee3, just for build
	case eMEVT_XMGR_CAS_APP_UI_TEST:	//테스트 임시용...
		local_ap_cas_va_preview_UpdateProgress(pContext->ucUsedCwNum, pContext->ucMaxCwNum);
		break;
*/
	default:
		break;
	}
}

STATIC BUS_Result_t	local_cas_va_UnlockConsultationProc(HINT32 message, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if 0 // hjlee3
	s_stUnlockConsultationInstance.parentAppCallback = local_ap_cas_va_preview_proc;
	s_stUnlockConsultationInstance.ulDlgAttributes = (eCasVa_PinCheckEnterCA_InDlg)eVA_PinCheckEnter_UnlockConsultation;

	return AP_CAS_VA_CheckPinDialog_Proc(&s_stUnlockConsultationInstance, message, handle, p1, p2, p3);
#endif

}

STATIC HERROR local_ap_cas_va_PreviewUiProcess(HUINT32 ulAcsId, HUINT32 ulStbStreamHandle)
{
	HINT32 nResult;
	HERROR hError;
	VA_CAS_UI_PREVIEW_Context *pContext = local_ap_cas_va_GetContextPreview();

#if !(defined(WIN32) || defined(DBG_PREVIEW))
	hError = AP_CAS_VA_GetPreViewInfo(ulAcsId, ulStbStreamHandle, &pContext->ucUsedCwNum, &pContext->ucMaxCwNum, &nResult);

	ApCasVaPreviewPrint(DBG_DATA, ("[Get Preview Information] Handle(%x), CurrentCw(%d), MaxCW(%d), Error(%d,%d)\n",
		ulStbStreamHandle, pContext->ucUsedCwNum, pContext->ucMaxCwNum, hError, nResult));

	if(hError != ERR_OK)
	{
		if(nResult == VA_UI_ERR_CONSULTATION_LOCKED)
		{
			BUS_MGR_Create("local_cas_va_UnlockConsultationProc", APP_CAS_PRIORITY, local_cas_va_UnlockConsultationProc,
				0, (HINT32)eVA_PinCheckStep_InDlg_InputPincode, 0, 0);
		}
		else
		{
			return hError;
		}
	}
	else
#endif
	{
		if(BUS_MGR_Get(local_ap_cas_va_preview_proc) == NULL)
		{
			BUS_MGR_Create("local_ap_cas_va_preview_proc", APP_CAS_PRIORITY, local_ap_cas_va_preview_proc, 0, 0, 0, 0);
		}
		else
		{
/* hjlee3, just for build
			BUS_SendMessage(local_ap_cas_va_preview_proc, eMEVT_XMGR_CAS_APP_UI_TEST, 0, 0, 0, 0);
*/
		}
	}
}


/*******************************************************************/
/********************      	Global Function		********************/
/*******************************************************************/

#define __________AP_CAS_VA_PREVIEW_GLOBAL_FUNCTION__________

HERROR AP_CAS_VA_Preview(HUINT32 ulAcsId, HUINT32 ulStbStreamHandle)
{
	return local_ap_cas_va_PreviewUiProcess(ulAcsId, ulStbStreamHandle);
}

HERROR AP_CAS_VA_DestroyPreview()
{
	BUS_MGR_Destroy(local_ap_cas_va_preview_proc);
}

