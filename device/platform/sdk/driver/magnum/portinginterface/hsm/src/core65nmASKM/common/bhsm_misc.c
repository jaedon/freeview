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
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 12/9/09 4:30p $
 *
 * Module Description: This file contains Broadcom Host Secure Module (OS/platform 
 *                     independent) porting interface public functions.
 *                    			                    
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7420/b0/common/bhsm_misc.c $
 * 
 * Hydra_Software_Devel/5   12/9/09 4:30p atruong
 * SW7468-16: HSM PI Support For 7468A0
 * 
 * Hydra_Software_Devel/4   8/20/09 7:19p atruong
 * PR55169: HSM PI Support For 7420B0/7340A0/7342A0
 * PR55544: HSM PI Support For 7125A0
 * 
 * Hydra_Software_Devel/3   8/11/09 3:48p atruong
 * PR55231: HSM PI Support For 7340A0
 * 
 * Hydra_Software_Devel/2   8/5/09 10:41a atruong
 * PR55231: HSM PI Support for BCM7342A0
 * 
 * Hydra_Software_Devel/1   8/5/09 7:32a atruong
 * PR55169: HSM PI Support for 7420B0
 * 
 * Hydra_Software_Devel/1   6/29/09 5:38p atruong
 * PR55169: HSM PI Support for 7420A1
 * 
 * Hydra_Software_Devel/22   12/5/08 8:33p atruong
 * PR50045: HSM PI Support for BCM7420 A0
 * 
 * Hydra_Software_Devel/21   11/13/08 2:44p atruong
 * PR48869: Support HSM PI Raw Commands for BSP/Aegis
 * 
 * Hydra_Software_Devel/20   10/8/08 5:20p atruong
 * PR46656: Support for 3548 B0 - Bug fixes
 * 
 * Hydra_Software_Devel/19   10/3/08 11:49a atruong
 * PR46656: HSM PI Support for 3548 Rev B0
 * 
 * Hydra_Software_Devel/18   10/3/08 8:51a atruong
 * PR46656: HSM PI Support for Rev B0
 * 
 * Hydra_Software_Devel/18   10/3/08 1:17a atruong
 * PR46656: HSM PI Support for Rev B0
 * 
 * Hydra_Software_Devel/17   8/21/08 7:38p dbayani
 * PR 45985: Remove errors and warnings.
 * 
 * Hydra_Software_Devel/16   4/11/08 10:16a atruong
 * PR38948: HSM PI support for BCM93548
 * 
 * Hydra_Software_Devel/15   4/9/08 11:44a atruong
 * PR38948: HSM PI support for BCM3548
 * PR39464: HSM PI support for BCM3556
 * 
 * Hydra_Software_Devel/14   3/5/08 3:12p ptimariu
 * PR40084: add archID to SetArchIO
 * 
 * Hydra_Software_Devel/13   10/25/07 12:34p lyang
 * PR36043: Modify makefile and split the code to allow HSM to build
 * without source covered by Security SLA
 * 
 * Hydra_Software_Devel/12   10/15/07 3:40p lyang
 * PR 35021: HSM bringup on 7325, part of 7335 added also
 * 
 * Hydra_Software_Devel/11   8/3/07 2:46p lyang
 * PR 33325: add 7118B0 support and link
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



/* From Aegis */


#include "bhsm_misc.h"



BDBG_MODULE(BHSM);

#ifdef USER_MODE_BHSM_DEBUG
#define BDBG_MSG(x) printf x 
#define BDBG_ERR(x) printf x
#endif 



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
	      		  		(inoutp_setMiscBitsIO->bDisableClear << 4) |
			        (inoutp_setMiscBitsIO->bEnableEncBypass << 3) |
	      			  (inoutp_setMiscBitsIO->bEnableReadDMem << 2) |
					(inoutp_setMiscBitsIO->bEnableReadIMem << 1) |
					inoutp_setMiscBitsIO->bEnableWriteIMem;
			break;
			
		case 1: 		/*BCMD_SetMiscBitsSubCmd_eReserved1: eliminate the purge failure, will put it back later when the purge is stable 8/3/07 */
			commandData.unInputParamsBuf[unParamLen/4] =
	      			  ( (inoutp_setMiscBitsIO->bSetMiscReservedBit2 & 0x1)<< 2) |
					( (inoutp_setMiscBitsIO->bSetMiscReservedBit1 & 0x1) << 1) |
					(inoutp_setMiscBitsIO->bSetMiscReservedBit0 & 0x1);
			break;

		case 2:		/* BCMD_SetMiscBitsSubCmd_eReserved2:  */
			commandData.unInputParamsBuf[unParamLen/4] =
	      			  ( (inoutp_setMiscBitsIO->bSetMiscReservedBit2 & 0x1)<< 2) |
					( (inoutp_setMiscBitsIO->bSetMiscReservedBit1 & 0x1) << 1) |
					(inoutp_setMiscBitsIO->bSetMiscReservedBit0 & 0x1);
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

	inoutp_setMiscBitsIO->ucRaveStatus =  commandData.unOutputParamsBuf[unParamLen/4];
	unParamLen +=  4;

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

#if ((BCHP_CHIP==7420) && (BCHP_VER >= BCHP_VER_B0)) || (BCHP_CHIP == 7340) || \
    (BCHP_CHIP == 7342) || (BCHP_CHIP == 7125) || (BCHP_CHIP == 7468)
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(BHSM_ProgOTPPatternSeq(in_handle) != BERR_SUCCESS ) );	
#endif

	BKNI_Memset(&commandData, 0, sizeof(BHSM_P_CommandData_t));

	commandData.cmdId      = 0x19;         /* For OTP Read operation */
	commandData.unContMode = 0;
	
	/* only fill the command specific input parameters */
	commandData.unInputParamsBuf[0] = inoutp_readOTPIdIO->OtpId;
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








