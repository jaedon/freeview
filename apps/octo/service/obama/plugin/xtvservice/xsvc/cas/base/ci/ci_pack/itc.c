/* $Header:   //Mars/source/pvcs/VM/BASE/archives/PDR_LIGHT/mw/cas/ci/ci_pack/ITC.c-arc   1.4   Aug 21 2004 08:31:46   wskim  $ */
/***************************************************************
* $Workfile:   ITC.c  $
* $Modtime:   Aug 21 2004 07:31:16  $
* Auther : Jaehee Cho
*
* Desc :
*
* Copyright (c) 2002 by Humax Co., Ltd.
* All right reserved
****************************************************************/

/***************************************************************
* #ifdef statements and include
***************************************************************/
/* global header file */
#include <vkernel.h> // cipack_msg.h 에서 include 하고 있기 때문에 제거되어야 맞다.

/* local header file */
#include "cipack_msg.h"
#include "itc.h"

#include "ci.h"

/***************************************************************
* definition
***************************************************************/




#define NUM_OF_CI_ITC_LAYER			5
#define CI_MAX_OF_TIMER				32
#define CI_ITC_TIMER_PERIOD			100 // modified by thyou.2007.10.12 : 10 --> 100, For EMMA2, 1 tick = 10 ms
#define CI_ITC_MSG_SIZE				16

/***************************************************************
* typedef
***************************************************************/
typedef unsigned long	TASK_HANDLE;
typedef unsigned long	MSG_HANDLE;
typedef unsigned long	SEM_HANDLE;

typedef void (*CI_TASK_FUNC)(void *pvParam);

typedef struct {
	TASK_HANDLE hTask;
	MSG_HANDLE hMsg;
} CI_ITC_LAYER_INFO;

typedef struct {
	int nTimerId;
	MSG_HANDLE hMsg;
	unsigned int unTime;
	unsigned int unTimeOut;
} CI_ITC_TIMER;

/***************************************************************
* external variables and structures
***************************************************************/
extern void ci_pack_physical(void *);				/* Entry point of PHYSICAL Task				*/
extern void ci_pack_link(void*);					/* Entry point of LINK Task					*/
extern void ci_pack_transport(void*);			/* Entry point of TRANS Task				*/
extern void ci_pack_session(void*);				/* Entry point of SESS Task					*/
extern void CIRH_Task(void*);			/* Entry point of CIRH Task				*/
//extern void simRM(void *);
//extern void simulRM(void *);


/***************************************************************
* external variables and structures
***************************************************************/

/***************************************************************
* global variables and structures
***************************************************************/

/***************************************************************
* static variables and structures
***************************************************************/
static	char *s_CiTaskName[NUM_OF_CI_ITC_LAYER] =
{
	"PHYS",
	"LINK",
	"TRAN",
	"SESS",
	//"SIRM"		// used temporarily for simualtion resource manager by shinhs
	"CIRH"
};

static	char *s_CiMsgQueName[NUM_OF_CI_ITC_LAYER] =
{
	"PHYS_Q",
	"LINK_Q",
	"TRAN_Q",
	"SESS_Q",
	//"SIRM"		// used temporarily for simualtion resource manager by shinhs
	"CIRH_Q"
};

static CI_TASK_FUNC s_pCiTaskFunc[NUM_OF_CI_ITC_LAYER] =
{
	ci_pack_physical,
	ci_pack_link,
	ci_pack_transport,
	ci_pack_session,
	//simulRM		// used termporarily for sumation resource manger by shinhs
	CIRH_Task
};

static unsigned int s_unCiTaskStack[NUM_OF_CI_ITC_LAYER] =
{
	(PAL_CI_ITC_LAYER_TASK_STACK_SIZE * 2),	// 2048,
	(PAL_CI_ITC_LAYER_TASK_STACK_SIZE * 1),	// 1024,
	(PAL_CI_ITC_LAYER_TASK_STACK_SIZE * 2),	// 2048,
	(PAL_CI_ITC_LAYER_TASK_STACK_SIZE * 2),	// 2048,
	(PAL_CI_ITC_LAYER_TASK_STACK_SIZE * 8)	// 8192
};

