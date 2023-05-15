/***************************************************************************
 *     Copyright (c) 2005-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhsm.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 5/7/07 10:40a $
 *
 * Module Description: This file contains Broadcom Host Secure Module (OS/platform 
 *                     independent) porting interface public functions.
 *                    			                    
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7440/b0/common/bhsm.c $
 * 
 * Hydra_Software_Devel/5   5/7/07 10:40a lyang
 * PR 30454: HSM update for SOAP test support in SubmitRawCommand
 * 
 * Hydra_Software_Devel/4   5/7/07 9:42a lyang
 * PR 30454: HSM update for SOAP test support in SubmitRawCommand
 * 
 * Hydra_Software_Devel/3   5/4/07 5:08p lyang
 * PR 30454: HSM update for SOAP test support in SubmitRawCommand
 * 
 * Hydra_Software_Devel/2   3/30/07 2:45p lyang
 * PR 28969: BSHM to avoid the problem of an uncleared status register
 * when sending a BSP command through BBS and then sending HSM commands
 * 
 * Hydra_Software_Devel/9   2/9/07 6:07p lyang
 * PR 27777: HSM add IRDY waiting loop, add polling support 27393
 * 
 * Hydra_Software_Devel/8   2/9/07 3:57p lyang
 * PR 27777: HSM add IRDY waiting loop, add polling support 27393
 * 
 * Hydra_Software_Devel/7   10/26/06 4:19p lyang
 * PR 23412: hsm pi include files should be re-factored so they are not
 * order dependent. PR 25133:HSM header files should include dependencies
 * 
 * Hydra_Software_Devel/6   10/19/06 4:00p lyang
 * PR 25028: Release-mode compiler warnings in HSM, unused param/var
 * 
 * Hydra_Software_Devel/5   9/26/06 3:05p btan
 * PR 23906: Converted BCHP_REV to BCHP_VER.
 * 
 * Hydra_Software_Devel/4   8/4/06 12:15a btan
 * PR22187: Sync with 7401C0 BSP header files.
 * 
 * Hydra_Software_Devel/3   5/8/06 8:56p btan
 * PR 21183: HSM needs to compile for 7401 B0
 * 
 * Hydra_Software_Devel/2   4/26/06 11:06a btan
 * PR 20446: HSM needs to compile for 7400 A0
 * 
 * Hydra_Software_Devel/2   3/27/06 7:53p btan
 * PR 20436: Sync up with Aegis 7_0 release
 * 
 * Hydra_Software_Devel/1   1/20/06 5:13p btan
 * PR 19140: Do not encrypt generic mode files.
 * 
 * Hydra_Software_Devel/5   1/17/06 9:31p btan
 * PR 19140: Do not encrypt generic mode files.
 *
 *
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "bint.h"
#include "bchp_int_id_bsp.h"
#include "bchp_bsp_cmdbuf.h"
#include "bchp_bsp_glb_control.h"

#include "bhsm.h"



BDBG_MODULE(BHSM);

/* #define BDBG_MSG(x) printf x */

/*******************************************************************************
*	Default Module and Channel Settings.  Note that we could only modify 
*	Module settings during BHSM_Open.
*******************************************************************************/
static const BHSM_Settings BHSM_defHsmSettings =
{
	2, /* max channels */
	2000, /* 2000 milli seconds timeout */
	NULL,   /* exception Callback function */
	0		/* default to ISR;  TODO: default to polling needs a BHSM_SetSettings() function call first*/
};

static const BHSM_ChannelSettings BHSM_defHsmChannelSettings =
{
	2
};


/*******************************************************************************
*	Public Module Functions
*******************************************************************************/
BERR_Code BHSM_GetDefaultSettings(
		BHSM_Settings		*outp_sSettings,
		BCHP_Handle		in_chipHandle
)
{
	BERR_Code errCode = BERR_SUCCESS;

	BDBG_ENTER(BHSM_GetDefaultSettings);
	BDBG_ASSERT( in_chipHandle );	
	BSTD_UNUSED(in_chipHandle);  /* Removed this when we start using this variable */

	*outp_sSettings = BHSM_defHsmSettings;

	BDBG_LEAVE(BHSM_GetDefaultSettings);
	return( errCode );
}


