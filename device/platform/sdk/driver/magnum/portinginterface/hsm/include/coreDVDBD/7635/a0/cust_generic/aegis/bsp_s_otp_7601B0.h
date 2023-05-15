/***************************************************************************
 *     Copyright (c) 2005-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * OTP CMD layer abstraction.
 *
 * 
 * 2   12/1/08 11:59a aratim
 * PR7 : Replaced FastCheckEnable MSP bit with LegacySwizzleDisable.
 * 
 * 1   11/7/08 4:23p aratim
 * PR46: Updated RDB and compile flags for 7443B0.
 * 
 * 4   9/22/08 9:56a aratim
 * PR7 : FIRMWARE: BSECK: Update Firmware code for BCM7443 OTP MAP.
 * 
 * 3   5/8/08 4:08p bryanl
 * PR7: BSECK: Update Firmware code for BCM7443 OTP MAP
 * 
 * 2   5/5/08 2:49p bryanl
 * PR7: BSECK: Update Firmware code for BCM7443 OTP MAP
 * 
 * 1   1/22/08 4:31p aratim
 * PR3: 7443A0 Initial Compile
 * 
 * 
 ***************************************************************************/
 
#ifndef BSP_S_OTP_7601B0_H__
#define BSP_S_OTP_7601B0_H__

