/*******************************************************************************
* File name : di_rf4ce.c
* Author : D.K Lee
* description :
*
* Copyright (c)  by Humax Co., Ltd. Since 2007.
* All right reserved
*******************************************************************************/

/*******************************************************************************
* Header and definitions
*******************************************************************************/
/* global header files */

/* chip manufacture's  header files */

/* humax header files */
#include "htype.h"
#include "vkernel.h"
#include "taskdef.h"

#include "di_err.h"
#include "di_uart.h"
#include "di_rf4ce.h"
#include "di_rf4ce_priv.h"

#include "drv_err.h"
#if defined(CONFIG_RF4CE_EXTERNAL)
#include "drv_rf4ce.h"
#endif
#if defined(CONFIG_RF4CE_MICOM)
#include "drv_micom.h"
#endif
#if defined(CONFIG_RF4CE_MOTION_TOUCH_UEI_HILCREST)
#include "drv_hilcrest.h"
#endif
#include "drv_rcu_type.h"
#if defined(CONFIG_DIRECTFB) && !defined(CONFIG_PRODUCT_IMAGE_HWTEST)
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#endif
#include <sys/ipc.h>
#include <sys/msg.h>
#endif

/*******************************************************************************
* Debug level
*******************************************************************************/
#ifdef DI_RF4CE_DEBUG
#define PrintInfo		DI_UART_Print
#define PrintDebug	DI_UART_Print
#define PrintError	DI_UART_Print
#else
#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug	while (0) ((int (*)(char *, ...)) 0)
#define PrintError	DI_UART_Print
#define PrintInfo		DI_UART_Print
#endif


/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#define RF4CE_MAX_REPEAT_COUNT 1
#define RF4CE_MAX_REPEAT_PERIOD	  130        // 130ms
#define RF4CE_NOISE_FILTER_PERIOD      500  	//500ms

#define UNUSED(x) (void)(x)

typedef enum
{
	RF4CE_NOISE_IS_REMOVED = 0,
	RF4CE_NOISE_IS_NOT_REMOVED
}RF4CE_NOISE_STATUS_e;

typedef struct
{
	HUINT16 usCurrentCustomCode;	/* 현재 설정된 리모컨 Custom Code */
	HUINT32 ulRepeatCounter;		/* 리모컨 키 repetition 을 처리 */
	RF4CE_NOISE_STATUS_e eNoiseStatus;			/* Noise가 제거된 상태인지를 나타내는 flag */
	HUINT32 ulPreviousTickValue;	   /* 전에 리모컨 키가 눌린 시간 */
} RF4CE_INFO_t;

#if defined(CONFIG_DIRECTFB) && !defined(CONFIG_PRODUCT_IMAGE_HWTEST)
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

#endif // #if defined(CONFIG_DIRECTFB) && !defined(CONFIG_PRODUCT_IMAGE_HWTEST)

/*******************************************************************************
* Global variables and structures
*******************************************************************************/

/*******************************************************************************
* External variables and functions
*******************************************************************************/
#if defined(CONFIG_RF4CE_MICOM)
extern const RCU_INFO s_asRF4CEKeyCodeTable[RF4CE_NUMBER_OF_KEY];
#endif

#if defined(CONFIG_RF4CE_MICOM)
extern DRV_Error DRV_RF4CE_MICOM_Init(void);
extern void DRV_RF4CE_MICOM_Task(void *pvParam);
#endif

#if defined(CONFIG_RF4CE_EXTERNAL)
extern DRV_Error DRV_RF4CE_EXTERNAL_Init(void);
extern void DRV_RF4CE_EXTERNAL_Task(void *pvParam);
#endif

