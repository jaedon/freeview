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
 * $brcm_Workfile: nexus_otpmsp.h $
 * $brcm_Revision: 4 $
 * $brcm_Date: 9/19/12 11:54a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/extensions/security/otpmsp/include/nexus_otpmsp.h $
 * 
 * 4   9/19/12 11:54a atruong
 * SWSECURITY-53: Expand OTP/MSP programming mode enum
 * 
 * 3   3/5/12 1:20p atruong
 * SWSECURITY-113: Secure RSA2 Support for 40-nm Platforms
 * 
 * 2   10/13/11 9:17a atruong
 * SWSECURITY-69: BSP Zeus 2.0 FW Support
 * 
 * 1   1/28/11 7:11p atruong
 * SW7422-235: Enable Nexus Security extension modules for 97422
 *
 ******************************************************************************/
#ifndef NEXUS_OTPMSP_H__
#define NEXUS_OTPMSP_H__

#include "nexus_security_datatypes.h"
#include "nexus_security.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
Summary:
This enum defines types as passed by the MIPS to the 8051 that say whether the programming
command is in bit mode or array mode

Description:
This enumeration defines all the supported programming modes.
*****************************************************************************/
typedef enum NEXUS_OtpMspCmdProgMode
{
    NEXUS_OtpMspCmdProgMode_eEnum    = 0x10112,
    NEXUS_OtpMspCmdProgMode_eEnumHal = 0x34,
    NEXUS_OtpMspCmdProgMode_eBit     = 0x78
} NEXUS_OtpMspCmdProgMode;


