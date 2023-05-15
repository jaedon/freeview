/***************************************************************************
 *     Copyright (c) 2005-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhsm_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 8/12/08 5:01p $
 *
 * Module Description: This file contains Broadcom Host Secure Module (OS/platform 
 *                     independent) porting interface public functions.
 *                    			                    
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7601/a0/common/bhsm_priv.c $
 * 
 * Hydra_Software_Devel/1   8/12/08 5:01p atruong
 * PR45631: HSM PI support for BCM7601 platform
 * 
 * Hydra_Software_Devel/4   5/7/07 10:16a lyang
 * PR 30454: HSM update for SOAP test support in SubmitRawCommand
 * 
 * Hydra_Software_Devel/3   5/4/07 5:08p lyang
 * PR 30454: HSM update for SOAP test support in SubmitRawCommand
 * 
 * Hydra_Software_Devel/2   3/26/07 11:55a lyang
 * PR 28967: to add HSM support for 7440B0 Non-OS
 * 
 * Hydra_Software_Devel/13   2/9/07 3:57p lyang
 * PR 27777: HSM add IRDY waiting loop, add polling support 27393
 * 
 * Hydra_Software_Devel/12   12/19/06 6:58p lyang
 * PR 26037: HSM to support GenerateRouteContentKeys and MKB processing
 * commands. Add DVD performance experiment code
 * 
 * Hydra_Software_Devel/11   12/1/06 8:48p lyang
 * PR 25504: BHSM_P_CommonSubmitCommand releases mutex without acquireing
 * 
 * Hydra_Software_Devel/10   11/21/06 3:41p dbayani
 * PR 26105: Removed version check.
 * 
 * Hydra_Software_Devel/9   10/26/06 4:19p lyang
 * PR 23412: hsm pi include files should be re-factored so they are not
 * order dependent. PR 25133:HSM header files should include dependencies
 * 
 * Hydra_Software_Devel/8   10/19/06 4:00p lyang
 * PR 25028: Release-mode compiler warnings in HSM, unused param/var
 * 
 * Hydra_Software_Devel/7   10/17/06 6:40p lyang
 * PR 24980: HSM: MIPS & BSP have different versionID for certian
 * chip/version
 * 
 * Hydra_Software_Devel/6   9/26/06 3:40p btan
 * PR 23906: Converted BCHP_REV to BCHP_VER - take 2.
 * 
 * Hydra_Software_Devel/5   9/26/06 3:08p btan
 * PR 23906: Converted BCHP_REV to BCHP_VER.
 * 
 * Hydra_Software_Devel/4   9/20/06 5:51p lyang
 * PR 24455: HSM PI data size in bits is less than a byte or a word32,
 * rendering the size accordingly. Found from DH testing.
 * 
 * Hydra_Software_Devel/3   8/17/06 3:30p dbayani
 * PR 23715: Updated NSK PI for 7401B0.
 * 
 * Hydra_Software_Devel/2   4/26/06 11:06a btan
 * PR 20446: HSM needs to compile for 7400 A0
 * 
 * Hydra_Software_Devel/2   3/27/06 7:54p btan
 * PR 20436: Sync up with Aegis 7_0 release
 * 
 * Hydra_Software_Devel/1   1/20/06 5:15p btan
 * PR 19140: Do not encrypt generic mode files.
 * 
 * Hydra_Software_Devel/4   1/17/06 9:34p btan
 * PR 19140: Do not encrypt generic mode files.
 *
 *
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "bint.h"
#if (BCHP_CHIP==7401) &&  (BCHP_VER == BCHP_VER_A0)
#include "bchp_bsp_nonsecure_intr2.h"
#include "bchp_bsp_glb_nonsecure.h"
#else
#include "bchp_bsp_control_intr2.h"
#include "bchp_bsp_glb_control.h"
#endif

#include "bhsm_priv.h"
/*#include "bxpt.h"*/

#include "bhsm.h"



/* special support for  HSM ISR  vs. HSM Polling  */
#if BHSM_DEBUG_POLLING
#include "bchp_timer.h"   
#endif 



BDBG_MODULE(BHSM);

/* #define BDBG_MSG(x) printf x */
/* #define BDBG_ERR(x) printf x */


void BHSM_P_ExceptionInterruptCB_isr( 
	      void		*inp_handle,
	      void       	*inp_data 
) 
{

	BSTD_UNUSED(inp_data);  /* Removed this when we start using this variable */	

	BDBG_MSG(("I am in BHSM_P_ExceptionInterruptCB_isr, ucMaxChannels = %d\n", 
		((BHSM_Handle) inp_handle)->currentSettings.ucMaxChannels));
	
	BSTD_UNUSED(inp_handle);  /* Removed this when we start using this variable */	
}

