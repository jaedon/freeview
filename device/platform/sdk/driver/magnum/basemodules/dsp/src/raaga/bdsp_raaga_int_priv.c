/***************************************************************************
 *     Copyright (c) 2006-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdsp_raaga_int_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/19 $
 * $brcm_Date: 1/11/13 1:35p $
 * $brcm_Revision: Hydra_Software_Devel/19 $
 * $brcm_Date: 1/11/13 1:35p $
 *
 * Module Description:  DSP Interrupt Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dsp/src/raaga/bdsp_raaga_int_priv.c $
 * 
 * Hydra_Software_Devel/19   1/11/13 1:35p nihar
 * SW7425-4350: Restoring the L2 interrupts using a new function added in
 * bint- void BINT_ApplyL2State_isr. - Removing warning thrown by
 * coverity.Removing unused label.
 * 
 * Hydra_Software_Devel/18   1/11/13 12:19p nihar
 * SW7425-4350: Restoring the L2 interrupts using a new function added in
 * bint- void BINT_ApplyL2State_isr. - Removing warning thrown by
 * coverity. Initializing error flag.
 * 
 * Hydra_Software_Devel/17   1/9/13 11:34a kavitha
 * SW7425-4350: Restoring the L2 interrupts using a new function added in
 * bint- void BINT_ApplyL2State_isr. Merging to mainline.
 * 
 * Hydra_Software_Devel/17   1/8/13 3:59p kavitha
 * SW7425-4350: Restoring the L2 interrupts using a new function added in
 * bint- void BINT_ApplyL2State_isr. Merging to mainline.
 * 
 * Hydra_Software_Devel/17   1/8/13 1:54p kavitha
 * SW7425-4350: Restoring the L2 interrupts using a new function added in
 * bint- void BINT_ApplyL2State_isr. Merging to mainline.
 * 
 * Hydra_Software_Devel/SW7425-4350/1   12/19/12 11:54a kavitha
 * SW7425-4350: Restoring the L2 interrupts using a new function added in
 * bint- void BINT_ApplyL2State_isr(BINT_Handle IntHandle, uint32_t
 * L2Reg)
 * 
 * Hydra_Software_Devel/16   11/7/12 6:10p ananthan
 * SW7425-3808: Added BDSP changes to add new Event ID to notify change in
 * dialnorm.
 * 
 * Hydra_Software_Devel/16   11/7/12 6:01p sunilsk
 * SW7425-3808: Added BDSP changes to add new Event ID to notify change in
 * dialnorm.
 * 
 * Hydra_Software_Devel/16   11/7/12 5:59p sunilsk
 * SW7425-3808: Added BDSP changes to add new Event ID to notify change in
 * dialnorm.
 * 
 * Hydra_Software_Devel/15   10/17/12 3:58p kavitha
 * SW7425-4031: Pulling in reset changes to mainline.
 * 
 * Hydra_Software_Devel/BUS_ERROR_FIX/1   10/3/12 6:04p kavitha
 * SW7425-4031: Updates to the reset logic. Updated reset logic masks the
 * watchdog interrupt. So, post reset we need to unmask the required
 * interrupts.
 * 
 * Hydra_Software_Devel/14   10/6/12 2:50a ananthan
 * SW7346-914: Code to unmask Ancillary Data Interrupt.
 * 
 * Hydra_Software_Devel/13   9/21/12 12:20p ananthan
 * SW7346-914: Adding support for MPEG Ancillary data parsing.
 * 
 * Hydra_Software_Devel/12   9/18/12 6:35p ananthan
 * SW7346-914: Adding support for MPEG Ancillary data parsing.
 * 
 * Hydra_Software_Devel/11   4/11/12 7:52p gprasad
 * SW7425-2863: Add an interrupt to notify when the o/p buffer gets full
 * during the encoding process
 * 
 * Hydra_Software_Devel/10   3/14/12 8:31p ashoky
 * SW7231-585: Adding changes to unmask interrupts before task start and
 * masking them while stopping task. Masking all the external interrupts
 * in watchdog case.
 * 
 * Hydra_Software_Devel/9   1/3/12 2:20p ashoky
 * SW7435-5: Adding support for 2nd DSP into mainline.
 * 
 * Hydra_Software_Devel/8   9/12/11 9:13p ananthan
 * SW7552-113: [7552] Add audio ASTM support for 40nm chips [Correcting
 * TSM pass callback function]
 * 
 * Hydra_Software_Devel/7   7/8/11 4:38p gprasad
 * SW7552-64: Changes to provide stream sample rate along wityh base rate
 * to APE
 * 
 * Hydra_Software_Devel/6   6/30/11 9:42a jgarrett
 * SW7425-804: Using stream sample rate for passthrough instead of base
 * sample rate
 * 
 * Hydra_Software_Devel/5   6/16/11 6:03p gautamk
 * SW7422-369: [7422] Adding support for mulitple context for watchdog
 * 
 * Hydra_Software_Devel/4   6/15/11 5:53p gautamk
 * SW7425-683: [7425] Adding support for Power management in BDSP
 * 
 * Hydra_Software_Devel/3   5/11/11 11:24a srajapur
 * SW7422-357: [7422] Need to add new FW mixer support for APE/DSP
 * 
 * Hydra_Software_Devel/2   4/14/11 6:52p srajapur
 * SW7425-291: [7425] BDSP directory structure changes
 * 
 * Hydra_Software_Devel/1   4/6/11 2:28p srajapur
 * SW7425-291: [7425]BDSP directory structure changes
 * 
 * Hydra_Software_Devel/15   3/15/11 11:17p srajapur
 * SW7422-294 : [7422]Adding watchdog recovery support in BASE DSP
 * 
 * Hydra_Software_Devel/14   3/8/11 9:13a srajapur
 * SW7422-294 : [7422]Adding watchdog recovery support in BASE DSP
 * 
 * Hydra_Software_Devel/13   2/26/11 2:00a srajapur
 * SW7422-294 :[7422]  Adding watchdog recovery API support
 * 
 * Hydra_Software_Devel/12   2/3/11 3:08a srajapur
 * SW7422-229 : Adding support for MPEG algorithm. --> Passing the
 * correct modevalue to application.
 * 
 * Hydra_Software_Devel/11   1/26/11 12:53a gautamk
 * SW7422-191: Adding error recovery code for create task.
 * 
 * Hydra_Software_Devel/10   1/25/11 5:33a gautamk
 * SW7422-191: Changes in TSM config and status structure
 * 
 * Hydra_Software_Devel/9   1/25/11 2:22a gautamk
 * SW7422-191:Adding error recovery code for Raaga Open
 * 
 * Hydra_Software_Devel/8   1/21/11 7:05a gautamk
 * SW7422-191: changes in setInterrupthandler function.
 * 
 * Hydra_Software_Devel/7   1/20/11 8:11a gautamk
 * SW7422-191: Implementing code review comments
 * 
 * Hydra_Software_Devel/6   1/19/11 7:15a gautamk
 * SW7422-191: Various DSP Basmodules features and review comment
 * implementation
 * 
 * Hydra_Software_Devel/5   1/19/11 2:15a gautamk
 * SW7422-191: putting all the Details of Audio codec in a const array and
 * then using it across the code.
 * 
 * Hydra_Software_Devel/4   1/18/11 4:54a gautamk
 * SW7422-191: Implementing code review comments.
 * 
 * Hydra_Software_Devel/3   1/18/11 4:20a gautamk
 * SW7125-766: Added code for review comments.
 * 
 * Hydra_Software_Devel/2   1/15/11 3:32a gautamk
 * SW7422-191:[7422] CLeaning up unwanted debug message
 * 
 * Hydra_Software_Devel/1   1/13/11 5:33a gautamk
 * SW7422-191:[7422]Adding DSP support for Audio decode in Basemodule  It
 * includes - DSP Booting - Open time Download :  - Start Time Audio
 * Decode algorithm download  - Message buffer interaction with DSP:  -
 * Synchronized Interrupt:  - Async interrupts:  - Config Commands for
 * Decode/TSM/IDS: - Status Buffer for Decode/TSM/IDS:  - Start AC3 Audio
 * Decode:  - Pause/Resume Command:  - Frame Advance command:  - Device
 * level interrupt
 * 
 * 
 ***************************************************************************/

 #include "bdsp_raaga_int_priv.h" 
#include "bdsp_raaga_priv.h"
#include "bdsp_raaga_types.h"

 
 BDBG_MODULE(bdsp_raaga_int);   /* Register software module with debug interface */
 
 /*------------------------- GLOBALS AND EXTERNS ------------------------------*/
 static const  BINT_Id ui32DSPSynInterruptId[] =
 {
#if defined BCHP_INT_ID_SYNC_MSG
	 /* old Style RDB */
         BCHP_INT_ID_SYNC_MSG
#if defined BCHP_RAAGA_DSP_INTH_1_REG_START
	, BCHP_INT_ID_RAAGA_DSP_FW_INTH_1_SYNC_MSG
#endif 
#else
	 BCHP_INT_ID_RAAGA_DSP_FW_INTH_SYNC_MSG
#if defined BCHP_RAAGA_DSP_INTH_1_REG_START
	, BCHP_INT_ID_RAAGA_DSP_FW_INTH_1_SYNC_MSG
#endif 
#endif 
 };
 
 static const  BINT_Id ui32DSPAsynInterruptId[] =
 {
#if defined BCHP_INT_ID_ASYNC_MSG
         BCHP_INT_ID_ASYNC_MSG
#if defined BCHP_RAAGA_DSP_INTH_1_REG_START
	, BCHP_INT_ID_RAAGA_DSP_FW_INTH_1_ASYNC_MSG
#endif 
#else  		
	 BCHP_INT_ID_RAAGA_DSP_FW_INTH_ASYNC_MSG
#if defined BCHP_RAAGA_DSP_INTH_1_REG_START
	, BCHP_INT_ID_RAAGA_DSP_FW_INTH_1_ASYNC_MSG
#endif
#endif
 };
 
 static const  BINT_Id ui32DSPAckInterruptId[] =
 {
#if defined BCHP_INT_ID_HOST_MSG
         BCHP_INT_ID_HOST_MSG
#if defined BCHP_RAAGA_DSP_INTH_1_REG_START
	, BCHP_INT_ID_RAAGA_DSP_FW_INTH_1_HOST_MSG
#endif 
#else
	 BCHP_INT_ID_RAAGA_DSP_FW_INTH_HOST_MSG
#if defined BCHP_RAAGA_DSP_INTH_1_REG_START
	, BCHP_INT_ID_RAAGA_DSP_FW_INTH_1_HOST_MSG
#endif 
#endif
 };

