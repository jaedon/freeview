/***************************************************************************
 *     Copyright (c) 2005-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhsm_otpmsp.c $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 10/27/09 10:21a $
 *
 * Module Description: This file contains Broadcom Host Secure Module (OS/platform 
 *                     independent) porting interface public functions.
 *                    			                    
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7601/a0/otpmsp/bhsm_otpmsp.c $
 * 
 * Hydra_Software_Devel/7   10/27/09 10:21a atruong
 * SW7630-48: New command ID for OTP Pattern Programming for 7630B0
 * 
 * Hydra_Software_Devel/5   4/23/09 10:28a atruong
 * PR54377: Remove length checking for output parameters
 * 
 * Hydra_Software_Devel/4   4/1/09 11:10a atruong
 * PR52979: Support for BCM7635A0
 * 
 * Hydra_Software_Devel/3   12/5/08 8:43p atruong
 * PR49290: HSM PI Support for 7601B0
 * 
 * Hydra_Software_Devel/2   8/27/08 8:49a atruong
 * PR45631: BCM97601A0 Bring Up
 * 
 * Hydra_Software_Devel/1   8/12/08 6:09p atruong
 * PR45631: HSM PI support for BCM7601 platform
 * 
 * Hydra_Software_Devel/2   12/19/07 2:15p qxie
 * PR 38365: HSM: add data section programming interface with CRC
 * 
 * Hydra_Software_Devel/12   12/12/06 5:25p lyang
 * PR 26462: HSM: Latest code does not compile for the 7400A0
 * 
 * Hydra_Software_Devel/11   12/11/06 11:38p lyang
 * PR 26458: HSM, OTP DataSection implementation needed
 * 
 * Hydra_Software_Devel/10   10/26/06 4:16p lyang
 * PR 23412: hsm pi include files should be re-factored so they are not
 * order dependent. PR 25133:HSM header files should include dependencies
 * 
 * Hydra_Software_Devel/9   10/11/06 1:54p lyang
 * PR 24862: HSM: Add HSM for 7440A0
 * 
 * Hydra_Software_Devel/8   9/26/06 3:11p btan
 * PR 23906: Converted BCHP_REV to BCHP_VER.
 * 
 * Hydra_Software_Devel/7   9/20/06 4:11p lyang
 * PR 24145: remove the reserved debugging macro
 * 
 * Hydra_Software_Devel/6   9/8/06 6:02p dbayani
 * PR 24170: Remove warnings.
 * 
 * Hydra_Software_Devel/4   9/7/06 4:54p lyang
 * PR 24145: change ReadOTP result presentation, add ReadMSP, simplify
 * some PI
 * 
 * Hydra_Software_Devel/4   9/7/06 10:57a lyang
 * PR 24145: change ReadOTP result presentation, add ReadMSP, simplify
 * some PI
 * 
 * Hydra_Software_Devel/4   9/7/06 10:42a lyang
 * PR 24145: change ReadOTP result presentation, add ReadMSP, simplify
 * some PI
 * 
 * Hydra_Software_Devel/3   8/4/06 12:31a btan
 * PR22187: Sync with 7401C0 BSP header files.
 * 
 * Hydra_Software_Devel/2   5/8/06 9:04p btan
 * PR 21183: HSM needs to compile for 7401 B0
 * 
 * Hydra_Software_Devel/1   4/26/06 5:47p btan
 * PR 20446: HSM needs to compile for 7400 A0
 *
 *
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"

/*#include "bxpt.h"*/

#include "bchp_bsp_control_intr2.h"
#include "bchp_bsp_glb_control.h"



#include "bhsm_otpmsp.h"


BDBG_MODULE(BHSM);

/* #define BDBG_MSG(x) printf x */

#if defined(BSP_OTP_PROG_PATTERN_SEQUENCE)

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

	commandData.cmdId = 0x4C;
	commandData.unContMode = 0;
	
	/* only fill the command specific input parameters */
	i = BCMD_Otp_InCmdOtpProgPatternSequence_ePatternArray/4 - BCMD_CommonBufferFields_eStatus/4;
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

#endif



