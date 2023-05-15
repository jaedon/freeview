/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: nexus_hmac_sha_cmd.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 3/5/12 1:12p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/extensions/security/usercmd/7422/include/nexus_hmac_sha_cmd.h $
 * 
 * 3   3/5/12 1:12p atruong
 * SWSECURITY-113: Secure RSA2 Support for 40-nm Platforms
 * 
 * 2   2/15/12 12:55p atruong
 * SWSECURITY-109: HMAC-SHA command support for 40-nm platforms
 * 
 * 1   1/28/11 7:30p atruong
 * SW7422-235: Enable Nexus Security extension modules for 97422
 *
 ******************************************************************************/
#ifndef NEXUS_HMAC_SHA_CMD_H__
#define NEXUS_HMAC_SHA_CMD_H__

#include "nexus_security_datatypes.h"
#include "nexus_security.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NEXUS_HMACSHA_KEY_LEN           32       /* 20 for Sha1, 28 for Sha224, 32 for Sha256 */
#define NEXUS_HMACSHA_DIGEST_LEN	    32       /* in bytes, same as HMACSHA key length */

/**
Summary:

Description:
See Also:
**/
typedef enum NEXUS_HMACSHA_Op
{
    NEXUS_HMACSHA_Op_eSHA   = 0,
    NEXUS_HMACSHA_Op_eHMAC  = 1

} NEXUS_HMACSHA_Op;

/**
Summary:

Description:
See Also:
**/
typedef enum NEXUS_HMACSHA_KeyInclusion_Op
{
    NEXUS_HMACSHA_KeyInclusion_Op_eNo      = 0,
    NEXUS_HMACSHA_KeyInclusion_Op_eAppend  = 1

} NEXUS_HMACSHA_KeyInclusion_Op;

/**
Summary:

Description:
See Also:
**/
typedef enum NEXUS_HMACSHA_SHAType
{
    NEXUS_HMACSHA_SHAType_eSha160 = 0,
    NEXUS_HMACSHA_SHAType_eSha224 = 1,
	NEXUS_HMACSHA_SHAType_eSha256 = 2,
	NEXUS_HMACSHA_SHAType_eMax

} NEXUS_HMACSHA_SHAType;

/**
Summary:

Description:
See Also:
**/
typedef enum NEXUS_HMACSHA_BSPContext
{
    NEXUS_HMACSHA_BSPContext_eContext0 = 0x18,
    NEXUS_HMACSHA_BSPContext_eContext1 = 0x19

} NEXUS_HMACSHA_BSPContext;


/**
Summary:

Description:
See Also:
**/
typedef enum NEXUS_HMACSHA_DataMode
{
	NEXUS_HMACSHA_DataMode_eAllIn = 0x00,
	NEXUS_HMACSHA_DataMode_eMore  = 0x01,
	NEXUS_HMACSHA_DataMode_eLast  = 0x10
    
} NEXUS_HMACSHA_DataMode;

/**
Summary:

Description:
See Also:
**/
typedef enum NEXUS_HMACSHA_DataSource
{
    NEXUS_HMACSHA_DataSource_eCmdBuf = 0,
    NEXUS_HMACSHA_DataSource_eDRAM   = 1

} NEXUS_HMACSHA_DataSource;


/**
Summary:

Description:
See Also:
**/
typedef enum NEXUS_HMACSHA_KeySource
{
    NEXUS_HMACSHA_KeySource_eKeyVal    = 0,
    NEXUS_HMACSHA_KeySource_eKeyLadder = 1

} NEXUS_HMACSHA_KeySource;


/**
Summary:
This type defines the data structure for HMAC/SHA operation

Description:
See Also:
NEXUS_HMACSHA_PerformOp
**/
typedef struct NEXUS_HMACSHA_OpSettings 
{
	NEXUS_HMACSHA_Op                  opMode;    /* Which operation -- Hmac or Sha */
	NEXUS_HMACSHA_SHAType             shaType;   /* If op is Sha -- Which type         */
	NEXUS_HMACSHA_BSPContext          context;   /* which context to perform the op */
	NEXUS_HMACSHA_KeySource           keySource; /* Key is user supplied or from key ladder */
	NEXUS_SecurityVirtualKeyladderID  VKL;       /* If key is from key ladder - Which VKL */
	NEXUS_SecurityKeySource           keyLayer;  /* If key is from key ladder - Which Key layer */
	NEXUS_HMACSHA_DataSource          dataSrc;   /* Only support NEXUS_HMACSHA_DataSource_eDRAM for now */
	uint32_t			  keyLength; /* vliang, copy from 65nm, needed */
	unsigned char                     keyData[NEXUS_HMACSHA_KEY_LEN]; /* BE byte array */
	NEXUS_HMACSHA_DataMode            dataMode;  /* Data in one buffer or multiple ones */
	NEXUS_HMACSHA_KeyInclusion_Op     keyIncMode; /* how to include the key in calculation */
	bool                              byteSwap;  /* host needs to swap bytes (BE) for Aegis */
    uint32_t                          dataSize;
    uint8_t                           *dataAddress; /* attr{memory=cached} */

} NEXUS_HMACSHA_OpSettings;


/**
Summary:
This type defines the data structure for Digest output from a HMAC/SHA operation

Description:
See Also:
NEXUS_HMACSHA_PerformOp
**/
typedef struct NEXUS_HMACSHA_DigestOutput 
{
	unsigned char        digestData[NEXUS_HMACSHA_DIGEST_LEN]; /* BE byte array */
    uint32_t             digestSize;

} NEXUS_HMACSHA_DigestOutput;


/*****************************************************************************
Summary:
Get default settings
*****************************************************************************/
void NEXUS_HMACSHA_GetDefaultOpSettings(
	NEXUS_HMACSHA_OpSettings *pOpSettings		/* [out] */
    );

/*****************************************************************************
Summary:
Perform the requested HMAC/SHA operation
*****************************************************************************/
NEXUS_Error NEXUS_HMACSHA_PerformOp(
    const NEXUS_HMACSHA_OpSettings *pOpSettings,    /* structure holding input parameters */
    NEXUS_HMACSHA_DigestOutput     *pOutput         /* [out] structure holding digest buffer and size */
    );



#ifdef __cplusplus
}
#endif

#endif
