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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <assert.h>
#include <sys/queue.h>
#include <unistd.h>

#include "htype.h"
#include "vkernel.h"
#include "taskdef.h"
#include "inc/vk_p_common_api.h"

#define DEBUG_MODULE			TRACE_MODULE_VK_MSG

#ifdef MSG_DEBUG
#define PrintDebug(fmt, ...) 		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintError(fmt, ...)    	VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...)				VK_DBG_Print("(+)%s\n", __FUNCTION__)
#define PrintExit(...)				VK_DBG_Print("(-)%s\n", __FUNCTION__)
#else 
#define PrintDebug(fmt, ...)    	while (0) ((int (*)(char *, ...)) 0)
#define PrintError(fmt, ...)		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...) 			while (0) ((int (*)(char *, ...)) 0)				
#define PrintExit(...)				while (0) ((int (*)(char *, ...)) 0)		
#endif /* MSG_DEBUG */
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


static void list_init(struct vk_msg_queue *list, unsigned long ulSize, unsigned long ulCount, unsigned char ucSuspendType);

static void list_destroy(struct vk_msg_queue *list);

int list_ins_next(struct vk_msg_queue *list, unsigned long prio, const void *data, unsigned long size, unsigned char timeoutflag, struct timespec *timeo);

int list_rem_next(struct vk_msg_queue *list, void *data, unsigned long size, unsigned char timeoutflag, struct timespec *timeo);

void list_set_bitmap(struct vk_msg_queue *list, unsigned long prio);

void list_clear_bitmap(struct vk_msg_queue *list, unsigned long prio);

unsigned long list_find_highest_priority_bitmap(struct vk_msg_queue *list);

void list_bitmap_count_increase(struct vk_msg_queue *list, unsigned long prio);

void list_bitmap_count_decrease(struct vk_msg_queue *list, unsigned long prio);

void q_insert_temp_tail_element(struct vk_msg_queue *q, struct vk_msg_element *element);

void q_remove_temp_tail_element(struct vk_msg_queue *q, 	struct vk_msg_element **element);

void q_destory_temp_tail(struct vk_msg_queue *q);

static int P_MSG_Check(struct vk_msg_queue *q, unsigned long q_id);

#define list_size(list) ((list)->curr_msg_count_)

#define list_bitmap_size(list, i) ((list)->bitmap_count[(i)])

#define list_head(list, i) ((list)->head[i])

#define list_tail(list, i) ((list)->tail[i])

#define list_is_head(list, element) ((element) == (list)->head ? 1 : 0)

#define list_is_tail(element) ((element)->next == NULL ? 1 : 0)

#define list_data(element) ((element)->data)

#define list_next(element) ((element)->next)



/*
    list_init
    vk_msg_queue 구조체를 초기화 한다.
*/
static void list_init(struct vk_msg_queue *list, unsigned long ulSize, unsigned long ulCount, unsigned char ucSuspendType)
{
    int i;

    list->curr_msg_count_ = 0;
    list->msg_size_ = ulSize;
    list->msg_count_ = ulCount;
    list->writer_count_ = 0;
    list->reader_count_ = 0;
    list->suspend_type_ = ucSuspendType;
    pthread_mutex_init(&list->q_lock_, NULL);
    pthread_cond_init(&list->q_not_empty_, NULL);
    pthread_cond_init(&list->q_not_full_, NULL);

    list->temp_head = NULL;
    list->temp_tail = NULL;

    for(i=0; i<MSG_PRIORITY_MAX; i++)
    {
        list->head[i] = NULL;
        list->tail[i] = NULL;
        list->bitmap_count[i] = 0;
        list->bitmap[i] = 0;
    }

#ifdef VK_MSG_DEBUG
    list->sendTick = 0;
    list->receiveTick = 0;
    list->lastSendTick = 0;
    list->lastReceiveTick = 0;
#endif
    return;
}



/*
    list_destroy
    vk_msg_queue 구조체를 소멸시킨다.
*/
static void list_destroy(struct vk_msg_queue *list)
{
	void               *data;

	data = (char *)DD_MEM_Alloc(list->msg_size_);
	if (data == NULL)
	{
		return;
	}

	while (list_size(list) > 0)
	{
		if ( list_rem_next(list, (void *)data, list->msg_size_, FALSE, FALSE) != 0 )
		{
			PrintError("%s : list_rem_next Error!!\n", __func__);
		}
	}

	DD_MEM_Free(data);
	return;
}



