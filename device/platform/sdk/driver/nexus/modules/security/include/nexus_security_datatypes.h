/***************************************************************************
 *     (c)2008-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_security_datatypes.h $
 * $brcm_Revision: 23 $
 * $brcm_Date: 3/5/12 3:57p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/security/7400/include/nexus_security_datatypes.h $
 * 
 * 23   3/5/12 3:57p erickson
 * SWSECURITY-113: add NEXUS_SecurityAlgorithm_eAesCounter alias
 * 
 * 22   10/12/11 12:38p mphillip
 * SW7346-481: Merge B0 changes to main
 * 
 * SW7346-481/3   10/10/11 3:04p mphillip
 * SW7346-481: Nexus API change board changes (whitespace, enum
 *  equivalents)
 * 
 * SW7346-481/2   9/9/11 7:13p mphillip
 * SW7346-481: Add bRestrictSourceDropPktEnable for HSM's
 *  bDropRregionPackets
 * 
 * SW7346-481/1   9/9/11 6:28p mphillip
 * SW7346-481: Initial 7346 B0 and 7425 B0 support
 * 
 * 21   5/9/11 6:55p mphillip
 * SW7422-430: Merge API change to main
 * 
 * SW7422-430/1   4/20/11 7:05p mphillip
 * SW7422-430: Separate CA even/odd configuration, add keyslot type
 *  support when allocating
 * 
 * 20   3/17/11 4:53p mphillip
 * SW7422-335: Merge API change to main
 * 
 * SW7422-335/1   3/10/11 5:04p mphillip
 * SW7422-335: Update key2select for 40nm
 * 
 * 19   3/10/11 3:52p mphillip
 * SW7422-269: Merge changes to main
 * 
 * SW7422-269/2   3/9/11 5:09p mphillip
 * SW7422-264: Update per-key IV datatype name.
 * 
 * SW7422-269/1   3/3/11 4:57p mphillip
 * SW7422-269: Preliminary IV handling
 * 
 * 18   2/23/11 11:14a mphillip
 * SW7422-253: Merge API changes to main
 * 
 * SW7422-253/2   2/16/11 7:41p mphillip
 * SW7422-253: Allow SC bits to be controlled per-packet-type
 * 
 * SW7422-253/1   2/15/11 7:53p mphillip
 * SW7422-253: Add types for AV keyladder
 * 
 * 17   12/22/10 5:48p mphillip
 * SW7422-104: Merge 40nm support to /main
 * 
 * SW7422-104/3   12/22/10 5:34p mphillip
 * SW7422-104: 40nm enums completed
 * 
 * SW7422-104/2   12/22/10 11:41a mphillip
 * SW7422-104: Structure updates/fixups
 * 
 * SW7422-104/1   12/16/10 6:14p mphillip
 * SW7422-104: Initial support for 7422
 * 
 * 16   11/10/10 1:57p mphillip
 * SW7400-2948: Add a way for HSM to return an internal error code to the
 *  app
 * 
 * 15   8/4/10 3:30p mphillip
 * SW7405-4325: Merge keyslot refactoring to main
 * 
 * SW7405-4325/1   7/2/10 7:23p mphillip
 * SW7405-4325: Move keyslot allocation/deallocation to base to break DMA
 *  dependency
 * 
 * 14   3/16/10 9:46a erickson
 * SW7400-2678: add NEXUS_Security_InvalidateKey
 *
 * 13   12/14/09 10:12a erickson
 * SW7468-16: add NEXUS_SecurityOtpId and NEXUS_SecurityKeySlotType
 *
 * 12   8/24/09 4:34p erickson
 * SW7420-223: 7420B0 support
 *
 * 11   8/12/09 11:49a erickson
 * PR55169: 7420B0 ASKM Support
 *
 * 10   4/13/09 3:29p erickson
 * PR53745: added more settings
 *
 * 9   4/2/09 1:51p erickson
 * PR53817: nexus_base.h should not be included in any public api
 *
 * 8   3/31/09 7:57p vsilyaev
 * PR 53733: Added ASF related types
 *
 * 7   3/6/09 2:13p erickson
 * PR52786: added NEXUS_SecurityAlgorithmSettings.enableTimestamps
 *
 * 6   1/7/09 2:30p jgarrett
 * PR 50093: Adding Multi2
 *
 * 5   9/3/08 3:29p erickson
 * PR45612: update
 *
 * 4   6/23/08 6:15p mphillip
 * PR40027: Tidy up naming conventions to match with Nexus
 *
 * 3   6/17/08 6:16p mphillip
 * PR40027: Retrieve more information from KeySlotHandle
 *
 * 2   6/17/08 12:31a mphillip
 * PR40027: Keyslot changes for shim layer
 *
 * 1   5/28/08 2:32p mphillip
 * PR38369: Refactor keyslot configuration to Security module from Crypto
 *  module
 *
 * 1   5/2/08 5:45p mphillip
 * PR38369: Add crypto headers
 *
 **************************************************************************/
