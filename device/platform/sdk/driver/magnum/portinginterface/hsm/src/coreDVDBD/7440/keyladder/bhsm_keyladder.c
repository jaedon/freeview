/***************************************************************************
 *     Copyright (c) 2005-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhsm_keyladder.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 8/13/07 5:53p $
 *
 * Module Description: This file contains Broadcom Host Secure Module (OS/platform 
 *                     independent) porting interface public functions.
 *                    			                    
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7440/b0/keyladder/bhsm_keyladder.c $
 * 
 * Hydra_Software_Devel/2   8/13/07 5:53p lyang
 * PR 33325: add 7118B0 support and link
 * 
 * Hydra_Software_Devel/10   10/26/06 4:20p lyang
 * PR 23412: hsm pi include files should be re-factored so they are not
 * order dependent. PR 25133:HSM header files should include dependencies
 * 
 * Hydra_Software_Devel/9   10/13/06 5:25p lyang
 * PR 24861: HSM: Add API to reset the control bits and invalidate the
 * keys in BSP key slot
 * 
 * 24860: HSM: Add API to remap pid channel to default key slot
 * 
 * Hydra_Software_Devel/8   9/26/06 3:10p btan
 * PR 23906: Converted BCHP_REV to BCHP_VER.
 * 
 * Hydra_Software_Devel/7   7/20/06 10:58p btan
 * PR 21609: Fixed potential uninitialized data.
 * 
 * Hydra_Software_Devel/6   7/20/06 7:12p btan
 * PR 21762: Fixed the alignment.
 * 
 * Hydra_Software_Devel/5   5/25/06 11:02a dbayani
 * PR 21762: Fix bug involving the loading of M2M alg bits to Aegis
 * command.
 * 
 * Hydra_Software_Devel/4   5/17/06 7:48p btan
 * PR 20446: Fixed a load route key key size issue.
 * 
 * Hydra_Software_Devel/3   5/1/06 7:11p btan
 * PR 20446: Fixed a typo.
 * 
 * Hydra_Software_Devel/2   4/26/06 5:32p btan
 * PR 20446: HSM needs to compile for 7400 A0
 * 
 * Hydra_Software_Devel/1   2/28/06 8:24p btan
 * PR 19931: Added HSM support for 7400A0
 * 
 * Hydra_Software_Devel/1   1/20/06 5:38p btan
 * PR 19140: Do not encrypt generic mode files.
 * 
 * Hydra_Software_Devel/5   1/17/06 10:29p btan
 * PR 19140: Do not encrypt generic mode files.
 *
 *
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"




/* #include "bxpt.h"  */

/* From Aegis */

#include "bhsm.h"
#include "bhsm_keyladder.h"



BDBG_MODULE(BHSM);

/* #define BDBG_MSG(x) printf x */


BERR_Code BHSM_InitKeySlot (
		BHSM_Handle			in_handle,
		BHSM_InitKeySlotIO_t	*inoutp_initKeySlotIO
)
{
	BERR_Code errCode = BERR_SUCCESS;
	BHSM_ChannelHandle channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t unParamLen = 0, i, j, k;
	BHSM_P_CommandData_t commandData;
	
	BDBG_ENTER(BHSM_BHSM_InitKeySlot);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

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

	in_handle->aunCAKeySlotTypeInfo[ BCMD_XptSecKeySlot_eType5].unKeySlotNum = 
				inoutp_initKeySlotIO->unKeySlotType5Num;
	in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType5].ucKeySlotStartOffset = 
		in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType4].ucKeySlotStartOffset +
		inoutp_initKeySlotIO->unKeySlotType4Num;

	in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType6].unKeySlotNum = 
				inoutp_initKeySlotIO->unKeySlotType6Num;
	in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType6].ucKeySlotStartOffset = 
		in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType5].ucKeySlotStartOffset +
		inoutp_initKeySlotIO->unKeySlotType5Num;
		

	in_handle->unTotalCAKeySlotNum = 
					inoutp_initKeySlotIO->unKeySlotType0Num +
					inoutp_initKeySlotIO->unKeySlotType1Num +
					inoutp_initKeySlotIO->unKeySlotType2Num +
					inoutp_initKeySlotIO->unKeySlotType3Num +
					inoutp_initKeySlotIO->unKeySlotType4Num +
					inoutp_initKeySlotIO->unKeySlotType5Num +
					inoutp_initKeySlotIO->unKeySlotType6Num;	

	k=0;
	for (i=0; i<BCMD_XptSecKeySlot_eTypeMax; i++)	{
		for (j=0; j<in_handle->aunCAKeySlotTypeInfo[i].unKeySlotNum; j++) {
			in_handle->aunCAKeySlotInfo[k].keySlotType = i;	
			in_handle->aunCAKeySlotInfo[k++].bIsUsed = false;
		}
	}
	
		
	commandData.cmdId = BCMD_cmdType_eSESSION_INIT_KEYSLOT;
	commandData.unContMode = 0;    

	/* only fill the command specific input parameters */
	commandData.unInputParamsBuf[0] = 
			inoutp_initKeySlotIO->unKeySlotType0Num;
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
	
	commandData.unInputParamsBuf[commandData.unInputParamLen /4] = 
			inoutp_initKeySlotIO->unKeySlotType5Num;
	commandData.unInputParamLen += 4;

	commandData.unInputParamsBuf[commandData.unInputParamLen /4] = 
			inoutp_initKeySlotIO->unKeySlotType6Num;
	commandData.unInputParamLen += 4;

	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

	unParamLen = 0;
	/* Parse the command specific output parameters */
	inoutp_initKeySlotIO->unStatus =  commandData.unOutputParamsBuf[0];
	unParamLen += 4;	

	/* Check output paramter length */
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_PARM_LEN_ERR, 
		(unParamLen != commandData.unOutputParamLen) );		

	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_initKeySlotIO->unStatus != 0) );		


	
BHSM_P_DONE_LABEL:

	
	BDBG_LEAVE(BHSM_BHSM_InitKeySlot);
	return( errCode );
}


BERR_Code BHSM_AllocateCAKeySlot (
		BHSM_Handle					in_handle,
		BCMD_XptSecKeySlot_e			in_keySlotType,
		unsigned int					*outp_unKeySlotNum
)
{
	BERR_Code errCode = BERR_SUCCESS;
	unsigned int i;

	BDBG_MSG(("BHSM_AllocateCAKeySlot in_keySlotType=%d\n", in_keySlotType));

	BDBG_ENTER(BHSM_AllocateCAKeySlot);
	BDBG_ASSERT( in_handle );

	*outp_unKeySlotNum = BHSM_SLOT_NUM_INIT_VAL;	
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, BKNI_AcquireMutex( in_handle->caKeySlotMutexLock));
	
	/* Search for vacant key slot */
	for (i=in_handle->aunCAKeySlotTypeInfo[in_keySlotType].ucKeySlotStartOffset;
			i<in_handle->aunCAKeySlotTypeInfo[in_keySlotType].ucKeySlotStartOffset+
			in_handle->aunCAKeySlotTypeInfo[ in_keySlotType].unKeySlotNum; i++) {
		
		if (in_handle->aunCAKeySlotInfo[i].bIsUsed == false) {
			/* Found */
			in_handle->aunCAKeySlotInfo[i].bIsUsed = true;
			break;
		}
	}
	
	/* Return if there is no vacant key slot or key slot type mismatched */
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(i == (in_handle->aunCAKeySlotTypeInfo[in_keySlotType].ucKeySlotStartOffset+
		in_handle->aunCAKeySlotTypeInfo[ in_keySlotType].unKeySlotNum)) || 
		(in_handle->aunCAKeySlotInfo[i].keySlotType != in_keySlotType));

	/* Here is the vacant key slot */
	*outp_unKeySlotNum = i - in_handle->aunCAKeySlotTypeInfo[in_keySlotType].ucKeySlotStartOffset;

	BDBG_MSG(("BHSM_AllocateCAKeySlot  outp_unKeySlotNum = %d\n", i));

	
BHSM_P_DONE_LABEL:

	/* Unlock mutex */
	BKNI_ReleaseMutex( in_handle->caKeySlotMutexLock);		
	
	BDBG_LEAVE(BHSM_AllocateCAKeySlot);
	return( errCode );

}