/*
    list_ins_next
    element에 data를 추가한다.
*/
int list_ins_next(struct vk_msg_queue *list, unsigned long prio, const void *data, unsigned long size, unsigned char timeoutflag, struct timespec *timeo)
{
	struct vk_msg_element           *new_element;
	int rc = 0;

	PrintEnter();

	list->writer_count_++;

	while (list->msg_count_ == list->curr_msg_count_ && 0 == rc)
	{
	    if(TRUE == timeoutflag)
	    {
	        rc = pthread_cond_timedwait(&list->q_not_full_, &list->q_lock_, timeo);
	    }
	    else
	    {
	        rc = pthread_cond_wait(&list->q_not_full_, &list->q_lock_);
	    }
	}    

	list->writer_count_--;

	if(rc != 0)
	{
	    PrintDebug("%s : rc[%d] !!\n", __FUNCTION__, rc);
	    return rc;
	}

	q_remove_temp_tail_element(list, &new_element);

	VK_memcpy(new_element->data, data, size);

	if(list->tail[prio] == NULL)
	{
		list->head[prio] = new_element;
		new_element->next = NULL;
		new_element->prev = NULL;
		list->tail[prio] = new_element;
	}
	else
	{
		list->tail[prio]->next = new_element;
		new_element->next = NULL;
		new_element->prev = list->tail[prio];
		list->tail[prio] = new_element;
	}

	list->curr_msg_count_++;
	list_bitmap_count_increase(list, prio);
	list_set_bitmap(list, prio);

	if (list->reader_count_ > 0)
	{
		pthread_cond_signal(&list->q_not_empty_);
	}

	PrintExit();
	return 0;
}



/*
    list_rem_next
    element에서 data를 삭제한다.
*/
int list_rem_next(struct vk_msg_queue *list, void *data, unsigned long size, unsigned char timeoutflag, struct timespec *timeo)
{
	struct vk_msg_element           *old_element;
	unsigned long prio;
	int rc = 0;

	PrintEnter();

	list->reader_count_++;

	if ((list_size(list) == 0 || list->reader_count_ > 1) && 0 == rc)
	{
		if(TRUE == timeoutflag)
		{
			rc = pthread_cond_timedwait(&list->q_not_empty_, &list->q_lock_, timeo);
		}
		else
		{
			rc = pthread_cond_wait(&list->q_not_empty_, &list->q_lock_);
		}
	}

	list->reader_count_--;

	if (rc != 0)
	{
		PrintDebug("%s : rc[%d] !!\n", __FUNCTION__, rc);
		return rc;
	}

	prio = list_find_highest_priority_bitmap(list);
	PrintDebug("list_find_highest_priority_bitmap : prio = %d\n", prio);

	VK_memcpy(data, list->head[prio]->data, size);
	old_element = list->head[prio];
	if(NULL == list->head[prio]->next)
	{
		list->head[prio] = NULL;
		list->tail[prio] = NULL;
	}
	else
	{
		list->head[prio] = list->head[prio]->next;
	}

	q_insert_temp_tail_element(list, old_element);
	list->curr_msg_count_--;
	list_bitmap_count_decrease(list, prio);

	if(list->bitmap_count[prio] == 0)
	    list_clear_bitmap(list, prio);

	if (list->writer_count_ > 0)
	    pthread_cond_signal(&list->q_not_full_);

	PrintExit();

	return rc;
}


/*
    print_list
    vk_msg_queue 구조체 전체를 Print한다.
*/
void print_list(const struct vk_msg_queue *list)
{
    struct vk_msg_element           *element;
    int                *data, i, count;

    VK_DBG_Print("Msg Queue Current total size is %ld\n", list_size(list));

    for(count=0; count<MSG_PRIORITY_MAX; count++)
    {
        i = 0;

        if( list->bitmap[count] != 0)
        {
            element = list_head(list, count);

            while (1)
            {
               data = list_data(element);
               VK_DBG_Print("priority[%d] : qlist[%03d]=%03d\n", count, i, *data);

               i++;

               if (list_is_tail(element))
                  break;
               else
                  element = list_next(element);
            }
        }
    }
}



/*
    list_set_bitmap
    우선순위에 해당하는 비트맵을 set 한다.
*/
void list_set_bitmap(struct vk_msg_queue *list, unsigned long prio)
{
    list->bitmap[prio] = 1;
}



/*
    list_clear_bitmap
    우선순위에 해당하는 비트맵을 clear 한다.
*/
void list_clear_bitmap(struct vk_msg_queue *list, unsigned long prio)
{
    list->bitmap[prio] = 0;
}



/*
    list_find_highest_priority_bitmap
    bitmap에서 가장 높은 우선순위를 찾는다.
*/
unsigned long list_find_highest_priority_bitmap(struct vk_msg_queue *list)
{
    int i;

    for(i=MSG_PRIORITY_MAX-1; i>=0; i--)
    {
        if(list->bitmap[i] == 1)
            return i;
    }

    return 0;
}



