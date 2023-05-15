/***************************************************************************
 *     (c)2007-2011 Broadcom Corporation
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
 * $brcm_Workfile: bhsm_keyladder.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 10/15/12 11:53a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/src/core40nm/keyladder/bhsm_keyladder.c $
 * 
 * Hydra_Software_Devel/5   10/15/12 11:53a atruong
 * SWSECURITY-187: Bug fix for M2M key slot allocation - CA Key slot data
 * overlapped
 * 
 * Hydra_Software_Devel/4   9/11/12 2:56p atruong
 * SWSECURITY-175: Remove symbolic links in HSM PI
 * 
 * Hydra_Software_Devel/3   1/14/12 2:13a atruong
 * SWSECURITY-71:Defining BHSM_FastLoadEncryptedHdcpKey for HDCP Support
 * on 40-nm
 * 
 * Hydra_Software_Devel/2   10/18/11 4:53p atruong
 * SWSECURITY-69: Support for 40-nm B0 Zeus 2.0 platforms
 * 
 * Hydra_Software_Devel/3   6/29/11 7:09p atruong
 * SW7425-812:HSM PI API modification for new HMAC-SHA support
 * 
 * Hydra_Software_Devel/2   1/24/11 10:53a atruong
 * SW7422-26: Add HSM PI Support For 7422A0  - CC header format
 *
 ******************************************************************************/

#include "bchp_bsp_control_intr2.h"
#include "bchp_bsp_glb_control.h"

#include "bhsm_keyladder.h"



BDBG_MODULE(BHSM);

/* Global variable */
bool						bHSMPIRunningFullROM  =  false;



BERR_Code BHSM_InitKeySlot (
		BHSM_Handle				in_handle,
		BHSM_InitKeySlotIO_t	*inoutp_initKeySlotIO
)
{
	BERR_Code            errCode = BERR_SUCCESS;
	BHSM_ChannelHandle   channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t             unParamLen = 0, i, j, k;
	BHSM_P_CommandData_t commandData;
	uint32_t             regVal;

#if (BHSM_SECURE_PI_SUPPORT_KEYLADDER == 1)
	BHSM_VKLCustSubModeAssociationIO_t	inoutp_VKLCustSubModeAssociationIO;
#endif

	
	BDBG_ENTER(BHSM_InitKeySlot);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BDBG_ASSERT( inoutp_initKeySlotIO );
	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	/* First to find out if we are running with FullROM or BSECK code */
	regVal = BREG_Read32(channelHandle->moduleHandle->regHandle, BCHP_BSP_GLB_CONTROL_FW_FLAGS);
#if defined(HUMAX_PLATFORM_BASE)
	if (((regVal & 0x07000000) == 0x07000000) ||
		((regVal & 0x08000000) == 0x08000000))       /* we are running with BSECK code */
#else
	if ((regVal & 0x07000000) == 0x07000000)   /* we are running with BSECK code */
#endif	
	{
		bHSMPIRunningFullROM = false;
		BDBG_MSG(("BHSM_InitKeySlot -- Running with BSECK code."));  

	}
	else
	{
		bHSMPIRunningFullROM = true;
		BDBG_MSG(("BHSM_InitKeySlot -- Running with FullROM code."));  
	}
#if HSM_IS_ASKM_40NM_ZEUS_1_0
    if (bHSMPIRunningFullROM)
	{

		/* Keep track of the number of key slots per key slot type */
		in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType0].unKeySlotNum =
					inoutp_initKeySlotIO->unKeySlotType0Num;
		in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType0].ucKeySlotStartOffset = 0;

		in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType1].unKeySlotNum = 
					inoutp_initKeySlotIO->unKeySlotType1Num;
		in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType1].ucKeySlotStartOffset = 
			in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType0].ucKeySlotStartOffset +
			inoutp_initKeySlotIO->unKeySlotType0Num;
		
		in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType2].unKeySlotNum = 
					inoutp_initKeySlotIO->unKeySlotType2Num;
		in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType2].ucKeySlotStartOffset = 
			in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType1].ucKeySlotStartOffset +
			inoutp_initKeySlotIO->unKeySlotType1Num;
			
		in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType3].unKeySlotNum= 
					inoutp_initKeySlotIO->unKeySlotType3Num;
		in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType3].ucKeySlotStartOffset = 
			in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType2].ucKeySlotStartOffset +
			inoutp_initKeySlotIO->unKeySlotType2Num;

		in_handle->aunCAKeySlotTypeInfo[ BCMD_XptSecKeySlot_eType4].unKeySlotNum = 
					inoutp_initKeySlotIO->unKeySlotType4Num;
		in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType4].ucKeySlotStartOffset = 
			in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType3].ucKeySlotStartOffset +
			inoutp_initKeySlotIO->unKeySlotType3Num; 

		in_handle->unTotalCAKeySlotNum = 
						inoutp_initKeySlotIO->unKeySlotType0Num +
						inoutp_initKeySlotIO->unKeySlotType1Num +
						inoutp_initKeySlotIO->unKeySlotType2Num +
						inoutp_initKeySlotIO->unKeySlotType3Num +
						inoutp_initKeySlotIO->unKeySlotType4Num;

		for (i = 0; i < BCMD_XptSecKeySlot_eTypeMax; i++)	
		{
			k = in_handle->aunCAKeySlotTypeInfo[i].ucKeySlotStartOffset;
			for (j=0; j < in_handle->aunCAKeySlotTypeInfo[i].unKeySlotNum; j++) 
			{
				in_handle->aunCAKeySlotInfo[k + j].keySlotType = i;	
				in_handle->aunCAKeySlotInfo[k + j].bIsUsed     = false;
			}
		}
		
			
		commandData.cmdId = BCMD_cmdType_eSESSION_INIT_KEYSLOT;
		commandData.unContMode = 0;    

		/* only fill the command specific input parameters */
		commandData.unInputParamsBuf[0] = 
				inoutp_initKeySlotIO->unKeySlotType0Num |
				(inoutp_initKeySlotIO->bConfigMulti2KeySlot << 24);
		commandData.unInputParamLen += 4;

		commandData.unInputParamsBuf[commandData.unInputParamLen /4] = 
				inoutp_initKeySlotIO->unKeySlotType1Num;
		commandData.unInputParamLen += 4;

		commandData.unInputParamsBuf[commandData.unInputParamLen /4] = 
				inoutp_initKeySlotIO->unKeySlotType2Num;
		commandData.unInputParamLen += 4;

		commandData.unInputParamsBuf[commandData.unInputParamLen /4] = 
				inoutp_initKeySlotIO->unKeySlotType3Num;	
		commandData.unInputParamLen += 4;

		commandData.unInputParamsBuf[commandData.unInputParamLen /4] = 
				inoutp_initKeySlotIO->unKeySlotType4Num;
		commandData.unInputParamLen += 4;
		
		BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
				BHSM_P_CommonSubmitCommand (channelHandle, 
						&commandData));

		unParamLen = 0;
		/* Parse the command specific output parameters */
		inoutp_initKeySlotIO->unStatus =  commandData.unOutputParamsBuf[0];
		unParamLen += 4;	

		BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
			(inoutp_initKeySlotIO->unStatus != 0) );
	}
	else
	{

#endif

	/* send the command to BSP FW first  */
	commandData.cmdId = BCMD_cmdType_eSESSION_INIT_KEYSLOT;
	commandData.unContMode = 0;    
	
	/* only fill the command specific input parameters */
	commandData.unInputParamsBuf[0] = 
			inoutp_initKeySlotIO->unKeySlotType0Num |
			(inoutp_initKeySlotIO->bConfigMulti2KeySlot << 24);
	commandData.unInputParamLen += 4;
	
	commandData.unInputParamsBuf[commandData.unInputParamLen /4] = 
			inoutp_initKeySlotIO->unKeySlotType1Num;
	commandData.unInputParamLen += 4;
	
	commandData.unInputParamsBuf[commandData.unInputParamLen /4] = 
			inoutp_initKeySlotIO->unKeySlotType2Num;
	commandData.unInputParamLen += 4;
	
	commandData.unInputParamsBuf[commandData.unInputParamLen /4] = 
			inoutp_initKeySlotIO->unKeySlotType3Num;	
	commandData.unInputParamLen += 4;
	
	commandData.unInputParamsBuf[commandData.unInputParamLen /4] = 
			inoutp_initKeySlotIO->unKeySlotType4Num;
	commandData.unInputParamLen += 4;
	
	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));
	
	unParamLen = 0;
	/* Parse the command specific output parameters */
	inoutp_initKeySlotIO->unStatus =  commandData.unOutputParamsBuf[0];
	unParamLen += 4;	
	
	/* the command either returns successfully or this is not the first command sent */
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		((inoutp_initKeySlotIO->unStatus != 0) && (inoutp_initKeySlotIO->unStatus != 0x20) ));
	
	/* start reading the parameters the BSP FW returns */
	j = 1;                          /* start with the first returned dword */
	in_handle->bMulti2SysKey = (bool)commandData.unOutputParamsBuf[j++];
	inoutp_initKeySlotIO->bMulti2SysKey = in_handle->bMulti2SysKey;
	
	/* Save the number of key slots per key slot type, returned by BSP FW */
	/* construct the HSM PI internal key slot table */

	/* eType0 key slots */
	inoutp_initKeySlotIO->numKeySlotType0 = commandData.unOutputParamsBuf[j++];
	in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType0].unKeySlotNum =
				inoutp_initKeySlotIO->numKeySlotType0;
	in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType0].ucKeySlotStartOffset = 0;

	
	/* eType1 key slots */
	inoutp_initKeySlotIO->numKeySlotType1 = commandData.unOutputParamsBuf[j++];
	in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType1].unKeySlotNum = 
				inoutp_initKeySlotIO->numKeySlotType1;
	in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType1].ucKeySlotStartOffset = 
		in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType0].ucKeySlotStartOffset +
		inoutp_initKeySlotIO->numKeySlotType0;
	
	/* eType2 key slots */
	inoutp_initKeySlotIO->numKeySlotType2 = commandData.unOutputParamsBuf[j++];
	in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType2].unKeySlotNum = 
				inoutp_initKeySlotIO->numKeySlotType2;
	in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType2].ucKeySlotStartOffset = 
		in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType1].ucKeySlotStartOffset +
		inoutp_initKeySlotIO->numKeySlotType1;
		
	/* eType3 key slots */
	inoutp_initKeySlotIO->numKeySlotType3 = commandData.unOutputParamsBuf[j++];
	in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType3].unKeySlotNum= 
				inoutp_initKeySlotIO->numKeySlotType3;
	in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType3].ucKeySlotStartOffset = 
		in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType2].ucKeySlotStartOffset +
		inoutp_initKeySlotIO->numKeySlotType2;

	/* eType4 key slots */
	inoutp_initKeySlotIO->numKeySlotType4 = commandData.unOutputParamsBuf[j++];
	in_handle->aunCAKeySlotTypeInfo[ BCMD_XptSecKeySlot_eType4].unKeySlotNum = 
				inoutp_initKeySlotIO->numKeySlotType4;
	in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType4].ucKeySlotStartOffset = 
		in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType3].ucKeySlotStartOffset +
		inoutp_initKeySlotIO->numKeySlotType3; 

	in_handle->unTotalCAKeySlotNum = 
					inoutp_initKeySlotIO->numKeySlotType0 +
					inoutp_initKeySlotIO->numKeySlotType1 +
					inoutp_initKeySlotIO->numKeySlotType2 +
					inoutp_initKeySlotIO->numKeySlotType3 +
					inoutp_initKeySlotIO->numKeySlotType4;

	for (i = 0; i < BCMD_XptSecKeySlot_eTypeMax; i++)	
	{
		k = in_handle->aunCAKeySlotTypeInfo[i].ucKeySlotStartOffset;
		for (j=0; j < in_handle->aunCAKeySlotTypeInfo[i].unKeySlotNum; j++) 
		{
			in_handle->aunCAKeySlotInfo[k + j].keySlotType = i;	
			in_handle->aunCAKeySlotInfo[k + j].bIsUsed     = false;
		}
	}

	/* Next we need to send a command to query the VKL-CustSubMode association table */
