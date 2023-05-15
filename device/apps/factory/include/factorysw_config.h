/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Application Library
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FACTORYSW_CONFIG_H_
#define _FACTORYSW_CONFIG_H_

/* model specific configuration header */
#include "factorysw_def.h"
#include "BOX_factorysw.h"
#include "factorysw_version.h"

#if !defined(MODEL_NAME)
	#define MODEL_NAME "Model Name"
#endif
#if !defined(MODEL_STRING)
	#define MODEL_STRING ""
#endif
#if !defined(RELEASE_VERSION)
	#define RELEASE_VERSION "0.0"
#endif
#if !defined(TOTAL_SHEET_NUMBER)
	#define TOTAL_SHEET_NUMBER 3
#endif
#if !defined(TV_STANDARD_OUTPUT_TYPE)
	#define TV_STANDARD_OUTPUT_TYPE TV_STANDARD_OUTPUT_PAL
#endif

/* 
 * VFD display string
 */
#if (FRONT_DISPLAY_UI_LENGTH >= 8)

#define FAI_TESTMODE_STR 						"TESTMODE"

#define FAI_SC_ATRCHECK_OUT_STR 				"CARD Out"
#define FAI_SC_ATRCHECK_PASS_STR 				"CARD Pass"
#define FAI_SC_ATRCHECK_FAIL_STR 				"CARD Fail"

#define FAI_CI_OUT_STR							"CI OUT"
#define FAI_CI_PASS_STR 						"CI Pass"
#define FAI_CI_FAIL_STR 						"CI Fail"

#define FAI_CI_AV_LOCK_STR 						"C0_Lock"
#define FAI_CI_AV_UNLOCK_STR					"C0_Unlock"

#define FAI_AV_T0_PLAYSD_PASS_STR 				"T0SDLock"
#define FAI_AV_T0_PLAYSD_FAIL_STR 				"T0SDUnLo"

#define FAI_AV_T1_PLAYSD_PASS_STR	 			"T1SDLock"
#define FAI_AV_T1_PLAYSD_FAIL_STR 				"T1SDUnLo"

#define FAI_AV_T0_PLAYHD_PASS_STR 				"T0HDLock"
#define FAI_AV_T0_PLAYHD_FAIL_STR 				"T0HDUnLo"

#define FAI_AV_T1_PLAYHD_PASS_STR 				"T1HDLock"
#define FAI_AV_T1_PLAYHD_FAIL_STR 				"T1HDUnLo"

#define FAI_AV_COLORBARBEEPTEST_STEP1_STR 		"S-vid On"
#define FAI_AV_COLORBARBEEPTEST_STEP2_STR 		"Color On"
#define FAI_AV_COLORBARBEEPTEST_STEP3_STR 		"Color Off"

#define FAI_VOLTAGE_STEP1_STR 					"STEP 1"
#define FAI_VOLTAGE_STEP2_STR 					"STEP 2"
#define FAI_VOLTAGE_STEP3_STR 					"STEP 3"
#define FAI_VOLTAGE_STEP4_STR 					"STEP 4"

#define FAI_CHANNEL_PERFORMANCE_PASS_STR 		"BER Pass"
#define FAI_CHANNEL_PERFORMANCE_FAIL_STR 		"BER Fail"

#define FAI_FLASH_CHECK_PASS_STR 				"Fsh Pass"
#define FAI_FLASH_CHECK_FAIL_STR 				"Fsh Fail"

#define FAI_SCART_VCRBYPASS_ON_STR 				"VCR ByOn"
#define FAI_SCART_VCRBYPASS_OFF_STR 			"VCR ByOf"

#define FAI_SCART_TVBYPASS_ON_STR 				"TV ByOn"
#define FAI_SCART_TVBYPASS_OFF_STR 				"TV ByOf"

#define FAI_AUDIO_MUTE_ON_STR 					"Mute On"
#define FAI_AUDIO_MUTE_OFF_STR 					"Mute Off"

