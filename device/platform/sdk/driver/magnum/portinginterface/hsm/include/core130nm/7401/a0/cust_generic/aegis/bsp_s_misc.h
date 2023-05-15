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
 * $brcm_Revision: Aegis_Firmware_Devel/8 $
 * $brcm_Date: 3/21/06 8:07p $
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
    BCMD_MISC_SetArchField_InCmdField_eSlot = (5<<2) + 3, /* 8 bit index */
    BCMD_MISC_SetArchField_InCmdField_eControlMode = (6<<2) + 3, /* 8 bit index */
    BCMD_MISC_SetArchField_InCmdField_eAddrRangeLow = (7<<2) + 0, /* 32 bit index */
    BCMD_MISC_SetArchField_InCmdField_eAddrRangeHigh = (8<<2) + 0, /* 32 bit index */   
    BCMD_MISC_SetArchField_InCmdField_eReadRightsLow = (9<<2) + 0, /* 32 bit index */
    BCMD_MISC_SetArchField_InCmdField_eReadRightsHigh = (10<<2) + 0, /* 32 bit index */
    BCMD_MISC_SetArchField_InCmdField_eWriteRightsLow = (11<<2) + 0, /* 32 bit index */
    BCMD_MISC_SetArchField_InCmdField_eWriteRightsHigh = (12<<2) + 0 /* 32 bit index */

} BCMD_MISC_SetArchField_InCmdField_e;




#endif /* BSP_S_MISC_H__ */

