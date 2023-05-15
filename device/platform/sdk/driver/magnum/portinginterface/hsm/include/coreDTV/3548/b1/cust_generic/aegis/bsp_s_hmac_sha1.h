/***************************************************************************
 *     Copyright (c) 2005-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsp_s_hmac_sha1.h $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 6/8/07 1:40p $
 * Security Processor Code for Register Access
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7400/a0/cust_generic/aegis/bsp_s_hmac_sha1.h $
 * 
 * Hydra_Software_Devel/6   6/8/07 1:40p lyang
 * PR 27711: HSM Purge and shared *.h source udpated for 7400A0,
 * 7401B0/C1/C2/C3
 * 
 * Aegis_Firmware_Devel/19   10/19/06 4:49p dbayani
 * PR 430: Fixed assignment of hmacsha1 contexts to regions.
 * 
 * Aegis_Firmware_Devel/18   9/18/06 11:18a dbayani
 * PR 397: Seperated chip defines so if the chip is not defined, it would
 * cause an error message.
 * 
 * Aegis_Firmware_Devel/17   9/13/06 2:11p dbayani
 * PR 387: For 7400B0 support.
 * 
 * Aegis_Firmware_Devel/16   9/8/06 2:44p whou
 * PR397: add BCM7400B0 support.
 * 
 * Aegis_Firmware_Devel/15   8/25/06 3:31p dbayani
 * PR 387: Added BRCM_IF statements.
 * 
 * Aegis_Firmware_Devel/14   7/12/06 12:19p dbayani
 * PR 167: Remove 7400A0 compile error.
 * 
 * Aegis_Firmware_Devel/13   6/26/06 7:25p dbayani
 * PR 358: Permit use of key5a as HMAC key source in generic mode.
 * 
 * Aegis_Firmware_Devel/12   6/7/06 3:08p dbayani
 * PR 352: Added signature verification for RAVE code.
 * 
 * Aegis_Firmware_Devel/11   4/6/06 12:04p dbayani
 * PR324: Added SICH to region verification.
 * 
 * Aegis_Firmware_Devel/10   3/1/06 10:55a dbayani
 * PR277: Remove 7440A0 build errors.
 * 
 * Aegis_Firmware_Devel/9   2/3/06 6:30p dbayani
 * PR232, PR236: Made updates to build for 7401B0 and 7118A0.
 * 
 * Aegis_Firmware_Devel/8   1/6/06 3:04p dbayani
 * PR255: Fixed bug introduced when syncing aegis and host files.
 * 
 * Aegis_Firmware_Devel/7   1/6/06 1:49a btan
 * PR 255: Update shared files to sync up both Aegis and Host sides.
 * 
 * Aegis_Firmware_Devel/6   10/10/05 11:02a dbayani
 * PR167: Updated to support 7400 with addition of second video checker.
 * 
 * Aegis_Firmware_Devel/5   9/19/05 6:53p btan
 * PR 185: Updated the shared file for host compilation
 * 
 * Aegis_Firmware_Devel/4   8/22/05 8:22a dbayani
 * PR23: Added description of each host command.
 * 
 * Aegis_Firmware_Devel/3   7/11/05 10:04a dbayani
 * PR42: Changed comment for different builds regarding HMAC_SHA1 context
 * 11.
 * 
 * Aegis_Firmware_Devel/2   6/14/05 5:19p dbayani
 * PR13830: Coding Conventions update,
 * 
 * Aegis_Firmware_Devel/1   6/10/05 11:07a dbayani
 * PR13830: Added shared enums and defines with MIPS to share directory.
 * 
 *
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
    BPI_HmacSha1_Op_eHmac,
    BPI_HmacSha1_Op_eHmac_KeySrcReserve1    /*this is only for MIPS Apps, will be translated by MIPS, BSP doesn't see this*/    
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

    BPI_HmacSha1_Context_eVideo1 = 0xA,	   /* Video1 Decoder */
    BPI_HmacSha1_Context_eReserved1 = 0xB,     /* BRCM reserved */
    BPI_HmacSha1_Context_eCmdSig = 0xB,     /* Input command signature calculation region */
    BPI_HmacSha1_Context_eReserved2 = 0xC,     /* BRCM reserved */
    BPI_HmacSha1_Context_eHmacSha1Ctx0 = 0xD,     /* HMAC_SHA1 cmd */
    BPI_HmacSha1_Context_eHmacSha1Ctx1 = 0xE,     /* HMAC_SHA1 cmd */    
    BPI_HmacSha1_Context_eHmacSha1Ctx2 = 0xF,     /* HMAC_SHA1 cmd */ 
    

    BPI_HmacSha1_Context_eMax        
}BPI_HmacSha1_Context_e;

#endif /* BSP_S_HMAC_SHA1_H__ */