/*****************************************************************************
Summary:
*****************************************************************************/
typedef enum NEXUS_OtpCmdMsp
{
	NEXUS_OtpCmdMsp_eReserved0								= 0,	/* previously eFlashType*/
	NEXUS_OtpCmdMsp_eReserved1							=	1,
	NEXUS_OtpCmdMsp_eReserved2							=	2,
	NEXUS_OtpCmdMsp_eReserved3							=	3,
	NEXUS_OtpCmdMsp_eResetDelayTimerDisable 				= 4,
	NEXUS_OtpCmdMsp_eCrLockEnable							= 5,
	NEXUS_OtpCmdMsp_eCrDisable								= 6,
	NEXUS_OtpCmdMsp_eReserved7							=	7,
	NEXUS_OtpCmdMsp_ePciClientProtect						= 8,
	NEXUS_OtpCmdMsp_ePcieHostProtect						= 9,
	NEXUS_OtpCmdMsp_ePcieClientProtect						= 10,
	NEXUS_OtpCmdMsp_eSecurityPinBoundaryScanProtect 		= 11,
	NEXUS_OtpCmdMsp_eGenericPinBoundaryScanProtect			= 12,
	NEXUS_OtpCmdMsp_eReserved13 						=	13,
	NEXUS_OtpCmdMsp_eReserved14 						=	14,
	NEXUS_OtpCmdMsp_eReserved15 						=	15,
	NEXUS_OtpCmdMsp_eReserved16 						=	16,
	NEXUS_OtpCmdMsp_eEnforceDramScramblerPause				= 17,
	NEXUS_OtpCmdMsp_eForceDramScrambler 					= 18,
	NEXUS_OtpCmdMsp_eReserved19 						=	19,
	NEXUS_OtpCmdMsp_eReserved20 						=	20,
	NEXUS_OtpCmdMsp_eReserved21 						=	21,
	NEXUS_OtpCmdMsp_eReserved22 						=	22,
	NEXUS_OtpCmdMsp_eReserved23 						=	23,
	NEXUS_OtpCmdMsp_eReserved24 						=	24,
	NEXUS_OtpCmdMsp_eReserved25 						=	25,
	NEXUS_OtpCmdMsp_eReserved26 						=	26,
	NEXUS_OtpCmdMsp_eReserved27 						=	27,
	NEXUS_OtpCmdMsp_eReserved28 						=	28,
	NEXUS_OtpCmdMsp_eReserved29 						=	29,
	NEXUS_OtpCmdMsp_eReserved30 						=	30,
	NEXUS_OtpCmdMsp_eReserved31 						=	31,
	NEXUS_OtpCmdMsp_eReserved32 						=	32,
	NEXUS_OtpCmdMsp_eReserved33 						=	33,
	NEXUS_OtpCmdMsp_eReserved34 						=	34,
	NEXUS_OtpCmdMsp_eDataSectionLockByte					= 35,
	NEXUS_OtpCmdMsp_eDataSectionReadProtectBits 			= 36,
	NEXUS_OtpCmdMsp_eReserved37 						=	37,
	NEXUS_OtpCmdMsp_eReserved38 						=	38,
	NEXUS_OtpCmdMsp_eReserved39 						=	39,
	NEXUS_OtpCmdMsp_eReserved40 						=	40,
	NEXUS_OtpCmdMsp_eReserved41 						=	41,
	NEXUS_OtpCmdMsp_eReserved42 						=	42,
	NEXUS_OtpCmdMsp_eAskmStbOwnerId 						= 43,
	NEXUS_OtpCmdMsp_eReserved44 						=	44,
	NEXUS_OtpCmdMsp_eCaControlBits							= 45,
	NEXUS_OtpCmdMsp_eCpM2MControlBits						= 46,
	NEXUS_OtpCmdMsp_eCaKeyLadderDisable 					= 47,
	NEXUS_OtpCmdMsp_eCpKeyLadderDisable 					= 48,
	NEXUS_OtpCmdMsp_eCustKeytoCaKeyLadderDisable			= 49,
	NEXUS_OtpCmdMsp_eCustKeytoCpKeyLadderDisable			= 50,
	NEXUS_OtpCmdMsp_eReserved51 						=	51,
	NEXUS_OtpCmdMsp_eReserved52 						=	52,
	NEXUS_OtpCmdMsp_eReserved53 						=	53,
	NEXUS_OtpCmdMsp_eReserved54 						=	54,
	NEXUS_OtpCmdMsp_eReserved55 						=	55,
	NEXUS_OtpCmdMsp_eReserved56 						=	56,
	NEXUS_OtpCmdMsp_e1DesKeyLadderDisable					= 57,
	NEXUS_OtpCmdMsp_eRouted3DesKeyK1K2CheckEnable			= 58,
	NEXUS_OtpCmdMsp_eCaSoftwareKeyDisable					= 59,
	NEXUS_OtpCmdMsp_eCpAesEcbMscSoftwareKeyDisable			= 60,
	NEXUS_OtpCmdMsp_eCpM2mDesSoftwareKeyDisable 			= 61,
	NEXUS_OtpCmdMsp_eCpM2m3DesSoftwareKeyDisable			= 62,
	NEXUS_OtpCmdMsp_eCpM2mAesSoftwareKeyDisable 			= 63,
	NEXUS_OtpCmdMsp_eCpM2mAesCounterSoftwareKeyDisable		= 64,
	NEXUS_OtpCmdMsp_eM2mC2CssSoftwareKeyDisable 			= 65,
	NEXUS_OtpCmdMsp_eM2mM6SoftwareKeyDisable				= 66,
	NEXUS_OtpCmdMsp_eReserved67 						=	67,
	NEXUS_OtpCmdMsp_eReserved68 						=	68,
	NEXUS_OtpCmdMsp_eReserved69 						=	69,
	NEXUS_OtpCmdMsp_eReserved70 						=	70,
	NEXUS_OtpCmdMsp_eReserved71 						=	71,
	NEXUS_OtpCmdMsp_eReserved72 						=	72,
	NEXUS_OtpCmdMsp_ePublicKey0Index						= 73,
	NEXUS_OtpCmdMsp_eSecureBootEnable						= 74,
	NEXUS_OtpCmdMsp_eReserved75 						=	75,
	NEXUS_OtpCmdMsp_eReserved76 						=	76,
	NEXUS_OtpCmdMsp_eReserved77 						=	77,
	NEXUS_OtpCmdMsp_eReserved78 						=	78,
	NEXUS_OtpCmdMsp_eReserved79 						=	79,
	NEXUS_OtpCmdMsp_eReserved80 						=	80,
	NEXUS_OtpCmdMsp_eHostOutOfResetOnErrorDisable			= 81,
	NEXUS_OtpCmdMsp_eReserved82 						=	82,
	NEXUS_OtpCmdMsp_eBootSequenceSelect 					= 83,
	NEXUS_OtpCmdMsp_eReserved84 						=	84,
	NEXUS_OtpCmdMsp_eKey0PrimeSigningRights 				= 85,
	NEXUS_OtpCmdMsp_eKey0SigningRights						= 86,
	NEXUS_OtpCmdMsp_eSystemEpoch							= 87,
	NEXUS_OtpCmdMsp_eReserved88 						=	88,
	NEXUS_OtpCmdMsp_eBseckEnable							= 89,
	NEXUS_OtpCmdMsp_eReserved90 						=	90,
	NEXUS_OtpCmdMsp_eReserved91 						=	91,
	NEXUS_OtpCmdMsp_eReserved92 						=	92,
	NEXUS_OtpCmdMsp_eReserved93 						=	93,
	NEXUS_OtpCmdMsp_eMarketId								= 94,
	NEXUS_OtpCmdMsp_eMpodDisable							= 95,
	NEXUS_OtpCmdMsp_eReserved96 						=	96,
	NEXUS_OtpCmdMsp_eReserved97 						=	97,
	NEXUS_OtpCmdMsp_eReserved98 						=	98,
	NEXUS_OtpCmdMsp_eReserved99 						=	99,
	NEXUS_OtpCmdMsp_eReserved100							=	100,
	NEXUS_OtpCmdMsp_eReserved101							=	101,
	NEXUS_OtpCmdMsp_eReserved102							=	102,
	NEXUS_OtpCmdMsp_eReserved103							=	103,
	NEXUS_OtpCmdMsp_eReserved104							=	104,
	NEXUS_OtpCmdMsp_eReserved105							=	105,
	NEXUS_OtpCmdMsp_eReserved106							=	106,
	NEXUS_OtpCmdMsp_eReserved107							=	107,
	NEXUS_OtpCmdMsp_eReserved108							=	108,
	NEXUS_OtpCmdMsp_eReserved109							=	109,
	NEXUS_OtpCmdMsp_eReserved110							=	110,
	NEXUS_OtpCmdMsp_eReserved111							=	111,
	NEXUS_OtpCmdMsp_eReserved112							=	112,
	NEXUS_OtpCmdMsp_eReserved113							=	113,
	NEXUS_OtpCmdMsp_eReserved114							=	114,
	NEXUS_OtpCmdMsp_eReserved115							=	115,
	NEXUS_OtpCmdMsp_eReserved116							=	116,
	NEXUS_OtpCmdMsp_eReserved117							=	117,
	NEXUS_OtpCmdMsp_eReserved118							=	118,
	NEXUS_OtpCmdMsp_eReserved119							=	119,
	NEXUS_OtpCmdMsp_eReserved120							=	120,

	NEXUS_OtpCmdMsp_eReserved121							=	121,
	NEXUS_OtpCmdMsp_eReserved122							=	122,
	NEXUS_OtpCmdMsp_eReserved123							=	123,
	NEXUS_OtpCmdMsp_eReserved124							=	124,
	NEXUS_OtpCmdMsp_eReserved125							=	125,
	NEXUS_OtpCmdMsp_eReserved126							=	126,

	NEXUS_OtpCmdMsp_eReserved127							=	127,
	NEXUS_OtpCmdMsp_eReserved128							=	128,
	NEXUS_OtpCmdMsp_eReserved129							=	129,
	NEXUS_OtpCmdMsp_eReserved130							=	130,
	NEXUS_OtpCmdMsp_eReserved131							=	131,
	NEXUS_OtpCmdMsp_ePciGwinDisable 						= 132,
	NEXUS_OtpCmdMsp_ePcieGwinDisable						= 133,
	NEXUS_OtpCmdMsp_ePciMwinDisable 						= 134,
	NEXUS_OtpCmdMsp_ePciMwinRestrictEnable					= 135,
	NEXUS_OtpCmdMsp_ePcieMwinDisable						= 136,
	NEXUS_OtpCmdMsp_ePcieMwinRestrictEnable 				= 137,
	NEXUS_OtpCmdMsp_eCaAesEcbMscSoftwareKeyDisable			= 138,
	NEXUS_OtpCmdMsp_eReserved139							=	139,
	NEXUS_OtpCmdMsp_eReserved140							=	140,
	NEXUS_OtpCmdMsp_eReserved141							= 141,		/* previously eOtpOptionSpare*/
	NEXUS_OtpCmdMsp_eReserved142							=	142,
	NEXUS_OtpCmdMsp_eReserved143							=	143,
	NEXUS_OtpCmdMsp_eReserved144							=	144,
	NEXUS_OtpCmdMsp_eReserved145							=	145,
	NEXUS_OtpCmdMsp_eReserved146							=	146,
	NEXUS_OtpCmdMsp_eReserved147							=	147,
	NEXUS_OtpCmdMsp_eReserved148							=	148,
	NEXUS_OtpCmdMsp_eReserved149							=	149,
	NEXUS_OtpCmdMsp_eReserved150							=	150,
	NEXUS_OtpCmdMsp_eReserved151							=	151,
/*-Zeus 2.0 additions-*/
	NEXUS_OtpCmdMsp_eDramScramblerKeyReuseDisable			= 152,
	NEXUS_OtpCmdMsp_eMMCBootDisable 						= 153,
	NEXUS_OtpCmdMsp_eFekDecryptionRootKey					= 154,
	NEXUS_OtpCmdMsp_eFlashDecryptionActivation				= 155,
	NEXUS_OtpCmdMsp_eReserved156							=	156,
	NEXUS_OtpCmdMsp_eAesDpaStallPercentage					= 157,
	NEXUS_OtpCmdMsp_eAesDpaStallCountHigh					= 158,
	NEXUS_OtpCmdMsp_eAesDpaRoundStallingEnable				= 159,
	NEXUS_OtpCmdMsp_eReserved160							=	160,
	NEXUS_OtpCmdMsp_eReserved161							=	161,
	NEXUS_OtpCmdMsp_eReserved162							=	162,
	NEXUS_OtpCmdMsp_eReserved163							=	163,
	NEXUS_OtpCmdMsp_eReserved164							=	164,
	NEXUS_OtpCmdMsp_eReserved165							=	165,
	NEXUS_OtpCmdMsp_eReserved166							=	166,
	NEXUS_OtpCmdMsp_eReserved167							=	167,
	NEXUS_OtpCmdMsp_eReserved168							=	168,
	NEXUS_OtpCmdMsp_eReserved169							=	169,
	NEXUS_OtpCmdMsp_eReserved170							=	170,
	NEXUS_OtpCmdMsp_eReserved171							=	171,
	NEXUS_OtpCmdMsp_eReserved172							=	172,
	NEXUS_OtpCmdMsp_eReserved173							=	173,
	NEXUS_OtpCmdMsp_eReserved174							=	174,
	NEXUS_OtpCmdMsp_eReserved175							=	175,
	NEXUS_OtpCmdMsp_eReserved176							=	176,
	NEXUS_OtpCmdMsp_eReserved177							=	177,
	NEXUS_OtpCmdMsp_eReserved178							=	178,
	NEXUS_OtpCmdMsp_eReserved179							=	179,
	NEXUS_OtpCmdMsp_eReserved180							=	180,
	NEXUS_OtpCmdMsp_eReserved181							=	181,
	NEXUS_OtpCmdMsp_eReserved182							=	182,
	NEXUS_OtpCmdMsp_eReserved183							=	183,
	NEXUS_OtpCmdMsp_eReserved184							=	184,
	NEXUS_OtpCmdMsp_eStrictEntropyMappingEnable 			= 185,
	NEXUS_OtpCmdMsp_eReserved186							=	186,
	NEXUS_OtpCmdMsp_eReserved187							=	187,
	NEXUS_OtpCmdMsp_eReserved188							=	188,
	NEXUS_OtpCmdMsp_eReserved189							=	189,
	NEXUS_OtpCmdMsp_eDualCoreAcpuMode						= 190,
	NEXUS_OtpCmdMsp_eSecureSandboxEnable					= 191,
	NEXUS_OtpCmdMsp_eWebCpuVerifyEnable 					= 192,
	NEXUS_OtpCmdMsp_eRaagaAVerifyEnable 					= 193,
	NEXUS_OtpCmdMsp_eRaagaBVerifyEnable 					= 194,
	NEXUS_OtpCmdMsp_eHwKeyLadderDisable 					= 195,
	NEXUS_OtpCmdMsp_eExternalKeyTableDisable				= 196,
	NEXUS_OtpCmdMsp_eOneWayProgramEnable					= 197,
	NEXUS_OtpCmdMsp_eReserved198							=	198,
	NEXUS_OtpCmdMsp_eMax									= 199
} NEXUS_OtpCmdMsp;



