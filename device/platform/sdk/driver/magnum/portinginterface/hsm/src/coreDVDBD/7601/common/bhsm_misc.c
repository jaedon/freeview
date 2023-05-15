/***************************************************************************
 *     Copyright (c) 2005-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhsm_misc.c $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 9/15/09 12:26a $
 *
 * Module Description: This file contains Broadcom Host Secure Module (OS/platform 
 *                     independent) porting interface public functions.
 *                    			                    
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7601/a0/common/bhsm_misc.c $
 * 
 * Hydra_Software_Devel/6   9/15/09 12:26a atruong
 * SW7630-18: HSM PI Support for 7630B0
 * 
 * Hydra_Software_Devel/5   4/23/09 10:25a atruong
 * PR54377: Remove length checking for output parameters
 * 
 * Hydra_Software_Devel/4   3/30/09 12:25p atruong
 * PR52979: Support for 7635A0
 * 
 * Hydra_Software_Devel/3   12/4/08 1:03p atruong
 * PR49290: HSM PI Support for 7601B0
 * 
 * Hydra_Software_Devel/2   8/25/08 10:15p atruong
 * PR45631: BCM7601A0 Bring up
 * 
 * Hydra_Software_Devel/1   8/12/08 5:01p atruong
 * PR45631: HSM PI support for BCM7601 platform
 * 
 * Hydra_Software_Devel/2   3/26/07 11:54a lyang
 * PR 28967: to add HSM support for 7440B0 Non-OS
 * 
 * Hydra_Software_Devel/10   12/4/06 10:29a lyang
 * PR 25470: add new PIs DBG_DUMP, SET_PCI_MEM_WIN, SET_VICH
 * 
 * Hydra_Software_Devel/9   10/26/06 4:19p lyang
 * PR 23412: hsm pi include files should be re-factored so they are not
 * order dependent. PR 25133:HSM header files should include dependencies
 * 
 * Hydra_Software_Devel/8   10/17/06 6:40p lyang
 * PR 24980: HSM: MIPS & BSP have different versionID for certian
 * chip/version
 * 
 * Hydra_Software_Devel/7   10/11/06 6:28p lyang
 * PR 24073: Update HSM PI code for 7401C0 (add DTVBits, ICAMBits)
 * 
 * Hydra_Software_Devel/6   10/4/06 4:58p lyang
 * PR 24073: HSM: Update HSM PI code for 7401C0 (add DTVBits, ICAMBits)
 * 
 * Hydra_Software_Devel/5   9/26/06 3:07p btan
 * PR 23906: Converted BCHP_REV to BCHP_VER.
 * 
 * Hydra_Software_Devel/4   9/19/06 3:01p btan
 * PR 24370: replaced BHSM_SetRaveIO with BHSM_SetMiscBitsIO
 * 
 * Hydra_Software_Devel/3   9/5/06 5:41p dbayani
 * PR 24089: Update HSM RAVE enable PI.
 * 
 * Hydra_Software_Devel/2   8/4/06 12:16a btan
 * PR22187: Sync with 7401C0 BSP header files.
 * 
 * Hydra_Software_Devel/1   2/28/06 8:21p btan
 * PR 19931: Added HSM support for 7400A0
 * 
 * Hydra_Software_Devel/1   1/20/06 5:14p btan
 * PR 19140: Do not encrypt generic mode files.
 * 
 * Hydra_Software_Devel/4   1/17/06 9:32p btan
 * PR 19140: Do not encrypt generic mode files.
 *
 *
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"




/* #include "bxpt.h"  ly 03/23/07 for non-os */


/* From Aegis */


#include "bhsm_misc.h"



BDBG_MODULE(BHSM);

/* #define BDBG_MSG(x) printf x */


