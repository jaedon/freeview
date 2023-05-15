/*
 * Copyright (c) 2006 HUMAX Co., Ltd. All rights reserved.
 *
 * This is very confidential.
 * This code must not be distributed outside.
 */

#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include "errno.h"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#include "vkernel.h"
#if defined(CONFIG_DEBUG)
#include "cmdtool.h"
#endif

#include <linden_trace.h>

#define VK_IPC_CB_MASK		0x40000000
#define VK_IPC_CB_ACK(x)	((x) | 0x40000000)
#define VK_IPC_RCV_ACK		0xC0000000

#define DEBUG_MODULE			TRACE_MODULE_VK_TASK

#define VK_UNUSED(x) ((x)=(x))

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

int VK_IPC_MSG_Init(void)
{
	return VK_OK;
}

int P_IPC_MSG_ReceiveTimeout(unsigned int q_id, unsigned int msgtype, void *msg, unsigned int size, unsigned int timeout);
int VK_IPC_MSG_Open(unsigned long q_count,unsigned long msg_size, const char *q_name, int *q_id)
{

	HAPPY(q_count);
	HAPPY(q_name);
	HAPPY(q_id);
	HAPPY(msg_size);

	return VK_OK;
}

int P_IPC_MSG_Send(unsigned int q_id, unsigned int msgtype, const void *msg, unsigned int size)
{

	HAPPY(q_id);
	HAPPY(msgtype);
	HAPPY(msg);
	HAPPY(size);

	return VK_OK;

}

int P_IPC_MSG_SendTimeout(unsigned int q_id, unsigned int msgtype, const void *msg, unsigned int size, unsigned int timeout)
{
	HAPPY(q_id);
	HAPPY(msgtype);
	HAPPY(msg);
	HAPPY(size);
	HAPPY(timeout);

	return VK_OK;

}

int P_IPC_MSG_Receive(unsigned int q_id, unsigned int msgtype, const void *msg, unsigned int size)
{
	HAPPY(q_id);
	HAPPY(msgtype);
	HAPPY(msg);
	HAPPY(size);

	return VK_OK;
}

int P_IPC_MSG_ReceiveTimeout(unsigned int q_id, unsigned int msgtype, void *msg, unsigned int size, unsigned int timeout)
{

	HAPPY(q_id);
	HAPPY(msgtype);
	HAPPY(msg);
	HAPPY(size);
	HAPPY(timeout);

	return VK_OK;
}

int VK_IPC_MSG_Send(unsigned int q_id, const void *msg, unsigned int size)
{
	HAPPY(q_id);
	HAPPY(msg);
	HAPPY(size);

	return VK_OK;
}

int VK_IPC_MSG_SendTimeout(unsigned int q_id, const void *msg, unsigned int size, unsigned int timeout)
{
	HAPPY(q_id);
	HAPPY(msg);
	HAPPY(size);
	HAPPY(timeout);

	return VK_OK;
}

int VK_IPC_MSG_Receive(unsigned int q_id, void *msg, unsigned int size)
{
	HAPPY(q_id);
	HAPPY(msg);
	HAPPY(size);

	return VK_OK;
}

int VK_IPC_MSG_ReceiveTimeout(unsigned int q_id, void *msg, unsigned int size, unsigned int timeout)
{
	HAPPY(q_id);
	HAPPY(msg);
	HAPPY(size);
	HAPPY(timeout);

	return VK_OK;
}

void VK_IPC_MSG_Close(unsigned int q_id)
{
	HAPPY(q_id);

}

int VK_IPC_MSG_Call(unsigned int queue_id, unsigned int callbackId, unsigned int *pResult, void *msg, unsigned int size)
{
	HAPPY(queue_id);
	HAPPY(callbackId);
	HAPPY(pResult);
	HAPPY(msg);
	HAPPY(size);

	return 0;
}


void P_IPC_MSG_ThreadProc(void *Context)
{
	HAPPY(Context);

}


int VK_IPC_MSG_RegisterCallback(unsigned int queue_id, unsigned int callbackId, pfnVK_IPC_MSG_NotifyFUNC pfnNotify)
{
	HAPPY(queue_id);
	HAPPY(callbackId);
	HAPPY(pfnNotify);

	return VK_OK;
}

int VK_IPC_MSG_UnRegisterCallback(unsigned int queue_id, unsigned int callbackId)
{
	HAPPY(queue_id);
	HAPPY(callbackId);

	return VK_OK;
}