#if (BHSM_SECURE_PI_SUPPORT_KEYLADDER == 1)

	BKNI_Memset(&inoutp_VKLCustSubModeAssociationIO, 0, sizeof(BHSM_VKLCustSubModeAssociationIO_t));
	inoutp_VKLCustSubModeAssociationIO.keyLadderType      = BCMD_KeyLadderType_e3DESABA;
#if HSM_IS_ASKM_40NM_ZEUS_1_0
	inoutp_VKLCustSubModeAssociationIO.subCmdID 		  = BCMD_KeyGenSubCmdID_eVKLAssociationQuery;
#else
	inoutp_VKLCustSubModeAssociationIO.subCmdID           = BCMD_VKLAssociationQueryFlag_eQuery;
	inoutp_VKLCustSubModeAssociationIO.keyGenMode		  = BCMD_KeyGenFlag_eNoGen; 
#endif
	inoutp_VKLCustSubModeAssociationIO.virtualKeyLadderID = BCMD_VKL0;
	inoutp_VKLCustSubModeAssociationIO.keyLayer           = BCMD_KeyRamBuf_eKey3;
	errCode = BHSM_RetrieveVKL_CustSubModeAssociation(in_handle, &inoutp_VKLCustSubModeAssociationIO);

#endif	

#if HSM_IS_ASKM_40NM_ZEUS_1_0
	
	}
#endif

BHSM_P_DONE_LABEL:

	BDBG_LEAVE(BHSM_InitKeySlot);
	return( errCode );
}


BERR_Code BHSM_AllocateCAKeySlot (
		BHSM_Handle					in_handle,
		BCMD_XptSecKeySlot_e		in_keySlotType,
		unsigned int				*outp_unKeySlotNum
)
{
	BERR_Code		errCode = BERR_SUCCESS;
	unsigned int	i;


	/*BDBG_MSG(("BHSM_AllocateCAKeySlot in_keySlotType=%d\n", in_keySlotType));*/

	BDBG_ENTER(BHSM_AllocateCAKeySlot);
	BDBG_ASSERT( in_handle );

	*outp_unKeySlotNum = BHSM_SLOT_NUM_INIT_VAL;	
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	/* Return if key slot type not supported */
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
		(in_keySlotType >= BCMD_XptSecKeySlot_eTypeMax) );

	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, BKNI_AcquireMutex( in_handle->caKeySlotMutexLock));
	
	/* Search for vacant key slot */
	for (i = in_handle->aunCAKeySlotTypeInfo[in_keySlotType].ucKeySlotStartOffset;
	     i < in_handle->aunCAKeySlotTypeInfo[in_keySlotType].ucKeySlotStartOffset+
	         in_handle->aunCAKeySlotTypeInfo[in_keySlotType].unKeySlotNum; i++) 
	{
		if (in_handle->aunCAKeySlotInfo[i].bIsUsed == false) 
		{
			/* Found */
			in_handle->aunCAKeySlotInfo[i].bIsUsed = true;
			in_handle->aunCAKeySlotInfo[i].aKeySlotAlgorithm[BCMD_KeyDestEntryType_eOddKey].ulCAControlWordHi  = 0xFFFFFFFF;
			in_handle->aunCAKeySlotInfo[i].aKeySlotAlgorithm[BCMD_KeyDestEntryType_eOddKey].ulCAControlWordLo  = 0xFFFFFFFF;
			in_handle->aunCAKeySlotInfo[i].aKeySlotAlgorithm[BCMD_KeyDestEntryType_eOddKey].ulCPDControlWordHi = 0xFFFFFFFF;
			in_handle->aunCAKeySlotInfo[i].aKeySlotAlgorithm[BCMD_KeyDestEntryType_eOddKey].ulCPDControlWordLo = 0xFFFFFFFF;
			in_handle->aunCAKeySlotInfo[i].aKeySlotAlgorithm[BCMD_KeyDestEntryType_eOddKey].ulCPSControlWordHi = 0xFFFFFFFF;
			in_handle->aunCAKeySlotInfo[i].aKeySlotAlgorithm[BCMD_KeyDestEntryType_eOddKey].ulCPSControlWordLo = 0xFFFFFFFF;

			in_handle->aunCAKeySlotInfo[i].aKeySlotAlgorithm[BCMD_KeyDestEntryType_eEvenKey].ulCAControlWordHi  = 0xFFFFFFFF;
			in_handle->aunCAKeySlotInfo[i].aKeySlotAlgorithm[BCMD_KeyDestEntryType_eEvenKey].ulCAControlWordLo  = 0xFFFFFFFF;
			in_handle->aunCAKeySlotInfo[i].aKeySlotAlgorithm[BCMD_KeyDestEntryType_eEvenKey].ulCPDControlWordHi = 0xFFFFFFFF;
			in_handle->aunCAKeySlotInfo[i].aKeySlotAlgorithm[BCMD_KeyDestEntryType_eEvenKey].ulCPDControlWordLo = 0xFFFFFFFF;
			in_handle->aunCAKeySlotInfo[i].aKeySlotAlgorithm[BCMD_KeyDestEntryType_eEvenKey].ulCPSControlWordHi = 0xFFFFFFFF;
			in_handle->aunCAKeySlotInfo[i].aKeySlotAlgorithm[BCMD_KeyDestEntryType_eEvenKey].ulCPSControlWordLo = 0xFFFFFFFF;

			in_handle->aunCAKeySlotInfo[i].aKeySlotAlgorithm[BCMD_KeyDestEntryType_eReserved2].ulCAControlWordHi  = 0xFFFFFFFF;
			in_handle->aunCAKeySlotInfo[i].aKeySlotAlgorithm[BCMD_KeyDestEntryType_eReserved2].ulCAControlWordLo  = 0xFFFFFFFF;
			in_handle->aunCAKeySlotInfo[i].aKeySlotAlgorithm[BCMD_KeyDestEntryType_eReserved2].ulCPDControlWordHi = 0xFFFFFFFF;
			in_handle->aunCAKeySlotInfo[i].aKeySlotAlgorithm[BCMD_KeyDestEntryType_eReserved2].ulCPDControlWordLo = 0xFFFFFFFF;
			in_handle->aunCAKeySlotInfo[i].aKeySlotAlgorithm[BCMD_KeyDestEntryType_eReserved2].ulCPSControlWordHi = 0xFFFFFFFF;
			in_handle->aunCAKeySlotInfo[i].aKeySlotAlgorithm[BCMD_KeyDestEntryType_eReserved2].ulCPSControlWordLo = 0xFFFFFFFF;

			in_handle->aunCAKeySlotInfo[i].KeySlotIDData[BCMD_KeyDestEntryType_eOddKey].ulCAVendorID             = 0;
			in_handle->aunCAKeySlotInfo[i].KeySlotIDData[BCMD_KeyDestEntryType_eOddKey].ulModuleIDKey2Select     = 0;
			in_handle->aunCAKeySlotInfo[i].KeySlotIDData[BCMD_KeyDestEntryType_eOddKey].ulSTBOwnerIDSelect       = 0;
			
			in_handle->aunCAKeySlotInfo[i].KeySlotIDData[BCMD_KeyDestEntryType_eEvenKey].ulCAVendorID            = 0;
			in_handle->aunCAKeySlotInfo[i].KeySlotIDData[BCMD_KeyDestEntryType_eEvenKey].ulModuleIDKey2Select    = 0;
			in_handle->aunCAKeySlotInfo[i].KeySlotIDData[BCMD_KeyDestEntryType_eEvenKey].ulSTBOwnerIDSelect      = 0;

			in_handle->aunCAKeySlotInfo[i].KeySlotIDData[BCMD_KeyDestEntryType_eReserved2].ulCAVendorID          = 0;
			in_handle->aunCAKeySlotInfo[i].KeySlotIDData[BCMD_KeyDestEntryType_eReserved2].ulModuleIDKey2Select  = 0;
			in_handle->aunCAKeySlotInfo[i].KeySlotIDData[BCMD_KeyDestEntryType_eReserved2].ulSTBOwnerIDSelect    = 0;

			break;
		}
	}
	
	/* Return if there is no vacant key slot or key slot type mismatched */
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(i == (in_handle->aunCAKeySlotTypeInfo[in_keySlotType].ucKeySlotStartOffset+
		       in_handle->aunCAKeySlotTypeInfo[in_keySlotType].unKeySlotNum)));

	/* Here is the vacant key slot */
	*outp_unKeySlotNum = i - in_handle->aunCAKeySlotTypeInfo[in_keySlotType].ucKeySlotStartOffset;

	/*BDBG_MSG(("BHSM_AllocateCAKeySlot  outp_unKeySlotNum = %d errCode=%X\n", *outp_unKeySlotNum, errCode));*/

	
BHSM_P_DONE_LABEL:

	/* Unlock mutex */
	BKNI_ReleaseMutex( in_handle->caKeySlotMutexLock);		
	
	BDBG_LEAVE(BHSM_AllocateCAKeySlot);
      BDBG_MSG(("BHSM_AllocateCAKeySlot in_keySlotType=%d outp_unKeySlotNum = %d errCode=%X\n", in_keySlotType, *outp_unKeySlotNum, errCode));	
	return( errCode );

}




BERR_Code BHSM_FreeCAKeySlot (
		BHSM_Handle					in_handle,
		unsigned int				in_unPidChannel,
		BHSM_PidChannelType_e		in_pidChannelType,		
		BCMD_XptSecKeySlot_e		in_keySlotType,
		unsigned int				in_unKeySlotNum
)
{
	BERR_Code      errCode = BERR_SUCCESS;
	uint8_t        ucKeySlotStartOffset;
	uint16_t       pidChannel, pidChannelType;


	BDBG_ENTER(BHSM_FreeCAKeySlot);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );
	
	/* perform parameter check on in_keySlotType and in_unKeySlotNum */
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
		(in_handle->aunCAKeySlotTypeInfo[in_keySlotType].unKeySlotNum < in_unKeySlotNum ) );
	

	ucKeySlotStartOffset = 
		in_handle->aunCAKeySlotTypeInfo[in_keySlotType].ucKeySlotStartOffset;
 	

	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, BKNI_AcquireMutex( in_handle->caKeySlotMutexLock));


	BDBG_MSG(("BHSM_FreeCAKeySlot  aunCAKeySlotInfo[%d] = %d\n", 
		(ucKeySlotStartOffset+in_unKeySlotNum), 
		in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+in_unKeySlotNum)].bIsUsed));	
	
	/* Return if the key slot is already empty */
	if (in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+in_unKeySlotNum)].bIsUsed == false)  {
		goto BHSM_P_DONE_LABEL;
	}

	in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+in_unKeySlotNum)].bIsUsed = false;

	if (in_unPidChannel >= DUMMY_PID_CHANNEL_TOAKEYSLOT) 
	{ 
		/* does nothing to avoid risky memory modification */
		/* For NSK2, go through the entire association table to fish out the association */
		for (pidChannel = 0; pidChannel < BCMD_TOTAL_PIDCHANNELS; pidChannel++)
			for (pidChannelType = 0; pidChannelType < BHSM_PidChannelType_eMax; pidChannelType++)
			{
				if ((in_handle->aPidChannelToCAKeySlotNum[pidChannel][pidChannelType].keySlotType == in_keySlotType) &&
				    (in_handle->aPidChannelToCAKeySlotNum[pidChannel][pidChannelType].unKeySlotNum == in_unKeySlotNum))
				{
					/* Dissociate the pid channel from the key slot */
					in_handle->aPidChannelToCAKeySlotNum[pidChannel][pidChannelType].keySlotType  = BCMD_XptSecKeySlot_eTypeMax ;	
					in_handle->aPidChannelToCAKeySlotNum[pidChannel][pidChannelType].unKeySlotNum = BHSM_SLOT_NUM_INIT_VAL; 
					goto BHSM_P_DONE_LABEL;
				}
			}
	} 
	else
	{
		/* De-associate the pid channel from the key slot */
		in_handle->aPidChannelToCAKeySlotNum[in_unPidChannel][in_pidChannelType].keySlotType  = BCMD_XptSecKeySlot_eTypeMax ;	
		in_handle->aPidChannelToCAKeySlotNum[in_unPidChannel][in_pidChannelType].unKeySlotNum = BHSM_SLOT_NUM_INIT_VAL;			
    }
	  
