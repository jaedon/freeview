/* $Header:   //BASE/archives/Group_MHP/Premium_MHP/mw/cas/satsa/satsa_sci/satsa_sci.c-arc   1.3   Jul 06 2007 09:19:56   chlim  $ */
/******************************************************************************
 * $Workfile:   satsa_sci.c  $
 * $Modtime:   Jun 28 2007 14:12:02  $
 * Author : ChangHyun Lim(POOH)
 * description : SATSA interface
 *
 * Copyright (c) 2006 by Humax Co., Ltd.
 * All right reserved
 ******************************************************************************/

/*****************************************************************************
							include file
******************************************************************************/

#include "octo_common.h"
#include "pal_sc.h"
#include "_svc_cas_err.h"

#include "satsa_sci.h"
#include "satsa_sci_adapt.h"

/**************************************************************************************************/
#define ________SATSA_SCI_Golbal_Value________________________________________________________
/**************************************************************************************************/



/**************************************************************************************************/
#define ________SATSA_SCI_Private_Define_______________________________________________________
/**************************************************************************************************/



/*****************************************************************************
							typedef
******************************************************************************/

/*****************************************************************************
							external variable declaration
******************************************************************************/

/*****************************************************************************
							global/static variable declaration
******************************************************************************/
static void SATSA_SCI_Task(void *args);

#define	SATSA_TASK_PRIORITY			10//USER_PRIORITY9	//IR :6 NA :10 CX :9
#define	SATSA_TASK_STACK_SIZE		2048
#define	SATSA_TASK_NAME				"satsaTask"

#define	SATSA_QUEUE_STACK_SIZE		256
#define	SATSA_QUEUE_NAME			"satsaQueue"

static HUINT32	satsaTaskId;
static HUINT32	satsaQueueId;
static HUINT32	satsaSemId;

static HUINT8 *SECURE_MESSAGE = "SECU";
static HUINT8 *preMessage;
/*****************************************************************************
							static function prototype
******************************************************************************/
//void INT_SCI_SetSecureStatus(HUINT8 flag);

/*****************************************************************************
							static function body
******************************************************************************/

static void SATSA_SCI_Task(void *args)
{
	SATSA_MSG	InMsg;

	while (1)
	{
		if (SATSA_MSG_Receive(Get_satsaQueueId(), (void *)&InMsg, sizeof(SATSA_MSG)) == ERR_OK)
		{
			HxLOG_Print("*************** InMsg.wMsg = %x, InMsg.nMsg = %x, InMsg.lParam1 = %x, InMsg.lParam2 = %x, InMsg.lParam3 = %x, InMsg.lParam4 = %x***************\r\n\n\r"
																			, InMsg.wMsg, InMsg.nMsg, InMsg.lParam1, InMsg.lParam2, InMsg.lParam3, InMsg.lParam4));
			switch (InMsg.wMsg)
			{
				case MSG_SCI_FROM_UI:
					switch (InMsg.nMsg)
					{
						case SET_SECURE_STATUS:
							HxLOG_Print(" SET_SECURE_STATUS = %x \r\n ",InMsg.lParam1);
							//INT_SCI_SetSecureStatus(InMsg.lParam1);
							break;

						default:
							break;
					}
					break;

				case MSG_SCI_FROM_SC:
					switch (InMsg.nMsg)
					{
						case SCI_FROM_SCMGR_INSERT:
						case SCI_FROM_SCMGR_EXTRACT:
						case SCI_FROM_SCMGR_MUTE:
						case SCI_FROM_SCMGR_INVALID:
						case SCI_FROM_SCMGR_DENIED:
							MW_Satsa_Sci_Adapt_cardStatus(InMsg.lParam1,InMsg.nMsg);
						default:
							HxLOG_Print(" MSG_SCI_FROM_SC SlotID = %x , status = %x \r\n\n\r",InMsg.lParam1,InMsg.nMsg);
							break;
					}
					break;

				default:
					HxLOG_Print(" Other Message \r\n\n\r");
					break;
			}
		}
	}
}

