/***************************************************************************
 *     Copyright (c) 2005-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * OTP command layer.
 *
 * $brcm_Workfile: bsp_s_otp.h $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 11/4/08 11:54p $
 * Broadcom Security Processor Code
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7405/b0/cust_generic/aegis/bsp_s_otp.h $
 * 
 * Hydra_Software_Devel/4   11/4/08 11:54p atruong
 * PR48561: New OTP PROGMSP code
 * 
 * Hydra_Software_Devel/3   4/30/07 5:06p lyang
 * PR 27711: Add 7405 support to HSM. Purge and shared *.h source udpated
 * 
 * Aegis_Firmware_Devel/4   2/23/07 2:32p ksekar
 * PR 505: Add 7405A0 compile flags
 * 
 * Aegis_Firmware_Devel/3   2/1/07 9:58a whou
 * PR525: add support for 7405A0.
 * 
 * Aegis_Firmware_Devel/2   1/31/07 6:31p whou
 * PR525: support 7405A0.
 * 
 * Aegis_Firmware_Devel/54   10/20/06 10:41a ksekar
 * PR 462: Change Offline_status command output format for 7400B0
 * 
 * Aegis_Firmware_Devel/53   9/22/06 3:16p btan
 * PR 426: Fixed the header files that shared between Mips and BSP.
 * 
 * Aegis_Firmware_Devel/52   9/20/06 3:54p ksekar
 * PR 396: Update OTP firmware for new 65nm OTP cell and wrapper
 * 
 * Aegis_Firmware_Devel/51   9/15/06 8:04a ksekar
 * PR 396: Update code for 65nm OTP in 7400B0
 * 
 * Aegis_Firmware_Devel/50   9/8/06 11:29a ksekar
 * PR 396: Incorporate new 7400B0 OTP
 * 
 * Aegis_Firmware_Devel/49   8/25/06 12:22p ksekar
 * PR 387: code cleanup to change customer code review compile flag to
 * formatted comments to separate customer code review from real
 * functional code build.
 * 
 * Aegis_Firmware_Devel/48   8/14/06 9:13a btan
 * PR  385 : Fixed a few minor compilation issues.
 * 
 * Aegis_Firmware_Devel/47   5/22/06 4:36p ksekar
 * PR 349: Update OTP firmware to make isSvpVgs bit readable by
 * OfflineOTPRead command for 7400A0.
 * 
 * Aegis_Firmware_Devel/46   3/14/06 6:47p ksekar
 * PR 308: Update to incorporate new OTP MC0 bit to distinguish between
 * SVP+ICAM2 and SVP+VGS mode
 * 
 * Aegis_Firmware_Devel/45   3/5/06 3:30a ksekar
 * PR 300: Update to incorporate new OTP map bits for 7401B0 and 7118A0
 * 
 * Aegis_Firmware_Devel/44   1/13/06 2:40p grasche
 * PR 255: Updated the shared files - make BSP_DEBUG compile
 * 
 * Aegis_Firmware_Devel/43   1/12/06 11:26p btan
 * PR 255: Update shared files to sync up both Aegis and Host sides. Added
 * more reserved enum.
 * 
 * Aegis_Firmware_Devel/42   1/12/06 3:24p grasche
 * PR 261: reduce 7401A0 ROM code size
 * 
 * Aegis_Firmware_Devel/41   1/10/06 9:34a grasche
 * PR 260: remove 7401A0 PROG algorithm selects - only 0 is supported by
 * chip
 * 
 * Aegis_Firmware_Devel/40   1/6/06 1:55a btan
 * PR 255: Update shared files to sync up both Aegis and Host sides.
 * 
 * Aegis_Firmware_Devel/39   12/30/05 2:32p grasche
 * PR 251: clean up comments in H file
 * 
 * Aegis_Firmware_Devel/38   12/5/05 3:44p grasche
 * PR 206: did changes to support NEG tests
 * 
 * Aegis_Firmware_Devel/37   11/30/05 11:11a grasche
 * PR 206: added BPI_Otp_Algorithm_eNumber2
 * 
 * Aegis_Firmware_Devel/36   11/29/05 4:58p grasche
 * PR 206: first 7118 build
 * 
 * Aegis_Firmware_Devel/35   11/28/05 11:03a grasche
 * PR 206: add parameter to debug command
 * 
 * Aegis_Firmware_Devel/34   11/16/05 11:48a grasche
 * PR 206: add eAlgorithm to OTP PROG
 * 
 * Aegis_Firmware_Devel/33   11/10/05 10:30a grasche
 * PR 206: added BCMD_OTP_FLT_STAT_XXX defines
 * 
 * Aegis_Firmware_Devel/32   10/27/05 10:21p grasche
 * PR 206: change from OTP_UNIT_TEST to PATCH define
 * 
 * Aegis_Firmware_Devel/31   10/18/05 2:04p grasche
 * PR 206: update status read command
 * 
 * Aegis_Firmware_Devel/30   10/10/05 6:37p grasche
 * PR 206: reduced OTP_WRITE_EN command count for 7400 debug commands
 * 
 * Aegis_Firmware_Devel/29   10/10/05 4:01p grasche
 * PR 206: new feature for 7400 code base OTP_WRITE_XXX commands
 * 
 * Aegis_Firmware_Devel/28   10/10/05 1:51p grasche
 * PR 205: replace BSP_DSS with BSP_DIRECTV
 * 
 * Aegis_Firmware_Devel/27   9/30/05 7:45a grasche
 * PR 194: for customer code review
 * 
 * Aegis_Firmware_Devel/26   9/21/05 1:36p grasche
 * PR 186: "NONSEC" to "CONTROL" variable names
 * 
 * Aegis_Firmware_Devel/25   9/20/05 5:47p grasche
 * PR 185: moved REG read/write to bpi_otp.h for MIPS
 * 
 * Aegis_Firmware_Devel/24   9/19/05 6:57p btan
 * PR 185: Updated the shared file for host compilation
 * 
 * Aegis_Firmware_Devel/23   9/13/05 6:51a grasche
 * PR 166: Comcast in NGNA
 * 
 * Aegis_Firmware_Devel/22   9/12/05 12:02p grasche
 * PR 166: add BSP_NDS to data section PROG/READ code
 * 
 * Aegis_Firmware_Devel/21   9/7/05 5:46a grasche
 * PR 161: correct access control call for DS PROG, and add
 * BPI_Otp_ProgBitMode routine
 * 
 * Aegis_Firmware_Devel/21   9/7/05 5:44a grasche
 * PR 161: correct access control call for DS PROG, and add
 * BPI_Otp_ProgBitMode routine
 * 
 * Aegis_Firmware_Devel/20   8/30/05 10:38a grasche
 * PR 56: deleted comment
 * 
 * Aegis_Firmware_Devel/19   8/26/05 8:20a grasche
 * PR 128: add BPI_Otp_CmdMsp_e to make bit/row information not in ENUM
 * for CMD
 * 
 * Aegis_Firmware_Devel/18   8/24/05 2:43p grasche
 * PR 56: added comments
 * 
 * Aegis_Firmware_Devel/17   8/22/05 11:05a grasche
 * PR 127: change BPI_Otp_CmdProgMode_eEnum to 0x12 to make in more unique
 * 
 * Aegis_Firmware_Devel/16   8/19/05 1:44p grasche
 * PR 56: code reviewed OTP and Uncataloged file element "bsp_s_acl.h".
 * 
 * Aegis_Firmware_Devel/15   8/15/05 5:58p grasche
 * PR 56: code cleanup for code review
 * 
 * Aegis_Firmware_Devel/14   8/12/05 2:51p whou
 * PR23: include "bbm_reg.h" to avoid compilation error.
 * 
 * Aegis_Firmware_Devel/13   7/23/05 4:22p grasche
 * PR 42: FIRMWARE: GENERAL: Edited code for E* review
 * 
 * Aegis_Firmware_Devel/12   7/23/05 4:16p grasche
 * PR 42: FIRMWARE: GENERAL: Edited code for E* review
 * 
 * Aegis_Firmware_Devel/11   7/20/05 11:13a grasche
 * PR 42: make BCMD_Otp_eNumErrorsMax  BSP_BROADCOM_ONLY
 * 
 * Aegis_Firmware_Devel/10   7/8/05 1:30p grasche
 * PR 41: changes for code review with customer
 * 
 * Aegis_Firmware_Devel/9   7/6/05 1:06p grasche
 * PR 23: added comments for estar code review
 * 
 * Aegis_Firmware_Devel/8   6/30/05 6:21p grasche
 * PR 35: added BCMD_Otp_eNumErrorsMax parameter
 * 
 * Aegis_Firmware_Devel/7   6/29/05 4:45p grasche
 * PR 23: customer code cleanup
 * 
 * Aegis_Firmware_Devel/6   6/29/05 8:33a grasche
 * PR 23: customer code cleanup
 * 
 * Aegis_Firmware_Devel/5   6/28/05 1:04p grasche
 * PR 23: make data section enum customer specific
 * 
 * Aegis_Firmware_Devel/4   6/28/05 12:30p grasche
 * PR 22: make data section PROG twice not cause WDOG reset
 * 
 * Aegis_Firmware_Devel/3   6/20/05 7:14a grasche
 * PR 1: make BSP_DSS and BSP_GVP code changes
 * 
 * Aegis_Firmware_Devel/2   6/7/05 9:19a grasche
 * PR 13830: made PROG KEY A/B/C not use OS time delay
 * 
 * Aegis_Firmware_Devel/1   5/27/05 1:36p grasche
 * PR 13830: broke out for MIPS
 * 
 ***************************************************************************/
 