void BHSM_P_IntHandler_isr(
	void *inp_param1,		/* Device  handle */
	int in_param2			/* reserved */
)
{
	/* BDBG_ASSERT( inp_param1 ); */
	
	BHSM_Handle 	moduleHandle = (BHSM_Handle) inp_param1;
	uint32_t unCpuIntrStatus, unCpuIntrMask;
	BSTD_UNUSED( in_param2 ); 

	BDBG_MSG(("Enter BHSM_P_IntHandler_isr\n"));

#if (BCHP_CHIP==7401) &&  (BCHP_VER == BCHP_VER_A0)
	unCpuIntrStatus = BREG_Read32( moduleHandle->regHandle, BCHP_BSP_GLB_NONSECURE_GLB_HOST_INTR_STATUS );
	unCpuIntrMask = BREG_Read32( moduleHandle->regHandle, BCHP_BSP_GLB_NONSECURE_GLB_HOST_INTR_EN );
#else
	unCpuIntrStatus = BREG_Read32( moduleHandle->regHandle, BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_STATUS );
	unCpuIntrMask = BREG_Read32( moduleHandle->regHandle, BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_EN );
#endif

	BDBG_MSG((" BHSM_P_IntHandler_isr: BCHP_BSP_GLB_NONSECURE_GLB_HOST_INTR_STATUS = 0x%08x, BCHP_BSP_GLB_NONSECURE_GLB_HOST_INTR_EN = 0x%08x\n",
		unCpuIntrStatus, unCpuIntrMask));

#if (BCHP_CHIP==7401) &&  (BCHP_VER == BCHP_VER_A0)
	if ( (unCpuIntrStatus & BCHP_BSP_GLB_NONSECURE_GLB_HOST_INTR_STATUS_OLOAD1_INT_MASK) ) 
#else
	if ( (unCpuIntrStatus & BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_STATUS_OLOAD1_INT_MASK) ) 

#endif
	{
		if (moduleHandle->channelHandles[BHSM_HwModule_eCmdInterface1]->bIsOpen == true) {
			BKNI_SetEvent( 
				moduleHandle->channelHandles[BHSM_HwModule_eCmdInterface1]->oLoadWait);
		}   	

#if (BCHP_CHIP==7401) &&  (BCHP_VER == BCHP_VER_A0)		
		/* Clear OLOAD bit */
		BREG_Write32(  moduleHandle->regHandle,
			BCHP_BSP_GLB_NONSECURE_GLB_OLOAD1,
			BREG_Read32(  moduleHandle->regHandle, 
				BCHP_BSP_GLB_NONSECURE_GLB_OLOAD1) & 
					(~BCHP_BSP_GLB_NONSECURE_GLB_OLOAD1_CMD_OLOAD1_MASK));		

		/* Clear OLOAD2 Interrupt. */
		BREG_Write32( moduleHandle->regHandle, 
			BCHP_BSP_GLB_NONSECURE_GLB_HOST_INTR_STATUS,
			BREG_Read32(  moduleHandle->regHandle, 
				BCHP_BSP_GLB_NONSECURE_GLB_HOST_INTR_STATUS) & 
					(~BCHP_BSP_GLB_NONSECURE_GLB_HOST_INTR_STATUS_OLOAD1_INT_MASK));
#else
		/* Clear OLOAD bit */
		BREG_Write32(  moduleHandle->regHandle,
			BCHP_BSP_GLB_CONTROL_GLB_OLOAD1,
			BREG_Read32(  moduleHandle->regHandle, 
				BCHP_BSP_GLB_CONTROL_GLB_OLOAD1) & 
					(~BCHP_BSP_GLB_CONTROL_GLB_OLOAD1_CMD_OLOAD1_MASK));		

		/* Clear OLOAD2 Interrupt. */
		BREG_Write32( moduleHandle->regHandle, 
			BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_STATUS,
			BREG_Read32(  moduleHandle->regHandle, 
				BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_STATUS) & 
					(~BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_STATUS_OLOAD1_INT_MASK));
#endif
		
	}

#if (BCHP_CHIP==7401) &&  (BCHP_VER == BCHP_VER_A0)
	if ( (unCpuIntrStatus & BCHP_BSP_GLB_NONSECURE_GLB_HOST_INTR_STATUS_OLOAD2_INT_MASK) ) 
#else
	if ( (unCpuIntrStatus & BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_STATUS_OLOAD2_INT_MASK) ) 
#endif
	{
		
		if (moduleHandle->channelHandles[BHSM_HwModule_eCmdInterface2]->bIsOpen == true) {
				
			BKNI_SetEvent( 
				moduleHandle->channelHandles[BHSM_HwModule_eCmdInterface2]->oLoadWait);
		}   	

#if (BCHP_CHIP==7401) &&  (BCHP_VER == BCHP_VER_A0)		
		/* Clear OLOAD bit */
		BREG_Write32(  moduleHandle->regHandle,
			BCHP_BSP_GLB_NONSECURE_GLB_OLOAD2,
			BREG_Read32(  moduleHandle->regHandle, 
				BCHP_BSP_GLB_NONSECURE_GLB_OLOAD2) & 
					(~BCHP_BSP_GLB_NONSECURE_GLB_OLOAD2_CMD_OLOAD2_MASK));		

		/* Clear OLOAD2 Interrupt. */
		BREG_Write32( moduleHandle->regHandle, 
			BCHP_BSP_GLB_NONSECURE_GLB_HOST_INTR_STATUS,
			BREG_Read32(  moduleHandle->regHandle, 
				BCHP_BSP_GLB_NONSECURE_GLB_HOST_INTR_STATUS) & 
					(~BCHP_BSP_GLB_NONSECURE_GLB_HOST_INTR_STATUS_OLOAD2_INT_MASK));
#else
		/* Clear OLOAD bit */
		BREG_Write32(  moduleHandle->regHandle,
			BCHP_BSP_GLB_CONTROL_GLB_OLOAD2,
			BREG_Read32(  moduleHandle->regHandle, 
				BCHP_BSP_GLB_CONTROL_GLB_OLOAD2) & 
					(~BCHP_BSP_GLB_CONTROL_GLB_OLOAD2_CMD_OLOAD2_MASK));		

		/* Clear OLOAD2 Interrupt. */
		BREG_Write32( moduleHandle->regHandle, 
			BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_STATUS,
			BREG_Read32(  moduleHandle->regHandle, 
				BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_STATUS) & 
					(~BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_STATUS_OLOAD2_INT_MASK));
#endif
	}

#if (BCHP_CHIP==7401) &&  (BCHP_VER == BCHP_VER_A0)
	unCpuIntrStatus = BREG_Read32( moduleHandle->regHandle, BCHP_BSP_NONSECURE_INTR2_CPU_STATUS );
#else
	unCpuIntrStatus = BREG_Read32( moduleHandle->regHandle, BCHP_BSP_CONTROL_INTR2_CPU_STATUS );
#endif
	BDBG_MSG((" BHSM_P_IntHandler_isr: BCHP_BSP_NONSECURE_INTR2_CPU_STATUS = 0x%08x\n",
		unCpuIntrStatus));	

#if (BCHP_CHIP==7401) &&  (BCHP_VER == BCHP_VER_A0)
	if ( unCpuIntrStatus & BCHP_BSP_NONSECURE_INTR2_CPU_STATUS_BSP_TO_HOST_INTR_MASK) 
#else
	if ( unCpuIntrStatus & BCHP_BSP_CONTROL_INTR2_CPU_STATUS_BSP_TO_HOST_INTR_MASK) 
#endif		
	{
		if (moduleHandle->bIsOpen == true) {
			if (moduleHandle->currentSettings.exceptionCBfunc != NULL) {
				(*(moduleHandle->currentSettings.exceptionCBfunc)) (moduleHandle, NULL);
			}
		}   			
	}
	

	BDBG_MSG(("Exit BHSM_P_IntHandler_isr\n"));	
		
}


