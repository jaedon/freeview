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
 * $brcm_Workfile: bhsm_misc.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 2/8/11 8:58a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7640/a0/user_misc_cmd/bhsm_misc.c $
 * 
 * Hydra_Software_Devel/1   2/8/11 8:58a cdisc
 * SWBLURAY-23691: branching files
 * 
 * Hydra_Software_Devel/1   1/25/11 6:42p atruong
 * SW7422-26: Add HSM PI Support For 7422A0  - CC header format
 *
 ******************************************************************************/


#include "bhsm_misc.h"



BDBG_MODULE(BHSM);



BERR_Code BHSM_SetMiscBits (
		BHSM_Handle				in_handle,
		BHSM_SetMiscBitsIO_t	*inoutp_setMiscBitsIO
)
{
	BERR_Code 				errCode = BERR_SUCCESS;
	BHSM_ChannelHandle 		channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t 				unParamLen = 0;
	BHSM_P_CommandData_t 	commandData;	

	BDBG_MSG(("Inside BHSM_SetMiscBits\n"));
	BDBG_ENTER(BHSM_SetMiscBits);
	BDBG_ASSERT( in_handle );
	BDBG_ASSERT( inoutp_setMiscBitsIO );

	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	commandData.cmdId = BCMD_cmdType_eOFFLINE_RAVE_COMMAND;
	commandData.unContMode = 0;
	
	/* only fill the command specific input parameters */

	commandData.unInputParamsBuf[unParamLen/4] =  inoutp_setMiscBitsIO->setMiscBitsSubCmd;
	unParamLen += 4;

	switch (inoutp_setMiscBitsIO->setMiscBitsSubCmd)
	{
		
		case BCMD_SetMiscBitsSubCmd_eRaveBits:
			
			commandData.unInputParamsBuf[unParamLen/4] =
			            ((inoutp_setMiscBitsIO->bDisableClear         & 0x1)  << 4) |
			            ((inoutp_setMiscBitsIO->bRAVEEncryptionBypass & 0x1)  << 3) |
			            ((inoutp_setMiscBitsIO->bEnableReadDMem       & 0x1)  << 2) |
			            ((inoutp_setMiscBitsIO->bEnableReadIMem       & 0x1)  << 1) |
			            ( inoutp_setMiscBitsIO->bEnableWriteIMem      & 0x1);
			break;
			
		case BCMD_SetMiscBitsSubCmd_eReserved1: 		
			commandData.unInputParamsBuf[unParamLen/4] =
			            ((inoutp_setMiscBitsIO->bDMAM2MPacketBypassEnable & 0x1) << 2) |
			            ((inoutp_setMiscBitsIO->bNMAPacketBypassEnable    & 0x1) << 1) |
			             (inoutp_setMiscBitsIO->bDMAXPTPacketBypassEnable & 0x1);
			break;

		case BCMD_SetMiscBitsSubCmd_eM2MEndianSwapBits:
			commandData.unInputParamsBuf[unParamLen/4] =
			            ((inoutp_setMiscBitsIO->bCBCMACKeySwapEnable     & 0x1) << 2) |
			            ((inoutp_setMiscBitsIO->bCBCMACDataOutSwapEnable & 0x1) << 1) |
			             (inoutp_setMiscBitsIO->bCBCMACDataInSwapEnable  & 0x1);
			break;
		
		case BCMD_SetMiscBitsSubCmd_eForceSCBit:
			commandData.unInputParamsBuf[unParamLen/4] =
			             (inoutp_setMiscBitsIO->bBandSelect_PB  & 0xFF)      |
			            ((inoutp_setMiscBitsIO->bandNumber      & 0xFF) << 8);
			unParamLen += 4;

			commandData.unInputParamsBuf[unParamLen/4] =
			            ((inoutp_setMiscBitsIO->SCForceEnValue & 0x3) << 2) |
			            ((inoutp_setMiscBitsIO->bSCForceEnNZ   & 0x1) << 1) |
			             (inoutp_setMiscBitsIO->bSCForceEnAll  & 0x1);
			break;

		case BCMD_SetMiscBitsSubCmd_eXPTM2MStatusDump:  /* No parameter, just get status */

			unParamLen -= 4;                            /* adjust for no parameter */
			break;

		default:
			errCode = BERR_TRACE((BHSM_STATUS_INPUT_PARM_ERR));		
			goto BHSM_P_DONE_LABEL;							



	}
	
			
    unParamLen += 4;
			
	commandData.unInputParamLen = unParamLen;
	
	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

	unParamLen = 0;  
	
	inoutp_setMiscBitsIO->unStatus =  commandData.unOutputParamsBuf[unParamLen/4]; 
	unParamLen += 4;

	/* Check status */
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(0 != inoutp_setMiscBitsIO->unStatus  ) );			

	if (inoutp_setMiscBitsIO->setMiscBitsSubCmd == BCMD_SetMiscBitsSubCmd_eXPTM2MStatusDump)
	{
		inoutp_setMiscBitsIO->unM2MSecurityErrStatus =  commandData.unOutputParamsBuf[unParamLen/4];
		unParamLen +=  4;
		inoutp_setMiscBitsIO->unXPTSecurityErrStatus =  commandData.unOutputParamsBuf[unParamLen/4];
		unParamLen +=  4;
	}

BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_SetMiscBits);
	return( errCode );
}


static BERR_Code BHSM_ProgOTPPatternSeq (
		BHSM_Handle					in_handle
)
{
	BERR_Code 				errCode = BERR_SUCCESS;
	uint32_t				unStatus;
	BHSM_ChannelHandle 		channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t 				unParamLen = 0;
	BHSM_P_CommandData_t	commandData;
	unsigned int			i;

	BDBG_MSG(("Inside BHSM_ProgOTPPatternSeq\n"));
	BDBG_ENTER(BHSM_ProgOTPPatternSeq);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );
	
	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	commandData.cmdId = 0x51;
	commandData.unContMode = 0;
	
	/* only fill the command specific input parameters */
	i = 0;
	commandData.unInputParamsBuf[i++] = 0xBC32F4AC;
	commandData.unInputParamLen += 4;
	commandData.unInputParamsBuf[i++] = 0xD18616B6;
	commandData.unInputParamLen += 4;
	commandData.unInputParamsBuf[i++] = 0x9FEB4D54;
	commandData.unInputParamLen += 4;
	commandData.unInputParamsBuf[i++] = 0x4A27BF4A;
	commandData.unInputParamLen += 4;
	commandData.unInputParamsBuf[i++] = 0xCF1C3178;
	commandData.unInputParamLen += 4;
	commandData.unInputParamsBuf[i++] = 0xE2DB98A0;
	commandData.unInputParamLen += 4;
	commandData.unInputParamsBuf[i++] = 0x24F64BBA;
	commandData.unInputParamLen += 4;
	commandData.unInputParamsBuf[i++] = 0x7698E712;
	commandData.unInputParamLen += 4;
	commandData.unInputParamsBuf[i++] = 0x0000F48D;
	commandData.unInputParamLen += 4;

	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

	/* Parse the command specific output parameters */
	unStatus =  commandData.unOutputParamsBuf[0];

	unParamLen += 4;	

 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(unStatus != 0) );			

BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_ProgOTPPatternSeq);
	return( errCode );



}