#ifdef BSP_BROADCOM_ONLY
BERR_Code BHSM_ProgramOTP (
		BHSM_Handle			in_handle,
		BHSM_ProgramOtpIO_t	*inoutp_progOtpIO
)
{
	BERR_Code errCode = BERR_SUCCESS;

	BDBG_MSG(("Inside BHSM_ProgramOTP\n"));
	BDBG_ENTER(BHSM_ProgramOTP);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

BSTD_UNUSED(inoutp_progOtpIO);  /* Removed this when we start using this variable */		
BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_ProgramOTP);
	return( errCode );
}
#endif

BERR_Code BHSM_ReadOTP (
		BHSM_Handle			in_handle,
		BHSM_ReadOtpIO_t		*inoutp_readOtpIO
)
{
	BERR_Code errCode = BERR_SUCCESS;
	BHSM_ChannelHandle channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t unParamLen = 0, i, j;
	BHSM_P_CommandData_t commandData;	
#if defined(BSP_OTP_PROG_PATTERN_SEQUENCE)
		BHSM_ProgramOtpPatSeqIO_t	inout_progOtpPatSeqIO;
#endif

	BDBG_MSG(("Inside BHSM_ReadOTP\n"));
	BDBG_ENTER(BHSM_ReadOTP);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

#if defined(BSP_OTP_PROG_PATTERN_SEQUENCE)
		BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
			(BHSM_ProgramOTPPatternSequence(in_handle, &inout_progOtpPatSeqIO) != 
														BERR_SUCCESS ) );	
#endif

	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	commandData.cmdId = BCMD_cmdType_eOFFLINE_OTP_READ;
	commandData.unContMode = 0;
	
	/* only fill the command specific input parameters */
	commandData.unInputParamsBuf[BCMD_Otp_InCmdOfflineOtpRead_eEnum/4 - BCMD_CommonBufferFields_eStatus/4] = 
			inoutp_readOtpIO->readOtpEnum;
	commandData.unInputParamLen += 4;

	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

	/* Parse the command specific output parameters */
	inoutp_readOtpIO->unStatus =  commandData.unOutputParamsBuf[0];
	unParamLen += 4;	

	/*1) all items have Word0(LSB), Word1(MSB), are swapped. 
	   2) If not all, pls swap the items case by case
		if ( inoutp_readOtpIO->readOtpEnum == BPI_Otp_CmdReadRegister_eIdAWord ||
			inoutp_readOtpIO->readOtpEnum == BPI_Otp_CmdReadRegister_eIdBWord ||
			inoutp_readOtpIO->readOtpEnum == BPI_Otp_CmdReadRegister_eIdCWord		){...}
	       
		
  	    3) for (i=0, j = 1; i<BHSM_OTP_KEY_ID_LEN; j++, i+=4) {...} //no swap
 		 for (i=0, j = 2; i<BHSM_OTP_KEY_ID_LEN; j--, i+=4) {...} // swap
	*/

		for (i=0, j = 2; i<BHSM_OTP_KEY_ID_LEN; j--, i+=4) {

			inoutp_readOtpIO->aucKeyId[i]     = (commandData.unOutputParamsBuf[j ] >> 24) & 0xFF;
			inoutp_readOtpIO->aucKeyId[i+1] = (commandData.unOutputParamsBuf[j ] >> 16) & 0xFF; 
			inoutp_readOtpIO->aucKeyId[i+2] = (commandData.unOutputParamsBuf[j ] >> 8) & 0xFF;
			inoutp_readOtpIO->aucKeyId[i+3] = commandData.unOutputParamsBuf[j ] & 0xFF; 

			unParamLen += 4;		
		}

 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_readOtpIO->unStatus != 0) );		
	
BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_ReadOTP);
	return( errCode );
}

BERR_Code BHSM_ProgramMSP (
		BHSM_Handle				in_handle, 
		
		BHSM_ProgramMspIO_t		*inoutp_progMspIO
)
{

	BERR_Code errCode = BERR_SUCCESS;

	BHSM_ChannelHandle 	channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t 				unParamLen = 0;
	BHSM_P_CommandData_t commandData;	
	unsigned char			i;
#if defined(BSP_OTP_PROG_PATTERN_SEQUENCE)
	BHSM_ProgramOtpPatSeqIO_t	inout_progOtpPatSeqIO;
#endif

	BDBG_MSG(("Inside BHSM_ProgramMSP\n"));
	BDBG_ENTER(BHSM_ProgramMSP);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BDBG_MSG(("progMode =%x", inoutp_progMspIO->progMode));
	for (i=0;i < BHSM_MSP_DATA_LEN;i++)  {		
		BDBG_MSG(("%.2x ", inoutp_progMspIO->aucMspData[i]));
	}
	
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
		( inoutp_progMspIO->progMspEnum >=  BCMD_Otp_CmdMsp_eSize ||
 		  inoutp_progMspIO->progMode != BCMD_OTP_PROGMSP_MODE));