BERR_Code BHSM_P_CommonSubmitCommand (
		BHSM_ChannelHandle		in_channelHandle,	
		BHSM_P_CommandData_t	*inoutp_commandData
)
{
	BERR_Code errCode = BERR_SUCCESS;
	uint32_t	unVersionNum, unOwnerId, unContMode, unTagId, i, unIReady, j=0;
#if BHSM_DEBUG_POLLING
	 uint32_t  startValue, endValue=0;
#endif
	BDBG_MSG(("Inside BHSM_P_CommonSubmitCommand\n"));	
	BDBG_ENTER(BHSM_P_CommonSubmitCommand);
	BDBG_ASSERT( in_channelHandle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_channelHandle->ulMagicNumber != BHSM_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );	

	/* Lock mutex to synchronize the submit command operation */
	if ( 	!(inoutp_commandData ->bLockWaive) )		/* 10/05/05,Allen.C  */
	      BHSM_P_CHECK_ERR_CODE_FUNC(errCode, BKNI_AcquireMutex( in_channelHandle->mutexLock));

	/* Check IReady , ulTimeOutInMilSecs_max = 120 000, multiplied by 500 is till less than FFFFFFFF, no overflow yet*/
	for (i=0; i< in_channelHandle->moduleHandle->currentSettings.ulTimeOutInMilSecs * 500 ;i++)
	{	
		unIReady = BREG_Read32( in_channelHandle->moduleHandle->regHandle, 
							in_channelHandle->ulIReadyRegAddr);
		if (unIReady & in_channelHandle->ulIReadyVal) {
			/*
			errCode = BERR_TRACE(errCode);	
			goto BHSM_P_DONE_LABEL;	
			*/
			break;
		}	
		BKNI_Delay(2);   /*  2us per loop, still blocking so far. May give CPU chance to others if it's non-blocking on other platforms*/
	}
	if ( i ==  in_channelHandle->moduleHandle->currentSettings.ulTimeOutInMilSecs * 500)
	{
		 BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_IRDY_ERR, 1 );
	}

	/* Clear output command buffer */
	for(i = 0; i<BCMD_BUFFER_BYTE_SIZE/4; i++) {
		BREG_Write32( in_channelHandle->moduleHandle->regHandle, 
			in_channelHandle->ulOutCmdBufAddr+ i*4, 0);  
	}	

	/* Check Parameter length */
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_PARM_LEN_ERR, 
		( inoutp_commandData->unInputParamLen > BCMD_NON_HEADER_BUFFER_BYTE_SIZE ) );

	
	/* Fill input buffer. */
	/* Version Number */
	BREG_Write32(  in_channelHandle->moduleHandle->regHandle, 
		in_channelHandle->ulInCmdBufAddr + (BCMD_CommonBufferFields_eVerNum / 4) * 4, 
		BSP_S_DRV_VERSION);

	/* Need a seq num: Owner ID */
	BREG_Write32(  in_channelHandle->moduleHandle->regHandle, 
		in_channelHandle->ulInCmdBufAddr + (BCMD_CommonBufferFields_eOwnerId / 4) * 4,
		in_channelHandle->ulSequenceNum);	

	/* Continuous Mode */
	BREG_Write32(  in_channelHandle->moduleHandle->regHandle, 
		in_channelHandle->ulInCmdBufAddr + (BCMD_CommonBufferFields_eContMode / 4) * 4, 
		inoutp_commandData->unContMode);	

	/* Fill in Tag ID */
	BREG_Write32(  in_channelHandle->moduleHandle->regHandle, 
		in_channelHandle->ulInCmdBufAddr + (BCMD_CommonBufferFields_eTagId / 4) * 4, 
		inoutp_commandData->cmdId ); 		

	/* Parameter Length */
	BREG_Write32(  in_channelHandle->moduleHandle->regHandle, 
		in_channelHandle->ulInCmdBufAddr + (BCMD_CommonBufferFields_eParamLenPlus1 / 4) * 4, 
		inoutp_commandData->unInputParamLen); 

	/* Fill in specific input command parameters */
	for(i = 0; i<(inoutp_commandData->unInputParamLen+3)/4; i++) {
		BREG_Write32( in_channelHandle->moduleHandle->regHandle, 
			in_channelHandle->ulInCmdBufAddr+ ((BCMD_CommonBufferFields_eStatus / 4) * 4) + i*4, 
			inoutp_commandData->unInputParamsBuf[i]);  
	}

	/* Dump out the input commands */
	{
		unsigned int i;
		BDBG_MSG(("MIPS -----------------------------------> BSP:"));  
		for(i = 0; i<(inoutp_commandData->unInputParamLen+3+BCMD_HEADER_BYTE_SIZE)/4; i++) {
	        /* Data */
			BDBG_MSG(("\t\t\t%.3d - %08X ", i*4, BREG_Read32( in_channelHandle->moduleHandle->regHandle, in_channelHandle->ulInCmdBufAddr+ i*4)));  
		}
	}