/*
    list_bitmap_count_increase
    해당하는 우선순위의 bitmap Count를 증가 시킨다.
*/
void list_bitmap_count_increase(struct vk_msg_queue *list, unsigned long prio)
{
    list->bitmap_count[prio]++;
}



/*
    list_bitmap_count_decrease
    해당하는 우선순위의 bitmap Count를 감소 시킨다.
*/
void list_bitmap_count_decrease(struct vk_msg_queue *list, unsigned long prio)
{
    list->bitmap_count[prio]--;
}


/*
    list_mapping_task_priority_to_msgq
    태스크 우선순위를 기준으로 메세지큐 우선순위를 결정한다.
*/
int list_find_msg_q_priority_from_task_priority(unsigned long *prio)
{
    struct sched_param	param;
    int policy = 0;
    int rc;

    rc = pthread_getschedparam( pthread_self(), &policy, &param);
    if(rc != VK_OK)
    {
		*prio = MSG_PRIORITY0;
		return VK_OK;
    }
    PrintDebug("policy = %d\n", policy);
    PrintDebug("param.sched_priority = %d\n", param.sched_priority);

    switch(param.sched_priority)
    {
        case USER_PRIORITY0:
            *prio = MSG_PRIORITY0;
            break;
        case USER_PRIORITY1:
            *prio = MSG_PRIORITY1;
            break;
        case USER_PRIORITY2:
            *prio = MSG_PRIORITY2;
            break;
        case USER_PRIORITY3:
            *prio = MSG_PRIORITY3;
            break;
        case USER_PRIORITY4:
            *prio = MSG_PRIORITY4;
            break;
        case USER_PRIORITY5:
            *prio = MSG_PRIORITY5;
            break;
        case USER_PRIORITY6:
            *prio = MSG_PRIORITY6;
            break;
        case USER_PRIORITY7:
            *prio = MSG_PRIORITY7;
            break;
        case USER_PRIORITY8:
            *prio = MSG_PRIORITY8;
            break;
        case USER_PRIORITY9:
            *prio = MSG_PRIORITY9;
            break;
        case USER_PRIORITY10:
            *prio = MSG_PRIORITY10;
            break;
        case USER_PRIORITY11:
            *prio = MSG_PRIORITY11;
            break;
        case USER_PRIORITY12:
            *prio = MSG_PRIORITY12;
            break;
        case USER_PRIORITY13:
            *prio = MSG_PRIORITY13;
            break;
        case USER_PRIORITY14:
            *prio = MSG_PRIORITY14;
            break;
        case USER_PRIORITY15:
            *prio = MSG_PRIORITY15;
            break;
        default:
            *prio = MSG_PRIORITY15;
            break;
    }

    return VK_OK;
}


/*
	q_insert_temp_tail_element
	temp Qeue에 element를 연결 한다.
*/
void q_insert_temp_tail_element(struct vk_msg_queue *q, struct vk_msg_element *element)
{
	if(q->temp_tail == NULL)
	{
		q->temp_head = element;
		element->next = NULL;
		element->prev = NULL;
		q->temp_tail = element;
	}
	else
	{
		q->temp_tail->next = element;
		element->next = NULL;
		element->prev = q->temp_tail;
		q->temp_tail = element;
	}
}

/*
	q_remove_temp_tail_element
	temp Qeue에서 마지막 element를 제거하여 넘겨준다.
*/
void q_remove_temp_tail_element(struct vk_msg_queue *q, 	struct vk_msg_element **element)
{
	PrintEnter();

      	*element = q->temp_tail;

	if(q->temp_head->next == NULL) //element 1개 남은 경우.
	{
		q->temp_head = NULL;
		q->temp_tail = NULL;
	}
	else
	{
		q->temp_tail->prev->next = NULL;
		q->temp_tail = q->temp_tail->prev;
	}
	PrintExit();
}

/*
	q_destory_temp_tail
	temp Qeue의 element memory를 소멸시킨다.
*/
void q_destory_temp_tail(struct vk_msg_queue *q)
{
	struct vk_msg_element *q_element;

	PrintEnter();

	do
	{
		q_element = q->temp_tail;
		q->temp_tail = q->temp_tail->prev;
	} while(q->temp_tail != NULL);

	if (q->pPool)
	{
		DD_MEM_Free(q->pPool);
		q->pPool = NULL;
	}
	if (q->pDataPool)
	{
		DD_MEM_Free(q->pDataPool);
		q->pDataPool = NULL;
	}

	PrintExit();
}

/*
    VK_MSG_Init
    메세지 큐를 초기화 한다.
*/
int VK_MSG_Init(void)
{
#if defined(VK_MSG_DEBUG)
	TAILQ_INIT(&msg_q_list_head);
#endif
    return VK_OK;
}