/**************************************************************************
        
 OTP Command API functions available to MIPs - documented in this H file 
 
 ***************************************************************************/ 

#ifndef BSP_S_OTP_H__
#define BSP_S_OTP_H__

#define BCMD_OTP_DATASECTIONPROG_MODE   0x00010112



/*

BCMD_cmdType_eOFFLINE_OTP_READ - read OTP settings for given OTP bits

Parameters passed:
eEnum - must be BPI_Otp_CmdReadReg_e

InCmdOfflineOtpRead: At offset eEnum in the command buffer is the 
BPI_Otp_CmdReadRegister_e selected for this read operation.
|-------------------|-------------------|-------------------|------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             | 
|-------------------|-------------------|-------------------|------------------| 
|  unused           | unused            | unused            | eEnum - 8        | 
|-------------------|-------------------|-------------------|------------------| 
    
OutCmdOfflineOtpRead: at offset eRegValueLo and eRegValueHi are each half of register
being read when a 64 bit register is being read.  When a 32 bit register is 
being read only the eRegValueLo is used. 
|-------------------|-------------------|-------------------|------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             | 
|-------------------|-------------------|-------------------|------------------| 
|                  eRegValueLo - 32                                            | 
|-------------------|-------------------|-------------------|------------------| 
|                  eRegValueHi - 32                                            | 
|-------------------|-------------------|-------------------|------------------| 

 */