#if defined(BSP_OTP_PROG_PATTERN_SEQUENCE)
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(BHSM_ProgramOTPPatternSequence(in_handle, &inout_progOtpPatSeqIO) != 
																BERR_SUCCESS ) );	
#endif	

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

	BERR_Code 			errCode = BERR_SUCCESS;
	BHSM_ChannelHandle 	channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t 				unParamLen = 0, i, j;
	BHSM_P_CommandData_t commandData;	
#if defined(BSP_OTP_PROG_PATTERN_SEQUENCE)
	BHSM_ProgramOtpPatSeqIO_t	inout_progOtpPatSeqIO;
#endif


	BDBG_MSG(("Inside BHSM_ReadMSP\n"));
	BDBG_ENTER(BHSM_ReadMSP);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

#if defined(BSP_OTP_PROG_PATTERN_SEQUENCE)
		BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
			(BHSM_ProgramOTPPatternSequence(in_handle, &inout_progOtpPatSeqIO) != 
																	BERR_SUCCESS ) );	
#endif

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

	if (inoutp_readMspIO->bCheckSignature){
			commandData.unInputParamsBuf[BCMD_Otp_InCmdOfflineMspRead_eReserved0/4 - BCMD_CommonBufferFields_eStatus/4] 
						= inoutp_readMspIO->ucCustKeyHigh & 0x3F;  /*current - base*/
														
			commandData.unInputParamsBuf[BCMD_Otp_InCmdOfflineMspRead_eReserved1/4 - BCMD_CommonBufferFields_eStatus/4] 
						= inoutp_readMspIO->ucCustKeyLow & 0x3F;

			commandData.unInputParamsBuf[BCMD_Otp_InCmdOfflineMspRead_eReserved2/4 - BCMD_CommonBufferFields_eStatus/4] 
						= inoutp_readMspIO->ucKeyVarHigh;

			commandData.unInputParamsBuf[BCMD_Otp_InCmdOfflineMspRead_eReserved3/4 - BCMD_CommonBufferFields_eStatus/4] 
						= inoutp_readMspIO->ucKeyVarLow;

			
			/*************************************************************************************
			1) entire input byte stream is at big endian usage order --->  entire procin field is at big endian byte steam order
			    this is not the current design
			    
			BKNI_Memcpy(  	(char*)&(commandData.unInputParamsBuf[BCMD_Otp_InCmdOfflineMspRead_eReserved4/4 - BCMD_CommonBufferFields_eStatus/4] ),
						    	(char*)&(inoutp_readMspIO->aucProcInForKey3[0]),
						  	BHSM_MSP_KEY3_DATA_LEN		);
						  	
			2) entire input byte stream is at big endian order --->  entire procin field is sliced into uint32_word array, 
			   inside each uint32_word, assignment should be at big endian, yet this word's memory storage is still little endian

			   current design is 2).
			**************************************************************************************/			
			for (i=0, j=BCMD_Otp_InCmdOfflineMspRead_eReserved4/4 - BCMD_CommonBufferFields_eStatus/4;
			      i< BHSM_MSP_KEY3_DATA_LEN; i +=4 ){
				  	commandData.unInputParamsBuf[j++] = inoutp_readMspIO->aucProcInForKey3[i]<<24 |
				  	 								inoutp_readMspIO->aucProcInForKey3[i+1] << 16 |
				  									inoutp_readMspIO->aucProcInForKey3[i+2] << 8 |
				  	 								inoutp_readMspIO->aucProcInForKey3[i+3];
			}			
			
			/*			 
			BKNI_Memcpy(	(char*)&(commandData.unInputParamsBuf[BCMD_Otp_InCmdOfflineMspRead_eReserved5/4 - BCMD_CommonBufferFields_eStatus/4] ),
						  	(char*)&(inoutp_readMspIO->aucProcInForKey4[0]),
						  	BHSM_MSP_KEY4_DATA_LEN		);			
			*/			
			for (i=0, j= BCMD_Otp_InCmdOfflineMspRead_eReserved5/4 - BCMD_CommonBufferFields_eStatus/4;
			      i< BHSM_MSP_KEY4_DATA_LEN; i +=4 ){
				  	commandData.unInputParamsBuf[j++] = inoutp_readMspIO->aucProcInForKey4[i]<<24 |
				  	 								inoutp_readMspIO->aucProcInForKey4[i+1] << 16 |
				  	 								inoutp_readMspIO->aucProcInForKey4[i+2] << 8 |
				  	 								inoutp_readMspIO->aucProcInForKey4[i+3];
			}			
			
			/* 
			BKNI_Memcpy(	(char*)&(commandData.unInputParamsBuf[BCMD_Otp_InCmdOfflineMspRead_eReserved6/4 - BCMD_CommonBufferFields_eStatus/4] ),
						  	(char*)&(inoutp_readMspIO->aucSignature[0]),
						  	BHSM_MSP_SIGNATURE_DATA_LEN		);			
			*/			
			for (i=0, j= BCMD_Otp_InCmdOfflineMspRead_eReserved6/4 - BCMD_CommonBufferFields_eStatus/4;
			      i< BHSM_MSP_SIGNATURE_DATA_LEN; 
			      i +=4 ){
				  	commandData.unInputParamsBuf[j++] = inoutp_readMspIO->aucSignature[i]<<24  |
				  									inoutp_readMspIO->aucSignature[i+1] << 16 |
				  									inoutp_readMspIO->aucSignature[i+2] << 8 |
				  									inoutp_readMspIO->aucSignature[i+3] ;
			}			
			/*commandData.unInputParamLen += (BCMD_Otp_InCmdOfflineMspRead_eReserved1/4)*4 - (BCMD_Otp_InCmdOfflineMspRead_eReserved0/4)*4;
			commandData.unInputParamLen += (BCMD_Otp_InCmdOfflineMspRead_eReserved2/4)*4 - (BCMD_Otp_InCmdOfflineMspRead_eReserved1/4)*4;
			commandData.unInputParamLen += (BCMD_Otp_InCmdOfflineMspRead_eReserved3/4)*4 - (BCMD_Otp_InCmdOfflineMspRead_eReserved2/4)*4;		
			commandData.unInputParamLen += (BCMD_Otp_InCmdOfflineMspRead_eReserved4/4)*4 - (BCMD_Otp_InCmdOfflineMspRead_eReserved3/4)*4;
			commandData.unInputParamLen += (BCMD_Otp_InCmdOfflineMspRead_eReserved5/4)*4 - (BCMD_Otp_InCmdOfflineMspRead_eReserved4/4)*4;
			commandData.unInputParamLen += (BCMD_Otp_InCmdOfflineMspRead_eReserved6/4)*4 - (BCMD_Otp_InCmdOfflineMspRead_eReserved5/4)*4;
			commandData.unInputParamLen += (BCMD_Otp_InCmdOfflineMspRead_eReserved7/4)*4 - (BCMD_Otp_InCmdOfflineMspRead_eReserved6/4)*4;
            */
			
			/* one length adjustment after the last one, for all the previous fields*/
			commandData.unInputParamLen += (BCMD_Otp_InCmdOfflineMspRead_eReserved7/4)*4 - (BCMD_CommonBufferFields_eStatus/4)*4;
		
	}
	
	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

	/* Parse the command specific output parameters */
	inoutp_readMspIO->unStatus =  commandData.unOutputParamsBuf[0];
	unParamLen += 4;	

	for (i=0, j = 1; i<BHSM_MSP_OUTPUT_DATA_LEN; j++, i+=4) {

		inoutp_readMspIO->aucMspData[i]     = (commandData.unOutputParamsBuf[j ] >> 24) & 0xFF;
		inoutp_readMspIO->aucMspData[i+1] = (commandData.unOutputParamsBuf[j ] >> 16) & 0xFF; 
		inoutp_readMspIO->aucMspData[i+2] = (commandData.unOutputParamsBuf[j ] >> 8) & 0xFF;
		inoutp_readMspIO->aucMspData[i+3] = commandData.unOutputParamsBuf[j ] & 0xFF; 

		unParamLen += 4;		
	}

 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_readMspIO->unStatus != 0) );		


       /* BSTD_UNUSED(inoutp_readMspIO);  Removed this when we start using this variable, removed by lyang 2006.08.22*/	
BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_ReadMSP);
	return( errCode );
}


BERR_Code BHSM_GetStatus (
		BHSM_Handle			in_handle,
		BHSM_GetStatusIO_t	*inoutp_getStatusIO
)
{

	BERR_Code errCode = BERR_SUCCESS;
	BHSM_ChannelHandle channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t unParamLen = 0;
	BHSM_P_CommandData_t commandData;
#if defined(BSP_OTP_PROG_PATTERN_SEQUENCE)
	BHSM_ProgramOtpPatSeqIO_t	inout_progOtpPatSeqIO;
#endif
	
	BDBG_MSG(("Inside BHSM_GetStatus\n"));	
	BDBG_ENTER(BHSM_GetStatus);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );

#if defined(BSP_OTP_PROG_PATTERN_SEQUENCE)
		BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
			(BHSM_ProgramOTPPatternSequence(in_handle, &inout_progOtpPatSeqIO) != 
																	BERR_SUCCESS ) );	
#endif

	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	commandData.cmdId = BCMD_cmdType_eOFFLINE_STATUS;
	commandData.unContMode = 0;
	
	/* only fill the command specific input parameters */
 	/* None in this command */
	
	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));
	
	/* Parse the command specific output parameters */
	inoutp_getStatusIO->unStatus =  commandData.unOutputParamsBuf[0];
	unParamLen += ( (BCMD_Otp_OutCmdStatus_eStatusSAL/4) * 4 -
					(BCMD_CommonBufferFields_eStatus /4) * 4);

	inoutp_getStatusIO->unSALStatus =  commandData.unOutputParamsBuf[1];
	unParamLen +=  ( (BCMD_Otp_OutCmdStatus_eSignalRead/4) * 4 -
					(BCMD_Otp_OutCmdStatus_eStatusSAL /4) * 4);	

	inoutp_getStatusIO->unSignalReadStatus =  commandData.unOutputParamsBuf[2];
	unParamLen +=  ( (BCMD_Otp_OutCmdStatus_eFltStat/4) * 4 -
					(BCMD_Otp_OutCmdStatus_eSignalRead /4) * 4);	

	inoutp_getStatusIO->unOtpFltStatus =  commandData.unOutputParamsBuf[3];
	unParamLen += ( (BCMD_Otp_OutCmdStatus_eFltStatKeyA/4) * 4 -
					(BCMD_Otp_OutCmdStatus_eFltStat /4) * 4);	

	inoutp_getStatusIO->unOtpAFltStatus =  commandData.unOutputParamsBuf[4];
	unParamLen += ( (BCMD_Otp_OutCmdStatus_eFltStatKeyB/4) * 4 -
					(BCMD_Otp_OutCmdStatus_eFltStatKeyA /4) * 4);		

	inoutp_getStatusIO->unOtpBFltStatus =  commandData.unOutputParamsBuf[5];
	unParamLen += ( (BCMD_Otp_OutCmdStatus_eFltStatKeyC/4) * 4 -
					(BCMD_Otp_OutCmdStatus_eFltStatKeyB /4) * 4);		

	inoutp_getStatusIO->unOtpCFltStatus =  commandData.unOutputParamsBuf[6];
