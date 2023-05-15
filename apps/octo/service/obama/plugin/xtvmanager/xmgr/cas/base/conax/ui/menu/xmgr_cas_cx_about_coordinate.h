
/**
	@file     ap_cas_cx_about_coordinate.h
	@brief    coordinate repository of conax Menu - About

	Description: Portal dimension header file for kraken \n
	Module: Application(Layer), Menu(module), CAS_CX(module) \n
	Remarks : \n
	 Date			Modification							Name\n
	-----------	-------------------------------------	------------------\n
	 2009/07/23		init					 				jhlee\n

	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef __XMGR_CAS_CX_ABOUT_COORDINATE_H__
#define __XMGR_CAS_CX_ABOUT_COORDINATE_H__

#define	CAS_CX_ABOUT_APP_WINDOW_WIDTH			OSD_SCREEN_WIDTH
#define	CAS_CX_ABOUT_APP_WINDOW_HEIGHT			OSD_SCREEN_HEIGHT
#define	CAS_CX_ABOUT_APP_WINDOW_X				(OSD_SCREEN_WIDTH/2-CAS_CX_ABOUT_APP_WINDOW_WIDTH/2)
#define	CAS_CX_ABOUT_APP_WINDOW_Y				(OSD_SCREEN_HEIGHT/2-CAS_CX_ABOUT_APP_WINDOW_HEIGHT/2)


/* for not screwing up the ID, public DEFINE object use after 0x0a00 ~IDs */
#define	CAS_CX_ABOUT_ITEM_WIDTH				702
#define	CAS_CX_ABOUT_ITEM_HEIGHT			36
#define	CAS_CX_ABOUT_ITEM_LINESPACE			48

/* mainitem 01 position */
#define	CAS_CX_ABOUT_ITEM01_ID			(eMenuFrame_Window_ConditionalAccess + 0x0001)

#define	CAS_CX_ABOUT_ITEM01_WIDTH		CAS_CX_ABOUT_ITEM_WIDTH
#define	CAS_CX_ABOUT_ITEM01_HEIGHT		CAS_CX_ABOUT_ITEM_HEIGHT
#define	CAS_CX_ABOUT_ITEM01_X			250
#define	CAS_CX_ABOUT_ITEM01_Y			145

/* mainitem 02 position */
#define	CAS_CX_ABOUT_ITEM02_ID			(eMenuFrame_Window_ConditionalAccess + 0x0002)

#define	CAS_CX_ABOUT_ITEM02_WIDTH		CAS_CX_ABOUT_ITEM_WIDTH
#define	CAS_CX_ABOUT_ITEM02_HEIGHT		CAS_CX_ABOUT_ITEM_HEIGHT
#define	CAS_CX_ABOUT_ITEM02_X			CAS_CX_ABOUT_ITEM01_X
#define	CAS_CX_ABOUT_ITEM02_Y			(CAS_CX_ABOUT_ITEM01_Y + CAS_CX_ABOUT_ITEM_LINESPACE)

/* mainitem 03 position */
#define	CAS_CX_ABOUT_ITEM03_ID			(eMenuFrame_Window_ConditionalAccess + 0x0003)

#define	CAS_CX_ABOUT_ITEM03_WIDTH		CAS_CX_ABOUT_ITEM_WIDTH
#define	CAS_CX_ABOUT_ITEM03_HEIGHT		CAS_CX_ABOUT_ITEM_HEIGHT
#define	CAS_CX_ABOUT_ITEM03_X			CAS_CX_ABOUT_ITEM01_X
#define	CAS_CX_ABOUT_ITEM03_Y			(CAS_CX_ABOUT_ITEM02_Y + CAS_CX_ABOUT_ITEM_LINESPACE)

/* mainitem 04 position */
#define	CAS_CX_ABOUT_ITEM04_ID			(eMenuFrame_Window_ConditionalAccess + 0x0004)

#define	CAS_CX_ABOUT_ITEM04_WIDTH		CAS_CX_ABOUT_ITEM_WIDTH
#define	CAS_CX_ABOUT_ITEM04_HEIGHT		CAS_CX_ABOUT_ITEM_HEIGHT
#define	CAS_CX_ABOUT_ITEM04_X			CAS_CX_ABOUT_ITEM01_X
#define	CAS_CX_ABOUT_ITEM04_Y			(CAS_CX_ABOUT_ITEM03_Y + CAS_CX_ABOUT_ITEM_LINESPACE)