#if defined(CONFIG_RF4CE_MOTION_TOUCH_UEI_HILCREST)
extern DRV_Error DRV_HILCREST_InitHilCresetLib (void);
#endif
/*******************************************************************************
* Static variables and structures
*******************************************************************************/
RF4CE_INSTANCE_t g_tRF4CEInstance;
static unsigned long s_ulRf4ceTaskId = 0;
static unsigned long s_ulRf4ceMsgQId = 0;
#if defined(CONFIG_RF4CE)
static RF4CE_INFO_t s_stRf4ceInfo;
#endif
static pfnDI_RF4CE_Notify pfnRf4ceCallback = NULL;


/*******************************************************************************
* Static function prototypes
*******************************************************************************/
#if defined(CONFIG_RF4CE_MICOM) && defined(CONFIG_RF4CE_MOTION_TOUCH_UEI_HILCREST)
static void DRV_RF4CE_RelMotion_Callback(int nXDisplacement, int nYDisplacement, int nZDisplacement);
#if 0
static void DRV_RF4CE_AbsMotion_Callback(int nXDisplacement, int nYDisplacement, int nZDisplacement);
#endif
#endif
static void DRV_RF4CE_Button_Callback(HINT8 ucRepeatType, HUINT8 ucButtonCode);
#if defined(CONFIG_DIRECTFB) && !defined(CONFIG_PRODUCT_IMAGE_HWTEST)
static void DRV_RF4CE_DirectFB_Callback(DI_RF4CE_EVENT_MSG_t *pstRf4ceEventMsg);
#endif
static void DRV_RF4CE_Task(void *argv);


