/***************************************************************************
 *     Copyright (c) 2005-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhsm_dvd_cmds.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 3/18/11 6:31p $
 *
 * Module Description: This file contains Broadcom Host Secure Module (OS/platform 
 *                     independent) porting interface public functions.
 *                    			                    
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7640/a0/dvd_cmds/bhsm_dvd_cmds.c $
 * 
 * Hydra_Software_Devel/3   3/18/11 6:31p cdisc
 * SWBLURAY-25150: unified media key fix
 * 
 * Hydra_Software_Devel/2   3/18/11 4:16p cdisc
 * SWBLURAY-25150: adding AACS support
 * 
 * Hydra_Software_Devel/1   2/8/11 11:54a cdisc
 * SWBLURAY-23691: branching files from 7630 dvd_cmds
 * 
 * Hydra_Software_Devel/7   9/25/09 12:03p atruong
 * SW7630-18: HSM PI Bringup for 7630B0
 * 
 * Hydra_Software_Devel/6   9/15/09 12:29a atruong
 * SW7630-18: HSM PI Support for 7630B0
 * 
 * Hydra_Software_Devel/5   4/23/09 10:26a atruong
 * PR54377: Remove length checking for output parameters
 * 
 * Hydra_Software_Devel/4   1/13/09 12:44p atruong
 * PR49290: HSM PI Support for 7601B0
 * 
 * Hydra_Software_Devel/3   12/4/08 1:05p atruong
 * PR49290: HSM PI Support for 7601B0
 * 
 * Hydra_Software_Devel/2   8/25/08 10:23p atruong
 * PR45631: BCM7601A0 Bring up
 * 
 * Hydra_Software_Devel/1   8/12/08 6:01p atruong
 * PR45631: HSM PI support for BCM7601 platform
 * 
 * Hydra_Software_Devel/10   1/17/08 2:52p qxie
 * PR38806: HSM: add new commad to support BD-RE
 * 
 * Hydra_Software_Devel/9   4/13/07 10:38a lyang
 * PR 27763: HSM to add 2 new PIs to support 7440B0 and some length change
 * on 4/13/07 according to BSP change
 * 
 * Hydra_Software_Devel/8   4/3/07 4:01p lyang
 * PR 27763: HSM to add 2 new PIs to support 7440B0
 * 
 * Hydra_Software_Devel/7   4/2/07 5:35p lyang
 * PR 27763: HSM to add 2 new PIs to support 7440B0
 * 
 * Hydra_Software_Devel/6   3/8/07 3:21p lyang
 * PR 27763: HSM to add a new PI to support 7440B0
 * 
 * Hydra_Software_Devel/5   3/6/07 6:27p lyang
 * PR 27763: HSM to add a new PI to support 7440B0
 * 
 * Hydra_Software_Devel/4   3/5/07 9:10p lyang
 * PR 27763: HSM to add a new PI to support 7440B0
 * 
 * Hydra_Software_Devel/3   3/2/07 4:40p lyang
 * PR 27763: HSM to add a new PI to support 7440B0
 * 
 * Hydra_Software_Devel/2   2/13/07 7:04p lyang
 * PR 27763: HSM to add a new PI to support 7440B0, net DMA, SHARF
 * 
 * Hydra_Software_Devel/1   11/28/06 10:50a btan
 * PR 26037: Added BHSM_UnwrapMultipleContentKeys(),
 * BHSM_UpdateSecretKey() and BHSM_CalculateMediaKey() to support
 * GenerateRouteContentKeys and MKB processing commands.
 * 
 *
 *
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"

#include "bhsm_datatypes.h"

/* From Aegis */
#include "bsp_s_commands.h"

#include "bhsm_datatypes.h"
#include "bhsm_priv.h"
#include "bhsm.h"
#include "bhsm_dvd_cmds.h"



BDBG_MODULE(BHSM);

/* #define BDBG_MSG(x) printf x */