BERR_Code BHSM_FreeCAKeySlot (
		BHSM_Handle					in_handle,
		unsigned int					in_unPidChannel,
		BHSM_PidChannelType_e		in_pidChannelType,		
		BCMD_XptSecKeySlot_e			in_keySlotType,
		unsigned int					in_unKeySlotNum
)
{
	BERR_Code errCode = BERR_SUCCESS;
	uint8_t ucKeySlotStartOffset = 
		in_handle->aunCAKeySlotTypeInfo[in_keySlotType].ucKeySlotStartOffset;

	BDBG_ENTER(BHSM_FreeCAKeySlot);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, BKNI_AcquireMutex( in_handle->caKeySlotMutexLock));


	BDBG_MSG(("BHSM_FreeCAKeySlot  aunCAKeySlotInfo[%d] = %d\n", 
		(ucKeySlotStartOffset+in_unKeySlotNum), 
		in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+in_unKeySlotNum)].bIsUsed));	
	
	/* Return if the key slot is already empty */
	if (in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+in_unKeySlotNum)].bIsUsed == false)  {
		goto BHSM_P_DONE_LABEL;
	}

	in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+in_unKeySlotNum)].bIsUsed = false;

	in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+in_unKeySlotNum)].aKeySlotAlgorithm[BCMD_KeyDestEntryType_eOddKey].ulCPDControlBits = 0;			
	in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+in_unKeySlotNum)].aKeySlotAlgorithm[BCMD_KeyDestEntryType_eEvenKey].ulCAControlBits = 0;
	in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+in_unKeySlotNum)].aKeySlotAlgorithm[BCMD_KeyDestEntryType_eReserved0].ulCPSControlBits = 0;

	if (in_unPidChannel >= DUMMY_PID_CHANNEL_TOAKEYSLOT) { 
		/* does nothing to avoid risky memory modification */ 
	} else{

	/* De-associate the pid channel from the key slot */
	    in_handle->aPidChannelToCAKeySlotNum[in_unPidChannel][in_pidChannelType].keySlotType = BCMD_XptSecKeySlot_eTypeMax ;	
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
		unsigned int					in_unPidChannel,
		BHSM_PidChannelType_e		in_pidChannelType,
		BCMD_XptSecKeySlot_e			*outp_ucKeySlotType,
		unsigned int					*outp_unKeySlotNum
)
{
	BERR_Code errCode = BERR_SUCCESS;

	BDBG_ENTER(BHSM_HasKeySlotAssigned);
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
			!=  BHSM_SLOT_NUM_INIT_VAL) ) {
			
		*outp_ucKeySlotType = in_handle->aPidChannelToCAKeySlotNum[in_unPidChannel][in_pidChannelType].keySlotType ;	
		*outp_unKeySlotNum = in_handle->aPidChannelToCAKeySlotNum[in_unPidChannel][in_pidChannelType].unKeySlotNum ;			
	}
	else {
		*outp_ucKeySlotType = BCMD_XptSecKeySlot_eTypeMax ;	
		*outp_unKeySlotNum = BHSM_SLOT_NUM_INIT_VAL;			

	}
	
	
BHSM_P_DONE_LABEL:

	/* Unlock mutex */
	BKNI_ReleaseMutex( in_handle->caKeySlotMutexLock);	
	
	BDBG_LEAVE(BHSM_HasKeySlotAssigned);
	return( errCode );

}

BERR_Code BHSM_AllocateM2MKeySlot (
		BHSM_Handle					in_handle,
		unsigned int					*outp_unKeySlotNum
)
{
	BERR_Code errCode = BERR_SUCCESS;
	unsigned int i;

	BDBG_ENTER(BHSM_AllocateM2MKeySlot);
	BDBG_ASSERT( in_handle );

	*outp_unKeySlotNum = BHSM_SLOT_NUM_INIT_VAL;	
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, BKNI_AcquireMutex( in_handle->m2mKeySlotMutexLock));
	
	/* Search for vacant key slot */
	for (i=0;	i<BCMD_MAX_M2M_KEY_SLOT; i++) {
		
		if (in_handle->aunM2MKeySlotInfo[i].bIsUsed == false) {
			/* Found */
			in_handle->aunM2MKeySlotInfo[i].bIsUsed = true;
			*outp_unKeySlotNum = i;
			break;
		}
	}
	
	/* Return if there is no vacant key slot or key slot type mismatched */
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(i == BCMD_MAX_M2M_KEY_SLOT));

	BDBG_MSG(("BHSM_AllocateCAKeySlot  outp_unKeySlotNum = %d\n", *outp_unKeySlotNum));

	
BHSM_P_DONE_LABEL:

	/* Unlock mutex */
	BKNI_ReleaseMutex( in_handle->m2mKeySlotMutexLock);		
	
	BDBG_LEAVE(BHSM_AllocateM2MKeySlot);
	return( errCode );

}

BERR_Code BHSM_FreeM2MKeySlot (
		BHSM_Handle					in_handle,
		unsigned int					in_unKeySlotNum
)
{
	BERR_Code errCode = BERR_SUCCESS;

	BDBG_ENTER(BHSM_FreeM2MKeySlot);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, BKNI_AcquireMutex( in_handle->m2mKeySlotMutexLock));


	BDBG_MSG(("BHSM_FreeCAKeySlot  key slot = %d\n", in_unKeySlotNum));	
	
	/* Return if the key slot is already empty */
	if (in_handle->aunM2MKeySlotInfo[in_unKeySlotNum].bIsUsed == false)  {
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
		BHSM_Handle						in_handle,
		BHSM_ConfigPidKeyPointerTableIO_t	*inoutp_configPidKeyPointerTableIO
)
{
	BERR_Code errCode = BERR_SUCCESS;
	BHSM_ChannelHandle channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t unParamLen = 0;
	BHSM_P_CommandData_t commandData;

	BDBG_ENTER(BHSM_ConfigPidKeyPointerTable);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );

	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	commandData.cmdId = BCMD_cmdType_eSESSION_CONFIG_PIDKEYPOINTERTABLE;
	commandData.unContMode = 0;
	
	/* only fill the command specific input parameters */
	commandData.unInputParamsBuf[BCMD_KeyPointer_InCmdCfg_ePidChan/4 - BCMD_CommonBufferFields_eStatus/4] = 
			inoutp_configPidKeyPointerTableIO->unPidChannel;
	commandData.unInputParamLen += (BCMD_KeyPointer_InCmdCfg_eSlotType/4) * 4
		 - (BCMD_KeyPointer_InCmdCfg_ePidChan/4) * 4;
	
	commandData.unInputParamsBuf[BCMD_KeyPointer_InCmdCfg_eSlotType /4 - BCMD_KeyPointer_InCmdCfg_ePidChan/4] = 
			inoutp_configPidKeyPointerTableIO->ucKeySlotType;
	commandData.unInputParamLen += (BCMD_KeyPointer_InCmdCfg_eSlotNumber/4) * 4
		 - (BCMD_KeyPointer_InCmdCfg_eSlotType/4) * 4;

#if (BCHP_CHIP == 7401) &&  (BCHP_VER == BCHP_VER_A0)

	commandData.unInputParamsBuf[BCMD_KeyPointer_InCmdCfg_eSlotNumber/4 - BCMD_KeyPointer_InCmdCfg_ePidChan/4] = 
			inoutp_configPidKeyPointerTableIO->unKeySlotNum;
	commandData.unInputParamLen += 4;

#else

	commandData.unInputParamsBuf[BCMD_KeyPointer_InCmdCfg_eSlotNumber/4 - BCMD_KeyPointer_InCmdCfg_ePidChan/4] = 
			inoutp_configPidKeyPointerTableIO->unKeySlotNum;
	commandData.unInputParamLen += (BCMD_KeyPointer_InCmdCfg_eSlotTypeB/4) * 4
		 - (BCMD_KeyPointer_InCmdCfg_eSlotNumber/4) * 4;

	commandData.unInputParamsBuf[BCMD_KeyPointer_InCmdCfg_eSlotTypeB/4 - BCMD_KeyPointer_InCmdCfg_ePidChan/4] = 
			inoutp_configPidKeyPointerTableIO->unKeySlotB;
	commandData.unInputParamLen += (BCMD_KeyPointer_InCmdCfg_eSlotNumberB/4) * 4
		 - (BCMD_KeyPointer_InCmdCfg_eSlotTypeB/4) * 4;

	commandData.unInputParamsBuf[BCMD_KeyPointer_InCmdCfg_eSlotNumberB/4 - BCMD_KeyPointer_InCmdCfg_ePidChan/4] = 
			inoutp_configPidKeyPointerTableIO->unKeySlotNumberB;
	commandData.unInputParamLen += (BCMD_KeyPointer_InCmdCfg_eKeyPointerSel/4) * 4
		 - (BCMD_KeyPointer_InCmdCfg_eSlotNumberB/4) * 4;
	
	commandData.unInputParamsBuf[BCMD_KeyPointer_InCmdCfg_eKeyPointerSel/4 - BCMD_KeyPointer_InCmdCfg_ePidChan/4] = 
			inoutp_configPidKeyPointerTableIO->unKeyPointerSel;
	commandData.unInputParamLen += 4;	