static const  BINT_Id ui32DSPWatchDogInterruptId[] =
{
#if defined BCHP_INT_ID_WATCHDOG_TIMER_ATTN
	BCHP_INT_ID_WATCHDOG_TIMER_ATTN
#if defined BCHP_RAAGA_DSP_INTH_1_REG_START
	, BCHP_INT_ID_RAAGA_DSP_INTH_1_WATCHDOG_TIMER_ATTN
#endif 
#else
	BCHP_INT_ID_RAAGA_DSP_INTH_WATCHDOG_TIMER_ATTN
#if defined BCHP_RAAGA_DSP_INTH_1_REG_START
	, BCHP_INT_ID_RAAGA_DSP_INTH_1_WATCHDOG_TIMER_ATTN
#endif 
#endif
 };

#define BDSP_RAAGA_DEFAULT_WATCHDOG_COUNT	(0x3FFFFFFF)

 
 /***************************************************************************
 Description:
     This API will be called when any synchronous interrupt will be raised by 
     FW for any task.
 Returns:
     void
 See Also:    
     BDSP_P_DSP2HostSyn_isr, BDSP_P_DSP2HostAsyn_isr, BDSP_P_InterruptInstall
 ***************************************************************************/
 static void BDSP_Raaga_P_DSP2HostSyn_isr(
         void    *pDeviceHandle,
         int     iParm2
 )
 {
     BDSP_Raaga *pDevice = (BDSP_Raaga *)pDeviceHandle;
     uint32_t    ui32SyncTaskIds = 0;    
     uint32_t    ui32DspId = 0, ui32TaskId = 0;
     int  i=0;     
     BDSP_RaagaTask *pRaagaTask;
     uint32_t uiTaskIndex=0;
     
     BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);
     
     BDBG_ENTER(BDSP_Raaga_P_DSP2HostSyn_isr);
 
     BDBG_MSG (("... Sync Message Posted by DSP ... "));
     
     ui32DspId = iParm2;
 
     for(i  = 0; i < BDSP_RAAGA_MAX_FW_TASK_PER_DSP; i++)
     {       
     
        pRaagaTask = pDevice->taskDetails.pRaagaTask[ui32DspId][i];
        
        if(pRaagaTask == NULL)
        {
            continue;
        }
        
        if (pRaagaTask->taskId == BDSP_RAAGA_P_INVALID_TASK_ID)
        {
            continue;
        }     
         ui32SyncTaskIds = BDSP_Read32_isr(
                             pDevice->regHandle, 
                             BCHP_RAAGA_DSP_PERI_SW_MSG_BITS_STATUS_0  + pDevice->dspOffset[ui32DspId]);  
         
         if (ui32SyncTaskIds & (1 << pRaagaTask->taskId))
         {
            ui32TaskId = pRaagaTask->taskId;

            BDSP_Write32_isr(
                         pDevice->regHandle, 
                         BCHP_RAAGA_DSP_PERI_SW_MSG_BITS_CLEAR_0 + pDevice->dspOffset[ui32DspId], ui32SyncTaskIds);        
             uiTaskIndex = ui32TaskId - BDSP_RAAGA_TASK_ID_START_OFFSET;

             if (uiTaskIndex < BDSP_RAAGA_MAX_FW_TASK_PER_DSP)
             {
                 BKNI_SetEvent(pDevice->taskDetails.pRaagaTask[ui32DspId][uiTaskIndex]->hEvent);
             }
             
             switch(pRaagaTask->lastEventType)
             {
                 case BDSP_RAAGA_START_TASK_COMMAND_ID:
                     pRaagaTask->isStopped=false;
                     break;
                     
                 case BDSP_RAAGA_STOP_TASK_COMMAND_ID:
                     pRaagaTask->isStopped=true;
                     break;
                     
                 default:
                     break;
             }            
         }
     }
 
     BDBG_LEAVE(BDSP_Raaga_P_DSP2HostSyn_isr);
     return;
 }

 static void BDSP_Raaga_P_CdbItbOverflow_isr(
     void *pTaskHandle , void *pParam2)
 {
     
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;   
    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
    BSTD_UNUSED(pParam2);

     BDBG_MSG(("BDSP_Raaga_P_CdbItbOverflow_isr: cdbItb Overflow interrupt occured for Task %d",
                 pRaagaTask->taskId));
     /* Call the application Stream Info Available  callback function */
     if(pRaagaTask->audioInterruptHandlers.cdbItbOverflow.pCallback_isr) {
         pRaagaTask->audioInterruptHandlers.cdbItbOverflow.pCallback_isr (
             pRaagaTask->audioInterruptHandlers.cdbItbOverflow.pParam1,
             pRaagaTask->audioInterruptHandlers.cdbItbOverflow.param2);
     }  
 }
 
 
 static void BDSP_Raaga_P_CdbItbUnderflow_isr(
     void *pTaskHandle , void *pParam2)
 {
     
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;   
    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
    BSTD_UNUSED(pParam2);

     BDBG_MSG(("BDSP_Raaga_P_CdbItbUnderflow_isr: cdbItb Underflow interrupt occured for Task %d",
                 pRaagaTask->taskId));
     /* Call the application Stream Info Available  callback function */
     if(pRaagaTask->audioInterruptHandlers.cdbItbUnderflow.pCallback_isr) {
         pRaagaTask->audioInterruptHandlers.cdbItbUnderflow.pCallback_isr (
             pRaagaTask->audioInterruptHandlers.cdbItbUnderflow.pParam1,
             pRaagaTask->audioInterruptHandlers.cdbItbUnderflow.param2);
     }  
 }

 static void BDSP_Raaga_P_EncoderOutputOverflow_isr(
     void *pTaskHandle , void *pParam2)
 {
     
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;   
    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
    BSTD_UNUSED(pParam2);

     BDBG_MSG(("BDSP_Raaga_P_EncoderOutputOverflow_isr: Encoder output Overflow interrupt occured for Task %d",
                 pRaagaTask->taskId));
     /* Call the application Stream Info Available  callback function */
     if(pRaagaTask->audioInterruptHandlers.encoderOutputOverflow.pCallback_isr) {
         pRaagaTask->audioInterruptHandlers.encoderOutputOverflow.pCallback_isr (
             pRaagaTask->audioInterruptHandlers.encoderOutputOverflow.pParam1,
             pRaagaTask->audioInterruptHandlers.encoderOutputOverflow.param2);
     }  
 }
 
 static void BDSP_Raaga_P_AncillaryDataPresent_isr(
     void *pTaskHandle , void *pParam2)
 {
     
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;   
    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
    BSTD_UNUSED(pParam2);

    BDBG_MSG(("BDSP_Raaga_P_AncillaryDataPresent_isr: MPEG Ancillary Data interrupt occured for Task %d",
                 pRaagaTask->taskId));
     
     /* Call the application Stream Info Available  callback function */
     if(pRaagaTask->audioInterruptHandlers.ancillaryData.pCallback_isr) {
         pRaagaTask->audioInterruptHandlers.ancillaryData.pCallback_isr (
             pRaagaTask->audioInterruptHandlers.ancillaryData.pParam1,
             pRaagaTask->audioInterruptHandlers.ancillaryData.param2);		 
	}
				 
 }
 
  static void BDSP_Raaga_P_DialnormChange_isr(
     void *pTaskHandle , void *pParam2)
 {
     
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;   
    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
    BSTD_UNUSED(pParam2);

    BDBG_MSG(("BDSP_Raaga_P_DialnormChange_isr: Change in Dialnorm  Event occured for Task %d",
                 pRaagaTask->taskId));
     
     /* Call the application Stream Info Available  callback function */
     if(pRaagaTask->audioInterruptHandlers.dialnormChange.pCallback_isr) {
         pRaagaTask->audioInterruptHandlers.dialnormChange.pCallback_isr (
             pRaagaTask->audioInterruptHandlers.dialnormChange.pParam1,
             pRaagaTask->audioInterruptHandlers.dialnormChange.param2);		 
	}
				 
 } 
 
 static void  BDSP_Raaga_P_DecoderUnlock_isr(
     void *pTaskHandle , void *pParam2)
 {
     
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;   
    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
    BSTD_UNUSED(pParam2);

     BDBG_MSG(("BDSP_Raaga_P_DecoderUnlock_isr: Decoder unlock interrupt occured for Task %d",
                 pRaagaTask->taskId));

    pRaagaTask->decLocked = false;
     /* Call the application Stream Info Available  callback function */
     if(pRaagaTask->audioInterruptHandlers.unlock.pCallback_isr) {
         pRaagaTask->audioInterruptHandlers.unlock.pCallback_isr (
             pRaagaTask->audioInterruptHandlers.unlock.pParam1,
             pRaagaTask->audioInterruptHandlers.unlock.param2);
     }  
 }

 static void  BDSP_Raaga_P_DecoderLock_isr(
     void *pTaskHandle , void *pParam2)
 {
     
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;   
    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
    BSTD_UNUSED(pParam2);

     BDBG_MSG(("BDSP_Raaga_P_DecoderLock_isr: Decoder lock interrupt occured for Task %d",
                 pRaagaTask->taskId));

     pRaagaTask->decLocked = true;

     /* Call the application Stream Info Available  callback function */
     if(pRaagaTask->audioInterruptHandlers.lock.pCallback_isr) {
         pRaagaTask->audioInterruptHandlers.lock.pCallback_isr (
             pRaagaTask->audioInterruptHandlers.lock.pParam1,
             pRaagaTask->audioInterruptHandlers.lock.param2);
     }  
 }

 

