/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_g3_priv_timer.c $
 * $brcm_Revision: Hydra_Software_Devel/14 $
 * $brcm_Date: 10/17/12 10:35a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/g3/common/bast_g3_priv_timer.c $
 * 
 * Hydra_Software_Devel/14   10/17/12 10:35a enavarro
 * SWSATFE-174: 4538 workaround for timer count failing to load
 * 
 * Hydra_Software_Devel/13   9/21/12 2:29p enavarro
 * SWSATFE-174: add debug code for 4538
 * 
 * Hydra_Software_Devel/12   2/22/12 10:16a enavarro
 * SWSATFE-86: added support for BAST_LOG_REG_ACCESS build option
 * 
 * Hydra_Software_Devel/11   3/7/11 8:25a enavarro
 * SWSATFE-86: print BDBG_ERR message when invalid timer is specified
 * 
 * Hydra_Software_Devel/10   3/3/11 10:52a enavarro
 * SWSATFE-75: put all diseqc state variables in BAST_g3_P_DiseqcDevice
 * struct and allocate this only if the channel has diseqc capability
 * 
 * Hydra_Software_Devel/9   10/12/10 3:28p enavarro
 * SWSATFE-75: added BDBG_MODULE()
 * 
 * Hydra_Software_Devel/8   10/11/10 11:35a enavarro
 * SWSATFE-75: fixed compiler warnings
 * 
 * Hydra_Software_Devel/7   9/22/10 5:18p enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/6   9/21/10 10:27a enavarro
 * SWSATFE-75: fixed compiler warnings
 * 
 * Hydra_Software_Devel/5   9/20/10 5:25p ronchan
 * SWSATFE-75: fixed compile errors
 * 
 * Hydra_Software_Devel/4   9/20/10 3:21p enavarro
 * SWSATFE-75: added BAST_g3_P_TimerIsRunning() and
 * BAST_g3_P_TimerIsExpired() functions
 * 
 * Hydra_Software_Devel/3   9/17/10 5:28p ronchan
 * SWSATFE-75: initial compile
 * 
 * Hydra_Software_Devel/2   9/1/10 5:01p enavarro
 * SWSATFE-75: in BAST_g3_P_EnableTimer(), dont block if func==NULL
 * 
 * Hydra_Software_Devel/1   9/1/10 9:26a enavarro
 * SWSATFE-75: initial version
 *
 ***************************************************************************/
#include "bstd.h"
#include "bdbg.h"
#include "bast.h"
#include "bast_priv.h"
#include "bast_g3_priv.h"


BDBG_MODULE(bast_g3_priv_timer);


#if BCHP_CHIP==4538   
extern void * irq_sub_funct;
#endif