#ifndef NEXUS_SECURITY_DATATYPES_H__
#define NEXUS_SECURITY_DATATYPES_H__

#include "nexus_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NEXUS_Security_P_Handle * NEXUS_SecurityHandle;


/**
Summary:
This enum defines the supported crypto alogrithms.
**/
typedef enum NEXUS_SecurityAlgorithm
{
    NEXUS_SecurityAlgorithm_eDvb,    /* DVB will only be used by CA descrambler */
    NEXUS_SecurityAlgorithm_eDvbCsa2 = NEXUS_SecurityAlgorithm_eDvb,
    NEXUS_SecurityAlgorithm_eMulti2,
    NEXUS_SecurityAlgorithm_eDes,
    NEXUS_SecurityAlgorithm_e3DesAba,
    NEXUS_SecurityAlgorithm_e3DesAbc,
    NEXUS_SecurityAlgorithm_eAes,
    NEXUS_SecurityAlgorithm_eAes128 = NEXUS_SecurityAlgorithm_eAes,
    NEXUS_SecurityAlgorithm_eAes192,
    NEXUS_SecurityAlgorithm_eTivo,

    /* The 4 algorithms below are only supported for M2M */
    NEXUS_SecurityAlgorithm_eC2,
    NEXUS_SecurityAlgorithm_eCss,
    NEXUS_SecurityAlgorithm_eM6Ke,
    NEXUS_SecurityAlgorithm_eM6,

    /* added WMDRM_PD */
    NEXUS_SecurityAlgorithm_eRc4,
    NEXUS_SecurityAlgorithm_eCbcMac,
    NEXUS_SecurityAlgorithm_eWMDrmPd,

    NEXUS_SecurityAlgorithm_eAes128G,
    NEXUS_SecurityAlgorithm_eHdDVD,
    NEXUS_SecurityAlgorithm_eBrDVD,

    NEXUS_SecurityAlgorithm_eDvbCsa3,
    NEXUS_SecurityAlgorithm_eAsf,
    NEXUS_SecurityAlgorithm_eAesCounter = NEXUS_SecurityAlgorithm_eAsf,
    NEXUS_SecurityAlgorithm_eMSMultiSwapMac,
    NEXUS_SecurityAlgorithm_eAsa,

    /* Reserved values, passed down as 0..8 */
    NEXUS_SecurityAlgorithm_eReserved0,
    NEXUS_SecurityAlgorithm_eReserved1,
    NEXUS_SecurityAlgorithm_eReserved2,
    NEXUS_SecurityAlgorithm_eReserved3,
    NEXUS_SecurityAlgorithm_eReserved4,
    NEXUS_SecurityAlgorithm_eReserved5,
    NEXUS_SecurityAlgorithm_eReserved6,
    NEXUS_SecurityAlgorithm_eReserved7,
    NEXUS_SecurityAlgorithm_eReserved8,

    NEXUS_SecurityAlgorithm_eMax
} NEXUS_SecurityAlgorithm;

