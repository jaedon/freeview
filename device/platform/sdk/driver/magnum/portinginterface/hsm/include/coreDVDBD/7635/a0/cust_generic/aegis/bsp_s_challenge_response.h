/***************************************************************************
 *     Copyright (c) 2005-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsp_s_challenge_response.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 8/12/08 5:28p $
 * Security Processor Code for Register Access
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7601/a0/cust_generic/aegis/bsp_s_challenge_response.h $
 * 
 * Hydra_Software_Devel/1   8/12/08 5:28p atruong
 * PR45631: HSM PI support for BCM7601 platform
 * 
 * Aegis_Firmware_Devel/2   6/11/07 1:13p hzhu
 * PR660: pre BSECK code clean up - remove chips/customer modes defines
 * which are not applicable or unnecessary
 * 
 * Aegis_Firmware_Devel/11   9/22/06 3:16p btan
 * PR 426: Fixed the header files that shared between Mips and BSP.
 * Aegis_Firmware_Devel/1   6/10/05 11:06a dbayani
 * PR13830: Added shared enums and defines with MIPS to share directory.
 * 
 *
 ***************************************************************************/

#ifndef BSP_S_CHALLENGE_RESPONSE_H__
#define BSP_S_CHALLENGE_RESPONSE_H__

/*

BCMD_Challenge_ExecuteCommand - Request OTP ID from Aegis to determine
the valid encrypted OTP XOR nonce to send in response command.

Parameters passed:
eMethod - Whether to perform Response method 0 or 1.
    0 - legacy response.
*/    
/* 1- reserved  */
/*
eId - OTP Id to read from Aegis.

Returned:
eStatus - Status of cancel command.
eIdHi - Upper 32 bits of 64 bit OTP Id.
eIdLo - Lower 32 bits of 64 bit OTP Id.

BCMD_Challenge_InputCommandField_e: 
|-------------------|-------------------|-------------------|-------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             	| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            | eMethod - 8		| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            | eId - 8			| 
|-------------------|-------------------|-------------------|-------------------| 
    
BCMD_Challenge_OutCmdField_e: 
|-------------------|-------------------|-------------------|-------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             	| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused			|  unused			|  unused			| eStatus -	8		|
|-------------------|-------------------|-------------------|-------------------| 
|  eIdHi -	32																	|
|-------------------|-------------------|-------------------|-------------------| 
|  eIdLo -	32																	|
|-------------------|-------------------|-------------------|-------------------| 

*/
typedef enum BCMD_Challenge_InputCommandField_e
{
    BCMD_Challenge_InputCommandField_eMethod    = (5<<2) + 3,  /* 8 bit index */
    BCMD_Challenge_InputCommandField_eId        = (6<<2) + 3  /* 8 bit index */
}BCMD_Challenge_InputCommandField_e;

typedef enum BCMD_Challenge_OutCmdField_e
{
    BCMD_Challenge_OutCmdField_eStatus = (5<<2) + 3, /* 8 bit index */
    BCMD_Challenge_OutCmdField_eIdHi = (6<<2) + 0,     /* 32 bit index */
    BCMD_Challenge_OutCmdField_eIdLo = (7<<2) + 0 /* 32 bit index */
}BCMD_Challenge_OutCmdField_e;

/*

BCMD_Response_ExecuteCommand - Send encrypted OTP XOR value to Aegis
to verify valid user requesting to open test ports.

Parameters passed:
eMethod - Whether to perform Response method 0 or 1.
    0 - legacy response.
*/    
/*    1- reserved    */
/*
eOtpIdId - OTP Key ID to compare with after decrypting nonce.
eKeySrc - Key source used for key 2 generation.
eCustKeySelH - Bits[5:0] used to select cust key high for swizzle 0.
	Bit[6] used to indicate DES encryption or decryption for key 2L generation.
eCustKeySelL - - Bits[5:0] used to select cust key low for swizzle 0.
	Bit[6] used to indicate DES encryption or decryption for key 2H generation.
eKeyVarH - used to select key var high for swizzle 0.
eKeyVarL - used to select key var low for swizzle 0.
eOTPKey - 64 bit encrypted OTP XOR.
	
Returned:
eStatus - Status of command.
*/

/*

BCMD_Response_InCmdField_e: 
|-------------------|-------------------|-------------------|-------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             	| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            | eMethod - 8		| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            | eOtpId - 8		| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            | eKeySrc - 8		| 
|-------------------|-------------------|-------------------|-------------------| 
*/
/*
|  unused           | unused            | unused            | eCustKeySelH - 8	| 
*/
/*
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            | eCustKeySelL - 8	| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            | eKeyVarH - 8		| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            | eKeyVarL - 8		| 
|-------------------|-------------------|-------------------|-------------------| 
|  eOTPKey - 64																	| 
|-------------------|-------------------|-------------------|-------------------|
|																				| 
|-------------------|-------------------|-------------------|-------------------|
    
BCMD_Response_OutCmdField_e: 
|-------------------|-------------------|-------------------|-------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             	| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused			|  unused			|  unused			| eStatus -	8		|
|-------------------|-------------------|-------------------|-------------------| 
*/
typedef enum BCMD_Response_InCmdField_e
{
    BCMD_Response_InCmdField_eMethod        = (5<<2) + 3,   /* 8 bit index */
    BCMD_Response_InCmdField_eOtpId         = (6<<2) + 3,   /* 8 bit index */
    BCMD_Response_InCmdField_eKeySrc        = (7<<2) + 3,   /* 8 bit index */
    BCMD_Response_InCmdField_eCustKeySelH   = (8<<2) + 3,   /* 8 bit index */
    BCMD_Response_InCmdField_eReserved0         = (8<<2) + 0,   /* 128 bit index */ 
    BCMD_Response_InCmdField_eCustKeySelL   = (9<<2) + 3,    /* 8 bit index */
    BCMD_Response_InCmdField_eKeyVarH       = (10<<2) + 3,  /* 8 bit index */
    BCMD_Response_InCmdField_eKeyVarL       = (11<<2) + 3,  /* 8 bit index */
    BCMD_Response_InCmdField_eOTPKey        = (12<<2) + 0   /* 64 bit index */
}BCMD_Response_InCmdField_e;

typedef enum BCMD_Response_OutCmdField_e
{
    BCMD_Response_OutCmdField_eStatus = (5<<2) + 3, /* 8 bit */
    BCMD_Response_OutCmdField_eReserved0 = (6<<2) + 0   /* 128 bit index */ 
}BCMD_Response_OutCmdField_e;

/* BAMCM_USER_RSA packet operations */
typedef enum BCMD_Response_Method_e
{
    BCMD_Response_Method_eLegacy,
    BCMD_Response_Method_eReserved  = 1
}BCMD_Response_Method_e;


#endif /* BSP_S_CHALLENGE_RESPONSE_H__ */