/*
    VK_MSG_Create
    메세지 큐를 생성한다.
*/
int VK_MSG_Create(unsigned long q_count, unsigned long msg_size,
	const char *q_name, unsigned long *q_id, VK_SUSPENDTYPE suspendType)
{
    struct vk_msg_queue               *q;
    struct vk_msg_element           *q_element;
    unsigned long					i;
    struct vk_msg_element           *q_element_pool;
	unsigned char					*data_pool;
	HAPPY(q_name);

    PrintEnter();

    if(suspendType >= VK_SUSPENDTYPE_MAX)
    {
        PrintError("%s[%d] : ulSuspendType %d Error\n", __FUNCTION__, __LINE__,  suspendType);
        return VK_ERROR;
    }

	q = (struct vk_msg_queue *)DD_MEM_Alloc(sizeof(struct vk_msg_queue));
    if (q == NULL)
    {
        PrintError("%s[%d] : DD_MEM_Alloc Error\n", __FUNCTION__, __LINE__);
        return VK_ERROR;
    }

	VK_memset32(q, 0, sizeof(struct vk_msg_queue));

    list_init(q, msg_size, q_count, suspendType);
	q_element_pool = (struct vk_msg_element *)DD_MEM_Alloc(sizeof(struct vk_msg_element)*q_count);
	data_pool = (unsigned char *)DD_MEM_Alloc(msg_size*q_count);
    for (i=0; i<q_count; i++)
    {
		q_element = &q_element_pool[i];
		q_element->data = &data_pool[i*msg_size];
        q_insert_temp_tail_element(q, q_element);
    }
	q->pDataPool = data_pool;
	q->pPool = q_element_pool;

    *q_id = (unsigned long)q;

#if defined(VK_MSG_DEBUG)
	TAILQ_INSERT_TAIL(&msg_q_list_head, q, entries);
#endif
#if defined(CONFIG_DEBUG)
	if (q_name)
		strncpy(q->name, q_name, sizeof(q->name)-1);
	else
		q->name[0] = 0;
#endif
	PrintExit();

    return VK_OK;
}


/*
    VK_MSG_Destroy
    메세지큐를 소멸시킨다.
*/
int VK_MSG_Destroy(unsigned long q_id)
{
	struct vk_msg_queue *q;

	PrintEnter();

	q = (struct vk_msg_queue *)q_id;
	if (q == NULL)
	{
        PrintError("%s[%d] : VK_MSG_Destroy Error\n", __FUNCTION__, __LINE__);
		return VK_ERROR;
	}

	pthread_cond_destroy(&q->q_not_full_);
	pthread_cond_destroy(&q->q_not_empty_);
	pthread_mutex_destroy(&q->q_lock_);

	list_destroy(q);
	q_destory_temp_tail(q);

#if defined(VK_MSG_DEBUG)
	TAILQ_REMOVE(&msg_q_list_head, q, entries);
#endif

	if(q != NULL)
	{
		DD_MEM_Free(q);
	}

	PrintExit();

	return VK_OK;
}

int VK_MSG_Clear(unsigned long q_id)
{
	struct vk_msg_queue 			  *q;
	int rc = VK_OK;
	//	  struct timespec cur_time;
	long long exp_nsec;
	struct timespec timeo, cur_time;
	unsigned long size;
	void			*pBuf;


	PrintEnter();

	q = (struct vk_msg_queue *)q_id;
	if (q == NULL)
	{
        PrintError("%s[%d] : VK_MSG_Clear Error\n", __FUNCTION__, __LINE__);
		return VK_ERROR;
	}

	size = q->msg_size_;
	pBuf = DD_MEM_Alloc(size);
	if (pBuf == NULL)
	{
        PrintError("%s[%d] : VK_MSG_Clear Error\n", __FUNCTION__, __LINE__);
		return VK_ERROR;
	}

	(void)pthread_mutex_lock(&q->q_lock_);
	do
	{
 		clock_gettime(CLOCK_REALTIME, &cur_time);
		exp_nsec = (long long)cur_time.tv_sec * (long long)1000 * (long long)1000 * (long long)1000 + (long long)cur_time.tv_nsec;
		timeo.tv_sec = exp_nsec / (1000 * 1000 * 1000);
		timeo.tv_nsec = exp_nsec % (1000 * 1000 * 1000);

		rc = list_rem_next(q, pBuf, size, TRUE, &timeo);
		if (0 != rc)
		{
			if(ETIMEDOUT == rc)
			{
				(void)pthread_mutex_unlock(&q->q_lock_);
				DD_MEM_Free(pBuf);
				return VK_OK;
			}
		}
	} while (1);
#ifdef DEBUG_MSG_LIST_PRINT
	print_list(q);
#endif

	(void)pthread_mutex_unlock(&q->q_lock_);

	DD_MEM_Free(pBuf);
	PrintExit();
	return VK_OK;
}


