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
 * @file	  		nx_prism_component_popuplist.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_PRISM_COMPONENT_POPUPLIST_H__
#define __NX_PRISM_COMPONENT_POPUPLIST_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/
#include <nx_common.h>
#include <nx_prism_app.h>


/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/


/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/
typedef enum {

	eListDlg_OSD_FULL 		= 0x00000001, 			// FULL OSD Rect
	eListDlg_OSD_USER 		= 0x00000002, 			// USER OSD Rect
	eListDlg_OSD_BG 		= 0x00000004,			// Draw BG Color
	eListDlg_ICON_L 		= 0x00000008,			// Draw ICON Left of text
	eListDlg_ICON_R 		= 0x00000010,			// Draw ICON right of text
	eListDlg_Title			= 0x00000020,			// Draw Title Text
	eListDlg_Scroll			= 0x00000040,			// Draw Scroll

	eListDlg_BG_MenuSubItemApp	= 0x00010000,		// Menu Sub Item list + Background
	eListDlg_SubItemApp 		= 0x00020000,		// Sub Item list, ex)Live Tv Option
	eListDlg_MultiSelectVApp 	= 0x00040000,		// Multi Selective Vertical
	eListDlg_MultiSelectHApp 	= 0x00080000,		// Multi Selective Horizontal
	eListDlg_SelectItemApp 		= 0x00100000,		// Selective Item, ex) Sorting, Group select Item

	eListDlg_IMAGE_OptionLApp	= 0x01000000,		// Option Sub Item List + Icon Image
	eListDlg_IMAGE_OptionRApp	= 0x02000000,		// Option Sub Item List + Icon Image

} eListDlg_AttributeType_t;


/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/

#define	NX_PRISM_COMP_POPUPLISTAPP_ITEM_MAX					9


typedef struct tag_Nx_PrismListDialog_context
{
	/* for custom behaviors each application */
	ONDK_GWM_Callback_t		parentApp;
	ONDK_Rect_t				appArea;
	HUINT32					eAttribute;

	HUINT8					*pDisbleList;

	HUINT32					ulCurIdxOfList;
	HUINT32					ulNumOfList;
	HUINT32					ulNumOfPerPage;
	HUINT32					ulList;
	HINT32					lLineSpace;
	HINT32					lLineWSpace;
	HINT32 					lFontSize;
	ONDK_Rect_t				extArea;
	HUINT32					ulImageList;
	HUINT32					ulImageExtList;

	// Used for Multi Selective Dialogue Window
	HUINT8					*pszTitle;
} Nx_PrismListDialog_Content_t;


/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/

/*******************************************************************/
/********************	global function prototype   ****************/
/*******************************************************************/
extern inline Nx_PrismListDialog_Content_t	*NX_PRISM_COMP_ListDlg_GetContents(void);

extern ONDK_Result_t	NX_PRISM_COMP_ListDlg_SetTitle(Nx_PrismListDialog_Content_t *pstContents, HUINT8 *pszTitle);
extern ONDK_Result_t	NX_PRISM_COMP_ListDlg_SetParentApp(Nx_PrismListDialog_Content_t *pstContents, ONDK_GWM_Callback_t parentApp);
extern ONDK_Result_t	NX_PRISM_COMP_ListDlg_SetAttribute(Nx_PrismListDialog_Content_t *pstContents, HUINT32 eAttribute);
extern ONDK_Result_t	NX_PRISM_COMP_ListDlg_SetContentList(Nx_PrismListDialog_Content_t *pstContents, HUINT32 ppList);
extern ONDK_Result_t	NX_PRISM_COMP_ListDlg_SetAppArea(Nx_PrismListDialog_Content_t *pstContents, ONDK_Rect_t *pRect);
extern ONDK_Result_t	NX_PRISM_COMP_ListDlg_SetNumOfList(Nx_PrismListDialog_Content_t *pstContents, HUINT32 ulNumOfList);
extern ONDK_Result_t	NX_PRISM_COMP_ListDlg_SetNumOfPerPage(Nx_PrismListDialog_Content_t *pstContents, HUINT32 ulNumOfPerPage);
extern ONDK_Result_t	NX_PRISM_COMP_ListDlg_SetZeroListDlg(Nx_PrismListDialog_Content_t *pstContents);
extern ONDK_Result_t	NX_PRISM_COMP_ListDlg_SetDisable(Nx_PrismListDialog_Content_t *pstContents, HUINT8 *pDisableList);
extern ONDK_Result_t	NX_PRISM_COMP_ListDlg_SetLineSpace(Nx_PrismListDialog_Content_t *pstContents, HINT32 lLineSpace);
extern ONDK_Result_t	NX_PRISM_COMP_ListDlg_SetLineWithSpace(Nx_PrismListDialog_Content_t *pstContents, HINT32 lLineWSpace);
extern ONDK_Result_t	NX_PRISM_COMP_ListDlg_SetFontSize(Nx_PrismListDialog_Content_t *pstContents, HINT32 lFontSize);
extern ONDK_Result_t	NX_PRISM_COMP_ListDlg_SetImageList(Nx_PrismListDialog_Content_t *pstContents, HUINT32 ppImageList);
extern ONDK_Result_t	NX_PRISM_COMP_ListDlg_SetImageExtList(Nx_PrismListDialog_Content_t *pstContents, HUINT32 ppImageExtList);
extern ONDK_Result_t	NX_PRISM_COMP_ListDlg_SetExternalArea(Nx_PrismListDialog_Content_t *pstContents, ONDK_Rect_t *pRect);
extern ONDK_Result_t	NX_PRISM_COMP_ListDlg_SetListPopup(Nx_PrismListDialog_Content_t *pstContents, ONDK_GWM_Callback_t parentApp, ONDK_Rect_t *pRect,
											HINT32 lLineSpace, HUINT32 eAttribute, HUINT32 ulNumOfList, HUINT32 ulNumOfPerPage, HUINT32 ulCurIdx, HUINT32 ppList);
extern ONDK_Result_t	NX_PRISM_COMP_ListDlg_Proc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_COMP_ListDlg_ProtoProc(Nx_PrismListDialog_Content_t *pstListDlg_Content, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

#endif // __NX_PRISM_COMPONENT_POPUPLIST_H__

