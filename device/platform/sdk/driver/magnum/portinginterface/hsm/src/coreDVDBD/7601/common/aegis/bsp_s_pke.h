/***************************************************************************
 *     Copyright (c) 2005-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsp_s_pke.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 8/12/08 4:42p $
 * Security Processor Code for Register Access
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7601/a0/common/aegis/bsp_s_pke.h $
 * 
 * Hydra_Software_Devel/1   8/12/08 4:42p atruong
 * PR45631: HSM PI support for BCM7601 platform
 * 
 * Aegis_Firmware_Devel/6   12/13/05 4:01p dbayani
 * PR231: Added command for PKE Hal test.  This command will only be
 * available during debug builds.
 * 
 * Aegis_Firmware_Devel/5   9/20/05 4:31p dbayani
 * PR23: Changes based on code review.
 * 
 * Aegis_Firmware_Devel/4   9/19/05 6:56p btan
 * PR 185: Updated the shared file for host compilation
 * 
 * Aegis_Firmware_Devel/3   8/22/05 8:22a dbayani
 * PR23: Added description of each host command.
 * 
 * Aegis_Firmware_Devel/2   6/14/05 5:20p dbayani
 * PR13830: Coding Conventions update,
 * 
 * Aegis_Firmware_Devel/1   6/10/05 11:07a dbayani
 * PR13830: Added shared enums and defines with MIPS to share directory.
 * 
 *
 ***************************************************************************/

#ifndef BSP_S_PKE_H__
#define BSP_S_PKE_H__


/* Max size of data length for each type of packet. */
#define BCMD_PKE_DH_MAX_DATA_LENGTH     344
#define BCMD_PKE_DSA_MAX_DATA_LENGTH    336
#define BCMD_PKE_RSA_MAX_DATA_LENGTH    344


/* Common status field for all PKE output buffers. */
typedef enum BCMD_PKE_CommonOutFields_e
{
    BCMD_PKE_CommonOutFields_eStatus = (5<<2) + 3   /* 8 bit index */
}BCMD_PKE_CommonOutFields_e;

/*

BCMD_PKE_ExecuteDH - Perform DH public or secret key generate.
Public Key Generate:
public key = g^x mod p
Secret Key Generate:
Shared secret key = (public key from other party)x mod p

Parameters passed:
eOp - perform public or secret key generation or pass through
	mode (send eData directly to public key engine).
eModSrcIdx - index to key buffer containing modulus p.
eBaseSrcIdx - index to key buffer containing base g for public key
	generate or index to store secret key (instead of storing secret
	key to buffer, it can also be returned through output buffer).
ePrivateKeySrcIdx - index to private key x.
eDataLen - length of data contained in eData.  For public key
	generate, no data is passed.  For secret key, the public key
	of other host is passed through here.
eData - pass through data or public key of other party.
	
Returned:
eStatus - Status of command.
eKeyLen - Length of key in bits.
eKeyBase - public key or private key.

BCMD_PKE_DHInputCmdField_e: 
|-------------------|-------------------|-------------------|-------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             	| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            | eOp - 8			| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            | eModSrcIdx - 8	| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            | eBaseSrcdx - 8	| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            |ePrivateKeySrcIdx-8| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | eDataLen - 16							| 
|-------------------|-------------------|-------------------|-------------------| 
|  eData - eDataLen*8															| 
|-------------------|-------------------|-------------------|-------------------| 

    
BCMD_PKE_DHOutCmdField_e: 
|-------------------|-------------------|-------------------|-------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             	| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused			|  unused			|  unused			| eStatus -	8		|
|-------------------|-------------------|-------------------|-------------------| 
|  unused			|  unused			|  eKeyLen - 16							|
|-------------------|-------------------|-------------------|-------------------| 
|  eKeyBase - eKeyLen															|
|-------------------|-------------------|-------------------|-------------------| 

*/
typedef enum BCMD_PKE_DHInputCmdField_e
{
    BCMD_PKE_DHInputCmdField_eOp            = (5<<2) + 3,   /* 8 bit index */
    BCMD_PKE_DHInputCmdField_eModSrcIdx          = (6<<2) + 3,   /* 8 bit index */
    BCMD_PKE_DHInputCmdField_eBaseSrcIdx         = (7<<2) + 3,   /* 8 bit index */
    BCMD_PKE_DHInputCmdField_ePrivateKeySrcIdx  = (8<<2) + 3,   /* 8 bit index */
#if defined(BSP_DEBUG_PKE_HAL_UNIT_TEST)
    BCMD_PKE_DHInputCmdField_eMask         = (7<<2) + 3,   /* 8 bit index */
    BCMD_PKE_DHInputCmdField_eMaskedValue  = (8<<2) + 3,   /* 8 bit index */
#endif    
    BCMD_PKE_DHInputCmdField_eDataLen          = (9<<2) + 2,   /* 16 bit index */
    BCMD_PKE_DHInputCmdField_eData            = (10<<2) + 0   /* 32 bit index */
}BCMD_PKE_DHInputCmdField_e;