/*****************************************************************************
Summary:
Select bits to be read
*****************************************************************************/
typedef enum NEXUS_OtpCmdReadRegister
{
	NEXUS_OtpCmdReadRegister_eMc0S_Reserved0                         = 0,
	NEXUS_OtpCmdReadRegister_eMc0S_Reserved1                         = 1,

	NEXUS_OtpCmdReadRegister_eMc0S_Reserved2 						= 2,
	NEXUS_OtpCmdReadRegister_eMc0S_Reserved3 						= 3,

	NEXUS_OtpCmdReadRegister_eMc0S_Reserved4 						= 4,
	NEXUS_OtpCmdReadRegister_eMc0S_Reserved5 						= 5,

    NEXUS_OtpCmdReadRegister_eKeyMc0_CustomerMode                    = 6,
	NEXUS_OtpCmdReadRegister_eKeyMc0_Reserved7 						= 7,
    NEXUS_OtpCmdReadRegister_eKeyMc0_DeobfuscationEnable             = 8,
    NEXUS_OtpCmdReadRegister_eKeyMc0_BlackBoxId                      = 9,
    NEXUS_OtpCmdReadRegister_eKeyMc0_CaKeyLadderDisallow             = 10,
    NEXUS_OtpCmdReadRegister_eKeyMc0_CpKeyLadderDisallow             = 11,
    NEXUS_OtpCmdReadRegister_eKeyMc0_Gp1KeyLadderDisallow            = 12,
    NEXUS_OtpCmdReadRegister_eKeyMc0_Gp2KeyLadderDisallow            = 13,
	NEXUS_OtpCmdReadRegister_eKeyMc0_Reserved14 						= 14,
    NEXUS_OtpCmdReadRegister_eKeyID                                  = 15,
    NEXUS_OtpCmdReadRegister_eKeyHash                                = 16,
    NEXUS_OtpCmdReadRegister_eBseckHashCrc                           = 17,
	NEXUS_OtpCmdReadRegister_eMc0S_Reserved18 					    = 18,
    NEXUS_OtpCmdReadRegister_eMc0S_FixedDeobfuscationVariantEnable   = 19,
    NEXUS_OtpCmdReadRegister_eKeyMc0_RootKeySwapDisallow             = 20,
	NEXUS_OtpCmdReadRegister_eKeyMc0_Reserved21						= 21,
    NEXUS_OtpCmdReadRegister_eMax
} NEXUS_OtpCmdReadRegister;