BERR_Code BHSM_SetSettings(
		BHSM_Handle				   hHsm,
	  	BHSM_NewSettings_t		* newSettings

)
{
	BERR_Code 		errCode = BERR_SUCCESS;	
	BDBG_MSG(("Inside BHSM_SetSettings\n"));
	BDBG_ENTER(BHSM_SetSettings);

	if (hHsm == NULL) {	return (BHSM_STATUS_INPUT_PARM_ERR);	}
	
	/*
		BHSM_defHsmSettings.uSpecialControl = ((BHSM_defHsmSettings.uSpecialControl >> leftPartShift) << leftPartShift)|
							((BHSM_defHsmSettings.uSpecialControl << rightPartShift) >> rightPartShift)|
							(ctrlValue << whichControl);
	*/

	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
						(newSettings->whichControl & (~BHSM_SPECIALCTROL_FLAGS))  != 0);

	if (((newSettings->whichControl & BHSM_CTRLS_TIMEOUT) == BHSM_CTRLS_TIMEOUT) && 
		 (newSettings->timeoutMs > BHSM_SPECIALCTROL_TIMEOUT_MAX) )
	{
		BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 1);
	}

	/* legal setting here */

	if ((newSettings->whichControl & BHSM_CTRLS_POLLINGORISR) == BHSM_CTRLS_POLLINGORISR)
	{
		hHsm->currentSettings.uSpecialControl = (newSettings->ctrlValue & 0x1) | (hHsm->currentSettings.uSpecialControl & (~0x1));
		hHsm->channelHandles[BHSM_HwModule_eCmdInterface2]->moduleHandle->currentSettings.uSpecialControl = hHsm->currentSettings.uSpecialControl;
		BDBG_MSG(("BHSM_CTRLS_POLLINGORISR is %x\n",hHsm->currentSettings.uSpecialControl ));

		if ( (newSettings->ctrlValue & 0x1) ==1 )
		{
			BHSM_P_CHECK_ERR_CODE_FUNC( errCode, BINT_DisableCallback(hHsm->IntCallback2) );
			BHSM_P_CHECK_ERR_CODE_FUNC( errCode, BINT_DisableCallback(hHsm->IntCallback) );
			
			BREG_Write32(hHsm->channelHandles[BHSM_HwModule_eCmdInterface2]->moduleHandle->regHandle, BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_EN, 
							BREG_Read32(  hHsm->channelHandles[BHSM_HwModule_eCmdInterface2]->moduleHandle->regHandle, 
										BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_EN) & 
										(~BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_STATUS_OLOAD2_INT_MASK));	 

			/* this clear may be optional, just to be safer*/
		       BREG_Write32(  hHsm->channelHandles[BHSM_HwModule_eCmdInterface2]->moduleHandle->regHandle,
						BCHP_BSP_GLB_CONTROL_GLB_OLOAD2,
						BREG_Read32(  hHsm->channelHandles[BHSM_HwModule_eCmdInterface2]->moduleHandle->regHandle, 
										BCHP_BSP_GLB_CONTROL_GLB_OLOAD2) & 
										(~BCHP_BSP_GLB_CONTROL_GLB_OLOAD2_CMD_OLOAD2_MASK));		

			BDBG_MSG(("//////////BHSM_CTRLS_POLLING to disable ISR2, %x, channel num=%d\n",
					hHsm->channelHandles[BHSM_HwModule_eCmdInterface2]->moduleHandle->currentSettings.uSpecialControl,
					hHsm->channelHandles[BHSM_HwModule_eCmdInterface2]->ucChannelNumber));


		}else
		{
			BHSM_P_CHECK_ERR_CODE_FUNC( errCode, BINT_EnableCallback(hHsm->IntCallback2) );
			BHSM_P_CHECK_ERR_CODE_FUNC( errCode, BINT_EnableCallback(hHsm->IntCallback) );	

			BREG_Write32(hHsm->channelHandles[BHSM_HwModule_eCmdInterface2]->moduleHandle->regHandle, BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_EN, 
							BREG_Read32(  hHsm->channelHandles[BHSM_HwModule_eCmdInterface2]->moduleHandle->regHandle, 	BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_EN) | 
										BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_STATUS_OLOAD2_INT_MASK);	 
			BDBG_MSG(("//////////BHSM_CTRLS_POLLING to enable ISR2, %x, channel num=%d\n",
					hHsm->channelHandles[BHSM_HwModule_eCmdInterface2]->moduleHandle->currentSettings.uSpecialControl,
					hHsm->channelHandles[BHSM_HwModule_eCmdInterface2]->ucChannelNumber));

		}

	}
	
	
	if ((newSettings->whichControl & BHSM_CTRLS_TIMEOUT) == BHSM_CTRLS_TIMEOUT)
	{
		hHsm->currentSettings.ulTimeOutInMilSecs = newSettings->timeoutMs;
		hHsm->channelHandles[BHSM_HwModule_eCmdInterface2]->moduleHandle->currentSettings.ulTimeOutInMilSecs =hHsm->currentSettings.ulTimeOutInMilSecs;
		BDBG_MSG(("BHSM_CTRLS_TIMEOUT is %x\n",hHsm->currentSettings.ulTimeOutInMilSecs ));
	}

	/* for 3563, this is not really used
	if ((newSettings->whichControl & BHSM_CTRLS_CHANNELNUM) == BHSM_CTRLS_CHANNELNUM)
	{
		hHsm->currentSettings.ucMaxChannels = newSettings->maxChannelNum;
		hHsm->channelHandles[BHSM_HwModule_eCmdInterface2]->moduleHandle->currentSettings.ulTimeOutInMilSecs =hHsm->currentSettings.ucMaxChannels ;
		BDBG_MSG(("BHSM_CTRLS_CHANNELNUM is %x\n",hHsm->currentSettings.ucMaxChannels  ));
	}   */

	