static void  BDSP_Raaga_P_CrcError_isr(
     void *pTaskHandle , void *pParam2)
 {
     
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;   
    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
    BSTD_UNUSED(pParam2);

     BDBG_MSG(("BDSP_Raaga_P_CrcError_isr: CRC error interrupt occured for Task %d",
                 pRaagaTask->taskId));
 }    

  static void  BDSP_Raaga_P_SampleRateChange_isr(
     void *pTaskHandle , BDSP_DSPCHN_P_FwSampleinfo *psFwSampleInfo)
 {
     
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;     
    
    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
    BSTD_UNUSED(psFwSampleInfo);

     BDBG_MSG(("BDSP_Raaga_P_SampleRateChange_isr: Sample rate change interrupt occured for Task %d",
                 pRaagaTask->taskId));


     /* Call the application Stream Info Available  callback function */
     if(pRaagaTask->audioInterruptHandlers.sampleRateChange.pCallback_isr) {

         pRaagaTask->audioInterruptHandlers.sampleRateChange.pCallback_isr (
             pRaagaTask->audioInterruptHandlers.sampleRateChange.pParam1,
             pRaagaTask->audioInterruptHandlers.sampleRateChange.param2,
             psFwSampleInfo->sStreamSamplingRate.ui32SamplingRate,
             psFwSampleInfo->sBaseSamplingRate.ui32SamplingRate);
     }  
 }


  static void  BDSP_Raaga_P_BitRateChange_isr(
     void *pTaskHandle , BDSP_DSPCHN_P_FwBitRateChangeInfo *pFwBitrateChangeInfo)
 {
     
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;         
    BDSP_AudioBitRateChangeInfo  bitrateChangeInfo;
    
    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
    BSTD_UNUSED(pFwBitrateChangeInfo);

     BDBG_MSG(("BDSP_Raaga_P_BitRateChange_isr: Bit rate change interrupt occured for Task %d",
                 pRaagaTask->taskId));

     /*Prepare the structure for application */
     bitrateChangeInfo.eType = pRaagaTask->settings.pBranchInfo[0]->sFwStgInfo[0].uAlgorithm.eDecAudioAlgo;
     bitrateChangeInfo.ui32BitRate = pFwBitrateChangeInfo->ui32BitRate;
     bitrateChangeInfo.ui32BitRateIndex = pFwBitrateChangeInfo->ui32BitRateIndex;


     /* Call the application Stream Info Available  callback function */
     if(pRaagaTask->audioInterruptHandlers.bitrateChange.pCallback_isr) {
         pRaagaTask->audioInterruptHandlers.bitrateChange.pCallback_isr (
             pRaagaTask->audioInterruptHandlers.bitrateChange.pParam1,
             pRaagaTask->audioInterruptHandlers.bitrateChange.param2,
             &bitrateChangeInfo);
     } 

 }

  static void  BDSP_Raaga_P_ModeChange_isr(
      void *pTaskHandle , BDSP_DSPCHN_P_FwModeChangeInfo *pFwModeChangeInfo)
  {
      
     BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;
     uint32_t ui32ModeValue;
     
     BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
 
     BDBG_MSG(("BDSP_Raaga_P_ModeChange_isr: mode change interrupt occured for Task %d",
               pRaagaTask->taskId));

 
     /*Prepare the structure for application */
     ui32ModeValue = pFwModeChangeInfo->ui32ModeValue;
     
    /* Call the application Stream Info Available  callback function */
    if(pRaagaTask->audioInterruptHandlers.modeChange.pCallback_isr) {
      pRaagaTask->audioInterruptHandlers.modeChange.pCallback_isr (
          pRaagaTask->audioInterruptHandlers.modeChange.pParam1,
          pRaagaTask->audioInterruptHandlers.modeChange.param2,
          ui32ModeValue);
    } 

  }


 static void  BDSP_Raaga_P_FirstPtsReady_isr(
     void *pTaskHandle , BDSP_Raaga_P_PtsInfo *pPtsInfo)
 {
     
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;   
    BDSP_AudioTaskTsmStatus	ptsInfo;

 
    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
    ptsInfo.ui32RunningPts = pPtsInfo->ui32RunningPts;
    ptsInfo.i32PtsToStcPhase = pPtsInfo->i32Pts2StcPhase;
    ptsInfo.ePtsType = pPtsInfo->ePtsType;
    ptsInfo.i32TSMUpperThreshold = pPtsInfo->i32TSMUpperThreshold;    
    
    BDBG_MSG(("BDSP_Raaga_P_FirstPtsReady_isr: First PTS ready interrupt occured for Task %d",
             pRaagaTask->taskId));

    /* Call the application Stream Info Available  callback function */
    if(pRaagaTask->audioInterruptHandlers.firstPts.pCallback_isr) {
     pRaagaTask->audioInterruptHandlers.firstPts.pCallback_isr (
         pRaagaTask->audioInterruptHandlers.firstPts.pParam1,
         pRaagaTask->audioInterruptHandlers.firstPts.param2,
         &ptsInfo);
    }  
 }
                 
static void  BDSP_Raaga_P_TsmFail_isr(
     void *pTaskHandle , BDSP_Raaga_P_PtsInfo*pPtsInfo)
{

    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;   
    BDSP_AudioTaskTsmStatus  ptsInfo;

    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
    
    ptsInfo.ui32RunningPts = pPtsInfo->ui32RunningPts;
    ptsInfo.i32PtsToStcPhase = pPtsInfo->i32Pts2StcPhase;
    ptsInfo.ePtsType = pPtsInfo->ePtsType;
    ptsInfo.i32TSMUpperThreshold = pPtsInfo->i32TSMUpperThreshold;    

    BDBG_MSG(("BDSP_Raaga_P_TsmFail_isr: TSm Fail interrupt occured for Task %d",
             pRaagaTask->taskId));

    /* Call the application Stream Info Available  callback function */
    if(pRaagaTask->audioInterruptHandlers.tsmFail.pCallback_isr) {
     pRaagaTask->audioInterruptHandlers.tsmFail.pCallback_isr (
         pRaagaTask->audioInterruptHandlers.tsmFail.pParam1,
         pRaagaTask->audioInterruptHandlers.tsmFail.param2,
         &ptsInfo);
    }    
}

 static void  BDSP_Raaga_P_StartPtsReached_isr(
  void *pTaskHandle)
 {
 
     BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;   
 
     BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
 
     BDBG_MSG(("BDSP_Raaga_P_StartPtsReached_isr: Start PTS reached interrupt occured for Task %d",
              pRaagaTask->taskId));
 
     /* Call the application Stream Info Available  callback function */
     if(pRaagaTask->audioInterruptHandlers.startPtsReached.pCallback_isr) {
      pRaagaTask->audioInterruptHandlers.startPtsReached.pCallback_isr (
          pRaagaTask->audioInterruptHandlers.startPtsReached.pParam1,
          pRaagaTask->audioInterruptHandlers.startPtsReached.param2);
     }     
 }

 static void  BDSP_Raaga_P_StopPtsReached_isr(
  void *pTaskHandle)
 {
 
     BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;   
 
     BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
 
     BDBG_MSG(("BDSP_Raaga_P_StopPtsReached_isr: Start PTS reached interrupt occured for Task %d",
              pRaagaTask->taskId));
 
     /* Call the application Stream Info Available  callback function */
     if(pRaagaTask->audioInterruptHandlers.startPtsReached.pCallback_isr) {
      pRaagaTask->audioInterruptHandlers.startPtsReached.pCallback_isr (
          pRaagaTask->audioInterruptHandlers.startPtsReached.pParam1,
          pRaagaTask->audioInterruptHandlers.startPtsReached.param2);
     }     
 }

 static void  BDSP_Raaga_P_AstmPass_isr(
     void *pTaskHandle , BDSP_Raaga_P_PtsInfo *pPtsInfo)
{

    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;   
    BDSP_AudioTaskTsmStatus  ptsInfo;

    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
    
    ptsInfo.ui32RunningPts = pPtsInfo->ui32RunningPts;
    ptsInfo.i32PtsToStcPhase = pPtsInfo->i32Pts2StcPhase;
    ptsInfo.ePtsType = pPtsInfo->ePtsType;
    ptsInfo.i32TSMUpperThreshold = pPtsInfo->i32TSMUpperThreshold;   

    BDBG_MSG(("BDSP_Raaga_P_AstmPass_isr: ASTM Pass interrupt occured for Task %d",
             pRaagaTask->taskId));

    /* Call the application TSM Pass callback function */
    if(pRaagaTask->audioInterruptHandlers.tsmPass.pCallback_isr) {
     pRaagaTask->audioInterruptHandlers.tsmPass.pCallback_isr (
         pRaagaTask->audioInterruptHandlers.tsmPass.pParam1,
         pRaagaTask->audioInterruptHandlers.tsmPass.param2,
         &ptsInfo);
    }    
}
 
  static void  BDSP_Raaga_P_RampEnable_isr(
     void *pTaskHandle , BDSP_DSPCHN_RampEnableInfo *pRampEnableInfo)
{

    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;   

    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
    BSTD_UNUSED(pRampEnableInfo);

    BDBG_MSG(("BDSP_Raaga_P_RampEnable_isr: Ramp Enable interrupt occured for Task %d",
             pRaagaTask->taskId));

    /* Call the application Stream Info Available  callback function */
    if(pRaagaTask->audioInterruptHandlers.rampEnable.pCallback_isr) {
        pRaagaTask->audioInterruptHandlers.rampEnable.pCallback_isr (
        pRaagaTask->audioInterruptHandlers.rampEnable.pParam1,
        pRaagaTask->audioInterruptHandlers.rampEnable.param2);
    }    
    

}

  static void  BDSP_Raaga_P_StreamInfoAvailable_isr(
      void *pTaskHandle )
 {
 
     BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;   
 
     BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
 
     BDBG_MSG(("BDSP_Raaga_P_StreamInfoAvailable_isr: Stream Information available interrupt occured for Task %d",
              pRaagaTask->taskId));
 
     /* Call the application Stream Info Available  callback function */
     if(pRaagaTask->audioInterruptHandlers.statusReady.pCallback_isr) {
      pRaagaTask->audioInterruptHandlers.statusReady.pCallback_isr (
          pRaagaTask->audioInterruptHandlers.statusReady.pParam1,
          pRaagaTask->audioInterruptHandlers.statusReady.param2);
     }    
 }

 