/* select otp key type for the field to be read */
typedef enum NEXUS_OtpKeyType
{
    NEXUS_OtpKeyType_eA,
    NEXUS_OtpKeyType_eB,
    NEXUS_OtpKeyType_eC,
    NEXUS_OtpKeyType_eD,
    NEXUS_OtpKeyType_eE,
    NEXUS_OtpKeyType_eF,

    NEXUS_OtpKeyType_eSize

} NEXUS_OtpKeyType ;


#define NEXUS_OTP_KEY_ID_LEN            8
#define NEXUS_MSP_DATA_LEN              4
#define NEXUS_MSP_SIGNATURE_DATA_LEN    20          /* 16 for legacy, 20 for ASKM */
#define NEXUS_MSP_OUTPUT_DATA_LEN       4
#define NEXUS_OTP_DATASECTION_LEN       (32)        /* in byte now, may change to word32*/
#define NEXUS_OTP_CRC_LEN               8



/*****************************************************************************
Summary:
*****************************************************************************/
typedef struct NEXUS_ReadOtpIO {
    unsigned char       otpKeyIdBuf[NEXUS_OTP_KEY_ID_LEN];      /* Buffer to hold OTP Key ID for the current OTP Key ID read request */
    unsigned int        otpKeyIdSize;                           /* Actual size of Otp read buffer */
} NEXUS_ReadOtpIO;