#endif

	
	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

	/* Parse the command specific output parameters */
	inoutp_configPidKeyPointerTableIO->unStatus =  commandData.unOutputParamsBuf[0];
	unParamLen += 4;	

	/* Check output paramter length */
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_PARM_LEN_ERR, 
		(unParamLen != commandData.unOutputParamLen) );	

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
		BHSM_LoadRouteUserKeyIO_t		*inoutp_loadRouteUserKeyIO
)
{
	BERR_Code errCode = BERR_SUCCESS;
	BHSM_ChannelHandle channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t unParamLen = 0, i , j, unKeyPosition, unKeyWordNum;
	BHSM_P_CommandData_t commandData;
	
	BDBG_ENTER(BHSM_LoadRouteUserKey);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );

	
	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	commandData.cmdId = BCMD_cmdType_eSESSION_LOAD_ROUTE_USERKEY;
	commandData.unContMode = 0;
	
	/* only fill the command specific input parameters */
	commandData.unInputParamsBuf[(BCMD_LoadUseKey_InCmd_eKeyBuffer/4 - BCMD_CommonBufferFields_eStatus/4)] = 
			inoutp_loadRouteUserKeyIO->keySource;
	commandData.unInputParamLen +=  (BCMD_LoadUseKey_InCmd_eKeyLength/4) * 4
		 - (BCMD_LoadUseKey_InCmd_eKeyBuffer/4) * 4;

	switch (inoutp_loadRouteUserKeyIO->keySource) {
		case BCMD_KeyRamBuf_eFirstRam:		
		case BCMD_KeyRamBuf_eSecondRam:		
		case BCMD_KeyRamBuf_eThirdRam:

			BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
				(inoutp_loadRouteUserKeyIO->keySize.eKeySize > BCMD_KeySize_e192) );			

			unKeyWordNum = 2 * (inoutp_loadRouteUserKeyIO->keySize.eKeySize + 1);	

			commandData.unInputParamsBuf[(BCMD_LoadUseKey_InCmd_eKeyLength/4 - BCMD_LoadUseKey_InCmd_eKeyBuffer/4) ] = 
					inoutp_loadRouteUserKeyIO->keySize.eKeySize;  /* Use the more generous one */
			commandData.unInputParamLen +=  (BCMD_LoadUseKey_InCmd_eKeyData/4) * 4
				 - (BCMD_LoadUseKey_InCmd_eKeyLength/4) * 4;
			
			break;

		case BCMD_KeyRamBuf_eFirstRam2048:		
		case BCMD_KeyRamBuf_eSecondRam2048:		
		case BCMD_KeyRamBuf_eThirdRam2048:	

			BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
				(inoutp_loadRouteUserKeyIO->keySize.usKeySize > (BCMD_USERKEY_KEYRAM2048_SIZE*8)) );

			unKeyWordNum = inoutp_loadRouteUserKeyIO->keySize.usKeySize/32;	

			commandData.unInputParamsBuf[(BCMD_LoadUseKey_InCmd_eKeyLength/4 - BCMD_LoadUseKey_InCmd_eKeyBuffer/4) ] = 
					inoutp_loadRouteUserKeyIO->keySize.usKeySize;  /* Use the more generous one */
			commandData.unInputParamLen +=  (BCMD_LoadUseKey_InCmd_eKeyData/4) * 4
				 - (BCMD_LoadUseKey_InCmd_eKeyLength/4) * 4;			
			break;		
		
		default:
			errCode = BERR_TRACE((BHSM_STATUS_INPUT_PARM_ERR));
			goto BHSM_P_DONE_LABEL;	
	}

	unKeyPosition = BCMD_USERKEY_KEYRAM2048_SIZE/4 -unKeyWordNum;

	BDBG_MSG((" unKeyPosition = %d, unKeyWordNum = %d\n", unKeyPosition, unKeyWordNum));

	/* 
		Per F/W implementation, we support up to 64-word key (2048 bit).
		We always pack the key the l.s.b position.
		For example, we should pack 192-bit key at 
		inoutp_loadRouteUserKeyIO->aucKeyData[58]
	*/
	commandData.unInputParamLen +=  BCMD_USERKEY_KEYRAM2048_SIZE;		
	
	for (i=0, 
		j = (BCMD_LoadUseKey_InCmd_eKeyLength/4 - BCMD_LoadUseKey_InCmd_eKeyBuffer/4) +1 + unKeyPosition;
		i<unKeyWordNum*4; j++, i+=4) {
			
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

	commandData.unInputParamsBuf[(BCMD_LoadUseKey_InCmd_eRouteKeyFlag/4 - BCMD_LoadUseKey_InCmd_eKeyBuffer/4) ] = 
			inoutp_loadRouteUserKeyIO->bIsRouteKeyRequired;
	commandData.unInputParamLen +=  (BCMD_LoadUseKey_InCmd_eBlkType/4) * 4
		 - (BCMD_LoadUseKey_InCmd_eRouteKeyFlag/4) * 4;

	commandData.unInputParamsBuf[(BCMD_LoadUseKey_InCmd_eBlkType/4 - BCMD_LoadUseKey_InCmd_eKeyBuffer/4) ] = 
			inoutp_loadRouteUserKeyIO->keyDestBlckType;
	commandData.unInputParamLen +=  (BCMD_LoadUseKey_InCmd_eEntryType/4) * 4
		 - (BCMD_LoadUseKey_InCmd_eBlkType/4) * 4;
	
	commandData.unInputParamsBuf[(BCMD_LoadUseKey_InCmd_eEntryType/4 - BCMD_LoadUseKey_InCmd_eKeyBuffer/4) ] = 
			inoutp_loadRouteUserKeyIO->keyDestEntryType;
	commandData.unInputParamLen +=  (BCMD_LoadUseKey_InCmd_eKeySlotType/4) * 4
		 - (BCMD_LoadUseKey_InCmd_eEntryType/4) * 4;

	commandData.unInputParamsBuf[(BCMD_LoadUseKey_InCmd_eKeySlotType/4 - BCMD_LoadUseKey_InCmd_eKeyBuffer/4) ] = 
			inoutp_loadRouteUserKeyIO->caKeySlotType;
	commandData.unInputParamLen +=  (BCMD_LoadUseKey_InCmd_eKeySlotNumber/4) * 4
		 - (BCMD_LoadUseKey_InCmd_eKeySlotType/4) * 4;

	commandData.unInputParamsBuf[(BCMD_LoadUseKey_InCmd_eKeySlotNumber/4 - BCMD_LoadUseKey_InCmd_eKeyBuffer/4) ] = 
			inoutp_loadRouteUserKeyIO->unKeySlotNum;
	commandData.unInputParamLen +=  (BCMD_LoadUseKey_InCmd_eKeyMode/4) * 4
		 - (BCMD_LoadUseKey_InCmd_eKeySlotNumber/4) * 4;	

	commandData.unInputParamsBuf[(BCMD_LoadUseKey_InCmd_eKeyMode/4 - BCMD_LoadUseKey_InCmd_eKeyBuffer/4) ] = 
			inoutp_loadRouteUserKeyIO->keyMode;
	commandData.unInputParamLen +=  4;		


	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

	/* Parse the command specific output parameters */
	inoutp_loadRouteUserKeyIO->unStatus =  commandData.unOutputParamsBuf[0];
	unParamLen += 4;	

	/* Check output paramter length */
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_PARM_LEN_ERR, 
		(unParamLen != commandData.unOutputParamLen) );	
	
 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_loadRouteUserKeyIO->unStatus != 0) );	
		