BHSM_P_DONE_LABEL:	
	
	BDBG_LEAVE(BHSM_SetSettings);
	BDBG_MSG(("Exit BHSM_SetSettings\n"));
	return( errCode );
}


BERR_Code BHSM_SetSettings_isr(
		BHSM_Handle				   hHsm,
	  	BHSM_NewSettings_t		* newSettings

)
{
	BERR_Code 		errCode = BERR_SUCCESS;	
	BDBG_MSG(("Inside BHSM_SetSettings\n"));
	BDBG_ENTER(BHSM_SetSettings_isr);

	if (hHsm == NULL) {	return (BHSM_STATUS_INPUT_PARM_ERR);	}
	
	/*
		BHSM_defHsmSettings.uSpecialControl = ((BHSM_defHsmSettings.uSpecialControl >> leftPartShift) << leftPartShift)|
							((BHSM_defHsmSettings.uSpecialControl << rightPartShift) >> rightPartShift)|
							(ctrlValue << whichControl);
	*/

	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
						(newSettings->whichControl & (~BHSM_SPECIALCTROL_FLAGS))  != 0);

	if (((newSettings->whichControl & BHSM_CTRLS_TIMEOUT) == BHSM_CTRLS_TIMEOUT) && 
		( newSettings->timeoutMs > BHSM_SPECIALCTROL_TIMEOUT_MAX ||newSettings->timeoutMs < BHSM_SPECIALCTROL_TIMEOUT_MIN ) )
	{
		BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 1);
	}

	/* legal setting here */

	if ((newSettings->whichControl & BHSM_CTRLS_POLLINGORISR) == BHSM_CTRLS_POLLINGORISR)
	{
		hHsm->currentSettings.uSpecialControl = (newSettings->ctrlValue & 0x1) | (hHsm->currentSettings.uSpecialControl & (~0x1));
		hHsm->channelHandles[BHSM_HwModule_eCmdInterface2]->moduleHandle->currentSettings.uSpecialControl = hHsm->currentSettings.uSpecialControl;
		BDBG_MSG(("BHSM_CTRLS_POLLINGORISR is %x\n",hHsm->currentSettings.uSpecialControl ));

		if ( (newSettings->ctrlValue & 0x1) ==1 )
		{
			BHSM_P_CHECK_ERR_CODE_FUNC( errCode, BINT_DisableCallback_isr(hHsm->IntCallback2) );
			BHSM_P_CHECK_ERR_CODE_FUNC( errCode, BINT_DisableCallback(hHsm->IntCallback) );

			BREG_Write32(hHsm->channelHandles[BHSM_HwModule_eCmdInterface2]->moduleHandle->regHandle, BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_EN, 
							BREG_Read32(  hHsm->channelHandles[BHSM_HwModule_eCmdInterface2]->moduleHandle->regHandle, 
										BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_EN) & 
										(~BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_STATUS_OLOAD2_INT_MASK));	 

			/* this clear may be optional, just to be safer*/
		       BREG_Write32(  hHsm->channelHandles[BHSM_HwModule_eCmdInterface2]->moduleHandle->regHandle,
						BCHP_BSP_GLB_CONTROL_GLB_OLOAD2,
						BREG_Read32(  hHsm->channelHandles[BHSM_HwModule_eCmdInterface2]->moduleHandle->regHandle, 
										BCHP_BSP_GLB_CONTROL_GLB_OLOAD2) & 
										(~BCHP_BSP_GLB_CONTROL_GLB_OLOAD2_CMD_OLOAD2_MASK));		

			BDBG_MSG(("//////////BHSM_CTRLS_POLLING to disable ISR2, %x, channel num=%d\n",
					hHsm->channelHandles[BHSM_HwModule_eCmdInterface2]->moduleHandle->currentSettings.uSpecialControl,
					hHsm->channelHandles[BHSM_HwModule_eCmdInterface2]->ucChannelNumber));


		}else
		{
			BHSM_P_CHECK_ERR_CODE_FUNC( errCode, BINT_EnableCallback_isr(hHsm->IntCallback2) );
			BHSM_P_CHECK_ERR_CODE_FUNC( errCode, BINT_EnableCallback(hHsm->IntCallback) );	

			BREG_Write32(hHsm->channelHandles[BHSM_HwModule_eCmdInterface2]->moduleHandle->regHandle, BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_EN, 
							BREG_Read32(  hHsm->channelHandles[BHSM_HwModule_eCmdInterface2]->moduleHandle->regHandle, 	BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_EN) | 
										BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_STATUS_OLOAD2_INT_MASK);	 
			BDBG_MSG(("//////////BHSM_CTRLS_POLLING to enable ISR2, %x, channel num=%d\n",
					hHsm->channelHandles[BHSM_HwModule_eCmdInterface2]->moduleHandle->currentSettings.uSpecialControl,
					hHsm->channelHandles[BHSM_HwModule_eCmdInterface2]->ucChannelNumber));

		}

	}
	
	
	if ((newSettings->whichControl & BHSM_CTRLS_TIMEOUT) == BHSM_CTRLS_TIMEOUT)
	{
		hHsm->currentSettings.ulTimeOutInMilSecs = newSettings->timeoutMs;
		hHsm->channelHandles[BHSM_HwModule_eCmdInterface2]->moduleHandle->currentSettings.ulTimeOutInMilSecs =hHsm->currentSettings.ulTimeOutInMilSecs;
		BDBG_MSG(("BHSM_CTRLS_TIMEOUT is %x\n",hHsm->currentSettings.ulTimeOutInMilSecs ));
	}

	/* for 3563, this is not really used
	if ((newSettings->whichControl & BHSM_CTRLS_CHANNELNUM) == BHSM_CTRLS_CHANNELNUM)
	{
		hHsm->currentSettings.ucMaxChannels = newSettings->maxChannelNum;
		hHsm->channelHandles[BHSM_HwModule_eCmdInterface2]->moduleHandle->currentSettings.ulTimeOutInMilSecs =hHsm->currentSettings.ucMaxChannels ;
		BDBG_MSG(("BHSM_CTRLS_CHANNELNUM is %x\n",hHsm->currentSettings.ucMaxChannels  ));
	}   */

	
