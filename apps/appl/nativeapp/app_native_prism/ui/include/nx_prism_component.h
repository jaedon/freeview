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
 * @file	  		nx_prism_component.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_PRISM_COMPONENT_H__
#define __NX_PRISM_COMPONENT_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/
//
#include <nx_prism_live.h>
#include <nx_prism_search.h>
#include <nx_system_startlogo.h>
#include <nx_prism_definition.h>
#include <nx_prism_component_popuplist.h>
#include <nx_prism_comp_dialog_input.h>
#include <nx_prism_component_pindialog.h>
#include <nx_prism_component_timedialog.h>
#include <nx_prism_component_ipdialog.h>

// menu/settings/
//#include <nx_ui_search.h>

/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/


/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/
typedef enum {
	NxPrism_FindFocus_Up = -1,
	NxPrism_FindFocus_Current = 0,
	NxPrism_FindFocus_Down = 1,
} NxPrism_FindFocus_t;


/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/
#define	NX_PRISM_COMP_INPUT_DIGHT_5								5


/*******************************************************************/
/********************		Struct			************************/
/*******************************************************************/
typedef struct tagNxPrsimTextDrawInfo
{
	HUINT32		ulDrawMode;		/* TEXTMODE_NORMAL, TEXTMODE_FORCE_FIT ... */
	HUINT32		ulXAlign;		/* TEXT_ALIGN_LEFT, TEXT_ALIGN_CENTER ... */
	HUINT32		ulYAlign;		/* TEXT_ALIGN_TOP, TEXT_ALIGN_BOTTOM ... */
	HUINT32		ulTextColor;
	HUINT32		ulFontSize;
}NxPrsim_TEXT_DRAW_INFO_t;


/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/


/*******************************************************************/
/********************	global function prototype   ****************/
/*******************************************************************/
extern void		NX_PRISM_COMP_Settings_InitNaviMainItems1Depth(Nx_SettingsNaviMainItem_t *pMainItemList, HUINT32 ulMaxMainItem, GWM_ObjDraw_t objDrawMethod);
extern HERROR	NX_PRISM_COMP_Settings_InitNaviMainSubItemList1Depth(Nx_SettingsNaviMainItem_t *pMainItemList, HINT32 lMainItemIndex, HBOOL bFirst);
extern HERROR	NX_PRISM_COMP_Settings_NaviReflectFocus1Depth(Nx_SettingsNaviContext_t *pstContents, HINT32 lSubItemObjId, HBOOL bFindNext, HINT32 lDirection);

extern void		NX_PRISM_COMP_Settings_InitNaviMainItems2Depth(Nx_SettingsNaviMainItem_t *pMainItemList, HUINT32 ulMaxMainItem, GWM_ObjDraw_t objDrawMethod);
extern HERROR	NX_PRISM_COMP_Settings_InitNaviMainSubItemList2Depth(Nx_SettingsNaviMainItem_t *pMainItemList, HINT32 lMainItemIndex, HBOOL bFirst);
extern HERROR	NX_PRISM_COMP_Settings_NaviReflectFocus2Depth(Nx_SettingsNaviContext_t *pstContents, HINT32 lSubItemObjId, HBOOL bFindNext, HINT32 lDirection);

extern HINT32	NX_PRISM_COMP_Settings_NaviGetNextFocusIdx(Nx_SettingsNaviContext_t *pstContents, HINT32 lObjIdx, HINT32 lDirection);

extern void		NX_PRISM_COMP_Settings_InitLeafSubItemsInMenuContents (void *pvContents, GWM_ObjDraw_t pfDrawMethod);

extern void		NX_PRISM_COMP_Settings_DrawBackground(void);
extern void		NX_PRISM_COMP_Settings_DrawTitle(HUINT8 *pTitle);
extern void		NX_PRISM_COMP_Settings_DrawCurrentDateTime(HUINT8 *pszDate, HUINT8 *pszTime);
extern void		NX_PRISM_COMP_Settings_DrawUpdateCurrentDateTime(HUINT8 *pszDate, HUINT8 *pszTime);
extern void		NX_PRISM_COMP_Settings_DrawTitleLine(void);
extern void		NX_PRISM_COMP_Settings_DrawCursor(ONDK_Rect_t *pRect);
extern void		NX_PRISM_COMP_Settings_DrawCursorArrowImage(ONDK_Rect_t *pRect);
extern void		NX_PRISM_COMP_Settings_DrawUpdateCursor(Nx_SettingsNaviContext_t *pstContents, HINT32 lObjId);

