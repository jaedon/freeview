/***************************************************************************
 *     Copyright (c) 2005-2007, Broadcom Corporation
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
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/15/07 6:12p $
 * Broadcom Security Processor Code
 ***************************************************************************/
 
/**************************************************************************
        
 OTP Command API functions available to MIPs - documented in this H file 
 
 ***************************************************************************/ 

#ifndef BSP_S_OTP_H__
#define BSP_S_OTP_H__



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

#define BCMD_OTP_PROGMSP_MODE   0x00010112

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

typedef enum BCMD_Otp_OutCmdDataSectionRead_e
{
    BCMD_Otp_OutCmdDataSectionRead_eArrayData         = (6 << 2)+0  /* 32 bit index */
    
}BCMD_Otp_OutCmdDataSectionRead_e;

/********************************************************************
BCMD_cmdType_eOTP_DATA_SECTION_PROG - PROG one data section

eEnum - BPI_Otp_DataSection_e data selection 
eArrayData - 32 bytes, arranged as 32 bit x 8 array
*/
/*
eMode32           - BCMD_OTP_DATASECTIONPROG_MODE  
*/
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
/*
|  eMode32 - 32                                                                | */
/*
|-------------------|-------------------|-------------------|------------------| 
|  eCRC - 32 bits                                                              | 
|-------------------|-------------------|-------------------|------------------| 

 */
 
typedef enum BCMD_Otp_InCmdDataSectionProg_e 
{
    BCMD_Otp_InCmdDataSectionProg_eEnum               = (5 << 2)+3, /* 8  bit index */
    BCMD_Otp_InCmdDataSectionProg_eArrayData          = (6 << 2)+0, /* 32 bit index */
    
    BCMD_Otp_InCmdDataSectionProg_eReserved0          = (14<< 2)+3,  /* 8 bit index */
    BCMD_Otp_InCmdDataSectionProg_eReserved1          = (14<< 2)+2,  /* 8 bit index */
    BCMD_Otp_InCmdDataSectionProg_eReserved2          = (14<< 2)+1,  /* 8 bit index */  
    BCMD_Otp_InCmdDataSectionProg_eMode32             = (14<< 2)+0, /* 32 bit index */
    

    BCMD_Otp_InCmdDataSectionProg_eCrc                = (15<< 2)+0  /* 32 bit index */

}BCMD_Otp_InCmdDataSectionProg_e;

typedef enum BCMD_Otp_OutCmdDataSectionProg_e
{
    BCMD_Otp_OutCmdDataSectionProg_eReserved          = (6 << 2)+0  /* 32 bit index */
}BCMD_Otp_OutCmdDataSectionProg_e;

#define BCMD_OTP_DATASECTIONPROG_MODE   0x00010112



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
| eFltStatKeyC   - 32                                                          | 
|-------------------|-------------------|-------------------|------------------| 
| eFltStatDs     - 32                                                          | */
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
    
    BCMD_Otp_OutCmdStatus_eFltStatDs                  = (12 << 2)+0,  /* 32 bit index */

    BCMD_Otp_OutCmdStatus_eReserved0                  = (12 << 2)+0, 
/* all 65nm defines - removed 130nm defines */
    BCMD_Otp_OutCmdStatus_eReserved1                  = (13 << 2)+0, 
    BCMD_Otp_OutCmdStatus_eReserved2                  = (14 << 2)+0, 
    BCMD_Otp_OutCmdStatus_eReserved3                  = (15 << 2)+0, 
    BCMD_Otp_OutCmdStatus_eReserved4                  = (16 << 2)+0, 
    BCMD_Otp_OutCmdStatus_eReserved5                  = (17 << 2)+0, 
    BCMD_Otp_OutCmdStatus_eReserved6                  = (18 << 2)+0, 
    BCMD_Otp_OutCmdStatus_eReserved7                  = (19 << 2)+0,
    BCMD_Otp_OutCmdStatus_eReserved8                  = (20 << 2)+0, 
    BCMD_Otp_OutCmdStatus_eReserved9                  = (21 << 2)+0, 
    BCMD_Otp_OutCmdStatus_eReserved10                 = (22 << 2)+0,


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

    BPI_Otp_CmdReadRegister_eReserved9              = 9,
    BPI_Otp_CmdReadRegister_eReserved10             = 10,
    BPI_Otp_CmdReadRegister_eReserved11             = 11,
    BPI_Otp_CmdReadRegister_eReserved12             = 12,
    
    BPI_Otp_CmdReadRegister_eReserved13             = 13,

    BPI_Otp_CmdReadRegister_eReserved14             = 14,
    BPI_Otp_CmdReadRegister_eReserved15             = 15,
    BPI_Otp_CmdReadRegister_eReserved16             = 16,
    BPI_Otp_CmdReadRegister_eSize
} BPI_Otp_CmdReadRegister_e ;


typedef enum BPI_Otp_HalOperationMode_e 
{
    BPI_Otp_HalOperationMode_eGeneric       = 0x00,
    
    BPI_Otp_HalOperationMode_eReserved1     = 0x01,


    BPI_Otp_HalOperationMode_eReserved2     = 0x02,


    BPI_Otp_HalOperationMode_eReserved3     = 0x03,

    
    BPI_Otp_HalOperationMode_eReserved4     = 0x04,


    BPI_Otp_HalOperationMode_eReserved5     = 0x05,


    BPI_Otp_HalOperationMode_eReserved6     = 0x06,


    BPI_Otp_HalOperationMode_eReserved7     = 0x07,

    
    BPI_Otp_HalOperationMode_eReserved8     = 0x08,


    BPI_Otp_HalOperationMode_eReserved9     = 0x09,


    BPI_Otp_HalOperationMode_eReserved10     = 0x0a,


    BPI_Otp_HalOperationMode_eReserved11    = 0x0b,

    
    BPI_Otp_HalOperationMode_eReserved12    = 0x0c,


    BPI_Otp_HalOperationMode_eReserved13    = 0x0d, 


    BPI_Otp_HalOperationMode_eReserved14    = 0x0e 


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

#endif /*  BSP_S_OTP_H__ */

