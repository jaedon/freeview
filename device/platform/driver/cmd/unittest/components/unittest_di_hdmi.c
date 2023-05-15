/**
 * unittest_di_hdmi.c
*/

/**
 * @author		Kevin Woo
*/

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

#include "linden_trace.h"
#include "unittest.h"

#include "di_err.h"
#include "di_uart.h"
#include "di_hdmi.h"

#include "vkernel.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DEBUG_MODULE TRACE_MODULE_DI_HDMI

#define CEC_MAX_OPERAND_LEN	15
#define CEC_INVALID_PHYSICAL_ADDR				0xFFFF

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
/****************************** typedef ****************************/
/*******************************************************************/
typedef enum
{
	CEC_STB_POWER_UNKNOWN,
	CEC_STB_POWER_ON,
	CEC_STB_POWER_STANDBY,
	CEC_STB_POWER_TO_ON,
	CEC_STB_POWER_TO_STANDBY

} cec_StbPwrStatus_t;

typedef struct
{
	HUINT8	initiator;
	HUINT8	destination;

	HBOOL	opcodeValid;
	HUINT8	opcode;

	HUINT32	numOperand;
	HUINT8	operand[CEC_MAX_OPERAND_LEN];
} CEC_Msg_t;

typedef struct
{
	HUINT16	physicalAddr;
	HUINT8	logicalAddr;

	HBOOL	activeSource;

	cec_StbPwrStatus_t	pwrState;

} cec_Info_t;


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
cec_Info_t	ut_cec_info;
CEC_Msg_t txMsg;

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/


/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/


static void UT_HDMI_SelectTestCase(HUINT32 ulTestNumber);
static void UT_HDMI_Menu(void);

#if defined(CONFIG_HDMI_CEC)
static UT_RESULT ut_GetCecLogicalAddress(HUINT8 *logicalAddr_p)
{
	DI_ERR_CODE 	result;
	int 			ret = UT_FAIL;

	if (logicalAddr_p == NULL)
	{
		goto done;
	}

	result= DI_HDMI_GetLogicalAddress(logicalAddr_p);
	DONE_ERR(result != DI_ERR_OK);

	ret = UT_OK;

	done:
		return ret;
}

static UT_RESULT ut_GetCecPhysicalAddress(HUINT16 *physicalAddr_p)
{
	DI_ERR_CODE 	result;
	int 			ret = UT_FAIL;

	HDMI_RECEIVER_INFO			rcvInfo;
	HDMI_RECEIVER_INFO_OPTION	rcvInfoOption;
	DI_HDMI_AVAILABLE_FORMATS	stCapa;
	HUINT16	phyA,phyB,phyC,phyD;

	rcvInfoOption = IS_CONNECTED | IS_HDMI_DEVICE | IS_AUTHENCATED;
	DI_HDMI_GetState(rcvInfoOption, &rcvInfo);
	DONE_ERR(rcvInfo.bIsConnected == FALSE);

	/* get info */
	result = DI_HDMI_GetCapability(&stCapa);
	DONE_ERR(result != DI_ERR_OK);

	phyA = (HUINT16)stCapa.sPhysicalAddr.NodeA;
	phyB = (HUINT16)stCapa.sPhysicalAddr.NodeB;
	phyC = (HUINT16)stCapa.sPhysicalAddr.NodeC;
	phyD = (HUINT16)stCapa.sPhysicalAddr.NodeD;

	*physicalAddr_p = (phyA << 12) | (phyB << 8) | (phyC << 4) | phyD;

	ret = UT_OK;

	done:
		return ret;
}

static UT_RESULT ut_SendCecMessage(CEC_Msg_t *txMsg_p)
{
	DI_ERR_CODE 	result;
	int 			ret = UT_FAIL;
	DI_HDMI_CEC_MSG_t diMsg;

	if(txMsg_p->opcodeValid == FALSE)
	{
		goto done;
	}

	VK_memset(&diMsg, 0, sizeof(DI_HDMI_CEC_MSG_t));

	/* set initiator and destination */
	diMsg.ucInitAddr = txMsg_p->initiator;
	diMsg.ucDestAddr = txMsg_p->destination;

	/* set opcode */
	if (txMsg_p->opcodeValid == TRUE)
	{
		diMsg.ucMessage[0] = txMsg_p->opcode;
		diMsg.ucMegLength = 1;

		if (txMsg_p->numOperand != 0)
		{
			/* copy operand */
			if(txMsg_p->numOperand < CEC_MAX_OPERAND_LEN)
			{
				VK_memcpy(&diMsg.ucMessage[1], &txMsg_p->operand, txMsg_p->numOperand);
				diMsg.ucMegLength += txMsg_p->numOperand;
			}
			else
			{
				/* too many operand */
				return ERR_FAIL;
			}
		}
	}

	result = DI_HDMI_SendCecMsg(&diMsg);
	DONE_ERR(result != DI_ERR_OK);

	ret = UT_OK;

	done:
		return ret;
}



