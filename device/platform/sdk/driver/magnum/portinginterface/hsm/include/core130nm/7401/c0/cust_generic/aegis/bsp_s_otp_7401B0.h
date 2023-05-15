/***************************************************************************
 *     Copyright (c) 2005-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * OTP CMD layer abstraction.
 *
 * $brcm_Workfile: bsp_s_otp_7401B0.h $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 12/4/06 4:34p $
 * Broadcom Security Processor Code
 ***************************************************************************/
 
#ifndef BSP_S_OTP_7401B0_H__
#define BSP_S_OTP_7401B0_H__

typedef enum BCMD_Otp_CmdMsp_e      
{

    
    BCMD_Otp_CmdMsp_eBackgroundCheckEn   = 2,
    BCMD_Otp_CmdMsp_eFlashRegion0_2      = 3,  /* 3 bits */
    BCMD_Otp_CmdMsp_eBootMemChkEn        = 4,
    BCMD_Otp_CmdMsp_eExportCntrlEnable   = 5,
    BCMD_Otp_CmdMsp_ePubKeyIndx0_3       = 6,  /* 4 bits */
    
    BCMD_Otp_CmdMsp_eStopB               = 7,
    BCMD_Otp_CmdMsp_eStartB              = 8,
    BCMD_Otp_CmdMsp_eStopA               = 9,
    BCMD_Otp_CmdMsp_eStartA              = 10,
    
    
    BCMD_Otp_CmdMsp_eLockByte0_7           = 13,  /* 8 bits */
    
    BCMD_Otp_CmdMsp_eLockBitC              = 14,
    BCMD_Otp_CmdMsp_eCrOtpBit0             = 15,
    BCMD_Otp_CmdMsp_eCrOtpBit1             = 16,
    BCMD_Otp_CmdMsp_ePciHostProtectWithCR  = 17,
    BCMD_Otp_CmdMsp_ePciClientProtectWithCR= 18,
    BCMD_Otp_CmdMsp_eTestPortProtectWithCR = 19,
    BCMD_Otp_CmdMsp_eForceDramScrambler    = 20,
    

    BCMD_Otp_CmdMsp_eCrcHardwareEn     = 25,
    BCMD_Otp_CmdMsp_eOtpProductId0_3   = 26,  /* 4 bits*/
    
    BCMD_Otp_CmdMsp_eOtpCtrlBit0_30    = 27,  /* 31 bits*/
    
    
    BCMD_Otp_CmdMsp_eOtpDebugDisable0_3 = 29, /* 4 bits */
    BCMD_Otp_CmdMsp_eOtpClkDisable0_3   = 30, /* 4 bits */
    BCMD_Otp_CmdMsp_eOtpModeConfigExt0_31 = 31, /* 32 bits */
    
    


    BCMD_Otp_CmdMsp_eOtpCtrlBitSet2      = 36, /* 12 bits */


    BCMD_Otp_CmdMsp_ePatchRAMDisable      = 38, /* 4 bits */
   
    BCMD_Otp_CmdMsp_eOtpReserved0         = 39, 
   

    BCMD_Otp_CmdMsp_eOtpCtrlBit44_eDisableCaKeyLadder = 41, /* 1 bit */


    BCMD_Otp_CmdMsp_eSize                 = 43
    
} BCMD_Otp_CmdMsp_e ;
    
#endif
