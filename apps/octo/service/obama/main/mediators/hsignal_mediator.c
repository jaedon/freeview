
#include <vkernel.h>

#include "hsignal_mediator.h"

#define	HSIG_BUFFER_SIZE			16
#define	HSM_MSGNAME				"HSMSIG_MSG"
#define	HSM_SEMNAME				"HSMSIG_SEM"

typedef struct
{
	void				*userdata;
	HINT32				nPriority;
	CBHSMSIGHandler		notifyCb;
	HCHAR				szModuleName[32];
} HSMModuleInfo_t;
#if	0
typedef struct
{
	HUINT32				prio;
	HxList_t			*connectedSignalList;
	CBHSMSIGHandler		notifyCb;
	HCHAR 				modulename[128];
} HSMModuleInfo_t;
#endif

static HBOOL	sighsm_findListByModuleName	(void *moduleName, void *pvSigHsmInfo);
static HINT32 	sighsm_ItemSortCallback		(void *s1, void *s2);
//static HBOOL 	sighsm_findHsmInfo_bySignal(void *eSignal, void *pvSigHsmInfo);
static void 	notify_hsignal(HSIGNAL_e eSignal, HSIGNALTYPE_e eSignalType, HINT32 size);


static	HxList_t	*s_SigHsmModuleList = NULL;
//static 	HULONG		s_ulSigHsmMsgQId;
static	HULONG		s_ulHsmSemId;

static void octoHsm_hama_event_callback(HSIGNAL_e eSignal, HSIGNALTYPE_e eSignalType, HINT32 size, void *data)
{
	VK_SEM_Get(s_ulHsmSemId);
	notify_hsignal(eSignal, eSignalType, size);
	VK_SEM_Release(s_ulHsmSemId);

	(void)(data);
#if	0
	HSMMsg_t stMsg;

	stMsg.eSignal		=	eSignal;
	stMsg.eSignalType	=	eSignalType;
	stMsg.size			=	size;
	stMsg.data			=	(void*)data;

	VK_MSG_Send(s_ulSigHsmMsgQId, (void*)&stMsg, sizeof(HSMMsg_t));
#endif
	//notify_hsignal(dbtSignal, (HSMSIGNALTYPE_e)eSignalType, size, (void*)data);
}

/******************************************************************************/
/*****************************  Global Function *******************************/
/******************************************************************************/

HERROR 	HSMSIG_Main(void)		// signal distributor
{
	VK_SEM_Create((unsigned long*)&s_ulHsmSemId, HSM_SEMNAME, VK_SUSPENDTYPE_FIFO);

	//	MSG Queue를 사용하면 Signal을 놓친다.
	//VK_MSG_Create(HSIG_BUFFER_SIZE, sizeof(HSMMsg_t), HSM_MSGNAME, (unsigned long*)&s_ulSigHsmMsgQId, VK_SUSPENDTYPE_FIFO);

	//	Mediator를 사용하는 모듈이 필요한 Signal을 등록하는 것이 좋다.
	//	HAPI_ConnectSignal(eHSIG_ReadyShutDown);

	return HAPI_Register("octoMdtHapi",70, 128 * 1024,octoHsm_hama_event_callback);
}

#if	0		//	이런 함수가 존재하면 안된다. MSG QUEUE를 두군데서 사용하면 답이 안나옴.
HERROR	HSMSIG_GetMsgQueueId(HUINT32 *pulMsgQueueId)
{
	if (pulMsgQueueId == NULL)
	{
		return ERR_FAIL;
	}

	*pulMsgQueueId = s_ulSigHsmMsgQId;

	return ERR_OK;
}
#endif

HERROR	HSMSIG_Register(HCHAR *pszModuleName, HUINT32 prio, CBHSMSIGHandler pfnHandler, void *userdata)
{
	HxList_t	*list;
	HSMModuleInfo_t	*hsmInfo	=	NULL;

	VK_SEM_Get(s_ulHsmSemId);

	list = HLIB_LIST_FindEx(s_SigHsmModuleList, (void*)pszModuleName, sighsm_findListByModuleName);

	if(list)
	{
		HxLOG_Critical(" Already Registered Module ( %s )!!! \n", pszModuleName);
		return ERR_FAIL;
	}

	hsmInfo = HLIB_STD_MemAlloc(sizeof(HSMModuleInfo_t));

	HLIB_STD_StrNCpySafe(hsmInfo->szModuleName, pszModuleName, 32);
	hsmInfo->notifyCb	=	pfnHandler;
	hsmInfo->nPriority	=	prio;
	hsmInfo->userdata	=	userdata;

	// 낮은값이 priority 가 높다.
	s_SigHsmModuleList	=	HLIB_LIST_AppendSortEx(s_SigHsmModuleList, hsmInfo, sighsm_ItemSortCallback);

	VK_SEM_Release(s_ulHsmSemId);

	return ERR_OK;
}

