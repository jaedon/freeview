/***************************************************************************
 *     Copyright (c) 2005-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhsm_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 8/28/09 1:03a $
 *
 * Module Description: This file contains Broadcom Host Secure Module (OS/platform 
 *                     independent) porting interface public functions.
 *                    			                    
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/3548/b3/common/bhsm_priv.c $
 * 
 * Hydra_Software_Devel/1   8/28/09 1:03a atruong
 * SW3548-2422: Add HSM PI Support for 3548B3
 * 
 * Hydra_Software_Devel/36   6/9/09 2:28p yili
 * PR42660:minor cleanup.
 * 
 * Hydra_Software_Devel/35   4/14/09 11:06a atruong
 * PR54104: HSM PI Support for 35130A0
 * 
 * Hydra_Software_Devel/34   4/14/09 7:49a atruong
 * PR54104: HSM PI Support for 35130A0
 * 
 * Hydra_Software_Devel/33   2/19/09 5:02p yili
 * PR42660:IPTV Integration
 * 
 * Hydra_Software_Devel/32   4/29/08 1:46p atruong
 * PR42227: New BSP Command Header format for BCMxxxx reference platforms
 * 
 * Hydra_Software_Devel/31   4/9/08 10:53a atruong
 * PR38948: HSM PI support for BCM3548
 * PR39464: HSM PI support for BCM3556
 * 
 * Hydra_Software_Devel/30   3/6/08 2:38p yili
 * PR40357:HSM interrupt handler improvement
 * 
 * Hydra_Software_Devel/29   6/21/07 5:46p lyang
 * PR 28967: 7401C3, 3563C0 7403A1, 7440b0 no-os HSM support; remove
 * makefiles hardcoding
 * 
 * Hydra_Software_Devel/28   5/31/07 5:04p lyang
 * PR 28967: 7403A1, 7440b0 no-os HSM support; remove makefiles hardcoding
 * 
 * Hydra_Software_Devel/27   5/31/07 2:04p lyang
 * PR 28967: 7403A1, 7440b0 no-os HSM support; remove makefiles hardcoding
 * 
 * Hydra_Software_Devel/26   5/25/07 10:19a lyang
 * PR 27711: Add 7400B0, 7405A0 support to HSM. Purge and shared *.h
 * source udpated
 * 
 * Hydra_Software_Devel/25   5/9/07 10:33a lyang
 * PR 30454: HSM update for SOAP test support in SubmitRawCommand
 * 
 * Hydra_Software_Devel/24   5/9/07 10:03a lyang
 * PR 30454: HSM update for SOAP test support in SubmitRawCommand
 * 
 * Hydra_Software_Devel/20   4/5/07 11:51a lyang
 * PR 28969: for HPC20 project, no bkni_delay(), Can not get XPT_PB0 TS
 * data sent to remux module on 7401 C1/C0 (B0 works fine)
 * 
 * Hydra_Software_Devel/19   3/22/07 10:47p lyang
 * PR 28967: to add HSM support for 7440B0 Non-OS
 * 
 * Hydra_Software_Devel/18   3/1/07 10:45p lyang
 * PR 27378: HSM keyloading during DMA action,m2m hw patch
 * 
 * Hydra_Software_Devel/17   3/1/07 10:09p lyang
 * PR 27378: HSM keyloading during DMA action,m2m hw patch
 * 
 * Hydra_Software_Devel/16   2/28/07 8:25p lyang
 * PR 28247: HSM has the recovery issue when CTRL-C is pressed,
 * BKNI_WaitForEvent() returns OS_ERR
 * 
 * Hydra_Software_Devel/15   2/28/07 7:01p lyang
 * PR 26218: HSM: Need to add support for 7400 B0
 * 
 * Hydra_Software_Devel/14   2/22/07 6:59p lyang
 * PR 26218: HSM: Need to add support for 7400 B0
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

/* To support new BSP command header format */
/* starting with BCM97405B0                 */
#define CMODE_PADDING_MSK	0x000000FF
#define CMODE_PADDING		0xABCDEF00
#define CTAGID_PADDING_MSK	0x000000FF
#define CTAGID_PADDING		0x0055AA00
#define CTAGID_SHFT		24
#define CPLEN_PADDING_MSK	0x0000FFFF
#define CPLEN_PADDING		0x789A0000



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
		if (moduleHandle->channelHandles[BHSM_HwModule_eCmdInterface1]->bIsOpen == true) {
			moduleHandle->channelHandles[BHSM_HwModule_eCmdInterface1]->oLoadSet = 1;
			BKNI_SetEvent( 
				moduleHandle->channelHandles[BHSM_HwModule_eCmdInterface1]->oLoadWait);
		}   	

		
	}