if ((in_channelHandle->moduleHandle->currentSettings.uSpecialControl & BHSM_CTRLS_POLLINGORISR) ==  BHSM_CTRL_SET_POLLING)
{
	/* now to disable HSM ISR,  the following polling should dominate; 
          Note: this is only disable the channel2 HSM ISR.  channel1 HSM ISR  (high priority for Cancel cmd) is not existing for 3563
       */	

	/* moved to PI, once per setting change as Bryant suggested, which may reduce robustness
	BREG_Write32(in_channelHandle->moduleHandle->regHandle, BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_EN, 
							BREG_Read32(  in_channelHandle->moduleHandle->regHandle, 
										BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_EN) & 
										(~BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_STATUS_OLOAD2_INT_MASK));	 
	
       BREG_Write32(  in_channelHandle->moduleHandle->regHandle,
						BCHP_BSP_GLB_CONTROL_GLB_OLOAD2,
						BREG_Read32(  in_channelHandle->moduleHandle->regHandle, 
										BCHP_BSP_GLB_CONTROL_GLB_OLOAD2) & 
										(~BCHP_BSP_GLB_CONTROL_GLB_OLOAD2_CMD_OLOAD2_MASK));		
	*/

	BDBG_MSG(("////////// HSM in Polling,  ISR2 is disabled, %x, channel num=%d\n",
					in_channelHandle->moduleHandle->currentSettings.uSpecialControl,
					in_channelHandle->ucChannelNumber));
}else
{
	/*
	BREG_Write32(in_channelHandle->moduleHandle->regHandle, BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_EN, 
							BREG_Read32(  in_channelHandle->moduleHandle->regHandle, 	BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_EN) | 
										BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_STATUS_OLOAD2_INT_MASK);	 */
	BDBG_MSG(("//////////HSM in ISR,  ISR2 is enabled, %x, channel num=%d\n",
					in_channelHandle->moduleHandle->currentSettings.uSpecialControl,
					in_channelHandle->ucChannelNumber));
}


	/* Set ILOAD to signal BSP there's a command ready. */
	BDBG_MSG(("BHSM_P_SubmitCommand: ulILoadRegAddr = 0x%08lx, ulILoadVal = %ld\n", 
		in_channelHandle->ulILoadRegAddr, in_channelHandle->ulILoadVal));	
	BREG_Write32( in_channelHandle->moduleHandle->regHandle, 
			in_channelHandle->ulILoadRegAddr,			
			in_channelHandle->ulILoadVal);

	

/*--------------------start of experiement  12/19/06: ly----------------------------------------------
	as Bryant request for a test for Jeff Wong
       replace the ISR based processing with polling, to see if there is some performance improvement
 */


