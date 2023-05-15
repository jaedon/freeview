/**
 * di_key.c
*/

/**
 * @defgroup		DI_KEY DI_KEY : Key event device interface
 * @author		Jin-hong Kim
 * @note			key event API file
 * @brief			Define key event apis
 * @file 			di_key.c
*/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* include common utils first! */
#include "linden_trace.h"
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include "vkernel.h"
#include "taskdef.h"

#include "bstd.h"

/* include drv headers */
#include "drv_remocon.h"
#include "drv_micom.h"
/* include di headers */
//#include "di_key.h"
#include "drv_key.h"
#include "drv_pm.h"
#include "drv_rcu_type.h"

//#if defined(CONFIG_IR_MULTI_CUSTOM_CODE)
#include "drv_remocon_multi_custom_code.h"
//#endif

#if defined(CONFIG_FRONTKEY_GPIO) || (defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT) && defined(CONFIG_MICOM))
#include "drv_gpio.h"
#endif

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/

#if defined(CONFIG_PRODUCT_IMAGE_FACTORY)
#undef CONFIG_DIRECTFB
#endif

#define DEBUG_MODULE	TRACE_MODULE_DI_KEY

#define IR_MAX_REPEAT_COUNT 1

#if defined(CONFIG_RCU_INPUTMODE_RSTEP)
#define IR_NOISE_FILTER_PERIOD      200  	//200ms
#else
#define IR_NOISE_FILTER_PERIOD      500  	//500ms
#endif
#define	DI_FRONT_KEY_ID_MAX (20)

#if defined(CONFIG_FRONTKEY_GPIO)

#define FRONT_KEY_DELAY_TIME				100

#if defined(CONFIG_OS_UCOS)
#define FRONT_KEY_VALID_REPEAT_COUNT			1
#else
#define FRONT_KEY_VALID_REPEAT_COUNT			5
#endif /* #if defined(CONFIG_OS_UCOS) */
#endif

#define UNUSED(a) (void)(a)

/* Data Code from RCU about Release Key */
#define RELEASE_KEY_FROM_RCU	0x8f
/* Period used to check release key from rcu */
#if defined(CONFIG_RCU_RELEASE_TIME)
#define RELEASE_KEY_WAITING_TIME		CONFIG_RCU_RELEASE_TIME   /* default 200 */
#else
#define RELEASE_KEY_WAITING_TIME		200
#endif
/* Initialize Value for Repeat Key Check Timer */
#define KEY_TIMER_ID_INITIAL_VALUE	0

/*#define RELEASE_KEY_DEBUG_TIMER*/
/*#define RELEASE_KEY_DEBUG_COUNT*/

#if defined(CONFIG_RCU_INPUTMODE_RC5)
/*======================================================*/
/* MSB 14 13 12 11 10 9  8  7  6  5  4  3  2  1  0  LSB */
/*     S1 S2 CB A4 A3 A2 A1 A0 C6 C5 C4 C3 C2 C1 C0     */
/*    |START|CB| Address bits |    Command bits    |    */
/*======================================================*/
#define IR_RC5_CONTROL_BIT_SHIFT	0xC
#define IR_RC5_CONTROL_BIT_MASK		0x1
#define IR_RC5_ADDRESS_BITS_SHIFT	0x7
#define IR_RC5_ADDRESS_BITS_MASK	0x1F
#define IR_RC5_COMMAND_BITS_SHIFT	0x0
#define IR_RC5_COMMAND_BITS_MASK	0x7F

#define IR_RC5_RESERVED_SYSTEM_CODE	0xFF
#endif
/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/

struct IRRepeat	//delete? or not?
{
	HUINT32 nRepeat;
	HUINT32 CurrentTicValue;
	HUINT32 PreviousTicValue;
};

typedef enum
{
	IR_NOISE_IS_REMOVED = 0,
	IR_NOISE_IS_NOT_REMOVED
}IR_NOISE_STATUS;

struct IR_INFO
{
	HUINT16 		        usCurrentCustomCode;	/* 현재 설정된 리모컨 Custom Code */
	HUINT32                 ulRepeatCounter;  		/* 리모컨 키 repetition 을 처리 */
	IR_NOISE_STATUS         eNoiseStatus;     		/* Noise가 제거된 상태인지를 나타내는 flag */
	HUINT32                  ulPreviousTickValue;      /* 전에 리모컨 키가 눌린 시간 */
};

typedef struct IR_INFO * pIR_INFO;

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
extern const RCU_INFO s_asKeyCodeTable[IR_NUMBER_OF_KEY];
#if defined(CONFIG_RCU_MULTI_DECODER)
#if defined(CONFIG_RCU_INPUTMODE_MULTI_XMP)
extern const RCU_INFO s_asKeyCodeTableXmp[IR_NUMBER_OF_KEY];
#endif
#endif

// platform\configure\CPU#-MODELNAME_revx.x borad_cfg.c 파일에
// b/d 별 front key mapping array ( s_BoardFrontKeyMapping[] ) 등록되어 있음.
// 모델 변경되어 front key mapping 변경시 위 위치에서 변경해 주어야 함.
extern const DI_KEY_FRONT s_BoardFrontKeyMapping[];
extern HUINT32 DRV_CFG_GetKeyMaxNum(void);

#if defined(CONFIG_FRONTKEY_GPIO)
extern HUINT32 DRV_CFG_GetGPIOFrontKeyMapMaxNum(void);
extern GPIO_KeyMappingTable_t s_GPIO_KeyConfig[];
#endif
/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
static pIR_INFO 			s_pIrInfo = NULL;
static pfnDI_KEY_Notify key_callback = NULL;
static pfnDI_KEY_Notify key_preprocessing_callback = NULL;
static unsigned long	s_ulIRMsgQId;

#if defined(CONFIG_UNSUPPORTED_RCU)
#else
static unsigned long 	s_ulKeyTaskId;
#endif

#if defined(CONFIG_RCU_INTERNAL)&& !defined(CONFIG_RCU_INPUTMODE_RC5)
#if defined(CONFIG_UNSUPPORTED_RCU)
#else
static HUINT32 s_ulReleaseKeyCheckTimerID = KEY_TIMER_ID_INITIAL_VALUE;
static HBOOL s_bIsKeyReleased = TRUE;
static HUINT32 s_ulRepeatKeyPreviousTickValue = 0;
static HUINT32 s_ulRepeatKeyCurrentTickValue = 0;
#if (defined(CONFIG_DIRECTFB) && !defined(CONFIG_PRODUCT_IMAGE_HWTEST)) || defined (CONFIG_NOVA_CE)
static HUINT32 s_ulPreviousPressedKeyCode = 0;
#endif
#endif
#endif

#if defined(CONFIG_RCU_MULTI_DECODER)
static unsigned long	s_ulIRMsgQId2;
static unsigned long 	s_ulKeyTaskId2;
#endif

#if defined(CONFIG_FRONTKEY_GPIO)
static unsigned long keySendTaskID;
static unsigned long keyReceiveTaskID;
static unsigned long keyMsgID;
#endif

//static IRRepeat IR_REPEAT_INFO = NULL;
#if defined(CONFIG_RCU_INTERNAL) && !defined(CONFIG_RCU_INPUTMODE_RC5)
#if defined(CONFIG_UNSUPPORTED_RCU)
#else
static HUINT32 s_ulIRNoiseFilterTimerID = VK_TIMERID_NULL;
#endif
#endif

#if defined(CONFIG_RCU_INPUTMODE_RC5)
// RC5 RCU의 종류를 구분하기 위한 system code (address code)
static HUINT8 s_ucRC5SystemCode	= IR_RC5_RESERVED_SYSTEM_CODE; // reserved system code, from RC-5 Spec.
#endif

/*******************************************************************/
/******************* static function Prototypes ********************/
/*******************************************************************/
#if defined(CONFIG_RCU_INTERNAL) && !defined(CONFIG_RCU_INPUTMODE_RC5)
#if defined(CONFIG_UNSUPPORTED_RCU)
#else
static void DRV_KEY_NoiseFilterTimer_Callback(unsigned long TimerId, void *params);
#endif
#endif

#if defined(CONFIG_RCU_MICOM)
static void DRV_KEY_RcuKey_Callback(HUINT8 ucIrFormat, HINT8 ucRepeatType, HUINT8 *pucKeyCode, HUINT8 ulLength);
#endif

#if defined(CONFIG_FRONTKEY_GPIO)
static DRV_Error P_KEY_GPIO_FrontKey_Init(void);
#endif

//#define DEBUG_KEY
#if defined(DEBUG_KEY)
static void P_KEY_PrintIRMSG(IR_MSG msg);
#endif

/* For key pre-processing */
static unsigned long sIntrTaskId;

/*******************************************************************/
/******************* extern function Prototypes ********************/
/*******************************************************************/


/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/

/*******************************************************************/
/*********************** Function Description***************************/
/*******************************************************************/
// Function Name: DRV_IR_NoiseFilterTimer_Callback
// Function Description:
// Parameter:
// Return Value:
/*******************************************************************/
#if defined(CONFIG_RCU_INTERNAL) && !defined(CONFIG_RCU_INPUTMODE_RC5)
#if defined(CONFIG_UNSUPPORTED_RCU)
#else
static void DRV_KEY_NoiseFilterTimer_Callback(unsigned long TimerId, void *params)
{
	BSTD_UNUSED(params);

	if (s_ulIRNoiseFilterTimerID == TimerId)
		s_pIrInfo->eNoiseStatus = IR_NOISE_IS_REMOVED;
	s_ulIRNoiseFilterTimerID = VK_TIMERID_NULL;
}
#endif /* CONFIG_UNSUPPORTED_RCU */
#endif

/*
 * DEBUG
 * Function Name : P_KEY_CountKeyEvent
 * Function Description : Prints number of key events
 * INPUT : DI_KEY_TYPE eKeyType
 * OUTPUT : NONE
 */
#if defined(CONFIG_RCU_INTERNAL)&& !defined(CONFIG_RCU_INPUTMODE_RC5)
#if defined(CONFIG_UNSUPPORTED_RCU)
#else
static void P_KEY_CountKeyEvent(DI_KEY_TYPE eKeyType)
{
#if defined(RELEASE_KEY_DEBUG_COUNT)
	static HUINT32 s_ulPressKeyCount = 0;
	static HUINT32 s_ulReleaseKeyCount = 0;

	if(eKeyType == DI_KEY_PRESSED)
	{
		s_ulPressKeyCount++;
		PrintError("[PRESS] Press : %5d, Release : %5d\n", s_ulPressKeyCount, s_ulReleaseKeyCount);
	}
	else if(eKeyType == DI_KEY_RELEASED)
	{
		s_ulReleaseKeyCount++;
		PrintError("[RELEASE] Press : %5d, Release : %5d\n", s_ulPressKeyCount, s_ulReleaseKeyCount);
	}
#else
	//Remove compile warning
	(void)eKeyType;
#endif
}
#endif
#endif

/*
 * DEBUG
 * Function Name : P_KEY_PrintTimerInfo
 * Function Description : Prints Timer Setting Info
 * INPUT : HBOOL set, HUINT32 timerID
 * OUTPUT : NONE
 */
#if defined(CONFIG_RCU_INTERNAL)&& !defined(CONFIG_RCU_INPUTMODE_RC5)
#if defined(CONFIG_UNSUPPORTED_RCU)
#else
static void P_KEY_PrintTimerInfo(HBOOL bSet, HUINT32 ulTimerID)
{
#if defined(RELEASE_KEY_DEBUG_TIMER)
	if(bSet == TRUE)
	{
		PrintError("[SET] Timer ID : %5d\n", ulTimerID);
	}
	else if(bSet == FALSE)
	{
		PrintError("[CANCEL] Timer ID : %5d\n", ulTimerID);
	}
#else
	//Remove compile warning
	(void)bSet;
	(void)ulTimerID;
#endif
}
#endif
#endif

/*
 * DEBUG
 * Function Name : P_KEY_PrintTimeDiff
 * Function Description : Print difference between two ticks
 * INPUT : HUINT32 prev, cur(system tick)
 * OUTPUT : NONE
 */
#if defined(CONFIG_RCU_INTERNAL)&& !defined(CONFIG_RCU_INPUTMODE_RC5)
#if defined(CONFIG_UNSUPPORTED_RCU)
#else
static void P_KEY_PrintTimeDiff(HUINT32 prev, HUINT32 cur)
{
#if defined(RELEASE_KEY_DEBUG_TIMER)
	PrintError("[Tick] DIFF : %d PREV : %d CUR : %d\n", cur - prev, prev, cur);
#else
	//Remove compile warning
	(void)prev;
	(void)cur;
#endif
}
#endif
#endif

