/***************************************************************************
 *     Copyright (c) 2005-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhsm_user_cmds.c $
 * $brcm_Revision: Hydra_Software_Devel/23 $
 * $brcm_Date: 10/28/09 5:49p $
 *
 * Module Description: This file contains Broadcom Host Secure Module (OS/platform 
 *                     independent) porting interface public functions.
 *                    			                    
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7400/a0/common/bhsm_user_cmds.c $
 * 
 * Hydra_Software_Devel/23   10/28/09 5:49p atruong
 * SW7405-3298: Modification for 7405B0 ICAM Build
 * 
 * Hydra_Software_Devel/22   7/27/09 6:42p atruong
 * PR56432: Bug fix - Merging code for 3556C0
 *
 * Hydra_Software_Devel/21   7/25/09 4:12p atruong
 * PR56432: HSM PI Support for Secure RSA/Access cmds and FastCycling
 *
 * Hydra_Software_Devel/20   4/14/09 7:50a atruong
 * PR54104: HSM PI Support for 35130A0
 * 
 * Hydra_Software_Devel/19   3/25/09 6:47p yili
 * PR42660:Fix build error caused by IPTV checkin
 * 
 * Hydra_Software_Devel/18   3/25/09 11:18a yili
 * PR42660: Fixed build issue caused by IPTV check-in
 * 
 * Hydra_Software_Devel/17   3/19/09 1:36p yili
 * PR42660:IPTV integration
 * 
 * Hydra_Software_Devel/16   3/11/09 11:59a yili
 * PR42660:IPTV integration
 * 
 * Hydra_Software_Devel/15   2/19/09 5:02p yili
 * PR42660:IPTV Integration
 * 
 * Hydra_Software_Devel/14   8/21/08 7:38p dbayani
 * PR 45985: Remove errors and warnings.
 * 
 * Hydra_Software_Devel/13   11/3/07 9:18a lyang
 * PR 35987: HSM to support BSECK, MS IPTV commands, SHA1 udpate
 * 
 * Hydra_Software_Devel/12   10/30/07 5:23p lyang
 * PR 35987: HSM to support BSECK, MS IPTV commands, SHA1 udpate
 * 
 * Hydra_Software_Devel/11   10/11/07 5:52p lyang
 * PR 35987: HSM to support MS IPTV commands, SHA1 udpate
 * 
 * Hydra_Software_Devel/10   5/9/07 4:22p lyang
 * PR 27711: Add 7400B0, 7405A0 support to HSM. Purge and shared *.h
 * source udpated
 * 
 * Hydra_Software_Devel/9   12/1/06 10:40p lyang
 * PR 25188: HSM, add function header, data struct header description for
 * those public APIs
 * 
 * Hydra_Software_Devel/5   9/20/06 5:52p lyang
 * PR 24455: HSM PI data size in bits is less than a byte or a word32,
 * rendering the size accordingly. Found from DH testing.
 * - 3DESABA, fix key order from AB to BA, 2nd line key is key_A
 * 
 * Hydra_Software_Devel/4   9/8/06 10:36a lyang
 * PR 24145: fix RSA PI to return correct de/en-cypted msg;  "More" code
 * a3, a8 for both RSA, HMACSHA1
 * 
 * Hydra_Software_Devel/3   5/8/06 8:56p btan
 * PR 21183: HSM needs to compile for 7401 B0
 * 
 * Hydra_Software_Devel/2   4/26/06 11:07a btan
 * PR 20446: HSM needs to compile for 7400 A0
 * 
 * Hydra_Software_Devel/1   2/28/06 8:20p btan
 * PR 19931: Added HSM support for 7400A0
 * 
 * Hydra_Software_Devel/1   1/20/06 5:15p btan
 * PR 19140: Do not encrypt generic mode files.
 * 
 * Hydra_Software_Devel/5   1/17/06 9:31p btan
 * PR 19140: Do not encrypt generic mode files.
 *
 *
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"


#include "bchp_bsp_control_intr2.h"
#include "bchp_bsp_glb_control.h"

#include "bhsm_user_cmds.h"


BDBG_MODULE(BHSM);

BERR_Code      BHSM_UserRSA_PackParam (
		BHSM_UserRsaIO_t	*inoutp_userRsaIO,
		BHSM_P_CommandData_t *p_commandData,
		uint32_t *p_unParamLen		
);

BERR_Code      BHSM_UserDSA_PackParam (
		BHSM_UserDsaIO_t	*inoutp_userDsaIO,
		BHSM_P_CommandData_t *p_commandData,
		uint32_t *p_unParamLen		
);

BERR_Code      BHSM_UserDH_PackParam (
		BHSM_UserDhIO_t	*inoutp_userDhIO,
		BHSM_P_CommandData_t *p_commandData,
		uint32_t *p_unParamLen
	);

BERR_Code      BHSM_UserHmacSha1_PackParam (
		BHSM_UserHmacSha1IO_t	*inoutp_userHmacSha1IO,
		BHSM_P_CommandData_t *p_commandData,
		uint32_t *p_unParamLen		
);


BERR_Code    BHSM_Token_SubmitCommad (
		BHSM_Handle		in_handle,		
		BHSM_P_CommandData_t *p_commandData,
		bool	*p_token,
		int	seqNo
	);

void BHSM_Pack_8BIT_TO_32BIT(uint32_t *p_dest, unsigned char *p_src, unsigned int len);
void BHSM_UnPack_32BIT_TO_8BIT(unsigned char *p_dest, uint32_t *p_src, unsigned int len);
/* #define BDBG_MSG(x) printf x */

BERR_Code BHSM_UserRandomNumber (
		BHSM_Handle					in_handle,
		BHSM_UserRandomNumberIO_t	*inoutp_userRandomNumberIO
)
{
	BERR_Code errCode = BERR_SUCCESS;
	BHSM_ChannelHandle channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t unParamLen = 0; 	/* Parameter length based on Unit32( 32Bit ) */
	BHSM_P_CommandData_t commandData;

	BDBG_MSG(("Inside BHSM_UserRandomNumber\n"));
	BDBG_ENTER(BHSM_UserRandomNumber);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	commandData.cmdId = BCMD_cmdType_eUSER_RANDOM_NUMBER;
	commandData.unContMode = 0;

	unParamLen = 0;
	/* only fill the command specific input parameters */
	if ( inoutp_userRandomNumberIO->ucRandomNumType != BSP_RNG_RNG_CORE_CTRL_RNG_TYPE_RNGRAW &&
		inoutp_userRandomNumberIO->ucRandomNumType != BSP_RNG_RNG_CORE_CTRL_RNG_TYPE_FIPS186_X  &&
		inoutp_userRandomNumberIO->ucRandomNumType != BSP_RNG_RNG_CORE_CTRL_RNG_TYPE_FIPS186_K  &&
		inoutp_userRandomNumberIO->ucRandomNumType != BSP_RNG_RNG_CORE_CTRL_RNG_TYPE_RNGSHA )
	{
/*		errCode = BHSM_STATUS_PARM_VALUE_ERR; */
		errCode = BHSM_STATUS_INPUT_PARM_ERR;				
			goto BHSM_P_DONE_LABEL;
	}		
	
	commandData.unInputParamsBuf[unParamLen] = inoutp_userRandomNumberIO->ucRandomNumType;
	unParamLen ++;

	/* check keyDeat and user input data length */
	switch ( inoutp_userRandomNumberIO->keyDest )
	{
		case RANDOM_NUMBER_OUTPUT_TO_HOST :
			if (inoutp_userRandomNumberIO->unDataLen > BHSM_MAX_RANDOM_NUM_LEN)
			{
/*				errCode = BHSM_STATUS_INDATA_TOOLONG_ERR; */
				errCode = BHSM_STATUS_INPUT_PARM_ERR;			
				goto BHSM_P_DONE_LABEL;
			}
			break;

		case BCMD_KeyRamBuf_eFirstRNRam1024 :
			if (inoutp_userRandomNumberIO->unDataLen > MAX_DATA_LENTH_USER_RANDOM_NUMBER_RNRam1024)
			{
/*				errCode = BHSM_STATUS_INDATA_TOOLONG_ERR; */
				errCode = BHSM_STATUS_INPUT_PARM_ERR;
				goto BHSM_P_DONE_LABEL;
			}		
			break;

		case BCMD_KeyRamBuf_eSecondRNRam1024:	
			if (inoutp_userRandomNumberIO->unDataLen > MAX_DATA_LENTH_USER_RANDOM_NUMBER_RNRam1024)
			{
/*				errCode = BHSM_STATUS_INDATA_TOOLONG_ERR; */
				errCode = BHSM_STATUS_INPUT_PARM_ERR;
				goto BHSM_P_DONE_LABEL;
			}		
			break;
			
		default:
/*			errCode = BHSM_STATUS_PARM_VALUE_ERR; */
			errCode = BHSM_STATUS_INPUT_PARM_ERR;			
			goto BHSM_P_DONE_LABEL;
			
	}		
	commandData.unInputParamsBuf[unParamLen]	= inoutp_userRandomNumberIO->keyDest;
	unParamLen ++;

	if (inoutp_userRandomNumberIO->unDataLen%UINT32_SIZE_IN_BYTES != 0 )
	{
		errCode = BHSM_STATUS_PARM_LEN_ERR;
		goto BHSM_P_DONE_LABEL;
	}	
	commandData.unInputParamsBuf[unParamLen]	= inoutp_userRandomNumberIO->unDataLen;
	unParamLen ++;

	commandData.unInputParamLen = unParamLen*UINT32_SIZE_IN_BYTES;

	if ((errCode = BHSM_P_CommonSubmitCommand (channelHandle, &commandData)))
	{
		errCode = BERR_TRACE(errCode);		
		goto BHSM_P_DONE_LABEL;				
			
	}

	unParamLen = 0;
	/* Parse the command specific output parameters */
	inoutp_userRandomNumberIO->unStatus =  commandData.unOutputParamsBuf[unParamLen];
	unParamLen ++;	

	/* Check status  */
	if( inoutp_userRandomNumberIO->unStatus != BSP_RETURN_SUCESS ) 
	{
		errCode = BHSM_STATUS_FAILED;
		goto BHSM_P_DONE_LABEL;
	}

	/* Check output paramter length */
	if ( (inoutp_userRandomNumberIO->unDataLen -1)/UINT32_SIZE_IN_BYTES + 1  != 
		commandData.unOutputParamLen/UINT32_SIZE_IN_BYTES -unParamLen )
	{
		errCode = BHSM_STATUS_PARM_LEN_ERR;
		goto BHSM_P_DONE_LABEL;
	}

	BHSM_UnPack_32BIT_TO_8BIT( inoutp_userRandomNumberIO->aucData, 
								&commandData.unOutputParamsBuf[unParamLen] , 
								inoutp_userRandomNumberIO->unDataLen );

BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_UserRandomNumber);
	return( errCode );
}