#define FAI_ETHERNET_TEST_PASS_STR 				"Eth Pass"
#define FAI_ETHERNET_TEST_FAIL_STR 				"Eth Fail"

#define FAI_PANEL_DISPLAY_ON_STR 				"FR DI ON"
#define FAI_PANEL_DISPLAY_OFF_STR 				"FR DI OF"

#define FAI_USB_DETECT_PASS_STR 				"USB Pass"
#define FAI_USB_DETECT_PASS_1_2_STR 			"USBAPass"
#define FAI_USB_DETECT_PASS_1_STR 				"USB1Pass"
#define FAI_USB_DETECT_PASS_2_STR 				"USB2Pass"
#define FAI_USB_DETECT_FAIL_STR 				"USB Fail"
#define FAI_USB_DETECT_FAIL_1_2_STR 			"USBAFail"

#define FAI_USB3_DETECT_PASS_STR				"USB3 Pass"
#define FAI_USB3_DETECT_FAIL_STR				"USB3 Fail"

#define FAI_HDD_TEST_PASS_STR 					"HDD Pass"
#define FAI_HDD_TEST_FAIL_STR 					"HDD Fail"

#define FAI_HDCP_PASS_STR 						"HDCPPass"
#define FAI_HDCP_FAIL_STR 						"HDCPFail"

#define FAI_CI_PLUS_PASS_STR 					"CI+ Pass"
#define FAI_CI_PLUS_FAIL_STR 					"CI+ Fail"

#define FAI_CSC_PASS_STR 						"CSC Pass"
#define FAI_CSC_FAIL_STR 						"CSC Fail"

#define FAI_DTCP_PASS_STR 						"DTCPPass"
#define FAI_DTCP_FAIL_STR 						"DTCPFail"

#define FAI_CIPLUS_PASS_STR						"CI+ Pass"
#define FAI_CIPLUS_FAIL_STR						"CI+ Fail"

#define FAI_MMC_TEST_PASS_STR					"SD Pass"
#define FAI_MMC_TEST_FAIL_STR					"SD Fail"

#define FAI_SYSTEM_INFORMATION_STR 				"Info"

#define FAI_SYSTEM_DEFUALT_SETTING_READY_STR 	"DEF Rdy"
#define FAI_SYSTEM_DEFUALT_SETTING_START_STR 	"DEF Srt"
#define FAI_SYSTEM_DEFUALT_SETTING_PASS_STR 	"DEF Pass"
#define FAI_SYSTEM_DEFUALT_SETTING_FAIL_STR 	"DEF Fail"
#define FAI_SYSTEM_DEFUALT_SETTING_EXIT_STR 	"DEF Exit"

#define FAI_MES_DOWNLOAD_READY_STR 				"MES Rdy"
#define FAI_MES_DOWNLOAD_START_STR 				"MES Srt"
#define FAI_MES_DOWNLOAD_PASS_STR 				"MES Pass"
#define FAI_MES_DOWNLOAD_FAIL_STR 				"MES Fail"