BHSM_P_DONE_LABEL:	
	
	BDBG_LEAVE(BHSM_SetSettings_isr);
	BDBG_MSG(("Exit BHSM_SetSettings_isr\n"));
	return( errCode );
}


BERR_Code BHSM_Open(
		BHSM_Handle			*outp_handle, 
		BREG_Handle			in_regHandle,     
		BCHP_Handle			in_chipHandle,
		BINT_Handle			in_interruptHandle,
		const BHSM_Settings	*inp_sSettings
)
{
	BERR_Code errCode = BERR_SUCCESS;
 	BHSM_Handle moduleHandle;
	unsigned int channelNum, i, j;

	BDBG_ENTER(BHSM_Open);
	BDBG_ASSERT( in_chipHandle );
	BDBG_ASSERT( in_regHandle );
	BDBG_ASSERT( in_interruptHandle );


	*outp_handle = NULL;

	/* Alloc memory from the system heap */
	if ((moduleHandle = 
				(BHSM_Handle) BKNI_Malloc( sizeof( BHSM_P_Handle))) 
				== NULL) {
		/* wrap initially detected error code */
		errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		goto BHSM_P_DONE_LABEL;
	}

	BKNI_Memset(moduleHandle, 0, sizeof( BHSM_P_Handle ));
	
	moduleHandle->ulMagicNumber = BHSM_P_HANDLE_MAGIC_NUMBER;
	moduleHandle->chipHandle = in_chipHandle;
	moduleHandle->regHandle = in_regHandle;
	moduleHandle->interruptHandle = in_interruptHandle;

	BKNI_CreateMutex( &(moduleHandle->caKeySlotMutexLock));
	BKNI_CreateMutex( &(moduleHandle->m2mKeySlotMutexLock));
	

	if (inp_sSettings == NULL)
		moduleHandle->currentSettings = BHSM_defHsmSettings;
	else {
		moduleHandle->currentSettings = *inp_sSettings;
	}	

	BHSM_SetIntrCallback(	moduleHandle, BHSM_IntrType_eException, BHSM_P_ExceptionInterruptCB_isr);

	BDBG_MSG(("Inside BHSM_Open: Before BINT_CreateCallback \n"));
	/*  Enable interrupt   */	
	BHSM_P_CHECK_ERR_CODE_FUNC( errCode, BINT_CreateCallback(
			&(moduleHandle->IntCallback), moduleHandle->interruptHandle, 
			BCHP_INT_ID_BSP_OLOAD1_INTR,
			BHSM_P_IntHandler_isr, (void *) moduleHandle, 0x00 ) );	
	BHSM_P_CHECK_ERR_CODE_FUNC( errCode, BINT_EnableCallback(moduleHandle->IntCallback) );

	BHSM_P_CHECK_ERR_CODE_FUNC( errCode, BINT_CreateCallback(
			&(moduleHandle->IntCallback2), moduleHandle->interruptHandle, 
			BCHP_INT_ID_BSP_OLOAD2_INTR,
			BHSM_P_IntHandler_isr, (void *) moduleHandle, 0x01 ) );		

	BDBG_MSG(("Inside BHSM_Open: Before BINT_EnableCallback2 \n"));			
	BHSM_P_CHECK_ERR_CODE_FUNC( errCode, BINT_EnableCallback(moduleHandle->IntCallback2) );

#if (BCHP_CHIP==7401) &&  (BCHP_VER == BCHP_VER_A0)
	BREG_Write32( moduleHandle->regHandle, BCHP_BSP_GLB_NONSECURE_GLB_HOST_INTR_EN, 
		0);
	/*		BCHP_BSP_GLB_NONSECURE_GLB_HOST_INTR_EN_HOST_INTR_STATUS_EN_MASK);	 */
#else
	BREG_Write32( moduleHandle->regHandle, BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_EN, 
		0);
	/*		BCHP_BSP_GLB_NONSECURE_GLB_HOST_INTR_EN_HOST_INTR_STATUS_EN_MASK);	 */
#endif

	BREG_Write32( moduleHandle->regHandle, BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_STATUS, 0);
	BREG_Write32( moduleHandle->regHandle, BCHP_BSP_GLB_CONTROL_GLB_OLOAD2, 0);
	BREG_Write32( moduleHandle->regHandle, BCHP_BSP_GLB_CONTROL_GLB_OLOAD1, 0);
	BREG_Write32( moduleHandle->regHandle, BCHP_BSP_GLB_CONTROL_GLB_ILOAD2, 0);
	BREG_Write32( moduleHandle->regHandle, BCHP_BSP_GLB_CONTROL_GLB_ILOAD1, 0);


	/* Get the chip information */
	BCHP_GetChipInfo( moduleHandle->chipHandle, &moduleHandle->chipId, &moduleHandle->chipRev );
	BDBG_MSG(( "Chip Information \n" ));
	BDBG_MSG(( "chipId=0x%x, chipRev=0x%x\n", moduleHandle->chipId, moduleHandle->chipRev ));

   	/* 
		If inp_sSettings->maxChannels == 0, set it to 
		BHSM_MAX_SUPPOTED_CHANNELS 
	*/
	if (moduleHandle->currentSettings.ucMaxChannels == 0)
		moduleHandle->currentSettings.ucMaxChannels = 
							BHSM_MAX_SUPPOTED_CHANNELS;
		
	for( channelNum = 0; 
		channelNum < moduleHandle->currentSettings.ucMaxChannels; 
		channelNum++ )	{
			
		moduleHandle->channelHandles[channelNum] = NULL;
	}


	/* Initialize members */
	for (i=0; i<BCMD_TOTAL_PIDCHANNELS*2; i++) {

		moduleHandle->aunCAKeySlotInfo[i].keySlotType = 
			BCMD_XptSecKeySlot_eTypeMax;	
		moduleHandle->aunCAKeySlotInfo[i].bIsUsed = 
					false;	

	}

	/* Initialize PidChannelToKeySlotNum matrices */
	for (i=0; i<BCMD_TOTAL_PIDCHANNELS; i++) {
		for (j=0; j < 2; j++) {
			moduleHandle->aPidChannelToCAKeySlotNum[i][j].keySlotType = 
				BCMD_XptSecKeySlot_eTypeMax;	
			moduleHandle->aPidChannelToCAKeySlotNum[i][j].unKeySlotNum = 
						BHSM_SLOT_NUM_INIT_VAL;	
		}
	}


	/* Initialize M2M Key Slot info matrices */
	for (i=0; i<BCMD_MAX_M2M_KEY_SLOT; i++) {

		moduleHandle->aunM2MKeySlotInfo[i].bIsUsed = 	false;	

	}	
	
	/* End of Initialize members */	

	*outp_handle = moduleHandle;

	moduleHandle->bIsOpen = true;
	
BHSM_P_DONE_LABEL:	     

	if( errCode != BERR_SUCCESS )
	{
		if( moduleHandle != NULL )
		{
			BKNI_Free( moduleHandle );

		}
	}	
	BDBG_LEAVE(BHSM_Open);
	BDBG_MSG(("Exit BHSM_Open\n"));	
	return( errCode );

}

