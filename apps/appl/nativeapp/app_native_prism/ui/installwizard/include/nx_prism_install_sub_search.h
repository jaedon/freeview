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

#ifndef __NX_PRISM_INSTALL_SUB_SEARCH_H__
#define __NX_PRISM_INSTALL_SUB_SEARCH_H__

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

#define	NX_PRISM_INSTALL_SEARCH_ITEM_ID			        ((NX_PRISM_INSTALL_ITEM_ID)+1100)
#define	NX_PRISM_INSTALL_SEARCH_FRAME_ID			    ((NX_PRISM_INSTALL_SEARCH_ITEM_ID)+1)
#define	NX_PRISM_INSTALL_SEARCH_TITLE_LINE_ID	        ((NX_PRISM_INSTALL_SEARCH_ITEM_ID)+2)
#define	NX_PRISM_INSTALL_SEARCH_AUTO_ARROW_IMG_ID	    ((NX_PRISM_INSTALL_SEARCH_ITEM_ID)+3)
#define	NX_PRISM_INSTALL_SEARCH_SAT_INFO_IMG_ID 	    ((NX_PRISM_INSTALL_SEARCH_ITEM_ID)+4)
#define	NX_PRISM_INSTALL_SEARCH_SAT_INFO_TEXT_ID 	    ((NX_PRISM_INSTALL_SEARCH_ITEM_ID)+5)

#define	NX_PRISM_INSTALL_SEARCH_TEXT_CONTEXT_ID 	    ((NX_PRISM_INSTALL_SEARCH_ITEM_ID)+6)//+1

#define	NX_PRISM_INSTALL_SEARCH_BTN_ID			        ((NX_PRISM_INSTALL_SEARCH_ITEM_ID)+10)
#define	NX_PRISM_INSTALL_SEARCH_BTN_PREVIOUS_ID		    ((NX_PRISM_INSTALL_SEARCH_BTN_ID)+1)
#define	NX_PRISM_INSTALL_SEARCH_BTN_NEXT_ID			    ((NX_PRISM_INSTALL_SEARCH_BTN_ID)+2)
#define	NX_PRISM_INSTALL_SEARCH_BTN_STOP_ID			    ((NX_PRISM_INSTALL_SEARCH_BTN_ID)+3)

#define	NX_PRISM_INSTALL_SEARCH_BTN_AUTO_OPT_ID		    ((NX_PRISM_INSTALL_SEARCH_BTN_ID)+10)
#define	NX_PRISM_INSTALL_SEARCH_BTN_OPT_ON_ID	        ((NX_PRISM_INSTALL_SEARCH_BTN_AUTO_OPT_ID)+1)
#define	NX_PRISM_INSTALL_SEARCH_BTN_OPT_SKIP_ID	        ((NX_PRISM_INSTALL_SEARCH_BTN_AUTO_OPT_ID)+2)

#define	NX_PRISM_INSTALL_SEARCH_TEXT_TV_TITLE_ID	    ((NX_PRISM_INSTALL_SEARCH_ITEM_ID)+20)
#define NX_PRISM_INSTALL_SEARCH_BOX_TV_INFO_ID          ((NX_PRISM_INSTALL_SEARCH_TEXT_TV_TITLE_ID)+1)
#define NX_PRISM_INSTALL_SEARCH_TEXT_TV_INFO_ID         ((NX_PRISM_INSTALL_SEARCH_BOX_TV_INFO_ID)+1)//+NX_PRISM_INSTALL_SEARCH_TV_INFO_LIMIT

#define	NX_PRISM_INSTALL_SEARCH_TEXT_RADIO_TITLE_ID	    ((NX_PRISM_INSTALL_SEARCH_ITEM_ID)+40)
#define NX_PRISM_INSTALL_SEARCH_BOX_RADIO_INFO_ID       ((NX_PRISM_INSTALL_SEARCH_TEXT_RADIO_TITLE_ID)+1)
#define NX_PRISM_INSTALL_SEARCH_TEXT_RADIO_INFO_ID      ((NX_PRISM_INSTALL_SEARCH_BOX_RADIO_INFO_ID)+1)//+NX_PRISM_INSTALL_SEARCH_TV_INFO_LIMIT

