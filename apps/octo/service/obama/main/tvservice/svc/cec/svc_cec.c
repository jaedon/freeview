/**
	@file     svc_cec.c
	@brief    svc_cec.c (CEC Service)

	Description: CEC data를 처리하는 모듈.                                  \n
	Module: MW/CEC 															\n
	Remarks : 																\n

	Copyright (c) 2008 HUMAX Co., Ltd.										\n
	All rights reserved.													\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <hapi.h>
#include <directfb.h>

#include <pal_pipe.h>
//#include <mwc_util.h>
#include <db_param.h>
#include <svc_cec.h>

#include <pal_cec.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/* <Polling Message> retry */
#define CEC_MAX_SND_POLLING_MESSAGE				3
#define CEC_RETRY_DELAY_POLLING_MESSAGE			100	/* 100msec */

/* <Image View On>, <Active Source>에 대한 최대 retry */
#define CEC_MAX_SND_ERR_IMAGE_VIEW_ON			10
#define CEC_RETRY_DELAY_IMAGE_VIEW_ON			1000	/* 1000msec */

#define CEC_MAX_ERR_WAIT_TV_ON					20
#define CEC_RETRY_DELAY_WAIT_TV_ON				1000	/* 1000msec */

#define CEC_MAX_SND_ERR_ACTIVE_SOURCE			20
#define CEC_RETRY_DELAY_ACITVE_SOURCE			1000	/* 1000msec */
#define CEC_RETRY_DELAY_TV_POWEROFF				100	/* 100msec */

#define CEC_NUM_RCV_BUFFER						16

#define CEC_INVALID_PHYSICAL_ADDR				0xFFFF

typedef enum
{
	eCEC_TASK_MSG_CHK_TASK_RUN,
	eCEC_TASK_MSG_PLUG_OUT,
	eCEC_TASK_MSG_PLUG_IN,
	eCEC_TASK_MSG_SEND_IMAGE_VIEW_ON,
	eCEC_TASK_MSG_WAIT_TV_ON,
	eCEC_TASK_MSG_SEND_ACTIVE_SOURCE,
	eCEC_TASK_MSG_DATA_RCVED,
	eCEC_TASK_MSG_TX_AGING,
	eCEC_TASK_MSG_TV_POWER_OFF,
    eCEC_TASK_MSG_TV_USERCTRL_PRESSED,
} svcCec_TaskEvt_e;

typedef enum
{
	eCEC_TV_POWER_UNKNOWN,
	eCEC_TV_POWER_ON,
	eCEC_TV_POWER_STANDBY,
	eCEC_TV_POWER_TO_ON,
	eCEC_TV_POWER_TO_STANDBY
} svcCec_TvPwrStatus_e;

typedef struct
{
	svcCec_TaskEvt_e	eEvent;
	HUINT32				aulArg[4];
} svcCec_TaskMsg_t;

typedef struct
{
	HUINT16	usPhysicalAddr;
	HUINT8	ucLogicalAddr;

	HBOOL	bActiveSource;

	HUINT32	ulPwrSyncCnt;
	HBOOL	bPwrState;	/* TRUE=on, FALSE=off */

	HUINT8	szModelName[CEC_MAX_OPERAND_LEN];
} svcCec_Info_t;

typedef struct
{
	HBOOL		bValid;
	PalCec_Msg_t	stRxMsg;
} svcCec_MsgBuf_t;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/* debug */
STATIC char *s_szfile = "mw_cec_main.c";

/* cec task */
STATIC HULONG	s_ulCecMsgQId = 0;
STATIC HULONG	s_ulCecTaskId = 0;

STATIC svcCec_Info_t	s_stCecInfo = { 0, };

STATIC svcCec_MsgBuf_t	s_stRxMsgBuf[CEC_NUM_RCV_BUFFER];

/* One Touch Play */
STATIC HUINT32				s_ulCecSndImageViewOnCnt;
STATIC HUINT32				s_ulCecWaitTvOnCnt;
STATIC HUINT32				s_ulCecSndActiveSrcCnt;
STATIC svcCec_TvPwrStatus_e	s_eCecTvPwrStatus;

STATIC PostMsgToMgrCb_t		s_fnSvcCecPostMsgToMgr = NULL;

/*******************************************************************/
/********************      proto-type functions  *******************/
/*******************************************************************/


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
#define ____DEBUG____________________________________________

void svc_cec_PrintCommand(HUINT8 opcode)
{
#if defined(CONFIG_DEBUG)
	switch (opcode)
	{
		/* One Touch Play */
		case CEC_OPCODE_ACTIVE_SOURCE:
			HxLOG_Print("<Active Source>\n");
			break;

		case CEC_OPCODE_IMAGE_VIEW_ON:
			HxLOG_Print("<Image View On>\n");
			break;

		case CEC_OPCODE_TEXT_VIEW_ON:
			HxLOG_Print("<Text View On>\n");
			break;

		/* Routing Control */
		case CEC_OPCODE_REQUEST_ACTIVE_SOURCE:
			HxLOG_Print("<Request Active Source>\n");
			break;

		case CEC_OPCODE_ROUTING_CHANGE:
			HxLOG_Print("<Routing Change>\n");
			break;

		case CEC_OPCODE_ROUTING_INFORMATION:
			HxLOG_Print("<Routing Infomation>\n");
			break;

		case CEC_OPCODE_SET_STREAM_PATH:
			HxLOG_Print("<Set Stream Path>\n");
			break;

		/* Standby */
		case CEC_OPCODE_STANDBY:
			HxLOG_Print("<Standby>\n");
			break;

		/* One Touch Record Feature */
		case CEC_OPCODE_RECORD_OFF:
			HxLOG_Print("<Record Off>\n");
			break;

		case CEC_OPCODE_RECORD_ON:
			HxLOG_Print("<Record On>\n");
			break;

		case CEC_OPCODE_RECORD_STATUS:
			HxLOG_Print("<Record Status>\n");
			break;

		case CEC_OPCODE_RECORD_TV_SCREEN:
			HxLOG_Print("<Record TV Screen>\n");
			break;

		/* System Information */
		case CEC_OPCODE_GIVE_PHYSICAL_ADDRESS:
			HxLOG_Print("<Give Physical Address>\n");
			break;

		case CEC_OPCODE_GET_MENU_LANGUAGE:
			HxLOG_Print("<Get Menu Language>\n");
			break;

		case CEC_OPCODE_REPORT_PHYSICAL_ADDRESS:
			HxLOG_Print("<Report Physical Address>\n");
			break;

		case CEC_OPCODE_SET_MENU_LANGUAGE:
			HxLOG_Print("<Set Menu Language>\n");
			break;

		/* Deck Control Feature */
		case CEC_OPCODE_DECK_CONTROL:
			HxLOG_Print("<Deck Control>\n");
			break;

		case CEC_OPCODE_DECK_STATUS:
			HxLOG_Print("<Deck Status>\n");
			break;

		case CEC_OPCODE_GIVE_DECK_STATUS:
			HxLOG_Print("<Give Deck Status>\n");
			break;

		case CEC_OPCODE_DECK_PLAY:
			HxLOG_Print("<Deck Play>\n");
			break;

		/* Tuner control */
		case CEC_OPCODE_GIVE_TUNER_DEVICE_STATUS:
			HxLOG_Print("<Give Tuner Device Status>\n");
			break;

		case CEC_OPCODE_SELECT_DIGITAL_SERVICE:
			HxLOG_Print("<Select Digital Service>\n");
			break;

		case CEC_OPCODE_TUNER_DEVICE_STATUS:
			HxLOG_Print("<Tuner Device Status>\n");
			break;

		case CEC_OPCODE_TUNER_STEP_DECREMENT:
			HxLOG_Print("<Tuner Step Decrement>\n");
			break;

		case CEC_OPCODE_TUNER_STEP_INCREMENT:
			HxLOG_Print("<Tuner Stop Increment>\n");
			break;

		/* Vendor Specific Commands Feature */
		case CEC_OPCODE_DEVICE_VENDOR_ID:
			HxLOG_Print("<Device Vendor ID>\n");
			break;

		case CEC_OPCODE_GIVE_VENDOR_ID:
			HxLOG_Print("<Give Vendor ID>\n");
			break;

		case CEC_OPCODE_VENDOR_COMMAND	:
			HxLOG_Print("<Vendor Command>\n");
			break;

		case CEC_OPCODE_VENDOR_REMOTE_BUTTON_DOWN:
			HxLOG_Print("<Vendor Remote Button Down>\n");
			break;

		case CEC_OPCODE_VENDOR_REMOTE_BUTTON_UP:
			HxLOG_Print("<Vendor Remote Button Up>\n");
			break;

		/* OSD Status Display Feature */
		case CEC_OPCODE_SET_OSD_STRING:
			HxLOG_Print("<Set OSD String>\n");
			break;

		/* Device OSD Transfer Feature */
		case CEC_OPCODE_GIVE_OSD_NAME:
			HxLOG_Print("<Give OSD Name>\n");
			break;

		case CEC_OPCODE_SET_OSD_NAME:
			HxLOG_Print("<Set OSD Name>\n");
			break;

		/* Device Menu Control Feature */
		case CEC_OPCODE_MENU_REQUEST:
			HxLOG_Print("<Menu Request>\n");
			break;

		case CEC_OPCODE_MENU_STATUS:
			HxLOG_Print("<Menu Status>\n");
			break;

		case CEC_OPCODE_USER_CONTROL_PRESSED:
			HxLOG_Print("<User Control Pressed>\n");
			break;

		case CEC_OPCODE_USER_CONTROL_RELEASED:
			HxLOG_Print("<User Control Released>\n");
			break;

		/* Power Status */
		case CEC_OPCODE_GIVE_POWER_STATUS:
			HxLOG_Print("<Give Power Status>\n");
			break;

		case CEC_OPCODE_REPORT_POWER_STATUS:
			HxLOG_Print("<Report Power Status>\n");
			break;

		/* General Protocol */
		case CEC_OPCODE_FEATURE_ABORT:
			HxLOG_Print("<Feature Abort>\n");
			break;

		case CEC_OPCODE_ABORT:
			HxLOG_Print("<Abort>\n");
			break;

		/* unknown */
		default:
			HxLOG_Print("<Unknown opcode>\n");
			break;
	}
#endif
}

/*---------------------------------------------------------------------------*/
#define ________CEC_LOCAL_FUCNTION________
/*---------------------------------------------------------------------------*/
STATIC void svc_cec_InitInfo(void)
{
	/* address 만 초기화 한다. 동작중에 호출 되므로 다른 데이터는 초기화 하지 말 것 */
	s_stCecInfo.usPhysicalAddr	= CEC_INVALID_PHYSICAL_ADDR;
	s_stCecInfo.ucLogicalAddr	= CEC_LA_UNREGISTERED;
	s_stCecInfo.bActiveSource	= FALSE;
}

STATIC void svc_cec_UpdateInfo(void)
{
	HERROR	hErr;
	HUINT16	usPhysicalAddr;
	HUINT8	ucLogicalAddr;

	/* get physical address */
	hErr = PAL_CEC_GetPhysicalAddress(&usPhysicalAddr);
	if (hErr != ERR_OK)
	{
		return;
	}

	/* get logical address */
	hErr = PAL_CEC_GetLogicalAddress(&ucLogicalAddr);
	if (hErr != ERR_OK)
	{
		return;
	}

	s_stCecInfo.usPhysicalAddr = usPhysicalAddr;
	s_stCecInfo.ucLogicalAddr = ucLogicalAddr;
}

/*---------------------------------------------------------------------------*/
#define ________CEC_SEND_MESSAGE________
/*---------------------------------------------------------------------------*/
STATIC HINT32 svc_cec_PostMsgToMgr(HINT32 nMsg, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	if (s_fnSvcCecPostMsgToMgr)
	{
		return (* s_fnSvcCecPostMsgToMgr)(nMsg, hAction, nParam1, nParam2, nParam3);
	}

	return ERR_OK;
}

/*---------------------------------------------------------------------------*/
#define ________CEC_SEND_ONE_TOUCH_PLAY_________
/*---------------------------------------------------------------------------*/
/* One Touch Play
 *
 * CEC_OPCODE_ACTIVE_SOURCE
 * CEC_OPCODE_IMAGE_VIEW_ON
 * CEC_OPCODE_TEXT_VIEW_ON
 */

/*******************************************************************************
* function: svc_cec_SendActiveSource
* description: Send <Acitve Source> message
*		1. Used by a new source to indicate that it has started to transmit a
*				stream OR used in response to a <Request Active Source>
*		2. Parameters	: [Physical Address]
*				[Physical Address]	: 2 byte
*		3. Response		: A current active source should take appropriate
*				action. TV should switch to the appropriate input.
*				Any CEC switches between source and root should switch to the
*				appropriate input and come out of standby if necessary.
*		4. Address		: Broadcast
*		5. Mandatory for Initiator	: All sources
*		6. Mandatory for Follower	: TV, CEC Switches
* type  : SYNC API
* input :
*		1. None
* output:
*		1. None
* return:
*		1. ERR_OK
*		2. ERR_FAIL
* return message
*		1. None
*******************************************************************************/
STATIC HERROR svc_cec_SendActiveSource(void)
{
	HERROR hErr;
	PalCec_Msg_t stTxMsg;

	HxLOG_Print("%s, %d : send <Acitve Source>\n", s_szfile, __LINE__);

	/* check physical address */
	if (s_stCecInfo.usPhysicalAddr == CEC_INVALID_PHYSICAL_ADDR)
	{
		HxLOG_Print("%s, %d : inbValid physical address\n", s_szfile, __LINE__);
		return ERR_FAIL;
	}

	/* send */
	HxSTD_memset(&stTxMsg, 0, sizeof(PalCec_Msg_t));

	/* set header */
	stTxMsg.initiator = s_stCecInfo.ucLogicalAddr;
	stTxMsg.destination = CEC_LA_BROADCAST;

	/* set opcode */
	stTxMsg.opcodeValid = TRUE;
	stTxMsg.opcode = CEC_OPCODE_ACTIVE_SOURCE;

	/* set data operand */
	stTxMsg.numOperand = 2;
	stTxMsg.operand[0] = (HINT8)(s_stCecInfo.usPhysicalAddr >> 8);
	stTxMsg.operand[1] = (HINT8)(s_stCecInfo.usPhysicalAddr & 0x00FF);

	/* send */
	hErr = PAL_CEC_SendCecMessage(&stTxMsg);
	if (hErr != ERR_OK)
	{
		HxLOG_Print("%s, %d : fail send <acitve source>\n", s_szfile, __LINE__);
	}

	return hErr;
}

/*******************************************************************************
* function: svc_cec_SendImageViewOn
* description: Send <Image View On> message
*		1. Sent by a source device to the TV whenever it enters the active state
*				(alternatively it may send <Text View On>).
*		2. Parameters	: None
*		3. Response		: Turn on (if not on). If in 'Text Display' state then
*				the TV enters 'Image Display' state. Note: Does not change TV
*				menu or PIP status.
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	: All sources shall implement at least one
*				of <Image View On> or <Text View On>
*		6. Mandatory for Follower	: TV
* type  : SYNC API
* input :
*		1. None
* output:
*		1. None
* return:
*		1. ERR_OK
*		2. ERR_FAIL
* return message
*		1. None
*******************************************************************************/
STATIC HERROR svc_cec_SendImageViewOn(void)
{
	HERROR		hErr;
	PalCec_Msg_t	stTxMsg;

	HxLOG_Print("%s, %d : send <Image View On>\n", s_szfile, __LINE__);

	HxSTD_memset(&stTxMsg, 0, sizeof(PalCec_Msg_t));

	/* set header */
	stTxMsg.initiator = s_stCecInfo.ucLogicalAddr;
	stTxMsg.destination = CEC_LA_TV;

	/* set opcode */
	stTxMsg.opcodeValid = TRUE;
	stTxMsg.opcode = CEC_OPCODE_IMAGE_VIEW_ON;

	/* set data operand */
	stTxMsg.numOperand = 0;

	/* send */
	hErr = PAL_CEC_SendCecMessage(&stTxMsg);
	if (hErr != ERR_OK)
	{
		HxLOG_Print("%s, %d : fail send <image view on>\n", s_szfile, __LINE__);
	}

	return hErr;
}