/*
 * Function Name : P_KEY_SendReleaseKey
 * Function Description : Sends Release Key to MW
 * INPUT : NONE
 * OUTPUT : NONE
 */
#if defined(CONFIG_RCU_INTERNAL)&& !defined(CONFIG_RCU_INPUTMODE_RC5)
#if defined(CONFIG_UNSUPPORTED_RCU)
#else
static void P_KEY_SendReleaseKey(void)
{
	P_KEY_CountKeyEvent(DI_KEY_RELEASED);

#if (defined(CONFIG_DIRECTFB) && !defined(CONFIG_PRODUCT_IMAGE_HWTEST)) || defined (CONFIG_NOVA_CE)
	key_callback(DI_RCU_KEY, s_asKeyCodeTable[s_ulPreviousPressedKeyCode].ulKeyCode, DI_KEY_RELEASED);
	PrintDebug("RELEASE KEY CODE : 0x%x\n", s_ulPreviousPressedKeyCode);
#else
	key_callback(DI_RCU_KEY, s_asKeyCodeTable[RELEASE_KEY_FROM_RCU].ulKeyCode, DI_KEY_RELEASED);
#endif

	s_bIsKeyReleased = TRUE;
	s_ulReleaseKeyCheckTimerID = KEY_TIMER_ID_INITIAL_VALUE;

	s_ulRepeatKeyPreviousTickValue = 0;
	s_ulRepeatKeyCurrentTickValue = 0;
}
#endif
#endif

/*
 * Function Name : P_KEY_ReleaseKeyTimerCallback
 * Function Description : Sends Release Key to MW(Callback)
 * INPUT : NONE
 * OUTPUT : NONE
 */
#if defined(CONFIG_RCU_INTERNAL)&& !defined(CONFIG_RCU_INPUTMODE_RC5)
#if defined(CONFIG_UNSUPPORTED_RCU)
#else
static void P_KEY_ReleaseKeyTimerCallback(unsigned long TimerId, void *params)
{
	BSTD_UNUSED(params);

#if defined(RELEASE_KEY_DEBUG_TIMER)
	PrintError("[Release Check Timer] FIRE ! TimerID : %d\n", TimerId);
#else
	//Remove compile warning
	(void)TimerId;
	(void)params;
#endif
	P_KEY_SendReleaseKey();
}
#endif
#endif

#if defined(CONFIG_MICOM) || defined(CONFIG_MICOM_JP) || defined(CONFIG_DEBUG) || defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
static unsigned char P_KEY_ConvertKeyenumToKeyCode(DI_KEY_REMOCON nRcuKey)
{
	unsigned int i=0;

	for (i=0; i<IR_NUMBER_OF_KEY; i++)
	{
		if (s_asKeyCodeTable[i].ulKeyCode == nRcuKey)
		{
			return (unsigned char)(i&0xFF);
		}
	}

	PrintError("Wrong nRcuKey(0x%x) \n", nRcuKey);

	return 0xff;	//invalid
}
#endif

#if defined(DEBUG_KEY)
static void P_KEY_PrintIRMSG(IR_MSG msg)
{
/*
	typedef struct IRMessage
	{
	DI_KEY_DEVICE eDevice;
	HUINT32 code;
	DI_KEY_TYPE eKeyType;
	HUINT8 	        		ucDecodedCode;
	HUINT32 				ulRepeatCounter;
#if defined(CONFIG_IR_MULTI_CUSTOM_CODE)
	HBOOL				bMultiCustomMessage;
	HUINT16 				usDecodedCustomCode;
	HUINT32 				ulNextState;
#endif
	}IR_MSG;
*/
	PrintError("[INFO] eDevice : ")
	if(msg.eDevice == DI_FRONT_KEY)
	{
		PrintError("Front Key\n");
	}
	else if(msg.eDevice == DI_RCU_KEY)
	{
		PrintError("Rcu Key\n");
	}
	else
	{
		PrintError("???\n");
	}

	PrintError("[INFO] code : 0x%x\n", msg.code);

	PrintError("[INFO] eKeyType : ")
	if(msg.eKeyType == DI_KEY_PRESSED)
	{
		PrintError("Pressed\n");
	}
	else if(msg.eKeyType == DI_KEY_REPEATED)
	{
		PrintError("Repeated\n");
	}
	else if(msg.eKeyType == DI_KEY_RELEASED)
	{
		PrintError("Released\n");
	}
	else if(msg.eKeyType == DI_KEY_TOGGLED)
	{
		PrintError("Toggled\n");
	}
	else if(msg.eKeyType == DI_KEY_POSITION_XY)
	{
		PrintError("Position(X,Y)\n");
	}
	else
	{
		PrintError("???\n");
	}

	if(msg.eKeyType == DI_KEY_POSITION_XY)
	{
		PrintError("[INFO] X : 0x%x, Y : 0x%x\n", (msg.ucDecodedCode & 0xFF00),
													(msg.ucDecodedCode & 0x00FF));
	}
	else
	{
		PrintError("[INFO] ucDecodedCode : 0x%x\n", msg.ucDecodedCode);
	}
	PrintError("[INFO] ulRepeatCounter : 0x%x\n", msg.ulRepeatCounter);

#if defined(CONFIG_IR_MULTI_CUSTOM_CODE)
	if(msg.bMultiCustomMessage == TRUE)
	{
		PrintError("[INFO] bMultiCustomMessage : TRUE\n");
	}
	else if(msg.bMultiCustomMessage == FALSE)
	{
		PrintError("[INFO] bMultiCustomMessage : FALSE\n");
	}

	PrintError("[INFO] usDecodedCustomCode : 0x%x\n", msg.usDecodedCustomCode);

	PrintError("[INFO] ulNextState : 0x%x\n", msg.ulNextState);
#endif

}
#endif

HUINT8 P_KEY_GetRcuKey( IR_MSG *stIRMSG )
{
	HUINT8 readKeyNum = 0;
	HINT8 ucRepeatType = 0;
	HUINT32 pucKeyCode = 0;
	HUINT8 ulQueueNum = 0;
	HUINT8 InverseIrCode = 0, ucDecodedCode = 0;
	HUINT16 decodedCustomCode = 0;

#if defined(CONFIG_RCU_INPUTMODE_RSTEP)
	HINT8 ucBatterLevel=0;
#endif


#if defined(CONFIG_IR_MULTI_CUSTOM_CODE)
	HUINT8 ucIsValidKey;
#endif

	readKeyNum = DRV_REMOCON_GetEvents(&ucRepeatType, &pucKeyCode, &ulQueueNum);
	if ( readKeyNum == 0 )
	{
		return readKeyNum;
	}
#if defined (CONFIG_RCU_INPUTMODE_SEJIN_56KHZ)
	PrintDebug("[P_KEY_GetRcuKey] IrCode : 0x%x \n", pucKeyCode);

	/* 리모컨 Custom Code 확인 */
	decodedCustomCode = (pucKeyCode & 0xFFFF0000)>>16;

	/* 리모컨 신호 입력*/
	ucDecodedCode=	(HUINT8)((pucKeyCode & 0x0000FF00)>>8);/* trim unnecessary remocon code */
	InverseIrCode = ~((HUINT8)(pucKeyCode & 0x000000FF));
#else
	PrintDebug("[P_KEY_GetRcuKey] IrCode : 0x%x \n", pucKeyCode);

#if defined(CONFIG_RCU_INPUTMODE_RSTEP)
	ucDecodedCode = ((HUINT8)((pucKeyCode & 0x000000FF)));
	decodedCustomCode = ((HUINT8)((pucKeyCode & 0x0001E000)>>13));
	if (pucKeyCode & 0x00000100)
	{
		ucBatterLevel = TRUE;
	}
	else
	{
		ucBatterLevel = FALSE;
	}

	PrintDebug("[P_KEY_GetRcuKey] Raw Data(0x%x), Keycode(%d), Custom code (%d), Battery Level(%d)\n", pucKeyCode, ucDecodedCode, decodedCustomCode, ucBatterLevel);
#else

	/* 리모컨 Custom Code 확인 */
	decodedCustomCode = (pucKeyCode & 0x0000FFFF);

	/* 리모컨 신호 입력*/
	ucDecodedCode=	(HUINT8)((pucKeyCode & 0x00FF0000)>>16);/* trim unnecessary remocon code */
	InverseIrCode = ~((HUINT8)((pucKeyCode & 0xFF000000)>>24));

	/* Check Inverse Code and Data Code */
#if !defined(CONFIG_TEMP_ROCKYPOM) // CVC_DEMO
	if(ucDecodedCode != InverseIrCode)
	{
		PrintError("[P_KEY_GetRcuKey] Wrong IR Code : IrCode is 0x%x / InverseIrCode is 0x%x\n", ucDecodedCode, InverseIrCode);
		return 0;
	}
#endif
#endif
#endif

#if defined(CONFIG_IR_MULTI_CUSTOM_CODE)
	ucIsValidKey = DRV_REMOCON_MultiCusomCode_IsValidKey(decodedCustomCode, ucDecodedCode);
	if(ucIsValidKey == IR_MULTI_CUSTOM_CODE_INVALID_KEY)
	{
		PrintDebug("[P_KEY_GetRcuKey] Pressed Custom Code is Invalid =[0X%x] \n", decodedCustomCode);
		return 0;
	}
	PrintDebug("[P_KEY_GetRcuKey] Pressed Custom Code = [0X%x], Key Code = [0x%x] \n", decodedCustomCode, ucDecodedCode);
#else
#if defined(CONFIG_RC_H101)
	if ((s_pIrInfo->usCurrentCustomCode == decodedCustomCode) || (((s_pIrInfo->usCurrentCustomCode)>>8) == decodedCustomCode))
	{
		PrintDebug("[P_KEY_GetRcuKey] Pressed Custom Code = [0X%x]\n", decodedCustomCode);
	}
	else
#else
#if defined(CONFIG_TEMP_ROCKYPOM) // CVC_DEMO
	if ( 0 )
#else
	if (s_pIrInfo->usCurrentCustomCode != decodedCustomCode)
#endif
	{
		PrintError("[P_KEY_GetRcuKey] Wrong remocon is used. Right CustomCode : 0x%x / Worng CustomCode : 0x%x\n", s_pIrInfo->usCurrentCustomCode, decodedCustomCode);
		return 0;
	}
#endif
#endif

	//1. DI_KEY_TASK로 보낼 정보 정리
//	stIRMSG.code = code;
	stIRMSG->ucDecodedCode = ucDecodedCode;

	stIRMSG->eDevice = DI_RCU_KEY;
	stIRMSG->eKeyType = ucRepeatType? DI_KEY_REPEATED: DI_KEY_PRESSED;

	/* 2.1 Key is Repeated */
	if(stIRMSG->eKeyType == DI_KEY_REPEATED)
	{
		s_pIrInfo->ulRepeatCounter++;
		stIRMSG->ulRepeatCounter = s_pIrInfo->ulRepeatCounter;
	}
	/* 2.2 Key is Pressed */
	else if(stIRMSG->eKeyType == DI_KEY_PRESSED)
	{
		// Init Data which is needed to process repeat key
		s_pIrInfo->ulRepeatCounter = 0;
		stIRMSG->ulRepeatCounter = 0;

#if defined (CONFIG_NOVA_CE)
#else
		/* 2.2.1 Key is Released */
		if(stIRMSG->ucDecodedCode == RELEASE_KEY_FROM_RCU)
		{
			stIRMSG->eKeyType = DI_KEY_RELEASED;
		}
#endif
	}
	/* 2.3 Real Release Key */
	else if(stIRMSG->eKeyType == DI_KEY_RELEASED)
	{
	   PrintDebug("Released\n");
	}
	/* 2.4 Error case */
	else
	{
	   PrintError("Unknown Key Type\n");
	   return 0;
	}

#if defined(CONFIG_IR_MULTI_CUSTOM_CODE)
	stIRMSG->bMultiCustomMessage = FALSE;
	stIRMSG->usDecodedCustomCode = decodedCustomCode;
#endif
	return ulQueueNum;
}

#if defined(CONFIG_FRONT_MICOM_TOUCH) && defined(CONFIG_DEBUG)
HUINT32 g_LastKeyInfo = 0;
extern HBOOL g_FrontTestFlag;
void P_KEY_SaveLastKeyInfo(DI_KEY_DEVICE eDevice, HUINT32 code, DI_KEY_TYPE eKeyType)
{
	if(g_FrontTestFlag == TRUE)
	{
		g_LastKeyInfo = ((eDevice&0x01)<<31) | ((eKeyType&0x01)<<30) | (code&0x3fffffff);
		PrintDebug("[%s] %s : 0x%x\n", ((eDevice==DI_FRONT_KEY)?"FRONT":"RCU"), ((eKeyType==DI_KEY_PRESSED)?"Press":"Repeat"), code);
	}
}
#endif

