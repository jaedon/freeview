/***************************************************************************
 *     Copyright (c) 2005-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * OTP CMD layer abstraction.
 *
 * $brcm_Workfile: bsp_s_otp_7405A0.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 4/26/07 10:11p $
 * Broadcom Security Processor Code
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7405/a0/cust_generic/aegis/bsp_s_otp_7405A0.h $
 * 
 * Hydra_Software_Devel/1   4/26/07 10:11p lyang
 * PR 27711: Add 7405 support to HSM
 * 
 * Aegis_Firmware_Devel/3   2/23/07 2:32p ksekar
 * PR 563: Add new OTP MSP bits for download code and second-tier keys
 * 
 * Aegis_Firmware_Devel/2   2/13/07 5:44p ksekar
 * PR 505: Update share file
 * 
 * Aegis_Firmware_Devel/7   2/8/07 11:06a ksekar
 * PR 542: Add OTP MSP bit 23 for FlashPublicKeyAuthenticate
 * 
 * Aegis_Firmware_Devel/6   12/27/06 2:16p hzhu
 * PR511:PR513:ACL support for allowing external keys/internal key routing
 * for the new ciphers and the new corresponding OTP bits ACL.
 * 
 * Aegis_Firmware_Devel/5   12/22/06 11:51a ksekar
 * PR 511: Add new OTP MSP bits (126 and 128-137)
 * 
 * Aegis_Firmware_Devel/4   11/6/06 12:32p ksekar
 * PR 483: Add new OTP MSP bit (bit 24) for Fast Region Check Skip of
 * Audio/Video
 * 
 * Aegis_Firmware_Devel/3   10/18/06 6:47p ksekar
 * PR 447: Update code to include new OTP MSP bits for RaveVerifyEnable,
 * FastRegionCheckSkip and DisableMICH
 * 
 * Aegis_Firmware_Devel/2   9/20/06 3:55p ksekar
 * PR 396: Update OTP firmware for new 65nm OTP cell and wrapper
 * 
 * Aegis_Firmware_Devel/1   9/8/06 11:25a ksekar
 * PR 396: New OTP file for 7400B0
 * 
 * Aegis_Firmware_Devel/8   8/25/06 12:30p ksekar
 * PR 387: code cleanup to change customer code review compile flag to
 * formatted comments to separate customer code review from real
 * functional code build.
 * 
 * Aegis_Firmware_Devel/7   6/27/06 12:32p ksekar
 * PR 357: Add Otp Ctrl Bit for Tivo Cipher Disable
 * 
 * Aegis_Firmware_Devel/6   5/10/06 11:29p ksekar
 * PR 337: Update firmware to incorporate new OTP control bits for NDS SVP
 * 
 * Aegis_Firmware_Devel/4   1/6/06 8:24a grasche
 * PR 255: Updated the shared files for customer specific build
 * 
 * Aegis_Firmware_Devel/3   12/21/05 9:31a grasche
 * PR 238: add OtpCtrl bits 5 and 6 to MIPS API
 * 
 * Aegis_Firmware_Devel/2   12/13/05 2:28p grasche
 * PR 232: first 7401B0 build
 * 
 * Aegis_Firmware_Devel/1   12/13/05 11:57a grasche
 * PR 232: add 7401B0 support to aegis builds
 * 
 * Aegis_Firmware_Devel/1   11/29/05 2:23p grasche
 * PR 206: first 7118 build
 * 
 ***************************************************************************/
 
#ifndef BSP_S_OTP_7405A0_H__
#define BSP_S_OTP_7405A0_H__

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
    
    BCMD_Otp_CmdMsp_eReserved47          = 47, /* 1 bit */
    BCMD_Otp_CmdMsp_eReserved48          = 48, /* 1 bit */
    BCMD_Otp_CmdMsp_eReserved49          = 49, /* 1 bit */
    
    BCMD_Otp_CmdMsp_eFastRegionCheckSkipAudioVideo = 50, /* 1 bit */
    
    BCMD_Otp_CmdMsp_eM2mWmdrmPd          = 51, /* 1 bit */
    BCMD_Otp_CmdMsp_eReserved52          = 52, /* 1 bit */
    BCMD_Otp_CmdMsp_eOtpCtrlBitSet3      = 53, /* 10 bits */
    
    BCMD_Otp_CmdMsp_eFlashPublicKeyAuthenticate = 54, /* 54 - 1 bit */
    BCMD_Otp_CmdMsp_eReserved55          = 55, /* 55 - 1 bit */
    
    BCMD_Otp_CmdMsp_eHashCodeDigest0     = 56, /* 56 - 32 bits */
    BCMD_Otp_CmdMsp_eHashCodeDigest1     = 57, /* 57 - 32 bits */
    BCMD_Otp_CmdMsp_eHashCodeDigest2     = 58, /* 58 - 32 bits */
    BCMD_Otp_CmdMsp_eHashCodeDigest3     = 59, /* 59 - 32 bits */
    BCMD_Otp_CmdMsp_eHashCodeDigest4     = 60, /* 60 - 32 bits */
    
    BCMD_Otp_CmdMsp_eTestCodeDigest0     = 61, /* 61 - 32 bits */
    BCMD_Otp_CmdMsp_eTestCodeDigest1     = 62, /* 62 - 8 bits */
    
    BCMD_Otp_CmdMsp_eXptCustomerCtrl     = 63, /* 63 - 8 bits */
    BCMD_Otp_CmdMsp_eOtpFullRomDisable   = 64, /* 64 - 1 bit */
    BCMD_Otp_CmdMsp_eOtpBootRomCrcEn     = 65, /* 65 - 1 bit */
    BCMD_Otp_CmdMsp_eOtpBspCodeBckgrdChkEnable = 66, /* 66 - 1 bit */
    BCMD_Otp_CmdMsp_eAesprime_KeySelect  = 67, /* 67 - 6 bits */
    BCMD_Otp_CmdMsp_eOtpSecurityMode     = 68, /* 68 - 5 bits */
    BCMD_Otp_CmdMsp_eZPartEnable         = 69, /* 69 - 2 bits */
    BCMD_Otp_CmdMsp_eMarketId            = 70, /* 70 - 8 bits */
    BCMD_Otp_CmdMsp_eCurrentEpoch        = 71, /* 71 - 8 bits */
    BCMD_Otp_CmdMsp_eKey0PrimeSigningRules = 72, /* 72 - 8 bits */
    BCMD_Otp_CmdMsp_eKey0SigningRules    = 73, /* 73 - 8 bits */
    BCMD_Otp_CmdMsp_eForceK1Only         = 74, /* 74 - 1 bit */
    BCMD_Otp_CmdMsp_eForceK1K2Only       = 75, /* 75 - 1 bit */
    BCMD_Otp_CmdMsp_eExtendedBootProtect = 76, /* 76 - 1 bit */
    BCMD_Otp_CmdMsp_eFwProcessorChkEn    = 77, /* 77 - 1 bit */
    BCMD_Otp_CmdMsp_eErrorMipsOutResetDisable = 78, /* 78 - 1 bit */
    
    BCMD_Otp_CmdMsp_eSize                = 79
    
} BCMD_Otp_CmdMsp_e ;
    
#endif