if ( (in_channelHandle->moduleHandle->currentSettings.uSpecialControl & BHSM_CTRLS_POLLINGORISR) == BHSM_CTRL_SET_POLLING)
{

#if BHSM_DEBUG_POLLING	 
	 BREG_Write32(in_channelHandle->moduleHandle->regHandle, BCHP_TIMER_TIMER1_CTRL , 0x9FFFFFFF); /*start */
	 startValue = BREG_Read32( in_channelHandle->moduleHandle->regHandle, BCHP_TIMER_TIMER1_STAT ) & 0x3FFFFFFF;
#endif

        /*for(;;),   about 20us per attemp, 20x50 = 1000 = 1ms */
	for (i=0; i< in_channelHandle->moduleHandle->currentSettings.ulTimeOutInMilSecs * 50 ;i++)
	{
		j = BREG_Read32(  in_channelHandle->moduleHandle->regHandle, 	BCHP_BSP_GLB_CONTROL_GLB_OLOAD2);

#if BHSM_DEBUG_POLLING	 /* as close as to the OLOAD2 reading moment*/
		endValue = BREG_Read32( in_channelHandle->moduleHandle->regHandle, BCHP_TIMER_TIMER1_STAT ) & 0x3FFFFFFF; 
#endif
		BDBG_MSG(( "////BREG_Read32(BCHP_BSP_GLB_CONTROL_GLB_OLOAD2) = %x ", j));
		if (	( j & BCHP_BSP_GLB_CONTROL_GLB_OLOAD2_CMD_OLOAD2_MASK) == 1  ) 
		{
			break;
		}
		BKNI_Delay(19);   /* 19 plus ~1*/

	
	}
#if BHSM_DEBUG_POLLING   /* buffer it to improve timing performance*/
	/* convert to units of us, slighter faster than the actual timing due to a timer1 late start*/
	in_channelHandle->pollingIntervalBuf[in_channelHandle->pollingBufIndex] = (uint16_t)((endValue - startValue )/27);  
	in_channelHandle->pollingBufIndex = (in_channelHandle->pollingBufIndex+1)%BHSM_DEBUG_POLLINGBUF_MAX;
	BREG_Write32(in_channelHandle->moduleHandle->regHandle, BCHP_TIMER_TIMER1_CTRL , 0x1FFFFFFF); /*stop */

#endif
	
        if (j==0)
        {
		BDBG_ERR(("BHSM_P_CommonSubmitCommand: Polling timeout error %d\n", in_channelHandle->moduleHandle->currentSettings.ulTimeOutInMilSecs));
		errCode = BHSM_STATUS_TIME_OUT;				
		goto BHSM_P_DONE_LABEL;							
        }
	  /* crash the system, no direct operation. *(volatile uint32_t *) 0x1030b020 =0;  */ /* clear the Output ready flag*/
	   BREG_Write32(  in_channelHandle->moduleHandle->regHandle,
						BCHP_BSP_GLB_CONTROL_GLB_OLOAD2,
						BREG_Read32(  in_channelHandle->moduleHandle->regHandle, 
										BCHP_BSP_GLB_CONTROL_GLB_OLOAD2) & 
										(~BCHP_BSP_GLB_CONTROL_GLB_OLOAD2_CMD_OLOAD2_MASK));		

 
}
else   
{

	if ( (errCode = BERR_TRACE(BKNI_WaitForEvent( in_channelHandle->oLoadWait, 
				in_channelHandle->moduleHandle->currentSettings.ulTimeOutInMilSecs))) != BERR_SUCCESS ) {

		BDBG_ERR(("BHSM_P_CommonSubmitCommand: BKNI_WaitForEvent timeout error %d\n", in_channelHandle->moduleHandle->currentSettings.ulTimeOutInMilSecs));
		errCode = BHSM_STATUS_TIME_OUT;				
		goto BHSM_P_DONE_LABEL;							
	}	
 }
	/*BKNI_Delay(100); */ /* 1) if no such delay, the following first Buffer2_Read is failed with a 'zero' reading ;
	                                   2) 50us is not long enough to solve this issue
	                                   3) 500us or 600us more is good to get the right data from Buffer2*/
	/* Read output buffer */
	/* Version Number */
	unVersionNum = BREG_Read32(  in_channelHandle->moduleHandle->regHandle, 
			in_channelHandle->ulOutCmdBufAddr + (BCMD_CommonBufferFields_eVerNum / 4) * 4);
#if 0
/*#if ! (((BCHP_CHIP==7401) &&  (BCHP_VER == BCHP_VER_B0)) || ((BCHP_CHIP==7400) &&  (BCHP_VER == BCHP_VER_A0)))*/
	/* Check Version Number */
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_VERSION_ERR, 
		((unVersionNum & BSP_S_CHECK_VERSION_MASK)  !=  ((BSP_S_DRV_VERSION) & BSP_S_CHECK_VERSION_MASK) ));	
#endif	
	/* Owner ID */
	unOwnerId = BREG_Read32(  in_channelHandle->moduleHandle->regHandle, 
			in_channelHandle->ulOutCmdBufAddr +  (BCMD_CommonBufferFields_eOwnerId / 4) * 4);

	/* Continuous Mode */
	unContMode = BREG_Read32(  in_channelHandle->moduleHandle->regHandle, 
			in_channelHandle->ulOutCmdBufAddr +  (BCMD_CommonBufferFields_eContMode / 4) * 4);

	/* Tag ID */
	unTagId = BREG_Read32(  in_channelHandle->moduleHandle->regHandle, 
			in_channelHandle->ulOutCmdBufAddr +  (BCMD_CommonBufferFields_eTagId / 4) * 4);    

	/* Check Tag ID */
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_PARM_LEN_ERR, 
		(unTagId !=  inoutp_commandData->cmdId ) );		

	/* Parameter Length */
	inoutp_commandData->unOutputParamLen = BREG_Read32(  in_channelHandle->moduleHandle->regHandle, 
			in_channelHandle->ulOutCmdBufAddr +   (BCMD_CommonBufferFields_eParamLenPlus1 / 4) * 4);      

	/* Check Parameter length */
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_PARM_LEN_ERR, 
		(inoutp_commandData->unOutputParamLen > BCMD_NON_HEADER_BUFFER_BYTE_SIZE ) );	

	/* Check if owner id still the same */
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_OWNER_ID_ERR, 
		(unOwnerId != in_channelHandle->ulSequenceNum ) );	

	/* Fill in specific output command parameters , +3 to resolve DH return in bits, e.g. 2018 bits 252bytes+2 bits*/
	for(i = 0; i< (inoutp_commandData->unOutputParamLen + 3)/4; i++) {
		
		inoutp_commandData->unOutputParamsBuf[i] =
			BREG_Read32( in_channelHandle->moduleHandle->regHandle, 
			in_channelHandle->ulOutCmdBufAddr+ ((BCMD_CommonBufferFields_eStatus / 4) * 4)  + i*4);  
		/*  strong debug to see what is actually copied from ISR buffer into HSM command buffer
		BDBG_MSG( ("i =%d, inoutp_commandData->unOutputParamsBuf[i]=%.8x",i, inoutp_commandData->unOutputParamsBuf[i]));
		*/
	}

	/* Dump out the output commands, +3 to dump the rest bits in the last word32 */
	{
		unsigned int i;
		BDBG_MSG(("MIPS <----------------------------------- BSP:"));  
		for(i = 0; i<(inoutp_commandData->unOutputParamLen+3+BCMD_HEADER_BYTE_SIZE)/4; i++) {
	        /* Data */
			BDBG_MSG(("\t\t\t%.3d - %08X",  i*4,
				BREG_Read32( in_channelHandle->moduleHandle->regHandle, 
					in_channelHandle->ulOutCmdBufAddr+ i*4)));  
		}
	}
	
	BDBG_MSG(("BHSM_P_CommonSubmitCommand output: unVersionNum = %08X, unOwnerId = %08X, unContMode = %x, unTagId = %08X, outp_unOutputParamLen = %08X\n", 
		unVersionNum, unOwnerId, unContMode, unTagId, inoutp_commandData->unOutputParamLen));

	/* Increment the sequence number by 1 */
	if ((++in_channelHandle->ulSequenceNum) >= 256)
		in_channelHandle->ulSequenceNum = 0;

	/* Clear Input command buffer */
	for(i = 0; i<BCMD_BUFFER_BYTE_SIZE/4; i++) {
		BREG_Write32( in_channelHandle->moduleHandle->regHandle, 
			in_channelHandle->ulInCmdBufAddr+ i*4, 0);  
	}	