/*
    VK_MSG_Send
    메세지를 보낸다.
*/
int VK_MSG_Send(unsigned long q_id, const void *msg, unsigned long size)
{
	struct vk_msg_queue *q;
	int rc = VK_ERROR;
	unsigned long prio;

	PrintEnter();

	q = (struct vk_msg_queue *)q_id;
	if (q == NULL)
	{
		PrintError("[%s,%d] Error! invalid queue id(0x%.8x)\n", __FUNCTION__, __LINE__, q_id);
		goto done;
	}

	if (size > q->msg_size_)
	{
		PrintError("[%s,%d] Error! message size(%d) is bigger than queue msg size(%d)\n", __FUNCTION__, __LINE__, size, q->msg_size_);
		goto done;
	}

	VK_TASK_CheckCallback();

	rc = P_MSG_Check(q, q_id);
	if (VK_OK != rc)
	{
		PrintError("[%s,%d] Error! msg queue full\n(queue: %s)\n", __func__, __LINE__, q->name);
		goto done;		
	}

	if (VK_SUSPENDTYPE_PRIORITY == q->suspend_type_)
	{
		rc = list_find_msg_q_priority_from_task_priority(&prio);
		if(rc != VK_OK)
		{
			prio = MSG_PRIORITY0;
		}
	}
	else
	{
		prio = MSG_PRIORITY0;
	}

	(void)pthread_mutex_lock(&q->q_lock_);

	rc = list_ins_next(q, prio, msg, size, FALSE, NULL);
	if (VK_OK != rc)
	{
		(void)pthread_mutex_unlock(&q->q_lock_);
		PrintError("[%s,%d] Error! list_ins_next() rc(%d)\n", __FUNCTION__, __LINE__, rc);        
		goto done;
	}

#ifdef DEBUG_MSG_LIST_PRINT
	print_list(q);
#endif

	(void)pthread_mutex_unlock(&q->q_lock_);

#ifdef VK_MSG_DEBUG
	q->lastSendTick = VK_TIMER_GetSystemTick();
#endif

	rc = VK_OK;
done:
	if (rc != VK_OK)
	{
		(void)vk_p_common_PrintReturnFunction(vk_p_common_GetPid(), __builtin_return_address(0), __FUNCTION__, __LINE__);
	}
	PrintExit();
	return rc;
}



/*
    VK_MSG_SendTimeout
    메세지를 Timeout내에 보낸다.
*/
int VK_MSG_SendTimeout(unsigned long q_id, const void *msg, unsigned long size, unsigned long timeout)
{
	struct vk_msg_queue *q;
	int rc = VK_ERROR;
	unsigned long prio = 0;
	struct timespec timeo;
	struct timespec cur_time;
	long long exp_nsec;

	PrintEnter();

	if (timeout == 0xFFFFFFFF)
	{
		return VK_MSG_Send(q_id, msg, size);
	}

	q = (struct vk_msg_queue *)q_id;
	if (q == NULL)
	{
		PrintError("[%s,%d] Error! invalid queue id(0x%.8x)\n", __FUNCTION__, __LINE__, q_id);
		goto done;
	}

	if (size > q->msg_size_)
	{
		PrintError("[%s,%d] Error! message size(%d) is bigger than queue msg size(%d)\n", __FUNCTION__, __LINE__, size, q->msg_size_);
		goto done;
	}	

	if (VK_SUSPENDTYPE_PRIORITY == q->suspend_type_)
	{
		rc = list_find_msg_q_priority_from_task_priority(&prio);
		if(rc != VK_OK)
		{
			prio = MSG_PRIORITY0;
		}
	}
	else
	{
		prio = MSG_PRIORITY0;
	}

	if(timeout == 0)
	{
		timeo.tv_sec = 0;
		timeo.tv_nsec = 0;
	}
	else
	{   
		clock_gettime(CLOCK_REALTIME, &cur_time);        
		exp_nsec = (long long)cur_time.tv_sec * (long long)1000 * (long long)1000 * (long long)1000 + (long long)cur_time.tv_nsec;
		exp_nsec += (long long)(timeout * (long long)1000) * (long long)1000;
		timeo.tv_sec = exp_nsec / (1000 * 1000 * 1000);
		timeo.tv_nsec = exp_nsec % (1000 * 1000 * 1000);
	}

	(void)pthread_mutex_lock(&q->q_lock_);

	rc = list_ins_next(q, prio, msg, size, TRUE, &timeo);
	if (0 != rc)
	{
	    (void)pthread_mutex_unlock(&q->q_lock_);

	    if (ETIMEDOUT == rc)
	    {
	        PrintDebug("[%s,%d] TIMEOUT! list_ins_next() rc(%d)\n", __FUNCTION__, __LINE__, rc);
	        goto done;
	    }
	    else
	    {
	        PrintError("[%s,%d] Error! list_ins_next() rc(%d)\n", __FUNCTION__, __LINE__, rc);
	        goto done;
	    }
	}

#ifdef DEBUG_MSG_LIST_PRINT
	print_list(q);
#endif

	(void)pthread_mutex_unlock(&q->q_lock_);

	rc = VK_OK;
done:
	if (rc != VK_OK)
	{
		(void)vk_p_common_PrintReturnFunction(vk_p_common_GetPid(), __builtin_return_address(0), __FUNCTION__, __LINE__);
	}
	PrintExit();
	return rc;
}