/**
Summary:
This enum defines the supported crypto alogrithm variants.
**/
typedef enum NEXUS_SecurityAlgorithmVariant
{
    NEXUS_SecurityAlgorithmVariant_eEcb,
    NEXUS_SecurityAlgorithmVariant_eXpt = NEXUS_SecurityAlgorithmVariant_eEcb,  /* for NEXUS_SecurityAlgorithm_eDvb, scramble level */
    NEXUS_SecurityAlgorithmVariant_eCbc,
    NEXUS_SecurityAlgorithmVariant_ePes = NEXUS_SecurityAlgorithmVariant_eCbc,  /* for NEXUS_SecurityAlgorithm_eDvb, scramble level */
    NEXUS_SecurityAlgorithmVariant_eCounter,
    NEXUS_SecurityAlgorithmVariant_eRCbc,
    NEXUS_SecurityAlgorithmVariant_eMax
} NEXUS_SecurityAlgorithmVariant;

/**
Summary:
This enum defines the customer modes.
**/
typedef enum NEXUS_Security_CustomerMode
{
    NEXUS_Security_CustomerMode_eGeneric,
    NEXUS_Security_CustomerMode_eReserved1,
    NEXUS_Security_CustomerMode_eReserved2,
    NEXUS_Security_CustomerMode_eNDS_NonNSK
} NEXUS_Security_CustomerMode;

/**
Summary:
This enum defines the supported termination modes.
For pairs of Enums, first one for ASKM; second for regular
**/
typedef enum NEXUS_SecurityTerminationMode
{
    NEXUS_SecurityTerminationMode_eClear,
    NEXUS_SecurityTerminationMode_eCbcResidual,
    NEXUS_SecurityTerminationMode_eBlock = NEXUS_SecurityTerminationMode_eCbcResidual,
    NEXUS_SecurityTerminationMode_eReserved2,
    NEXUS_SecurityTerminationMode_eCipherStealing = NEXUS_SecurityTerminationMode_eReserved2,
    NEXUS_SecurityTerminationMode_eCtsCpcm,
    NEXUS_SecurityTerminationMode_eCipherStealingComcast = NEXUS_SecurityTerminationMode_eCtsCpcm,
    NEXUS_SecurityTerminationMode_eReserved4,
    NEXUS_SecurityTerminationMode_eReserved5,
    NEXUS_SecurityTerminationMode_eReserved6,
    NEXUS_SecurityTerminationMode_eMax
} NEXUS_SecurityTerminationMode;

typedef enum NEXUS_SecurityIVMode
{
    NEXUS_SecurityIVMode_eRegular,
    NEXUS_SecurityIVMode_eReserved1,
    NEXUS_SecurityIVMode_eReserved2,
    NEXUS_SecurityIVMode_eMax
} NEXUS_SecurityIVMode;

typedef enum NEXUS_SecuritySolitarySelect
{
    NEXUS_SecuritySolitarySelect_eClear,
    NEXUS_SecuritySolitarySelect_eSa,
    NEXUS_SecuritySolitarySelect_eCbcXorIV,
    NEXUS_SecuritySolitarySelect_eReserved3,
    NEXUS_SecuritySolitarySelect_eReserved4,
    NEXUS_SecuritySolitarySelect_eMax

} NEXUS_SecuritySolitarySelect;