BHSM_P_DONE_LABEL:

	
	BDBG_LEAVE(BHSM_LoadRouteUserKey);
	return( errCode );
}

BERR_Code BHSM_GenerateRouteKey (
		BHSM_Handle					in_handle,
		BHSM_GenerateRouteKeyIO_t		*inoutp_generateRouteKeyIO
)
{
	BERR_Code errCode = BERR_SUCCESS;
	BHSM_ChannelHandle channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t unParamLen = 0, i , j, unTmp;
	BHSM_P_CommandData_t commandData;


	BDBG_ENTER(BHSM_GenerateRouteKey);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	commandData.cmdId = BCMD_cmdType_eSESSION_GENERATE_ROUTE_KEY;
	commandData.unContMode = 0;
	
	/* only fill the command specific input parameters */
	commandData.unInputParamsBuf[(BCMD_CommonGenKey_InCmd_eKeyLadderType/4 - BCMD_CommonBufferFields_eStatus/4)] = 
			inoutp_generateRouteKeyIO->keyLadderType;
	commandData.unInputParamLen +=  (BCMD_CommonGenKey_InCmd_eRootKeySrc/4) * 4
		 - (BCMD_CommonGenKey_InCmd_eKeyLadderType/4) * 4;

	commandData.unInputParamsBuf[(BCMD_CommonGenKey_InCmd_eRootKeySrc/4 - BCMD_CommonGenKey_InCmd_eKeyLadderType/4) ] = 
			inoutp_generateRouteKeyIO->rootKeySrc;
	commandData.unInputParamLen +=  (BCMD_CommonGenKey_InCmd_eSwizzle1IndexSel/4) * 4
		 - (BCMD_CommonGenKey_InCmd_eRootKeySrc/4) * 4;

	commandData.unInputParamsBuf[(BCMD_CommonGenKey_InCmd_eSwizzle1IndexSel/4 - BCMD_CommonGenKey_InCmd_eKeyLadderType/4) ] = 
			inoutp_generateRouteKeyIO->ucSwizzle1Index;
	commandData.unInputParamLen +=  (BCMD_CommonGenKey_InCmd_eSwizzleType/4) * 4
		 - (BCMD_CommonGenKey_InCmd_eSwizzle1IndexSel/4) * 4;

	commandData.unInputParamsBuf[(BCMD_CommonGenKey_InCmd_eSwizzleType/4 - BCMD_CommonGenKey_InCmd_eKeyLadderType/4) ] = 
			inoutp_generateRouteKeyIO->swizzleType;
	commandData.unInputParamLen +=  (BCMD_CommonGenKey_InCmd_eCusKeySelL/4) * 4
		 - (BCMD_CommonGenKey_InCmd_eSwizzleType/4) * 4;

	unTmp = inoutp_generateRouteKeyIO->ucCustKeyLow | 
		( inoutp_generateRouteKeyIO->bUseCustKeyLowDecrypt << 6);
	commandData.unInputParamsBuf[(BCMD_CommonGenKey_InCmd_eCusKeySelL/4 - BCMD_CommonGenKey_InCmd_eKeyLadderType/4) ] = 
			unTmp;
	commandData.unInputParamLen +=  (BCMD_CommonGenKey_InCmd_eKeyVarL/4) * 4
		 - (BCMD_CommonGenKey_InCmd_eCusKeySelL/4) * 4;

	commandData.unInputParamsBuf[(BCMD_CommonGenKey_InCmd_eKeyVarL/4 - BCMD_CommonGenKey_InCmd_eKeyLadderType/4) ] = 
			inoutp_generateRouteKeyIO->ucKeyVarLow;
	commandData.unInputParamLen +=  (BCMD_CommonGenKey_InCmd_eCusKeySelH/4) * 4
		 - (BCMD_CommonGenKey_InCmd_eKeyVarL/4) * 4;	

	unTmp = inoutp_generateRouteKeyIO->ucCustKeyHigh | 
		( inoutp_generateRouteKeyIO->bUseCustKeyHighDecrypt << 6);
	commandData.unInputParamsBuf[(BCMD_CommonGenKey_InCmd_eCusKeySelH/4 - BCMD_CommonGenKey_InCmd_eKeyLadderType/4) ] = 
			unTmp;
	commandData.unInputParamLen +=  (BCMD_CommonGenKey_InCmd_eKeyVarH/4) * 4
		 - (BCMD_CommonGenKey_InCmd_eCusKeySelH/4) * 4;

	commandData.unInputParamsBuf[(BCMD_CommonGenKey_InCmd_eKeyVarH/4 - BCMD_CommonGenKey_InCmd_eKeyLadderType/4) ] = 
			inoutp_generateRouteKeyIO->ucKeyVarHigh;
	commandData.unInputParamLen +=  (BCMD_GenKey_InCmd_eKeySrc/4) * 4
		 - (BCMD_CommonGenKey_InCmd_eKeyVarH/4) * 4;	

	commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eKeySrc/4 - BCMD_CommonGenKey_InCmd_eKeyLadderType/4) ] = 
			inoutp_generateRouteKeyIO->keySrc;
	commandData.unInputParamLen +=  (BCMD_GenKey_InCmd_eKeyBuffer/4) * 4
		 - (BCMD_GenKey_InCmd_eKeySrc/4) * 4;

	commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eKeyBuffer/4 - BCMD_CommonGenKey_InCmd_eKeyLadderType/4) ] = 
			inoutp_generateRouteKeyIO->keyDest;
	commandData.unInputParamLen +=  (BCMD_GenKey_InCmd_eKeyLadderOpera/4) * 4
		 - (BCMD_GenKey_InCmd_eKeyBuffer/4) * 4;

	commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eKeyLadderOpera/4 - BCMD_CommonGenKey_InCmd_eKeyLadderType/4) ] = 
			inoutp_generateRouteKeyIO->bIs3DESDecrypt;
	commandData.unInputParamLen +=  (BCMD_GenKey_InCmd_eProcIn/4) * 4
		 - (BCMD_GenKey_InCmd_eKeyLadderOpera/4) * 4;

	for (i=0, 
		j = (BCMD_GenKey_InCmd_eProcIn/4 - BCMD_CommonGenKey_InCmd_eKeyLadderType/4)
			+ ( BHSM_GEN_ROUTE_KEY_DATA_LEN-inoutp_generateRouteKeyIO->ucKeyDataLen)/4;
		i<inoutp_generateRouteKeyIO->ucKeyDataLen; j++, i+=4) {
			
		commandData.unInputParamsBuf[j ] = 
			inoutp_generateRouteKeyIO->aucKeyData[i] << 24 |
			inoutp_generateRouteKeyIO->aucKeyData[i+1] << 16 |
			inoutp_generateRouteKeyIO->aucKeyData[i+2] << 8 |
			inoutp_generateRouteKeyIO->aucKeyData[i+3]  ;
			
/*		commandData.unInputParamLen +=  4;		 */
	}

	/* 12/14: Fixed command length */
	commandData.unInputParamLen +=  (BCMD_GenKey_InCmd_eKeySize/4) * 4
		 - (BCMD_GenKey_InCmd_eProcIn/4) * 4;	

	commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eKeySize/4 - BCMD_CommonGenKey_InCmd_eKeyLadderType/4) ] = 
			inoutp_generateRouteKeyIO->keySize;
	commandData.unInputParamLen +=  (BCMD_GenKey_InCmd_eRouteKeyFlag/4) * 4
		 - (BCMD_GenKey_InCmd_eKeySize/4) * 4;

	commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eRouteKeyFlag/4 - BCMD_CommonGenKey_InCmd_eKeyLadderType/4) ] = 
			inoutp_generateRouteKeyIO->bIsRouteKeyRequired;
	commandData.unInputParamLen +=  (BCMD_GenKey_InCmd_eBlkType/4) * 4
		 - (BCMD_GenKey_InCmd_eRouteKeyFlag/4) * 4;

	commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eBlkType/4 - BCMD_CommonGenKey_InCmd_eKeyLadderType/4) ] = 
			inoutp_generateRouteKeyIO->keyDestBlckType;
	commandData.unInputParamLen +=  (BCMD_GenKey_InCmd_eEntryType/4) * 4
		 - (BCMD_GenKey_InCmd_eBlkType/4) * 4;
	
	commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eEntryType/4 - BCMD_CommonGenKey_InCmd_eKeyLadderType/4) ] = 
			inoutp_generateRouteKeyIO->keyDestEntryType;
	commandData.unInputParamLen +=  (BCMD_GenKey_InCmd_eKeySlotType/4) * 4
		 - (BCMD_GenKey_InCmd_eEntryType/4) * 4;

	commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eKeySlotType/4 - BCMD_CommonGenKey_InCmd_eKeyLadderType/4) ] = 
			inoutp_generateRouteKeyIO->caKeySlotType;
	commandData.unInputParamLen +=  (BCMD_GenKey_InCmd_eKeySlotNumber/4) * 4
		 - (BCMD_GenKey_InCmd_eKeySlotType/4) * 4;
   
	commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eKeySlotNumber/4 - BCMD_CommonGenKey_InCmd_eKeyLadderType/4) ] = 
			inoutp_generateRouteKeyIO->unKeySlotNum;
	commandData.unInputParamLen +=  (BCMD_GenKey_InCmd_eKeyMode/4) * 4
		 - (BCMD_GenKey_InCmd_eKeySlotNumber/4) * 4;

	commandData.unInputParamsBuf[(BCMD_GenKey_InCmd_eKeyMode/4 - BCMD_CommonGenKey_InCmd_eKeyLadderType/4) ] = 
			inoutp_generateRouteKeyIO->keyMode;
	commandData.unInputParamLen +=  4;		