/*
static unsigned int s_unCiTaskStack[NUM_OF_CI_ITC_LAYER] =
{
	4096,
	4096,
	4096,
	4096,
	4096
};
*/

SEM_HANDLE			s_hCiTimerMgrSem;

CI_ITC_LAYER_INFO 	s_CiItcLayerInfo[NUM_OF_CI_ITC_LAYER];
static CI_ITC_TIMER	s_CiItcTimer[CI_MAX_OF_TIMER];
TASK_HANDLE			s_hItcTimerMgr;

/***************************************************************
* function prototypes
***************************************************************/
static MSG_HANDLE itc_GetMsgHandle(TASK_HANDLE hTask);
static void itc_TimerMgr(void *pvParam);
static CI_ITC_TIMER * itc_GetTimer(int nTimerId);
static CI_ITC_TIMER * itc_GetNewTimer(void);
//static void itc_DeleteTimer(int nTimerId);
#ifdef CI_VK_TASK_GETCURRENTID_WORKAROUND // 20070305 thyou : VK_TASK_GetCurrentId가 정상동작 하지 않아 아래와 같이 임시로 코드를 작성합.
static TASK_HANDLE itc_GetTaskHandle(CI_TASK_ID eTaskID);
#endif

/***************************************************************
* function bodies
***************************************************************/
void ItcKernelInit( void )
{
	int i;

	if (VK_SEM_Create(&s_hCiTimerMgrSem,"citimermgr", VK_SUSPENDTYPE_PRIORITY) != VK_OK)
	{
		HxLOG_Error("[CIITC]---> ItcKernelInit()---> Error : VK_SEM_Create() !!!\n");
		return;
	}

	HxSTD_memset(s_CiItcTimer, 0, sizeof(CI_ITC_TIMER) * CI_MAX_OF_TIMER);

	/* initialize layer info handles.. parkbi add.. 020819 */
	HxSTD_memset(s_CiItcLayerInfo, 0, sizeof(CI_ITC_LAYER_INFO) * NUM_OF_CI_ITC_LAYER);

	for (i=0; i<NUM_OF_CI_ITC_LAYER; i++)
	{
		if (VK_MSG_Create(PAL_CI_ITC_MSG_Q_SIZE, CI_ITC_MSG_SIZE, s_CiMsgQueName[i], &s_CiItcLayerInfo[i].hMsg, VK_SUSPENDTYPE_PRIORITY) != VK_OK)
		{
			HxLOG_Error("[CIITC]---> ItcKernelInit()---> Error : VK_MSG_Create() !!!\n");
			// to be added deleting msg q
			return;
		}
	}

	if (VK_TASK_Create(itc_TimerMgr,
						PAL_CI_ITC_TIMER_TASK_PRIORITY,
						PAL_CI_ITC_TIMER_TASK_STACK_SIZE,
						"ITC_T", NULL,
						&s_hItcTimerMgr, 0) != VK_OK)
	{
		HxLOG_Error("[CIITC]---> ItcKernelInit()---> Error : VK_TASK_Create(itc_TimerMgr) !!!\n");
		// to be added deleting task
		return;
	}

	if (VK_TASK_Start(s_hItcTimerMgr) != VK_OK)
	{
		HxLOG_Error("[CIITC]---> ItcKernelInit()---> Error : VK_TASK_Start(itc_TimerMgr) !!!\n");
		// to be added deleting task
		return;
	}

	for (i=0; i<NUM_OF_CI_ITC_LAYER; i++)
	{
		if (VK_TASK_Create(s_pCiTaskFunc[i],
							PAL_CI_ITC_LAYER_TASK_PRIORITY,
							s_unCiTaskStack[i],
							s_CiTaskName[i], NULL,
							&s_CiItcLayerInfo[i].hTask, 0) != VK_OK)
		{
			HxLOG_Error("[CIITC]---> ItcKernelInit()---> Error : VK_TASK_Create(%s) !!!\n", s_CiTaskName[i]);
			// to be added deleting task
			return;
		}
	}

	for (i=0; i<NUM_OF_CI_ITC_LAYER; i++)
	{
		if (VK_TASK_Start(s_CiItcLayerInfo[i].hTask) != VK_OK)
		{
			HxLOG_Error("[CIITC]---> ItcKernelInit()---> Error : VK_TASK_Start(%s) !!!\n", s_CiTaskName[i]);
			// to be added deleting task
			return;
		}
	}
}


