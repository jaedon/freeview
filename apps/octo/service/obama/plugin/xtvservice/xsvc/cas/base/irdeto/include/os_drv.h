
#ifndef _OS_DRV_H_
#define _OS_DRV_H_


#define ____________________OS_DEFINITIONS__________________________________
/* OS definitions for Irdeto module */
#define HMXIR3_TASK_PRIORITY		(MW_CAS_TASK_PRIORITY)
#define HMXIR3_TASK_STACK_SIZE		(MW_CAS_TASK_STACK_SIZE)
#define HMXIR3_TASK_MSGQ_SIZE		(MW_CAS_MSGQ_SIZE)

#define IR3SCELL_TASK_PRIORITY_11	(MW_CAS_TASK_PRIORITY)
#define IR3SCELL_TASK_PRIORITY_12	(MW_CAS_TASK_PRIORITY+1)
#define IR3SCELL_TASK_PRIORITY_13	(MW_CAS_TASK_PRIORITY+2)
#define IR3SCELL_TASK_PRIORITY_14	(MW_CAS_TASK_PRIORITY+3)

#define IR3SCELL_TASK_STACK_SIZE	(MW_CAS_TASK_STACK_SIZE)
#define IR3SCELL_TASK_MSGQ_SIZE	(MW_CAS_MSGQ_SIZE)

typedef struct
{
	void	*pvMsg;
	HUINT16	usMsgLength;
} OS_DRV_MESSAGE;

typedef struct
{
    HUINT32	task_id;
    HUINT32 queue_id;
    void    (*task_ptr)(void *arg);
} IR_OS_task_info;

/********************************************************************
*   extern variable
********************************************************************/

/********************************************************************
*   Global Function Prototypes definitions
*********************************************************************/


#endif	/*	_OS_DRV_H_	*/