BERR_Code BHSM_Close(
		BHSM_Handle inout_handle
)
{
	BERR_Code errCode = BERR_SUCCESS;


	BDBG_ENTER(BHSM_Close);
	BDBG_ASSERT( inout_handle );	

	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, (inout_handle ==  NULL) );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inout_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inout_handle->bIsOpen ==  false) );

	
	BHSM_P_CHECK_ERR_CODE_FUNC( errCode, BINT_DisableCallback( inout_handle->IntCallback));
	BHSM_P_CHECK_ERR_CODE_FUNC( errCode, BINT_DestroyCallback( inout_handle->IntCallback2 ) );
	

BHSM_P_DONE_LABEL:	
	BKNI_DestroyMutex( inout_handle->caKeySlotMutexLock);
	BKNI_DestroyMutex( inout_handle->m2mKeySlotMutexLock);
	
	inout_handle->bIsOpen = false;
	BKNI_Free(  inout_handle );	
	inout_handle = NULL;

	BDBG_LEAVE(BHSM_Close);
	return( errCode );
}

BERR_Code BHSM_GetTotalChannels(
		BHSM_Handle		in_handle, 
		unsigned char		*outp_ucTotalChannels
)
{
	BERR_Code errCode = BERR_SUCCESS;

	BDBG_ENTER(BHSM_GetTotalChannels);
	BDBG_ASSERT( in_handle );

	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	*outp_ucTotalChannels = in_handle->currentSettings.ucMaxChannels;

BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_GetTotalChannels);
	return( errCode );
}