BERR_Code BHSM_UserRSA (
		BHSM_Handle		in_handle,
		BHSM_UserRsaIO_t	*inoutp_userRsaIO
)
{
	BERR_Code errCode = BERR_SUCCESS;
/*	BHSM_ChannelHandle channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2]; */
	uint32_t unParamLen = 0; 	/* Parameter length based on Unit32( 32Bit ) */
	uint32_t remainDataLen,dataBufOffset;
	BHSM_P_CommandData_t commandData;
	int seqNo;

	BDBG_MSG(("Inside BHSM_UserRSA\n"));
	BDBG_ENTER(BHSM_UserRSA);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	if ( (errCode = BHSM_UserRSA_PackParam ( inoutp_userRsaIO,&commandData, &unParamLen)))
	{
		errCode = BERR_TRACE(errCode);		
		goto BHSM_P_DONE_LABEL;
	}

	if ( commandData.unContMode == CONT_MODE_ALL_DATA )
	{
		commandData.unInputParamsBuf[unParamLen] = inoutp_userRsaIO->unInputDataLen;
		unParamLen ++;

		BHSM_Pack_8BIT_TO_32BIT( 	&commandData.unInputParamsBuf[unParamLen] , 	
									inoutp_userRsaIO->aucInputData,
									inoutp_userRsaIO->unInputDataLen );
		unParamLen += (inoutp_userRsaIO->unInputDataLen -1 )/UINT32_SIZE_IN_BYTES + 1;		

		commandData.unInputParamLen = unParamLen * UINT32_SIZE_IN_BYTES;

		if ( (errCode = BHSM_Token_SubmitCommad ( in_handle,&commandData, &(in_handle->bIsBusyPKE), 0 )))
		{
			errCode = BERR_TRACE(errCode);	
			goto BHSM_P_DONE_LABEL;						
		}

	}
	else
	{
		BDBG_ASSERT( (BCMD_PKE_RSA_MAX_DATA_LENGTH%UINT32_SIZE_IN_BYTES) == 0);
	
		remainDataLen = inoutp_userRsaIO->unInputDataLen;
		dataBufOffset = unParamLen;
		seqNo = 0;
		
		while( remainDataLen > 0 ) 
		{					
			if ( remainDataLen <= BCMD_PKE_RSA_MAX_DATA_LENGTH  ) 
			{
				commandData.unContMode = CONT_MODE_LAST_DATA;
				commandData.unInputParamsBuf[unParamLen]= remainDataLen;
				unParamLen ++;

				BHSM_Pack_8BIT_TO_32BIT( 	&commandData.unInputParamsBuf[unParamLen] , 	
					&inoutp_userRsaIO->aucInputData[inoutp_userRsaIO->unInputDataLen-remainDataLen],
					remainDataLen) ;
				unParamLen += (remainDataLen -1) /UINT32_SIZE_IN_BYTES + 1;							
				remainDataLen = 0;
			}			
			else
			{
				commandData.unContMode = CONT_MODE_MORE_DATA;	
				commandData.unInputParamsBuf[unParamLen]= BCMD_PKE_DSA_MAX_DATA_LENGTH;					
				unParamLen ++;
				
				BHSM_Pack_8BIT_TO_32BIT( 	&commandData.unInputParamsBuf[unParamLen] , 	
					&inoutp_userRsaIO->aucInputData[inoutp_userRsaIO->unInputDataLen -remainDataLen],
					BCMD_PKE_DSA_MAX_DATA_LENGTH);
				unParamLen += BCMD_PKE_DSA_MAX_DATA_LENGTH /UINT32_SIZE_IN_BYTES;	
				
				remainDataLen = remainDataLen - BCMD_PKE_DSA_MAX_DATA_LENGTH;
			}	
			
			commandData.unInputParamLen = unParamLen * UINT32_SIZE_IN_BYTES;
			if ( (errCode = BHSM_Token_SubmitCommad ( in_handle,&commandData, &(in_handle->bIsBusyPKE), seqNo )))
			{
				errCode = BERR_TRACE(errCode);	
				goto BHSM_P_DONE_LABEL;						
			}

				
			unParamLen = dataBufOffset ;	/*only pack the data  from the offset of field "inpDataLen" */
			seqNo ++;
			
		}	/*end of while( remainDataLen > 0 ) */;
	}	

	unParamLen = 0;
	/* Parse the command specific output parameters */
	inoutp_userRsaIO ->unStatus =  commandData.unOutputParamsBuf[unParamLen];
	unParamLen ++;	

	/* Check status  */
	if ( inoutp_userRsaIO ->unStatus  != BSP_RETURN_SUCESS )
	{
		errCode = BHSM_STATUS_FAILED;
		goto BHSM_P_DONE_LABEL;
	}

	inoutp_userRsaIO ->unOutputDataLen =  commandData.unOutputParamsBuf[unParamLen];
	unParamLen ++;	
	
	/* Check output paramter length */
	if ( ((inoutp_userRsaIO ->unOutputDataLen+7)/8 +UINT32_SIZE_IN_BYTES-1)/UINT32_SIZE_IN_BYTES != 
		commandData.unOutputParamLen/UINT32_SIZE_IN_BYTES - unParamLen  ) 
	{
		errCode = BHSM_STATUS_PARM_LEN_ERR;
		goto BHSM_P_DONE_LABEL;
	}
		
	BHSM_UnPack_32BIT_TO_8BIT( inoutp_userRsaIO->aucOutputData, 
								&commandData.unOutputParamsBuf[unParamLen] , 
								(inoutp_userRsaIO ->unOutputDataLen+7)/8 );


BHSM_P_DONE_LABEL:
	/*  debug code only
	#include <stdio.h>  
	int i;
	fprintf(stderr,"inoutp_userRsaIO ->unOutputDataLen=%.8X\n commandData.unOutputParamLen=%.8X\nunParamLen=%d\n",
			inoutp_userRsaIO ->unOutputDataLen,  commandData.unOutputParamLen, unParamLen );	

	for (i=0; i<4; i++)
		fprintf(stderr, "commandData.unOutputParamsBuf[%d]=%.8X\n", i, commandData.unOutputParamsBuf[i]);
	*/
	
	BDBG_LEAVE(BHSM_UserRSA);
	return( errCode );
}

BERR_Code BHSM_UserRSA_PackParam (
		BHSM_UserRsaIO_t	*inoutp_userRsaIO,
		BHSM_P_CommandData_t *p_commandData,
		uint32_t *p_unParamLen		
)
{
	BERR_Code errCode = BERR_SUCCESS;

	BDBG_MSG(("BHSM_UserRSA_PackParam \n"));	
	BDBG_ENTER(BHSM_UserRSA_PackParam);

	BKNI_Memset(p_commandData, 0, sizeof(BHSM_P_CommandData_t));

	p_commandData->cmdId = BCMD_cmdType_eUSER_RSA;
	p_commandData->unContMode = 0;

	*p_unParamLen = 0;
	/* only fill the command specific input parameters */
	switch (inoutp_userRsaIO->ucOprMode)
	{
/*		case BPI_PKE_RSAOperation_ePublicKeyDec:  */
		case BPI_PKE_RSAOperation_eNormal:
			/* check user input data length */
			/*
			BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INDATA_TOOLONG_ERR,
				(inoutp_userRsaIO->unInputDataLen > 256));
			*/
			BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR,
				(inoutp_userRsaIO->unInputDataLen > 256));
			p_commandData->unContMode = CONT_MODE_ALL_DATA ;		
			break;
			
/*		case BPI_PKE_RSAOperation_ePrivateKeyEnc:	*/
		case BPI_PKE_RSAOperation_eCrt:
			/* check user input data length */
			/*
			BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INDATA_TOOLONG_ERR,
				(inoutp_userRsaIO->unInputDataLen > 256));
			*/
			BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR,
				(inoutp_userRsaIO->unInputDataLen > 256));			
			p_commandData->unContMode = CONT_MODE_ALL_DATA ;							
			break;
			
		case BPI_PKE_PASS_THROUGH:	
			/* check user input data length */
			/*
			BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INDATA_TOOLONG_ERR,
				(inoutp_userRsaIO->unInputDataLen > 128*5 + 256 + 8));
			*/
			BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR,
				(inoutp_userRsaIO->unInputDataLen > 128*5 + 256 + 8));
			
			if (  inoutp_userRsaIO->unInputDataLen <= BCMD_PKE_RSA_MAX_DATA_LENGTH )
				p_commandData->unContMode = CONT_MODE_ALL_DATA ;	
			else
				p_commandData->unContMode = CONT_MODE_MORE_DATA ;	
			
			break;

		default:
/*			errCode = BHSM_STATUS_PARM_VALUE_ERR; */
			errCode = BHSM_STATUS_INPUT_PARM_ERR;		
			goto BHSM_P_DONE_LABEL;
			
	}
	p_commandData->unInputParamsBuf[*p_unParamLen] = inoutp_userRsaIO->ucOprMode;
	(*p_unParamLen) ++;

	if ( inoutp_userRsaIO->ucOprMode != BPI_PKE_PASS_THROUGH )
	{
		if ( inoutp_userRsaIO->keySrcIndex1 !=  BCMD_KeyRamBuf_eFirstRam2048 &&
			inoutp_userRsaIO->keySrcIndex1 != BCMD_KeyRamBuf_eSecondRam2048 &&
			inoutp_userRsaIO->keySrcIndex1 !=  BCMD_KeyRamBuf_eThirdRam2048)
		{
/*			errCode = BHSM_STATUS_PARM_VALUE_ERR; */
			errCode = BHSM_STATUS_INPUT_PARM_ERR;					
				goto BHSM_P_DONE_LABEL;
		}		

		if ( inoutp_userRsaIO->expSrcIdx !=  BCMD_KeyRamBuf_eFirstRam2048 &&
			inoutp_userRsaIO->expSrcIdx != BCMD_KeyRamBuf_eSecondRam2048 &&
			inoutp_userRsaIO->expSrcIdx !=  BCMD_KeyRamBuf_eThirdRam2048)
		{
/*			errCode = BHSM_STATUS_PARM_VALUE_ERR; */
			errCode = BHSM_STATUS_INPUT_PARM_ERR;					
				goto BHSM_P_DONE_LABEL;
		}		

/*		if (inoutp_userRsaIO->ucOprMode == BPI_PKE_RSAOperation_ePrivateKeyEnc) */
		if (inoutp_userRsaIO->ucOprMode == BPI_PKE_RSAOperation_eCrt )			
			if ( inoutp_userRsaIO->keySrcIndex2 !=  BCMD_KeyRamBuf_eFirstRam2048 &&
				inoutp_userRsaIO->keySrcIndex2 != BCMD_KeyRamBuf_eSecondRam2048 &&
				inoutp_userRsaIO->keySrcIndex2 !=  BCMD_KeyRamBuf_eThirdRam2048)
			{
/*				errCode = BHSM_STATUS_PARM_VALUE_ERR; */
				errCode = BHSM_STATUS_INPUT_PARM_ERR;				
					goto BHSM_P_DONE_LABEL;
			}		
	}


	p_commandData->unInputParamsBuf[*p_unParamLen] = inoutp_userRsaIO->keySrcIndex1;
	(*p_unParamLen) ++;

	p_commandData->unInputParamsBuf[*p_unParamLen] = inoutp_userRsaIO->expSrcIdx;
	(*p_unParamLen) ++;

	p_commandData->unInputParamsBuf[*p_unParamLen] = inoutp_userRsaIO->keySrcIndex2;
	(*p_unParamLen) ++;


BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_UserRSA_PackParam);
	return( errCode );
}


BERR_Code BHSM_UserDSA (
		BHSM_Handle		in_handle,
		BHSM_UserDsaIO_t	*inoutp_userDsaIO
)
{
	BERR_Code errCode = BERR_SUCCESS;
/*	BHSM_ChannelHandle channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2]; */
	uint32_t unParamLen; 	/* Parameter length based on Unit32( 32Bit ) */
	uint32_t remainDataLen,dataBufOffset;
	BHSM_P_CommandData_t commandData;
	int seqNo;

	BDBG_MSG(("Inside BHSM_UserDSA\n"));
	BDBG_ENTER(BHSM_UserDSA);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	if ((errCode = BHSM_UserDSA_PackParam ( inoutp_userDsaIO,&commandData, &unParamLen)))
	{
		errCode = BERR_TRACE((errCode));		
		goto BHSM_P_DONE_LABEL;
	}

	if ( commandData.unContMode == CONT_MODE_ALL_DATA )
	{
		commandData.unInputParamsBuf[unParamLen] = inoutp_userDsaIO->unInputDataLen;
		unParamLen ++;

		BHSM_Pack_8BIT_TO_32BIT( 	&commandData.unInputParamsBuf[unParamLen] , 	
									inoutp_userDsaIO->aucInputData,
									inoutp_userDsaIO->unInputDataLen );
		unParamLen += (inoutp_userDsaIO->unInputDataLen - 1)/UINT32_SIZE_IN_BYTES + 1;		

		commandData.unInputParamLen = unParamLen * UINT32_SIZE_IN_BYTES;

		if ( (errCode = BHSM_Token_SubmitCommad ( in_handle,&commandData, &(in_handle->bIsBusyPKE), 0 )))
		{
			errCode = BERR_TRACE(errCode);	
			goto BHSM_P_DONE_LABEL;						
		}
	}
	else
	{
		BDBG_ASSERT( (BCMD_PKE_DSA_MAX_DATA_LENGTH%UINT32_SIZE_IN_BYTES) == 0);
	
		remainDataLen = inoutp_userDsaIO->unInputDataLen;
		dataBufOffset = unParamLen;
		seqNo = 0;
		
		while( remainDataLen > 0 ) 
		{					
			if ( remainDataLen <= BCMD_PKE_DSA_MAX_DATA_LENGTH  )
			{
				commandData.unContMode = CONT_MODE_LAST_DATA;
				commandData.unInputParamsBuf[unParamLen]= remainDataLen;
				unParamLen ++;

				BHSM_Pack_8BIT_TO_32BIT( 	&commandData.unInputParamsBuf[unParamLen] , 	
					&inoutp_userDsaIO->aucInputData[inoutp_userDsaIO->unInputDataLen -remainDataLen],
					remainDataLen) ;
				unParamLen += (remainDataLen -1 )/UINT32_SIZE_IN_BYTES + 1;			
				remainDataLen = 0;
			}			
			else
			{
				commandData.unContMode = CONT_MODE_MORE_DATA;	
				commandData.unInputParamsBuf[unParamLen]= BCMD_PKE_DSA_MAX_DATA_LENGTH;					
				unParamLen ++;
				
				BHSM_Pack_8BIT_TO_32BIT( 	&commandData.unInputParamsBuf[unParamLen] , 	
					&inoutp_userDsaIO->aucInputData[inoutp_userDsaIO->unInputDataLen -remainDataLen],
					BCMD_PKE_DSA_MAX_DATA_LENGTH);
				unParamLen += BCMD_PKE_DSA_MAX_DATA_LENGTH/UINT32_SIZE_IN_BYTES;				
				remainDataLen = remainDataLen - BCMD_PKE_DSA_MAX_DATA_LENGTH;
			}	
			
			commandData.unInputParamLen = unParamLen * UINT32_SIZE_IN_BYTES;
			if ( (errCode = BHSM_Token_SubmitCommad ( in_handle,&commandData, &(in_handle->bIsBusyPKE), seqNo )))
			{
				errCode = BERR_TRACE(errCode);	
				goto BHSM_P_DONE_LABEL;						
			}
					
			unParamLen = dataBufOffset ;	/*only pack the data  from the offset of field "inpDataLen" */
			seqNo ++;
			
		}	/*end of while( remainDataLen > 0 ) */;
	}	

	unParamLen = 0;
	/* Parse the command specific output parameters */
	inoutp_userDsaIO ->unStatus =  commandData.unOutputParamsBuf[unParamLen];
	unParamLen ++;	

	/* Check status  */
	if ( inoutp_userDsaIO ->unStatus  != BSP_RETURN_SUCESS) 
	{
		errCode = BHSM_STATUS_FAILED;	
		goto BHSM_P_DONE_LABEL;						
	
	}
	
	/* Check output paramter length */
	if( inoutp_userDsaIO->ucOprMode  == BPI_PKE_DSAOperation_ePrivateKeySign ) 
		if (commandData.unOutputParamLen /UINT32_SIZE_IN_BYTES - unParamLen !=40 / UINT32_SIZE_IN_BYTES) 
		{
			errCode = BHSM_STATUS_PARM_LEN_ERR;	
			goto BHSM_P_DONE_LABEL;						
		}

	if (inoutp_userDsaIO->ucOprMode  == BPI_PKE_DSAOperation_ePublicKeyVer)  
		if (commandData.unOutputParamLen /UINT32_SIZE_IN_BYTES - unParamLen != 20/UINT32_SIZE_IN_BYTES) 	
		{
			errCode = BHSM_STATUS_PARM_LEN_ERR;	
			goto BHSM_P_DONE_LABEL;						
		}
	
	BHSM_UnPack_32BIT_TO_8BIT( inoutp_userDsaIO->aucOutputData, 
								&commandData.unOutputParamsBuf[unParamLen] , 
								commandData.unOutputParamLen - unParamLen * UINT32_SIZE_IN_BYTES);
	
BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_UserDSA);
	return( errCode );
}