#if 0
void INT_SCI_SetSecureStatus(HUINT8 flag)
{
	HUINT32 errRet;

	if (flag == TRUE)
	{
		errRet = DCM_USRIO_SetLedState(LED_ID_STANDBY, DCM_LED_STATE_BLINK);

		if (errRet != ERR_OK)
		{
			HxLOG_Info("[Satsa_Ui_setSecureStatus] ERROR [%x]\r\n",errRet);
		}else
			HxLOG_Print("[Satsa_Ui_setSecureStatus] Standby Led ON \r\n");

		errRet = DCM_USRIO_GetDisplayString(preMessage);

		if (errRet != ERR_OK)
		{
			HxLOG_Info("[Satsa_Ui_setSecureStatus] ERROR [%x]\r\n",errRet);
		}else
			HxLOG_Print("[Satsa_Ui_setSecureStatus] DisplayPanel \r\n");

		errRet = DCM_USRIO_DisplayPanel(SECURE_MESSAGE);

		if (errRet != ERR_OK)
		{
			HxLOG_Info("[Satsa_Ui_setSecureStatus] ERROR [%x]\r\n",errRet);
		}else
			HxLOG_Print("[Satsa_Ui_setSecureStatus] DisplayPanel \r\n");

	}
	else
	{
		errRet = DCM_USRIO_SetLedState(LED_ID_STANDBY, LED_STATE_OFF);

		if (errRet != ERR_OK)
		{
			HxLOG_Info("[Satsa_Ui_setSecureStatus] ERROR [%x]\r\n",errRet);
		}else
			HxLOG_Print("[Satsa_Ui_setSecureStatus] Standby Led OFF \r\n");

		errRet = DCM_USRIO_DisplayPanel(preMessage);

		if (errRet != ERR_OK)
		{
			HxLOG_Info("[Satsa_Ui_setSecureStatus] ERROR [%x]\r\n",errRet);
		}else
			HxLOG_Print("[Satsa_Ui_setSecureStatus] DisplayPanel \r\n");
	}
}
#endif
static void Satsa_Sci_CreateSem(void)
{
	HUINT32 	errcode;

	HxLOG_Print("[+Satsa_Sci_CreateSem]\n");

	errcode = (HUINT32)SATSA_SEM_Create((unsigned long*)&satsaSemId, "satsa_sci");
	if (errcode != ERR_OK)
		HxLOG_Print(" Error in Creating Sem : Smart Card\n");

	HxLOG_Print("\t[-Satsa_Sci_CreateSem]\n");

}

static void Satsa_Sci_CreateQueue(void)
{
	HUINT32 	errcode;

	HxLOG_Print("[+Satsa_Sci_CreateQueue]\n");

	errcode = (HUINT32)SATSA_MSG_Create(
						SATSA_QUEUE_STACK_SIZE,
						sizeof(SATSA_MSG),
						(char *)SATSA_QUEUE_NAME,
						(unsigned long *)&satsaQueueId);

	if (errcode != ERR_OK)
		HxLOG_Print(" Error in Creating Queue : Smart Card\n");

	HxLOG_Print("\t[-Satsa_Sci_CreateQueue]\n");

}

static void Satsa_Sci_StartTask(void)
{
	int	err;

	HxLOG_Print("[+Satsa_Sci_StartTask]\n");

	err = VK_TASK_Create(
			SATSA_SCI_Task,
			SATSA_TASK_PRIORITY,
			SATSA_TASK_STACK_SIZE,
			(char *) SATSA_TASK_NAME,
			(void*)NULL,
			(unsigned long *)&satsaTaskId, 0);

	if (err)
		HxLOG_Info("[Satsa_Sci_StartTask] Error in creating CARD task 0x%02x\n", err);

	err = VK_TASK_Start(satsaTaskId);
	if (err)
		HxLOG_Info("[Satsa_Sci_StartTask] CARD Task Start Error  0x%02x\n", err);

	HxLOG_Print("\t[-Satsa_Sci_StartTask]\n");

}


/*****************************************************************************
							global function body
******************************************************************************/
HUINT32 Get_satsaTaskId()
{
	return satsaTaskId;
}

HUINT32 Get_satsaQueueId()
{
	return satsaQueueId;
}

HUINT32 Get_satsaSemId()
{
	return satsaSemId;
}

void local_Satsa_Sci_Init(void)
{
	Satsa_Sci_CreateSem();
	Satsa_Sci_CreateQueue();
	Satsa_Sci_StartTask();
}

HERROR xsvc_cas_SatsaInit(void)
{
	int ulErr;

///////////////////////////////////////////////////////////////////////////////////////
	local_Satsa_Sci_Init();
///////////////////////////////////////////////////////////////////////////////////////
	ulErr = MW_Satsa_Sci_Adapt_StartCard();	//SC MGR
	if(ulErr)
		HxLOG_Info("\r\n\t[Satsa_Sci_Init] ERROR Satsa_Sci_Adapt_StartCard\n\r");

	return ERR_OK;
}