extern ONDK_Result_t	NX_PRISM_COMP_Settings_DrawNaviMainItem_Cursor_A1(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
extern ONDK_Result_t	NX_PRISM_COMP_Settings_DrawNaviMainItem_Cursor_A1_Value(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
extern ONDK_Result_t	NX_PRISM_COMP_Settings_DrawNaviMainItem_Cursor_A1_Arrow(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
extern ONDK_Result_t	NX_PRISM_COMP_Settings_DrawNaviSubItemList_1Depth(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
extern ONDK_Result_t	NX_PRISM_COMP_Settings_DrawNaviSubItemList_2Depth(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
extern ONDK_Result_t	NX_PRISM_COMP_Settings_DrawLeafSubItem_Cursor_C1(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
extern ONDK_Result_t	NX_PRISM_COMP_Settings_DrawLeafSubItem_Cursor_C1_Arrow(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
extern ONDK_Result_t	NX_PRISM_COMP_Settings_DrawLeafSubItem_Cursor_B1_Arrow(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
extern ONDK_Result_t	NX_PRISM_COMP_Settings_CreateDrawStrengthBar(ONDK_Rect_t *rectTitle, ONDK_Rect_t *rectBar, ONDK_Rect_t *rectValue, HCHAR *text);
extern ONDK_Result_t	NX_PRISM_COMP_Settings_UpdateDrawStrenghtBar(HUINT32 ulStrength);
extern ONDK_Result_t	NX_PRISM_COMP_Settings_CreateDrawQualitytBar(ONDK_Rect_t *rectTitle, ONDK_Rect_t *rectBar, ONDK_Rect_t *rectValue, HCHAR *text);
extern ONDK_Result_t	NX_PRISM_COMP_Settings_UpdateDrawQualityBar(HUINT32 ulQuality);
extern ONDK_Result_t	NX_PRISM_COMP_Settings_CreateDrawBERBar(ONDK_Rect_t *rectTitle, ONDK_Rect_t *rectBar, ONDK_Rect_t *rectValue, HCHAR *text);
extern ONDK_Result_t	NX_PRISM_COMP_Settings_UpdateDrawBERBar(HUINT32 ulBER, HINT32 lDecimal, HINT32 lFloat, HINT32 lExp);

extern void				NX_PRISM_COMP_Settings_DrawHelpText(HUINT8 *pszMessage);
extern void				NX_PRISM_COMP_Settings_UpdateHelpText(HUINT8 *pszMessage);
extern void				NX_PRISM_COMP_Settings_UpdateHelpLocation(HUINT8 *pszMessage);

extern void				NX_PRISM_COMP_Settings_DrawTitlePopUp(HUINT8 *pTitle);
extern void				NX_PRISM_COMP_Settings_DrawBackgroundPopUp(void);
extern ONDK_Result_t	NX_PRISM_COMP_Settings_DrawPopUpSubItem_Cursor_B1(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
extern ONDK_Result_t	NX_PRISM_COMP_Settings_DrawPopUpSubItem_Cursor_B1_Arrow(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);

// Media Draw Function
extern void				NX_PRISM_COMP_Media_DrawTitle(HUINT8 *pTitle);
extern void				NX_PRISM_COMP_Media_DrawTitleLine(void);

// Draw Common Option Menu
extern void				NX_PRISM_COMP_OPT_InitMenuContents (void *pvContents, GWM_ObjDraw_t pfDrawMethod);
extern ONDK_Result_t	NX_PRISM_COMP_OPT_DrawCursor_A1_Arrow(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
extern ONDK_Result_t	NX_PRISM_COMP_OPT_DrawButton_A1(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);

// Draw Common Window
extern ONDK_Result_t	NX_PRISM_COMP_DrawWindowPopUp(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
extern ONDK_Result_t	NX_PRISM_COMP_DrawWindowOptionLeft(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
extern ONDK_Result_t	NX_PRISM_COMP_DrawWindowOptionRight(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);

extern ONDK_Result_t	NX_PRISM_COMP_DrawImageOptionLeftPointer(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
extern ONDK_Result_t	NX_PRISM_COMP_DrawImageOptionRightPointer(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);

extern ONDK_Result_t	NX_PRISM_COMP_DrawDialugeWindow(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
extern void				NX_PRISM_COMP_DrawDialugeWindowMessage(ONDK_Rect_t *rect, HUINT8 *pMMessage, ONDK_Color_t color);
extern ONDK_Result_t	NX_PRISM_COMP_DrawDialugeInputButton(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
extern ONDK_Result_t	NX_PRISM_COMP_DrawDCNInputButton(ONDKSurfaceHandle screen, ONDK_Region_t *area,
										ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
// Draw Common Button
extern ONDK_Result_t	NX_PRISM_COMP_DrawButton_A1(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
extern ONDK_Result_t	NX_PRISM_COMP_DrawButton_B1(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
extern ONDK_Result_t	NX_PRISM_COMP_DrawButton_C1(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
extern ONDK_Result_t	NX_PRISM_COMP_DrawButton_Scroll_ArrowLBox(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
extern ONDK_Result_t	NX_PRISM_COMP_DrawButton_Scroll_ArrowRBox(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
extern ONDK_Result_t	NX_PRISM_COMP_DrawButton_PinCodeButton(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
extern ONDK_Result_t	NX_PRISM_COMP_DrawButton_TimeDataButton(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
extern ONDK_Result_t	NX_PRISM_COMP_DrawButton_IpDataButton(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);

extern ONDK_Result_t	NX_PRISM_COMP_DrawOption_HelpR(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
extern ONDK_Result_t	NX_PRISM_COMP_DrawEdit_HelpR(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);

// Draw Common BAR
extern ONDK_Result_t	NX_PRISM_COMP_DrawStatusBarA1(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
extern ONDK_Result_t	NX_PRISM_COMP_DrawStatusBarB1_B2_B3(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
extern ONDK_Result_t	NX_PRISM_COMP_DrawStatusBarB4(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
extern ONDK_Result_t	NX_PRISM_COMP_DrawStatusBarB5(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
extern ONDK_Result_t	NX_PRISM_COMP_DrawWEmpty(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);

// Draw Common Frame
extern ONDK_Result_t	NX_PRISM_COMP_DrawFrameBG_SS(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
extern ONDK_Result_t	NX_PRISM_COMP_DrawFrameBox_A(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
extern ONDK_Result_t	NX_PRISM_COMP_DrawFrameCursor_A1(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
extern ONDK_Result_t	NX_PRISM_COMP_DrawFrameCursor_C2(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
extern ONDK_Result_t	NX_PRISM_COMP_DrawFrameCursor_D1(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
extern ONDK_Result_t	NX_PRISM_COMP_DrawFrameScroll_BG(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);

extern ONDK_Result_t	NX_PRISM_COMP_DrawFrameDialogueWindowBG(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
extern ONDK_Result_t	NX_PRISM_COMP_DrawFrameDialogueWindowBG_BottomLine(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
extern ONDK_Result_t	NX_PRISM_COMP_DrawFrameDialogueWindowBG_TopLine(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
extern ONDK_Result_t	NX_PRISM_COMP_DrawFrameDialogueWindowBG_TBLine(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
extern ONDK_Result_t	NX_PRISM_COMP_DrawFrameBottomDim(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);

// Draw Common List Cursor
extern ONDK_Result_t	NX_PRISM_COMP_DrawList_Cursor_A1(ONDKSurfaceHandle *screen, ONDK_Region_t *area,
								ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);

// Draw Animation
extern ONDK_Result_t	NX_PRISM_COMP_ActionGUIAnimation(HINT32 lObjId, ONDK_Rect_t pRect, HINT32 lAlign,
									HINT32 lSpeed, HINT32 lSAlpha, HINT32 lEAlpha, HINT32 lLastKey);


#endif // __NX_PRISM_COMPONENT_H__