BHSM_P_DONE_LABEL:
	
	/* Unlock mutex */
	BKNI_ReleaseMutex( in_handle->caKeySlotMutexLock);			
	
	BDBG_LEAVE(BHSM_FreeCAKeySlot);
	return( errCode );

}

BERR_Code BHSM_LocateCAKeySlotAssigned (
		BHSM_Handle					in_handle,
		unsigned int				in_unPidChannel,
		BHSM_PidChannelType_e		in_pidChannelType,
		BCMD_XptSecKeySlot_e		*outp_ucKeySlotType,
		unsigned int				*outp_unKeySlotNum
)
{
	BERR_Code errCode = BERR_SUCCESS;


	BDBG_ENTER(BHSM_LocateCAKeySlotAssigned);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, BKNI_AcquireMutex( in_handle->caKeySlotMutexLock));

	BDBG_MSG(("BHSM_HasKeySlotAssigned in_unPidChannel=%d, in_pidChannelType = %d\n", in_unPidChannel, in_pidChannelType));
	BDBG_MSG(("keySlotType = %d, unKeySlotNum = %d\n",
	in_handle->aPidChannelToCAKeySlotNum[in_unPidChannel][in_pidChannelType].keySlotType ,
	in_handle->aPidChannelToCAKeySlotNum[in_unPidChannel][in_pidChannelType].unKeySlotNum));


	if ( (in_handle->aPidChannelToCAKeySlotNum[in_unPidChannel][in_pidChannelType].keySlotType 
			!= BCMD_XptSecKeySlot_eTypeMax ) &&
		(in_handle->aPidChannelToCAKeySlotNum[in_unPidChannel][in_pidChannelType].unKeySlotNum 
			!=  BHSM_SLOT_NUM_INIT_VAL) ) 
	{
		*outp_ucKeySlotType = in_handle->aPidChannelToCAKeySlotNum[in_unPidChannel][in_pidChannelType].keySlotType ;	
		*outp_unKeySlotNum = in_handle->aPidChannelToCAKeySlotNum[in_unPidChannel][in_pidChannelType].unKeySlotNum ;			
	}
	else 
	{
		*outp_ucKeySlotType = BCMD_XptSecKeySlot_eTypeMax ;	
		*outp_unKeySlotNum = BHSM_SLOT_NUM_INIT_VAL;			
	}
	
	
BHSM_P_DONE_LABEL:

	/* Unlock mutex */
	BKNI_ReleaseMutex( in_handle->caKeySlotMutexLock);	
	
	BDBG_LEAVE(BHSM_LocateCAKeySlotAssigned);
	return( errCode );

}

BERR_Code BHSM_AllocateM2MKeySlot (
		BHSM_Handle					in_handle,
		unsigned int				*outp_unKeySlotNum
)
{
	BERR_Code      errCode = BERR_SUCCESS;
	unsigned int   i;
 

	BDBG_ENTER(BHSM_AllocateM2MKeySlot);
	BDBG_ASSERT( in_handle );

	*outp_unKeySlotNum = BHSM_SLOT_NUM_INIT_VAL;	
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, BKNI_AcquireMutex( in_handle->m2mKeySlotMutexLock));
	
	/* Search for vacant key slot */
	for (i=0;	i<BCMD_MAX_M2M_KEY_SLOT; i++) {
		
		if (in_handle->aunM2MKeySlotInfo[i].bIsUsed == false) 
		{
			/* Found */
			in_handle->aunM2MKeySlotInfo[i].bIsUsed = true;

			in_handle->aunM2MKeySlotInfo[i].aKeySlotAlgorithm[BCMD_KeyDestEntryType_eOddKey].ulM2MControlWordHi  = 0xFFFFFFFF;
			in_handle->aunM2MKeySlotInfo[i].aKeySlotAlgorithm[BCMD_KeyDestEntryType_eOddKey].ulM2MControlWordLo  = 0xFFFFFFFF;

			in_handle->aunM2MKeySlotInfo[i].aKeySlotAlgorithm[BCMD_KeyDestEntryType_eEvenKey].ulM2MControlWordHi  = 0xFFFFFFFF;
			in_handle->aunM2MKeySlotInfo[i].aKeySlotAlgorithm[BCMD_KeyDestEntryType_eEvenKey].ulM2MControlWordLo  = 0xFFFFFFFF;

			in_handle->aunM2MKeySlotInfo[i].aKeySlotAlgorithm[BCMD_KeyDestEntryType_eReserved2].ulM2MControlWordHi  = 0xFFFFFFFF;
			in_handle->aunM2MKeySlotInfo[i].aKeySlotAlgorithm[BCMD_KeyDestEntryType_eReserved2].ulM2MControlWordLo  = 0xFFFFFFFF;



			in_handle->aunM2MKeySlotInfo[i].KeySlotIDData[BCMD_KeyDestEntryType_eOddKey].ulCAVendorID            = 0;
			in_handle->aunM2MKeySlotInfo[i].KeySlotIDData[BCMD_KeyDestEntryType_eOddKey].ulModuleIDKey2Select    = 0;
			in_handle->aunM2MKeySlotInfo[i].KeySlotIDData[BCMD_KeyDestEntryType_eOddKey].ulSTBOwnerIDSelect      = 0;
			
			in_handle->aunM2MKeySlotInfo[i].KeySlotIDData[BCMD_KeyDestEntryType_eEvenKey].ulCAVendorID            = 0;
			in_handle->aunM2MKeySlotInfo[i].KeySlotIDData[BCMD_KeyDestEntryType_eEvenKey].ulModuleIDKey2Select    = 0;
			in_handle->aunM2MKeySlotInfo[i].KeySlotIDData[BCMD_KeyDestEntryType_eEvenKey].ulSTBOwnerIDSelect      = 0;

			in_handle->aunM2MKeySlotInfo[i].KeySlotIDData[BCMD_KeyDestEntryType_eReserved2].ulCAVendorID            = 0;
			in_handle->aunM2MKeySlotInfo[i].KeySlotIDData[BCMD_KeyDestEntryType_eReserved2].ulModuleIDKey2Select    = 0;
			in_handle->aunM2MKeySlotInfo[i].KeySlotIDData[BCMD_KeyDestEntryType_eReserved2].ulSTBOwnerIDSelect      = 0;
			
			*outp_unKeySlotNum = i;
			break;
		}
	}
	
	/* Return if there is no vacant key slot or key slot type mismatched */
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
	                                          (i == BCMD_MAX_M2M_KEY_SLOT));

	BDBG_MSG(("BHSM_AllocateM2MKeySlot  outp_unKeySlotNum = %d\n", *outp_unKeySlotNum));

	
BHSM_P_DONE_LABEL:

	/* Unlock mutex */
	BKNI_ReleaseMutex( in_handle->m2mKeySlotMutexLock);		
	
	BDBG_LEAVE(BHSM_AllocateM2MKeySlot);
	return( errCode );

}


BERR_Code BHSM_FreeM2MKeySlot (
		BHSM_Handle					in_handle,
		unsigned int				in_unKeySlotNum
)
{
	BERR_Code errCode = BERR_SUCCESS;


	BDBG_ENTER(BHSM_FreeM2MKeySlot);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );

	/* parameter checking - in_unKeySlotNum */
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
		(in_unKeySlotNum >= BCMD_MAX_M2M_KEY_SLOT) );
		

	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, BKNI_AcquireMutex( in_handle->m2mKeySlotMutexLock));


	BDBG_MSG(("BHSM_FreeM2MKeySlot  key slot = %d\n", in_unKeySlotNum));

	
	/* Return if the key slot is already empty */
	if (in_handle->aunM2MKeySlotInfo[in_unKeySlotNum].bIsUsed == false)  
	{
		goto BHSM_P_DONE_LABEL;
	}

	in_handle->aunM2MKeySlotInfo[in_unKeySlotNum].bIsUsed = false;

	
BHSM_P_DONE_LABEL:
	
	/* Unlock mutex */
	BKNI_ReleaseMutex( in_handle->m2mKeySlotMutexLock);			
	
	BDBG_LEAVE(BHSM_FreeM2MKeySlot);
	return( errCode );

}




