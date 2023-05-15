/***************************************************************************
*     Copyright (c) 2003-2006, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: intmgr.c $
* $brcm_Revision: Hydra_Software_Devel/4 $
* $brcm_Date: 4/13/06 1:56p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /rockford/appframework/src/vxworks/intmgr.c $
* 
* Hydra_Software_Devel/4   4/13/06 1:56p jessem
* PR18367: Added 7411 support. Removed typedefs and placed in header file
* instead.
* 
* Hydra_Software_Devel/3   4/11/06 1:34p jessem
* PR18367: Reorganized functions in int1.c and intmgr.c to use VxWorks
* bsp functions. Modified memmgr.c to work under VxWorks.
* 
* Hydra_Software_Devel/1   3/10/06 12:46p maivu
* PR 18367: Initial Vxworks support
* 
***************************************************************************/
 
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "bstd.h"
#include "intmgr.h"
#include "btst_kni.h"
#include "bkni.h"

#include <sysLib.h>
#include <taskLib.h>
#include <msgQLib.h>
#include <logLib.h>

/* We redefine this to use OUR register ISR function in order to capture the L1 shift value.
** Use the real register ISR function in THIS file. */
#undef CPUINT1_ConnectIsr
#include "int1.h"
#include "intLib.h" 


BDBG_MODULE(INTMGR);

isrInfo isr_array[MAX_INTERRUPTS];

typedef struct 
{
	int     intMsgStartId;
	void*   intHandle;
	int     L1Shift;
    int     intId;
	int     intMsgEndId;
} INT_MSG;

static struct {
	int      taskId;
	MSG_Q_ID msgQId;
} isrTask;

#define BINT_Isr b_vxworks_Isr

void b_vxworks_Isr
( 
	BINT_Handle intHandle, 
	int intId
)
{
	INT_MSG intMsg;

#ifdef INTERRUPT_MONITOR
	static int led_isr=0;
	unsigned long addr = BCM3250_REG_BASE;
	volatile unsigned char *led = (unsigned char *)(addr + BCM3250_UPG_CORE_OFFSET + LED_DIGIT1);
	*led = led_isr;
	led_isr++;
#endif
		
	CPUINT1_Disable(intId);

    /* check to make sure that we can retrieve L1Shift */
	intMsg.intMsgStartId = INT_MSG_START_ID;
	intMsg.intHandle     =  intHandle;
	intMsg.L1Shift       = isr_array[intId].L1Shift; /* or L1Shift */
    intMsg.intId         = intId; /* IntID */  
	intMsg.intMsgEndId   = INT_MSG_END_ID;
	if (msgQSend(isrTask.msgQId, (char *)&intMsg.intMsgStartId, sizeof(INT_MSG), NO_WAIT,
				MSG_PRI_NORMAL) != OK)
		logMsg("b_vxworks_Isr: msg send failed\n", 0,0,0,0,0,0);
}

static void vxInterruptTask(void)
{
	INT_MSG intMsg;
	int     rc;
    int     key = 0;

	for(;;) {

#ifdef INTERRUPT_MONITOR
		static int led_isr=0;
		unsigned long addr = BCM3250_REG_BASE;
		volatile unsigned char *led = (unsigned char *)(addr + BCM3250_UPG_CORE_OFFSET + LED_DIGIT4);
		*led = led_isr;
		led_isr++;
#endif

	 	/* Wait for the new ints message to come in */
	   	rc = msgQReceive(isrTask.msgQId, (char *)&intMsg.intMsgStartId, sizeof(INT_MSG), 100 /*WAIT_FOREVER*/);

		if (rc == ERROR || rc != sizeof(INT_MSG)) {
			BDBG_ERR(("msgQReceive failed rc=%d", rc));
			continue;
		}
		if ((intMsg.intMsgStartId != INT_MSG_START_ID) ||
   			(intMsg.intMsgEndId != INT_MSG_END_ID) ) {
			BDBG_ERR(("invalid id: %d %d", intMsg.intMsgStartId, intMsg.intMsgEndId));
			break;
    	}

        if (isr_array[intMsg.intId].pfunc == NULL) 
        {
            BDBG_ERR(("No ISR for id: %d", intMsg.intId));
			break;
        }

        /* jrubio: This intLock must be here otherwise we may end up in a 
            deadlock state. For Daughter Cards the interrupt is 
            enabled within the isrfunction below */
		BKNI_EnterCriticalSection();
        key = intLock();
        CPUINT1_Enable(intMsg.intId);
        (*isr_array[intMsg.intId].pfunc)(intMsg.intHandle,intMsg.L1Shift);
        intUnlock(key);
        BKNI_LeaveCriticalSection();
            
    }
	return;
}

/* b_intmgr_vxInit */
int intmgr_Init( void )
{
	isrTask.msgQId = msgQCreate(MAX_INT_MSG, sizeof(INT_MSG), MSG_Q_FIFO);

	if (isrTask.msgQId == NULL)
	{
		BDBG_ERR(("msgQCreate failed"));
		return -1;
	}
	
	/* Create the task for interrupt handling.
	TASK_PRIORITY_HIGH is 60, which is not high enough. We chose
	10 to still leave a little headroom for other tasks. */
	isrTask.taskId = taskSpawn((char *) "vxInterruptTask", 10,
			0 /* no floating point VX_FP_TASK */, 
			0x1000, (FUNCPTR) vxInterruptTask,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	if ( isrTask.taskId == ERROR )
	{
		BDBG_ERR(("b_intmgr_vxInit: failed, can't create task"));
		return -1;
	}

    memset(isr_array, 0, sizeof(isrInfo) * 64);
    return 0;
}

void intmgr_Uninit( void )
{
	BKNI_EnterCriticalSection();
	taskDelete( isrTask.taskId);
	BKNI_LeaveCriticalSection();
	msgQDelete( isrTask.msgQId);
}

/***********************************************************************
 * redefined CPUINT1_ConnectIsr() for Vxworks
 ***********************************************************************/
int CPUINT1_ConnectIsr_vxWorks
(
	unsigned long intId, 
	void * pfunc, 
	void *param1, 
	int param2
)
{
    if (intId >= 64 ) 
    {
        BDBG_ERR(("CPUINT1_ConnectIsr_vxWorks: Interrupt Number is greater then 64 or less then 0: intId = %0x, %d",intId, intId));
		return -1;
    }
    
    /* insert function in isr array */
    isr_array[intId].pfunc = (void *)pfunc;
    isr_array[intId].L1Shift= param2;

    /* L1Shift is saved above and intId is passed down */
    CPUINT1_ConnectIsr(intId, (FN_L1_ISR) b_vxworks_Isr ,param1,intId);

    return 0;

}

