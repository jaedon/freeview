/*
 * Copyright (c) 2006 HUMAX Co., Ltd. All rights reserved.
 *
 * This is very confidential.
 * This code must not be distributed outside.
 */

#include "stdio.h"
#include "string.h"
#include "errno.h"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#include "vkernel.h"
#if defined(CONFIG_DEBUG)
#include "cmdtool.h"
#endif

#define VK_IPC_CB_MASK		0x40000000
#define VK_IPC_CB_ACK(x)	((x) | 0x40000000)
#define VK_IPC_RCV_ACK		0xC0000000

#define DEBUG_MODULE			TRACE_MODULE_VK_TASK

#define VK_UNUSED(x) ((x)=(x))

#if defined(CONFIG_DEBUG)
//#define IPC_DEBUG
#endif

#ifdef IPC_DEBUG
#define PrintDebug(fmt, ...) 		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintError(fmt, ...)    	VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...)				VK_DBG_Print("(+)%s\n", __FUNCTION__)
#define PrintExit(...)				VK_DBG_Print("(-)%s\n", __FUNCTION__)
#else 
#define PrintDebug(fmt, ...)    	while (0) ((int (*)(char *, ...)) 0)
#define PrintError(fmt, ...)		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...) 			while (0) ((int (*)(char *, ...)) 0)				
#define PrintExit(...)				while (0) ((int (*)(char *, ...)) 0)			
#endif /* MEM_DEBUG */

#ifndef bool
#define bool unsigned int
#endif
#ifndef TRUE
#define TRUE 	1
#endif
#ifndef FALSE
#define FALSE	0
#endif

#define IPC_MSG_ACK					"^^ACK"

#define	VK_ERR_IPC_SND_EAGAIN		2		/* queue is full, and IPC_NOWAIT was asserted */
#define VK_ERR_IPC_SND_EACCES		3		/* permission denied, no write permission */
#define VK_ERR_IPC_SND_EFAULT		4		/* msgp address isn't accessable - invalid */
#define VK_ERR_IPC_SND_EIDRM		5		/* the message queue has been removed */
#define VK_ERR_IPC_SND_EINTR		6		/* received a signal while waiting to write */
#define VK_ERR_IPC_SND_EINVAL		7		/* invalid message queue identifier, nonpositive message type, or invalid message size */
#define VK_ERR_IPC_SND_ENOMEM		8		/* not enough memory to copy message buffer */

#define VK_ERR_IPC_RCV_E2BIG		9		/* message length is greater than msgsz, no MSG_NOERROR) */
#define VK_ERR_IPC_RCV_EACCES		10 		/* no read permission */
#define VK_ERR_IPC_RCV_EFAULT		11		/* address pointed to by msgp is invalid */
#define VK_ERR_IPC_RCV_EIDRM		12 		/* queue was removed during retrieval */
#define VK_ERR_IPC_RCV_EINTR		13		/* interrupted by arriving signal */
#define VK_ERR_IPC_RCV_EINVAL		14		/* msgqid invalid, or msgsz less than 0 */
#define VK_ERR_IPC_RCV_ENOMSG		15		/* IPC_NOWAIT asserted, and no message exits in the queue to satisfy the request */

#define VK_MSG_TYPE_MSG				102

#define VK_IPC_MSG_CALLBACK_INDEX_MAX			100
#define VK_IPC_MSG_QUEUE_INDEX_MAX				100

typedef struct hmsbuf 
{ 
    unsigned int	msgtype; 
	unsigned char	mtext[1024]; 
} HMSBUF_T; 

typedef struct VK_IPC_NOTI_t
{
	pfnVK_IPC_MSG_NotifyFUNC	CB_IPCNotify;
	unsigned int				callbackId;
} VK_IPC_NOTI_T;

typedef struct VK_IPC_CALLBACK_t
{
	VK_IPC_NOTI_T				noti_function[VK_IPC_MSG_CALLBACK_INDEX_MAX];
	unsigned int				queue_id;
	unsigned int				task_id;
	unsigned int				sema_id;
} VK_IPC_CALLBACK_T;