#define	NX_PRISM_INSTALL_SEARCH_TEXT_PROGRESS_ID	    ((NX_PRISM_INSTALL_SEARCH_ITEM_ID)+80)
#define	NX_PRISM_INSTALL_SEARCH_TEXT_PROGRESS_RATE_ID	((NX_PRISM_INSTALL_SEARCH_TEXT_PROGRESS_ID)+1)
#define	NX_PRISM_INSTALL_SEARCH_BOX_PROGRESS_BG_ID	    ((NX_PRISM_INSTALL_SEARCH_TEXT_PROGRESS_ID)+2)
#define	NX_PRISM_INSTALL_SEARCH_BOX_PROGRESS_LEVEL_ID	((NX_PRISM_INSTALL_SEARCH_TEXT_PROGRESS_ID)+3)
#define	NX_PRISM_INSTALL_SEARCH_TEXT_SEARCHED_ID        ((NX_PRISM_INSTALL_SEARCH_TEXT_PROGRESS_ID)+4)
#define	NX_PRISM_INSTALL_SEARCH_TEXT_SEARCHED_NUM_ID    ((NX_PRISM_INSTALL_SEARCH_TEXT_PROGRESS_ID)+5)

#define	NX_PRISM_INSTALL_SEARCH_POPUP_ID            	((NX_PRISM_INSTALL_SEARCH_ITEM_ID)+100)
#define	NX_PRISM_INSTALL_SEARCH_BOX_POPUP_ID        	((NX_PRISM_INSTALL_SEARCH_POPUP_ID)+0)
#define	NX_PRISM_INSTALL_SEARCH_BOX_POPUP_FRAME01_ID   	((NX_PRISM_INSTALL_SEARCH_POPUP_ID)+1)
#define	NX_PRISM_INSTALL_SEARCH_BOX_POPUP_FRAME02_ID    ((NX_PRISM_INSTALL_SEARCH_POPUP_ID)+2)
#define	NX_PRISM_INSTALL_SEARCH_TEXT_POPUP_CONTEXT_ID   ((NX_PRISM_INSTALL_SEARCH_POPUP_ID)+3)
#define	NX_PRISM_INSTALL_SEARCH_BTN_POPUP_YES_ID        ((NX_PRISM_INSTALL_SEARCH_POPUP_ID)+4)
#define	NX_PRISM_INSTALL_SEARCH_BTN_POPUP_NO_ID         ((NX_PRISM_INSTALL_SEARCH_POPUP_ID)+5)

#define	NX_PRISM_INSTALL_SEARCH_OBJ_POPUP_NUM           (6)


#define NX_PRISM_INSTALL_SEARCH_BTN_X                    380
#define NX_PRISM_INSTALL_SEARCH_BTN_Y                    615
#define NX_PRISM_INSTALL_SEARCH_BTN_W                    252
#define NX_PRISM_INSTALL_SEARCH_BTN_H                    55
#define NX_PRISM_INSTALL_SEARCH_BTN_GAP                  17

#define NX_PRISM_INSTALL_SEARCH_TEXT_CONTEXT_X           60
#define NX_PRISM_INSTALL_SEARCH_TEXT_CONTEXT_Y           141
#define NX_PRISM_INSTALL_SEARCH_TEXT_CONTEXT_W           1160
#define NX_PRISM_INSTALL_SEARCH_TEXT_CONTEXT_H           32


#define NX_PRISM_INSTALL_SEARCH_STOP_BTN_X               483
#define NX_PRISM_INSTALL_SEARCH_STOP_BTN_Y               615
#define NX_PRISM_INSTALL_SEARCH_STOP_BTN_W               315
#define NX_PRISM_INSTALL_SEARCH_STOP_BTN_H               55

#define NX_PRISM_INSTALL_SEARCH_AUTO_OPT_BTN_X           60
#define NX_PRISM_INSTALL_SEARCH_AUTO_OPT_BTN_Y           270
#define NX_PRISM_INSTALL_SEARCH_AUTO_OPT_BTN_W           750
#define NX_PRISM_INSTALL_SEARCH_AUTO_OPT_BTN_H           55

#define NX_PRISM_INSTALL_SEARCH_AUTO_OPT_VALUE_X         815
#define NX_PRISM_INSTALL_SEARCH_AUTO_OPT_VALUE_Y         270
#define NX_PRISM_INSTALL_SEARCH_AUTO_OPT_VALUE_W         405
#define NX_PRISM_INSTALL_SEARCH_AUTO_OPT_VALUE_H         55

