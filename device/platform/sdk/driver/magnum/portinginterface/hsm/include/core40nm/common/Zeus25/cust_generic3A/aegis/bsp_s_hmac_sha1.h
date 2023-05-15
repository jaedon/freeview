/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsp_s_hmac_sha1.h $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 4/2/12 9:43a $
 * Security Processor Code for Register Access
 ***************************************************************************/

#ifndef BSP_S_HMAC_SHA1_H__
#define BSP_S_HMAC_SHA1_H__

/*
BCMD_HmacSha1_ExecuteCommand - Request to calculate HMAC or SHA digest

Parameters passed:
#if defined(BSP_GENERIC)
NOTE: Only GeneralPurpose_2 Key4 is allowed by the ACL
eVKL 		- if performing HMAC, which VKL to use
eKeyLayer 	- if performing HMAC, which KeyLayer Key to use.
#endif
eOperation 	- Perform either HMAC or SHA1.
eContextId 	- which context to perform digest calculation. Only UserHmacSha0/1 contexts are allowed.
eShaType 	- Sha160, Sha224 or Sha256
eIsAddress	- MUST be 1. Support for Hmac-Sha data from command buffer has been made obsolete. This
			  field is there to provide backward compatibility 
eKeyData 	- 160 bits, 224 bits or 256 bits start key to use if HMAC digest requested
           	  depending on if SHA160, SHA224 or SHA256 needs to be performed
eDataLen 	- Length of data for HMAC or SHA
eAddress	- Address in DRAM0 from where to fetch the data
	
Returned:
eStatus - Status of command.
eDigest - 160 bits, 224 bits or 256 bits of calculated digest.

BCMD_HmacSha1_InCmdField_e: 
|-------------------|-------------------|-------------------|-------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             	| 
|-------------------|-------------------|-------------------|-------------------| 
#if defined(BSP_GENERIC)
|  unused           | eVKL - 8          | eKeyLayer - 8     | eOperation - 8	| 
#else
|  unused           | unused            | unused            | eOperation - 8	| 
#endif
|-------------------|-------------------|-------------------|-------------------| 
|  eIncludeKey - 8  | eIsAddress - 8    | eShaType - 8      | eContextId - 8	| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            | eKeyLen - 8       |
|-------------------|-------------------|-------------------|-------------------| 
|  eKeyData - 160 bits (SHA160), 224 bits (SHA224) or 256 bits (SHA256)         | 
|-------------------|-------------------|-------------------|-------------------| 
|  																				| 
|-------------------|-------------------|-------------------|-------------------| 
|  																				| 
|-------------------|-------------------|-------------------|-------------------| 
|  																				| 
|-------------------|-------------------|-------------------|-------------------| 
|  																				| 
|-------------------|-------------------|-------------------|-------------------|
|                               eDataLen - 32							        | 
|-------------------|-------------------|-------------------|-------------------|
|  eAddress - 32      (if eIsAddress == 1)                                      |
|-------------------|-------------------|-------------------|-------------------|
    
BCMD_HmacSha1_OutCmdField_e: 
|-------------------|-------------------|-------------------|-------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             	| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused			|  unused			|  unused			| eStatus -	8		|
|-------------------|-------------------|-------------------|-------------------| 
|  eDigest - 160 bits (SHA160), 224 bits (SHA224) or 256 bits (SHA256)          |
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
    BCMD_HmacSha1_InCmdField_eVKL           = (5<<2) + 1,    /* 8 bit index */
    BCMD_HmacSha1_InCmdField_eKeyLayer      = (5<<2) + 2,    /* 8 bit index */
    BCMD_HmacSha1_InCmdField_eOperation     = (5<<2) + 3,    /* 8 bit index */
#if defined(USER_SHA_KEY_INCLUDE)
    BCMD_HmacSha1_InCmdField_eIncludeKey    = (6<<2) + 0,    /* 8 bit index */ 
#endif /* USER_SHA_KEY_INCLUDE */
    BCMD_HmacSha1_InCmdField_eIsAddress     = (6<<2) + 1,    /* 8 bit index */ 
    BCMD_HmacSha1_InCmdField_eShaType       = (6<<2) + 2,    /* 8 bit index */ 
    BCMD_HmacSha1_InCmdField_eContextId     = (6<<2) + 3,    /* 8 bit index */