#define FAI_FKEY_STANDBY_STR 					"Standby"
#define FAI_FKEY_GUIDE_STR 						"Guide"
#define FAI_FKEY_SELECT_STR 					"Select"
#define FAI_FKEY_OK_STR 						"OK"
#define FAI_FKEY_CHUP_STR 						"CH Up"
#define FAI_FKEY_CHDOWN_STR 					"CH Down"
#define FAI_FKEY_VOLUP_STR 						"VOL Up"
#define FAI_FKEY_VOLDOWN_STR 					"VOL Down"
#define FAI_FKEY_TVRADIO_STR 					"TV RADIO"
#define FAI_FKEY_PAUSE_STR 						"Pause"
#define FAI_FKEY_FF_STR 						"FF"
#define FAI_FKEY_REW_STR 						"REW"
#define FAI_FKEY_PLAY_STR 						"Play"
#define FAI_FKEY_EXIT_STR 						"Exit"
#define FAI_FKEY_ACTIVE_STR 					"Active"
#define FAI_FKEY_INFO_STR 						"Info"
#define FAI_FKEY_WPS_STR 						"WPS"
#define FAI_FKEY_NETWORK_STR 					"Network"
#define FAI_FKEY_UP_STR 						"Up"
#define FAI_FKEY_DOWN_STR 						"Down"
#define FAI_FKEY_LEFT_STR 						"Left"
#define FAI_FKEY_RIGHT_STR 						"Right"
#define FAI_FKEY_MENU_STR 						"Menu"
#define FAI_FKEY_REVOLUTION_STR 				"Revoluti"
#define FAI_FKEY_BACK_STR 						"Back"
#define FAI_FKEY_OPTION_STR 					"Option"
#define FAI_FKEY_EPG_STR 						"EPG"
#define FAI_FKEY_SWRESET_STR 					"SW RESET"



#define FAI_FKEY_PASS_STR 						"AlKeyPas"

#define FAI_FAN_ON_PASS_STR 					"FOn Pass"
#define FAI_FAN_ON_FAIL_STR 					"FOn Fail"
#define FAI_FAN_OFF_PASS_STR 					"FOf Pass"
#define FAI_FAN_OFF_FAIL_STR 					"FOf Fail"

#define FAI_DPORT_CTRL_STEP1_STR 				"DP STP 1"
#define FAI_DPORT_CTRL_STEP2_STR 				"DP STP 2"
#define FAI_DPORT_CTRL_STEP3_STR 				"DP STP 3"
#define FAI_DPORT_CTRL_STEP1_FAIL 				"DF STP 1"
#define FAI_DPORT_CTRL_STEP2_FAIL 				"DF STP 2"
#define FAI_DPORT_CTRL_STEP3_FAIL 				"DF STP 3"

#define FAI_IRB_TEST_PASS_STR 					"BLA Pass"
#define FAI_IRB_TEST_FAIL_STR 					"BLA Fail"

#define FAI_MODEM_VOLTAGE_PASS					"ModVPass"
#define FAI_MODEM_VOLTAGE_FAIL					"ModVFail"
#define FAI_MODEM_COMM_21_PASS 					"Mo21Pass"
#define FAI_MODEM_COMM_21_FAIL 					"Mo21Fail"
#define FAI_MODEM_COMM_23_PASS 					"Mo23Pass"
#define FAI_MODEM_COMM_23_FAIL 					"Mo23Fail"

#endif /* End of (FRONT_DISPLAY_UI_LENGTH==FRONT_DISPLAY_VFD) */

/* 
 *  7seg display string 
 */
#if (FRONT_DISPLAY_UI_LENGTH < 8)
#define FAI_TESTMODE_STR 						"tESt"

#define FAI_SC_ATRCHECK_OUT_STR 				"CnOt"
#define FAI_SC_ATRCHECK_PASS_STR 				"C-Oh"
#define FAI_SC_ATRCHECK_FAIL_STR 				"C-nG"

#define FAI_CI_OUT_STR							"CI-O"
#define FAI_CI_PASS_STR 						"CI-P"
#define FAI_CI_FAIL_STR 						"CI-F"

#define FAI_CI_AV_LOCK_STR 						"CILO"
#define FAI_CI_AV_UNLOCK_STR					"CIUL"

#define FAI_AV_T0_PLAYSD_PASS_STR 				"T0SL"
#define FAI_AV_T0_PLAYSD_FAIL_STR 				"T0SU"

#define FAI_AV_T1_PLAYSD_PASS_STR 				"T1SL"
#define FAI_AV_T1_PLAYSD_FAIL_STR 				"T1SU"