#define	NX_PRISM_INSTALL_SEARCH_ARROW_IMG_X_TAB	    	 7
#define	NX_PRISM_INSTALL_SEARCH_ARROW_IMG_Y_TAB			 19
#define	NX_PRISM_INSTALL_SEARCH_ARROW_IMG_W				 13
#define	NX_PRISM_INSTALL_SEARCH_ARROW_IMG_H	    		 20

#define NX_PRISM_INSTALL_SEARCH_SAT_INFO_IMG_X           60
#define NX_PRISM_INSTALL_SEARCH_SAT_INFO_IMG_Y           390
#define NX_PRISM_INSTALL_SEARCH_SAT_INFO_IMG_W           750
#define NX_PRISM_INSTALL_SEARCH_SAT_INFO_IMG_H           160

#define NX_PRISM_INSTALL_SEARCH_SAT_INFO_TEXT_X          ((NX_PRISM_INSTALL_SEARCH_SAT_INFO_IMG_X)+15)
#define NX_PRISM_INSTALL_SEARCH_SAT_INFO_TEXT_Y          ((NX_PRISM_INSTALL_SEARCH_SAT_INFO_IMG_Y)+137)
#define NX_PRISM_INSTALL_SEARCH_SAT_INFO_TEXT_W          720
#define NX_PRISM_INSTALL_SEARCH_SAT_INFO_TEXT_H          20

#define NX_PRISM_INSTALL_SEARCH_TEXT_TV_TITLE_X          80
#define NX_PRISM_INSTALL_SEARCH_TEXT_TV_TITLE_Y          158
#define NX_PRISM_INSTALL_SEARCH_TEXT_TV_TITLE_W          530
#define NX_PRISM_INSTALL_SEARCH_TEXT_TV_TITLE_H          36

#define NX_PRISM_INSTALL_SEARCH_BOX_TV_INFO_X            60
#define NX_PRISM_INSTALL_SEARCH_BOX_TV_INFO_Y            192
#define NX_PRISM_INSTALL_SEARCH_BOX_TV_INFO_W            570
#define NX_PRISM_INSTALL_SEARCH_BOX_TV_INFO_H            248

#define NX_PRISM_INSTALL_SEARCH_TEXT_TV_INFO_X           20
#define NX_PRISM_INSTALL_SEARCH_TEXT_TV_INFO_Y           18
#define NX_PRISM_INSTALL_SEARCH_TEXT_TV_INFO_W           530
#define NX_PRISM_INSTALL_SEARCH_TEXT_TV_INFO_H           32

#define NX_PRISM_INSTALL_SEARCH_TV_INFO_LIMIT            7
#define NX_PRISM_INSTALL_SEARCH_RADIO_INFO_LIMIT         7

#define NX_PRISM_INSTALL_SEARCH_TEXT_RADIO_TITLE_X       670
#define NX_PRISM_INSTALL_SEARCH_TEXT_RADIO_TITLE_Y       (NX_PRISM_INSTALL_SEARCH_TEXT_TV_TITLE_Y)
#define NX_PRISM_INSTALL_SEARCH_TEXT_RADIO_TITLE_W       (NX_PRISM_INSTALL_SEARCH_TEXT_TV_TITLE_W)
#define NX_PRISM_INSTALL_SEARCH_TEXT_RADIO_TITLE_H       (NX_PRISM_INSTALL_SEARCH_TEXT_TV_TITLE_H)

#define NX_PRISM_INSTALL_SEARCH_BOX_RADIO_INFO_X         650
#define NX_PRISM_INSTALL_SEARCH_BOX_RADIO_INFO_Y         (NX_PRISM_INSTALL_SEARCH_BOX_TV_INFO_Y)
#define NX_PRISM_INSTALL_SEARCH_BOX_RADIO_INFO_W         (NX_PRISM_INSTALL_SEARCH_BOX_TV_INFO_W)
#define NX_PRISM_INSTALL_SEARCH_BOX_RADIO_INFO_H         (NX_PRISM_INSTALL_SEARCH_BOX_TV_INFO_H)