#if defined(USER_SHA_FIPS_KEY_256)
    BCMD_HmacSha1_InCmdField_eKeyLen        = (7<<2) + 3,
    BCMD_HmacSha1_InCmdField_eKeyData       = (8<<2) + 0,    /* 160 bits, 224 bits or 256 bits depending on eShaType */
#else
    BCMD_HmacSha1_InCmdField_eKeyData       = (7<<2) + 0    /* 160 bits, 224 bits or 256 bits depending on eShaType */
#endif /* USER_SHA_FIPS_KEY_256 */

/*    BCMD_HmacSha1_InCmdField_eDataLen	: Address calculated depending on size of eKeyData based on eShaType */
/*    BCMD_HmacSha1_InCmdField_eAddress : Address after eDataLen */
} BCMD_HmacSha1_InCmdField_e;

typedef enum BCMD_HmacSha1_OutCmdField_e
{
    BCMD_HmacSha1_OutCmdField_eStatus = (5<<2) + 3,          /* 8 bit */
    BCMD_HmacSha1_OutCmdField_eDigest = (6<<2) + 0           /* 160 bits, 224 bits or 256 bits depending on eShaType */
} BCMD_HmacSha1_OutCmdField_e;

/*
BCMD_VerifySecureBootParams - Compute SHA-256 of Boot & DRAM parameter section and compare it against
    the SHA value stored in BSP parameter section. If pass, return BSP parameters to host. Else, reset.

Parameters passed:
eReserved       - Reserved for future use
eData           - Data on which SHA needs to be computed
	
Returned:
eStatus         - Status of command.
eBspParamsSize  - Size of returned BSP Parameter Section in bytes
eBspParams      - BSP Parameter Section Data

BCMD_VerifySecureBootParams_InCmdField_e: 
|-------------------|-------------------|-------------------|-------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             	| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            | unused            | 
|-------------------|-------------------|-------------------|-------------------| 
|   eSecureBootParams - The BSP will hardcode this data size to 320 bytes       |
|   for the first three commands and 96 bytes for the last command              |
|-------------------|-------------------|-------------------|-------------------|

BCMD_VerifySecureBootParams_OutCmdField_e: 
|-------------------|-------------------|-------------------|-------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             	| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused			|  unused			|  unused			| eStatus -	8		|
|-------------------|-------------------|-------------------|-------------------| 
|  unused			|  unused			|  unused           | eBspParamsSize    |
|-------------------|-------------------|-------------------|-------------------| 
|                       eBspParams - size of eBspParamsSize bytes               |
|-------------------|-------------------|-------------------|-------------------| 
*/

typedef enum BCMD_VerifySecureBootParams_InCmdField_e
{
    BCMD_VerifySecureBootParams_InCmdField_eReserved    = (5<<2) + 0,   /* 32 bit index */ 
    BCMD_VerifySecureBootParams_InCmdField_eDramParams  = (6<<2) + 0   /* BSP will hardcode this data size to 320 bytes for the */
                                                                    /* first three commands and 96 bytes for the last command */
} BCMD_VerifySecureBootParams_InCmdField_e;

typedef enum BCMD_VerifySecureBootParams_OutCmdField_e
{
    BCMD_VerifySecureBootParams_OutCmdField_eStatus        = (5<<2) + 3,   /* 8 bits */
    BCMD_VerifySecureBootParams_OutCmdField_eBspParamsSize = (6<<2) + 3,   /* 8 bits */
    BCMD_VerifySecureBootParams_OutCmdField_eBspParams     = (7<<2) + 0    /* BSP Parameters Section - size specified by eBspParamsSize in bytes */
} BCMD_VerifySecureBootParams_OutCmdField_e;


typedef enum BPI_HmacSha1_Op_e
{
    BPI_HmacSha1_Op_eSha1,
    BPI_HmacSha1_Op_eHmac
} BPI_HmacSha1_Op_e;

typedef enum BPI_HmacSha1_ShaType_e
{
    BPI_HmacSha1_ShaType_eSha160 = 0,
    BPI_HmacSha1_ShaType_eSha224 = 1,
    BPI_HmacSha1_ShaType_eSha256 = 2,
    BPI_HmacSha1_ShaType_eMax
} BPI_HmacSha1_ShaType_e;

/* NOTE : If you add more HMAC contexts below than you will also need to update 
 * 1) #define NUMBER_OF_HMAC_CONTEXTS in bpi_hmac_sha1.h, and
 * 2) BPI_HmacSha1_Initialize() in bpi_hmac_sha.c
 * 3) ucRegionNumToRegionType[] table in bcmd_region_verify.c
 */
