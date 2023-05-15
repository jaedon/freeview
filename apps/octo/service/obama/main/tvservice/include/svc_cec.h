/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  svc_cec.h		\n
	@brief	  cec service	\n

	Description: CEC data 처리 모듈. \n
	Module: MW/CEC									 					\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__SVC_CEC_H__
#define	__SVC_CEC_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*--------------------------------------------------------*/
#define ________CEC_HUMAX_CODE________
/*--------------------------------------------------------*/
#define CEC_VENDOR_ID_HUMAX					0x000378

/*--------------------------------------------------------*/
#define ________CEC_LOGICAL_ADDRESS________
/*--------------------------------------------------------*/
#define CEC_LA_TV							0
#define CEC_LA_RECORDING_DEVICE1			1
#define CEC_LA_RECORDING_DEVICE2			2
#define CEC_LA_STB1							3
#define CEC_LA_DVD1							4
#define CEC_LA_AUDIO_SYSTEM					5
#define CEC_LA_STB2							6
#define CEC_LA_STB3							7
#define CEC_LA_DVD2							8
#define CEC_LA_RECORDING_DEVICE3			9
#define CEC_LA_STB4							10
#define CEC_LA_PLAYBACK						11
#define CEC_LA_RESEERVED3					12
#define CEC_LA_RESEERVED4					13
#define CEC_LA_FREE_USE						14
#define CEC_LA_UNREGISTERED					15	/* as initiator */
#define CEC_LA_BROADCAST					15	/* as destination */



/*--------------------------------------------------------*/
#define ________CEC_OPCODE________
/*--------------------------------------------------------*/

/* CEC 6 Frame Description : opcode와 operand를 포함해서 16byte */
#define CEC_MAX_OPERAND_LEN							15

/* One Touch Play */
#define CEC_OPCODE_ACTIVE_SOURCE					0x82
#define CEC_OPCODE_IMAGE_VIEW_ON					0x04
#define CEC_OPCODE_TEXT_VIEW_ON						0x0D
#define CEC_OPCODE_TV_POWER_OFF						0x36

/* Routing Control */
#define CEC_OPCODE_REQUEST_ACTIVE_SOURCE			0x85
#define CEC_OPCODE_ROUTING_CHANGE					0x80
#define CEC_OPCODE_ROUTING_INFORMATION				0x81
#define CEC_OPCODE_SET_STREAM_PATH					0x86

/* Standby */
#define CEC_OPCODE_STANDBY							0x36

/* One Touch Record Feature */
#define CEC_OPCODE_RECORD_OFF						0x0B
#define CEC_OPCODE_RECORD_ON						0x09
#define CEC_OPCODE_RECORD_STATUS					0x0A
#define CEC_OPCODE_RECORD_TV_SCREEN					0x0F

#define CEC_OPERAND_RECORD_OWN_SOURCE				1
#define CEC_OPERAND_RECORD_DIGITAL_SERVICE			2

/* System Information */
#define CEC_OPCODE_GIVE_PHYSICAL_ADDRESS			0x83
#define CEC_OPCODE_GET_MENU_LANGUAGE				0x91
#define CEC_OPCODE_REPORT_PHYSICAL_ADDRESS			0x84
#define CEC_OPCODE_SET_MENU_LANGUAGE				0x32

#define CEC_OPERAND_DEVICE_TYPE_TV					0
#define CEC_OPERAND_DEVICE_TYPE_RECORDING			1
#define CEC_OPERAND_DEVICE_TYPE_STB					3
#define CEC_OPERAND_DEVICE_TYPE_DVD					4
#define CEC_OPERAND_DEVICE_TYPE_AUDIO				5

/* Deck Control Feature */
#define CEC_OPCODE_DECK_CONTROL						0x42
#define CEC_OPCODE_DECK_STATUS						0x1B
#define CEC_OPCODE_GIVE_DECK_STATUS					0x1A
#define CEC_OPCODE_DECK_PLAY						0x41

/* Tuner control */
#define CEC_OPCODE_GIVE_TUNER_DEVICE_STATUS			0x08
#define CEC_OPCODE_SELECT_DIGITAL_SERVICE			0x93
#define CEC_OPCODE_TUNER_DEVICE_STATUS				0x07
#define CEC_OPCODE_TUNER_STEP_DECREMENT				0x06
#define CEC_OPCODE_TUNER_STEP_INCREMENT				0x05

#define CEC_OPERAND_BROADCAST_SYSTEM_ARIB			0
#define CEC_OPERAND_BROADCAST_SYSTEM_ATSC			1
#define CEC_OPERAND_BROADCAST_SYSTEM_DVB			2