typedef enum BCMD_Otp_InCmdOfflineOtpRead_e 
{
    BCMD_Otp_InCmdOfflineOtpRead_eEnum                = (5 << 2)+3  /* 8  bit index */
}BCMD_Otp_InCmdOfflineOtpRead_e;

typedef enum BCMD_Otp_OutCmdOfflineOtpRead_e
{
    BCMD_Otp_OutCmdOfflineOtpRead_eRegValueLo         = (6 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdOfflineOtpRead_eRegValueHi         = (7 << 2)+0  /* 32 bit index */
}BCMD_Otp_OutCmdOfflineOtpRead_e;
    
/*

BCMD_cmdType_eOFFLINE_PROG_MSP - PROG MSP 'eEnum' with the given 'eData'.

Parameters passed:
eMode32 - must be BCMD_OTP_PROGMSP_MODE
eEnum - type where this ENUM is BCMD_Otp_CmdMsp_e
eNumOfBits - number of bits to program.  
eMask - enable programming of that bit with '1', disable with '0' for 32 bits.  
eData - 1 to 32 bit actual data to program

InCmdOfflineProgMsp: 
|-------------------|-------------------|-------------------|------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             | 
|-------------------|-------------------|-------------------|------------------| */
/*
|  eMode32 - 32                                                                | */
/*
|-------------------|-------------------|-------------------|------------------| 
|  unused           | unused            | unused            | eEnum - 8        | 
|-------------------|-------------------|-------------------|------------------| 
|  unused           | unused            | unused            | eNumOfBits   - 8 |
|-------------------|-------------------|-------------------|------------------| 
|  eMask - 32                                                                  |
|-------------------|-------------------|-------------------|------------------| 
|  eData - 32                                                                  |
|-------------------|-------------------|-------------------|------------------| 
    
OutCmdOfflineProgMsp: 
|-------------------|-------------------|-------------------|------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             | 
|-------------------|-------------------|-------------------|------------------| 
|                  eReserved   - 32                                            | 
|-------------------|-------------------|-------------------|------------------| 
 */

typedef enum BCMD_Otp_InCmdOfflineProgMsp_e 
{
    
    BCMD_Otp_InCmdOfflineProgMsp_eMode32              = ( 5 << 2)+0, /* 32  bit index */

    BCMD_Otp_InCmdOfflineProgMsp_eEnum                = ( 6 << 2)+3, /* 8  bit index */
    BCMD_Otp_InCmdOfflineProgMsp_eNumOfBits           = ( 7 << 2)+3, /* 8  bit index */
    BCMD_Otp_InCmdOfflineProgMsp_eMask                = ( 8 << 2)+0, /* 32 bit index */
    BCMD_Otp_InCmdOfflineProgMsp_eData                = ( 9 << 2)+0  /* 32 bit index */
}BCMD_Otp_InCmdOfflineProgMsp_e;

typedef enum BCMD_Otp_OutCmdOfflineProgMsp_e
{
    BCMD_Otp_OutCmdOfflineProgMsp_eReserved           = (6 << 2)+0   /* 32 bit index */
}BCMD_Otp_OutCmdOfflineProgMsp_e;

#define BCMD_OTP_PROGMSP_MODE   0x00010012

/*

BCMD_cmdType_eOFFLINE_MSP_READ - READ MSP 'eEnum' 

Parameters passed:
eEnum - type where this ENUM is BCMD_Otp_CmdMsp_e
eData - 1 to 32 bit actual data read 
*/
/*    
InCmdOfflineMspRead: 
|-------------------|-------------------|-------------------|------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             | 
|-------------------|-------------------|-------------------|------------------| 
|  unused           | unused            | unused            | eEnum - 8        | */
/*    
    
OutCmdOfflineMspRead: 
|-------------------|-------------------|-------------------|------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             | 
|-------------------|-------------------|-------------------|------------------| 
|  eMspValue - 32                                                              |
|-------------------|-------------------|-------------------|------------------| 

*/

typedef enum BCMD_Otp_InCmdOfflineMspRead_e 
{
    BCMD_Otp_InCmdOfflineMspRead_eEnum                = ( 5<< 2)+ 3, /* 8  bit index */
    
    BCMD_Otp_InCmdOfflineMspRead_eReserved0            = ( 6<< 2)+ 3, 
    BCMD_Otp_InCmdOfflineMspRead_eReserved1            = ( 7<< 2)+ 3, 
    BCMD_Otp_InCmdOfflineMspRead_eReserved2             = ( 8<< 2)+ 3,
    BCMD_Otp_InCmdOfflineMspRead_eReserved3           = ( 9<< 2)+ 3, 
    BCMD_Otp_InCmdOfflineMspRead_eReserved4          = (10<<2) + 0, 
    BCMD_Otp_InCmdOfflineMspRead_eReserved5          = (14<<2) + 0,
    BCMD_Otp_InCmdOfflineMspRead_eReserved6          = (18<<2) + 0,


    BCMD_Otp_InCmdOfflineMspRead_eReserved7                = (22<<2) + 3  /* Need this to determine the length of Reserved6 */

}BCMD_Otp_InCmdOfflineMspRead_e;

typedef enum BCMD_Otp_OutCmdOfflineMspRead_e
{
    BCMD_Otp_OutCmdOfflineMspRead_eMspValue           = (6 << 2)+0  /* 32 bit index */
}BCMD_OutCmdOfflineMspRead_e;





/********************************************************************
BCMD_cmdType_eOFFLINE_STATUS input and output buffer structure 

The values read for eFltStatKeyA, eFltStatKeyB, eFltStatKeyC, eFltStatDs
are defined in the #define statements below.

InCmdReloadFromOtp: 
|-------------------|-------------------|-------------------|------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             | 
|-------------------|-------------------|-------------------|------------------| 
| eStatusSal     - 32                                                          | 
|-------------------|-------------------|-------------------|------------------| 
| eSignalRead    - 32                                                          | 
|-------------------|-------------------|-------------------|------------------| 
| eFltStat       - 32                                                          | 
|-------------------|-------------------|-------------------|------------------| 
| eFltStatKeyA   - 32                                                          | 
|-------------------|-------------------|-------------------|------------------| 
| eFltStatKeyB   - 32                                                          | 
|-------------------|-------------------|-------------------|------------------| 
| eFltStatKeyC   - 32                                                          | */
/*
|-------------------|-------------------|-------------------|------------------| 
| Reserved - 32                                                                | */
/*
|-------------------|-------------------|-------------------|------------------| 
    
 */

typedef enum BCMD_Otp_InStatusCmd_e 
{
    BCMD_Otp_InCmdOffline_eNotUsedIndex               = (5 << 0) /* 32 bit index - index not used */
}BCMD_Otp_InCmdStatus_e;

typedef enum BCMD_Otp_OutCmdStatus_e
{
    BCMD_Otp_OutCmdStatus_eStatusSAL                  = ( 6 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eSignalRead                 = ( 7 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eFltStat                    = ( 8 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eFltStatKeyA                = ( 9 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eFltStatKeyB                = (10 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eFltStatKeyC                = (11 << 2)+0, /* 32 bit index */
    
    BCMD_Otp_OutCmdStatus_eReserved0                  = (12 << 2)+0, 
    


    BCMD_Otp_OutCmdStatus_eReserved1                  = (13 << 2)+0, 
    BCMD_Otp_OutCmdStatus_eReserved2                  = (14 << 2)+0, 
    BCMD_Otp_OutCmdStatus_eReserved3                  = (15 << 2)+0, 
    BCMD_Otp_OutCmdStatus_eReserved4                  = (16 << 2)+0, 
    BCMD_Otp_OutCmdStatus_eReserved5                  = (17 << 2)+0, 
    BCMD_Otp_OutCmdStatus_eReserved6                  = (18 << 2)+0, 
    BCMD_Otp_OutCmdStatus_eReserved7                  = (19 << 2)+0,


    BCMD_Otp_OutCmdStatus_eReserved
}BCMD_Otp_OutCmdStatus_e;

#define BCMD_OTP_FLT_STAT_KEYA_OTP_KEYA_ERR_SYNDROME                0x00000100
#define BCMD_OTP_FLT_STAT_KEYB_OTP_KEYB_ERR_SYNDROME                0x00000100
#define BCMD_OTP_FLT_STAT_KEYC_OTP_KEYC_ERR_SYNDROME                0x00000100
#define BCMD_OTP_FLT_STAT_DS_OTP_DS_ERR_SYNDROME                    0x00000100




/* select bits to be read */
typedef enum BPI_Otp_CmdReadRegister_e 
{
    BPI_Otp_CmdReadRegister_eMc0OperationModeSelect = 0,

    BPI_Otp_CmdReadRegister_eReserved1              = 1, 

    BPI_Otp_CmdReadRegister_eReserved2              = 2,

    BPI_Otp_CmdReadRegister_eIdAWord                = 3,
    BPI_Otp_CmdReadRegister_eIdBWord                = 4,
    BPI_Otp_CmdReadRegister_eIdCWord                = 5,
    

    BPI_Otp_CmdReadRegister_eReserved6              = 6,

    BPI_Otp_CmdReadRegister_eReserved7              = 7,

    BPI_Otp_CmdReadRegister_eReserved8              = 8,


    BPI_Otp_CmdReadRegister_eSize
} BPI_Otp_CmdReadRegister_e ;


typedef enum BPI_Otp_HalOperationMode_e 
{
    BPI_Otp_HalOperationMode_eGeneric       = 0x00,
    
    BPI_Otp_HalOperationMode_eReserved0     = 0x01,

    BPI_Otp_HalOperationMode_eReserved1     = 0x02,

    BPI_Otp_HalOperationMode_eReserved2     = 0x03,
    
    BPI_Otp_HalOperationMode_eReserved3     = 0x04,

    BPI_Otp_HalOperationMode_eReserved4     = 0x05,

    BPI_Otp_HalOperationMode_eReserved5     = 0x06,

    BPI_Otp_HalOperationMode_eReserved6     = 0x07,
    
    BPI_Otp_HalOperationMode_eReserved7     = 0x08,

    BPI_Otp_HalOperationMode_eReserved8     = 0x09,

    BPI_Otp_HalOperationMode_eReserved9     = 0x0a,

    BPI_Otp_HalOperationMode_eReserved10    = 0x0b,
    
    BPI_Otp_HalOperationMode_eReserved11    = 0x0c,

    BPI_Otp_HalOperationMode_eReserved12    = 0x0d 
    
} BPI_Otp_HalOperationMode_e ;


#define BPI_OTP_PROGRAM_MSP_NUMBER_BITS_TO_PROG_MAX     32  /* how many bits */

/* there are 3 keys in OTP, the first and second have start/stop, the third
 * has lock/unlock.
 */
typedef enum BPI_Otp_HalProgKey_e 
{
    BPI_Otp_HalProgKey_eA,
    BPI_Otp_HalProgKey_eB,
    BPI_Otp_HalProgKey_eC,
    BPI_Otp_HalProgKey_eCAddEccCrc,
    BPI_Otp_HalProgKey_eSize
} BPI_Otp_HalProgKey_e ;

typedef enum BPI_Otp_HalReadRegister_e 
{
    BPI_Otp_HalReadRegister_eMc0OperationMode   = 0,
    
    BPI_Otp_HalReadRegister_eReserved0          = 1, 

    BPI_Otp_HalReadRegister_eReserved1          = 2,

    BPI_Otp_HalReadRegister_e_size 
} BPI_Otp_HalReadRegister_e ;


typedef enum BPI_Otp_DataSection_e 
{
    BPI_Otp_DataSection_e0 = 0x0,
    BPI_Otp_DataSection_e1 = 0x1,
    BPI_Otp_DataSection_e2 = 0x2,
    BPI_Otp_DataSection_e3 = 0x3,
    BPI_Otp_DataSection_e4 = 0x4,
    BPI_Otp_DataSection_e5 = 0x5,
    BPI_Otp_DataSection_e6 = 0x6,
    BPI_Otp_DataSection_e7 = 0x7,
    BPI_Otp_DataSection_e_size = 0x8
} BPI_Otp_DataSection_e ;

 #endif 