/*
	printf("Key ladder Data length = %d\n", commandData.unInputParamLen );
	printf("Key ladder generate route key data :\n");	
	for ( i= 0; i<commandData.unInputParamLen/4; i++)
		printf ("data[%d]= %08lx\n ",i,commandData.unInputParamsBuf[i]);
*/
	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

	/* Parse the command specific output parameters */
	inoutp_generateRouteKeyIO->unStatus =  commandData.unOutputParamsBuf[0];
	unParamLen += 4;	

	/* Check output paramter length */
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_PARM_LEN_ERR, 
		(unParamLen != commandData.unOutputParamLen) );	
	
 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_generateRouteKeyIO->unStatus != 0) );	
	
BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_GenerateRouteKey);
	return( errCode );
}


BERR_Code BHSM_ProcOutCmd (
		BHSM_Handle			in_handle,
		BHSM_ProcOutCmdIO_t	*inoutp_procOutCmdIO
)
{
	BERR_Code errCode = BERR_SUCCESS;
	BHSM_ChannelHandle channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t unParamLen = 0,  i,  j;
	BHSM_P_CommandData_t commandData;

	BDBG_ENTER(BHSM_ProcOutCmd);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	commandData.cmdId = BCMD_cmdType_eSESSION_PROC_OUT_CMD;
	commandData.unContMode = 0;
	
	/* only fill the command specific input parameters */
	commandData.unInputParamsBuf[(BCMD_ProcOut_InCmd_eKeySrc)/4 - 
			(BCMD_CommonBufferFields_eStatus)/4] = 
			inoutp_procOutCmdIO->keyId;
	commandData.unInputParamLen +=  (BCMD_ProcOut_InCmd_eProcIn/4) * 4
		 - (BCMD_ProcOut_InCmd_eKeySrc/4) * 4;

	for (i=0, 
		j = (BCMD_ProcOut_InCmd_eProcIn/4 - BCMD_ProcOut_InCmd_eKeySrc/4);
		i<inoutp_procOutCmdIO->unProcInLen; j++, i+=4) {
			
		commandData.unInputParamsBuf[j ] = 
			inoutp_procOutCmdIO->aucProcIn[i] << 24 |
			inoutp_procOutCmdIO->aucProcIn[i+1] << 16 |
			inoutp_procOutCmdIO->aucProcIn[i+2] << 8 |
			inoutp_procOutCmdIO->aucProcIn[i+3]  ;
			
		commandData.unInputParamLen +=  4;
	}	
	
	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

	/* Parse the command specific output parameters */
	inoutp_procOutCmdIO->unStatus =  commandData.unOutputParamsBuf[0];
	unParamLen += 4;	

	inoutp_procOutCmdIO->unProcOutLen = commandData.unOutputParamLen  - unParamLen;
		
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_PARM_LEN_ERR, 
		(inoutp_procOutCmdIO->unStatus != 0) );		

	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_PARM_LEN_ERR, 
		(inoutp_procOutCmdIO->unProcOutLen > BHSM_MAX_PROC_OUT_DATA_LEN) );
	
	for (i=0, j = 1; i<inoutp_procOutCmdIO->unProcOutLen; j++, i+=4) {

		inoutp_procOutCmdIO->aucProcOut[i]     = (commandData.unOutputParamsBuf[j ] >> 24) & 0xFF;
		inoutp_procOutCmdIO->aucProcOut[i+1] = (commandData.unOutputParamsBuf[j ] >> 16) & 0xFF; 
		inoutp_procOutCmdIO->aucProcOut[i+2] = (commandData.unOutputParamsBuf[j ] >> 8) & 0xFF;
		inoutp_procOutCmdIO->aucProcOut[i+3] = commandData.unOutputParamsBuf[j ] & 0xFF; 
	}

	/* Cannot check output paramter length for this one */
	/*BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_PARM_LEN_ERR, 
		(unParamLen != commandData.unOutputParamLen) );	*/

 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_procOutCmdIO->unStatus != 0) );	
	
BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_ProcOutCmd);
	return( errCode );
}

BERR_Code BHSM_InvalidateKey (
		BHSM_Handle				in_handle,
		BHSM_InvalidateKeyIO_t		*inoutp_invalidateKeyIO
)
{
	BERR_Code errCode = BERR_SUCCESS;
	BHSM_ChannelHandle channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t unParamLen = 0;
	BHSM_P_CommandData_t commandData;

	BDBG_ENTER(BHSM_InvalidateKey);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	commandData.cmdId = BCMD_cmdType_eSESSION_INVALIDATE_KEY;
	commandData.unContMode = 0;
	
	/* only fill the command specific input parameters */
	commandData.unInputParamsBuf[(BCMD_InvalidateKey_InCmd_eKeyFlag/4 - BCMD_CommonBufferFields_eStatus/4)] = 
			inoutp_invalidateKeyIO->invalidKeyType;
	commandData.unInputParamLen +=  (BCMD_InvalidateKey_InCmd_eKeySrc/4) * 4
		 - (BCMD_InvalidateKey_InCmd_eKeyFlag/4) * 4;

	commandData.unInputParamsBuf[(BCMD_InvalidateKey_InCmd_eKeySrc/4 - BCMD_InvalidateKey_InCmd_eKeyFlag/4) ] = 
			inoutp_invalidateKeyIO->keySrc;
	commandData.unInputParamLen +=  (BCMD_InvalidateKey_InCmd_eBlkType/4) * 4
		 - (BCMD_InvalidateKey_InCmd_eKeySrc/4) * 4;

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

	commandData.unInputParamsBuf[(BCMD_InvalidateKey_InCmd_eKeySlotNumber/4 - BCMD_InvalidateKey_InCmd_eKeyFlag/4) ] = 
			inoutp_invalidateKeyIO->unKeySlotNum;
	commandData.unInputParamLen +=  4;

	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

	/* Parse the command specific output parameters */
	inoutp_invalidateKeyIO->unStatus =  commandData.unOutputParamsBuf[0];
	unParamLen += 4;	

	/* Check output paramter length */
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_PARM_LEN_ERR, 
		(unParamLen != commandData.unOutputParamLen) );

 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_invalidateKeyIO->unStatus != 0) );		
	
BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_InvalidateKey);
	return( errCode );
}

