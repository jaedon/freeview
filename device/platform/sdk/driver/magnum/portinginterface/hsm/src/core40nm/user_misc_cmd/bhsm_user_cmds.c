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
 * $brcm_Workfile: bhsm_user_cmds.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 10/4/12 4:24p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/src/core40nm/user_misc_cmd/bhsm_user_cmds.c $
 * 
 * Hydra_Software_Devel/5   10/4/12 4:24p atruong
 * SW7425-3915:KeyLength field deleted for FullROM HMAC/SHA BSP command
 * 
 * Hydra_Software_Devel/4   7/3/12 11:21a atruong
 * SWSECURITY-53: Security code maintenance - unnecessary bound checking
 * 
 * Hydra_Software_Devel/3   6/21/12 4:09p atruong
 * SWSECURITY-109: SHA Digest calculation for non DWORD multiple data
 * buffers.
 * 
 * Hydra_Software_Devel/2   2/15/12 1:04p atruong
 * SWSECURITY-109: HMAC-SHA command support for 40-nm platforms
 * 
 * Hydra_Software_Devel/1   10/12/11 8:36p atruong
 * SW7425-1251: HSM PI Support For BCM7425B0
 * 
 * Hydra_Software_Devel/4   6/29/11 7:22p atruong
 * SW7425-812:HSM PI API modification for new HMAC-SHA support
 * 
 * Hydra_Software_Devel/4   6/29/11 7:18p atruong
 * SW7425-812:HSM PI API modification for new HMAC-SHA support
 * 
 * Hydra_Software_Devel/4   6/29/11 7:15p atruong
 * SW7425-812:HSM PI API modification for new HMAC-SHA support
 * 
 * Hydra_Software_Devel/3   6/9/11 3:14p katrep
 * SW7231-4:fixed compiler warning
 * 
 * Hydra_Software_Devel/2   5/26/11 7:15a atruong
 * SW7422-450:Support of BSP HMAC/SHA command in multiple buffer mode in
 * HSM PI for 40-nm platforms
 * 
 * Hydra_Software_Devel/1   1/25/11 6:42p atruong
 * SW7422-26: Add HSM PI Support For 7422A0  - CC header format
 *
 ******************************************************************************/


#include "bchp_bsp_control_intr2.h"
#include "bchp_bsp_glb_control.h"

#include "bhsm_user_cmds.h"


BDBG_MODULE(BHSM);



BERR_Code      BHSM_UserHmacSha_PackParam (
		BHSM_UserHmacShaIO_t	*inoutp_userHmacShaIO,
		BHSM_P_CommandData_t    *p_commandData,
		uint32_t                *p_unParamLen,		
		uint32_t				unInputDataLen,
		uint32_t					uInputData
);


BERR_Code    BHSM_Token_SubmitCommand (
		BHSM_Handle		in_handle,		
		BHSM_P_CommandData_t *p_commandData,
		bool	*p_token,
		int	seqNo
	);

void BHSM_Pack_8BIT_TO_32BIT(uint32_t *p_dest, unsigned char *p_src, unsigned int len);
void BHSM_UnPack_32BIT_TO_8BIT(unsigned char *p_dest, uint32_t *p_src, unsigned int len);


static const int HmacShaKeySize[] = { 20, 28, 32 };

BERR_Code BHSM_UserRandomNumber (
		BHSM_Handle					in_handle,
		BHSM_UserRandomNumberIO_t	*inoutp_userRandomNumberIO
)
{
	BERR_Code 				errCode       = BERR_SUCCESS;
	BHSM_ChannelHandle 		channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t 				unParamLen    = 0; 	/* Parameter length based on Unit32( 32Bit ) */
	BHSM_P_CommandData_t 	commandData;


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
	if ( inoutp_userRandomNumberIO->RandomNumType >= BSP_RNG_RNG_CORE_CTRL_RNG_TYPE_MAX )
	{
		errCode = BHSM_STATUS_INPUT_PARM_ERR;				
		goto BHSM_P_DONE_LABEL;
	}		
	
	commandData.unInputParamsBuf[unParamLen] = inoutp_userRandomNumberIO->RandomNumType;
	unParamLen ++;

	/* check keyDeat and user input data length */
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
		(inoutp_userRandomNumberIO->keyDest != RANDOM_NUMBER_OUTPUT_TO_HOST ) );
	
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 
		(inoutp_userRandomNumberIO->unDataLen > BHSM_MAX_RANDOM_NUM_LEN ) );


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