BERR_Code BHSM_ReadScArray (
		BHSM_Handle				in_handle,
		BHSM_ReadScArrayIO_t		*inoutp_readScArrayIO
)
{
	BERR_Code errCode = BERR_SUCCESS;
	BHSM_ChannelHandle channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t unParamLen = 0, i, j;
	BHSM_P_CommandData_t commandData;	

	BDBG_MSG(("Inside BHSM_ReadScArray\n"));
	BDBG_ENTER(BHSM_ReadScArray);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	


	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	commandData.cmdId = BCMD_cmdType_eUSER_SC_ARRAY_STATUS;
	commandData.unContMode = 0;
	
	/* only fill the command specific input parameters */
	/* No input command parameter */

	
	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

	/* Parse the command specific output parameters */
	inoutp_readScArrayIO->unStatus =  commandData.unOutputParamsBuf[0];
	unParamLen +=  4;

	/* DigitalTV has 4 word32 returned,*/
	for (i=0, j=1; i< 16; i++, j++) {
		inoutp_readScArrayIO->unScArrayStatus[i] =  
				commandData.unOutputParamsBuf[j];
		unParamLen +=  4;
	}
	
 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_readScArrayIO->unStatus != 0) );
	
BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_ReadScArray);
	return( errCode );
}

BERR_Code BHSM_SetMiscBits (
		BHSM_Handle			in_handle,
		BHSM_SetMiscBitsIO_t		*inoutp_setMiscBitsIO
)
{
	BERR_Code errCode = BERR_SUCCESS;
	BHSM_ChannelHandle channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t unParamLen = 0;
	BHSM_P_CommandData_t commandData;	

	BDBG_MSG(("Inside BHSM_SetMiscBits\n"));
	BDBG_ENTER(BHSM_SetMiscBits);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	commandData.cmdId = BCMD_cmdType_eOFFLINE_RAVE_COMMAND;
	commandData.unContMode = 0;
	
	/* only fill the command specific input parameters */

	commandData.unInputParamsBuf[unParamLen/4] =  inoutp_setMiscBitsIO->setMiscBitsSubCmd;
	unParamLen += 4;

	switch (inoutp_setMiscBitsIO->setMiscBitsSubCmd){
		
		case BCMD_SetMiscBitsSubCmd_eRaveBits:
			
			commandData.unInputParamsBuf[unParamLen/4] =
#if (BCHP_CHIP!=7400) ||  (BCHP_VER != BCHP_VER_A0)		
	      		  		(inoutp_setMiscBitsIO->bDisableClear << 4) |
#endif	        
			        (inoutp_setMiscBitsIO->bEnableEncBypass << 3) |
	      			  (inoutp_setMiscBitsIO->bEnableReadDMem << 2) |
					(inoutp_setMiscBitsIO->bEnableReadIMem << 1) |
					inoutp_setMiscBitsIO->bEnableWriteIMem;
			break;
			
#if defined (BCM7400A0) || defined(BCM7401B0)|| defined(BCM7401C0)||defined(BCM7118A0)


		case BCMD_SetMiscBitsSubCmd_eReserved1:
			commandData.unInputParamsBuf[unParamLen/4] =
	      			  ( (inoutp_setMiscBitsIO->bSetMiscReservedBit2 & 0x1)<< 2) |
					( (inoutp_setMiscBitsIO->bSetMiscReservedBit1 & 0x1) << 1) |
					(inoutp_setMiscBitsIO->bSetMiscReservedBit0 & 0x1);
			break;

		case BCMD_SetMiscBitsSubCmd_eReserved2:
			commandData.unInputParamsBuf[unParamLen/4] =
	      			  ( (inoutp_setMiscBitsIO->bSetMiscReservedBit2 & 0x1)<< 2) |
					( (inoutp_setMiscBitsIO->bSetMiscReservedBit1 & 0x1) << 1) |
					(inoutp_setMiscBitsIO->bSetMiscReservedBit0 & 0x1);
			break;
		
#endif
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

#if (BCHP_CHIP!=7400) ||  (BCHP_VER != BCHP_VER_A0)
	inoutp_setMiscBitsIO->ucRaveStatus =  commandData.unOutputParamsBuf[unParamLen/4];
	unParamLen +=  4;
#endif	
	
BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_SetMiscBits);
	return( errCode );
}

