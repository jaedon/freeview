/***************************************************************************
 *     Copyright (c) 2005-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhsm_otpmsp.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 12/17/10 5:57p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7550/a0/otpmsp/bhsm_otpmsp.h $
 * 
 * Hydra_Software_Devel/1   12/17/10 5:57p rpereira
 * SWCFE-400: adding files
 *
 *
 ***************************************************************************/

#ifndef BHSM_OTPMSP_H__
#define BHSM_OTPMSP_H__

#include "bhsm_priv.h"

#include "bhsm.h"


#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
Summary:

Description:
Structure that defines which OTP field to read and holds the returned value of that OTP field.

See Also:
BHSM_ReadOTP
**************************************************************************************************/
typedef struct BHSM_ReadOtpIO {

	/* In:  which OTP field to read */
	uint32_t		readOtpEnum;

	/* Out: 0 for success, otherwise failed */
	uint32_t		unStatus;

	/* Out:  returned OTP field, big endian byte array, not necessary to hold Key ID*/
	unsigned long	otpID[2];

} BHSM_ReadOtpIO_t;


/*****************************************************************************
Summary:

This function returns one OTP key identifiers or one MC0 OTP value.

Description:

This function shall be used to read either OTP key identifier or other OTP field value.
Depends on the access control matrix, only certain OTP fields can be read in a specific
customer mode.  Note that this function can only read back one OTP value at a time.

Calling Context:

This function can be called anytime after HSM is initialized.


Performance and Timing:
This is a synchronous/blocking function that would not return until it is done or failed.


Input:
in_handle  - BHSM_Handle, Host Secure module handle.
inoutp_readOtpIO  - BHSM_ReadOtpIO_t.


Output:
inoutp_readOtpIO  - BHSM_ReadOtpIO_t.


Returns:
BERR_SUCCESS - success
BHSM_STATUS_FAILED - failure

See Also:
BHSM_ReadMSP

*****************************************************************************/
BERR_Code      BHSM_ReadOTP (
		BHSM_Handle			in_handle,
		BHSM_ReadOtpIO_t	*inoutp_readOtpIO
);

/**************************************************************************************************
Summary:

Description:
Structure that defines which MSP field to program, with what data,  using what mask and proper mode,  and holds the returned status
of a programming request

See Also:
BHSM_ProgramMSP
**************************************************************************************************/
typedef struct BHSM_ProgramMspIO {
	/* in: specifies which MSP bits to program. The values this field can take are specified by the
	typedef enum BPI_Otp_CmdMsp_e in the share C header file.  Each chip will have different enums
	and customers will only have access to the files for the chips that they use.
	*/
	uint32_t			progMspEnum;

	/* in: number of bits of the MSP enum to program, from 1 to 32*/
	unsigned char		ucDataBitLen;

	/* MSP Data to be programmed */
	uint32_t			ucMspData;
} BHSM_ProgramMspIO_t;


/*****************************************************************************
Summary:

This function allows the programming of each of the field programmable
OTP (MSP) bits.

Description:
This function allows the programming of each of the field programmable
OTP (MSP) bits.  Based on the Access Control Matrix (ACL), programming of the
bit is allowed or disallowed.

Calling Context:
This function can be called anytime after HSM is initialized.

Performance and Timing:
This is a synchronous/blocking function that would not return until it is done or failed.


Input:
in_handle  - BHSM_Handle, Host Secure module handle.
inoutp_progMspIO  - BHSM_ProgramMspIO_t.

Output:
inoutp_progMspIO  - BHSM_ProgramMspIO_t.

Returns:
BERR_SUCCESS - success
BHSM_STATUS_FAILED - failure

See Also:
BHSM_ProgramOTP

*****************************************************************************/
BERR_Code      BHSM_ProgramMSP (
		BHSM_Handle				in_handle,
		BHSM_ProgramMspIO_t		*inoutp_progMspIO
);

#define BHSM_MSP_OUTPUT_DATA_LEN	    4

/**************************************************************************************************
Summary:

Description:
Structure that defines which MSP field to read and its required key/data if command authentication is needed by BSP,
and holds the returned value of the MSP field

See Also:
BHSM_ReadMSP
**************************************************************************************************/
typedef struct BHSM_ReadMspIO {

	/* In: which MSP to read */
	uint32_t			readMspEnum;

	/* in: number of bits of the MSP enum to program, from 1 to 32*/
	unsigned char		ucDataBitLen;

	/* Out: the return MSP field*/
	uint32_t			ucMspData;

	/* Out: 0 for success, otherwise failed */
	uint32_t			unStatus;

} BHSM_ReadMspIO_t;


/*****************************************************************************
Summary:

This function returns one MSP value.

Description:

This function shall be used to read MSP field value. Depends on the access control matrix, only
certain MSP fields can be read in a specific customer mode.  Note that this function can only
read back one MSP value at a time.

Calling Context:

This function can be called anytime after HSM is initialized.


Performance and Timing:
This is a synchronous/blocking function that would not return until it is done or failed.

Input:
in_handle  - BHSM_Handle, Host Secure module handle.
inoutp_readMspIO  - BHSM_ReadMspIO_t.


Output:
inoutp_readMspIO  - BHSM_ReadMspIO_t.


Returns:
BERR_SUCCESS - success
BHSM_STATUS_FAILED - failure

See Also:
BHSM_ReadOTP

*****************************************************************************/
BERR_Code      BHSM_ReadMSP (
		BHSM_Handle				in_handle,
		BHSM_ReadMspIO_t		*inoutp_readMspIO
);

/* End of Module Specific Functions */

#ifdef __cplusplus
}
#endif

#endif /* BHSM_OTPMSP_H__ */