BERR_Code BHSM_UserDSA_PackParam (
		BHSM_UserDsaIO_t	*inoutp_userDsaIO,
		BHSM_P_CommandData_t *p_commandData,
		uint32_t *p_unParamLen		
)
{
	BERR_Code errCode = BERR_SUCCESS;

	BDBG_MSG(("BHSM_UserDSA_PackParam \n"));	
	BDBG_ENTER(BHSM_UserDSA_PackParam);

	BKNI_Memset(p_commandData, 0, sizeof(BHSM_P_CommandData_t));

	p_commandData->cmdId = BCMD_cmdType_eUSER_DSA;

	*p_unParamLen = 0;
	/* only fill the command specific input parameters */
	BDBG_ASSERT( (BCMD_PKE_DSA_MAX_DATA_LENGTH%UINT32_SIZE_IN_BYTES) == 0);
	
	switch (inoutp_userDsaIO->ucOprMode)
	{
		case BPI_PKE_DSAOperation_ePrivateKeySign:
			/* check user input data length */
			/*
			BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INDATA_TOOLONG_ERR,
				(inoutp_userDsaIO->unInputDataLen > 20));
			*/	
			BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR,
				(inoutp_userDsaIO->unInputDataLen > 20));			
			p_commandData->unContMode = CONT_MODE_ALL_DATA ;		
			break;
			
		case BPI_PKE_DSAOperation_ePublicKeyVer:				
			/* check user input data length */
			/*
			BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INDATA_TOOLONG_ERR,
				(inoutp_userDsaIO->unInputDataLen > 128+ 60));
			*/	
			BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR,
				(inoutp_userDsaIO->unInputDataLen > 128+ 60));
			
			p_commandData->unContMode = CONT_MODE_ALL_DATA ;							
			break;
			
		case BPI_PKE_PASS_THROUGH:	
			/* check user input data length */
			/*
			BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INDATA_TOOLONG_ERR,
				(inoutp_userDsaIO->unInputDataLen > 3*256 + 100 + 12));
			*/
			BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR,
				(inoutp_userDsaIO->unInputDataLen > 3*256 + 100 + 12));			
			
			if (  inoutp_userDsaIO->unInputDataLen <= BCMD_PKE_DSA_MAX_DATA_LENGTH )
				p_commandData->unContMode = CONT_MODE_ALL_DATA ;	
			else
				p_commandData->unContMode = CONT_MODE_MORE_DATA ;	
			
			break;

		default:
/*			errCode = BHSM_STATUS_PARM_VALUE_ERR; */
			errCode =BHSM_STATUS_INPUT_PARM_ERR;
			goto BHSM_P_DONE_LABEL;
			
	}

	if (inoutp_userDsaIO->ucOprMode !=  BPI_PKE_PASS_THROUGH )
	{
		if ( inoutp_userDsaIO->srcPIndex !=  BCMD_KeyRamBuf_eFirstRam2048 &&
			inoutp_userDsaIO->srcPIndex != BCMD_KeyRamBuf_eSecondRam2048 &&
			inoutp_userDsaIO->srcPIndex !=  BCMD_KeyRamBuf_eThirdRam2048)
		{
/*			errCode = BHSM_STATUS_PARM_VALUE_ERR; */
			errCode = BHSM_STATUS_INPUT_PARM_ERR;					
				goto BHSM_P_DONE_LABEL;
		}		

		if ( inoutp_userDsaIO->srcGIndex !=  BCMD_KeyRamBuf_eFirstRam2048 &&
			inoutp_userDsaIO->srcGIndex != BCMD_KeyRamBuf_eSecondRam2048 &&
			inoutp_userDsaIO->srcGIndex !=  BCMD_KeyRamBuf_eThirdRam2048 )
		{
/*			errCode = BHSM_STATUS_PARM_VALUE_ERR; */
			errCode = BHSM_STATUS_INPUT_PARM_ERR;			
				goto BHSM_P_DONE_LABEL;
		}		

		if ( inoutp_userDsaIO->srcQIndex !=  BCMD_KeyRamBuf_eFirstRam2048 &&
			inoutp_userDsaIO->srcQIndex != BCMD_KeyRamBuf_eSecondRam2048 &&
			inoutp_userDsaIO->srcQIndex !=  BCMD_KeyRamBuf_eThirdRam2048 )
		{
/*			errCode = BHSM_STATUS_PARM_VALUE_ERR; */
			errCode = BHSM_STATUS_INPUT_PARM_ERR;			
				goto BHSM_P_DONE_LABEL;
		}		
		
		if ( inoutp_userDsaIO->privKeyXIndex !=  BCMD_KeyRamBuf_eFirstRNRam1024 &&
			inoutp_userDsaIO->privKeyXIndex != BCMD_KeyRamBuf_eSecondRNRam1024 )
		{
/*			errCode = BHSM_STATUS_PARM_VALUE_ERR; */
			errCode = BHSM_STATUS_INPUT_PARM_ERR;			
				goto BHSM_P_DONE_LABEL;
		}		

		if ( inoutp_userDsaIO->privKeyKIndex !=  BCMD_KeyRamBuf_eFirstRNRam1024 &&
			inoutp_userDsaIO->privKeyKIndex != BCMD_KeyRamBuf_eSecondRNRam1024 )
		{
/*			errCode = BHSM_STATUS_PARM_VALUE_ERR; */
			errCode = BHSM_STATUS_INPUT_PARM_ERR;			
				goto BHSM_P_DONE_LABEL;
		}		
	}

	/*Copy user input paramenter even though they might not be used in some operation mode */
	p_commandData->unInputParamsBuf[*p_unParamLen] = inoutp_userDsaIO->ucOprMode;
	(*p_unParamLen) ++;
	
	p_commandData->unInputParamsBuf[*p_unParamLen] = inoutp_userDsaIO->srcPIndex;
	(*p_unParamLen) ++;

	p_commandData->unInputParamsBuf[*p_unParamLen] = inoutp_userDsaIO->srcGIndex;
	(*p_unParamLen) ++;

	p_commandData->unInputParamsBuf[*p_unParamLen] = inoutp_userDsaIO->srcQIndex;
	(*p_unParamLen) ++;
	
	p_commandData->unInputParamsBuf[*p_unParamLen] = inoutp_userDsaIO->privKeyXIndex;
	(*p_unParamLen) ++;

	p_commandData->unInputParamsBuf[*p_unParamLen] = inoutp_userDsaIO->privKeyKIndex;
	(*p_unParamLen) ++;

BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_UserDSA_PackParam );
	return( errCode );
}


BERR_Code BHSM_UserDH (
		BHSM_Handle		in_handle,
		BHSM_UserDhIO_t	*inoutp_userDhIO
)
{
	BERR_Code errCode = BERR_SUCCESS;
/*	BHSM_ChannelHandle channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];  */
	uint32_t unParamLen = 0; 	/* Parameter length based on Unit32( 32Bit ) */
	BHSM_P_CommandData_t commandData;
	uint32_t maxBufDataLen,remainDataLen ,dataBufOffset;
	int seqNo;

	BDBG_MSG(("Inside BHSM_UserDH\n"));
	BDBG_ENTER(BHSM_UserDH);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BHSM_UserDH_PackParam ( inoutp_userDhIO, &commandData, &unParamLen);

	if ( commandData.unContMode == CONT_MODE_ALL_DATA )
	{
		commandData.unInputParamsBuf[unParamLen] = inoutp_userDhIO->unInputDataLen;
		unParamLen ++;

		BHSM_Pack_8BIT_TO_32BIT( 	&commandData.unInputParamsBuf[unParamLen] , 	
									inoutp_userDhIO->aucInputData,
									inoutp_userDhIO->unInputDataLen );
		unParamLen += inoutp_userDhIO->unInputDataLen /UINT32_SIZE_IN_BYTES;		

		commandData.unInputParamLen = unParamLen * UINT32_SIZE_IN_BYTES;

		if ( (errCode = BHSM_Token_SubmitCommad ( in_handle,&commandData, &(in_handle->bIsBusyPKE), 0)))
		{
			errCode = BERR_TRACE(errCode);	
			goto BHSM_P_DONE_LABEL;						
		}

	}
	else
	{
		BDBG_ASSERT( (BCMD_PKE_DH_MAX_DATA_LENGTH%UINT32_SIZE_IN_BYTES) == 0);
	
		maxBufDataLen = BCMD_PKE_DH_MAX_DATA_LENGTH ;
		remainDataLen = inoutp_userDhIO->unInputDataLen;
		dataBufOffset = unParamLen;
		seqNo = 0;

		while( remainDataLen > 0 ) 
		{
						
			if ( remainDataLen <=  maxBufDataLen )	
			{
				commandData.unContMode = CONT_MODE_LAST_DATA;
				commandData.unInputParamsBuf[unParamLen]= remainDataLen;
				unParamLen ++;

				BHSM_Pack_8BIT_TO_32BIT( 	&commandData.unInputParamsBuf[unParamLen] , 	
					&inoutp_userDhIO->aucInputData[inoutp_userDhIO->unInputDataLen -remainDataLen],
					remainDataLen);
				unParamLen += (remainDataLen - 1) /UINT32_SIZE_IN_BYTES + 1;		
				remainDataLen = 0;
			}			
			else
			{
				commandData.unContMode = CONT_MODE_MORE_DATA;	
				commandData.unInputParamsBuf[unParamLen]= maxBufDataLen;					
				unParamLen ++;
				
				BHSM_Pack_8BIT_TO_32BIT( 	&commandData.unInputParamsBuf[unParamLen] , 	
					&inoutp_userDhIO->aucInputData[inoutp_userDhIO->unInputDataLen -remainDataLen],
					maxBufDataLen);
				unParamLen += maxBufDataLen/UINT32_SIZE_IN_BYTES;	
				remainDataLen = remainDataLen - maxBufDataLen;
			}	
			
			commandData.unInputParamLen = unParamLen * UINT32_SIZE_IN_BYTES;

			if ( (errCode = BHSM_Token_SubmitCommad ( in_handle,&commandData, &(in_handle->bIsBusyPKE), seqNo )))
			{
				errCode = BERR_TRACE(errCode);	
				goto BHSM_P_DONE_LABEL;						
			}
			
			unParamLen = dataBufOffset ;	/*only pack the data  from the offset of field "inpDataLen" */
			seqNo ++ ;
						
		}/* end of while(remainDataLen > 0)*/
	}	

	unParamLen = 0;
	/* Parse the command specific output parameters */
	inoutp_userDhIO ->unStatus =  commandData.unOutputParamsBuf[unParamLen];
	unParamLen ++;	

	/* Check status  */
	if ( inoutp_userDhIO ->unStatus  != BSP_RETURN_SUCESS) 
	{
		errCode = BHSM_STATUS_FAILED;	
		goto BHSM_P_DONE_LABEL;						
	
	}
	
	inoutp_userDhIO ->unOutputDataLen =  commandData.unOutputParamsBuf[unParamLen];
	unParamLen ++;	

	/* Check output paramter length */
	/* According to the firmware document, this length is in Bits */
	if (  (inoutp_userDhIO ->unOutputDataLen/8 -1 )/ UINT32_SIZE_IN_BYTES  + 1 != 
				commandData.unOutputParamLen /UINT32_SIZE_IN_BYTES -  unParamLen  )	
	{
		errCode = BHSM_STATUS_PARM_LEN_ERR;
		goto BHSM_P_DONE_LABEL;
	}

	/* 1) rest bit at MSB of the last Word32, (inoutp_userDhIO ->unOutputDataLen+7)/8
	     2) rest bit at LSB of the last Word32, (inoutp_userDhIO ->unOutputDataLen+31)/32*UINT32_SIZE_IN_BYTES
	*/
	BHSM_UnPack_32BIT_TO_8BIT( inoutp_userDhIO->aucOutputData, 
								&commandData.unOutputParamsBuf[unParamLen] , 
								(inoutp_userDhIO ->unOutputDataLen+31)/32*UINT32_SIZE_IN_BYTES);

BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_UserDH);
	return( errCode );
}

