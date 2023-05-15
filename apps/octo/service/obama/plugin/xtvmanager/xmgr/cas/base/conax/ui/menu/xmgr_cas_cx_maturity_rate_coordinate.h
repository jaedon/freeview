
/**
	@file     ap_cas_cx_maturity_rate_coordinate.h
	@brief    coordinate repository of conax Menu - Maturity rating

	Description: Portal dimension header file for kraken \n
	Module: Application(Layer), Menu(module), CAS_CX(module) \n
	Remarks : \n
	 Date			Modification							Name\n
	-----------	-------------------------------------	------------------\n
	 2009/07/28		init					 				jhlee\n

	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


#ifndef __XMGR_CAS_CX_MATURITY_RATE_COORDINATE_H__
#define __XMGR_CAS_CX_MATURITY_RATE_COORDINATE_H__

#define	CAS_CX_MR_WINDOW_WIDTH				OSD_SCREEN_WIDTH
#define	CAS_CX_MR_WINDOW_HEIGHT				OSD_SCREEN_HEIGHT
#define	CAS_CX_MR_WINDOW_X					(OSD_SCREEN_WIDTH/2-CAS_CX_MR_WINDOW_WIDTH/2)
#define	CAS_CX_MR_WINDOW_Y					(OSD_SCREEN_HEIGHT/2-CAS_CX_MR_WINDOW_HEIGHT/2)

#define	CAS_CX_MR_ITEM_WIDTH				702
#define	CAS_CX_MR_ITEM_HEIGHT				36
#define	CAS_CX_MR_ITEM_LINESPACE			48

#define	CAS_CX_MR_ITEM_INFO_WIDTH			50
#define	CAS_CX_MR_ITEM_INFOTEXT_WIDTH		652

#define	CAS_CX_MR_ITEM_X					158
#define	CAS_CX_MR_ITEM_Y					(145 + CAS_CX_MR_ITEM_LINESPACE)

/* mainitem 01 position - Card Level */
#define	CAS_CX_MR_ITEM01_ID				(eMenuFrame_Window_ConditionalAccess + 0x0001)

#define	CAS_CX_MR_ITEM01_WIDTH			CAS_CX_MR_ITEM_WIDTH
#define	CAS_CX_MR_ITEM01_HEIGHT			CAS_CX_MR_ITEM_HEIGHT
#define	CAS_CX_MR_ITEM01_X				CAS_CX_MR_ITEM_X
#define	CAS_CX_MR_ITEM01_Y				(CAS_CX_MR_ITEM_Y + CAS_CX_MR_ITEM_LINESPACE)

/* mainitem 02 position */
#define	CAS_CX_MR_ITEM02_ID				(eMenuFrame_Window_ConditionalAccess + 0x0002)

#define	CAS_CX_MR_ITEM02_WIDTH			CAS_CX_MR_ITEM_WIDTH
#define	CAS_CX_MR_ITEM02_HEIGHT			CAS_CX_MR_ITEM_HEIGHT
#define	CAS_CX_MR_ITEM02_X				CAS_CX_MR_ITEM01_X
#define	CAS_CX_MR_ITEM02_Y				(CAS_CX_MR_ITEM01_Y + CAS_CX_MR_ITEM_LINESPACE)

/* mainitem 03 position - Available Settings : */
#define	CAS_CX_MR_ITEM03_ID				(eMenuFrame_Window_ConditionalAccess + 0x0003)

#define	CAS_CX_MR_ITEM03_WIDTH			CAS_CX_MR_ITEM_WIDTH
#define	CAS_CX_MR_ITEM03_HEIGHT			CAS_CX_MR_ITEM_HEIGHT
#define	CAS_CX_MR_ITEM03_X				CAS_CX_MR_ITEM01_X
#define	CAS_CX_MR_ITEM03_Y				(CAS_CX_MR_ITEM02_Y + CAS_CX_MR_ITEM_LINESPACE)

/* mainitem 04 position - G*/
#define	CAS_CX_MR_ITEM04_ID				(eMenuFrame_Window_ConditionalAccess + 0x0004)

#define	CAS_CX_MR_ITEM04_WIDTH			CAS_CX_MR_ITEM_INFO_WIDTH
#define	CAS_CX_MR_ITEM04_HEIGHT			CAS_CX_MR_ITEM_HEIGHT
#define	CAS_CX_MR_ITEM04_X				CAS_CX_MR_ITEM01_X
#define	CAS_CX_MR_ITEM04_Y				(CAS_CX_MR_ITEM03_Y + CAS_CX_MR_ITEM_LINESPACE)

/* mainitem 08 position - G Content */
#define	CAS_CX_MR_ITEM08_ID				(eMenuFrame_Window_ConditionalAccess + 0x0008)