/******************************************************************************
 BAST_g3_P_DisableTimer() - ISR context
******************************************************************************/
BERR_Code BAST_g3_P_DisableTimer(BAST_ChannelHandle h, BAST_TimerSelect t)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t reg = BCHP_SDS_MISC_TMRCTL;
   BINT_CallbackHandle hCb = NULL;
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t and_mask = 0xFFFFFFFF;
   
   switch (t)
   {
      case BAST_TimerSelect_eBaud:
      case BAST_TimerSelect_eBaudUsec:
         and_mask = ~0x02;
         hCb = hChn->hBaudTimerCb;
         hChn->baudTimerIsr = NULL;
         break;
      case BAST_TimerSelect_eBer:
         and_mask = ~0x04;
         hCb = hChn->hBerTimerCb;
         hChn->berTimerIsr = NULL;
         break;
      case BAST_TimerSelect_eGen1:
         and_mask = ~0x08;
         hCb = hChn->hGen1TimerCb;
         hChn->gen1TimerIsr = NULL;
         break;
      case BAST_TimerSelect_eGen2:
         and_mask = ~0x10;
         hCb = hChn->hGen2TimerCb;
         hChn->gen2TimerIsr = NULL;
         break;
      case BAST_TimerSelect_eGen3:
         and_mask = ~0x20;
         hCb = hChn->hGen3TimerCb;
         hChn->gen3TimerIsr = NULL;
         break;
      case BAST_TimerSelect_eDiseqc1:
         if (!hChn->bHasDiseqc)
            return BERR_NOT_SUPPORTED;
         and_mask = ~0x01;
         hCb = hChn->diseqc->hDiseqc1TimerCb;         
         reg = BCHP_SDS_DSEC_DSTMRCTL;
         break;
      case BAST_TimerSelect_eDiseqc2:
         if (!hChn->bHasDiseqc)
            return BERR_NOT_SUPPORTED;
         and_mask = ~0x02;
         hCb = hChn->diseqc->hDiseqc2TimerCb;            
         reg = BCHP_SDS_DSEC_DSTMRCTL;
         break;         
      default:
         BDBG_ERR(("invalid timer specified"));
         BDBG_ASSERT(0); /* should never get here */
         return BERR_TRACE(BERR_INVALID_PARAMETER);
   }
   
   BINT_DisableCallback_isr(hCb);
   BINT_ClearCallback_isr(hCb);

   BAST_g3_P_AndRegister(h, reg, and_mask);

   return retCode;
}


/******************************************************************************
 BAST_g3_P_EnableTimer() - ISR context
******************************************************************************/
BERR_Code BAST_g3_P_EnableTimer(BAST_ChannelHandle h, BAST_TimerSelect t, uint32_t delay, BAST_g3_FUNCT func)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BINT_CallbackHandle hCb = NULL;
   uint32_t tmrctl_reg = BCHP_SDS_MISC_TMRCTL;   
   uint32_t tmrctl_set = 0, tmrctl_clr = 0;
   uint32_t count_reg;
   
   BDBG_ASSERT(delay);
   BDBG_ASSERT(h);

#ifdef BAST_LOG_REG_ACCESS
   if ((hChn->acqState != BAST_AcqState_eMonitorLock) && (hChn->acqState != BAST_AcqState_eIdle))
      BKNI_Printf("delay %1d %08X\n", t, delay);   
