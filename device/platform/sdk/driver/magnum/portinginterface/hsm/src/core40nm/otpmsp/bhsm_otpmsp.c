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
 * $brcm_Workfile:  $
 * $brcm_Revision:  $
 * $brcm_Date:  $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log:  $
 *
 ******************************************************************************/


#include "bchp_bsp_control_intr2.h"
#include "bchp_bsp_glb_control.h"



#include "bhsm_otpmsp.h"





BDBG_MODULE(BHSM);


BERR_Code BHSM_ProgramOTPPatternSequence (
		BHSM_Handle					in_handle,
		BHSM_ProgramOtpPatSeqIO_t	*inoutp_progOtpPatSeqIO
)
{
	BERR_Code 				errCode = BERR_SUCCESS;

	BHSM_ChannelHandle 		channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t 				unParamLen = 0;
	BHSM_P_CommandData_t	commandData;
	unsigned int			i;

	BDBG_MSG(("Inside BHSM_ProgramOTPPatternSequence\n"));
	BDBG_ENTER(BHSM_ProgramOTPPatternSequence);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );
	
	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	commandData.cmdId = 0x51;
	commandData.unContMode = 0;
	
	/* only fill the command specific input parameters */
	i = BCMD_Otp_InCmdOtpProgPatternSequence_ePatternArray/4 - BCMD_CommonBufferFields_eStatus/4;
	commandData.unInputParamsBuf[i++] = 0xBC32F4AC;   /* OTP_PROG_PATTERN_0 */
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
	commandData.unInputParamsBuf[i++] = 0x7698E712;   /* OTP_PROG_PATTERN_7 */
	commandData.unInputParamLen += 4;
	commandData.unInputParamsBuf[i++] = 0x0000F48D;   /* OTP Cell unlock pattern */
	commandData.unInputParamLen += 4;

	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

	/* Parse the command specific output parameters */
	inoutp_progOtpPatSeqIO->unStatus =  commandData.unOutputParamsBuf[0];

	unParamLen += 4;	

 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_progOtpPatSeqIO->unStatus != 0) );			

BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_ProgramOTPPatternSequence);
	return( errCode );



}



BERR_Code BHSM_ReadOTP (
		BHSM_Handle				in_handle,
		BHSM_ReadOtpIO_t		*inoutp_readOtpIO
)
{
	BERR_Code 					errCode = BERR_SUCCESS;
	BHSM_ChannelHandle 			channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t 					unParamLen = 0, i, j;
	BHSM_P_CommandData_t 		commandData;	

	BDBG_MSG(("Inside BHSM_ReadOTP\n"));
	BDBG_ENTER(BHSM_ReadOTP);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	


	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	commandData.cmdId = BCMD_cmdType_eOFFLINE_OTP_READ;
	commandData.unContMode = 0;
	
	/* only fill the command specific input parameters */
	commandData.unInputParamsBuf[BCMD_Otp_InCmdOfflineOtpRead_eEnum/4 - BCMD_CommonBufferFields_eStatus/4] = 
			(inoutp_readOtpIO->readOtpEnum |
			 (inoutp_readOtpIO->keyType << 8 ));
	commandData.unInputParamLen += 4;

	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

	/* Parse the command specific output parameters */
	inoutp_readOtpIO->unStatus =  commandData.unOutputParamsBuf[0];
	unParamLen += 4;	

 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_readOtpIO->unStatus != 0) );		

	for (i=0, j = 1; i<BHSM_OTP_DATA_LEN; j++, i += 4) 
	{
		inoutp_readOtpIO->regValueLo[i]   = (commandData.unOutputParamsBuf[j ] >> 24) & 0xFF;
		inoutp_readOtpIO->regValueLo[i+1] = (commandData.unOutputParamsBuf[j ] >> 16) & 0xFF; 
		inoutp_readOtpIO->regValueLo[i+2] = (commandData.unOutputParamsBuf[j ] >> 8) & 0xFF;
		inoutp_readOtpIO->regValueLo[i+3] = commandData.unOutputParamsBuf[j ] & 0xFF; 
		unParamLen += 4;		
	}
	/* one extra dword for keyID and key hash */
	if ((inoutp_readOtpIO->readOtpEnum == BPI_Otp_CmdReadRegister_eKeyID) ||
	    (inoutp_readOtpIO->readOtpEnum == BPI_Otp_CmdReadRegister_eKeyHash ))
	{
		for (i=0, j = 2; i<BHSM_OTP_DATA_LEN; j++, i += 4) 
		{
			inoutp_readOtpIO->regValueHi[i]   = (commandData.unOutputParamsBuf[j ] >> 24) & 0xFF;
			inoutp_readOtpIO->regValueHi[i+1] = (commandData.unOutputParamsBuf[j ] >> 16) & 0xFF; 
			inoutp_readOtpIO->regValueHi[i+2] = (commandData.unOutputParamsBuf[j ] >> 8) & 0xFF;
			inoutp_readOtpIO->regValueHi[i+3] = commandData.unOutputParamsBuf[j ] & 0xFF; 
			unParamLen += 4;		
		}
	
	}

	
BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_ReadOTP);
	return( errCode );
}


BERR_Code BHSM_ProgramMSP (
		BHSM_Handle				in_handle, 
		BHSM_ProgramMspIO_t		*inoutp_progMspIO
)
{

	BERR_Code 				errCode = BERR_SUCCESS;

	BHSM_ChannelHandle 		channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t 				unParamLen = 0;
	BHSM_P_CommandData_t	commandData;
	unsigned int			i;
	BHSM_ProgramOtpPatSeqIO_t	inout_progOtpPatSeqIO;


	BDBG_MSG(("Inside BHSM_ProgramMSP\n"));
	BDBG_ENTER(BHSM_ProgramMSP);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BDBG_MSG(("progMode =%x", inoutp_progMspIO->progMode));
	for (i=0;i < BHSM_MSP_DATA_LEN;i++)  
	{		
		BDBG_MSG(("%.2x ", inoutp_progMspIO->aucMspData[i]));
	}
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
		(inoutp_progMspIO->progMspEnum >=  BCMD_Otp_CmdMsp_eSize) ||
 		(inoutp_progMspIO->progMode != 0x00010112));

	/* BUG in ROM code...Need to send out OTP programming pattern at most twice */
	errCode = BHSM_ProgramOTPPatternSequence(in_handle, &inout_progOtpPatSeqIO);
	if (errCode != BERR_SUCCESS)
	{	
		BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		       (BHSM_ProgramOTPPatternSequence(in_handle, &inout_progOtpPatSeqIO) != 
				  														BERR_SUCCESS ) );	
	}
	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));
 
	commandData.cmdId = BCMD_cmdType_eOFFLINE_PROG_MSP;
	commandData.unContMode = 0;	
	
	commandData.unInputParamsBuf[0] =  inoutp_progMspIO->progMode;
	commandData.unInputParamLen += 4;   

	commandData.unInputParamsBuf[commandData.unInputParamLen/4] =  inoutp_progMspIO->progMspEnum & 0xFF;
	commandData.unInputParamLen += 4;   


	commandData.unInputParamsBuf[commandData.unInputParamLen/4] =  inoutp_progMspIO->ucDataBitLen & 0xFF;
	commandData.unInputParamLen += 4;   
	
		commandData.unInputParamsBuf[commandData.unInputParamLen/4] =  
		(inoutp_progMspIO->aucDataBitMask[0]<<24 ) |
		(inoutp_progMspIO->aucDataBitMask[1]<<16 ) |
		(inoutp_progMspIO->aucDataBitMask[2]<<8 ) |
		inoutp_progMspIO->aucDataBitMask[3];
	
		commandData.unInputParamLen += 4;  
		
	commandData.unInputParamsBuf[commandData.unInputParamLen/4] =
		(inoutp_progMspIO->aucMspData[0]<<24 ) |
		(inoutp_progMspIO->aucMspData[1]<<16 ) |
		(inoutp_progMspIO->aucMspData[2]<<8 ) |
		inoutp_progMspIO->aucMspData[3];
	
	commandData.unInputParamLen += 4;  

	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

	inoutp_progMspIO->unStatus =  commandData.unOutputParamsBuf[0];

	unParamLen += 4;	

 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_progMspIO->unStatus != 0) );			

BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_ProgramMSP);
	return( errCode );

}