typedef enum BCMD_PKE_DHOutCmdField_e
{
    BCMD_PKE_DHOutCmdField_eKeyLen = (6<<2) + 2,   /* 16 bit */
    BCMD_PKE_DHOutCmdField_eKeyBase = (7<<2) + 0   /* 32 bit */
}BCMD_PKE_DHOutCmdField_e;

/*

BCMD_PKE_ExecuteDSA - Perform DSA sign or verify
Sign:
r = (g^k mod p) mod q
s = (k^-1(h + xr)) mod q
Verify:
w = s^-1 mod q
u1 = ((hw) mod q
u2 = (rw) mod q
v = ((g^(u1)y^(u2)) mod p) mod q

Parameters passed:
eOp - sign, verify, or pass through mode.  In pass through mode, the data from
	eData is sent directly to the public key engine).
ePKeySrc - index to public key P.
eGKeySrc - index to public key G.
eQKeySrc - index to public key Q.
ePrivKeyX - index to private key X.
ePrivKeyK - index to private key K.
eDataLen - Length of hash for sign or length of public key y
	hash, and r,s when verify.
eData - Data.
	
Returned:
eStatus - Status of command.
eData - (r,s) for sign or v for verify

BCMD_PKE_DSAInCmdField_e: 
|-------------------|-------------------|-------------------|-------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             	| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            | eOp - 8			| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            | ePKeySrc - 8		| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            | eGKeySrc - 8		| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            | eQKeySrc - 8		| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            | ePrivKeyX - 8		| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            | ePrivKeyK - 8		| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused			| unused			| eDataLen - 16							|																	| 
|-------------------|-------------------|-------------------|-------------------|
| eData	- eDataLen*8															|
|-------------------|-------------------|-------------------|-------------------|
    
BCMD_PKE_DSAOutCmdField_e: 
|-------------------|-------------------|-------------------|-------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             	| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused			|  unused			|  unused			| eStatus -	8		|
|-------------------|-------------------|-------------------|-------------------| 
|  eData - 320 (r,s) or 160 (v)															|
|-------------------|-------------------|-------------------|-------------------| 

*/
typedef enum BCMD_PKE_DSAInCmdField_e
{
    BCMD_PKE_DSAInCmdField_eOp       = (5<<2) + 3,   /* 8 bit index */
    BCMD_PKE_DSAInCmdField_ePKeySrc       = (6<<2) + 3,   /* 8 bit index */
    BCMD_PKE_DSAInCmdField_eGKeySrc       = (7<<2) + 3,   /* 8 bit index */
    BCMD_PKE_DSAInCmdField_eQKeySrc       = (8<<2) + 3,   /* 8 bit index */
    BCMD_PKE_DSAInCmdField_ePrivKeyX   = (9<<2) + 3,   /* 8 bit index */
    BCMD_PKE_DSAInCmdField_ePrivKeyK   = (10<<2) + 3,   /* 8 bit index */
    BCMD_PKE_DSAInCmdField_eDataLen        = (11<<2) + 2,   /* 16 bit index */
    BCMD_PKE_DSAInCmdField_eData       = (12<<2) + 0    /* 32 bit index */
}BCMD_PKE_DSAInCmdField_e;

typedef enum BCMD_PKE_DSAOutCmdField_e
{
    BCMD_PKE_DSAOutCmdField_eData   = (6<<2) + 0    /* 32 bit index */
}BCMD_PKE_DSAOutCmdField_e;