//	모듈 별로 connected 를 관리 하느니 그냥 받는 모듈이 버리는것이 좋다.
//HERROR	HSMSIG_ConnectSignal(HCHAR *ModuleName, HSIGNALTYPE_e eSignal)
HERROR	HSMSIG_ConnectSignal(HSIGNAL_e eSignal)
{
	HAPI_ConnectSignal((HSIGNAL_e)eSignal);

#if	0
	HxList_t 	*list;
	HxList_t	*signalList;
	HSMModuleInfo_t	*hsmInfo	=	NULL;

	VK_SEM_Get(s_ulHsmSemId);

	list = HLIB_LIST_FindEx(s_SigHsmModuleList, (void*)ModuleName, sighsm_findListByModuleName);

	if(!list)
	{
		// 먼저 Registry 가 필요.
		HxLOG_Critical("Not Registered Module!! \n");
		return ERR_FAIL;
	}

	hsmInfo		=	HLIB_LIST_Data(list);
	signalList	=	HLIB_LIST_Find(hsmInfo->connectedSignalList, (void*)eSignal);

	if(signalList)
	{
		// 이미 등록된 시그널
		HxLOG_Critical("Already Connected Signal ( 0x%X ) !!\n", eSignal);
		return ERR_FAIL;
	}

	hsmInfo->connectedSignalList = HLIB_LIST_Append(hsmInfo->connectedSignalList, (void*)eSignal);

	HAPI_ConnectSignal((HSIGNAL_e)eSignal);

	VK_SEM_Release(s_ulHsmSemId);
#endif

	return ERR_OK;
}

HERROR	HSMSIG_SendSignalNumber(HSIGNAL_e eSignal, HUINT32 ulParam)
{
	return HAPI_SendSignalNumber((HSIGNAL_e)eSignal, ulParam);
}

HERROR	HSMSIG_SendSignalString(HSIGNAL_e eSignal, HCHAR *contents)
{
	return HAPI_SendSignalString((HSIGNAL_e)eSignal, (void*)contents);
}

HERROR	HSMSIG_SendSignalBinary(HSIGNAL_e eSignal, HINT32 size, HUINT8 *data)
{
	return HAPI_SendSignalBinary((HSIGNAL_e)eSignal, size, (void*)data);
}


/******************************************************************************/
/*****************************  Local Function ********************************/
/******************************************************************************/

static HBOOL	sighsm_findListByModuleName(void *moduleName, void *pvSigHsmInfo)
{

	HCHAR *pszModuleName = (HCHAR *)moduleName;
	HSMModuleInfo_t	*pstSigHsmInfo = (HSMModuleInfo_t*)pvSigHsmInfo;

	if (HxSTD_StrCmp(pszModuleName, pstSigHsmInfo->szModuleName) == 0)
		return TRUE;

	return FALSE;
}

static HINT32 sighsm_ItemSortCallback( void *s1, void *s2 )
{
	HSMModuleInfo_t	*pstDbtInfo1= ( HSMModuleInfo_t * )s1;
	HSMModuleInfo_t	*pstDbtInfo2= ( HSMModuleInfo_t * )s2;

	return	(pstDbtInfo2->nPriority - pstDbtInfo1->nPriority);
}

#if	0	//	모듈 별로 connected 를 관리 하느니 그냥 받는 모듈이 버리는것이 좋다.

static HBOOL sighsm_findHsmInfo_bySignal(void *eSignal, void *pvSigHsmInfo)
{
	HxList_t *list;

	HSIGNAL_e signal 			= (HSIGNAL_e )eSignal;
	HSMModuleInfo_t *pstHsmInfo = (HSMModuleInfo_t *)pvSigHsmInfo;

	list = HLIB_LIST_Find(pstHsmInfo->connectedSignalList, (void*)signal);

	if(list)
		return TRUE;

	return FALSE;
}
#endif

static void notify_hsignal(HSIGNAL_e eSignal, HSIGNALTYPE_e eSignalType, HINT32 size)
{
	HxList_t	*hsmList	= s_SigHsmModuleList;
	HSMModuleInfo_t	*pstHsmModuleInfo;

	while(hsmList)
	{
		pstHsmModuleInfo = (HSMModuleInfo_t*)HLIB_LIST_Data(hsmList);
		if (pstHsmModuleInfo && pstHsmModuleInfo->notifyCb)
			pstHsmModuleInfo->notifyCb(eSignal, eSignalType, size, (void*)pstHsmModuleInfo->userdata);

		hsmList	= hsmList->next;
	}
}