BERR_Code BHSM_ConfigPidKeyPointerTable (
		BHSM_Handle							in_handle,
		BHSM_ConfigPidKeyPointerTableIO_t	*inoutp_configPidKeyPointerTableIO
)
{
	BERR_Code            errCode = BERR_SUCCESS;
	BHSM_ChannelHandle   channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t             unParamLen = 0;
	BHSM_P_CommandData_t commandData;
	uint32_t             resetPID = 0;

	BDBG_ENTER(BHSM_ConfigPidKeyPointerTable);
	BDBG_ASSERT( in_handle );
	BDBG_ASSERT( inoutp_configPidKeyPointerTableIO );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );

	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	commandData.cmdId = BCMD_cmdType_eSESSION_CONFIG_PIDKEYPOINTERTABLE;
	commandData.unContMode = 0;
	if (inoutp_configPidKeyPointerTableIO->bResetPIDToDefault)
		resetPID = 1;
	
	/* Parameter checking on unKeySlotNum */
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
		(inoutp_configPidKeyPointerTableIO->unKeySlotNum >= 
	     in_handle->aunCAKeySlotTypeInfo[inoutp_configPidKeyPointerTableIO->ucKeySlotType].unKeySlotNum) );

	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
		(inoutp_configPidKeyPointerTableIO->unKeySlotNumberB >= 
	     in_handle->aunCAKeySlotTypeInfo[inoutp_configPidKeyPointerTableIO->unKeySlotBType].unKeySlotNum) );

	/* only fill the command specific input parameters */
	commandData.unInputParamsBuf[BCMD_KeyPointer_InCmdCfg_ePidChan/4 - BCMD_CommonBufferFields_eStatus/4] = 
			(inoutp_configPidKeyPointerTableIO->unPidChannel |
			 (inoutp_configPidKeyPointerTableIO->spidProgType << 31) |
			 (resetPID << 30 ));
	commandData.unInputParamLen += (BCMD_KeyPointer_InCmdCfg_eSlotType/4) * 4
		 - (BCMD_KeyPointer_InCmdCfg_ePidChan/4) * 4;
	
	commandData.unInputParamsBuf[BCMD_KeyPointer_InCmdCfg_eSlotType /4 - BCMD_KeyPointer_InCmdCfg_ePidChan/4] = 
			inoutp_configPidKeyPointerTableIO->ucKeySlotType;
	commandData.unInputParamLen += (BCMD_KeyPointer_InCmdCfg_eSlotNumber/4) * 4
		 - (BCMD_KeyPointer_InCmdCfg_eSlotType/4) * 4;

	commandData.unInputParamsBuf[BCMD_KeyPointer_InCmdCfg_eSlotNumber/4 - BCMD_KeyPointer_InCmdCfg_ePidChan/4] = 
			inoutp_configPidKeyPointerTableIO->unKeySlotNum;
	commandData.unInputParamLen += (BCMD_KeyPointer_InCmdCfg_eSlotTypeB/4) * 4
		 - (BCMD_KeyPointer_InCmdCfg_eSlotNumber/4) * 4;

	commandData.unInputParamsBuf[BCMD_KeyPointer_InCmdCfg_eSlotTypeB/4 - BCMD_KeyPointer_InCmdCfg_ePidChan/4] = 
			inoutp_configPidKeyPointerTableIO->unKeySlotBType;
	commandData.unInputParamLen += (BCMD_KeyPointer_InCmdCfg_eSlotNumberB/4) * 4
		 - (BCMD_KeyPointer_InCmdCfg_eSlotTypeB/4) * 4;

	commandData.unInputParamsBuf[BCMD_KeyPointer_InCmdCfg_eSlotNumberB/4 - BCMD_KeyPointer_InCmdCfg_ePidChan/4] = 
			inoutp_configPidKeyPointerTableIO->unKeySlotNumberB;
	commandData.unInputParamLen += (BCMD_KeyPointer_InCmdCfg_eKeyPointerSel/4) * 4
		 - (BCMD_KeyPointer_InCmdCfg_eSlotNumberB/4) * 4;
	
	commandData.unInputParamsBuf[BCMD_KeyPointer_InCmdCfg_eKeyPointerSel/4 - BCMD_KeyPointer_InCmdCfg_ePidChan/4] = 
			inoutp_configPidKeyPointerTableIO->unKeyPointerSel;
	commandData.unInputParamLen += 4;	

	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

	/* Parse the command specific output parameters */
	inoutp_configPidKeyPointerTableIO->unStatus =  commandData.unOutputParamsBuf[0];
	unParamLen += 4;	

 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_configPidKeyPointerTableIO->unStatus != 0) );

	/* Keep track of which key slot associate with which pid channel */
	in_handle->aPidChannelToCAKeySlotNum[inoutp_configPidKeyPointerTableIO->unPidChannel][inoutp_configPidKeyPointerTableIO->pidChannelType].keySlotType = 
				inoutp_configPidKeyPointerTableIO->ucKeySlotType;	
	in_handle->aPidChannelToCAKeySlotNum[inoutp_configPidKeyPointerTableIO->unPidChannel][inoutp_configPidKeyPointerTableIO->pidChannelType].unKeySlotNum = 
				inoutp_configPidKeyPointerTableIO->unKeySlotNum;	
	
BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_ConfigPidKeyPointerTable);
	return( errCode );
}


BERR_Code BHSM_LoadRouteUserKey (
		BHSM_Handle					in_handle,
		BHSM_LoadRouteUserKeyIO_t	*inoutp_loadRouteUserKeyIO
)
{
	BERR_Code						errCode = BERR_SUCCESS;
	BHSM_ChannelHandle 				channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t 						unParamLen = 0, 
									i , 
									j, 
									unKeyPosition, 
									unKeyWordNum;
	BHSM_P_CommandData_t			commandData;
	unsigned char					ucKeySlotStartOffset = 0 ;
	BHSM_P_XPTKeySlotAlgorithm_t	*pXPTKeySlotAlgorithm = NULL;
	BHSM_P_M2MKeySlotAlgorithm_t	*pM2MKeySlotAlgorithm = NULL;
	

	unsigned int					unKeySlotNum  = inoutp_loadRouteUserKeyIO->unKeySlotNum;


	
	BDBG_ENTER(BHSM_LoadRouteUserKey);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );

	BDBG_ASSERT( inoutp_loadRouteUserKeyIO );

	/* Get pointer to structure BHSM_P_KeySlotAlgorithm_t for this keyslot */
	switch (inoutp_loadRouteUserKeyIO->keyDestBlckType)
	{
		case BCMD_KeyDestBlockType_eMem2Mem :

			/* Parameter checking on unKeySlotNum */
			BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
				(unKeySlotNum >= BCMD_MAX_M2M_KEY_SLOT) );
			
			pM2MKeySlotAlgorithm = &(in_handle->aunM2MKeySlotInfo[unKeySlotNum].aKeySlotAlgorithm[inoutp_loadRouteUserKeyIO->keyDestEntryType]);
			BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_UNCONFIGURED_KEYSLOT_ERR, 
				(pM2MKeySlotAlgorithm->ulM2MControlWordHi == 0xFFFFFFFF) );
			break;

		case BCMD_KeyDestBlockType_eCA :
		case BCMD_KeyDestBlockType_eCPDescrambler :
		case BCMD_KeyDestBlockType_eCPScrambler :

			ucKeySlotStartOffset = 
				in_handle->aunCAKeySlotTypeInfo[inoutp_loadRouteUserKeyIO->caKeySlotType].ucKeySlotStartOffset;
			
			/* Parameter checking on unKeySlotNum */
			BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
				(unKeySlotNum >= in_handle->aunCAKeySlotTypeInfo[inoutp_loadRouteUserKeyIO->caKeySlotType].unKeySlotNum) );
			
			pXPTKeySlotAlgorithm = &(in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+unKeySlotNum)].aKeySlotAlgorithm[inoutp_loadRouteUserKeyIO->keyDestEntryType]);
			BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_UNCONFIGURED_KEYSLOT_ERR, 
				((pXPTKeySlotAlgorithm->ulCAControlWordHi  == 0xFFFFFFFF) &&
				 (pXPTKeySlotAlgorithm->ulCPDControlWordHi == 0xFFFFFFFF) &&
				 (pXPTKeySlotAlgorithm->ulCPSControlWordHi == 0xFFFFFFFF))	);
			break;

		default :
			/* no checking for other types; no key slot associated */
			break;
	}
	
	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	commandData.cmdId = BCMD_cmdType_eSESSION_LOAD_ROUTE_USERKEY;
	commandData.unContMode = 0;
	
	/* only fill the command specific input parameters */
	commandData.unInputParamsBuf[(BCMD_LoadUseKey_InCmd_eKeyLength/4 - BCMD_CommonBufferFields_eStatus/4)] = 
			inoutp_loadRouteUserKeyIO->keySize.eKeySize;
	commandData.unInputParamLen +=  (BCMD_LoadUseKey_InCmd_eKeyData/4) * 4
		 - (BCMD_LoadUseKey_InCmd_eKeyLength/4) * 4;

	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
		(inoutp_loadRouteUserKeyIO->keySize.eKeySize > BCMD_KeySize_e192) );			
	
	/* if 64 bits - 2 dwords; 128 bits - 4 dwords; 192 bits - 6 dwords */
	unKeyWordNum = 2 * (inoutp_loadRouteUserKeyIO->keySize.eKeySize + 1);	
	/* where to start loading the key in the 256 bit (32 byte or 8 dword) space */
	unKeyPosition = BCMD_KEYLADDER_KEYRAM_SIZE/4 - unKeyWordNum;

	BDBG_MSG((" unKeyPosition = %d, unKeyWordNum = %d\n", unKeyPosition, unKeyWordNum));

	/* Load the key into the 256 bit (32 byte or 8 dword) space, right justtified, zero padded.  */
	for (i=0, 
	     j = (BCMD_LoadUseKey_InCmd_eKeyData/4 - BCMD_LoadUseKey_InCmd_eKeyLength/4) + unKeyPosition;
	     i < unKeyWordNum*4; j++, i+=4) 
	{
			
		commandData.unInputParamsBuf[j ] = 
			inoutp_loadRouteUserKeyIO->aucKeyData[i] << 24 |
			inoutp_loadRouteUserKeyIO->aucKeyData[i+1] << 16 |
			inoutp_loadRouteUserKeyIO->aucKeyData[i+2] << 8 |
			inoutp_loadRouteUserKeyIO->aucKeyData[i+3]  ;
		BDBG_MSG((" commandData.unInputParamsBuf[%d] = 0x%08lX, \n", j, commandData.unInputParamsBuf[j ]));	
		BDBG_MSG((" aucKeyData[%d] = 0x%08lX, aucKeyData[%d] = 0x%08lX, aucKeyData[%d] = 0x%08lX, aucKeyData[%d] = 0x%08lX,\n", 
			i, inoutp_loadRouteUserKeyIO->aucKeyData[i],
			i+1, inoutp_loadRouteUserKeyIO->aucKeyData[i+1],
			i+2, inoutp_loadRouteUserKeyIO->aucKeyData[i+2],
			i+3, inoutp_loadRouteUserKeyIO->aucKeyData[i+3]));	
	}

	commandData.unInputParamLen +=  BCMD_KEYLADDER_KEYRAM_SIZE;		



	commandData.unInputParamsBuf[(BCMD_LoadUseKey_InCmd_eRouteKeyFlag/4 - BCMD_LoadUseKey_InCmd_eKeyLength/4) ] = 
			inoutp_loadRouteUserKeyIO->bIsRouteKeyRequired;
	commandData.unInputParamLen +=  (BCMD_LoadUseKey_InCmd_eBlkType/4) * 4
		 - (BCMD_LoadUseKey_InCmd_eRouteKeyFlag/4) * 4;

	commandData.unInputParamsBuf[(BCMD_LoadUseKey_InCmd_eBlkType/4 - BCMD_LoadUseKey_InCmd_eKeyLength/4) ] = 
			inoutp_loadRouteUserKeyIO->keyDestBlckType;
	commandData.unInputParamLen +=  (BCMD_LoadUseKey_InCmd_eEntryType/4) * 4
		 - (BCMD_LoadUseKey_InCmd_eBlkType/4) * 4;
	
	commandData.unInputParamsBuf[(BCMD_LoadUseKey_InCmd_eEntryType/4 - BCMD_LoadUseKey_InCmd_eKeyLength/4) ] = 
			(inoutp_loadRouteUserKeyIO->keyDestEntryType |
			 (inoutp_loadRouteUserKeyIO->keyDestIVType << 8));
	commandData.unInputParamLen +=  (BCMD_LoadUseKey_InCmd_eKeySlotType/4) * 4
		 - (BCMD_LoadUseKey_InCmd_eEntryType/4) * 4;

	commandData.unInputParamsBuf[(BCMD_LoadUseKey_InCmd_eKeySlotType/4 - BCMD_LoadUseKey_InCmd_eKeyLength/4) ] = 
			inoutp_loadRouteUserKeyIO->caKeySlotType;
	commandData.unInputParamLen +=  (BCMD_LoadUseKey_InCmd_eKeySlotNumber/4) * 4
		 - (BCMD_LoadUseKey_InCmd_eKeySlotType/4) * 4;

	/* if dest block is HDMI, do the port emun - HDMI serializer address conversion */
	if (inoutp_loadRouteUserKeyIO->keyDestBlckType == BCMD_KeyDestBlockType_eHdmi)
	{
		commandData.unInputParamsBuf[(BCMD_LoadUseKey_InCmd_eKeySlotNumber/4 - BCMD_LoadUseKey_InCmd_eKeyLength/4) ] = 
				BHSM_P_HDMIEnumToSerializerAddress(inoutp_loadRouteUserKeyIO->unKeySlotNum);
	}
	else
	{
		commandData.unInputParamsBuf[(BCMD_LoadUseKey_InCmd_eKeySlotNumber/4 - BCMD_LoadUseKey_InCmd_eKeyLength/4) ] = 
				inoutp_loadRouteUserKeyIO->unKeySlotNum;
	}
	commandData.unInputParamLen +=  (BCMD_LoadUseKey_InCmd_eKeyMode/4) * 4
		 - (BCMD_LoadUseKey_InCmd_eKeySlotNumber/4) * 4;	

	commandData.unInputParamsBuf[(BCMD_LoadUseKey_InCmd_eKeyMode/4 - BCMD_LoadUseKey_InCmd_eKeyLength/4) ] = 
			((inoutp_loadRouteUserKeyIO->keyMode & 0xFF) |
	          ((inoutp_loadRouteUserKeyIO->RpipeSC01Val & 0x03) << 8) |
	          ((inoutp_loadRouteUserKeyIO->GpipeSC01Val & 0x03) << 16));
	commandData.unInputParamLen +=  (BCMD_LoadUseKey_InCmd_eCtrlWord0/4) * 4
		 - (BCMD_LoadUseKey_InCmd_eKeyMode/4) * 4;

	if ((inoutp_loadRouteUserKeyIO->keyDestBlckType == BCMD_KeyDestBlockType_eCA)            ||
	    (inoutp_loadRouteUserKeyIO->keyDestBlckType == BCMD_KeyDestBlockType_eCPDescrambler) || 
	    (inoutp_loadRouteUserKeyIO->keyDestBlckType == BCMD_KeyDestBlockType_eCPScrambler)     )
	{
		commandData.unInputParamsBuf[(BCMD_LoadUseKey_InCmd_eCtrlWord0/4 - BCMD_LoadUseKey_InCmd_eKeyLength/4) ] = 
				pXPTKeySlotAlgorithm->ulCAControlWordHi;
		commandData.unInputParamLen +=	(BCMD_LoadUseKey_InCmd_eCtrlWord1/4) * 4
	 		 - (BCMD_LoadUseKey_InCmd_eCtrlWord0/4) * 4;
		
		commandData.unInputParamsBuf[(BCMD_LoadUseKey_InCmd_eCtrlWord1/4 - BCMD_LoadUseKey_InCmd_eKeyLength/4) ] = 
				pXPTKeySlotAlgorithm->ulCAControlWordLo;
		commandData.unInputParamLen +=	(BCMD_LoadUseKey_InCmd_eCtrlWord2/4) * 4
	 		 - (BCMD_LoadUseKey_InCmd_eCtrlWord1/4) * 4;
	
		commandData.unInputParamsBuf[(BCMD_LoadUseKey_InCmd_eCtrlWord2/4 - BCMD_LoadUseKey_InCmd_eKeyLength/4) ] = 
				pXPTKeySlotAlgorithm->ulCPSControlWordHi;
		commandData.unInputParamLen +=	(BCMD_LoadUseKey_InCmd_eCtrlWord3/4) * 4
			 - (BCMD_LoadUseKey_InCmd_eCtrlWord2/4) * 4;

		commandData.unInputParamsBuf[(BCMD_LoadUseKey_InCmd_eCtrlWord3/4 - BCMD_LoadUseKey_InCmd_eKeyLength/4) ] = 
				pXPTKeySlotAlgorithm->ulCPSControlWordLo;
		commandData.unInputParamLen +=	(BCMD_LoadUseKey_InCmd_eCtrlWord4/4) * 4
			 - (BCMD_LoadUseKey_InCmd_eCtrlWord3/4) * 4;

		commandData.unInputParamsBuf[(BCMD_LoadUseKey_InCmd_eCtrlWord4/4 - BCMD_LoadUseKey_InCmd_eKeyLength/4) ] = 
				pXPTKeySlotAlgorithm->ulCPDControlWordHi;
		commandData.unInputParamLen +=	(BCMD_LoadUseKey_InCmd_eCtrlWord5/4) * 4
			 - (BCMD_LoadUseKey_InCmd_eCtrlWord4/4) * 4;
	
		commandData.unInputParamsBuf[(BCMD_LoadUseKey_InCmd_eCtrlWord5/4 - BCMD_LoadUseKey_InCmd_eKeyLength/4) ] = 
				pXPTKeySlotAlgorithm->ulCPDControlWordLo;
		commandData.unInputParamLen += 4;
	}
	else if (inoutp_loadRouteUserKeyIO->keyDestBlckType == BCMD_KeyDestBlockType_eMem2Mem)
	{
		commandData.unInputParamsBuf[(BCMD_LoadUseKey_InCmd_eCtrlWord0/4 - BCMD_LoadUseKey_InCmd_eKeyLength/4) ] = 
				pM2MKeySlotAlgorithm->ulM2MControlWordHi;
		commandData.unInputParamLen +=	(BCMD_LoadUseKey_InCmd_eCtrlWord1/4) * 4
	 		 - (BCMD_LoadUseKey_InCmd_eCtrlWord0/4) * 4;
		
		commandData.unInputParamsBuf[(BCMD_LoadUseKey_InCmd_eCtrlWord1/4 - BCMD_LoadUseKey_InCmd_eKeyLength/4) ] = 
				pM2MKeySlotAlgorithm->ulM2MControlWordLo;
		commandData.unInputParamLen +=	(BCMD_LoadUseKey_InCmd_eCtrlWord2/4) * 4
	 		 - (BCMD_LoadUseKey_InCmd_eCtrlWord1/4) * 4;
	
		commandData.unInputParamsBuf[(BCMD_LoadUseKey_InCmd_eCtrlWord2/4 - BCMD_LoadUseKey_InCmd_eKeyLength/4) ] = 
				0;
		commandData.unInputParamLen +=	(BCMD_LoadUseKey_InCmd_eCtrlWord3/4) * 4
			 - (BCMD_LoadUseKey_InCmd_eCtrlWord2/4) * 4;

		commandData.unInputParamsBuf[(BCMD_LoadUseKey_InCmd_eCtrlWord3/4 - BCMD_LoadUseKey_InCmd_eKeyLength/4) ] = 
				0;
		commandData.unInputParamLen +=	(BCMD_LoadUseKey_InCmd_eCtrlWord4/4) * 4
			 - (BCMD_LoadUseKey_InCmd_eCtrlWord3/4) * 4;

		commandData.unInputParamsBuf[(BCMD_LoadUseKey_InCmd_eCtrlWord4/4 - BCMD_LoadUseKey_InCmd_eKeyLength/4) ] = 
				0;
		commandData.unInputParamLen +=	(BCMD_LoadUseKey_InCmd_eCtrlWord5/4) * 4
			 - (BCMD_LoadUseKey_InCmd_eCtrlWord4/4) * 4;
	
		commandData.unInputParamsBuf[(BCMD_LoadUseKey_InCmd_eCtrlWord5/4 - BCMD_LoadUseKey_InCmd_eKeyLength/4) ] = 
				0;
		commandData.unInputParamLen += 4;


	}
	else
	{
		/* adjust command buffer pointer for the other types */
		commandData.unInputParamLen += 24;
	}

	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));
	/* Parse the command specific output parameters */
	inoutp_loadRouteUserKeyIO->unStatus =  commandData.unOutputParamsBuf[0];
	unParamLen += 4;	


 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_loadRouteUserKeyIO->unStatus != 0) );	
		