int ITC_Init(unsigned char task )
{
	return 1;
}

int ITC_End(void)
{
	VK_TASK_Destroy(0);
	return 1;
}

#ifdef CI_VK_TASK_GETCURRENTID_WORKAROUND // 20070305 thyou : VK_TASK_GetCurrentId가 정상동작 하지 않아 아래와 같이 임시로 코드를 작성합.
int ITC_WaitEvt(unsigned short timeOut, unsigned char * evt, unsigned char * trans, xmsg *infos, CI_TASK_ID eTaskID)
#else
int ITC_WaitEvt(unsigned short timeOut, unsigned char * evt, unsigned char * trans, xmsg *infos )
#endif
{
	TASK_HANDLE	 hTask;
	MSG_HANDLE hMsg;
	unsigned long	ulMsg[4];
	int	nRet;

	/* parkbi add.  020820 */
	if( (evt == NULL)||(trans == NULL)||(infos == NULL) )
	{
		HxLOG_Error("[CIITC]---> ITC_WaitEvt()---> Error : parameter error !!!\n");
		return 0;
	}

#ifdef CI_VK_TASK_GETCURRENTID_WORKAROUND // 20070305 thyou : VK_TASK_GetCurrentId가 정상동작 하지 않아 아래와 같이 임시로 코드를 작성합.
    hTask = itc_GetTaskHandle(eTaskID);
	if (hTask == 0)
	{
		HxLOG_Error("[CIITC]---> ITC_WaitEvt()---> Error : VK_TASK_GetCurrentId() !!!\n");
		return 0;
	}
#else
	if (VK_TASK_GetCurrentId(&hTask) != VK_OK)
	{
		HxLOG_Error("[CIITC]---> ITC_WaitEvt()---> Error : VK_TASK_GetCurrentId() !!!\n");
		return 0;
	}
#endif

	hMsg = itc_GetMsgHandle(hTask);
	if (hMsg == (MSG_HANDLE)NULL)
	{
		HxLOG_Error("[CIITC]---> ITC_WaitEvt()---> Error : itc_GetMsgHandle() !!!\n");
		return 0;
	}

	if (timeOut == ITC_INFINITE)
	{
		nRet = VK_MSG_Receive(hMsg, (void *)ulMsg, CI_ITC_MSG_SIZE);
		if (nRet != VK_OK)
		{
			HxLOG_Error("[CIITC]---> ITC_WaitEvt()---> Error : VK_MSG_Receive() !!!\n");
			return 0;
		}
	}
	else
	{
		nRet = VK_MSG_ReceiveTimeout(hMsg, (void *)ulMsg, CI_ITC_MSG_SIZE, (unsigned long)timeOut);
		if (nRet != VK_OK)
		{
			if (nRet == VK_TIMEOUT)
			{
				/*
				*evt = TIMER;
				*trans = 0;
				infos->timer = 0;
				*/
				return 1;
			}

			HxLOG_Error("[CIITC]---> ITC_WaitEvt()---> Error : VK_MSG_ReceiveTimeout() !!!\n");
			return 0;
		}
	}

	if (nRet == VK_OK)
	{
		if (ulMsg[0] == MESSAGE)
		{
			*evt = (unsigned char)ulMsg[0];
			*trans = (unsigned char)ulMsg[1];
			HxSTD_memcpy((unsigned char *)infos, (unsigned char *)ulMsg[2], sizeof(xmsg));
			OxCAS_Free((unsigned char *)ulMsg[2]);
		}
		else if (ulMsg[0] == TIMER)
		{
			*evt = (unsigned char)ulMsg[0];
			*trans = 0;
			infos->timer = (unsigned short)ulMsg[1];
		}
		else
			HxLOG_Error("[CIITC]---> ITC_WaitEvt()---> Error : wrong event !!!\n");
	}

	return 1;
}

