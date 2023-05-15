 /***************************************************************************
 *     Copyright (c) 2005-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsp_s_misc.h $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 6/8/07 6:42p $
 * Module Description: This file contains misc data structures that shall be shared with
 *					host. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7400/b0/cust_generic/aegis/bsp_s_misc.h $
 * 
 * Hydra_Software_Devel/3   6/8/07 6:42p lyang
 * PR 27711: HSM Purge and shared *.h source udpated for 7400A0,
 * 7401B0/C1/C2/C3
 * 
 * Hydra_Software_Devel/2   5/9/07 3:39p lyang
 * PR 27711: Add 7400B0, 7405A0 support to HSM. Purge and shared *.h
 * source udpated
 * 
 * Aegis_Firmware_Devel/14   10/27/06 1:49p wychen
 * PR464: Add Start AVD command
 * 
 * Aegis_Firmware_Devel/13   10/19/06 10:48a rjejuri
 * PR 388: Adding 7440 #define for sucessful compilation
 * 
 * Aegis_Firmware_Devel/12   10/18/06 12:22p whou
 * PR397: add BCM7400B0 support.
 * 
 * Aegis_Firmware_Devel/11   9/22/06 3:16p btan
 * PR 426: Fixed the header files that shared between Mips and BSP.
 * 
 * Aegis_Firmware_Devel/10   8/21/06 12:41p hzhu
 * PR387:code cleanup to change customer code review compile flag to
 * formatted comments to separate customer code review from real
 * functional code build.
 * 
 * Aegis_Firmware_Devel/9   7/7/06 9:48p hzhu
 * PR369:PR354:SetArch command is redefined and added Offline_PCI_MEMWIN
 * command
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
5)  SC Array 4: Status for SC Array 4
6)  SC Array 5: Status for SC Array 5
7)  SC Array 6: Status for SC Array 6
8)  SC Array 7: Status for SC Array 7
9)  SC Array 8: Status for SC Array 8
10) SC Array 9: Status for SC Array 9
11) SC Array 10: Status for SC Array 10
12) SC Array 11: Status for SC Array 11
13) SC Array 12: Status for SC Array 12
14) SC Array 13: Status for SC Array 13
15) SC Array 14: Status for SC Array 14
16) SC Array 15: Status for SC Array 15


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
|  SC Array 4 - 32                                                             | 
|-------------------|-------------------|-------------------|------------------| 
|  SC Array 5 - 32                                                             | 
|-------------------|-------------------|-------------------|------------------| 
|  SC Array 6 - 32                                                             | 
|-------------------|-------------------|-------------------|------------------| 
|  SC Array 7 - 32                                                             | 
|-------------------|-------------------|-------------------|------------------| 
|  SC Array 8 - 32                                                             | 
|-------------------|-------------------|-------------------|------------------| 
|  SC Array 9 - 32                                                             | 
|-------------------|-------------------|-------------------|------------------| 
|  SC Array 10 - 32                                                            | 
|-------------------|-------------------|-------------------|------------------| 
|  SC Array 11 - 32                                                            | 
|-------------------|-------------------|-------------------|------------------| 
|  SC Array 12 - 32                                                            | 
|-------------------|-------------------|-------------------|------------------| 
|  SC Array 13 - 32                                                            | 
|-------------------|-------------------|-------------------|------------------| 
|  SC Array 14 - 32                                                            | 
|-------------------|-------------------|-------------------|------------------| 
|  SC Array 15 - 32                                                            | 
|-------------------|-------------------|-------------------|------------------| 

 */
typedef enum BCMD_GetSCArrayField_OutCmdField_e
{
	BCMD_GetSCArrayField_OutCmdField_eLastArray = (21<<2) + 0 /* 32 bit index */	
} BCMD_GetSCArrayField_OutCmdField_e;

/* sub command ID for BCMD_SetMiscBits command*/
typedef enum BCMD_SetMiscBitsSubCmd_e
{
	BCMD_SetMiscBitsSubCmd_eRaveBits = 0,
	BCMD_SetMiscBitsSubCmd_eReserved1=1,   /* need to udpate the shared *.h to avoid overriding*/
	BCMD_SetMiscBitsSubCmd_eReserved2=2,		
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
*/
/*
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

typedef enum BCMD_SetMiscBitsField_OutCmdField_e
{
	BCMD_SetMiscBitsField_OutCmdField_eRaveStatusBit = (6 <<2) + 3 /* 8 bit index */	
} BCMD_SetMiscBitsField_OutCmdField_e;

