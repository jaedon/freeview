/**
 * vk_msg.c
*/

/**
 * @defgroup		VK_MSG
 * @author		Tae-Hyuk Kang
 * @note			Virsual Message Queue APIs
 * @file 			vk_msg.c
 * @remarks		Copyright (c) 2008 HUMAX Co., Ltd. All rights reserved.
*/

#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

#include <linden_trace.h>

#include <pthread.h>
#include <errno.h>
#include <assert.h>
#include <sys/queue.h>
#include <unistd.h>

#include "htype.h"
#include "vkernel.h"
#include "taskdef.h"
#include "os.h"
#include "os_cfg_app.h"

#define DEBUG_MODULE			TRACE_MODULE_VK_MSG
#define UCOS_Q_SIZE_MAX		4096 //size of unsigned char. see  OS_Q->OSQSize & OS_Q->OSQEntries in ucospriv.h
#define MAX_MSG_COUNT			64
#define MAX_MSG_NAME_LENGTH		32

#define MS_TO_TICKS(x)      ((x * OS_CFG_TICK_RATE_HZ)/ 1000)

#define VK_MSG_ID_MAGIC_CODE 0x820610

typedef struct DI_UCOSMsgQ
{
	OS_Q	msgQ;
	HBOOL 	isUsed;
	HCHAR	msgQName[MAX_MSG_NAME_LENGTH];
	HINT32	msgQCount;
}DI_UCOSMsgQ_t;

static DI_UCOSMsgQ_t ucos_msg_array[MAX_MSG_COUNT];

#define UNUSED(x) (void)(x)

struct  vk_msg_element
{
    void             *data;
    struct vk_msg_element  *next;
    struct vk_msg_element  *prev;
};


//MSG_PRIORITY의 값은 Task Priority의 갯수보다 1개 많아야 한다.
typedef enum
{
    MSG_PRIORITY0,
    MSG_PRIORITY1,
    MSG_PRIORITY2,
    MSG_PRIORITY3,
    MSG_PRIORITY4,
    MSG_PRIORITY5,
    MSG_PRIORITY6,
    MSG_PRIORITY7,
    MSG_PRIORITY8,
    MSG_PRIORITY9,
    MSG_PRIORITY10,
    MSG_PRIORITY11,
    MSG_PRIORITY12,
    MSG_PRIORITY13,
    MSG_PRIORITY14,
    MSG_PRIORITY15,
    MSG_PRIORITY16,
    MSG_PRIORITY_MAX   //최상위의 PRIORITY는 Urgent로 사용함..
}VK_MSG_PRIORITY;

struct vk_msg_queue
{

    pthread_mutex_t     q_lock_;
    pthread_cond_t      q_not_empty_;
    pthread_cond_t      q_not_full_;
    unsigned long       curr_msg_count_;
    unsigned long       msg_size_;
    unsigned long       msg_count_;
    unsigned long       bitmap_count[MSG_PRIORITY_MAX];
    unsigned char       bitmap[MSG_PRIORITY_MAX];
    unsigned char       suspend_type_;
	void				*pPool;
	void				*pDataPool;

    int writer_count_;
    int reader_count_;

    struct vk_msg_element          *temp_head;
    struct vk_msg_element          *temp_tail;
    struct vk_msg_element          *head[MSG_PRIORITY_MAX];
    struct vk_msg_element          *tail[MSG_PRIORITY_MAX];

#if defined(CONFIG_DEBUG)
    unsigned long sendTick;
    unsigned long receiveTick;
    unsigned long lastSendTick;
    unsigned long lastReceiveTick;
    char name[32];
    TAILQ_ENTRY(vk_msg_queue) entries;
#endif
};

#if defined(VK_MSG_DEBUG)
TAILQ_HEAD(listhead, vk_msg_queue) msg_q_list_head;
#endif

#if defined(CONFIG_DEBUG)
unsigned long	tickForVkDebug;
#endif


extern HINT8 *VK_DBG_OSErr2Str(HINT32 ucosErr/* OS_ERR */);

/*
    VK_MSG_Init
    메세지 큐를 초기화 한다.
*/
int VK_MSG_Init(void)
{
    return VK_OK;
}

static HINT32 _vk_msg_VKQIDtoUCOSQID(unsigned long q_id)
{
	HINT32 ucos_msg_id = 0;

	ucos_msg_id = (HINT32)q_id - VK_MSG_ID_MAGIC_CODE;
	if(0 <= ucos_msg_id && ucos_msg_id <= MAX_MSG_COUNT)
	{
		return ucos_msg_id;
	}
	return -1;
}