/*******************************************************************************
* function: svc_cec_SendTVPowerOff
* description: Send <Tv Power Off> message
*		1. Sent by a source device to the TV whenever it enters the standby mode
*				(alternatively it may send <Tv Power Off>).
*		2. Parameters	: None
*		3. Response		: Turn off
*		4. Address		: Directly addressed
* type  : SYNC API
* input :
*		1. None
* output:
*		1. None
* return:
*		1. ERR_OK
*		2. ERR_FAIL
* return message
*		1. None
*******************************************************************************/
STATIC HERROR svc_cec_SendTVPowerOff(void)
{
	HERROR		hErr;
	PalCec_Msg_t	stTxMsg;

	HxLOG_Print("%s, %d : send <Tv Power Off>\n", s_szfile, __LINE__);

	HxSTD_memset(&stTxMsg, 0, sizeof(PalCec_Msg_t));

	stTxMsg.initiator = 0;
	stTxMsg.destination = CEC_LA_TV;

	/* set opcode */
	stTxMsg.opcodeValid = TRUE;
	stTxMsg.opcode = CEC_OPCODE_STANDBY;

	/* set data operand */
	stTxMsg.numOperand = 0;

	/* send */
	hErr = PAL_CEC_SendCecMessage(&stTxMsg);
	if (hErr != ERR_OK)
	{
		HxLOG_Print("%s, %d : fail send <Tv Power Off>\n", s_szfile, __LINE__);
	}

	return hErr;
}

/*---------------------------------------------------------------------------*/
#define ________CEC_SEND_USER_CONTROL_PRESSED________
/*---------------------------------------------------------------------------*/
STATIC HERROR svc_cec_SendUserCtrlPressed(HUINT32 param)
{
	HERROR		hErr;
	PalCec_Msg_t	stTxMsg;

	HxLOG_Print("%s, %d : send <Image View On>\n", s_szfile, __LINE__);

	HxSTD_memset(&stTxMsg, 0, sizeof(PalCec_Msg_t));

	/* set header */
	stTxMsg.initiator = s_stCecInfo.ucLogicalAddr;
	stTxMsg.destination = CEC_LA_TV;

	/* set opcode */
	stTxMsg.opcodeValid = TRUE;
	stTxMsg.opcode = CEC_OPCODE_USER_CONTROL_PRESSED;

	/* set data operand */
    stTxMsg.operand[0] = param;
	stTxMsg.numOperand = 1;

	/* send */
	hErr = PAL_CEC_SendCecMessage(&stTxMsg);
	if (hErr != ERR_OK)
	{
		HxLOG_Print("%s, %d : fail send <User Contrl Pressed>\n", s_szfile, __LINE__);
	}

	return hErr;
}

/*---------------------------------------------------------------------------*/
#define ________CEC_SEND_ROUTING_CONTROL_________
/*---------------------------------------------------------------------------*/
/* Routing Control
 *
 * CEC_OPCODE_REQUEST_ACTIVE_SOURCE
 * CEC_OPCODE_ROUTING_CHANGE
 * CEC_OPCODE_ROUTING_INFORMATION
 * CEC_OPCODE_SET_STREAM_PATH
 */

/*---------------------------------------------------------------------------*/
#define ________CEC_SEND_STANDBY_________
/*---------------------------------------------------------------------------*/
/* Standby
 *
 * CEC_OPCODE_STANDBY
 */

/*******************************************************************************
* function: svc_cec_SendStandby
* description: Receive <Standby> message
*		1. Switches one or all devices into standby mode. Can be used as a
*				broadcast message or be addressed to a specific device. It is
*				recommended that a directly addressed message is only sent if
*				the initiator originally brought the device out of standby.
*		2. Parameters	: None
*		3. Response		: Switch the device into standby. Ignore the message if
*				already in standby.
*		4. Address		: Directly addressed, Broadcast
*		5. Mandatory for Initiator	:
*		6. Mandatory for Follower	: All
* type  : SYNC API
* input :
*		1. dest	- destination logical address
* output:
*		1. None
* return:
*		1. ERR_OK
*		2. ERR_FAIL
* return message
*		1. None
*******************************************************************************/
STATIC HERROR svc_cec_SendStandby(HUINT8 dest)
{
	HERROR		hErr;
	PalCec_Msg_t	stTxMsg;

	HxLOG_Print("%s, %d : send <Standby>\n", s_szfile, __LINE__);

	HxSTD_memset(&stTxMsg, 0, sizeof(PalCec_Msg_t));

	/* set header */
	stTxMsg.initiator = s_stCecInfo.ucLogicalAddr;
	stTxMsg.destination = dest;

	/* set opcode */
	stTxMsg.opcodeValid = TRUE;
	stTxMsg.opcode = CEC_OPCODE_STANDBY;

	/* set data operand */
	stTxMsg.numOperand = 0;

	/* send */
	hErr = PAL_CEC_SendCecMessage(&stTxMsg);

	return hErr;
}

/*---------------------------------------------------------------------------*/
#define ________CEC_SEND_ONE_TOUCH_RECORD_________
/*---------------------------------------------------------------------------*/
/* One Touch Record
 *
 * CEC_OPCODE_RECORD_OFF
 * CEC_OPCODE_RECORD_ON
 * CEC_OPCODE_RECORD_STATUS
 * CEC_OPCODE_RECORD_TV_SCREEN
 */

/*---------------------------------------------------------------------------*/
#define ________CEC_SEND_SYSTEM_INFORMATION_________
/*---------------------------------------------------------------------------*/
/* System Information
 *
 * CEC_OPCODE_GIVE_PHYSICAL_ADDRESS
 * CEC_OPCODE_GET_MENU_LANGUAGE
 * CEC_OPCODE_POLLING
 * CEC_OPCODE_REPORT_PHYSICAL_ADDRESS
 * CEC_OPCODE_SET_MENU_LANGUAGE
 */

/*******************************************************************************
* function: svc_cec_SendGivePhysicalAddress
* description: Send <Give Physical Address> message
*		1. A request to a device to return its physical address.
*		2. Parameters	: None
*		3. Response		: <Report Physical Address>
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	:
*		6. Mandatory for Follower	: All
* type  : SYNC API
* input :
*		1. dest	- destination logical address
* output:
*		1. None
* return:
*		1. ERR_OK
*		2. ERR_FAIL
* return message
*		1. None
*******************************************************************************/
STATIC HERROR svc_cec_SendGivePhysicalAddress(HUINT8 ucDest)
{
	HERROR		hErr;
	PalCec_Msg_t	stTxMsg;

	HxLOG_Print("%s, %d : send <Give Physical Address>\n", s_szfile, __LINE__);

	HxSTD_memset(&stTxMsg, 0, sizeof(PalCec_Msg_t));

	/* set header */
	stTxMsg.initiator = s_stCecInfo.ucLogicalAddr;
	stTxMsg.destination = ucDest;

	/* set opcode */
	stTxMsg.opcodeValid = TRUE;
	stTxMsg.opcode = CEC_OPCODE_GIVE_PHYSICAL_ADDRESS;

	/* set data operand */
	stTxMsg.numOperand = 0;

	/* send */
	hErr = PAL_CEC_SendCecMessage(&stTxMsg);

	return hErr;
}

/*******************************************************************************
* function: svc_cec_SendPolling
* description: Send <Polling> message
*		1. Used by any device for device discovery - similar to ping
*				in other protocols.
*		2. Parameters	: None
*		3. Response		: Shall set a low level ACK.
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	: All except for TV and CEC Switches
*		6. Mandatory for Follower	:
* type  : SYNC API
* input :
*		1. dest	- destination logical address
* output:
*		1. None
* return:
*		1. ERR_OK
*		2. ERR_FAIL
* return message
*		1. None
*******************************************************************************/
STATIC HERROR svc_cec_SendPolling(HUINT8 ucDest)
{
	HERROR		hErr;
	PalCec_Msg_t	stTxMsg;

	HxLOG_Print("%s, %d : send <Polling>\n", s_szfile, __LINE__);

	HxSTD_memset(&stTxMsg, 0, sizeof(PalCec_Msg_t));

	/* set header */
	stTxMsg.initiator = s_stCecInfo.ucLogicalAddr;
	stTxMsg.destination = ucDest;

	/* set opcode */
	stTxMsg.opcodeValid = FALSE;

	/* set data operand */
	stTxMsg.numOperand = 0;

	/* send */
	hErr = PAL_CEC_SendCecMessage(&stTxMsg);

	return hErr;
}

/*******************************************************************************
* function: svc_cec_SendReportPhysicalAddress
* description: Send <Report Physical Address> message
*		1. Used to inform all other devices of the mapping between physical and
*				logical address of the initiator.
*		2. Parameters	: [Physical Address] [Device Type]
*				[Physical Address]	: 2 byte
*				[Device Type]		: 1 byte
*		3. Response		:
*		4. Address		: Broadcast
*		5. Mandatory for Initiator	: All
*		6. Mandatory for Follower	: TV
* type  : SYNC API
* input :
*		1. None
* output:
*		1. None
* return:
*		1. ERR_OK
*		2. ERR_FAIL
* return message
*		1. None
*******************************************************************************/
STATIC HERROR svc_cec_SendReportPhysicalAddress(void)
{
	HERROR		hErr;
	PalCec_Msg_t	stTxMsg;

	HxLOG_Print("%s, %d : send <Report Physical Address>\n", s_szfile, __LINE__);

	/* get physical address */
	svc_cec_UpdateInfo();

	HxSTD_memset(&stTxMsg, 0, sizeof(PalCec_Msg_t));

	/* set header */
	stTxMsg.initiator = s_stCecInfo.ucLogicalAddr;
	stTxMsg.destination = CEC_LA_BROADCAST;

	/* set opcode */
	stTxMsg.opcodeValid = TRUE;
	stTxMsg.opcode = CEC_OPCODE_REPORT_PHYSICAL_ADDRESS;

	/* set data operand */
	stTxMsg.numOperand = 3;
	stTxMsg.operand[0] = (HINT8)(s_stCecInfo.usPhysicalAddr >> 8);
	stTxMsg.operand[1] = (HINT8)(s_stCecInfo.usPhysicalAddr & 0x00FF);
	stTxMsg.operand[2] = CEC_OPERAND_DEVICE_TYPE_STB;

	/* send */
	hErr = PAL_CEC_SendCecMessage(&stTxMsg);

	return hErr;
}

/*---------------------------------------------------------------------------*/
#define ________CEC_SEND_DECK_CONTROL_________
/*---------------------------------------------------------------------------*/
/* Deck Control Feature
 *
 * CEC_OPCODE_DECK_CONTROL
 * CEC_OPCODE_DECK_STATUS
 * CEC_OPCODE_GIVE_DECK_STATUS
 * CEC_OPCODE_DECK_PLAY
 */

/*---------------------------------------------------------------------------*/
#define ________CEC_SEND_TUNER_CONTROL_________
/*---------------------------------------------------------------------------*/
/* Tuner control
 *
 * CEC_OPCODE_GIVE_TUNER_DEVICE_STATUS
 * CEC_OPCODE_SELECT_DIGITAL_SERVICE
 * CEC_OPCODE_TUNER_DEVICE_STATUS
 * CEC_OPCODE_TUNER_STEP_DECREMENT
 * CEC_OPCODE_TUNER_STEP_INCREMENT
 */

/*---------------------------------------------------------------------------*/
#define ________CEC_SEND_VENDOR_SPECIFIC_COMMAND_________
/*---------------------------------------------------------------------------*/
/* Vendor Specific Commands Feature
 *
 * CEC_OPCODE_DEVICE_VENDOR_ID
 * CEC_OPCODE_GIVE_VENDOR_ID
 * CEC_OPCODE_VENDOR_COMMAND
 * CEC_OPCODE_VENDOR_REMOTE_BUTTON_DOWN
 * CEC_OPCODE_VENDOR_REMOTE_BUTTON_UP
 */

/* 파이오니아 요청사항
 *		PDP에서 <Give Device Vendor ID>, <Give OSD Name>에 대해서
 *		응답을 하지 않으면 에러 표시를 한다고 함.
 *		파이오니아 모델에만 적용한다.
 */

/*******************************************************************************
* function: svc_cec_SendDeviceVendorId
* description: Send <Device Vendor Id> message
*		1. Reports the vendor ID of this device.
*		2. Parameters	: [Vendor ID]
*				[Vendor ID]	: 3 byte
*		3. Response		:
*		4. Address		: Broadcast
*		5. Mandatory for Initiator	: As needed for Devices supporting Vendor
*				Specific Command.
*		6. Mandatory for Follower	: As needed for Devices supporting Vendor
*				Specific Command.
* type  : SYNC API
* input :
*		1. None
* output:
*		1. None
* return:
*		1. ERR_OK
*		2. ERR_FAIL
* return message
*		1. None
*******************************************************************************/
STATIC HERROR svc_cec_SendDeviceVendorId(void)
{
	HERROR		hErr;
	PalCec_Msg_t	stTxMsg;

	HxLOG_Print("%s, %d : send <Device Vendor Id>\n", s_szfile, __LINE__);

	HxSTD_memset(&stTxMsg, 0, sizeof(PalCec_Msg_t));

	/* set header */
	stTxMsg.initiator = s_stCecInfo.ucLogicalAddr;
	stTxMsg.destination = CEC_LA_BROADCAST;

	/* set opcode */
	stTxMsg.opcodeValid = TRUE;
	stTxMsg.opcode = CEC_OPCODE_DEVICE_VENDOR_ID;

	/* set data operand */
	stTxMsg.numOperand = 3;

	stTxMsg.operand[0] = (HUINT8)((CEC_VENDOR_ID_HUMAX & 0xFF0000) >> 16);
	stTxMsg.operand[1] = (HUINT8)((CEC_VENDOR_ID_HUMAX & 0x00FF00) >> 8);
	stTxMsg.operand[2] = (HUINT8)(CEC_VENDOR_ID_HUMAX & 0x0000FF);

	/* send */
	hErr = PAL_CEC_SendCecMessage(&stTxMsg);

	return hErr;
}

/*---------------------------------------------------------------------------*/
#define ________CEC_SEND_OSD_STATUS_DISPLAY_________
/*---------------------------------------------------------------------------*/
/* OSD Status Display Feature
 *
 * CEC_OPCODE_SET_OSD_STRING
 */

/*---------------------------------------------------------------------------*/
#define ________CEC_SEND_DEVICE_OSD_TRANSFER_________
/*---------------------------------------------------------------------------*/
/* Device OSD Transfer Feature
 *
 * CEC_OPCODE_GIVE_OSD_NAME
 * CEC_OPCODE_SET_OSD_NAME
 */