BHSM_P_DONE_LABEL:

	/* Unlock mutex */
	if ( 	!(inoutp_commandData ->bLockWaive) ){    /* mandatory for VxWorks*/
		BKNI_ReleaseMutex( in_channelHandle->mutexLock);	
	}
	
	BDBG_LEAVE(BHSM_P_CommonSubmitCommand);
	BDBG_MSG(("Exit BHSM_P_CommonSubmitCommand\n"));	
	return( errCode );
}

#ifdef BHSM_AUTO_TEST

BERR_Code BHSM_P_CommonSubmitRawCommand (
		BHSM_ChannelHandle		in_channelHandle,	
		BHSM_P_CommandData_t	*inoutp_commandData
)
{
	BERR_Code errCode = BERR_SUCCESS;
	uint32_t	unVersionNum, unOwnerId, unContMode, unTagId, i, unIReady, j=0;
#if BHSM_DEBUG_POLLING
	 uint32_t  startValue, endValue;
#endif

	BDBG_MSG(("Inside BHSM_P_CommonSubmitCommandRaw\n"));	
	BDBG_ENTER(BHSM_P_CommonSubmitCommandRaw);
	BDBG_ASSERT( in_channelHandle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_channelHandle->ulMagicNumber != BHSM_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );	

	/* Lock mutex to synchronize the submit command operation */
	/* 
	if ( 	!(inoutp_commandData ->bLockWaive) )	*/	/* 10/05/05,Allen.C  */  /*
	      BHSM_P_CHECK_ERR_CODE_FUNC(errCode, BKNI_AcquireMutex( in_channelHandle->mutexLock)); */

	/* Check IReady , ulTimeOutInMilSecs_max = 120 000, multiplied by 500 is till less than FFFFFFFF, no overflow yet*/
	for (i=0; i< in_channelHandle->moduleHandle->currentSettings.ulTimeOutInMilSecs * 500 ;i++)
	{	
		unIReady = BREG_Read32( in_channelHandle->moduleHandle->regHandle, in_channelHandle->ulIReadyRegAddr);
		if (unIReady & in_channelHandle->ulIReadyVal) 
		{
			break;
		}	
		BKNI_Delay(2);   /*  2us per loop, still blocking so far. May give CPU chance to others if it's non-blocking on other platforms*/
	}
	if ( i ==  in_channelHandle->moduleHandle->currentSettings.ulTimeOutInMilSecs * 500)
	{
		 BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_IRDY_ERR, 1 );
	}

	/* Clear output command buffer */
	for(i = 0; i<BCMD_BUFFER_BYTE_SIZE/4; i++) {
		BREG_Write32( in_channelHandle->moduleHandle->regHandle, 
			in_channelHandle->ulOutCmdBufAddr+ i*4, 0);  
	}	

	/* Fill in specific input command parameters ( entire cmd including header section words in RawCmd), loop in words */
	for(i = 0; i< (inoutp_commandData->unInputParamLen/4); i++) {  /* this length including header or not? try 'yes' */
		BREG_Write32( in_channelHandle->moduleHandle->regHandle, 
			in_channelHandle->ulInCmdBufAddr+ i*4, 
			inoutp_commandData->unInputParamsBuf[i]);  /*  unInputParamsBuf[]  has 5 header-section-words, in RawCmd*/
	}

	/* Dump out the input commands */
	{
		unsigned int i;
		BDBG_MSG(("MIPS -----------------------------------> BSP:"));  
		for(i = 0; i<(inoutp_commandData->unInputParamLen+3)/4; i++) {
	        /* Data */
			BDBG_MSG(("\t\t\t%.3d - %08X ", i*4, BREG_Read32( in_channelHandle->moduleHandle->regHandle, in_channelHandle->ulInCmdBufAddr+ i*4)));  
		}
	}
