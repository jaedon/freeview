
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/queue.h>

#include "htype.h"
#include "vkernel.h"
#include "taskdef.h"

#define DEBUG_MODULE			TRACE_MODULE_VK_TIMER

#ifdef INTR_DEBUG
#define PrintDebug(fmt, ...) 		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintError(fmt, ...)    	VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...)				VK_DBG_Print("(+)%s\n", __FUNCTION__)
#define PrintExit(...)				VK_DBG_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintDebug(fmt, ...)    	while (0) ((int (*)(char *, ...)) 0)
#define PrintError(fmt, ...)		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...) 			while (0) ((int (*)(char *, ...)) 0)
#define PrintExit(...)				while (0) ((int (*)(char *, ...)) 0)
#endif /* INTR_DEBUG */

#define INTR_TASK_STACK_SIZE	(1*1024*1024)

typedef struct INTR_LIST_t
{
	void 					(*CallBackFunc)(void *);
	char					name[16];
	unsigned long			argSize;
	unsigned long			intrId;
	unsigned long			msgId;
#if defined(CONFIG_DEBUG)
	struct INTR_LIST_t 		*head;
	struct INTR_LIST_t 		*next;
#endif
} INTR_LIST_T ;

typedef struct INTER_MSG_TYPE_t
{
	unsigned long	intrId;
	void			*pMsg;
} INTER_MSG_TYPE_T;

static unsigned long	s_intr_msg_id[VK_INTR_TASK_MAX];
static unsigned long	s_intr_task_id[VK_INTR_TASK_MAX];

#if defined(CONFIG_DEBUG)
static INTR_LIST_T 		*stIntrListHead;
static unsigned long	s_intr_sem_id;
#endif

/********************************************************************
 function: 	intr_task
 description :
 argument:
 return:
*********************************************************************/
static void intr_task(void *params)
{
	INTR_LIST_T			*pIntrList ;
	INTER_MSG_TYPE_T	msg;
	int					vkresult;
	unsigned long 		msg_id;
#if defined(CONFIG_DEBUG)
	unsigned long	startTick, endTick;
#endif

    PrintEnter();

	msg_id = *(unsigned long*)params;

	while(1)
	{
		VK_memset(&msg, 0, sizeof(INTER_MSG_TYPE_T));
		vkresult = VK_MSG_Receive(msg_id, &msg, sizeof(INTER_MSG_TYPE_T));
		if (vkresult != VK_OK)
		{
			if (msg.pMsg != NULL)
			{
				DD_MEM_Free(msg.pMsg);
				msg.pMsg = NULL;
			}
			continue;
		}

#if defined(CONFIG_DEBUG)
		startTick = VK_TIMER_GetSystemTick();
#endif
		pIntrList = (INTR_LIST_T*)msg.intrId;
		if (pIntrList->CallBackFunc)
        {
			pIntrList->CallBackFunc(msg.pMsg);
		}
		PrintDebug("[%s] intr_task%d : callback(%p) done\n", __FUNCTION__, pIntrList->msgId ,pIntrList->CallBackFunc);

#if defined(CONFIG_DEBUG)
		endTick = VK_TIMER_GetSystemTick();
		if (VK_TIMER_TimeAfter(startTick + 10, endTick) == VK_TIMER_EXPIRED)
		{
			PrintDebug("[intr_task] too long execution time(%ld) func = %08X\n", (endTick - startTick), (int)pIntrList->CallBackFunc);
		}
#endif

		if (msg.pMsg != NULL)
		{
			DD_MEM_Free(msg.pMsg);
			msg.pMsg = NULL;
		}
	}

    PrintExit();
} /* intr_task */

int VK_INTR_Init(void)
{
	int err;
	int i;
	char name[32] = {0,};

#if defined(CONFIG_DEBUG)
	err = VK_SEM_Create(&s_intr_sem_id, "INTR_SEM", DEFAULT_SUSPEND_TYPE) ;
	if (err != VK_OK)
	{
		PrintError("[%s] Error! fail to get sema\n", __FUNCTION__);
		return VK_ERROR;
	}
#endif

	for(i=0; i<VK_INTR_TASK_MAX; i++)
	{
		snprintf(name, sizeof(name)-1, "intr_msg%d", i);
		err = VK_MSG_Create(32, sizeof(INTER_MSG_TYPE_T), name, &s_intr_msg_id[i], VK_SUSPENDTYPE_FIFO);
		if(err != VK_OK)
		{
			PrintError("Error::VK_INTR_Init - VK_MSG_Create err = %d\n", err);
			return VK_CREATE_ERROR ;
		}

		snprintf(name, sizeof(name)-1, "intr_task%d", i);
		err = VK_TASK_Create(
				intr_task,
				USER_PRIORITY11,
				INTR_TASK_STACK_SIZE,
				name,
				(void *)&s_intr_msg_id[i],
				&s_intr_task_id[i], FALSE);
		if(err != VK_OK)
		{
			PrintError("Error::VK_INTR_Init - task create error = %d\n", err);
			return VK_CREATE_ERROR ;
		}
	}

	for(i=0; i<VK_INTR_TASK_MAX; i++) {
		VK_TASK_Start(s_intr_task_id[i]);
	}

	return VK_OK ;
}

