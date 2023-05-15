/***************************************************************************
 *     Copyright (c) 2005-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsp_s_mem_auth.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 8/12/08 6:11p $
 * Security Processor Code for Register Access
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7601/a0/region_verify/aegis/bsp_s_mem_auth.h $
 * 
 * Hydra_Software_Devel/1   8/12/08 6:11p atruong
 * PR45631: HSM PI support for BCM7601 platform
 * 
 * Aegis_Firmware_Devel/2   9/13/07 5:58p ksekar
 * PR 820: Session_Mem_Auth command format change
 * 
 * Aegis_Firmware_Devel/2   7/25/07 2:25p ksekar
 * PR 659: Add SCB Burst Size field to SessionMemAuth command
 * 
 * Aegis_Firmware_Devel/8   11/3/06 7:08p dbayani
 * PR 430: verification enhancements.
 * 
 * Aegis_Firmware_Devel/7   7/5/06 6:05p dbayani
 * PR 366, PR 367, PR 368, PR 353: Updated command to support E*
 * requirements for disabling region checking, query, interval check, and
 * pause.
 * 
 * Aegis_Firmware_Devel/6   3/3/06 5:29p dbayani
 * PR299: Added change to support NAND flash for region verification.
 * 
 * Aegis_Firmware_Devel/5   9/19/05 7:00p btan
 * PR 185: Updated the shared file for host compilation
 * 
 * Aegis_Firmware_Devel/4   8/22/05 8:22a dbayani
 * PR23: Added description of each host command.
 * 
 * Aegis_Firmware_Devel/3   8/2/05 9:07a dbayani
 * PR82: Item 3 - Added check pause option.
 * 
 * Aegis_Firmware_Devel/2   6/14/05 5:19p dbayani
 * PR13830: Coding Conventions update,
 * 
 * Aegis_Firmware_Devel/1   6/10/05 11:07a dbayani
 * PR13830: Added shared enums and defines with MIPS to share directory.
 * 
 *
 ***************************************************************************/

#ifndef BSP_S_MEM_AUTH_H__
#define BSP_S_MEM_AUTH_H__