if ((in_channelHandle->moduleHandle->currentSettings.uSpecialControl & BHSM_CTRLS_POLLINGORISR) ==  BHSM_CTRL_SET_POLLING)
{
	/* now to disable HSM ISR,  the following polling should dominate; 
          Note: this is only disable the channel2 HSM ISR.  channel1 HSM ISR  (high priority for Cancel cmd) is not existing for 3563
       */	

	/* moved to PI, once per setting change as Bryant suggested, which may reduce robustness
	BREG_Write32(in_channelHandle->moduleHandle->regHandle, BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_EN, 
							BREG_Read32(  in_channelHandle->moduleHandle->regHandle, 
										BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_EN) & 
										(~BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_STATUS_OLOAD2_INT_MASK));	 
	
       BREG_Write32(  in_channelHandle->moduleHandle->regHandle,
						BCHP_BSP_GLB_CONTROL_GLB_OLOAD2,
						BREG_Read32(  in_channelHandle->moduleHandle->regHandle, 
										BCHP_BSP_GLB_CONTROL_GLB_OLOAD2) & 
										(~BCHP_BSP_GLB_CONTROL_GLB_OLOAD2_CMD_OLOAD2_MASK));		
	*/

	BDBG_MSG(("////////// HSM in Polling,  ISR2 is disabled, %x, channel num=%d\n",
					in_channelHandle->moduleHandle->currentSettings.uSpecialControl,
					in_channelHandle->ucChannelNumber));
}else
{
	/*
	BREG_Write32(in_channelHandle->moduleHandle->regHandle, BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_EN, 
							BREG_Read32(  in_channelHandle->moduleHandle->regHandle, 	BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_EN) | 
										BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_STATUS_OLOAD2_INT_MASK);	 */
	BDBG_MSG(("//////////HSM in ISR,  ISR2 is enabled, %x, channel num=%d\n",
					in_channelHandle->moduleHandle->currentSettings.uSpecialControl,
					in_channelHandle->ucChannelNumber));
}


	/* Set ILOAD to signal BSP there's a command ready. */
	BDBG_MSG(("BHSM_P_SubmitCommand: ulILoadRegAddr = 0x%08lx, ulILoadVal = %ld\n", 
		in_channelHandle->ulILoadRegAddr, in_channelHandle->ulILoadVal));	
	BREG_Write32( in_channelHandle->moduleHandle->regHandle, 
			in_channelHandle->ulILoadRegAddr,			
			in_channelHandle->ulILoadVal);

	

/*
	as Bryant request for 3563 dual support, ISR  or Polling, runtime selection
 */


if ( (in_channelHandle->moduleHandle->currentSettings.uSpecialControl & BHSM_CTRLS_POLLINGORISR) == BHSM_CTRL_SET_POLLING)
{

#if BHSM_DEBUG_POLLING	 
	 BREG_Write32(in_channelHandle->moduleHandle->regHandle, BCHP_TIMER_TIMER1_CTRL , 0x9FFFFFFF); /*start */
	 startValue = BREG_Read32( in_channelHandle->moduleHandle->regHandle, BCHP_TIMER_TIMER1_STAT ) & 0x3FFFFFFF;
#endif

        /*for(;;),   about 20us per attemp, 20x50 = 1000 = 1ms */
	for (i=0; i< in_channelHandle->moduleHandle->currentSettings.ulTimeOutInMilSecs * 500 ;i++)
	{
		j = BREG_Read32(  in_channelHandle->moduleHandle->regHandle, 	BCHP_BSP_GLB_CONTROL_GLB_OLOAD2);

#if BHSM_DEBUG_POLLING	 /* as close as to the OLOAD2 reading moment*/
		endValue = BREG_Read32( in_channelHandle->moduleHandle->regHandle, BCHP_TIMER_TIMER1_STAT ) & 0x3FFFFFFF; 
#endif
		BDBG_MSG(( "////BREG_Read32(BCHP_BSP_GLB_CONTROL_GLB_OLOAD2) = %x ", j));
		if (	( j & BCHP_BSP_GLB_CONTROL_GLB_OLOAD2_CMD_OLOAD2_MASK) == 1  ) 
		{
			break;
		}
		BKNI_Delay(2);   /* 2 us per loop*/

	
	}
#if BHSM_DEBUG_POLLING   /* buffer it to improve timing performance*/
	/* convert to units of us, slighter faster than the actual timing due to a timer1 late start*/
	in_channelHandle->pollingIntervalBuf[in_channelHandle->pollingBufIndex] = (uint16_t)((endValue - startValue )/27);  
	in_channelHandle->pollingBufIndex = (in_channelHandle->pollingBufIndex+1)%BHSM_DEBUG_POLLINGBUF_MAX;
	BREG_Write32(in_channelHandle->moduleHandle->regHandle, BCHP_TIMER_TIMER1_CTRL , 0x1FFFFFFF); /*stop */

#endif
	
        if (j==0)
        {
		BDBG_ERR(("BHSM_P_CommonSubmitCommand: Polling timeout error %d\n", in_channelHandle->moduleHandle->currentSettings.ulTimeOutInMilSecs));
		errCode = BHSM_STATUS_TIME_OUT;				
		goto BHSM_P_DONE_LABEL;							
        }
	  /* crash the system, no direct operation. *(volatile uint32_t *) 0x1030b020 =0;  */ /* clear the Output ready flag*/
	   BREG_Write32(  in_channelHandle->moduleHandle->regHandle,
						BCHP_BSP_GLB_CONTROL_GLB_OLOAD2,
						BREG_Read32(  in_channelHandle->moduleHandle->regHandle, 
										BCHP_BSP_GLB_CONTROL_GLB_OLOAD2) & 
										(~BCHP_BSP_GLB_CONTROL_GLB_OLOAD2_CMD_OLOAD2_MASK));		

 
}
else   
{

	if ( (errCode = BERR_TRACE(BKNI_WaitForEvent( in_channelHandle->oLoadWait, 
				in_channelHandle->moduleHandle->currentSettings.ulTimeOutInMilSecs))) != BERR_SUCCESS ) {

		BDBG_ERR(("BHSM_P_CommonSubmitCommand: BKNI_WaitForEvent timeout error %d\n", in_channelHandle->moduleHandle->currentSettings.ulTimeOutInMilSecs));
		errCode = BHSM_STATUS_TIME_OUT;				
		goto BHSM_P_DONE_LABEL;							
	}	
 }
	/*BKNI_Delay(100); to resolve on 2/9/2007 */ /* 1) if no such delay, the following first Buffer2_Read is failed with a 'zero' reading ;
	                                   2) 50us is not long enough to solve this issue
	                                   3) 500us or 600us more is good to get the right data from Buffer2*/
	/* Read output buffer */
	/* Version Number */
	unVersionNum = BREG_Read32(  in_channelHandle->moduleHandle->regHandle, 
			in_channelHandle->ulOutCmdBufAddr + (BCMD_CommonBufferFields_eVerNum / 4) * 4);


	/* Owner ID */
	unOwnerId = BREG_Read32(  in_channelHandle->moduleHandle->regHandle, 
			in_channelHandle->ulOutCmdBufAddr +  (BCMD_CommonBufferFields_eOwnerId / 4) * 4);

	/* Continuous Mode */
	unContMode = BREG_Read32(  in_channelHandle->moduleHandle->regHandle, 
			in_channelHandle->ulOutCmdBufAddr +  (BCMD_CommonBufferFields_eContMode / 4) * 4);

	/* Tag ID */
	unTagId = BREG_Read32(  in_channelHandle->moduleHandle->regHandle, 
			in_channelHandle->ulOutCmdBufAddr +  (BCMD_CommonBufferFields_eTagId / 4) * 4);    

	/* Parameter Length, including header section, translated in bytes */
	inoutp_commandData->unOutputParamLen = BCMD_HEADER_BYTE_SIZE + BREG_Read32(  in_channelHandle->moduleHandle->regHandle, 
			in_channelHandle->ulOutCmdBufAddr +   (BCMD_CommonBufferFields_eParamLenPlus1 / 4) * 4);      

	/* Check Parameter length */
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_PARM_LEN_ERR, 
		(inoutp_commandData->unOutputParamLen > BCMD_BUFFER_BYTE_SIZE ) );	


	/* Fill in specific output command parameters, loop in words */
	for(i = 0; i<(inoutp_commandData->unOutputParamLen + 3)/4; i++) {
		
		inoutp_commandData->unOutputParamsBuf[i] =
			BREG_Read32( in_channelHandle->moduleHandle->regHandle, 	in_channelHandle->ulOutCmdBufAddr  + i*4);  
	}

	/* Dump out the output commands */
	{
		unsigned int i;
                 BDBG_MSG(("MIPS <----------------------------------- BSP:"));  
		for(i = 0; i<(inoutp_commandData->unOutputParamLen+3)/4; i++) {
	        /* Data */
			BDBG_MSG(("\t\t\t%.3d - %08X",  i*4,
				BREG_Read32( in_channelHandle->moduleHandle->regHandle, 
					in_channelHandle->ulOutCmdBufAddr+ i*4)));  
		}
	}



	/* Clear Input command buffer */
	for(i = 0; i<BCMD_BUFFER_BYTE_SIZE/4; i++) {
		BREG_Write32( in_channelHandle->moduleHandle->regHandle, 
			in_channelHandle->ulInCmdBufAddr+ i*4, 0);  
	}	