/**
Summary:
This enum defines the supported Module ID used for platforms with ASKM support.
**/
typedef enum NEXUS_SecurityAskmModuleID
{
    NEXUS_SecurityAskmModuleID_eZero, /* Zero module ID */
    NEXUS_SecurityAskmModuleID_eNSK, /* NSK_KEY2 module ID */
    NEXUS_SecurityAskmModuleID_eAVK, /* AVK_KEY2 module ID */
    NEXUS_SecurityAskmModuleID_eModuleID_3, /* Module ID 3 */
    NEXUS_SecurityAskmModuleID_eModuleID_4, /* Module ID 4 */
    NEXUS_SecurityAskmModuleID_eModuleID_5, /* Module ID 5 */
    NEXUS_SecurityAskmModuleID_eModuleID_6, /* Module ID 6 */
    NEXUS_SecurityAskmModuleID_eModuleID_7, /* Module ID 7 */
    NEXUS_SecurityAskmModuleID_eModuleID_8, /* Module ID 8 */
    NEXUS_SecurityAskmModuleID_eModuleID_9, /* Module ID 9 */
    NEXUS_SecurityAskmModuleID_eModuleID_10, /* Module ID 10 */
    NEXUS_SecurityAskmModuleID_eModuleID_11, /* Module ID 11 */
    NEXUS_SecurityAskmModuleID_eModuleID_12, /* Module ID 12 */
    NEXUS_SecurityAskmModuleID_eModuleID_13, /* Module ID 13 */
    NEXUS_SecurityAskmModuleID_eModuleID_14, /* Module ID 14 */
    NEXUS_SecurityAskmModuleID_eModuleID_15, /* Module ID 15 */
    NEXUS_SecurityAskmModuleID_eModuleID_16, /* Module ID 16 */
    NEXUS_SecurityAskmModuleID_eModuleID_17, /* Module ID 17 */
    NEXUS_SecurityAskmModuleID_eModuleID_18, /* Module ID 18 */
    NEXUS_SecurityAskmModuleID_eModuleID_19, /* Module ID 19 */
    NEXUS_SecurityAskmModuleID_eModuleID_20, /* Module ID 20 */
    NEXUS_SecurityAskmModuleID_eModuleID_21, /* Module ID 21 */
    NEXUS_SecurityAskmModuleID_eModuleID_22, /* Module ID 22 */
    NEXUS_SecurityAskmModuleID_eModuleID_23, /* Module ID 23 */
    NEXUS_SecurityAskmModuleID_eModuleID_24, /* Module ID 24 */
    NEXUS_SecurityAskmModuleID_eModuleID_25, /* Module ID 25 */
    NEXUS_SecurityAskmModuleID_eMax
} NEXUS_SecurityAskmModuleID;


/**
Summary:
This enum defines the choices for OTP ID Select used for platforms with ASKM support.
**/
typedef enum NEXUS_SecurityOtpId
{
    NEXUS_SecurityOtpId_eOtpVal,  /* Use value programmed in OTP */
    NEXUS_SecurityOtpId_eOneVal,  /* Use value 1   */
    NEXUS_SecurityOtpId_eZeroVal, /* Use value 0   */
    NEXUS_SecurityOtpId_eMax
} NEXUS_SecurityOtpId;


/**
Summary:
This enum defines the choices for Key2 selection used for platforms with ASKM support.
**/
typedef enum NEXUS_SecurityKey2Select
{
    NEXUS_SecurityKey2Select_eRealKey,
    NEXUS_SecurityKey2Select_eReserved1,
    NEXUS_SecurityKey2Select_eFixedKey,
    NEXUS_SecurityKey2Select_eMax
} NEXUS_SecurityKey2Select;


/**
Summary:
This enum defines the supported DVB scrambling levels.
**/
typedef enum NEXUS_SecurityDvbScrambleLevel
{
    NEXUS_SecurityDvbScrambleLevel_eTs,
    NEXUS_SecurityDvbScrambleLevel_ePes,
    NEXUS_SecurityDvbScrambleLevel_eMax
} NEXUS_SecurityDvbScrambleLevel;

/**
Summary:
This enum defines the supported AES counter modes.
**/
typedef enum NEXUS_SecurityAesCounterSize
{
    NEXUS_SecurityAesCounterSize_e32Bits,
    NEXUS_SecurityAesCounterSize_e64Bits,
    NEXUS_SecurityAesCounterSize_e96Bits,
    NEXUS_SecurityAesCounterSize_e128Bits,
    NEXUS_SecurityAesCounterSize_e128BitsS = NEXUS_SecurityAesCounterSize_e128Bits,
    NEXUS_SecurityAesCounterSize_eMax
} NEXUS_SecurityAesCounterSize;