static void  _vk_msg_dbg_DumpErrorUCOSMsgQData(HINT32 idx, const HCHAR *func, HINT32 line)
{
	if(1 == ucos_msg_array[idx].isUsed)
	{
		PrintError("[%s:%d] idx(%d) name(%s) msgQCount(%d) queue(0x%x) \r\n", func, line, idx, ucos_msg_array[idx].msgQName, ucos_msg_array[idx].msgQCount, ucos_msg_array[idx].msgQ);
	}
}

/*
    VK_MSG_Create
    메세지 큐를 생성한다.
*/
int VK_MSG_Create(unsigned long q_count, unsigned long msg_size,
	const char *q_name, unsigned long *q_id, VK_SUSPENDTYPE suspendType)
{
	OS_ERR err;
	int i = 0, msg_index = 0;

	if (q_id == NULL)
	{
		PrintError("Error at %s() %d lines\n",__FUNCTION__, __LINE__);
		return VK_ERROR;
	}

	for(i=0; i<MAX_MSG_COUNT; i++)
	{
		if(ucos_msg_array[i].isUsed == 0)
		{
			ucos_msg_array[i].isUsed = 1;
			msg_index = i;
			break;
		}
	}
	if( i >= MAX_MSG_COUNT)
	{
		PrintError("\r\n\r\n============================(%s:%d)\n",__FUNCTION__, __LINE__);
		PrintError("\033[1;33m out of number for msg array (%s:%d)\033[0m\n",__FUNCTION__, __LINE__);
		PrintError("============================(%s:%d)\r\n\r\n",__FUNCTION__, __LINE__);
		return VK_ERROR;
	}

	if(q_count > UCOS_Q_SIZE_MAX)
	{
		q_count = UCOS_Q_SIZE_MAX;
	}


	if (suspendType >= VK_SUSPENDTYPE_MAX)
	{
		PrintError("Error at %s() %d lines\n",__FUNCTION__, __LINE__);
		return VK_ERROR;
	}

	VK_strncpy(ucos_msg_array[msg_index].msgQName, q_name, (MAX_MSG_NAME_LENGTH-1));
	ucos_msg_array[msg_index].msgQCount = q_count;

	PrintDebug("msg_size : %d\n",(OS_MSG_QTY)msg_size);
	OSQCreate(&ucos_msg_array[msg_index].msgQ,
              (char *)ucos_msg_array[msg_index].msgQName,
              ((OS_MSG_QTY)ucos_msg_array[msg_index].msgQCount),
              &err);
	if (err != OS_ERR_NONE)
	{
		PrintError("[%s:%d] OSQCreate Error at - error(%d, %s)\n",__FUNCTION__, __LINE__, err, VK_DBG_OSErr2Str(err));
		_vk_msg_dbg_DumpErrorUCOSMsgQData(msg_index, __FUNCTION__, __LINE__);
		return VK_ERROR;
	}

	*q_id = (unsigned int)msg_index+VK_MSG_ID_MAGIC_CODE;
	PrintDebug("msg_index : %d, name : %s, type : %d\n", msg_index, q_name, ucos_msg_array[msg_index].msgQ.Type);


	PrintExit("VK_MSG_Create()");

    return VK_OK;

}


/*
    VK_MSG_Destroy
    메세지큐를 소멸시킨다.
*/
int VK_MSG_Destroy(unsigned long q_id)
{
	int		real_q_id = 0;

	real_q_id = _vk_msg_VKQIDtoUCOSQID(q_id);
	if( 0 > real_q_id)
	{
		PrintError("Error at %s() %d lines\n",__FUNCTION__, __LINE__);
		return VK_ERROR;
	}


	// NOT SUPPORTED.
	HAPPY(q_id);
	PrintDebug("%s(%d) : Not Support Yet\n", __FUNCTION__, __LINE__);
	return VK_OK;

#if 0
	OS_ERR	err;

	PrintEnter("VK_MSG_Destroy()");

	OSQDel(&ucos_msg_array[q_id].msgQ,
				OS_OPT_DEL_ALWAYS,
				&err);
	if (err != OS_ERR_NONE) {
		PrintError("Error at OSQDel Func.\n");
		return VK_ERROR;
	}

	PrintExit("VK_MSG_Destroy()");

	return VK_OK;
#endif

}