BERR_Code BHSM_ConfigAlgorithm (
		BHSM_Handle				in_handle,
		BHSM_ConfigAlgorithmIO_t	*inoutp_configAlgorithmIO
)
{
	BERR_Code errCode = BERR_SUCCESS;
	BHSM_ChannelHandle channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t unParamLen = 0, unOprMode = 0, unGlobalCtrl = 0, unCryptoAlg =0, unResidueMode = 0;
	BHSM_P_CommandData_t commandData;
	unsigned char ucKeySlotStartOffset = 0 ;

	unsigned int unKeySlotNum = inoutp_configAlgorithmIO->unKeySlotNum;

	BDBG_ENTER(BHSM_ConfigAlgorithm);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	commandData.cmdId = BCMD_cmdType_eSESSION_CONFIG_ALGORITHM;
	commandData.unContMode = 0;
	
	/* only fill the command specific input parameters */
	commandData.unInputParamsBuf[(BCMD_Alg_InCmdCfg_eKeySrc/4 - BCMD_CommonBufferFields_eStatus/4)] = 
			inoutp_configAlgorithmIO->keySource;
	commandData.unInputParamLen +=  (BCMD_Alg_InCmdCfg_eBlkType/4) * 4
		 - (BCMD_Alg_InCmdCfg_eKeySrc/4) * 4;

	commandData.unInputParamsBuf[(BCMD_Alg_InCmdCfg_eBlkType/4 - BCMD_Alg_InCmdCfg_eKeySrc/4) ] = 
			inoutp_configAlgorithmIO->keyDestBlckType;
	commandData.unInputParamLen +=  (BCMD_Alg_InCmdCfg_eEntryType/4) * 4
		 - (BCMD_Alg_InCmdCfg_eBlkType/4) * 4;

	commandData.unInputParamsBuf[(BCMD_Alg_InCmdCfg_eEntryType/4 - BCMD_Alg_InCmdCfg_eKeySrc/4) ] = 
			inoutp_configAlgorithmIO->keyDestEntryType;
	commandData.unInputParamLen +=  (BCMD_Alg_InCmdCfg_eSlotType/4) * 4
		 - (BCMD_Alg_InCmdCfg_eEntryType/4) * 4;		
	
	commandData.unInputParamsBuf[(BCMD_Alg_InCmdCfg_eSlotType/4 - BCMD_Alg_InCmdCfg_eKeySrc/4) ] = 
			inoutp_configAlgorithmIO->caKeySlotType;
	commandData.unInputParamLen +=  (BCMD_Alg_InCmdCfg_eSlotNumber/4) * 4
		 - (BCMD_Alg_InCmdCfg_eSlotType/4) * 4;	
	
	commandData.unInputParamsBuf[(BCMD_Alg_InCmdCfg_eSlotNumber/4 - BCMD_Alg_InCmdCfg_eKeySrc/4) ] = 
			inoutp_configAlgorithmIO->unKeySlotNum;
	commandData.unInputParamLen +=  (BCMD_Alg_InCmdCfg_eOperaModeWord0/4) * 4
		 - (BCMD_Alg_InCmdCfg_eSlotNumber/4) * 4;

	switch (inoutp_configAlgorithmIO->keyDestBlckType) {
		case BCMD_KeyDestBlockType_eMem2Mem:
			unCryptoAlg = inoutp_configAlgorithmIO->cryptoAlg.m2mCryptAlg.m2mSecAlg;
			break;
			
		case BCMD_KeyDestBlockType_eCA:
		case BCMD_KeyDestBlockType_eReserved0:
		case BCMD_KeyDestBlockType_eReserved1:
		case BCMD_KeyDestBlockType_eRmx:
			ucKeySlotStartOffset = 
				in_handle->aunCAKeySlotTypeInfo[inoutp_configAlgorithmIO->caKeySlotType].ucKeySlotStartOffset;
			unCryptoAlg = inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.caSecAlg;
			break;

		default:
			errCode = BERR_TRACE((BHSM_STATUS_INPUT_PARM_ERR));
			goto BHSM_P_DONE_LABEL;	
			
	}

	if ((inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.caSecAlg == BCMD_XptSecCryptoAlg_eReserved0) ||
		(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.caSecAlg == BCMD_XptSecCryptoAlg_eDvb)) {
		unResidueMode = inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.residueMode.dvbScrambleLevel;
	}
	else {
		unResidueMode = inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.residueMode.residueMode;
	}

#if (BCHP_CHIP==7401) && (BCHP_VER == BCHP_VER_A0)
	if (inoutp_configAlgorithmIO->keyDestBlckType < BCMD_KeyDestBlockType_eMem2Mem) {
		unOprMode = 
			(unCryptoAlg << BHSM_CaModeShift_eCryptoAlgorithmShift) |
			(unResidueMode << BHSM_CaModeShift_eResidueModeShift) |
			(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.bScrambleCtrlMod << BHSM_CaModeShift_eScModShift) |
			(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.bRestrictEnable << BHSM_CaModeShift_eRestrEnShift) |
			(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.bGlobalEnable << BHSM_CaModeShift_eGlobalEnShift) ;

		unGlobalCtrl = 
			(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.bEncScPolarity << BHSM_CaModeShift_eEncScPolarityShift) |
			(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.ucMulti2KeySelect << BHSM_CaModeShift_eMulti2KeySelectShift) |
			(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.bAtscScrambleCtrl << BHSM_CaModeShift_eAtscScrambleCtrlShift) |
			(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.bGlobalDropPktCtrl << BHSM_CaModeShift_eGlobalDropPktCtrlShift) |
			(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.bGlobalRegOverwrite << BHSM_CaModeShift_eGlobalRegOverwriteShift) ;

		
		BDBG_MSG(("unOprMode = 0x%lx, unGlobalCtrl = 0x%lx\n", unOprMode, unGlobalCtrl));
	}
	else { /* M2M */
		unOprMode = 
			(unCryptoAlg << BHSM_M2mModeShift_eCryptoAlgorithmShift) |
			(inoutp_configAlgorithmIO->cryptoAlg.m2mCryptAlg.ucAESCounterKeyMode << BHSM_M2mModeShift_eAESCounterKeyModeShift) |
			(inoutp_configAlgorithmIO->cryptoAlg.m2mCryptAlg.ucAuthCtrl << BHSM_M2mModeShift_eAuthCtrlShift)  ;

		BDBG_MSG(("unOprMode = 0x%lx, unCryptoAlg = 0x%lx, ucAESCounterKeyMode = 0x%lx, ucAuthCtrl = 0x%lx\n", 
			unOprMode, unCryptoAlg, inoutp_configAlgorithmIO->cryptoAlg.m2mCryptAlg.ucAESCounterKeyMode,
			inoutp_configAlgorithmIO->cryptoAlg.m2mCryptAlg.ucAuthCtrl));
	}

	switch (inoutp_configAlgorithmIO->keyDestBlckType) {

		case BCMD_KeyDestBlockType_eReserved0:
			in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+unKeySlotNum)].aKeySlotAlgorithm[inoutp_configAlgorithmIO->keyDestEntryType].ulCPDControlBits = unOprMode;			
			break;
			
		case BCMD_KeyDestBlockType_eCA:
			in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+unKeySlotNum)].aKeySlotAlgorithm[inoutp_configAlgorithmIO->keyDestEntryType].ulCAControlBits = unOprMode;
			break;

			
		case BCMD_KeyDestBlockType_eReserved1:
		case BCMD_KeyDestBlockType_eRmx:
			in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+unKeySlotNum)].aKeySlotAlgorithm[inoutp_configAlgorithmIO->keyDestEntryType].ulCPSControlBits = unOprMode;
			break;

		case BCMD_KeyDestBlockType_eMem2Mem:
			/* Don't have to save it for M2M */
			break;
			

		default:
			errCode = BERR_TRACE((BHSM_STATUS_INPUT_PARM_ERR));
			goto BHSM_P_DONE_LABEL;	
			
	}

	if (inoutp_configAlgorithmIO->keyDestBlckType < BCMD_KeyDestBlockType_eMem2Mem) {

		BDBG_MSG(("ulCAControlBits = 0x%lx, ulCPDControlBits = 0x%lx, OperaModeWord0= 0x%lx\n", 
			in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+unKeySlotNum)].aKeySlotAlgorithm[inoutp_configAlgorithmIO->keyDestEntryType].ulCAControlBits,
			in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+unKeySlotNum)].aKeySlotAlgorithm[inoutp_configAlgorithmIO->keyDestEntryType].ulCPDControlBits,
			((in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+unKeySlotNum)].aKeySlotAlgorithm[inoutp_configAlgorithmIO->keyDestEntryType].ulCAControlBits 
					&  BHSM_CaModeShift_eCAMask) << BHSM_CaModeShift_eCAShift) |
			((in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+unKeySlotNum)].aKeySlotAlgorithm[inoutp_configAlgorithmIO->keyDestEntryType].ulCPDControlBits
					& BHSM_CaModeShift_eCPD0Mask) << BHSM_CaModeShift_eCPD0Shift)));
		
		commandData.unInputParamsBuf[(BCMD_Alg_InCmdCfg_eOperaModeWord0/4 - BCMD_Alg_InCmdCfg_eKeySrc/4) ] = 
			((in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+unKeySlotNum)].aKeySlotAlgorithm[inoutp_configAlgorithmIO->keyDestEntryType].ulCAControlBits 
					&  BHSM_CaModeShift_eCAMask) << BHSM_CaModeShift_eCAShift) |
			((in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+unKeySlotNum)].aKeySlotAlgorithm[inoutp_configAlgorithmIO->keyDestEntryType].ulCPDControlBits
					& BHSM_CaModeShift_eCPD0Mask) << BHSM_CaModeShift_eCPD0Shift);

		commandData.unInputParamLen +=  (BCMD_Alg_InCmdCfg_eOperaModeWord1/4) * 4
		 	- (BCMD_Alg_InCmdCfg_eOperaModeWord0/4) * 4;
		
		commandData.unInputParamsBuf[(BCMD_Alg_InCmdCfg_eOperaModeWord1/4 - BCMD_Alg_InCmdCfg_eKeySrc/4) ] = 
			((in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+unKeySlotNum)].aKeySlotAlgorithm[inoutp_configAlgorithmIO->keyDestEntryType].ulCPDControlBits 
					&  BHSM_CaModeShift_eCPD1Mask) << BHSM_CaModeShift_eCPD1Shift) |
			((in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+unKeySlotNum)].aKeySlotAlgorithm[inoutp_configAlgorithmIO->keyDestEntryType].ulCPSControlBits
					& BHSM_CaModeShift_eCPSMask) << BHSM_CaModeShift_eCPSShift);
		
		commandData.unInputParamLen +=  (BCMD_Alg_InCmdCfg_eGlobalXptSecureCtrl/4) * 4
			 - (BCMD_Alg_InCmdCfg_eOperaModeWord1/4) * 4;

		commandData.unInputParamsBuf[(BCMD_Alg_InCmdCfg_eGlobalXptSecureCtrl/4 - BCMD_Alg_InCmdCfg_eKeySrc/4) ] = 
				unGlobalCtrl;
		commandData.unInputParamLen +=  (BCMD_Alg_InCmdCfg_eDummyWord/4) * 4
			 - (BCMD_Alg_InCmdCfg_eGlobalXptSecureCtrl/4) * 4;

		commandData.unInputParamsBuf[(BCMD_Alg_InCmdCfg_eDummyWord/4 - BCMD_Alg_InCmdCfg_eKeySrc/4) ] = 
				0;
		commandData.unInputParamLen +=  4;
		/* commandData.unInputParamLen +=  (BCMD_Alg_InCmdCfg_eKeyMode/4) * 4
			 - (BCMD_Alg_InCmdCfg_eDummyWord/4) * 4;			 */
	}
	else { /* M2M */
		commandData.unInputParamsBuf[(BCMD_Alg_InCmdCfg_eOperaModeWord0/4 - BCMD_Alg_InCmdCfg_eKeySrc/4) ] = 
			unOprMode;
		commandData.unInputParamLen +=  (BCMD_Alg_InCmdCfg_eOperaModeWord1/4) * 4
		 - (BCMD_Alg_InCmdCfg_eOperaModeWord0/4) * 4;

		/* Next 12 bytes should be all 0  for M2M */
		commandData.unInputParamLen += 12;
		
	}