int ITC_MsgSend(unsigned char trans, unsigned char *txmsg )
{
	unsigned char	ucLayerId;
	unsigned long	ulMsg[4];

	/* parkbi add.. 020820 */
	if(txmsg == NULL)
	{
		HxLOG_Error("[CIITC]---> ITC_MsgSend()---> Error : parameter error !!!\n");
		return 0;
	}

	ucLayerId = (trans & 0x0f) - 1;

	ulMsg[0] = MESSAGE;
	ulMsg[1] = trans;
	ulMsg[2] = (unsigned long)OxCAS_Malloc(sizeof(xmsg));

	if(ulMsg[2] == (unsigned long)NULL)
	{
		HxLOG_Error("[CIITC]---> ITC_MsgSend(%d)---> Error : OxCAS_Malloc() !!!\n", ucLayerId);
		return 0;
	}
	HxSTD_memcpy((unsigned char *)ulMsg[2], txmsg, sizeof(xmsg));

#if 0
{
	xmsg *pMsg;

//	HxLOG_Print(("ITC(%d):MsgSend(%02x)\n", ucLayerId, trans));
	pMsg = (xmsg *)txmsg;
	HxLOG_Print(("\t[HEADER]:layerid[%d],trans[0x%x],type[0x%x],len[0x%x]\n",
				ucLayerId,
				pMsg->message.header.trans,
				pMsg->message.header.type,
				pMsg->message.header.len));

	switch (pMsg->message.header.type)
	{
	case PH_INSERT:
		HxLOG_Print(("\t[PHINSERT]:slot[%d],status[0x%x],size[0x%x]\n",
					pMsg->message.body.insert.slot_id,
					pMsg->message.body.insert.status,
					pMsg->message.body.insert.size));
		break;
	case PH_DATA:
		HxLOG_Print(("\t[PH_DATA]:slot[%d],len[0x%x],data[0x%x]\n",
					pMsg->message.body.data.slot_id,
					pMsg->message.body.data.len,
					pMsg->message.body.data.pdata[0]));
		break;
	case PH_ERROR:
		HxLOG_Print(("\t[PH_ERROR]:slot[%d],status[0x%x]\n",
					pMsg->message.body.error.slot_id,
					pMsg->message.body.error.status));
		break;
	case PH_EXTRACT	:
		HxLOG_Print(("\t[PH_EXTRACT]:slot[%d]\n",
					pMsg->message.body.extract.slot_id));
		break;
	/* end og physical layer */

	/* Link Layer */
	case LN_CONNECT	:
		HxLOG_Print(("\t[LN_CONNECT]:slot[%d],status[0x%x],size[0x%x]\n",
					pMsg->message.body.lconn.slot_id,
					pMsg->message.body.lconn.status,
					pMsg->message.body.lconn.size));
		break;
	case LN_DISCONNECT:
		HxLOG_Print(("\t[LN_DISCONNECT]:slot[%d]\n",
					pMsg->message.body.disc.slot_id));
		break;
	case LPDU:
		HxLOG_Print(("\t[LPDU]:slot[%d],len[0x%x]\n",
					pMsg->message.body.lpdu.slot_id,
					pMsg->message.body.lpdu.len));
		break;
	case LN_ERROR:
		HxLOG_Print(("\t[LN_ERROR]:slot[%d],status[0x%x]\n",
					pMsg->message.body.errhw.slot_id,
					pMsg->message.body.errhw.status));
		break;
	case LN_ACK:
		HxLOG_Print(("\t[LN_ACK]:slot[%d]\n",
					pMsg->message.body.ack.slot_id));
		break;
	/* end of link layer */

	/* Transport Layer */
	case TP_CONNECT:
		HxLOG_Print(("\t[TP_CONNECT]:slot[%d]\n",
					pMsg->message.body.conn.slot_id));
		break;
	case TP_DISCONNECT:
		HxLOG_Print(("\t[TP_DISCONNECT]:slot[%d]\n",
					pMsg->message.body.deconn.slot_id));
		break;
	case TPDU:
		HxLOG_Print(("\t[TPDU]:slot[%d],tcid[0x%x],len[0x%x]\n",
					pMsg->message.body.tpdu.slot_id,
					pMsg->message.body.tpdu.tcid,
					pMsg->message.body.tpdu.len));
		break;
	#ifdef FASTALLOC
	case TPDUDYN:
		break;
	#endif
	/* end of transport layer */

	/* Session Layer */
	case SS_TC_CREATED:
		HxLOG_Print(("\t[SS_TC_CREATED]:tcid[0x%x]\n",
					pMsg->message.body.creat.tcid));
		break;
	case SS_DELETE_TC:
		HxLOG_Print(("\t[SS_DELETE_TC]:tcid[0x%x]\n",
					pMsg->message.body.del.tcid));
		break;
	case SS_TC_DELETED:
		HxLOG_Print(("\t[SS_TC_DELETED]:tcid[0x%x]\n",
					pMsg->message.body.deleted.tcid));
		break;
	case SPDU:
		HxLOG_Print(("\t[SPDU]:tcid[0x%x],len[0x%x]\n",
					pMsg->message.body.spdu.tcid,
					pMsg->message.body.spdu.len));
		break;
	/* end of session layer */

	/* Resource Manager layer */
	case RM_TC_CREATED:
		HxLOG_Print(("\t[RM_TC_CREATED]:tcid[0x%x]\n",
					pMsg->message.body.sdeleted.tcid));
		break;
	case RM_OPEN_SS_REQ:
		HxLOG_Print(("\t[RM_OPEN_SS_REQ]:tcid[0x%x],ssnb[0x%x]\n",
					pMsg->message.body.open.tcid,
					pMsg->message.body.open.ssnb));
		break;
	case RM_OPEN_SS_RSP:
		HxLOG_Print(("\t[RM_OPEN_SS_RSP]:ssnb[0x%x], status[0x%x],rtcid[0x%x]\n",
					pMsg->message.body.openrsp.ssnb,
					pMsg->message.body.openrsp.status,
					pMsg->message.body.openrsp.rtcid));
		break;
	case RM_SS_OPENED:
		HxLOG_Print(("\t[RM_SS_OPENED]:ssnb[0x%x],tcid[0x%x]\n",
					pMsg->message.body.opened.ssnb,
					pMsg->message.body.opened.tcid));
		break;
	case APDU:
		HxLOG_Print(("\t[APDU]:ssnb[0x%x],len[0x%x]\n",
					pMsg->message.body.apdu.ssnb,
					pMsg->message.body.apdu.len));
		break;
	case RM_CLOSE_SS:
		HxLOG_Print(("\t[RM_CLOSE_SS]:ssnb[0x%x]\n",
					pMsg->message.body.close.ssnb));
		break;
	case RM_SS_CLOSED:
		HxLOG_Print(("\t[RM_SS_CLOSED]:ssnb[0x%x]\n",
					pMsg->message.body.close.ssnb));
		break;
	case RM_DELETE_TC:
		HxLOG_Print(("\t[RM_DELETE_TC]:tcid[0x%x]\n",
					pMsg->message.body.sdel.tcid));
		break;
	case RM_TC_DELETED:
		HxLOG_Print(("\t[RM_TC_DELETED]:tcid[0x%x]\n",
					pMsg->message.body.sdeleted.tcid));
		break;

	/* Timeout Messages Base */
	case TIMEOUT:	/* Transport layer timeout					*/
		HxLOG_Print(("\t[TIMEOUT]\n"));
		break;
	case TIMEPOLL:	/* Transport polling timeout				*/
	/* Ex : TIMEOUT/TIMEPOLL for tcid 3 = TIMEOUT/TIMEPOLL + tcid = 0x53/0x63	*/
		break;

	case TIMESESS:	/* Session layer timeout					*/
		HxLOG_Print(("\t[TIMESESS]\n"));
		break;
	case SSNBTIMER:	/* simultaneous TIMESESS timer count		*/
	/* Ex : TIMESESS : unique identifier between TIMESESS and TIMESESS + 16	--> 16 simultaneous active TIMESESS timers */
		break;

	/* WARNING : General messages values must be less than DVB messages values  */
	case MSG_MAXVAL:
		break;
	}
}
#endif

	if (VK_MSG_Send(s_CiItcLayerInfo[ucLayerId].hMsg, (void *)ulMsg, CI_ITC_MSG_SIZE) != VK_OK)
	{
		HxLOG_Error("[CIITC]---> ITC_MsgSend(%d)---> Error : VK_MSG_Send() !!!\n", ucLayerId);
		return 0;
	}

	return 1;
}