BHSM_P_DONE_LABEL:

	/* Unlock mutex */
	/*BKNI_ReleaseMutex( in_channelHandle->mutexLock);		*/
	
	BDBG_LEAVE(BHSM_P_CommonSubmitCommandRaw);
	BDBG_MSG(("Exit BHSM_P_CommonSubmitCommandRaw, errCode = 0x%0x\n", errCode));	
	return( errCode );
}

#endif

#if BHSM_DEBUG_POLLING
BERR_Code BHSM_TEST_PollingDebugInfor( BHSM_P_Handle    *  hHsm )
{
	  int 	i;
	  
	  BDBG_WRN(("=======HSM Polling Interval Data (us)========"));
	 for (i=0; i<BHSM_DEBUG_POLLINGBUF_MAX; i+=8 )
 	{
			  /*if ( ( i%16 ==0) && (i !=0)) BDBG_MSG(( "\n"));*/
	 		  BDBG_WRN((" %4d %4d %4d %4d %4d %4d %4d %4d", 
	 		  		hHsm->channelHandles[BHSM_HwModule_eCmdInterface2]->pollingIntervalBuf[i],
	 		  		hHsm->channelHandles[BHSM_HwModule_eCmdInterface2]->pollingIntervalBuf[i+1],
	 		  		hHsm->channelHandles[BHSM_HwModule_eCmdInterface2]->pollingIntervalBuf[i+2],
	 		  		hHsm->channelHandles[BHSM_HwModule_eCmdInterface2]->pollingIntervalBuf[i+3],
	 		  		hHsm->channelHandles[BHSM_HwModule_eCmdInterface2]->pollingIntervalBuf[i+4],
	 		  		hHsm->channelHandles[BHSM_HwModule_eCmdInterface2]->pollingIntervalBuf[i+5],
	 		  		hHsm->channelHandles[BHSM_HwModule_eCmdInterface2]->pollingIntervalBuf[i+6],
	 		  		hHsm->channelHandles[BHSM_HwModule_eCmdInterface2]->pollingIntervalBuf[i+7]
	 		  		));
 	}

	  BDBG_WRN(("=======Polling Interval Location: %d ========\n", 
	  	   hHsm->channelHandles[BHSM_HwModule_eCmdInterface2]->pollingBufIndex));	  
	  return (BERR_SUCCESS);
}

#endif

