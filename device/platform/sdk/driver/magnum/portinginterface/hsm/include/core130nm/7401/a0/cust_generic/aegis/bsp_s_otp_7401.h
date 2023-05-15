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
 * $brcm_Workfile: bsp_s_otp_7401.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 4/25/06 1:52p $
 * Broadcom Security Processor Code
 ***************************************************************************/
 
#ifndef BSP_S_OTP_7401A0_H__
#define BSP_S_OTP_7401A0_H__

typedef enum BCMD_Otp_CmdMsp_e      
{
    BCMD_Otp_CmdMsp_eReserved0          = 0,
    BCMD_Otp_CmdMsp_eReserved1       = 1,
    
    BCMD_Otp_CmdMsp_eBackgroundCheckEn   = 2,
    BCMD_Otp_CmdMsp_eFlashRegion0_2      = 3,  /* 3 bits */
    BCMD_Otp_CmdMsp_eBootMemChkEn        = 4,
    BCMD_Otp_CmdMsp_eExportCntrlEnable   = 5,
    BCMD_Otp_CmdMsp_ePubKeyIndx0_3       = 6,  /* 4 bits */
    
    BCMD_Otp_CmdMsp_eReserved2               = 7,
    BCMD_Otp_CmdMsp_eReserved3              = 8,
    BCMD_Otp_CmdMsp_eReserved4               = 9,
    BCMD_Otp_CmdMsp_eReserved5              = 10,
    
    BCMD_Otp_CmdMsp_eReserved6       = 11,
    BCMD_Otp_CmdMsp_eReserved7   = 12,
    
    BCMD_Otp_CmdMsp_eReserved8           = 13,
    
    BCMD_Otp_CmdMsp_eLockBitC              = 14,
    BCMD_Otp_CmdMsp_eCrOtpBit0             = 15,
    BCMD_Otp_CmdMsp_eCrOtpBit1             = 16,
    BCMD_Otp_CmdMsp_ePciHostProtectWithCR  = 17,
    BCMD_Otp_CmdMsp_ePciClientProtectWithCR= 18,
    BCMD_Otp_CmdMsp_eTestPortProtectWithCR = 19,
    BCMD_Otp_CmdMsp_eForceDramScrambler    = 20,
    
    BCMD_Otp_CmdMsp_eReserved9          = 21,
    BCMD_Otp_CmdMsp_eReserved10          = 22,
    BCMD_Otp_CmdMsp_eReserved11         = 23,
    BCMD_Otp_CmdMsp_eReserved12        = 24,

    BCMD_Otp_CmdMsp_eCrcHardwareEn     = 25,
    BCMD_Otp_CmdMsp_eOtpProductId0_3   = 26,  /* 4 bits*/
    
    BCMD_Otp_CmdMsp_eOtpCtrlBit0_28    = 27,  /* 29 bits*/
    
    BCMD_Otp_CmdMsp_eReserved13     = 28,
    
    BCMD_Otp_CmdMsp_eReserved14     = 29,

    BCMD_Otp_CmdMsp_eReserved15     = 30,
    
    BCMD_Otp_CmdMsp_eReserved16 = 31,
    
    BCMD_Otp_CmdMsp_eReserved17       = 32, 
    
    BCMD_Otp_CmdMsp_eSize                 = 33
    
} BCMD_Otp_CmdMsp_e ;
    
#endif
