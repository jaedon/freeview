/***************************************************************************
 *     Copyright (c) 2005-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhsm_otpmsp.h $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 12/19/07 2:15p $
 *
 * Module Description: 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7440/c0/otpmsp/bhsm_otpmsp.h $
 * 
 * Hydra_Software_Devel/5   12/19/07 2:15p qxie
 * PR 38365: HSM: add data section programming interface with CRC
 * 
 * Hydra_Software_Devel/4   10/15/07 11:16a lyang
 * PR 27763: HSM to add 7440C0 B2 support
 * 
 * Hydra_Software_Devel/3   3/7/07 2:22p lyang
 * PR 27763: HSM to add a new PI to support 7440B0
 * 
 * Hydra_Software_Devel/2   3/5/07 2:22p lyang
 * PR 27763: HSM to add a new PI to support 7440B0
 * 
 * Hydra_Software_Devel/8   12/14/06 8:15p lyang
 * PR 24864: HSM: Add HSM support for 7403, PR 25689: add 7403 support to
 * HSM/SCD
 * 
 * Hydra_Software_Devel/7   12/1/06 10:39p lyang
 * PR 25188: HSM, add function header, data struct header description for
 * those public APIs
 * 
 * Hydra_Software_Devel/6   11/28/06 11:29a btan
 * PR 26037: Added BHSM_UnwrapMultipleContentKeys(),
 * BHSM_UpdateSecretKey() and BHSM_CalculateMediaKey() to support
 * GenerateRouteContentKeys and MKB processing commands.
 * 
 * Hydra_Software_Devel/5   10/27/06 11:35a lyang
 * PR 23412: remove HSM_PI_FUNC, my tool should be compatible with the
 * current API Reporter. PR 25133: remove HSM_PI_FUNC, my tool should be
 * compatible with the current API Reporter
 * 
 * Hydra_Software_Devel/4   10/26/06 4:16p lyang
 * PR 23412: hsm pi include files should be re-factored so they are not
 * order dependent. PR 25133:HSM header files should include dependencies
 * 
 * Hydra_Software_Devel/3   9/26/06 3:12p btan
 * PR 23906: Converted BCHP_REV to BCHP_VER.
 * 
 * Hydra_Software_Devel/2   9/7/06 4:56p lyang
 * PR 24145: for easy ReadMSP cmd which requires authentication
 * 
 * Hydra_Software_Devel/1   4/26/06 5:46p btan
 * PR 20446: HSM needs to compile for 7400 A0
 * 
 *
 ***************************************************************************/

#ifndef BHSM_OTPMSP_H__
#define BHSM_OTPMSP_H__

#include "bsp_s_version_number.h"

#include "bhsm_priv.h"

/* added to remove header file dependency chain*/
#if (BCHP_CHIP==7401) &&  (BCHP_VER == BCHP_VER_A0)
#include "bsp_s_otp_7401.h"
#elif (BCHP_CHIP==7401) &&  (BCHP_VER >= BCHP_VER_B0)
#include "bsp_s_otp_7401B0.h"
#elif (BCHP_CHIP==7400) &&  (BCHP_VER == BCHP_VER_A0)
#include "bsp_s_otp_7400A0.h"
#elif (BCHP_CHIP==7400) &&  (BCHP_VER == BCHP_VER_B0)
#include "bsp_s_otp_7400B0.h"
#elif (BCHP_CHIP==7400) &&  (BCHP_VER == BCHP_VER_C0)   /* 7400C0 =B0 after the confirmation*/
#include "bsp_s_otp_7400B0.h"
#elif (BCHP_CHIP==7400) &&  (BCHP_VER == BCHP_VER_D0)   /* 7400C0 =B0 after the confirmation*/
#include "bsp_s_otp_7400D0.h"


#elif (BCHP_CHIP==7118) &&  (BCHP_VER >= BCHP_VER_A0)  && (BCHP_VER <= BCHP_VER_B0)  /* 7118A1=A0 */
#include "bsp_s_otp_7118A0.h"

#elif (BCHP_CHIP==7440) &&  (BCHP_VER < BCHP_VER_B0)
#include "bsp_s_otp_7440A0.h"