BERR_Code BHSM_UserDH_PackParam (
		BHSM_UserDhIO_t	*inoutp_userDhIO,
		BHSM_P_CommandData_t *p_commandData,
		uint32_t *p_unParamLen
	)
{
	BERR_Code errCode = BERR_SUCCESS;

	BDBG_MSG(("BHSM_UserDH_PackParam \n"));	
	BDBG_ENTER(BHSM_UserDH_PackParam);

	BKNI_Memset(p_commandData, 0, sizeof(BHSM_P_CommandData_t));

	p_commandData->cmdId = BCMD_cmdType_eUSER_DH;

	*p_unParamLen = 0;
	/* only fill the command specific input parameters */
	switch (inoutp_userDhIO->ucOprMode)
	{
		case BPI_PKE_DHOperation_ePublicKeyGen:
			/* check user input data length */
			/*
			BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INDATA_TOOLONG_ERR,
				(inoutp_userDhIO->unInputDataLen > 0));
			*/	
			BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR,
				(inoutp_userDhIO->unInputDataLen > 0));			
			
			p_commandData->unContMode = CONT_MODE_ALL_DATA ;	
			break;
			
		case BPI_PKE_DHOperation_eSecretKeyGen:				
			/* check user input data length */
			/*
			BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INDATA_TOOLONG_ERR,
				(inoutp_userDhIO->unInputDataLen > 256));
			*/	
			BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR,
				(inoutp_userDhIO->unInputDataLen > 256));
			p_commandData->unContMode = CONT_MODE_ALL_DATA ;							
			break;
			
		case BPI_PKE_PASS_THROUGH:	
			/* check user input data length */
			/*
			BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INDATA_TOOLONG_ERR,
				(inoutp_userDhIO->unInputDataLen > 3*256 + 12));
			*/			
			BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR,
				(inoutp_userDhIO->unInputDataLen > 3*256 + 12));			
			
			if (  inoutp_userDhIO->unInputDataLen <= BCMD_PKE_DH_MAX_DATA_LENGTH )
				p_commandData->unContMode = CONT_MODE_ALL_DATA ;	
			else
				p_commandData->unContMode = CONT_MODE_MORE_DATA ;	
			
			break;

		default:
/*			errCode = BHSM_STATUS_PARM_VALUE_ERR; */
			errCode = BHSM_STATUS_INPUT_PARM_ERR;
			goto BHSM_P_DONE_LABEL;
			
	}
	p_commandData->unInputParamsBuf[*p_unParamLen] = inoutp_userDhIO->ucOprMode;
	(*p_unParamLen) ++;

	if (inoutp_userDhIO->ucOprMode !=  BPI_PKE_PASS_THROUGH )
	{

		if ( inoutp_userDhIO->srcPIndex !=  BCMD_KeyRamBuf_eFirstRam2048 &&
			inoutp_userDhIO->srcPIndex != BCMD_KeyRamBuf_eSecondRam2048 &&
			inoutp_userDhIO->srcPIndex !=  BCMD_KeyRamBuf_eThirdRam2048)
		{
/*			errCode = BHSM_STATUS_PARM_VALUE_ERR; */
			errCode = BHSM_STATUS_INPUT_PARM_ERR;			
			goto BHSM_P_DONE_LABEL;
		}		

		if ( inoutp_userDhIO->srcGIndex !=  BCMD_KeyRamBuf_eFirstRam2048 &&
			inoutp_userDhIO->srcGIndex != BCMD_KeyRamBuf_eSecondRam2048 &&
			inoutp_userDhIO->srcGIndex !=  BCMD_KeyRamBuf_eThirdRam2048 &&
			inoutp_userDhIO->srcGIndex !=  BPI_PKE_PASS_THROUGH )
		{
/*			errCode = BHSM_STATUS_PARM_VALUE_ERR; */
			errCode = BHSM_STATUS_INPUT_PARM_ERR;					
			goto BHSM_P_DONE_LABEL;
		}		
		
		if ( inoutp_userDhIO->privKeyXIndex !=  BCMD_KeyRamBuf_eFirstRNRam1024 &&
			inoutp_userDhIO->privKeyXIndex != BCMD_KeyRamBuf_eSecondRNRam1024 )
		{
/*			errCode = BHSM_STATUS_PARM_VALUE_ERR; */
			errCode = BHSM_STATUS_INPUT_PARM_ERR;			
			goto BHSM_P_DONE_LABEL;
		}		
	}

	p_commandData->unInputParamsBuf[*p_unParamLen] = inoutp_userDhIO->srcPIndex;
	(*p_unParamLen) ++;

	p_commandData->unInputParamsBuf[*p_unParamLen] = inoutp_userDhIO->srcGIndex;
	(*p_unParamLen) ++;
	
	p_commandData->unInputParamsBuf[*p_unParamLen] = inoutp_userDhIO->privKeyXIndex;
	(*p_unParamLen) ++;

BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_UserDH_PackParam );
	return( errCode );
}

BERR_Code BHSM_UserDesAes (
		BHSM_Handle			in_handle,
		BHSM_UserDesAesIO_t	*inoutp_userDesAesIO
)
{
	BERR_Code errCode = BERR_SUCCESS;
	BHSM_ChannelHandle channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t unParamLen = 0; 	/* Parameter length based on Unit32( 32Bit ) */
	BHSM_P_CommandData_t commandData;
	unsigned char				tempChar, i;

	BDBG_MSG(("Inside BHSM_UserDesAes\n"));	
	BDBG_ENTER(BHSM_UserDesAes);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));
#if (((BCHP_CHIP ==7325) || (BCHP_CHIP ==7335) || (BCHP_CHIP ==3548) || (BCHP_CHIP ==3556) ) && (BCHP_VER >= BCHP_VER_B0)) || \
    (BCHP_CHIP ==7336) || (BCHP_CHIP == 35130) || ((BCHP_CHIP==7405) && (BCHP_VER >= BCHP_VER_B0))
	commandData.cmdId = BCMD_cmdType_eRESERVED_17;
#else
	commandData.cmdId = BCMD_cmdType_eUSER_DES_AES;
#endif
	commandData.unContMode = 0;
	unParamLen = 0;
	/* only fill the command specific input parameters */
	/*
	BDBG_ASSERT( (KEY_INPUT_PARAMETER_SIZE%UINT32_SIZE_IN_BYTES) == 0);
	*/


	if(inoutp_userDesAesIO->ucAlgorithm == BCMD_USER_DES_AES_INPUT_ALGORITHM_SEL_THREE_DES_ABA_ECB){
		for (i=0; i<8; i++){
			tempChar = inoutp_userDesAesIO->auKeyData[i];
			inoutp_userDesAesIO->auKeyData[i] = inoutp_userDesAesIO->auKeyData[i+8];
			inoutp_userDesAesIO->auKeyData[i+8]= tempChar;
		}
	}


	BHSM_Pack_8BIT_TO_32BIT( 	&commandData.unInputParamsBuf[unParamLen] , 	
								inoutp_userDesAesIO->auKeyData,
								KEY_INPUT_PARAMETER_SIZE);

	unParamLen += (KEY_INPUT_PARAMETER_SIZE - 1)/UINT32_SIZE_IN_BYTES + 1;

	if ( 	inoutp_userDesAesIO->ucAlgorithm != BCMD_USER_DES_AES_INPUT_ALGORITHM_SEL_ONE_DES_ECB &&
		inoutp_userDesAesIO->ucAlgorithm != BCMD_USER_DES_AES_INPUT_ALGORITHM_SEL_THREE_DES_ABA_ECB&&
		inoutp_userDesAesIO->ucAlgorithm != BCMD_USER_DES_AES_INPUT_ALGORITHM_SEL_AES_128_ECB)
	{
/*		errCode = BHSM_STATUS_PARM_VALUE_ERR; */
		errCode = BHSM_STATUS_INPUT_PARM_ERR;			
		
		goto BHSM_P_DONE_LABEL;
	}			
	commandData.unInputParamsBuf[unParamLen] = inoutp_userDesAesIO->ucAlgorithm ;
	unParamLen ++;

	if ( inoutp_userDesAesIO->ucOprMode != BHSM_USER_DES_AES_OPRMODE_DECRYPTION &&
		inoutp_userDesAesIO->ucOprMode != BHSM_USER_DES_AES_OPRMODE_ENCRYPTION)
	{
/*		errCode = BHSM_STATUS_PARM_VALUE_ERR; */
		errCode = BHSM_STATUS_INPUT_PARM_ERR;			
		goto BHSM_P_DONE_LABEL;
	}		
	commandData.unInputParamsBuf[unParamLen]	= inoutp_userDesAesIO->ucOprMode;
	unParamLen ++;

	/* check user input data length */
	if(inoutp_userDesAesIO->unInputDataLen > BCMD_USER_DES_AES_INPUT_DATA_MAX_LENGTH)
	{
/*		errCode = BHSM_STATUS_INDATA_TOOLONG_ERR; */
		errCode = BHSM_STATUS_INPUT_PARM_ERR;
		goto BHSM_P_DONE_LABEL;
	}

	commandData.unInputParamsBuf[unParamLen]	= inoutp_userDesAesIO->unInputDataLen;
	unParamLen ++;

	BHSM_Pack_8BIT_TO_32BIT( 	&commandData.unInputParamsBuf[unParamLen] , 	
								inoutp_userDesAesIO->aucInputData,
								inoutp_userDesAesIO->unInputDataLen );
	unParamLen += (inoutp_userDesAesIO->unInputDataLen -1)/UINT32_SIZE_IN_BYTES + 1;
	
	commandData.unInputParamLen = unParamLen * UINT32_SIZE_IN_BYTES;

	if ((errCode = BHSM_P_CommonSubmitCommand (channelHandle, &commandData)))
	{
		errCode = BERR_TRACE(errCode);		
		goto BHSM_P_DONE_LABEL;							
	}

	unParamLen = 0;
	/* Parse the command specific output parameters */
	inoutp_userDesAesIO ->unStatus =  commandData.unOutputParamsBuf[unParamLen];
	unParamLen ++;	

	/* Check status  */
	if( inoutp_userDesAesIO ->unStatus  != BSP_RETURN_SUCESS)
	{
		errCode = BHSM_STATUS_FAILED;
		goto BHSM_P_DONE_LABEL;
	}

	inoutp_userDesAesIO ->unOutputDataLen =  commandData.unOutputParamsBuf[unParamLen];
	unParamLen ++;	
	
	/* Check output paramter length */
	if (  (inoutp_userDesAesIO ->unOutputDataLen -1 )/ UINT32_SIZE_IN_BYTES + 1 != 
				commandData.unOutputParamLen /UINT32_SIZE_IN_BYTES - unParamLen  )	
	{
		errCode = BHSM_STATUS_PARM_LEN_ERR;
		goto BHSM_P_DONE_LABEL;
	}

	BHSM_UnPack_32BIT_TO_8BIT( inoutp_userDesAesIO->aucOutputData, 
								&commandData.unOutputParamsBuf[unParamLen] , 
								inoutp_userDesAesIO ->unOutputDataLen);


BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_UserDesAes);
	return( errCode );
}

#if  (BHSM_IPTV ==1)	

typedef struct
{
	unsigned char 		uBlkData[64];
	unsigned int			uDataSize; /* in bytes, 1~64,  0 if the buffer is empty*/
}LastBlockOfSegment_t;

	
BERR_Code BHSM_UserHmacSha1 (
		BHSM_Handle				in_handle,
		BHSM_UserHmacSha1IO_t	*inoutp_userHmacSha1IO
)
{
	BERR_Code 				errCode = BERR_SUCCESS;
	uint32_t 					unParamLen = 0; 	/* in words */
	BHSM_P_CommandData_t 	commandData;
	uint32_t 					maxBufDataLen,remainDataLen=0 ,dataBufOffset;
	unsigned int 						seqNo, i, indexData;

	static 	LastBlockOfSegment_t		backupBlock;	

	LastBlockOfSegment_t				workshopBlock;
	bool								bTotalOneBlockOrLess=false;

	BDBG_MSG(("Inside BHSM_UserHmacSha1\n"));	
	BDBG_ENTER(BHSM_UserHmacSha1);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
								(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	/********* clean up backup block buffer,  upon the first segment*/
	if (inoutp_userHmacSha1IO->eFirstSegment == FIRST_DATA_SEGMENT_SHA1)
	{
		BKNI_Memset(&backupBlock, 0, sizeof(LastBlockOfSegment_t)  );
	}

	BKNI_Memset(&workshopBlock, 0, sizeof(LastBlockOfSegment_t)  );

	/* no enough data for a complete 64B block ? */
	if ( (backupBlock.uDataSize + inoutp_userHmacSha1IO->unInputDataLen)<= 64 )  
	{
		/* merge/append, need to avoid array overflow at the boundary */
		if (backupBlock.uDataSize < 64)
		{
			for(i=0; i< inoutp_userHmacSha1IO->unInputDataLen; i++)
				{
					backupBlock.uBlkData[backupBlock.uDataSize+i]=inoutp_userHmacSha1IO ->pucInputData[i];
				}		
			backupBlock.uDataSize += inoutp_userHmacSha1IO->unInputDataLen;
		}

		
		/* quick return or copy to workbuf*/
		if (inoutp_userHmacSha1IO->eLastSegment != LAST_DATA_SEGMENT_SHA1) 
		{
			return (BERR_SUCCESS);
		}else
		{
			for(i=0; i<backupBlock.uDataSize; i++ )
			{
				workshopBlock.uBlkData[i] = backupBlock.uBlkData[i];
			}
			workshopBlock.uDataSize = backupBlock.uDataSize;		/* <= 64 */
		}
		bTotalOneBlockOrLess =true;


	}else
	{
		if (backupBlock.uDataSize > 0)
		{
			for(i=0; i<backupBlock.uDataSize; i++ )		
			{
				workshopBlock.uBlkData[i] = backupBlock.uBlkData[i];
			}
		}
		workshopBlock.uDataSize = backupBlock.uDataSize;
		
		if (workshopBlock.uDataSize <64 )			/* avoid array boundary overflow */
		{
			for (i=workshopBlock.uDataSize; i<64; i++)
			{
				workshopBlock.uBlkData[i] = inoutp_userHmacSha1IO ->pucInputData[i-workshopBlock.uDataSize];
			}
		}
		/* update input data index */
		indexData = 64- workshopBlock.uDataSize;		 
		workshopBlock.uDataSize=64;		

		/* update backup */
		i = (backupBlock.uDataSize + inoutp_userHmacSha1IO->unInputDataLen)%64;
		if(i==0)  i = 64;
		backupBlock.uDataSize =i;
		for(i=0; i<backupBlock.uDataSize; i++ )
		{
			backupBlock.uBlkData[i]=inoutp_userHmacSha1IO ->pucInputData[inoutp_userHmacSha1IO->unInputDataLen -backupBlock.uDataSize + i];
		}

		/* remained 64*N data */
		remainDataLen = inoutp_userHmacSha1IO->unInputDataLen -backupBlock.uDataSize - indexData;
		bTotalOneBlockOrLess =false;				
	}

	/*****************************************************************/
	unParamLen = 0;				
	if ( (errCode = BHSM_UserHmacSha1_PackParam ( inoutp_userHmacSha1IO,&commandData, &unParamLen)))
	{
		errCode = BERR_TRACE((errCode));		
		goto BHSM_P_DONE_LABEL;
	}
	/* unParamLen is updated to point to "sha1 data length", in words
	   "commandData" is partially populated with operation, context, hmac key content already
	*/
	dataBufOffset = unParamLen;


	/*************  send out workshop block, 1~64B **********************************************/
	if (bTotalOneBlockOrLess == true )  /*  workshop == backup has all the data, no other data  */		
	{
		if (inoutp_userHmacSha1IO->eLastSegment == LAST_DATA_SEGMENT_SHA1)		/* always true, if this is hit */
		{
			commandData.unContMode = CONT_MODE_LAST_DATA;				
		}
		
	}else	/* must be == 64,    workshop + remained 64*N + backup */
	{
		/* send the 1st 64 workshop */
		commandData.unContMode = CONT_MODE_MORE_DATA;				
	}

	{
			commandData.unInputParamsBuf[unParamLen] = workshopBlock.uDataSize;
			unParamLen++;		/* move down to sha1 data field */
		
			BHSM_Pack_8BIT_TO_32BIT( 	&commandData.unInputParamsBuf[unParamLen] , 	
										&workshopBlock.uBlkData[0],
										workshopBlock.uDataSize );
			
			unParamLen += (workshopBlock.uDataSize+3)/4;	/* include sha1 data field*/			

			commandData.unInputParamLen = unParamLen * UINT32_SIZE_IN_BYTES;  /* convert to bytes */

			if (inoutp_userHmacSha1IO ->eFirstSegment== FIRST_DATA_SEGMENT_SHA1){	seqNo=0;	}
			else {  seqNo = 1;   /* any non-zero number is ok */}

			if ( (errCode = BHSM_Token_SubmitCommad ( in_handle, &commandData, 
					&(in_handle->bIsBusyHmacSha1[inoutp_userHmacSha1IO->contextSwitch-BPI_HmacSha1_Context_eHmacSha1Ctx0]), 
					seqNo )))
			{
				errCode = BERR_TRACE(errCode);	
				goto BHSM_P_DONE_LABEL;						
			}

	}

	/*************  send out N*64 **********************************************/	
	maxBufDataLen = BCMD_HMACSHA1_MAX_DATA_LEN - BCMD_HMACSHA1_MAX_DATA_LEN%( 16 * UINT32_SIZE_IN_BYTES); /* 320 bytes*/

	if (bTotalOneBlockOrLess == false)
	{
		seqNo=1;
		while (remainDataLen > 0)
		{
			unParamLen =	dataBufOffset; 
			
			if ( remainDataLen <=  maxBufDataLen )	
			{

				commandData.unContMode = CONT_MODE_MORE_DATA;
				commandData.unInputParamsBuf[unParamLen]= remainDataLen;
				unParamLen ++;

				BHSM_Pack_8BIT_TO_32BIT( 	&commandData.unInputParamsBuf[unParamLen] , 	
					&inoutp_userHmacSha1IO ->pucInputData[inoutp_userHmacSha1IO->unInputDataLen-remainDataLen-backupBlock.uDataSize],
					remainDataLen);
				unParamLen += (remainDataLen +3)/UINT32_SIZE_IN_BYTES;
				remainDataLen = 0;
			}			
			else
			{
				commandData.unContMode = CONT_MODE_MORE_DATA;	
				commandData.unInputParamsBuf[unParamLen]= maxBufDataLen;					
				unParamLen ++;
				
				BHSM_Pack_8BIT_TO_32BIT( 	&commandData.unInputParamsBuf[unParamLen] , 	
					&inoutp_userHmacSha1IO->pucInputData[inoutp_userHmacSha1IO->unInputDataLen-remainDataLen-backupBlock.uDataSize],
					maxBufDataLen);
				unParamLen += maxBufDataLen/UINT32_SIZE_IN_BYTES;	
				remainDataLen = remainDataLen - maxBufDataLen;
			}	
			
			commandData.unInputParamLen = unParamLen * UINT32_SIZE_IN_BYTES;
			if ( (errCode = BHSM_Token_SubmitCommad ( in_handle, &commandData, 
					&(in_handle->bIsBusyHmacSha1[inoutp_userHmacSha1IO->contextSwitch-BPI_HmacSha1_Context_eHmacSha1Ctx0]), 
					seqNo )))
			{
				errCode = BERR_TRACE(errCode);	
				goto BHSM_P_DONE_LABEL;						
			}

			seqNo++;
		}
	}

	/*************  send out the backup **********************************************/	
	if (bTotalOneBlockOrLess == false)
	{
		if(inoutp_userHmacSha1IO->eLastSegment == LAST_DATA_SEGMENT_SHA1)
		{
			unParamLen =	dataBufOffset; 
			commandData.unContMode = CONT_MODE_LAST_DATA;
			
			commandData.unInputParamsBuf[unParamLen]= backupBlock.uDataSize;
			unParamLen ++;
			

			BHSM_Pack_8BIT_TO_32BIT( 	&commandData.unInputParamsBuf[unParamLen] , 	
					&backupBlock.uBlkData[0],
					backupBlock.uDataSize);

			unParamLen +=  (backupBlock.uDataSize+3)/UINT32_SIZE_IN_BYTES;
			commandData.unInputParamLen = unParamLen * UINT32_SIZE_IN_BYTES;
			
			if ( (errCode = BHSM_Token_SubmitCommad ( in_handle, &commandData, 
					&(in_handle->bIsBusyHmacSha1[inoutp_userHmacSha1IO->contextSwitch-BPI_HmacSha1_Context_eHmacSha1Ctx0]), 
					seqNo )))
			{
				errCode = BERR_TRACE(errCode);	
				goto BHSM_P_DONE_LABEL;						
			}						
			
		}
	}
	


	if ( commandData.unContMode == CONT_MODE_MORE_DATA)
	{
		/* do nothing, fall through, just return the 'errCode' which is set to BERR_SUCCESS for all
		     non-final data piece.   No real SHA1 finger print data returned to upper layers
		*/
	}else	/*CONT_MODE_ALL_DATA or  CONT_MODE_LAST_DATA */
	{
		unParamLen = 0;
		/* Parse the command specific output parameters */
		inoutp_userHmacSha1IO ->unStatus =  commandData.unOutputParamsBuf[unParamLen];
		unParamLen ++;	

		/* Check status  */
		if ( inoutp_userHmacSha1IO ->unStatus  != BSP_RETURN_SUCESS )
		{
			errCode = BHSM_STATUS_FAILED;
			goto BHSM_P_DONE_LABEL;
		}

		/* Check output paramter length */
		if ( (commandData.unOutputParamLen/UINT32_SIZE_IN_BYTES  - unParamLen) != BHSM_SHA1_HMAC_DIGIST_LENGTH/UINT32_SIZE_IN_BYTES ) 
		{
			errCode = BHSM_STATUS_PARM_LEN_ERR;
			goto BHSM_P_DONE_LABEL;
		}

		BHSM_UnPack_32BIT_TO_8BIT( inoutp_userHmacSha1IO->aucOutputData, 
								&commandData.unOutputParamsBuf[unParamLen] , 
								commandData.unOutputParamLen - unParamLen *UINT32_SIZE_IN_BYTES);
	}
BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_UserHmacSha1);
	return( errCode );
}
#else
BERR_Code BHSM_UserHmacSha1 (
		BHSM_Handle				in_handle,
		BHSM_UserHmacSha1IO_t	*inoutp_userHmacSha1IO
)
{
	BERR_Code errCode = BERR_SUCCESS;
/*	BHSM_ChannelHandle channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2]; */
	uint32_t unParamLen = 0; 	/* Parameter length based on Unit32( 32Bit ) */
	BHSM_P_CommandData_t commandData;
	uint32_t maxBufDataLen,remainDataLen ,dataBufOffset;
	int seqNo ;

	BDBG_MSG(("Inside BHSM_UserHmacSha1\n"));	
	BDBG_ENTER(BHSM_UserHmacSha1);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	unParamLen = 0;
	if ( (errCode = BHSM_UserHmacSha1_PackParam ( inoutp_userHmacSha1IO,&commandData, &unParamLen)))
	{
		errCode = BERR_TRACE((errCode));		
		goto BHSM_P_DONE_LABEL;
	}

	if ( commandData.unContMode == CONT_MODE_ALL_DATA )
	{
		commandData.unInputParamsBuf[unParamLen] = inoutp_userHmacSha1IO->unInputDataLen;
		unParamLen ++;

		BHSM_Pack_8BIT_TO_32BIT( 	&commandData.unInputParamsBuf[unParamLen] , 	
									inoutp_userHmacSha1IO->pucInputData,
									inoutp_userHmacSha1IO->unInputDataLen );
		unParamLen += (inoutp_userHmacSha1IO->unInputDataLen - 1) /UINT32_SIZE_IN_BYTES + 1;	/* uint32_t alligned*/	

		commandData.unInputParamLen = unParamLen * UINT32_SIZE_IN_BYTES;

		if ( (errCode = BHSM_Token_SubmitCommad ( in_handle, &commandData, 
				&(in_handle->bIsBusyHmacSha1[inoutp_userHmacSha1IO->contextSwitch-BPI_HmacSha1_Context_eHmacSha1Ctx0]), 
				0)))
		{
			errCode = BERR_TRACE(errCode);	
			goto BHSM_P_DONE_LABEL;						
		}
	}
	else
	{
		/* All HMAC SH1 Data except last one need be 16 words(4Bytes per word) aligned */
		maxBufDataLen = BCMD_HMACSHA1_MAX_DATA_LEN - BCMD_HMACSHA1_MAX_DATA_LEN%( 16 * UINT32_SIZE_IN_BYTES);
		remainDataLen = inoutp_userHmacSha1IO->unInputDataLen;
		dataBufOffset = unParamLen;
		seqNo = 0;

		while( remainDataLen > 0 ) 
		{
						
			if ( remainDataLen <=  maxBufDataLen )	
			{
				if (inoutp_userHmacSha1IO ->eLastSegment ==LAST_DATA_SEGMENT_SHA1)	/* final data segment ?*/
				{
					commandData.unContMode = CONT_MODE_LAST_DATA;
				}else
				{
					commandData.unContMode = CONT_MODE_MORE_DATA;
				}
				commandData.unInputParamsBuf[unParamLen]= remainDataLen;
				unParamLen ++;

				BHSM_Pack_8BIT_TO_32BIT( 	&commandData.unInputParamsBuf[unParamLen] , 	
					&inoutp_userHmacSha1IO ->pucInputData[inoutp_userHmacSha1IO->unInputDataLen-remainDataLen],
					remainDataLen);
				unParamLen += (remainDataLen - 1)/UINT32_SIZE_IN_BYTES + 1;		
				remainDataLen = 0;
			}			
			else
			{
				commandData.unContMode = CONT_MODE_MORE_DATA;	
				commandData.unInputParamsBuf[unParamLen]= maxBufDataLen;					
				unParamLen ++;
				
				BHSM_Pack_8BIT_TO_32BIT( 	&commandData.unInputParamsBuf[unParamLen] , 	
					&inoutp_userHmacSha1IO->pucInputData[inoutp_userHmacSha1IO->unInputDataLen-remainDataLen],
					maxBufDataLen);
				unParamLen += maxBufDataLen/UINT32_SIZE_IN_BYTES;	
				remainDataLen = remainDataLen - maxBufDataLen;
			}	
			
			commandData.unInputParamLen = unParamLen * UINT32_SIZE_IN_BYTES;
			if ( (errCode = BHSM_Token_SubmitCommad ( in_handle, &commandData, 
					&(in_handle->bIsBusyHmacSha1[inoutp_userHmacSha1IO->contextSwitch-BPI_HmacSha1_Context_eHmacSha1Ctx0]), 
					seqNo )))
			{
				errCode = BERR_TRACE(errCode);	
				goto BHSM_P_DONE_LABEL;						
			}
			
			unParamLen = dataBufOffset ;	/*only pack the data  from the offset of field "inpDataLen" */	
			seqNo ++;						
		}/* end of while(remainDataLen > 0)*/
	}	

	if ( commandData.unContMode == CONT_MODE_MORE_DATA)
	{
		/* do nothing, fall through, just return the 'errCode' which is set to BERR_SUCCESS for all
		     non-final data piece.   No real SHA1 finger print data returned to upper layers
		*/
	}else	/*CONT_MODE_ALL_DATA or  CONT_MODE_LAST_DATA */
	{
		unParamLen = 0;
		/* Parse the command specific output parameters */
		inoutp_userHmacSha1IO ->unStatus =  commandData.unOutputParamsBuf[unParamLen];
		unParamLen ++;	

		/* Check status  */
		if ( inoutp_userHmacSha1IO ->unStatus  != BSP_RETURN_SUCESS )
		{
			errCode = BHSM_STATUS_FAILED;
			goto BHSM_P_DONE_LABEL;
		}

		/* Check output paramter length */
		if (commandData.unOutputParamLen/UINT32_SIZE_IN_BYTES  - unParamLen	!= 20/UINT32_SIZE_IN_BYTES ) 
		{
			errCode = BHSM_STATUS_PARM_LEN_ERR;
			goto BHSM_P_DONE_LABEL;
		}

		BHSM_UnPack_32BIT_TO_8BIT( inoutp_userHmacSha1IO->aucOutputData, 
								&commandData.unOutputParamsBuf[unParamLen] , 
								commandData.unOutputParamLen - unParamLen *UINT32_SIZE_IN_BYTES);
	}
BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_UserHmacSha1);
	return( errCode );
}