static void  BDSP_Raaga_P_UnlicensedAlgo_isr(
void *pTaskHandle, BDSP_Raaga_P_UnsupportedAlgoInfo 	*pUnlicensedAlgoInfo )
{
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;   

    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);

    BDBG_ERR(("BDSP_P_DSP2HostAsyn_isr: Unlicensed Algo (%s) interrupt occured for Task %d"
            ,BDSP_P_PrintDecEncPpTypeName(pUnlicensedAlgoInfo->ui32AudioAlgorithm),
            pRaagaTask->taskId));

    /* Call the application Stream Info Available  callback function */
    if(pRaagaTask->audioInterruptHandlers.unlicensedAlgo.pCallback_isr) {
        pRaagaTask->audioInterruptHandlers.unlicensedAlgo.pCallback_isr (
        pRaagaTask->audioInterruptHandlers.unlicensedAlgo.pParam1,
        pRaagaTask->audioInterruptHandlers.unlicensedAlgo.param2);
    }    
}


 /***************************************************************************
 Description:
     This API will be called when any asynchronous interrupt will be raised by 
     FW for any task.
 Returns:
     void
 See Also:    
     BDSP_P_DSP2HostSyn_isr, BDSP_P_DSP2HostAsyn_isr, BDSP_P_InterruptInstall
 ***************************************************************************/
 static void BDSP_Raaga_P_DSP2HostAsyn_isr(
            void    *pDeviceHandle,
            int     iParm2
 )
 {
    uint32_t    ui32ASyncTaskIds = 0;    
    uint32_t    ui32DspId = 0;
    int i=0;
    BDSP_Raaga *pDevice = (BDSP_Raaga *)pDeviceHandle;
    BDSP_RaagaTask *pRaagaTask;    
    BDSP_Raaga_P_AsynEventMsg  *pEventMsg = NULL;
    
    BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);
    BDBG_ENTER(BDSP_Raaga_P_DSP2HostAsyn_isr);

    BDBG_MSG (("... Async Message(s) Posted by DSP ... "));    
    
    ui32DspId = iParm2;


    for(i = 0; i < BDSP_RAAGA_MAX_FW_TASK_PER_DSP; i++)
    {
        pRaagaTask = pDevice->taskDetails.pRaagaTask[ui32DspId][i];
        if(pRaagaTask == NULL)
        {
            continue;
        }
        
        if((pRaagaTask->taskId == BDSP_RAAGA_P_INVALID_TASK_ID))
        {
            continue;
        }

        if(BDSP_Raaga_P_CheckIfPtrInvalid((void *)pRaagaTask->taskMemGrants.sTaskQueue.sAsyncMsgBufmem.ui32BaseAddr) == true)
        {
            continue;
        }        
        
        ui32ASyncTaskIds = BDSP_Read32_isr(
                                pDevice->regHandle, 
                                BCHP_RAAGA_DSP_PERI_SW_MSG_BITS_STATUS_1  + pDevice->dspOffset[ui32DspId]); 


        if (ui32ASyncTaskIds & (1 << pRaagaTask->taskId))
        {
            unsigned int uiNumMsgs = 0, i = 0;

            BDSP_Write32_isr(
                pDevice->regHandle, 
                BCHP_RAAGA_DSP_PERI_SW_MSG_BITS_CLEAR_1  + pDevice->dspOffset[ui32DspId], (1 << pRaagaTask->taskId));

            pEventMsg = (void *)pRaagaTask->taskMemGrants.sTaskQueue.sAsyncMsgBufmem.ui32BaseAddr;

            /* Read the message in sEventMsg */
            BDSP_Raaga_P_GetAsyncMsg_isr(pRaagaTask->hAsyncMsgQueue,pEventMsg, &uiNumMsgs);

            /* Now check which response came from FW and work according to it*/
            for(i = 0; i < uiNumMsgs; i++)
            {
                if(pEventMsg[i].sMsgHeader.ui32EventID == BDSP_RAAGA_FRAME_SYNC_LOCK_LOST_EVENT_ID)
                {
                    BDSP_Raaga_P_DecoderUnlock_isr((void *)pRaagaTask,NULL);
                }
                else if(pEventMsg[i].sMsgHeader.ui32EventID == BDSP_RAAGA_FRAME_SYNC_LOCK_EVENT_ID)
                {
                    BDSP_Raaga_P_DecoderLock_isr((void *)pRaagaTask,NULL);
                }
                else if(pEventMsg[i].sMsgHeader.ui32EventID == BDSP_RAAGA_SAMPLING_RATE_CHANGE_EVENT_ID)
                {
                    BDSP_Raaga_P_SampleRateChange_isr((void *)pRaagaTask,&(pEventMsg[i].uFWMessageInfo.sFwSampleInfo));
                }
                else if (pEventMsg[i].sMsgHeader.ui32EventID == BDSP_RAAGA_BIT_RATE_CHANGE_EVENT_ID)
                {
                    BDSP_Raaga_P_BitRateChange_isr((void *)pRaagaTask,&(pEventMsg[i].uFWMessageInfo.sFwBitRateChange));
                }
                else if (pEventMsg[i].sMsgHeader.ui32EventID == BDSP_RAAGA_AUDIO_MODE_CHANGE_EVENT_ID)
                {
                    BDSP_Raaga_P_ModeChange_isr((void *)pRaagaTask,&(pEventMsg[i].uFWMessageInfo.sFwModeChange));            	
                }
                else if (pEventMsg[i].sMsgHeader.ui32EventID == BDSP_RAAGA_CRC_ERROR_EVENT_ID)
                {
                    BDSP_Raaga_P_CrcError_isr((void *)pRaagaTask,NULL);                        	
                }
                else if (pEventMsg[i].sMsgHeader.ui32EventID == BDSP_RAAGA_FIRST_PTS_RECEIVED_FROM_ITB_EVENT_ID)
                {
                    BDSP_Raaga_P_FirstPtsReady_isr((void *)pRaagaTask,&(pEventMsg[i].uFWMessageInfo.sPtsInfo));             	
                }
                else if (pEventMsg[i].sMsgHeader.ui32EventID  == BDSP_RAAGA_START_PTS_EVENT_ID)
                {
                    BDSP_Raaga_P_StartPtsReached_isr((void *)pRaagaTask);
                }   	
                else if (pEventMsg[i].sMsgHeader.ui32EventID  == BDSP_RAAGA_STOP_PTS_EVENT_ID)
                {
                    BDSP_Raaga_P_StopPtsReached_isr((void *)pRaagaTask);
                }  
                else if (pEventMsg[i].sMsgHeader.ui32EventID  == BDSP_RAAGA_ASTMTSM_PASS_EVENT_ID)
                {
                    BDSP_Raaga_P_AstmPass_isr((void *)pRaagaTask,&(pEventMsg[i].uFWMessageInfo.sPtsInfo));
                } 
                else if (pEventMsg[i].sMsgHeader.ui32EventID  == BDSP_RAAGA_TSM_FAIL_EVENT_ID)
                {
                    BDSP_Raaga_P_TsmFail_isr((void *)pRaagaTask,&(pEventMsg[i].uFWMessageInfo.sPtsInfo));  ;
                }       
                else if (pEventMsg[i].sMsgHeader.ui32EventID  == BDSP_RAAGA_RAMP_ENABLE_EVENT_ID)
                {
                    BDSP_Raaga_P_RampEnable_isr((void *)pRaagaTask,&(pEventMsg[i].uFWMessageInfo.sRampEnableInfo));  ;
                }          
                else if (pEventMsg[i].sMsgHeader.ui32EventID  == BDSP_RAAGA_STREAM_INFO_AVAIL_EVENT_ID)
                {
                    BDSP_Raaga_P_StreamInfoAvailable_isr((void *)pRaagaTask);
                }
                else if (pEventMsg[i].sMsgHeader.ui32EventID  == BDSP_RAAGA_CDB_ITB_OVERFLOW_EVENT_ID)
                {
                    BDSP_Raaga_P_CdbItbOverflow_isr((void *)pRaagaTask,NULL);
                }
                else if (pEventMsg[i].sMsgHeader.ui32EventID  == BDSP_RAAGA_CDB_ITB_UNDERFLOW_EVENT_ID)
                {
                    BDSP_Raaga_P_CdbItbUnderflow_isr((void *)pRaagaTask,NULL);
                }               
                else if (pEventMsg[i].sMsgHeader.ui32EventID  == BDSP_RAAGA_UNLICENSED_ALGO_EVENT_ID)
                {
                    BDSP_Raaga_P_UnlicensedAlgo_isr((void *)pRaagaTask,&(pEventMsg[i].uFWMessageInfo.sUnsupportedAlgoInfo));
                }    
                else if(pEventMsg[i].sMsgHeader.ui32EventID == BDSP_RAAGA_ENCODER_OVERFLOW_EVENT_ID)
                {
                    BDSP_Raaga_P_EncoderOutputOverflow_isr((void *)pRaagaTask,NULL);
                }
				else if(pEventMsg[i].sMsgHeader.ui32EventID == BDSP_RAAGA_ANCDATA_EVENT_ID)
                {
                    BDSP_Raaga_P_AncillaryDataPresent_isr((void *)pRaagaTask,NULL);
                }
                else if(pEventMsg[i].sMsgHeader.ui32EventID == BDSP_RAAGA_CHANGE_IN_DIALNORM_EVENT_ID)
                {
                    BDSP_Raaga_P_DialnormChange_isr((void *)pRaagaTask,NULL);
                }
                else
                {
                    BDBG_WRN (("BDSP_Raaga_P_DSP2HostAsyn_isr :: <<< Not a Valid Async Message. EventID >>> %d",
                        pEventMsg[i].sMsgHeader.ui32EventID));
                }
            }
        }
    } /*    for(ui32DspId =0; ui32DspId < BDSP_RM_P_MAX_DSPS; ++ui32DspId) */

    BDBG_LEAVE(BDSP_Raaga_P_DSP2HostAsyn_isr);
    return;
}
 


 /***************************************************************************
 Description:
     This API Installs the top level interrupt handlers for DSP ping command.
 Returns:
     BERR_Code 
 See Also:
     BDSP_Raaga_P_AckUninstall
 ***************************************************************************/
 BERR_Code BDSP_Raaga_P_AckInstall(
     void *pDeviceHandle
 )
 {
     BERR_Code       ret = BERR_SUCCESS;
#if 0 /* Not used now. May be used in future*/
     unsigned int    uiDspId;     
     uint32_t        ui32RegVal = 0;
     bool            bWdgRecovery = false;
#endif

    BDSP_Raaga *pDevice = (BDSP_Raaga *)pDeviceHandle;
    BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);

 
     BDBG_ENTER(BDSP_Raaga_P_AckInstall);
 