BERR_Code  BHSM_UnwrapMultipleContentKeys(
		BHSM_Handle						in_handle,
		BHSM_MultipleContentKeysIO_t		*inoutp_updateMultiContentKeyIO
)
{
	BERR_Code errCode = BERR_SUCCESS;
	BHSM_ChannelHandle channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t unParamLen = 0, i , j, k;
	BHSM_P_CommandData_t commandData;


	BDBG_ENTER(BHSM_UnwrapMultipleContentKeys);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	commandData.cmdId = BCMD_cmdType_eUNWRAP_CONTENT_KEYS;  /* 0x2F*/
	commandData.unContMode = 0;
	
	/* only fill the command specific input parameters */
	commandData.unInputParamsBuf[(BCMD_UserMultipleKeys_CmdInputField_eKeyID0/4 - BCMD_CommonBufferFields_eStatus/4)] = 
			inoutp_updateMultiContentKeyIO->aucKeyID0;
	commandData.unInputParamLen +=  (BCMD_UserMultipleKeys_CmdInputField_eKeyID1/4) * 4
		 - (BCMD_UserMultipleKeys_CmdInputField_eKeyID0/4) * 4;

	commandData.unInputParamsBuf[(BCMD_UserMultipleKeys_CmdInputField_eKeyID1/4 - BCMD_UserMultipleKeys_CmdInputField_eKeyID0/4) ] = 
			inoutp_updateMultiContentKeyIO->aucKeyID1;
	commandData.unInputParamLen +=  (BCMD_UserMultipleKeys_CmdInputField_eTitleKey0/4) * 4
		 - (BCMD_UserMultipleKeys_CmdInputField_eKeyID1/4) * 4;

	for (i=0, 
		j = (BCMD_UserMultipleKeys_CmdInputField_eTitleKey0/4 - BCMD_UserMultipleKeys_CmdInputField_eKeyID0/4);
		i<BHSM_MULTIPLE_CONTENT_TITLE_KEY_DATA_LEN; j++, i+=4) {
			
		commandData.unInputParamsBuf[j ] = 
			inoutp_updateMultiContentKeyIO->aucTitleKeyAData[i] << 24 |
			inoutp_updateMultiContentKeyIO->aucTitleKeyAData[i+1] << 16 |
			inoutp_updateMultiContentKeyIO->aucTitleKeyAData[i+2] << 8 |
			inoutp_updateMultiContentKeyIO->aucTitleKeyAData[i+3]  ;
			
		commandData.unInputParamLen +=  4;
	}	
		
	for (i=0, 
		j = (BCMD_UserMultipleKeys_CmdInputField_eTitleKey1/4 - BCMD_UserMultipleKeys_CmdInputField_eKeyID0/4);
		i<BHSM_MULTIPLE_CONTENT_TITLE_KEY_DATA_LEN; j++, i+=4) {
			
		commandData.unInputParamsBuf[j ] = 
			inoutp_updateMultiContentKeyIO->aucTitleKeyBData[i] << 24 |
			inoutp_updateMultiContentKeyIO->aucTitleKeyBData[i+1] << 16 |
			inoutp_updateMultiContentKeyIO->aucTitleKeyBData[i+2] << 8 |
			inoutp_updateMultiContentKeyIO->aucTitleKeyBData[i+3]  ;
			
		commandData.unInputParamLen +=  4;
	}	

	commandData.unInputParamsBuf[(BCMD_UserMultipleKeys_CmdInputField_eAESVariant/4 - BCMD_UserMultipleKeys_CmdInputField_eKeyID0/4) ] = 
			inoutp_updateMultiContentKeyIO->aesVariant;
	commandData.unInputParamLen +=  (BCMD_UserMultipleKeys_CmdInputField_eNumContentKeys/4) * 4
		 - (BCMD_UserMultipleKeys_CmdInputField_eAESVariant/4) * 4;

	commandData.unInputParamsBuf[(BCMD_UserMultipleKeys_CmdInputField_eNumContentKeys/4 - BCMD_UserMultipleKeys_CmdInputField_eKeyID0/4) ] = 
			inoutp_updateMultiContentKeyIO->aucNumContentKeys;
	commandData.unInputParamLen +=  (BCMD_UserMultipleKeys_CmdInputField_eTitleKeyMap/4) * 4
		 - (BCMD_UserMultipleKeys_CmdInputField_eNumContentKeys/4) * 4;	

	commandData.unInputParamsBuf[(BCMD_UserMultipleKeys_CmdInputField_eTitleKeyMap/4 - BCMD_UserMultipleKeys_CmdInputField_eKeyID0/4) ] = 
			inoutp_updateMultiContentKeyIO->unTitleKeyIndexMap ; 
	commandData.unInputParamLen +=  (BCMD_UserMultipleKeys_CmdInputField_eKeySlotMapBase/4) * 4
		 - (BCMD_UserMultipleKeys_CmdInputField_eTitleKeyMap/4) * 4;

	for (i=0, 
		j = (BCMD_UserMultipleKeys_CmdInputField_eKeySlotMapBase/4 - BCMD_UserMultipleKeys_CmdInputField_eKeyID0/4);
		i<BHSM_MULTIPLE_CONTENT_MAX_KEY_SLOT_LEN; j++, i+=4) {
			
		commandData.unInputParamsBuf[j ] = 
			inoutp_updateMultiContentKeyIO->aucKeySlotsMap[i] << 24 |
			inoutp_updateMultiContentKeyIO->aucKeySlotsMap[i+1] << 16 |
			inoutp_updateMultiContentKeyIO->aucKeySlotsMap[i+2] << 8 |
			inoutp_updateMultiContentKeyIO->aucKeySlotsMap[i+3]  ;
			
		commandData.unInputParamLen +=  4;
	}

	for (k=0; k<BHSM_MULTIPLE_CONTENT_MAX_KEY_SLOT_SIZE; k++) {
		for (i=0, 
			j = (BCMD_UserMultipleKeys_CmdInputField_eContentKey0/4 - BCMD_UserMultipleKeys_CmdInputField_eKeyID0/4) 
				+ k * BHSM_MULTIPLE_CONTENT_CONTENT_KEY_DATA_LEN/4;
			i<BHSM_MULTIPLE_CONTENT_CONTENT_KEY_DATA_LEN; j++, i+=4) {
				
				commandData.unInputParamsBuf[j ] = 
					inoutp_updateMultiContentKeyIO->aucContentKeyData[k][i] << 24 |
					inoutp_updateMultiContentKeyIO->aucContentKeyData[k][i+1] << 16 |
					inoutp_updateMultiContentKeyIO->aucContentKeyData[k][i+2] << 8 |
					inoutp_updateMultiContentKeyIO->aucContentKeyData[k][i+3]  ;
					
				commandData.unInputParamLen +=  4;
		}
	}

	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

	/* Parse the command specific output parameters */
	inoutp_updateMultiContentKeyIO->unStatus =  commandData.unOutputParamsBuf[0];
	unParamLen += 4;	

 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_updateMultiContentKeyIO->unStatus != 0) );	
	
BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_UnwrapMultipleContentKeys);
	return( errCode );
}


BERR_Code  BHSM_CalculateMediaKey(
		BHSM_Handle				  	in_handle,
		BHSM_CalculateMediaKeyIO_t		*inoutp_calcMediaKeyIO
)
{
	BERR_Code errCode = BERR_SUCCESS;
	BHSM_ChannelHandle channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t unParamLen = 0, i , j;
	BHSM_P_CommandData_t commandData;


	BDBG_ENTER(BHSM_CalculateMediaKey);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	commandData.cmdId = BCMD_cmdType_eCALC_MEDIA_KEY;   /* 0x30*/
	commandData.unContMode = 0;
	
	/* only fill the command specific input parameters */
	commandData.unInputParamsBuf[(BCMD_UserMediaKey_CmdInputField_eDevKeyLoc/4 - BCMD_CommonBufferFields_eStatus/4)] = 
			inoutp_calcMediaKeyIO->eDevKeyLoc;

	commandData.unInputParamLen +=  (BCMD_UserMediaKey_CmdInputField_eVirtualKeyLadder/4) * 4
		 - (BCMD_UserMediaKey_CmdInputField_eDevKeyLoc/4) * 4;

	commandData.unInputParamsBuf[(BCMD_UserMediaKey_CmdInputField_eVirtualKeyLadder/4 - BCMD_UserMediaKey_CmdInputField_eDevKeyLoc/4) ] =
			inoutp_calcMediaKeyIO->eVirtualKeyLadder;
	commandData.unInputParamLen +=  (BCMD_UserMediaKey_CmdInputField_eUV/4) * 4
		 - (BCMD_UserMediaKey_CmdInputField_eVirtualKeyLadder/4) * 4;


	commandData.unInputParamsBuf[(BCMD_UserMediaKey_CmdInputField_eUV/4 - BCMD_UserMediaKey_CmdInputField_eDevKeyLoc/4) ] = 
			inoutp_calcMediaKeyIO->unUV;
	commandData.unInputParamLen +=  (BCMD_UserMediaKey_CmdInputField_eMv/4) * 4
		 - (BCMD_UserMediaKey_CmdInputField_eUV/4) * 4;

	commandData.unInputParamsBuf[(BCMD_UserMediaKey_CmdInputField_eMv/4 - BCMD_UserMediaKey_CmdInputField_eDevKeyLoc/4) ] = 
			inoutp_calcMediaKeyIO->unMaskValue;
	commandData.unInputParamLen +=  (BCMD_UserMediaKey_CmdInputField_eMvPrime/4) * 4
		 - (BCMD_UserMediaKey_CmdInputField_eMv/4) * 4;

	commandData.unInputParamsBuf[(BCMD_UserMediaKey_CmdInputField_eMvPrime/4 - BCMD_UserMediaKey_CmdInputField_eDevKeyLoc/4) ] = 
			inoutp_calcMediaKeyIO->unMaskValuePrime;
	commandData.unInputParamLen +=  (BCMD_UserMediaKey_CmdInputField_eMediaKeyData/4) * 4
		 - (BCMD_UserMediaKey_CmdInputField_eMvPrime/4) * 4;


	for (i=0, 
		j = (BCMD_UserMediaKey_CmdInputField_eMediaKeyData/4 - BCMD_UserMediaKey_CmdInputField_eDevKeyLoc/4);
		i<BHSM_MEDIA_KEY_DATA_LEN; j++, i+=4) {
			
		commandData.unInputParamsBuf[j ] = 
			inoutp_calcMediaKeyIO->aucMediaKeyData[i] << 24 |
			inoutp_calcMediaKeyIO->aucMediaKeyData[i+1] << 16 |
			inoutp_calcMediaKeyIO->aucMediaKeyData[i+2] << 8 |
			inoutp_calcMediaKeyIO->aucMediaKeyData[i+3]  ;
			
		commandData.unInputParamLen +=  4;
	}

	commandData.unInputParamsBuf[(BCMD_UserMediaKey_CmdInputField_eKCDFlag/4 - BCMD_UserMediaKey_CmdInputField_eDevKeyLoc/4) ] = 
			inoutp_calcMediaKeyIO->bKCDFlag;
	commandData.unInputParamLen +=  (BCMD_UserMediaKey_CmdInputField_eKCD/4) * 4
		 - (BCMD_UserMediaKey_CmdInputField_eKCDFlag/4) * 4;	

	for (i=0, 
		j = (BCMD_UserMediaKey_CmdInputField_eKCD/4 - BCMD_UserMediaKey_CmdInputField_eDevKeyLoc/4);
		i<BHSM_MEDIA_KCD_LEN; j++, i+=4) {
			
		commandData.unInputParamsBuf[j ] = 
			inoutp_calcMediaKeyIO->aucKCD[i] << 24 |
			inoutp_calcMediaKeyIO->aucKCD[i+1] << 16 |
			inoutp_calcMediaKeyIO->aucKCD[i+2] << 8 |
			inoutp_calcMediaKeyIO->aucKCD[i+3]  ;
			
		commandData.unInputParamLen +=  4;
	}

	for (i=0, 
		j = (BCMD_UserMediaKey_CmdInputField_eVolumeID/4 - BCMD_UserMediaKey_CmdInputField_eDevKeyLoc/4);
		i<BHSM_MEDIA_KEY_VOLUME_ID_LEN; j++, i+=4) {
			
		commandData.unInputParamsBuf[j ] = 
			inoutp_calcMediaKeyIO->aucVolumeID[i] << 24 |
			inoutp_calcMediaKeyIO->aucVolumeID[i+1] << 16 |
			inoutp_calcMediaKeyIO->aucVolumeID[i+2] << 8 |
			inoutp_calcMediaKeyIO->aucVolumeID[i+3]  ;
			
		commandData.unInputParamLen +=  4;
	}

	for (i=0, 
		j = (BCMD_UserMediaKey_CmdInputField_eVerificationData/4 - BCMD_UserMediaKey_CmdInputField_eDevKeyLoc/4);
		i<BHSM_MEDIA_KEY_VERIFICATION_DATA_LEN; j++, i+=4) {
			
		commandData.unInputParamsBuf[j ] = 
			inoutp_calcMediaKeyIO->aucVerificationData[i] << 24 |
			inoutp_calcMediaKeyIO->aucVerificationData[i+1] << 16 |
			inoutp_calcMediaKeyIO->aucVerificationData[i+2] << 8 |
			inoutp_calcMediaKeyIO->aucVerificationData[i+3]  ;
			
		commandData.unInputParamLen +=  4;
	}		



/*	printf("BHSM_CalculateMediaKey:  Media Key After Data length = %d\n", commandData.unInputParamLen );
	printf("BHSM_CalculateMediaKey: Media Key After key data :\n");	
	for ( i= 0; i<commandData.unInputParamLen/4; i++)
		printf ("data[%d]= %08lx\n ",i,commandData.unInputParamsBuf[i]);
*/

	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

	/* Parse the command specific output parameters */
	unParamLen=0;
	inoutp_calcMediaKeyIO->unStatus =  commandData.unOutputParamsBuf[unParamLen/4];
	unParamLen += 4;	

	inoutp_calcMediaKeyIO->unWhichKeyUpdated =  commandData.unOutputParamsBuf[unParamLen/4];
	unParamLen += 4;	

 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_calcMediaKeyIO->unStatus != 0) );	
	
BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_CalculateMediaKey);
	return( errCode );
}


BERR_Code  BHSM_CalculateUnifiedMediaKey(
		BHSM_Handle				  	in_handle,
		BHSM_CalculateUnifiedMediaKeyIO_t		*inoutp_calcUnifiedMediaKeyIO
)
{
	BERR_Code errCode = BERR_SUCCESS;
	BHSM_ChannelHandle channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t unParamLen = 0, i , j;
	BHSM_P_CommandData_t commandData;


	BDBG_ENTER(BHSM_CalculateUnifiedMediaKey);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED,
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );

	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	commandData.cmdId = BCMD_cmdType_eCALC_UNIFIED_MEDIA_KEY;   /* 0x3c */
	commandData.unContMode = 0;

	/* only fill the command specific input parameters */
	commandData.unInputParamsBuf[(BCMD_UserUnifiedMediaKey_CmdInputField_eDevKeyLoc/4 - BCMD_CommonBufferFields_eStatus/4)] =
			inoutp_calcUnifiedMediaKeyIO->eDevKeyLoc;
	commandData.unInputParamLen +=  (BCMD_UserUnifiedMediaKey_CmdInputField_eVirtualKeyLadder/4) * 4
		 - (BCMD_UserUnifiedMediaKey_CmdInputField_eDevKeyLoc/4) * 4;

	commandData.unInputParamsBuf[(BCMD_UserUnifiedMediaKey_CmdInputField_eVirtualKeyLadder/4 - BCMD_UserUnifiedMediaKey_CmdInputField_eDevKeyLoc/4) ] =
			inoutp_calcUnifiedMediaKeyIO->eVirtualKeyLadder;
	commandData.unInputParamLen +=  (BCMD_UserUnifiedMediaKey_CmdInputField_eUV/4) * 4
		 - (BCMD_UserMediaKey_CmdInputField_eVirtualKeyLadder/4) * 4;

	commandData.unInputParamsBuf[(BCMD_UserUnifiedMediaKey_CmdInputField_eUV/4 - BCMD_UserUnifiedMediaKey_CmdInputField_eDevKeyLoc/4) ] =
			inoutp_calcUnifiedMediaKeyIO->unUV;
	commandData.unInputParamLen +=  (BCMD_UserUnifiedMediaKey_CmdInputField_eMv/4) * 4
		 - (BCMD_UserUnifiedMediaKey_CmdInputField_eUV/4) * 4;

	commandData.unInputParamsBuf[(BCMD_UserUnifiedMediaKey_CmdInputField_eMv/4 - BCMD_UserUnifiedMediaKey_CmdInputField_eDevKeyLoc/4) ] =
			inoutp_calcUnifiedMediaKeyIO->unMaskValue;
	commandData.unInputParamLen +=  (BCMD_UserUnifiedMediaKey_CmdInputField_eMvPrime/4) * 4
		 - (BCMD_UserUnifiedMediaKey_CmdInputField_eMv/4) * 4;

	commandData.unInputParamsBuf[(BCMD_UserUnifiedMediaKey_CmdInputField_eMvPrime/4 - BCMD_UserUnifiedMediaKey_CmdInputField_eDevKeyLoc/4) ] =
			inoutp_calcUnifiedMediaKeyIO->unMaskValuePrime;
	commandData.unInputParamLen +=  (BCMD_UserUnifiedMediaKey_CmdInputField_eMediaKeyVariantData/4) * 4
		 - (BCMD_UserUnifiedMediaKey_CmdInputField_eMvPrime/4) * 4;


	for (i=0,
		j = (BCMD_UserUnifiedMediaKey_CmdInputField_eMediaKeyVariantData/4 - BCMD_UserUnifiedMediaKey_CmdInputField_eDevKeyLoc/4);
		i<BHSM_MEDIA_KEY_DATA_LEN; j++, i+=4) {

		commandData.unInputParamsBuf[j ] =
			inoutp_calcUnifiedMediaKeyIO->aucMediaKeyVariantData[i] << 24 |
			inoutp_calcUnifiedMediaKeyIO->aucMediaKeyVariantData[i+1] << 16 |
			inoutp_calcUnifiedMediaKeyIO->aucMediaKeyVariantData[i+2] << 8 |
			inoutp_calcUnifiedMediaKeyIO->aucMediaKeyVariantData[i+3]  ;

		commandData.unInputParamLen +=  4;
	}

	for (i=0,
		j = (BCMD_UserUnifiedMediaKey_CmdInputField_eNonce/4 - BCMD_UserUnifiedMediaKey_CmdInputField_eDevKeyLoc/4);
		i<BHSM_MEDIA_KCD_LEN; j++, i+=4) {

		commandData.unInputParamsBuf[j ] =
			inoutp_calcUnifiedMediaKeyIO->aucNonce[i] << 24 |
			inoutp_calcUnifiedMediaKeyIO->aucNonce[i+1] << 16 |
			inoutp_calcUnifiedMediaKeyIO->aucNonce[i+2] << 8 |
			inoutp_calcUnifiedMediaKeyIO->aucNonce[i+3]  ;

		commandData.unInputParamLen +=  4;
	}

	for (i=0,
		j = (BCMD_UserUnifiedMediaKey_CmdInputField_eVolumeID/4 - BCMD_UserUnifiedMediaKey_CmdInputField_eDevKeyLoc/4);
		i<BHSM_MEDIA_KEY_VOLUME_ID_LEN; j++, i+=4) {

		commandData.unInputParamsBuf[j ] =
			inoutp_calcUnifiedMediaKeyIO->aucVolumeID[i] << 24 |
			inoutp_calcUnifiedMediaKeyIO->aucVolumeID[i+1] << 16 |
			inoutp_calcUnifiedMediaKeyIO->aucVolumeID[i+2] << 8 |
			inoutp_calcUnifiedMediaKeyIO->aucVolumeID[i+3]  ;

		commandData.unInputParamLen +=  4;
	}

	for (i=0,
		j = (BCMD_UserUnifiedMediaKey_CmdInputField_eVerificationData/4 - BCMD_UserUnifiedMediaKey_CmdInputField_eDevKeyLoc/4);
		i<BHSM_MEDIA_KEY_VERIFICATION_DATA_LEN; j++, i+=4) {

		commandData.unInputParamsBuf[j ] =
			inoutp_calcUnifiedMediaKeyIO->aucVerificationData[i] << 24 |
			inoutp_calcUnifiedMediaKeyIO->aucVerificationData[i+1] << 16 |
			inoutp_calcUnifiedMediaKeyIO->aucVerificationData[i+2] << 8 |
			inoutp_calcUnifiedMediaKeyIO->aucVerificationData[i+3]  ;

		commandData.unInputParamLen +=  4;
	}

	BHSM_P_CHECK_ERR_CODE_FUNC(errCode,
			BHSM_P_CommonSubmitCommand (channelHandle,
					&commandData));

	/* Parse the command specific output parameters */
	unParamLen=0;
	inoutp_calcUnifiedMediaKeyIO->unStatus =  commandData.unOutputParamsBuf[unParamLen/4];
	unParamLen += 4;

	inoutp_calcUnifiedMediaKeyIO->unKvn =  commandData.unOutputParamsBuf[unParamLen/4];
	unParamLen += 4;

 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED,
		(inoutp_calcUnifiedMediaKeyIO->unStatus != 0) );