BHSM_P_DONE_LABEL:

	
	BDBG_LEAVE(BHSM_LoadRouteUserKey);
	return( errCode );

}


BERR_Code BHSM_InvalidateKey (
		BHSM_Handle					in_handle,
		BHSM_InvalidateKeyIO_t		*inoutp_invalidateKeyIO
)
{
	BERR_Code 				errCode = BERR_SUCCESS;
	BHSM_ChannelHandle 		channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t 				unParamLen = 0,
							unTmp;
	BHSM_P_CommandData_t 	commandData;


	BDBG_ENTER(BHSM_InvalidateKey);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BDBG_ASSERT( inoutp_invalidateKeyIO );
	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	commandData.cmdId = BCMD_cmdType_eSESSION_INVALIDATE_KEY;
	commandData.unContMode = 0;
	
	/* only fill the command specific input parameters */
	commandData.unInputParamsBuf[(BCMD_InvalidateKey_InCmd_eKeyFlag/4 - BCMD_CommonBufferFields_eStatus/4)] = 
			inoutp_invalidateKeyIO->invalidKeyType;
	commandData.unInputParamLen +=  (BCMD_InvalidateKey_InCmd_eVKLID/4) * 4
		 - (BCMD_InvalidateKey_InCmd_eKeyFlag/4) * 4;

	unTmp = inoutp_invalidateKeyIO->virtualKeyLadderID | (inoutp_invalidateKeyIO->keyLayer << BHSM_KEYLADDER_KL_SHIFT);
	commandData.unInputParamsBuf[(BCMD_InvalidateKey_InCmd_eVKLID/4 - BCMD_InvalidateKey_InCmd_eKeyFlag/4) ] = 
			unTmp;
	commandData.unInputParamLen +=  (BCMD_InvalidateKey_InCmd_eBlkType/4) * 4
		 - (BCMD_InvalidateKey_InCmd_eVKLID/4) * 4;

	commandData.unInputParamsBuf[(BCMD_InvalidateKey_InCmd_eBlkType/4 - BCMD_InvalidateKey_InCmd_eKeyFlag/4) ] = 
			inoutp_invalidateKeyIO->keyDestBlckType;
	commandData.unInputParamLen +=  (BCMD_InvalidateKey_InCmd_eEntryType/4) * 4
		 - (BCMD_InvalidateKey_InCmd_eBlkType/4) * 4;

	commandData.unInputParamsBuf[(BCMD_InvalidateKey_InCmd_eEntryType/4 - BCMD_InvalidateKey_InCmd_eKeyFlag/4) ] = 
			inoutp_invalidateKeyIO->keyDestEntryType;
	commandData.unInputParamLen +=  (BCMD_InvalidateKey_InCmd_eKeySlotType/4) * 4
		 - (BCMD_InvalidateKey_InCmd_eEntryType/4) * 4;

	commandData.unInputParamsBuf[(BCMD_InvalidateKey_InCmd_eKeySlotType/4 - BCMD_InvalidateKey_InCmd_eKeyFlag/4) ] = 
			inoutp_invalidateKeyIO->caKeySlotType;
	commandData.unInputParamLen +=  (BCMD_InvalidateKey_InCmd_eKeySlotNumber/4) * 4
		 - (BCMD_InvalidateKey_InCmd_eKeySlotType/4) * 4;

	/* if dest block is HDMI, do the port emun - HDMI serializer address conversion */
	if (inoutp_invalidateKeyIO->keyDestBlckType == BCMD_KeyDestBlockType_eHdmi)
	{
		commandData.unInputParamsBuf[(BCMD_InvalidateKey_InCmd_eKeySlotNumber/4 - BCMD_InvalidateKey_InCmd_eKeyFlag/4) ] = 
				BHSM_P_HDMIEnumToSerializerAddress(inoutp_invalidateKeyIO->unKeySlotNum);
	}
	else
	{
		
		if (inoutp_invalidateKeyIO->keyDestBlckType == BCMD_KeyDestBlockType_eMem2Mem)
		{
			/* Parameter checking on unKeySlotNum */
			BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
				(inoutp_invalidateKeyIO->unKeySlotNum >= BCMD_MAX_M2M_KEY_SLOT) );
		}
		else
		{
			/* Parameter checking on unKeySlotNum */
			BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
				(inoutp_invalidateKeyIO->unKeySlotNum >= in_handle->aunCAKeySlotTypeInfo[inoutp_invalidateKeyIO->caKeySlotType].unKeySlotNum) );
		
		}
		commandData.unInputParamsBuf[(BCMD_InvalidateKey_InCmd_eKeySlotNumber/4 - BCMD_InvalidateKey_InCmd_eKeyFlag/4) ] = 
				inoutp_invalidateKeyIO->unKeySlotNum;
	}

	commandData.unInputParamLen +=  4;

	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

	/* Parse the command specific output parameters */
	inoutp_invalidateKeyIO->unStatus =  commandData.unOutputParamsBuf[0];
	unParamLen += 4;	

 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_invalidateKeyIO->unStatus != 0) );		
	
BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_InvalidateKey);
	return( errCode );
}



BERR_Code BHSM_ConfigAlgorithm (
		BHSM_Handle					in_handle,
		BHSM_ConfigAlgorithmIO_t	*inoutp_configAlgorithmIO
)
{
	BERR_Code			errCode       = BERR_SUCCESS;
	uint32_t			unCryptoAlg   = 0;
	unsigned char		ucKeySlotStartOffset = 0 ;
	uint32_t			m2mCntrlWordHi = 0;
	uint32_t			m2mCntrlWordLo = 0;
	uint32_t			xptCntrlWordHi = 0;
	uint32_t			xptCntrlWordLo = 0;
	unsigned int		unKeySlotNum  = inoutp_configAlgorithmIO->unKeySlotNum;


	BDBG_ENTER(BHSM_ConfigAlgorithm);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	


	BDBG_ASSERT( inoutp_configAlgorithmIO );

	/* Get setting for OpMode word 1 (CA ctrl bits) and in CA case offset to get to aunCAKeySlotInfo. */
	/* For M2M case, unKeySlotNum is sufficient. */
	switch (inoutp_configAlgorithmIO->keyDestBlckType) 
	{
		case BCMD_KeyDestBlockType_eMem2Mem:
			/* Parameter checking on unKeySlotNum */
			BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
				(unKeySlotNum >= BCMD_MAX_M2M_KEY_SLOT) );
			unCryptoAlg = inoutp_configAlgorithmIO->cryptoAlg.m2mCryptAlg.m2mSecAlg;
			m2mCntrlWordLo = 
				(unCryptoAlg														 << BHSM_M2mModeShift_eCryptoAlgorithmShift) |
				(inoutp_configAlgorithmIO->cryptoAlg.m2mCryptAlg.m2mCipherMode		 << BHSM_M2mModeShift_eCipherModeSel) |
				(inoutp_configAlgorithmIO->cryptoAlg.m2mCryptAlg.TerminationAESCounterKeyMode << BHSM_M2mModeShift_eAESCounterTermModeShift) |
				(inoutp_configAlgorithmIO->cryptoAlg.m2mCryptAlg.counterSize         << BHSM_M2mModeShift_eAESCounterTermModeShift) |
				(inoutp_configAlgorithmIO->cryptoAlg.m2mCryptAlg.IVModeSelect		 << BHSM_M2mModeShift_eIVModeSel) |
				(inoutp_configAlgorithmIO->cryptoAlg.m2mCryptAlg.SolitarySelect 	 << BHSM_M2mModeShift_eSolitaryTermSel) | 
				((inoutp_configAlgorithmIO->cryptoAlg.m2mCryptAlg.keyOffset & 0x7F)  << BHSM_M2mModeShift_eKeyOffset)  |
				(inoutp_configAlgorithmIO->cryptoAlg.m2mCryptAlg.bUseExtKey 		 << BHSM_M2mModeShift_eUseExtKey)  |
				(inoutp_configAlgorithmIO->cryptoAlg.m2mCryptAlg.bUseExtIV			 << BHSM_M2mModeShift_eUseExtIV);
			
			m2mCntrlWordHi = 
				(inoutp_configAlgorithmIO->cryptoAlg.m2mCryptAlg.ucAuthCtrl 		 << BHSM_M2mModeShift_eAuthCtrlShift)  |
				(inoutp_configAlgorithmIO->cryptoAlg.m2mCryptAlg.bDisallowGG		 << BHSM_M2mModeShift_eDisallowGG) |
				(inoutp_configAlgorithmIO->cryptoAlg.m2mCryptAlg.bDisallowGR		 << BHSM_M2mModeShift_eDisallowGR) |
				(inoutp_configAlgorithmIO->cryptoAlg.m2mCryptAlg.bDisallowRG		 << BHSM_M2mModeShift_eDisallowRG) |
				(inoutp_configAlgorithmIO->cryptoAlg.m2mCryptAlg.bDisallowRR		 << BHSM_M2mModeShift_eDisallowRR) |
				(inoutp_configAlgorithmIO->cryptoAlg.m2mCryptAlg.bEnableTimestamp	 << BHSM_M2mModeShift_eEnableTimeStamp) |
				(inoutp_configAlgorithmIO->cryptoAlg.m2mCryptAlg.bMscCtrlSel	     << BHSM_M2mModeShift_eMscCtrlSel);

			BDBG_MSG(("m2mCntrlWordLo = 0x%lx, m2mCntrlWordHi = 0x%lx\n", m2mCntrlWordLo, m2mCntrlWordHi));

			/* Store m2mCntrlWordLo and m2mCntrlWordHi  */
			in_handle->aunM2MKeySlotInfo[unKeySlotNum].aKeySlotAlgorithm[inoutp_configAlgorithmIO->keyDestEntryType].ulM2MControlWordLo = m2mCntrlWordLo;
			in_handle->aunM2MKeySlotInfo[unKeySlotNum].aKeySlotAlgorithm[inoutp_configAlgorithmIO->keyDestEntryType].ulM2MControlWordHi = m2mCntrlWordHi;
			break;
			
		case BCMD_KeyDestBlockType_eCA:
		case BCMD_KeyDestBlockType_eCPScrambler:
		case BCMD_KeyDestBlockType_eCPDescrambler:
			ucKeySlotStartOffset = 
				in_handle->aunCAKeySlotTypeInfo[inoutp_configAlgorithmIO->caKeySlotType].ucKeySlotStartOffset;
			unCryptoAlg = inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.caSecAlg;
			/* Parameter checking on unKeySlotNum */
			BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
				(unKeySlotNum >= in_handle->aunCAKeySlotTypeInfo[inoutp_configAlgorithmIO->caKeySlotType].unKeySlotNum) );
			xptCntrlWordLo = 
				(unCryptoAlg														<< BHSM_CaModeShift_eCryptoAlgorithmShift) |
				(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.cipherDVBCSA2Mode   << BHSM_CaModeShift_eCipherModeShift) |
				(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.terminationMode 	<< BHSM_CaModeShift_eTerminationModeShift) |
				(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.IVMode				<< BHSM_CaModeShift_eIVModeShift) |
				(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.solitaryMode		<< BHSM_CaModeShift_eSolitaryTermShift) |
				((inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.keyOffset & 0x7F)  << BHSM_CaModeShift_eKeyOffset)    |
				(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.bUseExtKey          << BHSM_CaModeShift_eUseExtKeyShift)  |
				((inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.ivOffset & 0x7F)   << BHSM_CaModeShift_eIVOffsetShift)   |
				(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.bUseExtIV           << BHSM_CaModeShift_eUseExtIVShift);

			xptCntrlWordHi = 
				(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.bRestrictEnable 	    << BHSM_CaModeShift_eRestrEnShift)     |
				(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.bGlobalEnable		    << BHSM_CaModeShift_eGlobalEnShift)    |
				((inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.restrSCVal  & 0x3)     << BHSM_CaModeShift_eRestrScValShift)  |
				((inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.globalSCVal & 0x3)     << BHSM_CaModeShift_eGlobalScValShift)  |
				(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.bRestrictDropPktEnabled << BHSM_CaModeShift_eRestrictDropPktCtrlShift) |
				(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.bGlobalDropPktEnabled   << BHSM_CaModeShift_eGlobalDropPktCtrlShift) |
				(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.ucMulti2KeySelect	    << BHSM_CaModeShift_eMscSelMulti2SelShift) |
				(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.ucMSCLengthSelect       << BHSM_CaModeShift_eMscSelMulti2SelShift) |
				(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.bGlobalRegOverride      << BHSM_CaModeShift_eGlobalRegOverrideShift) |
				(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.bDropRregionPackets     << BHSM_CaModeShift_eDropFromRSourceShift)   |
				(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.bEncryptBeforeRave	    << BHSM_CaModeShift_eEncryptBeforeRaveShift) |
				(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.customerType            << BHSM_CaModeShift_eCustomerNoShift)        |
				((inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.DVBCSA3dvbcsaVar & 0x1F)  << BHSM_CaModeShift_NDSDVBCSA3DvbCsaVarShift)|
				((inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.DVBCSA3permutation & 0x7) << BHSM_CaModeShift_NDSDVBCSA3PermShift)     |
				(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.DVBCSA3modXRC           << BHSM_CaModeShift_NDSDVBCSA3ModXRCShift)   |
				((inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.DVBCSA2keyCtrl & 0x7)  << BHSM_CaModeShift_NDSDVBCSA2KeyCtrlShift)  |
				((inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.DVBCSA2ivCtrl & 0x3)   << BHSM_CaModeShift_NDSDVBCSA2IVCtrlShift)   |
				(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.DVBCSA2modEnabled       << BHSM_CaModeShift_ESTARDVBCSA2ModEnShift)  |
				(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.bGpipePackets2Rregion   << BHSM_CaModeShift_GpipeForceRestrictedShift) |
				(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.bRpipePackets2Rregion   << BHSM_CaModeShift_RpipeForceRestrictedShift);


			BDBG_MSG(("xptCntrlWordLo = 0x%lx, xptCntrlWordHi = 0x%lx\n", xptCntrlWordLo, xptCntrlWordHi));

			/* store the control words according to CA, CP-S, or CP-D */
			if (inoutp_configAlgorithmIO->keyDestBlckType == BCMD_KeyDestBlockType_eCA)
			{
				in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+unKeySlotNum)].aKeySlotAlgorithm[inoutp_configAlgorithmIO->keyDestEntryType].ulCAControlWordHi = xptCntrlWordHi;
				in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+unKeySlotNum)].aKeySlotAlgorithm[inoutp_configAlgorithmIO->keyDestEntryType].ulCAControlWordLo = xptCntrlWordLo;

			}
			else if (inoutp_configAlgorithmIO->keyDestBlckType == BCMD_KeyDestBlockType_eCPScrambler)
			{
				in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+unKeySlotNum)].aKeySlotAlgorithm[inoutp_configAlgorithmIO->keyDestEntryType].ulCPSControlWordHi = xptCntrlWordHi;
				in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+unKeySlotNum)].aKeySlotAlgorithm[inoutp_configAlgorithmIO->keyDestEntryType].ulCPSControlWordLo = xptCntrlWordLo;
			}
			else if (inoutp_configAlgorithmIO->keyDestBlckType == BCMD_KeyDestBlockType_eCPDescrambler)
			{
				in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+unKeySlotNum)].aKeySlotAlgorithm[inoutp_configAlgorithmIO->keyDestEntryType].ulCPDControlWordHi = xptCntrlWordHi;
				in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+unKeySlotNum)].aKeySlotAlgorithm[inoutp_configAlgorithmIO->keyDestEntryType].ulCPDControlWordLo = xptCntrlWordLo;
			}

			break;

		default:
			errCode = BERR_TRACE((BHSM_STATUS_INPUT_PARM_ERR));
			goto BHSM_P_DONE_LABEL;	
			
	}

		