/* mainitem 05 position */
#define	CAS_CX_ABOUT_ITEM05_ID			(eMenuFrame_Window_ConditionalAccess + 0x0005)

#define	CAS_CX_ABOUT_ITEM05_WIDTH		CAS_CX_ABOUT_ITEM_WIDTH
#define	CAS_CX_ABOUT_ITEM05_HEIGHT		CAS_CX_ABOUT_ITEM_HEIGHT
#define	CAS_CX_ABOUT_ITEM05_X			CAS_CX_ABOUT_ITEM01_X
#define	CAS_CX_ABOUT_ITEM05_Y			(CAS_CX_ABOUT_ITEM04_Y + CAS_CX_ABOUT_ITEM_LINESPACE)

/* mainitem 06 position */
#define	CAS_CX_ABOUT_ITEM06_ID			(eMenuFrame_Window_ConditionalAccess + 0x0006)

#define	CAS_CX_ABOUT_ITEM06_WIDTH		CAS_CX_ABOUT_ITEM_WIDTH
#define	CAS_CX_ABOUT_ITEM06_HEIGHT		CAS_CX_ABOUT_ITEM_HEIGHT
#define	CAS_CX_ABOUT_ITEM06_X			CAS_CX_ABOUT_ITEM01_X
#define	CAS_CX_ABOUT_ITEM06_Y			(CAS_CX_ABOUT_ITEM05_Y + CAS_CX_ABOUT_ITEM_LINESPACE)

/* mainitem 07 position */
#define	CAS_CX_ABOUT_ITEM07_ID			(eMenuFrame_Window_ConditionalAccess + 0x0007)

#define	CAS_CX_ABOUT_ITEM07_WIDTH		CAS_CX_ABOUT_ITEM_WIDTH
#define	CAS_CX_ABOUT_ITEM07_HEIGHT		CAS_CX_ABOUT_ITEM_HEIGHT
#define	CAS_CX_ABOUT_ITEM07_X			CAS_CX_ABOUT_ITEM01_X
#define	CAS_CX_ABOUT_ITEM07_Y			(CAS_CX_ABOUT_ITEM06_Y + CAS_CX_ABOUT_ITEM_LINESPACE)

/* mainitem 08 position */
#define	CAS_CX_ABOUT_ITEM08_ID			(eMenuFrame_Window_ConditionalAccess + 0x0008)

#define	CAS_CX_ABOUT_ITEM08_WIDTH		CAS_CX_ABOUT_ITEM_WIDTH
#define	CAS_CX_ABOUT_ITEM08_HEIGHT		CAS_CX_ABOUT_ITEM_HEIGHT
#define	CAS_CX_ABOUT_ITEM08_X			CAS_CX_ABOUT_ITEM01_X
#define	CAS_CX_ABOUT_ITEM08_Y			(CAS_CX_ABOUT_ITEM07_Y + CAS_CX_ABOUT_ITEM_LINESPACE)

/* mainitem 09 position */
#define	CAS_CX_ABOUT_ITEM09_ID			(eMenuFrame_Window_ConditionalAccess + 0x0009)

#define	CAS_CX_ABOUT_ITEM09_WIDTH		CAS_CX_ABOUT_ITEM_WIDTH
#define	CAS_CX_ABOUT_ITEM09_HEIGHT		CAS_CX_ABOUT_ITEM_HEIGHT
#define	CAS_CX_ABOUT_ITEM09_X			CAS_CX_ABOUT_ITEM01_X
#define	CAS_CX_ABOUT_ITEM09_Y			(CAS_CX_ABOUT_ITEM08_Y + CAS_CX_ABOUT_ITEM_LINESPACE)

/* mainitem 10 position */
#define	CAS_CX_ABOUT_ITEM0A_ID			(eMenuFrame_Window_ConditionalAccess + 0x000A)

#define	CAS_CX_ABOUT_ITEM0A_WIDTH		CAS_CX_ABOUT_ITEM_WIDTH
#define	CAS_CX_ABOUT_ITEM0A_HEIGHT		CAS_CX_ABOUT_ITEM_HEIGHT
#define	CAS_CX_ABOUT_ITEM0A_X			CAS_CX_ABOUT_ITEM01_X
#define	CAS_CX_ABOUT_ITEM0A_Y			(CAS_CX_ABOUT_ITEM09_Y + CAS_CX_ABOUT_ITEM_LINESPACE)

#endif /* __XMGR_CAS_CX_ABOUT_COORDINATE_H__ */

/* end of file */

