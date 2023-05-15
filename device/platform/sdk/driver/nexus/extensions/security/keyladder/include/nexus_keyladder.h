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
 * $brcm_Workfile: nexus_keyladder.h $
 * $brcm_Revision: 6 $
 * $brcm_Date: 4/2/12 1:25p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/extensions/security/keyladder/7422/include/nexus_keyladder.h $
 * 
 * 6   4/2/12 1:25p atruong
 * SW7425-2807:Add support for BSP PKL hardware
 * 
 * 5   3/5/12 1:57p atruong
 * SWSECURITY-113: Secure RSA2 Support for 40-nm Platforms
 * 
 * 4   10/13/11 9:14a atruong
 * SWSECURITY-69: BSP Zeus 2.0 FW Support
 * 
 * 2   5/25/11 11:24a atruong
 * SW7346-168: Adding IV setting for key ladder setup
 * 
 * 1   1/28/11 7:07p atruong
 * SW7422-235: Enable Nexus Security extension modules for 97422
 *
 ******************************************************************************/
#ifndef NEXUS_KEYLADDER_H__
#define NEXUS_KEYLADDER_H__

#include "nexus_security_datatypes.h"
#include "nexus_security.h"

/*=**************************
The KeyLadder is an alternate method of deriving cryptographic and conditional access keys.
This extension provides support for the mechanism.
****************************/