BERR_Code BHSM_GetChannelDefaultSettings(
		BHSM_Handle				in_handle, 
		BHSM_HwModule			in_channelNo, 
		BHSM_ChannelSettings		*outp_sSettings
)
{
	BERR_Code errCode = BERR_SUCCESS;


	BDBG_ENTER(BHSM_GetChannelDefaultSettings);
	BDBG_ASSERT( in_handle );
	
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );

	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BERR_INVALID_PARAMETER, 
		(in_channelNo >= in_handle->currentSettings.ucMaxChannels) );

	*outp_sSettings = BHSM_defHsmChannelSettings;
	
BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_GetChannelDefaultSettings);
	return( errCode );
}

BERR_Code BHSM_Channel_Open(
		BHSM_Handle					in_handle, 
		BHSM_ChannelHandle			*outp_channelHandle, 
		BHSM_HwModule				in_channelNo, 
		const BHSM_ChannelSettings	*inp_channelDefSettings 
)
{
	BERR_Code errCode = BERR_SUCCESS;
 	BHSM_ChannelHandle channelHandle = NULL;	

	BDBG_MSG(("Inside BHSM_Channel_Open\n"));
	BDBG_ENTER(BHSM_Channel_Open);
	BDBG_ASSERT( in_handle );
	
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BERR_INVALID_PARAMETER, 
		(in_channelNo >= in_handle->currentSettings.ucMaxChannels) );

	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->bIsOpen ==  false) );

	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inp_channelDefSettings ==  NULL) );	

	/* channel handle must be NULL.  */
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->channelHandles[in_channelNo]  != NULL) );

	*outp_channelHandle = NULL;
	
	/* Alloc memory from the system heap */
	if ((channelHandle = 
			(BHSM_ChannelHandle) BKNI_Malloc(sizeof(BHSM_P_ChannelHandle))) 
			== NULL) {
		/* wrap initially detected error code */
		errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);		
		goto BHSM_P_DONE_LABEL;
	}	

	BKNI_Memset(channelHandle, 0, sizeof( BHSM_P_ChannelHandle ));

	channelHandle->ulMagicNumber = BHSM_P_CHANNEL_HANDLE_MAGIC_NUMBER;
	channelHandle->moduleHandle = in_handle;


	channelHandle->currentChannelSettings.ucUnknown = 
		inp_channelDefSettings->ucUnknown;

	BKNI_CreateMutex( &(channelHandle->mutexLock));
	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
		BKNI_CreateEvent( &(channelHandle->oLoadWait)));	
	
				
	BDBG_MSG(("in_channelNo = %d\n", in_channelNo));

	if (inp_channelDefSettings == NULL)
		channelHandle->currentChannelSettings = BHSM_defHsmChannelSettings;
	else {
		channelHandle->currentChannelSettings = *inp_channelDefSettings;
	}
	
	BDBG_MSG(("in_channelNo = %d\n", in_channelNo));

	switch(in_channelNo) {
		case BHSM_HwModule_eCmdInterface1:
			channelHandle->ulInCmdBufAddr = BHSM_IN_BUF1_ADDR;
			channelHandle->ulOutCmdBufAddr = BHSM_OUT_BUF1_ADDR;
#if (BCHP_CHIP==7401) &&  (BCHP_VER == BCHP_VER_A0)			
			channelHandle->ulILoadRegAddr = BCHP_BSP_GLB_NONSECURE_GLB_ILOAD1;
			channelHandle->ulILoadVal = BCHP_BSP_GLB_NONSECURE_GLB_ILOAD1_CMD_ILOAD1_MASK;
			channelHandle->ulIReadyRegAddr = BCHP_BSP_GLB_NONSECURE_GLB_IRDY;
			channelHandle->ulIReadyVal= BCHP_BSP_GLB_NONSECURE_GLB_IRDY_CMD_IDRY1_MASK;			
#else
			channelHandle->ulILoadRegAddr = BCHP_BSP_GLB_CONTROL_GLB_ILOAD1;
			channelHandle->ulILoadVal = BCHP_BSP_GLB_CONTROL_GLB_ILOAD1_CMD_ILOAD1_MASK;
			channelHandle->ulIReadyRegAddr = BCHP_BSP_GLB_CONTROL_GLB_IRDY;
			channelHandle->ulIReadyVal= BCHP_BSP_GLB_CONTROL_GLB_IRDY_CMD_IDRY1_MASK;			

#endif

			break;

		case BHSM_HwModule_eCmdInterface2:
			channelHandle->ulInCmdBufAddr = BHSM_IN_BUF2_ADDR;		
			channelHandle->ulOutCmdBufAddr = BHSM_OUT_BUF2_ADDR;	
#if (BCHP_CHIP==7401) &&  (BCHP_VER == BCHP_VER_A0)			
			channelHandle->ulILoadRegAddr = BCHP_BSP_GLB_NONSECURE_GLB_ILOAD2;			
			channelHandle->ulILoadVal = BCHP_BSP_GLB_NONSECURE_GLB_ILOAD2_CMD_ILOAD2_MASK;
			channelHandle->ulIReadyRegAddr = BCHP_BSP_GLB_NONSECURE_GLB_IRDY;
			channelHandle->ulIReadyVal= BCHP_BSP_GLB_NONSECURE_GLB_IRDY_CMD_IDRY2_MASK;
#else
			channelHandle->ulILoadRegAddr = BCHP_BSP_GLB_CONTROL_GLB_ILOAD2;			
			channelHandle->ulILoadVal = BCHP_BSP_GLB_CONTROL_GLB_ILOAD2_CMD_ILOAD2_MASK;
			channelHandle->ulIReadyRegAddr = BCHP_BSP_GLB_CONTROL_GLB_IRDY;
			channelHandle->ulIReadyVal= BCHP_BSP_GLB_CONTROL_GLB_IRDY_CMD_IDRY2_MASK;

#endif

			break;

		default:
			errCode = BERR_INVALID_PARAMETER;
			goto BHSM_P_DONE_LABEL;		
	}
	
	BDBG_MSG(("ulInCmdBufAddr = 0x%lx, ulOutCmdBufAddr = 0x%lx \n", 
			channelHandle->ulInCmdBufAddr, channelHandle->ulOutCmdBufAddr));

	channelHandle->ucChannelNumber =  in_channelNo;
	in_handle->channelHandles[in_channelNo] = channelHandle;

	*outp_channelHandle = channelHandle;
	channelHandle->bIsOpen = true;	

