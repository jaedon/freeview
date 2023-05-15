
/**
	@file     ap_cas_va_statuslist_coordinate.h
	@brief    coordinate repository of conax Menu - Status List

	Description: Portal dimension header file for kraken \n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification							Name\n
	-----------	-------------------------------------	------------------\n
	 2009/07/24		init					 				jhlee\n

	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef __AP_CAS_VA_STATUSLIST_COORIDNATE_H__
#define __AP_CAS_VA_STATUSLIST_COORIDNATE_H__

#define	CAS_VA_STATUSLIST_APP_WINDOW_WIDTH			OSD_SCREEN_WIDTH
#define	CAS_VA_STATUSLIST_APP_WINDOW_HEIGHT			OSD_SCREEN_HEIGHT
#define	CAS_VA_STATUSLIST_APP_WINDOW_X				(OSD_SCREEN_WIDTH/2-CAS_VA_STATUSLIST_APP_WINDOW_WIDTH/2)
#define	CAS_VA_STATUSLIST_APP_WINDOW_Y				(OSD_SCREEN_HEIGHT/2-CAS_VA_STATUSLIST_APP_WINDOW_HEIGHT/2)

/* Cursor Info */
#define	CAS_VA_STATUSLIST_CURSOR_WIDTH				947
#define	CAS_VA_STATUSLIST_CURSOR_HEIGHT				58
#define	CAS_VA_STATUSLIST_CURSOR_X					17
#define	CAS_VA_STATUSLIST_CURSOR_Y					15

/* Item Text  */
#define	CAS_VA_STATUSLIST_TEXT_HEIGHT				40
#define	CAS_VA_STATUSLIST_TEXT_FIRST_Y				23
#define	CAS_VA_STATUSLIST_TEXT_CONTENT_X			32

#define	CAS_VA_STATUSLIST_TEXT_CONTENT_WIDTH		932
#define	CAS_VA_STATUSLIST_LIST_TITLE_WIDTH			800
#define	CAS_VA_STATUSLIST_LIST_WIDTH_SUB			430
#define	CAS_VA_STATUSLIST_ITEM_VALUE_X				50
#define	CAS_VA_STATUSLIST_ITEM_EVEN_LOCATION		0
#define	CAS_VA_STATUSLIST_ITEM_ODD_LOCATION			1
#define	CAS_VA_STATUSLIST_TITLE_BUTTON_TEXT_X		20
#define	CAS_VA_STATUSLIST_TITLE_BUTTON_MARGIN_Y		20

#define	CAS_VA_STATUSLIST_TEXT_MARGIN				10

#define	CAS_VA_STATUSLIST_RIGHTS_TEXT_WIDTH01		190
#define	CAS_VA_STATUSLIST_RIGHTS_TEXT_WIDTH02		500
#define	CAS_VA_STATUSLIST_RIGHTS_TEXT_WIDTH03		700

/* for not screwing up the ID, public DEFINE object use after 0x0a00 ~IDs */

#define	CAS_VA_STATUSLIST_ITEM_LINESPACE			48

#define	CAS_VA_STATUSLIST_RIGHTS_X_MARGIN			50

/* List Title Item 01 */
#define	CAS_VA_STATUSLIST_TEXTLIST_TITLE01_ID		(eMenuFrame_Window_ConditionalAccess + 0x0001)
#define	CAS_VA_STATUSLIST_TITLE_WIDTH				920
#define	CAS_VA_STATUSLIST_TITLE_HEIGHT				40
#define	CAS_VA_STATUSLIST_TITLE_X					180
#define	CAS_VA_STATUSLIST_TITLE_Y					138

/* List Title Item 02 */
#define	CAS_VA_STATUSLIST_TEXTLIST_TITLE02_ID		(eMenuFrame_Window_ConditionalAccess + 0x0002)

/* List Title Item 03 */
#define	CAS_VA_STATUSLIST_TEXTLIST_TITLE03_ID		(eMenuFrame_Window_ConditionalAccess + 0x0003)

/* List Box */
#define	CAS_VA_STATUSLIST_LIST_BOX_ID				(eMenuFrame_Window_ConditionalAccess + 0x0004)
#define	CAS_VA_STATUSLIST_LIST_BOX_WIDTH			987
#define	CAS_VA_STATUSLIST_LIST_BOX_HEIGHT			(468 + CAS_VA_STATUSLIST_ITEM_LINESPACE)
#define	CAS_VA_STATUSLIST_LIST_BOX_TITLE_BUTTON_WIDTH	320
#define	CAS_VA_STATUSLIST_LIST_BOX_X				143
#define	CAS_VA_STATUSLIST_LIST_BOX_Y				(168 - CAS_VA_STATUSLIST_ITEM_LINESPACE)

/* List Box Scroll */
#define	CAS_VA_STATUSLIST_LIST_SCROLL_HEIGHT		(435 + CAS_VA_STATUSLIST_ITEM_LINESPACE)
#define	CAS_VA_STATUSLIST_LIST_SCROLL_X				962
#define	CAS_VA_STATUSLIST_LIST_SCROLL_Y				16
#define	CAS_VA_STATUSLIST_LIST_SCROLLBALL_H			20

/* List Box Item */
#define	CAS_VA_STATUSLIST_LIST_ID					(eMenuFrame_Window_ConditionalAccess + 0x0005)
#define	CAS_VA_STATUSLIST_LIST_ITEM_HEIGHT			48
#define	CAS_VA_STATUSLIST_LIST_WIDTH				987
#define	CAS_VA_STATUSLIST_LIST_HEIGHT				(468 + CAS_VA_STATUSLIST_ITEM_LINESPACE)
#define	CAS_VA_STATUSLIST_LIST_X					143
#define	CAS_VA_STATUSLIST_LIST_Y					168

#endif /* __AP_CAS_VA_STATUSLIST_COORIDNATE_H__ */

/* end of file */