/*****************************************************************************
Summary:
This function returns one OTP key identifiers or one MC0 OTP value.

Description:
This function shall be used to read either OTP key identifier or other OTP field value.
Depends on the access control matrix, only certain OTP fields can be read in a specific
customer mode.  Note that this function can only read back one OTP value at a time.

Performance and Timing:
This is a synchronous/blocking function that would not return until it is done or failed.

See Also:
NEXUS_Security_ReadMSP
*****************************************************************************/
NEXUS_Error NEXUS_Security_ReadOTP(
    NEXUS_OtpCmdReadRegister    readOtpEnum,
	NEXUS_OtpKeyType            keyType,
    NEXUS_ReadOtpIO             *pReadOtpIO         /* [out] structure holding read OTP buffer and size */
    );


/**************************************************************************************************
Summary:

Description:
Structure that defines which MSP field to program, with what data,  using what mask and proper mode,  and holds the returned status
of a programming request

See Also:
NEXUS_OtpMsp_ProgramMSP
**************************************************************************************************/
typedef struct NEXUS_ProgramMspIO {
    NEXUS_OtpMspCmdProgMode    progMode;    /* This field should contain the value of NEXUS_OtpMspCmdProgMode
           for the bits to be programmed. This is a sanity check on the command.
           The value NEXUS_OtpMspCmdProgMode_Enum  specifies that command enum mode
           programming is used.
        */

    NEXUS_OtpCmdMsp            progMspEnum; /* specifies which MSP bits to program. The values this field can take are specified by the
          typedef enum NEXUS_OtpCmdMsp in the share C header file.  Each chip will have different enums
          and customers will only have access to the files for the chips that they use.
        */

    unsigned char               dataBitLen; /* number of bits of the MSP enum to program, from 1 to 32 */

    unsigned char               dataBitMask [NEXUS_MSP_DATA_LEN];   /* 0x0000_0001 to 0xFFFF_FFFF
          A value 1 in a bit position specifies that the data value at that bit position is to be programmed.
          BSP will not program bits that have a zero bit in the mask.
          For example 0x0F means to program the 4 LSBits of an enum.
          For example 0x8F means to program the bit 7 and 4 LSBits of an enum.
        */
    unsigned char               mspData[NEXUS_MSP_DATA_LEN];    /* the value that needs to be programmed */

} NEXUS_ProgramMspIO;