#elif (BCHP_CHIP==7440) &&  (BCHP_VER >= BCHP_VER_B0)
#include "bsp_s_otp_7440B0.h"

#elif (BCHP_CHIP==7403) &&  (BCHP_VER >= BCHP_VER_A0) && (BCHP_VER <= BCHP_VER_A1)  /* 7403A1=A0 */
#include "bsp_s_otp_7403A0.h"

#elif (BCHP_CHIP==7405) &&  (BCHP_VER >= BCHP_VER_A0) && (BCHP_VER <= BCHP_VER_A1)
#include "bsp_s_otp_7405A0.h"

#elif (BCHP_CHIP==7325) &&  (BCHP_VER >= BCHP_VER_A0)  
#include "bsp_s_otp_7325A0.h"

#elif (BCHP_CHIP==7335) &&  (BCHP_VER >= BCHP_VER_A0)  
#include "bsp_s_otp_7335A0.h"


#else
#error "unknown chip"
#endif


#include "bhsm.h"


#ifdef __cplusplus
extern "C" {
#endif

#ifdef BSP_BROADCOM_ONLY

#define BHSM_OTP_MC0_DATA_LEN	    (((BCMD_Otp_InCmdOfflineProgOtp_eMc0ValueReserved/4)*4) - ((BCMD_Otp_InCmdOfflineProgOtp_eMc0Value/4)*4))
#define BHSM_OTP_MC0_RESERVED_DATA_LEN	    (((BCMD_Otp_InCmdOfflineProgOtp_e14x26ArrayKeyId/4)*4) - ((BCMD_Otp_InCmdOfflineProgOtp_eMc0ValueReserved/4)*4))
#define BHSM_OTP_KEY_ID_DATA_LEN	    (((BCMD_Otp_InCmdOfflineProgOtp_e14x26ArrayKey/4)*4) - ((BCMD_Otp_InCmdOfflineProgOtp_e14x26ArrayKeyId/4)*4))
#define BHSM_OTP_KEY_DATA_LEN	    (((BCMD_Otp_InCmdOfflineProgOtp_e14x26ArrayCrc/4)*4) - ((BCMD_Otp_InCmdOfflineProgOtp_e14x26ArrayKey/4)*4))
#define BHSM_OTP_CRC_LEN	    8

typedef struct BHSM_ProgramOtpIO {

	/* in:   	reserved	 */
	BCMD_Otp_CmdProgMode_e	progMode;
	
	/* in: reserved*/
	BPI_Otp_HalProgKey_e		progKeyEnum;
	
	/* in: reserved*/
	unsigned char				aucMC0Data[BHSM_OTP_MC0_DATA_LEN]; /* include 10-bit ECC */
	
	/* in:  reserved*/
	unsigned char				aucKeyId[BHSM_OTP_KEY_ID_DATA_LEN]; /* include 10-bit ECC */

	/* in:  reserved*/
	unsigned char				aucKeyData[BHSM_OTP_KEY_DATA_LEN]; /* include 10-bit ECC */

	/* in:  reserved*/
	unsigned char				aucCRC[BHSM_OTP_CRC_LEN]; /* include 10-bit ECC */


	/* out:  0 for success, otherwise failed */
	uint32_t					unStatus;
	
} BHSM_ProgramOtpIO_t;


/*****************************************************************************
Summary:

This function is reserved for Broadcom internal usage only. 
 The programming tool is strictly controlled.

See Also:
BHSM_ProgramMSP

*****************************************************************************/
BERR_Code      BHSM_ProgramOTP (
		BHSM_Handle			in_handle,
		BHSM_ProgramOtpIO_t	*inoutp_progOtpIO
);
#endif

#define  BHSM_OTP_KEY_ID_LEN ((BCMD_Otp_OutCmdOfflineOtpRead_eRegValueHi /4) * 4 - (BCMD_CommonBufferFields_eStatus /4) * 4 )

/**************************************************************************************************
Summary:

Description:
Structure that defines which OTP field to read and holds the returned value of that OTP field.

See Also:
BHSM_ReadOTP
**************************************************************************************************/		
typedef struct BHSM_ReadOtpIO {

	/* In:  which OTP field to read */
	BPI_Otp_CmdReadRegister_e		readOtpEnum;			

	/* Out: 0 for success, otherwise failed */
	uint32_t						unStatus;

	/* Out:  returned OTP field, big endian byte array, not necessary to hold Key ID*/
	unsigned char					aucKeyId[BHSM_OTP_KEY_ID_LEN];  
	
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
		BHSM_ReadOtpIO_t		*inoutp_readOtpIO
);

#define BHSM_MSP_DATA_LEN	4

typedef enum BCMD_Msp_CmdProgMode_e 
{ 
    BCMD_Msp_CmdProgMode_eEnum    = 0x12,    

    BCMD_Msp_CmdProgMode_eReserved   /* 3 others reserved, don't use this for MSP programming*/
} BCMD_Msp_CmdProgMode_e ; 


/**************************************************************************************************
Summary:

Description:
Structure that defines which MSP field to program, with what data,  using what mask and proper mode,  and holds the returned status
of a programming request

See Also:
BHSM_ProgramMSP
**************************************************************************************************/
typedef struct BHSM_ProgramMspIO {

	/* in: This field should contain the value of BCMD_OTP_PROGMSP_MODE (i.e., 0x00000012) 
	for the bits to be programmed. This is a sanity check on the command.
	BSP_BROADCOM_ONLY:The LSB (0x12) specifies that command enum mode programming is used.
	*/
	BCMD_Msp_CmdProgMode_e progMode;
	
	/* in: specifies which MSP bits to program. The values this field can take are specified by the 
	typedef enum BPI_Otp_CmdMsp_e in the share C header file.  Each chip will have different enums 
	and customers will only have access to the files for the chips that they use.  
	*/
	BCMD_Otp_CmdMsp_e		progMspEnum;
	
	/* in: number of bits of the MSP enum to program, from 1 to 32*/
	unsigned char				ucDataBitLen;  
	
	/* in :0x0000_0001 to 0xFFFF_FFFF   
		A value 1 in a bit position specifies that the data value at that bit position is to be programmed. 
		BSP will not program bits that have a zero bit in the mask.
			For example 0x0F means to program the 4 LSBits of an enum.  
			For example 0x8F means to program the bit 7 and 4 LSBits of an enum.

	*/
	unsigned char				aucDataBitMask [BHSM_MSP_DATA_LEN]; 
	
	/* in: the value that needs to be programmed*/
	unsigned char				aucMspData[BHSM_MSP_DATA_LEN]; 

	/* Out: MSP programming command PASSed/0, otherwise FAILed. */
	uint32_t					unStatus;
	
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

#define BHSM_MSP_KEY3_DATA_LEN	    (((BCMD_Otp_InCmdOfflineMspRead_eReserved5/4)*4) - ((BCMD_Otp_InCmdOfflineMspRead_eReserved4/4)*4))
#define BHSM_MSP_KEY4_DATA_LEN	     (((BCMD_Otp_InCmdOfflineMspRead_eReserved6/4)*4) - ((BCMD_Otp_InCmdOfflineMspRead_eReserved5/4)*4))
#define BHSM_MSP_SIGNATURE_DATA_LEN	16
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
	BCMD_Otp_CmdMsp_e		readMspEnum;
	
	/* In:  ask BSP do the command authentication for this MSP read, true/yes, false/no
	  only if readOtpEnum == BCMD_Otp_CmdMsp_eOtpReserved32, we requires to check signature
	*/
    	bool						bCheckSignature;	
	
	/* In: only when bCheckSignaure is true,  must be set correctly*/
	unsigned char 			ucCustKeyHigh;   

	/* In: only when bCheckSignaure is true,  must be set correctly*/
	unsigned char 			ucCustKeyLow; 	

	/* In: only when bCheckSignaure is true,  must be set correctly*/
	unsigned char 			ucKeyVarHigh;	

	/* In: only when bCheckSignaure is true,  must be set correctly*/
	unsigned char 			ucKeyVarLow;	
	
	/* In: only when bCheckSignaure is true,  must be set correctly, big endian */
	unsigned char 			aucProcInForKey3[BHSM_MSP_KEY3_DATA_LEN]; 

	/* In: only when bCheckSignaure is true,  must be set correctly, big endian */
	unsigned char 			aucProcInForKey4[BHSM_MSP_KEY4_DATA_LEN];	

	/* In: only when bCheckSignaure is true,  must be set correctly, big endian */
	unsigned char 			aucSignature[BHSM_MSP_SIGNATURE_DATA_LEN];	

	/* Out: 0 for success, otherwise failed */
	uint32_t					unStatus;
	
	/* Out: the return MSP field*/
	unsigned char				aucMspData[BHSM_MSP_OUTPUT_DATA_LEN];   

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


/**************************************************************************************************
Summary:

Description:
Structure that defines which holds the returned status of each programmed MC0 OTP field and Security Assurance Logic status.

See Also:
BHSM_GetStatus
**************************************************************************************************/

typedef struct BHSM_GetStatusIO {
	
	/* Out: read Pass or failure*/
	uint32_t			unStatus;			
	
	/* Out: returned status of Security Assurance Logic, only LSB byte valid*/
	uint32_t			unSALStatus;            
	/* Out: returned register content of BSP_OTP_SIGNAL_READ, only LSB byte valid */
	uint32_t			unSignalReadStatus;	

	/* Out:returned register content of BSP_OTP_FLT_STAT, only LSB byte valid */
	uint32_t			unOtpFltStatus;		

	/*Out: returned register content of BSP_OTP_FLT_STAT_KEYA, only LSB byte valid */
	uint32_t			unOtpAFltStatus;		

	/*Out: returned register content of BSP_OTP_FLT_STAT_KEYB, only LSB byte valid */
	uint32_t			unOtpBFltStatus;		

	/*Out: returned register content of BSP_OTP_FLT_STAT_KEYC, only LSB byte valid */
	uint32_t			unOtpCFltStatus;		
	
	/* Out: returned register content of BSP_OTP_FLT_STAT_DS, only LSB byte valid */
	uint32_t			unOtpDsFltStatus;	


#if (BCHP_CHIP!=7401) ||   (BCHP_VER != BCHP_VER_A0)
	/* Out: returned Broadcom only value, defined in bsp_s_otp.h*/
	uint32_t			unReserved1Status;  

	/* Out: returned Broadcom only value, defined in bsp_s_otp.h*/
	uint32_t			unReserved2Status;	

	/* Out: returned Broadcom only value, defined in bsp_s_otp.h*/
	uint32_t			unReserved3Status;	

	/* Out: returned Broadcom only value, defined in bsp_s_otp.h*/
	uint32_t			unReserved4Status;	

	/* Out: returned Broadcom only value, defined in bsp_s_otp.h*/
	uint32_t			unReserved5Status;	

	/* Out: returned Broadcom only value, defined in bsp_s_otp.h*/
	uint32_t			unReserved6Status;	

	/* Out: returned Broadcom only value, defined in bsp_s_otp.h*/
	uint32_t			unReserved7Status;	
#endif

} BHSM_GetStatusIO_t;


/*****************************************************************************
Summary:

This function returns status of each programmed MC0 OTP field and Security Assurance Logic status.

Description:

This function is used to read status of each programmed MC0 OTP field and Security Assurance Logic
status. The interpretation of each output result is reserved for internal usage.


Calling Context:

It can be called anytime after HSM is initialized.


Performance and Timing:
This is a synchronous/blocking function that would not return until it is done or failed. 

Input:
in_handle  - BHSM_Handle, Host Secure module handle.
inoutp_getStatusIO  - BHSM_GetStatusIO_t.
			
Output:
inoutp_getStatusIO  - BHSM_GetStatusIO_t.

Returns:
BERR_SUCCESS - success
BHSM_STATUS_FAILED - failure

See Also:
BHSM_ReadOTP
*****************************************************************************/
BERR_Code      BHSM_GetStatus (
		BHSM_Handle			in_handle,
		BHSM_GetStatusIO_t	*inoutp_getStatusIO
);




#define BHSM_READ_DATA_SECTION_DATA_LEN	    (32)		/* in byte now, may change to word32*/

/**************************************************************************************************
Summary:

Description:
Structure that defines which OTP data section to read, and will hold the returned the data from that section

See Also:
BHSM_ReadDataSect
**************************************************************************************************/
typedef struct BHSM_ReadDataSectIO {

	/* In:  which OTP data section to read, between BPI_Otp_DataSection_e0 ~ BPI_Otp_DataSection_e7*/
	BPI_Otp_DataSection_e		readDsEnum;

	/* Out: 0 for success, otherwise failed */
	uint32_t					unStatus;

	/* Out: the returned 32 bytes from the OTP data section as specified */
	unsigned char				aucDataSectData[BHSM_READ_DATA_SECTION_DATA_LEN]; 

} BHSM_ReadDataSectIO_t;

/*****************************************************************************
Summary:

This function returns one 32-byte data section value.

Description:

There are total of 8 32-byte data sections. This function shall be used to read each 32-byte data section.


Calling Context:

This function can be called anytime after HSM is initialized.


Performance and Timing:
This is a synchronous/blocking function that would not return until it is done or failed. 

Input:
in_handle  - BHSM_Handle, Host Secure module handle.
inoutp_readDataSectIO  - BHSM_ReadDataSectIO_t

			
Output:
inoutp_readDataSectIO  - BHSM_ReadDataSectIO_t


See Also:
BHSM_ProgramDataSect
*****************************************************************************/
BERR_Code      BHSM_ReadDataSect (
		BHSM_Handle				in_handle,
		BHSM_ReadDataSectIO_t	*inoutp_readDataSectIO
);



#define BHSM_WRITE_DATA_SECTION_DATA_LEN	     (((56/4)*4) - ((24/4)*4))

/**************************************************************************************************
Summary:

Description:
Structure that defines which OTP data section to program with what data using a proper mode, and holds 
the returned status

See Also:
BHSM_ProgramDataSect
**************************************************************************************************/
typedef struct BHSM_ProgramDataSectIO {

	/* In: select which OTP data section to program, between BPI_Otp_DataSection_e0 ~
			BPI_Otp_DataSection_e7 */
	BPI_Otp_DataSection_e		progDsEnum;

	/* In: provide the actual 32-byte data to be programmed into the specified OTP data section*/
	unsigned char				aucDataSectData[BHSM_WRITE_DATA_SECTION_DATA_LEN]; 

	/* In: a kind of program magic number, must be BCMD_OTP_DATASECTIONPROG_MODE (0x00010112).
			if it is not this value the command will be rejected by  a sanity check at BSP
	 */
	uint32_t					unMode;
	uint32_t                               unCrc;

	/* Out: 0 for success, otherwise failed  */
	uint32_t					unStatus;

} BHSM_ProgramDataSectIO_t;

/*****************************************************************************
Summary:

This function is used to programm one 32-byte data section value.

Description:

There are total of 8 32-byte data sections. This function shall be used to write one 32-byte data
section.


Calling Context:

This function can be called anytime after HSM is initialized.


Performance and Timing:
This is a synchronous/blocking function that would not return until it is done or failed. 

Input:
in_handle  - BHSM_Handle, Host Secure module handle.
inoutp_progDataSectIO  - BHSM_ProgramDataSectIO_t

			
Output:
inoutp_progDataSectIO  - BHSM_ProgramDataSectIO_t

See Also:
BHSM_ReadDataSect
*****************************************************************************/
BERR_Code BHSM_ProgramDataSect (
		BHSM_Handle				in_handle,
		BHSM_ProgramDataSectIO_t 	*inoutp_progDataSectIO
);



#ifdef BSP_BROADCOM_ONLY
typedef struct BHSM_ReloadOtpIO {

	/* In:  no dedicated input parameter needed */

	/* Out: 0 for success, otherwise failed */
	uint32_t					unStatus;
} BHSM_ReloadOtpIO_t;

/*****************************************************************************
Summary:

This function is reserved for Broadcom internal usage only.  
The programming tool is strictly controlled.

See Also:
BHSM_ProgramOTP
*****************************************************************************/
BERR_Code      BHSM_ReloadOTP (
		BHSM_Handle			in_handle,
		BHSM_ReloadOtpIO_t	*inoutp_reloadOtpIO
);
#endif



/* End of Module Specific Functions */


#ifdef __cplusplus
}
#endif

#endif /* BHSM_OTPMSP_H__ */