typedef enum BCMD_Otp_CmdMsp_e      
{

    BCMD_Otp_CmdMsp_eReserved0           = 0,
    BCMD_Otp_CmdMsp_eReserved1           = 1,
    
    BCMD_Otp_CmdMsp_eBackgroundCheckEn   = 2,
    BCMD_Otp_CmdMsp_eFlashRegion0_2      = 3,  /* 3 bits */
    BCMD_Otp_CmdMsp_eBootMemChkEn        = 4,
    BCMD_Otp_CmdMsp_eExportCntrlEnable   = 5,
    BCMD_Otp_CmdMsp_ePubKeyIndx0_3       = 6,  /* 4 bits */
    
    BCMD_Otp_CmdMsp_eStopB               = 7,
    BCMD_Otp_CmdMsp_eStartB              = 8,
    BCMD_Otp_CmdMsp_eStopA               = 9,
    BCMD_Otp_CmdMsp_eStartA              = 10,
    
    BCMD_Otp_CmdMsp_eReserved11          = 11,
    BCMD_Otp_CmdMsp_eReserved12          = 12,
    
    BCMD_Otp_CmdMsp_eLockByte0_7         = 13,  /* 8 bits */
    
    BCMD_Otp_CmdMsp_eLockBitC            = 14,
    BCMD_Otp_CmdMsp_eCrOtpBit0           = 15,
    BCMD_Otp_CmdMsp_eCrOtpBit1           = 16,
    BCMD_Otp_CmdMsp_ePciHostProtectWithCR = 17,
    BCMD_Otp_CmdMsp_ePciClientProtectWithCR = 18,
    BCMD_Otp_CmdMsp_eTestPortProtectWithCR = 19,
    BCMD_Otp_CmdMsp_eForceDramScrambler  = 20,
    
    BCMD_Otp_CmdMsp_eReserved21          = 21,
    BCMD_Otp_CmdMsp_eReserved22          = 22,
    BCMD_Otp_CmdMsp_eReserved23          = 23,
    BCMD_Otp_CmdMsp_eReserved24          = 24,

    BCMD_Otp_CmdMsp_eCrcHardwareEn       = 25,
    BCMD_Otp_CmdMsp_eOtpProductId0_3     = 26,  /* 4 bits*/
    
    BCMD_Otp_CmdMsp_eOtpCtrlBit0_30      = 27,  /* 31 bits*/
    
    BCMD_Otp_CmdMsp_eReserved28          = 28,
    
    BCMD_Otp_CmdMsp_eOtpDebugDisable0_3  = 29, /* 4 bits */
    BCMD_Otp_CmdMsp_eOtpClkDisable0_3    = 30, /* 4 bits */
    BCMD_Otp_CmdMsp_eOtpModeConfigExt0_31 = 31, /* 32 bits */
    
    BCMD_Otp_CmdMsp_eReserved32          = 32,
    
    BCMD_Otp_CmdMsp_eReserved33          = 33,
    BCMD_Otp_CmdMsp_eReserved34          = 34,

    BCMD_Otp_CmdMsp_eReserved35          = 35,

    BCMD_Otp_CmdMsp_eOtpCtrlBitSet2      = 36, /* 12 bits */

    BCMD_Otp_CmdMsp_eReserved37          = 37,

    BCMD_Otp_CmdMsp_ePatchRAMDisable     = 38, /* 4 bits */
   
    BCMD_Otp_CmdMsp_eOtpReserved39       = 39, 
   
    BCMD_Otp_CmdMsp_eReserved40          = 40,

    BCMD_Otp_CmdMsp_eOtpCtrlBit44_eDisableCaKeyLadder = 41, /* 1 bit */

    BCMD_Otp_CmdMsp_eReserved42          = 42,

    BCMD_Otp_CmdMsp_eReserved43          = 43, /* 1 bit */

    BCMD_Otp_CmdMsp_eReserved44          = 44,

    BCMD_Otp_CmdMsp_eFastRegionCheckSkip = 45, /* 1 bit */
    BCMD_Otp_CmdMsp_eDisableMICH         = 46, /* 1 bit */
    
    BCMD_Otp_CmdMsp_eDisableSecretKeyToM2M = 47, /* 1 bit */
    BCMD_Otp_CmdMsp_eDisableUpdateSecretKeyToBSP = 48, /* 1 bit */
    BCMD_Otp_CmdMsp_eDisableOTPKeyAsRootKey = 49, /* 1 bit */
    
    BCMD_Otp_CmdMsp_eFastRegionCheckSkipAudioVideo = 50, /* 1 bit */
    
    BCMD_Otp_CmdMsp_eM2mWmdrmPd          = 51, /* 1 bit */
    BCMD_Otp_CmdMsp_eM2mHddvdBluray      = 52, /* 1 bit */
    BCMD_Otp_CmdMsp_eOtpCtrlBitSet3      = 53, /* 10 bits */
    
    BCMD_Otp_CmdMsp_eFlashPublicKeyAuthenticate = 54, /* 54 - 1 bit */
    BCMD_Otp_CmdMsp_eCustomerCommandDisable = 55, /* 55 - 1 bit */
    
    BCMD_Otp_CmdMsp_eHashCodeDigest0     = 56, /* 56 - 32 bits */
    BCMD_Otp_CmdMsp_eHashCodeDigest1     = 57, /* 57 - 32 bits */
    BCMD_Otp_CmdMsp_eHashCodeDigest2     = 58, /* 58 - 32 bits */
    BCMD_Otp_CmdMsp_eHashCodeDigest3     = 59, /* 59 - 32 bits */
    BCMD_Otp_CmdMsp_eHashCodeDigest4     = 60, /* 60 - 32 bits */
   
    BCMD_Otp_CmdMsp_eESACtrl             = 61, /* 61 - 1 bit */
    BCMD_Otp_CmdMsp_eOtpFullRomDisable   = 62, /* 62 - 1 bit */
    BCMD_Otp_CmdMsp_eOtpBootRomCrcEn     = 63, /* 63 - 1 bit */
    BCMD_Otp_CmdMsp_eOtpBspCodeBckgrdChkEnable = 64, /* 64 - 1 bit */
    BCMD_Otp_CmdMsp_eAesprime_KeySelect  = 65, /* 65 - 6 bits */
    BCMD_Otp_CmdMsp_eOtpSecurityMode     = 66, /* 66 - 5 bits */
    BCMD_Otp_CmdMsp_eZPartEnable         = 67, /* 67 - 2 bits */
    BCMD_Otp_CmdMsp_eMarketId            = 68, /* 68 - 8 bits */
    BCMD_Otp_CmdMsp_eCurrentEpoch        = 69, /* 69 - 8 bits */
    BCMD_Otp_CmdMsp_eKey0PrimeSigningRules = 70, /* 70 - 4 bits */
    BCMD_Otp_CmdMsp_eKey0SigningRules    = 71, /* 71 - 4 bits */
    BCMD_Otp_CmdMsp_eForceK1Only         = 72, /* 72 - 1 bit */
    BCMD_Otp_CmdMsp_eForceK1K2Only       = 73, /* 73 - 1 bit */
    BCMD_Otp_CmdMsp_eExtendedBootProtect = 74, /* 74 - 1 bit */
    BCMD_Otp_CmdMsp_eErrorMipsOutResetDisable = 75, /* 75 - 1 bit */
    
    BCMD_Otp_CmdMsp_eGenericPinBoundaryScanProtect = 76, /* 76 - 1 bit */
    
    BCMD_Otp_CmdMsp_eLegacySwizzleDisable = 77, /* 77 - 1 bit */   
    BCMD_Otp_CmdMsp_eMichEnable          = 78, /* 78 - 1 bit */
    BCMD_Otp_CmdMsp_eVideoVerifyEnable   = 79, /* 79 - 1 bit */
    BCMD_Otp_CmdMsp_eAudioVerifyEnable   = 80, /* 80 - 1 bit */
    
	BCMD_Otp_CmdMsp_eReserved81          = 81, /* 81 - 2 bits */
	BCMD_Otp_CmdMsp_eReserved82          = 82, /* 82 - 1 bit */
	BCMD_Otp_CmdMsp_eReserved83          = 83, /* 83 - 1 bit */
	BCMD_Otp_CmdMsp_eReserved84          = 84, /* 84 - 2 bits */
    BCMD_Otp_CmdMsp_eReserved85          = 85, /* 85 - 1 bit */
	BCMD_Otp_CmdMsp_eSpdifI2SDisable     = 86, /* 86 - 1 bit */
    
	BCMD_Otp_CmdMsp_eOtpProgRights       = 87, /* 87 - 2 bits */
    BCMD_Otp_CmdMsp_eConformanceDisable  = 88, /* 88 - 1 bit */
    BCMD_Otp_CmdMsp_eReserved89          = 89, /* 89 - 1 bit */
    BCMD_Otp_CmdMsp_eOtpKeyDeobfuscationEn = 90, /* 90 - 1 bit */
    BCMD_Otp_CmdMsp_eOtpObfuscatedKeyHashReadDisable = 91, /* 91 - 1 bit */
    
    BCMD_Otp_CmdMsp_eEnableAesEcbCtsSa   = 92, /* 92 - 1 bit */
    BCMD_Otp_CmdMsp_eForce_3DES_Key_Ladder = 93, /* 93 - 1 bit */
    BCMD_Otp_CmdMsp_eStartHash           = 94, /* 94 - 1 bit */
    BCMD_Otp_CmdMsp_eStopHash            = 95, /* 95 - 1 bit */
    BCMD_Otp_CmdMsp_eReserved96          = 96, /* 96 - 1 bit */
    BCMD_Otp_CmdMsp_eEnable_MSIPTV       = 97, /* 97 - 1 bit */
    BCMD_Otp_CmdMsp_eDtsEnable           = 98, /* 98 - 1 bit */
    BCMD_Otp_CmdMsp_eSize                = 99

} BCMD_Otp_CmdMsp_e ;
    
#endif