typedef struct VK_IPC_CALL_IN_t
{
	unsigned int				callbackId;
	unsigned int				msg_size;
	unsigned char				szMsg[1024-4*2];
} VK_IPC_CALL_IN_T;

typedef struct VK_IPC_CALL_OUT_t
{
	unsigned int				vkResult;
	unsigned int				result;
	unsigned char				szMsg[1024-4*2];
} VK_IPC_CALL_OUT_T;
	
static VK_IPC_CALLBACK_T		s_IPCNotify[VK_IPC_MSG_QUEUE_INDEX_MAX];

int VK_IPC_MSG_Init(void)
{
	return VK_OK;
}

int P_IPC_MSG_ReceiveTimeout(unsigned int q_id, unsigned int msgtype, void *msg, unsigned int size, unsigned int timeout);
int VK_IPC_MSG_Open(unsigned long q_count,unsigned long msg_size, const char *q_name, int *q_id)
{
	key_t key_idid=-1;
	int			i;
	int			len;
	unsigned int	unique_id;

	HAPPY(q_count);
	HAPPY(msg_size);

	PrintEnter();

	if (q_name == NULL)
	{
		PrintError("%s[%d] [VK_IPC_MSG_Open] error : null q_name!!\n", __FUNCTION__, __LINE__);
		return VK_ERROR;
	}

	len = strlen(q_name);
	if (len == 0)
	{
		PrintError("%s[%d] [VK_IPC_MSG_Open] error : null q_name!!\n", __FUNCTION__, __LINE__);
		return VK_ERROR;
	}

	unique_id = 0;
	for (i=0; i<len; i++)
	{
		unique_id = unique_id * unique_id;
		unique_id |= q_name[i];
	}
	
	/*
	* IPC_CREAT : to create a new message queue. If the key which is already created existed,it will return back the message queue identifyer.
	* IPC_EXCL : using with IPC_CREATE together. If you use IPC_EXCL and the key is aready existing, it will return -1 back and set the errno.
	*/
	key_idid = msgget((key_t)unique_id, IPC_CREAT|0666);
	if (key_idid == -1)
	{
		PrintError("%s[%d] : msgget error !!\n", __FUNCTION__, __LINE__);
		return VK_ERROR;		
	}
	PrintDebug("[%d:%s] q_id = %d key %d\n", __LINE__, __FUNCTION__, key_idid, key);

	*q_id = key_idid+1;

	PrintExit();
	return VK_OK;
}

int P_IPC_MSG_Send(unsigned int q_id, unsigned int msgtype, const void *msg, unsigned int size)
{
	int rc;
	HMSBUF_T *sndbuf;

	PrintEnter();
	
	if (msg==NULL)
	{
		PrintError("%s[%d] invalid buffer error !!\n", __FUNCTION__, __LINE__);
		return VK_ERROR;
	}
	
	sndbuf = DD_MEM_Alloc(sizeof(HMSBUF_T));
	if (sndbuf == NULL)
	{
		return VK_ERROR;
	}
	memset(sndbuf, 0x00, sizeof(HMSBUF_T)); 
	memcpy(sndbuf->mtext, msg, size); 

	sndbuf->msgtype = msgtype; 
		
	/*
	* IPC_NOWAIT :  if it's configured, then the call instead fails with the error EAGAIN.
	*/
	/*
	* CAUTION : (sizeof(HMSBUF_T)-sizeof(unsigned int)), the "sizeof(unsigned int)" means the argument "unsigned int msgtype" in HMSBUF_T structure.
	* 			 if we don't subtract this, it's going to crash the memory on running time.
	*/
	rc = msgsnd(q_id-1, (void *)sndbuf, (sizeof(HMSBUF_T)-sizeof(unsigned int)), 0); // BLOCK : 0, NONBLOCK : IPC_NOWAIT
	if (rc==-1)
	{
		if (errno==EAGAIN)
		{
			rc = VK_ERR_IPC_SND_EAGAIN; 
		}
		else if(errno==EACCES)
		{
			rc = VK_ERR_IPC_SND_EACCES;	
		}
		else if(errno==EFAULT)
		{
			rc = VK_ERR_IPC_SND_EFAULT;	
		}
		else if(errno==EIDRM)
		{
			rc = VK_ERR_IPC_SND_EIDRM;	
		}
		else if(errno==EINTR)
		{
			rc = VK_ERR_IPC_SND_EINTR;	
		}
		else if(errno==EINVAL)
		{
			rc = VK_ERR_IPC_SND_EINVAL; 
		}
		else if(errno==ENOMEM)
		{
			rc = VK_ERR_IPC_SND_ENOMEM;
		}
		else
		{
			rc = VK_ERROR;
		}
		PrintError("%s[%d] : msgsnd error NO : %d !!\n", __FUNCTION__, __LINE__, rc);
		DD_MEM_Free(sndbuf);
		return VK_ERROR;
	}
	DD_MEM_Free(sndbuf);
	PrintExit();	
	return VK_OK;
}