#ifdef __cplusplus
extern "C" {
#endif

#define NEXUS_SECURITY_KEYLADDER_KEY_SIZE 32
#define NEXUS_SECURITY_KL_ACTCODE_SIZE    16

/**
Summary:
This enum defines the supported root key sources.
**/
typedef enum NEXUS_SecurityRootKeySrc
{
    NEXUS_SecurityRootKeySrc_eCuskey,
    NEXUS_SecurityRootKeySrc_eOtpKeyA,
    NEXUS_SecurityRootKeySrc_eOtpKeyB,
    NEXUS_SecurityRootKeySrc_eOtpKeyC,
	NEXUS_SecurityRootKeySrc_eOtpKeyD,
	NEXUS_SecurityRootKeySrc_eOtpKeyE,
	NEXUS_SecurityRootKeySrc_eOtpKeyF,
    NEXUS_SecurityRootKeySrc_eReserved0,
    NEXUS_SecurityRootKeySrc_eReserved1,
    NEXUS_SecurityRootKeySrc_eReserved2,

    /* Add new key entry type definition before this line */
    NEXUS_SecurityRootKeySrc_eMax
}   NEXUS_SecurityRootKeySrc;

/**
Summary:
This enum defines the supported key generation subcommands.
**/
typedef enum NEXUS_SecurityKeyGenCmdID
{
    NEXUS_SecurityKeyGenCmdID_eKeyGen,
    NEXUS_SecurityKeyGenCmdID_eVKLAssocQuery,

    /* Add new key entry type definition before this line */
    NEXUS_SecurityKeyGenCmdID_eMax
}   NEXUS_SecurityKeyGenCmdID;


/**
Summary:
This enum defines the supported KeyGen Key3 operations.
**/
typedef enum NEXUS_SecuritySessionKeyOp
{
    NEXUS_SecuritySessionKeyOp_eNoProcess,
    NEXUS_SecuritySessionKeyOp_eExport,

    /* Add new key entry type definition before this line */
    NEXUS_SecuritySessionKeyOp_eMax
}   NEXUS_SecuritySessionKeyOp;



/**
Summary:
This enum defines the supported keyladder types.
**/
typedef enum NEXUS_SecurityKeyladderType
{
    NEXUS_SecurityKeyladderType_e1Des,
    NEXUS_SecurityKeyladderType_e3Des,
    NEXUS_SecurityKeyladderType_eAes128,
	NEXUS_SecurityKeyladderType_ePKSM,       /* HD DTA */

    /* Add new key entry type definition before this line */
    NEXUS_SecurityKeyladderType_eMax
}   NEXUS_SecurityKeyladderType;

/**
Summary:
This enum defines the supported Key Ladder selection.
**/
typedef enum NEXUS_SecurityKeyLadderSelect
{
    NEXUS_SecurityKeyLadderSelect_eFWKL,
	NEXUS_SecurityKeyLadderSelect_eHWKL,
	NEXUS_SecurityKeyLadderSelect_ePKL,

    /* Add new swizzle type definition before this line */
    NEXUS_SecurityKeyLadderSelect_eMax
}   NEXUS_SecurityKeyLadderSelect;


/**
Summary:
This enum defines the supported keyladder IDs.
**/
typedef enum NEXUS_SecurityKeyladderID
{
    NEXUS_SecurityKeyladderID_eA,
    NEXUS_SecurityKeyladderID_eB,
    NEXUS_SecurityKeyladderID_eC,

    /* Add new key entry type definition before this line */
    NEXUS_SecurityKeyladderID_eMax
}   NEXUS_SecurityKeyladderID;


/**
Summary:
This enum defines the supported Customer SubMode.
**/
typedef enum NEXUS_SecurityCustomerSubMode
{
    NEXUS_SecurityCustomerSubMode_eGeneric_CA_64_4  = 0x0,  /* Key Ladder for Generic CA with 64 bit keys using Key4 */
    NEXUS_SecurityCustomerSubMode_eGeneric_CP_64_4  = 0x1,  /* Key Ladder for Generic CP with 64 bit keys using Key4 */
    NEXUS_SecurityCustomerSubMode_eGeneric_CA_64_5  = 0x2,  /* Key Ladder for Generic CA with 64 bit keys using Key5 */
    NEXUS_SecurityCustomerSubMode_eGeneric_CP_64_5  = 0x3,  /* Key Ladder for Generic CP with 64 bit keys using Key5 */
    NEXUS_SecurityCustomerSubMode_eGeneric_CA_128_4 = 0x4,  /* Key Ladder for Generic CA with 128 bit keys using Key4 */
    NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_4 = 0x5,  /* Key Ladder for Generic CP with 128 bit keys using Key4 */
    NEXUS_SecurityCustomerSubMode_eGeneric_CA_128_5 = 0x6,  /* Key Ladder for Generic CA with 128 bit keys using Key5 */
    NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_5 = 0x7,  /* Key Ladder for Generic CP with 128 bit keys using Key5 */
    NEXUS_SecurityCustomerSubMode_eReserved8        = 0x8,
    NEXUS_SecurityCustomerSubMode_eReserved9        = 0x9,
    NEXUS_SecurityCustomerSubMode_eReserved10       = 0xA,
    NEXUS_SecurityCustomerSubMode_eReserved11       = 0xB,
    NEXUS_SecurityCustomerSubMode_eReserved12       = 0xC,
    NEXUS_SecurityCustomerSubMode_eReserved13       = 0xD,
    NEXUS_SecurityCustomerSubMode_eGeneralPurpose1  = 0xE,  /* Key Ladder for HDMI, IV and Signed Commands */
    NEXUS_SecurityCustomerSubMode_eGeneralPurpose2  = 0xF,  /* Key Ladder for User Hmac */
    NEXUS_SecurityCustomerSubMode_eReserved16       = 0x10,
    NEXUS_SecurityCustomerSubMode_eReserved17       = 0x11,
    NEXUS_SecurityCustomerSubMode_eGeneric_CA_64_45 = 0x12, /* Key Ladder for Generic CA with 64 bit keys using Key4 and Key5 */
    NEXUS_SecurityCustomerSubMode_eGeneric_CP_64_45 = 0x13, /* Key Ladder for Generic CP with 64 bit keys using Key4 and Key5 */
	NEXUS_SecurityCustomerSubMode_eReserved20		= 0x14,
	NEXUS_SecurityCustomerSubMode_eReserved21		= 0x15,
	NEXUS_SecurityCustomerSubMode_eSecureRSA2       = 0x16,
	NEXUS_SecurityCustomerSubMode_eETSI_5           = 0x17,
	NEXUS_SecurityCustomerSubMode_eDTA_M_CA         = 0x18,
	NEXUS_SecurityCustomerSubMode_eDTA_M_CP         = 0x19,
	NEXUS_SecurityCustomerSubMode_eDTA_C_CA         = 0x1A,
	NEXUS_SecurityCustomerSubMode_eDTA_C_CP         = 0x1B,


    /* Add new Customer SubMode definition before this line */
    NEXUS_SecurityCustomerSubMode_eMax
}   NEXUS_SecurityCustomerSubMode;



/**
Summary:
This enum defines the supported swizzle types.
**/
typedef enum NEXUS_SecuritySwizzleType
{
    NEXUS_SecuritySwizzleType_eNone,
    NEXUS_SecuritySwizzleType_eSwizzle1,
    NEXUS_SecuritySwizzleType_eSwizzle0,

    /* Add new swizzle type definition before this line */
    NEXUS_SecuritySwizzleType_eMax
}   NEXUS_SecuritySwizzleType;

/**
Summary:
This enum defines the supported key modes.
**/
typedef enum NEXUS_SecurityKeyMode
{
    NEXUS_SecurityKeyMode_eRegular,
    NEXUS_SecurityKeyMode_eDes56,
    NEXUS_SecurityKeyMode_eReserved2,
    NEXUS_SecurityKeyMode_eReserved3,
    NEXUS_SecurityKeyMode_eDvbConformance,

    /* Add new swizzle type definition before this line */
    NEXUS_SecurityKeyMode_eMax
}   NEXUS_SecurityKeyMode;


/**
Summary:
This enum defines the supported key modes.
**/
typedef enum NEXUS_SecurityHWKLLenSelect
{
	NEXUS_SecurityHWKLLenSelect_eLen0,
	NEXUS_SecurityHWKLLenSelect_eLen1,
	NEXUS_SecurityHWKLLenSelect_eLen2,
	NEXUS_SecurityHWKLLenSelect_eLen3,

    /* Add new swizzle type definition before this line */
    NEXUS_SecurityHWKLLenSelect_eMax
}   NEXUS_SecurityHWKLLenSelect;



/**
Summary:
This struct defines the encrypted session structure.

Description:
This structure contains the information necessary to generate the
Session Key which is Key3 in BRCM terminology.
**/
typedef struct NEXUS_SecurityEncryptedKey3
{
    NEXUS_SecurityKeyladderID           keyladderID;    /* not used for 40-nm;  for compatibility */
	NEXUS_SecurityKeyGenCmdID           keyGenCmdID;    /* Key Generation or VKL Association query */
    NEXUS_SecurityKeyladderType         keyladderType;
	NEXUS_SecurityKeyLadderSelect		keyLadderSelect; /*FW key ladder | HW key ladder | Poway key ladder */
	bool								rootKeySwap;
    NEXUS_SecurityRootKeySrc            rootKeySrc;        
    NEXUS_SecuritySwizzleType           swizzleType;
    NEXUS_SecurityKeyType               keyEntryType;
    NEXUS_SecurityKeyIVType             keyDestIVType;
    NEXUS_SecurityAlgorithmConfigDestination dest;/* This member is required only for CACP keyslot configuration */
    NEXUS_SecurityOperation             operation;      /* key ladder operation, encrypt or decrypt */
    NEXUS_SecurityOperation             operationKey2;  /* cusKeyL and cusKeyH encrypt or decrypt */
    bool                                bASKMMode;
	bool								bkeyGenBlocked; /* set to true to instruct FW not to generate key */
    NEXUS_SecurityCustomerSubMode       custSubMode;
    NEXUS_SecurityVirtualKeyladderID    virtualKeyLadderID;
    NEXUS_SecurityKeyMode               keyMode;
    NEXUS_SecuritySessionKeyOp          sessionKeyOp;
    unsigned char                       cusKeyL;        /* also used as SwizzleIndex for Swizzle1 */                                      
    unsigned char                       cusKeyH;                                      
    unsigned char                       cusKeyVarL;                                   
    unsigned char                       cusKeyVarH;                                   
    unsigned char                       keyData[NEXUS_SECURITY_KEYLADDER_KEY_SIZE]; 
    bool                                bSwapAESKey;  
    bool                                bRouteKey;
	NEXUS_SecurityHWKLLenSelect			hwklLen;
	NEXUS_SecurityAlgorithm				hwklDestAlg;
    unsigned char                       actCode[NEXUS_SECURITY_KL_ACTCODE_SIZE]; 
} NEXUS_SecurityEncryptedSessionKey;




/**
Summary:
This struct defines the encrypted control word (CW) structure.

Description:
This structure contains the information necessary to generate the
control word which is Key4 in BRCM terminology.
**/
typedef struct NEXUS_SecurityEncryptedKey4
{
    NEXUS_SecurityKeyladderID           keyladderID;    /* not used for 40-nm;  for compatibility */
	NEXUS_SecurityKeyGenCmdID           keyGenCmdID;    /* Key Generation or VKL Association query */
    NEXUS_SecurityKeyladderType         keyladderType;
	NEXUS_SecurityKeyLadderSelect		keyLadderSelect; /*FW key ladder | HW key ladder | Poway key ladder */
	bool								rootKeySwap;
    NEXUS_SecurityRootKeySrc            rootKeySrc;
    NEXUS_SecurityKeyType               keyEntryType;
    NEXUS_SecurityKeyIVType             keyDestIVType;
    NEXUS_SecurityAlgorithmConfigDestination dest;/* This member is required only for CACP keyslot configuration */
    NEXUS_SecuritySwizzleType           swizzleType;
    bool                                bASKMMode;
	bool								bkeyGenBlocked; /* set to true to instruct FW not to generate key */
    NEXUS_SecurityCustomerSubMode       custSubMode;
    NEXUS_SecurityVirtualKeyladderID    virtualKeyLadderID;
    NEXUS_SecurityKeyMode               keyMode;
    NEXUS_SecurityAlgorithmScPolarity   sc01Polarity[NEXUS_SecurityPacketType_eMax]; /* Packets with SC bits 0x01 will be treated as this polarity */
    unsigned int                        keySize;
    unsigned char                       keyData[NEXUS_SECURITY_KEYLADDER_KEY_SIZE]; 
    NEXUS_SecurityOperation             operation;          /* operation mode. This member is NOT required for CA */
    bool                                bSwapAESKey;  
    bool                                bRouteKey;
	NEXUS_SecurityHWKLLenSelect			hwklLen;
	NEXUS_SecurityAlgorithm				hwklDestAlg;
    unsigned char                       actCode[NEXUS_SECURITY_KL_ACTCODE_SIZE]; 
} NEXUS_SecurityEncryptedControlWord;

/**
Summary:
This function retrieves default settings for encrypted session key generation.

Description:
This function shall retrieve default settings for encrypted session key generation.

See Also:
NEXUS_Security_GenerateSessionKey

**/
void NEXUS_Security_GetDefaultSessionKeySettings(
    NEXUS_SecurityEncryptedSessionKey  *pSettings    /* [out] */
    );


/**
Summary:
This function generates session key for a keyslot.

Description:
This function shall generate session key for a keyslot.  This function
must be called BEFORE the NEXUS_Security_GenerateControlWord
function can be called.

Performance and Timing:
This is a synchronous function that will return when it is done.

See Also:
NEXUS_Security_GenerateControlWord
**/
NEXUS_Error NEXUS_Security_GenerateSessionKey(
    NEXUS_KeySlotHandle               keyHandle,
    const NEXUS_SecurityEncryptedSessionKey *pSessionKey
    );



/**
Summary:
This function retrieves default settings for control word generation.

Description:
This function shall retrieve default settings for control word generation.

See Also:
NEXUS_Security_GenerateControlWord


**/
void NEXUS_Security_GetDefaultControlWordSettings(
    NEXUS_SecurityEncryptedControlWord  *pSettings    /* [out] */
    );

NEXUS_Error NEXUS_Security_PklInit(void);

/**
Summary:
This function generates the CW for a keyslot.

Description:
This function shall generate the CW for a keyslot.  This function
must be called after the NEXUS_Security_GenerateSessionKey has been
called to generate the session key.

Performance and Timing:
This is a synchronous function that will return when it is done.

See Also:
NEXUS_Security_GenerateSessionKey
**/
NEXUS_Error NEXUS_Security_GenerateControlWord(
    NEXUS_KeySlotHandle                      keyHandle,
    const NEXUS_SecurityEncryptedControlWord *pCW
    );


/**
Summary:
This function generates the key5 for a keyslot.

Description:
This function shall generate the key5 for a keyslot.  This function
must be called after the NEXUS_Security_GenerateSessionKey and
NEXUS_Security_GenerateControlWord have been called.

Note that this function takes the same structure as
NEXUS_Security_GenerateControlWord.

Performance and Timing:
This is a synchronous function that will return when it is done.

See Also:
NEXUS_Security_GenerateSessionKey, NEXUS_Security_GenerateControlWord
**/
NEXUS_Error NEXUS_Security_GenerateKey5(
    NEXUS_KeySlotHandle                      keyHandle,
    const NEXUS_SecurityEncryptedControlWord *pCW
    );


#ifdef __cplusplus
}
#endif

#endif