#define FAI_AV_T0_PLAYHD_PASS_STR 				"T0HL"
#define FAI_AV_T0_PLAYHD_FAIL_STR 				"T0HU"

#define FAI_AV_T1_PLAYHD_PASS_STR 				"T1HL"
#define FAI_AV_T1_PLAYHD_FAIL_STR 				"T1HU"

#define FAI_AV_COLORBARBEEPTEST_STEP1_STR 		"YCOn"
#define FAI_AV_COLORBARBEEPTEST_STEP2_STR 		"COOn"
#define FAI_AV_COLORBARBEEPTEST_STEP3_STR 		"COOF"

#define FAI_VOLTAGE_STEP1_STR 					"StP1"
#define FAI_VOLTAGE_STEP2_STR 					"StP2"
#define FAI_VOLTAGE_STEP3_STR 					"StP3"
#define FAI_VOLTAGE_STEP4_STR 					"StP4"

#define FAI_CHANNEL_PERFORMANCE_PASS_STR 		"bErP"
#define FAI_CHANNEL_PERFORMANCE_FAIL_STR 		"bErF"

#define FAI_FLASH_CHECK_PASS_STR 				"FLCP"
#define FAI_FLASH_CHECK_FAIL_STR 				"FLCF"

#define FAI_SCART_VCRBYPASS_ON_STR 				"vbOn"
#define FAI_SCART_VCRBYPASS_OFF_STR 			"vbOf"

#define FAI_SCART_TVBYPASS_ON_STR 				"TbOn"
#define FAI_SCART_TVBYPASS_OFF_STR 				"TbOf"

#define FAI_AUDIO_MUTE_ON_STR 					"M-On"
#define FAI_AUDIO_MUTE_OFF_STR 					"M-Of"

#define FAI_ETHERNET_TEST_PASS_STR 				"EtHP"
#define FAI_ETHERNET_TEST_FAIL_STR 				"EtHF"

#define FAI_USB_DETECT_PASS_STR 				"USbP"
#define FAI_USB_DETECT_PASS_1_2_STR 			"UbAP"
#define FAI_USB_DETECT_PASS_1_STR 				"Ub1P"
#define FAI_USB_DETECT_PASS_2_STR 				"Ub2P"
#define FAI_USB_DETECT_FAIL_STR 				"USbF"
#define FAI_USB_DETECT_FAIL_1_2_STR 			"UbAF"

#define FAI_HDD_TEST_PASS_STR 					"HddP"
#define FAI_HDD_TEST_FAIL_STR 					"HddF"

#define FAI_HDCP_PASS_STR 						"HdPP"
#define FAI_HDCP_FAIL_STR 						"HdPF"

#define FAI_VIRTUAL_KEY_PASS_STR 				"WEdP"
#define FAI_VIRTUAL_KEY_FAIL_STR 				"WEdF"

#define FAI_DTCP_PASS_STR 						"dtcP"
#define FAI_DTCP_FAIL_STR 						"dtcF"

#define FAI_CIPLUS_PASS_STR						"cIPP"
#define FAI_CIPLUS_FAIL_STR						"cIPF"

#define FAI_MMC_TEST_PASS_STR					"Sd P"
#define FAI_MMC_TEST_FAIL_STR					"Sd F"

#define FAI_CM_PASS_STR 							"CMPP"    /* NPI 협의에서 누락.협의 추가 필요*/
#define FAI_CM_FAIL_STR 							"CMPF"

#define FAI_SYSTEM_INFORMATION_STR 				"InFO"

#define FAI_SYSTEM_DEFUALT_SETTING_READY_STR 	"rEdY"
#define FAI_SYSTEM_DEFUALT_SETTING_START_STR 	"dEFS"
#define FAI_SYSTEM_DEFUALT_SETTING_PASS_STR 	"dEFP"
#define FAI_SYSTEM_DEFUALT_SETTING_FAIL_STR 	"dEFF"
#define FAI_SYSTEM_DEFUALT_SETTING_EXIT_STR 	"DExt"