#if 0 /* Not used now. May be used in future*/
     for(uiDspId =0 ; uiDspId< BDSP_RAAGA_MAX_DSP ; uiDspId++)
    {
#if 0     
     if(false == bWdgRecovery)
#endif        
         {    
             ret = BINT_CreateCallback(
                 &pDevice->ackCallbackHandle[uiDspId],
                 pDevice->intHandle,
                 ui32DSPAckInterruptId[uiDspId],
                 BRAP_P_DSP2HostAck_isr,
                 (void*)pDevice,
                 0 
                 );
             if(ret != BERR_SUCCESS )
             {
                 BDBG_ERR(("Create Callback hDspAckCallback failed for DSP = %d BCHP_INT_ID_ESR_SO2",
                             uiDspId));
                 ret = BERR_TRACE(ret);
             }
             if(pDevice->ackCallbackHandle[uiDspId])
             {
                 ret = BINT_EnableCallback(pDevice->ackCallbackHandle[uiDspId]);
                 if(ret != BERR_SUCCESS )
                 {
                     BDBG_ERR(("Enable Callback hDspAckCallback failed for DSP = %d BCHP_INT_ID_ESR_SO2",
                             uiDspId));
                     ret = BERR_TRACE(ret);
                 }
             }
         }
#if 0
        else
         {
             if(hDsp->hDspAckCallback)
             {
                 ret = BINT_DisableCallback(hDsp->hDspAckCallback);
                 if(ret != BERR_SUCCESS )
                 {
                     BDBG_MSG(("Disable Callback hDspAckCallback failed for DSP = %d BCHP_INT_ID_ESR_SO2",
                     uiDspId));
                     ret = BERR_TRACE(ret);
                 }
             }
             if(hDsp->hDspAckCallback)
             {            
                 ret = BINT_EnableCallback(hDsp->hDspAckCallback);
                 if(ret != BERR_SUCCESS )
                 {
                     BDBG_MSG(("Enable Callback hDspAckCallback failed for DSP = %d BCHP_INT_ID_ESR_SO2",
                     uiDspId));
                     ret = BERR_TRACE(ret);
                 }
             }
     
             ui32RegVal =0;
             ui32RegVal |=BCHP_MASK(AUD_DSP_INTH0_R5F_MASK_CLEAR, ESR_SO2);    
             BDSP_Write32_isr(hDsp->hRegister, BCHP_AUD_DSP_INTH0_R5F_MASK_CLEAR, ui32RegVal);    
         }
#endif     
    }
 #endif
     BDBG_LEAVE(BDSP_Raaga_P_AckInstall);        
     return ret;
 }

  
   /***************************************************************************
   Description:
       This API Un Installs the top level interrupt handlers for DSP ping command.
   Returns:
       BERR_Code 
   See Also:
       BDSP_Raaga_P_AckInstall
   ***************************************************************************/
   BERR_Code BDSP_Raaga_P_AckUninstall(
       void *pDeviceHandle
   )
{
    BERR_Code       ret = BERR_SUCCESS;  
    BDSP_Raaga *pDevice = (BDSP_Raaga *)pDeviceHandle;
    BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);
    return ret;
}


  /***************************************************************************
 Description:
     This API uninstalls the top level interrupt handlers for all the interrups.
 Returns:
     BERR_Code 
 See Also:
     BDSP_P_InterruptUnInstall
 ***************************************************************************/
 BERR_Code BDSP_Raaga_P_InterruptUninstall (
     void    *pTaskHandle        /* [in] Raptor Channel handle */
 )
 {
     BERR_Code       ret = BERR_SUCCESS;
     unsigned int    uiCounter;
     BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;
 
     BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
     
     BDBG_ENTER (BDSP_Raaga_P_InterruptUninstall);    
 
#if 0
     bool            bWdgRecovery = false;
     bWdgRecovery = BDSP_P_GetWatchdogRecoveryFlag(pRaagaTask->hRap);
 
     if(false == bWdgRecovery)
#endif        
     {
         /* We will create two callbacks for Asynchronous Interrupt & for 
         Synchtonous interrupt on both DAP0 as well as on DS1 */
         for(uiCounter = 0; uiCounter < BDSP_RAAGA_MAX_DSP; ++uiCounter)
         {
            if(pRaagaTask->interruptCallbacks.hDSPAsync[uiCounter])
            {
                 ret = BINT_DisableCallback(pRaagaTask->interruptCallbacks.hDSPAsync[uiCounter]);
                 if (ret!=BERR_SUCCESS)
                 {
                     ret = BERR_TRACE(ret);
                     goto end;
                 }
                 ret = BINT_DestroyCallback(pRaagaTask->interruptCallbacks.hDSPAsync[uiCounter]);
                 if (ret!=BERR_SUCCESS)
                 {
                     ret = BERR_TRACE(ret);
                     goto end;
                 }
            }

             if(pRaagaTask->interruptCallbacks.hDSPSync[uiCounter])
             {
                 ret = BINT_DisableCallback(pRaagaTask->interruptCallbacks.hDSPSync[uiCounter]);
                 if (ret!=BERR_SUCCESS)
                 {
                     ret = BERR_TRACE(ret);
                     goto end;
                 }
                 ret = BINT_DestroyCallback(pRaagaTask->interruptCallbacks.hDSPSync[uiCounter]);
                 if (ret!=BERR_SUCCESS)
                 {
                     ret = BERR_TRACE(ret);
                     goto end;
                 }              
            }
         }        
     }
#if 0	
         else
     {   
         /* Now enable the callback */
         if ( (BDSP_ChannelType_eDecode == pRaagaTask->eChannelType) ||
              (BDSP_ChannelType_ePcmCapture== pRaagaTask->eChannelType)        
            )            
         {
             for(uiCounter = 0; uiCounter < BDSP_RM_P_MAX_DSPS; ++uiCounter)
             {
                 if (pRaagaTask->interruptCallbacks.hDSPSync[uiCounter])
                 {        
                     ret = BINT_EnableCallback(pRaagaTask->interruptCallbacks.hDSPSync[uiCounter]);
                     if(ret != BERR_SUCCESS )
                     {
                         BDBG_ERR(("Enable Callback failed for BCHP_INT_ID_ESR_SO0"));
                         ret = BERR_TRACE(ret);
                         goto end;
                     }
                 }
 
                 if (pRaagaTask->interruptCallbacks.hDSPAsync[uiCounter])
                 {        
                     ret = BINT_EnableCallback(pRaagaTask->interruptCallbacks.hDSPAsync[uiCounter]);
                     if(ret != BERR_SUCCESS )
                     {
                         BDBG_ERR(("Enable Callback failed for  INT_ID_ESR_SO1"));
                         ret = BERR_TRACE(ret);
                         goto end;
                     }
                 }
             }
         }
 
         if (pRaagaTask->interruptCallbacks.hFMM)
         {        
             ret = BINT_EnableCallback(pRaagaTask->interruptCallbacks.hFMM);     
             if ( ret != BERR_SUCCESS )
             {
                 BDBG_ERR(("Enable Callback failed for FMM INT_ID[%d]", BCHP_INT_ID_FMM_BF2));
                 ret = BERR_TRACE(ret);
                 goto end;
             }
         }
    }
#endif
 end:
     BDBG_LEAVE(BDSP_Raaga_P_InterruptUninstall);
     return ret;    
 }


 /***************************************************************************
Description:
	This API installs the top level interrupt handlers for all the interrups.
Returns:
	BERR_Code 
See Also:
	BDSP_P_InterruptUnInstall
***************************************************************************/
BERR_Code BDSP_Raaga_P_InterruptInstall (
	void    *pTaskHandle		/* [in] Raptor Channel handle */
)
{
    BERR_Code       ret = BERR_SUCCESS;
    unsigned int    uiCounter;
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;
    BDSP_Raaga *pDevice = pRaagaTask->pContext->pDevice;

    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
    
    BDBG_ENTER (BDSP_Raaga_P_InterruptInstall);    


#if 0
    bool            bWdgRecovery = false;
    bWdgRecovery = BDSP_P_GetWatchdogRecoveryFlag(pRaagaTask->hRap);

    if(false == bWdgRecovery)
#endif        
    {
        /* We will create two callbacks for Asynchronous Interrupt & for 
        Synchtonous interrupt on both DAP0 as well as on DS1 */
        for(uiCounter = 0; uiCounter < BDSP_RAAGA_MAX_DSP; ++uiCounter)
        {
            ret = BINT_CreateCallback(
                                        &pRaagaTask->interruptCallbacks.hDSPAsync[uiCounter],
                                        pDevice->intHandle,
                                        ui32DSPAsynInterruptId[uiCounter],
                                        BDSP_Raaga_P_DSP2HostAsyn_isr,
                                        (void*)pDevice,
                                        uiCounter
                                        );
            if(ret != BERR_SUCCESS )
            {
                BDBG_ERR(("Create Callback failed for DSP = %d INT_ID_ESR_SO1",uiCounter));
                ret = BERR_TRACE(ret);
                goto err_callback;
            }
            ret = BINT_EnableCallback(pRaagaTask->interruptCallbacks.hDSPAsync[uiCounter]);
            if(ret != BERR_SUCCESS )
            {
                BDBG_ERR(("Enable Callback failed for DSP = %d INT_ID_ESR_SO1",uiCounter));
                ret = BERR_TRACE(ret);
                goto err_callback;
            }

            ret = BINT_CreateCallback(
                                        &pRaagaTask->interruptCallbacks.hDSPSync[uiCounter],
                                        pDevice->intHandle,
                                        ui32DSPSynInterruptId[uiCounter],
                                        BDSP_Raaga_P_DSP2HostSyn_isr,
                                        (void*)pDevice,
                                        uiCounter 
                                        );
            if(ret != BERR_SUCCESS )
            {
                BDBG_ERR(("Create Callback failed for DSP = %d BCHP_INT_ID_ESR_SO0",uiCounter));
                ret = BERR_TRACE(ret);
                goto err_callback;
            }
            
            ret = BINT_EnableCallback(pRaagaTask->interruptCallbacks.hDSPSync[uiCounter]);
            if(ret != BERR_SUCCESS )
            {
                BDBG_ERR(("Enable Callback failed for DSP = %d BCHP_INT_ID_ESR_SO0",uiCounter));
                ret = BERR_TRACE(ret);
                goto err_callback;
            }
        }        
    }
#if 0	
        else
	{   
        /* Now enable the callback */
        if ( (BDSP_ChannelType_eDecode == pRaagaTask->eChannelType) ||
             (BDSP_ChannelType_ePcmCapture== pRaagaTask->eChannelType)        
           )            
        {
            for(uiCounter = 0; uiCounter < BDSP_RM_P_MAX_DSPS; ++uiCounter)
            {
                if (pRaagaTask->interruptCallbacks.hDSPSync[uiCounter])
                {        
                    ret = BINT_EnableCallback(pRaagaTask->interruptCallbacks.hDSPSync[uiCounter]);
                    if(ret != BERR_SUCCESS )
                    {
                        BDBG_ERR(("Enable Callback failed for BCHP_INT_ID_ESR_SO0"));
                        ret = BERR_TRACE(ret);
                        goto end;
                    }
                }

                if (pRaagaTask->interruptCallbacks.hDSPAsync[uiCounter])
                {        
                    ret = BINT_EnableCallback(pRaagaTask->interruptCallbacks.hDSPAsync[uiCounter]);
                    if(ret != BERR_SUCCESS )
                    {
                        BDBG_ERR(("Enable Callback failed for  INT_ID_ESR_SO1"));
                        ret = BERR_TRACE(ret);
                        goto end;
                    }
                }
            }
        }

        if (pRaagaTask->interruptCallbacks.hFMM)
        {        
            ret = BINT_EnableCallback(pRaagaTask->interruptCallbacks.hFMM);		
            if ( ret != BERR_SUCCESS )
            {
                BDBG_ERR(("Enable Callback failed for FMM INT_ID[%d]", BCHP_INT_ID_FMM_BF2));
                ret = BERR_TRACE(ret);
                goto end;
            }
        }
   }
#endif

goto end;

err_callback:    
    if(ret != BERR_SUCCESS )
    {
        BDSP_Raaga_P_InterruptUninstall(pTaskHandle);
    }

end:
	BDBG_LEAVE(BDSP_Raaga_P_InterruptInstall);
	return ret;    
}

