/***************************************************************************
*     Copyright (c) 2005-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: int1.h $
* $brcm_Revision: Hydra_Software_Devel/3 $
* $brcm_Date: 9/14/12 10:18a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /rockford/appframework/src/common/appframework/int1.h $
* 
* Hydra_Software_Devel/3   9/14/12 10:18a pblanco
* SW7346-572: Added code to make framework interrupt setup and teardown
* symmetrical.
* 
* Hydra_Software_Devel/2   8/13/09 5:44p tdo
* PR55225: Increase support to 96 interrupt bits
*
* Hydra_Software_Devel/1   6/16/06 3:59p maivu
* PR 22212: Move to common directory
*
* Hydra_Software_Devel/4   4/13/06 1:55p jessem
* PR 18367: Rearrange and reorganized typedefs.
*
* Hydra_Software_Devel/3   4/11/06 1:34p jessem
* PR18367: Reorganized functions in int1.c and intmgr.c to use VxWorks
* bsp functions. Modified memmgr.c to work under VxWorks.
*
* Hydra_Software_Devel/1   3/10/06 12:45p maivu
* PR 18367: Initial Vxworks support
***************************************************************************/

#ifndef _INT1_H
#define _INT1_H

#include "bint.h"
#include "intmgr.h"

#if __cplusplus
extern "C" {
#endif


/***************************************************************************
 *  Level 1 ISR Type Definition
 **************************************************************************/
typedef void (*FN_L1_ISR) (void *, int);


/***************************************************************************
 *  Level 1 Interrupt Control Structure
 **************************************************************************/
typedef struct Int1Control {

	unsigned long  IntrW0Status;
	unsigned long  IntrW1Status;
	unsigned long  IntrW2Status;
	unsigned long  IntrW0MaskStatus;
	unsigned long  IntrW1MaskStatus;
	unsigned long  IntrW2MaskStatus;
	unsigned long  IntrW0MaskSet;
	unsigned long  IntrW1MaskSet;
	unsigned long  IntrW2MaskSet;
	unsigned long  IntrW0MaskClear;
	unsigned long  IntrW1MaskClear;
	unsigned long  IntrW2MaskClear;
} Int1Control;

/***************************************************************************
 *  Level 1 Interrupt Function Prototypes
 **************************************************************************/
extern void CPUINT1_SetInt1Control(Int1Control *int1c);
extern unsigned long CPUINT1_GetInt1ControlAddr(void);
extern void CPUINT1_Isr(void);
extern void CPUINT1_Disable(unsigned long intId);
extern void CPUINT1_Enable(unsigned long intId);
extern int  CPUINT1_ConnectIsr(unsigned long intId, FN_L1_ISR pfunc, void *param1, int param2);
extern void CPUINT1_DisconnectIsr(unsigned long intId);

#if __cplusplus
}
#endif

#endif /* _INT1_H */