BHSM_P_DONE_LABEL:
	if( errCode != BERR_SUCCESS )
	{
		if( channelHandle != NULL )
		{
			BKNI_Free( channelHandle );
		}
	}

	BDBG_LEAVE(BHSM_Channel_Open);
	BDBG_MSG(("Exit BHSM_Channel_Open\n"));	
	return( errCode );
}

BERR_Code BHSM_Channel_Close(
		BHSM_ChannelHandle inout_channelHandle
)
{
	BERR_Code errCode = BERR_SUCCESS;
	BHSM_Handle moduleHandle;

	BDBG_ENTER(BHSM_Channel_Close);
	BDBG_MSG(("Enter BHSM_Channel_Close\n"));
	BDBG_ASSERT( inout_channelHandle );
	
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inout_channelHandle->ulMagicNumber != BHSM_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );
	
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inout_channelHandle->bIsOpen ==  false) );	

   
BHSM_P_DONE_LABEL:

	BKNI_DestroyMutex( inout_channelHandle->mutexLock);

	BKNI_DestroyEvent( inout_channelHandle->oLoadWait);		
	
	inout_channelHandle->bIsOpen = false;
	
	moduleHandle = inout_channelHandle->moduleHandle;	
	moduleHandle->channelHandles[inout_channelHandle->ucChannelNumber] = NULL;	
	
	BKNI_Free( inout_channelHandle );	
	inout_channelHandle = NULL;

	BDBG_MSG(("Leave BHSM_Channel_Close\n"));
	BDBG_LEAVE(BHSM_Channel_Close);
	return( errCode );
}

BERR_Code BHSM_Channel_GetDevice(
		BHSM_ChannelHandle	in_channelHandle, 
		BHSM_Handle			*outp_handle 
)
{
	BERR_Code errCode = BERR_SUCCESS;

	BDBG_ENTER(BHSM_Channel_GetDevice);
	BDBG_ASSERT( in_channelHandle );

	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_channelHandle->moduleHandle->bIsOpen ==  false) );	

	*outp_handle = NULL;
	
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_channelHandle->ulMagicNumber != BHSM_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );

	*outp_handle = in_channelHandle->moduleHandle;

BHSM_P_DONE_LABEL:
	BDBG_LEAVE(BHSM_Channel_GetDevice);
	return( errCode );
}