/*

BCMD_PKE_ExecuteRSA - Perform encrpyt and decrypt.
Normal:
cleartext = c^e mod n.
Chinese Remainder Theorem:
v1 = md mod (p-1) mod p
v2 = md mod (q-1) mod q

C2 = p-1 mod q
u = (v2 - v1) C2 mod q
v1 + up = md mod n

Parameters passed:
eOp - normal, crt, or pass through (data is
	sent directly to public key engine).
eKeySrcIdx1 - index for source n (decrypt) or p and q (encrypt).
eExpSrcIdx - index for exponent e (decrypt) or pinv (encrypt).
eKeySrcIdx2 - index for dp and dq (encrypt).
eDataLen - length of clear text, cipher text, or pass through data.
eData - clear text, cipher text, or pass through data.
	
Returned:
eStatus - Status of command.
eDataLen - length of clear or cipher in bits.
eData - clear or cipher.

BCMD_PKE_RSAInCmdField_e: 
|-------------------|-------------------|-------------------|-------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             	| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            | eOp - 8			| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            | eKeySrcIdx1 - 8	| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            | eExpSrcIdx - 8	| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            | eKeySrcIdx2 - 8	| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused			| unused			| eDataLen - 16							|																	| 
|-------------------|-------------------|-------------------|-------------------|
| eData	- eDataLen*8															|
|-------------------|-------------------|-------------------|-------------------|
    
BCMD_PKE_RSAOutCmdField_e: 
|-------------------|-------------------|-------------------|-------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             	| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused			|  unused			|  unused			| eStatus -	8		|
|-------------------|-------------------|-------------------|-------------------| 
|  unused			|  unused			|  eDataLen - 16						|
|-------------------|-------------------|-------------------|-------------------| 
|  eData - eDataLen																|
|-------------------|-------------------|-------------------|-------------------| 

*/
typedef enum BCMD_PKE_RSAInCmdField_e
{
    BCMD_PKE_RSAInCmdField_eOp   = (5<<2) + 3,   /* 8 bit index */
    BCMD_PKE_RSAInCmdField_eKeySrcIdx1= (6<<2) + 3,   /* 8 bit index */
    BCMD_PKE_RSAInCmdField_eExpSrcIdx = (7<<2) + 3,   /* 8 bit index */
    BCMD_PKE_RSAInCmdField_eKeySrcIdx2= (8<<2) + 3,   /* 8 bit index */
    BCMD_PKE_RSAInCmdField_eDataLen = (9<<2) + 2,   /* 16 bit index */
    BCMD_PKE_RSAInCmdField_eData   = (10<<2) + 0   /* 32 bit index */
}BCMD_PKE_RSAInCmdField_e;

typedef enum BCMD_PKE_RSAOutCmdField_e
{
    BCMD_PKE_RSAOutCmdField_eDataLen   = (6<<2) + 2,   /* 16 bit index */
    BCMD_PKE_RSAOutCmdField_eData   = (7<<2) + 0   /* 32 bit index */
}BCMD_PKE_RSAOutCmdField_e;

/* PKE opcodes */
typedef enum BPI_PKE_Opcode_e
{
    BPI_PKE_Opcode_eDHPublicKeyGen = 1,
    BPI_PKE_Opcode_eDHPrivateKeyGen,
    BPI_PKE_Opcode_eRSANormal,
    BPI_PKE_Opcode_eRSACrt,
    BPI_PKE_Opcode_eDSASign,
    BPI_PKE_Opcode_eDSAVerify
}BPI_PKE_Opcode_e;

/* BAMCM_USER_DH packet operations */
typedef enum BPI_PKE_DHOperation_e
{
    BPI_PKE_DHOperation_ePublicKeyGen,
    BPI_PKE_DHOperation_eSecretKeyGen
}BPI_PKE_DHOperation_e;

/* BAMCM_USER_RSA packet operations */
typedef enum BPI_PKE_RSAOperation_e
{
    BPI_PKE_RSAOperation_eNormal,
    BPI_PKE_RSAOperation_eCrt
}BPI_PKE_RSAOperation_e;

/* BAMCM_USER_DSA packet operations */
typedef enum BPI_PKE_DSAOperation_e
{
    BPI_PKE_DSAOperation_ePrivateKeySign,
    BPI_PKE_DSAOperation_ePublicKeyVer
}BPI_PKE_DSAOperation_e;

/* PKE Specific command field definitions */
#define BPI_PKE_PASS_THROUGH	0xFF

#endif /* BSP_S_PKE_H__ */