/***************************************************************************
Description:
	The APIs below are the low level interrupt hanlders which are dispatched 
	from the top level handlers
Returns:
	All of them return void
Note:
	This comment is common for all the APIs below
***************************************************************************/

static void BDSP_Raaga_P_Watchdog_isr(void *pDeviceHandle,  int iParm2 )
{
    BDSP_Raaga   *pRaagaDevice = (BDSP_Raaga   *)pDeviceHandle;   
    BDSP_RaagaContext *pRaagaContext=NULL;    
	/* Param2 is dspIndex */
    /* BSTD_UNUSED (iParm2);  */
    BDBG_OBJECT_ASSERT(pRaagaDevice, BDSP_Raaga);

    pRaagaDevice->deviceWatchdogFlag = true;

	/* Mask all the external interrupts for this dsp */
	BDSP_Write32(pRaagaDevice->regHandle, \
		BCHP_RAAGA_DSP_ESR_SI_MASK_SET + pRaagaDevice->dspOffset[iParm2], \
		0xffffffff);

    BDSP_Raaga_P_ResetHardware_isr(pDeviceHandle, iParm2);

    for ( pRaagaContext = BLST_S_FIRST(&pRaagaDevice->contextList);
          pRaagaContext != NULL;
          pRaagaContext = BLST_S_NEXT(pRaagaContext, node) )
    {
        pRaagaContext->contextWatchdogFlag = true;
        BDBG_WRN(("Raaga Watchdog Interrupt called. calling Function callback for  Context handle 0x%08x", pRaagaContext));
        
	/* Call the application callback */
	if(pRaagaContext->interruptHandlers.watchdog.pCallback_isr) 
	{
	
		pRaagaContext->interruptHandlers.watchdog.pCallback_isr (
			pRaagaContext->interruptHandlers.watchdog.pParam1,
			iParm2
		);        
	}
	
    }
}


/***************************************************************************
Description:
	This API Installs callbacks for device level interrupts.
Returns:
	BERR_Code 
See Also:
	BDSP_P_DeviceLevelInterruptUnInstall
***************************************************************************/
BERR_Code BDSP_Raaga_P_ContextInterruptInstall (
    void    *pContextHandle        
)
{
    BERR_Code ret = BERR_SUCCESS;
	uint32_t	ui32DspIndex;
    BDSP_RaagaContext   *pRaagaContext =(BDSP_RaagaContext *)pContextHandle;

    BDBG_OBJECT_ASSERT(pRaagaContext, BDSP_RaagaContext);
    
    if(BLST_S_EMPTY(&(pRaagaContext->pDevice->contextList))) /*callback should be created for first context*/
    {
	    for(ui32DspIndex = 0; ui32DspIndex < BDSP_RAAGA_MAX_DSP; ui32DspIndex++)
    	{
    		ret = BINT_CreateCallback(
	    				&pRaagaContext->pDevice->hWatchdogCallback[ui32DspIndex],
    					pRaagaContext->pDevice->intHandle,
	    				ui32DSPWatchDogInterruptId[ui32DspIndex],
    					BDSP_Raaga_P_Watchdog_isr,
    					(void*)pRaagaContext->pDevice,
	    				ui32DspIndex 
    					);

    		if (ret!=BERR_SUCCESS)
			{
				ret = BERR_TRACE(ret);
				goto end;
    		}
	    }  
    }  
end:     
	return ret;
}