BERR_Code BHSM_ReadMSP (
		BHSM_Handle			in_handle,
		BHSM_ReadMspIO_t	*inoutp_readMspIO
)
{

	BERR_Code 					errCode = BERR_SUCCESS;
	BHSM_ChannelHandle 			channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t 					unParamLen = 0;
	BHSM_P_CommandData_t 		commandData;	


	BDBG_MSG(("Inside BHSM_ReadMSP\n"));
	BDBG_ENTER(BHSM_ReadMSP);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	


	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	/* command header*/
	commandData.cmdId = BCMD_cmdType_eOFFLINE_MSP_READ;
	commandData.unContMode = 0;	
	
	/*command specific data, must be continuous + in sequence,
		in commandData.unInputParamsBuf[j], per j= current - base
		in commandData.unInputParamLen,     per increment = next -current
		
	*/
	commandData.unInputParamsBuf[BCMD_Otp_InCmdOfflineMspRead_eEnum/4 - BCMD_CommonBufferFields_eStatus/4] 
			= inoutp_readMspIO->readMspEnum;
	commandData.unInputParamLen += 4;


	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

	/* Parse the command specific output parameters */
	inoutp_readMspIO->unStatus =  commandData.unOutputParamsBuf[0];
	unParamLen += 4;	

 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_readMspIO->unStatus != 0) );		

	inoutp_readMspIO->aucMspData[0] = (commandData.unOutputParamsBuf[1] >> 24) & 0xFF;
	inoutp_readMspIO->aucMspData[1] = (commandData.unOutputParamsBuf[1] >> 16) & 0xFF; 
	inoutp_readMspIO->aucMspData[2] = (commandData.unOutputParamsBuf[1] >> 8) & 0xFF;
	inoutp_readMspIO->aucMspData[3] =  commandData.unOutputParamsBuf[1] & 0xFF; 

	unParamLen += 4;		

	inoutp_readMspIO->aucLockMspData[0] = (commandData.unOutputParamsBuf[2] >> 24) & 0xFF;
	inoutp_readMspIO->aucLockMspData[1] = (commandData.unOutputParamsBuf[2] >> 16) & 0xFF; 
	inoutp_readMspIO->aucLockMspData[2] = (commandData.unOutputParamsBuf[2] >> 8) & 0xFF;
	inoutp_readMspIO->aucLockMspData[3] =  commandData.unOutputParamsBuf[2] & 0xFF; 

	unParamLen += 4;		



BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_ReadMSP);
	return( errCode );
}



BERR_Code BHSM_ReadDataSect (
		BHSM_Handle				in_handle,
		BHSM_ReadDataSectIO_t	*inoutp_readDataSectIO
)
{
	BERR_Code 					errCode = BERR_SUCCESS;
	BHSM_ChannelHandle 			channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t 					unParamLen = 0, i,j;
	BHSM_P_CommandData_t		commandData;	


	BDBG_MSG(("Inside BHSM_ReadDataSect\n"));
	BDBG_ENTER(BHSM_ReadDataSect);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
 		 inoutp_readDataSectIO->readDsEnum >=  BPI_Otp_DataSection_e_size);	

	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	/* command header*/
	commandData.cmdId = BCMD_cmdType_eOTP_DATA_SECTION_READ;
	commandData.unContMode = 0;	
	commandData.unInputParamsBuf[0] =  inoutp_readDataSectIO->readDsEnum;
	commandData.unInputParamLen += 4;   /* no furhter fields according to current cmd defition*/

	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

	/* Parse the command specific output parameters */
	inoutp_readDataSectIO->unStatus =  commandData.unOutputParamsBuf[0];
	unParamLen += 4;	

	 BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		 (inoutp_readDataSectIO->unStatus != 0) );			 

	/* Convert data from [15:0][31:16][47:32][63:48][79:64][95:80][111:96][127:112][143:128][159:144][175:160][191:176][207:192][223:208][239:224][255:240] */
	/*                     to  [255:240][239:224][223:208][207:192][191:176][175:160][159:144][143:128]...[63:48][47:32][31:16][15:0] */
	for (i = 0, j = BHSM_READ_DATA_SECTION_DATA_LEN/4 ;   
		 i < BHSM_READ_DATA_SECTION_DATA_LEN; 
		 i += 4, j--) 
	{
		inoutp_readDataSectIO->aucDataSectData[i]   = (commandData.unOutputParamsBuf[j ] >> 8)  & 0xFF;
		inoutp_readDataSectIO->aucDataSectData[i+1] = (commandData.unOutputParamsBuf[j ])       & 0xFF; 
		inoutp_readDataSectIO->aucDataSectData[i+2] = (commandData.unOutputParamsBuf[j ] >> 24) & 0xFF;
		inoutp_readDataSectIO->aucDataSectData[i+3] = (commandData.unOutputParamsBuf[j ] >> 16) & 0xFF; 
 
		unParamLen += 4;		
	}


BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_ReadDataSect);
	return( errCode );

}



