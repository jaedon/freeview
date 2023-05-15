/***************************************************************************
 *     Copyright (c) 2005-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsp_s_str.h $
 * $brcm_Revision: Aegis_Firmware_Devel/1 $
 * $brcm_Date: 11/30/11 6:03p $
 * Broadcom Security Processor Code
 ***************************************************************************/
#ifndef BSP_S_GenerateRouteOnceUsedRandomMacKey_H__
#define BSP_S_GenerateRouteOnceUsedRandomMacKey_H__

/* This header file contains common enums for BSP and host*/
/*
|--------------|------------|---------|----------------------------------------| 
|  31..24      | 23..16     | 15..8   | 7..0                                   | 
|--------------|------------|---------|----------------------------------------| 
|  unused      | unused     | unused  |  BCMD_GenerateRouteOnceUsedRandomMacKey_InCmd_eOtpID                 | 
|--------------|------------|---------|----------------------------------------|
|  unused      | unused     | unused  |  BCMD_GenerateRouteOnceUsedRandomMacKey_InCmd_eRngGenFlag            | 
|--------------|------------|---------|----------------------------------------|
|  unused      | unused     | unused  |  BCMD_GenerateRouteOnceUsedRandomMacKey_InCmd_eM2MSlotNum            | 
|--------------|------------|---------|----------------------------------------|
*/
typedef enum BCMD_GenerateRouteOnceUsedRandomMacKey_InCmdField_e
{
    BCMD_GenerateRouteOnceUsedRandomMacKey_InCmd_eOtpID              	= (5<<2) + 3,  /* 8 bit index*/
    BCMD_GenerateRouteOnceUsedRandomMacKey_InCmd_eRngGenFlag			= (6<<2) + 3,  /* 8 bit index*/
    BCMD_GenerateRouteOnceUsedRandomMacKey_InCmd_eM2MSlotNum            = (7<<2) + 3   /* 8 bit index*/     
}BCMD_GenerateRouteOnceUsedRandomMacKey_InCmdField_e;

typedef enum BCMD_RngGenFlag_e 
{
    BCMD_RngGenFlag_eNoGen			= 0, 
    BCMD_RngGenFlag_eGen			= 1, 
    BCMD_RngGenFlag_eMax
} BCMD_RngGenFlag_e;


#endif