BHSM_P_DONE_LABEL:

	BDBG_LEAVE(BHSM_CalculateUnifiedMediaKey);
	return( errCode );
}



BERR_Code  BHSM_UpdateSecretKey(
		BHSM_Handle				  	in_handle,
		BHSM_UpdateSecretKeyIO_t		*inoutp_updateSecretKeyIO
)
{
	BERR_Code errCode = BERR_SUCCESS;
	BHSM_ChannelHandle channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t unParamLen = 0, i , j;
	BHSM_P_CommandData_t commandData;


	BDBG_ENTER(BHSM_UpdateSecretKey);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	commandData.cmdId = BCMD_cmdType_eUPDATE_SECRET_KEYS;   /* 0x34*/
	commandData.unContMode = 0;

	/* only fill the command specific input parameters */
	commandData.unInputParamsBuf[(BCMD_UserUpdateSecretKey_CmdInputField_eKeyID/4 - BCMD_CommonBufferFields_eStatus/4)] = 
			inoutp_updateSecretKeyIO->eKeyId;
	commandData.unInputParamLen +=  (BCMD_UserUpdateSecretKey_CmdInputField_eReserved6/4) * 4
		 - (BCMD_UserUpdateSecretKey_CmdInputField_eKeyID/4) * 4;


	for (i=0, 
		j = (BCMD_UserUpdateSecretKey_CmdInputField_eKeyData/4 - BCMD_UserUpdateSecretKey_CmdInputField_eKeyID/4);
		i<BHSM_UPDATE_SECRET_KEY_DATA_LEN; j++, i+=4) {
			
		commandData.unInputParamsBuf[j ] = 
			inoutp_updateSecretKeyIO->aucKeyData[i] << 24 |
			inoutp_updateSecretKeyIO->aucKeyData[i+1] << 16 |
			inoutp_updateSecretKeyIO->aucKeyData[i+2] << 8 |
			inoutp_updateSecretKeyIO->aucKeyData[i+3]  ;
			
		commandData.unInputParamLen +=  4;
	}

	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

	/* Parse the command specific output parameters */
	inoutp_updateSecretKeyIO->unStatus =  commandData.unOutputParamsBuf[0];
	unParamLen += 4;	

 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_updateSecretKeyIO->unStatus != 0) );	
	
BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_UpdateSecretKey);
	return( errCode );
}



BERR_Code  BHSM_LoadKeysAfterOperation(
		BHSM_Handle				  in_handle,
		BHSM_KeyAfterOpIO_t		*inoutp_loadKeyAfterOpIO
)
{
	BERR_Code 				errCode = BERR_SUCCESS;
	BHSM_ChannelHandle 		channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t 					unParamLen = 0, i , j;
	BHSM_P_CommandData_t 	commandData;


	BDBG_ENTER(BHSM_LoadKeysAfterOperation);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 	
		inoutp_loadKeyAfterOpIO->aucNumOfKeys > BCMD_KEYSAFTEROP_MAX);

	if (inoutp_loadKeyAfterOpIO->eKeySize == BCMD_KeySize_e192)
	{
		BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 	
					(inoutp_loadKeyAfterOpIO->aucNumOfKeys*24) > BCMD_KEYSAFTEROP_KEYDATA_MAX);		
	}

	if( ( inoutp_loadKeyAfterOpIO->eKeyBlkType != BCMD_KeyDestBlockType_eMem2Mem)
		&& ( inoutp_loadKeyAfterOpIO->eKeyBlkType != BCMD_KeyDestBlockType_eSharf)
	  )
	{
		BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 	1);		

	}

	if( ( inoutp_loadKeyAfterOpIO->unOptDetails[0] <= BCMD_KeyRamBuf_eSecretKey_Begin ) ||
 	    ( inoutp_loadKeyAfterOpIO->unOptDetails[0] >=  BCMD_KeyRamBuf_eSecretKey_End)		)
	{
		BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 	1);		
	}


	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	commandData.cmdId = BCMD_cmdType_eLOAD_KEYS_AFTER_OPERATION;    /* 0x33 */
	commandData.unContMode = 0;

	/* only fill the command specific input parameters */
	commandData.unInputParamsBuf[(BCMD_UserLoadKeyAfterOp_CmdInputField_eNumOfKeys/4 - BCMD_HEADER_BYTE_SIZE/4)] = 
			inoutp_loadKeyAfterOpIO->aucNumOfKeys;   /* 5 */

	commandData.unInputParamsBuf[(BCMD_UserLoadKeyAfterOp_CmdInputField_eKeySize/4 - BCMD_HEADER_BYTE_SIZE/4)] = 
			inoutp_loadKeyAfterOpIO->eKeySize;		/* 6 */

	commandData.unInputParamsBuf[(BCMD_UserLoadKeyAfterOp_CmdInputField_eBlkType/4 - BCMD_HEADER_BYTE_SIZE/4)] = 
			inoutp_loadKeyAfterOpIO->eKeyBlkType;	/* 7 */

	commandData.unInputParamsBuf[(BCMD_UserLoadKeyAfterOp_CmdInputField_eOpType/4 - BCMD_HEADER_BYTE_SIZE/4)] = 
			inoutp_loadKeyAfterOpIO->eOptType;		/* 8 */

	for (i=0; i<4;i++)
	{

		commandData.unInputParamsBuf[(BCMD_UserLoadKeyAfterOp_CmdInputField_eOpDetail0/4 - BCMD_HEADER_BYTE_SIZE/4)+i] = 
			inoutp_loadKeyAfterOpIO->unOptDetails[i];		/* 9~12 */
	}


	commandData.unInputParamsBuf[(BCMD_UserLoadKeyAfterOp_CmdInputField_eCustSubMode/4 - BCMD_HEADER_BYTE_SIZE/4)] =
				inoutp_loadKeyAfterOpIO->eCustSubMode;
	commandData.unInputParamsBuf[(BCMD_UserLoadKeyAfterOp_CmdInputField_eReserved14/4 - BCMD_HEADER_BYTE_SIZE/4)] = 
			inoutp_loadKeyAfterOpIO->unReserved14;		/* 14 */



	j= inoutp_loadKeyAfterOpIO->aucNumOfKeys/4;
	for (i=0; i< j*4; i +=4)
	{
		commandData.unInputParamsBuf[(BCMD_UserLoadKeyAfterOp_CmdInputField_eKeySlotBase/4 - BCMD_HEADER_BYTE_SIZE/4 + i/4)] 
			= inoutp_loadKeyAfterOpIO->aucKeySlotIndexMap[i]<< 24 |
			inoutp_loadKeyAfterOpIO->aucKeySlotIndexMap[i+1]<< 16 |
			inoutp_loadKeyAfterOpIO->aucKeySlotIndexMap[i+2]<< 8 |
			inoutp_loadKeyAfterOpIO->aucKeySlotIndexMap[i+3];		/* 15~18 */		
	}
	j = inoutp_loadKeyAfterOpIO->aucNumOfKeys%4 ;         /* i is carried on continuously*/
	if (j==1)
	{
		commandData.unInputParamsBuf[(BCMD_UserLoadKeyAfterOp_CmdInputField_eKeySlotBase/4 - BCMD_HEADER_BYTE_SIZE/4 + i/4)] 
			= inoutp_loadKeyAfterOpIO->aucKeySlotIndexMap[i]<< 24 ;		

	}else if( j==2)
	{
		commandData.unInputParamsBuf[(BCMD_UserLoadKeyAfterOp_CmdInputField_eKeySlotBase/4 - BCMD_HEADER_BYTE_SIZE/4 + i/4)] 
			= inoutp_loadKeyAfterOpIO->aucKeySlotIndexMap[i]<< 24 |
			inoutp_loadKeyAfterOpIO->aucKeySlotIndexMap[i+1]<< 16;
	
	}else if (j==3)
	{
		commandData.unInputParamsBuf[(BCMD_UserLoadKeyAfterOp_CmdInputField_eKeySlotBase/4 - BCMD_HEADER_BYTE_SIZE/4 + i/4)] 
			= inoutp_loadKeyAfterOpIO->aucKeySlotIndexMap[i]<< 24 |
			inoutp_loadKeyAfterOpIO->aucKeySlotIndexMap[i+1]<< 16 |
			inoutp_loadKeyAfterOpIO->aucKeySlotIndexMap[i+2]<< 8 ;	
	
	}else
	{
					/* 4/6/8/16 doing nothing*/
	}

	for (i=0; i<16; i+=4)
	{
		commandData.unInputParamsBuf[(BCMD_UserLoadKeyAfterOp_CmdInputField_eOperation_Data0/4 - BCMD_HEADER_BYTE_SIZE/4 + i/4)] 
						=	inoutp_loadKeyAfterOpIO->opData0XorWithKey[i]	<< 24|
							inoutp_loadKeyAfterOpIO->opData0XorWithKey[i+1] << 16|
							inoutp_loadKeyAfterOpIO->opData0XorWithKey[i+2] << 8|
							inoutp_loadKeyAfterOpIO->opData0XorWithKey[i+3];

	}

	for (i=0; i<16; i+=4)
	{
		commandData.unInputParamsBuf[(BCMD_UserLoadKeyAfterOp_CmdInputField_eOperation_Data1/4 - BCMD_HEADER_BYTE_SIZE/4 + i/4)] 
						=	inoutp_loadKeyAfterOpIO->opData1XorWithDataOut[i]	<< 24|
							inoutp_loadKeyAfterOpIO->opData1XorWithDataOut[i+1] << 16|
							inoutp_loadKeyAfterOpIO->opData1XorWithDataOut[i+2] << 8|
							inoutp_loadKeyAfterOpIO->opData1XorWithDataOut[i+3];

	}


	if (inoutp_loadKeyAfterOpIO->eKeySize == BCMD_KeySize_e64)
	{
		j = inoutp_loadKeyAfterOpIO->aucNumOfKeys *8;		
	}else if(inoutp_loadKeyAfterOpIO->eKeySize == BCMD_KeySize_e128)
	{
		j = inoutp_loadKeyAfterOpIO->aucNumOfKeys *16;		
	}else if(inoutp_loadKeyAfterOpIO->eKeySize == BCMD_KeySize_e192)
	{
		j = inoutp_loadKeyAfterOpIO->aucNumOfKeys *24;		
	}else{ j=0;}
	for (i=0; i<j ; i+=4)
	{
		commandData.unInputParamsBuf[(BCMD_UserLoadKeyAfterOp_CmdInputField_eKeyDataBase/4 - BCMD_HEADER_BYTE_SIZE/4 + i/4)] = 
			inoutp_loadKeyAfterOpIO->aucKeyData[i] << 24 |
			inoutp_loadKeyAfterOpIO->aucKeyData[i+1] << 16 |
			inoutp_loadKeyAfterOpIO->aucKeyData[i+2] << 8 |
			inoutp_loadKeyAfterOpIO->aucKeyData[i+3]  ;              /*  27~90  */
	}

	commandData.unInputParamsBuf[(BCMD_UserLoadKeyAfterOp_CmdInputField_eFree/4 - BCMD_HEADER_BYTE_SIZE/4)] = 
			inoutp_loadKeyAfterOpIO->unFree;				/* 91 */
	
	commandData.unInputParamLen = BCMD_UserLoadKeyAfterOp_CmdInputField_eDummyLast - BCMD_HEADER_BYTE_SIZE;