#endif




BERR_Code BHSM_UserHmacSha1_PackParam (
		BHSM_UserHmacSha1IO_t	*inoutp_userHmacSha1IO,
		BHSM_P_CommandData_t *p_commandData,
		uint32_t *p_unParamLen		
)
{
	BERR_Code errCode = BERR_SUCCESS;

	BDBG_MSG(("BHSM_UserHmacSha1_PackParam \n"));	
	BDBG_ENTER(BHSM_UserHmacSha1_PackParam);

	BKNI_Memset(p_commandData, 0, sizeof(BHSM_P_CommandData_t));

	p_commandData->cmdId = BCMD_cmdType_eUSER_SHA1;

	/* only fill the command specific input parameters */
	if ( inoutp_userHmacSha1IO->oprMode != BPI_HmacSha1_Op_eSha1 &&
		inoutp_userHmacSha1IO->oprMode != BPI_HmacSha1_Op_eHmac && 
		inoutp_userHmacSha1IO->oprMode != BPI_HmacSha1_Op_eHmac_KeySrcReserve1 )
	{

		errCode = BHSM_STATUS_INPUT_PARM_ERR;
			goto BHSM_P_DONE_LABEL;
	}		
	/* this patch */
	
	if (inoutp_userHmacSha1IO->oprMode == BPI_HmacSha1_Op_eHmac_KeySrcReserve1)
	{
		inoutp_userHmacSha1IO->oprMode = BPI_HmacSha1_Op_eHmac | ( (BCMD_KeyRamBuf_eKey5KeyLadder1 & 0xFF) <<8);
	}
	p_commandData->unInputParamsBuf[*p_unParamLen] = inoutp_userHmacSha1IO->oprMode ;
	(*p_unParamLen) ++;

	if ( inoutp_userHmacSha1IO->contextSwitch != BPI_HmacSha1_Context_eHmacSha1Ctx0  

#if (BCHP_CHIP !=7440)
		&&
		inoutp_userHmacSha1IO->contextSwitch != BPI_HmacSha1_Context_eHmacSha1Ctx1 

#if ((BCHP_CHIP !=7400) || (BCHP_VER < BCHP_VER_B0)) && (BCHP_CHIP !=7405)
		&&		
		inoutp_userHmacSha1IO->contextSwitch != BPI_HmacSha1_Context_eHmacSha1Ctx2
#endif
		)
#else
	if ( inoutp_userHmacSha1IO->contextSwitch <= BPI_HmacSha1_Context_eHmacSha1Ctx0  ||
		inoutp_userHmacSha1IO->contextSwitch  >= BPI_HmacSha1_Context_eMax)
#endif
	{

		errCode = BHSM_STATUS_INPUT_PARM_ERR;
		goto BHSM_P_DONE_LABEL;		
	}			
	p_commandData->unInputParamsBuf[*p_unParamLen] = inoutp_userHmacSha1IO->contextSwitch ;
	(*p_unParamLen) ++;
	
	BDBG_ASSERT( (BCMD_HMACSHA1_KEY_LEN%UINT32_SIZE_IN_BYTES) == 0);

	BHSM_Pack_8BIT_TO_32BIT( 	&p_commandData->unInputParamsBuf[*p_unParamLen] , 	
								inoutp_userHmacSha1IO->keyData,
								BCMD_HMACSHA1_KEY_LEN);
	*p_unParamLen += (BCMD_HMACSHA1_KEY_LEN - 1)/UINT32_SIZE_IN_BYTES + 1;

	if (inoutp_userHmacSha1IO->unInputDataLen < BCMD_HMACSHA1_MAX_DATA_LEN )
		if (inoutp_userHmacSha1IO ->eLastSegment ==LAST_DATA_SEGMENT_SHA1)	/* final data segment ?*/
		{
			p_commandData->unContMode = CONT_MODE_ALL_DATA ;	
		}else{
			p_commandData->unContMode = CONT_MODE_MORE_DATA ;	
		}
	else
		p_commandData->unContMode = CONT_MODE_MORE_DATA ;	

BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_UserHmacSha1_PackParam);
	return( errCode );
}