BERR_Code BHSM_ProgramDataSect (
		BHSM_Handle					in_handle,
		BHSM_ProgramDataSectIO_t 	*inoutp_progDataSectIO
)
{
	BERR_Code 					errCode = BERR_SUCCESS;
	BHSM_ChannelHandle 			channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t 					unParamLen = 0;
	unsigned char				j;
	BHSM_P_CommandData_t 		commandData;	
	unsigned char				i;
	BHSM_ProgramOtpPatSeqIO_t	inout_progOtpPatSeqIO;


	BDBG_MSG(("Inside BHSM_ProgramDataSect\n"));
	BDBG_ENTER(BHSM_ProgramDataSect);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BDBG_MSG(("unMode =%x", inoutp_progDataSectIO->mode));
	for (i=0;i < 32;i++)  
	{		
		BDBG_MSG(("%.2x ", inoutp_progDataSectIO->aucDataSectData[i]));
	}

	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
		( inoutp_progDataSectIO->progDsEnum >=  BPI_Otp_DataSection_e_size ||
 		  inoutp_progDataSectIO->mode!= BCMD_OTP_DATASECTIONPROG_MODE));	
	
	/* BUG in ROM code...Need to send out OTP programming pattern at most twice */
	errCode = BHSM_ProgramOTPPatternSequence(in_handle, &inout_progOtpPatSeqIO);
	if (errCode != BERR_SUCCESS)
	{	
		BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		       (BHSM_ProgramOTPPatternSequence(in_handle, &inout_progOtpPatSeqIO) != 
				  														BERR_SUCCESS ) );	
	}

	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));
 
	commandData.cmdId = BCMD_cmdType_eOTP_DATA_SECTION_PROG;
	commandData.unContMode = 0;	
	
	commandData.unInputParamsBuf[0] =  inoutp_progDataSectIO->progDsEnum;
	commandData.unInputParamLen += 4;   

	/* app-to-hsm: 32-byte-array in big endian 
	     hsm-to-BSP:  a particular wrapping to help BSP  word0/LSB, word7/MSB, word=[15:0] ||[31:16] */
	/*
	Suppose Data Section 3 needs to be programmed with 256 bits (MSB to LSB): DS[255:0] = 0x01234567 89abcdef fedcba98 76543210 02468ace 13579bdf fdb97531 eca86420
 
	The programming command that needs to be given for this is as follows:
 
	version#
	owner id
	0x0 (continual mode)
	0x1F (tag id)
	0x2c (param length)
	0x3   (data section #)
	6420eca8   // data section data   = DS[15:0]       || DS[31:16]
	7531fdb9                                   = DS[47:32]     || DS[63:48]
	9bdf1357                                   = DS[79:64]     || DS[95:80]
	8ace0246                                  = DS[111:96]   || DS[127:112]
	32107654                                  = DS[143:128] || DS[159:144]
	ba98fedc                                   = DS[175:160] || DS[191:176]
	cdef89ab                                   = DS[207:192] || DS[223:208]
	45670123                                  = DS[239:224] || DS[255:240]
	00000012  // eMode
	6778eef5   // CRC of data section data
	*/

	j=commandData.unInputParamLen/4 + 7;
	 
	for (i=0 ; 	i < BHSM_READ_DATA_SECTION_DATA_LEN;  i += 4, j--) 
	{
		commandData.unInputParamsBuf[j] =  ((inoutp_progDataSectIO->aucDataSectData[i]   <<  8) |   
	                                        (inoutp_progDataSectIO->aucDataSectData[i+1]      ) |
		                                    (inoutp_progDataSectIO->aucDataSectData[i+2] << 24) |
		                                    (inoutp_progDataSectIO->aucDataSectData[i+3] << 16) );
 		BDBG_MSG(("j=%d, %.8x ",j, commandData.unInputParamsBuf[j] ));
		commandData.unInputParamLen += 4;		
	}

	commandData.unInputParamsBuf[commandData.unInputParamLen/4] = inoutp_progDataSectIO ->mode;
	commandData.unInputParamLen += 4;			

	/* TODO: app-to-hsm, no CRC is necessary. hsm should calculate the CRC 
		 		yet for security reason, HSM doesn't want to expose BSP information, hide CRC so far.
		 		to use checksum or a different CRC ?
	*/
	commandData.unInputParamsBuf[commandData.unInputParamLen/4] =
	           ((inoutp_progDataSectIO->aucCrc[0] << 24) |   
	            (inoutp_progDataSectIO->aucCrc[1] << 16) |
	            (inoutp_progDataSectIO->aucCrc[2] <<  8) |
	            (inoutp_progDataSectIO->aucCrc[3]        ));
 	
	BDBG_MSG(("j=%d, %.8x ",j, commandData.unInputParamsBuf[commandData.unInputParamLen/4] ));
	
	commandData.unInputParamLen += 4;	

	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

	unParamLen = 0;

	/* Parse the command specific output parameters */
	inoutp_progDataSectIO->unStatus =  commandData.unOutputParamsBuf[0];
	unParamLen += 4;	

 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_progDataSectIO->unStatus != 0) );			


BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_ProgramDataSect);
	return( errCode );
}