BHSM_P_DONE_LABEL:

	
	BDBG_LEAVE(BHSM_ConfigAlgorithm);
	return( errCode );
	
}



BERR_Code BHSM_GetKeySlotConfigAlgorithm (
		BHSM_Handle				in_handle,
		BHSM_KeySlotInfo_t		*inoutp_KeySlotInfoIO
)
{
	BERR_Code			errCode              = BERR_SUCCESS;
	unsigned char		ucKeySlotStartOffset = 0 ;
	uint32_t			m2mCntrlWordHi = 0;
	uint32_t			m2mCntrlWordLo = 0;
	uint32_t			xptCntrlWordHi = 0;
	uint32_t			xptCntrlWordLo = 0;
	unsigned int		unKeySlotNum   = inoutp_KeySlotInfoIO->unKeySlotNum;



	BDBG_ENTER(BHSM_GetKeySlotConfigAlgorithm);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	
	BDBG_ASSERT( inoutp_KeySlotInfoIO );

	/* Get setting for OpMode word 1 (CA ctrl bits) and in CA case offset to get to aunCAKeySlotInfo. */
	/* For M2M case, unKeySlotNum is sufficient. */
	switch (inoutp_KeySlotInfoIO->keyDestBlckType) 
	{
		case BCMD_KeyDestBlockType_eMem2Mem:
			/* Parameter checking on unKeySlotNum */
			BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
				(unKeySlotNum >= BCMD_MAX_M2M_KEY_SLOT) );

			/* Retrieve m2mCntrlWordLo and m2mCntrlWordHi	*/
			m2mCntrlWordLo = in_handle->aunM2MKeySlotInfo[unKeySlotNum].aKeySlotAlgorithm[inoutp_KeySlotInfoIO->keyDestEntryType].ulM2MControlWordLo;
			m2mCntrlWordHi = in_handle->aunM2MKeySlotInfo[unKeySlotNum].aKeySlotAlgorithm[inoutp_KeySlotInfoIO->keyDestEntryType].ulM2MControlWordHi;
			inoutp_KeySlotInfoIO->cryptoAlg.m2mCryptAlg.m2mSecAlg     = (m2mCntrlWordLo >> BHSM_M2mModeShift_eCryptoAlgorithmShift) & 0x1F;
			inoutp_KeySlotInfoIO->cryptoAlg.m2mCryptAlg.m2mCipherMode = (m2mCntrlWordLo >> BHSM_M2mModeShift_eCipherModeSel) & 0x07;
			inoutp_KeySlotInfoIO->cryptoAlg.m2mCryptAlg.TerminationAESCounterKeyMode = (m2mCntrlWordLo >> BHSM_M2mModeShift_eAESCounterTermModeShift) & 0x07;
			inoutp_KeySlotInfoIO->cryptoAlg.m2mCryptAlg.counterSize   = (m2mCntrlWordLo >> BHSM_M2mModeShift_eAESCounterTermModeShift) & 0x07;
			inoutp_KeySlotInfoIO->cryptoAlg.m2mCryptAlg.IVModeSelect  = (m2mCntrlWordLo >> BHSM_M2mModeShift_eIVModeSel) & 0x03;
			inoutp_KeySlotInfoIO->cryptoAlg.m2mCryptAlg.SolitarySelect = (m2mCntrlWordLo >> BHSM_M2mModeShift_eSolitaryTermSel) & 0x07; 

			BDBG_MSG(("m2mCntrlWordLo = 0x%lx, m2mCntrlWordHi = 0x%lx\n", m2mCntrlWordLo, m2mCntrlWordHi));

			break;
			
		case BCMD_KeyDestBlockType_eCA:
		case BCMD_KeyDestBlockType_eCPScrambler:
		case BCMD_KeyDestBlockType_eCPDescrambler:
			ucKeySlotStartOffset = 
				in_handle->aunCAKeySlotTypeInfo[inoutp_KeySlotInfoIO->caKeySlotType].ucKeySlotStartOffset;
			/* Parameter checking on unKeySlotNum */
			BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
				(unKeySlotNum >= in_handle->aunCAKeySlotTypeInfo[inoutp_KeySlotInfoIO->caKeySlotType].unKeySlotNum) );
			/* retrieve the control words according to CA, CP-S, or CP-D */
			if (inoutp_KeySlotInfoIO->keyDestBlckType == BCMD_KeyDestBlockType_eCA)
			{
				xptCntrlWordHi = in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+unKeySlotNum)].aKeySlotAlgorithm[inoutp_KeySlotInfoIO->keyDestEntryType].ulCAControlWordHi;
				xptCntrlWordLo = in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+unKeySlotNum)].aKeySlotAlgorithm[inoutp_KeySlotInfoIO->keyDestEntryType].ulCAControlWordLo;

			}
			else if (inoutp_KeySlotInfoIO->keyDestBlckType == BCMD_KeyDestBlockType_eCPScrambler)
			{
				xptCntrlWordHi = in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+unKeySlotNum)].aKeySlotAlgorithm[inoutp_KeySlotInfoIO->keyDestEntryType].ulCPSControlWordHi;
				xptCntrlWordLo = in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+unKeySlotNum)].aKeySlotAlgorithm[inoutp_KeySlotInfoIO->keyDestEntryType].ulCPSControlWordLo;
			}
			else if (inoutp_KeySlotInfoIO->keyDestBlckType == BCMD_KeyDestBlockType_eCPDescrambler)
			{
				xptCntrlWordHi = in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+unKeySlotNum)].aKeySlotAlgorithm[inoutp_KeySlotInfoIO->keyDestEntryType].ulCPDControlWordHi;
				xptCntrlWordLo = in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+unKeySlotNum)].aKeySlotAlgorithm[inoutp_KeySlotInfoIO->keyDestEntryType].ulCPDControlWordLo;
			}

			inoutp_KeySlotInfoIO->cryptoAlg.caCryptAlg.caSecAlg          = (xptCntrlWordLo >> BHSM_CaModeShift_eCryptoAlgorithmShift) & 0x1F;
			inoutp_KeySlotInfoIO->cryptoAlg.caCryptAlg.cipherDVBCSA2Mode = (xptCntrlWordLo >> BHSM_CaModeShift_eCipherModeShift) & 0x07;
			inoutp_KeySlotInfoIO->cryptoAlg.caCryptAlg.terminationMode   = (xptCntrlWordLo >> BHSM_CaModeShift_eTerminationModeShift) & 0x07;
			inoutp_KeySlotInfoIO->cryptoAlg.caCryptAlg.IVMode            = (xptCntrlWordLo >> BHSM_CaModeShift_eIVModeShift) & 0x03;
			inoutp_KeySlotInfoIO->cryptoAlg.caCryptAlg.solitaryMode      = (xptCntrlWordLo >> BHSM_CaModeShift_eSolitaryTermShift) & 0x07;
			inoutp_KeySlotInfoIO->cryptoAlg.caCryptAlg.restrSCVal        = (xptCntrlWordHi >> BHSM_CaModeShift_eRestrScValShift) & 0x03;
			inoutp_KeySlotInfoIO->cryptoAlg.caCryptAlg.globalSCVal       = (xptCntrlWordHi >> BHSM_CaModeShift_eGlobalScValShift) & 0x03;

			BDBG_MSG(("xptCntrlWordLo = 0x%lx, xptCntrlWordHi = 0x%lx\n", xptCntrlWordLo, xptCntrlWordHi));

			break;

		default:
			errCode = BERR_TRACE((BHSM_STATUS_INPUT_PARM_ERR));
			goto BHSM_P_DONE_LABEL; 
			
	}



BHSM_P_DONE_LABEL:

	
	BDBG_LEAVE(BHSM_GetKeySlotConfigAlgorithm);
	return( errCode );

}


BERR_Code BHSM_ConfigKeySlotIDData (
		BHSM_Handle						in_handle,
		BHSM_ConfigKeySlotIDDataIO_t	*inoutp_configKeySlotIDDataIO
)
{
	BERR_Code					errCode              = BERR_SUCCESS;
	BHSM_P_KeySlotIDData_t		*pKeySlotIDData;
	unsigned char				ucKeySlotStartOffset = 0 ;
	unsigned int				unKeySlotNum         = inoutp_configKeySlotIDDataIO->unKeySlotNum;


	BDBG_ENTER(BHSM_ConfigKeySlotIDData);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	
	BDBG_ASSERT( inoutp_configKeySlotIDDataIO );

	switch (inoutp_configKeySlotIDDataIO->keyDestBlckType) 
	{
		case BCMD_KeyDestBlockType_eMem2Mem:
			/* Parameter checking on unKeySlotNum */
			BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
				(unKeySlotNum >= BCMD_MAX_M2M_KEY_SLOT) );
			pKeySlotIDData = &(in_handle->aunM2MKeySlotInfo[unKeySlotNum].KeySlotIDData[inoutp_configKeySlotIDDataIO->keyDestEntryType]); 
			break;
			
		case BCMD_KeyDestBlockType_eCA:
		case BCMD_KeyDestBlockType_eCPScrambler:
		case BCMD_KeyDestBlockType_eCPDescrambler:
			/* Parameter checking on unKeySlotNum */
			BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
				(unKeySlotNum >= in_handle->aunCAKeySlotTypeInfo[inoutp_configKeySlotIDDataIO->caKeySlotType].unKeySlotNum) );
			ucKeySlotStartOffset = 
				in_handle->aunCAKeySlotTypeInfo[inoutp_configKeySlotIDDataIO->caKeySlotType].ucKeySlotStartOffset;
			pKeySlotIDData = &(in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+unKeySlotNum)].KeySlotIDData[inoutp_configKeySlotIDDataIO->keyDestEntryType]);
			break;

		default:
			errCode = BERR_TRACE((BHSM_STATUS_INPUT_PARM_ERR));
			goto BHSM_P_DONE_LABEL;	
			
	}

	/* Store the ID Data now */
	pKeySlotIDData->ulCAVendorID         = inoutp_configKeySlotIDDataIO->CAVendorID;
	pKeySlotIDData->ulModuleIDKey2Select = inoutp_configKeySlotIDDataIO->ModuleID |
	                                       (inoutp_configKeySlotIDDataIO->key2Select << 8);
	pKeySlotIDData->ulSTBOwnerIDSelect   = inoutp_configKeySlotIDDataIO->STBOwnerIDSelect;

		
BHSM_P_DONE_LABEL:

	
	BDBG_LEAVE(BHSM_ConfigKeySlotIDData);
	return( errCode );
	
}