#if defined(BCM7601A0) || defined(BCM7601B0) || defined(BCM7635A0) || defined(BCM7630B0)
	unParamLen += ( (BCMD_Otp_OutCmdStatus_eFltStatDs/4) * 4 -
					(BCMD_Otp_OutCmdStatus_eFltStatKeyC /4) * 4);
	
#else
	unParamLen += ( (BCMD_Otp_OutCmdStatus_eReserved0/4) * 4 -
					(BCMD_Otp_OutCmdStatus_eFltStatKeyC/4) * 4);
#endif

#if (BCHP_CHIP == 7401) &&  (BCHP_VER == BCHP_VER_A0)

	inoutp_getStatusIO->unOtpDsFltStatus =  commandData.unOutputParamsBuf[7];
	unParamLen += 4;	/*?? How to avoid mismatch */

#else

	inoutp_getStatusIO->unOtpDsFltStatus =  commandData.unOutputParamsBuf[7];
#if defined(BCM7601A0) || defined(BCM7601B0) || defined(BCM7635A0) || defined(BCM7630B0)
	unParamLen += ( (BCMD_Otp_OutCmdStatus_eReserved1/4) * 4 -
					(BCMD_Otp_OutCmdStatus_eFltStatDs /4) * 4);

#else
	unParamLen += ( (BCMD_Otp_OutCmdStatus_eReserved1/4) * 4 -
					(BCMD_Otp_OutCmdStatus_eReserved0 /4) * 4);