#define FAI_MES_DOWNLOAD_READY_STR 				"rEdY"
#define FAI_MES_DOWNLOAD_START_STR 				"MESS"
#define FAI_MES_DOWNLOAD_PASS_STR 				"MESP"
#define FAI_MES_DOWNLOAD_FAIL_STR 				"MESF"

#define FAI_FKEY_STANDBY_STR 					"Stby"
#define FAI_FKEY_GUIDE_STR 						"GUId"
#define FAI_FKEY_SELECT_STR 					"SELt"
#define FAI_FKEY_OK_STR 						"Oh"
#define FAI_FKEY_CHUP_STR 						"CHUP"
#define FAI_FKEY_CHDOWN_STR 					"CHdn"
#define FAI_FKEY_VOLUP_STR 						"VOUP"
#define FAI_FKEY_VOLDOWN_STR 					"VOdn"
#define FAI_FKEY_TVRADIO_STR 					"tUrd"
#define FAI_FKEY_PAUSE_STR 						"PRSE"
#define FAI_FKEY_FF_STR 						"FF"
#define FAI_FKEY_REW_STR 						"rEW"
#define FAI_FKEY_PLAY_STR 						"PLAY"
#define FAI_FKEY_EXIT_STR 						"EHIt"
#define FAI_FKEY_ACTIVE_STR 					"ACtI"
#define FAI_FKEY_INFO_STR 						"InFO"
#define FAI_FKEY_WPS_STR 						"WPS"
#define FAI_FKEY_NETWORK_STR 					"NET"
#define FAI_FKEY_UP_STR 						"UP"
#define FAI_FKEY_DOWN_STR 						"DOWn"
#define FAI_FKEY_LEFT_STR 						"LEFt"
#define FAI_FKEY_RIGHT_STR 						"rIGt"
#define FAI_FKEY_MENU_STR 						"MEnU"
#define FAI_FKEY_REVOLUTION_STR 				"rESL"
#define FAI_FKEY_BACK_STR 						"bACh"
#define FAI_FKEY_OPTION_STR 					"OPtS"
#define FAI_FKEY_EPG_STR 						"EPG"
#define FAI_FKEY_SWRESET_STR 					"SWRS"


#define FAI_FAN_ON_PASS_STR 					"FOnP"
#define FAI_FAN_ON_FAIL_STR 					"FOnF"
#define FAI_FAN_OFF_PASS_STR 					"FOfP"
#define FAI_FAN_OFF_FAIL_STR 					"FOfF"

#define FAI_FKEY_PASS_STR 						"PASS"

#define FAI_DPORT_CTRL_STEP1_STR 				"dPS1"
#define FAI_DPORT_CTRL_STEP2_STR 				"dPS2"
#define FAI_DPORT_CTRL_STEP3_STR 				"dPS3"
#define FAI_DPORT_CTRL_STEP1_FAIL 				"dFS1"
#define FAI_DPORT_CTRL_STEP2_FAIL 				"dFS2"
#define FAI_DPORT_CTRL_STEP3_FAIL 				"dFS3"

#define FAI_IRB_TEST_PASS_STR 					"IrbP"
#define FAI_IRB_TEST_FAIL_STR 					"IrbF"

#define FAI_MODEM_VOLTAGE_PASS					"MOUP"
#define FAI_MODEM_VOLTAGE_FAIL					"MOUF"
#define FAI_MODEM_COMM_21_PASS 					"M21P"
#define FAI_MODEM_COMM_21_FAIL 					"M21F"
#define FAI_MODEM_COMM_23_PASS 					"M23P"
#define FAI_MODEM_COMM_23_FAIL 					"M23F"

#endif /* End of (FRONT_DISPLAY_UI_LENGTH==FRONT_DISPLAY_7SEG) */

#endif /* factorysw_config.h */