/**
Summary:
This enum defines key sources.
Enums are defined in pairs.  First one is for ASKM mode;  Second one is for regular mode.
**/
typedef enum NEXUS_SecurityKeySource
{
    NEXUS_SecurityKeySource_eReserved0,
    NEXUS_SecurityKeySource_eFirstRam = NEXUS_SecurityKeySource_eReserved0,
    NEXUS_SecurityKeySource_eReserved1,
    NEXUS_SecurityKeySource_eSecondRam = NEXUS_SecurityKeySource_eReserved1,
    NEXUS_SecurityKeySource_eReserved2,
    NEXUS_SecurityKeySource_eThirdRam = NEXUS_SecurityKeySource_eReserved2,
    NEXUS_SecurityKeySource_eKey3,
    NEXUS_SecurityKeySource_eKey3KeyLadder1 = NEXUS_SecurityKeySource_eKey3,
    NEXUS_SecurityKeySource_eKey4,
    NEXUS_SecurityKeySource_eKey4KeyLadder1 = NEXUS_SecurityKeySource_eKey4,
    NEXUS_SecurityKeySource_eKey5,
    NEXUS_SecurityKeySource_eKey5KeyLadder1 = NEXUS_SecurityKeySource_eKey5,
    NEXUS_SecurityKeySource_eReserved6,
    NEXUS_SecurityKeySource_eAvCPCW = NEXUS_SecurityKeySource_eReserved6,
    NEXUS_SecurityKeySource_eKey3KeyLadder2 = NEXUS_SecurityKeySource_eAvCPCW,
    NEXUS_SecurityKeySource_eReserved7,
    NEXUS_SecurityKeySource_eAvCW = NEXUS_SecurityKeySource_eReserved7,
    NEXUS_SecurityKeySource_eKey4KeyLadder2 = NEXUS_SecurityKeySource_eAvCW,
    NEXUS_SecurityKeySource_eFirstRamAskm,
    NEXUS_SecurityKeySource_eKey5KeyLadder2 = NEXUS_SecurityKeySource_eFirstRamAskm,
    NEXUS_SecurityKeySource_ePkl = NEXUS_SecurityKeySource_eKey5KeyLadder2,
    NEXUS_SecurityKeySource_eSecondRamAskm,
    NEXUS_SecurityKeySource_eKey3KeyLadder3 = NEXUS_SecurityKeySource_eSecondRamAskm,
    NEXUS_SecurityKeySource_eFirstRamAskm40nm = NEXUS_SecurityKeySource_eKey3KeyLadder3,
    NEXUS_SecurityKeySource_eThirdRamAskm,
    NEXUS_SecurityKeySource_eKey4KeyLadder3 = NEXUS_SecurityKeySource_eThirdRamAskm,
    NEXUS_SecurityKeySource_eFek = NEXUS_SecurityKeySource_eKey4KeyLadder3,
    NEXUS_SecurityKeySource_eFirstRam2048,
    NEXUS_SecurityKeySource_eKey5KeyLadder3 = NEXUS_SecurityKeySource_eFirstRam2048,
    NEXUS_SecurityKeySource_eSecondRam2048,
    NEXUS_SecurityKeySource_eThirdRam2048,
    NEXUS_SecurityKeySource_eFirstRNRam1024,
    NEXUS_SecurityKeySource_eSecondRNRam1024,
    NEXUS_SecurityKeySource_eMax
} NEXUS_SecurityKeySource;

/**
Summary:
This enum defines crypto operations.
**/
typedef enum NEXUS_SecurityOperation
{
    NEXUS_SecurityOperation_eEncrypt,
    NEXUS_SecurityOperation_eDecrypt,
    NEXUS_SecurityOperation_ePassThrough, /* M2M only */
    NEXUS_SecurityOperation_eMax
}  NEXUS_SecurityOperation;