/*******************************************************************************
* function : DRV_RF4CE_Init
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_RF4CE_Init(void)
{
	HINT32	nVkRet;

#if defined(CONFIG_RF4CE_MOTION_TOUCH_UEI_HILCREST)
	DRV_HILCREST_InitHilCresetLib();
#endif

	VK_MEM_Memset(&s_stRf4ceInfo, 0x00, sizeof(s_stRf4ceInfo));
	s_stRf4ceInfo.ulRepeatCounter = 0;
	s_stRf4ceInfo.eNoiseStatus = RF4CE_NOISE_IS_REMOVED;
	s_stRf4ceInfo.ulPreviousTickValue = 0;

#if defined(CONFIG_RF4CE_MICOM)
	DRV_MICOM_RegisterRf4ceEventCallback((pfnDRV_UCOM_RF4CEKey_Notify)DRV_RF4CE_Button_Callback);
#endif
#if defined(CONFIG_RF4CE_EXTERNAL)
	DRV_RF4CE_RegisterRf4ceEventCallback((pfnRF4CE_KeyCallback)DRV_RF4CE_Button_Callback);
#endif
#if defined(CONFIG_RF4CE_MOTION_TOUCH_UEI_HILCREST)
	DRV_HILCREST_RegisterRelativeMotionCallback((pfnDRV_HILCREST_RF4CEMotion_Notify)DRV_RF4CE_RelMotion_Callback);
#endif
#if defined(CONFIG_DIRECTFB) && !defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	pfnRf4ceCallback = (pfnDI_RF4CE_Notify)DRV_RF4CE_DirectFB_Callback;
#endif

	nVkRet = VK_MSG_Create(MSGQ_SIZE_NORMAL, sizeof(DI_RF4CE_EVENT_MSG_t), "RF4CEEventMsgQ", &s_ulRf4ceMsgQId, VK_SUSPENDTYPE_FIFO);
	if(nVkRet != VK_OK)
	{
		PrintError( "[DRV_RF4CE_Init] VK_MSG_Create error : nVkRet is 0x%x\n", nVkRet);
		return DRV_ERR;
	}

	nVkRet = VK_TASK_Create(DRV_RF4CE_Task, RF4CE_TASK_PRIORITY, RF4CE_TASK_STACK_SIZE, "DI_RF4CE_Task", (void *)0, &s_ulRf4ceTaskId, FALSE);
	if(nVkRet != VK_OK)
	{
		PrintError( "[DRV_RF4CE_Init] VK_TASK_Create error : nVkRet is 0x%x\n", nVkRet);
		return DRV_ERR;
	}

	nVkRet = VK_TASK_Start(s_ulRf4ceTaskId);
	if(nVkRet != VK_OK)
	{
		PrintError( "[DRV_RF4CE_Init] VK_TASK_Start error : nVkRet is 0x%x\n", nVkRet);
		return DRV_ERR;
	}

	VK_MEM_Memset(&g_tRF4CEInstance, 0x00, sizeof(g_tRF4CEInstance));
	nVkRet = VK_SEM_Create((unsigned long*)&g_tRF4CEInstance.ulSemID, "RF4CE_S", VK_SUSPENDTYPE_FIFO);
	if(nVkRet != VK_OK)
	{
		PrintError("[%s] Creating RF4CE VK_SEM_Create failed!\n",__func__);
		return DRV_ERR;
	}

	nVkRet = VK_MSG_Create(RF4CE_MAX_MSGS_CNTS, sizeof(RF4CE_MSG_t),"RF4CE_Q",(unsigned long*)&g_tRF4CEInstance.ulMsgQID,VK_SUSPENDTYPE_FIFO);
	if(nVkRet != VK_OK)
	{
		PrintError("[%s] Creating RF4CE VK_MSG_Create failed!\n",__func__);
		return DRV_ERR;
	}

#if defined(CONFIG_RF4CE_MICOM)
	nVkRet = VK_TASK_Create(DRV_RF4CE_MICOM_Task, RF4CE_MICOM_TASK_PRIORITY, RF4CE_MICOM_TASK_STACK_SIZE, "RF4CE_T", (void *)0, (unsigned long*)&g_tRF4CEInstance.ulTaskID, FALSE);
#endif

#if defined(CONFIG_RF4CE_EXTERNAL)
	nVkRet = VK_TASK_Create(DRV_RF4CE_EXTERNAL_Task, RF4CE_EXTERNAL_TASK_PRIORITY, RF4CE_EXTERNAL_TASK_STACK_SIZE, "RF4CE_T", (void *)0, (unsigned long*)&g_tRF4CEInstance.ulTaskID, FALSE);
#endif
	if(nVkRet != VK_OK)
	{
		PrintError("[%s] Creating RF4CE VK_TASK_Create failed!\n",__func__);
		return DRV_ERR;
	}

	VK_TASK_Start(g_tRF4CEInstance.ulTaskID);

#if defined(CONFIG_RF4CE_MICOM)
	DRV_RF4CE_MICOM_Init();
#endif

#if defined(CONFIG_RF4CE_EXTERNAL)
	DRV_RF4CE_EXTERNAL_Init();
#endif

	return DRV_OK;
}

/*******************************************************************************
* function : DRV_RF4CE_Task
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static void DRV_RF4CE_Task(void *argv)
{
	DI_RF4CE_EVENT_MSG_t stRf4ceEventMSG;
	int nVkRet = 0;
#if defined(CONFIG_RF4CE_INTERNAL)
	HUINT32 ulCurrentTickValue = 0;
#endif

	UNUSED(argv);

	while(1)
	{
		nVkRet = VK_MSG_Receive(s_ulRf4ceMsgQId, &stRf4ceEventMSG, sizeof(stRf4ceEventMSG));

		if(nVkRet != VK_OK)
		{
			PrintError( "[DRV_RF4CE_Task] VK_MSG_Receive Error ret");
			continue;
		}
#if defined(CONFIG_RF4CE_INTERNAL)
		if(stRf4ceEventMSG.eRf4ceType == DI_RF4CE_REPEATED)
		{
			ulCurrentTickValue = (HUINT32)VK_TIMER_GetSystemTick();

			/**********************************************************************/
			/* BKIR_IsRepeated 에 의해 얻어온 repeat flag의 경우 일정 시간 뒤에도 refresh가 되지 않아		*/
			/* 한참 후에 들어오는 repeat key에 대해서도 repeat key로 인식하여 처리하는 문제가 있다. 		*/
			/* 이럴 경우 마지막 상태가 CH+ Repeat 상태이고 다음 입력이 임의의 repeat key가 들어오면 		*/
			/* CH+ Key로 인식하여 동작하게 된다. 즉 사용자가 Volume+를 누르고 Timing상 Box가 Volume+ */
			/* Repeat Key를 수신시 CH+ Key의 repeat key로 인식하여 잘 못 동작하는 이슈가 발생할 수 있다.*/
			/* 이를 수정하기 위해 전에 입력된 Repeat Key의 Tick 값과 현재 수신된 Repeat Key의 Tick값을	  */
			/* 비교하여 Repeat Period를 벗어난 Repeat Key는 무시하도록 아래와 같이 코드를 추가하였다.	 */
			/* thyou.2007.10.24 													  */
			/**********************************************************************/
			if((ulCurrentTickValue - s_pRf4ceInfo->ulPreviousTickValue) >= RF4CE_MAX_REPEAT_PERIOD)
			{
				PrintError("[DRV_RF4CE_Task] invalid repeat Key : CurrentRepeatKeyTime=%d, PreviousRepeatKeyTime=%d\n",
					ulCurrentTickValue, s_pRf4ceInfo->ulPreviousTickValue);

				continue;
			}

			s_pRf4ceInfo->ulPreviousTickValue = VK_TIMER_GetSystemTick();

			/* 500ms 이내에 발생하는 IR Signal은 모두 무시한다. */
			if(s_pRf4ceInfo->eNoiseStatus == RF4CE_NOISE_IS_NOT_REMOVED)
				continue;

			/* Repeat key가 몇번 발생하면 1개의 key로 인식하여 mw에 올려줄 것인가를 결정 */
			/* 보통 리모컨에서 Repeate Key가 100ms 마다 발생하므로 Repeat Count는 1개가 적당함. */
			if((stRf4ceEventMSG.ulRepeatCounter >= RF4CE_MAX_REPEAT_COUNT) && (pfnRf4ceCallback != NULL))
			{

				PrintDebug("[DRV_RF4CE_Task] DecodedCode = %x is repeated\n", stRf4ceEventMSG.ucDecodedCode);
				pfnRf4ceCallback(&stRf4ceEventMSG);
				s_pRf4ceInfo->ulRepeatCounter = 0;

			}
		}
		else if((stRf4ceEventMSG.eRf4ceType == DI_RF4CE_PRESSED) && (pfnRf4ceCallback != NULL))
		{
			/**********************************************************/
			/* 현재 Press된 Tick Value를 저장한다. Repeat Period를 벗어난 repeat key를	*/
			/* 제거하기 위함. thyou. 2007.10.24 							*/
			/**********************************************************/
			s_pRf4ceInfo->ulPreviousTickValue = VK_TIMER_GetSystemTick();

			PrintDebug("[DRV_RF4CE_Task] DecodedCode = %x is pressed\n", stRf4ceEventMSG.ucDecodedCode);
			/* 리모컨에서 한 번 Key를 누룰시 해당 Key를 mw에 올려준다. */
#if defined(CONFIG_DIRECTFB) && !defined(CONFIG_PRODUCT_IMAGE_HWTEST)
			pfnRf4ceCallback(&stRf4ceEventMSG);
			stRf4ceEventMSG.eRf4ceType = DI_RF4CE_RELEASED;
			pfnRf4ceCallback(&stRf4ceEventMSG);
#else
			pfnRf4ceCallback(&stRf4ceEventMSG);
#endif

			/**************************************************************/
			/* 보통 1번 IR Key를 눌렀는데 500ms 안에 repeat key가 발생하는 경우가 있다. 	*/
			/* 이렇게 되면 보통 key가 2번 눌린 것 처럼 발생하여 오동작을 한다. 이러한 증상을	*/
			/* 제거하기 위해서 한 번 key가 눌린 후 500ms 이내에 발생하는 Repeat key는 Noise  */
			/* 로 간주 하여 제거한다. Timer 사용										*/
			/**************************************************************/
			s_pRf4ceInfo->eNoiseStatus = RF4CE_NOISE_IS_NOT_REMOVED;

			if (VK_TIMERID_NULL != s_ulIRNoiseFilterTimerID)
			{
				nVkRet = VK_TIMER_Cancel(s_ulIRNoiseFilterTimerID);
				s_ulIRNoiseFilterTimerID = VK_TIMERID_NULL;
			}
			nVkRet = VK_TIMER_EventAfter(RF4CE_NOISE_FILTER_PERIOD, DRV_KEY_NoiseFilterTimer_Callback,
				NULL, 0,(unsigned long *) &s_ulIRNoiseFilterTimerID) ;

			if(nVkRet != VK_OK)
			{
				s_pRf4ceInfo->eNoiseStatus = RF4CE_NOISE_IS_REMOVED;
				PrintError("[DRV_RF4CE_Task] VK_TIMER_EventAfter error : ucReturnCode is 0x%x\n", nVkRet);
			}
		}
		else
		{
			/* Unknown Event */
			PrintError("[DRV_RF4CE_Task] Unknown Event\n");
		}
