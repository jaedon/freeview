/***************************************************************************
 *     Copyright (c) 2005-2008, Broadcom Corporation
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
 * $brcm_Date: 8/12/08 9:33p $
 * Broadcom Security Processor Code
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7325/b0/cust_generic/aegis/bsp_s_download.h $
 * 
 * Hydra_Software_Devel/1   8/12/08 9:33p atruong
 * PR44849: 7335 B0 Bringup/Cleanup
 * 
 * Aegis_Firmware_Devel/2   4/24/07 2:24p ksekar
 * PR 505: Update second_tier_key_verify and second_stage_code_load
 * commands
 * 
 * Aegis_Firmware_Devel/1   2/25/07 1:37p ksekar
 * PR 505: Share file for download code and second-tier key verify
 * commands
 * 
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
    BCMD_Download_InCmdSecondTierKeyVerify_eFlashAddress    = ( 7 << 2)+0  /* 32 bit index */
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

#endif /*  BSP_S_DOWNLOAD_H__ */