#ifdef CI_VK_TASK_GETCURRENTID_WORKAROUND // 20070305 thyou : VK_TASK_GetCurrentId가 정상동작 하지 않아 아래와 같이 임시로 코드를 작성합.
int ITC_SetTimer(unsigned short ident, unsigned short value, CI_TASK_ID eCITaskID)
#else
int ITC_SetTimer(unsigned short ident, unsigned short value)
#endif
{
	CI_ITC_TIMER *pTimer;
	unsigned long	ulTaskId;

	VK_SEM_Get(s_hCiTimerMgrSem);

	pTimer = itc_GetNewTimer();
	if (pTimer == NULL)
	{
		VK_SEM_Release(s_hCiTimerMgrSem);
		HxLOG_Error("[CIITC]---> ITC_SetTimer()---> Error : itc_GetNewTimer for timerId[0x%x] !!!\n", ident);
		return 0;
	}

	pTimer->nTimerId = ident;
#if 1
	pTimer->unTimeOut = value / CI_ITC_TIMER_PERIOD;
	if (pTimer->unTimeOut == 0)
		pTimer->unTimeOut = 1;
#else
	pTimer->unTimeOut = (value / CI_ITC_TIMER_PERIOD) + 1;
#endif
	pTimer->unTime = pTimer->unTimeOut;

#ifdef CI_VK_TASK_GETCURRENTID_WORKAROUND // 20070305 thyou : VK_TASK_GetCurrentId가 정상동작 하지 않아 아래와 같이 임시로 코드를 작성합.
	ulTaskId = itc_GetTaskHandle(eCITaskID);
	if (ulTaskId == 0)
	{
		VK_SEM_Release(s_hCiTimerMgrSem);
		HxLOG_Error("[CIITC]---> ITC_SetTimer()---> Error : itc_GetTaskHandle for timerId[0x%x] !!!\n", ident);
		return 0;
	}
#else
	if (VK_TASK_GetCurrentId(&ulTaskId) != VK_OK)
	{
		VK_SEM_Release(s_hCiTimerMgrSem);
		HxLOG_Error("[CIITC]---> ITC_SetTimer()---> Error : VK_TASK_GetCurrentId for timerId[0x%x] !!!\n", ident);
		return 0;
	}
#endif

	pTimer->hMsg = itc_GetMsgHandle(ulTaskId);
	if (pTimer->hMsg == 0)
	{
		VK_SEM_Release(s_hCiTimerMgrSem);
		HxLOG_Error("[CIITC]---> ITC_SetTimer()---> Error : itc_GetMsgHandle for timerId[0x%x] !!!\n", ident);
		return 0;
	}

	VK_SEM_Release(s_hCiTimerMgrSem);

	return ident ;
}