/*

BCMD_MEMAUTH_ExecuteCommand - Command used to control region verification.
	This includes enabling, disabling regions, pausing, disabling
	region verification, and verifying the region is verified or paused.

Parameters passed:
eRegionOp - Enable region , disable region , pause verification, 
	disable verification, check if region is verified, check if
	verification is paused.
eRegionNum - Which region to apply above operation.
eStartAddr - Start address of region to enable.
eEndAddr - End address of region o enable.
eSigStartAddr - start address of region signature.
eSigEndAddr - end address of region signature.
eCustKeyH - Bits[5:0] used to select cust key high for swizzle 0.
	Bit[6] used to indicate DES encryption or decryption for key 2L generation.
eCustKeyL - - Bits[5:0] used to select cust key low for swizzle 0.
	Bit[6] used to indicate DES encryption or decryption for key 2H generation.
eKeyVarH - used to select key var high for swizzle 0.
eKeyVarL - used to select key var low for swizzle 0.
eProcInKey3 - data used as proc in for key3 generation.
eProcInKey4 - data used as proc in for key4 generation.
eSig - encrypted HMAC signature of input command.
	
Returned:
eStatus - Status of command.

BCMD_MemAuth_InCmdField_e: 
|-------------------|-------------------|-------------------|-------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             	| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            | eRegionOp - 8		| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            | eRegionNum - 8	| 
|-------------------|-------------------|-------------------|-------------------| 
|  eStartAddr - 32																| 
|-------------------|-------------------|-------------------|-------------------| 
|  eEndAddr - 32																| 
|-------------------|-------------------|-------------------|-------------------| 
|  eSigStartAddr - 32															| 
|-------------------|-------------------|-------------------|-------------------| 
|  eSigEndAddr - 32																| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused			| unused   			| unused			| eCustKeyH - 8		|																	| 
|-------------------|-------------------|-------------------|-------------------|
|  unused			| unused   			| unused			| eCustKeyL - 8		| 
|-------------------|-------------------|-------------------|-------------------|
|  unused			| unused   			| unused			| eKeyVarH - 8		|																	| 
|-------------------|-------------------|-------------------|-------------------|
|  unused			| unused   			| unused			| eKeyVarL - 8		| 
|-------------------|-------------------|-------------------|-------------------|
|  eProcInKey3 - 128															| 
|-------------------|-------------------|-------------------|-------------------|
|																				| 
|-------------------|-------------------|-------------------|-------------------|
|																				| 
|-------------------|-------------------|-------------------|-------------------|
|																				| 
|-------------------|-------------------|-------------------|-------------------|
|  eProcInKey4 - 128															| 
|-------------------|-------------------|-------------------|-------------------|
|																				| 
|-------------------|-------------------|-------------------|-------------------|
|																				| 
|-------------------|-------------------|-------------------|-------------------|
|																				| 
|-------------------|-------------------|-------------------|-------------------|
|  eSig - 128																	| 
|-------------------|-------------------|-------------------|-------------------|
|																				| 
|-------------------|-------------------|-------------------|-------------------|
|																				| 
|-------------------|-------------------|-------------------|-------------------|
|																				| 
|-------------------|-------------------|-------------------|-------------------|

    
BCMD_MemAuth_OutCmdField_e: 
|-------------------|-------------------|-------------------|-------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             	| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused			|  unused			|  unused			| eStatus -	8		|
|-------------------|-------------------|-------------------|-------------------| 

*/
typedef enum BCMD_MemAuth_InCmdField_e
{
    BCMD_MemAuth_InCmdField_eRegionOp       = (5<<2) + 3,   /* 8 bit index */
    BCMD_MemAuth_InCmdField_eRegionNum      = (6<<2) + 3,   /* 8 bit index */
    BCMD_MemAuth_InCmdField_eNumOfBadBlocks = (6<<2) + 3,   /* 8 bit index */
    BCMD_MemAuth_InCmdField_eStartAddr      = (7<<2) + 0,   /* 32 bit index */
    BCMD_MemAuth_InCmdField_eEndAddr        = (8<<2) + 0,   /* 32 bit index */
    BCMD_MemAuth_InCmdField_eSigStartAddr   = (9<<2) + 0,   /* 32 bit index */
    BCMD_MemAuth_InCmdField_eSigEndAddr     = (10<<2) + 0,  /* 32 bit index */
    BCMD_MemAuth_InCmdField_eIntervalCheckBw = (11<<2) + 3, /* 8 bit index */
    BCMD_MemAuth_InCmdField_eScbBurstSize   = (11<<2) + 2,  /* 8 bit index */
    BCMD_MemAuth_InCmdField_eFastCheckEn    = (11<<2) + 1,  /* 8 bit index */
    BCMD_MemAuth_InCmdField_eRsaKeyId       = (12<<2) + 3,  /* 8 bit index */
    BCMD_MemAuth_InCmdField_eCodeRules      = (13<<2) + 0,  /* 32 bit index */
    BCMD_MemAuth_InCmdField_eNoRelocatableCode = (13<<2) + 3, /* 8 bit index */
    BCMD_MemAuth_InCmdField_eCpuId          = (13<<2) + 2,  /* 8 bit index */
    BCMD_MemAuth_InCmdField_eEpoch          = (13<<2) + 1   /* 8 bit index */
    
} BCMD_MemAuth_InCmdField_e;

typedef enum BCMD_MemAuth_OutCmdField_e
{
    BCMD_MemAuth_OutCmdField_eStatus = (5<<2) + 3,   /* 8 bit index */
    BCMD_MemAuth_OutCmdField_eRegionPauseStatus = (6<<2) + 3,   /* 8 bit index */
    BCMD_MemAuth_OutCmdField_eRegion0Status = (7<<2) + 3   /* 8 bit index */    
        
} BCMD_MemAuth_OutCmdField_e;

typedef enum BCMD_MemAuth_Operation_e
{
	BCMD_MemAuth_Operation_eDisableRegion,
	BCMD_MemAuth_Operation_eEnableRegion,
	BCMD_MemAuth_Operation_eQueryRegionInfo,
	BCMD_MemAuth_Operation_ePause,
	BCMD_MemAuth_Operation_eRegionVerified,
	BCMD_MemAuth_Operation_eRegionVerifPaused,
	BCMD_MemAuth_Operation_eDefineBadBlocks,
	BCMD_MemAuth_Operation_eDefineRegion,
    BCMD_MemAuth_Operation_eMichDisable

} BCMD_MemAuth_Operation_e;

typedef enum BCMD_ScbBurstSize_e
{
    BCMD_ScbBurstSize_e64    = 0,
    BCMD_ScbBurstSize_e128   = 1,
    BCMD_ScbBurstSize_e256   = 2,
    BCMD_ScbBurstSize_eMax   = 3
    
} BCMD_ScbBurstSize_e;

typedef enum BCMD_MemAuth_CpuId_e
{
    BCMD_MemAuth_CpuId_eMips   = 0,
    BCMD_MemAuth_CpuId_eRaptor = 1,
    BCMD_MemAuth_CpuId_eAvd    = 2,
    BCMD_MemAuth_CpuId_eRave   = 3,
    BCMD_MemAuth_CpuId_eMax    = 4
    
} BCMD_MemAuth_CpuId_e;

#endif /* BSP_S_MEM_AUTH_H__ */