int P_IPC_MSG_SendTimeout(unsigned int q_id, unsigned int msgtype, const void *msg, unsigned int size, unsigned int timeout)
{
	int rc;
	HMSBUF_T *sndbuf;
	unsigned long timeoutTick, currTick;
	VK_TIMER_STATE	timeAfter;

	PrintEnter();
	if (msg==NULL) {
		PrintError("%s[%d] invalid buffer error !!\n", __FUNCTION__, __LINE__);
		return VK_ERROR;
	}

	sndbuf = DD_MEM_Alloc(sizeof(HMSBUF_T));
	if (sndbuf == NULL) {
		return VK_ERROR;
	}
			
	memset(sndbuf, 0x00, sizeof(HMSBUF_T));  
	memcpy(sndbuf->mtext, msg, size); 
	sndbuf->msgtype = msgtype; 
	timeoutTick = VK_TIMER_GetSystemTick() + timeout;
	while (1)
	{
		/*
		 * CAUTION : (sizeof(HMSBUF_T)-sizeof(unsigned int)), the "sizeof(unsigned int)" means the argument "unsigned int msgtype" in HMSBUF_T structure.
		 * 			 if we don't subtract this, it's going to crash the memory on running time.
		 */
		rc = msgsnd(q_id-1, (void *)sndbuf, (sizeof(HMSBUF_T)-sizeof(unsigned int)), IPC_NOWAIT); // BLOCK : 0, NONBLOCK : IPC_NOWAIT
		if (rc==-1)
		{
#if defined(CONFIG_DEBUG)
			if (errno==EAGAIN)
			{
				rc = VK_ERR_IPC_SND_EAGAIN; 
			}
			else if(errno==EACCES)
			{
				rc = VK_ERR_IPC_SND_EACCES;	
			}
			else if(errno==EFAULT)
			{
				rc = VK_ERR_IPC_SND_EFAULT;	
			}
			else if(errno==EIDRM)
			{
				rc = VK_ERR_IPC_SND_EIDRM;	
			}
			else if(errno==EINTR)
			{
				rc = VK_ERR_IPC_SND_EINTR;	
			}
			else if(errno==EINVAL)
			{
				rc = VK_ERR_IPC_SND_EINVAL; 
			}
			else if(errno==ENOMEM)
			{
				rc = VK_ERR_IPC_SND_ENOMEM;
			}
			else
			{
				rc = VK_ERROR;
			}
			
			PrintError("%s[%d] : msgsnd error NO : %d !!\n", __FUNCTION__, __LINE__, rc);
#endif
			currTick = VK_TIMER_GetSystemTick();
			timeAfter = VK_TIMER_TimeAfter(timeoutTick, currTick);
			if (VK_TIMER_EXPIRED == timeAfter)
			{
				DD_MEM_Free(sndbuf);
				return VK_TIMEOUT;
			}
			VK_TASK_Sleep(2);
		}
		else
		{
			break;
		}
	}
	DD_MEM_Free(sndbuf);

	PrintExit();
	return VK_OK;
}