/*
    VK_MSG_SendUrgent
    긴급 메세지를 보낸다.
*/
int VK_MSG_SendUrgent(unsigned long q_id, const void *msg, unsigned long size)
{
    struct vk_msg_queue *q;
    int rc = VK_OK;
    unsigned long prio;

    PrintEnter();

    q = (struct vk_msg_queue *)q_id;
    if (q == NULL)
    {
        PrintError("%s[%d] : VK_MSG_SendUrgent Error\n", __FUNCTION__, __LINE__);
		return VK_ERROR;
    }

    if (size > q->msg_size_)
    {
        PrintError("%s[%d] : size %ld Error\n", __FUNCTION__, __LINE__, size);
        return VK_ERROR;
    }

	rc = P_MSG_Check(q, q_id);
	if (VK_OK != rc)
	{
#if defined(CONFIG_DEBUG)
		PrintError("%s(%d) : P_MSG_Check : VK_MSG_FULL!! queue name (%s)\n", __func__, __LINE__, q->name);
#endif
		return rc;
	}

    if (VK_SUSPENDTYPE_PRIORITY == q->suspend_type_)
    {
        prio = MSG_PRIORITY_MAX-1; //가장 높은 우선순위로 보낸다.
    }
    else
    {
        prio = MSG_PRIORITY0;
    }

    (void)pthread_mutex_lock(&q->q_lock_);

    rc = list_ins_next(q, prio, msg, size, FALSE, FALSE);
    if (0 != rc)
    {
        PrintError("%s : list_ins_next() rc[%d] Error!!\n", __FUNCTION__, rc);
        (void)pthread_mutex_unlock(&q->q_lock_);
        return rc;
    }

#ifdef DEBUG_MSG_LIST_PRINT
    print_list(q);
#endif

    (void)pthread_mutex_unlock(&q->q_lock_);

#ifdef VK_MSG_DEBUG
    q->lastSendTick = VK_TIMER_GetSystemTick();
#endif

    PrintExit();

    return VK_OK;
}


