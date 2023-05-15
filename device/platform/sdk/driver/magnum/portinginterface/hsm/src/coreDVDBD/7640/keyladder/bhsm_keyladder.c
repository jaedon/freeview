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
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 3/18/11 4:16p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7640/a0/keyladder/bhsm_keyladder.c $
 * 
 * Hydra_Software_Devel/3   3/18/11 4:16p cdisc
 * SWBLURAY-25150: adding AACS support
 * 
 * Hydra_Software_Devel/2   2/8/11 12:36p cdisc
 * SWBLURAY-23691: adding 7640 specific mods
 * 
 * Hydra_Software_Devel/1   2/8/11 8:53a cdisc
 * SWBLURAY-23691: branching files
 * 
 * Hydra_Software_Devel/2   1/24/11 10:53a atruong
 * SW7422-26: Add HSM PI Support For 7422A0  - CC header format
 *
 ******************************************************************************/

#include "bchp_bsp_control_intr2.h"
#include "bchp_bsp_glb_control.h"

#include "bhsm_keyladder.h"



BDBG_MODULE(BHSM);



BERR_Code BHSM_InitKeySlot (
		BHSM_Handle				in_handle,
		BHSM_InitKeySlotIO_t	*inoutp_initKeySlotIO
)
{
	BERR_Code            errCode = BERR_SUCCESS;
	BHSM_ChannelHandle   channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t             unParamLen = 0, i, j, k;
	BHSM_P_CommandData_t commandData;
	
	BDBG_ENTER(BHSM_InitKeySlot);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BDBG_ASSERT( inoutp_initKeySlotIO );
	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));


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
	BERR_Code errCode = BERR_SUCCESS;
	uint8_t   ucKeySlotStartOffset;


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



			in_handle->aunCAKeySlotInfo[i].KeySlotIDData[BCMD_KeyDestEntryType_eOddKey].ulCAVendorID            = 0;
			in_handle->aunCAKeySlotInfo[i].KeySlotIDData[BCMD_KeyDestEntryType_eOddKey].ulModuleIDKey2Select = 0;
			in_handle->aunCAKeySlotInfo[i].KeySlotIDData[BCMD_KeyDestEntryType_eOddKey].ulSTBOwnerIDSelect      = 0;
			
			in_handle->aunCAKeySlotInfo[i].KeySlotIDData[BCMD_KeyDestEntryType_eEvenKey].ulCAVendorID            = 0;
			in_handle->aunCAKeySlotInfo[i].KeySlotIDData[BCMD_KeyDestEntryType_eEvenKey].ulModuleIDKey2Select = 0;
			in_handle->aunCAKeySlotInfo[i].KeySlotIDData[BCMD_KeyDestEntryType_eEvenKey].ulSTBOwnerIDSelect      = 0;

			in_handle->aunCAKeySlotInfo[i].KeySlotIDData[BCMD_KeyDestEntryType_eReserved2].ulCAVendorID            = 0;
			in_handle->aunCAKeySlotInfo[i].KeySlotIDData[BCMD_KeyDestEntryType_eReserved2].ulModuleIDKey2Select = 0;
			in_handle->aunCAKeySlotInfo[i].KeySlotIDData[BCMD_KeyDestEntryType_eReserved2].ulSTBOwnerIDSelect      = 0;
			
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
			inoutp_loadRouteUserKeyIO->keyMode;
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


/*	printf("BHSM_LoadRouteUserKey: Key ladder Data length = %d\n", commandData.unInputParamLen );
	printf("BHSM_LoadRouteUserKey: Key ladder generate route key data :\n");	
	for ( i= 0; i<commandData.unInputParamLen/4; i++)
		printf ("data[%d]= %08lx\n ",i,commandData.unInputParamsBuf[i]);

*/


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
				(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.bEncryptBeforeRave	    << BHSM_CaModeShift_eEncryptBeforeRaveShift) |
				(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.customerType            << BHSM_CaModeShift_eCustomerNoShift)        |
				((inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.DVBCSA3dvbcsaVar & 0x1F)  << BHSM_CaModeShift_NDSDVBCSA3DvbCsaVarShift)|
				((inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.DVBCSA3permutation & 0x7) << BHSM_CaModeShift_NDSDVBCSA3PermShift)     |
				(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.DVBCSA3modXRC           << BHSM_CaModeShift_NDSDVBCSA3ModXRCShift)   |
				((inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.DVBCSA2keyCtrl & 0x7)  << BHSM_CaModeShift_NDSDVBCSA2KeyCtrlShift)  |
				((inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.DVBCSA2ivCtrl & 0x3)   << BHSM_CaModeShift_NDSDVBCSA2IVCtrlShift)   |
				(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.DVBCSA2modEnabled       << BHSM_CaModeShift_ESTARDVBCSA2ModEnShift);


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
