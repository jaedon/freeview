 /***************************************************************************
 *     Copyright (c) 2005-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsp_s_misc.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 11/15/06 3:11p $
 * Module Description: This file contains misc data structures that shall be shared with
 *					host. 
 *
 ***************************************************************************/
#ifndef BSP_S_MISC_H__
#define BSP_S_MISC_H__

/*

BCMD_cmdType_eUSER_SC_ARRAY_STATUS  - Use this command to 
read the scrambling control array, 2 bits per PID 
channel and each SC array supports 16 channels

Input Parameters passed:
None


BCMD_GetSCArrayField_InCmdField_e
|-------------------|-------------------|-------------------|------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             | 
|-------------------|-------------------|-------------------|------------------| 
|  None                                                                        |
|-------------------|-------------------|-------------------|------------------|

Output Parameters:
1)  SC Array 0: Status for SC Array 0
2)  SC Array 1: Status for SC Array 1
3)  SC Array 2: Status for SC Array 2
4)  SC Array 3: Status for SC Array 3


BCMD_GetSCArrayField_OutCmdField_e: 
|-------------------|-------------------|-------------------|------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             | 
|-------------------|-------------------|-------------------|------------------| 
|  SC Array 0 - 32                                                             | 
|-------------------|-------------------|-------------------|------------------| 
|  SC Array 1 - 32                                                             | 
|-------------------|-------------------|-------------------|------------------| 
|  SC Array 2 - 32                                                             | 
|-------------------|-------------------|-------------------|------------------| 
|  SC Array 3 - 32                                                             | 
|-------------------|-------------------|-------------------|------------------| 

 */
typedef enum BCMD_GetSCArrayField_OutCmdField_e
{
	BCMD_GetSCArrayField_OutCmdField_eLastArray = (9<<2) + 0 /* 32 bit index */	
} BCMD_GetSCArrayField_OutCmdField_e;

/* sub command ID for BCMD_SetMiscBits command*/
typedef enum BCMD_SetMiscBitsSubCmd_e
{
#if defined(BCM3563A0)
	BCMD_SetMiscBitsSubCmd_eRaveBits = 0,
#endif
	BCMD_SetMiscBitsSubCmd_eMax	
}BCMD_SetMiscBitsSubCmd_e;


/*

BCMD_cmdType_eOFFLINE_SetMiscBits_COMMAND  - Use this command to 
control if host can access to RAVE memories

Input Parameters passed:
1)  Control Bits for BCMD_SetMiscBitsSubCmd_eRaveBits:
Bit 0: EnableWrImem: 1 = Enable RAVE0 IMEM to be written by host, 0 = Disable. 
	Preferably we should set this to 0 to prevent host from writing to Rave0 IMEM.
Bit 1: EnableRdImem: 1 = Enable RAVE0 IMEM to be read by host, 0 = Disable
Bit 2: EnableRdDmem: 1 = Enable RAVE0 DMEM to be read by host, 0 = Disable. 
	Preferably we should set this to 1 to enable host to read Rave0 DRAM.
Bit 3: Bypass encryption engine for RAVE.
Bit 4: RAVE0_DISABLE_CLR

BCMD_SetMiscBitsField_InCmdField_e
|-------------------|-------------------|-------------------|------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             | 
|-------------------|-------------------|-------------------|------------------| 
|  unused           | unused            | unused            |rave clear bit status | 
|-------------------|-------------------|-------------------|------------------|

Output Parameters:
Bit 0 is the status bit of rave clear bit


BCMD_SetMiscBitsField_InCmdField_e: 
|-------------------|-------------------|-------------------|------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             | 
|-------------------|-------------------|-------------------|------------------| 
|                                                            rave status for rave subcommand| 
|-------------------|-------------------|-------------------|------------------| 

 */
typedef enum BCMD_SetMiscBitsField_InCmdField_e
{
	BCMD_SetMiscBitsField_InCmdField_eSubCommandId = (5<<2) + 3, /* 8 bit index */
	BCMD_SetMiscBitsField_InCmdField_eCtrlBits = (6<<2) + 3 /* 8 bit index */
} BCMD_SetMiscBitsField_InCmdField_e;


#endif /* BSP_S_MISC_H__ */