#else		/* 7400 A0 */
	if (inoutp_configAlgorithmIO->keyDestBlckType < BCMD_KeyDestBlockType_eMem2Mem) {
		unOprMode = 
			(unCryptoAlg << BHSM_CaModeShift_eCryptoAlgorithmShift) |
			(unResidueMode << BHSM_CaModeShift_eResidueModeShift) |
			/* (inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.bScrambleCtrlMod << BHSM_CaModeShift_eScModShift) |*/
			(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.bRestrictEnable << BHSM_CaModeShift_eRestrEnShift) |
			(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.bGlobalEnable << BHSM_CaModeShift_eGlobalEnShift) |
			(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.bRestrictScMod << BHSM_CaModeShift_eRestrScModShift) |
			(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.bGlobalScMod << BHSM_CaModeShift_eGlobalScModShift) ;

		unGlobalCtrl = 
			(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.bEncScPolarity << BHSM_CaModeShift_eEncScPolarityShift) |
			(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.ucMulti2KeySelect << BHSM_CaModeShift_eMulti2KeySelectShift) |
			(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.bAtscScrambleCtrl << BHSM_CaModeShift_eAtscScrambleCtrlShift) |
			(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.bAtscMod << BHSM_CaModeShift_eAtscScModShift) |
			(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.bGlobalDropPktCtrl << BHSM_CaModeShift_eGlobalDropPktCtrlShift) |
			(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.bRestrictDropPktCtrl << BHSM_CaModeShift_eRestrictDropPktCtrlShift) |
			(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.bGlobalRegOverwrite << BHSM_CaModeShift_eGlobalRegOverwriteShift) |
			(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.bCPDDisable << BHSM_CaModeShift_eCPDDisableShift) |
			(inoutp_configAlgorithmIO->cryptoAlg.caCryptAlg.bCPSDisable << BHSM_CaModeShift_eCPSDisableShift) ;
		
		BDBG_MSG(("unOprMode = 0x%lx, unGlobalCtrl = 0x%lx\n", unOprMode, unGlobalCtrl));

	}
	else { /* M2M */
		unOprMode = 
			(unCryptoAlg << BHSM_M2mModeShift_eCryptoAlgorithmShift) |
			(inoutp_configAlgorithmIO->cryptoAlg.m2mCryptAlg.ucAESCounterKeyMode << BHSM_M2mModeShift_eAESCounterKeyModeShift) |
			(inoutp_configAlgorithmIO->cryptoAlg.m2mCryptAlg.ucAuthCtrl << BHSM_M2mModeShift_eAuthCtrlShift)  ;
		
	}

	if (inoutp_configAlgorithmIO->keyDestBlckType < BCMD_KeyDestBlockType_eMem2Mem) {

		switch (inoutp_configAlgorithmIO->keyDestBlckType) {

			case BCMD_KeyDestBlockType_eReserved0:
				in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+unKeySlotNum)].aKeySlotAlgorithm[inoutp_configAlgorithmIO->keyDestEntryType].ulCPDControlBits = unOprMode;			
				break;
				
			case BCMD_KeyDestBlockType_eCA:
				in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+unKeySlotNum)].aKeySlotAlgorithm[inoutp_configAlgorithmIO->keyDestEntryType].ulCAControlBits = unOprMode;
				break;

				
			case BCMD_KeyDestBlockType_eReserved1:
			case BCMD_KeyDestBlockType_eRmx:
				in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+unKeySlotNum)].aKeySlotAlgorithm[inoutp_configAlgorithmIO->keyDestEntryType].ulCPSControlBits = unOprMode;
				break;

			default:
				errCode = BERR_TRACE((BHSM_STATUS_INPUT_PARM_ERR));
				goto BHSM_P_DONE_LABEL;	
				
		}

		commandData.unInputParamsBuf[(BCMD_Alg_InCmdCfg_eOperaModeWord0/4 - BCMD_Alg_InCmdCfg_eKeySrc/4) ] =
			unGlobalCtrl;					
		commandData.unInputParamLen +=  (BCMD_Alg_InCmdCfg_eOperaModeWord1/4) * 4
		 	- (BCMD_Alg_InCmdCfg_eOperaModeWord0/4) * 4;
		
		commandData.unInputParamsBuf[(BCMD_Alg_InCmdCfg_eOperaModeWord1/4 - BCMD_Alg_InCmdCfg_eKeySrc/4) ] = 
			(in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+unKeySlotNum)].aKeySlotAlgorithm[inoutp_configAlgorithmIO->keyDestEntryType].ulCAControlBits 
					&  BHSM_CaModeShift_eCAMask) ;		
		commandData.unInputParamLen +=  (BCMD_Alg_InCmdCfg_eOperaModeWord2/4) * 4
			 - (BCMD_Alg_InCmdCfg_eOperaModeWord1/4) * 4;

		commandData.unInputParamsBuf[(BCMD_Alg_InCmdCfg_eOperaModeWord2/4 - BCMD_Alg_InCmdCfg_eKeySrc/4) ] = 
			(in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+unKeySlotNum)].aKeySlotAlgorithm[inoutp_configAlgorithmIO->keyDestEntryType].ulCPSControlBits
					& BHSM_CaModeShift_eCAMask) ;
		
		commandData.unInputParamLen +=  (BCMD_Alg_InCmdCfg_eOperaModeWord3/4) * 4
			 - (BCMD_Alg_InCmdCfg_eOperaModeWord2/4) * 4;

		commandData.unInputParamsBuf[(BCMD_Alg_InCmdCfg_eOperaModeWord3/4 - BCMD_Alg_InCmdCfg_eKeySrc/4) ] = 
							(in_handle->aunCAKeySlotInfo[(ucKeySlotStartOffset+unKeySlotNum)].aKeySlotAlgorithm[inoutp_configAlgorithmIO->keyDestEntryType].ulCPDControlBits 
					&  BHSM_CaModeShift_eCAMask);

		commandData.unInputParamLen +=  4;
		/* commandData.unInputParamLen +=  (BCMD_Alg_InCmdCfg_eKeyMode/4) * 4
			 - (BCMD_Alg_InCmdCfg_eOperaModeWord3/4) * 4; */
	}
	else { /* M2M */
		commandData.unInputParamsBuf[(BCMD_Alg_InCmdCfg_eOperaModeWord0/4 - BCMD_Alg_InCmdCfg_eKeySrc/4) ] = 
			unOprMode;
		commandData.unInputParamLen +=  (BCMD_Alg_InCmdCfg_eOperaModeWord1/4) * 4
		 - (BCMD_Alg_InCmdCfg_eOperaModeWord0/4) * 4;
	}