#if (BCHP_CHIP==7401) &&  (BCHP_VER == BCHP_VER_A0)
	if ( (unCpuIntrStatus & BCHP_BSP_GLB_NONSECURE_GLB_HOST_INTR_STATUS_OLOAD2_INT_MASK) ) 
#else
	if ( (unCpuIntrStatus & BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_STATUS_OLOAD2_INT_MASK) ) 
#endif
	{
		
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
		if (moduleHandle->channelHandles[BHSM_HwModule_eCmdInterface2]->bIsOpen == true) {
				
			moduleHandle->channelHandles[BHSM_HwModule_eCmdInterface2]->oLoadSet = 1;
			BKNI_SetEvent( 
				moduleHandle->channelHandles[BHSM_HwModule_eCmdInterface2]->oLoadWait);
		}   	

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
	uint32_t  inVal;

#if BHSM_DEBUG_POLLING
	 uint32_t  startValue, endValue=0;
#endif
	BDBG_MSG(("Inside BHSM_P_CommonSubmitCommand\n"));	
	BDBG_ENTER(BHSM_P_CommonSubmitCommand);
	BDBG_ASSERT( in_channelHandle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_channelHandle->ulMagicNumber != BHSM_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );	

	/* Lock mutex to synchronize the submit command operation */

#if (BCHP_CHIP != 3563) && (BCHP_CHIP != 3548) && (BCHP_CHIP != 3556) && (BCHP_CHIP != 35130)
	if ( 	!(inoutp_commandData ->bLockWaive) )		/* 10/05/05,Allen.C  */
	      BHSM_P_CHECK_ERR_CODE_FUNC(errCode, BKNI_AcquireMutex( in_channelHandle->mutexLock));
#endif

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
#if (BHSM_IPTV ==1) 
	for(i = 1; i<BCMD_BUFFER_BYTE_SIZE/4; i++) { /* Keep the version */
#else
	for(i = 0; i<BCMD_BUFFER_BYTE_SIZE/4; i++) {
#endif
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
	inVal = (inoutp_commandData->unContMode & CMODE_PADDING_MSK) | CMODE_PADDING;
	BREG_Write32(  in_channelHandle->moduleHandle->regHandle, 
		in_channelHandle->ulInCmdBufAddr + (BCMD_CommonBufferFields_eContMode / 4) * 4, 
		inVal);	

	/* Fill in Tag ID */
	inVal = (inoutp_commandData->cmdId & CTAGID_PADDING_MSK) |
	        ((~(inoutp_commandData->cmdId)) << CTAGID_SHFT) |
	        CTAGID_PADDING;
	BREG_Write32(  in_channelHandle->moduleHandle->regHandle, 
		in_channelHandle->ulInCmdBufAddr + (BCMD_CommonBufferFields_eTagId / 4) * 4, 
		inVal ); 		

	/* Parameter Length */
	inVal = (inoutp_commandData->unInputParamLen & CPLEN_PADDING_MSK) |
	        CPLEN_PADDING;
	BREG_Write32(  in_channelHandle->moduleHandle->regHandle, 
		in_channelHandle->ulInCmdBufAddr + (BCMD_CommonBufferFields_eParamLenPlus1 / 4) * 4, 
		inVal);

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

#if (BCHP_CHIP != 3563) && (BCHP_CHIP != 3548) && (BCHP_CHIP != 3556) && (BCHP_CHIP != 35130)
        BKNI_ResetEvent(in_channelHandle->oLoadWait);		/* CTRL-C or any system level signal*/
#endif
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

        /*for(;;),   about 20us per attemp, 2x500 = 1000 = 1ms */
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
		BKNI_Delay(2);   /* to 2 from 19 plus ~1*/

	
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

		/* CTRL-C issue: according to Min requirement, App will loop to free the key slot, one slot each call ,
			App wants to continue anyway, assume CTRL-C caused an extra Event.
			Side effect: the timeout duration is doubled at worst case. Timeout normally won't happen. 2/26/07
		*/

		if (errCode == BERR_OS_ERROR)
		{
			/*  1) use BKNI_Delay(), polling for a non-OS return, with a max of tolerated time of ulTimeOutInMilSecs 
					blocking with 1us (seems ok), 1us-pe-waiting which will be asynced
 			      2) use BKNI_Sleep(),returns OS_ERROR also; then we have to mark the OS event in ISR handler 
			*/
			for (i=0; i<in_channelHandle->moduleHandle->currentSettings.ulTimeOutInMilSecs*1000; i++)				
			{			      	     
				BKNI_Delay(1);         

				if (in_channelHandle->oLoadSet)  /*   ISR action must be checked before waitforevent */
				{
					errCode = BERR_SUCCESS; 		/*  assume it's good if ISR has set OLoad*/
					break;		
				}
#if 0				
				errCode = BERR_TRACE(BKNI_WaitForEvent( in_channelHandle->oLoadWait, 
										in_channelHandle->moduleHandle->currentSettings.ulTimeOutInMilSecs));	
				if (errCode != BERR_OS_ERROR)
				{
					break;	/*  stop the loop to continue the cmd parsing, for TIMEOUT or SUCCESS */
				}
#endif
			}
		}
		
		if(i== in_channelHandle->moduleHandle->currentSettings.ulTimeOutInMilSecs*1000)
		{
			errCode = BHSM_STATUS_TIME_OUT;   /* CTRL-C, OS_ERROR stays all the time in current app cycle */
		}
		
		/* by here, should not be an OS event*/
		if (errCode != BERR_SUCCESS)
		{
			BDBG_ERR(("BHSM-SubmitCommand:  Error Code= %x returned from BKNI_WaitForEvent \n", errCode));					
			/*errCode = BHSM_STATUS_TIME_OUT;	 no enforement */
			goto BHSM_P_DONE_LABEL;							
		}
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

	unTagId &= CTAGID_PADDING_MSK;
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
#if (BCHP_CHIP != 3563) && (BCHP_CHIP != 3548) && (BCHP_CHIP != 3556) && (BCHP_CHIP != 35130)
	if ( 	!(inoutp_commandData ->bLockWaive) ){    /* mandatory for VxWorks*/
		BKNI_ReleaseMutex( in_channelHandle->mutexLock);	
	}
#endif
	
	in_channelHandle->oLoadSet=0;		/* avoid the leftover from a good HSM comand into a CTRL-C interrupted HSM cmd*/
	
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
	uint32_t	inVal;
#if BHSM_DEBUG_POLLING
	 uint32_t  startValue, endValue=0;
#endif

	BDBG_MSG(("Inside BHSM_P_CommonSubmitRawCommand\n"));	
	BDBG_ENTER(BHSM_P_CommonSubmitRawCommand);
	BDBG_ASSERT( in_channelHandle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_channelHandle->ulMagicNumber != BHSM_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );	

	/* Lock mutex to synchronize the submit command operation */
#if (BCHP_CHIP != 3563) && (BCHP_CHIP != 3548) && (BCHP_CHIP != 3556) && (BCHP_CHIP != 35130)
	if ( 	!(inoutp_commandData ->bLockWaive) )		/* 10/05/05,Allen.C  */
	      BHSM_P_CHECK_ERR_CODE_FUNC(errCode, BKNI_AcquireMutex( in_channelHandle->mutexLock));
#endif

	/* Check IReady */
	unIReady = BREG_Read32( in_channelHandle->moduleHandle->regHandle, 
							in_channelHandle->ulIReadyRegAddr);

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
		switch (i)
		{	
			case 2 : /* Continual Mode */
				inVal = ((inoutp_commandData->unInputParamsBuf[i]) & CMODE_PADDING_MSK) | CMODE_PADDING;
				break;
			case 3 : /* Command Tag ID */
				inVal = ((inoutp_commandData->unInputParamsBuf[i]) & CTAGID_PADDING_MSK) | ((~(inoutp_commandData->unInputParamsBuf[i])) << CTAGID_SHFT) |CTAGID_PADDING;
				break;
			case 4 : /* Command Parameter Length */
				inVal = ((inoutp_commandData->unInputParamsBuf[i]) & CPLEN_PADDING_MSK) | CPLEN_PADDING;
				break;
			default:
				inVal = inoutp_commandData->unInputParamsBuf[i];

				 	
		}
		BREG_Write32( in_channelHandle->moduleHandle->regHandle, 
			in_channelHandle->ulInCmdBufAddr+ i*4, 
			inVal);  /*  unInputParamsBuf[]  has 5 header-section-words, in RawCmd*/
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
	BDBG_MSG(("////////// HSM in Polling,  ISR2 is disabled, %x, channel num=%d\n",
					in_channelHandle->moduleHandle->currentSettings.uSpecialControl,
					in_channelHandle->ucChannelNumber));
}else
{

        BKNI_ResetEvent(in_channelHandle->oLoadWait);		/* CTRL-C or any system level signal*/
	BDBG_MSG(("//////////HSM in ISR,  ISR2 is enabled, %x, channel num=%d\n",
					in_channelHandle->moduleHandle->currentSettings.uSpecialControl,
					in_channelHandle->ucChannelNumber));
}


	/* Set ILOAD to signal BSP there's a command ready. */
	BDBG_MSG(("BHSM_P_CommonSubmitRawCommand: ulILoadRegAddr = 0x%08lx, ulILoadVal = %ld\n", 
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

        /*for(;;),   about 20us per attemp, 2x500 = 1000 = 1ms */
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
		BKNI_Delay(2);   /* to 2 from 19 plus ~1*/

	
	}
#if BHSM_DEBUG_POLLING   /* buffer it to improve timing performance*/
	/* convert to units of us, slighter faster than the actual timing due to a timer1 late start*/
	in_channelHandle->pollingIntervalBuf[in_channelHandle->pollingBufIndex] = (uint16_t)((endValue - startValue )/27);  
	in_channelHandle->pollingBufIndex = (in_channelHandle->pollingBufIndex+1)%BHSM_DEBUG_POLLINGBUF_MAX;
	BREG_Write32(in_channelHandle->moduleHandle->regHandle, BCHP_TIMER_TIMER1_CTRL , 0x1FFFFFFF); /*stop */

#endif
	
        if (j==0)
        {
		BDBG_ERR(("BHSM_P_CommonSubmitRawCommand: Polling timeout error %d\n", in_channelHandle->moduleHandle->currentSettings.ulTimeOutInMilSecs));
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

		/* CTRL-C issue: according to Min requirement, App will loop to free the key slot, one slot each call ,
			App wants to continue anyway, assume CTRL-C caused an extra Event.
			Side effect: the timeout duration is doubled at worst case. Timeout normally won't happen. 2/26/07
		*/

		if (errCode == BERR_OS_ERROR)
		{
			/*  1) use BKNI_Delay(), polling for a non-OS return, with a max of tolerated time of ulTimeOutInMilSecs 
					blocking with 1us (seems ok), 1us-pe-waiting which will be asynced
 			      2) use BKNI_Sleep(),returns OS_ERROR also; then we have to mark the OS event in ISR handler 
			*/
			for (i=0; i<in_channelHandle->moduleHandle->currentSettings.ulTimeOutInMilSecs*1000; i++)				
			{			      	     
				BKNI_Delay(1);         

				if (in_channelHandle->oLoadSet)  /*   ISR action must be checked before waitforevent */
				{
					errCode = BERR_SUCCESS; 		/*  assume it's good if ISR has set OLoad*/
					break;		
				}
#if 0				
				errCode = BERR_TRACE(BKNI_WaitForEvent( in_channelHandle->oLoadWait, 
										in_channelHandle->moduleHandle->currentSettings.ulTimeOutInMilSecs));	
				if (errCode != BERR_OS_ERROR)
				{
					break;	/*  stop the loop to continue the cmd parsing, for TIMEOUT or SUCCESS */
				}
#endif
			}
		}
		
		if(i== in_channelHandle->moduleHandle->currentSettings.ulTimeOutInMilSecs*1000)
		{
			errCode = BHSM_STATUS_TIME_OUT;   /* CTRL-C, OS_ERROR stays all the time in current app cycle */
		}
		
		/* by here, should not be an OS event*/
		if (errCode != BERR_SUCCESS)
		{
			BDBG_ERR(("BHSM_P_CommonSubmitRawCommand:  Error Code= %x returned from BKNI_WaitForEvent \n", errCode));					
			/*errCode = BHSM_STATUS_TIME_OUT;	 no enforement */
			goto BHSM_P_DONE_LABEL;							
		}
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
	unTagId &= CTAGID_PADDING_MSK;

	/* Check Tag ID, not for no-os */
	/*	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_PARM_LEN_ERR, 
		(unTagId !=  inoutp_commandData->cmdId ) );		
	*/
	/* Parameter Length */
	inoutp_commandData->unOutputParamLen = BCMD_HEADER_BYTE_SIZE +BREG_Read32(  in_channelHandle->moduleHandle->regHandle, 
			in_channelHandle->ulOutCmdBufAddr +   (BCMD_CommonBufferFields_eParamLenPlus1 / 4) * 4);      

	/* Check Parameter length */
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_PARM_LEN_ERR, 
		(inoutp_commandData->unOutputParamLen > BCMD_BUFFER_BYTE_SIZE ) );	

	/* Check if owner id still the same */
	/*	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_OWNER_ID_ERR, 
		(unOwnerId != in_channelHandle->ulSequenceNum ) );	
	*/

	/* Fill in specific output command parameters , +3 to resolve DH return in bits, e.g. 2018 bits 252bytes+2 bits*/
	for(i = 0; i< (inoutp_commandData->unOutputParamLen + 3)/4; i++) {
		
		inoutp_commandData->unOutputParamsBuf[i] =
			BREG_Read32( in_channelHandle->moduleHandle->regHandle, 
			in_channelHandle->ulOutCmdBufAddr+  i*4);   
		/*  strong debug to see what is actually copied from ISR buffer into HSM command buffer
		BDBG_MSG( ("i =%d, inoutp_commandData->unOutputParamsBuf[i]=%.8x",i, inoutp_commandData->unOutputParamsBuf[i]));
		*/
	}

	/* Dump out the output commands, +3 to dump the rest bits in the last word32 */
	{
		unsigned int i;
		BDBG_MSG(("MIPS <----------------------------------- BSP:"));  
		for(i = 0; i<(inoutp_commandData->unOutputParamLen+3)/4; i++) {
	        /* Data */
			BDBG_MSG(("\t\t\t%.3d - %08X\n",  i*4, 
						inoutp_commandData->unOutputParamsBuf[i]));
			/*
				BREG_Read32( in_channelHandle->moduleHandle->regHandle, 
					in_channelHandle->ulOutCmdBufAddr+ i*4)));  */
		}
	}
	
	BDBG_MSG(("BHSM_P_CommonSubmitRawCommand output: unVersionNum = %08X, unOwnerId = %08X, unContMode = %x, unTagId = %08X, outp_unOutputParamLen = %08X\n", 
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
#if (BCHP_CHIP != 3563) && (BCHP_CHIP != 3548) && (BCHP_CHIP != 3556) && (BCHP_CHIP != 35130)
	if ( 	!(inoutp_commandData ->bLockWaive) ){    /* mandatory for VxWorks*/
		BKNI_ReleaseMutex( in_channelHandle->mutexLock);	
	}
#endif
	
	in_channelHandle->oLoadSet=0;		/* avoid the leftover from a good HSM comand into a CTRL-C interrupted HSM cmd*/
	
	BDBG_LEAVE(BHSM_P_CommonSubmitRawCommand);
	BDBG_MSG(("Exit BHSM_P_CommonSubmitRawCommand\n"));	
	return( errCode );
}
#endif

#if BHSM_PATCH_MEMC_DMA			/* added to reduce footprint for other chip/versions without patch*/

BERR_Code BHSM_P_CommonSubmitCommand_DMA (
		BHSM_ChannelHandle		in_channelHandle,	
		BHSM_P_CommandData_t	*inoutp_commandData
)
{
	BERR_Code errCode = BERR_SUCCESS;
	uint32_t	unVersionNum, unOwnerId, unContMode, unTagId, i, unIReady, j=0;
#if BHSM_DEBUG_POLLING
	 uint32_t  startValue, endValue=0;
#endif

	uint32_t  inVal;

#if BHSM_PATCH_MEMC_DMA
	uint32_t			memcBackup=0;
#endif


	BDBG_MSG(("Inside BHSM_P_CommonSubmitCommand_DMA\n"));	
	BDBG_ENTER(BHSM_P_CommonSubmitCommand_DMA);
	BDBG_ASSERT( in_channelHandle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_channelHandle->ulMagicNumber != BHSM_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );	

	/* Lock mutex to synchronize the submit command operation */
#if (BCHP_CHIP != 3563) && (BCHP_CHIP != 3548) && (BCHP_CHIP != 3556) && (BCHP_CHIP != 35130)
	if ( 	!(inoutp_commandData ->bLockWaive) )		/* 10/05/05,Allen.C  */
	      BHSM_P_CHECK_ERR_CODE_FUNC(errCode, BKNI_AcquireMutex( in_channelHandle->mutexLock));
#endif

#if BHSM_PATCH_MEMC_DMA
    	memcBackup = BREG_Read32( in_channelHandle->moduleHandle->regHandle, 	BHSM_PATCH_MEMC_DMA_REG );
	BREG_Write32( in_channelHandle->moduleHandle->regHandle,BHSM_PATCH_MEMC_DMA_REG,BHSM_PATCH_MEMC_DMA_BOMASK);

#if BKNI_ACQUIREMUTEX_LOCK_TRUST
	i = (BHSM_PATCH_MEMC_DMA_WAITING*2+500)/1000;   /* 1.5ms will be treated as 2ms */
	BKNI_Sleep(i);  
#else
	for (i=0; i<  BHSM_PATCH_MEMC_DMA_WAITING ;i++)   /* pls adjust the macro value to change your actual timing*/
	{	
		BKNI_Delay(2);  
	}
#endif 

#endif


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
	inVal = (inoutp_commandData->unContMode & CMODE_PADDING_MSK) | CMODE_PADDING;
	BREG_Write32(  in_channelHandle->moduleHandle->regHandle, 
		in_channelHandle->ulInCmdBufAddr + (BCMD_CommonBufferFields_eContMode / 4) * 4, 
		inVal);	

	/* Fill in Tag ID */
	inVal = (inoutp_commandData->cmdId & CTAGID_PADDING_MSK) |
	        ((~(inoutp_commandData->cmdId)) << CTAGID_SHFT) |
	        CTAGID_PADDING;
	BREG_Write32(  in_channelHandle->moduleHandle->regHandle, 
		in_channelHandle->ulInCmdBufAddr + (BCMD_CommonBufferFields_eTagId / 4) * 4, 
		inVal ); 		

	/* Parameter Length */
	inVal = (inoutp_commandData->unInputParamLen & CPLEN_PADDING_MSK) |
	        CPLEN_PADDING;
	BREG_Write32(  in_channelHandle->moduleHandle->regHandle, 
		in_channelHandle->ulInCmdBufAddr + (BCMD_CommonBufferFields_eParamLenPlus1 / 4) * 4, 
		inVal);

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
	BDBG_MSG(("////////// HSM in Polling,  ISR2 is disabled, %x, channel num=%d\n",
					in_channelHandle->moduleHandle->currentSettings.uSpecialControl,
					in_channelHandle->ucChannelNumber));
}else
{

        BKNI_ResetEvent(in_channelHandle->oLoadWait);		/* CTRL-C or any system level signal*/
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

        /*for(;;),   about 20us per attemp, 2x500 = 1000 = 1ms */
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
		BKNI_Delay(2);   /* to 2 from 19 plus ~1*/

	
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

		/* CTRL-C issue: according to Min requirement, App will loop to free the key slot, one slot each call ,
			App wants to continue anyway, assume CTRL-C caused an extra Event.
			Side effect: the timeout duration is doubled at worst case. Timeout normally won't happen. 2/26/07
		*/

		if (errCode == BERR_OS_ERROR)
		{
			/*  1) use BKNI_Delay(), polling for a non-OS return, with a max of tolerated time of ulTimeOutInMilSecs 
					blocking with 1us (seems ok), 1us-pe-waiting which will be asynced
 			      2) use BKNI_Sleep(),returns OS_ERROR also; then we have to mark the OS event in ISR handler 
			*/
			for (i=0; i<in_channelHandle->moduleHandle->currentSettings.ulTimeOutInMilSecs*1000; i++)				
			{			      	     
				BKNI_Delay(1);         

				if (in_channelHandle->oLoadSet)  /*   ISR action must be checked before waitforevent */
				{
					errCode = BERR_SUCCESS; 		/*  assume it's good if ISR has set OLoad*/
					break;		
				}
#if 0				
				errCode = BERR_TRACE(BKNI_WaitForEvent( in_channelHandle->oLoadWait, 
										in_channelHandle->moduleHandle->currentSettings.ulTimeOutInMilSecs));	
				if (errCode != BERR_OS_ERROR)
				{
					break;	/*  stop the loop to continue the cmd parsing, for TIMEOUT or SUCCESS */
				}
#endif
			}
		}
		
		if(i== in_channelHandle->moduleHandle->currentSettings.ulTimeOutInMilSecs*1000)
		{
			errCode = BHSM_STATUS_TIME_OUT;   /* CTRL-C, OS_ERROR stays all the time in current app cycle */
		}
		
		/* by here, should not be an OS event*/
		if (errCode != BERR_SUCCESS)
		{
			BDBG_ERR(("BHSM-SubmitCommand:  Error Code= %x returned from BKNI_WaitForEvent \n", errCode));					
			/*errCode = BHSM_STATUS_TIME_OUT;	 no enforement */
			goto BHSM_P_DONE_LABEL;							
		}
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

	unTagId &= CTAGID_PADDING_MSK;
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

#if BHSM_PATCH_MEMC_DMA

#if BKNI_ACQUIREMUTEX_LOCK_TRUST
	i = (BHSM_PATCH_MEMC_DMA_WAITING*2+500)/1000;   /* 1.5ms will be treated as 2ms */
	BKNI_Sleep(i);  
#else
	for (i=0; i<  BHSM_PATCH_MEMC_DMA_WAITING ;i++)   /* pls adjust the macro value to change your actual timing*/
	{	
		BKNI_Delay(2);  
	}
#endif 
		
	BREG_Write32( in_channelHandle->moduleHandle->regHandle, 	BHSM_PATCH_MEMC_DMA_REG, memcBackup );	
#endif

	/* Unlock mutex */
#if (BCHP_CHIP != 3563) && (BCHP_CHIP != 3548) && (BCHP_CHIP != 3556) && (BCHP_CHIP != 35130)
	if ( 	!(inoutp_commandData ->bLockWaive) ){    /* mandatory for VxWorks*/
		BKNI_ReleaseMutex( in_channelHandle->mutexLock);	
	}
#endif

	in_channelHandle->oLoadSet=0;		/* avoid the leftover from a good HSM comand into a CTRL-C interrupted HSM cmd*/
	
	BDBG_LEAVE(BHSM_P_CommonSubmitCommand_DMA);
	BDBG_MSG(("Exit BHSM_P_CommonSubmitCommand_DMA\n"));	
	return( errCode );
}

#endif

#ifdef BHSM_AUTO_TEST

BERR_Code BHSM_P_CommonSubmitRawCommand_DMA (
		BHSM_ChannelHandle		in_channelHandle,	
		BHSM_P_CommandData_t	*inoutp_commandData
)
{
	BERR_Code errCode = BERR_SUCCESS;
	uint32_t	unVersionNum, unOwnerId, unContMode, unTagId, i, unIReady;
	uint32_t	inVal;

#if BHSM_PATCH_MEMC_DMA
	uint32_t			memcBackup=0;
#endif


	BDBG_MSG(("Inside BHSM_P_CommonSubmitRawCommand_DMA\n"));	
	BDBG_ENTER(BHSM_P_CommonSubmitRawCommand_DMA);
	BDBG_ASSERT( in_channelHandle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_channelHandle->ulMagicNumber != BHSM_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );	



	/* Lock mutex to synchronize the submit command operation */
#if (BCHP_CHIP != 3563) && (BCHP_CHIP != 3548) && (BCHP_CHIP != 3556) && (BCHP_CHIP != 35130)
	if ( 	!(inoutp_commandData ->bLockWaive) )		/* 10/05/05,Allen.C  */
	      BHSM_P_CHECK_ERR_CODE_FUNC(errCode, BKNI_AcquireMutex( in_channelHandle->mutexLock));
#endif

#if BHSM_PATCH_MEMC_DMA
	memcBackup = BREG_Read32( in_channelHandle->moduleHandle->regHandle, 	BHSM_PATCH_MEMC_DMA_REG );
	BREG_Write32( in_channelHandle->moduleHandle->regHandle,BHSM_PATCH_MEMC_DMA_REG,BHSM_PATCH_MEMC_DMA_BOMASK);
	
#if BKNI_ACQUIREMUTEX_LOCK_TRUST
	i = (BHSM_PATCH_MEMC_DMA_WAITING*2+500)/1000;   /* 1.5ms will be treated as 2ms */
	BKNI_Sleep(i);  
#else
	for (i=0; i<  BHSM_PATCH_MEMC_DMA_WAITING ;i++)   /* pls adjust the macro value to change your actual timing*/
	{	
		BKNI_Delay(2);  
	}
#endif 

#endif

	/* Check IReady */

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

	/* Fill in specific input command parameters */
	for(i = 0; i<inoutp_commandData->unInputParamLen/4; i++) {
                switch (i)
                {
                        case 2 : /* Continual Mode */
                                inVal = ((inoutp_commandData->unInputParamsBuf[i]) & CMODE_PADDING_MSK) | CMODE_PADDING;
                                break;
                        case 3 : /* Command Tag ID */
                                inVal = ((inoutp_commandData->unInputParamsBuf[i]) & CTAGID_PADDING_MSK) | ((~(inoutp_commandData->unInputParamsBuf[i])) << CTAGID_SHFT) |CTAGID_PADDING;
                                break;
                        case 4 : /* Command Parameter Length */
                                inVal = ((inoutp_commandData->unInputParamsBuf[i]) & CPLEN_PADDING_MSK) | CPLEN_PADDING;
                                break;
                        default:
                                inVal = inoutp_commandData->unInputParamsBuf[i];


                }

		BREG_Write32( in_channelHandle->moduleHandle->regHandle, 
			in_channelHandle->ulInCmdBufAddr+ i*4, 
			inVal);  
	}

	/* Dump out the input commands */
	{
		unsigned int i;
		for(i = 0; i<inoutp_commandData->unInputParamLen; i++) {
	        /* Data */
			BDBG_MSG(("%08X\n",  BREG_Read32( in_channelHandle->moduleHandle->regHandle, 
				in_channelHandle->ulInCmdBufAddr+ i*4)));  
		}
	}

	/* Set ILOAD to signal BSP there's a command ready. */
	BDBG_MSG(("BHSM_P_SubmitRawCommand_DMA: ulILoadRegAddr = 0x%08lx, ulILoadVal = %ld\n", 
		in_channelHandle->ulILoadRegAddr, in_channelHandle->ulILoadVal));	
	BREG_Write32( in_channelHandle->moduleHandle->regHandle, 
			in_channelHandle->ulILoadRegAddr,			
			in_channelHandle->ulILoadVal);

	

#if 1
	if ( (errCode = BERR_TRACE(BKNI_WaitForEvent( in_channelHandle->oLoadWait, 
				in_channelHandle->moduleHandle->currentSettings.ulTimeOutInMilSecs))) != BERR_SUCCESS ) {

		BDBG_ERR(("BHSM_P_CommonSubmitRawCommand_DMA: BKNI_WaitForEvent timeout error %d\n", in_channelHandle->moduleHandle->currentSettings.ulTimeOutInMilSecs));
		errCode = BHSM_STATUS_TIME_OUT;				
		goto BHSM_P_DONE_LABEL;							
	}	
#else
	BKNI_WaitForEvent( in_channelHandle->oLoadWait, 
				in_channelHandle->moduleHandle->currentSettings.ulTimeOutInMilSecs);
#endif				

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

#if BHSM_PATCH_MEMC_DMA	
#if BKNI_ACQUIREMUTEX_LOCK_TRUST
	i = (BHSM_PATCH_MEMC_DMA_WAITING*2+500)/1000;   /* 1.5ms will be treated as 2ms */
	BKNI_Sleep(i);  
#else
	for (i=0; i<  BHSM_PATCH_MEMC_DMA_WAITING ;i++)   /* pls adjust the macro value to change your actual timing*/
	{	
		BKNI_Delay(2);  
	}
#endif 
	BREG_Write32( in_channelHandle->moduleHandle->regHandle, 	BHSM_PATCH_MEMC_DMA_REG, memcBackup );	

#endif

	/* Unlock mutex */
#if (BCHP_CHIP != 3563) && (BCHP_CHIP != 3548) && (BCHP_CHIP != 3556) && (BCHP_CHIP != 35130)
	BKNI_ReleaseMutex( in_channelHandle->mutexLock);	
#endif	
	
	BDBG_LEAVE(BHSM_P_CommonSubmitRawCommand_DMA);
	BDBG_MSG(("Exit BHSM_P_CommonSubmitRawCommand_DMA, errCode = 0x%0x\n", errCode));	
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