static void BHSM_Copy_Swap ( unsigned char * pDest,  unsigned char * pData, unsigned int len)
{
	unsigned int alignment;
	unsigned int tmp;
	unsigned int i;
	unsigned int runs, left_over;

	alignment = ((unsigned int)pDest) & 0x3;
	if ( alignment != 0 )
	{
		unsigned int left_over = 4 - alignment;
		unsigned int * pStart = (unsigned int*)(pDest-alignment);

		tmp = (*pStart) & (0xFFFFFFFF<< (left_over*8));
		do 
		{
			tmp |= ((*pData) << ((left_over-1)*8) );
			len--;
			left_over--;
			pDest ++;
			pData ++;
		} while ((len>0) && (left_over>0 ));

		(*pStart) = tmp;
	}

	if ( len==0 )
		return;

	/* pDest should be aligned to 32 bits boundary by NOW */
	runs = len>>2;
	left_over = len & 3;

	for (i=0;i<runs;i++ )
	{
		tmp = (((unsigned int)pData[0])<<24) |
			(((unsigned int)pData[1])<<16) |
			(((unsigned int)pData[2])<<8) |
			((unsigned int)pData[3]); 

		* ((unsigned int *)pDest) = tmp;
		pDest+=4;
		pData+=4;
	}

	if ( left_over )
	{
		tmp = 0;
		for (i=0;i<left_over;i++)
		{
			tmp |= (((unsigned int)pData[i])<<(24-i*8));
		}
		* ((unsigned int *)pDest) = tmp;
	}

}


BERR_Code BHSM_UserHmacShaFlush (
		BHSM_Handle				in_handle,
		BHSM_UserHmacShaIO_t	*inoutp_userHmacShaIO
)
{
	BERR_Code             errCode = BERR_SUCCESS;
	uint32_t              unParamLen = 0; 	/* Parameter length based on Unit32( 32Bit ) */
	uint32_t              i;
	uint32_t              HmacShaDigestLen;
	BHSM_P_SHAContext_t * pContext = NULL;
	BHSM_P_CommandData_t  commandData;

	BDBG_MSG(("Inside BHSM_UserHmacSha\n"));	
	BDBG_ENTER(BHSM_UserHmacSha);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	pContext = &(in_handle->shaContext);
	for (i = 0; i < 2; i++)
	{
		unParamLen = 0;
		if ( (errCode = BHSM_UserHmacSha_PackParam ( inoutp_userHmacShaIO,&commandData, &unParamLen, 10, pContext->shaBufferOffset)))
		{
			errCode = BERR_TRACE((errCode));		
			goto BHSM_P_DONE_LABEL;
		}
		HmacShaDigestLen = HmacShaKeySize[inoutp_userHmacShaIO->shaType];
		
		commandData.unInputParamLen = unParamLen * UINT32_SIZE_IN_BYTES;
		if (i == 1)
			commandData.unContMode = BHSM_HMACSHA_ContinualMode_eLastSeg;
		else
			commandData.unContMode = BHSM_HMACSHA_ContinualMode_eMoreSeg;
		if ( (errCode = BHSM_Token_SubmitCommand (in_handle, 
												   &commandData,
												   &(in_handle->bIsBusyHmacSha1[inoutp_userHmacShaIO->contextSwitch-BPI_HmacSha1_Context_eHmacSha1Ctx0]), 
												   0
												  )))
		{
			BDBG_ERR(("Error calling BHSM_Token_SubmitCommand()\n"));
			errCode = BERR_TRACE(errCode);	
			goto BHSM_P_DONE_LABEL; 					
		}
		BKNI_Sleep (1);  /* This is to avoid BSP WD reset */

	}

BHSM_P_DONE_LABEL:
	BDBG_LEAVE(BHSM_UserHmacSha);
	return( errCode );

}