BERR_Code BHSM_SetArch (
		BHSM_Handle			in_handle,
		BHSM_SetArchIO_t		*inoutp_setArchIO
)
{
	BERR_Code errCode = BERR_SUCCESS;
	BHSM_ChannelHandle channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t unParamLen = 0;
	BHSM_P_CommandData_t commandData;	

	BDBG_MSG(("Inside BHSM_SetArch\n"));
	BDBG_ENTER(BHSM_SetArch);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	


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
    			
	commandData.unInputParamLen = unParamLen;    
	
	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

    unParamLen += 4;    
					
	/* Parse the command specific output parameters */
	inoutp_setArchIO->unStatus =  commandData.unOutputParamsBuf[unParamLen/4];
	unParamLen +=  4;
	
 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_setArchIO->unStatus != 0) );
BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_SetArch);
	return( errCode );
}


/************************************************************************
     what chip/version can support the following new PIs, 
     TODO: add the condition. BCMxxxxyy, xxxx - chip number, yy -version number, all upper case
                e.g. #if 
************************************************************************/
#if defined(BCM7401A0) || defined(BCM7601A0) || defined(BCM7601B0) || defined(BCM7635A0) || defined(BCM7630B0)

#else
BERR_Code  BHSM_SetVichRegPar (
		BHSM_Handle					in_handle,
		BHSM_SetVichRegParIO_t		*inoutp_setVichIO
)
{

	BERR_Code 			errCode = BERR_SUCCESS;
	BHSM_ChannelHandle 	channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t 				unParamLen = 0;
	BHSM_P_CommandData_t commandData;	

	BDBG_MSG(("Inside BHSM_SetVichRegPar\n"));
	BDBG_ENTER(BHSM_SetVichRegPar);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	
	
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 	
		(inoutp_setVichIO->whichVich >=BCMD_SetVichRegPar_Max )	);

	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	commandData.cmdId = BCMD_cmdType_eOFFLINE_SET_VICH_REG_PAR;
	commandData.unContMode = 0;
	
	/* only fill the command specific input parameters */
	commandData.unInputParamsBuf[BCMD_SetVichRegParField_InCmdField_eVichId/4 - BCMD_CommonBufferFields_eStatus/4 ] 
							= 	inoutp_setVichIO->whichVich;
	
	commandData.unInputParamsBuf[BCMD_SetVichRegParField_InCmdField_eRegPar0Start/4 - BCMD_CommonBufferFields_eStatus/4 ] 
		                                = 	inoutp_setVichIO->unRange0Start;
	

	commandData.unInputParamsBuf[BCMD_SetVichRegParField_InCmdField_eRegPar0End/4 - BCMD_CommonBufferFields_eStatus/4 ] 
		                                = 	inoutp_setVichIO->unRange0End;

	commandData.unInputParamsBuf[BCMD_SetVichRegParField_InCmdField_eRegPar1Start/4 - BCMD_CommonBufferFields_eStatus/4 ] 
		                                = 	inoutp_setVichIO->unRange0Start;
	

	commandData.unInputParamsBuf[BCMD_SetVichRegParField_InCmdField_eRegPar1End/4 - BCMD_CommonBufferFields_eStatus/4 ] 
		                                = 	inoutp_setVichIO->unRange0End;
    
    			
	commandData.unInputParamLen = (BCMD_SetVichRegParField_InCmdField_eRegPar1End/4 - BCMD_CommonBufferFields_eStatus/4)*4+4 ;    
	
	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

    unParamLen =0;    
					
	/* Parse the command specific output parameters */
	inoutp_setVichIO->unStatus =  commandData.unOutputParamsBuf[unParamLen/4];
	unParamLen +=  4;
	
 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_setVichIO->unStatus != 0) );
BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_SetVichRegPar);
	return( errCode );


}