int VK_MSG_Clear(unsigned long q_id)
{
	int		real_q_id = 0;

	real_q_id = _vk_msg_VKQIDtoUCOSQID(q_id);
	if( 0 > real_q_id)
	{
		PrintError("Error at %s() %d lines\n",__FUNCTION__, __LINE__);
		return VK_ERROR;
	}

	HAPPY(q_id);
	PrintDebug("%s(%d) : Not Support Yet\n", __FUNCTION__, __LINE__);
	return VK_OK;
}



int P_MSG_SendTimeout(unsigned long q_id, const void *msg, unsigned long size, unsigned long timeout)
{

	OS_ERR	err;
	void	*ptr;
	int		real_q_id = 0;

	PrintEnter("P_MSG_SendTimeout()");

	HAPPY(timeout);

	if (msg == NULL)
	{
		PrintError("Error at %s() %d lines\n",__FUNCTION__, __LINE__);
		return VK_ERROR;
	}
	if (size <= 0)
	{
		PrintError("Error at %s() %d lines\n",__FUNCTION__, __LINE__);
		return VK_ERROR;
	}
	real_q_id = _vk_msg_VKQIDtoUCOSQID(q_id);
	if( 0 > real_q_id)
	{
		PrintError("Error at %s() %d lines\n",__FUNCTION__, __LINE__);
		return VK_ERROR;
	}

	if(NULL != msg && size > 0 )
	{
		ptr = (void *)VK_MEM_Alloc(size);
		if(NULL == ptr)
		{
			PrintError("Out of memory (%s:%d)\n", __FUNCTION__, __LINE__);
			return VK_ERROR;
		}
		VK_memcpy(ptr, msg, size);
	}

	OSQPost(&ucos_msg_array[real_q_id].msgQ,
				ptr,
				(OS_MSG_SIZE)size,
				OS_OPT_POST_FIFO + OS_OPT_POST_ALL ,
				&err	);
	switch(err)
	{
	case OS_ERR_NONE:
		break;

	case OS_ERR_TIMEOUT:
		if(ptr)
		{
			VK_MEM_Free(ptr);
		}
		return VK_TIMEOUT;

	default:
		PrintError("[%s:%d] OSQPost Error at - error(%d, %s), q_id(%d)\n",__FUNCTION__, __LINE__, err, VK_DBG_OSErr2Str(err), real_q_id);
		_vk_msg_dbg_DumpErrorUCOSMsgQData(real_q_id, __FUNCTION__, __LINE__);
		if(ptr)
		{
			VK_MEM_Free(ptr);
		}
		return VK_ERROR;
	}

	PrintExit("P_MSG_SendTimeout()");

	return VK_OK;

}



/*
    VK_MSG_Send
    메세지를 보낸다.
*/
int VK_MSG_Send(unsigned long q_id, const void *msg, unsigned long size)
{
	return P_MSG_SendTimeout(q_id, msg, size, 0xFFFFFFFF);
}

/*
    VK_MSG_SendTimeout
    메세지를 Timeout내에 보낸다.
*/
int VK_MSG_SendTimeout(unsigned long q_id, const void *msg, unsigned long size, unsigned long timeout)
{
	HAPPY(timeout);

	return P_MSG_SendTimeout(q_id, msg, size, 0xFFFFFFFF);
}


/*
    VK_MSG_SendUrgent
    긴급 메세지를 보낸다.
*/
int VK_MSG_SendUrgent(unsigned long q_id, const void *msg, unsigned long size)
{

	HAPPY(q_id);
	HAPPY(msg);
	HAPPY(size);
	PrintDebug("%s(%d) : Not Support Yet\n", __FUNCTION__, __LINE__);
	return VK_OK;
}


/*
    VK_MSG_SendUrgentTimeout
    긴급 메세지를 Timeout 내에 보낸다.
*/
int VK_MSG_SendUrgentTimeout(unsigned long q_id, const void *msg, unsigned long size, unsigned long timeout)
{

	HAPPY(q_id);
	HAPPY(msg);
	HAPPY(size);
	HAPPY(timeout);
	PrintDebug("%s(%d) : Not Support Yet\n", __FUNCTION__, __LINE__);
	return VK_OK;
}