#endif   

   BAST_g3_P_DisableTimer(h, t);

   switch (t)
   {
      case BAST_TimerSelect_eBaud:
      case BAST_TimerSelect_eBaudUsec:       
         count_reg = BCHP_SDS_MISC_BTMR;                  
         if (func)
         {
            hChn->baudTimerIsr = func;
            hCb = hChn->hBaudTimerCb;
         }
         else
            hChn->baudTimerIsr = NULL;         
         if (t == BAST_TimerSelect_eBaudUsec)
            tmrctl_set |= 0x01;
         else
            tmrctl_clr |= 0x01;
         tmrctl_set |= 0x02; /* timer start */
         break;

      case BAST_TimerSelect_eBer:
         count_reg = BCHP_SDS_MISC_BERTMR;                 
         if (func)
         {
            hChn->berTimerIsr = func;
            hCb = hChn->hBerTimerCb;
         }
         else
            hChn->berTimerIsr = NULL;
         tmrctl_set |= 0x04;
         break;

      case BAST_TimerSelect_eGen1:
         count_reg = BCHP_SDS_MISC_GENTMR1;
         if (func)
         {
            hChn->gen1TimerIsr = func;
            hCb = hChn->hGen1TimerCb; 
         }
         else
            hChn->gen1TimerIsr = NULL;
         tmrctl_set |= 0x08;
         break;

      case BAST_TimerSelect_eGen2:
         count_reg = BCHP_SDS_MISC_GENTMR2;
         if (func)
         {
            hChn->gen2TimerIsr = func;
            hCb = hChn->hGen2TimerCb; 
         }
         else
            hChn->gen2TimerIsr = NULL;
         tmrctl_set |= 0x10;
         break;

      case BAST_TimerSelect_eGen3:
         count_reg = BCHP_SDS_MISC_GENTMR3;        
         if (func)
         {
            hChn->gen3TimerIsr = func;
            hCb = hChn->hGen3TimerCb;   
         }
         else
            hChn->gen3TimerIsr = NULL;         
         tmrctl_set |= 0x20;
         break;

      case BAST_TimerSelect_eDiseqc1:
         if (!hChn->bHasDiseqc)
            return BERR_NOT_SUPPORTED;
         count_reg = BCHP_SDS_DSEC_DSGENTMR1;    
         if (func)
         {
            hChn->diseqc->diseqc1TimerIsr = func;
            hCb = hChn->diseqc->hDiseqc1TimerCb;   
         }
         else
            hChn->diseqc->diseqc1TimerIsr = NULL;  
         tmrctl_reg = BCHP_SDS_DSEC_DSTMRCTL;   
         tmrctl_set |= 0x01;
         break;
         
      case BAST_TimerSelect_eDiseqc2:
         if (!hChn->bHasDiseqc)
            return BERR_NOT_SUPPORTED;
         count_reg = BCHP_SDS_DSEC_DSGENTMR2;  
         if (func)
         {
            hChn->diseqc->diseqc2TimerIsr = func;
            hCb = hChn->diseqc->hDiseqc2TimerCb;   
         }
         else
            hChn->diseqc->diseqc2TimerIsr = NULL;          
         tmrctl_reg = BCHP_SDS_DSEC_DSTMRCTL;  
         tmrctl_set |= 0x02;
         break;
         
      default:
         BDBG_ERR(("invalid timer specified"));
         BDBG_ASSERT(0); /* should never get here */
         return BERR_INVALID_PARAMETER;
   }

   if (func)
   {
      /* clear interrupt and enable */
      BINT_ClearCallback_isr(hCb);
      BINT_EnableCallback_isr(hCb);
   }

   /* load timer value */
   BAST_g3_P_WriteRegister(h, count_reg, &delay); 
   BAST_g3_P_WriteRegister(h, count_reg, &delay); 
   
   /* start the timer */
   BAST_g3_P_ReadModifyWriteRegister(h, tmrctl_reg, ~tmrctl_clr, tmrctl_set);
   
#if 0   
   if (func == NULL)
   {
      do
      {
         BAST_g3_P_ReadRegister(h, count_reg, &val);
      } while (val > 0);
   }
#endif   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_BaudTimer_isr()
******************************************************************************/
void BAST_g3_P_BaudTimer_isr(void *p, int param)
{
   BAST_ChannelHandle h = (BAST_ChannelHandle)p;
   BAST_g3_P_ChannelHandle *hChn = h->pImpl;
   BAST_g3_FUNCT funct;

   BSTD_UNUSED(param);

   /* BDBG_MSG(("in BAST_g3_P_BaudTimer_isr()")); */
   BAST_g3_P_IncrementInterruptCounter(h, BAST_g3_IntID_eBaudTimer);   
   funct = hChn->baudTimerIsr;
#if BCHP_CHIP==4538   
   irq_sub_funct = funct; /* for debug only */
#endif
   BAST_g3_P_DisableTimer(h, BAST_TimerSelect_eBaudUsec);
   if (funct)
      funct(h);
}


/******************************************************************************
 BAST_g3_P_BerTimer_isr()
******************************************************************************/
void BAST_g3_P_BerTimer_isr(void *p, int param)
{
   BAST_ChannelHandle h = (BAST_ChannelHandle)p;
   BAST_g3_P_ChannelHandle *hChn = h->pImpl;
   BAST_g3_FUNCT funct;
   
   BSTD_UNUSED(param);

   /* BDBG_MSG(("in BAST_g3_P_BerTimer_isr()")); */
   BAST_g3_P_IncrementInterruptCounter(h, BAST_g3_IntID_eBerTimer);      
   funct = hChn->berTimerIsr;
   
   BAST_g3_P_DisableTimer(h, BAST_TimerSelect_eBer);
   if (funct)
      funct(h);
}