#elif defined(CONFIG_RF4CE_MICOM)
		/* send key to MW */
		if(pfnRf4ceCallback != NULL)
		{
			/*change key device to RCU_KEY from RF4CE_KEY */
			stRf4ceEventMSG.ulKeyCode = s_asRF4CEKeyCodeTable[stRf4ceEventMSG.ucDecodedCode].ulKeyCode;
			pfnRf4ceCallback(&stRf4ceEventMSG);
		}
#elif defined(CONFIG_RF4CE_EXTERNAL)
		/* send key to MW */
		if(pfnRf4ceCallback != NULL)
		{
			pfnRf4ceCallback(&stRf4ceEventMSG);
			PrintDebug("[DRV_RF4CE_Task] RF4CE_EXTERNAL / pfnRf4ceCallback != NULL\n");
		}
		else
		{
			PrintError("[DRV_RF4CE_Task] RF4CE_EXTERNAL / pfnRf4ceCallback == NULL\n");
		}
#else
#error "RF4CE Type is not defined"

#endif /* end of CONFIG_RF4CE_INTERNAL */
	}

	return ;
}


#if (defined(CONFIG_RF4CE_MICOM) || defined(CONFIG_RF4CE_EXTERNAL))
/*******************************************************************************
* function : DRV_RF4CE_Button_Callback
* description : micom으로 부터 올라온 RF4CE key를 처리하기 위한 callback.
* input : keyType(repeat, pressed(down), released(up), keycode
* output : none
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static void DRV_RF4CE_Button_Callback(HINT8 ucRepeatType, HUINT8 ucKeyCode)
{
	DI_RF4CE_EVENT_MSG_t stRf4ceEventMSG;
	int nVkRet = VK_OK;
	DI_RF4CE_EVENT_e eRf4ceEvent = 0;

	switch(ucRepeatType)
	{
		case 0:
			eRf4ceEvent = DI_RF4CE_EVENT_PRESSED;
			break;
		case 1:
			eRf4ceEvent = DI_RF4CE_EVENT_REPEATED;
			break;
		case 2:
			eRf4ceEvent = DI_RF4CE_EVENT_RELEASED;
			break;
		default:
			break;
	}

	stRf4ceEventMSG.ucDecodedCode = (HUINT8)ucKeyCode;
	stRf4ceEventMSG.eRf4ceEvent = eRf4ceEvent;

	nVkRet = VK_MSG_SendTimeout(s_ulRf4ceMsgQId, &stRf4ceEventMSG, sizeof(stRf4ceEventMSG),0);
	if(nVkRet != VK_OK)
	{
		PrintError( "[DRV_RF4CE_Button_Callback] VK_MSG_Send error : nVkRet is 0x%x\n", nVkRet);
	}

	return;
}
#endif

#if defined(CONFIG_RF4CE_MICOM) && defined(CONFIG_RF4CE_MOTION_TOUCH_UEI_HILCREST)
/*******************************************************************************
* function : DRV_RF4CE_RelMotion_Callback
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static void DRV_RF4CE_RelMotion_Callback(int nXDisplacement, int nYDisplacement, int nZDisplacement)
{
	DI_RF4CE_EVENT_MSG_t stRf4ceEventMSG;
	int nVkRet = VK_OK;

	VK_MEM_Memset(&stRf4ceEventMSG, 0x0, sizeof(DI_RF4CE_EVENT_MSG_t));

	stRf4ceEventMSG.eRf4ceEvent = DI_RF4CE_EVENT_REL_MOTION;
	stRf4ceEventMSG.nXDisplacement = nXDisplacement;
	stRf4ceEventMSG.nYDisplacement = nYDisplacement;
	stRf4ceEventMSG.nZDisplacement = nZDisplacement;

	nVkRet = VK_MSG_SendTimeout(s_ulRf4ceMsgQId, &stRf4ceEventMSG, sizeof(stRf4ceEventMSG),0);
	if(nVkRet != VK_OK)
	{
		PrintError( "[DRV_RF4CE_RelMotion_Callback] VK_MSG_Send error : nVkRet is 0x%x\n", nVkRet);
	}

	return;
}


#if 0
/*******************************************************************************
* function : DRV_RF4CE_AbsMotion_Callback
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static void DRV_RF4CE_AbsMotion_Callback(int nXDisplacement, int nYDisplacement, int nZDisplacement)
{
	DI_RF4CE_EVENT_MSG_t stRf4ceEventMSG;
	int nVkRet = VK_OK;
	DI_RF4CE_EVENT_e eRf4ceEvent = 0;

	VK_MEM_Memset(&stRf4ceEventMSG, 0x0, sizeof(DI_RF4CE_EVENT_MSG_t));

	stRf4ceEventMSG.eRf4ceEvent = DI_RF4CE_EVENT_ABS_MOTION;
	stRf4ceEventMSG.nXDisplacement = nXDisplacement;
	stRf4ceEventMSG.nYDisplacement = nYDisplacement;
	stRf4ceEventMSG.nZDisplacement = nZDisplacement;

	nVkRet = VK_MSG_SendTimeout(s_ulRf4ceMsgQId, &stRf4ceEventMSG, sizeof(stRf4ceEventMSG),0);
	if(nVkRet != VK_OK)
	{
		PrintError( "[DRV_RF4CE_RelMotion_Callback] VK_MSG_Send error : nVkRet is 0x%x\n", nVkRet);
	}

	return;
}
#endif
#endif  // defined(CONFIG_RF4CE_MICOM) && defined(CONFIG_RF4CE_MOTION_TOUCH_UEI_HILCREST)

/*******************************************************************************
* function : DI_RF4CE_RegisterButtonCallback
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
void DI_RF4CE_RegisterButtonCallback(pfnDI_RF4CE_Notify pfnNotify)
{
#if !defined(CONFIG_DIRECTFB) || defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	pfnRf4ceCallback = pfnNotify;
#else
	(void)(pfnNotify);
#endif
}

/*******************************************************************************
* function : DI_RF4CE_RegisterEventCallback
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DI_ERR_CODE DI_RF4CE_RegisterEventCallback(DI_RF4CE_NOTIFY_EVT_e eEvent, pfnRF4CE_EVT_CALLBACK pfnCallback)
{
	HINT32 nVkRet;

	if(eEvent >= DI_RF4CE_NOTIFY_EVT_MAX)
	{
		PrintError("DI_RF4CE_RegisterEventCallback] eEvent Error \n");
		return DI_ERR_ERROR;
	}

	nVkRet = VK_SEM_Get(g_tRF4CEInstance.ulSemID);
	if(nVkRet != VK_OK)
	{
		PrintError("[%s] VK_SEM_Get failed!\n",__func__);
		return DI_ERR_ERROR;
	}

	g_tRF4CEInstance.tCallbackInfo[eEvent].pfnCallback = pfnCallback;

	VK_SEM_Release(g_tRF4CEInstance.ulSemID);

	return DI_ERR_OK;
}

#if defined(CONFIG_DIRECTFB) && !defined(CONFIG_PRODUCT_IMAGE_HWTEST)
/*******************************************************************************
* function : DRV_RF4CE_DirectFB_Callback
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static void DRV_RF4CE_DirectFB_Callback(DI_RF4CE_EVENT_MSG_t *pstRf4ceEventMsg)
{
	key_t key_id;
	struct dfbmsgbuf sendbuf;

	key_id = msgget((key_t)24680, IPC_CREAT|0666);
	if (key_id == -1)
	{
		perror("msgget error");
		return;
	}

	VK_MEM_Memset(&sendbuf, 0x0, sizeof(struct dfbmsgbuf));

	sendbuf.msgtype = 3; /* 1 : Front Key, 2 :  IR, 3 : RF4CE */
	sendbuf.u.directfbrf4ce.device = 2;
	sendbuf.u.directfbrf4ce.code = pstRf4ceEventMsg->ulKeyCode;
	sendbuf.u.directfbrf4ce.type = pstRf4ceEventMsg->eRf4ceEvent;

	switch(pstRf4ceEventMsg->eRf4ceEvent)
	{
		case DI_RF4CE_EVENT_REL_MOTION:
		case DI_RF4CE_EVENT_ABS_MOTION:
			if(pstRf4ceEventMsg->nXDisplacement != 0)
			{
				sendbuf.u.directfbrf4ce.xdisplacement = pstRf4ceEventMsg->nXDisplacement;
				if (msgsnd( key_id, (void *)&sendbuf, sizeof(struct dfbmsgbuf) - sizeof(long), IPC_NOWAIT) == -1)
				{
					perror("msgsnd error");
				}
			}

			if(pstRf4ceEventMsg->nYDisplacement != 0)
			{
				sendbuf.u.directfbrf4ce.ydisplacement = pstRf4ceEventMsg->nYDisplacement;
				if (msgsnd( key_id, (void *)&sendbuf, sizeof(struct dfbmsgbuf) - sizeof(long), IPC_NOWAIT) == -1)
				{
					perror("msgsnd error");
				}
			}

			if(pstRf4ceEventMsg->nZDisplacement != 0)
			{
				sendbuf.u.directfbrf4ce.zdisplacement = pstRf4ceEventMsg->nZDisplacement;
				if (msgsnd( key_id, (void *)&sendbuf, sizeof(struct dfbmsgbuf) - sizeof(long), IPC_NOWAIT) == -1)
				{
					perror("msgsnd error");
				}
			}

			break;
		case DI_RF4CE_EVENT_PRESSED:
		case DI_RF4CE_EVENT_REPEATED:
		case DI_RF4CE_EVENT_RELEASED:
			if (msgsnd( key_id, (void *)&sendbuf, sizeof(struct dfbmsgbuf) - sizeof(long), IPC_NOWAIT) == -1)
			{
				perror("msgsnd error");
			}
			PrintDebug("%s sendbuf = [%x][%x][%x]\n", __FUNCTION__, sendbuf.u.directfbrf4ce.device, sendbuf.u.directfbrf4ce.code, sendbuf.u.directfbrf4ce.type);
			break;
		default:
			break;
	}
	return;
}

#endif // #if defined(CONFIG_DIRECTFB) && !defined(CONFIG_PRODUCT_IMAGE_HWTEST)
/* end of file */