int P_IPC_MSG_Receive(unsigned int q_id, unsigned int msgtype, const void *msg, unsigned int size)
{
	int rc;
	HMSBUF_T *rcvbuf; 
	
	PrintEnter();
	rcvbuf = DD_MEM_Alloc(sizeof(HMSBUF_T));
	if (rcvbuf == NULL)
	{
		return VK_ERROR;
	}
	
	memset(rcvbuf, 0x00, sizeof(HMSBUF_T));
	/*
	 * CAUTION : (sizeof(HMSBUF_T)-sizeof(unsigned int)), the "sizeof(unsigned int)" means the argument "unsigned int msgtype" in HMSBUF_T structure.
	 * 			 if we don't subtract this, it's going to crash the memory on running time.
	 */
	rc = msgrcv(q_id-1, (void *)rcvbuf, (sizeof(HMSBUF_T)-sizeof(unsigned int)), msgtype, 0); // BLOCK : 0, NONBLOCK : IPC_NOWAIT
	if (rc==-1)
	{
		if (errno==E2BIG)
		{
		}
		else if(errno==EACCES)
		{
			rc = VK_ERR_IPC_RCV_EACCES;
		}
		else if(errno==EFAULT)
		{
			rc = VK_ERR_IPC_RCV_EFAULT;
		}
		else if(errno==EIDRM)
		{
			rc = VK_ERR_IPC_RCV_EIDRM;
		}
		else if(errno==EINTR)
		{
			rc = VK_ERR_IPC_RCV_EINTR;
		}
		else if(errno==EINVAL)
		{
			rc = VK_ERR_IPC_RCV_EINVAL;
		}
		else if(errno==ENOMSG)
		{
			rc = VK_ERR_IPC_RCV_ENOMSG;
		}
		else
		{
			rc = VK_ERROR;
		}
		PrintError("%s[%d] : msgrcv error NO : %d !!\n", __FUNCTION__, __LINE__, rc);
		DD_MEM_Free(rcvbuf);
		return VK_ERROR;
	}

	if (msg)
	{
		VK_memcpy((void *)msg, rcvbuf->mtext, size);
	}

	DD_MEM_Free(rcvbuf);
	PrintExit();
	return VK_OK;
}

int P_IPC_MSG_ReceiveTimeout(unsigned int q_id, unsigned int msgtype, void *msg, unsigned int size, unsigned int timeout)
{
	int rc;
	HMSBUF_T *rcvbuf;
	unsigned long timeoutTick, currTick;
	VK_TIMER_STATE	timeAfter;

	PrintEnter();
	rcvbuf = DD_MEM_Alloc(sizeof(HMSBUF_T));
	if (rcvbuf == NULL)
	{
		return VK_ERROR;
	}

	timeoutTick = VK_TIMER_GetSystemTick() + timeout;
	while (1)
	{
		memset(rcvbuf, 0x00, sizeof(HMSBUF_T));
		/*
		 * CAUTION : (sizeof(HMSBUF_T)-sizeof(unsigned int)), the "sizeof(unsigned int)" means the argument "unsigned int msgtype" in HMSBUF_T structure.
		 * 			 if we don't subtract this, it's going to crash the memory on running time.
		 */
		rc = msgrcv(q_id-1, (void *)rcvbuf, (sizeof(HMSBUF_T)-sizeof(unsigned int)), msgtype, IPC_NOWAIT); // BLOCK : 0, NONBLOCK : IPC_NOWAIT
		if (rc==-1)
		{
			if (errno==E2BIG)
			{
			}
			else if(errno==EACCES)
			{
				rc = VK_ERR_IPC_RCV_EACCES;
			}
			else if(errno==EFAULT)
			{
				rc = VK_ERR_IPC_RCV_EFAULT;
			}
			else if(errno==EIDRM)
			{
				rc = VK_ERR_IPC_RCV_EIDRM;
			}
			else if(errno==EINTR)
			{
				rc = VK_ERR_IPC_RCV_EINTR;
			}
			else if(errno==EINVAL)
			{
				rc = VK_ERR_IPC_RCV_EINVAL;
			}
			else if(errno==ENOMSG)
			{
				rc = VK_ERR_IPC_RCV_ENOMSG;
			}
			else
			{
				rc = VK_ERROR;
			}
			PrintError("%s[%d] : msgrcv error NO : %d !!\n", __FUNCTION__, __LINE__, rc);
			if (rc!=VK_ERR_IPC_RCV_ENOMSG)
			{
				DD_MEM_Free(rcvbuf);
				return -rc;
			}
		}
		else
		{
			if (msg)
			{
				VK_memcpy((void *)msg, rcvbuf->mtext, size);
			}
			break;
		}
		
		currTick = VK_TIMER_GetSystemTick();
		timeAfter = VK_TIMER_TimeAfter(timeoutTick, currTick);
		if (VK_TIMER_EXPIRED == timeAfter)
		{
			DD_MEM_Free(rcvbuf);
			PrintExit();
			return VK_TIMEOUT;
		}
		VK_TASK_Sleep(2);
	} 

	DD_MEM_Free(rcvbuf);
	PrintExit();
	return VK_OK;
}