/**
Summary:
This enum defines the supported key entry types.
**/
typedef enum NEXUS_SecurityKeyType
{
    NEXUS_SecurityKeyType_eOdd,      /* ODD key */
    NEXUS_SecurityKeyType_eEven,     /* EVEN key */
    NEXUS_SecurityKeyType_eClear,    /* CLEAR key */
    NEXUS_SecurityKeyType_eIv,       /* Initial Vector (for chips which do not support per-key IVs) */
    NEXUS_SecurityKeyType_eOddAndEven,/* For configuring a single algorithm for both ODD and EVEN types at the same time */
    NEXUS_SecurityKeyType_eMax
} NEXUS_SecurityKeyType;

/**
Summary:
This enum defines the IV types when associated with specific keys.
**/
typedef enum NEXUS_SecurityKeyIVType
{
    NEXUS_SecurityKeyIVType_eNoIV,
    NEXUS_SecurityKeyIVType_eIV,         /* Normal IV */
    NEXUS_SecurityKeyIVType_eAesShortIV,
    NEXUS_SecurityKeyIVType_eMax
} NEXUS_SecurityKeyIVType;


/**
Summary:
This enum defines the algorithm configuration options for M2M/CACP keyslot.
**/
typedef enum NEXUS_SecurityAlgorithmConfigDestination
{
    NEXUS_SecurityAlgorithmConfigDestination_eCa,
    NEXUS_SecurityAlgorithmConfigDestination_eCps,
    NEXUS_SecurityAlgorithmConfigDestination_eCpd,
    NEXUS_SecurityAlgorithmConfigDestination_eRmx,
    NEXUS_SecurityAlgorithmConfigDestination_eMem2Mem,
    NEXUS_SecurityAlgorithmConfigDestination_eHdmi,
    NEXUS_SecurityAlgorithmConfigDestination_eMax
} NEXUS_SecurityAlgorithmConfigDestination;

/**
Summary:
This enum defines the algorithm configuration options on SC bits for CACP keyslot.
**/
typedef enum NEXUS_SecurityAlgorithmScPolarity
{
    NEXUS_SecurityAlgorithmScPolarity_eClear,
    NEXUS_SecurityAlgorithmScPolarity_eMpegOdd,
    NEXUS_SecurityAlgorithmScPolarity_eAtscClear = NEXUS_SecurityAlgorithmScPolarity_eMpegOdd,
    NEXUS_SecurityAlgorithmScPolarity_eEven,
    NEXUS_SecurityAlgorithmScPolarity_eOdd,
    NEXUS_SecurityAlgorithmScPolarity_eMax
} NEXUS_SecurityAlgorithmScPolarity;


/**
Summary:
This enum defines the supported Virtual Key Ladder ID for 7420B0.
**/
typedef enum NEXUS_SecurityVirtualKeyladderID
{
    NEXUS_SecurityVirtualKeyladderID_eVKL0,
    NEXUS_SecurityVirtualKeyladderID_eVKL1,
    NEXUS_SecurityVirtualKeyladderID_eVKL2,
    NEXUS_SecurityVirtualKeyladderID_eVKL3,
    NEXUS_SecurityVirtualKeyladderID_eVKL4,
    NEXUS_SecurityVirtualKeyladderID_eVKL5,
    NEXUS_SecurityVirtualKeyladderID_eVKL6,
    NEXUS_SecurityVirtualKeyladderID_eVKL7,
    NEXUS_SecurityVirtualKeyladderID_eVKLDummy,
    NEXUS_SecurityVirtualKeyladderID_eReserved9,
    NEXUS_SecurityVirtualKeyladderID_eSWKey,
    NEXUS_SecurityVirtualKeyladderID_eReserved11,
    NEXUS_SecurityVirtualKeyladderID_eReserved12,
    NEXUS_SecurityVirtualKeyladderID_eReserved13,
    NEXUS_SecurityVirtualKeyladderID_eReserved14,
    NEXUS_SecurityVirtualKeyladderID_eReserved15,
    NEXUS_SecurityVirtualKeyladderID_eMax
} NEXUS_SecurityVirtualKeyladderID;