/*	printf("BHSM_LoadKeysAfterOperation: Load Key After Data length = %d\n", commandData.unInputParamLen );
	printf("BHSM_LoadKeysAfterOperation: Load Key After key data :\n");	
	for ( i= 0; i<commandData.unInputParamLen/4; i++)
		printf ("data[%d]= %08lx\n ",i,commandData.unInputParamsBuf[i]);
*/	
	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

	/* Parse the command specific output parameters */
	inoutp_loadKeyAfterOpIO->unStatus =  commandData.unOutputParamsBuf[0];
	unParamLen += 4;	

 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_loadKeyAfterOpIO->unStatus != 0) );	
	
BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_LoadKeysAfterOperation);
	return( errCode );
}

BERR_Code  BHSM_LoadSecretKey(
		BHSM_Handle				 	 in_handle,
		BHSM_LoadSecretKeyIO_t		*inoutp_loadSecretKeyIO
)
{
	BERR_Code 				errCode = BERR_SUCCESS;
	BHSM_ChannelHandle 		channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t 					unParamLen = 0;
	BHSM_P_CommandData_t 	commandData;


	BDBG_ENTER(BHSM_LoadSecretKey);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	if( ( inoutp_loadSecretKeyIO->eKeyBlkType != BCMD_KeyDestBlockType_eMem2Mem)
		&& ( inoutp_loadSecretKeyIO->eKeyBlkType != BCMD_KeyDestBlockType_eSharf) 
	   )
	{
		BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 	1);		
	}

	if( ( inoutp_loadSecretKeyIO->eKeyId <= BCMD_KeyRamBuf_eSecretKey_Begin ) ||
 	    ( inoutp_loadSecretKeyIO->eKeyId >=  BCMD_KeyRamBuf_eSecretKey_End)		)
	{
		BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 	1);		
	}

	if(  inoutp_loadSecretKeyIO->unKeySlot >= BCMD_LOADSECRETKEY_KEYSLOT_MAX)
	{
		BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 	1);		
	}


	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	commandData.cmdId = BCMD_cmdType_eLOAD_SECRET_KEY;    /* 0x32 */
	commandData.unContMode = 0;

	/* only fill the command specific input parameters */
	commandData.unInputParamsBuf[(BCMD_UserLoadSecretKey_CmdInputField_eKeyID/4 - BCMD_HEADER_BYTE_SIZE/4)] = 
			inoutp_loadSecretKeyIO->eKeyId;   /* 5 */

	commandData.unInputParamsBuf[(BCMD_UserLoadSecretKey_CmdInputField_eBlkType/4 - BCMD_HEADER_BYTE_SIZE/4)] = 
			inoutp_loadSecretKeyIO->eKeyBlkType;   /* 6 */


	commandData.unInputParamsBuf[(BCMD_UserLoadSecretKey_CmdInputField_eCustSubMode/4 - BCMD_HEADER_BYTE_SIZE/4)] =
			inoutp_loadSecretKeyIO->eCustSubMode;   /* 7 */

	commandData.unInputParamsBuf[(BCMD_UserLoadSecretKey_CmdInputField_eSlotNum/4 - BCMD_HEADER_BYTE_SIZE/4)] = 
			inoutp_loadSecretKeyIO->unKeySlot;   /* 8 */
	
	commandData.unInputParamLen = BCMD_UserLoadSecretKey_CmdInputField_eEnd - BCMD_HEADER_BYTE_SIZE;

	
	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));
	
	/* Parse the command specific output parameters */
	unParamLen=0;

	/* returned field by field. command specific data only. the common header (20 bytes) already checked */
	inoutp_loadSecretKeyIO->unStatus =  commandData.unOutputParamsBuf[0];  
	unParamLen += 4;	

 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_loadSecretKeyIO->unStatus != 0) );	
	
BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_LoadSecretKey);
	return( errCode );



}