int ITC_KillTimer(int ident)
{
	CI_ITC_TIMER *pTimer;

	VK_SEM_Get(s_hCiTimerMgrSem);

	pTimer = itc_GetTimer(ident);
	if (pTimer == NULL)
	{
		HxLOG_Error("[CIITC]---> ITC_KillTimer()---> Warning : timerId[0x%x] already killed or not set !!!\n", ident);
		VK_SEM_Release(s_hCiTimerMgrSem);
		return 0;
	}

	pTimer->hMsg = (MSG_HANDLE)NULL;

	VK_SEM_Release(s_hCiTimerMgrSem);

	return 1;
}

unsigned char * ITC_DynAlloc(unsigned long size)
{
	return (unsigned char *)OxCAS_Malloc(size);
}

unsigned char * ITC_DynReAlloc(unsigned char * pt, unsigned long size)
{
	return OxCAS_Realloc((void *)pt, size);
}

int ITC_DynFree(unsigned char * pt)
{
	if(pt != NULL)
		OxCAS_Free(pt);

	return 1;
}

void ITC_Sleep(int delay)
{
	VK_TASK_Sleep(delay);
}


static MSG_HANDLE itc_GetMsgHandle(TASK_HANDLE hTask)
{
	int i;

	if(hTask == (TASK_HANDLE)NULL)
		return (MSG_HANDLE)NULL;

	for (i=0; i<NUM_OF_CI_ITC_LAYER; i++)
	{
		if (s_CiItcLayerInfo[i].hTask == hTask)
			return s_CiItcLayerInfo[i].hMsg;
	}

	return (MSG_HANDLE)NULL;
}