void P_KEY_Callback( void )	//remoconCallback
{
	IR_MSG stIRMSG;
	HUINT32 ulReturnCode;

	stIRMSG.eDevice = DI_RCU_KEY;

	ulReturnCode = VK_MSG_SendTimeout(s_ulIRMsgQId, &stIRMSG, sizeof(stIRMSG),0);
	if(ulReturnCode != VK_OK)
	{
		PrintError( "[P_KEY_Callback] VK_MSG_Send error : ulReturnCode is 0x%x\n", ulReturnCode);
	}
}

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT) && defined(CONFIG_MICOM)
void P_KEY_GPIO_IntrCallback(void *ctxt1, int ctxt2)
{
	HAPPY(ctxt1);
	HAPPY(ctxt2);

	PrintDebug("[%s]standby key pressed.\n" ,__func__);
}
#endif

#if defined(CONFIG_RCU_MULTI_DECODER)
void P_KEY_Callback2( void )	//remoconCallback
{
	IR_MSG stIRMSG;
	HUINT32 ulReturnCode;

	ulReturnCode = VK_MSG_SendTimeout(s_ulIRMsgQId2, &stIRMSG, sizeof(stIRMSG),0);
	if(ulReturnCode != VK_OK)
	{
		PrintError( "[P_KEY_Callback2] VK_MSG_Send error : ulReturnCode is 0x%x\n", ulReturnCode);
	}
}
#endif

#if defined(CONFIG_DIRECTFB) && !defined(CONFIG_PRODUCT_IMAGE_HWTEST)
#include <sys/ipc.h>
#include <sys/msg.h>

struct dfbmsgbuf
{
    long msgtype;
	union
    {
        struct
        {
		    unsigned int device;
			unsigned int code;
			unsigned int type;
			char mtext[255-24];
        }directfb;

		struct
		{
			unsigned int device;
			unsigned int code;
			unsigned int type;
			int xdisplacement;
			int ydisplacement;
			int zdisplacement;
			char mtext[255-48];
		}directfbrf4ce;
		char mtext[255];
	}u;
};

static void DIRCTFB_KeyCallback(DI_KEY_DEVICE eDevice, HUINT32 ulKeyCode, DI_KEY_TYPE eKeyType)
{
	key_t key_id;
	struct dfbmsgbuf sendbuf;

	key_id = msgget((key_t)24680, IPC_CREAT|0666);
	if (key_id == -1)
	{
		perror("msgget error");
		return;
	}

	VK_memset(&sendbuf, 0x0, sizeof(struct dfbmsgbuf));

	sendbuf.msgtype = eDevice + 1; /* 1 : front key, 2 : RCU, 3 : RF4CE */
	sendbuf.u.directfb.device = eDevice;
	sendbuf.u.directfb.code = ulKeyCode;
	sendbuf.u.directfb.type = eKeyType;

	if (msgsnd( key_id, (void *)&sendbuf, sizeof(struct dfbmsgbuf) - sizeof(long), IPC_NOWAIT) == -1)
	{
		perror("msgsnd error");
	}

	return;
}
#endif

static void P_KEY_PreProcessing_Intr(void *arg)
{
	IR_MSG *pIrMsgT = (IR_MSG*)arg;

	DONE_ERR((!pIrMsgT));

	if (key_preprocessing_callback)
	{
		key_preprocessing_callback(pIrMsgT->eDevice, pIrMsgT->ucDecodedCode, pIrMsgT->eKeyType);
	}

	PrintDebug("[%s,%d] Intr(Key Pre-Processing) callback function called! \n", __FUNCTION__, __LINE__);
done:
	return;
}

