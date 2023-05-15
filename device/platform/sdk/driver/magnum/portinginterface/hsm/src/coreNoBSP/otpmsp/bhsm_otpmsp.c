/***************************************************************************
 *     Copyright (c) 2005-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhsm_otpmsp.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 3/11/11 5:14p $
 *
 * Module Description: This file contains Broadcom Host Secure Module (OS/platform
 *                     independent) porting interface public functions.
 *
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7550/a1/otpmsp/bhsm_otpmsp.c $
 * 
 * Hydra_Software_Devel/2   3/11/11 5:14p rpereira
 * SW7550-707: Checking in 7550 HSM code
 *
 * Hydra_Software_Devel/1   12/17/10 5:59p rpereira
 * SWCFE-400: adding file
 *
 * Hydra_Software_Devel/15   10/15/07 3:40p lyang
 * PR 35021: HSM bringup on 7325, part of 7335 added also
 *
 * Hydra_Software_Devel/14   8/2/07 11:22a lyang
 * PR 33325: add 7118B0 support and link
 *
 * Hydra_Software_Devel/13   3/26/07 4:43p dbayani
 * PR 29072: Added support for OTP MSP programming.
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

#include "bhsm_otpmsp.h"

#include "bstd.h"
#include "bxpt.h"
#include "bhsm_7550_otp.h"

BDBG_MODULE(BHSM);

/* #define BDBG_MSG(x) printf x */

BERR_Code BHSM_ReadOTP (
		BHSM_Handle			in_handle,
		BHSM_ReadOtpIO_t	*inoutp_readOtpIO
)
{
	BERR_Code errCode = BERR_SUCCESS;
	unsigned long	otpID[2];

	BDBG_MSG(("Inside BHSM_ReadOTP\n"));
	BDBG_ENTER(BHSM_ReadOTP);
	BDBG_ASSERT( in_handle );

	read_OTP_ID(in_handle, otpID);

	inoutp_readOtpIO->otpID[0] = otpID[0];
	inoutp_readOtpIO->otpID[1] = otpID[1];

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

	unsigned char			i;

	BDBG_MSG(("Inside BHSM_ProgramMSP\n"));
	BDBG_ENTER(BHSM_ProgramMSP);
	BDBG_ASSERT( in_handle );

	BDBG_MSG(("progMspEnum =%x", inoutp_progMspIO->progMspEnum));

	errCode = BPI_Otp_ProgramMspEnum(in_handle,inoutp_progMspIO->progMspEnum, inoutp_progMspIO->ucDataBitLen, inoutp_progMspIO->ucMspData);


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
	unsigned long	regVal;

	BDBG_MSG(("Inside BHSM_ReadMSP\n"));
	BDBG_ENTER(BHSM_ReadMSP);
	BDBG_ASSERT( in_handle );

	read_MSP(in_handle,  &(inoutp_readMspIO->ucMspData), inoutp_readMspIO->readMspEnum, inoutp_readMspIO->ucDataBitLen);

BHSM_P_DONE_LABEL:

	BDBG_LEAVE(BHSM_ReadMSP);
	return( errCode );
}