BERR_Code BHSM_UserHmacSha (
		BHSM_Handle				in_handle,
		BHSM_UserHmacShaIO_t	*inoutp_userHmacShaIO
)
{
	BERR_Code             errCode = BERR_SUCCESS;
	uint32_t              unParamLen = 0; 	/* Parameter length based on Unit32( 32Bit ) */
	BHSM_P_CommandData_t  commandData;
	uint32_t              HmacShaDigestLen = 0;
	BHSM_P_SHAContext_t * pContext = NULL;
	uint32_t              unInputDataLen;
	unsigned char		* pData;
	uint32_t              bytesToSend, bytesToCopy;
	uint32_t              segmentCount = 0;

	BDBG_MSG(("Inside BHSM_UserHmacSha\n"));	
	BDBG_ENTER(BHSM_UserHmacSha);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		( (inoutp_userHmacShaIO->unInputDataLen != 0)&&(inoutp_userHmacShaIO->pucInputData==NULL) ) );	

	pContext = &(in_handle->shaContext);
	unInputDataLen = inoutp_userHmacShaIO->unInputDataLen;
	if (inoutp_userHmacShaIO->contMode == BHSM_HMACSHA_ContinualMode_eMoreSeg)
	{
		pContext->bMultiple = true;
	}
	pData = inoutp_userHmacShaIO->pucInputData;

	do 
	{
		segmentCount += 1;
		if (unInputDataLen <= pContext->bufferSizeMax)
			bytesToSend = unInputDataLen;
		else
			bytesToSend = pContext->bufferSizeMax;

#if 1
		if(!bytesToSend%UINT32_SIZE_IN_BYTES)
			{
				bytesToCopy = bytesToSend;
			}
		else
			{
				bytesToCopy = (bytesToSend/UINT32_SIZE_IN_BYTES)*UINT32_SIZE_IN_BYTES+UINT32_SIZE_IN_BYTES;
			}

		if (inoutp_userHmacShaIO->byteSwap)
			/* copy and also byte swap */
			BHSM_Copy_Swap ( (pContext->pBuffer+pContext->bufferSize), pData, bytesToCopy);
		else
			/* just copy */
			BKNI_Memcpy((pContext->pBuffer+pContext->bufferSize), pData, bytesToCopy);
#else
		if (inoutp_userHmacShaIO->byteSwap)
			/* copy and also byte swap */
			BHSM_Copy_Swap ( (pContext->pBuffer+pContext->bufferSize), pData, bytesToSend);
		else
			/* just copy */
			BKNI_Memcpy((pContext->pBuffer+pContext->bufferSize), pData, bytesToSend);


#endif
		
		unInputDataLen -= bytesToSend;
		pData += bytesToSend;
		unParamLen = 0;
		if ( (errCode = BHSM_UserHmacSha_PackParam ( inoutp_userHmacShaIO,&commandData, &unParamLen, bytesToSend, pContext->shaBufferOffset)))
		{
			errCode = BERR_TRACE((errCode));		
			goto BHSM_P_DONE_LABEL;
		}
		HmacShaDigestLen = HmacShaKeySize[inoutp_userHmacShaIO->shaType];
		
		commandData.unInputParamLen = unParamLen * UINT32_SIZE_IN_BYTES;
		if (unInputDataLen == 0)
		{
			if (pContext->bMultiple)
			{
				commandData.unContMode = inoutp_userHmacShaIO->contMode;
			}
			else
			{
				if (segmentCount == 1)
					commandData.unContMode = BHSM_HMACSHA_ContinualMode_eAllSeg;
				else
					commandData.unContMode = BHSM_HMACSHA_ContinualMode_eLastSeg;
			}
		}
		else
		{
			commandData.unContMode = BHSM_HMACSHA_ContinualMode_eMoreSeg;
		}
		
		if ( (errCode = BHSM_Token_SubmitCommand (in_handle, 
		                                           &commandData,
		                                           &(in_handle->bIsBusyHmacSha1[inoutp_userHmacShaIO->contextSwitch-BPI_HmacSha1_Context_eHmacSha1Ctx0]), 
		                                           0
		                                          )))
		{
			BDBG_ERR(("Error calling BHSM_Token_SubmitCommand()\n"));
			errCode = BERR_TRACE(errCode);	
			goto BHSM_P_DONE_LABEL; 					
		}
		BKNI_Sleep (1);  /* This is to avoid BSP WD reset */


	}while (unInputDataLen > 0);

	/* Start collecting output */	
	if ((inoutp_userHmacShaIO->contMode == BHSM_HMACSHA_ContinualMode_eLastSeg) ||
	    (inoutp_userHmacShaIO->contMode == BHSM_HMACSHA_ContinualMode_eAllSeg))
	{

		unParamLen = 0;
		/* Parse the command specific output parameters */
		inoutp_userHmacShaIO->unStatus =  commandData.unOutputParamsBuf[unParamLen];
		unParamLen ++;	
		
		/* Check status  */
		if ( inoutp_userHmacShaIO ->unStatus  != BSP_RETURN_SUCESS )
		{
			errCode = BHSM_STATUS_FAILED;
			goto BHSM_P_DONE_LABEL;
		}
		
		/* Check output paramter length */
		if (commandData.unOutputParamLen/UINT32_SIZE_IN_BYTES  - unParamLen != HmacShaDigestLen/UINT32_SIZE_IN_BYTES ) 
		{
			errCode = BHSM_STATUS_PARM_LEN_ERR;
			goto BHSM_P_DONE_LABEL;
		}
		
		BHSM_UnPack_32BIT_TO_8BIT( inoutp_userHmacShaIO->aucOutputDigest, 
								&commandData.unOutputParamsBuf[unParamLen] , 
								commandData.unOutputParamLen - unParamLen *UINT32_SIZE_IN_BYTES);
		
		/* return also the digest length */
		inoutp_userHmacShaIO->digestSize = commandData.unOutputParamLen - unParamLen *UINT32_SIZE_IN_BYTES;
		
		/* reset multiple app buffer setting */
		pContext->bMultiple = false;

	}

	/* The else case here is just to return success to fall through */