typedef enum BPI_HmacSha1_Context_e
{
    BPI_HmacSha1_Context_eRegionMin  = 0x00, /* First checkable region context */

    BPI_HmacSha1_Context_eCpuMin     = 0x00, /* First cpu context */
    BPI_HmacSha1_Context_eCpu0       = 0x00, /* MIPS boot region */
    BPI_HmacSha1_Context_eCpu1       = 0x01, /* MIPS region 1 */
    BPI_HmacSha1_Context_eCpu2       = 0x02, /* MIPS region 2 */
    BPI_HmacSha1_Context_eCpu3       = 0x03, /* MIPS region 3 */
    BPI_HmacSha1_Context_eCpu4       = 0x04, /* MIPS region 4 */
    BPI_HmacSha1_Context_eCpu5       = 0x05, /* MIPS region 5 */
    BPI_HmacSha1_Context_eCpu6       = 0x06, /* MIPS region 6 */
    BPI_HmacSha1_Context_eCpu7       = 0x07, /* MIPS region 7 */
    BPI_HmacSha1_Context_eCpuMax     = 0x07, /* Last cpu context */
    
    BPI_HmacSha1_Context_eRave0      = 0x08, /* RAVE region. This region is not background checked */
    
    BPI_HmacSha1_Context_eAudio0     = 0x09, /* Raaga region */
    
    BPI_HmacSha1_Context_eAvd0_Ila   = 0x0A, /* AVD Inner Loop Arc region */
    BPI_HmacSha1_Context_eAvd0_Ola   = 0x0B, /* AVD Outer Loop Arc region */
 
    BPI_HmacSha1_Context_eSvd0_Ila   = 0x0C, /* SVD Inner Loop Arc region */
    BPI_HmacSha1_Context_eSvd0_Ola   = 0x0D, /* SVD Outer Loop Arc region */
    BPI_HmacSha1_Context_eSvd0_Bld   = 0x0E, /* SVD Base Layer Arc region */

    BPI_HmacSha1_Context_eVice0_Pic  = 0x0F, /* ViCE Picture Arc region */
    BPI_HmacSha1_Context_eVice0_Mb   = 0x10, /* ViCE Macroblock Arc region */
    BPI_HmacSha1_Context_eRegion16   = 0x10, /* Region number 16 */

    BPI_HmacSha1_Context_eSid0       = 0x11, /* Still Image Decoder (SID) Arc region */

    BPI_HmacSha1_Context_eRaaga1     = 0x12, /* Raaga1 region */

    BPI_HmacSha1_Context_eVice1_Pic  = 0x13, /* ViCE1 Picture Arc region */
    BPI_HmacSha1_Context_eVice1_Mb   = 0x14, /* ViCE1 Macroblock Arc region */
   
    BPI_HmacSha1_Context_eRaaga0_IntSCM = 0x15,   /* Raaga SCM IMEM/DMEM region (region not background checked) */
    BPI_HmacSha1_Context_eRaaga0_IntAUD = 0x16,   /* Raaga AUDio IMEM/DMEM region (region not background checked) */

    BPI_HmacSha1_Context_eWebCpu0    = 0x17, /* Core1 aka WebCPU: we only do one MICH region */
    
    BPI_HmacSha1_Context_eRegionMax  = 0x17, /* The last context representing a checkable region. Used for query command */
    
    BPI_HmacSha1_Context_eHmacSha1CtxMin = 0x18,   /* Minimum valid User HMAC_SHA cmd context */
    BPI_HmacSha1_Context_eHmacSha1Ctx0 = 0x18,     /* User HMAC_SHA cmd context */
    BPI_HmacSha1_Context_eHmacSha1Ctx1 = 0x19,     /* User HMAC_SHA cmd context */

    BPI_HmacSha1_Context_eHmacSha1CtxSCM = 0x1a,     /* User HMAC_SHA cmd context for SCM thread ONLY */
    BPI_HmacSha1_Context_eHmacSha1CtxMax = 0x1a,   /* Maximum valid User HMAC_SHA cmd context */

    BPI_HmacSha1_Context_eCmdSig0    = 0x1b, /* Context for processing command signature, second-tier key, EStar Root Key, Secure RSA etc */
                                             /* Context information NOT saved across commands (cannot be used by continual mode commands) */
    BPI_HmacSha1_Context_eCmdSig1    = 0x1c, /* Context for processing command signature, second-tier key, EStar Root Key, Secure RSA etc */
                                             /* Context information NOT saved across commands (cannot be used by continual mode commands) */
    
    BPI_HmacSha1_Context_eMax        
} BPI_HmacSha1_Context_e;