#endif		
		
		

/*	Key mode moved to LoadRouteKey, GenerateRouteKey.
	commandData.unInputParamsBuf[(BCMD_Alg_InCmdCfg_eKeyMode/4 - BCMD_Alg_InCmdCfg_eKeySrc/4) ] = 
			inoutp_configAlgorithmIO->keyMode;
	commandData.unInputParamLen +=  4;
*/	

	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

	/* Parse the command specific output parameters */
	inoutp_configAlgorithmIO->unStatus =  commandData.unOutputParamsBuf[0];
	unParamLen += 4;	

	/* Check output paramter length */
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_PARM_LEN_ERR, 
		(unParamLen != commandData.unOutputParamLen) );	

 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_configAlgorithmIO->unStatus != 0) );	
	
BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_ConfigAlgorithm);
	return( errCode );
}


BERR_Code BHSM_ConfigMulti2 (
		BHSM_Handle				in_handle,
		BHSM_ConfigMulti2IO_t		*inoutp_configMulti2IO
)
{
	BERR_Code errCode = BERR_SUCCESS;
	BHSM_ChannelHandle channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t unParamLen = 0, i, j;
	BHSM_P_CommandData_t commandData;

	BDBG_ENTER(BHSM_ConfigMulti2);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	commandData.cmdId = BCMD_cmdType_eCONFIG_MULTI2;
	commandData.unContMode = 0;
	
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

	/* Check output paramter length */
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_PARM_LEN_ERR, 
		(unParamLen != commandData.unOutputParamLen) );	

 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_configMulti2IO->unStatus != 0) );	
	
BHSM_P_DONE_LABEL:

	BDBG_LEAVE(BHSM_ConfigMulti2);
	return( errCode );
}

BERR_Code BHSM_ConfigPidChannelToDefaultKeySlot (
		BHSM_Handle						in_handle,
		BHSM_ConfigPidKeyPointerTableIO_t	*inoutp_configPidKeyPointerTableIO
)
{
	BERR_Code 		errCode = BERR_SUCCESS;
	
	BDBG_ENTER(BHSM_ConfigPidChannelToDefaultKeySlot);
	BDBG_ASSERT( in_handle );
	inoutp_configPidKeyPointerTableIO->unPidChannel |= 0x40000000;  /* do a confirm sizeof(int)=4, done*/

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
		BERR_Code 				errCode = BERR_SUCCESS;
		BHSM_ConfigAlgorithmIO_t	inoutp_configAlgorithmIO;
		bool                                 illegalInput=true;


		BDBG_ENTER(BHSM_ResetKeySlotCtrlBits);
		BDBG_ASSERT( in_handle );

		if( kSlotDestBlockType < BCMD_KeyDestBlockType_eInvalid )
		{
			if(  kSlotType<BCMD_XptSecKeySlot_eTypeMax)
			{
			   switch(kSlotType)
			   {
			   	case BCMD_XptSecKeySlot_eType0:
					if( kSlotNum < in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType0].unKeySlotNum)
					{
						illegalInput= false;						
					}
					break;
			   	case BCMD_XptSecKeySlot_eType1:
					if(kSlotNum < in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType1].unKeySlotNum)
					{
						illegalInput= false;						
					}
					break;
			   	case BCMD_XptSecKeySlot_eType2:
					if( kSlotNum < in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType2].unKeySlotNum)
					{
						illegalInput= false;						
					}
					break;
			   	case BCMD_XptSecKeySlot_eType3:
					if( kSlotNum < in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType3].unKeySlotNum)
					{
						illegalInput= false;						
					}
					break;
			   	case BCMD_XptSecKeySlot_eType4:
					if( kSlotNum < in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType4].unKeySlotNum)
					{
						illegalInput= false;						
					}
					break;
			   	case BCMD_XptSecKeySlot_eType5:
					if( kSlotNum < in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType5].unKeySlotNum)
					{
						illegalInput= false;						
					}
					break;
			   	case BCMD_XptSecKeySlot_eType6:
					if( kSlotNum < in_handle->aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eType6].unKeySlotNum)
					{
						illegalInput= false;						
					}
					break;

				default: break;
					
			   }
			}
		}

		BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, illegalInput );					
		
		BKNI_Memset(&inoutp_configAlgorithmIO, 0, sizeof(BHSM_ConfigAlgorithmIO_t));	

		inoutp_configAlgorithmIO.caKeySlotType 	= kSlotType;
		inoutp_configAlgorithmIO.unKeySlotNum 	= kSlotNum;
		inoutp_configAlgorithmIO.keyDestBlckType = kSlotDestBlockType;

		switch(kSlotDestBlockType)
		{
			case  BCMD_KeyDestBlockType_eReserved0: 
			case  BCMD_KeyDestBlockType_eCA: 
			case  BCMD_KeyDestBlockType_eReserved1: 			
			case  BCMD_KeyDestBlockType_eRmx: 			  
					inoutp_configAlgorithmIO.keyDestEntryType= BCMD_KeyDestEntryType_eOddKey;
				 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
					              BHSM_ConfigAlgorithm(in_handle, &inoutp_configAlgorithmIO)  );

					inoutp_configAlgorithmIO.keyDestEntryType= BCMD_KeyDestEntryType_eEvenKey;
				 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
					              BHSM_ConfigAlgorithm(in_handle, &inoutp_configAlgorithmIO)  );

					if (kSlotType == BCMD_XptSecKeySlot_eType6)
					{
						inoutp_configAlgorithmIO.keyDestEntryType= BCMD_KeyDestEntryType_eReserved0;
					 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
						              BHSM_ConfigAlgorithm(in_handle, &inoutp_configAlgorithmIO)  );
					}
					break;

			case  BCMD_KeyDestBlockType_eMem2Mem: 			  
				 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
					              BHSM_ConfigAlgorithm(in_handle, &inoutp_configAlgorithmIO)  );
					break;

			case  BCMD_KeyDestBlockType_eHdmi: 	
					break;      /* TODO  HDMI */
					BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 1 );					

#if defined(BCM7400B0)				
			case  BCMD_KeyDestBlockType_eReserved2: 
					break;		/* TODO  what is this? eye on this */
					BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 1 );					

			case  BCMD_KeyDestBlockType_eMem2Mem1:   /* TODO  same as M2M?  */
				 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
					              BHSM_ConfigAlgorithm(in_handle, &inoutp_configAlgorithmIO)  );
					break;
					
#endif				
			default: break;
		}		


BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_ResetKeySlotCtrlBits);
	return( errCode );
		
}
