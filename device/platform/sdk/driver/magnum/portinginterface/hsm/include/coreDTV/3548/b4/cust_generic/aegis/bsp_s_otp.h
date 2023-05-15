/***************************************************************************
 *     Copyright (c) 2005-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * OTP command layer.
 *
 
 ***************************************************************************/ 

#ifndef BSP_S_OTP_H__
#define BSP_S_OTP_H__



/*

BCMD_cmdType_eOFFLINE_OTP_READ - read OTP settings for given OTP bits

Parameters passed:
eSubCmd


InCmdOfflineOtpRead: 
|-------------------|-------------------|-------------------|------------------|
|  31..24           | 23..16            | 15..8             | 7..0             |
|-------------------|-------------------|-------------------|------------------|
|  unused           | unused            | unused            | eSubCmd - 8      |
|-------------------|-------------------|-------------------|------------------|
|                   eBitAddress - 32 [Only for SubCmd = RAW READ]              |
|-------------------|-------------------|-------------------|------------------|

OutCmdOfflineOtpRead: at offset eRegValueLo and eRegValueHi are each half of register
being read when a 64 bit register is being read.  When a 32 bit register is
being read only the eRegValueLo is used.
|-------------------|-------------------|-------------------|------------------|
|  31..24           | 23..16            | 15..8             | 7..0             |
|-------------------|-------------------|-------------------|------------------|
|                  eDataValue - 32                                             |
|-------------------|-------------------|-------------------|------------------|


 */
typedef enum BCMD_Otp_InCmdOfflineOtpRead_e
{
    BCMD_Otp_InCmdOfflineOtpRead_eSubCmd                = (5 << 2)+3,  /* 8  bit index */
    BCMD_Otp_InCmdOfflineOtpRead_eRawBitAddress         = (6 << 2)+2  /* 16 bit index */
}BCMD_Otp_InCmdOfflineOtpRead_e;


typedef enum BCMD_Otp_OutCmdOfflineOtpRead_e
{
    /* The data value could represent the 32 bit data read for bit address, 32 bit key id, 
    ** or the 32 bit fault status value
    */
    BCMD_Otp_OutCmdOfflineOtpRead_eDataValue            = (6 << 2)+0, /* 32 bit index */
}BCMD_Otp_OutCmdOfflineOtpRead_e;


typedef enum BCMD_Otp_CmdReadSubCmd_e
{
    
    BCMD_Otp_CmdReadSubCmd_eRawBit      = 0,
    BCMD_Otp_CmdReadSubCmd_eKeyId       = 1,
    BCMD_Otp_CmdReadSubCmd_eFaultStatus = 2,
    BCMD_Otp_CmdReadSubCmd_eMax         = 3,
}BCMD_Otp_CmdReadSubCmd_e;


#endif /*  BSP_S_OTP_H__ */

