/***************************************************************************
 *     Copyright (c) 2005-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhsm_docsis.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 9/5/06 6:29p $
 *
 * Module Description: This file contains Broadcom Host Secure Module (OS/platform 
 *                     independent) porting interface public functions.
 *                    			                    
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7118/A0/docsis/bhsm_docsis.c $
 * 
 * Hydra_Software_Devel/1   9/5/06 6:29p dbayani
 * PR 24090: Add for docsis support.
 *
 *
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"

#include "bxpt.h"
#include "bhsm_datatypes.h"

/* From Aegis */
#include "bsp_s_commands.h"

#include "bhsm_datatypes.h"
#include "bhsm_priv.h"
#include "bhsm.h"
#include "bhsm_docsis.h"


BDBG_MODULE(BHSM);

/* #define BDBG_MSG(x) printf x */


BERR_Code BHSM_SetDocsisMipsBoot (
		BHSM_Handle				in_handle,
		BHSM_DocsisIO_t	*inoutp_DocsisIO
)
{
    BHSM_P_CommandData_t inoutp_commandData;
	BHSM_ChannelHandle channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];    
    uint32_t ulParamLen = 0;	
	BERR_Code errCode = BERR_SUCCESS;

	BDBG_MSG(("Inside BHSM_SetDocsisMipsBoot\n"));	
	BDBG_ENTER(BHSM_SetDocsisMipsBoot);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	inoutp_commandData.cmdId = BCMD_cmdType_eDOCSIS_MIPS_BOOT;
	inoutp_commandData.unContMode = CONT_MODE_ALL_DATA;
	
	inoutp_commandData.unInputParamsBuf[ulParamLen/4] = inoutp_DocsisIO->unW1Mask;
	ulParamLen += 4;
	
	inoutp_commandData.unInputParamsBuf[ulParamLen/4] = inoutp_DocsisIO->unW1BaseIn;
	ulParamLen += 4;
	
	inoutp_commandData.unInputParamsBuf[ulParamLen/4] = inoutp_DocsisIO->unW1BaseOut;
	ulParamLen += 4;
	
	inoutp_commandData.unInputParamsBuf[ulParamLen/4] = inoutp_DocsisIO->unW2Mask;
	ulParamLen += 4;
	
	inoutp_commandData.unInputParamsBuf[ulParamLen/4] = inoutp_DocsisIO->unW2BaseIn;
	ulParamLen += 4;
	
	inoutp_commandData.unInputParamsBuf[ulParamLen/4] = inoutp_DocsisIO->unW2BaseOut;
	ulParamLen += 4;
		
	inoutp_commandData.unInputParamLen = ulParamLen;       

	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
		BHSM_P_CommonSubmitCommand (channelHandle, 
			&inoutp_commandData));		

	ulParamLen = 0;  
	
	inoutp_DocsisIO->unStatus =  inoutp_commandData.unOutputParamsBuf[ulParamLen/4]; 
	
	ulParamLen += 4;

	/* Check status */
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(0 != inoutp_DocsisIO->unStatus  ) );			
		
	/* Check output paramter length */
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_PARM_LEN_ERR, 
		(ulParamLen != inoutp_commandData.unOutputParamLen  ) );	
		
BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_SetDocsisMipsBoot);
	return( errCode );
}