int VK_INTR_Register(void (*CallBackFunc)(void *), const char *name, int LocalArgSize, unsigned long *intrId, VK_INTR_TASK_E task_num)
{
	INTR_LIST_T		*pIntrList ;

	PrintEnter();

	if (intrId == NULL || task_num >= VK_INTR_TASK_MAX)
	{
		return VK_ERROR;
	}

	pIntrList = (INTR_LIST_T *)DD_MEM_Alloc(sizeof(INTR_LIST_T)) ;
	if (pIntrList == NULL)
	{
		PrintError("Error::VK_INTR_Register - DD_MEM_Alloc:error intrList = %p\n", pIntrList);
		return VK_ALLOC_ERROR;
	}
	VK_memset(pIntrList, 0, sizeof(INTR_LIST_T));

	pIntrList->argSize = LocalArgSize;
	pIntrList->CallBackFunc = CallBackFunc;
	pIntrList->intrId = (unsigned long)pIntrList;
	pIntrList->msgId = (unsigned long)task_num;
	VK_strncpy(pIntrList->name, name, sizeof(pIntrList->name)-1);

	*intrId = (unsigned long)pIntrList;

#if defined(CONFIG_DEBUG)
	VK_SEM_Get(s_intr_sem_id);
	if (!stIntrListHead)
	{
		pIntrList->head = pIntrList;
		pIntrList->next = NULL;
		stIntrListHead = pIntrList;
	}
	else
	{
		INTR_LIST_T *p = stIntrListHead;
		while(p)
		{
			if (!p->next) break;
			p = p->next;
		}

		p->next = pIntrList;
		pIntrList->head = p;
	}
	VK_SEM_Release(s_intr_sem_id);
#endif

	PrintExit();

	return VK_OK ;
} /* VK_INTR_Register */

int	VK_INTR_Unregister(unsigned long intrId)
{
	INTR_LIST_T		*pIntrList ;

	PrintEnter();

	if (intrId == 0)
	{
		return VK_ERROR ;
	}

	pIntrList = (INTR_LIST_T*)intrId;
	if (pIntrList->intrId != (unsigned long)intrId)
	{
		return VK_ERROR;
	}

	pIntrList->intrId = 0;
#if defined(CONFIG_DEBUG)
	VK_SEM_Get(s_intr_sem_id);
	if (pIntrList->head == pIntrList)
	{
		// in case of head
		INTR_LIST_T *p = pIntrList->next;

		if (p) {
			p->head = p;
		}

		stIntrListHead = p;
	}
	else
	{
		// not head
		INTR_LIST_T *p = pIntrList->head;

		p->next = pIntrList->next;
	}
	VK_SEM_Release(s_intr_sem_id);
#endif

	DD_MEM_Free(pIntrList);

	PrintExit();
	return VK_OK ;
} /* VK_INTR_Unregister */

int VK_INTR_Event(unsigned long intrId, void *pMsg)
{
	INTR_LIST_T			*pIntrList ;
 	int					vkresult;
	INTER_MSG_TYPE_T	msg;
 	unsigned long		msgSize;
	void				*pArg;

	PrintEnter();

	if (intrId == 0)
	{
		return VK_ERROR ;
	}

	pIntrList = (INTR_LIST_T*)intrId;
	if (pIntrList->intrId != (unsigned long)intrId)
	{
		return VK_ERROR;
	}

	msgSize = pIntrList->argSize;

	if (msgSize != 0 && pMsg != NULL)
	{
		pArg = DD_MEM_Alloc(msgSize);
		if (pArg == NULL)
		{
			return VK_ERROR;
		}
		VK_memcpy(pArg, pMsg, msgSize);
	}
	else
	{
		pArg = NULL;
	}

	VK_memset(&msg, 0, sizeof(INTER_MSG_TYPE_T));
	msg.intrId = intrId;
	msg.pMsg = pArg;
	vkresult = VK_MSG_Send(s_intr_msg_id[pIntrList->msgId], &msg, sizeof(INTER_MSG_TYPE_T));
	if (vkresult != VK_OK)
	{
		return vkresult;
	}

	PrintExit();

	return VK_OK ;
} /* VK_INTR_Register */

#if defined(CONFIG_DEBUG)
void VK_INTR_PrintIntrList(void)
{
	INTR_LIST_T *p;

	VK_SEM_Get(s_intr_sem_id);
	p = stIntrListHead;
	VK_printf("\n##################################################################");
	VK_printf("\n##################       VK INTR LIST        #####################");

	while(p)
	{
		char str[128];

		snprintf(str, sizeof(str)-1, "Name(%s)\tIntrTask%d\tCallback(0x%x)", p->name, (int)p->msgId, (unsigned int)p->CallBackFunc);
		VK_printf("\n##  %s", str);

		p = p->next;
	}
	VK_printf("\n\n");
	VK_SEM_Release(s_intr_sem_id);
}
#endif