BERR_Code BHSM_ConfigMulti2 (
		BHSM_Handle				in_handle,
		BHSM_ConfigMulti2IO_t		*inoutp_configMulti2IO
)
{
	BERR_Code             errCode = BERR_SUCCESS;
	BHSM_ChannelHandle    channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t              unParamLen = 0, i, j;
	BHSM_P_CommandData_t  commandData;

	BDBG_ENTER(BHSM_ConfigMulti2);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	commandData.cmdId = BCMD_cmdType_eCONFIG_MULTI2;
	commandData.unContMode = 0;

	/* Parameter checking on ucSysKeyDest */
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
		(inoutp_configMulti2IO->ucSysKeyDest >= BCMD_MULTI2_MAXSYSKEY ) );
	
	/* only fill the command specific input parameters */
	commandData.unInputParamsBuf[(BCMD_Multi2_InCmdCfg_eRoundCount/4 - BCMD_CommonBufferFields_eStatus/4)] = 
			inoutp_configMulti2IO->ucMulti2RndCnt;
	commandData.unInputParamLen +=  (BCMD_Multi2_InCmdCfg_eSystemKeys/4) * 4
		 - (BCMD_Multi2_InCmdCfg_eRoundCount/4) * 4;

	for (i=0, j = (BCMD_Multi2_InCmdCfg_eRoundCount/4 - BCMD_CommonBufferFields_eStatus/4) +1;
		i<BHSM_MULTI2_SYS_KEY_SIZE; j++, i+=4) {
			
		commandData.unInputParamsBuf[j ] = 
			inoutp_configMulti2IO->aucMulti2SysKey[i] << 24 |
			inoutp_configMulti2IO->aucMulti2SysKey[i+1] << 16 |
			inoutp_configMulti2IO->aucMulti2SysKey[i+2] << 8 |
			inoutp_configMulti2IO->aucMulti2SysKey[i+3]  ;
			
		commandData.unInputParamLen +=  4;		
	}
		
	commandData.unInputParamsBuf[(BCMD_Multi2_InCmdCfg_eWhichSysKey/4 - BCMD_Multi2_InCmdCfg_eRoundCount/4) ] = 
			inoutp_configMulti2IO->ucSysKeyDest;
	commandData.unInputParamLen +=  4;

	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

	/* Parse the command specific output parameters */
	inoutp_configMulti2IO->unStatus =  commandData.unOutputParamsBuf[0];
	unParamLen += 4;	

	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_configMulti2IO->unStatus != 0) );	
	
BHSM_P_DONE_LABEL:

	BDBG_LEAVE(BHSM_ConfigMulti2);
	return( errCode );
}

BERR_Code BHSM_ConfigPidChannelToDefaultKeySlot (
		BHSM_Handle							in_handle,
		BHSM_ConfigPidKeyPointerTableIO_t	*inoutp_configPidKeyPointerTableIO
)
{
	BERR_Code 		errCode = BERR_SUCCESS;
	
	BDBG_ENTER(BHSM_ConfigPidChannelToDefaultKeySlot);
	BDBG_ASSERT( in_handle );

	inoutp_configPidKeyPointerTableIO->bResetPIDToDefault = true;

 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
					BHSM_ConfigPidKeyPointerTable(in_handle,inoutp_configPidKeyPointerTableIO ));

BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_ConfigPidChannelToDefaultKeySlot);
	return( errCode );
}



/*  1) HDMI keyslot,  parameter error
   	2) M2M1  keyslot, same as M2M(0)
   	3) Reserved2 keyslot for future chips, parameter error
   	4) the keyslot should be invalidated first ? or this step is optional.  Not needed
   	5) if to clear CA in one function call, other block types in the same keyslot (e.g. RMX) are NOT affected, vice versa   	   
   	    so, if apps want to clear all CA/Reserved0/Reserved1-or-RMX in the slot, pls call this PI three times, e.g.
   		BHSM_ResetKeySlotCtrlBits(handle, BCMD_KeyDestBlockType_eCA, 		slottype, slotnum);
  		BHSM_ResetKeySlotCtrlBits(handle, BCMD_KeyDestBlockType_eReserved0, 	slottype, slotnum);
   		BHSM_ResetKeySlotCtrlBits(handle, BCMD_KeyDestBlockType_eReserved1 OR BCMD_KeyDestBlockType_eRmx, slottype, slotnum);
   		
   	6) Performance issue: each such PI call may take 2.4ms for slottype0~5, 3.6ms for slottype6

   	7) this PI is to support new chip/version since 7401C0 or 2006.Oct
*/

BERR_Code BHSM_ResetKeySlotCtrlBits (
	BHSM_Handle				in_handle,
	BCMD_KeyDestBlockType_e 	kSlotDestBlockType,
	BCMD_XptSecKeySlot_e		 kSlotType, 
	unsigned int 				kSlotNum
)
{
		BERR_Code 					errCode = BERR_SUCCESS;
		BHSM_ConfigAlgorithmIO_t	inoutp_configAlgorithmIO;


		BDBG_ENTER(BHSM_ResetKeySlotCtrlBits);
		BDBG_ASSERT( in_handle );


		BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
		             ((kSlotType >= BCMD_XptSecKeySlot_eTypeMax) ||
		              (kSlotNum >= in_handle->aunCAKeySlotTypeInfo[kSlotType].unKeySlotNum) ) );					
		
		BKNI_Memset(&inoutp_configAlgorithmIO, 0, sizeof(BHSM_ConfigAlgorithmIO_t));	

		inoutp_configAlgorithmIO.caKeySlotType 	= kSlotType;
		inoutp_configAlgorithmIO.unKeySlotNum 	= kSlotNum;
		inoutp_configAlgorithmIO.keyDestBlckType = kSlotDestBlockType;

		switch(kSlotDestBlockType)
		{
			case  BCMD_KeyDestBlockType_eMem2Mem: 
			case  BCMD_KeyDestBlockType_eCA: 
			case  BCMD_KeyDestBlockType_eCPDescrambler: 			
			case  BCMD_KeyDestBlockType_eCPScrambler: 			  
					inoutp_configAlgorithmIO.keyDestEntryType= BCMD_KeyDestEntryType_eOddKey;
				 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED_FIRST, 
					              BHSM_ConfigAlgorithm(in_handle, &inoutp_configAlgorithmIO)  );

					inoutp_configAlgorithmIO.keyDestEntryType= BCMD_KeyDestEntryType_eEvenKey;
				 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED_FIRST, 
					              BHSM_ConfigAlgorithm(in_handle, &inoutp_configAlgorithmIO)  );

					inoutp_configAlgorithmIO.keyDestEntryType= BCMD_KeyDestEntryType_eReserved2;
				 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED_REST, 
					              BHSM_ConfigAlgorithm(in_handle, &inoutp_configAlgorithmIO)  );
					break;

			case  BCMD_KeyDestBlockType_eHdmi: 	
					BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 1 );					
					break;      /* TODO  HDMI */


			default: 
					break;
		}		


BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_ResetKeySlotCtrlBits);
	return( errCode );
		
}

/* ericz - 12/14/2011 - CSP465507 - Build errors with "export NEXUS_HDCP_SUPPORT=y" set for DCX3600_7425 target */
#if (BHSM_SECURE_PI_SUPPORT_KEYLADDER == 1)
BERR_Code BHSM_FastLoadEncryptedHdcpKey (
	BHSM_Handle				        in_handle,
	uint32_t                        index,
	BHSM_EncryptedHdcpKeyStruct     *keyStruct
)
{
	BERR_Code errCode = BERR_SUCCESS;
	BHSM_GenerateRouteKeyIO_t		generateRouteKeyIO;
	uint32_t KeyParameter ;
	
	BDBG_ASSERT(in_handle) ;

	BKNI_Memset( &generateRouteKeyIO, 0, sizeof(BHSM_GenerateRouteKeyIO_t))  ;	

	/* generate Key3 from custom key */
	generateRouteKeyIO.bASKMModeEnabled = 0;
	
#if HSM_IS_ASKM_40NM_ZEUS_2_0
	generateRouteKeyIO.subCmdID   = BCMD_VKLAssociationQueryFlag_eNoQuery;
#else
	generateRouteKeyIO.subCmdID   = BCMD_KeyGenSubCmdID_eKeyGen;
#endif

	generateRouteKeyIO.rootKeySrc =  BCMD_RootKeySrc_eCusKey; 
	generateRouteKeyIO.customerSubMode = BCMD_CustomerSubMode_eGeneralPurpose1;
	
	generateRouteKeyIO.ucSwizzle1Index = 0;
	generateRouteKeyIO.swizzleType = BCMD_SwizzleType_eSwizzle0;

	generateRouteKeyIO.bUseCustKeyHighDecrypt = false ;		
	generateRouteKeyIO.bUseCustKeyLowDecrypt =	false ;

	generateRouteKeyIO.virtualKeyLadderID = BCMD_VKL1;
	generateRouteKeyIO.keyLayer = BCMD_KeyRamBuf_eKey3 ;

	generateRouteKeyIO.key3Op				   = BCMD_Key3Op_eKey3NoProcess;
	generateRouteKeyIO.bIsKeyLadder3DESEncrypt = false;
	generateRouteKeyIO.bSwapAesKey			   = false;

	generateRouteKeyIO.keyLadderType = BCMD_KeyLadderType_e3DESABA;
	generateRouteKeyIO.keySize= BCMD_KeySize_e64;
	generateRouteKeyIO.ucKeyDataLen = 8 ;

	generateRouteKeyIO.bIsRouteKeyRequired = true;
	generateRouteKeyIO.keyDestBlckType = BCMD_KeyDestBlockType_eHdmi;
	generateRouteKeyIO.keyDestEntryType = BCMD_KeyDestEntryType_eOddKey;
	generateRouteKeyIO.keyDestIVType		   = BCMD_KeyDestIVType_eNoIV;
	generateRouteKeyIO.caKeySlotType = BCMD_XptSecKeySlot_eType0 ;
	generateRouteKeyIO.keyMode = BCMD_KeyMode_eRegular;


	generateRouteKeyIO.keyLadderType = keyStruct->Alg;
	generateRouteKeyIO.ucCustKeyLow = keyStruct->cusKeySel;	 
	generateRouteKeyIO.ucKeyVarLow = keyStruct->cusKeyVarL;	
	generateRouteKeyIO.ucCustKeyHigh = keyStruct->cusKeySel;	
	generateRouteKeyIO.ucKeyVarHigh = keyStruct->cusKeyVarH;	
	
	BKNI_Memset(&generateRouteKeyIO.aucKeyData, 0, sizeof(generateRouteKeyIO.aucKeyData));				
	KeyParameter = keyStruct->HdcpKeyHi;
	generateRouteKeyIO.aucKeyData[0]  =  (KeyParameter >> 24) & 0xff ;
	generateRouteKeyIO.aucKeyData[1]  =  (KeyParameter >> 16) & 0xff ;
	generateRouteKeyIO.aucKeyData[2]  =  (KeyParameter >>	8) & 0xff ;
	generateRouteKeyIO.aucKeyData[3]  =  (KeyParameter) 		  & 0xff ;
	
	KeyParameter = keyStruct->HdcpKeyLo;
	generateRouteKeyIO.aucKeyData[4]  =  (KeyParameter >> 24) & 0xff ;
	generateRouteKeyIO.aucKeyData[5]  =  (KeyParameter >> 16) & 0xff ;
	generateRouteKeyIO.aucKeyData[6]  =  (KeyParameter >>  8) & 0xff ;
	generateRouteKeyIO.aucKeyData[7]  =  (KeyParameter) 		  & 0xff ;					
		
	generateRouteKeyIO.unKeySlotNum = index ; 
	errCode= BHSM_GenerateRouteKey (in_handle, &generateRouteKeyIO) ;	
	BDBG_MSG(("generateRouteKeyIO key 3 unStatus = 0x%08X\n", generateRouteKeyIO.unStatus)) ;

	return( errCode );
}

#endif

