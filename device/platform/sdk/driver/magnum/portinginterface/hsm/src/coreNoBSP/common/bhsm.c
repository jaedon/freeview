/***************************************************************************
 *     (c)2007-2008 Broadcom Corporation
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
 * $brcm_Workfile: bhsm.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 9/28/12 3:50p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/src/coreNoBSP/common/bhsm.c $
 * 
 * Hydra_Software_Devel/2   9/28/12 3:50p atruong
 * SWSECURITY-175: Symbolic link removal for 7408 and 7550 HSM PI
 * 
 * Hydra_Software_Devel/1   1/21/11 9:51a haoboy
 * SW7550-666:initial version
 * 
 * Hydra_Software_Devel/10   11/26/10 5:00p jhaberf
 * SW35125-13: Added 35125 DTV chip support
 * 
 * Hydra_Software_Devel/9   9/29/10 11:29a qcheng
 * SW7408-82 : Add security support for 7408 A0
 * 
 * Hydra_Software_Devel/8   6/14/10 11:36a qcheng
 * SW35230-51:   Magnum HSM PI needs to build for the 35230
 * 
 * Hydra_Software_Devel/7   6/10/10 12:05p qcheng
 * SW7408-82 : Add security support for 7408 A0
 * 
 * Hydra_Software_Devel/6   6/2/10 2:32p cdisc
 * SW35230-51:   Magnum HSM PI needs to build for the 35230
 * 
 * Hydra_Software_Devel/5   5/28/10 6:22p qcheng
 * SW7550-398 : PVR encryption feature is required for 7540 production
 * with PVR function
 * 
 * Hydra_Software_Devel/4   5/25/10 6:26p qcheng
 * SW7408-82 : Add security support for 7408 A0
 * 
 * Hydra_Software_Devel/3   4/2/10 7:26p qcheng
 * SW7550-353: PVR encryption feature is required for 7540 production with
 * PVR function
 * 
 * Hydra_Software_Devel/2   3/4/10 11:26a qcheng
 * SW7408-82 : Add security support for 7408 A0
 * 
 * Hydra_Software_Devel/1   12/4/09 5:25p qcheng
 * SW7550-102 : Add Security support for 7550
 *
 **************************************************************************/

#include "bhsm.h"

#include "bstd.h" 
#include "bkni.h"
#include "bhsm_keyladder.h" 
#if ( BCHP_CHIP == 7550)
#include "xpt_security_key_table.h"
#include "chip_secure_7550.h"
#elif (( BCHP_CHIP == 35230) || ( BCHP_CHIP == 35125 ) || ( BCHP_CHIP == 35233 ))
#include "xpt_security_key_table.h"
#include "chip_secure_35230.h"
#elif ( BCHP_CHIP == 7408)
#include "mmscram_key_table.h"
#include "xpt_mmscram.h"
#endif



BDBG_MODULE(BHSM);

/* #define BDBG_MSG(x) printf x */

