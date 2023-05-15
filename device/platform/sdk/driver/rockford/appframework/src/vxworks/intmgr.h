/***************************************************************************
*     Copyright (c) 2003-2006, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: intmgr.h $
* $brcm_Revision: Hydra_Software_Devel/5 $
* $brcm_Date: 6/20/06 12:43p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /rockford/appframework/src/vxworks/intmgr.h $
* 
* Hydra_Software_Devel/5   6/20/06 12:43p maivu
* PR 22212: Cleanup
* 
* Hydra_Software_Devel/4   4/13/06 1:55p jessem
* PR 18367: Rearrange and reorganized typedefs.
* 
* Hydra_Software_Devel/3   4/11/06 1:34p jessem
* PR18367: Reorganized functions in int1.c and intmgr.c to use VxWorks
* bsp functions. Modified memmgr.c to work under VxWorks.
* 
* Hydra_Software_Devel/1   3/10/06 12:46p maivu
* PR 18367: Initial Vxworks support
* 
***************************************************************************/
 
#ifndef INTMGR_H
#define INTMGR_H

#include "bint.h"

#define MAX_INT_MSG               64
#define TASK_PRIORITY_HIGH        60
#define INT_MSG_START_ID        1234
#define INT_MSG_END_ID          6789
#define MAX_INTERRUPTS            64


/* We wrap the real connect ISR function with our own version */
#define CPUINT1_ConnectIsr CPUINT1_ConnectIsr_vxWorks

#if 0
/* Typedef for function pointer */
typedef void *(*isr_t)();

typedef struct
{
    isr_t pfunc;
    int L1Shift;
}isrInfo;
#endif

void intmgr_EnableIsrs( BINT_Handle intHandle );

#if (BCHP_7411_VER >= BCHP_VER_C0)
void intmgr_Enable7411_Isr( 
	BINT_Handle *phInt0,
	BINT_Handle *phInt1 );
#endif

int intmgr_Init( void ); 
void intmgr_Uninit(void);

int intmgr_DebugLevelSet(int level);

#endif

