/**
	@file     ap_cas_va_pincode_ui_coordinate.h
	@brief    file_name & simple comment.

	Description: for VIACCESS Check Pin code UI \n
	Module: Application(Layer), UI(module), CAS(module), VIACCESS(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2009/07/30		modified					jhlee\n

	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


#ifndef __AP_CAS_VA_PINCODE_COORDINATE_H__
#define __AP_CAS_VA_PINCODE_COORDINATE_H__

/*  -- Pin-code Change UI --     */

// frame
#define	CASVA_PIN_FRAME_W					620
#define	CASVA_PIN_FRAME_H					299

#define	CASVA_PIN_FRAME_X					(OSD_SCREEN_WIDTH/2 - CASVA_PIN_FRAME_W/2)
#define	CASVA_PIN_FRAME_Y					(OSD_SCREEN_HEIGHT/2 - CASVA_PIN_FRAME_H/2)

// MSG
#define	CASVA_PIN_MSG_X						30
#define	CASVA_PIN_MSG_Y						100
#define	CASVA_PIN_MSG_W						(CASVA_PIN_FRAME_W - 70)
#define	CASVA_PIN_MSG_H						90

// pin code btn
#define	CASVA_PIN_CODE_BTN_X				180
#define	CASVA_PIN_CODE_BTN_Y				(CASVA_PIN_MSG_Y + CASVA_PIN_MSG_H)

/*  -- Pin-code Check UI --     */
#define	CASVA_PIN_CHECK_FRAME_X				10
#define	CASVA_PIN_CHECK_FRAME_Y				10

#endif /* __AP_CAS_VA_PINCODE_COORDINATE_H__ */
// End of file
