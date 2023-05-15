/***************************************************************************
 *     Copyright (c) 2005-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * This file contains header information for commands relating to download code.
 *
 * $brcm_Workfile: bsp_s_download.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 2/8/11 11:54a $
 * Broadcom Security Processor Code
 ***************************************************************************/
 
/**************************************************************************
        
 Download Code Related Command API functions available to MIPs are documented 
 in this H file 
 
 ***************************************************************************/ 

#ifndef BSP_S_DOWNLOAD_H__
#define BSP_S_DOWNLOAD_H__

/*
BCMD_cmdType_eSECOND_TIER_KEY_VERIFY - perform signature verification of specified 
second-tier key using specified root key

Parameters passed:
eKeyIdentifier - the key number that needs to be verified. Valid values are 2 to 8
eRootKeySource - the root key using which to verify eKeyIdentifier. Valid values
                 are 0 for Key0Prime and 1 for Key0
eFlashAddress  - flash start address of where the second-tier key, its parameters
                 and the signature are stored. The size of this data in flash is
                 constant: 256 (Key) + 4 (parameters) + 256 (signature) bytes

InCmdSecondTierKeyVerify: 
|-------------------|-------------------|-------------------|------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             | 
|-------------------|-------------------|-------------------|------------------| 
|  unused           | unused            | unused            | eKeyIdentifier-8 | 
|-------------------|-------------------|-------------------|------------------| 
|  unused           | unused            | unused            | eRootKeySource-8 |
|-------------------|-------------------|-------------------|------------------| 
|  eFlashAddress - 32                                                          |
|-------------------|-------------------|-------------------|------------------| 
    
OutCmdSecondTierKeyVerify: 
|-------------------|-------------------|-------------------|------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             | 
|-------------------|-------------------|-------------------|------------------| 
|  unused           | unused            | unused            | eStatus - 8      |
|-------------------|-------------------|-------------------|------------------| 
*/

typedef enum BCMD_Download_InCmdSecondTierKeyVerify_e
{
    BCMD_Download_InCmdSecondTierKeyVerify_eKeyIdentifier   = ( 5 << 2)+3, /* 8  bit index */
    BCMD_Download_InCmdSecondTierKeyVerify_eRootKeySource   = ( 6 << 2)+3, /* 8  bit index */
    BCMD_Download_InCmdSecondTierKeyVerify_eAddress         = ( 7 << 2)+0  /* 32 bit index */
}BCMD_Download_InCmdSecondTierKeyVerify_e;

typedef enum BCMD_Download_OutCmdSecondTierKeyVerify_e
{
    BCMD_Download_OutCmdSecondTierKeyVerify_eReserved       = (6 << 2)+0   /* 32 bit index */
}BCMD_Download_OutCmdSecondTierKeyVerify_e;

/*
BCMD_cmdType_eSECOND_STAGE_CODE_LOAD - store the parameters specified for second stage
code download in sticky RAM, and reset the chip

Parameters passed: 
eFlashAddrSecondStageCodePtrs - flash address where the code pointers to the second stage
                                code are located
eFlashAddrSecondStageCodeSig  - flash address where the signature of the second stage code
                                is located
eKeySelect                    - second-tier key number that is used to verify the second
                                stage code. Valid values are 1 to 8.
                                
InCmdSecondStageCodeLoad: 
|-------------------|-------------------|-------------------|------------------|
|  31..24           | 23..16            | 15..8             | 7..0             | 
|-------------------|-------------------|-------------------|------------------|
|  eFlashAddrSecondStageCodePtrs - 32                                          |
|-------------------|-------------------|-------------------|------------------|
|  eFlashAddrSecondStageCodeSig - 32                                           |
|-------------------|-------------------|-------------------|------------------|
|  unused           | unused            | unused            | eKeySelect - 8   |
|-------------------|-------------------|-------------------|------------------|

OutCmdSecondStageCodeLoad: 
|-------------------|-------------------|-------------------|------------------|
|  31..24           | 23..16            | 15..8             | 7..0             |
|-------------------|-------------------|-------------------|------------------|
|  unused           | unused            | unused            | eStatus - 8      |
|-------------------|-------------------|-------------------|------------------|
*/

typedef enum BCMD_Download_InCmdSecondStageCodeLoad_e
{
    BCMD_Download_InCmdSecondStageCodeLoad_eFlashAddrSecondStageCodePtrs = (5 << 2)+0, /* 32 bit */
    BCMD_Download_InCmdSecondStageCodeLoad_eFlashAddrSecondStageCodeSig = (6 << 2)+0, /* 32 bits */
    BCMD_Download_InCmdSecondStageCodeLoad_eKeySelect = (7 << 2)+3 /* 8 bits
    */
}BCMD_Download_InCmdSecondStageCodeLoad_e;

typedef enum BCMD_Download_OutCmdSecondStageCodeLoad_e
{
    BCMD_Download_OutCmdSecondStageCodeLoad_eReserved = (6 << 2)+0 /* 32 bit */ 
}BCMD_Download_OutCmdSecondStageCodeLoad_e;

/* Enums for command fields */

typedef enum BCMD_SecondTierKeyVerify_KeyIdentifier_e
{
    BCMD_SecondTierKeyVerify_KeyIdentifier_eKey1   = 1,
    BCMD_SecondTierKeyVerify_KeyIdentifier_eKey2   = 2,
    BCMD_SecondTierKeyVerify_KeyIdentifier_eKey3   = 3,
    BCMD_SecondTierKeyVerify_KeyIdentifier_eKey4   = 4,
    BCMD_SecondTierKeyVerify_KeyIdentifier_eKey5   = 5,
    BCMD_SecondTierKeyVerify_KeyIdentifier_eKey6   = 6,
    BCMD_SecondTierKeyVerify_KeyIdentifier_eKey7   = 7,
    BCMD_SecondTierKeyVerify_KeyIdentifier_eKey8   = 8
} BCMD_SecondTierKeyVerify_KeyIdentifier_e;

typedef enum BCMD_SecondTierKeyVerify_RootKeySource_e
{
    BCMD_SecondTierKeyVerify_RootKeySource_eKey0Prime  = 0,
    BCMD_SecondTierKeyVerify_RootKeySource_eKey0       = 1
}BCMD_SecondTierKeyVerify_RootKeySource_e;

typedef enum BCMD_SecondStageCodeLoad_KeySelect_e
{
    BCMD_SecondStageCodeLoad_KeySelect_eKey1       = 1,
    BCMD_SecondStageCodeLoad_KeySelect_eKey2       = 2,
    BCMD_SecondStageCodeLoad_KeySelect_eKey3       = 3,
    BCMD_SecondStageCodeLoad_KeySelect_eKey4       = 4,
    BCMD_SecondStageCodeLoad_KeySelect_eKey5       = 5,
    BCMD_SecondStageCodeLoad_KeySelect_eKey6       = 6,
    BCMD_SecondStageCodeLoad_KeySelect_eKey7       = 7,
    BCMD_SecondStageCodeLoad_KeySelect_eKey8       = 8
}BCMD_SecondStageCodeLoad_KeySelect_e;    

#endif /*  BSP_S_DOWNLOAD_H__ */