/*

BCMD_cmdType_eOFFLINE_ARCH_COMMAND  - The host DRAM is 
partitioned into Secure Region and Global Region. The 
OFFLINE_ARCH_COMMAND command is used to configure the 
address range of each region and control which client 
module can access to that specific region. Refer RDB 
document on registers MEMC_SECURITY_ARCHx_CNTRL_REG, 
MEMC_SECURITY_ARCHx_ADRS_RANGE_LOW, 
MEMC_SECURITY_ARCHx_ADRS_RANGE_HIGH, 
MEMC_SECURITY_ARCHx_READ_RIGHTS_LOW, 
MEMC_SECURITY_ARCHx_READ_RIGHTS_HIGH, 
MEMC_SECURITY_ARCHx_WRITE_RIGHTS_LOW and 
MEMC_SECURITY_ARCHx_WRITE_RIGHTS_HIGH (where x = 0 or 
1) for details.
Note that since ARCH0 is reserved for secure region, 
the value specified in 
MEMC_SECURITY_ARCH0_ADRS_RANGE_LOW, 
MEMC_SECURITY_ARCH0_ADRS_RANGE_HIGH shall be copied 
to XPT_SECURITY_SECURITY_REGION_ADDR_LO and 
XPT_SECURITY_SECURITY_REGION_ADDR_HI automatically.  
Refer RDB document on registers 
XPT_SECURITY_SECURITY_REGION_ADDR_LO 
and XPT_SECURITY_SECURITY_REGION_ADDR_HI


Input Parameters passed:
1)  Arch Slot	
0x00 - Slot 0 used to control the accessibility to Secure Region
0x01 - Slot 1 used to control the accessibility to Global Region

2) Control Mode	
bit 0 - Mode: 0 - Non-Exclusive, 1 - Exclusive
bit 1 - Read_Check: 0 - Disable, 1 -Enable
bit 2 - Write_Check: 0 - Disable, 1 -Enable	

3) Lower Range Address: Address for lower range
4) Upper Range Address: Address for upper range
5) Lower Read Rights: Set Lower Read Rights
6) Upper Read Rights: Set Upper Read Rights
7) Lower Write Rights: Set Lower Write Rights
8) Upper Write Rights: Set Upper Write Rights




BCMD_MISC_SetArchField_InCmdField_e
|-------------------|-------------------|-------------------|------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             | 
|-------------------|-------------------|-------------------|------------------| 
|  unused           | unused            | unused            |Slot - 8          | 
|-------------------|-------------------|-------------------|------------------|
|  unused           | unused            | unused            |ControlMode - 8   | 
|-------------------|-------------------|-------------------|------------------|
| AddrRangeLow - 32                                                            | 
|-------------------|-------------------|-------------------|------------------|
| AddrRangeHigh - 32                                                           | 
|-------------------|-------------------|-------------------|------------------|
| ReadRightsLow - 32                                                           |
|-------------------|-------------------|-------------------|------------------|
| ReadRightsHigh - 32                                                          |
|-------------------|-------------------|-------------------|------------------|
| WriteRightsLow - 32                                                          |
|-------------------|-------------------|-------------------|------------------|
| WriteRightsHigh - 32                                                         |
|-------------------|-------------------|-------------------|------------------|

Output Parameters:
None


BCMD_MISC_SetArchField_OutCmdField_e: 
|-------------------|-------------------|-------------------|------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             | 
|-------------------|-------------------|-------------------|------------------| 
|                                                           |Rave_Clr bits     | 
|-------------------|-------------------|-------------------|------------------| 

 */

typedef enum BCMD_MISC_SetArchField_InCmdField_e
{
    BCMD_MISC_SetArchField_InCmdField_eAddrRangeLow = (5 << 2) + 0, /* 32 bit index */
    BCMD_MISC_SetArchField_InCmdField_eAddrRangeHigh = (6 << 2) + 0 /* 32 bit index */   
} BCMD_MISC_SetArchField_InCmdField_e;