BHSM_P_DONE_LABEL:
	if ( (errCode != BERR_SUCCESS ) && (pContext != NULL ))
	{
		BDBG_ERR(("Error calling BHSM_UserHmacSha()\n"));
		/* Reset SHA context in case of failture */
		pContext->bufferSize = 0;
		pContext->bMultiple = false;
		/* flush out the pending HMACSHA command */
		BHSM_UserHmacShaFlush(in_handle, inoutp_userHmacShaIO);
	}
	
	BDBG_LEAVE(BHSM_UserHmacSha);
	return( errCode );
}



BERR_Code BHSM_UserHmacSha_PackParam (
		BHSM_UserHmacShaIO_t	*inoutp_userHmacShaIO,
		BHSM_P_CommandData_t    *p_commandData,
		uint32_t                *p_unParamLen,
		uint32_t				unInputDataLen,
		uint32_t				uInputData
)
{
	BERR_Code errCode = BERR_SUCCESS;
	int       HmacShaKeyLen;

	BDBG_MSG(("BHSM_UserHmacSha_PackParam \n"));	
	BDBG_ENTER(BHSM_UserHmacSha_PackParam);

	BKNI_Memset(p_commandData, 0, sizeof(BHSM_P_CommandData_t));


	p_commandData->cmdId      = BCMD_cmdType_eUSER_SHA1;
	p_commandData->unContMode = inoutp_userHmacShaIO->contMode;

	/* only fill the command specific input parameters */
	if ( inoutp_userHmacShaIO->oprMode != BPI_HmacSha1_Op_eSha1 &&
		inoutp_userHmacShaIO->oprMode  != BPI_HmacSha1_Op_eHmac )

	{
		errCode = BHSM_STATUS_INPUT_PARM_ERR;
			goto BHSM_P_DONE_LABEL;
	}		

	p_commandData->unInputParamsBuf[*p_unParamLen] = 
			inoutp_userHmacShaIO->oprMode |
			(inoutp_userHmacShaIO->VirtualKeyLadderID << BHSM_HMACSHA_VKL_SHIFT) |
			(inoutp_userHmacShaIO->keyLayer           << BHSM_HMACSHA_KL_SHIFT);
	(*p_unParamLen) ++;

	if ( inoutp_userHmacShaIO->contextSwitch <  BPI_HmacSha1_Context_eHmacSha1Ctx0  ||
		inoutp_userHmacShaIO->contextSwitch  >= BPI_HmacSha1_Context_eMax)
	{

		errCode = BHSM_STATUS_INPUT_PARM_ERR;
		goto BHSM_P_DONE_LABEL;		
	}			
	p_commandData->unInputParamsBuf[*p_unParamLen] = 
			inoutp_userHmacShaIO->contextSwitch |
			(inoutp_userHmacShaIO->shaType    << BHSM_HMACSHA_SHATYPE_SHIFT) |
			(inoutp_userHmacShaIO->dataSource << BHSM_HMACSHA_DRAM_SHIFT   ) |
	        (inoutp_userHmacShaIO->keyIncMode << BHSM_HMACSHA_KEYINC_SHIFT );
	(*p_unParamLen) ++;

	HmacShaKeyLen = HmacShaKeySize[inoutp_userHmacShaIO->shaType];
	/* in response to SW7425-3915: different command format for FullROM mode  */
	if (!bHSMPIRunningFullROM)
	{
		p_commandData->unInputParamsBuf[*p_unParamLen] = HmacShaKeyLen;
		(*p_unParamLen) ++;
	}


	BHSM_Pack_8BIT_TO_32BIT(&p_commandData->unInputParamsBuf[*p_unParamLen] , 	
							inoutp_userHmacShaIO->keyData,
							HmacShaKeyLen);
	*p_unParamLen += (HmacShaKeyLen - 1)/UINT32_SIZE_IN_BYTES + 1;

	p_commandData->unInputParamsBuf[*p_unParamLen] = unInputDataLen;
	(*p_unParamLen)++;

	p_commandData->unInputParamsBuf[*p_unParamLen] = uInputData;
	(*p_unParamLen)++;


BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_UserHmacSha_PackParam);
	return( errCode );

}




BERR_Code BHSM_Token_SubmitCommand (
		BHSM_Handle		in_handle,		
		BHSM_P_CommandData_t *p_commandData,
		bool	*p_token,
		int	seqNo
	)
{
	BERR_Code errCode = BERR_SUCCESS;
	BHSM_ChannelHandle channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t unStatus;

	BSTD_UNUSED(seqNo);

	BDBG_MSG(("Inside BHSM_Token_SubmitCommand \n"));	
	BDBG_ENTER(BHSM_Token_SubmitCommand);

	/*Lock the Mutex 
	if ( (errCode = BKNI_AcquireMutex( channelHandle->mutexLock)) )
	{
		errCode = BERR_TRACE(errCode);	
		goto BHSM_P_DONE_LABEL;	
	}*/

#if 0
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
	
	BDBG_LEAVE(BHSM_Token_SubmitCommand);
	return( errCode );
}



/* This performs byte packing -- big endian */
/* 0102030401020304  --> 0403020104030201 */

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


