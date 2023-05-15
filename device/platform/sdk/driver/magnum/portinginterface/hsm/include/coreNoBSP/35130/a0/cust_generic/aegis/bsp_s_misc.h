 /***************************************************************************
 *     Copyright (c) 2005-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsp_s_misc.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 4/14/09 1:39a $
 * Module Description: This file contains misc data structures that shall be shared with
 *					host. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/35130/a0/cust_generic/aegis/bsp_s_misc.h $
 * 
 * Hydra_Software_Devel/1   4/14/09 1:39a atruong
 * PR54104: HSM PI Support For 35130A0
 * 
 * Hydra_Software_Devel/1   10/3/08 1:27a atruong
 * PR46656: HSM PI Support for Rev B0
 * 
 * Aegis_Firmware_Devel/6   3/13/08 5:01p kostas
 * PR1021:
 * 
 * Aegis_Firmware_Devel/5   2/22/08 1:04p kostas
 * PR1021: Allow host to force xpt SC bit
 * 
 * Aegis_Firmware_Devel/4   2/18/08 12:45p kostas
 * PR1042: Remove ScArray command
 * 
 * Aegis_Firmware_Devel/3   2/28/07 2:42p sshekhar
 * PR 413: revision update for C0
 * 
 * Aegis_Firmware_Devel/2   9/11/06 3:38p sshekhar
 * PR 381: initial code base for DTV, 3563A0. SetMiscBits cmd update
 * 
 * Aegis_Firmware_Devel/8   3/21/06 8:07p hzhu
 * PR185: fixed a bug in Set_Arch command in-field enum
 * 
 * Aegis_Firmware_Devel/7   3/17/06 8:23p dbayani
 * PR311: Added new VICH command set REG PAR.
 * 
 * Aegis_Firmware_Devel/6   3/6/06 3:16p hzhu
 * PR291:added return rave_disable status bit and comment out misc
 * commands for 7401a0 due to code space.
 * 
 * Aegis_Firmware_Devel/5   3/5/06 8:19p hzhu
 * PR291:more bits to add
 * 
 * Aegis_Firmware_Devel/4   2/25/06 4:35p hzhu
 * PR291:change BCMD_MISC_SetRave to BCMD_MISC_SetMiscBits
 * 
 * Aegis_Firmware_Devel/3   9/19/05 7:02p btan
 * PR 185: Updated the shared file for host compilation
 * 
 * Aegis_Firmware_Devel/2   8/19/05 6:30a btan
 * PR 122: Modified the input and output commands to conform with coding
 * convention. Merged 3 RAVE input fields into 1 field.
 * 
 * Aegis_Firmware_Devel/1   6/16/05 1:55p btan
 * PR 13830:  Moved data structures to shared head files.
 * 
 *
 ***************************************************************************/
#ifndef BSP_S_MISC_H__
#define BSP_S_MISC_H__



/* sub command ID for BCMD_SetMiscBits command*/
typedef enum BCMD_SetMiscBitsSubCmd_e
{	
	BCMD_SetMiscBitsSubCmd_eRaveBits = 0,
	BCMD_SetMiscBitsSubCmd_eReserved1 = 1,
	BCMD_SetMiscBitsSubCmd_eReserved2 = 2,
	BCMD_SetMiscBitsSubCmd_eReserved3 = 3,
	BCMD_SetMiscBitsSubCmd_eForceSCBit = 4,
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

#define BCMD_SetMiscBitsField_InCmdField_eCtrlBits_IMEM_WR_Mask 0x00000001UL
#define BCMD_SetMiscBitsField_InCmdField_eCtrlBits_IMEM_RD_Mask 0x00000002UL
#define BCMD_SetMiscBitsField_InCmdField_eCtrlBits_DMEM_RD_Mask 0x00000004UL
#define BCMD_SetMiscBitsField_InCmdField_eCtrlBits_RaveDis_CLR_Mask 0x00000010UL

typedef enum BCMD_SetMiscBitsField_InCmdField_e
{
	BCMD_SetMiscBitsField_InCmdField_eSubCommandId = (5<<2) + 3, /* 8 bit index */
	BCMD_SetMiscBitsField_InCmdField_eCtrlBits = (6<<2) + 3, /* 8 bit index */
	BCMD_SetMiscBitsField_InCmdField_eBandSel = (6<<2) + 3, /* 8 bit index */
	BCMD_SetMiscBitsField_InCmdField_eBandNum = (6<<2) + 2, /* 8 bit index */
	BCMD_SetMiscBitsField_InCmdField_eForceSCbits = (7<<2) + 3 /* 8 bit index */

} BCMD_SetMiscBitsField_InCmdField_e;

typedef enum BCMD_SetMiscBitsField_OutCmdField_e
{
	BCMD_SetMiscBitsField_OutCmdField_eRaveStatusBit = (6 <<2) + 3 /* 8 bit index */	
} BCMD_SetMiscBitsField_OutCmdField_e;

#endif /* BSP_S_MISC_H__ */

