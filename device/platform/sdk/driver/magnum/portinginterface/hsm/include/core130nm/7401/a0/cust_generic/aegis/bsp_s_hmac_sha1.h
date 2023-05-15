/***************************************************************************
 *     Copyright (c) 2005-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsp_s_hmac_sha1.h $
 * $brcm_Revision: Aegis_Firmware_Devel/10 $
 * $brcm_Date: 3/1/06 10:55a $
 * Security Processor Code for Register Access
 ***************************************************************************/

 #ifndef BSP_S_HMAC_SHA1_H__
#define BSP_S_HMAC_SHA1_H__


/* The max size the HMAC SHA1 data length can be. */
#define BCMD_HMACSHA1_MAX_DATA_LEN 332 /* 332 bytes */

#define BPI_HMACSHA1_DIGEST_BYTE_SIZE   20


/*

BCMD_HmacSha1_ExecuteCommand - Request to calculate HMAC of SHA1 digest

Parameters passed:
eOperation - Perform either HMAC or SHA1.
eContextId - which context to perform digest calculation.
eKeyData - 160 bit start key to use if HMAC digest requested.
eDataLen - Length of data to follow that will be used to calculate
	HMAC or SHA1.
eData - Data used for calculation.
	
Returned:
eStatus - Status of command.
eDigest - 160 bit calculated digest.

BCMD_HmacSha1_InCmdField_e: 
|-------------------|-------------------|-------------------|-------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             	| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            | eOperation - 8	| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            | eContextId - 8	| 
|-------------------|-------------------|-------------------|-------------------| 
|  eKeyData - 160																| 
|-------------------|-------------------|-------------------|-------------------| 
|  																				| 
|-------------------|-------------------|-------------------|-------------------| 
|  																				| 
|-------------------|-------------------|-------------------|-------------------| 
|  																				| 
|-------------------|-------------------|-------------------|-------------------| 
|  																				| 
|-------------------|-------------------|-------------------|-------------------|
|  unused			| unused			| eDataLen - 16							|																| 
|-------------------|-------------------|-------------------|-------------------|
|  eData - eDataLen*8  																				| 
|-------------------|-------------------|-------------------|-------------------|
    
BCMD_HmacSha1_OutCmdField_e: 
|-------------------|-------------------|-------------------|-------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             	| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused			|  unused			|  unused			| eStatus -	8		|
|-------------------|-------------------|-------------------|-------------------| 
|  eDigest - 160																|
|-------------------|-------------------|-------------------|-------------------| 
|  																				| 
|-------------------|-------------------|-------------------|-------------------| 
|  																				| 
|-------------------|-------------------|-------------------|-------------------| 
|  																				| 
|-------------------|-------------------|-------------------|-------------------| 
|  																				| 
|-------------------|-------------------|-------------------|-------------------|

*/

typedef enum BCMD_HmacSha1_InCmdField_e
{
    BCMD_HmacSha1_InCmdField_eOperation     = (5<<2) + 3,   /* 8 bit index */
    BCMD_HmacSha1_InCmdField_eContextId = (6<<2) + 3,   /* 8 bit index */
    BCMD_HmacSha1_InCmdField_eKeyData     = (7<<2) + 0,   /* 160 bit index */
    BCMD_HmacSha1_InCmdField_eDataLen     = (12<<2) + 2,   /* 16 bit index */
    BCMD_HmacSha1_InCmdField_eData     = (13<<2) + 0   /* 32 bit index */
}BCMD_HmacSha1_InCmdField_e;

typedef enum BCMD_HmacSha1_OutCmdField_e
{
    BCMD_HmacSha1_OutCmdField_eStatus = (5<<2) + 3,   /* 8 bit */
    BCMD_HmacSha1_OutCmdField_eDigest = (6<<2) + 0 /* 160 bit */
}BCMD_HmacSha1_OutCmdField_e;

typedef enum BPI_HmacSha1_Op_e
{
    BPI_HmacSha1_Op_eSha1,
    BPI_HmacSha1_Op_eHmac
}BPI_HmacSha1_Op_e;

typedef enum BPI_HmacSha1_Context_e
{
    BPI_HmacSha1_Context_eCpu0 = 0x0,      /* MIPS boot region */
    BPI_HmacSha1_Context_eCpu1 = 0x1,      /* MIPS region 1 */
    BPI_HmacSha1_Context_eCpu2 = 0x2,      /* MIPS region 2 */
    BPI_HmacSha1_Context_eCpu3 = 0x3,      /* MIPS region 3 */
    BPI_HmacSha1_Context_eCpu4 = 0x4,      /* MIPS region 4 */
    BPI_HmacSha1_Context_eCpu5 = 0x5,      /* MIPS region 5 */
    BPI_HmacSha1_Context_eCpu6 = 0x6,      /* MIPS region 6 */
    BPI_HmacSha1_Context_eCpu7 = 0x7,      /* MIPS region 7 */
    BPI_HmacSha1_Context_eAudio0 = 0x8,      /* Audio region */
    BPI_HmacSha1_Context_eVideo0 = 0x9,      /* Video0 Decoder */
    BPI_HmacSha1_Context_eReserved0 = 0xA,	   /* BRCM reserved */
    BPI_HmacSha1_Context_eCmdSig = 0xB,     /* Input command signature calculation region */
    BPI_HmacSha1_Context_eReserved1 = 0xC,     /* BRCM reserved */
    BPI_HmacSha1_Context_eHmacSha1Ctx0 = 0xD,     /* HMAC_SHA1 cmd */
    BPI_HmacSha1_Context_eHmacSha1Ctx1 = 0xE,     /* HMAC_SHA1 cmd */
    BPI_HmacSha1_Context_eHmacSha1Ctx2 = 0xF,     /* HMAC_SHA1 cmd */
    BPI_HmacSha1_Context_eMax        
}BPI_HmacSha1_Context_e;

#endif /* BSP_S_HMAC_SHA1_H__ */