BERR_Code	BHSM_ReadOTPId(
		BHSM_Handle			in_handle,
		BHSM_ReadOTPIdIO_t	*inoutp_readOTPIdIO
)
{
	BERR_Code errCode = BERR_SUCCESS;
	BHSM_ChannelHandle channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t unParamLen = 0, i, j;
	BHSM_P_CommandData_t commandData;	


	BDBG_MSG(("Inside BHSM_ReadOTPId\n"));
	BDBG_ENTER(BHSM_ReadOTPId);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BDBG_ASSERT( inoutp_readOTPIdIO );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(BHSM_ProgOTPPatternSeq(in_handle) != BERR_SUCCESS ) );	

	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	commandData.cmdId      = 0x19;         /* For OTP Read operation */
	commandData.unContMode = 0;
	
	/* only fill the command specific input parameters */
	commandData.unInputParamsBuf[0] = (inoutp_readOTPIdIO->OtpId << 8) | 0x0F;
	commandData.unInputParamLen += 4;

	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

	/* Parse the command specific output parameters */
	inoutp_readOTPIdIO->unStatus =  commandData.unOutputParamsBuf[0];
	unParamLen += 4;	

	/* First     DWORD is [31:0] of OTP ID */
	/* Second DWORD is [63:32] of OTP ID */
	/* output buffer is    [byte7][byte6][...][byte1][byte0] */

	for (i=0, j = 2; i<BHSM_OTP_ID_LEN; j--, i+=4) {
		inoutp_readOTPIdIO->aucOTPId[i]   = (commandData.unOutputParamsBuf[j ] >> 24) & 0xFF;
		inoutp_readOTPIdIO->aucOTPId[i+1] = (commandData.unOutputParamsBuf[j ] >> 16) & 0xFF; 
		inoutp_readOTPIdIO->aucOTPId[i+2] = (commandData.unOutputParamsBuf[j ] >> 8) & 0xFF;
		inoutp_readOTPIdIO->aucOTPId[i+3] = commandData.unOutputParamsBuf[j ] & 0xFF; 
		unParamLen += 4;		
	}

	inoutp_readOTPIdIO->unIdSize = BHSM_OTP_ID_LEN;
 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_readOTPIdIO->unStatus != 0) );		
	
BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_ReadOTPId);
	return( errCode );
}


BERR_Code BHSM_SetArch (
		BHSM_Handle			in_handle,
		BHSM_SetArchIO_t	*inoutp_setArchIO
)
{
	BERR_Code 				errCode = BERR_SUCCESS;
	BHSM_ChannelHandle 		channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t 				unParamLen = 0;
	BHSM_P_CommandData_t 	commandData;	

	BDBG_MSG(("Inside BHSM_SetArch\n"));
	BDBG_ENTER(BHSM_SetArch);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	


	BDBG_ASSERT( inoutp_setArchIO );
	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	commandData.cmdId = BCMD_cmdType_eOFFLINE_ARCH_COMMAND;
	commandData.unContMode = 0;
	
	/* only fill the command specific input parameters */
	commandData.unInputParamsBuf[unParamLen/4] = 
			inoutp_setArchIO->unLowerRangeAddress;
    unParamLen += 4;

	commandData.unInputParamsBuf[unParamLen/4] = 
			inoutp_setArchIO->unUpperRangeAddress;
    unParamLen += 4;    

	commandData.unInputParamsBuf[unParamLen/4] = 
				inoutp_setArchIO->ArchSel         |
				(inoutp_setArchIO->PCIArch <<  8) |
				(inoutp_setArchIO->DRAMSel << 16);
	unParamLen += 4;	
	
	commandData.unInputParamLen = unParamLen;    
	
	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

    unParamLen =0;    
					
	/* Parse the command specific output parameters */
	inoutp_setArchIO->unStatus =  commandData.unOutputParamsBuf[unParamLen/4];
	unParamLen +=  4;

 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_setArchIO->unStatus != 0) );
BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_SetArch);
	return( errCode );

}




