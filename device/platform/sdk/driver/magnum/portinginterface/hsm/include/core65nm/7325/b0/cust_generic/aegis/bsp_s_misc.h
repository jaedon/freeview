 /***************************************************************************
 *     Copyright (c) 2005-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsp_s_misc.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 8/21/08 7:38p $
 * Module Description: This file contains misc data structures that shall be shared with
 *					host. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7325/b0/cust_generic/aegis/bsp_s_misc.h $
 * 
 * Hydra_Software_Devel/2   8/21/08 7:38p dbayani
 * PR 45985: Remove errors and warnings.
 * 
 * Hydra_Software_Devel/1   8/12/08 9:33p atruong
 * PR44849: 7335 B0 Bringup/Cleanup
 * 
 * Aegis_Firmware_Devel/12   3/11/08 4:33p pilmore
 * PR1058: Support for CPS/Rave timestamp bit
 * 
 * Aegis_Firmware_Devel/11   1/23/08 4:19p pilmore
 * PR823: Allow host to force xpt SC bit for Generic mode ONLY
 * 
 * Aegis_Firmware_Devel/10   1/15/08 10:36a wychen
 * PR957, PR956, PR958, PR955, PR954, PR952, PR953: Fix memory wrap around
 * issue. Merge PCIWin command into SetArch command
 * 
 * Aegis_Firmware_Devel/9   12/13/07 6:50p wychen
 * PR940, PR941: remove GetScArrayStatus command
 * 
 * Aegis_Firmware_Devel/8   8/13/07 7:43p wychen
 * PR768, PR770, PR771, PR774: Support StartAVD to program up to 8
 * register in AVD.
 * 
 * Aegis_Firmware_Devel/7   8/12/07 3:21p wychen
 * PR768, PR770, PR771, PR774: change SetVich, StartAVD to sign command
 * 
 * Aegis_Firmware_Devel/6   8/6/07 11:21p pilmore
 * PR685: Add reserved[12] enums to BCMD_SetMiscBitsSubCmd_e
 * 
 * Aegis_Firmware_Devel/5   7/30/07 2:48p pilmore
 * PR714: Support HW changes in DRM byte swap
 * 
 * Aegis_Firmware_Devel/4   6/11/07 1:14p hzhu
 * PR660: pre BSECK code clean up - remove chips/customer modes defines
 * which are not applicable or unnecessary
 * 
 * Aegis_Firmware_Devel/3   6/1/07 6:53p ksekar
 * PR 656: Add 7325A0 compile flags
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
	BCMD_SetMiscBitsSubCmd_eM2MEndianSwapBits = 3,
	BCMD_SetMiscBitsSubCmd_eForceSCBit = 4,
	BCMD_SetMiscBitsSubCmd_eCPSRaveTimestampDis = 5,
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
	BCMD_SetMiscBitsField_InCmdField_eCtrlBits = (6<<2) + 3, /* 8 bit index */
	BCMD_SetMiscBitsField_InCmdField_eBandSel = (6<<2) + 3, /* 8 bit index */
	BCMD_SetMiscBitsField_InCmdField_eBandNum = (6<<2) + 2, /* 8 bit index */
	BCMD_SetMiscBitsField_InCmdField_eForceSCbits = (7<<2) + 3, /* 8 bit index */
	BCMD_SetMiscBitsField_InCmdField_eCPSRaveTimestampDis = (6<<2) + 3 /* 8 bit index */
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
    BCMD_MISC_SetArchField_InCmdField_eAddrRangeHigh = (6 << 2) + 0, /* 32 bit index */   
    BCMD_MISC_SetArchField_InCmdField_ePCIWin = (7 << 2) + 2, /* 8 bit index */   
    BCMD_MISC_SetArchField_InCmdField_eArchSel = (7 << 2) + 3 /* 8 bit index */   
} BCMD_MISC_SetArchField_InCmdField_e;

/*
BCMD_MISC_SetVichField_InCmdField_e
|-------------------|-------------------|-------------------|------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             | 
|-------------------|-------------------|-------------------|------------------| 
|Reserved  - 32                                                            | 
|-------------------|-------------------|-------------------|------------------|
|RegPar0Start - 32                                                            | 
|-------------------|-------------------|-------------------|------------------|
|RegPar0End  - 32                                                           | 
|-------------------|-------------------|-------------------|------------------|
|RegPar1Start  - 32                                                           |
|-------------------|-------------------|-------------------|------------------|
|RegPar1End  - 32                                                          |
|-------------------|-------------------|-------------------|------------------|
|Vich1 RegPar0Start - 32                                                            | 
|-------------------|-------------------|-------------------|------------------|
|Vich1 RegPar0End  - 32                                                           | 
|-------------------|-------------------|-------------------|------------------|
|Vich1 RegPar1Start  - 32                                                           |
|-------------------|-------------------|-------------------|------------------|
|Vich1 RegPar1End  - 32                                                          |
|-------------------|-------------------|-------------------|------------------|
|Signature  - 160                                                          |
|-------------------|-------------------|-------------------|------------------|

Output Parameters:
BCMD_SetVichBitsField_InCmdField_e: 
|-------------------|-------------------|-------------------|------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             | 
|-------------------|-------------------|-------------------|------------------| 
|  unused			|  unused			|  unused			| eStatus -	8		|
|-------------------|-------------------|-------------------|-------------------| 
|  unused			|  unused			|  unused			| eSigVerifyStatusBit -	8		|
|-------------------|-------------------|-------------------|-------------------| 

*/