BERR_Code BHSM_GetChannel(
		BHSM_Handle			in_handle, 
		BHSM_HwModule		in_channelNo, 
		BHSM_ChannelHandle	*outp_channelHandle
)
{
	BERR_Code errCode = BERR_SUCCESS;
 	BHSM_ChannelHandle channelHandle = NULL;

	BDBG_ENTER(BHSM_GetChannel);
	BDBG_ASSERT( in_handle );

	*outp_channelHandle = NULL;	
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );

	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BERR_INVALID_PARAMETER, 
		(in_channelNo >= in_handle->currentSettings.ucMaxChannels) );
	
	channelHandle = in_handle->channelHandles[in_channelNo];

	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(channelHandle->ulMagicNumber != BHSM_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );	
	
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(channelHandle == NULL ) );

	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(channelHandle->bIsOpen ==  false) );	

	*outp_channelHandle = channelHandle;
	
BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_GetChannel);
	return( errCode );
}


BERR_Code BHSM_SetIntrCallback(
		BHSM_Handle			in_handle,
		BHSM_IntrType		in_eIntType,
		BHSM_IsrCallbackFunc 	in_callback	
)
{
	BERR_Code errCode = BERR_SUCCESS;	

	BDBG_ASSERT( in_handle );
	BDBG_ASSERT( in_callback );
											   
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BERR_INVALID_PARAMETER, 
		(in_eIntType >= BHSM_IntrType_eMax ) );
		
	switch (in_eIntType) {

		case BHSM_IntrType_eException:
			BKNI_EnterCriticalSection();
			/* Assign the callback into the interrupt table. */
			in_handle->currentSettings.exceptionCBfunc	= in_callback;
			BKNI_LeaveCriticalSection();	
			break;
			
		default:
			BDBG_ERR(( "callback for in_eIntType %lu is not supported!", in_eIntType ));
			errCode = BERR_TRACE( BERR_INVALID_PARAMETER );

	}


	/*BDBG_MSG((", final ulVal = 0x%x\n ", ulVal)); */
	
BHSM_P_DONE_LABEL:

	BDBG_LEAVE(BHSM_Channel_EnableIntrCallback);
	return( errCode );

}



#if 0
BERR_Code BHSM_Channel_DisableIntrCallback(
		BHSM_Handle			in_handle,
		BHSM_IntrType    		in_eIntType
)
{
	BERR_Code errCode = BERR_SUCCESS;	
	
	BDBG_ASSERT( in_handle );

	BKNI_EnterCriticalSection();	
	BREG_Write32( in_handle->regHandle, BCHP_BSP_NONSECURE_INTR2_CPU_MASK_SET, 
		 				(1ul << in_eIntType));  
	BKNI_LeaveCriticalSection();		

	/* BDBG_MSG((", final ulVal = 0x%x\n ", ulVal)); */
BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_Channel_DisableIntrCallback);
	return( errCode );
}
#endif

#ifdef BHSM_AUTO_TEST

BERR_Code BHSM_SubmitRawCommand (
		BHSM_Handle 		in_handle,
		BHSM_HwModule	in_interface,
		uint32_t			in_unInputParamLenInWord,  /* length in words, including header sections words*/
		uint32_t			*inp_unInputParamsBuf,    /* entire cmd, incluidng header section words*/
		uint32_t			*outp_unOutputParamLenInWord,  /* length in words, including header sections words*/
		uint32_t			*outp_unOutputParamsBuf        /* entire cmd, incluidng header section words*/
)
{
	BERR_Code 		errCode = BERR_SUCCESS;
	BHSM_P_CommandData_t 	commandData;
	unsigned int 		i;
	BHSM_ChannelHandle 	channelHandle = in_handle->channelHandles[in_interface];
	
	BDBG_MSG(("Inside BHSM_SubmitRawCommand\n"));	

	BKNI_Memset(&commandData, 0, sizeof( BHSM_P_CommandData_t ));
	
	commandData.unInputParamLen = in_unInputParamLenInWord*4;	/* passed-in param in words*/

	for (i=0; i<commandData.unInputParamLen/4; i++) {
		commandData.unInputParamsBuf[i] = inp_unInputParamsBuf[i];		/* including header section words*/
	}

	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
		BHSM_P_CommonSubmitRawCommand(channelHandle, &commandData));

	*outp_unOutputParamLenInWord = (commandData.unOutputParamLen+3)/4;   /* to soapApp, translated in words*/
	
	for(i = 0; i<(commandData.unOutputParamLen+3)/4; i++) {
		outp_unOutputParamsBuf[i] = commandData.unOutputParamsBuf[i];
	}	

BHSM_P_DONE_LABEL:
	BDBG_MSG(("Exit BHSM_SubmitRawCommand\n"));		
	return errCode;
	
}

#endif