/***************************************************************************
Description:
	This API Uninstalls callbacks for device level interrupts.
Returns:
	BERR_Code 
See Also:
	BDSP_P_DeviceLevelInterruptUnInstall
***************************************************************************/
BERR_Code BDSP_Raaga_P_ContextInterruptUninstall (
    void    *pContextHandle        
)
{
    BERR_Code ret = BERR_SUCCESS;
	uint32_t	ui32DspIndex;
    BDSP_RaagaContext   *pRaagaContext =(BDSP_RaagaContext *)pContextHandle;

    BDBG_OBJECT_ASSERT(pRaagaContext, BDSP_RaagaContext);

    if(BLST_S_EMPTY(&(pRaagaContext->pDevice->contextList))) /*callback  should be destroyed for last context*/
    {
	    for(ui32DspIndex = 0; ui32DspIndex < BDSP_RAAGA_MAX_DSP; ui32DspIndex++)
        {
	        if(pRaagaContext->pDevice->hWatchdogCallback[ui32DspIndex])
			{
	             ret = BINT_DisableCallback(pRaagaContext->pDevice->hWatchdogCallback[ui32DspIndex]);
				 if (ret!=BERR_SUCCESS)
				 {
					 ret = BERR_TRACE(ret);
					 goto end;
				 }
	             ret = BINT_DestroyCallback(pRaagaContext->pDevice->hWatchdogCallback[ui32DspIndex]);
				 if (ret!=BERR_SUCCESS)
				 {
					 ret = BERR_TRACE(ret);
					 goto end;
				 }  
 		         pRaagaContext->pDevice->hWatchdogCallback[ui32DspIndex] = NULL;
	        }
        }
    }
     
end:     
    BDBG_LEAVE (BDSP_P_DeviceLevelInterruptInstall);
	return ret;
}


void BDSP_Raaga_P_GetInterruptHandlers(
    void *pContextHandle, 
    BDSP_ContextInterruptHandlers *pInterrupts)
{
    BDSP_RaagaContext   *pRaagaContext = (BDSP_RaagaContext *)pContextHandle;       
    BDBG_OBJECT_ASSERT(pRaagaContext, BDSP_RaagaContext);
    BDBG_ASSERT(pInterrupts);

    *pInterrupts = pRaagaContext->interruptHandlers;    

}

 BERR_Code BDSP_Raaga_P_EnableDspWatchdogTimer (
						void    *pDeviceHandle,
						bool				bEnable
						)
{
    BERR_Code err = BERR_SUCCESS;
    BDSP_Raaga *pDevice = (BDSP_Raaga *)pDeviceHandle;
    uint32_t regVal = 0,i=0;

    BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);

    if (bEnable) 
    {
        /* Disable auto reload of count */
        regVal = regVal & ~(BCHP_MASK(RAAGA_DSP_TIMERS_WATCHDOG_TIMER, CM));
        /* Enable timer bit */
        regVal = regVal | BCHP_MASK(RAAGA_DSP_TIMERS_WATCHDOG_TIMER, ET);
        /* Program default watchdog count */
        regVal = (regVal & ~(BCHP_MASK(RAAGA_DSP_TIMERS_WATCHDOG_TIMER, COUNT)))
                        | BCHP_FIELD_DATA(RAAGA_DSP_TIMERS_WATCHDOG_TIMER, COUNT, BDSP_RAAGA_DEFAULT_WATCHDOG_COUNT);
    }
    else {
        /* Disable timer bit */
        regVal = regVal & ~(BCHP_MASK(RAAGA_DSP_TIMERS_WATCHDOG_TIMER, ET));
    }
    for(i = 0; i < BDSP_RAAGA_MAX_DSP; i++)
    {
        BDSP_Write32(pDevice->regHandle, BCHP_RAAGA_DSP_TIMERS_WATCHDOG_TIMER + pDevice->dspOffset[i], regVal);
    }
    return err;
}	


BERR_Code BDSP_Raaga_P_SetInterruptHandlers(
    void *pContextHandle,
    const BDSP_ContextInterruptHandlers *pInterrupts)
{
    BDSP_RaagaContext *pRaagaContext = (BDSP_RaagaContext *)pContextHandle;
	uint32_t			ui32DspIndex;
    BERR_Code err=BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(pRaagaContext, BDSP_RaagaContext);
    BDBG_ASSERT(pInterrupts);    

    if(pInterrupts->watchdog.pCallback_isr != NULL)
    {    
        err = BDSP_Raaga_P_EnableDspWatchdogTimer((void *)pRaagaContext->pDevice,true);

        if (err!=BERR_SUCCESS)
        {         
            return BERR_TRACE(err);
        }        
		for(ui32DspIndex = 0; ui32DspIndex < BDSP_RAAGA_MAX_DSP; ui32DspIndex++)
        {
	        if(pRaagaContext->pDevice->hWatchdogCallback[ui32DspIndex])
			{
	            err = BINT_EnableCallback (pRaagaContext->pDevice->hWatchdogCallback[ui32DspIndex]);
				if(err!=BERR_SUCCESS)
				{         
					return BERR_TRACE(err); 
	            }        
		    }
		}
    }
    
    pRaagaContext->interruptHandlers = *pInterrupts;    

    return err;
}

BERR_Code BDSP_Raaga_P_ProcessWatchdogInterrupt(
    void *pContextHandle)
{
    BDSP_RaagaContext *pRaagaContext = (BDSP_RaagaContext *)pContextHandle;
    BERR_Code err=BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(pRaagaContext, BDSP_RaagaContext);

    BKNI_EnterCriticalSection();
    /* BDSP_Raaga_P_ProcessWatchdogInterrupt() should not be called from within 
    Critical sections. To ensure this, added these few lines of code. 
    Do nothing here. If BDSP_Raaga_P_ProcessWatchdogInterrupt is called incorrectly from 
    a critical section, this piece of code will cause it to deadlock and exit */
    BKNI_LeaveCriticalSection();
    
    BKNI_AcquireMutex(pRaagaContext->pDevice->watchdogMutex);            

    if(pRaagaContext->pDevice->deviceWatchdogFlag == true)
    {
        BDSP_Raaga_P_Open(pRaagaContext->pDevice);    
        pRaagaContext->pDevice->deviceWatchdogFlag = false;
    }
    pRaagaContext->contextWatchdogFlag = false;
    
    BKNI_ReleaseMutex(pRaagaContext->pDevice->watchdogMutex);        

    BDBG_WRN(("BDSP_Raaga_P_ProcessWatchdogInterrupt called"));

    return err;
}

void BDSP_Raaga_P_GetAudioInterruptHandlers_isr(
    void *pTaskHandle,
    BDSP_AudioInterruptHandlers *pHandlers)
{
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;       
    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
    BDBG_ASSERT(pHandlers);

    *pHandlers = pRaagaTask->audioInterruptHandlers;    

}