/*****************************************************************************
Summary:
This function allows the programming of each of the field programmable
OTP (MSP) bits.

Description:
This function allows the programming of each of the field programmable
OTP (MSP) bits.  Based on the Access Control Matrix (ACL), programming of the
bit is allowed or disallowed.

Performance and Timing:
This is a synchronous/blocking function that would not return until it is done or failed.

See Also:
NEXUS_Security_ProgramOTP
*****************************************************************************/
NEXUS_Error NEXUS_Security_ProgramMSP(
    const NEXUS_ProgramMspIO    *pProgMspIO
    );


#define NEXUS_MSP_KEY3_DATA_LEN        16
#define NEXUS_MSP_KEY4_DATA_LEN        16


/**************************************************************************************************
Summary:

Description:
Structure that defines which MSP field to read and its required key/data if command authentication is needed by BSP,
and holds the returned value of the MSP field

See Also:
NEXUS_OtpMsp_ReadMSP
**************************************************************************************************/
typedef struct NEXUS_ReadMspParms
{
    NEXUS_OtpCmdMsp             readMspEnum;     /* which MSP to read */

} NEXUS_ReadMspParms;


/*****************************************************************************
Summary:
*****************************************************************************/
typedef struct NEXUS_ReadMspIO {
    unsigned char       mspDataBuf[NEXUS_MSP_OUTPUT_DATA_LEN];      /* Buffer to hold MSP data for the current MSP read request   */
	unsigned char		lockMspDataBuf[NEXUS_MSP_OUTPUT_DATA_LEN];  /* Buffer to hold lock MSP data for the current read request       */
	                                                                /* This is used to tell if value '0' is programmed or unprogrammed */
    unsigned int        mspDataSize;                                /* Actual size of MSP output buffer */
} NEXUS_ReadMspIO;

