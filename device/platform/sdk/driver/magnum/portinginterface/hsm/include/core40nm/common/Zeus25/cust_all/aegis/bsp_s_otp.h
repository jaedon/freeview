/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
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
 * $brcm_Revision: Aegis_Firmware_Devel/6 $
 * $brcm_Date: 5/2/12 11:13a $
 * Broadcom Security Processor Code
 ***************************************************************************/
 
/**************************************************************************
        
 OTP Command API functions available to MIPs - documented in this H file 
 
 ***************************************************************************/ 

#ifndef BSP_S_OTP_H__
#define BSP_S_OTP_H__

/* BRCM_IF (BSP_CODEREV_BROADCOM_ONLY) */

/*

BCMD_cmdType_eOFFLINE_PROG_OTP  - PROG OTP for the given 'eKey' when
'eMode' is 'eEnum', or PROG OTP with eBitData when 'eMode' is 'eBit' .
This command is disabled when OTP 'DebugDisable' is TRUE

Parameters passed:
eMode - must be BCMD_Otp_CmdProgMode_eEnum or BCMD_Otp_CmdProgMode_eBit or BCMD_Otp_CmdProgMode_eRow
eAlgorithm        - 0, 1, or 2 - This is a don't care field which is reserved.
eNumErrorsMax     - 0, 1

only used in eMode==BCMD_Otp_CmdProgMode_eEnum
eKey              - key selection to PROG which is BPI_Otp_HalProgKey_e
eKeyParams        - Max 8*32 bytes of data. Depends on each key section

only used in eMode==BCMD_Otp_CmdProgMode_eBit
eMspBit    - TRUE or FALSE boolean
eBitAddr   - bit address
eRowAddr   - row address
eBitData   - 1 bit data

only used in eMode==BCMD_Otp_CmdProgMode_eRow
eRowData   - Row data
eRowAddr   - row address


InCmdOfflineProgOtp:
|-------------------|-------------------|-------------------|---------------------|
|  31..24           | 23..16            | 15..8             | 7..0                |
|-------------------|-------------------|-------------------|---------------------|
|  unused           | eNumErrorsMax     | eAlgorithm        | eMode           - 8 |
|-------------------|-------------------|-------------------|---------------------|
|  unused           | unused            | unused            | eKey            - 8 |
|-------------------|-------------------|-------------------|---------------------|
|  KEY Section Params [8*32] - Filled depending on which key section              |
|-------------------|-------------------|-------------------|---------------------|
|  Row data[30:24]  | Row data[23:16]   | Row data[15:8]    | Row data[7:0]       |
|-------------------|-------------------|-------------------|---------------------|
|  unused           | unused            | unused            |eBitAddr/eMspBit - 8 |
|-------------------|-------------------|-------------------|---------------------|
|  unused           | unused            | eRowAddr                           - 16 |
|-------------------|-------------------|-------------------|---------------------|
|                                                           | eBitData        - 8 |
|-------------------|-------------------|-------------------|---------------------|

OutCmdOfflineProgOtp:
|-------------------|-------------------|-------------------|---------------------|
|  31..24           | 23..16            | 15..8             | 7..0                |
|-------------------|-------------------|-------------------|---------------------|
|                  eReserved   - 32                                               |
|-------------------|-------------------|-------------------|---------------------|

*/

/* This typedef applies for all 40nm chips*/
typedef enum BCMD_Otp_InCmdOfflineProgOtp_e
{
    BCMD_Otp_InCmdOfflineProgOtp_eMode                  = (  5 << 2)+3, /*  8 bit index */
    BCMD_Otp_InCmdOfflineProgOtp_eReserved              = (  5 << 2)+2, /*  8 bit index */
    BCMD_Otp_InCmdOfflineProgOtp_eNumErrorsMax          = (  5 << 2)+1, /*  8 bit index */

    BCMD_Otp_InCmdOfflineProgOtp_eKey                   = (  6 << 2)+3, /*  8 bit index */
    BCMD_Otp_InCmdOfflineProgOtp_eKeyParams             = (  7 << 2)+0, /* 32 bit index */
    BCMD_Otp_InCmdOfflineProgOtp_eRowData               = ( 15 << 2)+0, /* 32 bit index */

    BCMD_Otp_InCmdOfflineProgOtp_eMspBit                = ( 16 << 2)+3, /*  8 bit index */
    BCMD_Otp_InCmdOfflineProgOtp_eBitAddr               = ( 16 << 2)+3, /*  8 bit index */
    BCMD_Otp_InCmdOfflineProgOtp_eRowAddr               = ( 17 << 2)+2, /* 16 bit index */
    BCMD_Otp_InCmdOfflineProgOtp_eBitData               = ( 18 << 2)+3  /*  8 bit index */

}BCMD_Otp_InCmdOfflineProgOtp_e;


typedef enum BCMD_Otp_OutCmdOfflineProgOtp_e
{
    BCMD_Otp_OutCmdOfflineProgOtp_eRowProgramStatus     = (  6 << 2)+0   /* 32 bit index */
}BCMD_Otp_OutCmdOfflineProgOtp_e;

#define BCMD_OTP_PROGOTP_MODE   0x00010112UL