/**
Summary:
This enum defines packet types for SC bit rewriting.
**/
typedef enum NEXUS_SecurityPacketType {
    NEXUS_SecurityPacketType_eGlobal,
    NEXUS_SecurityPacketType_eRestricted,
    NEXUS_SecurityPacketType_eMax
} NEXUS_SecurityPacketType;

/**
Summary:
This struct defines settings for algorithm configuration.
**/
typedef struct NEXUS_SecurityAlgorithmSettings
{
    NEXUS_SecurityAlgorithm algorithm;

    NEXUS_SecurityAlgorithmVariant algorithmVar; /* Cipher chain mode for selected cipher */

    NEXUS_SecurityTerminationMode terminationMode; /* Termination Type for residual block to be ciphered */

    NEXUS_SecurityIVMode          ivMode; /* Initialization Vector (IV) type - to be used by the selected cipher */

    NEXUS_SecuritySolitarySelect  solitarySelect; /* Process selection for short blocks in data packets to be ciphered */

    uint32_t                      caVendorID; /* Conditional Access Vendor ID - assigned by Broadcom for cipher key selection/computation */

    NEXUS_SecurityAskmModuleID    askmModuleID; /* Used in ASKM mode for key2 generation - one-to-one mapped into Customer Submode */

    NEXUS_SecurityOtpId           otpId; /* Source for OTP ID - used in ASKM mode for key2 generation */

    NEXUS_SecurityKey2Select      key2Select;     /* For 40-nm platforms */

    NEXUS_SecurityAesCounterSize aesCounterSize; /* This member is required for AES counter mode */

    NEXUS_SecurityDvbScrambleLevel dvbScramLevel; /* This member is only required for DVB */

    NEXUS_SecurityKeyType keyDestEntryType;

    /* This member is NOT required for CA */
    NEXUS_SecurityOperation operation; /* Appropriate settings:
                                            CA:   N/A
                                            M2M:  NEXUS_SecurityOperation_eEncrypt or NEXUS_SecurityOperation_eDecrypt or NEXUS_SecurityOperation_ePassThrough
                                            CP:   NEXUS_SecurityOperation_eEncrypt or NEXUS_SecurityOperation_eDecrypt
                                            RMX:  NEXUS_SecurityOperation_eEncrypt or NEXUS_SecurityOperation_eDecrypt
                                        */

    NEXUS_SecurityAlgorithmConfigDestination dest; /* This member is required only for CACP keyslot configuration */

    NEXUS_SecurityKeySource keySource; /* This member should only be changed from the default for keyladder operations */

    uint16_t multi2KeySelect; /* This member should only be changed from the default for Multi2 operations */

    bool enableTimestamps; /* This member should be set for m2m dma and pvr operations on transport streams with timestamps enabled */

    bool enableExtKey;
    bool enableExtIv;

    /* The following are only required for CA and CP */
    bool bRestrictEnable;
    bool bGlobalEnable;
    bool bScAtscMode;
    bool bAtscModEnable;
    bool bGlobalDropPktEnable;
    bool bRestrictDropPktEnable;
    bool bGlobalRegionOverwrite;
    bool bEncryptBeforeRave;        /* this setting is for ASKM platforms only */
    bool bRestrictSourceDropPktEnable; /* ASKM 40nm Zeus 2 only */

    /* The following are for CA/CP on legacy (pre-ASKM) systems only */
    bool bScPolarityEnable;
    bool bSynEnable;
    bool bCPDDisable;
    bool bCPSDisable;

    /* The following control rewriting the SC bits on newer platforms. */
    bool modifyScValue[NEXUS_SecurityPacketType_eMax];
    NEXUS_SecurityAlgorithmScPolarity scValue[NEXUS_SecurityPacketType_eMax];

    /* The following are only required for M2M */
    bool mscBitSelect;
    bool bDisallowGG;
    bool bDisallowGR;
    bool bDisallowRG;
    bool bDisallowRR;

    /* The following are only required for CA/CP on 40nm chips */
    uint8_t keyOffset;
    uint8_t ivOffset;
    uint8_t mscLengthSelect;
    NEXUS_Security_CustomerMode customerType;
    uint8_t dvbCsa2keyCtrl;
    uint8_t dvbCsa2ivCtrl;
    bool dvbCsa2modEnabled;
    uint8_t dvbCsa3dvbcsaVar;
    uint8_t dvbCsa3permutation;
    bool dvbCsa3modXRC;
    /* The following is for ASKM 40nm Zeus 2 only. This array corresponds to bGpipePackets2Rregion and bRpipePackets2Rregion. */
    bool bRoutePipeToRestrictedRegion[NEXUS_SecurityPacketType_eMax];

    /* The following are legacy settings and should generally not be used */
    bool testKey2Select; /* Test Key2 to be used for cipher key generation when set. (For debugging purposes only) */
} NEXUS_SecurityAlgorithmSettings;