#ifdef CI_VK_TASK_GETCURRENTID_WORKAROUND // 20070305 thyou : VK_TASK_GetCurrentId가 정상동작 하지 않아 아래와 같이 임시로 코드를 작성합.
static TASK_HANDLE itc_GetTaskHandle(CI_TASK_ID eTaskID)
{
	if(eTaskID > CI_TASK_ID_RH)
		return (TASK_HANDLE)NULL;

	return s_CiItcLayerInfo[eTaskID].hTask;
}
#endif

static void itc_TimerMgr(void *pvParam)
{
	unsigned long ulMsg[4];
	int i;

	while (1)
	{
		VK_SEM_Get(s_hCiTimerMgrSem);
		for (i=0; i<CI_MAX_OF_TIMER; i++)
		{
			if (s_CiItcTimer[i].hMsg == (MSG_HANDLE)NULL)
				continue;

			if (--s_CiItcTimer[i].unTime == 0)
			{
				ulMsg[0] = TIMER;
				ulMsg[1] = s_CiItcTimer[i].nTimerId;

				s_CiItcTimer[i].unTime = s_CiItcTimer[i].unTimeOut;

				if (VK_MSG_SendTimeout(s_CiItcTimer[i].hMsg, (void *)ulMsg, CI_ITC_MSG_SIZE, 0) != VK_OK)
					s_CiItcTimer[i].unTime = 1;
			}
		}
		VK_SEM_Release(s_hCiTimerMgrSem);
		VK_TASK_Sleep(CI_ITC_TIMER_PERIOD);
	}
}

static CI_ITC_TIMER * itc_GetTimer(int nTimerId)
{
	int i;

	for (i=0; i<CI_MAX_OF_TIMER; i++)
	{
		if ((s_CiItcTimer[i].hMsg != (MSG_HANDLE)NULL)
			&& (s_CiItcTimer[i].nTimerId == nTimerId))
		{
			return &s_CiItcTimer[i];
		}
	}

	return NULL;
}

static CI_ITC_TIMER * itc_GetNewTimer(void)
{
	int i;

	for (i=0; i<CI_MAX_OF_TIMER; i++)
	{
		if (s_CiItcTimer[i].hMsg == (MSG_HANDLE)NULL)
			return &s_CiItcTimer[i];
	}

	return NULL;
}

/* 현재로선 사용되지 않아 막아둠.
static void itc_DeleteTimer(int nTimerId)
{
	int i;

	for (i=0; i<CI_MAX_OF_TIMER; i++)
	{
		if ((s_CiItcTimer[i].hMsg != (MSG_HANDLE)NULL)
			&& (s_CiItcTimer[i].nTimerId == nTimerId))
		{
			s_CiItcTimer[i].hMsg = (MSG_HANDLE)NULL;
		}
	}
}
*/

/**************** test print.. parkbi ******************/
#if 0
void CiItc_PrintTimerInfo(void)
{
	int i;
	for(i=0 ; i < CI_MAX_OF_TIMER; i++)
	{
		HxLOG_Error("s_CiItcTimer[%2d]::nTimerId:%d[0x%x], hMsg:0x%x, unTime:%d, unTimeOut:%d\n",
			i, s_CiItcTimer[i].nTimerId, s_CiItcTimer[i].nTimerId, s_CiItcTimer[i].hMsg, s_CiItcTimer[i].unTime, s_CiItcTimer[i].unTimeOut);
	}
}

void CiItc_PrintLayerInfo(void)
{
	int i;
	for(i=0 ; i < NUM_OF_CI_ITC_LAYER; i++)
	{
		HxLOG_Error("s_CiItcLayerInfo[%d]::hTask:0x%x, hMsg:0x%x\n",
			i, s_CiItcLayerInfo[i].hTask, s_CiItcLayerInfo[i].hMsg);
	}
}

#endif

/* end of file */