BERR_Code BDSP_Raaga_P_SetAudioInterruptHandlers_isr(
    void *pTaskHandle,
    const BDSP_AudioInterruptHandlers *pHandlers)
{
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;  
    BDSP_RaagaContext *pRaagaContext = (BDSP_RaagaContext *)pRaagaTask->pContext;
    BDSP_Raaga  *pDevice= pRaagaContext->pDevice;
    BERR_Code err=BERR_SUCCESS;
    BDSP_Raaga_P_Command     sFwCommand;    
    
    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
    BDBG_OBJECT_ASSERT(pRaagaContext, BDSP_RaagaContext);
    BDBG_ASSERT(pHandlers);
        
/*First PTS Interrupt*/
    if((pHandlers->firstPts.pCallback_isr == NULL))
    {
        /*Send the command to DSP to mask the interrupt*/
        pRaagaTask->eventEnabledMask &= ~(BDSP_Raaga_P_EventIdMask_eFirstPTS_Received);
    }
    else
    {
        /*Send the command to DSP to unmask the interrupt*/
        pRaagaTask->eventEnabledMask |= (BDSP_Raaga_P_EventIdMask_eFirstPTS_Received);        
    }

/*Sample Rate Change Interrupt*/
    if((pHandlers->sampleRateChange.pCallback_isr == NULL))
    {
        /*Send the command to DSP to mask the interrupt*/
        pRaagaTask->eventEnabledMask &= ~(BDSP_Raaga_P_EventIdMask_eSampleRateChange);  
    }
    else
    {
        /*Send the command to DSP to unmask the interrupt*/
        pRaagaTask->eventEnabledMask |= (BDSP_Raaga_P_EventIdMask_eSampleRateChange);        
    }        

/*TSM Fail Interrupt*/
    if((pHandlers->tsmFail.pCallback_isr == NULL))
    {
        /*Send the command to DSP to mask the interrupt*/
        pRaagaTask->eventEnabledMask &= ~(BDSP_Raaga_P_EventIdMask_eTsmFail);
    }
    else
    {
        /*Send the command to DSP to unmask the interrupt*/
        pRaagaTask->eventEnabledMask |= (BDSP_Raaga_P_EventIdMask_eTsmFail);        
    }    

/*TSM Pass Interrupt*/
    if((pHandlers->tsmPass.pCallback_isr == NULL))
    {
        /*Send the command to DSP to mask the interrupt*/
        pRaagaTask->eventEnabledMask &= ~(BDSP_Raaga_P_EventIdMask_eAstmTsmPass);
    }
    else 
    {
        /*Send the command to DSP to unmask the interrupt*/
        pRaagaTask->eventEnabledMask |= (BDSP_Raaga_P_EventIdMask_eAstmTsmPass);
    }        

/*Lock Interrupt*/
    if((pHandlers->lock.pCallback_isr == NULL))
    {
        /*Send the command to DSP to mask the interrupt*/
        pRaagaTask->eventEnabledMask &= ~(BDSP_Raaga_P_EventIdMask_eFrameSyncLock);
    }
    else
    {
        /*Send the command to DSP to unmask the interrupt*/
        pRaagaTask->eventEnabledMask |= (BDSP_Raaga_P_EventIdMask_eFrameSyncLock);        
    }      

/*Unlock Interrupt*/
    if((pHandlers->unlock.pCallback_isr == NULL))
    {
        /*Send the command to DSP to mask the interrupt*/
        pRaagaTask->eventEnabledMask &= ~(BDSP_Raaga_P_EventIdMask_eFrameSyncLockLost);
    }
    else
    {
        /*Send the command to DSP to unmask the interrupt*/
        pRaagaTask->eventEnabledMask |= (BDSP_Raaga_P_EventIdMask_eFrameSyncLockLost);        
    } 

/*Status Interrupt*/
    if((pHandlers->statusReady.pCallback_isr == NULL))
    {
        /*Send the command to DSP to mask the interrupt*/
        pRaagaTask->eventEnabledMask &= ~(BDSP_Raaga_P_EventIdMask_eStreamInfoAvail);    
    }
    else
    {
        /*Send the command to DSP to unmask the interrupt*/
        pRaagaTask->eventEnabledMask |= (BDSP_Raaga_P_EventIdMask_eStreamInfoAvail);  
    } 

    /*Mode Change Interrupt*/
    if((pHandlers->modeChange.pCallback_isr == NULL))
    {
        /*Send the command to DSP to mask the interrupt*/
        pRaagaTask->eventEnabledMask &= ~(BDSP_Raaga_P_EventIdMask_eAudioModeChange);    
    }
    else
    {
        /*Send the command to DSP to unmask the interrupt*/
        pRaagaTask->eventEnabledMask |= (BDSP_Raaga_P_EventIdMask_eAudioModeChange);  
    } 

    /*Bit rate Change Interrupt*/
    if((pHandlers->bitrateChange.pCallback_isr == NULL))
    {
        /*Send the command to DSP to mask the interrupt*/
        pRaagaTask->eventEnabledMask &= ~(BDSP_Raaga_P_EventIdMask_eBitRateChange);    
    }
    else
    {
        /*Send the command to DSP to unmask the interrupt*/
        pRaagaTask->eventEnabledMask |= (BDSP_Raaga_P_EventIdMask_eBitRateChange);  
    }  

    /*CDB/ITB overflow Interrupt*/
    if((pHandlers->cdbItbOverflow.pCallback_isr == NULL))
    {
        /*Send the command to DSP to mask the interrupt*/
        pRaagaTask->eventEnabledMask &= ~(BDSP_Raaga_P_EventIdMask_eCdbItbOverflow);    
    }
    else
    {
        /*Send the command to DSP to unmask the interrupt*/
        pRaagaTask->eventEnabledMask |= (BDSP_Raaga_P_EventIdMask_eCdbItbOverflow);  
    }        
    
    /*CDB/ITB underflow Interrupt*/
    if((pHandlers->cdbItbUnderflow.pCallback_isr == NULL))
    {
        /*Send the command to DSP to mask the interrupt*/
        pRaagaTask->eventEnabledMask &= ~(BDSP_Raaga_P_EventIdMask_eCdbItbUnderflow);    
    }
    else
    {
        /*Send the command to DSP to unmask the interrupt*/
        pRaagaTask->eventEnabledMask |= (BDSP_Raaga_P_EventIdMask_eCdbItbUnderflow);  
    }    

    /*Start PTS reached Interrupt*/
    if((pHandlers->startPtsReached.pCallback_isr == NULL))
    {
        /*Send the command to DSP to mask the interrupt*/
        pRaagaTask->eventEnabledMask &= ~(BDSP_Raaga_P_EventIdMask_eStartOnPTS);    
    }
    else
    {
        /*Send the command to DSP to unmask the interrupt*/
        pRaagaTask->eventEnabledMask |= (BDSP_Raaga_P_EventIdMask_eStartOnPTS);  
    }     

    /*Stop PTS reached Interrupt*/
    if((pHandlers->stopPtsReached.pCallback_isr == NULL))
    {
        /*Send the command to DSP to mask the interrupt*/
        pRaagaTask->eventEnabledMask &= ~(BDSP_Raaga_P_EventIdMask_eStopOnPTS);    
    }
    else
    {
        /*Send the command to DSP to unmask the interrupt*/
        pRaagaTask->eventEnabledMask |= (BDSP_Raaga_P_EventIdMask_eStopOnPTS);  
    }         

    /*Unlicensed Algo Interrupt*/
    if((pHandlers->unlicensedAlgo.pCallback_isr == NULL))
    {
        /*Send the command to DSP to mask the interrupt*/
        pRaagaTask->eventEnabledMask &= ~(BDSP_Raaga_P_EventIdMask_eUnlicensedAlgo);    
    }
    else
    {
        /*Send the command to DSP to unmask the interrupt*/
        pRaagaTask->eventEnabledMask |= (BDSP_Raaga_P_EventIdMask_eUnlicensedAlgo);  
    }     

    /*Ramp Enable Interrupt*/
    if((pHandlers->rampEnable.pCallback_isr == NULL))
    {
        /*Send the command to DSP to mask the interrupt*/
        pRaagaTask->eventEnabledMask &= ~(BDSP_Raaga_P_EventIdMask_eRampEnable);    
    }
    else
    {
        /*Send the command to DSP to unmask the interrupt*/
        pRaagaTask->eventEnabledMask |= (BDSP_Raaga_P_EventIdMask_eRampEnable);  
    } 

    /*Encoder output overflow Interrupt*/
    if((pHandlers->encoderOutputOverflow.pCallback_isr == NULL))
    {
        /*Send the command to DSP to mask the interrupt*/
        pRaagaTask->eventEnabledMask &= ~(BDSP_Raaga_P_EventIdMask_eEncoderOverflow);
    }
    else
    {
        /*Send the command to DSP to unmask the interrupt*/
        pRaagaTask->eventEnabledMask |= (BDSP_Raaga_P_EventIdMask_eEncoderOverflow);        
    }

     /* Ancillary Data Interrupt */
     if((pHandlers->ancillaryData.pCallback_isr == NULL))
     {
        /*Send the command to DSP to mask the interrupt*/
        pRaagaTask->eventEnabledMask &= ~(BDSP_Raaga_P_EventIdMask_eAncData);
     }
     else
     {
        /*Send the command to DSP to unmask the interrupt*/
        pRaagaTask->eventEnabledMask |= (BDSP_Raaga_P_EventIdMask_eAncData);
     }
     
     /* Dialnorm Change Interrupt */
     if((pHandlers->dialnormChange.pCallback_isr == NULL))
     {
        /*Send the command to DSP to mask the interrupt*/
        pRaagaTask->eventEnabledMask &= ~(BDSP_Raaga_P_EventIdMask_eChangeInDialnorm);
     }
     else
     {
        /*Send the command to DSP to unmask the interrupt*/
        pRaagaTask->eventEnabledMask |= (BDSP_Raaga_P_EventIdMask_eChangeInDialnorm);
     }


    if((pRaagaTask->isStopped == false))
    {
        /*  Prepare message structure for FW to write in message queue */
        sFwCommand.sCommandHeader.ui32CommandID = BDSP_RAAGA_EVENT_NOTIFICATION_COMMAND_ID;
        sFwCommand.sCommandHeader.ui32CommandSizeInBytes = sizeof(BDSP_Raaga_P_Command);
        sFwCommand.sCommandHeader.eResponseType = BDSP_Raaga_P_ResponseType_eNone;
        sFwCommand.sCommandHeader.ui32TaskID = pRaagaTask->taskId;
        sFwCommand.sCommandHeader.ui32CommandCounter = pRaagaTask->commandCounter++;;
        sFwCommand.uCommand.sEnableDisableEvent.ui32EnableEvent = 
            pRaagaTask->eventEnabledMask ;
        
        /* Write in Message queue */
        err = BDSP_Raaga_P_SendCommand_isr(pDevice->hCmdQueue[pRaagaTask->settings.dspIndex], &sFwCommand,(void *)pRaagaTask);
        if(err != BERR_SUCCESS)
        {
            BDBG_ERR(("BDSP_Raaga_P_SetAudioInterruptHandlers_isr: Send command failed"));
            err = BERR_TRACE(err);  
            goto end;
        }    
    }
    pRaagaTask->audioInterruptHandlers = *pHandlers;
end:
    return err;
}


/**********************************************************
Function: BDSP_Raaga_P_RestoreInterrupts_isr

Description :  Isr version of  BDSP_Raaga_P_RestoreInterrupts
				Restores the interrupts to their state
				before reset

**********************************************************/
uint32_t BDSP_Raaga_P_RestoreInterrupts_isr(
    void                *pDeviceHandle,
    uint32_t    uiDspIndex)
{	
	BDSP_Raaga *pDevice = (BDSP_Raaga *)pDeviceHandle;
	BERR_Code err = BERR_SUCCESS;
	uint32_t L2Reg ;
	
	BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);
	/* Dsp Sync and Async interrupt registers use the same L2 register so using only one
	of the name of the interrupts to derive the L2 register name*/	
	L2Reg = BCHP_INT_ID_GET_REG(ui32DSPSynInterruptId[uiDspIndex]);
	BINT_ApplyL2State_isr(pDevice->intHandle,L2Reg );
	
	L2Reg = BCHP_INT_ID_GET_REG(ui32DSPWatchDogInterruptId[uiDspIndex]);
	BINT_ApplyL2State_isr(pDevice->intHandle,L2Reg );

	return err;  
		
}