BERR_Code  BHSM_DebugDumpExc (
		BHSM_Handle					in_handle,
		BHSM_DebugDumpExcIO_t		*inoutp_debugDumpExcIO
)
{
	BERR_Code 			errCode = BERR_SUCCESS;
	BHSM_ChannelHandle 	channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t 				unParamLen = 0, i, j;
	BHSM_P_CommandData_t commandData;	

	BDBG_MSG(("Inside BHSM_DebugDumpExc\n"));
	BDBG_ENTER(BHSM_DebugDumpExc);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	


	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	commandData.cmdId = BCMD_cmdType_eDEBUG_DUMP_EXC;
	commandData.unContMode = 0;
	
	/* only fill the command specific input parameters */
	/* No input command parameter */

	
	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

	/* Parse the command specific output parameters */
	i=0;
	inoutp_debugDumpExcIO->unStatus =  commandData.unOutputParamsBuf[i++];


	if (i*4 < commandData.unOutputParamLen) 	
		inoutp_debugDumpExcIO->unResetStatusReg = commandData.unOutputParamsBuf[i++];


	if (i*4 < commandData.unOutputParamLen) 	
		inoutp_debugDumpExcIO->unExceptionStatus= commandData.unOutputParamsBuf[i++];


	if (i*4 < commandData.unOutputParamLen) 	
		inoutp_debugDumpExcIO->unProgramCounter= commandData.unOutputParamsBuf[i++];

	for (j=0; j< BSP_EXPT_CAUSE_STICKY_WORD32_NUM; j++)
	{
		if (i*4 < commandData.unOutputParamLen) 
		{	
				inoutp_debugDumpExcIO->unStickyWord[j] = commandData.unOutputParamsBuf[i++];
		}else{ break;}
	}
		
	unParamLen = i*4;	  /* actual length*/

	/* Check output paramter length */
	if ( unParamLen < sizeof(BHSM_DebugDumpExcIO_t) ){
		BDBG_MSG(("\t Warning: dumped data is shorter than expected"));   /* allow this so far for some flexibility*/
	}else if (commandData.unOutputParamLen> sizeof(BHSM_DebugDumpExcIO_t))
	{
		BDBG_MSG(("\t Warning: dumped data is longer than expected, the extra is not interpreted"));  
	}else{}

	
 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_debugDumpExcIO->unStatus != 0) );
	
BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_DebugDumpExc);
	return( errCode );
}




BERR_Code  BHSM_SetPciMemWin (
		BHSM_Handle					in_handle,
		BCMD_SetPciMemWinIO_t		*inoutp_setPciMemWinIO
)
{
	BERR_Code 			errCode = BERR_SUCCESS;
	BHSM_ChannelHandle 	channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
	uint32_t 				unParamLen = 0;
	BHSM_P_CommandData_t commandData;	

	BDBG_MSG(("Inside BCMD_SetPciMemWin\n"));
	BDBG_ENTER(BCMD_SetPciMemWin);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
								(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	/*more strict check is preferred once when the infor is available, e.g. High bigger than Low? */
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_INPUT_PARM_ERR, 	
		(inoutp_setPciMemWinIO->unAddrRangeHigh > 0x7FFFFFF || 
		inoutp_setPciMemWinIO->unAddrRangeLow > 0x7FFFFFF)	);         /* accoring to hongbo feedback, bit26:0 mask*/

	
	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	commandData.cmdId = BCMD_cmdType_eOFFLINE_SetPCIMemWin;
	commandData.unContMode = 0;
	
	/* only fill the command specific input parameters */
	commandData.unInputParamsBuf[BCMD_MISC_SetPCI_MEMWIN_InCmdField_eSlot/4 - BCMD_CommonBufferFields_eStatus/4 ] 
							= 	inoutp_setPciMemWinIO->whichWin;
	
	commandData.unInputParamsBuf[BCMD_MISC_SetPCI_MEMWIN_InCmdField_eAddrRangeLow/4 - BCMD_CommonBufferFields_eStatus/4 ] 
		                                = 	inoutp_setPciMemWinIO->unAddrRangeLow;
	
	commandData.unInputParamsBuf[BCMD_MISC_SetPCI_MEMWIN_InCmdField_eAddrRangeHigh/4 - BCMD_CommonBufferFields_eStatus/4 ] 
		                                = 	inoutp_setPciMemWinIO->unAddrRangeHigh;


	commandData.unInputParamLen = (BCMD_MISC_SetPCI_MEMWIN_InCmdField_eAddrRangeHigh/4 - BCMD_CommonBufferFields_eStatus/4)*4+4 ;    
	
	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
					&commandData));

    	unParamLen =0;    
					
	/* Parse the command specific output parameters */
	inoutp_setPciMemWinIO->unStatus =  commandData.unOutputParamsBuf[unParamLen/4];
	unParamLen +=  4;
	
 	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(inoutp_setPciMemWinIO->unStatus != 0) );
BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BCMD_SetPciMemWin);
	return( errCode );


}



#endif