int VK_IPC_MSG_Send(unsigned int q_id, const void *msg, unsigned int size)
{
	return P_IPC_MSG_Send(q_id, VK_MSG_TYPE_MSG, msg, size);
}

int VK_IPC_MSG_SendTimeout(unsigned int q_id, const void *msg, unsigned int size, unsigned int timeout)
{
	return P_IPC_MSG_SendTimeout(q_id, VK_MSG_TYPE_MSG, msg, size, timeout);
}

int VK_IPC_MSG_Receive(unsigned int q_id, void *msg, unsigned int size)
{
	return P_IPC_MSG_Receive(q_id, VK_MSG_TYPE_MSG, msg, size);
}

int VK_IPC_MSG_ReceiveTimeout(unsigned int q_id, void *msg, unsigned int size, unsigned int timeout)
{
	return P_IPC_MSG_ReceiveTimeout(q_id, VK_MSG_TYPE_MSG, msg, size, timeout);
}

void VK_IPC_MSG_Close(unsigned int q_id)
{
	PrintEnter();
	msgctl(q_id, IPC_RMID, 0);
	PrintExit();
}

static VK_IPC_CALLBACK_T *P_VK_IPC_FindIpcCallback(unsigned int queue_id)
{
	unsigned int	queue_index;
	
	for (queue_index = 0; queue_index < VK_IPC_MSG_QUEUE_INDEX_MAX; queue_index++)
	{
		if (s_IPCNotify[queue_index].queue_id == queue_id)
		{
			return &s_IPCNotify[queue_index];
		}
	}
	
	return NULL;
}

static VK_IPC_NOTI_T *P_VK_IPC_FindIpcNoti(unsigned int queue_id, unsigned int callbackId)
{
	unsigned int	queue_index;
	unsigned int	noti_index;
	
	for (queue_index = 0; queue_index < VK_IPC_MSG_QUEUE_INDEX_MAX; queue_index++)
	{
		if (s_IPCNotify[queue_index].queue_id == queue_id)
		{
			for (noti_index = 0; noti_index < VK_IPC_MSG_CALLBACK_INDEX_MAX; noti_index++)
			{
				if (s_IPCNotify[queue_index].noti_function[noti_index].callbackId == callbackId)
				{
					return &s_IPCNotify[queue_index].noti_function[noti_index];
				}
			}
		}
	}
	
	return NULL;
}