#define NX_PRISM_INSTALL_SEARCH_TEXT_RADIO_INFO_X        (NX_PRISM_INSTALL_SEARCH_TEXT_TV_INFO_X)
#define NX_PRISM_INSTALL_SEARCH_TEXT_RADIO_INFO_Y        (NX_PRISM_INSTALL_SEARCH_TEXT_TV_INFO_Y)
#define NX_PRISM_INSTALL_SEARCH_TEXT_RADIO_INFO_W        (NX_PRISM_INSTALL_SEARCH_TEXT_TV_INFO_W)
#define NX_PRISM_INSTALL_SEARCH_TEXT_RADIO_INFO_H        (NX_PRISM_INSTALL_SEARCH_TEXT_TV_INFO_H)

#define NX_PRISM_INSTALL_SEARCH_TEXT_PROGRESS_X          80
#define NX_PRISM_INSTALL_SEARCH_TEXT_PROGRESS_Y          470
#define NX_PRISM_INSTALL_SEARCH_TEXT_PROGRESS_W          1000
#define NX_PRISM_INSTALL_SEARCH_TEXT_PROGRESS_H          22

#define NX_PRISM_INSTALL_SEARCH_TEXT_PROGRESS_RATE_X     1100
#define NX_PRISM_INSTALL_SEARCH_TEXT_PROGRESS_RATE_Y     470
#define NX_PRISM_INSTALL_SEARCH_TEXT_PROGRESS_RATE_W     100
#define NX_PRISM_INSTALL_SEARCH_TEXT_PROGRESS_RATE_H     22

#define NX_PRISM_INSTALL_SEARCH_BOX_PROGRESS_BG_X        80
#define NX_PRISM_INSTALL_SEARCH_BOX_PROGRESS_BG_Y        505
#define NX_PRISM_INSTALL_SEARCH_BOX_PROGRESS_BG_W        1120
#define NX_PRISM_INSTALL_SEARCH_BOX_PROGRESS_BG_H        2

#define NX_PRISM_INSTALL_SEARCH_BOX_PROGRESS_LEVEL_W     (NX_PRISM_INSTALL_SEARCH_BOX_PROGRESS_BG_W)
#define NX_PRISM_INSTALL_SEARCH_BOX_PROGRESS_LEVEL_H     3

#define NX_PRISM_INSTALL_SEARCH_BOX_POPUP_X              290
#define NX_PRISM_INSTALL_SEARCH_BOX_POPUP_Y              559//282
#define NX_PRISM_INSTALL_SEARCH_BOX_POPUP_W              700
#define NX_PRISM_INSTALL_SEARCH_BOX_POPUP_H              156

#define NX_PRISM_INSTALL_SEARCH_BOX_POPUP_FRAME01_H      1
#define NX_PRISM_INSTALL_SEARCH_BOX_POPUP_FRAME02_H      1

#define NX_PRISM_INSTALL_SEARCH_TEXT_POPUP_CONTEXT_X     ((NX_PRISM_INSTALL_SEARCH_BOX_POPUP_X)+30)
#define NX_PRISM_INSTALL_SEARCH_TEXT_POPUP_CONTEXT_Y     ((NX_PRISM_INSTALL_SEARCH_BOX_POPUP_Y)+23)
#define NX_PRISM_INSTALL_SEARCH_TEXT_POPUP_CONTEXT_W     640
#define NX_PRISM_INSTALL_SEARCH_TEXT_POPUP_CONTEXT_H     32

#define NX_PRISM_INSTALL_SEARCH_BTN_POPUP_YES_X          ((NX_PRISM_INSTALL_SEARCH_BOX_POPUP_X)+123)
#define NX_PRISM_INSTALL_SEARCH_BTN_POPUP_YES_Y          ((NX_PRISM_INSTALL_SEARCH_BOX_POPUP_Y)+76)
#define NX_PRISM_INSTALL_SEARCH_BTN_POPUP_YES_W          220
#define NX_PRISM_INSTALL_SEARCH_BTN_POPUP_YES_H          50