BERR_Code  BHSM_SetVichRegPar (
		BHSM_Handle					in_handle,
		BHSM_SetVichRegParIO_t		*inoutp_setVichIO
)
{

	BERR_Code 				errCode = BERR_SUCCESS;
	BHSM_ChannelHandle 		channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t 				unParamLen = 0;
	BHSM_P_CommandData_t	commandData;
	uint32_t				i;	

	BDBG_MSG(("Inside BHSM_SetVichRegPar\n"));
	BDBG_ENTER(BHSM_SetVichRegPar);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	
	
	BDBG_ASSERT( inoutp_setVichIO );
	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
		(inoutp_setVichIO->nRanges > MAX_AVD_SVD_RANGE) );
	
	commandData.cmdId      = BCMD_cmdType_eOFFLINE_SET_VICH_REG_PAR;
	commandData.unContMode = 0;
	
	/* only fill the command specific input parameters */
	
	commandData.unInputParamsBuf[unParamLen/4 ] = 
	            inoutp_setVichIO->virtualKeyLadderID | (inoutp_setVichIO->keyLayer << 8);
	unParamLen += 4;

	commandData.unInputParamsBuf[unParamLen/4 ] = inoutp_setVichIO->VDECId;
	unParamLen += 4;

	commandData.unInputParamsBuf[unParamLen/4 ] = inoutp_setVichIO->nRanges;
	unParamLen += 4;

	for (i = 0; i < inoutp_setVichIO->nRanges; i++)
	{
		commandData.unInputParamsBuf[unParamLen/4 ] = inoutp_setVichIO->unRangeLo[i];
		unParamLen += 4;

		commandData.unInputParamsBuf[unParamLen/4 ] = inoutp_setVichIO->unRangeHi[i];
		unParamLen += 4;

	}

	/* Load signature */
	for(i = 0; i < BHSM_HMACSHA256_SIGNATURE_SIZE; i += 4)
	{
		 commandData.unInputParamsBuf[unParamLen/4] = 
			 ((unsigned long)inoutp_setVichIO->aucSignature[i])   << 24 |
			 ((unsigned long)inoutp_setVichIO->aucSignature[i+1]) << 16 |
			 ((unsigned long)inoutp_setVichIO->aucSignature[i+2]) <<  8 |
			 ((unsigned long)inoutp_setVichIO->aucSignature[i+3]);

		 unParamLen += 4;	 
	}

    
	commandData.unInputParamLen = unParamLen;

	
	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

    unParamLen =0;    

	/* Parse the command specific output parameters */
	inoutp_setVichIO->unStatus    =  commandData.unOutputParamsBuf[unParamLen/4];
	unParamLen +=  4;
	

 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_setVichIO->unStatus != 0) );
BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_SetVichRegPar);
	return( errCode );


}


BERR_Code  BHSM_StartAVD(
		BHSM_Handle				in_handle,
		BHSM_StartAVDIO_t		*inoutp_startAVDIO
)
{

	BERR_Code 				errCode = BERR_SUCCESS;
	BHSM_ChannelHandle 		channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t 				unParamLen = 0;
	BHSM_P_CommandData_t	commandData;
	uint32_t				i;	

	BDBG_MSG(("Inside BHSM_StartAVD\n"));
	BDBG_ENTER(BHSM_StartAVD);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	
	
	BDBG_ASSERT( inoutp_startAVDIO );
	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
		(inoutp_startAVDIO->numAVDReg > MAX_AVD_SVD_RANGE) );

	commandData.cmdId      = BCMD_cmdType_eSTART_AVD;
	commandData.unContMode = 0;
	
	/* only fill the command specific input parameters */

	commandData.unInputParamsBuf[unParamLen/4 ] = 
	            inoutp_startAVDIO->virtualKeyLadderID | (inoutp_startAVDIO->keyLayer << 8);
	unParamLen += 4;

	commandData.unInputParamsBuf[unParamLen/4 ] = inoutp_startAVDIO->avdID;
	unParamLen += 4;

	commandData.unInputParamsBuf[unParamLen/4 ] = inoutp_startAVDIO->numAVDReg;
	unParamLen += 4;

	for (i = 0; i < inoutp_startAVDIO->numAVDReg; i++)
	{
		commandData.unInputParamsBuf[unParamLen/4 ] = inoutp_startAVDIO->avdRegAddr[i];
		unParamLen += 4;

		commandData.unInputParamsBuf[unParamLen/4 ] = inoutp_startAVDIO->avdRegVal[i];
		unParamLen += 4;

	}

	/* Load signature */
	for(i = 0; i < BHSM_HMACSHA256_SIGNATURE_SIZE; i += 4)
	{
		 commandData.unInputParamsBuf[unParamLen/4] = 
			 ((unsigned long)inoutp_startAVDIO->aucSignature[i])   << 24 |
			 ((unsigned long)inoutp_startAVDIO->aucSignature[i+1]) << 16 |
			 ((unsigned long)inoutp_startAVDIO->aucSignature[i+2]) <<  8 |
			 ((unsigned long)inoutp_startAVDIO->aucSignature[i+3]);

		 unParamLen += 4;	 
	}

    
	commandData.unInputParamLen = unParamLen;

	
	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

    unParamLen =0;    

	/* Parse the command specific output parameters */
	inoutp_startAVDIO->unStatus    =  commandData.unOutputParamsBuf[unParamLen/4];
	unParamLen +=  4;

 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_startAVDIO->unStatus != 0) );

BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_StartAVD);
	return( errCode );


}