/*
    VK_MSG_SendUrgentTimeout
    긴급 메세지를 Timeout 내에 보낸다.
*/
int VK_MSG_SendUrgentTimeout(unsigned long q_id, const void *msg, unsigned long size, unsigned long timeout)
{
	struct vk_msg_queue *q;
	int rc = VK_OK;
	unsigned long prio = 0;
	struct timespec timeo;
	struct timespec cur_time;
	long long exp_nsec;

	PrintEnter();

	if (timeout == 0xFFFFFFFF)
	{
		return VK_MSG_SendUrgent(q_id, msg, size);
	}

	q = (struct vk_msg_queue *)q_id;
    if (q == NULL)
    {
        PrintError("%s[%d] : VK_MSG_SendUrgentTimeout Error\n", __FUNCTION__, __LINE__);
		return VK_ERROR;
    }

	if(size > q->msg_size_)
	{
	    PrintError("%s[%d] : size %ld Error\n", __FUNCTION__, __LINE__, size);
	    return VK_ERROR;
	}	

	if(VK_SUSPENDTYPE_PRIORITY == q->suspend_type_)
	{
	    prio = MSG_PRIORITY_MAX-1; //가장 높은 우선순위로 보낸다.
	}
	else
	{
	    prio = MSG_PRIORITY0;
	}

	if(timeout == 0)
	{
		timeo.tv_sec = 0;
		timeo.tv_nsec = 0;
	}
	else
	{
		clock_gettime(CLOCK_REALTIME, &cur_time);
		exp_nsec = (long long)cur_time.tv_sec * (long long)1000 * (long long)1000 * (long long)1000 + (long long)cur_time.tv_nsec;
		exp_nsec += (long long)(timeout * (long long)1000) * (long long)1000;
		timeo.tv_sec = exp_nsec / (1000 * 1000 * 1000);
		timeo.tv_nsec = exp_nsec % (1000 * 1000 * 1000);
	}

	(void)pthread_mutex_lock(&q->q_lock_);

	rc = list_ins_next(q, prio, msg, size, TRUE, &timeo);

	if (0 != rc)
	{
	    (void)pthread_mutex_unlock(&q->q_lock_);

	    if (ETIMEDOUT == rc)
	    {
	        PrintDebug("%s : list_ins_next() rc[%d] VK_TIMEOUT!!\n", __FUNCTION__, rc);
	        return VK_TIMEOUT;
	    }
	    else
	    {
	        PrintError("%s : list_ins_next() rc[%d] Error!!\n", __FUNCTION__, rc);
	        return VK_ERROR;
	    }
	}

#ifdef DEBUG_MSG_LIST_PRINT
	print_list(q);
#endif

	(void)pthread_mutex_unlock(&q->q_lock_);

	PrintExit();

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
    struct vk_msg_queue               *q;
//  unsigned long prio;
    int rc = VK_OK;

    PrintEnter();

	VK_TASK_CheckCallback();

    q = (struct vk_msg_queue *)q_id;
    if (q == NULL)
    {
        PrintError("%s[%d] : VK_MSG_Receive Error\n", __FUNCTION__, __LINE__);
		return VK_ERROR;
    }

    if(size > q->msg_size_)
    {
        PrintError("%s : size %ld Error!!\n", __FUNCTION__, size);
        return VK_ERROR;
    }

    (void)pthread_mutex_lock(&q->q_lock_);

#if defined(VK_MSG_DEBUG) && defined(VK_TASK_DEBUG)
	q->receiveTick = VK_TIMER_GetSystemTick();
	if (q->lastReceiveTick != 0 && (q->receiveTick - q->lastReceiveTick > VK_MSG_NORMAL_DELAY_TICK))
	{
		if ((q->receiveTick - tickForVkDebug) > VK_MSG_CRITICAL_DELAY_TICK)
		{
			PrintError("VK_MSG_Receive : It has excuted too long time (%d) queue(%s)\n", q->receiveTick - q->lastReceiveTick, q->name);
		}
	}
#endif
    rc = list_rem_next(q, msg, size, FALSE, NULL);
#if defined(VK_MSG_DEBUG) && defined(VK_TASK_DEBUG)
	q->lastReceiveTick = tickForVkDebug = VK_TIMER_GetSystemTick();
#endif
    if(VK_OK != rc)
    {
        PrintError("%s : list_rem_next() rc[%d] Error!!\n", __FUNCTION__, rc);
        (void)pthread_mutex_unlock(&q->q_lock_);
        return rc;
    }

#ifdef DEBUG_MSG_LIST_PRINT
    print_list(q);
#endif
    (void)pthread_mutex_unlock(&q->q_lock_);

    PrintExit();
    return VK_OK;
}

/*
    VK_MSG_ReceiveTimeout
    메세지를 Timeout내에 받는다.
*/
int VK_MSG_ReceiveTimeout(unsigned long q_id, void *msg, unsigned long size, unsigned long timeout)
{
	struct vk_msg_queue               *q;
	int rc = VK_OK;
	//    struct timespec cur_time;
	long long exp_nsec;
	struct timespec timeo, cur_time;

	PrintEnter();
	if (timeout != 0)
	{
		VK_TASK_CheckCallback();
	}

	if (timeout == 0xFFFFFFFF)
	{
		return VK_MSG_Receive(q_id, msg, size);
	}

	q = (struct vk_msg_queue *)q_id;
    if (q == NULL)
    {
        PrintError("%s[%d] : VK_MSG_ReceiveTimeout Error\n", __FUNCTION__, __LINE__);
		return VK_ERROR;
    }

	if(size > q->msg_size_)
	{
		PrintError("%s : size %ld Error!!\n", __FUNCTION__, size);
		return VK_ERROR;
	}

	if(timeout == 0)
	{
		timeo.tv_sec = 0;
		timeo.tv_nsec = 0;
	}
	else
	{
		clock_gettime(CLOCK_REALTIME, &cur_time);
		exp_nsec = (long long)cur_time.tv_sec * (long long)1000 * (long long)1000 * (long long)1000 + (long long)cur_time.tv_nsec;
		exp_nsec += (long long)(timeout * (long long)1000) * (long long)1000;
		timeo.tv_sec = exp_nsec / (1000 * 1000 * 1000);
		timeo.tv_nsec = exp_nsec % (1000 * 1000 * 1000);
	}

	(void)pthread_mutex_lock(&q->q_lock_);
#if defined(VK_MSG_DEBUG) && defined(VK_TASK_DEBUG)
	q->receiveTick = VK_TIMER_GetSystemTick();
	if (q->lastReceiveTick != 0 && (q->receiveTick - q->lastReceiveTick > VK_MSG_NORMAL_DELAY_TICK))
	{
		if ((q->receiveTick - tickForVkDebug) > VK_MSG_CRITICAL_DELAY_TICK)
		{
			PrintError("VK_MSG_ReceiveTimeout : It has excuted too long time (%d) queue(%s)\n", q->receiveTick - q->lastReceiveTick, q->name);
		}
	}
#endif

	rc = list_rem_next(q, msg, size, TRUE, &timeo);
#if defined(VK_MSG_DEBUG) && defined(VK_TASK_DEBUG)
	q->lastReceiveTick = tickForVkDebug = VK_TIMER_GetSystemTick();
#endif
	if (0 != rc)
	{
        (void)pthread_mutex_unlock(&q->q_lock_);

        if(ETIMEDOUT == rc)
        {
            PrintDebug("%s : list_rem_next() rc[%d] VK_TIMEOUT!!\n", __FUNCTION__, rc);
            return VK_TIMEOUT;
        }
        else
        {
            PrintError("%s : list_rem_next() rc[%d] Error!!\n", __FUNCTION__, rc);
            return VK_ERROR;
        }
	}

#ifdef DEBUG_MSG_LIST_PRINT
	print_list(q);
#endif

	(void)pthread_mutex_unlock(&q->q_lock_);

	PrintExit();
	return VK_OK;
}

