
/**
	@file     ap_cas_cx_menu_coordinate.h
	@brief    coordinate repository of conax smart menu

	Description: CONAX Menu dimension header file for kraken \n
	Module: Application(Layer), Menu(module), CAS_CX(module) \n
	Remarks : \n
	 Date			Modification							Name\n
	-----------	-------------------------------------	------------------\n
	 2009/07/22		init					 				jhlee\n

	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef __XMGR_CAS_CX_MENU_COORDINATE_H__
#define __XMGR_CAS_CX_MENU_COORDINATE_H__

#define	CAS_CX_MENU_APP_WINDOW_WIDTH				1280
#define	CAS_CX_MENU_APP_WINDOW_HEIGHT				720
#define	CAS_CX_MENU_APP_WINDOW_X					(OSD_SCREEN_WIDTH/2 - CAS_CX_MENU_APP_WINDOW_WIDTH/2)
#define	CAS_CX_MENU_APP_WINDOW_Y					(OSD_SCREEN_HEIGHT/2 - CAS_CX_MENU_APP_WINDOW_HEIGHT/2)

#define	CAS_CX_MENU_APP_TOUPPER_ON					FALSE
#define	CAS_CX_MENU_APP_TOUPPER_OFF					TRUE
#if 0
typedef enum
{
#if defined(CONFIG_CX_ORDERING)
	eCasCXMenuItem_Ordering,					//Product Ordering, Modem Ordering
#endif
	eCasCXMenuItem_Subscription,				// Subscription Status
	eCasCXMenuItem_Event,						// Event Status
	eCasCXMenuItem_Tokens,						// Tokens Status
	eCasCXMenuItem_Change_Pin,					// CA Pin Change
#if defined(CONFIG_CX_SIGNATURE)
	eCasCXMenuItem_Change_Signature_Pin,		//
#endif
	eCasCXMenuItem_Maturity_Rating,				// Maturity Level
#if defined(CONFIG_MW_CAS_CONAX_TEXT)
	eCasCXMenuItem_Text_View,					// Message
#endif
	eCasCXMenuItem_About,						// CA System Information
	eCasCXMenuItem_Max
} XmgrCas_CxCaMenuItem_e;
#endif
#endif /* __XMGR_CAS_CX_MENU_COORDINATE_H__ */

