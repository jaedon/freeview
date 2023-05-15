/***************************************************************************
*     Copyright (c) 2003-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: int1.c $
* $brcm_Revision: Hydra_Software_Devel/8 $
* $brcm_Date: 9/14/12 10:18a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /rockford/appframework/src/linuxuser/int1.c $
* 
* Hydra_Software_Devel/8   9/14/12 10:18a pblanco
* SW7346-572: Added code to make framework interrupt setup and teardown
* symmetrical.
* 
* Hydra_Software_Devel/7   3/18/10 2:04p pntruong
* SW7340-143: Coverity: data over-run issue in BINT.
*
* Hydra_Software_Devel/6   9/1/09 12:57a pntruong
* SW7125-13: Added initial 7125 support.
*
* Hydra_Software_Devel/5   8/13/09 5:44p tdo
* PR55225: Increase support to 96 interrupt bits
*
* Hydra_Software_Devel/4   9/29/08 4:53p rpan
* PR47411: Enable interrupt support for 3548/3556 for new user mode
* driver.
*
* Hydra_Software_Devel/3   7/1/08 10:29a yuxiaz
* PR44351: Coverity Defect ID:9709 OVERRUN_STATIC int1.c
* Product=97405nexus.
*
* Hydra_Software_Devel/2   2/7/08 3:38p yuxiaz
* PR37720: Fixed the problem on 7325 when SDS interrupts are enabled.
*
* Hydra_Software_Devel/1   6/16/06 4:13p maivu
* PR 22212: Move to linuxuser directory
*
* Hydra_Software_Devel/2   9/1/05 2:56p jasonh
* PR 16827: Fixed ID to REG/BIT macros.
*
* Hydra_Software_Devel/1   7/27/05 3:49p jasonh
* PR 16397: Initial cleaned-up version.
*
***************************************************************************/
#include "int1.h"
#include "platform.h"

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

#ifdef BCHP_HIF_CPU_INTR1_INTR_W2_STATUS
#define INT1_MAX_VECTORS       96
#else
#define INT1_MAX_VECTORS       64
#endif

/***********************************************************************
 * Static variables
 ***********************************************************************/
#define CPUINT1C ((Int1Control * const)(CPUINT1C_ADR_BASE))
static Int1Control *CpuInt1Control = CPUINT1C;
static Int1Vector CpuInt1VectorTable[INT1_MAX_VECTORS];


/***********************************************************************
 * void CPUINT1_SetInt1Control()
 *
 * Set the interrupt control structure. Normally it is CPUINT1C.
 * For linux user mode, it should be set to point to a user
 * structure.  If a NULL is passed to this function, it will
 * use set the control structure to CPUINT1C.
 ***********************************************************************/
void CPUINT1_SetInt1Control(Int1Control *int1c)
{
	if (int1c)
		CpuInt1Control = int1c;
	else
		CpuInt1Control = CPUINT1C;
}


/***********************************************************************
 * void CPUINT1_Isr()
 *
 * Main interrupt handler: Handle all CPU L1 interrupts
 ***********************************************************************/
void CPUINT1_Isr(void)
{
   unsigned long IntrW0Status;
   unsigned long IntrW1Status;
   unsigned long IntrW2Status;
   unsigned long IntrW0MaskStatus;
   unsigned long IntrW1MaskStatus;
   unsigned long IntrW2MaskStatus;
   Int1Vector *vec;
   int i;

   /* printf("stat1=%lx mask1=%lx\n", CpuInt1Control->IntrW1Status, CpuInt1Control->IntrW1MaskStatus);
      printf("stat0=%lx mask0=%lx\n", CpuInt1Control->IntrW0Status, CpuInt1Control->IntrW0MaskStatus); */

   if(((IntrW2Status=CpuInt1Control->IntrW2Status) &
       (IntrW2MaskStatus=CpuInt1Control->IntrW2MaskStatus)))
   {

      /* printf("stat1=%x mask1=%x\n", IntrW1Status, IntrW1MaskStatus); */

      for(i = 64; i < INT1_MAX_VECTORS; i++)
      {
         if(IntrW2Status & 0x1)
         {
            vec = &(CpuInt1VectorTable[i]);
            if(vec->isr!=0)
               vec->isr(vec->param1, vec->param2);
         }
         IntrW2Status >>= 1;
      }
   }

   if(((IntrW1Status=CpuInt1Control->IntrW1Status) &
       (IntrW1MaskStatus=CpuInt1Control->IntrW1MaskStatus)))
   {

      /* printf("stat1=%x mask1=%x\n", IntrW1Status, IntrW1MaskStatus); */

      for(i = 32; i < 64; i++)
      {
         if(IntrW1Status & 0x1)
         {
            vec = &(CpuInt1VectorTable[i]);
            if(vec->isr!=0)
               vec->isr(vec->param1, vec->param2);
         }
         IntrW1Status >>= 1;
      }
   }

   if(((IntrW0Status=CpuInt1Control->IntrW0Status) &
       (IntrW0MaskStatus=CpuInt1Control->IntrW0MaskStatus)))
   {
      /* printf("stat0=%x mask0=%x\n", IntrW0Status, IntrW0MaskStatus); */

      for(i = 0; i < 32; i++)
      {
         if(IntrW0Status & 0x1)
         {
            vec = &(CpuInt1VectorTable[i]);
            if(vec->isr!=0)
               vec->isr(vec->param1, vec->param2);
         }
         IntrW0Status >>= 1;
      }
   }
}