int	VK_MSG_GetMessageCount(unsigned long q_id, unsigned long *pulMsgCount, unsigned long *pulMaxMsgCount)
{
    struct vk_msg_queue *q;

    PrintEnter();

    q = (struct vk_msg_queue *)q_id;
    if (q == NULL)
    {
        PrintError("%s[%d] : VK_MSG_GetMessageCount Error\n", __FUNCTION__, __LINE__);
		return VK_ERROR;
    }

	(void)pthread_mutex_lock(&q->q_lock_);
	*pulMsgCount = q->curr_msg_count_;
	*pulMaxMsgCount = q->msg_count_;
	(void)pthread_mutex_unlock(&q->q_lock_);

	PrintExit();
	return VK_OK;
}

static int P_MSG_Check(struct vk_msg_queue *q, unsigned long q_id)
{
	unsigned long ulNearlyFullCount;
#if !defined(CONFIG_DEBUG)
	UNUSED(q_id);
#endif

	ulNearlyFullCount = (q->msg_count_ * 3);
	ulNearlyFullCount = ulNearlyFullCount/4;

	if (q->msg_count_ == q->curr_msg_count_)
	{
#if defined(CONFIG_DEBUG)
		if (VK_TIMER_GetSystemTick() - tickForVkDebug > 5000)
		{
			tickForVkDebug = VK_TIMER_GetSystemTick();
			// queue is full.
#if defined(CONFIG_DEBUG)
			PrintError("\n!!\n!!! %s(): WARNING: queue_id 0x%X, name:%s full !!!%c\n!!!\n", __FUNCTION__, q_id, q->name, 7);
	#if defined(VK_TASK_DEBUG)
			VK_TASK_PrintTaskList();
	#endif
	#if defined(VK_MSG_DEBUG)
			VK_MSG_PrintMsgQList();
	#endif
	#if defined(VK_SEM_DEBUG)
			VK_SEM_PrintSemList();
	#endif
#else
			PrintError("\n!!\n!!! %s(): WARNING: queue_id 0x%X, full !!!%c\n!!!\n", __FUNCTION__, q_id, 7);
#endif
		}
#endif
		return VK_MSG_FULL;
	}
	else if( q->curr_msg_count_ > ulNearlyFullCount )
	{
#if defined(CONFIG_DEBUG)
		PrintError("[VK_MSG_Send] MsgQue is nearly full - SendTask:%d, QueId:0x%X, (%ld, %ld), name:%s\r\n",
				pthread_self(), q_id, q->msg_count_, q->curr_msg_count_, q->name);
#endif
	}

	return VK_OK;
}


/*
    VK_MSG_PrintMsgQList
    메세지큐 정보를 출력한다.
*/
#if defined(VK_MSG_DEBUG)
void VK_MSG_PrintMsgQList(void)
{
	HINT32	ulCnt=0;
	struct vk_msg_queue *pMsgQ;

    PrintDebug("**********************************************************\n");
    PrintDebug("                Name Qid        Size Count Cur_Count Type\n");
    for (pMsgQ = msg_q_list_head.tqh_first; pMsgQ != NULL; pMsgQ = pMsgQ->entries.tqe_next)
    {
        PrintDebug("[%03d]%20s 0x%08X %04d %05d %09d %04d\n",ulCnt,
                                pMsgQ->name, (unsigned long)pMsgQ, pMsgQ->msg_size_, pMsgQ->msg_count_, pMsgQ->curr_msg_count_, pMsgQ->suspend_type_);
        ulCnt++;
    }
    PrintDebug("**********************************************************\n");
}
#endif


