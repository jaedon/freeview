#ifndef __TTX_MENU_H_
#define __TTX_MENU_H_
/*******************************************************************/
/********************      Header Files    		*************************/
/*******************************************************************/
#include <teletext.h>
#include <vkernel.h>
#include <hapi.h>
#include <ttx_errchk.h>
/*******************************************************************/
/********************      Defines         			*************************/
/*******************************************************************/

#define	 	TTXMENU_SURFACE_X				400
#define	 	TTXMENU_SURFACE_Y				590
#define	 	TTXMENU_SURFACE_WIDHT			490
#define	 	TTXMENU_SURFACE_HEIGHT			50

#define	 	TTXMENU_WINDOW_BGX				0
#define	 	TTXMENU_WINDOW_BGY				0
#define	 	TTXMENU_WINDOW_BGWIDHT			TTXMENU_SURFACE_WIDHT
#define	 	TTXMENU_WINDOW_BGHEIGHT			TTXMENU_SURFACE_HEIGHT


#define		TTXMENU_ITEM_TOP				5
#define 	TTXMENU_ITEM_SPACE				10
#define 	TTXMENU_ITEM_WIDHT				60
#define 	TTXMENU_ITEM_HEIGHT				40

#define 	TTXMENU_BG_PATH 					"/usr/image/125_4_00_BTN_K4_N.png"


/////////////////////////////////////////////////////////////////////////
#define 	TTXMENU_INDEX_X						(TTXMENU_WINDOW_BGX + 5)
#define 	TTXMENU_INDEX_FOCUSED_OFF_PATH		"/usr/image/811_1_01_Index_C_Off.png"
#define 	TTXMENU_INDEX_FOCUSED_ON_PATH		"/usr/image/811_1_01_Index_C_On.png"
#define 	TTXMENU_INDEX_FOCUSEABLE_OFF_PATH	"/usr/image/811_1_01_Index_N_Off.png"
/////////////////////////////////////////////////////////////////////////
#define 	TTXMENU_SUB_X						(TTXMENU_INDEX_X + TTXMENU_ITEM_SPACE + TTXMENU_ITEM_WIDHT)
#define 	TTXMENU_SUB_FOCUSED_OFF_PATH		"/usr/image/812_1_01_SubPage_C_Off.png"
#define 	TTXMENU_SUB_FOCUSED_ON_PATH			"/usr/image/812_1_01_SubPage_C_On.png"
#define 	TTXMENU_SUB_FOCUSEABLE_OFF_PATH		"/usr/image/812_1_01_SubPage_N_Off.png"
/////////////////////////////////////////////////////////////////////////
#define 	TTXMENU_MIX_X						(TTXMENU_SUB_X + TTXMENU_ITEM_SPACE + TTXMENU_ITEM_WIDHT)
#define 	TTXMENU_MIX_FOCUSED_ON100_PATH		"/usr/image/813_1_01_MIX_C_On.png"
#define 	TTXMENU_MIX_FOCUSED_ON50_PATH		"/usr/image/813_1_01_MIX_C_50.png"
#define 	TTXMENU_MIX_FOCUSED_OFF_PATH		"/usr/image/813_1_01_MIX_C_Off.png"
#define 	TTXMENU_MIX_FOCUSEABLE_OFF100_PATH	"/usr/image/813_1_01_MIX_N_On.png"
#define 	TTXMENU_MIX_FOCUSEABLE_OFF50_PATH	"/usr/image/813_1_01_MIX_N_50.png"
#define 	TTXMENU_MIX_FOCUSEABLE_OFF_PATH		"/usr/image/813_1_01_MIX_N_Off.png"
/////////////////////////////////////////////////////////////////////////
#define 	TTXMENU_CANCEL_X					(TTXMENU_MIX_X + TTXMENU_ITEM_SPACE + TTXMENU_ITEM_WIDHT)
#define 	TTXMENU_CANCEL_FOCUSED_OFF_PATH		"/usr/image/814_1_01_Cancel_C_Off.png"
#define 	TTXMENU_CANCEL_FOCUSED_ON_PATH		"/usr/image/814_1_01_Cancel_C_On.png"
#define 	TTXMENU_CANCEL_FOCUSEABLE_ON_PATH	"/usr/image/814_1_01_Cancel_N_On.png"
#define 	TTXMENU_CANCEL_FOCUSEABLE_OFF_PATH	"/usr/image/814_1_01_Cancel_N_Off.png"
/////////////////////////////////////////////////////////////////////////
#define 	TTXMENU_REVEAL_X					(TTXMENU_CANCEL_X + TTXMENU_ITEM_SPACE + TTXMENU_ITEM_WIDHT)
#define 	TTXMENU_REVEAL_FOCUSED_OFF_PATH		"/usr/image/815_1_01_Reveal_C_Off.png"
#define 	TTXMENU_REVEAL_FOCUSED_ON_PATH		"/usr/image/815_1_01_Reveal_C_On.png"
#define 	TTXMENU_REVEAL_FOCUSEABLE_ON_PATH	"/usr/image/815_1_01_Reveal_N_On.png"
#define 	TTXMENU_REVEAL_FOCUSEABLE_OFF_PATH	"/usr/image/815_1_01_Reveal_N_Off.png"
/////////////////////////////////////////////////////////////////////////
#define 	TTXMENU_DOUBLE_X					(TTXMENU_REVEAL_X + TTXMENU_ITEM_SPACE + TTXMENU_ITEM_WIDHT)
#define 	TTXMENU_DOUBLE_FOCUSED_OFF_PATH		"/usr/image/816_1_01_DoubleSize_C_Off.png"
#define 	TTXMENU_DOUBLE_FOCUSED_ON_PATH		"/usr/image/816_1_01_DoubleSize_C_On.png"
#define 	TTXMENU_DOUBLE_FOCUSEABLE_ON_PATH	"/usr/image/816_1_01_DoubleSize_N_On.png"
#define 	TTXMENU_DOUBLE_FOCUSEABLE_OFF_PATH	"/usr/image/816_1_01_DoubleSize_N_Off.png"
/////////////////////////////////////////////////////////////////////////
#define 	TTXMENU_HOLD_X						(TTXMENU_DOUBLE_X + TTXMENU_ITEM_SPACE + TTXMENU_ITEM_WIDHT)
#define 	TTXMENU_HOLD_FOCUSED_OFF_PATH		"/usr/image/817_1_01_PageHold_C_Off.png"
#define 	TTXMENU_HOLD_FOCUSED_ON_PATH		"/usr/image/817_1_01_PageHold_C_On.png"
#define 	TTXMENU_HOLD_FOCUSEABLE_ON_PATH		"/usr/image/817_1_01_PageHold_N_On.png"
#define 	TTXMENU_HOLD_FOCUSEABLE_OFF_PATH	"/usr/image/817_1_01_PageHold_N_Off.png"
/////////////////////////////////////////////////////////////////////////
extern void Teletext_MenuCreate(void);
extern HBOOL Teletext_MenuKeyEvent(DFBInputDeviceKeySymbol symbol);


#endif