/******************************************************************************
 BAST_g3_P_Gen1Timer_isr()
******************************************************************************/
void BAST_g3_P_Gen1Timer_isr(void *p, int param)
{
   BAST_ChannelHandle h = (BAST_ChannelHandle)p;
   BAST_g3_P_ChannelHandle *hChn = h->pImpl;
   BAST_g3_FUNCT funct;
   
   BSTD_UNUSED(param);

   /* BDBG_MSG(("in BAST_g3_P_Gen1Timer_isr()")); */
   BAST_g3_P_IncrementInterruptCounter(h, BAST_g3_IntID_eGenTimer1);     
   funct = hChn->gen1TimerIsr;
   BAST_g3_P_DisableTimer(h, BAST_TimerSelect_eGen1);
   if (funct)
      funct(h);
}


/******************************************************************************
 BAST_g3_P_Gen2Timer_isr()
******************************************************************************/
void BAST_g3_P_Gen2Timer_isr(void *p, int param)
{
   BAST_ChannelHandle h = (BAST_ChannelHandle)p;
   BAST_g3_P_ChannelHandle *hChn = h->pImpl;
   BAST_g3_FUNCT funct;
   
   BSTD_UNUSED(param);

   /* BDBG_MSG(("in BAST_g3_P_Gen2Timer_isr()")); */
   BAST_g3_P_IncrementInterruptCounter(h, BAST_g3_IntID_eGenTimer2);      
   funct = hChn->gen2TimerIsr;
   BAST_g3_P_DisableTimer(h, BAST_TimerSelect_eGen2);
   if (funct)
      funct(h);
}


/******************************************************************************
 BAST_g3_P_Gen3Timer_isr()
******************************************************************************/
void BAST_g3_P_Gen3Timer_isr(void *p, int param)
{
   BAST_ChannelHandle h = (BAST_ChannelHandle)p;
   BAST_g3_P_ChannelHandle *hChn = h->pImpl;
   BAST_g3_FUNCT funct;
   
   BSTD_UNUSED(param);

   /* BDBG_MSG(("in BAST_g3_P_Gen3Timer_isr()")); */
   BAST_g3_P_IncrementInterruptCounter(h, BAST_g3_IntID_eGenTimer3);     
   funct = hChn->gen3TimerIsr;
   BAST_g3_P_DisableTimer(h, BAST_TimerSelect_eGen3);
   if (funct)
      funct(h);
}


/******************************************************************************
 BAST_g3_P_Diseqc1Timer_isr()
******************************************************************************/
void BAST_g3_P_Diseqc1Timer_isr(void *p, int param)
{
   BAST_ChannelHandle h = (BAST_ChannelHandle)p;
   BAST_g3_P_ChannelHandle *hChn = h->pImpl;
   BAST_g3_FUNCT funct;
   
   BSTD_UNUSED(param);

   /* BDBG_MSG(("in BAST_g3_P_Diseqc1Timer_isr()")); */   
   BAST_g3_P_IncrementInterruptCounter(h, BAST_g3_IntID_eDiseqcTimer1);     
   funct = hChn->diseqc->diseqc1TimerIsr;
   BAST_g3_P_DisableTimer(h, BAST_TimerSelect_eDiseqc1);
   if (funct)
      funct(h);
}


/******************************************************************************
 BAST_g3_P_Diseqc2Timer_isr()
******************************************************************************/
void BAST_g3_P_Diseqc2Timer_isr(void *p, int param)
{
   BAST_ChannelHandle h = (BAST_ChannelHandle)p;
   BAST_g3_P_ChannelHandle *hChn = h->pImpl;
   BAST_g3_FUNCT funct;
   
   BSTD_UNUSED(param);

   /* BDBG_MSG(("in BAST_g3_P_Diseqc2Timer_isr()")); */     
   BAST_g3_P_IncrementInterruptCounter(h, BAST_g3_IntID_eDiseqcTimer2);     
   funct = hChn->diseqc->diseqc2TimerIsr;
   BAST_g3_P_DisableTimer(h, BAST_TimerSelect_eDiseqc2);
   if (funct)
      funct(h);
}