#endif

	inoutp_getStatusIO->unReserved1Status =  commandData.unOutputParamsBuf[8];
	unParamLen += ( (BCMD_Otp_OutCmdStatus_eReserved2/4) * 4 -
					(BCMD_Otp_OutCmdStatus_eReserved1 /4) * 4);		

	inoutp_getStatusIO->unReserved2Status =  commandData.unOutputParamsBuf[9];
	unParamLen += ( (BCMD_Otp_OutCmdStatus_eReserved3/4) * 4 -
					(BCMD_Otp_OutCmdStatus_eReserved2 /4) * 4);	

	inoutp_getStatusIO->unReserved3Status =  commandData.unOutputParamsBuf[10];
	unParamLen += ( (BCMD_Otp_OutCmdStatus_eReserved4/4) * 4 -
					(BCMD_Otp_OutCmdStatus_eReserved3 /4) * 4);	

	inoutp_getStatusIO->unReserved4Status =  commandData.unOutputParamsBuf[11];
	unParamLen += ( (BCMD_Otp_OutCmdStatus_eReserved5/4) * 4 -
					(BCMD_Otp_OutCmdStatus_eReserved4 /4) * 4);	

	inoutp_getStatusIO->unReserved5Status =  commandData.unOutputParamsBuf[12];
	unParamLen += ( (BCMD_Otp_OutCmdStatus_eReserved6/4) * 4 -
					(BCMD_Otp_OutCmdStatus_eReserved5 /4) * 4);	

	inoutp_getStatusIO->unReserved6Status =  commandData.unOutputParamsBuf[13];
	unParamLen += ( (BCMD_Otp_OutCmdStatus_eReserved7/4) * 4 -
					(BCMD_Otp_OutCmdStatus_eReserved6 /4) * 4);		

	inoutp_getStatusIO->unReserved7Status =  commandData.unOutputParamsBuf[14];
	unParamLen += 4;	 /*?? How to avoid mismatch */

#endif
	
	/* End of Parse the command specific output parameters */

 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_getStatusIO->unStatus != 0) );		
	
BHSM_P_DONE_LABEL:
 
	BDBG_LEAVE(BHSM_GetStatus);
	BDBG_MSG(("Exit BHSM_GetStatus\n"));		
	return( errCode );
}


BERR_Code BHSM_ReadDataSect (
		BHSM_Handle				in_handle,
		BHSM_ReadDataSectIO_t	*inoutp_readDataSectIO
)
{
	BERR_Code errCode = BERR_SUCCESS;

	BHSM_ChannelHandle 	channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t 				unParamLen = 0, i,j;
	BHSM_P_CommandData_t commandData;	
#if defined(BSP_OTP_PROG_PATTERN_SEQUENCE)
	BHSM_ProgramOtpPatSeqIO_t	inout_progOtpPatSeqIO;
#endif

	BDBG_MSG(("Inside BHSM_ReadDataSect\n"));
	BDBG_ENTER(BHSM_ReadDataSect);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
 		 inoutp_readDataSectIO->readDsEnum >=  BPI_Otp_DataSection_e_size);	

#if defined(BSP_OTP_PROG_PATTERN_SEQUENCE)
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
			(BHSM_ProgramOTPPatternSequence(in_handle, &inout_progOtpPatSeqIO) != 
																	BERR_SUCCESS ) );	
#endif


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

	 /* TODO assume the ret value: word32 internal big endian, word0/LSB at top, word7/MSB at bottom, */
	for (i=0, j=BHSM_READ_DATA_SECTION_DATA_LEN/4 ;   
		i<BHSM_READ_DATA_SECTION_DATA_LEN; 
		i +=4, j--) {
		inoutp_readDataSectIO->aucDataSectData[i]     = (commandData.unOutputParamsBuf[j ] >> 8) & 0xFF;
		inoutp_readDataSectIO->aucDataSectData[i+1] = (commandData.unOutputParamsBuf[j ]) & 0xFF; 
		inoutp_readDataSectIO->aucDataSectData[i+2] = (commandData.unOutputParamsBuf[j ] >> 24) & 0xFF;
		inoutp_readDataSectIO->aucDataSectData[i+3] = (commandData.unOutputParamsBuf[j ] >>16 )& 0xFF; 
 
		unParamLen += 4;		
	}

 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_readDataSectIO->unStatus != 0) );			


BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_ReadDataSect);
	return( errCode );
}

BERR_Code BHSM_ProgramDataSect (
		BHSM_Handle				in_handle,
		BHSM_ProgramDataSectIO_t 	*inoutp_progDataSectIO
)
{
	BERR_Code errCode = BERR_SUCCESS;

	BHSM_ChannelHandle 	channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t 				unParamLen = 0;
	unsigned char			j;
	BHSM_P_CommandData_t commandData;	
	unsigned char			i;
#if defined(BSP_OTP_PROG_PATTERN_SEQUENCE)
		BHSM_ProgramOtpPatSeqIO_t	inout_progOtpPatSeqIO;
#endif

	BDBG_MSG(("Inside BHSM_ProgramDataSect\n"));
	BDBG_ENTER(BHSM_ProgramDataSect);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BDBG_MSG(("unMode =%x", inoutp_progDataSectIO->unMode));
	for (i=0;i < 32;i++)  {		
		BDBG_MSG(("%.2x ", inoutp_progDataSectIO->aucDataSectData[i]));
	}
	

	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
		( inoutp_progDataSectIO->progDsEnum >=  BPI_Otp_DataSection_e_size ||
 		  inoutp_progDataSectIO->unMode != BCMD_OTP_DATASECTIONPROG_MODE));	
	
#if defined(BSP_OTP_PROG_PATTERN_SEQUENCE)
		BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
			(BHSM_ProgramOTPPatternSequence(in_handle, &inout_progOtpPatSeqIO) != 
																	BERR_SUCCESS ) );	
#endif

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
45670123                                  = DS[239:224] || DS[255:225]
00000012  // eMode
6778eef5   // CRC of data section data
*/
	j=commandData.unInputParamLen/4 + 7;
	 
	for (i=0 ; 	i<BHSM_WRITE_DATA_SECTION_DATA_LEN;  i +=4, j--) {
		
			commandData.unInputParamsBuf[j] =  ((inoutp_progDataSectIO->aucDataSectData[i] <<8) |   
										(inoutp_progDataSectIO->aucDataSectData[i+1] ) |
										(inoutp_progDataSectIO->aucDataSectData[i+2] <<24) |
										inoutp_progDataSectIO->aucDataSectData[i+3] <<16);
 			BDBG_MSG(("j=%d, %.8x ",j, commandData.unInputParamsBuf[j] ));
		commandData.unInputParamLen += 4;		
	}
	commandData.unInputParamsBuf[commandData.unInputParamLen/4] =inoutp_progDataSectIO ->unMode;
	commandData.unInputParamLen += 4;			

	/* TODO: app-to-hsm, no CRC is necessary. hsm should calculate the CRC 
		 		yet for security reason, HSM doesn't want to expose BSP information, hide CRC so far.
		 		to use checksum or a different CRC ?
	*/
	commandData.unInputParamsBuf[commandData.unInputParamLen/4] =inoutp_progDataSectIO ->unCrc;
	commandData.unInputParamLen += 4;			
	

	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

	/* Parse the command specific output parameters */
	inoutp_progDataSectIO->unStatus =  commandData.unOutputParamsBuf[0];
	unParamLen += 4;	

 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_progDataSectIO->unStatus != 0) );			


BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_ProgramDataSect);
	return( errCode );
}


#ifdef BSP_BROADCOM_ONLY
BERR_Code BHSM_ReloadOTP (
		BHSM_Handle			in_handle,
		BHSM_ReloadOtpIO_t	*inoutp_reloadOtpIO
)
{
	BERR_Code errCode = BERR_SUCCESS;

	BDBG_MSG(("Inside BHSM_ReloadOTP\n"));
	BDBG_ENTER(BHSM_ReloadOTP);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

BSTD_UNUSED(inoutp_reloadOtpIO);  /* Removed this when we start using this variable */	
BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_ReloadOTP);
	return( errCode );
}
#endif