#define	CAS_CX_MR_ITEM08_WIDTH			CAS_CX_MR_ITEM_INFOTEXT_WIDTH
#define	CAS_CX_MR_ITEM08_HEIGHT			CAS_CX_MR_ITEM_HEIGHT
#define	CAS_CX_MR_ITEM08_X				(CAS_CX_MR_ITEM01_X + CAS_CX_MR_ITEM_INFO_WIDTH)
#define	CAS_CX_MR_ITEM08_Y				(CAS_CX_MR_ITEM03_Y + CAS_CX_MR_ITEM_LINESPACE)

/* mainitem 05 position - PG */
#define	CAS_CX_MR_ITEM05_ID				(eMenuFrame_Window_ConditionalAccess + 0x0005)

#define	CAS_CX_MR_ITEM05_WIDTH			CAS_CX_MR_ITEM_INFO_WIDTH
#define	CAS_CX_MR_ITEM05_HEIGHT			CAS_CX_MR_ITEM_HEIGHT
#define	CAS_CX_MR_ITEM05_X				CAS_CX_MR_ITEM01_X
#define	CAS_CX_MR_ITEM05_Y				(CAS_CX_MR_ITEM04_Y + CAS_CX_MR_ITEM_LINESPACE)

/* mainitem 09 position - PG Content */
#define	CAS_CX_MR_ITEM09_ID				(eMenuFrame_Window_ConditionalAccess + 0x0009)

#define	CAS_CX_MR_ITEM09_WIDTH			CAS_CX_MR_ITEM_INFOTEXT_WIDTH
#define	CAS_CX_MR_ITEM09_HEIGHT			CAS_CX_MR_ITEM_HEIGHT
#define	CAS_CX_MR_ITEM09_X				(CAS_CX_MR_ITEM01_X + CAS_CX_MR_ITEM_INFO_WIDTH)
#define	CAS_CX_MR_ITEM09_Y				(CAS_CX_MR_ITEM04_Y + CAS_CX_MR_ITEM_LINESPACE)

/* mainitem 06 position - A */
#define	CAS_CX_MR_ITEM06_ID				(eMenuFrame_Window_ConditionalAccess + 0x0006)

#define	CAS_CX_MR_ITEM06_WIDTH			CAS_CX_MR_ITEM_INFO_WIDTH
#define	CAS_CX_MR_ITEM06_HEIGHT			CAS_CX_MR_ITEM_HEIGHT
#define	CAS_CX_MR_ITEM06_X				CAS_CX_MR_ITEM01_X
#define	CAS_CX_MR_ITEM06_Y				(CAS_CX_MR_ITEM05_Y + CAS_CX_MR_ITEM_LINESPACE)

/* mainitem 10 position - PG Content */
#define	CAS_CX_MR_ITEM10_ID				(eMenuFrame_Window_ConditionalAccess + 0x000A)

#define	CAS_CX_MR_ITEM10_WIDTH			CAS_CX_MR_ITEM_INFOTEXT_WIDTH
#define	CAS_CX_MR_ITEM10_HEIGHT			CAS_CX_MR_ITEM_HEIGHT
#define	CAS_CX_MR_ITEM10_X				(CAS_CX_MR_ITEM01_X + CAS_CX_MR_ITEM_INFO_WIDTH)
#define	CAS_CX_MR_ITEM10_Y				(CAS_CX_MR_ITEM05_Y + CAS_CX_MR_ITEM_LINESPACE)

/* mainitem 07 position - X */
#define	CAS_CX_MR_ITEM07_ID				(eMenuFrame_Window_ConditionalAccess + 0x0007)

#define	CAS_CX_MR_ITEM07_WIDTH			CAS_CX_MR_ITEM_INFO_WIDTH
#define	CAS_CX_MR_ITEM07_HEIGHT			CAS_CX_MR_ITEM_HEIGHT
#define	CAS_CX_MR_ITEM07_X				CAS_CX_MR_ITEM01_X
#define	CAS_CX_MR_ITEM07_Y				(CAS_CX_MR_ITEM06_Y + CAS_CX_MR_ITEM_LINESPACE)

/* mainitem 11 position - X Content */
#define	CAS_CX_MR_ITEM11_ID				(eMenuFrame_Window_ConditionalAccess + 0x000B)

#define	CAS_CX_MR_ITEM11_WIDTH			CAS_CX_MR_ITEM_INFOTEXT_WIDTH
#define	CAS_CX_MR_ITEM11_HEIGHT			CAS_CX_MR_ITEM_HEIGHT
#define	CAS_CX_MR_ITEM11_X				(CAS_CX_MR_ITEM01_X + CAS_CX_MR_ITEM_INFO_WIDTH)
#define	CAS_CX_MR_ITEM11_Y				(CAS_CX_MR_ITEM06_Y + CAS_CX_MR_ITEM_LINESPACE)

#define	CAS_CX_MR_ITEM12_ID				(eMenuFrame_Window_ConditionalAccess + 0x000C)


#endif /* __XMGR_CAS_CX_MATURITY_RATE_COORDINATE_H__ */

// End of file