/******************************************************************************
 BAST_g3_P_TimerIsRunning()
******************************************************************************/
bool BAST_g3_P_IsTimerRunning(BAST_ChannelHandle h, BAST_TimerSelect t)
{
   BAST_g3_P_ChannelHandle *hChn = h->pImpl;
   uint32_t val, mask = 0, reg = BCHP_SDS_MISC_TMRCTL;

   switch (t)
   {
      case BAST_TimerSelect_eBaud:
      case BAST_TimerSelect_eBaudUsec:
         mask = 0x02;
         break;
      case BAST_TimerSelect_eBer:
         mask = 0x04;
         break;
      case BAST_TimerSelect_eGen1:
         mask = 0x08;
         break;
      case BAST_TimerSelect_eGen2:
         mask = 0x10;
         break;
      case BAST_TimerSelect_eGen3:
         mask = 0x20;
         break;
      case BAST_TimerSelect_eDiseqc1:
         if (!hChn->bHasDiseqc)
            return BERR_NOT_SUPPORTED;
         mask = 0x01;       
         reg = BCHP_SDS_DSEC_DSTMRCTL;
         break;
      case BAST_TimerSelect_eDiseqc2:
         if (!hChn->bHasDiseqc)
            return BERR_NOT_SUPPORTED;
         mask = 0x02;          
         reg = BCHP_SDS_DSEC_DSTMRCTL;
         break;         
      default:
         /* BDBG_ERR(("invalid timer specified in BAST_g3_P_TimerIsRunning()")); */
         BDBG_ASSERT(0);
         return BERR_INVALID_PARAMETER;
   }
   
   BAST_g3_P_ReadRegister(h, reg, &val);    
   return (val & mask) ? true : false;
}


/******************************************************************************
 BAST_g3_P_TimerIsExpired()
******************************************************************************/
bool BAST_g3_P_IsTimerExpired(BAST_ChannelHandle h, BAST_TimerSelect t)
{
   BAST_g3_P_ChannelHandle *hChn = h->pImpl;
   uint32_t val, count_reg;
   
   if (BAST_g3_P_IsTimerRunning(h, t))
   {
      switch (t)
      {
         case BAST_TimerSelect_eBaud:
         case BAST_TimerSelect_eBaudUsec:       
            count_reg = BCHP_SDS_MISC_BTMR;                  
            break;

         case BAST_TimerSelect_eBer:
            count_reg = BCHP_SDS_MISC_BERTMR;                 
            break;

         case BAST_TimerSelect_eGen1:
            count_reg = BCHP_SDS_MISC_GENTMR1;
            break;

         case BAST_TimerSelect_eGen2:
            count_reg = BCHP_SDS_MISC_GENTMR2;
            break;

         case BAST_TimerSelect_eGen3:
            count_reg = BCHP_SDS_MISC_GENTMR3;        
            break;

         case BAST_TimerSelect_eDiseqc1:
            if (!hChn->bHasDiseqc)
               return BERR_NOT_SUPPORTED;
            count_reg = BCHP_SDS_DSEC_DSGENTMR1;    
            break;
            
         case BAST_TimerSelect_eDiseqc2:
            if (!hChn->bHasDiseqc)
               return BERR_NOT_SUPPORTED;
            count_reg = BCHP_SDS_DSEC_DSGENTMR2;  
            break;
            
         default:
            BDBG_ASSERT(0); /* should never get here */
            return BERR_INVALID_PARAMETER;
      }   
      BAST_g3_P_ReadRegister(h, count_reg, &val);    
      if (val == 0)
         return true;
   }
   return false;
}

