
/**
	@file     ap_cas_va_menu_coordinate.h
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

#ifndef __AP_CAS_VA_MENU_COORDINATE_H__
#define __AP_CAS_VA_MENU_COORDINATE_H__

#define	CAS_VA_MENU_APP_WINDOW_WIDTH				1280
#define	CAS_VA_MENU_APP_WINDOW_HEIGHT				720
#define	CAS_VA_MENU_APP_WINDOW_X					(OSD_SCREEN_WIDTH/2 - CAS_VA_MENU_APP_WINDOW_WIDTH/2)
#define	CAS_VA_MENU_APP_WINDOW_Y					(OSD_SCREEN_HEIGHT/2 - CAS_VA_MENU_APP_WINDOW_HEIGHT/2)

typedef enum
{
	eCasVAMenuItem_Rights,						// Subscription Status
	eCasVAMenuItem_Operator_Msg,				// Event Status
	eCasVAMenuItem_Stb_Info,					// STB Information Status
	eCasVAMenuItem_Smartcard_Info,				// Smartcard Information
#ifdef CONFIG_MW_CAS_VIACCESS_PVR
	eCasVAMenuItem_PvrPlayback_Info,			// PVR Playback information
#endif
	eCasVAMenuItem_Change_Pin,					// CA Pin Change
	eCasVAMenuItem_Maturity_Rating,				// Maturity Level
	//eCasVAMenuItem_Preselection_Del,			// Message
	//eCasVAMenuItem_Preselection_Add,			// CA System Information
	//eCasVAMenuItem_Class_Info,				// Class List Info
	eCasVAMenuItem_Max,
} CasViaccessCAMenu_Item_t;

typedef enum {
	eCasVa_StbInfoItem_VaManuf_ID,				/* Viaccess manufacturer identifier */
	eCasVa_StbInfoItem_VaModel_ID,				/* Viaccess model identifier */
	eCasVa_StbInfoItem_Teminal_SN,				/* Terminal serial number */
	eCasVa_StbInfoItem_HW_Version,				/* Hardware version */
	eCasVa_StbInfoItem_Bootloader_Version,		/* Boot loader version */
	eCasVa_StbInfoItem_SW_Version,				/* Software version */
	eCasVa_StbInfoItem_STB_ID,					/* STB identifier */
#ifdef CONFIG_MW_CAS_VIACCESS_PVR
	eCasVa_StbInfoItem_HDD_SN,					/* Hard disk serial number */
#endif
	eCasVa_StbInfoItem_UA,						/* UA currently inserted */
	eCasVa_StbInfoItem_ACS_Version,				/* ACS library version */
#ifdef CONFIG_MW_CAS_VIACCESS_PURPLE_BOX
	eCasVa_StbInfoItem_Terminal_ID,				/* Terminal id */
#endif
	eCasVa_StbInfoItem_CAMLock_Value,			/* CAMLock value */
	eCasVa_StbInfoItem_Max,
} CasVaStbInfoItem_t;


// STATIC smartcardinfo menu items
typedef enum {
	eCasVa_SmartCardInfoItem_FirstBlank,
	eCasVa_SmartCardInfoItem_AutoParingMode,
	eCasVa_SmartCardInfoItem_NumOfScPermiited,
	eCasVa_SmartCardInfoItem_SecondBlank,
	eCasVa_SmartCardInfoItem_UAofScPermitted,
	eCasVa_SmartCardInfoItem_Max,
} CasVaSmartCardInfoItem_t;

#ifdef CONFIG_MW_CAS_VIACCESS_PVR
typedef enum{
	eCasVa_PvrPlaybackInfoItem_RecordDate,
	eCasVa_PvrPlaybackInfoItem_FirstPbDate,
	eCasVa_PvrPlaybackInfoItem_NumOfPb,
	eCasVa_PvrPlaybackInfoItem_Max,
}CasVaPvrPlaybackInfoItem_t;
#endif


#define VA_UI_NO_MODE							0
#define VA_UI_AUTOPAIRING_MODE					1
#define VA_UI_PAIRING_MODE						4
#define VA_UI_SC_PAIRING_DEFAULT_ITEM_NUM 		2
#define VA_UI_SC_AUTOPAIRING_DEFAULT_ITEM_NUM	7

#define VA_UI_UA_SIZE							5	//kVA_UI_UA_SIZE
#define VA_UI_MAX_SO_LABLE						16	//kVA_UI_MAX_LABEL_SIZE
#define VA_UI_MAX_CLASS_MARK					32	//kVA_UI_MAX_CLASS_MASK_SIZE
#define VA_UI_OP_MSG_SIZE						256	//kVA_UI_MAX_OPERATOR_ITEM_SIZE
#define VA_UI_OP_DATA_SIZE						256	//kVA_UI_MAX_OPERATOR_ITEM_SIZE
#define VA_UI_CURRENCY_LABEL_SIZE				16	//kVA_UI_MAX_CURRENCY_LABEL_SIZE

#endif /* __AP_CAS_CX_MENU_COORDINATE_H__ */

