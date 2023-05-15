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

#ifndef __NX_PRISM_INSTALL_SUB_COUNTRY_H__
#define __NX_PRISM_INSTALL_SUB_COUNTRY_H__

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

#include <nx_prism_install_main_ui.h>
/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
//#define	NX_PRISM_INSTALL_COUNTRY_ITEM_ID			    ((NX_PRISM_INSTALL_ITEM_ID)+1100)
//#define	NX_PRISM_INSTALL_COUNTRY_FRAME_ID			    ((NX_PRISM_INSTALL_COUNTRY_ITEM_ID)+1)
//#define	NX_PRISM_INSTALL_COUNTRY_TITLE_LINE_ID			((NX_PRISM_INSTALL_COUNTRY_ITEM_ID)+2)
//#define	NX_PRISM_INSTALL_COUNTRY_LIST_ID				((NX_PRISM_INSTALL_COUNTRY_ITEM_ID)+10)
//
//#define	NX_INSTALL_COUNTRY_LIST_X						229
//#define	NX_INSTALL_COUNTRY_LIST_Y						212
//#define	NX_INSTALL_COUNTRY_LIST_W						822
//#define	NX_INSTALL_COUNTRY_LIST_H						494
//#define NX_INSTALL_COUNTRY_LIST_BTN_H                   65
//#define	NX_INSTALL_COUNTRY_LIST_BTN_Y_PADDING			12
//#define	NX_INSTALL_COUNTRY_LIST_DIM_X					0
//#define	NX_INSTALL_COUNTRY_LIST_DIM_Y					630
//#define	NX_INSTALL_COUNTRY_LIST_DIM_W					1280
//#define	NX_INSTALL_COUNTRY_LIST_DIM_H					90
//
//#define	NX_INSTALL_COUNTRY_LIST_PAGE_NUM				8
//#define NX_INSTALL_COUNTRY_SUPPORT_NUM                  7 //Guatemala, Panama, Dominica Republic, El Salvador, Nicaragua, Costa Rica, Honduras
//#define	NX_INSTALL_COUNTRY_SUPPORT_NUM_MBC				14

/******************************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

//typedef struct tagInstall_Country_Item {
//	HUINT8		     ucCountryName[NX_PRISM_INSTALL_MAX_ELEMENT_TEXT_LENGTH]; // Language Name
//	HxCountry_e      lCountryCode;
//	HINT32			 nGmtOffset;
//} Nx_Install_Country_Item_t;

//typedef struct tagInstall_Country_Context {
//	HINT32		lCurFocus;
//	HUINT32		ulNumOfMainItem, ulMaxOfMainItem;
//    Nx_InstallSubPage_t	*pstSubItems;
//
//	HINT32		lDefaultFocusIdx;
//	HUINT8		ucTitle[NX_PRISM_INSTALL_MAX_ELEMENT_TEXT_LENGTH];
//    HUINT8      ulNumOfSubPage;// Nx_InstallSubPage_t sub page number
//
//    NxInstallPage_t ePage;
//
//    Nx_Install_Country_Item_t *pstCountryArray;//supported language list
//    HUINT8      ulNumOfCountry;
//} Nx_Install_Country_Context_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

extern ONDK_Result_t	NX_PRISM_Install_Country_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

#endif // __NX_PRISM_INSTALL_SUB_LANGUAGE_H__