#if defined(CONFIG_UNSUPPORTED_RCU)
#else
static void P_KEY_Task(void *argv)
{
	IR_MSG stIRMSG;
#if defined(CONFIG_RCU_INPUTMODE_RC5) || defined(CONFIG_RCU_INTERNAL)
	HUINT8 readKeyNum = 0;
#endif
	HUINT32 ulReturnCode;
#if defined(CONFIG_RCU_INTERNAL) && !defined(CONFIG_RCU_INPUTMODE_RC5)
	HUINT32 ulCurrentTickValue = 0;
#endif
#if defined(CONFIG_IR_MULTI_CUSTOM_CODE) && !defined(CONFIG_RCU_INPUTMODE_RC5)
	pIRINFO pIRInfo;
	IR_MULTI_CUSTOM_OPERATION_STATE eOperation;
#endif
#if defined(CONFIG_RCU_INPUTMODE_RC5)
	HINT8 ucRepeatType = 0;
	HUINT32 pucKeyCode = 0;
	HUINT8 ulQueueNum = 0;
#endif
#if defined(CONFIG_RCU_INPUTMODE_XMP)
	DRV_Error result = DRV_OK;
	HUINT8 ulQueueNum = 0;
	DRV_RCU_XMP_REGISTRY_PACKET regPacket;
	DRV_RCU_XMP_DATA_PACKET dataPacket;
#endif

	argv = argv;

	while(1)
	{
		//1. 메시지 수신
		ulReturnCode = VK_MSG_Receive(s_ulIRMsgQId, &stIRMSG, sizeof(stIRMSG));
		//1.1 수신상태확인
		if(ulReturnCode != VK_OK)
		{
			PrintError( "[DRV_KEY_Task] VK_MSG_Receive Error ret");
			continue;
		}
#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
		if(DRV_PM_GetStandbyStatus() == TRUE)
		{
			PrintDebug("[%s] Stanby mode\n", __func__);
			continue;
		}
#endif
#if defined(CONFIG_RCU_INPUTMODE_RC5)
			readKeyNum = DRV_REMOCON_GetEvents(&ucRepeatType, &pucKeyCode, &ulQueueNum);
			if ( readKeyNum == 0 )
			{
				continue;
			}

			/*
				HM9504HD RCU System Code  : 0x0E (x 01110 xxxxxxx)
				Philips TV RCU System Code : 0 (x 00000 xxxxxxx)
				system code를 체크하지 않으면 rc5 type의 다른 rcu가 동작하게 된다.

				s_ucRC5SystemCode 값이 0xFF이면 System Code가 지정되지 않은 것이므로
				key값을 그냥 올려주도록 한다.
			 */
			PrintDebug("[%s, %d] Address: 0x%x, Command: 0x%x\n", __FUNCTION__, __LINE__, (HUINT8)((pucKeyCode >> IR_RC5_ADDRESS_BITS_SHIFT) & IR_RC5_ADDRESS_BITS_MASK), (HUINT8)(pucKeyCode & IR_RC5_COMMAND_BITS_MASK));
			if((((pucKeyCode >> IR_RC5_ADDRESS_BITS_SHIFT) & IR_RC5_ADDRESS_BITS_MASK) != s_ucRC5SystemCode) && (s_ucRC5SystemCode != IR_RC5_RESERVED_SYSTEM_CODE))
			{
				PrintError("[P_KEY_TASK] address code(0x%x) is not matched, keyCode = 0x%x\n", (HUINT8)((pucKeyCode >> IR_RC5_ADDRESS_BITS_SHIFT) & IR_RC5_ADDRESS_BITS_MASK), (HUINT8)(pucKeyCode & IR_RC5_COMMAND_BITS_MASK));
				continue;
			}

			stIRMSG.eDevice = DI_RCU_KEY;
			stIRMSG.ucDecodedCode = (pucKeyCode & IR_RC5_COMMAND_BITS_MASK); //RC5X version unmask 0x80

			/*
				RC5 Spec에 따라 field bit을 command code 앞에 끼워넣도록 수정하여
				DRV에서 넘겨주는 key 값이 12bit에서 13bit로 변경됨

				| eKeyType	| 	System Code		| 	ucDecodedCode	|
				|	1bit		|		5bits		|		7bits		|

				((pucKeyCode & 0x800)>>11) --> ((pucKeyCode & 0x1000)>>12)
			*/
			stIRMSG.eKeyType = ((pucKeyCode >> IR_RC5_CONTROL_BIT_SHIFT) & IR_RC5_CONTROL_BIT_MASK) ? DI_KEY_TOGGLED: DI_KEY_PRESSED;

#elif defined(CONFIG_RCU_INPUTMODE_XMP)
			result = DRV_REMOCON_Xmp_GetEvents(&regPacket, &dataPacket, &ulQueueNum);
			if ( result != DRV_OK )
			{
				PrintError("%s(%d) : DRV_REMOCON_Xmp_GetEvents Error!!!\n", __FUNCTION__, __LINE__);
				continue;
			}
			if ( ulQueueNum == 0 )
			{
				PrintDebug("%s(%d) : DRV_REMOCON_Xmp_GetEvents ulQueueNum = %d\n", __FUNCTION__, __LINE__, ulQueueNum);
				continue;
			}
			if(dataPacket.DataCode == 0)
			{
				continue;
			}
			if( (dataPacket.owner != DIGITURK_XMP_01_OWNER_CODE) && (dataPacket.owner != DIGITURK_XMP_01_OWNER_CODE_SUPERVISOR) )
			{
				PrintError("%s(%d) : DRV_REMOCON_Xmp_GetEvents ownercode = %d\n", __FUNCTION__, __LINE__, dataPacket.owner);
				continue;
			}

			stIRMSG.eDevice = DI_RCU_KEY;
			stIRMSG.ucDecodedCode = dataPacket.KeyData;
			stIRMSG.eKeyType = dataPacket.fRepeat ? DI_KEY_REPEATED : DI_KEY_PRESSED;

			/* 2.1 Key is Repeated */
			if(stIRMSG.eKeyType == DI_KEY_REPEATED)
			{
				s_pIrInfo->ulRepeatCounter++;
				stIRMSG.ulRepeatCounter = s_pIrInfo->ulRepeatCounter;
			}
			/* 2.2 Key is Pressed */
			else if(stIRMSG.eKeyType == DI_KEY_PRESSED)
			{
				// Init Data which is needed to process repeat key
				s_pIrInfo->ulRepeatCounter = 0;
				stIRMSG.ulRepeatCounter = 0;
			}
			else
			{
			   PrintError("Unknown Key Type\n");
			   return;
			}

#elif defined(CONFIG_RCU_INTERNAL)
			readKeyNum = P_KEY_GetRcuKey(&stIRMSG);
			if(readKeyNum == 0)
			{
				continue;
			}
#endif

			PrintDebug("[P_KEY_TASK] eDevice : 0x%x ucDecodedCode[0x%X] : 0x%x eKeyType : 0x%x \n",
				stIRMSG.eDevice, stIRMSG.ucDecodedCode, s_asKeyCodeTable[stIRMSG.ucDecodedCode].ulKeyCode, stIRMSG.eKeyType);

			if (key_preprocessing_callback)
			{
				PrintDebug("[%s,%d] Callback for pre-processing key is registered!\n", __FUNCTION__, __LINE__);
				VK_INTR_Event(sIntrTaskId, (void*)&stIRMSG);
			}

#if defined(CONFIG_RCU_INTERNAL)&& !defined(CONFIG_RCU_INPUTMODE_RC5)
	#if defined(CONFIG_IR_MULTI_CUSTOM_CODE) && !defined(CONFIG_RCU_INPUTMODE_XMP)
			pIRInfo = DRV_REMOCON_MultiCusomCode_GetIRInfo();

			if(stIRMSG.bMultiCustomMessage == TRUE)
			{
				/* 현재의 state를 clear하고 Next State로 초기화 한다. */
				DRV_REMOCON_MultiCusomCode_ClearState(pIRInfo, stIRMSG.ulNextState);

				PrintDebug("\n\n\n\n\n====================================================\n");
				PrintDebug("[P_KEY_Task] ** ulCurrentState=0x%x\n", pIRInfo->eOperationState);
				PrintDebug("====================================================\n\n\n\n\n\n");

				continue;
			}

			pIRInfo->usDecodedCustomCode = stIRMSG.usDecodedCustomCode;
			pIRInfo->ucDecodedCode = (HUINT8)stIRMSG.ucDecodedCode;

			/* 입력된 Key가 Multi Custom 동작 관련 Key인지 check하여 해당 key에 맞게 동작을 수행한다. */
			eOperation = DRV_REMOCON_MultiCusomCode_StateMachine(pIRInfo);
			if(eOperation != IR_MULTI_CUSTOM_OPERATION_IS_NORMAL)
			{
				continue;
			}
	#endif
			/***********************/
			/* IF-ELSE by eKeyType */
			/***********************/

			// 1. Get Current Tick
			s_ulRepeatKeyCurrentTickValue = VK_TIMER_GetSystemTick();
			P_KEY_PrintTimeDiff(s_ulRepeatKeyPreviousTickValue, s_ulRepeatKeyCurrentTickValue);

			/* 2.1 Key is Released - Got Release Key from RCU */
			if(stIRMSG.eKeyType == DI_KEY_RELEASED)
			{

				P_KEY_PrintTimerInfo(FALSE, s_ulReleaseKeyCheckTimerID);
				VK_TIMER_Cancel(s_ulReleaseKeyCheckTimerID);

				// Callback to MW
				P_KEY_SendReleaseKey();

				/**************************************************************
				 보통 1번 IR Key를 눌렀는데 500ms 안에 repeat key가 발생하는 경우가 있다.
				 이렇게 되면 보통 key가 2번 눌린 것 처럼 발생하여 오동작을 한다. 이러한 증상을
				 제거하기 위해서 한 번 key가 눌린 후 500ms 이내에 발생하는 Repeat key는 Noise
				 로 간주 하여 제거한다. Timer 사용
				**************************************************************/
				s_pIrInfo->eNoiseStatus = IR_NOISE_IS_NOT_REMOVED;

				// Reset Noise Filter Timer
				if (KEY_TIMER_ID_INITIAL_VALUE != s_ulIRNoiseFilterTimerID)
				{
					VK_TIMER_Cancel(s_ulIRNoiseFilterTimerID);
					s_ulIRNoiseFilterTimerID = KEY_TIMER_ID_INITIAL_VALUE;
				}
				ulReturnCode = VK_TIMER_EventAfter(IR_NOISE_FILTER_PERIOD, DRV_KEY_NoiseFilterTimer_Callback,
					NULL, 0,(unsigned long *) &s_ulIRNoiseFilterTimerID) ;

				if(ulReturnCode != VK_OK)
				{
					s_pIrInfo->eNoiseStatus = IR_NOISE_IS_REMOVED;
					PrintError("[P_KEY_Task] VK_TIMER_EventAfter error : ucReturnCode is 0x%x\n", ulReturnCode);
				}
			}
			// Key is repeated
			else if(stIRMSG.eKeyType == DI_KEY_REPEATED)
			{
				ulCurrentTickValue = VK_TIMER_GetSystemTick();

				/**********************************************************************/
				/* BKIR_IsRepeated 에 의해 얻어온 repeat flag의 경우 일정 시간 뒤에도 refresh가 되지 않아	    */
				/* 한참 후에 들어오는 repeat key에 대해서도 repeat key로 인식하여 처리하는 문제가 있다. 	    */
				/* 이럴 경우 마지막 상태가 CH+ Repeat 상태이고 다음 입력이 임의의 repeat key가 들어오면 	    */
				/* CH+ Key로 인식하여 동작하게 된다. 즉 사용자가 Volume+를 누르고 Timing상 Box가 Volume+ */
				/* Repeat Key를 수신시 CH+ Key의 repeat key로 인식하여 잘 못 동작하는 이슈가 발생할 수 있다.*/
				/* 이를 수정하기 위해 전에 입력된 Repeat Key의 Tick 값과 현재 수신된 Repeat Key의 Tick값을    */
				/* 비교하여 Repeat Period를 벗어난 Repeat Key는 무시하도록 아래와 같이 코드를 추가하였다.    */
				/* thyou.2007.10.24                                                                                                           */
				/**********************************************************************/
				if((ulCurrentTickValue - s_pIrInfo->ulPreviousTickValue) >= IR_MAX_REPEAT_PERIOD)
				{
					PrintError("[P_KEY_Task] invalid repeat Key : CurrentRepeatKeyTime=%d, PreviousRepeatKeyTime=%d\n",
						ulCurrentTickValue, s_pIrInfo->ulPreviousTickValue);

					continue;
				}

				s_pIrInfo->ulPreviousTickValue = VK_TIMER_GetSystemTick();

				/*********************/
				/* Release Key Check */
				/*********************/

				if(s_bIsKeyReleased == TRUE)
				{
					// Save Tick
					s_ulRepeatKeyPreviousTickValue = s_ulRepeatKeyCurrentTickValue;
					// Change State
					s_bIsKeyReleased = FALSE;
					// Set Timer
					VK_TIMER_EventAfter(RELEASE_KEY_WAITING_TIME, P_KEY_ReleaseKeyTimerCallback,
										NULL, 0, (unsigned long *)&s_ulReleaseKeyCheckTimerID);
					P_KEY_PrintTimerInfo(TRUE, s_ulReleaseKeyCheckTimerID);
				}
				else
				{
					// Reset Timer
					P_KEY_PrintTimerInfo(FALSE, s_ulReleaseKeyCheckTimerID);
					VK_TIMER_Cancel(s_ulReleaseKeyCheckTimerID);
					VK_TIMER_EventAfter(RELEASE_KEY_WAITING_TIME, P_KEY_ReleaseKeyTimerCallback,
										NULL, 0, (unsigned long *)&s_ulReleaseKeyCheckTimerID);
					P_KEY_PrintTimerInfo(TRUE, s_ulReleaseKeyCheckTimerID);
					P_KEY_PrintTimeDiff(s_ulRepeatKeyPreviousTickValue, s_ulRepeatKeyCurrentTickValue);

					// Save Tick
					s_ulRepeatKeyPreviousTickValue = s_ulRepeatKeyCurrentTickValue;
				}

				/* 500ms 이내에 발생하는 IR Signal은 모두 무시한다. */
				if(s_pIrInfo->eNoiseStatus == IR_NOISE_IS_NOT_REMOVED)
				{
					continue;
				}

				/* Repeat key가 몇번 발생하면 1개의 key로 인식하여 mw에 올려줄 것인가를 결정 */
				/* 보통 리모컨에서 Repeate Key가 100ms 마다 발생하므로 Repeat Count는 1개가 적당함. */
				if((stIRMSG.ulRepeatCounter >= IR_MAX_REPEAT_COUNT) && (key_callback != NULL))
				{

					PrintDebug("[P_KEY_Task] DecodedCode = %x [%s] is repeated :: key code = %x\n",
						stIRMSG.ucDecodedCode,
						s_asKeyCodeTable[stIRMSG.ucDecodedCode].aucKeyName,
						s_asKeyCodeTable[stIRMSG.ucDecodedCode].ulKeyCode);

	#if defined(CONFIG_FRONT_MICOM_TOUCH) && defined(CONFIG_DEBUG)
					P_KEY_SaveLastKeyInfo(stIRMSG.eDevice, s_asKeyCodeTable[stIRMSG.ucDecodedCode].ulKeyCode, DI_KEY_REPEATED);
	#endif
					key_callback(stIRMSG.eDevice, s_asKeyCodeTable[stIRMSG.ucDecodedCode].ulKeyCode, DI_KEY_REPEATED);
					s_pIrInfo->ulRepeatCounter = 0;

				}
				else
				{
					PrintDebug("[P_KEY_Task] .\n");
				}

			}
			else if((stIRMSG.eKeyType == DI_KEY_PRESSED) && (key_callback != NULL))
			{


				/*********************/
				/* Release Key Check */
				/*********************/
				if(s_bIsKeyReleased == TRUE)
				{
					// Save Tick
					s_ulRepeatKeyPreviousTickValue = s_ulRepeatKeyCurrentTickValue;
					// Change State
					s_bIsKeyReleased = FALSE;
					// Set Timer
					VK_TIMER_EventAfter(RELEASE_KEY_WAITING_TIME, P_KEY_ReleaseKeyTimerCallback,
										NULL, 0, (unsigned long *)&s_ulReleaseKeyCheckTimerID);

					P_KEY_PrintTimerInfo(TRUE, s_ulReleaseKeyCheckTimerID);
				}
				else
				{
					// Reset Timer
					P_KEY_PrintTimerInfo(FALSE, s_ulReleaseKeyCheckTimerID);
					VK_TIMER_Cancel(s_ulReleaseKeyCheckTimerID);
					VK_TIMER_EventAfter(RELEASE_KEY_WAITING_TIME, P_KEY_ReleaseKeyTimerCallback,
										NULL, 0, (unsigned long *)&s_ulReleaseKeyCheckTimerID);
					P_KEY_PrintTimerInfo(TRUE, s_ulReleaseKeyCheckTimerID);
					P_KEY_CountKeyEvent(DI_KEY_RELEASED);
					P_KEY_PrintTimeDiff(s_ulRepeatKeyPreviousTickValue, s_ulRepeatKeyCurrentTickValue);

					// Callback to MW
	#if (defined(CONFIG_DIRECTFB) && !defined(CONFIG_PRODUCT_IMAGE_HWTEST)) || defined (CONFIG_NOVA_CE)
					key_callback(DI_RCU_KEY, s_asKeyCodeTable[s_ulPreviousPressedKeyCode].ulKeyCode, DI_KEY_RELEASED);
					PrintDebug("RELEASE KEY CODE : 0x%x\n", s_ulPreviousPressedKeyCode);
	#else
					key_callback(DI_RCU_KEY, s_asKeyCodeTable[RELEASE_KEY_FROM_RCU].ulKeyCode, DI_KEY_RELEASED);
	#endif

					// Save Tick
					s_ulRepeatKeyPreviousTickValue = s_ulRepeatKeyCurrentTickValue;
				}

				/**********************************************************/
				/* 현재 Press된 Tick Value를 저장한다. Repeat Period를 벗어난 repeat key를  */
				/* 제거하기 위함. thyou. 2007.10.24                                                             */
				/**********************************************************/
				s_pIrInfo->ulPreviousTickValue = VK_TIMER_GetSystemTick();

				PrintDebug("[P_KEY_Task] DecodedCode = %x [%s] is pressed :: key code = %x\n",
					stIRMSG.ucDecodedCode,
					s_asKeyCodeTable[stIRMSG.ucDecodedCode].aucKeyName,
					s_asKeyCodeTable[stIRMSG.ucDecodedCode].ulKeyCode);

	#if defined(CONFIG_FRONT_MICOM_TOUCH) && defined(CONFIG_DEBUG)
				P_KEY_SaveLastKeyInfo(stIRMSG.eDevice, s_asKeyCodeTable[stIRMSG.ucDecodedCode].ulKeyCode, DI_KEY_PRESSED);
	#endif
				/* 리모컨에서 한 번 Key를 누룰시 해당 Key를 mw에 올려준다. */
	#if (defined(CONFIG_DIRECTFB) && !defined(CONFIG_PRODUCT_IMAGE_HWTEST)) || defined (CONFIG_NOVA_CE)
				P_KEY_CountKeyEvent(DI_KEY_PRESSED);
				key_callback(stIRMSG.eDevice, s_asKeyCodeTable[stIRMSG.ucDecodedCode].ulKeyCode, DI_KEY_PRESSED);
				s_ulPreviousPressedKeyCode = stIRMSG.ucDecodedCode;
				PrintDebug("PRESS KEY CODE : 0x%x\n", stIRMSG.ucDecodedCode);
	#else
				P_KEY_CountKeyEvent(DI_KEY_PRESSED);
				key_callback(stIRMSG.eDevice, s_asKeyCodeTable[stIRMSG.ucDecodedCode].ulKeyCode, DI_KEY_PRESSED);
	#endif

				/**************************************************************/
				/* 보통 1번 IR Key를 눌렀는데 500ms 안에 repeat key가 발생하는 경우가 있다.         */
				/* 이렇게 되면 보통 key가 2번 눌린 것 처럼 발생하여 오동작을 한다. 이러한 증상을        */
				/* 제거하기 위해서 한 번 key가 눌린 후 500ms 이내에 발생하는 Repeat key는 Noise  */
				/* 로 간주 하여 제거한다. Timer 사용								            */
				/**************************************************************/
				s_pIrInfo->eNoiseStatus = IR_NOISE_IS_NOT_REMOVED;

				if (VK_TIMERID_NULL != s_ulIRNoiseFilterTimerID)
				{
					VK_TIMER_Cancel(s_ulIRNoiseFilterTimerID);
					s_ulIRNoiseFilterTimerID = VK_TIMERID_NULL;
				}
				ulReturnCode = VK_TIMER_EventAfter(IR_NOISE_FILTER_PERIOD, DRV_KEY_NoiseFilterTimer_Callback,
					NULL, 0,(unsigned long *) &s_ulIRNoiseFilterTimerID) ;

				if(ulReturnCode != VK_OK)
				{
					s_pIrInfo->eNoiseStatus = IR_NOISE_IS_REMOVED;
					PrintError("[P_KEY_Task] VK_TIMER_EventAfter error : ucReturnCode is 0x%x\n", ulReturnCode);
				}
			}
			else
			{
				/* Unknown Event */
				PrintError("[P_KEY_Task] Unknown Event\n");
			}
#else
			PrintDebug("[P_KEY_Task] DecodedCode = %x [%s] is pressed :: key code = %x\n",
				stIRMSG.ucDecodedCode,
				s_asKeyCodeTable[stIRMSG.ucDecodedCode].aucKeyName,
				s_asKeyCodeTable[stIRMSG.ucDecodedCode].ulKeyCode);

	#if defined(CONFIG_FRONT_MICOM_TOUCH) && defined(CONFIG_DEBUG)
			P_KEY_SaveLastKeyInfo(stIRMSG.eDevice, s_asKeyCodeTable[stIRMSG.ucDecodedCode].ulKeyCode, stIRMSG.eKeyType);
	#endif
			/* send key to MW */
			if(key_callback != NULL)
			{
				key_callback(stIRMSG.eDevice, s_asKeyCodeTable[stIRMSG.ucDecodedCode].ulKeyCode, stIRMSG.eKeyType);
			}
#endif

#if defined(CONFIG_RCU_INTERNAL)
			if(readKeyNum > 1)
			{
				PrintDebug("[P_KEY_Task] Call one more key callback\n");
				P_KEY_Callback();
			}
#endif

	}
}
#endif