/*****************************************************************************
Summary:
This function returns one MSP value.

Description:

This function shall be used to read MSP field value. Depends on the access control matrix, only
certain MSP fields can be read in a specific customer mode.  Note that this function can only
read back one MSP value at a time.

Performance and Timing:
This is a synchronous/blocking function that would not return until it is done or failed.

See Also:
NEXUS_Security_ReadOTP
*****************************************************************************/
NEXUS_Error      NEXUS_Security_ReadMSP(
    const NEXUS_ReadMspParms    *pReadMspParms,     /* structure holding input parameters */
    NEXUS_ReadMspIO             *pReadMspIO         /* [out] structure holding read MSP buufer and size */
    );

/*****************************************************************************
Summary:
*****************************************************************************/
typedef enum NEXUS_OtpDataSection
{
    NEXUS_OtpDataSection_e0     = 0x0,
    NEXUS_OtpDataSection_e1     = 0x1,
    NEXUS_OtpDataSection_e2     = 0x2,
    NEXUS_OtpDataSection_e3     = 0x3,
    NEXUS_OtpDataSection_e4     = 0x4,
    NEXUS_OtpDataSection_e5     = 0x5,
    NEXUS_OtpDataSection_e6     = 0x6,
    NEXUS_OtpDataSection_e7     = 0x7,
    NEXUS_OtpDataSection_eMax   = 0x8

} NEXUS_OtpDataSection;

/*****************************************************************************
Summary:
*****************************************************************************/
typedef struct NEXUS_ReadDataSectIO {
    unsigned char       dataSectBuf[NEXUS_OTP_DATASECTION_LEN]; /* Buffer to hold DataSection data for the current DataSect read request */
    unsigned int        dataSectSize;                           /* Actual size of data section read */
} NEXUS_ReadDataSectIO;


/*****************************************************************************
Summary:
This function returns one 32-byte data section value.

Description:
There are total of 8 32-byte data sections. This function shall be used to read each 32-byte data section.

Performance and Timing:
This is a synchronous/blocking function that would not return until it is done or failed.

See Also:
NEXUS_Security_ProgramDataSect
*****************************************************************************/
NEXUS_Error NEXUS_Security_ReadDataSect(
    NEXUS_OtpDataSection        readDsEnum,       /* NEXUS_OtpDataSection enum of which data section to be read */
    NEXUS_ReadDataSectIO       *pReadDataSectIO    /* [out] structure holding read datasect buffer and size */
    );



#define NEXUS_OTP_DATASECTION_CRC_LEN               4
#define NEXUS_OTP_DATASECTIONPROG_MODE              0x00010112


/**************************************************************************************************
Summary:

Description:
Structure that defines which OTP data section to program with what data using a proper mode, and holds
the returned status

See Also:
NEXUS_Security_ProgramDataSect
**************************************************************************************************/
typedef struct NEXUS_ProgramDataSectIO {
    NEXUS_OtpDataSection       progDsEnum; /* select which OTP data section to program, between NEXUS_OtpDataSection_e0 ~
            NEXUS_OtpDataSection_e7 */

    unsigned char               dataSectData[NEXUS_OTP_DATASECTION_LEN]; /* provide the actual 32-byte data to be programmed into the specified OTP data section*/

    unsigned char               crc[NEXUS_OTP_DATASECTION_CRC_LEN]; /* provide the crc of data section */

    uint32_t                    mode; /* a kind of program magic number, must be NEXUS_OTP_DATASECTIONPROG_MODE (0x00010112).
            if it is not this value the command will be rejected by  a sanity check at BSP */

    uint32_t                    padding; /* padding field, to use the entire structure with HSM PI  */
} NEXUS_ProgramDataSectIO;


/*****************************************************************************
Summary:
This function is used to program one 32-byte data section value.

Description:
There are total of 8 32-byte data sections. This function shall be used to write one 32-byte data
section.

Performance and Timing:
This is a synchronous/blocking function that would not return until it is done or failed.

See Also:
NEXUS_Security_ReadDataSect
*****************************************************************************/
NEXUS_Error NEXUS_Security_ProgramDataSect(
    const NEXUS_ProgramDataSectIO   *pProgDataSectIO
    );

#ifdef __cplusplus
}
#endif

#endif