/*
 * NOTE: This define below MUST be updated if more contexts are added in the enum above.
 * Same value as BPI_HmacSha1_Context_eMax.
 */
#define BPI_HMACSHA1_CONTEXT_MAX        0x1D      /* Maximum number of HMAC-SHA contexts supported */

/* Note : IMP - Below contexts cannot NOT be used by continual mode commands
          MSIPTV Uses the following two contexts. The contexts are used only with a single command
          and other commands can reuse the contexts if needed (in non-continual mode).
*/
typedef enum BCMD_MSIPTV_HmacSha1_Context_e{

    BCMD_MSIPTV_HmacSha1_Context_eCtx0 = BPI_HmacSha1_Context_eCmdSig0,
    BCMD_MSIPTV_HmacSha1_Context_eCtx1 = BPI_HmacSha1_Context_eCmdSig1

}BCMD_MSIPTV_HmacSha1_Context_e;

#if defined(USER_SHA_KEY_INCLUDE)
typedef enum BPI_HmacSha1_IncludeKey_e
{
    BPI_HmacSha1_IncludeKey_eNo          = 0x0,
    BPI_HmacSha1_IncludeKey_eAppend      = 0x1,
    BPI_HmacSha1_IncludeKey_eMax         = 0x2
} BPI_HmacSha1_IncludeKey_e;
#endif /* USER_SHA_KEY_INCLUDE */

/*
 * Some macros to facilitate checking of region values.
 */

#define REGION_IS_VALID(r)          ((UINT8)(r) <= (UINT8)BPI_HmacSha1_Context_eRegionMax)
#define REGION_IS_MIPS(r)           ((UINT8)(r) <= (UINT8)BPI_HmacSha1_Context_eCpuMax)
#define REGION_IS_AUDIO(r)          ((BPI_HmacSha1_Context_e)(r) == BPI_HmacSha1_Context_eAudio0)
#define REGION_IS_RAAGA1(r)         ((BPI_HmacSha1_Context_e)(r) == BPI_HmacSha1_Context_eRaaga1)
#define REGION_IS_AVD0(r)           (((BPI_HmacSha1_Context_e)(r) == BPI_HmacSha1_Context_eAvd0_Ila) \
                                        || ((BPI_HmacSha1_Context_e)(r) == BPI_HmacSha1_Context_eAvd0_Ola))
#define REGION_IS_SVD0(r)           (((BPI_HmacSha1_Context_e)(r) == BPI_HmacSha1_Context_eSvd0_Ila) \
                                        || ((BPI_HmacSha1_Context_e)(r) == BPI_HmacSha1_Context_eSvd0_Ola) \
                                        || ((BPI_HmacSha1_Context_e)(r) == BPI_HmacSha1_Context_eSvd0_Bld))
#define REGION_IS_VICE0(r)          (((BPI_HmacSha1_Context_e)(r) == BPI_HmacSha1_Context_eVice0_Pic) \
                                        || ((BPI_HmacSha1_Context_e)(r) == BPI_HmacSha1_Context_eVice0_Mb))
#define REGION_IS_VICE1(r)          (((BPI_HmacSha1_Context_e)(r) == BPI_HmacSha1_Context_eVice1_Pic) \
                                        || ((BPI_HmacSha1_Context_e)(r) == BPI_HmacSha1_Context_eVice1_Mb))
#define REGION_IS_SID0(r)           ((BPI_HmacSha1_Context_e)(r) == BPI_HmacSha1_Context_eSid0)
#define REGION_IS_RAVE0(r)          ((BPI_HmacSha1_Context_e)(r) == BPI_HmacSha1_Context_eRave0)
#define REGION_IS_WEBCPU0(r)        ((BPI_HmacSha1_Context_e)(r) == BPI_HmacSha1_Context_eWebCpu0)
#define REGION_IS_LESS_THAN_16(r)   ((BPI_HmacSha1_Context_e)(r) < BPI_HmacSha1_Context_eRegion16)

/* macro to check if region is second-stage boot loader */
#define REGION_IS_SSBL(r)           ((UINT8)(r) == BPI_HmacSha1_Context_eCpu2)

#endif /* BSP_S_HMAC_SHA1_H__ */