#if defined(CONFIG_RCU_MULTI_DECODER)
static void P_KEY_Task2(void *argv)
{
#if defined(CONFIG_RCU_INPUTMODE_MULTI_XMP)
	IR_MSG stIRMSG;
	IR_MSG stIRMSG2; // only used receiving a message.
	HUINT32 ulReturnCode;
	HUINT8 ulQueueNum = 0;
	DRV_Error result = DRV_OK;
	DRV_RCU_XMP_REGISTRY_PACKET regPacket;
	DRV_RCU_XMP_DATA_PACKET dataPacket;
	HBOOL isToggle = FALSE;
	DI_KEY_TYPE prevKeyType = DI_KEY_RELEASED;

	argv = argv;
	stIRMSG.eKeyType = DI_KEY_PRESSED;

	while(1)
	{
		//1. 메시지 수신
		ulReturnCode = VK_MSG_Receive(s_ulIRMsgQId2, &stIRMSG2, sizeof(stIRMSG2));
		//1.1 수신상태확인
		if(ulReturnCode != VK_OK)
		{
			PrintError( "%s(%d) : VK_MSG_Receive Error ret", __FUNCTION__, __LINE__);
			continue;
		}

		result =  DRV_REMOCON_Xmp_GetEvents(&regPacket, &dataPacket, &ulQueueNum);
		if ( result != DRV_OK )
		{
			PrintError("%s(%d) : DRV_REMOCON_Xmp_GetEvents Error!!!\n", __FUNCTION__, __LINE__);
			continue;
		}
		if ( ulQueueNum == 0 )
		{
			PrintDebug("%s(%d) : DRV_REMOCON_Xmp_GetEvents ulQueueNum = %d\n", __FUNCTION__, __LINE__, ulQueueNum);
			continue;
		}

#if 0 // packet information.
		PrintDebug("Registery Data Info\n");
		PrintDebug("regPacket.RegCode = 0x%X\n", regPacket.RegCode);
		PrintDebug("regPacket.owner = 0x%X\n",regPacket.owner);
		PrintDebug("regPacket.checksum = 0x%X\n", regPacket.checksum);
		PrintDebug("regPacket.tag = 0x%X\n", regPacket.tag);
		PrintDebug("regPacket.peripheralId = 0x%X\n", regPacket.peripheralId);
		PrintDebug("regPacket.RegNum = 0x%X\n", regPacket.RegNum);

		PrintDebug("Key Data Info\n");
		PrintDebug("dataPacket.DataCode = 0x%X\n", dataPacket.DataCode);
		PrintDebug("dataPacket.owner = 0x%X\n", dataPacket.owner);
		PrintDebug("dataPacket.checksum = 0x%X\n", dataPacket.checksum);
		PrintDebug("dataPacket.fmb = 0x%X\n", dataPacket.fmb);
		PrintDebug("dataPacket.tag = 0x%X\n", dataPacket.tag);
		PrintDebug("dataPacket.KeyData = 0x%X\n", dataPacket.KeyData);
		PrintDebug("dataPacket.fCntl = 0x%X\n", dataPacket.fCntl);
		PrintDebug("dataPacket.fShift = 0x%X\n", dataPacket.fShift);
		PrintDebug("dataPacket.fAlt = 0x%X\n", dataPacket.fAlt);
#endif
		if(dataPacket.DataCode == 0) // 아직 data packet이 들어오지 않아서 보내지 않는다.
		{
			continue;
		}

#if defined(CONFIG_RCU_INPUTMODE_RC5)
		if(isToggle == TRUE)
		{
			if(prevKeyType == DI_KEY_PRESSED)
			{
				stIRMSG.eKeyType = DI_KEY_TOGGLED;
				PrintDebug("KeyType = DI_KEY_TOGGLED\n");
			}
			else
			{
				stIRMSG.eKeyType = DI_KEY_PRESSED;
				PrintDebug("KeyType = DI_KEY_TOGGLED\n");
			}
		}

		if(dataPacket.fmb == 1) // break packet이 이후에 다시 들어오는 packet은 new packet이므로 toggle을 해줘야 한다.
		{
			isToggle = TRUE;
		}
		else
		{
			isToggle = FALSE;
		}

		stIRMSG.eDevice = DI_RCU_KEY;
		stIRMSG.ucDecodedCode = dataPacket.KeyData;
		prevKeyType = stIRMSG.eKeyType;	// 다음 packet의 toggle 여부를 판단하기 위해 현재 Key Type을 저장한다.
#else
		stIRMSG.eDevice = DI_RCU_KEY;
		stIRMSG.ucDecodedCode = dataPacket.KeyData;
		stIRMSG.eKeyType = dataPacket.fRepeat ? DI_KEY_REPEATED: DI_KEY_PRESSED;

		/* maybe need codes for repeat key & noise removal  */
#endif
		/* send key to MW */
		if(key_callback != NULL)
		{
			PrintDebug("XMP : Device = %d, KeyCode = 0x%08X, KeyName = %s, KeyType = %d\n",
					stIRMSG.eDevice,
					s_asKeyCodeTableXmp[stIRMSG.ucDecodedCode].ulKeyCode,
					s_asKeyCodeTableXmp[stIRMSG.ucDecodedCode].aucKeyName,
					stIRMSG.eKeyType);
			key_callback(stIRMSG.eDevice, s_asKeyCodeTableXmp[stIRMSG.ucDecodedCode].ulKeyCode, stIRMSG.eKeyType);
		}
		else
		{
			/* Unknown Event */
			PrintError("[%s] Unknown Event\n", __FUNCTION__);
		}
	}
#endif
}
#endif

DRV_Error DRV_KEY_Init(void)
{
#if defined(CONFIG_UNSUPPORTED_RCU)
#if defined(CONFIG_FRONTKEY_GPIO)
	DRV_Error	nDrvErr = DRV_OK;

	nDrvErr= P_KEY_GPIO_FrontKey_Init();
	if(DRV_OK != nDrvErr)
	{
		PrintError("P_KEY_GPIO_FrontKey_Init error\n");
		return DRV_ERR;
	}
#endif
	return DRV_OK;
#else
	HINT32 uVKReturnCode = VK_OK;
#if !defined(CONFIG_IR_MULTI_CUSTOM_CODE) || defined(CONFIG_FRONTKEY_GPIO)|| (defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT) && defined(CONFIG_MICOM))
	DRV_Error	nDrvErr = DRV_OK;
#endif
	s_pIrInfo = DD_MEM_Alloc( sizeof(struct IR_INFO ) );
	s_pIrInfo->usCurrentCustomCode = IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT;
	s_pIrInfo->ulRepeatCounter = 0;
	s_pIrInfo->eNoiseStatus = IR_NOISE_IS_REMOVED;
	s_pIrInfo->ulPreviousTickValue = 0;

	//1. 메시지를 송신할 콜백함수 등록
#if defined(CONFIG_RCU_INTERNAL)
	DRV_REMOCON_RegisterKeyEventCallback((pfnDRV_REMOCON_Notify)P_KEY_Callback);
#elif defined(CONFIG_RCU_MICOM)
	DRV_MICOM_RegisterKeyEventCallback((pfnDRV_UCOM_RCUKey_Notify)DRV_KEY_RcuKey_Callback);
#endif
	//2. 메시지큐 생성
	uVKReturnCode = VK_MSG_Create(MSGQ_SIZE_NORMAL, sizeof(IR_MSG), "IRMsgQ", &s_ulIRMsgQId, VK_SUSPENDTYPE_FIFO);
	if(uVKReturnCode != VK_OK)
	{
		PrintError( "[DRV_KEY_Init] VK_MSG_Create error : uVKReturnCode is 0x%x\n", uVKReturnCode);
		return DRV_ERR;
	}

	//3. DI_KEY_Task로 태스크 생성
	uVKReturnCode = VK_TASK_Create(P_KEY_Task, KEY_TASK_PRIORITY, KEY_TASK_STACK_SIZE, "DI_KEY_Task", (void *)0, &s_ulKeyTaskId, 0);
	if(uVKReturnCode != VK_OK)
	{
		PrintError( "[DRV_KEY_Init] VK_TASK_Create error : uVKReturnCode is 0x%x\n", uVKReturnCode);
		return DRV_ERR;
	}

	//4. DI_KEY_Task 시작
	uVKReturnCode = VK_TASK_Start(s_ulKeyTaskId);
	if(uVKReturnCode != VK_OK)
	{
		PrintError( "[DRV_KEY_Init] VK_TASK_Start error : uVKReturnCode is 0x%x\n", uVKReturnCode);
		return DRV_ERR;
	}

#if defined(CONFIG_RCU_MULTI_DECODER)
	//1. 메시지를 송신할 콜백함수 등록
	DRV_REMOCON_RegisterKeyEventCallback2((pfnDRV_REMOCON_Notify)P_KEY_Callback2);

	//2. 메시지큐 생성
	uVKReturnCode = VK_MSG_Create(MSGQ_SIZE_NORMAL, sizeof(IR_MSG), "IRMsgQ2", &s_ulIRMsgQId2, VK_SUSPENDTYPE_FIFO);
	if(uVKReturnCode != VK_OK)
	{
		PrintError( "[DRV_KEY_Init] VK_MSG_Create error : uVKReturnCode is 0x%x\n", uVKReturnCode);
		return DRV_ERR;
	}

	//3. DI_KEY_Task로 태스크 생성
	uVKReturnCode = VK_TASK_Create(P_KEY_Task2, KEY_TASK_PRIORITY, KEY_TASK_STACK_SIZE, "DI_KEY_Task2", (void *)0, &s_ulKeyTaskId2, 0);
	if(uVKReturnCode != VK_OK)
	{
		PrintError( "[DRV_KEY_Init] VK_TASK_Create error : uVKReturnCode is 0x%x\n", uVKReturnCode);
		return DRV_ERR;
	}

	//4. DI_KEY_Task 시작
	uVKReturnCode = VK_TASK_Start(s_ulKeyTaskId2);
	if(uVKReturnCode != VK_OK)
	{
		PrintError( "[DRV_KEY_Init] VK_TASK_Start error : uVKReturnCode is 0x%x\n", uVKReturnCode);
		return DRV_ERR;
	}
#endif

#if defined(CONFIG_IR_MULTI_CUSTOM_CODE)
	DRV_REMOCON_MultiCustomCode_Init();
#if defined(CONFIG_DIRECTFB) && !defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	DRV_REMOCON_MultiCustomCode_RegisterCallBack((pfnDI_KEY_Notify)DIRCTFB_KeyCallback);
#endif
#else
	nDrvErr = DRV_MICOM_SetCustomCode(s_pIrInfo->usCurrentCustomCode);
	if (nDrvErr != DRV_OK)
	{
		PrintError("Error(0x%x) In DRV_MICOM_SetCustomCode() \n", nDrvErr);
		return	DRV_ERR;
	}
#endif

#if defined(CONFIG_DIRECTFB) && !defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	key_callback = (pfnDI_KEY_Notify)DIRCTFB_KeyCallback;
#endif

#if defined(CONFIG_FRONTKEY_GPIO)
	nDrvErr= P_KEY_GPIO_FrontKey_Init();
	if(DRV_OK != nDrvErr)
	{
		PrintError("P_KEY_GPIO_FrontKey_Init error\n");
		return DRV_ERR;
	}
#endif
#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT) && defined(CONFIG_MICOM)
	nDrvErr = DRV_GPIO_INTR_RegisterEventNotify(GPIO_ID_STBY_KEY_FROM_MICOM, P_KEY_GPIO_IntrCallback, NULL, 0);
	if(nDrvErr != DRV_OK)
	{
		PrintError("DRV_GPIO_INTR_RegisterEventNotify() - GPIO_ID_KEY_STBY Error\n");
	}

	nDrvErr = DRV_GPIO_INTR_Enable(GPIO_ID_STBY_KEY_FROM_MICOM);
	if(nDrvErr != DRV_OK)
	{
		PrintError("DRV_GPIO_INTR_Enable() - GPIO_ID_KEY_STBY Error\n");
	}
#endif

	uVKReturnCode = VK_INTR_Register(P_KEY_PreProcessing_Intr, "key-preprocss", sizeof(IR_MSG), &sIntrTaskId, VK_INTR_TASK1);
	if (uVKReturnCode != VK_OK)
	{
		PrintError("[%s] Error(0x%x) VK_INTR_Register()\n", __FUNCTION__, uVKReturnCode);
		return DRV_ERR;
	}

	return DRV_OK;
#endif
}