typedef enum BCMD_SetVichRegParField_InCmdField_e
{
	BCMD_SetVichRegParField_InCmdField_eReserved = (5<<2) + 3, /* 8 bit index */
	BCMD_SetVichRegParField_InCmdField_eRegPar0Start = (6<<2) + 0, /* 32 bit index */
	BCMD_SetVichRegParField_InCmdField_eRegPar0End = (7<<2) + 0, /* 32 bit index */
	BCMD_SetVichRegParField_InCmdField_eRegPar1Start = (8<<2) + 0, /* 32 bit index */
	BCMD_SetVichRegParField_InCmdField_eRegPar1End = (9<<2) + 0, /* 32 bit index */
	BCMD_SetVichRegParField_InCmdField_eRegPar0Start_Vich1 = (10<<2) + 0, /* 32 bit index */
	BCMD_SetVichRegParField_InCmdField_eRegPar0End_Vich1 = (11<<2) + 0, /* 32 bit index */
	BCMD_SetVichRegParField_InCmdField_eRegPar1Start_Vich1 = (12<<2) + 0, /* 32 bit index */
	BCMD_SetVichRegParField_InCmdField_eRegPar1End_Vich1 = (13<<2) + 0, /* 32 bit index */
	BCMD_SetVichRegParField_InCmdField_eSig = (14<<2) + 0 /* 160 bit index */
} BCMD_SetVichRegParField_InCmdField_e;

typedef enum BCMD_SetVichRegParField_OutCmdField_e
{
	BCMD_SetVichRegParField_OutCmdField_eSigVerifyStatusBit = (6 <<2) + 3 /* 8 bit index */	
} BCMD_SetVichRegParField_OutCmdField_e;

/*
BCMD_MISC_START_AVD_InCmdField_e
|-------------------|-------------------|-------------------|------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             | 
|-------------------|-------------------|-------------------|------------------| 
|  unused           | unused            | unused            |AVDId - 8         | 
|-------------------|-------------------|-------------------|------------------|
|  Reserved - 32  (for future used KeySrc)                                     | 
|-------------------|-------------------|-------------------|------------------|
|  unused           | unused            | unused            |NumOfReg - 8      | 
|-------------------|-------------------|-------------------|------------------|
|  Address1 - 32                                                               | 
|-------------------|-------------------|-------------------|------------------|
|  Value1 - 32                                                                 | 
|-------------------|-------------------|-------------------|------------------|
|  Address2 - 32                                                               | 
|-------------------|-------------------|-------------------|------------------|
|  Value2 - 32                                                                 | 
|-------------------|-------------------|-------------------|------------------|
|  Address3 - 32                                                               | 
|-------------------|-------------------|-------------------|------------------|
|  Value3 - 32                                                                 | 
|-------------------|-------------------|-------------------|------------------|
|  Address4 - 32                                                               | 
|-------------------|-------------------|-------------------|------------------|
|  Value4 - 32                                                                 | 
|-------------------|-------------------|-------------------|------------------|
|  Address5 - 32                                                               | 
|-------------------|-------------------|-------------------|------------------|
|  Value5 - 32                                                                 | 
|-------------------|-------------------|-------------------|------------------|
|  Address6 - 32                                                               | 
|-------------------|-------------------|-------------------|------------------|
|  Value6 - 32                                                                 | 
|-------------------|-------------------|-------------------|------------------|
|  Address7 - 32                                                               | 
|-------------------|-------------------|-------------------|------------------|
|  Value7 - 32                                                                 | 
|-------------------|-------------------|-------------------|------------------|
|  Address8 - 32                                                               | 
|-------------------|-------------------|-------------------|------------------|
|  Value8 - 32                                                                 | 
|-------------------|-------------------|-------------------|------------------|
|  Signature -160      												           | 
|-------------------|-------------------|-------------------|------------------|
*/
typedef enum BCMD_MISC_START_AVD_InCmdField_e
{
	BCMD_MISC_START_AVD_InCmdField_eAVDId = (5<<2) + 3, /* 8 bit index */
	BCMD_MISC_START_AVD_InCmdField_eNumofReg = (7<<2) + 3, /* 8 bit index */
	BCMD_MISC_START_AVD_InCmdField_eAddr1 = (8<<2) + 0, /* 32 bit index */
	BCMD_MISC_START_AVD_InCmdField_eValue1 = (9<<2) + 0, /* 32 bit index */
	BCMD_MISC_START_AVD_InCmdField_eSig = (24<<2) + 0 /* 160 bit index */
} BCMD_MISC_START_AVD_InCmdField_e;

typedef enum BCMD_StartAVD_OutCmdField_e
{
	BCMD_StartAVD_OutCmdField_eSigVerifyStatusBit = (6 <<2) + 3, /* 8 bit index */	
    BCMD_StartAVD_OutCmdField_eReg   = (7<<2) + 0   /* 8 bit index */
}BCMD_StartAVD_OutCmdField_e;

#endif /* BSP_S_MISC_H__ */

