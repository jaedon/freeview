/***************************************************************************
*     (c)2005-2011 Broadcom Corporation
*  
*  This program is the proprietary software of Broadcom Corporation and/or its licensors,
*  and may only be used, duplicated, modified or distributed pursuant to the terms and
*  conditions of a separate, written license agreement executed between you and Broadcom
*  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
*  no license (express or implied), right to use, or waiver of any kind with respect to the
*  Software, and Broadcom expressly reserves all rights in and to the Software and all
*  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
*  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
*  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
*   
*  Except as expressly set forth in the Authorized License,
*   
*  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
*  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
*  and to use this information only in connection with your use of Broadcom integrated circuit products.
*   
*  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
*  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
*  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
*  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
*  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
*  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
*  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
*  USE OR PERFORMANCE OF THE SOFTWARE.
*  
*  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
*  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
*  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
*  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
*  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
*  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
*  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
*  ANY LIMITED REMEDY.
* 
* $brcm_Workfile: baob_irq.c $
* $brcm_Revision: 1 $
* $brcm_Date: 3/21/11 5:57p $
*
* [File Description:]
*
* Revision History:
*
* $brcm_Log: /AP/bcm3128/a0/src/driver/aob/baob_irq.c $
* 
* 1   3/21/11 5:57p farshidf
* SW3128-1: add irq for AOB
* 
***************************************************************************/
#include "bstd.h"
#include "bchp_common.h"
#include "int1_api.h"
#include "bchp_leap_ctrl.h"
#include "bchp_leap_l1.h"
#include "bchp_oob.h"
#include "sysdefs.h"
#include "baob_irq.h"


/***********************************************************************
 * Local data structures
 ***********************************************************************/

/***************************************************************************
 *  Int AobL2 Control Interrupt Control Structure
 **************************************************************************/

typedef struct IntAobl2 {
  unsigned long        IntrAobL2Status;
  unsigned long        IntrAobL2Set;
  unsigned long        IntrAobL2Clr;
  unsigned long        IntrAobL2MaskStatus;
  unsigned long        IntrAobL2MaskSet;
  unsigned long        IntrAobL2MaskClear;
} IntAobL2;                                


typedef struct
{
   FN_AOB_ISR isr;
   void      *param1;
   int       param2;
} IntAobL2Vector;                               



/***********************************************************************
 * Local #define's
 ***********************************************************************/
#define INT_AOB_L2_ADR_BASE  \
				BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET + BCHP_OOB_IRQSTA2A)
	
#define INTAOBL2 ((IntAobL2 * const)(INT_AOB_L2_ADR_BASE))

#define INT_AOB_MAX_VECTORS 6



/***********************************************************************
 * Static variables
 ***********************************************************************/
static IntAobL2 *IntrAobL2Control = INTAOBL2;

static IntAobL2Vector IntrAobL2VectorTable[INT_AOB_MAX_VECTORS];

/***********************************************************************
 * void AobL2_Isr()
 *
 * Main interrupt handler: Handle all AobL2_Isr interrupts
 ***********************************************************************/
void AobL2_Isr(void)
{
   unsigned long IntrAobL2Status;
   unsigned long IntrAobL2MaskStatus;
   IntAobL2Vector *vec;
   int i;

   if (((IntrAobL2Status=IntrAobL2Control->IntrAobL2Status)
        & ~(IntrAobL2MaskStatus=IntrAobL2Control->IntrAobL2MaskStatus)))
   {
      IntrAobL2Status &= ~IntrAobL2MaskStatus;

      for (i = 0; ((i < INT_AOB_MAX_VECTORS) && IntrAobL2Status); i++)
      {
         if (IntrAobL2Status & 0x1)
         {
            vec = &(IntrAobL2VectorTable[i]);
            if (vec->isr!=0)
               vec->isr(vec->param1, vec->param2);
			 IntrAobL2Control->IntrAobL2Clr = (0x1 << i);   /* clear that specific interrupt */
         }   
         IntrAobL2Status >>= 1;
      }
   }
}
/***********************************************************************
 * void  AobL2_Disable()
 *
 * Disables CPU or external interrupt specified by 'intId'.  Valid
 * intId values/mnemonics can be found in int1.h.
 ***********************************************************************/
void AobL2_DisableIsr(unsigned long intId)
{
	IntrAobL2Control->IntrAobL2Clr = (0x1 << intId);
	IntrAobL2Control->IntrAobL2MaskSet = (0x1 << intId);
}


/***********************************************************************
 * AobL2_EnableIsr
 ***********************************************************************/
void AobL2_EnableIsr(unsigned long intId)
{
     IntrAobL2Control->IntrAobL2MaskClear = (0x1 << intId);
}

/***********************************************************************
 * int AobL2B_ConnectIsr()
 *
 ***********************************************************************/
int AobL2_ConnectIsr(unsigned long intId, FN_AOB_ISR pfunc,
                       void *param1, int param2)
{
   if (intId > INT_AOB_MAX_VECTORS)
      return 0;

   IntrAobL2VectorTable[intId].isr = pfunc;
   IntrAobL2VectorTable[intId].param1 = param1;
   IntrAobL2VectorTable[intId].param2 = param2;
   Leap_l1_ConnectIsr(BCHP_LEAP_L1_INTR_STATUS_OOB_INTR_SHIFT,(BINT_CallbackFunc)AobL2_Isr, NULL, 0);
   Leap_l1_Enable(BCHP_LEAP_L1_INTR_STATUS_OOB_INTR_SHIFT);
   return (int) pfunc;
}