#if defined(CONFIG_IR_MULTI_CUSTOM_CODE)
/*******************************************************************/
/*********************** Function Description***************************/
/*******************************************************************/
// Function Name: DDI_IR_GetIRTaskQueueID
// Function Description:
// Parameter:
// Return Value:
/*******************************************************************/
HUINT32 DRV_KEY_GetIRTaskQueueID(void)
{
	return s_ulIRMsgQId;
}

/*******************************************************************/
/*********************** Function Description***************************/
/*******************************************************************/
// Function Name: DDI_IR_MultiCustomCode_RegisterCallBack
// Function Description:
// Parameter:
// Return Value:
/*******************************************************************/
void DI_KEY_MultiCustomCode_RegisterCallBack(void (*NotifyFunc)(HUINT8 msgType, HUINT8	mode))
{
#if defined(CONFIG_DIRECTFB)
	UNUSED(NotifyFunc);
#else
	DRV_REMOCON_MultiCustomCode_RegisterCallBack((IR_MULTICUSTOMCODE_NOTIFY_APP_FUNC)NotifyFunc);
#endif
}

#if defined(CONFIG_DIRECTFB)
/*******************************************************************/
/*********************** Function Description***************************/
/*******************************************************************/
// Function Name: DI_KEY_GetCurrentCustomCode
// Function Description:
// Parameter:
// Return Value:
/*******************************************************************/
DI_ERR_CODE DI_KEY_GetCurrentCustomCode(HUINT16 *pCustomCode)
{
	HUINT16 customCode;

	customCode = DRV_REMOCON_MultiCusomCode_GetCurrentCustomCode();

	switch (customCode)
	{
		case IR_MULTI_CUSTOM_CODE_1 :
			*pCustomCode = IR_MULTI_CUSTOM_MODE_1;
			break;
		case IR_MULTI_CUSTOM_CODE_2 :
			*pCustomCode = IR_MULTI_CUSTOM_MODE_2;
			break;
		case IR_MULTI_CUSTOM_CODE_3  :
			*pCustomCode = IR_MULTI_CUSTOM_MODE_3;
			break;
		case IR_MULTI_CUSTOM_CODE_4 :
			*pCustomCode = IR_MULTI_CUSTOM_MODE_4;
			break;
		case IR_MULTI_CUSTOM_CODE_5 :
			*pCustomCode = IR_MULTI_CUSTOM_MODE_5;
			break;
		case IR_MULTI_CUSTOM_CODE_6 :
			*pCustomCode = IR_MULTI_CUSTOM_MODE_6;
			break;
		default:
			*pCustomCode = IR_MULTI_CUSTOM_MODE_NONE;
	}

	return DI_ERR_OK;
}
#endif
#endif

//only use
void DRV_KEY_GetKeyName(HUINT8 keyCode,char *keyName)
{
	int decodedCode;
	for (decodedCode=0;decodedCode<IR_NUMBER_OF_KEY;decodedCode++)
	{
		if(s_asKeyCodeTable[decodedCode].ulKeyCode == keyCode)
		{
			VK_MEM_Memcpy((void *) keyName,
				(void *)s_asKeyCodeTable[decodedCode].aucKeyName,
				sizeof(s_asKeyCodeTable[decodedCode].aucKeyName));
			break;
		}
	}
}

#if defined(CONFIG_PRODUCT_IMAGE_FACTORY)
DI_ERR_CODE DI_KEY_SetCustomCode(HUINT16 usCustomCode, HUINT16 *pusPreviousCustomCode)
{
#if defined(CONFIG_IR_MULTI_CUSTOM_CODE)
	pIRINFO pIRInfo;
#endif

	if (pusPreviousCustomCode == NULL)
	{
		PrintError("pusPreviousCustomCode is NULL\n");

		return DI_ERR_INVALID_PARAM;
	}

#if defined(CONFIG_IR_MULTI_CUSTOM_CODE)
	*pusPreviousCustomCode = DRV_REMOCON_MultiCusomCode_GetCurrentCustomCode();

	pIRInfo = DRV_REMOCON_MultiCusomCode_GetIRInfo();
	pIRInfo->usCurrentCustomCode = usCustomCode;
#else
	*pusPreviousCustomCode = s_pIrInfo->usCurrentCustomCode;
	s_pIrInfo->usCurrentCustomCode = usCustomCode;
#endif

	return DI_ERR_OK;
}
#endif

void DI_KEY_RegisterKeyEventCallback(pfnDI_KEY_Notify pfnNotify)
{
#if !defined(CONFIG_DIRECTFB) || defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	key_callback = pfnNotify;
#else
	(void)(pfnNotify);
#endif
}

void DI_KEY_RegisterKeyPreProcessingCallback(pfnDI_KEY_Notify pfnNotify)
{
	key_preprocessing_callback = pfnNotify;
}

DI_ERR_CODE DI_KEY_GetCapability(void *pstCapability)
{
	pstCapability = pstCapability;

	return DI_ERR_OK;
}


DI_ERR_CODE DI_KEY_SetKeyRepeatTime(HUINT32 ulRepeatTime)
{
	ulRepeatTime = ulRepeatTime;

	return DI_ERR_OK;
}

DI_ERR_CODE DI_KEY_GetKeyRepeatTime(HUINT32 *ulRepeatTime)
{
	ulRepeatTime = ulRepeatTime;

	return DI_ERR_OK;
}


DI_ERR_CODE DI_KEY_SetKeyHoldTime(HUINT32 ulHoldTime)
{
	ulHoldTime = ulHoldTime;

	return DI_ERR_OK;
}

DI_ERR_CODE DI_KEY_GetKeyHoldTime(HUINT32 *ulHoldTime)
{
	ulHoldTime = ulHoldTime;

	return DI_ERR_OK;
}

DI_ERR_CODE DI_KEY_SetWakeupRcuKey(DI_KEY_WAKEUP_KEY_TYPE nKeyType, DI_KEY_REMOCON nRcuKey)
{
#if defined(CONFIG_MICOM) || defined(CONFIG_MICOM_JP)
	DRV_Error	nDrvErr = DRV_OK;
	HUINT8	aucKeyCode[2] = {0, 0};
	HUINT8	ucKeyNum = 0;
	DRV_MICOM_SET_WAKEUPKEY_TYPE nMicomKeyType = DI_KEY_WAKEUP_KEY_TYPE_SINGLE;
#else
#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
	DRV_Error	nDrvErr = DRV_OK;
	HUINT32 ulKeyCodeWithCustomCode;
	HUINT8	ucKeyCode = 0x0;
#endif
#endif

	HAPPY(nKeyType);
	HAPPY(nRcuKey);
#if	defined(CONFIG_DEBUG)
	PrintDebug("[%s] nKeyType(0x%x), nRcuKey(0x%x), KeyCode(0x%x)\n", __func__, nKeyType, nRcuKey, P_KEY_ConvertKeyenumToKeyCode(nRcuKey));
#endif

#if defined(CONFIG_MICOM) || defined(CONFIG_MICOM_JP)
	switch(nKeyType)
	{
		case	DI_KEY_WAKEUP_KEY_TYPE_NUMBER:
			nMicomKeyType = DRV_MICOM_SET_WAKEUPKEY_TYPE_GROUP;
			aucKeyCode[0] = P_KEY_ConvertKeyenumToKeyCode(DI_KEY_REMOCON_1);		// GROUP START : RCU_KEY_1
			aucKeyCode[1] = P_KEY_ConvertKeyenumToKeyCode(DI_KEY_REMOCON_0);		// GROUP END : RCU_KEY_0
			ucKeyNum = 2;
			break;
		case	DI_KEY_WAKEUP_KEY_TYPE_CH_UP_DOWN:
			nMicomKeyType = DRV_MICOM_SET_WAKEUPKEY_TYPE_INDIVISUAL;
			aucKeyCode[0] = P_KEY_ConvertKeyenumToKeyCode(DI_KEY_REMOCON_CH_MINUS);		// 1'st key : RCU_KEY_CH+
			aucKeyCode[1] = P_KEY_ConvertKeyenumToKeyCode(DI_KEY_REMOCON_CH_PLUS);		// 2'nd key : RCU_KEY_CH-
			ucKeyNum = 2;
			break;
		case	DI_KEY_WAKEUP_KEY_TYPE_ARROW:
			nMicomKeyType = DRV_MICOM_SET_WAKEUPKEY_TYPE_GROUP;
			aucKeyCode[0] = P_KEY_ConvertKeyenumToKeyCode(DI_KEY_REMOCON_ARROWUP);	// GROUP START : RCU_KEY_ARROW_UP
			aucKeyCode[1] = P_KEY_ConvertKeyenumToKeyCode(DI_KEY_REMOCON_ARROWDOWN);	// GROUP END : RCU_KEY_ARROW_DOWN
			ucKeyNum = 2;
			break;
		case	DI_KEY_WAKEUP_KEY_TYPE_SINGLE:
			nMicomKeyType = DRV_MICOM_SET_WAKEUPKEY_TYPE_INDIVISUAL;
			aucKeyCode[0] = P_KEY_ConvertKeyenumToKeyCode(nRcuKey);		// 1'st key : nRcuKey
			ucKeyNum = 1;
			PrintDebug("[%s] KeyCode(0x%x)\n", __func__, aucKeyCode[0]);
			break;
		default:
			PrintError( "Not Supported nKeyType(0x%x):: nKeyType is 0x%x\n", nKeyType);
			return DI_ERR_INVALID_PARAM;
	}

	nDrvErr = DRV_MICOM_SetWakeupRcuKey(nMicomKeyType, aucKeyCode, ucKeyNum);
	if (nDrvErr != DRV_OK)
	{
		PrintError("Error(0x%x) In DRV_MICOM_SetWakeupRcuKey() \n", nDrvErr);
		return	DI_ERR_ERROR;
	}

#if	defined(CONFIG_MICOM_JP)
	// JAPAN RCU 에는 "11", "12" key 가 존재하므로, KEY_TYPE_NUMBER 일 때, 추가로 set 필요.
	if (nKeyType == DI_KEY_WAKEUP_KEY_TYPE_NUMBER)
	{
		nMicomKeyType = DRV_MICOM_SET_WAKEUPKEY_TYPE_INDIVISUAL;
		aucKeyCode[0] = P_KEY_ConvertKeyenumToKeyCode(DI_KEY_REMOCON_11);		// 1'st key : RCU_KEY_CH+
		aucKeyCode[1] = P_KEY_ConvertKeyenumToKeyCode(DI_KEY_REMOCON_12);		// 2'nd key : RCU_KEY_CH-
		ucKeyNum = 2;
		nDrvErr = DRV_MICOM_SetWakeupRcuKey(nMicomKeyType, aucKeyCode, ucKeyNum);
		if (nDrvErr != DRV_OK)
		{
			PrintError("Error(0x%x) In DRV_MICOM_SetWakeupRcuKey() \n", nDrvErr);
			return	DI_ERR_ERROR;
		}
	}
#endif

#else /* None Micom Model */
#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
	/* Check RCU key */
	if (nRcuKey == DI_KEY_REMOCON_UNKNOWN)
	{
		nRcuKey = DI_KEY_REMOCON_STANDBY;
	}

	/* Convert a key enum to a RCU key code of model*/
	ucKeyCode = P_KEY_ConvertKeyenumToKeyCode(nRcuKey);
	if(ucKeyCode == 0xff)
	{
		PrintError(" ucKeyCode(0x%x) In P_KEY_ConvertKeyenumToKeyCode() \n", ucKeyCode);
		return DRV_ERR;
	}

	/* Neuxs key filtering : set RCU wake up code to wake up from s2, s3 mode.  */
	/* After STB wakes up, DRV_REMOCON_ClearWakeupCode() should be called. */
	nDrvErr = DRV_REMOCON_SetWakeupCode(ucKeyCode, &ulKeyCodeWithCustomCode);
	if(nDrvErr != DRV_OK)
	{
		PrintError(" Error(0x%x) In DRV_REMOCON_SetWakeupCode() \n", nDrvErr);
		return DI_ERR_ERROR;
	}

	PrintDebug("[%s] Only one key can be filtered via Nexus IR filter.\n", __func__);
#endif
#endif

	return DI_ERR_OK;
}