static UT_RESULT ut_TestCecMessage(void)
{
	DI_ERR_CODE 	result;
	int 			ret = UT_FAIL;
	HUINT16	physicalAddr;
	HUINT8	logicalAddr;


	ut_cec_info.physicalAddr	= CEC_INVALID_PHYSICAL_ADDR;
	ut_cec_info.logicalAddr		= CEC_LA_UNREGISTERED;
	ut_cec_info.activeSource	= FALSE;

	/* get physical address */
	result = ut_GetCecPhysicalAddress(&physicalAddr);
	DONE_ERR(result != DI_ERR_OK);

	/* get logical address */
	result = ut_GetCecLogicalAddress(&logicalAddr);
	DONE_ERR(result != DI_ERR_OK);

	ut_cec_info.physicalAddr = physicalAddr;
	ut_cec_info.logicalAddr = logicalAddr;

	/* check physical address */
	if (ut_cec_info.physicalAddr == CEC_INVALID_PHYSICAL_ADDR)
	{
		DI_UART_Print("%s, %d : invalid physical address\n", __func__, __LINE__);
		goto done;
	}

	/* send */
	VK_memset(&txMsg, 0, sizeof(CEC_Msg_t));

	/* set header */
	txMsg.initiator = ut_cec_info.logicalAddr;
	txMsg.destination = 0;

	VK_printf("ut_cec_info.logicalAddr = %x\n",ut_cec_info.logicalAddr);

	/* set opcode */
	txMsg.opcodeValid = TRUE;
	txMsg.opcode = CEC_OPCODE_GIVE_PHYSICAL_ADDRESS;

	/* set data operand */
	txMsg.numOperand = 0;

	/* send */
	result = ut_SendCecMessage(&txMsg);
	DONE_ERR(result != DI_ERR_OK);

	ret = UT_OK;

done:
	return ret;


}
#endif

static void ut_CbEventNotify(void *pvParam)
{
	HUINT8 eEvent;

	eEvent = *(HUINT8 *)pvParam;

	DI_UART_Print("event %d \n",eEvent);

	if(DH_EVENT_HOTPLUG_CONNECT == eEvent)
	{
		DI_UART_Print("[HOTPLUG_CONNECT] - call unit test mw callback\n");
	}
	else if(DH_EVENT_HOTPLUG_DISCONNECT == eEvent)
	{
		DI_UART_Print("[DH_EVENT_HOTPLUG_DISCONNECT] - call unit test mw callback\n");
	}
	else if(DH_EVENT_HDCP_SUCCESS == eEvent)
	{
		DI_UART_Print("[DH_EVENT_HDCP_SUCCESS] - call unit test mw callback\n");
	}
	else if(DH_EVENT_HDCP_FAIL == eEvent)
	{
		DI_UART_Print("[DH_EVENT_HDCP_FAIL] - call unit test mw callback\n");
	}

	return;
}

static UT_RESULT ut_RegisterEventNotify(void)
{
	DI_ERR_CODE 	result;
	int 			ret = UT_FAIL;

	result = DI_HDMI_RegisterEventNotify(DH_EVENT_HOTPLUG_CONNECT, ut_CbEventNotify);
	DONE_ERR(result != DI_ERR_OK);
	result = DI_HDMI_RegisterEventNotify(DH_EVENT_HOTPLUG_DISCONNECT, ut_CbEventNotify);
	DONE_ERR(result != DI_ERR_OK);
	result = DI_HDMI_RegisterEventNotify(DH_EVENT_HDCP_SUCCESS, ut_CbEventNotify);
	DONE_ERR(result != DI_ERR_OK);
	result = DI_HDMI_RegisterEventNotify(DH_EVENT_HDCP_FAIL, ut_CbEventNotify);
	DONE_ERR(result != DI_ERR_OK);

	ret = UT_OK;

	done:
		return ret;
}