#define NX_PRISM_INSTALL_SEARCH_BTN_POPUP_NO_X           ((NX_PRISM_INSTALL_SEARCH_BOX_POPUP_X)+358)
#define NX_PRISM_INSTALL_SEARCH_BTN_POPUP_NO_Y           (NX_PRISM_INSTALL_SEARCH_BTN_POPUP_YES_Y)
#define NX_PRISM_INSTALL_SEARCH_BTN_POPUP_NO_W           (NX_PRISM_INSTALL_SEARCH_BTN_POPUP_YES_W)
#define NX_PRISM_INSTALL_SEARCH_BTN_POPUP_NO_H           (NX_PRISM_INSTALL_SEARCH_BTN_POPUP_YES_H)

#define NX_PRISM_INSTALL_SEARCH_TEXT_SEARCHED_X             140
#define NX_PRISM_INSTALL_SEARCH_TEXT_SEARCHED_Y             553
#define NX_PRISM_INSTALL_SEARCH_TEXT_SEARCHED_W             1000
#define NX_PRISM_INSTALL_SEARCH_TEXT_SEARCHED_H             32

#define	NX_PRISM_INSTALL_SEARCH_ANIMATION_TIMER_PERIOD	 (700)	/* 700msec */

#define	NX_PRISM_INSTALL_SEARCH_SVCTYPE_TV					0 // TV, RADIO
#define	NX_PRISM_INSTALL_SEARCH_SVCTYPE_RADIO					1 // TV, RADIO
#define	NX_PRISM_INSTALL_SEARCH_SVCTYPE_NUM					2 // TV, RADIO

/******************************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef enum {
	eInstall_Search_init = 0,
	eInstall_Search_searching, //searching channel
	eInstall_Search_pause,
	eInstall_Search_searchFinish,
	eInstall_Search_saveResult,
	eInstall_Search_cancel,
	eInstall_Search_searchingSattllite,
	eInstall_Search_checkingDiseq,
	eInstall_Search_checkingSattlliteType,
	eInstall_Search_errorOccurred,
	eInstall_Search_processEnd,
	eInstall_Search_MAX
} NxInstall_SearchState_t;

typedef struct tagInstall_Ch_Item {
	HUINT8		ucChName[NX_PRISM_INSTALL_MAX_ELEMENT_TEXT_LENGTH];
	HINT32      ulChCode;
} Nx_InstallChItem_t;

typedef struct tagInstall_Search_Context {
	HINT32		lCurFocus;
	HUINT32		ulNumOfMainItem, ulMaxOfMainItem;
    Nx_InstallSubPage_t	*pstSubItems;

	HINT32		lDefaultFocusIdx;
	HUINT8		ucTitle[NX_PRISM_INSTALL_MAX_ELEMENT_TEXT_LENGTH];
    HUINT8      ucNumOfSubPage;// Nx_InstallSubPage_t sub page number

    NxInstallPage_t ePage;
    Nx_InstallChItem_t *pstSearchedCh;//Searched Ch
    HUINT8      ulNumOfCh;

    HBOOL       bAutoSearch;
    HBOOL       bAutoExtendOpt;
    HINT32      ulSearchLevel;//Channel Search Progress Level
    NxInstall_SearchState_t eSearchState;

    HINT32      lLoadingIndex;

    HUINT32 	aulNumSvc[NX_PRISM_INSTALL_SEARCH_SVCTYPE_NUM];
	HUINT32		aulTotalSvc[NX_PRISM_INSTALL_SEARCH_SVCTYPE_NUM];

	HUINT32		aulSvcNameListSize[NX_PRISM_INSTALL_SEARCH_SVCTYPE_NUM];
	HUINT8		**pszSvcNameList[NX_PRISM_INSTALL_SEARCH_SVCTYPE_NUM];
    HxVector_t	*pstSearchedTPList;
	HUINT32 	ulTotalTuningNum;
	HUINT32		ulCompleteTpNum;
	HUINT32		ulSearchedServiceNum;
	HUINT32		ulTvServiceNum;
	HUINT32		ulRadioServiceNum;
    HUINT8		szTpInfoString[NX_PRISM_INSTALL_MAX_ELEMENT_TEXT_LENGTH];
    HUINT32		ulTunerCount;

    HINT32      lAntenaCount;
    DxAntInfo_t *stAntInfoArray;

    HCHAR       *pStrResult;

} Nx_Install_Search_Context_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

extern ONDK_Result_t	NX_PRISM_Install_Search_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_Install_Search_Progress_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_Install_Search_Auto_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

#endif // __NX_PRISM_INSTALL_SUB_LANGUAGE_H__