DI_ERR_CODE DI_KEY_GetWakeupRcuKey(HUINT8 *pucKeyCode)
{
	DRV_Error	nDrvErr = DRV_OK;

	nDrvErr = DRV_MICOM_GetWakeupRcuKey(pucKeyCode);
	if (nDrvErr != DRV_OK)
	{
		PrintError("Error(0x%x) In DRV_MICOM_GetWakeupRcuKey() \n", nDrvErr);
		return	DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DI_KEY_ClearWakeupRcuKey(void)
{
	DRV_Error nDrvErr = DRV_ERR;

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
	nDrvErr = DRV_REMOCON_ClearWakeupCode();
#else
	PrintError("Error(0x%x) disable POWER_MANAGEMENT. \n", nDrvErr);
	nDrvErr = DRV_ERR;
#endif

	if(nDrvErr != DRV_OK)
	{
		PrintError(" Error(0x%x) In DRV_REMOCON_ClearWakeupCode()\n", nDrvErr);
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


#if defined(CONFIG_FRONT_MICOM_TOUCH)
DI_ERR_CODE DI_KEY_GetTouchKeyThreshold(HUINT8 *pucThreshold, HUINT8 *pKeyNum)
{
	DRV_Error	nDrvErr = DRV_OK;

	nDrvErr = DRV_MICOM_GetTouchThreshold(pucThreshold, pKeyNum);

	if(nDrvErr != DRV_OK)
	{
		PrintError(" Error(0x%x) In DI_KEY_GetTouchKeyThreshhold()\n", nDrvErr);
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;

}

DI_ERR_CODE DI_KEY_SetTouchKeyThreshold(HUINT8 *pucThreshold, HUINT8 KeyNum)
{
	DRV_Error	nDrvErr = DRV_OK;

	nDrvErr = DRV_MICOM_SetTouchThreshold(pucThreshold, KeyNum);

	if(nDrvErr != DRV_OK)
	{
		PrintError(" Error(0x%x) In DI_KEY_SetTouchKeyThreshold()\n", nDrvErr);
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}
#endif

DRV_Error DRV_KEY_SendFrontKeyMessage(unsigned char ucKeyID, unsigned char ucKeyType)
{
	HUINT32		ulFrontKeyID;
	DI_KEY_TYPE	nFrontKeyType;

	if((key_callback == NULL) || (ucKeyID >= DRV_CFG_GetKeyMaxNum()))
	{
		PrintError("key_callback is null or invalid key ID.\n");
		return DRV_ERR;
	}

	/*
	configure\##CHIP-##MODEL_REVx.x (ex. configure\7335-icordhd2_rev0.0) 안에
	board_cfg.c file 의 const DI_KEY_FRONT s_BoardFrontKeyMapping[] 를
	Board Front silk 및 micom 에 맞게 수정해주어야 한다.
	각각의 Front key 에 대해 micom 에서 어떤 KeyId 를 올려주는지,
	그리고 그것을 DI_KEY_FRONT 의 어떤 enum 으로 mapping 해야 하는지 확인하자.
	*/
	if ( (s_BoardFrontKeyMapping[ucKeyID] < DI_KEY_FRONT_RESERVED_01) && (ucKeyID <= DI_FRONT_KEY_ID_MAX) )
	{
		PrintDebug( "[DRV_KEY_SendFrontKeyMessage] Front Key enum : 0x%08x -(%s)\n\r", s_BoardFrontKeyMapping[ucKeyID], ucKeyType == 0 ? "ONCE" : "REPEAT");
		ulFrontKeyID = s_BoardFrontKeyMapping[ucKeyID];
#if defined(CONFIG_FRONT_MICOM_JAPAN) || defined (CONFIG_FRONTKEY_GPIO_RELEASE)
		nFrontKeyType = ucKeyType;
#else
		nFrontKeyType = (ucKeyType == 0)?DI_KEY_PRESSED : DI_KEY_REPEATED;
#endif
#if defined(CONFIG_FRONT_MICOM_TOUCH) && defined(CONFIG_DEBUG)
		P_KEY_SaveLastKeyInfo(DI_FRONT_KEY, ulFrontKeyID, nFrontKeyType);
#endif
		key_callback(DI_FRONT_KEY, ulFrontKeyID, nFrontKeyType);
	}
	else
	{
		PrintError("Not Available Front ucKeyID(%d), (%s)\n", ucKeyID, ucKeyType == 0 ? "ONCE" : "REPEAT");
		return DRV_ERR;
	}

	return DRV_OK;
}

DRV_Error DRV_KEY_SendMultiKeyMessage(unsigned long ulKeyMap, unsigned char ucKeyType)
{
	HUINT32		i, ulFrontKeyID;
	DI_KEY_TYPE	nFrontKeyType;

	if(key_callback == NULL)
	{
		PrintError("key_callback is null.\n");
		return DRV_ERR;
	}

	/* Check pressed key */
	ulFrontKeyID = 0;
	for(i=0; i<DRV_CFG_GetKeyMaxNum(); i++)
	{
		if( (ulKeyMap&0x01) && (s_BoardFrontKeyMapping[i]<DI_KEY_FRONT_RESERVED_01))
			ulFrontKeyID |= s_BoardFrontKeyMapping[i];
		ulKeyMap >>= 1;
	}

	if ( ulFrontKeyID != 0 )
	{
		PrintDebug( "[DRV_KEY_SendMultiKeyMessage] Front Key enum : 0x%08x -(%s)\n\r", ulFrontKeyID, ucKeyType == 0 ? "ONCE" : "REPEAT");
#if defined(CONFIG_FRONT_MICOM_JAPAN) || defined (CONFIG_FRONTKEY_GPIO_RELEASE)
		nFrontKeyType = ucKeyType;
#else
		nFrontKeyType = (ucKeyType == 0)?DI_KEY_PRESSED : DI_KEY_REPEATED;
#endif
#if defined(CONFIG_FRONT_MICOM_TOUCH) && defined(CONFIG_DEBUG)
		P_KEY_SaveLastKeyInfo(DI_FRONT_KEY, ulFrontKeyID, nFrontKeyType);
#endif
		key_callback(DI_FRONT_KEY, ulFrontKeyID, nFrontKeyType);
	}
	else
	{
		PrintError("Not Available Front ucKeyID(0x%x), (%s)\n", ulKeyMap, ucKeyType == 0 ? "ONCE" : "REPEAT");
		return DRV_ERR;
	}

	return DRV_OK;
}

#if defined(CONFIG_RCU_MICOM)
/*******************************************************************************
* function : DRV_KEY_UcomRcuKey_Callback
* description : micom으로 부터 올라온 IR key를 처리하기 위한 callback.
* input : ir format, keyType(repeat, pressed(down), released(up), keycode, length of keycode
* output : none
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static void DRV_KEY_RcuKey_Callback(HUINT8 ucIrFormat, HINT8 ucRepeatType, HUINT8 *pucKeyCode, HUINT8 ulLength)	//remoconCallback
{
	IR_MSG stIRMSG;
	HUINT32 ulReturnCode;
	HUINT32	ulCustomCode = 0x0;
	HUINT32	ulKeyCode = 0x0;
	DI_KEY_TYPE	eKeyType = 0;
#if defined(CONFIG_IR_MULTI_CUSTOM_CODE)
	HUINT8 ucIsValidKey;
#endif

	switch((DRV_RCU_FORMAT)ucIrFormat)
	{
		case DRV_IR_FORMAT_NEC:
			/* costum[0] | custom[1] | data[0] | ~data[0] */
			if(ulLength < 4)
			{
				PrintError("[KeyCallback] Invalid Key data!!, ulLength : %d\n", ulLength);
				return;
			}
			ulCustomCode = (HUINT16)(((pucKeyCode[2]<<8)|pucKeyCode[3]) & 0xFFFF);
			ulKeyCode = (HUINT8)(pucKeyCode[1] & 0xFF);
			if( ((HUINT8)(pucKeyCode[0] & 0xFF) & (HUINT8)(pucKeyCode[1] & 0xFF)) != 0 )
			{
				PrintError("[P_KEY_Callback] Wrong remocon is used. Right CustomCode : 0x%x / Worng CustomCode : 0x%x\n", s_pIrInfo->usCurrentCustomCode, ulCustomCode);
				return;
			}
			eKeyType = ucRepeatType? DI_KEY_REPEATED: DI_KEY_PRESSED;
			break;
		case DRV_IR_FORMAT_RC5:
			break;
		case DRV_IR_FORMAT_JAPAN_CO:
		/* |Maker Id(16bit)|사업부(4bit)+MakerIdParity(4bit)|ModelCode(4bit)+DevliverySystem(4bit)|Data(8bit)| */
		/* multi custom code시 pucKeyCode[3]만 변경되므로, (HUINT16)으로 변경해도 무관하다. */
			if(ulLength < 5)
			{
				PrintError("[KeyCallback] Invalid Key data!!, ulLength : %d\n", ulLength);
				return;
			}
			ulCustomCode = (HUINT32)((pucKeyCode[0]<<24) | (pucKeyCode[1]<<16) | (pucKeyCode[2]<<8) | pucKeyCode[3]);
			ulKeyCode = (HUINT8)(pucKeyCode[4] & 0xFF);
			eKeyType = (DI_KEY_TYPE)ucRepeatType;
			break;
		default:
			break;
	}

	PrintDebug("[RcuKeyCallback]:  custom code: 0x%08X KeyCode (0x%X), KeyType (%d)\n",ulCustomCode, ulKeyCode, eKeyType);

#if defined(CONFIG_IR_MULTI_CUSTOM_CODE)
	ucIsValidKey = DRV_REMOCON_MultiCusomCode_IsValidKey(ulCustomCode, (HUINT16)ulKeyCode);
	if(ucIsValidKey == IR_MULTI_CUSTOM_CODE_INVALID_KEY)
	{
		PrintDebug("[P_KEY_Callback] Pressed Custom Code is Invalid =[0X%x] \n", ulCustomCode);
		return;
	}
	PrintDebug("[P_KEY_Callback] Pressed Custom Code = [0X%x], Key Code = [0x%x] \n", ulCustomCode, ulKeyCode);
#else
	if (s_pIrInfo->usCurrentCustomCode != (HUINT16)ulCustomCode)
	{
		PrintError("[P_KEY_Callback] Wrong remocon is used. Right CustomCode : 0x%x / Worng CustomCode : 0x%x\n", s_pIrInfo->usCurrentCustomCode, (HUINT16)ulCustomCode);
		return;
	}
#endif

	//1. DI_KEY_TASK로 보낼 정보 정리
	stIRMSG.ucDecodedCode = (HUINT8)ulKeyCode;
	stIRMSG.eDevice = DI_RCU_KEY;
	stIRMSG.eKeyType = eKeyType;

	if(stIRMSG.eKeyType == DI_KEY_REPEATED)
	{
		s_pIrInfo->ulRepeatCounter++;
		stIRMSG.ulRepeatCounter = s_pIrInfo->ulRepeatCounter;
	}
	else
	{
		s_pIrInfo->ulRepeatCounter = 0;
		stIRMSG.ulRepeatCounter = 0;
	}

#if defined(CONFIG_IR_MULTI_CUSTOM_CODE)
	stIRMSG.bMultiCustomMessage = FALSE;
	stIRMSG.usDecodedCustomCode = (HUINT16)ulCustomCode;
#endif

	//2. DI_KEY_TASK로 메시지 송신
	ulReturnCode = VK_MSG_SendTimeout(s_ulIRMsgQId, &stIRMSG, sizeof(stIRMSG),0);
	if(ulReturnCode != VK_OK)
	{
		PrintError( "[P_KEY_Callback] VK_MSG_Send error : ulReturnCode is 0x%x\n", ulReturnCode);
	}

}
#endif


#if defined(CONFIG_DIRECTFB) && !defined(CONFIG_PRODUCT_IMAGE_HWTEST)
HUINT32 DI_KEY_ConvertDecodeToKey(HUINT8 ucDecodedCode)
{
	return s_asKeyCodeTable[ucDecodedCode].ulKeyCode;
}
#endif

#if defined(CONFIG_FRONTKEY_GPIO)
static void P_KEY_GPIO_FrontKey_Send_Task( void *p )
{
	DRV_Error result = DRV_OK;
	HUINT32 *push_key = NULL;
	HUINT8 push_key_count;
	IR_MSG stIRMSG;
	HUINT32 i;
	HUINT32 ulFrontKeyMaxNum = 0;

	ulFrontKeyMaxNum = DRV_CFG_GetGPIOFrontKeyMapMaxNum();

	push_key = (HUINT32 *)DD_MEM_Alloc(sizeof(HUINT32)*ulFrontKeyMaxNum);
	if(push_key == NULL)
	{
		PrintError("%s(%d) : Error. can't alloc\n", __FUNCTION__, __LINE__);
		return;
	}

	BSTD_UNUSED(p);
	for(i=0; i<ulFrontKeyMaxNum; i++)
	{
		s_GPIO_KeyConfig[i].gpioState = GPIO_INVAILD;
		push_key[i] = 0xFFFFFFFF;
	}

	while(1)
	{

		#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
		if(DRV_PM_GetStandbyStatus() == TRUE)
		{
			PrintDebug("[%s] Stanby mode\n", __func__);
			continue;
		}
		#endif

		for(i=0; i<ulFrontKeyMaxNum; i++)
		{
			result = DRV_GPIO_Read(s_GPIO_KeyConfig[i].gpio_id, &s_GPIO_KeyConfig[i].gpioState);
			if(DRV_OK == result)
			{
				if(GPIO_LOW == s_GPIO_KeyConfig[i].gpioState)
				{
					if(push_key[i] == s_GPIO_KeyConfig[i].gpio_id)
					{
						s_GPIO_KeyConfig[i].key_type = DI_KEY_REPEATED;
					}
					else
					{
						s_GPIO_KeyConfig[i].key_type = DI_KEY_PRESSED;
					}
					PrintDebug("%s(%d) : gpio_name = %s\n", __FUNCTION__, __LINE__, s_GPIO_KeyConfig[i].gpio_name);
				}
				else
				{
					s_GPIO_KeyConfig[i].key_type = 0xFFFFFFFF;
					push_key[i] = 0xFFFFFFFF;
				}
			}
			else
			{
				PrintError("DRV_GPIO_Read Error\n", __FUNCTION__, __LINE__);
				s_GPIO_KeyConfig[i].gpioState = GPIO_INVAILD;
				push_key[i] = 0xFFFFFFFF;
			}
		}

		// 2개의 Key가 동시에 누르는 것은 지원하지 않게 처리 함.
		push_key_count = 0;
		for(i=0; i<ulFrontKeyMaxNum; i++)
		{
			if(s_GPIO_KeyConfig[i].gpioState == GPIO_LOW)
			{
				push_key_count++;
			}
		}

		if(push_key_count > 1)
		{
			PrintDebug("%s(%d) : push_key_count = %d, we support to push only one key\n", __FUNCTION__, __LINE__, push_key_count);
			VK_TASK_Sleep(FRONT_KEY_DELAY_TIME);
			continue;
		}
		else
		{
			for(i=0; i<ulFrontKeyMaxNum; i++)
			{
				if(s_GPIO_KeyConfig[i].gpioState == GPIO_LOW)
				{
					PrintDebug("%s(%d) : i = %d, ucKeyID = %d, key_type = %d\n", __FUNCTION__, __LINE__, i, s_GPIO_KeyConfig[i].ucKeyID, s_GPIO_KeyConfig[i].key_type);
					stIRMSG.eKeyType = s_GPIO_KeyConfig[i].key_type;
					stIRMSG.ucDecodedCode = s_GPIO_KeyConfig[i].ucKeyID;
					result = VK_MSG_SendTimeout(keyMsgID, &stIRMSG, sizeof(stIRMSG),0);
					if(result != VK_OK)
					{
						PrintError( "VK_MSG_SendTimeout error : ulReturnCode is 0x%x\n", result);
					}
					else
					{
						push_key[i] = s_GPIO_KeyConfig[i].gpio_id;
					}
				}
			}
		}

		VK_TASK_Sleep(FRONT_KEY_DELAY_TIME);
	}
}

static void P_KEY_GPIO_FrontKey_Receive_Task( void *p )
{
	DRV_Error ulReturnCode = DRV_OK;
	IR_MSG stIRMSG;
	HUINT8 ucReleaseKeyCheckFlag = FALSE;
#ifdef CONFIG_FRONTKEY_GPIO_RELEASE
	HUINT32 delayTime = ( FRONT_KEY_DELAY_TIME + (FRONT_KEY_DELAY_TIME/2) );
#endif
	HUINT8 repeatCount = 0;
	HBOOL repeatkeyStart = FALSE;

	BSTD_UNUSED(p);

	while(1)
	{
		#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
		if(DRV_PM_GetStandbyStatus() == TRUE)
		{
			PrintDebug("[%s] Stanby mode\n", __func__);
			continue;
		}
		#endif

#ifdef CONFIG_FRONTKEY_GPIO_RELEASE
		ulReturnCode = VK_MSG_ReceiveTimeout(keyMsgID, &stIRMSG, sizeof(stIRMSG), delayTime);
#else
		ulReturnCode = VK_MSG_Receive(keyMsgID, &stIRMSG, sizeof(stIRMSG));
#endif
		if(ulReturnCode != VK_OK)
		{
#ifdef CONFIG_FRONTKEY_GPIO_RELEASE
			if(ulReturnCode == VK_TIMEOUT)
			{
				if(TRUE == ucReleaseKeyCheckFlag)
				{
					PrintDebug("%s(%d) :ucDecodedCode = %d, DI_KEY_RELEASED\n", __FUNCTION__, __LINE__, stIRMSG.ucDecodedCode);
					stIRMSG.eKeyType = DI_KEY_RELEASED;
					ulReturnCode = DRV_KEY_SendFrontKeyMessage(stIRMSG.ucDecodedCode, stIRMSG.eKeyType);
					if(ulReturnCode != DRV_OK)
					{
						PrintError("DRV_KEY_SendFrontKeyMessage error : ulReturnCode is 0x%x\n", ulReturnCode);
					}
				}
				ucReleaseKeyCheckFlag = FALSE;
			}
			else
#endif
			{
				PrintError("VK_MSG_Receive or VK_MSG_ReceiveTimeout error : ulReturnCode is 0x%x\n", ulReturnCode);
				ucReleaseKeyCheckFlag = FALSE;
			}

			repeatCount = 0;
			repeatkeyStart = FALSE;
		}
		else
		{
			PrintDebug("%s(%d) :ucDecodedCode = %d, %s\n", __FUNCTION__, __LINE__, stIRMSG.ucDecodedCode, stIRMSG.eKeyType == 0 ? "DI_KEY_PRESSED" : "DI_KEY_REPEATED");

			if(DI_KEY_REPEATED == stIRMSG.eKeyType)
			{
				repeatCount++;
				if(repeatCount >= FRONT_KEY_VALID_REPEAT_COUNT)
				{
					repeatkeyStart = TRUE;
				}

				if(TRUE == repeatkeyStart)
				{
					ulReturnCode = DRV_KEY_SendFrontKeyMessage(stIRMSG.ucDecodedCode, stIRMSG.eKeyType);
					if(ulReturnCode != DRV_OK)
					{
						PrintError("DRV_KEY_SendFrontKeyMessage error : ulReturnCode is 0x%x\n", ulReturnCode);
					}
				}
			}
			else
			{
				ulReturnCode = DRV_KEY_SendFrontKeyMessage(stIRMSG.ucDecodedCode, stIRMSG.eKeyType);
				if(ulReturnCode != DRV_OK)
				{
					PrintError("DRV_KEY_SendFrontKeyMessage error : ulReturnCode is 0x%x\n", ulReturnCode);
				}
				repeatCount = 0;
				repeatkeyStart = FALSE;
			}
			ucReleaseKeyCheckFlag = TRUE;
		}
	}
}

static DRV_Error P_KEY_GPIO_FrontKey_Init(void)
{
	int retval;

	retval = VK_MSG_Create(MSGQ_SIZE_NORMAL, sizeof(IR_MSG), "keyMsgID", &keyMsgID, VK_SUSPENDTYPE_FIFO );
	if(retval != VK_OK)
	{
		PrintError("VK_MSG_Create Create Error\n");
		return DRV_ERR;
	}

	retval = VK_TASK_Create(P_KEY_GPIO_FrontKey_Send_Task, MICOM_KEY_TASK_PRIORITY, MICOM_RX_TASK_STACK_SIZE, "P_GPIO_Key_Send_Task", (void *)0, &keySendTaskID, FALSE);
	if(retval != VK_OK)
	{
		PrintError("VK_TASK_Create Create Error\n");
		return DRV_ERR;
	}

	retval = VK_TASK_Start(keySendTaskID);
	if(retval != VK_OK)
	{
		PrintError("VK_TASK_Start Error\n");
		return DRV_ERR;
	}

	retval = VK_TASK_Create(P_KEY_GPIO_FrontKey_Receive_Task, MICOM_KEY_TASK_PRIORITY, MICOM_RX_TASK_STACK_SIZE, "P_GPIO_Key_Receive_Task", (void *)0, &keyReceiveTaskID, FALSE);
	if(retval != VK_OK)
	{
		PrintError("VK_TASK_Create Create Error\n");
		return DRV_ERR;
	}

	retval = VK_TASK_Start(keyReceiveTaskID);
	if(retval != VK_OK)
	{
		PrintError("VK_TASK_Start Error\n");
		return DRV_ERR;
	}

	return DRV_OK;
}
#endif

#if defined(CONFIG_RCU_INPUTMODE_RC5)
/*	RC-5 type의 RCU를 사용하기 위해서는 반드시 이 API를 호출해 System Code를 설정해 주어야 한다.
	설정하지 않는 경우 System Code가 default 값인 0xFF로 지정되며
	System Code를 체크하지 않고 키값을 올려주게 된다.
	즉, RC-5 type의 모든 RCU가 동작하게 된다.
*/
DI_ERR_CODE DI_KEY_SetRc5SystemCode(HUINT8 ucSystemCode)
{
	s_ucRC5SystemCode = ucSystemCode;

	PrintDebug("[DI_KEY_SetRc5SystemCode] Set system code (0x%x)\n", s_ucRC5SystemCode);

	return DI_ERR_OK;
}
#endif


void Drv_KeyEmulate(HUINT32 ulKeyCode)
{
#if defined(CONFIG_DIRECTFB) && !defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	DIRCTFB_KeyCallback(DI_RCU_KEY, ulKeyCode, DI_KEY_PRESSED);
#else
	//not yet
	HAPPY(ulKeyCode);
#endif
}

HBOOL DI_KEY_IsPressedFrontKey(DI_KEY_FRONT key)
{
#if defined(CONFIG_FRONTKEY_GPIO)
	DRV_Error drvErr = DRV_OK;
	GPIO_STATE_t gpioState;
	HUINT8	ucKeyID = DI_FRONT_KEY_ID_MAX;
	HUINT32 ulFrontKeyMaxNum = 0;
	HUINT32 i = 0;
	HUINT32	ulCfgKeyMaxNum = 0;

	ulCfgKeyMaxNum = DRV_CFG_GetKeyMaxNum();
	for(i=0; i < ulCfgKeyMaxNum; i++)
	{
		if(key == s_BoardFrontKeyMapping[i])
		{
			ucKeyID = (HUINT8)i;
		}
	}
	if(DI_FRONT_KEY_ID_MAX <= ucKeyID)
	{
		return FALSE;
	}

	ulFrontKeyMaxNum = DRV_CFG_GetGPIOFrontKeyMapMaxNum();
	for(i=0; i<ulFrontKeyMaxNum; i++)
	{
		if((HUINT8)s_GPIO_KeyConfig[i].ucKeyID != ucKeyID)
		{
			continue;
		}

		drvErr = DRV_GPIO_Read(s_GPIO_KeyConfig[i].gpio_id, &gpioState);

		if(DRV_OK == drvErr)
		{
			if(GPIO_LOW == gpioState)
			{
				return TRUE;
			}
		}
	}
#endif
	HAPPY(key);
	return FALSE;
}