#if 0
static int P_VK_IPC_PrintIpcNoti(void)
{
	unsigned int	queue_index;
	unsigned int	noti_index;
	
	for (queue_index = 0; queue_index < VK_IPC_MSG_QUEUE_INDEX_MAX; queue_index++)
	{
		for (noti_index = 0; noti_index < VK_IPC_MSG_QUEUE_INDEX_MAX; noti_index++)
		{
			VK_DBG_Print("%d : %08X\n", s_IPCNotify[queue_index].noti_function[noti_index].callbackId, s_IPCNotify[queue_index].noti_function[noti_index].CB_IPCNotify);
		}
	}
	
	return NULL;
}
#endif

int VK_IPC_MSG_Call(unsigned int queue_id, unsigned int callbackId, unsigned int *pResult, void *msg, unsigned int size)
{
	int 				rc;
	VK_IPC_CALL_IN_T	sendMsg;
	VK_IPC_CALL_OUT_T	rcvMsg;

	sendMsg.callbackId = callbackId;
	sendMsg.msg_size = size;
	VK_memcpy(sendMsg.szMsg, msg, size);

	rc = P_IPC_MSG_Send(queue_id, callbackId, &sendMsg, sizeof(VK_IPC_CALL_IN_T));
	if (rc != VK_OK)
	{
		return rc;
	}

	rc = P_IPC_MSG_Receive(queue_id, VK_IPC_CB_ACK(callbackId), &rcvMsg, sizeof(VK_IPC_CALL_OUT_T));
	if (rc != VK_OK)
	{
		return rc;
	}

	if (rcvMsg.vkResult != VK_OK)
	{
		return rcvMsg.vkResult;
	}

	if (pResult)
	{
		*pResult = rcvMsg.result;
	}
	VK_memcpy(msg, rcvMsg.szMsg, size);

	return rcvMsg.vkResult;
}


void P_IPC_MSG_ThreadProc(void *Context)
{
	int					rc;
	unsigned int		queue_id;
	VK_IPC_NOTI_T		*pIpc_noti_t;
	unsigned int		callback_rc;
	VK_IPC_CALL_OUT_T	sendMsg;
	VK_IPC_CALL_IN_T	rcvMsg;

	queue_id = (unsigned int)Context;

	while (1)
	{
		rc = P_IPC_MSG_Receive(queue_id, VK_IPC_RCV_ACK, &rcvMsg, sizeof(VK_IPC_CALL_IN_T));
		if (rc != VK_OK)
		{
			continue;
		}

		pIpc_noti_t = P_VK_IPC_FindIpcNoti(queue_id, rcvMsg.callbackId);
		if (pIpc_noti_t == NULL)
		{
			PrintError("[P_IPC_MSG_ThreadProc] errro : P_VK_IPC_FindIpcNoti queue:%d, callbackId(%d)\n", queue_id, rcvMsg.callbackId);
#if 0
			P_VK_IPC_PrintIpcNoti();
#endif
			sendMsg.vkResult = VK_ERROR;
			sendMsg.result = 0;
			goto noti;
		}

		if (pIpc_noti_t->CB_IPCNotify == NULL)
		{
			PrintError("[P_IPC_MSG_ThreadProc] errro : pIpc_noti_t->CB_IPCNotify == NULL, queue:%d, callbackId(%d)\n", queue_id, rcvMsg.callbackId);
			sendMsg.vkResult = VK_ERROR;
			sendMsg.result = 0;
			goto noti;
		}
		callback_rc = (unsigned int)pIpc_noti_t->CB_IPCNotify(rcvMsg.szMsg, rcvMsg.msg_size);
		sendMsg.vkResult = VK_OK;
		sendMsg.result = (unsigned int)callback_rc;
		memcpy(sendMsg.szMsg, rcvMsg.szMsg, rcvMsg.msg_size);
noti:
		rc = P_IPC_MSG_Send(queue_id, VK_IPC_CB_ACK(rcvMsg.callbackId), &sendMsg, sizeof(VK_IPC_CALL_OUT_T));
		if (rc != VK_OK)
		{
			PrintError("[P_IPC_MSG_ThreadProc] errro : P_IPC_MSG_Send\n", 0);
			continue;
		}
	}
}