/*******************************************************************************
* function: svc_cec_SendSetOsdName
* description: Send <Set OSD Name> message
*		1. Used to set the preferred OSD name of a device for use in menus
*				associated with that device.
*		2. Parameters	: [OSD Name]
*				[Vendor ID]	: 1 - 8 byte
*		3. Response		:
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	:
*		6. Mandatory for Follower	:
* type  : SYNC API
* input :
*		1. None
* output:
*		1. None
* return:
*		1. ERR_OK
*		2. ERR_FAIL
* return message
*		1. None
*******************************************************************************/
STATIC HERROR svc_cec_SendSetOsdName(HUINT8 dest)
{
	HERROR		hErr;
	PalCec_Msg_t	stTxMsg;
	HUINT8		szOsdName[CEC_MAX_OPERAND_LEN];
	HUINT8		ucOsdNameLen;
	int			i;

	HxLOG_Print("%s, %d : send <Set OSD Name>\n", s_szfile, __LINE__);

#if 0
	hErr = PAL_SYS_GetModelName(szOsdName, FALSE);
	if (hErr != ERR_OK)
	{
		return ERR_FAIL;
	}
#else
	/* fix prevent #155473 (Buffer not null terminated) */
	HxSTD_StrNCpy(szOsdName, s_stCecInfo.szModelName, CEC_MAX_OPERAND_LEN - 1);
#endif

	HxSTD_memset(&stTxMsg, 0, sizeof(PalCec_Msg_t));

	/* set header */
	stTxMsg.initiator = s_stCecInfo.ucLogicalAddr;
	stTxMsg.destination = dest;

	/* set opcode */
	stTxMsg.opcodeValid = TRUE;
	stTxMsg.opcode = CEC_OPCODE_SET_OSD_NAME;

	/* set data operand */
	ucOsdNameLen = VK_strlen((char*)szOsdName);
	if (ucOsdNameLen > CEC_MAX_OPERAND_LEN)
	{
		ucOsdNameLen = CEC_MAX_OPERAND_LEN;
	}

	stTxMsg.numOperand = ucOsdNameLen;
	for (i=0; i<ucOsdNameLen; i++)
	{
		stTxMsg.operand[i] = szOsdName[i];
	}

	/* send */
	hErr = PAL_CEC_SendCecMessage(&stTxMsg);

	return hErr;
}

/*---------------------------------------------------------------------------*/
#define ________CEC_SEND_DEVICE_MENU_CONTROL_________
/*---------------------------------------------------------------------------*/
/* Device Menu Control Feature
 *
 * CEC_OPCODE_MENU_REQUEST
 * CEC_OPCODE_MENU_STATUS
 * CEC_OPCODE_USER_CONTROL_PRESSED
 * CEC_OPCODE_USER_CONTROL_RELEASED
 */

/*---------------------------------------------------------------------------*/
#define ________CEC_SEND_POWER_STATUS_________
/*---------------------------------------------------------------------------*/
/* Power Status
 *
 * CEC_OPCODE_GIVE_POWER_STATUS
 * CEC_OPCODE_REPORT_POWER_STATUS
 */

/*******************************************************************************
* function: svc_cec_SendGivePowerStatus
* description: Send <Give Power Status> message
*		1. Used to determine the current power status of a target device
*		2. Parameters	: None
*		3. Response		: <Report Power Status>
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	:
*		6. Mandatory for Follower	:
* type  : SYNC API
* input :
*		1. None
* output:
*		1. None
* return:
*		1. ERR_OK
*		2. ERR_FAIL
* return message
*		1. None
*******************************************************************************/
STATIC HERROR svc_cec_SendGivePowerStatus(HUINT8 dest)
{
	HERROR		hErr;
	PalCec_Msg_t	stTxMsg;

	HxLOG_Print("%s, %d : send <Give Power Status>\n", s_szfile, __LINE__);

	HxSTD_memset(&stTxMsg, 0, sizeof(PalCec_Msg_t));

	/* set header */
	stTxMsg.initiator = s_stCecInfo.ucLogicalAddr;
	stTxMsg.destination = dest;

	/* set opcode */
	stTxMsg.opcodeValid = TRUE;
	stTxMsg.opcode = CEC_OPCODE_GIVE_POWER_STATUS;

	/* set data operand */
	stTxMsg.numOperand = 0;

	/* send */
	hErr = PAL_CEC_SendCecMessage(&stTxMsg);

	return hErr;
}

/*******************************************************************************
* function: svc_cec_SendReportPowerStatus
* description: Send <Report Power Status> message
*		1. Used to inform a requesting device of the current power status
*		2. Parameters	: [Power Status]
*				[Power Status]	: 1 byte
*		3. Response		:
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	:
*		6. Mandatory for Follower	:
* type  : SYNC API
* input :
*		1. None
* output:
*		1. None
* return:
*		1. ERR_OK
*		2. ERR_FAIL
* return message
*		1. None
*******************************************************************************/
STATIC HERROR svc_cec_SendReportPowerStatus(HUINT8 dest)
{
	HERROR		hErr;
	PalCec_Msg_t	stTxMsg;

	HxLOG_Print("%s, %d : send <Report Power Status>\n", s_szfile, __LINE__);

	HxSTD_memset(&stTxMsg, 0, sizeof(PalCec_Msg_t));

	/* set header */
	stTxMsg.initiator = s_stCecInfo.ucLogicalAddr;
	stTxMsg.destination = dest;

	/* set opcode */
	stTxMsg.opcodeValid = TRUE;
	stTxMsg.opcode = CEC_OPCODE_REPORT_POWER_STATUS;

	/* set data operand */
	stTxMsg.numOperand = 1;

	if (s_stCecInfo.bPwrState == TRUE)
	{
		stTxMsg.operand[0] = CEC_OPERAND_POWER_STATE_ON;
	}
	else
	{
		stTxMsg.operand[0] = CEC_OPERAND_POWER_STATE_STANDBY;
	}

	/* send */
	hErr = PAL_CEC_SendCecMessage(&stTxMsg);

	return hErr;
}

/*---------------------------------------------------------------------------*/
#define ________CEC_SEND_GENERAL_PROTOCOL_________
/*---------------------------------------------------------------------------*/
/* General Protocol
 *
 * CEC_OPCODE_FEATURE_ABORT
 * CEC_OPCODE_ABORT
 */

/*******************************************************************************
* function: svc_cec_SendFeatureAbort
* description: Send <Feature Abort> message
*		1. Used as a response to indicate that the device does not support the
*				requested message type, or that it cannot execute it at the
*				present time.
*		2. Parameters	: [Feature Opcode] [Abort Reason]
*				[Feature Opcode]	: 1 byte
*				[Abort Reason]		: 1 byte
*		3. Response		: Assume that request is not supported or has not been
*				actioned.
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	: Generate if a message is not supported
*		6. Mandatory for Follower	: All
* type  : SYNC API
* input :
*		1. dest		: destination
*		2. opcode	: [Feature Opcode]
*		3. reason	: [Abort Reason]
* output:
*		1. None
* return:
*		1. ERR_OK
*		2. ERR_FAIL
* return message
*		1. None
*******************************************************************************/
STATIC HERROR svc_cec_SendFeatureAbort(HUINT8 dest, HUINT8 opcode, HUINT8 abortReason)
{
	HERROR		hErr;
	PalCec_Msg_t	stTxMsg;

	HxLOG_Print("%s, %d : send <Feature Abort>\n", s_szfile, __LINE__);

	HxSTD_memset(&stTxMsg, 0, sizeof(PalCec_Msg_t));

	/* set header */
	stTxMsg.initiator = s_stCecInfo.ucLogicalAddr;
	stTxMsg.destination = dest;

	/* set opcode */
	stTxMsg.opcodeValid = TRUE;
	stTxMsg.opcode = CEC_OPCODE_FEATURE_ABORT;

	/* set data operand */
	stTxMsg.numOperand = 2;
	stTxMsg.operand[0] = opcode;
	stTxMsg.operand[1] = abortReason;

	/* send */
	hErr = PAL_CEC_SendCecMessage(&stTxMsg);

	return hErr;
}

/*******************************************************************************
* function: svc_cec_SendAbort
* description: Send <Abort> message
*		1. This message is reserved for testing purposes.
*		2. Parameters	: None
*		3. Response		: A device shall never support this message, and shall
*				always respond with a <Feature Abort> message containing any
*				bValid value for [Abort Reason]. CEC switches shall not respond
*				to this message.
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	:
*		6. Mandatory for Follower	: All, except for CEC switches
* type  : SYNC API
* input :
*		1. dest		: destination
* output:
*		1. None
* return:
*		1. ERR_OK
*		2. ERR_FAIL
* return message
*		1. None
*******************************************************************************/
STATIC HERROR svc_cec_SendAbort(HUINT8 ucDest)
{
	HERROR		hErr;
	PalCec_Msg_t	stTxMsg;

	HxLOG_Print("%s, %d : send <Abort>\n", s_szfile, __LINE__);

	HxSTD_memset(&stTxMsg, 0, sizeof(PalCec_Msg_t));

	/* set header */
	stTxMsg.initiator = s_stCecInfo.ucLogicalAddr;
	stTxMsg.destination = ucDest;

	/* set opcode */
	stTxMsg.opcodeValid = TRUE;
	stTxMsg.opcode = CEC_OPCODE_ABORT;

	/* set data operand */
	stTxMsg.numOperand = 0;

	/* send */
	hErr = PAL_CEC_SendCecMessage(&stTxMsg);

	return hErr;
}

/*---------------------------------------------------------------------------*/
#define ________CEC_RCV_MESSAGE________
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
#define ________CEC_RCV_ONE_TOUCH_PLAY_________
/*---------------------------------------------------------------------------*/
/* One Touch Play
 *
 * CEC_OPCODE_ACTIVE_SOURCE
 * CEC_OPCODE_IMAGE_VIEW_ON
 * CEC_OPCODE_TEXT_VIEW_ON
 */

