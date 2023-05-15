int IPC_TASK_Init(void);
int IPC_TASK_Create(void (*start_func)(void *), unsigned long prio,unsigned long stack_size, const char *name, void *arg, unsigned long *taskId, int IsJoinable);
int IPC_TASK_Join(unsigned long taskId);
int IPC_TASK_Start(unsigned long taskId);
int IPC_TASK_Stop(unsigned long taskId);
int IPC_TASK_Sleep(unsigned long ultime);
int IPC_TASK_GetCurrentId(unsigned long *taskId);
int IPC_TASK_Destroy(unsigned long taskId);
void IPC_TASK_Lock(void);
void IPC_TASK_Unlock(void);
int IPC_TASK_GetCurrentPriority(unsigned long taskId, unsigned long *currpriority);
int IPC_TASK_SetCurrentPriority(unsigned long taskId, unsigned long newpriority);
int IPC_SEM_Create(unsigned long * sem_id, const char *name, unsigned int etSuspendType);
int IPC_SEM_CreateWithCount(unsigned long *sem_id, unsigned long count, const char *name, unsigned int etSuspendType);
int IPC_SEM_Get(unsigned long sem_id);
int IPC_SEM_GetTimeout(unsigned long sem_id, unsigned long timeout );
int IPC_SEM_Release(unsigned long sem_id);
int IPC_SEM_Destroy(unsigned long sem_id);
int IPC_SEM_CreateWithCountEx(unsigned long *sem_id, unsigned long count, const char *name, unsigned int suspend_type);
int IPC_MSG_Create(unsigned long q_count, unsigned long msg_size,
	const char *q_name, unsigned long *q_id, unsigned int suspendType);
int IPC_MSG_Destroy(unsigned long q_id);
int IPC_MSG_Clear(unsigned long q_id);
int IPC_MSG_Send(unsigned long q_id, const void *msg, unsigned long size);
int IPC_MSG_SendTimeout(unsigned long q_id, const void *msg, unsigned long size, unsigned long timeout);
int IPC_MSG_SendUrgent(unsigned long q_id, const void *msg, unsigned long size);
int IPC_MSG_SendUrgentTimeout(unsigned long q_id, const void *msg, unsigned long size, unsigned long timeout);
int IPC_MSG_Receive(unsigned long q_id, void *msg, unsigned long size);
int IPC_MSG_ReceiveTimeout(unsigned long q_id, void *msg, unsigned long size, unsigned long timeout);
int	IPC_MSG_GetMessageCount(unsigned long q_id, unsigned long *pulMsgCount, unsigned long *pulMaxMsgCount);









	























