
/**
	@file     ap_cas_cx_statuslist_coordinate.h
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

#ifndef __XMGR_CAS_CX_STATUSLIST_COORIDNATE_H__
#define __XMGR_CAS_CX_STATUSLIST_COORIDNATE_H__

#define	CAS_CX_STATUSLIST_APP_WINDOW_WIDTH			OSD_SCREEN_WIDTH
#define	CAS_CX_STATUSLIST_APP_WINDOW_HEIGHT			OSD_SCREEN_HEIGHT
#define	CAS_CX_STATUSLIST_APP_WINDOW_X				(OSD_SCREEN_WIDTH/2-CAS_CX_STATUSLIST_APP_WINDOW_WIDTH/2)
#define	CAS_CX_STATUSLIST_APP_WINDOW_Y				(OSD_SCREEN_HEIGHT/2-CAS_CX_STATUSLIST_APP_WINDOW_HEIGHT/2)

/* Cursor Info */
#define	CAS_CX_STATUSLIST_CURSOR_WIDTH				947
#define	CAS_CX_STATUSLIST_CURSOR_HEIGHT				58
#define	CAS_CX_STATUSLIST_CURSOR_X					17
#define	CAS_CX_STATUSLIST_CURSOR_Y					15

/* Item Text  */
#define	CAS_CX_STATUSLIST_TEXT_HEIGHT				40
#define	CAS_CX_STATUSLIST_TEXT_FIRST_Y				23
#define	CAS_CX_STATUSLIST_TEXT_CONTENT_X			32
#define	CAS_CX_STATUSLIST_TEXT_CONTENT_WIDTH		932
#define	CAS_CX_STATUSLIST_TEXT_WITH_MAIN			272
#define	CAS_CX_STATUSLIST_TEXT_WITH_MAIN02			80
#define	CAS_CX_STATUSLIST_TEXT_WITH_SUB				430
#define	CAS_CX_STATUSLIST_TEXT_WITH_SUB02			370
#define	CAS_CX_STATUSLIST_TEXT_WITH_ENT				200
#define	CAS_CX_STATUSLIST_TEXT_MARGIN				10

#define	CAS_CX_STATUSLIST_EVENT_TEXT_WITH01			320
#define	CAS_CX_STATUSLIST_EVENT_TEXT_WITH02			340
#define	CAS_CX_STATUSLIST_EVENT_TEXT_WITH03			240

#define	CAS_CX_STATUSLIST_MSG_TEXT_WITH01			80
#define	CAS_CX_STATUSLIST_MSG_TEXT_WITH02			800

#define	CAS_CX_STATUSLIST_DEBIT_TEXT_WITH01			320
#define	CAS_CX_STATUSLIST_DEBIT_TEXT_WITH02			240
#define	CAS_CX_STATUSLIST_DEBIT_TEXT_WITH03			340

/* for not screwing up the ID, public DEFINE object use after 0x0a00 ~IDs */
#define	CAS_CX_STATUSLIST_ITEM_WIDTH				702
#define	CAS_CX_STATUSLIST_ITEM_HEIGHT				36
#define	CAS_CX_STATUSLIST_ITEM_LINESPACE			48

/* List Title Item 01 */
#define	CAS_CX_STATUSLIST_TEXTLIST_TITLE01_ID		(eMenuFrame_Window_ConditionalAccess + 0x0001)

#define	CAS_CX_STATUSLIST_TITLE_WIDTH				920
#define	CAS_CX_STATUSLIST_TITLE_HEIGHT				40
#define	CAS_CX_STATUSLIST_TITLE_X					180
#define	CAS_CX_STATUSLIST_TITLE_Y					138

/* List Title Item 02 */
#define	CAS_CX_STATUSLIST_TEXTLIST_TITLE02_ID		(eMenuFrame_Window_ConditionalAccess + 0x0002)

/* List Title Item 03 */
#define	CAS_CX_STATUSLIST_TEXTLIST_TITLE03_ID		(eMenuFrame_Window_ConditionalAccess + 0x0003)

/* List Box */
#define	CAS_CX_STATUSLIST_LIST_BOX_ID				(eMenuFrame_Window_ConditionalAccess + 0x0004)

#define	CAS_CX_STATUSLIST_LIST_BOX_WIDTH			987
#define	CAS_CX_STATUSLIST_LIST_BOX_HEIGHT			468
#define	CAS_CX_STATUSLIST_LIST_BOX_X				143
#define	CAS_CX_STATUSLIST_LIST_BOX_Y				168

/* List Box Scroll */
#define	CAS_CX_STATUSLIST_LIST_SCROLL_WIDTH			9
#define	CAS_CX_STATUSLIST_LIST_SCROLL_HEIGHT		435
#define	CAS_CX_STATUSLIST_LIST_SCROLL_X				962
#define	CAS_CX_STATUSLIST_LIST_SCROLL_Y				16
#define	CAS_CX_STATUSLIST_LIST_SCROLLBALL_H 		20


/* List Box Item */
#define	CAS_CX_STATUSLIST_LIST_ID					(eMenuFrame_Window_ConditionalAccess + 0x0005)

#define	CAS_CX_STATUSLIST_LIST_ITEM_WIDTH			730
#define	CAS_CX_STATUSLIST_LIST_ITEM_HEIGHT			48	/* line space юс */

#define	CAS_CX_STATUSLIST_LIST_WIDTH				987
#define	CAS_CX_STATUSLIST_LIST_HEIGHT				468
#define	CAS_CX_STATUSLIST_LIST_X					143
#define	CAS_CX_STATUSLIST_LIST_Y					168

#endif /* __XMGR_CAS_CX_STATUSLIST_COORIDNATE_H__ */

/* end of file */