static UT_RESULT ut_GetCapability(void)
{
	DI_ERR_CODE 	result;
	int 			ret = UT_FAIL;

	DI_HDMI_AVAILABLE_FORMATS stCapbility;

	result = DI_HDMI_GetCapability(&stCapbility);
	DONE_ERR(result != DI_ERR_OK);

	DI_UART_Print("############### Hdmi Capability ###############\n");
	DI_UART_Print("ulVideoFormats : %d\n",stCapbility.ulVideoFormats);
	DI_UART_Print("ulAudioFormats : %d\n",stCapbility.ulAudioFormats);
	DI_UART_Print("ulLogicalAddr : %d\n",stCapbility.ulLogicalAddr);
	DI_UART_Print("ulNumOfVFormat : %d\n",stCapbility.ulNumOfVFormat);
	DI_UART_Print("ul3dFormats : %d\n",stCapbility.ul3dFormats);

	DI_UART_Print("sPhysicalAddr.NodeA = %x\n", stCapbility.sPhysicalAddr.NodeA);
	DI_UART_Print("sPhysicalAddr.NodeB = %x\n", stCapbility.sPhysicalAddr.NodeB);
	DI_UART_Print("sPhysicalAddr.NodeC = %x\n", stCapbility.sPhysicalAddr.NodeC);
	DI_UART_Print("sPhysicalAddr.NodeD = %x\n", stCapbility.sPhysicalAddr.NodeD);
	DI_UART_Print("ulLogicalAddr = %x\n", stCapbility.ulLogicalAddr);

	ret = UT_OK;

done:
	return ret;
}

static UT_RESULT ut_GetState(void)
{
	DI_ERR_CODE 	result;
	int 			ret = UT_FAIL;

	HDMI_RECEIVER_INFO hdmiInfo;
	HDMI_RECEIVER_INFO_OPTION hdmiOption;

	VK_memset(&hdmiInfo, 0x00, sizeof(HDMI_RECEIVER_INFO)); //prevent issue fixed

	hdmiOption = IS_CONNECTED | IS_HDMI_DEVICE | IS_AUTHENCATED;

	result = DI_HDMI_GetState(hdmiOption, &hdmiInfo);
	DONE_ERR(result != DI_ERR_OK);

	ret = UT_OK;

done:
	return ret;
}

static UT_RESULT ut_EnableHDCP(void)
{
	DI_ERR_CODE 	result;
	int 			ret = UT_FAIL;

	result = DI_HDMI_EnableHDCP(TRUE);
	DONE_ERR(result != DI_ERR_OK);

	ret = UT_OK;

	done:
		return ret;
}

static UT_RESULT ut_DisableHDCP(void)
{
	DI_ERR_CODE 	result;
	int 			ret = UT_FAIL;

	result = DI_HDMI_EnableHDCP(FALSE);
	DONE_ERR(result != DI_ERR_OK);

	ret = UT_OK;

	done:
		return ret;
}


static UT_MENU_ITEM_T	menu_hdmi[] =
{
	MENU_START,
	MENU_FUNC(ut_GetCapability, 5),
	MENU_FUNC(ut_GetState, 5),
	MENU_FUNC(ut_RegisterEventNotify, 5),
	MENU_FUNC(ut_DisableHDCP, 5),
	MENU_FUNC(ut_EnableHDCP, 5),
#if defined(CONFIG_HDMI_CEC)
	MENU_FUNC(ut_TestCecMessage, 5),
#endif
	MENU_END,
};

static UT_MENU_ITEM_T	menu_main_hdmi[] =
{
	MENU_SUB(menu_hdmi),
};

void UT_HDMI_Init(void)
{
	HINT32 i;
	HINT32	array_size;

	array_size = sizeof(menu_main_hdmi)/sizeof(menu_main_hdmi[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMainMenu(&menu_main_hdmi[i]);
	}
	array_size = sizeof(menu_hdmi)/sizeof(menu_hdmi[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMenu(&menu_hdmi[i]);
	}
}