BERR_Code BHSM_CancelCmd (
		BHSM_Handle			in_handle,
		BHSM_CancelCmdIO_t	*inoutp_cancelCmdIO
)
{
	BERR_Code errCode = BERR_SUCCESS;
	BHSM_ChannelHandle channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface1];
	uint32_t unParamLen = 0;
	BHSM_P_CommandData_t commandData;

	BDBG_MSG(("Inside BHSM_CancelCmd\n"));	
	BDBG_ENTER(BHSM_CancelCmd);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	commandData.cmdId = BCMD_cmdType_eSESSION_BRCM_CANCEL_COMMAND;
	commandData.unContMode = 0;
	
	/* only fill the command specific input parameters */

	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

	/* Parse the command specific output parameters */
	inoutp_cancelCmdIO->unStatus =  commandData.unOutputParamsBuf[0];
	unParamLen += 4;	

	/* Check output paramter length */
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_PARM_LEN_ERR, 
		(unParamLen != commandData.unOutputParamLen) );	

 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_cancelCmdIO->unStatus != 0) );		
	
BHSM_P_DONE_LABEL:

	
	BDBG_LEAVE(BHSM_CancelCmd);
	BDBG_MSG(("Exit BHSM_CancelCmd\n"));	
	return( errCode );
}

BERR_Code BHSM_Token_SubmitCommad (
		BHSM_Handle		in_handle,		
		BHSM_P_CommandData_t *p_commandData,
		bool	*p_token,
		int	seqNo
	)
{
	BERR_Code errCode = BERR_SUCCESS;
	BHSM_ChannelHandle channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t unStatus;

	BDBG_MSG(("Inside BHSM_Token_SubmitCommad \n"));	
	BDBG_ENTER(BHSM_Token_SubmitCommad);

	/*Lock the Mutex 
	if ( (errCode = BKNI_AcquireMutex( channelHandle->mutexLock)) )
	{
		errCode = BERR_TRACE(errCode);	
		goto BHSM_P_DONE_LABEL;	
	}*/

	/*Acquire the context token*/
	if ( seqNo == 0 || p_commandData->unContMode == CONT_MODE_ALL_DATA  ) 
	{
	 /* Only acquire context token in the first time submit */
		if ( !*p_token ) 
			*p_token = true;	 /* repeatedly this could be set once for every 1st 320-byte piece in each segment, this is not desired yet it's harmless so far */
		else
		{
			/*This Context token has been used*/
			errCode = BHSM_STATUS_HW_BUSY_ERR;	
			goto BHSM_P_DONE_LABEL;	
		}
	}					
				
#if (BHSM_IPTV ==0) 
	/* PKE calls should be synchronous due to PKE pollong support */
	p_commandData->bLockWaive = true;
#endif

	if ( (errCode = BHSM_P_CommonSubmitCommand (channelHandle, p_commandData)))
	{
		errCode = BERR_TRACE(errCode);
		*p_token = false;				
		goto BHSM_P_DONE_LABEL;	
	}

	unStatus =  p_commandData->unOutputParamsBuf[0];

	if ( p_commandData->unContMode == CONT_MODE_MORE_DATA)
	{     /* 1)if there is a new "More" defined in bsp_err.h, this line may be affected.
		     2) suggest to define just one "More" enum a3/a8/etc... to be one value.
	       */
		if ( unStatus  != BSP_NEED_MORE_DATA_PKE  && unStatus  != BSP_NEED_MORE_DATA_HMACSHA1)
		{
			errCode = BHSM_STATUS_FAILED;
			*p_token = false;			
			goto BHSM_P_DONE_LABEL;
		}	
	}	

	/*Release the context token*/
	if (  p_commandData->unContMode == CONT_MODE_ALL_DATA  || 
		p_commandData->unContMode == CONT_MODE_LAST_DATA)  
		*p_token = false;			

BHSM_P_DONE_LABEL:

	/* UnLock the Mutex
	BKNI_ReleaseMutex( channelHandle->mutexLock);*/
	
	BDBG_LEAVE(BHSM_Token_SubmitCommad);
	return( errCode );
}




void BHSM_Pack_8BIT_TO_32BIT(uint32_t *p_dest, unsigned char *p_src, unsigned int len)
{
	unsigned int i;

	if(len==0) return;

	for ( i = 0 ; i < len ; i++ )
	{
		if ( i % UINT32_SIZE_IN_BYTES == 0)	*(p_dest + i/UINT32_SIZE_IN_BYTES ) = 0;
		*(p_dest + i /UINT32_SIZE_IN_BYTES )  |=  (*(p_src + i ) << (32 - ( i%UINT32_SIZE_IN_BYTES + 1)*8));	
	}	

}


void BHSM_UnPack_32BIT_TO_8BIT(unsigned char *p_dest, uint32_t *p_src, unsigned int len)
{
	unsigned int i;

	for ( i = 0 ; i < len ; i++ )
		*(p_dest + i )  =  (unsigned char)((*(p_src+i /UINT32_SIZE_IN_BYTES) 
										>> 	(32 - ( i %UINT32_SIZE_IN_BYTES +1) *8)) & 0xFF) ;

}