/*
BCMD_MISC_SetVichField_InCmdField_e
|-------------------|-------------------|-------------------|------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             | 
|-------------------|-------------------|-------------------|------------------| 
|  unused           | unused            | unused            |VichId - 8          | 
|-------------------|-------------------|-------------------|------------------|
|RegPar0Start - 32                                                            | 
|-------------------|-------------------|-------------------|------------------|
|RegPar0End  - 32                                                           | 
|-------------------|-------------------|-------------------|------------------|
|RegPar1Start  - 32                                                           |
|-------------------|-------------------|-------------------|------------------|
|RegPar1End  - 32                                                          |
|-------------------|-------------------|-------------------|------------------|

*/

typedef enum BCMD_SetVichRegParField_InCmdField_e
{
	BCMD_SetVichRegParField_InCmdField_eVichId = (5<<2) + 3, /* 8 bit index */
	BCMD_SetVichRegParField_InCmdField_eRegPar0Start = (6<<2) + 0, /* 32 bit index */
	BCMD_SetVichRegParField_InCmdField_eRegPar0End = (7<<2) + 0, /* 32 bit index */
	BCMD_SetVichRegParField_InCmdField_eRegPar1Start = (8<<2) + 0, /* 32 bit index */
	BCMD_SetVichRegParField_InCmdField_eRegPar1End = (9<<2) + 0 /* 32 bit index */
} BCMD_SetVichRegParField_InCmdField_e;

/*
BCMD_cmdType_eOFFLINE_PCI_MEMWIN_COMMAND  -  The 
OFFLINE_PCI_MEMWIN COMMAND command is used as follows:
A new command OFFLINE_PCI_MEMWIN is defined to set up to 2 
system DRAM windows that does NOT allow external PCI master access. 
This command uses Aegis secure ARCH (Address Range Checker) to enforce 
the system DRAM access. The secure ARCH can allow read/write access 
to each of the RAM client. All the other memory client access for 
these windows are allowed except for PCI client(set in boot code). 
The default state is going to be no external PCI master access on DDR is allowed(boot). 
This command will be sent from trusted code to reconfigure the memory access map.  


Input Parameters passed:
1)  Arch Slot	
0x00 - Slot 0 used to control the accessibility to Region0
0x01 - Slot 1 used to control the accessibility to Region1

2) Lower Range Address: Address for lower range
3) Upper Range Address: Address for upper range



BCMD_MISC_SetPCI_MEMWIN_InCmdField_e
|-------------------|-------------------|-------------------|------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             | 
|-------------------|-------------------|-------------------|------------------| 
|  unused           | unused            | unused            |Slot - 8          | 
|-------------------|-------------------|-------------------|------------------|
| AddrRangeLow - 32                                                            | 
|-------------------|-------------------|-------------------|------------------|
| AddrRangeHigh - 32                                                           | 
|-------------------|-------------------|-------------------|------------------|

Output Parameters:
None


BCMD_MISC_SetPCI_MEMWIN Field_OutCmdField_e: 
|-------------------|-------------------|-------------------|------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             | 
|-------------------|-------------------|-------------------|------------------| 
|                                                           |Rave_Clr bits     | 
|-------------------|-------------------|-------------------|------------------| 

 */

typedef enum BCMD_MISC_SetPCI_MEMWIN_InCmdField_e
{
    BCMD_MISC_SetPCI_MEMWIN_InCmdField_eSlot = (5<<2) + 3, /* 8 bit index */
    BCMD_MISC_SetPCI_MEMWIN_InCmdField_eAddrRangeLow = (6<<2) + 0, /* 32 bit index */
    BCMD_MISC_SetPCI_MEMWIN_InCmdField_eAddrRangeHigh = (7<<2) + 0 /* 32 bit index */   

} BCMD_MISC_SetPCI_MEMWIN_InCmdField_e;

/*
BCMD_MISC_START_AVD_InCmdField_e
|-------------------|-------------------|-------------------|------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             | 
|-------------------|-------------------|-------------------|------------------| 
|  unused           | unused            | unused            |AVDId - 8          | 
|-------------------|-------------------|-------------------|------------------|
*/
typedef enum BCMD_MISC_START_AVD_InCmdField_e
{
	BCMD_MISC_START_AVD_InCmdField_eAVDId = (5<<2) + 3, /* 8 bit index */
	BCMD_MISC_START_AVD_InCmdField_eEnd	
} BCMD_MISC_START_AVD_InCmdField_e;




#endif /* BSP_S_MISC_H__ */