int VK_IPC_MSG_RegisterCallback(unsigned int queue_id, unsigned int callbackId, pfnVK_IPC_MSG_NotifyFUNC pfnNotify)
{
	VK_IPC_NOTI_T		*pIpc_noti_t;
	VK_IPC_CALLBACK_T	*pIpcCallback_t;
	unsigned long 		task_id;
	int					result;

	if (queue_id == 0)
	{
		PrintError("[VK_IPC_MSG_RegisterCallback] (invalid queue_id == 0)\n", 0);
		return VK_ERROR;
	}
	if (callbackId == 0 || callbackId & VK_IPC_CB_MASK)
	{
		PrintError("[VK_IPC_MSG_RegisterCallback] (invalid callbackId)\n", 0);
		return VK_ERROR;
	}

	pIpcCallback_t = P_VK_IPC_FindIpcCallback(queue_id);
	if (pIpcCallback_t == NULL)
	{
		pIpcCallback_t = P_VK_IPC_FindIpcCallback(0);
		if (pIpcCallback_t == NULL)
		{
			// no room
			PrintError("[VK_IPC_MSG_RegisterCallback] no more ipcballback env!!\n", 0);
			return VK_ERROR;
		}
		// create task
		result = VK_TASK_Create(P_IPC_MSG_ThreadProc,
							67,
							(1024*4),
							"ipc_task",
							(void*)queue_id,
							&task_id, FALSE);
		if( result != VK_OK )
		{
			PrintError("[VK_IPC_MSG_RegisterCallback] errro : VK_TASK_Create\n", 0);
			return VK_ERROR;
		}
		
		// register queue_id;
		pIpcCallback_t->queue_id = queue_id;
		pIpcCallback_t->task_id = task_id;
		VK_TASK_Start(task_id);
	}

	pIpc_noti_t = P_VK_IPC_FindIpcNoti(queue_id, callbackId);
	if (pIpc_noti_t == NULL)
	{
		pIpc_noti_t = P_VK_IPC_FindIpcNoti(queue_id, 0);
		if (pIpc_noti_t == NULL)
		{
			// no room
			PrintError("[VK_IPC_MSG_RegisterCallback] no more ipcnoti env!!\n", 0);
			return VK_ERROR;
		}
		
		// register callbackId;
		pIpc_noti_t->CB_IPCNotify = pfnNotify;
		pIpc_noti_t->callbackId = callbackId;
	}

	return VK_OK;
}

int VK_IPC_MSG_UnRegisterCallback(unsigned int queue_id, unsigned int callbackId)
{
	VK_IPC_NOTI_T		*pIpc_noti_t;
	VK_IPC_CALLBACK_T	*pIpcCallback_t;

	if (queue_id == 0)
	{
		PrintError("[VK_IPC_MSG_UnRegisterCallback] queue_id == 0\n", 0);
		return VK_ERROR;
	}
	if (callbackId == 0)
	{
		PrintError("[VK_IPC_MSG_UnRegisterCallback] callbackId == 0\n", 0);
		return VK_ERROR;
	}

	pIpcCallback_t = P_VK_IPC_FindIpcCallback(queue_id);
	if (pIpcCallback_t == NULL)
	{
		PrintError("[VK_IPC_MSG_UnRegisterCallback] cannot fild queue id in ipcCallback env %08X!!\n", queue_id);
		return VK_ERROR;
	}

	pIpc_noti_t = P_VK_IPC_FindIpcNoti(queue_id, callbackId);
	if (pIpc_noti_t == NULL)
	{
		PrintError("[VK_IPC_MSG_UnRegisterCallback] cannot fild callback id in ipcNoti env %08X!!\n", callbackId);
		return VK_ERROR;
	}
	
	// register callbackId;
	pIpc_noti_t->CB_IPCNotify = NULL;
	pIpc_noti_t->callbackId = 0;

	return VK_OK;
}