/*******************************************************************************
*	Default Module and Channel Settings.  Note that we could only modify 
*	Module settings during BHSM_Open.
*******************************************************************************/
static const BHSM_Settings BHSM_defHsmSettings =
{
	BHSM_CustMode_eGeneric
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


BERR_Code BHSM_Open(
		BHSM_Handle			*outp_handle, 
		BREG_Handle			in_regHandle,     
		BCHP_Handle			in_chipHandle,
		BINT_Handle         in_intHandle,
		const BHSM_Settings	*inp_sSettings
)
{
	BERR_Code errCode = BERR_SUCCESS;
 	BHSM_Handle moduleHandle;
	unsigned int  i;
#if ( BCHP_CHIP == 35230) || ( BCHP_CHIP == 35125 ) || ( BCHP_CHIP == 35233 )
    uint32_t regVal = 0;
#endif
    
	BDBG_ENTER(BHSM_Open);
	BDBG_ASSERT( in_chipHandle );
	BDBG_ASSERT( in_regHandle );

	*outp_handle = NULL;

	/* Alloc memory from the system heap */
	if ((moduleHandle = (BHSM_Handle) BKNI_Malloc( sizeof( BHSM_P_Handle))) == NULL) {
		/* wrap initially detected error code */
		errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		goto BHSM_P_DONE_LABEL;
	}

	BKNI_Memset(moduleHandle, 0, sizeof( BHSM_P_Handle ));
	
	moduleHandle->ulMagicNumber = BHSM_P_HANDLE_MAGIC_NUMBER;
	moduleHandle->chipHandle = in_chipHandle;
	moduleHandle->regHandle = in_regHandle;

	BKNI_CreateMutex( &(moduleHandle->caCpKeySlotMutexLock));
	BKNI_CreateMutex( &(moduleHandle->m2mKeySlotMutexLock));

	if (inp_sSettings == NULL)
		moduleHandle->currentSettings = BHSM_defHsmSettings;
	else 
		moduleHandle->currentSettings = *inp_sSettings;
		

	/* Get the chip information */
	BCHP_GetChipInfo( moduleHandle->chipHandle, &moduleHandle->chipId, &moduleHandle->chipRev );
	BDBG_MSG(( "Chip Information \n" ));
	BDBG_MSG(( "chipId=0x%x, chipRev=0x%x\n", moduleHandle->chipId, moduleHandle->chipRev ));

	/* Initialize members */
#if ( BCHP_CHIP != 7408 )
{
	int j;
	
	for ( i=0; i < BHSM_MaxCaCpKeySlot;  i++) {
 		moduleHandle->aCaCpKeySlotInfo[i].bIsUsed  = false;
		for ( j = 0 ; j < BHSM_KeyType_eMaxNumber ; j++) {
			moduleHandle->aCaCpKeySlotInfo[i].aKeySlotAlgorithm[j].ulGlobalControlBits = 0;
			/*We need give a valid CA offset address otherwise hardware ACL does not allow global control bits will be over-written if we only configure for CP */			
			moduleHandle->aCaCpKeySlotInfo[i].aKeySlotAlgorithm[j].ulCAControlBits = 
				( BHSM_KeyAddr_eCaKeyOdd1 << XPT_SECURITY_KEY_TABLE_MODULE_KEY_MODE_KEY_OFFSET_SHIFT  ) & XPT_SECURITY_KEY_TABLE_MODULE_KEY_MODE_KEY_OFFSET_MASK ;
			moduleHandle->aCaCpKeySlotInfo[i].aKeySlotAlgorithm[j].ulCPSControlBits = 0;
			moduleHandle->aCaCpKeySlotInfo[i].aKeySlotAlgorithm[j].ulCPDControlBits = 0;
		}	
	}	
}

	/* Initialize PidChannelToKeySlotNum matrices */
	for (i=0; i<BHSM_MaxPidChannelNum; i++)  
 			moduleHandle->aunPidChannelToCaCpKeySlotNum[i] = BHSM_SLOT_NUM_INIT_VAL;	
#endif	


#if ( BCHP_CHIP != 7550 )
	/* Initialize M2M Key Slot info matrices */
	for (i=0; i<BHSM_MaxM2MKeySlot; i++) {
		moduleHandle->aM2MKeySlotInfo[i].bIsUsed = false;
		moduleHandle->aM2MKeySlotInfo[i].aKeySlotAlgorithm.ulGlobalControlBits = 0;
		moduleHandle->aM2MKeySlotInfo[i].aKeySlotAlgorithm.ulM2MControlBits = 0;
	}
#endif	
	
	BHSM_P_EnableSecurity(moduleHandle);
	BHSM_P_ReqCustMode(moduleHandle, moduleHandle->currentSettings.eCustMode); 

#if ( BCHP_CHIP == 35230) || ( BCHP_CHIP == 35125 ) || ( BCHP_CHIP == 35233 )
    /* Disable ICAM for 35230 */
    regVal = BREG_Read32(moduleHandle->regHandle, XPT_SECURITY_SECURITY_CTRL);
    BREG_Write32(moduleHandle->regHandle, XPT_SECURITY_SECURITY_CTRL, regVal&0xFFFDFFFF);
#endif

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
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, (inout_handle->bIsOpen ==  false) );

	BHSM_P_DisableSecurity(inout_handle);

BHSM_P_DONE_LABEL:	
	BKNI_DestroyMutex( inout_handle->caCpKeySlotMutexLock);
	BKNI_DestroyMutex( inout_handle->m2mKeySlotMutexLock);
	inout_handle->bIsOpen = false;
	BKNI_Free(  inout_handle );	
	inout_handle = NULL;

	BDBG_LEAVE(BHSM_Close);
	return( errCode );
}


