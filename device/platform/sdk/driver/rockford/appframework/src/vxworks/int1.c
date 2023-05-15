/***************************************************************************
*     Copyright (c) 2003-2006, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: int1.c $
* $brcm_Revision: Hydra_Software_Devel/5 $
* $brcm_Date: 6/20/06 12:41p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /rockford/appframework/src/vxworks/int1.c $
* 
* Hydra_Software_Devel/5   6/20/06 12:41p maivu
* PR 22212: Clean up redundant code
* 
* Hydra_Software_Devel/4   4/13/06 1:54p jessem
* PR18367: Added 7411 support.
* 
* Hydra_Software_Devel/3   4/11/06 1:34p jessem
* PR18367: Reorganized functions in int1.c and intmgr.c to use VxWorks
* bsp functions. Modified memmgr.c to work under VxWorks.
* 
* Hydra_Software_Devel/1   3/10/06 12:44p maivu
* PR 18367: Initial Vxworks support
* 
***************************************************************************/

#include "int1.h"
#include "platform.h"

#if (BCHP_7411_VER >= BCHP_VER_C0)
#include "bchp_hif_cpu_intr1.h"
#include "bint_7411.h"
#endif

extern isrInfo isr_array[MAX_INTERRUPTS];

/***********************************************************************
 * Local data structures
 ***********************************************************************/
typedef struct
{
	FN_L1_ISR  isr;
	void      *param1;
	int        param2;
} Int1Vector;


/***********************************************************************
 * Local #define's
 ***********************************************************************/
#define INT1_GET_REG_NUM(id)   (((id)&0xe0) >> 5)
#define INT1_GET_BIT_NUM(id)   ((id)&0x1f)
#define INT1_MAX_VECTORS       64

BDBG_MODULE(INT1);

/* Enable all Level 1 _isrs that are mapped in the INT module */
void intmgr_EnableIsrs( BINT_Handle intHandle )
{
	unsigned long i;
	bool enableIsr;
	uint32_t l1masklo, l1maskhi;
	
	BINT_GetL1BitMask( intHandle, &l1masklo, &l1maskhi );
	for( i=0; i<64; i++ )
	{
		enableIsr = false;
		if( i >=32 )
		{
			if( l1maskhi & 1ul<<(i-32) )
			{
				enableIsr = true;
			}
		}
		else
		{
			if( l1masklo & 1ul<<i )
			{
				enableIsr = true;
			}
		}
		if( enableIsr )
		{
			BDBG_WRN(("Enabling L1 interrupt %ld", i));
			CPUINT1_ConnectIsr(i, (FN_L1_ISR)BINT_Isr, intHandle, i );
			CPUINT1_Enable(i);
		}
	}	
}

#if (BCHP_7411_VER >= BCHP_VER_C0)
void intmgr_Enable7411_Isr
( 
	BINT_Handle *phInt0,
	BINT_Handle *phInt1
)
{
	unsigned long intId = BINT_7411_IRQ_0;

    /* insert function in isr array */
    isr_array[intId].pfunc = (void *)phInt0;
    isr_array[intId].L1Shift= BINT_7411_IRQ_0;  /* Is this correct?? */

    isr_array[intId].pfunc = (void *)phInt1;
    isr_array[intId].L1Shift= BINT_7411_IRQ_0;  /* Is this correct?? */

	CPUINT1_ConnectIsr(BCHP_HIF_CPU_INTR1_INTR_W1_MASK_STATUS_EXT_IRQ_4_CPU_INTR_SHIFT+32, 
	                     (FN_L1_ISR)BINT_Isr, *phInt0, BINT_7411_IRQ_0);
	CPUINT1_ConnectIsr(BCHP_HIF_CPU_INTR1_INTR_W1_MASK_STATUS_EXT_IRQ_5_CPU_INTR_SHIFT+32, 
	                     (FN_L1_ISR)BINT_Isr,*phInt1, BINT_7411_IRQ_0);
	CPUINT1_Enable(BCHP_HIF_CPU_INTR1_INTR_W1_MASK_STATUS_EXT_IRQ_4_CPU_INTR_SHIFT+32);
	CPUINT1_Enable(BCHP_HIF_CPU_INTR1_INTR_W1_MASK_STATUS_EXT_IRQ_5_CPU_INTR_SHIFT+32);
}
#endif


