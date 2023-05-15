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
 * @file	  		nx_prism_installwizard.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_PRISM_INSTALL_MAIN_UI_H__
#define __NX_PRISM_INSTALL_MAIN_UI_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <ondk.h>
#include <ondk_type.h>
#include <ondk_gfx.h>
#include <hlib.h>

#include <nx_common.h>

#include <nx_prism_app.h>
#include <nx_prism_ui.h>
#include <nx_prism_component.h>

#include <nx_prism_install_model.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define	NX_PRISM_INSTALL_MAX_ELEMENT_TEXT_LENGTH				128
#define NX_PRISM_INSTALL_MAX_ITEM_TEXT_LENGTH					32

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NX_PRISM_INSTALL_ITEM_ID					         eMenuFrame_Window_Install

#define	NX_INSTALL_OBJECT_ID						        (NX_PRISM_INSTALL_ITEM_ID)
#define	NX_PRISM_INSTALL_ANIMATION_TIMER_ID                 ((NX_INSTALL_OBJECT_ID)+200)
#define	NX_PRISM_INSTALL_RESPONSE_TIMER_ID                  ((NX_INSTALL_OBJECT_ID)+201)
//POPUP
#define	NX_PRISM_INSTALL_POPUP_ID                           ((NX_INSTALL_OBJECT_ID)+300)
#define	NX_PRISM_INSTALL_POPUP_SELECTIVE_ID                 ((NX_PRISM_INSTALL_POPUP_ID) + 50)
#define	NX_PRISM_INSTALL_POPUP_RESPONSE_ID                  ((NX_PRISM_INSTALL_POPUP_ID) + 100)

//POPUP BUTTON
#define	NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_POSITIVE_ID        ((NX_PRISM_INSTALL_POPUP_SELECTIVE_ID) + 10)
#define	NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_NEGATIVE_ID        ((NX_PRISM_INSTALL_POPUP_SELECTIVE_ID) + 11)
#define	NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_CONFIRM_ID         ((NX_PRISM_INSTALL_POPUP_SELECTIVE_ID) + 12)

//TIMER ID
#define	NX_PRISM_INSTALL_ANIMATION_TIMER_PERIOD	 (700)	/* 700msec */
#define	NX_PRISM_INSTALL_RESPONSE_TIMER_3SEC_PERIOD	 (3000)	/* 700msec */

/******************************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct tagInstallSubPage {
	HCHAR		*hcTitleLocId;
	HUINT8		ucTitle[NX_PRISM_INSTALL_MAX_ELEMENT_TEXT_LENGTH];
    HUINT8		ucSubTitle[NX_PRISM_INSTALL_MAX_ELEMENT_TEXT_LENGTH];
    NxInstallPage_t ePageName;
    ONDK_GWM_Callback_t procCallback;
} Nx_InstallSubPage_t;

typedef struct tagInstallContext {
	HINT32		lCurFocus;
	HUINT32		ulNumOfMainItem, ulMaxOfMainItem;
    Nx_InstallSubPage_t	*pstSubItems;

	HINT32		lDefaultFocusIdx;
	HUINT8		ucTitle[NX_PRISM_INSTALL_MAX_ELEMENT_TEXT_LENGTH];
    HUINT8      ulNumOfSubPage;// Nx_InstallSubPage_t sub page number
    NxInstallPage_t ePage; //self
} Nx_InstallContext_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
#define _________NX_PRISM_INSTALL_MAIN_UI_Prototype_________________________
/*******************************************************************/

ONDK_Result_t   nx_install_ui_DrawBackgroundUI(void);
void            nx_install_ui_DrawTitle(HUINT8 *pTitle);
void            nx_install_ui_DrawExplain(HCHAR *pText);

ONDK_Result_t   nx_install_ui_DoNextStep(NxInstallPage_t currentPage,NxInstallPage_t nextPage, HINT32 p1, HINT32 p2, HINT32 p3);
void            nx_install_ui_SetDimBackgroundUI(HBOOL bDim);
ONDK_Result_t   nx_install_ui_SetProcessingPopupUI(HBOOL bShow, HCHAR* strContext);
ONDK_Result_t   nx_install_ui_GetListOfSubPage(Nx_InstallSubPage_t	**pstRetSubItemArray, HUINT8 *ulNumOfPage);
HBOOL           nx_install_ui_isAvailableIndicatorStep(NxInstallPage_t eStep);
ONDK_Result_t   nx_install_ui_DrawStepIndicatior(HUINT8 ulCurrentStep);
ONDK_Result_t   nx_install_ui_GetSubPage(Nx_InstallContext_t *pstContents);
ONDK_Result_t	nx_install_ui_UpdateProcessingPopupUI(HINT32 index);

ONDK_Result_t	nx_install_ui_OpenConfirmPopupUI(HCHAR** strContextList, HINT32 lineNum);
ONDK_Result_t	nx_install_ui_CloseConfirmPopupUI(void);
ONDK_Result_t	nx_install_ui_OpenSelectivePopupUI(HCHAR** strContextList, HINT32 lineNum,HCHAR* strPositive,HCHAR* strNegative);
ONDK_Result_t	nx_install_ui_CloseSelectivePopupUI(void);
ONDK_Result_t	nx_install_ui_OpenResponsePopupUI(HCHAR* strContext);
ONDK_Result_t	nx_install_ui_CloseResponsePopupUI(void);

ONDK_Result_t	nx_install_ui_ResponseDialogProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3);
ONDK_Result_t	nx_install_ui_DestroyResponseDialouge(void);
void	    	nx_install_ui_CreateResponseDialouge(HUINT8* pszString, HINT32 nTimeout);
ONDK_Result_t	nx_install_ui_MsgGwmKeyDown(HINT32 p1, HINT32 p2, HINT32 p3);

#endif // __NX_PRISM_INSTALL_MAIN_UI_H__
