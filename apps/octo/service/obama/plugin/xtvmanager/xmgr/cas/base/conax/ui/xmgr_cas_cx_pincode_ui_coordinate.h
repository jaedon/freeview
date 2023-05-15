/**
	@file     ap_cas_cx_pincode_ui_coordinate.h
	@brief    file_name & simple comment.

	Description: for CONAX Check Pin code UI \n
	Module: Application(Layer), UI(module), CAS(module), CONAX(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2009/07/30		modified					jhlee\n

	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


#ifndef __XMGR_CAS_CX_PINCODE_COORDINATE_H__
#define __XMGR_CAS_CX_PINCODE_COORDINATE_H__

/*  -- Pin-code Change UI --     */
// frame
#define	CASCX_PIN_FRAME_W					620
#define	CASCX_PIN_FRAME_H					299

#define	CASCX_PIN_FRAME_X					(OSD_SCREEN_WIDTH/2 - CASCX_PIN_FRAME_W/2)
#define	CASCX_PIN_FRAME_Y					(OSD_SCREEN_HEIGHT/2 - CASCX_PIN_FRAME_H/2)

// MSG
#define	CASCX_PIN_MSG_X						30
#define	CASCX_PIN_MSG_Y						100
#define	CASCX_PIN_MSG_W						(CASCX_PIN_FRAME_W - 70)
#define	CASCX_PIN_MSG_H						90

// pin code btn
#define	CASCX_PIN_CODE_BTN_X				180
#define	CASCX_PIN_CODE_BTN_Y				(CASCX_PIN_MSG_Y + CASCX_PIN_MSG_H)


/*  -- Pin-code Check UI --     */
#define	CASCX_PIN_CHECK_FRAME_X				10
#define	CASCX_PIN_CHECK_FRAME_Y				10


#endif /* __XMGR_CAS_CX_PINCODE_COORDINATE_H__ */

// End of file