/* Vendor Specific Commands Feature */
#define CEC_OPCODE_DEVICE_VENDOR_ID					0x87
#define CEC_OPCODE_GIVE_VENDOR_ID					0x8C
#define CEC_OPCODE_VENDOR_COMMAND					0x89
#define CEC_OPCODE_VENDOR_REMOTE_BUTTON_DOWN	0x8A
#define CEC_OPCODE_VENDOR_REMOTE_BUTTON_UP			0x8B

#define CEC_OPERAND_RCU_SELECT						0x00
#define CEC_OPERAND_RCU_UP							0x01
#define CEC_OPERAND_RCU_DOWN						0x02
#define CEC_OPERAND_RCU_LEFT						0x03
#define CEC_OPERAND_RCU_RIGHT						0x04
#define CEC_OPERAND_RCU_ROOT_MENU					0x09
#define CEC_OPERAND_RCU_EXIT						0x0D
#define CEC_OPERAND_RCU_11							0x1E
#define CEC_OPERAND_RCU_12							0x1F
#define CEC_OPERAND_RCU_0							0x20
#define CEC_OPERAND_RCU_1							0x21
#define CEC_OPERAND_RCU_2							0x22
#define CEC_OPERAND_RCU_3							0x23
#define CEC_OPERAND_RCU_4							0x24
#define CEC_OPERAND_RCU_5							0x25
#define CEC_OPERAND_RCU_6							0x26
#define CEC_OPERAND_RCU_7							0x27
#define CEC_OPERAND_RCU_8							0x28
#define CEC_OPERAND_RCU_9							0x29
#define CEC_OPERAND_RCU_CHANNEL_UP					0x30
#define CEC_OPERAND_RCU_CHANNEL_DOWN				0x31
#define CEC_OPERAND_RCU_EPG							0x53
#define CEC_OPERAND_RCU_BLUE						0x71
#define CEC_OPERAND_RCU_RED							0x72
#define CEC_OPERAND_RCU_GREEN						0x73
#define CEC_OPERAND_RCU_YELLOW						0x74
#define CEC_OPERAND_RCU_DATA						0x76

/* OSD Status Display Feature */
#define CEC_OPCODE_SET_OSD_STRING					0x64

/* Device OSD Transfer Feature */
#define CEC_OPCODE_GIVE_OSD_NAME					0x46
#define CEC_OPCODE_SET_OSD_NAME						0x47

/* Device Menu Control Feature */
#define CEC_OPCODE_MENU_REQUEST						0x8D
#define CEC_OPCODE_MENU_STATUS						0x8E
#define CEC_OPCODE_USER_CONTROL_PRESSED				0x44
#define CEC_OPCODE_USER_CONTROL_RELEASED			0x45

/* Power Status */
#define CEC_OPCODE_GIVE_POWER_STATUS				0x8F
#define CEC_OPCODE_REPORT_POWER_STATUS				0x90

#define CEC_OPERAND_POWER_STATE_ON					0x00
#define CEC_OPERAND_POWER_STATE_STANDBY				0x01
#define CEC_OPERAND_POWER_STATE_TO_ON				0x02
#define CEC_OPERAND_POWER_STATE_TO_STANDBY			0x03

/* General Protocol */
#define CEC_OPCODE_FEATURE_ABORT					0x00
#define CEC_OPCODE_ABORT							0xFF

#define CEC_OPERAND_UNRECOGNIZED_OPCODE				0
#define CEC_OPERAND_NOT_IN_CORRECT_MODE				1
#define CEC_OPERAND_CANNOT_PROVIDE_SOURCE			2
#define CEC_OPERAND_INVALID_OPERAND					3
#define CEC_OPERAND_REFUSED							4


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      enum          *************************/
/*******************************************************************/
enum
{
	eSEVT_MW_CEC_ = eSEVT_CEC_START,

	eSEVT_MW_CEC_POWER_OFF,

	eSEVT_MW_CEC_END  /* enum 끝 표시. 사용하는 메시지 아님. */
};

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
extern HERROR	SVC_CEC_Init(PostMsgToMgrCb_t fnPostMsgToMgr);
extern void		SVC_CEC_SetPowerState(HBOOL pwrState);
extern HERROR	SVC_CEC_SetModelName(HCHAR *pszModelName);
extern void SVC_CEC_GetPowerState(HBOOL *pbPwrState);
extern void		SVC_CEC_PowerOffTv(void);
extern void		SVC_CEC_PowerOnTv(void);
extern void     SVC_CEC_UserCtrl_Pressed(DxHdmiUserCtrlPressed_e eParam);
#define ______DEBUG_____________________________________________________________________________________________________


#endif /* !__SVC_CEC_H__ */