#if defined(VK_MSG_DEBUG) && defined(VK_TASK_DEBUG)
#define VK_MSG_CRITICAL_DELAY_TICK	20
#define VK_MSG_NORMAL_DELAY_TICK	200
#endif
/*
    VK_MSG_Receive
    메세지를 받는다.
*/
int VK_MSG_Receive(unsigned long q_id, void *msg, unsigned long size)
{

	OS_ERR			err;
	CPU_TS			ts;
	OS_MSG_SIZE 	msg_size;
	void			*recv_msg;
	int				real_q_id = 0;

	PrintEnter("VK_MSG_Receive()");

	real_q_id = _vk_msg_VKQIDtoUCOSQID(q_id);
	if( 0 > real_q_id)
	{
		PrintError("Error at %s() %d lines\n",__FUNCTION__, __LINE__);
		return VK_ERROR;
	}

	if (size <= 0)
	{
		PrintError("Error at %s() %d lines\n",__FUNCTION__, __LINE__);
		return VK_ERROR;
	}

	recv_msg = OSQPend(&ucos_msg_array[real_q_id].msgQ,
				0,
				OS_OPT_PEND_BLOCKING,
				&msg_size,
				&ts,
				&err);
	if (err != OS_ERR_NONE) {
		PrintError("[%s:%d] OSQPend Error at - error(%d, %s)\n",__FUNCTION__, __LINE__, err, VK_DBG_OSErr2Str(err));
		_vk_msg_dbg_DumpErrorUCOSMsgQData(real_q_id, __FUNCTION__, __LINE__);
		return VK_ERROR;
	}

	if(recv_msg)
	{
		VK_memcpy(msg, recv_msg, size);
		VK_MEM_Free(recv_msg);
	}

	PrintExit("VK_MSG_Receive()");

    return VK_OK;
}

/*
    VK_MSG_ReceiveTimeout
    메세지를 Timeout내에 받는다.
*/
int VK_MSG_ReceiveTimeout(unsigned long q_id, void *msg, unsigned long size, unsigned long timeout)
{
	OS_ERR			err;
	CPU_TS			ts;
	OS_MSG_SIZE 	msg_size;
	int 			ticks;
	void			*recv_msg;
	int				real_q_id = 0;

	PrintEnter("VK_MSG_Receive()");

	real_q_id = _vk_msg_VKQIDtoUCOSQID(q_id);
	if( 0 > real_q_id)
	{
		PrintError("Error at %s() %d lines\n",__FUNCTION__, __LINE__);
		return VK_ERROR;
	}

	ticks = MS_TO_TICKS(timeout);
	if(ticks == 0)
	{
		ticks = 1;
	}

	if (size <= 0)
	{
		PrintError("Error at %s() %d lines\n",__FUNCTION__, __LINE__);
		return VK_ERROR;
	}

	PrintDebug("q_id : %x, real_q_id : %x, size : %d, ticks : %d\n", q_id, real_q_id, size, ticks);

	recv_msg = OSQPend(&ucos_msg_array[real_q_id].msgQ,
				ticks,
				OS_OPT_PEND_BLOCKING,
				&msg_size,
				&ts,
				&err);

	switch(err)
	{
	case OS_ERR_NONE:
		break;

	case OS_ERR_TIMEOUT:
		return VK_TIMEOUT;

	default:
		PrintError("[%s:%d] OSQPend Error at - error(%d, %s)\n",__FUNCTION__, __LINE__, err, VK_DBG_OSErr2Str(err));
		_vk_msg_dbg_DumpErrorUCOSMsgQData(q_id, __FUNCTION__, __LINE__);
		return VK_ERROR;
	}

	if(recv_msg)
	{
		VK_memcpy(msg, recv_msg, size);
		VK_MEM_Free(recv_msg);
	}

	return VK_OK;
}


int	VK_MSG_GetMessageCount(unsigned long q_id, unsigned long *pulMsgCount, unsigned long *pulMaxMsgCount)
{

	HAPPY(q_id);
	HAPPY(pulMsgCount);
	HAPPY(pulMaxMsgCount);
	PrintDebug("%s(%d) : Not Support Yet\n", __FUNCTION__, __LINE__);
	return VK_OK;
}

/*
    VK_MSG_PrintMsgQList
    메세지큐 정보를 출력한다.
*/
#if 1//defined(VK_MSG_DEBUG)
void VK_MSG_PrintMsgQList(void)
{
	HINT32 i;

	for(i=0; i < MAX_MSG_COUNT; i++)
	{
		_vk_msg_dbg_DumpErrorUCOSMsgQData(i, __FUNCTION__, __LINE__);
	}

}
#endif