/*******************************************************************************
* function: svc_cec_RcvActiveSource
* description: Receive <Active Source> message
*		1. Used by a new source to indicate that it has started to transmit a
*				stream OR used in response to a <Request Active Source>
*		2. Parameters	: [Physical Address]
*				[Physical Address]	: 2 byte
*		3. Response		: A current active source should take appropriate
*				action. TV should switch to the appropriate input.
*				Any CEC switches between source and root should switch to the
*				appropriate input and come out of standby if necessary.
*		4. Address		: Broadcast
*		5. Mandatory for Initiator	: All sources
*		6. Mandatory for Follower	: TV, CEC Switches
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvActiveSource(PalCec_Msg_t *pstRxMsg)
{
	/* check inbValid destination address */
	if (pstRxMsg->destination != CEC_LA_BROADCAST)
	{
		HxLOG_Print("%s, %d : inbValid dest, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* check inbValid operand size */
	if (pstRxMsg->numOperand != 2)
	{
		HxLOG_Print("%s, %d : inbValid operand, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* CECT 11.2.1-2
	 * DUT ignores an <Active Source> message from logical address 15.
	 */
	if (pstRxMsg->initiator == CEC_LA_UNREGISTERED)
	{
		HxLOG_Print("%s, %d : inbValid initiator, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* disable active source */
	HxLOG_Print("%s, %d : disable active source\n", s_szfile, __LINE__);
	s_stCecInfo.bActiveSource = FALSE;

exit:
	return;
}

/*******************************************************************************
* function: svc_cec_RcvImageViewOn
* description: Receive <Image View On> message
*		1. Sent by a source device to the TV whenever it enters the active state
*				(alternatively it may send <Text View On>).
*		2. Parameters	: None
*		3. Response		: Turn on (if not on). If in 'Text Display' state then
*				the TV enters 'Image Display' state. Note: Does not change TV
*				menu or PIP status.
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	: All sources shall implement at least one
*				of <Image View On> or <Text View On>
*		6. Mandatory for Follower	: TV
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvImageViewOn(PalCec_Msg_t *pstRxMsg)
{
	HxLOG_Print("%s, %d : ignore\n", s_szfile, __LINE__);
}

/*******************************************************************************
* function: svc_cec_RcvTextViewOn
* description: Receive <Text View On> message
*		1. As <Image View On>, but also removes any text, menus and PIP windows
*				from the TV's display.
*		2. Parameters	: None
*		3. Response		: As <Image View On>, but removes PIPs and menus from
*				the screen. The TV enters 'Image Display' state regardless of
*				its previous state.
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	: All sources shall implement at least one
*				of <Image View On> or <Text View On>
*		6. Mandatory for Follower	: TV
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvTextViewOn(PalCec_Msg_t *pstRxMsg)
{
	HxLOG_Print("%s, %d : ignore\n", s_szfile, __LINE__);
}

/*---------------------------------------------------------------------------*/
#define ________CEC_RCV_ROUTING_CONTROL_________
/*---------------------------------------------------------------------------*/
/* Routing Control
 *
 * CEC_OPCODE_REQUEST_ACTIVE_SOURCE
 * CEC_OPCODE_ROUTING_CHANGE
 * CEC_OPCODE_ROUTING_INFORMATION
 * CEC_OPCODE_SET_STREAM_PATH
 */

/*******************************************************************************
* function: svc_cec_RcvRequestActiveSource
* description: Receive <Request Acitve Source> message
*		1. Used by a new device to discover the status of the system.
*		2. Parameters	: None
*		3. Response		: <Active Source> from the present active source.
*		4. Address		: Broadcast
*		5. Mandatory for Initiator	:
*		6. Mandatory for Follower	: All except CEC Switches
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvRequestActiveSource(PalCec_Msg_t *pstRxMsg)
{
	/* check inbValid destination address */
	if (pstRxMsg->destination != CEC_LA_BROADCAST)
	{
		HxLOG_Print("%s, %d : inbValid dest, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* check inbValid operand size */
	if (pstRxMsg->numOperand != 0)
	{
		HxLOG_Print("%s, %d : inbValid operand, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* check current active source? */
	if (s_stCecInfo.bActiveSource == TRUE)
	{
		/* send <Active Source> */
		svc_cec_SendActiveSource();
	}
	else
	{
		/* ignore */
		HxLOG_Print("%s, %d : not acitve source, ignore\n", s_szfile, __LINE__);
	}

exit:
	return;
}

/*******************************************************************************
* function: svc_cec_RcvRoutingChange
* description: Receive <Routing Change> message
*		1. Sent by a CEC Switch when it is manually switched to inform all other
*				devices on the network that the active route below the switch
*				has changed.
*		2. Parameters	: [Original Address] [New Address]
*				[Original Address]	: 2 byte
*				[New Address]		: 2 byte
*		3. Response		: If a CEC Switch is at the new address, it sends a
*				<Routing Information> message to indicate its current active
*				route.
*		4. Address		: Broadcast
*		5. Mandatory for Initiator	: CEC Switches
*		6. Mandatory for Follower	: CEC Switches
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvRoutingChange(PalCec_Msg_t *pstRxMsg)
{
	HxLOG_Print("%s, %d : ignore\n", s_szfile, __LINE__);
}

/*******************************************************************************
* function: svc_cec_RcvRoutingInformation
* description: Receive <Routing Information> message
*		1. Sent by a CEC Switch to indicate the active route below the switch.
*		2. Parameters	: [Physical Address]
*				[Physical Address]	: 2 byte
*		3. Response		: If a CEC Switch is at the specified address it should
*				relay a <Routing Information> message to indicate its current
*				active path.
*		4. Address		: Broadcast
*		5. Mandatory for Initiator	: CEC Switches
*		6. Mandatory for Follower	: CEC Switches
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvRoutingInformation(PalCec_Msg_t *pstRxMsg)
{
	HxLOG_Print("%s, %d : ignore\n", s_szfile, __LINE__);
}

/*******************************************************************************
* function: svc_cec_RcvSetStreamPath
* description: Receive <Set Stream Path> message
*		1. Used by the TV to request a streaming path from the specified
*				physical address.
*		2. Parameters	: [Physical Address]
*				[Physical Address]	: 2 byte
*		3. Response		: Any CEC switches between should switch inputs
*				according to the path defined in [Physical Address]. The device
*				at the new address should stream its output and broadcast an
				<Active Source> message.
*		4. Address		: Broadcast
*		5. Mandatory for Initiator	:
*		6. Mandatory for Follower	: CEC Switches
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvSetStreamPath(PalCec_Msg_t *pstRxMsg)
{
	HUINT16	usPhysicalAddr;

	/* check inbValid destination address */
	if (pstRxMsg->destination != CEC_LA_BROADCAST)
	{
		HxLOG_Print("%s, %d : inbValid dest, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* check inbValid operand size */
	if (pstRxMsg->numOperand != 2)
	{
		HxLOG_Print("%s, %d : inbValid operand, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	usPhysicalAddr = (HINT16)pstRxMsg->operand[0] << 8;
	usPhysicalAddr |= (HINT16)pstRxMsg->operand[1];
	/* check physical address */
	if (s_stCecInfo.usPhysicalAddr == usPhysicalAddr)
	{
		/* set active source */
		s_stCecInfo.bActiveSource = TRUE;

		/* send <Active Source> */
		svc_cec_SendActiveSource();
	}
	else
	{
		/* ignore */
		HxLOG_Print("%s, %d : not my physical address, ignore\n", s_szfile, __LINE__);
	}

exit:
	return;
}

/*---------------------------------------------------------------------------*/
#define ________CEC_RCV_STANDBY_________
/*---------------------------------------------------------------------------*/
/* Standby
 *
 * CEC_OPCODE_STANDBY
 */

/*******************************************************************************
* function: svc_cec_RcvStandby
* description: Receive <Standby> message
*		1. Switches one or all devices into standby mode. Can be used as a
*				broadcast message or be addressed to a specific device. It is
*				recommended that a directly addressed message is only sent if
*				the initiator originally brought the device out of standby.
*		2. Parameters	: None
*		3. Response		: Switch the device into standby. Ignore the message if
*				already in standby.
*		4. Address		: Directly addressed, Broadcast
*		5. Mandatory for Initiator	:
*		6. Mandatory for Follower	: All
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvStandby(PalCec_Msg_t *pstRxMsg)
{
	HBOOL	bEnable = FALSE;
	HERROR	hErr;

	/* check inbValid operand size */
	HxLOG_Print("pstRxMsg->numOperand %d\n", pstRxMsg->numOperand);
	if (pstRxMsg->numOperand != 0)
	{
		HxLOG_Print("%s, %d : inbValid dest, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* 전원 연동 option 확인 */
//	bEnable = MWC_PARAM_GetHdmiCecEnable();
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_HDMI_CEC_ENABLE, (HUINT32 *)&bEnable, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_HDMI_CEC_ENABLE) Error!!!\n");
	}

	if (bEnable == FALSE)		/* CEC OFF */
	{
		HxLOG_Print("%s, %d : CEC off state\n", s_szfile, __LINE__);
		return;
	}
	svc_cec_PostMsgToMgr(eSEVT_MW_CEC_POWER_OFF, PAL_PIPE_GetActionHandle(eActionId_VIEW_0), 0, 0, 0);
exit:
	return;
}

/*---------------------------------------------------------------------------*/
#define ________CEC_RCV_ONE_TOUCH_RECORD_________
/*---------------------------------------------------------------------------*/
/* One Touch Record
 *
 * CEC_OPCODE_RECORD_OFF
 * CEC_OPCODE_RECORD_ON
 * CEC_OPCODE_RECORD_STATUS
 * CEC_OPCODE_RECORD_TV_SCREEN
 */

/*******************************************************************************
* function: svc_cec_RcvRecordOff
* description: Receive <Record Off> message
*		1. Requests a device to stop a recording.
*		2. Parameters	: None
*		3. Response		: Exit 'Recording' state and stop recording if the
*				initiator is the same as the initiator of the <Record On>
*				message which started this recording.
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	:
*		6. Mandatory for Follower	: Recording Device
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvRecordOff(PalCec_Msg_t *pstRxMsg)
{
	/* check inbValid destination address */
	if (pstRxMsg->destination == CEC_LA_BROADCAST)
	{
		HxLOG_Print("%s, %d : inbValid dest, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* check inbValid operand size */
	if (pstRxMsg->numOperand != 2)
	{
		HxLOG_Print("%s, %d : inbValid operand, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* send <Feature Abort> */
	svc_cec_SendFeatureAbort(pstRxMsg->initiator, pstRxMsg->opcode, CEC_OPERAND_UNRECOGNIZED_OPCODE);

exit:
	return;
}

/*******************************************************************************
* function: svc_cec_RcvRecordOn
* description: Receive <Record On> message
*		1. Attempt to record the specified source.
*		2. Parameters	: [Record Source]
*				[Record Source] :
*					[Record Source Type] : 1 byte
*					[Record Source Type] [Digital Service Identification] : 8(1+7) byte
*		3. Response		: Enter 'Recording' state and start recording if
*				possible. Send the initiator <Record Status>.
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	:
*		6. Mandatory for Follower	: Recording Device, Own Source only
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvRecordOn(PalCec_Msg_t *pstRxMsg)
{
	/* check inbValid destination address */
	if (pstRxMsg->destination == CEC_LA_BROADCAST)
	{
		HxLOG_Print("%s, %d : inbValid dest, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* check inbValid operand size */
	if (pstRxMsg->numOperand == 1)
	{
		if (pstRxMsg->operand[0] != CEC_OPERAND_RECORD_OWN_SOURCE)
		{
			HxLOG_Print("%s, %d : inbValid operand, ignore\n", s_szfile, __LINE__);
			goto exit;
		}
	}
	else if (pstRxMsg->numOperand == 8)
	{
		if (pstRxMsg->operand[0] != CEC_OPERAND_RECORD_DIGITAL_SERVICE)
		{
			HxLOG_Print("%s, %d : inbValid operand, ignore\n", s_szfile, __LINE__);
			goto exit;
		}
	}
	else
	{
		HxLOG_Print("%s, %d : inbValid operand, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* send <Feature Abort> */
	svc_cec_SendFeatureAbort(pstRxMsg->initiator, pstRxMsg->opcode, CEC_OPERAND_UNRECOGNIZED_OPCODE);

exit:
	return;
}

/*******************************************************************************
* function: svc_cec_RcvRecordStatus
* description: Receive <Record Status> message
*		1. Used by a recording device to inform the initiator of the message
*				<Record On> about its status.
*		2. Parameters	: [Record Status Info]
*				[Record Status Info] : 1 byte
*		3. Response		: Enter 'Recording' state and start recording if
*				possible. Send the initiator <Record Status>.
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	: Recording Device
*		6. Mandatory for Follower	: Device Initiating a recording
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvRecordStatus(PalCec_Msg_t *pstRxMsg)
{
	/* check inbValid destination address */
	if (pstRxMsg->destination == CEC_LA_BROADCAST)
	{
		HxLOG_Print("%s, %d : inbValid dest, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* check inbValid operand size */
	if (pstRxMsg->numOperand != 1)
	{
		HxLOG_Print("%s, %d : inbValid operand, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* send <Feature Abort> */
	svc_cec_SendFeatureAbort(pstRxMsg->initiator, pstRxMsg->opcode, CEC_OPERAND_UNRECOGNIZED_OPCODE);

exit:
	return;
}

/*******************************************************************************
* function: svc_cec_RcvRecordTvScreen
* description: Receive <Record Tv Screen> message
*		1. Request by the recording device to record the presently displayed
*				source.
*		2. Parameters	: None
*		3. Response		: Initiate a recording using the <Record On> message,
*				or send a <Feature Abort> ["Cannot provide source"] if the
*				presently displayed source is not recordable.
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	:
*		6. Mandatory for Follower	:
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvRecordTvScreen(PalCec_Msg_t *pstRxMsg)
{
	/* check inbValid destination address */
	if (pstRxMsg->destination == CEC_LA_BROADCAST)
	{
		HxLOG_Print("%s, %d : inbValid dest, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* check inbValid operand size */
	if (pstRxMsg->numOperand != 0)
	{
		HxLOG_Print("%s, %d : inbValid operand, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* send <Feature Abort> */
	svc_cec_SendFeatureAbort(pstRxMsg->initiator, pstRxMsg->opcode, CEC_OPERAND_UNRECOGNIZED_OPCODE);

exit:
	return;
}

/*---------------------------------------------------------------------------*/
#define ________CEC_RCV_SYSTEM_INFORMATION_________
/*---------------------------------------------------------------------------*/
/* System Information
 *
 * CEC_OPCODE_GIVE_PHYSICAL_ADDRESS
 * CEC_OPCODE_GET_MENU_LANGUAGE
 * CEC_OPCODE_POLLING
 * CEC_OPCODE_REPORT_PHYSICAL_ADDRESS
 * CEC_OPCODE_SET_MENU_LANGUAGE
 */

/*******************************************************************************
* function: svc_cec_RcvGivePhysicalAddress
* description: Receive <Give Physical Address> message
*		1. A request to a device to return its physical address.
*		2. Parameters	: None
*		3. Response		: <Report Physical Address>
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	:
*		6. Mandatory for Follower	: All
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvGivePhysicalAddress(PalCec_Msg_t *pstRxMsg)
{
	/* check inbValid destination address */
	if (pstRxMsg->destination == CEC_LA_BROADCAST)
	{
		HxLOG_Print("%s, %d : inbValid dest, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* check inbValid operand size */
	if (pstRxMsg->numOperand != 0)
	{
		HxLOG_Print("%s, %d : inbValid operand, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* send <Report Physical Address */
	svc_cec_SendReportPhysicalAddress();

exit:
	return;
}

/*******************************************************************************
* function: svc_cec_RcvGetMenuLanguage
* description: Receive <Get Menu Language> message
*		1. Sent by a device capable of character generation (for OSD and Menus)
*				to a TV in order to discover the currently selected Menu
*				language. Also used by a TV during installation to discover the
*				currently set menu language.
*		2. Parameters	: None
*		3. Response		: The addressed device responds with a
*				<Set Menu Language> message
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	:
*		6. Mandatory for Follower	: TV with OSD / Menu generation capabilities
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvGetMenuLanguage(PalCec_Msg_t *pstRxMsg)
{
	/* check inbValid destination address */
	if (pstRxMsg->destination == CEC_LA_BROADCAST)
	{
		HxLOG_Print("%s, %d : inbValid dest, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* check inbValid operand size */
	if (pstRxMsg->numOperand != 0)
	{
		HxLOG_Print("%s, %d : inbValid operand, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* send <Feature Abort> */
	svc_cec_SendFeatureAbort(pstRxMsg->initiator, pstRxMsg->opcode, CEC_OPERAND_UNRECOGNIZED_OPCODE);

exit:
	return;
}

/*******************************************************************************
* function: svc_cec_RcvReportPhysicalAddress
* description: Send <Report Physical Address> message
*		1. Used to inform all other devices of the mapping between physical and
*				logical address of the initiator.
*		2. Parameters	: [Physical Address] [Device Type]
*				[Physical Address]	: 2 byte
*				[Device Type]		: 1 byte
*		3. Response		:
*		4. Address		: Broadcast
*		5. Mandatory for Initiator	: All
*		6. Mandatory for Follower	: TV
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvReportPhysicalAddress(PalCec_Msg_t *pstRxMsg)
{
	HxLOG_Print("%s, %d : ignore\n", s_szfile, __LINE__);
}

/*******************************************************************************
* function: svc_cec_RcvSetMenuLanguage
* description: Receive <Set Menu Language> message
*		1. Used by a TV or anther device to indicate the menu language.
*		2. Parameters	: [Language]
*				[Language]	: 3 byte
*		3. Response		: Set the menu language as specified, if possible.
*		4. Address		: Broadcast
*		5. Mandatory for Initiator	: TV
*		6. Mandatory for Follower	: All, except TV and CEC Switches
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvSetMenuLanguage(PalCec_Msg_t *pstRxMsg)
{
	HxLOG_Print("%s, %d : ignore\n", s_szfile, __LINE__);
}

/*---------------------------------------------------------------------------*/
#define ________CEC_RCV_DECK_CONTROL_________
/*---------------------------------------------------------------------------*/
/* Deck Control Feature
 *
 * CEC_OPCODE_DECK_CONTROL
 * CEC_OPCODE_DECK_STATUS
 * CEC_OPCODE_GIVE_DECK_STATUS
 * CEC_OPCODE_DECK_PLAY
 */

/*******************************************************************************
* function: svc_cec_RcvDeckControl
* description: Receive <Deck Control> message
*		1. Used to control a device's media functions.
*		2. Parameters	: [Deck Control Mode]
*				[Deck Control Mode] : 1 byte
*		3. Response		: Perform the specified actions, or return a
*				<Feature Abort> message. It is device dependent whether or not
*				a Skip Fwd/Skip Back (Wind/Rewind) command is legal when in the
*				'Deck Inactive' state. If the device is in standby and receives
*				an eject command, it should power on and eject its media.
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	:
*		6. Mandatory for Follower	: <Deck Control> ["Stop"] for a playback
*				device
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvDeckControl(PalCec_Msg_t *pstRxMsg)
{
	/* check inbValid destination address */
	if (pstRxMsg->destination == CEC_LA_BROADCAST)
	{
		HxLOG_Print("%s, %d : inbValid dest, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* check inbValid operand size */
	if (pstRxMsg->numOperand != 1)
	{
		HxLOG_Print("%s, %d : inbValid operand, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* send <Feature Abort> */
	svc_cec_SendFeatureAbort(pstRxMsg->initiator, pstRxMsg->opcode, CEC_OPERAND_UNRECOGNIZED_OPCODE);

exit:
	return;
}

/*******************************************************************************
* function: svc_cec_RcvDeckStatus
* description: Receive <Deck Status> message
*		1. Used to provide a deck's status to the initiator of the
*				<Give Deck Status> message.
*		2. Parameters	: [Deck Info]
*				[Deck Info] : 1 byte
*		3. Response		:
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	:
*		6. Mandatory for Follower	:
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvDeckStatus(PalCec_Msg_t *pstRxMsg)
{
	HxLOG_Print("%s, %d : ignore\n", s_szfile, __LINE__);
}

/*******************************************************************************
* function: svc_cec_RcvGiveDeckStatus
* description: Receive <Give Deck Status> message
*		1. Used to request the status of a device, regardless of whether or not
*				it is the current active source.
*		2. Parameters	: [Status Request]
*				[Status Request] : 1 byte
*		3. Response		: <Deck Status>
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	:
*		6. Mandatory for Follower	:
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvGiveDeckStatus(PalCec_Msg_t *pstRxMsg)
{
	/* check inbValid destination address */
	if (pstRxMsg->destination == CEC_LA_BROADCAST)
	{
		HxLOG_Print("%s, %d : inbValid dest, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* check inbValid operand size */
	if (pstRxMsg->numOperand != 1)
	{
		HxLOG_Print("%s, %d : inbValid operand, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* send <Feature Abort> */
	svc_cec_SendFeatureAbort(pstRxMsg->initiator, pstRxMsg->opcode, CEC_OPERAND_UNRECOGNIZED_OPCODE);

exit:
	return;
}

/*******************************************************************************
* function: svc_cec_RcvDeckPlay
* description: Receive <Deck Play> message
*		1. Used to control the playback behaviour of a source device.
*		2. Parameters	: [Play Mode]
*				[Play Mode]	: 1 byte
*		3. Response		: Perform the specified actions, or return a
*				<Feature Abort> message. If media is available the device enters
*				'Deck Active' state. If the device is in standby, has media
*				available and the parameter is ["Play Forward"] it should power
*				on.
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	:
*		6. Mandatory for Follower	: <Play> ["Forward"] for a playback device
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvDeckPlay(PalCec_Msg_t *pstRxMsg)
{
	/* check inbValid destination address */
	if (pstRxMsg->destination == CEC_LA_BROADCAST)
	{
		HxLOG_Print("%s, %d : inbValid dest, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* check inbValid operand size */
	if (pstRxMsg->numOperand != 1)
	{
		HxLOG_Print("%s, %d : inbValid operand, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* send <Feature Abort> */
	svc_cec_SendFeatureAbort(pstRxMsg->initiator, pstRxMsg->opcode, CEC_OPERAND_UNRECOGNIZED_OPCODE);

exit:
	return;
}

/*---------------------------------------------------------------------------*/
#define ________CEC_RCV_TUNER_CONTROL_________
/*---------------------------------------------------------------------------*/
/* Tuner control
 *
 * CEC_OPCODE_GIVE_TUNER_DEVICE_STATUS
 * CEC_OPCODE_SELECT_DIGITAL_SERVICE
 * CEC_OPCODE_TUNER_DEVICE_STATUS
 * CEC_OPCODE_TUNER_STEP_DECREMENT
 * CEC_OPCODE_TUNER_STEP_INCREMENT
 */

/*******************************************************************************
* function: svc_cec_RcvGiveTunerDeviceStatus
* description: Receive <Give Tuner Device Status> message
*		1. Used to request the status of a tuner device.
*		2. Parameters	: [Status Request]
*				[Status Request] : 1 byte
*		3. Response		: Respond with a <Tuner Device Status> message, or stop
*				reporting changes on receipt of the ["Off"] message.
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	:
*		6. Mandatory for Follower	:
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvGiveTunerDeviceStatus(PalCec_Msg_t *pstRxMsg)
{
	/* check inbValid destination address */
	if (pstRxMsg->destination == CEC_LA_BROADCAST)
	{
		HxLOG_Print("%s, %d : inbValid dest, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* check inbValid operand size */
	if (pstRxMsg->numOperand != 1)
	{
		HxLOG_Print("%s, %d : inbValid operand, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* send <Feature Abort> */
	svc_cec_SendFeatureAbort(pstRxMsg->initiator, pstRxMsg->opcode, CEC_OPERAND_UNRECOGNIZED_OPCODE);

exit:
	return;
}

/*******************************************************************************
* function: svc_cec_RcvSelectDigitalService
* description: Receive <Select Digital Service> message
*		1. Directly selects a Digital TV, Radio or Data Broadcast Service
*		2. Parameters	: [Digital Service Identification]
*				[Digital Service Identification] : 7 byte
*		3. Response		: Change to the selected digital service and stream its
*				output on the HDMI connection. If the tuner device is not
*				capable of selecting this service, respond with a <Feature Abort>
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	:
*		6. Mandatory for Follower	:
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvSelectDigitalService(PalCec_Msg_t *pstRxMsg)
{
	/* check inbValid destination address */
	if (pstRxMsg->destination == CEC_LA_BROADCAST)
	{
		HxLOG_Print("%s, %d : inbValid dest, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* check inbValid operand size */
	if (pstRxMsg->numOperand != 7)
	{
		HxLOG_Print("%s, %d : inbValid operand, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* check initiator logical address
	 * CECT 11.2.7 - 2 : Ensure that the DUT ignores a <Select Preset>
	 * message coming from the unregistered logical address (15).
	 */
	if (pstRxMsg->initiator == CEC_LA_UNREGISTERED)
	{
		HxLOG_Print("%s, %d : inbValid initiator, ingore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* ignore */
	HxLOG_Print("%s, %d : ignore\n", s_szfile, __LINE__);

exit:
	return;
}

/*******************************************************************************
* function: svc_cec_RcvTunerDeviceStatus
* description: Receive <Tuner Device Status> message
*		1. Use by a tuner device to provide its status to the initiator of the
*				<Give Tuner Device Status> message.
*		2. Parameters	: [Tuner Device Info]
*				[Tuner Device Info] : 8 byte
*		3. Response		:
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	:
*		6. Mandatory for Follower	:
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvTunerDeviceStatus(PalCec_Msg_t *pstRxMsg)
{
	HxLOG_Print("%s, %d : ignore\n", s_szfile, __LINE__);
}

/*******************************************************************************
* function: svc_cec_RcvTunerStepDecrement
* description: Receive <Tuner Step Decrement> message
*		1. Used to tune to next lowest service in a tuner’s service list. Can
*				be used for PIP.
*		2. Parameters	: None
*		3. Response		: Follower tunes to next lowest service in its service
*				list.
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	: TV without Tuner and with Program
*				Inc./ Dec. facility
*		6. Mandatory for Follower	: Tuner Device
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvTunerStepDecrement(PalCec_Msg_t *pstRxMsg)
{
	/* check inbValid destination address */
	if (pstRxMsg->destination == CEC_LA_BROADCAST)
	{
		HxLOG_Print("%s, %d : inbValid dest, ingnore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* check inbValid operand size */
	if (pstRxMsg->numOperand != 0)
	{
		HxLOG_Print("%s, %d : inbValid operand, ingnore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* ignore */
	HxLOG_Print("%s, %d : ignore\n", s_szfile, __LINE__);

exit:
	return;
}

/*******************************************************************************
* function: svc_cec_RcvTunerStepIncrement
* description: Receive <Tuner Step Increment> message
*		1. Used to tune to next highest service in a tuner’s service list. Can
*				be used for PIP.
*		2. Parameters	: None
*		3. Response		: Follower tunes to next highest service in its service
*				list.
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	: TV without Tuner and with Program
*				Inc./ Dec. facility
*		6. Mandatory for Follower	: Tuner Device
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvTunerStepIncrement(PalCec_Msg_t *pstRxMsg)
{
	/* check inbValid destination address */
	if (pstRxMsg->destination == CEC_LA_BROADCAST)
	{
		HxLOG_Print("%s, %d : inbValid dest, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* check inbValid operand size */
	if (pstRxMsg->numOperand != 0)
	{
		HxLOG_Print("%s, %d : inbValid operand, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* ignore */
	HxLOG_Print("%s, %d : ignore\n", s_szfile, __LINE__);

exit:
	return;
}

/*---------------------------------------------------------------------------*/
#define ________CEC_RCV_VENDOR_SPECIFIC_COMMAND_________
/*---------------------------------------------------------------------------*/
/* Vendor Specific Commands Feature
 *
 * CEC_OPCODE_DEVICE_VENDOR_ID
 * CEC_OPCODE_GIVE_VENDOR_ID
 * CEC_OPCODE_VENDOR_COMMAND
 * CEC_OPCODE_VENDOR_REMOTE_BUTTON_DOWN
 * CEC_OPCODE_VENDOR_REMOTE_BUTTON_UP
 */

/*******************************************************************************
* function: svc_cec_RcvDeviceVendorId
* description: Receive <Device Vendor ID> message
*		1. Reports the vendor ID of this device.
*		2. Parameters	: [Vendor ID]
*				[Vendor ID] : 3 byte
*		3. Response		: Any other interested device may store the vendor ID
*				of the device.
*		4. Address		: Broadcast
*		5. Mandatory for Initiator	: As needed for Devices supporting Vendor
*				Specific Command
*		6. Mandatory for Follower	: As needed for Devices supporting Vendor
*				Specific Command
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvDeviceVendorId(PalCec_Msg_t *pstRxMsg)
{
	HxLOG_Print("%s, %d : ignore\n", s_szfile, __LINE__);
}

/*******************************************************************************
* function: svc_cec_RcvGiveVendorId
* description: Receive <Give Vendor ID> message
*		1. Requests the Vendor ID from a device.
*		2. Parameters	: None
*		3. Response		: <Device Vendor ID>
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	: As needed for Devices supporting Vendor
*				Specific Command
*		6. Mandatory for Follower	: As needed for Devices supporting Vendor
*				Specific Command
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvGiveVendorId(PalCec_Msg_t *pstRxMsg)
{
	/* check inbValid destination address */
	if (pstRxMsg->destination == CEC_LA_BROADCAST)
	{
		HxLOG_Print("%s, %d : inbValid dest, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* check inbValid operand size */
	if (pstRxMsg->numOperand != 0)
	{
		HxLOG_Print("%s, %d : inbValid operand, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* send <Divice Vendor ID> */
	svc_cec_SendDeviceVendorId();

exit:
	return;
}

/*******************************************************************************
* function: svc_cec_RcvVendorCommand
* description: Receive <Vendor Command> message
*		1. Allows vendor specific commands to be sent between two devices.
*		2. Parameters	: [Vendor Specific Data]
*				[Vendor Specific Data] : shallnot exceed 14 data blocks
*		3. Response		: Vendor Specific
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	:
*		6. Mandatory for Follower	:
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvVendorCommand(PalCec_Msg_t *pstRxMsg)
{
	/* check inbValid destination address */
	if (pstRxMsg->destination == CEC_LA_BROADCAST)
	{
		HxLOG_Print("%s, %d : inbValid dest, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* check inbValid operand size */
	if (pstRxMsg->numOperand > 14)
	{
		HxLOG_Print("%s, %d : inbValid operand, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* send <Feature Abort> */
	svc_cec_SendFeatureAbort(pstRxMsg->initiator, pstRxMsg->opcode, CEC_OPERAND_UNRECOGNIZED_OPCODE);

exit:
	return;
}

/*******************************************************************************
* function: svc_cec_RcvVendorRemoteButtonDown
* description: Receive <Vendor REmote Button Down> message
*		1. Indicates that a remote control button has been depressed.
*		2. Parameters	: [Vendor Specific RC Code]
*		3. Response		: Vendor Specific
*		4. Address		: Directly addressed, Broadcast
*		5. Mandatory for Initiator	:
*		6. Mandatory for Follower	:
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvVendorRemoteButtonDown(PalCec_Msg_t *pstRxMsg)
{
	/* check inbValid destination address */
	if (pstRxMsg->destination == CEC_LA_BROADCAST)
	{
		HxLOG_Print("%s, %d : inbValid dest, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* send <Feature Abort> */
	svc_cec_SendFeatureAbort(pstRxMsg->initiator, pstRxMsg->opcode, CEC_OPERAND_UNRECOGNIZED_OPCODE);

exit:
	return;
}

/*******************************************************************************
* function: svc_cec_RcvVendorRemoteButtonUp
* description: Receive <Vendor REmote Button Up> message
*		1. Indicates that a remote control button (the last button pressed
*				indicated by the Vendor Remote Button Down message) has been
*				released.
*		2. Parameters	: None
*		3. Response		: Vendor Specific
*		4. Address		: Directly addressed, Broadcast
*		5. Mandatory for Initiator	:
*		6. Mandatory for Follower	:
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvVendorRemoteButtonUp(PalCec_Msg_t *pstRxMsg)
{
	/* check inbValid destination address */
	if (pstRxMsg->destination == CEC_LA_BROADCAST)
	{
		HxLOG_Print("%s, %d : inbValid dest, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* check inbValid operand size */
	if (pstRxMsg->numOperand != 0)
	{
		HxLOG_Print("%s, %d : inbValid operand, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* send <Feature Abort> */
	svc_cec_SendFeatureAbort(pstRxMsg->initiator, pstRxMsg->opcode, CEC_OPERAND_UNRECOGNIZED_OPCODE);

exit:
	return;
}

/*---------------------------------------------------------------------------*/
#define ________CEC_RCV_OSD_STATUS_DISPLAY_________
/*---------------------------------------------------------------------------*/
/* OSD Status Display Feature
 *
 * CEC_OPCODE_SET_OSD_STRING
 */

/*******************************************************************************
* function: svc_cec_RcvSetOsdString
* description: Receive <Set OSD String> message
*		1. Used to send a text message to output on a TV.
*		2. Parameters	: [Display Control] [OSD String]
*				[Display Control]	: 1 byte
*				[OSD String]		: 1-13 byte
*		3. Response		: TV displays the message.
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	:
*		6. Mandatory for Follower	:
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvSetOsdString(PalCec_Msg_t *pstRxMsg)
{
	/* check inbValid destination address */
	if (pstRxMsg->destination == CEC_LA_BROADCAST)
	{
		HxLOG_Print("%s, %d : inbValid dest, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* check inbValid operand size */
	if (pstRxMsg->numOperand > 14)
	{
		HxLOG_Print("%s, %d : inbValid operand, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* send <Feature Abort> */
	svc_cec_SendFeatureAbort(pstRxMsg->initiator, pstRxMsg->opcode, CEC_OPERAND_UNRECOGNIZED_OPCODE);

exit:
	return;
}

/*---------------------------------------------------------------------------*/
#define ________CEC_RCV_DEVICE_OSD_TRANSFER_________
/*---------------------------------------------------------------------------*/
/* Device OSD Transfer Feature
 *
 * CEC_OPCODE_GIVE_OSD_NAME
 * CEC_OPCODE_SET_OSD_NAME
 */

/*******************************************************************************
* function: svc_cec_RcvGiveOsdName
* description: Receive <Give OSD Name> message
*		1. Used to request the preferred OSD name of a device for use in menus
*				associated with that device.
*		2. Parameters	: None
*		3. Response		: <Set OSD Name>
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	:
*		6. Mandatory for Follower	:
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvGiveOsdName(PalCec_Msg_t *pstRxMsg)
{
	/* check inbValid destination address */
	if (pstRxMsg->destination == CEC_LA_BROADCAST)
	{
		HxLOG_Print("%s, %d : inbValid dest, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* check inbValid operand size */
	if (pstRxMsg->numOperand != 0)
	{
		HxLOG_Print("%s, %d : inbValid operand, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* send <Set OSD Name> */
	svc_cec_SendSetOsdName(pstRxMsg->initiator);

exit:
	return;
}

/*******************************************************************************
* function: svc_cec_RcvSetOsdName
* description: Receive <Set OSD Name> message
*		1. Used to set the preferred OSD name of a device for use in menus
*				associated with that device.
*		2. Parameters	: [OSD Name]
*				[OSD Name]	: 1-8 byte
*		3. Response		: Store the name and use it in any menus associated
*				with that device.
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	:
*		6. Mandatory for Follower	:
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvSetOsdName(PalCec_Msg_t *pstRxMsg)
{
	/* check inbValid destination address */
	if (pstRxMsg->destination == CEC_LA_BROADCAST)
	{
		HxLOG_Print("%s, %d : inbValid dest, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* check inbValid operand size */
	if (pstRxMsg->numOperand > 8)
	{
		HxLOG_Print("%s, %d : inbValid operand, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* send <Feature Abort> */
	svc_cec_SendFeatureAbort(pstRxMsg->initiator, pstRxMsg->opcode, CEC_OPERAND_UNRECOGNIZED_OPCODE);

exit:
	return;
}

/*---------------------------------------------------------------------------*/
#define ________CEC_RCV_DEVICE_MENU_CONTROL_________
/*---------------------------------------------------------------------------*/
/* Device Menu Control Feature
 *
 * CEC_OPCODE_MENU_REQUEST
 * CEC_OPCODE_MENU_STATUS
 * CEC_OPCODE_USER_CONTROL_PRESSED
 * CEC_OPCODE_USER_CONTROL_RELEASED
 */

/*******************************************************************************
* function: svc_cec_RcvMenuRequest
* description: Receive <Menu Request> message
*		1. A request from the TV for a device to show/remove a menu or to query
*				if a device is currently showing a menu.
*		2. Parameters	: [Menu Request Type]
*				[Menu Request Type]	: 1 byte
*		3. Response		: May enter or exit the 'Device Menu Active' state if
*				the parameter was "Activate" or "Deactivate" Send <Menu Status>
*				to indicate the current status of the devices menu.
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	:
*		6. Mandatory for Follower	:
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvMenuRequest(PalCec_Msg_t *pstRxMsg)
{
	/* check inbValid destination address */
	if (pstRxMsg->destination == CEC_LA_BROADCAST)
	{
		HxLOG_Print("%s, %d : inbValid dest, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* check inbValid operand size */
	if (pstRxMsg->numOperand != 1)
	{
		HxLOG_Print("%s, %d : inbValid operand, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* send <Feature Abort> */
	svc_cec_SendFeatureAbort(pstRxMsg->initiator, pstRxMsg->opcode, CEC_OPERAND_UNRECOGNIZED_OPCODE);

exit:
	return;
}

/*******************************************************************************
* function: svc_cec_RcvMenuStatus
* description: Receive <Menu Status> message
*		1. Used to indicate to the TV that the device is showing/has removed
*				a menu and requests the remote control keys to be passed though.
*		2. Parameters	: [Menu State]
*				[Menu State]	: 1 byte
*		3. Response		: If Menu State indicates activated, TV enters 'Device
*				Menu Active' state and forwards those Remote control commands,
*				shown in CEC Table 23, to the initiator. If deactivated, TV
*				enters 'Device Menu Inactive' state and stops forwarding remote
*				control commands.
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	:
*		6. Mandatory for Follower	:
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvMenuStatus(PalCec_Msg_t *pstRxMsg)
{
	/* check inbValid destination address */
	if (pstRxMsg->destination == CEC_LA_BROADCAST)
	{
		HxLOG_Print("%s, %d : inbValid dest, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* check inbValid operand size */
	if (pstRxMsg->numOperand != 1)
	{
		HxLOG_Print("%s, %d : inbValid operand, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* send <Feature Abort> */
	svc_cec_SendFeatureAbort(pstRxMsg->initiator, pstRxMsg->opcode, CEC_OPERAND_UNRECOGNIZED_OPCODE);

exit:
	return;
}

/*******************************************************************************
* function: svc_cec_RcvUserControlPressed
* description: Receive <User Control Pressed> message
*		1. Used to indicate that the user pressed a remote control button or
*				switched from one remote control button to another.
*		2. Parameters	: [UI Command]
*				[UI Command] : 1 byte
*		3. Response		: Update display or perform an action, as required.
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	:
*		6. Mandatory for Follower	:
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvUserControlPressed(PalCec_Msg_t *pstRxMsg)
{
	HUINT32 ulKeySymbol = DIKI_KEYDEF_END;

	/* check inbValid destination address */
	if (pstRxMsg->destination == CEC_LA_BROADCAST)
	{
		HxLOG_Print("%s, %d : inbValid dest, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* check inbValid operand size */
	if (pstRxMsg->numOperand != 1)
	{
		HxLOG_Print("%s, %d : inbValid operand, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	HxLOG_Print("%s, %d : <RCU> operan=0x%x\n", s_szfile, __LINE__, pstRxMsg->operand[0]);

	switch (pstRxMsg->operand[0])
	{
		case CEC_OPERAND_RCU_0:	ulKeySymbol = DIKS_0;	break;
		case CEC_OPERAND_RCU_1:	ulKeySymbol = DIKS_1;	break;
		case CEC_OPERAND_RCU_2:	ulKeySymbol = DIKS_2;	break;
		case CEC_OPERAND_RCU_3:	ulKeySymbol = DIKS_3;	break;
		case CEC_OPERAND_RCU_4:	ulKeySymbol = DIKS_4;	break;
		case CEC_OPERAND_RCU_5:	ulKeySymbol = DIKS_5;	break;
		case CEC_OPERAND_RCU_6:	ulKeySymbol = DIKS_6;	break;
		case CEC_OPERAND_RCU_7:	ulKeySymbol = DIKS_7;	break;
		case CEC_OPERAND_RCU_8:	ulKeySymbol = DIKS_8;	break;
		case CEC_OPERAND_RCU_9:	ulKeySymbol = DIKS_9;	break;
		case CEC_OPERAND_RCU_11:	ulKeySymbol = DIKS_CUSTOM10;	break;
		case CEC_OPERAND_RCU_12:	ulKeySymbol = DIKS_CUSTOM11;	break;
		case CEC_OPERAND_RCU_RED:	ulKeySymbol = DIKS_RED;	break;
		case CEC_OPERAND_RCU_GREEN:	ulKeySymbol = DIKS_GREEN;	break;
		case CEC_OPERAND_RCU_YELLOW:	ulKeySymbol = DIKS_YELLOW;	break;
		case CEC_OPERAND_RCU_BLUE:	ulKeySymbol = DIKS_BLUE;	break;
		case CEC_OPERAND_RCU_ROOT_MENU:	ulKeySymbol = DIKS_MENU;	break;
		case CEC_OPERAND_RCU_UP:	ulKeySymbol = DIKS_CURSOR_UP;	break;
		case CEC_OPERAND_RCU_DOWN:	ulKeySymbol = DIKS_CURSOR_DOWN;	break;
		case CEC_OPERAND_RCU_LEFT:	ulKeySymbol = DIKS_CURSOR_LEFT;	break;
		case CEC_OPERAND_RCU_RIGHT:	ulKeySymbol = DIKS_CURSOR_RIGHT;	break;
		case CEC_OPERAND_RCU_EXIT:	ulKeySymbol = DIKS_BACK;	break;
		case CEC_OPERAND_RCU_SELECT:	ulKeySymbol = DIKS_RETURN;	break;
		case CEC_OPERAND_RCU_EPG:	ulKeySymbol = DIKS_EPG;	break;
		case CEC_OPERAND_RCU_CHANNEL_UP:	ulKeySymbol = DIKS_CHANNEL_UP;	break;
		case CEC_OPERAND_RCU_CHANNEL_DOWN:	ulKeySymbol = DIKS_CHANNEL_DOWN;	break;
		case CEC_OPERAND_RCU_DATA:	ulKeySymbol = DIKS_CUSTOM61;	break;
		default:	break;
	}

	if (ulKeySymbol != DIKI_KEYDEF_END)
	{
		HAPI_SendRemoteRCUKey(ulKeySymbol);
	}

exit:
	return;
}

/*******************************************************************************
* function: svc_cec_RcvUserControlReleased
* description: Receive <User Control Released> message
*		1. Indicates that user released a remote control button (the last one
*				indicated by the <User Control Pressed> message)
*		2. Parameters	: None
*		3. Response		: Update display or perform an action, as required.
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	:
*		6. Mandatory for Follower	:
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvUserControlReleased(PalCec_Msg_t *pstRxMsg)
{
	/* check inbValid destination address */
	if (pstRxMsg->destination == CEC_LA_BROADCAST)
	{
		HxLOG_Print("%s, %d : inbValid dest, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* check inbValid operand size */
	if (pstRxMsg->numOperand != 0)
	{
		HxLOG_Print("%s, %d : inbValid operand, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* send <Feature Abort> */
	svc_cec_SendFeatureAbort(pstRxMsg->initiator, pstRxMsg->opcode, CEC_OPERAND_UNRECOGNIZED_OPCODE);

exit:
	return;
}

/*---------------------------------------------------------------------------*/
#define ________CEC_RCV_POWER_STATUS_________
/*---------------------------------------------------------------------------*/
/* Power Status
 *
 * CEC_OPCODE_GIVE_POWER_STATUS
 * CEC_OPCODE_REPORT_POWER_STATUS
 */

/*******************************************************************************
* function: svc_cec_RcvGivePowerStatus
* description: Receive <Give Power Status> message
*		1. Used to determine the current power status of a target device
*		2. Parameters	: None
*		3. Response		: <Report Power Status>
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	:
*		6. Mandatory for Follower	:
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvGivePowerStatus(PalCec_Msg_t *pstRxMsg)
{
	/* check inbValid destination address */
	if (pstRxMsg->destination == CEC_LA_BROADCAST)
	{
		HxLOG_Print("%s, %d : inbValid dest, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* check inbValid operand size */
	if (pstRxMsg->numOperand != 0)
	{
		HxLOG_Print("%s, %d : inbValid operand, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* send <Report Power Status */
	svc_cec_SendReportPowerStatus(pstRxMsg->initiator);

exit:
	return;
}

/*******************************************************************************
* function: svc_cec_RcvReportPowerStatus
* description: Receive <Report Power Status> message
*		1. Used to inform a requesting device of the current power status
*		2. Parameters	: [Power Status]
*			[Power Status]	: 1 byte
*		3. Response		: <Report Power Status>
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	:
*		6. Mandatory for Follower	:
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvReportPowerStatus(PalCec_Msg_t *pstRxMsg)
{
	HUINT8	pwrStatus;

	if ((pstRxMsg->initiator == CEC_LA_TV) && (pstRxMsg->numOperand == 1))
	{
		pwrStatus = pstRxMsg->operand[0];

		switch (pwrStatus)
		{
			case CEC_OPERAND_POWER_STATE_ON:
				s_eCecTvPwrStatus = eCEC_TV_POWER_ON;
				break;

			case CEC_OPERAND_POWER_STATE_STANDBY:
				s_eCecTvPwrStatus = eCEC_TV_POWER_STANDBY;
				break;

			case CEC_OPERAND_POWER_STATE_TO_ON:
				s_eCecTvPwrStatus = eCEC_TV_POWER_TO_ON;
				break;

			case CEC_OPERAND_POWER_STATE_TO_STANDBY:
				s_eCecTvPwrStatus = eCEC_TV_POWER_TO_STANDBY;
				break;

			default:
				s_eCecTvPwrStatus = eCEC_TV_POWER_UNKNOWN;
				break;
		}

		HxLOG_Print("%s() : pwrStatus=%d : tvPowerStatus=%d\n", __func__, pwrStatus, s_eCecTvPwrStatus);
	}
}

/*---------------------------------------------------------------------------*/
#define ________CEC_RCV_GENERAL_PROTOCOL_________
/*---------------------------------------------------------------------------*/
/* General Protocol
 *
 * CEC_OPCODE_FEATURE_ABORT
 * CEC_OPCODE_ABORT
 */

/*******************************************************************************
* function: svc_cec_RcvFeatureAbort
* description: Receive <Feature Abort> message
*		1. Used as a response to indicate that the device does not support the
*				requested message type, or that it cannot execute it at the
*				present time.
*		2. Parameters	: [Feature Opcode] [Abort Reason]
*				[Feature Opcode]	: 1 byte
*				[Abort Reason]		: 1 byte
*		3. Response		: Assume that request is not supported or has not been
*				actioned.
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	: Generate if a message is not supported
*		6. Mandatory for Follower	: All
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvFeatureAbort(PalCec_Msg_t *pstRxMsg)
{
	HxLOG_Print("%s, %d : ignore\n", s_szfile, __LINE__);
}

/*******************************************************************************
* function: svc_cec_RcvAbort
* description: Receive <Abort> message
*		1. This message is reserved for testing purposes.
*		2. Parameters	: None
*		3. Response		: A device shall never support this message, and shall
*				always respond with a <Feature Abort> message containing any
*				bValid value for [Abort Reason]. CEC switches shall not respond
*				to this message.
*		4. Address		: Directly addressed
*		5. Mandatory for Initiator	:
*		6. Mandatory for Follower	: All, except for CEC switches
* type  : SYNC API
* input :
*		1. pstRxMsg	- received message
* output:
*		1. None
* return:
*		1. None
* return message
*		1. None
*******************************************************************************/
STATIC void svc_cec_RcvAbort(PalCec_Msg_t *pstRxMsg)
{
	/* check inbValid destination address */
	if (pstRxMsg->destination == CEC_LA_BROADCAST)
	{
		HxLOG_Print("%s, %d : inbValid dest, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* check inbValid operand size */
	if (pstRxMsg->numOperand != 0)
	{
		HxLOG_Print("%s, %d : inbValid operand, ignore\n", s_szfile, __LINE__);
		goto exit;
	}

	/* send <Feature Abort> */
	svc_cec_SendFeatureAbort(pstRxMsg->initiator, pstRxMsg->opcode, CEC_OPERAND_UNRECOGNIZED_OPCODE);

exit:
	return;
}

/*---------------------------------------------------------------------------*/
#define ________CEC_TASK_________
/*---------------------------------------------------------------------------*/

STATIC void svc_cec_RcvSendImageViewOn(HUINT32 ulPwrSyncCnt)
{
	HERROR				hErr;
	svcCec_TaskMsg_t	stTaskMsg;
	HBOOL				bValidLA = TRUE;
	HBOOL				bSendImageViewOn = TRUE;

	/* check STB power */
	if (ulPwrSyncCnt != s_stCecInfo.ulPwrSyncCnt)
	{
		HxLOG_Print("%s, %d : Power changed\n", s_szfile, __LINE__);
		return;
	}

	if (s_stCecInfo.bPwrState == FALSE)
	{
		HxLOG_Print("%s, %d : Power changed to off\n", s_szfile, __LINE__);
		return;
	}

	/* update LA and Physical address */
	svc_cec_UpdateInfo();

	HxSTD_memset((void*)(&stTaskMsg), 0, sizeof(svcCec_TaskMsg_t));

	/* check tv power */
	switch (s_eCecTvPwrStatus)
	{
		case eCEC_TV_POWER_ON:
			/* send <Active Source> */
			bSendImageViewOn = FALSE;

			/* delay */
			VK_TASK_Sleep(CEC_RETRY_DELAY_ACITVE_SOURCE);

			stTaskMsg.eEvent = eCEC_TASK_MSG_SEND_ACTIVE_SOURCE;
			stTaskMsg.aulArg[0] = ulPwrSyncCnt;
			VK_MSG_Send(s_ulCecMsgQId, (void*)(&stTaskMsg), sizeof(svcCec_TaskMsg_t));
			return;

		case eCEC_TV_POWER_TO_ON:
			/* wait power state */
			bSendImageViewOn = FALSE;

			/* delay */
			VK_TASK_Sleep(CEC_RETRY_DELAY_WAIT_TV_ON);

			/* send <active source> */
			stTaskMsg.eEvent = eCEC_TASK_MSG_WAIT_TV_ON;
			stTaskMsg.aulArg[0] = ulPwrSyncCnt;
			VK_MSG_Send(s_ulCecMsgQId, (void*)(&stTaskMsg), sizeof(svcCec_TaskMsg_t));
			break;

		case eCEC_TV_POWER_UNKNOWN:
		case eCEC_TV_POWER_STANDBY:
		case eCEC_TV_POWER_TO_STANDBY:
		default:
			/* send <Image View On> */
			bSendImageViewOn = TRUE;
			break;
	}

	/* check condition */
	if (bSendImageViewOn == FALSE)
	{
		return;
	}

	/* increase cnt */
	s_ulCecSndImageViewOnCnt++;

	HxLOG_Print("%s(), <Image View On> cnt = %d\n", __func__, s_ulCecSndImageViewOnCnt);

	/* check timeout */
	if (s_ulCecSndImageViewOnCnt > CEC_MAX_SND_ERR_IMAGE_VIEW_ON)
	{
		/* CEC 대응 기기가 아닌것 같다. 그만하자. */
		HxLOG_Print("%s(), <Image View On> cnt = %d\n", __func__, s_ulCecSndImageViewOnCnt);
		return;
	}

	/* check Logical address */
	if (s_stCecInfo.ucLogicalAddr == CEC_LA_UNREGISTERED)
	{
		HxLOG_Print("%s, %d : STB1 임시 사용\n", s_szfile, __LINE__);

		bValidLA = FALSE;

		/* set STB LA for temp */
		s_stCecInfo.ucLogicalAddr = CEC_LA_STB1;
	}

	/* send image view on */
	hErr = svc_cec_SendImageViewOn();

	/* restore */
	if (bValidLA == FALSE)
	{
		s_stCecInfo.ucLogicalAddr = CEC_LA_UNREGISTERED;
	}

	/* check result */
	if (hErr != ERR_OK)
	{
		/* arbitration 실패 때만 재시도 한다 */
		VK_TASK_Sleep(CEC_RETRY_DELAY_IMAGE_VIEW_ON);

		/* retry <image view on> */
		stTaskMsg.eEvent = eCEC_TASK_MSG_SEND_IMAGE_VIEW_ON;
		stTaskMsg.aulArg[0] = ulPwrSyncCnt;
		VK_MSG_Send(s_ulCecMsgQId, (void*)(&stTaskMsg), sizeof(svcCec_TaskMsg_t));
	}
	else
	{
		/* delay */
		VK_TASK_Sleep(CEC_RETRY_DELAY_WAIT_TV_ON);

		/* send wait tv on */
		stTaskMsg.eEvent = eCEC_TASK_MSG_WAIT_TV_ON;
		stTaskMsg.aulArg[0] = ulPwrSyncCnt;
		VK_MSG_Send(s_ulCecMsgQId, (void*)(&stTaskMsg), sizeof(svcCec_TaskMsg_t));
	}
}

STATIC void svc_cec_RcvWaitTvOn(HUINT32 ulPwrSyncCnt)
{
	svcCec_TaskMsg_t	stTaskMsg;
	HBOOL				bRetry = FALSE;

	/* check STB power */
	if (ulPwrSyncCnt != s_stCecInfo.ulPwrSyncCnt)
	{
		HxLOG_Print("%s, %d : Power changed\n", s_szfile, __LINE__);
		return;
	}

	if (s_stCecInfo.bPwrState == FALSE)
	{
		HxLOG_Print("%s, %d : Power changed to off\n", s_szfile, __LINE__);
		return;
	}

	/* update LA and Physical address */
	svc_cec_UpdateInfo();

	/* increase cnt */
	s_ulCecWaitTvOnCnt++;

	HxLOG_Print("%s(), wait TV on cnt = %d\n", __func__, s_ulCecWaitTvOnCnt);

	/* check timeout */
	if (s_ulCecWaitTvOnCnt > CEC_MAX_ERR_WAIT_TV_ON)
	{
		/* CEC 대응 기기가 아닌것 같다. 그만하자. */
		HxLOG_Print("%s(), wait TV on cnt = %d\n", __func__, s_ulCecWaitTvOnCnt);
		return;
	}

	HxSTD_memset((void*)(&stTaskMsg), 0, sizeof(svcCec_TaskMsg_t));

	/* check LA */
	if (s_stCecInfo.ucLogicalAddr == CEC_LA_UNREGISTERED)
	{
		HxLOG_Print("%s, %d : LA=15, retry\n", s_szfile, __LINE__);
		bRetry = TRUE;
	}
	else if (s_stCecInfo.usPhysicalAddr == CEC_INVALID_PHYSICAL_ADDR)
	{
		HxLOG_Print("%s, %d : physical address=F.F.F.F, retry\n", s_szfile, __LINE__);
		bRetry = TRUE;
	}
	else
	{
		/* update tv status */
		svc_cec_SendGivePowerStatus(CEC_LA_TV);

		switch (s_eCecTvPwrStatus)
		{
			case eCEC_TV_POWER_ON:
				/* send <Active Source> */
				/* delay 0.5sec */
				VK_TASK_Sleep(CEC_RETRY_DELAY_ACITVE_SOURCE);

				stTaskMsg.eEvent = eCEC_TASK_MSG_SEND_ACTIVE_SOURCE;
				stTaskMsg.aulArg[0] = ulPwrSyncCnt;
				VK_MSG_Send(s_ulCecMsgQId, (void*)(&stTaskMsg), sizeof(svcCec_TaskMsg_t));
				break;

			case eCEC_TV_POWER_STANDBY:
				/* send <Image View On> again */
				VK_TASK_Sleep(CEC_RETRY_DELAY_IMAGE_VIEW_ON);

				stTaskMsg.eEvent  = eCEC_TASK_MSG_SEND_IMAGE_VIEW_ON;
				stTaskMsg.aulArg[0] = ulPwrSyncCnt;
				VK_MSG_Send(s_ulCecMsgQId, (void*)(&stTaskMsg), sizeof(svcCec_TaskMsg_t));
				break;

			case eCEC_TV_POWER_TO_ON:
				/* send <Active Source> : TV 입력 절환 속도 */
				svc_cec_SendActiveSource();

				/* check again */
				bRetry = TRUE;
				break;

			case eCEC_TV_POWER_UNKNOWN:
			case eCEC_TV_POWER_TO_STANDBY:
			default:
				/* check again */
				bRetry = TRUE;
				break;
		}
	}

	/* check retry */
	if (bRetry == TRUE)
	{
		/* delay */
		VK_TASK_Sleep(CEC_RETRY_DELAY_WAIT_TV_ON);

		/* wait again */
		stTaskMsg.eEvent  = eCEC_TASK_MSG_WAIT_TV_ON;
		stTaskMsg.aulArg[0] = ulPwrSyncCnt;
		VK_MSG_Send(s_ulCecMsgQId, (void*)(&stTaskMsg), sizeof(svcCec_TaskMsg_t));
	}
}

STATIC void svc_cec_RcvTVPowerOff(void)
{
	HERROR err;
	svcCec_TaskMsg_t stTaskMsg;

	/* update LA and Physical address */
	svc_cec_UpdateInfo();

	VK_memset((void*)(&stTaskMsg), 0, sizeof(svcCec_TaskMsg_t));

	/* send TV Power Off */
	err = svc_cec_SendTVPowerOff();

	/* check result */
	if (err != ERR_OK)
	{
		/* arbitration 실패 때만 재시도 한다 */
		VK_TASK_Sleep(CEC_RETRY_DELAY_TV_POWEROFF);
		/* retry <image CEC_TASK_MSG_TV_POWER_OFF> */
		stTaskMsg.eEvent = eCEC_TASK_MSG_TV_POWER_OFF;
		VK_MSG_Send(s_ulCecMsgQId, (void*)(&stTaskMsg), sizeof(svcCec_TaskMsg_t));
	}
}


STATIC void svc_cec_RcvSendActiveSource(HUINT32 ulPwrSyncCnt)
{
	HERROR				hErr;
	svcCec_TaskMsg_t	stTaskMsg;
	HBOOL				bRetry = FALSE;

	/* check STB power */
	if (ulPwrSyncCnt != s_stCecInfo.ulPwrSyncCnt)
	{
		HxLOG_Print("%s, %d : Power changed\n", s_szfile, __LINE__);
		return;
	}

	if (s_stCecInfo.bPwrState == FALSE)
	{
		HxLOG_Print("%s, %d : Power changed to off\n", s_szfile, __LINE__);
		return;
	}

	/* update LA and Physical address */
	svc_cec_UpdateInfo();

	/* increase cnt */
	s_ulCecSndActiveSrcCnt++;

	HxLOG_Print("%s(), <Active Source> cnt = %d\n", __func__, s_ulCecSndActiveSrcCnt);

	/* check timeout */
	if (s_ulCecSndActiveSrcCnt > CEC_MAX_SND_ERR_ACTIVE_SOURCE)
	{
		/* CEC 대응 기기가 아닌것 같다. 그만하자. */
		HxLOG_Print("%s(), <Active Source> cnt = %d\n", __func__, s_ulCecSndActiveSrcCnt);
		return;
	}

	HxSTD_memset((void*)(&stTaskMsg), 0, sizeof(svcCec_TaskMsg_t));

	/* check LA */
	if (s_stCecInfo.ucLogicalAddr == CEC_LA_UNREGISTERED)
	{
		HxLOG_Print("%s, %d : LA=15, retry\n", s_szfile, __LINE__);
		bRetry = TRUE;
	}
	else if (s_stCecInfo.usPhysicalAddr == CEC_INVALID_PHYSICAL_ADDR)
	{
		HxLOG_Print("%s, %d : physical address=F.F.F.F, retry\n", s_szfile, __LINE__);
		bRetry = TRUE;
	}
	else
	{
		/* send <Active Source> */
		hErr = svc_cec_SendActiveSource();
		if (hErr != ERR_OK)
		{
			HxLOG_Print("%s, %d : hErr=0x%x, retry\n", s_szfile, __LINE__, hErr);
			bRetry = TRUE;
		}
	}

	/* check retry */
	if (bRetry == TRUE)
	{
		/* delay */
		VK_TASK_Sleep(CEC_RETRY_DELAY_ACITVE_SOURCE);

		/* retry <active source */
		stTaskMsg.eEvent = eCEC_TASK_MSG_SEND_ACTIVE_SOURCE;
		stTaskMsg.aulArg[0] = ulPwrSyncCnt;
		VK_MSG_Send(s_ulCecMsgQId, (void*)(&stTaskMsg), sizeof(svcCec_TaskMsg_t));
	}
	else
	{
		/* set active source */
		s_stCecInfo.bActiveSource = TRUE;
	}
}

STATIC void svc_cec_RcvDataRcved(HUINT32 ulRxIndex)
{
	PalCec_Msg_t	*pstRxMsg;

	pstRxMsg = &s_stRxMsgBuf[ulRxIndex].stRxMsg;

	HxLOG_Print("%s, %d : rcv, src=%d, dest=%d, opcode=0x%02x\n", s_szfile, __LINE__, pstRxMsg->initiator, pstRxMsg->destination, pstRxMsg->opcode);

	/* check standby state */
	if (s_stCecInfo.bPwrState == FALSE)
	{
		HxLOG_Print("%s, %d : ignore data rcved, standby state\n", s_szfile, __LINE__);
		goto exit;
	}

	if (pstRxMsg->opcodeValid == FALSE)
	{
		HxLOG_Print("%s, %d : rcv <Ping>\n", s_szfile, __LINE__);
		goto exit;
	}

	switch (pstRxMsg->opcode)
	{
		/* One Touch Play */
		case CEC_OPCODE_ACTIVE_SOURCE:
			HxLOG_Print("%s, %d : rcv <Active Source>\n", s_szfile, __LINE__);
			svc_cec_RcvActiveSource(pstRxMsg);
			break;

		case CEC_OPCODE_IMAGE_VIEW_ON:
			HxLOG_Print("%s, %d : rcv <Image View On>\n", s_szfile, __LINE__);
			svc_cec_RcvImageViewOn(pstRxMsg);
			break;

		case CEC_OPCODE_TEXT_VIEW_ON:
			HxLOG_Print("%s, %d : rcv <Text View On>\n", s_szfile, __LINE__);
			svc_cec_RcvTextViewOn(pstRxMsg);
			break;

		/* Routing Control */
		case CEC_OPCODE_REQUEST_ACTIVE_SOURCE:
			HxLOG_Print("%s, %d : rcv <Request Active Source>\n", s_szfile, __LINE__);
			svc_cec_RcvRequestActiveSource(pstRxMsg);
			break;

		case CEC_OPCODE_ROUTING_CHANGE:
			HxLOG_Print("%s, %d : rcv <Routing Change>\n", s_szfile, __LINE__);
			svc_cec_RcvRoutingChange(pstRxMsg);
			break;

		case CEC_OPCODE_ROUTING_INFORMATION:
			HxLOG_Print("%s, %d : rcv <Routing Infomation>\n", s_szfile, __LINE__);
			svc_cec_RcvRoutingInformation(pstRxMsg);
			break;

		case CEC_OPCODE_SET_STREAM_PATH:
			HxLOG_Print("%s, %d : rcv <Set Stream Path>\n", s_szfile, __LINE__);
			svc_cec_RcvSetStreamPath(pstRxMsg);
			break;

		/* Standby */
		case CEC_OPCODE_STANDBY:
			HxLOG_Print("%s, %d : rcv <Standby>\n", s_szfile, __LINE__);
			svc_cec_RcvStandby(pstRxMsg);
			break;

		/* One Touch Record Feature */
		case CEC_OPCODE_RECORD_OFF:
			HxLOG_Print("%s, %d : rcv <Record Off>\n", s_szfile, __LINE__);
			svc_cec_RcvRecordOff(pstRxMsg);
			break;

		case CEC_OPCODE_RECORD_ON:
			HxLOG_Print("%s, %d : rcv <Record On>\n", s_szfile, __LINE__);
			svc_cec_RcvRecordOn(pstRxMsg);
			break;

		case CEC_OPCODE_RECORD_STATUS:
			HxLOG_Print("%s, %d : rcv <Record Status>\n", s_szfile, __LINE__);
			svc_cec_RcvRecordStatus(pstRxMsg);
			break;

		case CEC_OPCODE_RECORD_TV_SCREEN:
			HxLOG_Print("%s, %d : rcv <Record Tv Screen>\n", s_szfile, __LINE__);
			svc_cec_RcvRecordTvScreen(pstRxMsg);
			break;

		/* System Information */
		case CEC_OPCODE_GIVE_PHYSICAL_ADDRESS:
			HxLOG_Print("%s, %d : rcv <Give Physical Address>\n", s_szfile, __LINE__);
			svc_cec_RcvGivePhysicalAddress(pstRxMsg);
			break;

		case CEC_OPCODE_GET_MENU_LANGUAGE:
			HxLOG_Print("%s, %d : rcv <Get Menu Language>\n", s_szfile, __LINE__);
			svc_cec_RcvGetMenuLanguage(pstRxMsg);
			break;

		case CEC_OPCODE_REPORT_PHYSICAL_ADDRESS:
			HxLOG_Print("%s, %d : rcv <Report Physical Address>\n", s_szfile, __LINE__);
			svc_cec_RcvReportPhysicalAddress(pstRxMsg);
			break;

		case CEC_OPCODE_SET_MENU_LANGUAGE:
			HxLOG_Print("%s, %d : rcv <Set Menu Language>\n", s_szfile, __LINE__);
			svc_cec_RcvSetMenuLanguage(pstRxMsg);
			break;

		/* Deck Control Feature */
		case CEC_OPCODE_DECK_CONTROL:
			HxLOG_Print("%s, %d : rcv <Deck Control>\n", s_szfile, __LINE__);
			svc_cec_RcvDeckControl(pstRxMsg);
			break;

		case CEC_OPCODE_DECK_STATUS:
			HxLOG_Print("%s, %d : rcv <Deck Status>\n", s_szfile, __LINE__);
			svc_cec_RcvDeckStatus(pstRxMsg);
			break;

		case CEC_OPCODE_GIVE_DECK_STATUS:
			HxLOG_Print("%s, %d : rcv <Give Deck Status>\n", s_szfile, __LINE__);
			svc_cec_RcvGiveDeckStatus(pstRxMsg);
			break;

		case CEC_OPCODE_DECK_PLAY:
			HxLOG_Print("%s, %d : rcv <Deck Play>\n", s_szfile, __LINE__);
			svc_cec_RcvDeckPlay(pstRxMsg);
			break;

		/* Tuner control */
		case CEC_OPCODE_GIVE_TUNER_DEVICE_STATUS:
			HxLOG_Print("%s, %d : rcv <Give Tuner Device Status>\n", s_szfile, __LINE__);
			svc_cec_RcvGiveTunerDeviceStatus(pstRxMsg);
			break;

		case CEC_OPCODE_SELECT_DIGITAL_SERVICE:
			HxLOG_Print("%s, %d : rcv <Select Digital Service>\n", s_szfile, __LINE__);
			svc_cec_RcvSelectDigitalService(pstRxMsg);
			break;

		case CEC_OPCODE_TUNER_DEVICE_STATUS:
			HxLOG_Print("%s, %d : rcv <Tuner Device Status>\n", s_szfile, __LINE__);
			svc_cec_RcvTunerDeviceStatus(pstRxMsg);
			break;

		case CEC_OPCODE_TUNER_STEP_DECREMENT:
			HxLOG_Print("%s, %d : rcv <Tuner Step Decrement>\n", s_szfile, __LINE__);
			svc_cec_RcvTunerStepDecrement(pstRxMsg);
			break;

		case CEC_OPCODE_TUNER_STEP_INCREMENT:
			HxLOG_Print("%s, %d : rcv <Tuenr Step Increment>\n", s_szfile, __LINE__);
			svc_cec_RcvTunerStepIncrement(pstRxMsg);
			break;

		/* Vendor Specific Commands Feature */
		case CEC_OPCODE_DEVICE_VENDOR_ID:
			HxLOG_Print("%s, %d : rcv <Device Vendor ID>\n", s_szfile, __LINE__);
			svc_cec_RcvDeviceVendorId(pstRxMsg);
			break;

		case CEC_OPCODE_GIVE_VENDOR_ID:
			HxLOG_Print("%s, %d : rcv <Device Give Vendor ID>\n", s_szfile, __LINE__);
			svc_cec_RcvGiveVendorId(pstRxMsg);
			break;

		case CEC_OPCODE_VENDOR_COMMAND:
			HxLOG_Print("%s, %d : rcv <Device Vendor Command>\n", s_szfile, __LINE__);
			svc_cec_RcvVendorCommand(pstRxMsg);
			break;

		case CEC_OPCODE_VENDOR_REMOTE_BUTTON_DOWN:
			HxLOG_Print("%s, %d : rcv <Device Vendor Remote Button Down>\n", s_szfile, __LINE__);
			svc_cec_RcvVendorRemoteButtonDown(pstRxMsg);
			break;

		case CEC_OPCODE_VENDOR_REMOTE_BUTTON_UP:
			HxLOG_Print("%s, %d : rcv <Device Vendor Remote Button Up>\n", s_szfile, __LINE__);
			svc_cec_RcvVendorRemoteButtonUp(pstRxMsg);
			break;

		/* OSD Status Display Feature */
		case CEC_OPCODE_SET_OSD_STRING:
			HxLOG_Print("%s, %d : rcv <Set OSD String>\n", s_szfile, __LINE__);
			svc_cec_RcvSetOsdString(pstRxMsg);
			break;

		/* Device OSD Transfer Feature */
		case CEC_OPCODE_GIVE_OSD_NAME:
			HxLOG_Print("%s, %d : rcv <Give OSD Name>\n", s_szfile, __LINE__);
			svc_cec_RcvGiveOsdName(pstRxMsg);
			break;

		case CEC_OPCODE_SET_OSD_NAME:
			HxLOG_Print("%s, %d : rcv <Set OSD Name>\n", s_szfile, __LINE__);
			svc_cec_RcvSetOsdName(pstRxMsg);
			break;

		/* Device Menu Control Feature */
		case CEC_OPCODE_MENU_REQUEST:
			HxLOG_Print("%s, %d : rcv <Menu Request>\n", s_szfile, __LINE__);
			svc_cec_RcvMenuRequest(pstRxMsg);
			break;

		case CEC_OPCODE_MENU_STATUS:
			HxLOG_Print("%s, %d : rcv <Menu Status>\n", s_szfile, __LINE__);
			svc_cec_RcvMenuStatus(pstRxMsg);
			break;

#if defined(CONFIG_MW_CEC_RECEIVE_USER_CTRL_PRESSED)
		case CEC_OPCODE_USER_CONTROL_PRESSED:
			HxLOG_Print("%s, %d : rcv <User Control Pressed>\n", s_szfile, __LINE__);
			svc_cec_RcvUserControlPressed(pstRxMsg);
			break;

		case CEC_OPCODE_USER_CONTROL_RELEASED:
			HxLOG_Print("%s, %d : rcv <User Control Released>\n", s_szfile, __LINE__);
			svc_cec_RcvUserControlReleased(pstRxMsg);
			break;
#else
		// temporary disabled because current scenario allows only standby on/off
		// it can be enabled after discussion about CEC key handling
#endif

		/* Power Status */
		case CEC_OPCODE_GIVE_POWER_STATUS:
			HxLOG_Print("%s, %d : rcv <Give Power Status>\n", s_szfile, __LINE__);
			svc_cec_RcvGivePowerStatus(pstRxMsg);
			break;

		case CEC_OPCODE_REPORT_POWER_STATUS:
			HxLOG_Print("%s, %d : rcv <Report Power Status>\n", s_szfile, __LINE__);
			svc_cec_RcvReportPowerStatus(pstRxMsg);
			break;

		/* General Protocol */
		case CEC_OPCODE_FEATURE_ABORT:
			HxLOG_Print("%s, %d : rcv <Feature Abort>\n", s_szfile, __LINE__);
			svc_cec_RcvFeatureAbort(pstRxMsg);
			break;

		case CEC_OPCODE_ABORT:
			HxLOG_Print("%s, %d : rcv <Abort>\n", s_szfile, __LINE__);
			svc_cec_RcvAbort(pstRxMsg);
			break;

		default:
			HxLOG_Print("%s, %d : rcv unknown message(0x%02x)\n", s_szfile, __LINE__, pstRxMsg->opcode);
			break;
	}

exit:
	s_stRxMsgBuf[ulRxIndex].bValid = FALSE;
}

STATIC void svc_cec_Callback(PalCec_Event_t eCecEvent, HUINT32 ulArg)
{
	int n;
	svcCec_TaskMsg_t stTaskMsg;
	HUINT32			 nResult = 0;

	HxSTD_memset((void*)(&stTaskMsg), 0, sizeof(svcCec_TaskMsg_t));

	switch (eCecEvent)
	{
		case eCEC_PLUG_OUT:
			stTaskMsg.eEvent = eCEC_TASK_MSG_PLUG_OUT;
			nResult = VK_MSG_SendTimeout(s_ulCecMsgQId, (void*)(&stTaskMsg), sizeof(svcCec_TaskMsg_t), 0);
			if(nResult != ERR_OK)
			{
				HxLOG_Error("[%s][%d] VK_MSG_SendTimeout err !\n",__FUNCTION__,__LINE__);
			}
			break;

		case eCEC_PLUG_IN:
			stTaskMsg.eEvent  = eCEC_TASK_MSG_PLUG_IN;
			nResult = VK_MSG_SendTimeout(s_ulCecMsgQId, (void*)(&stTaskMsg), sizeof(svcCec_TaskMsg_t), 0);
			if(nResult != ERR_OK)
			{
				HxLOG_Error("[%s][%d] VK_MSG_SendTimeout err !\n",__FUNCTION__,__LINE__);
			}
			break;

		case eCEC_MESSAGE_RCVED:
			for (n=0; n<CEC_NUM_RCV_BUFFER; n++)
			{
				if (s_stRxMsgBuf[n].bValid == FALSE)
				{
					s_stRxMsgBuf[n].bValid = TRUE;
					HxSTD_memcpy(&s_stRxMsgBuf[n].stRxMsg, (void *)ulArg, sizeof(PalCec_Msg_t));

					stTaskMsg.eEvent  = eCEC_TASK_MSG_DATA_RCVED;
					stTaskMsg.aulArg[0] = n;
					nResult = VK_MSG_SendTimeout(s_ulCecMsgQId, (void*)(&stTaskMsg), sizeof(svcCec_TaskMsg_t), 0);
					if(nResult != ERR_OK)
					{
						HxLOG_Error("[%s][%d] VK_MSG_SendTimeout err !\n",__FUNCTION__,__LINE__);
					}
					break;
				}
			}

			if (n == CEC_NUM_RCV_BUFFER)
			{
				HxLOG_Print("%s, %d : rcv buffer full!!!\n", s_szfile, __LINE__);
			}
			break;

		default:
			break;
	}
}

STATIC void svc_cec_Task(void *pvArg)
{
	svcCec_TaskMsg_t	stTaskMsg;
	HINT32 				nRet = VK_ERROR;

	while(1)
	{
		nRet = VK_MSG_Receive(s_ulCecMsgQId, &stTaskMsg, sizeof(svcCec_TaskMsg_t));
		if (nRet != VK_OK)
		{
			HxLOG_Error("VK_MSG_Receive failed. s_ulCecMsgQId(%d) \n",s_ulCecMsgQId);
		}

		switch (stTaskMsg.eEvent)
		{
			case eCEC_TASK_MSG_CHK_TASK_RUN:
				HxLOG_Print("%s, %d : rcv eCEC_TASK_MSG_CHK_TASK_RUN\n", s_szfile, __LINE__);
				break;

			case eCEC_TASK_MSG_PLUG_OUT:
				HxLOG_Print("%s, %d : rcv eCEC_TASK_MSG_PLUG_OUT\n", s_szfile, __LINE__);
				break;

			case eCEC_TASK_MSG_PLUG_IN:
				HxLOG_Print("%s, %d : rcv eCEC_TASK_MSG_PLUG_IN\n", s_szfile, __LINE__);
				break;

			case eCEC_TASK_MSG_SEND_IMAGE_VIEW_ON:
				HxLOG_Print("%s, %d : rcv eCEC_TASK_MSG_SEND_IMAGE_VIEW_ON\n", s_szfile, __LINE__);
				svc_cec_RcvSendImageViewOn(stTaskMsg.aulArg[0]);
				break;

			case eCEC_TASK_MSG_WAIT_TV_ON:
				HxLOG_Print("%s, %d : rcv eCEC_TASK_MSG_WAIT_TV_ON\n", s_szfile, __LINE__);
				svc_cec_RcvWaitTvOn(stTaskMsg.aulArg[0]);
				break;

			case eCEC_TASK_MSG_SEND_ACTIVE_SOURCE:
				HxLOG_Print("%s, %d : rcv eCEC_TASK_MSG_SEND_ACTIVE_SOURCE\n", s_szfile, __LINE__);
				svc_cec_RcvSendActiveSource(stTaskMsg.aulArg[0]);
				break;

			case eCEC_TASK_MSG_DATA_RCVED:
				HxLOG_Info("%s, %d : rcv eCEC_TASK_MSG_DATA_RCVED\n", s_szfile, __LINE__);
				svc_cec_RcvDataRcved(stTaskMsg.aulArg[0]);
				break;

			case eCEC_TASK_MSG_TX_AGING:
				HxLOG_Info("%s, %d : rcv CEC_TASK_MSG_TX_AGING\n", s_szfile, __LINE__);
/*				svc_cec_SendAbort(0); */
				svc_cec_SendGivePhysicalAddress(0);
				break;
			case eCEC_TASK_MSG_TV_POWER_OFF:
				svc_cec_RcvTVPowerOff();
				break;
            case eCEC_TASK_MSG_TV_USERCTRL_PRESSED:
                HxLOG_Info("%s, %d : rcv eCEC_TASK_MSG_TV_USERCTRL_PRESSED(0x%x)\n", s_szfile, __LINE__,stTaskMsg.aulArg[0]);
                svc_cec_SendUserCtrlPressed(stTaskMsg.aulArg[0]);
                break;
			default:
				break;
		}
	}
}


/*****************************************************************************/
#define ____PUBLIC_APIs________________________________________________
/*****************************************************************************/
HERROR SVC_CEC_Init(PostMsgToMgrCb_t fnPostMsgToMgr)
{
	HERROR			hErr;
	STATIC HBOOL	bInit = FALSE;

	HxLOG_Debug("%s, %d : %s()\n", s_szfile, __LINE__, __FUNCTION__);

	if (bInit == FALSE)
	{
		bInit = TRUE;

		s_fnSvcCecPostMsgToMgr = fnPostMsgToMgr;

		svc_cec_InitInfo();

		hErr = (HERROR)VK_MSG_Create(MW_CEC_MSGQ_SIZE, sizeof(svcCec_TaskMsg_t), "CecMsgQ", &s_ulCecMsgQId, VK_SUSPENDTYPE_FIFO);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("Creation Error: CecMessageQ \n");
		}

		hErr = (HERROR)VK_TASK_Create (svc_cec_Task, MW_CEC_TASK_PRIORITY, MW_CEC_TASK_STACK_SIZE, "cecTask",  NULL, &s_ulCecTaskId, 0);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("Creation Error: cec Task\n");
		}

		PAL_CEC_RegisterCallback(svc_cec_Callback);

		VK_TASK_Start(s_ulCecTaskId);
	}
	else
	{
		HxLOG_Print("%s, %d : re-init?\n", s_szfile, __LINE__);
	}

	return ERR_OK;
}

void SVC_CEC_SetPowerState(HBOOL bPwrState)
{
	HxLOG_Print("%s, %d : %s(), bPwrState=%d\n", s_szfile, __LINE__, __FUNCTION__, bPwrState);

	s_stCecInfo.bPwrState = bPwrState;
	s_stCecInfo.ulPwrSyncCnt++;
}

void SVC_CEC_GetPowerState(HBOOL *pbPwrState)
{
	HxLOG_Print("%s, %d : %s(), bPwrState=%d\n", s_szfile, __LINE__, __FUNCTION__, s_stCecInfo.bPwrState);
        *pbPwrState = s_stCecInfo.bPwrState; 
}

HERROR SVC_CEC_SetModelName(HCHAR *pszModelName)
{
	if (NULL == pszModelName)
	{
		HxLOG_Print("%s, %d : %s(), NULL == pszModelName\n", s_szfile, __LINE__, __FUNCTION__);
		return ERR_FAIL;
	}

	HxLOG_Print("%d : %s(), pszModelName=[%s]\n", __LINE__, __FUNCTION__, pszModelName);

	HxSTD_MemSet(s_stCecInfo.szModelName, 0, CEC_MAX_OPERAND_LEN);
	HxSTD_StrNCpy(s_stCecInfo.szModelName, pszModelName, CEC_MAX_OPERAND_LEN-1);

	return ERR_OK;
}

void SVC_CEC_PowerOffTv(void)
{
	HERROR				hErr;
	HBOOL				bEnable;
	svcCec_TaskMsg_t 	stTaskMsg;

	HxLOG_Print("%s, %d : %s()\n", s_szfile, __LINE__, __func__);

	/* check CEC on */
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_HDMI_CEC_ENABLE, (HUINT32 *)&bEnable, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("%s, %d : CEC off state\n", s_szfile, __LINE__);
		return;
	}

	/* check CEC on */
	if (bEnable == FALSE)		/* CEC OFF */
	{
		HxLOG_Print("%s, %d : CEC off state\n", s_szfile, __LINE__);
		return;
	}

	/* send Tv Power Off */
	HxSTD_memset((void*)(&stTaskMsg), 0, sizeof(svcCec_TaskMsg_t));

	stTaskMsg.eEvent  = eCEC_TASK_MSG_TV_POWER_OFF;
	VK_MSG_Send(s_ulCecMsgQId, (void*)(&stTaskMsg), sizeof(svcCec_TaskMsg_t));
}

void SVC_CEC_PowerOnTv(void)
{
	HERROR				hErr;
	HBOOL				bEnable;
	svcCec_TaskMsg_t	stTaskMsg;

	HxLOG_Print("%s, %d : %s()\n", s_szfile, __LINE__, __func__);

	/* init error cnt */
	s_ulCecSndImageViewOnCnt = 0;
	s_ulCecWaitTvOnCnt = 0;
	s_ulCecSndActiveSrcCnt = 0;

	/* init tv power status */
	s_eCecTvPwrStatus = eCEC_TV_POWER_UNKNOWN;

//	bEnable = MWC_PARAM_GetHdmiCecEnable();
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_HDMI_CEC_ENABLE, (HUINT32 *)&bEnable, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_HDMI_CEC_ENABLE) Error!!!\n");
	}

	/* check CEC on */
	if (bEnable == FALSE)		/* CEC OFF */
	{
		HxLOG_Print("%s, %d : CEC off state\n", s_szfile, __LINE__);
		return;
	}
	/* send image view on */
	HxSTD_memset((void*)(&stTaskMsg), 0, sizeof(svcCec_TaskMsg_t));

	/* send <ImageViewOn> to power on TV */
	stTaskMsg.eEvent  = eCEC_TASK_MSG_SEND_IMAGE_VIEW_ON;
	stTaskMsg.aulArg[0] = s_stCecInfo.ulPwrSyncCnt;
	VK_MSG_Send(s_ulCecMsgQId, (void*)(&stTaskMsg), sizeof(svcCec_TaskMsg_t));
}

void SVC_CEC_UserCtrl_Pressed(DxHdmiUserCtrlPressed_e eParam)
{
	svcCec_TaskMsg_t	stTaskMsg;

	HxLOG_Print("%s, %d : %s()\n", s_szfile, __LINE__, __func__);
	HxSTD_memset((void*)(&stTaskMsg), 0, sizeof(svcCec_TaskMsg_t));

	stTaskMsg.eEvent  = eCEC_TASK_MSG_TV_USERCTRL_PRESSED;
    stTaskMsg.aulArg[0] = eParam;
	VK_MSG_Send(s_ulCecMsgQId, (void*)(&stTaskMsg), sizeof(svcCec_TaskMsg_t));
}


/*********************** End of File ******************************/