/***********************************************************************
 * void  CPUINT1_Disable()
 *
 * Disables CPU or external interrupt specified by 'intId'.  Valid
 * intId values/mnemonics can be found in int1.h.
 ***********************************************************************/
void CPUINT1_Disable(unsigned long intId)
{
   unsigned char reg_num = INT1_GET_REG_NUM(intId);
   unsigned char bit_num = INT1_GET_BIT_NUM(intId);

   /* Don't mess with an un-registered interrupt! */
   if(reg_num == 2) intId = 64 + bit_num;
   if(reg_num == 1) intId = 32 + bit_num;

   if(intId >= INT1_MAX_VECTORS)
   {
      /* printf("intID is not valid\n"); */
      return;
   }

   if(reg_num == 2)
      CpuInt1Control->IntrW2MaskStatus &= ~(0x1 << bit_num);
   else if(reg_num == 1)
      CpuInt1Control->IntrW1MaskStatus &= ~(0x1 << bit_num);
   else
      CpuInt1Control->IntrW0MaskStatus &= ~(0x1 << bit_num);
}


/***********************************************************************
 * void  CPUINT1_Enable()
 *
 * Enables the CPU or external interrupt specified by 'intId'.  Valid
 * intId values/mnemonics can be found in int1.h.
 ***********************************************************************/
void CPUINT1_Enable(unsigned long intId)
{

	unsigned char reg_num = INT1_GET_REG_NUM(intId);
	unsigned char bit_num = INT1_GET_BIT_NUM(intId);

	/* Don't mess with an un-registered interrupt! */
	if(reg_num == 2) intId = 64 + bit_num;
	if(reg_num == 1) intId = 32 + bit_num;

	if(intId >= INT1_MAX_VECTORS)
	{
		/* printf("intID is not valid\n"); */
		return;
	}
	if(!CpuInt1VectorTable[intId].isr) return;

	if(reg_num == 2)
		CpuInt1Control->IntrW2MaskStatus |= 0x1 << bit_num;
	else if(reg_num == 1)
		CpuInt1Control->IntrW1MaskStatus |= 0x1 << bit_num;
	else
		CpuInt1Control->IntrW0MaskStatus |= 0x1 << bit_num;
}


/***********************************************************************
 * int CPUINT1_ConnectIsr()
 *
 * Maps CPU or external interrupts.  Takes the ISR function pointer
 * 'pfunc' and the parameters 'param1' and 'param2' and stores the
 * values in a vector table, indexed by 'intId'.  'param1' and
 * 'param2' will be passed as parameters to the function 'pfunc'.
 * Valid intId values/mnemonics can be found in int1.h.
 ***********************************************************************/
int CPUINT1_ConnectIsr(unsigned long intId, FN_L1_ISR pfunc,
                       void *param1, int param2)
{
	unsigned char reg_num = INT1_GET_REG_NUM(intId);
	unsigned char bit_num = INT1_GET_BIT_NUM(intId);

	if(reg_num == 2)
	{
		/* This for interrupt in W1 register */
		intId = 64 + bit_num;
	}
	else if(reg_num == 1)
	{
		/* This for interrupt in W1 register */
		intId = 32 + bit_num;
	}

	if (intId >= INT1_MAX_VECTORS)
		return 0;

	CpuInt1VectorTable[intId].isr = pfunc;
	CpuInt1VectorTable[intId].param1 = param1;
	CpuInt1VectorTable[intId].param2 = param2;

	return (int) pfunc;
}

void CPUINT1_DisconnectIsr(unsigned long intId)
{
   CpuInt1VectorTable[intId].isr = NULL;
   CpuInt1VectorTable[intId].param1 = 0;
   CpuInt1VectorTable[intId].param2 = 0;
}