/* BRCM_ENDIF */


/*

BCMD_cmdType_eOFFLINE_OTP_READ - read OTP settings for given OTP bits

Parameters passed:
eEnum - must be BPI_Otp_CmdReadReg_e

InCmdOfflineOtpRead: At offset eEnum in the command buffer is the
BPI_Otp_CmdReadRegister_e selected for this read operation.
|-------------------|-------------------|-------------------|------------------|
|  31..24           | 23..16            | 15..8             | 7..0             |
|-------------------|-------------------|-------------------|------------------|
|  unused           | unused            | eKeyEnum - 8      | eEnum - 8        |
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
|                  eRegValue2  - 32                                            |
|-------------------|-------------------|-------------------|------------------|
|                  eRegValue3  - 32                                            |
|-------------------|-------------------|-------------------|------------------|

 */
typedef enum BCMD_Otp_InCmdOfflineOtpRead_e
{
    BCMD_Otp_InCmdOfflineOtpRead_eEnum                = (5 << 2)+3, /* 8  bit index */
    BCMD_Otp_InCmdOfflineOtpRead_eKeyType             = (5 << 2)+2  /* 8  bit index */
}BCMD_Otp_InCmdOfflineOtpRead_e;

typedef enum BCMD_Otp_OutCmdOfflineOtpRead_e
{
    BCMD_Otp_OutCmdOfflineOtpRead_eRegValueLo         = (6 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdOfflineOtpRead_eRegValueHi         = (7 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdOfflineOtpRead_eRegValue2          = (8 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdOfflineOtpRead_eRegValue3          = (9 << 2)+0  /* 32 bit index */
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
/* BRCM_IF (BSP_CODEREV_BROADCOM_ONLY) */
/*
|  unused           | unused            | eAlgorithm        | eMode - 8        | */
/* BRCM_ELSE */
/*
|  eMode32 - 32                                                                | */
/* BRCM_ENDIF */
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

/* BRCM_IF (BSP_CODEREV_BROADCOM_ONLY) */
    BCMD_Otp_InCmdOfflineProgMsp_eMode                = ( 5 << 2)+3, /*  8 bit index */
    BCMD_Otp_InCmdOfflineProgMsp_eReserved            = ( 5 << 2)+2, /*  8 bit index */
/* BRCM_ENDIF */
    BCMD_Otp_InCmdOfflineProgMsp_eMode32              = ( 5 << 2)+0, /* 32 bit index */

    BCMD_Otp_InCmdOfflineProgMsp_eEnum                = ( 6 << 2)+3, /*  8 bit index */
    BCMD_Otp_InCmdOfflineProgMsp_eNumOfBits           = ( 7 << 2)+3, /*  8 bit index */
    BCMD_Otp_InCmdOfflineProgMsp_eMask                = ( 8 << 2)+0, /* 32 bit index */
    BCMD_Otp_InCmdOfflineProgMsp_eData                = ( 9 << 2)+0 /* 32 bit index */
}BCMD_Otp_InCmdOfflineProgMsp_e;

typedef enum BCMD_Otp_OutCmdOfflineProgMsp_e
{
    BCMD_Otp_OutCmdOfflineProgMsp_eReserved           = ( 6 << 2)+0  /* 32 bit index */
}BCMD_Otp_OutCmdOfflineProgMsp_e;

#define BCMD_OTP_PROGMSP_MODE   0x00010112UL

/*

BCMD_cmdType_eOFFLINE_MSP_READ - READ MSP 'eEnum'

Parameters passed:
eEnum - type where this ENUM is BCMD_Otp_CmdMsp_e
eData - 1 to 32 bit actual data read

InCmdOfflineMspRead:
|-------------------|-------------------|-------------------|------------------|
|  31..24           | 23..16            | 15..8             | 7..0             |
|-------------------|-------------------|-------------------|------------------|
|  unused           | unused            | unused            | eEnum - 8        |
|-------------------|-------------------|-------------------|------------------|
*/
/*

OutCmdOfflineMspRead:
|-------------------|-------------------|-------------------|------------------|
|  31..24           | 23..16            | 15..8             | 7..0             |
|-------------------|-------------------|-------------------|------------------|
|  eMspValue - 32                                                              |
|-------------------|-------------------|-------------------|------------------|
|  eMspLockValue - 32                                                          |
|-------------------|-------------------|-------------------|------------------|

*/

typedef enum BCMD_Otp_InCmdOfflineMspRead_e
{
    BCMD_Otp_InCmdOfflineMspRead_eEnum                = ( 5<< 2)+ 3 /* 8  bit index */

}BCMD_Otp_InCmdOfflineMspRead_e;

typedef enum BCMD_Otp_OutCmdOfflineMspRead_e
{
    BCMD_Otp_OutCmdOfflineMspRead_eMspValue           = (6 << 2)+0,  /* 32 bit index */
    BCMD_Otp_OutCmdOfflineMspRead_eMspLockValue       = (7 << 2)+0  /* 32 bit index */
}BCMD_OutCmdOfflineMspRead_e;


/********************************************************************
BCMD_cmdType_eOTP_DATA_SECTION_READ - read one data section

Parameters passed:
eEnum - type where this ENUM is BCMD_Otp_CmdMsp_e

InCmdDataSectionRead:
|-------------------|-------------------|-------------------|------------------|
|  31..24           | 23..16            | 15..8             | 7..0             |
|-------------------|-------------------|-------------------|------------------|
|  unused           | unused            | unused            | eEnum - 8        |
|-------------------|-------------------|-------------------|------------------|

OutCmdDataSectionRead:
|-------------------|-------------------|-------------------|------------------|
|  eArrayData - 32 bytes, arranged 32 bit x 8 array, 32 same order as PROG     |
|-------------------|-------------------|-------------------|------------------|

 */

typedef enum BCMD_Otp_InCmdDataSectionRead_e
{
    BCMD_Otp_InCmdDataSectionRead_eEnum               = (5 << 2)+3  /* 8 bit index  */

}BCMD_Otp_InCmdDataSectionRead_e;

typedef enum BCMD_Otp_IntermedDataSectionRead_e
{
    BCMD_Otp_IntermedDataSectionRead_eData             = (8 << 2)  /* 8 bit index  */

}BCMD_Otp_IntermedDataSectionRead_e;

typedef enum BCMD_Otp_OutCmdDataSectionRead_e
{
    BCMD_Otp_OutCmdDataSectionRead_eArrayData         = (6 << 2)+0  /* 32 bit index */

}BCMD_Otp_OutCmdDataSectionRead_e;

/********************************************************************
BCMD_cmdType_eOTP_DATA_SECTION_PROG - PROG one data section

eEnum - BPI_Otp_DataSection_e data selection
eArrayData - 32 bytes, arranged as 32 bit x 8 array
*/
/* BRCM_IF (BSP_CODEREV_BROADCOM_ONLY) */
/*
eMode             - must be 0x12
eAlgorithm        - 0, 1, or 2
eNumErrorsMax     - 0, 1
*/
/* BRCM_ELSE */
/*
eMode32           - BCMD_OTP_DATASECTIONPROG_MODE
*/
/* BRCM_ENDIF */
/*
eCRC              - CRC over the Data Section data to be programmed

InCmdDataSectionProg:
|-------------------|-------------------|-------------------|------------------|
|  31..24           | 23..16            | 15..8             | 7..0             |
|-------------------|-------------------|-------------------|------------------|
|  unused           | unused            | unused            | eEnum - 8        |
|-------------------|-------------------|-------------------|------------------|
|  eArrayData - 32 bytes, arranged as 32 bit x 8 array - 32                    |
|-------------------|-------------------|-------------------|------------------| */
/* BRCM_IF (BSP_CODEREV_BROADCOM_ONLY) */
/*
|  unused           | eNumErrorMax      | eAlgorithm        | eMode - 8        | */
/* BRCM_ELSE */
/*
|  eMode32 - 32                                                                | */
/* BRCM_ENDIF */
/*
|-------------------|-------------------|-------------------|------------------|
|  eCRC - 32 bits                                                              |
|-------------------|-------------------|-------------------|------------------|

 */

typedef enum BCMD_Otp_InCmdDataSectionProg_e
{
    BCMD_Otp_InCmdDataSectionProg_eEnum             = ( 5 << 2)+3, /* 8  bit index */
    BCMD_Otp_InCmdDataSectionProg_eArrayData        = ( 6 << 2)+0, /* 32 bit index */

/* BRCM_IF (BSP_CODEREV_BROADCOM_ONLY) */
    BCMD_Otp_InCmdDataSectionProg_eMode             = (14 << 2)+3, /* 8  bit index */
    BCMD_Otp_InCmdDataSectionProg_eReserved         = (14 << 2)+2, /* 8  bit index */
    BCMD_Otp_InCmdDataSectionProg_eNumErrorsMax     = (14 << 2)+1, /* 8  bit index */
/* BRCM_ENDIF */
    BCMD_Otp_InCmdDataSectionProg_eReserved0        = (14 << 2)+3,  /* 8 bit index */
    BCMD_Otp_InCmdDataSectionProg_eReserved1        = (14 << 2)+2,  /* 8 bit index */
    BCMD_Otp_InCmdDataSectionProg_eReserved2        = (14 << 2)+1,  /* 8 bit index */
    BCMD_Otp_InCmdDataSectionProg_eMode32           = (14 << 2)+0, /* 32 bit index */

    BCMD_Otp_InCmdDataSectionProg_eCrc              = (15 << 2)+0  /* 32 bit index */

}BCMD_Otp_InCmdDataSectionProg_e;

typedef enum BCMD_Otp_OutCmdDataSectionProg_e
{
    BCMD_Otp_OutCmdDataSectionProg_eReserved          = (6 << 2)+0  /* 32 bit index */
}BCMD_Otp_OutCmdDataSectionProg_e;

#define BCMD_OTP_DATASECTIONPROG_MODE   0x00010112UL



/********************************************************************
BCMD_cmdType_eOFFLINE_STATUS input and output buffer structure

The values read for eFltStatKeyA, eFltStatKeyB, eFltStatKeyC, eFltStatDs
are defined in the #define statements below.

InCmdReloadFromOtp:
|-------------------|-------------------|-------------------|------------------|
|  31..24           | 23..16            | 15..8             | 7..0             |
|-------------------|-------------------|-------------------|------------------|
| eSignalRead               - 32                                               |
|-------------------|-------------------|-------------------|------------------|
| eFltStat                  - 32                                               |
|-------------------|-------------------|-------------------|------------------|
| eFltStatKeyMC0S           - 32                                               |
|-------------------|-------------------|-------------------|------------------|
| eFltStatKey[A-F]          - 32*6                                             |
|-------------------|-------------------|-------------------|------------------|
| eFltStatHash              - 32                                               |
|-------------------|-------------------|-------------------|------------------|
| eFltStatDs                - 32                                               | */
/* BRCM_IF (BSP_CODEREV_BROADCOM_ONLY) */
/*
|-------------------|-------------------|-------------------|------------------|
| eMC0SBadFlag              - 32                                               |
|-------------------|-------------------|-------------------|------------------|
| eKey[A-F]BadFlag          - 32*6                                             |
|-------------------|-------------------|-------------------|------------------|
| eHashBadFlag              - 32                                               |
|-------------------|-------------------|-------------------|------------------|
| eDSBadFlag                - 32                                               |
|-------------------|-------------------|-------------------|------------------|
| eExptCauseResetStatus     - 32                                               | */
/* BRCM_ENDIF */
/*
|-------------------|-------------------|-------------------|------------------|
| eFltStatSOCUid            - 32                                               |
|-------------------|-------------------|-------------------|------------------|
| eFltStatNPriv[0-7]        - 32*8                                             |
*/
/* BRCM_IF (BSP_CODEREV_BROADCOM_ONLY) */
/*
|-------------------|-------------------|-------------------|------------------|
| eSOCUidBadFlag            - 32                                                   |
|-------------------|-------------------|-------------------|------------------|
| eNPriv[0-7]BadFlag        - 32*8                                                 |
*/
/* BRCM_ENDIF */
/*
|-------------------|-------------------|-------------------|------------------|
*/

typedef enum BCMD_Otp_InStatusCmd_e
{
    BCMD_Otp_InCmdOffline_eNotUsedIndex             = (5 << 0) /* 32 bit index - index not used */
}BCMD_Otp_InCmdStatus_e;

typedef enum BCMD_Otp_OutCmdStatus_e
{       
    BCMD_Otp_OutCmdStatus_eSignalRead               = ( 6 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eFltStat                  = ( 7 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eFltStatMc0S              = ( 8 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eFltStatKeyA              = ( 9 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eFltStatKeyB              = (10 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eFltStatKeyC              = (11 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eFltStatKeyD              = (12 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eFltStatKeyE              = (13 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eFltStatKeyF              = (14 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eFltStatHash              = (15 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eFltStatDS                = (16 << 2)+0, /* 32 bit index */
/* BRCM_IF (BSP_CODEREV_BROADCOM_ONLY) */
    BCMD_Otp_OutCmdStatus_eMc0SBadFlag              = (17 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eKeyABadFlag              = (18 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eKeyBBadFlag              = (19 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eKeyCBadFlag              = (20 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eKeyDBadFlag              = (21 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eKeyEBadFlag              = (22 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eKeyFBadFlag              = (23 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eHashBadFlag              = (24 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eDSBadFlag                = (25 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eCauseResetStatus         = (26 << 2)+0, /* 32 bit index */
/* BRCM_ENDIF */
/*-Zeus 2.0 additions-*/
/* BRCM_IF (BSP_CODEREV_POWAY) */
    BCMD_Otp_OutCmdStatus_eFltStatSOCUid            = (27 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eFltStatNPriv0            = (28 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eFltStatNPriv1            = (29 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eFltStatNPriv2            = (30 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eFltStatNPriv3            = (31 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eFltStatNPriv4            = (32 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eFltStatNPriv5            = (33 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eFltStatNPriv6            = (34 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eFltStatNPriv7            = (35 << 2)+0, /* 32 bit index */
/* BRCM_ENDIF */

/* BRCM_IF (BSP_CODEREV_BROADCOM_ONLY) */
    BCMD_Otp_OutCmdStatus_eSOCUidBadFlag            = (36 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eNPriv0BadFlag            = (37 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eNPriv1BadFlag            = (38 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eNPriv2BadFlag            = (39 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eNPriv3BadFlag            = (40 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eNPriv4BadFlag            = (41 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eNPriv5BadFlag            = (42 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eNPriv6BadFlag            = (43 << 2)+0, /* 32 bit index */
    BCMD_Otp_OutCmdStatus_eNPriv7BadFlag            = (44 << 2)+0, /* 32 bit index */

/* BRCM_ENDIF */
/*-Zeus 3.0 additions-*/
    BCMD_Otp_OutCmdStatus_eFltStatHash2             = (45 << 2)+0, /* 32 bit index */
/* BRCM_IF (BSP_CODEREV_BROADCOM_ONLY) */
    BCMD_Otp_OutCmdStatus_eHash2BadFlag             = (46 << 2)+0, /* 32 bit index */
/* BRCM_ENDIF */
    BCMD_Otp_OutCmdStatus_eReserved                 = (47 << 2)+0 /* 32 bit index */

}BCMD_Otp_OutCmdStatus_e;

/* The following KEY_ERR_SYNDROME definitions are not related to RDB*/
#define BCMD_OTP_FLT_STAT_MC0S_OTP_KEY_ERR_SYNDROME                0x00000100UL
#define BCMD_OTP_FLT_STAT_KEYA_OTP_KEY_ERR_SYNDROME                0x00000100UL
#define BCMD_OTP_FLT_STAT_KEYB_OTP_KEY_ERR_SYNDROME                0x00000100UL
#define BCMD_OTP_FLT_STAT_KEYC_OTP_KEY_ERR_SYNDROME                0x00000100UL
#define BCMD_OTP_FLT_STAT_KEYD_OTP_KEY_ERR_SYNDROME                0x00000100UL
#define BCMD_OTP_FLT_STAT_KEYE_OTP_KEY_ERR_SYNDROME                0x00000100UL
#define BCMD_OTP_FLT_STAT_KEYF_OTP_KEY_ERR_SYNDROME                0x00000100UL
#define BCMD_OTP_FLT_STAT_DS_OTP_DS_ERR_SYNDROME                   0x00000100UL
#define BCMD_OTP_FLT_STAT_HASH_OTP_KEY_ERR_SYNDROME                0x00000100UL
#define BCMD_OTP_FLT_STAT_HASH2_OTP_KEY_ERR_SYNDROME               0x00000100UL
/*-Zeus 2.0 additions-*/
/* BRCM_IF (BSP_CODEREV_POWAY) */
#define BCMD_OTP_FLT_STAT_SOCUID_OTP_KEY_ERR_SYNDROME               0x00000100UL
#define BCMD_OTP_FLT_STAT_NPRIV0_OTP_KEY_ERR_SYNDROME               0x00000100UL
#define BCMD_OTP_FLT_STAT_NPRIV1_OTP_KEY_ERR_SYNDROME               0x00000100UL
#define BCMD_OTP_FLT_STAT_NPRIV2_OTP_KEY_ERR_SYNDROME               0x00000100UL
#define BCMD_OTP_FLT_STAT_NPRIV3_OTP_KEY_ERR_SYNDROME               0x00000100UL
#define BCMD_OTP_FLT_STAT_NPRIV4_OTP_KEY_ERR_SYNDROME               0x00000100UL
#define BCMD_OTP_FLT_STAT_NPRIV5_OTP_KEY_ERR_SYNDROME               0x00000100UL
#define BCMD_OTP_FLT_STAT_NPRIV6_OTP_KEY_ERR_SYNDROME               0x00000100UL
#define BCMD_OTP_FLT_STAT_NPRIV7_OTP_KEY_ERR_SYNDROME               0x00000100UL
/* BRCM_ENDIF */


/* BRCM_IF (BSP_CODEREV_BROADCOM_ONLY) */

/********************************************************************
BCMD_cmdType_eOtpKeyProgPasswdCheck - causes otp key prog passwd check
This command is disabled when OTP 'DebugDisable' is TRUE

InCmdOtpKeyProgPasswdCheck:
|-------------------|-------------------|-------------------|------------------|
|  31..24           | 23..16            | 15..8             | 7..0             |
|-------------------|-------------------|-------------------|------------------|
|  BSP_OTP_PASSWD_0 -32 bits                                                   |
|-------------------|-------------------|-------------------|------------------|
|  BSP_OTP_PASSWD_1 -32 bits                                                   |
|-------------------|-------------------|-------------------|------------------|
|  BSP_OTP_PASSWD_2 -32 bits                                                   |
|-------------------|-------------------|-------------------|------------------|
|  BSP_OTP_PASSWD_3 -32 bits                                                   |
|-------------------|-------------------|-------------------|------------------|
*/
typedef enum BCMD_Otp_InCmdKeyProgPasswdCheck_e
{
    BCMD_Otp_InCmdKeyProgPasswdCheck_ePasswdArray   = (5 << 2) + 0 /* 32 bit index for start of passwd array */
} BCMD_Otp_InCmdKeyProgPasswdCheck_e;

/* BRCM_ENDIF */


/********************************************************************
BCMD_cmdType_eOtpProgPatternSequence - The command enables the otp 
programming by writing a sequence of 8 32-bit patterns which are passed
thru the command.

InCmdOtpProgPatternSequence:
|-------------------|-------------------|-------------------|------------------|
|  31..24           | 23..16            | 15..8             | 7..0             |
|-------------------|-------------------|-------------------|------------------|
|  PROG_PATTERN_0 -32 bits                                                     |
|-------------------|-------------------|-------------------|------------------|
|  PROG_PATTERN_1 -32 bits                                                     |
|-------------------|-------------------|-------------------|------------------|
|  PROG_PATTERN_2 -32 bits                                                     |
|-------------------|-------------------|-------------------|------------------|
|  PROG_PATTERN_3 -32 bits                                                     |
|-------------------|-------------------|-------------------|------------------|
|  PROG_PATTERN_4 -32 bits                                                     |
|-------------------|-------------------|-------------------|------------------|
|  PROG_PATTERN_5 -32 bits                                                     |
|-------------------|-------------------|-------------------|------------------|
|  PROG_PATTERN_6 -32 bits                                                     |
|-------------------|-------------------|-------------------|------------------|
|  PROG_PATTERN_7 -32 bits                                                     |
|-------------------|-------------------|-------------------|------------------|
|                                       |      PROG_PATTERN_8 - 16 bits        |
|-------------------|-------------------|-------------------|------------------|
*/
typedef enum BCMD_Otp_InCmdOtpProgPatternSequence_e
{
    BCMD_Otp_InCmdOtpProgPatternSequence_ePatternArray  = (5 << 2) + 0 /* 32 bit index for start of passwd array */
} BCMD_Otp_InCmdOtpProgPatternSequence_e;

/* BRCM_IF (BSP_CODEREV_BROADCOM_ONLY) */

#if defined(BSP_PATCH_UNIT_TEST)

/********************************************************************
BCMD_cmdType_eOTP_WRITE_EN - input and output buffer structure

InCmdWriteEn:
|-------------------|-------------------|-------------------|------------------|
|  31..24           | 23..16            | 15..8             | 7..0             |
|-------------------|-------------------|-------------------|------------------|
| 60 x 26Array OTP image to be used                                            |
|-------------------|-------------------|-------------------|------------------|

 */

/* for 65nm  only - removed 130nm chips*/
typedef enum BCMD_Otp_InCmdWriteEn_e
{
    BCMD_Otp_InCmdWriteEn_eKeysABC               = ( 5 << 2)+0,/* 32 bit index - index not used */
    BCMD_Otp_InCmdWriteEn_eMsp                   = (59 << 2)+0 /* 32 bit index - index not used */
}BCMD_Otp_InCmdWriteEn_e;

typedef enum BCMD_Otp_OutCmdWriteEn_e
{
    BCMD_Otp_OutCmdWriteEn_eReserved             = (6 << 2)+0 /* 32 bit index - index not used */
}BCMD_Otp_OutCmdWriteEn_e;

#endif /* BSP_PATCH_UNIT_TEST */

#if defined(BSP_DEBUG) && defined(BSP_DEBUG_ANTI_GLITCH_OTP_PROGRAM_UNIT_TEST)
typedef enum BCMD_Otp_InCmdAntiGlitchOtpProg_e
{
    BCMD_Otp_InCmdAntiGlitchOtpProg_eBitAddr        = (5 << 2)+3,  /*  8 bits */
    BCMD_Otp_InCmdAntiGlitchOtpProg_eRowAddr        = (6 << 2)+2,  /* 16 bits */
    BCMD_Otp_InCmdAntiGlitchOtpProg_eDataValue      = (7 << 2)+3   /*  8 bits */
}BCMD_Otp_InCmdAntiGlitchOtpProg_e;
#endif /* BSP_DEBUG_ANTI_GLITCH_OTP_PROGRAM_UNIT_TEST */

#if defined (BSP_DEBUG_OTP_UNIT_TEST)

/********************************************************************
BCMD_OTP_ExecuteUnitTestCmd - Process the otp unit test commands
This command is disabled when OTP 'DebugDisable' is TRUE

BCMD_Otp_InCmdUnitTestFormat: 
|-------------------|-------------------|-------------------|------------------|
|  31..24           | 23..16            | 15..8             | 7..0             |
|-------------------|-------------------|-------------------|------------------|
|  unused           | unused            | unused            | eSubCmdId - 8    |
|-------------------|-------------------|-------------------|------------------|
|  eSubCmdData3 - 8 | eSubCmdData2 - 8  | eSubCmdData1 - 8  | eSubCmdData0 - 8 |
|-------------------|-------------------|-------------------|------------------|

*/
typedef enum BCMD_Otp_InCmdUnitTestFormat_e
{
    BCMD_Otp_UnitTestCmdFormat_eSubCmdId     = (5 << 2) + 3, /* 8 bits */
    BCMD_Otp_UnitTestCmdFormat_eSubCmdData0  = (6 << 2) + 3, /* 8 bits */
    BCMD_Otp_UnitTestCmdFormat_eSubCmdData1  = (6 << 2) + 2, /* 8 bits */
    BCMD_Otp_UnitTestCmdFormat_eSubCmdData2  = (6 << 2) + 1, /* 8 bits */
    BCMD_Otp_UnitTestCmdFormat_eSubCmdData3  = (6 << 2) + 0 /* 8 bits */
}BCMD_Otp_UnitTestCmdFormat_e;

typedef enum BCMD_Otp_InCmdUnitTestSubCmdId_e
{
    BCMD_Otp_InCmdUnitTestSubCmdId_eLotpInvalidCmd                  = 1,
    BCMD_Otp_InCmdUnitTestSubCmdId_eLotpRdntRowTestCmd              = 2,
    BCMD_Otp_InCmdUnitTestSubCmdId_eLotpMspDataBitLockBitFailCmd    = 3,
    BCMD_Otp_InCmdUnitTestSubCmdId_eAntiGlitchOtpProgCmd            = 4,
    BCMD_Otp_InCmdUnitTestSubCmdId_eLotpMspDataBitVerifyFailCmd     = 5,
    BCMD_Otp_InCmdUnitTestSubCmdId_eAntiGlitchOtpProgRowCmd         = 6,
    BCMD_Otp_InCmdUnitTestSubcmdId_ePowaySectionProgramFailcmd      = 7,
    BCMD_Otp_InCmdUnitTestSubCmdId_eMaxCmd                          = 8
}BCMD_Otp_InCmdUnitTestSubCmdId_e;

typedef enum BCMD_Otp_OutCmdUnitTestErrorCode_e
{
   BCMD_Otp_OutCmdUnitTestErrorCode_eSubCmdId       = 0x64,
   BCMD_Otp_OutCmdUnitTestErrorCode_eLotpCmdId      = 0x65,
   BCMD_Otp_OutCmdUnitTestErrorCode_eRdntRowParam   = 0x66,
   BCMD_Otp_OutCmdUnitTestErrorCode_eMspParam       = 0x67
}BCMD_Otp_OutCmdUnitTestErrorCode_e;

#endif



/* BRCM_ENDIF */

/* BRCM_IF (BSP_CODEREV_BROADCOM_ONLY) */

/********************************************************************/
/* OTP defines and structures */

/* types as passed by the MIPS to the 8051 that say whether the CMD is in bit
 * mode or array mode
 */
typedef enum BCMD_Otp_CmdProgMode_e
{
    BCMD_Otp_CmdProgMode_eEnum    = 0x12,
    BCMD_Otp_CmdProgMode_eEnumHal = 0x34,
    BCMD_Otp_CmdProgMode_eBit     = 0x78,
    BCMD_Otp_CmdProgMode_eRow     = 0x55,
    BCMD_Otp_CmdProgMode_eReserved
} BCMD_Otp_CmdProgMode_e ;

/* BRCM_ENDIF */

/* BRCM_IF (BSP_CODEREV_BROADCOM_ONLY) */

/* This enum can be used to set the value of these two parameters:
 *   BCMD_Otp_InCmdOfflineProgOtp_eNumErrorsMax
 *   BCMD_Otp_InCmdDataSectionProg_eNumErrorsMax
 *
 * If more than the given number of errors occur during OTP programming
 * redundant rows are used.  This is the MAX value acceptable for
 * programming 26 bits of OTP.  If more than this number of errors
 * occur then that key or data section redundant rows are used.  There
 * are 6 redundant rows per section of OTP.
 */
typedef enum BCMD_Otp_eNumErrorsMax
{
    BCMD_Otp_eNumErrorsMax0        = 0,
    BCMD_Otp_eNumErrorsMax1        = 1,
    BCMD_Otp_eNumErrorsMax2        = 2,
    BCMD_Otp_eNumErrors_size       = 3
} BCMD_Otp_eNumErrorsMax;


/* BRCM_ENDIF */

/* select otp field to be read */
typedef enum BPI_Otp_CmdReadRegister_e 
{
/* BRCM_IF (BSP_CODEREV_NDS)   */
    BPI_Otp_CmdReadRegister_eMc0S_KeyFieldProgram                   = 0,
/* BRCM_ENDIF */
    BPI_Otp_CmdReadRegister_eMc0S_Reserved0                         = 0,

/* BRCM_IF (BSP_CODEREV_DIRECTV)   */
    BPI_Otp_CmdReadRegister_eMc0S_AvLockDirecTv                     = 1,
/* BRCM_ENDIF */
    BPI_Otp_CmdReadRegister_eMc0S_Reserved1                         = 1,

/* BRCM_IF (BSP_CODEREV_NDS) || (BSP_CODEREV_DIRECTV) */
    BPI_Otp_CmdReadRegister_eMc0S_PubOtpBIDNds                      = 2,
    BPI_Otp_CmdReadRegister_eMc0S_NVSecondaryV_VersionSelectNds     = 3,
/* BRCM_ENDIF */
    BPI_Otp_CmdReadRegister_eMc0S_Reserved2                         = 2,
    BPI_Otp_CmdReadRegister_eMc0S_Reserved3                         = 3,

/* BRCM_IF (BSP_CODEREV_BROADCOM_ONLY) */
    BPI_Otp_CmdReadRegister_eMc0S_DebugDisable                      = 4,
    BPI_Otp_CmdReadRegister_eMc0S_PatchRamDisable                   = 5,
/* BRCM_ENDIF */
    BPI_Otp_CmdReadRegister_eMc0S_Reserved4                         = 4,
    BPI_Otp_CmdReadRegister_eMc0S_Reserved5                         = 5,

    BPI_Otp_CmdReadRegister_eKeyMc0_CustomerMode                    = 6,

/* BRCM_IF (BSP_CODEREV_NDS) || (BSP_CODEREV_DIRECTV) */
    BPI_Otp_CmdReadRegister_eKeyMc0_SecModeNds                      = 7,
/* BRCM_ENDIF */
	BPI_Otp_CmdReadRegister_eKeyMc0_Reserved7 						= 7,
    BPI_Otp_CmdReadRegister_eKeyMc0_DeobfuscationEnable             = 8,
    BPI_Otp_CmdReadRegister_eKeyMc0_BlackBoxId                      = 9,
    BPI_Otp_CmdReadRegister_eKeyMc0_CaKeyLadderDisallow             = 10,
    BPI_Otp_CmdReadRegister_eKeyMc0_CpKeyLadderDisallow             = 11,
    BPI_Otp_CmdReadRegister_eKeyMc0_Gp1KeyLadderDisallow            = 12,
    BPI_Otp_CmdReadRegister_eKeyMc0_Gp2KeyLadderDisallow            = 13,

/* BRCM_IF (BSP_CODEREV_BROADCOM_ONLY) */
    BPI_Otp_CmdReadRegister_eKeyMc0_CasIndex                        = 14,
/* BRCM_ENDIF */
    BPI_Otp_CmdReadRegister_eKeyMc0_Reserved14                      = 14,
    
    BPI_Otp_CmdReadRegister_eKeyID                                  = 15,
    BPI_Otp_CmdReadRegister_eKeyHash                                = 16,
    BPI_Otp_CmdReadRegister_eBseckHashCrc                           = 17,       /* Crc for the first(stage) BseckHash */

/* BRCM_IF (BSP_CODEREV_DIRECTV)   */
    BPI_Otp_CmdReadRegister_eMc0S_DssPktProcessingEnableDirecTV     = 18,
/* BRCM_ENDIF */
    BPI_Otp_CmdReadRegister_eMc0S_Reserved18                        = 18,

    BPI_Otp_CmdReadRegister_eMc0S_FixedDeobfuscationVariantEnable   = 19,
    BPI_Otp_CmdReadRegister_eKeyMc0_RootKeySwapDisallow             = 20,

/* BRCM_IF (BSP_CODEREV_IRDETO)   */
    BPI_Otp_CmdReadRegister_eKeyMc0_ExtCustomerMode                 = 21,
/* BRCM_ENDIF */
    BPI_Otp_CmdReadRegister_eKeyMc0_Reserved21                      = 21,

#ifndef PRE_ZEUS_3_0_OTP
    BPI_Otp_CmdReadRegister_eStage2BseckHashCrc                     = 22,       //Crc for Stage2BseckHash
#endif

    BPI_Otp_CmdReadRegister_eSize
} BPI_Otp_CmdReadRegister_e ;

/* select otp key type for the field to be read */
typedef enum BPI_Otp_KeyType_e
{
    BPI_Otp_KeyType_eA,
    BPI_Otp_KeyType_eB,
    BPI_Otp_KeyType_eC,
    BPI_Otp_KeyType_eD,
    BPI_Otp_KeyType_eE,
    BPI_Otp_KeyType_eF,
    BPI_Otp_KeyType_eSize
} BPI_Otp_KeyType_e ;


#define BPI_OTP_PROGRAM_MSP_NUMBER_BITS_TO_PROG_MAX     32  /* how many bits */

/* there are 3 keys in OTP, the first and second have start/stop, the third
 * has lock/unlock.
 * This enum is overloaded to include the Hash section introduced in the LOTP.
 * It has been included here so as to have minimum impact on the PI as we do
 * not want to add new commands.
 */
typedef enum BPI_Otp_HalProgKey_e
{
    BPI_Otp_HalProgKey_eMc0_S,
    BPI_Otp_HalProgKey_eA,
    BPI_Otp_HalProgKey_eB,
    BPI_Otp_HalProgKey_eC,
    BPI_Otp_HalProgKey_eD,
    BPI_Otp_HalProgKey_eE,
    BPI_Otp_HalProgKey_eF,
    BPI_Otp_HalProgKey_eHash,
/* BRCM_IF (BSP_CODEREV_POWAY) */
    BPI_Otp_HalProgKey_eSOCUID      =  8,
    BPI_Otp_HalProgKey_eNPRIV0      =  9,
    BPI_Otp_HalProgKey_eNPRIV1      = 10,
    BPI_Otp_HalProgKey_eNPRIV2      = 11,
    BPI_Otp_HalProgKey_eNPRIV3      = 12,
    BPI_Otp_HalProgKey_eNPRIV4      = 13,
    BPI_Otp_HalProgKey_eNPRIV5      = 14,
    BPI_Otp_HalProgKey_eNPRIV6      = 15,
    BPI_Otp_HalProgKey_eNPRIV7      = 16,
/* BRCM_ENDIF */
    BPI_Otp_HalProgKey_eReserved8   =  8,
    BPI_Otp_HalProgKey_eReserved9   =  9,
    BPI_Otp_HalProgKey_eReserved10  = 10,
    BPI_Otp_HalProgKey_eReserved11  = 11,
    BPI_Otp_HalProgKey_eReserved12  = 12,
    BPI_Otp_HalProgKey_eReserved13  = 13,
    BPI_Otp_HalProgKey_eReserved14  = 14,
    BPI_Otp_HalProgKey_eReserved15  = 15,
    BPI_Otp_HalProgKey_eReserved16  = 16,
/* BRCM_ENDIF */
    BPI_Otp_HalProgKey_eStage2Hash  = 17,       /* This enum is used only in programming the section */
    BPI_Otp_HalProgKey_eSize                    /* 18 */
} BPI_Otp_HalProgKey_e ;


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

#endif /*  BSP_S_OTP_H__ */