/**
Summary:
This struct defines the clear key structure.

Description:
This structure contains the clear parameters which will be filled
by the caller of NEXUS_Security_LoadClearKey.

See Also:
NEXUS_Security_LoadClearKey
**/
typedef struct NEXUS_SecurityClearKey
{
    unsigned int keySize;   /* keySize - size of the key */
    NEXUS_SecurityKeyType keyEntryType; /* keyEntryType - key entry type */
    NEXUS_SecurityKeyIVType keyIVType;  /* IV type for this key */
    NEXUS_SecurityAlgorithmConfigDestination dest;/* This member is required only for CACP keyslot configuration */
    NEXUS_SecurityAlgorithmScPolarity sc01Polarity[NEXUS_SecurityPacketType_eMax]; /* Packets with SC bits 0x01 will be treated as this polarity */

    uint8_t keyData[64];    /* keyData - key data array */
} NEXUS_SecurityClearKey;

/**
Summary:
This struct defines the multi2 configuration structure.

Description:
This structure contains the parameters which will be filled
by the caller of NEXUS_Security_ConfigMulti2.

See Also:
NEXUS_Security_ConfigMulti2
**/
typedef struct NEXUS_SecurityMulti2Settings
{
    uint16_t multi2KeySelect;
    uint8_t multi2Rounds;
    uint8_t multi2SysKey[32];
} NEXUS_SecurityMulti2Settings;

/**
Summary:
**/
typedef struct NEXUS_SecurityKeySlotSettings {
    NEXUS_SecurityEngine keySlotEngine;
    NEXUS_SecurityKeySource keySlotSource;
    NEXUS_SecurityKeySlotType keySlotType;
} NEXUS_SecurityKeySlotSettings;


/**
Summary:
This enum defines the key to be invalidated, source, destination, or both.
**/
typedef enum NEXUS_SecurityInvalidateKeyFlag
{
    NEXUS_SecurityInvalidateKeyFlag_eSrcKeyOnly,
    NEXUS_SecurityInvalidateKeyFlag_eDestKeyOnly,
    NEXUS_SecurityInvalidateKeyFlag_eAllKeys,
    NEXUS_SecurityInvalidateKeyFlag_eMax
} NEXUS_SecurityInvalidateKeyFlag;


/**
Summary:
This structure provides information for CA key invalidation

Description:
See Also:
NEXUS_Security_InvalidateKey
**/
typedef struct NEXUS_SecurityInvalidateKeySettings
{
    NEXUS_SecurityInvalidateKeyFlag             invalidateKeyType;
    NEXUS_SecurityVirtualKeyladderID            virtualKeyLadderID;
    NEXUS_SecurityKeySource                     keySrc;
    NEXUS_SecurityAlgorithmConfigDestination    keyDestBlckType;
    NEXUS_SecurityKeyType                       keyDestEntryType;
} NEXUS_SecurityInvalidateKeySettings;


/**
Summary:
This error code is used to pass HSM error codes to the application.
The value from HSM is embedded in the lowest byte via OR.
**/
#define NEXUS_SECURITY_HSM_ERROR NEXUS_MAKE_ERR_CODE(0x109, 0)



#ifdef __cplusplus
}
#endif

#endif
