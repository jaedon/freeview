 /***************************************************************************
 *     Copyright (c) 2005-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsp_s_misc.h $
 * $brcm_Revision: Aegis_Firmware_Devel/3 $
 * $brcm_Date: 12/13/11 4:11p $
 * Module Description: This file contains misc data structures that shall be shared with
 *					host. 
 *
 ***************************************************************************/
#ifndef BSP_S_MISC_H__
#define BSP_S_MISC_H__

/* sub command ID for BCMD_SetMiscBits command*/
typedef enum BCMD_SetMiscBitsSubCmd_e
{
	BCMD_SetMiscBitsSubCmd_eRaveBits = 0,
	BCMD_SetMiscBitsSubCmd_eDTVBits = 1,
	BCMD_SetMiscBitsSubCmd_eReserved1 = 1,
	BCMD_SetMiscBitsSubCmd_eReserved2 = 2,
	BCMD_SetMiscBitsSubCmd_eM2MEndianSwapBits = 3,
	BCMD_SetMiscBitsSubCmd_eForceSCBit = 4,
    BCMD_SetMiscBitsSubCmd_eCPSRaveTimestampDis = 5,
    BCMD_SetMiscBitsSubCmd_eXPTM2MStatusDump = 6,
    BCMD_SetMiscBitsSubCmd_eXPTPipesBypass   = 7,
    BCMD_SetMiscBitsSubCmd_eXPTReset   = 8,
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
2) Control Bits for	BCMD_SetMiscBitsSubCmd_eDTVBits,
Bit 0: NDS_MSC_AUX_PACKET_BYPASS_EN: 
	Controls DTV AUX packet scrambling/descrambling using NDS P199 algorithm.
Bit	1 - DTV_MSC_AUX_PACKET_BYPASS_EN for XPT security:
	Controls DTV AUX packet scrambling/descrambling using DTV AES ECB MSC algorithm.
Bit 2 - DTV_MSC_AUX_PACKET_BYPASS_EN for Mem2mem
	Controls DTV AUX packet scrambling/descrambling using DTV AES ECB MSC algorithm.
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


typedef enum BCMD_XPT_BandSel_e 
{
   BCMD_XPT_BandSel_eInBand = 0,
   BCMD_XPT_BandSel_ePlayBack,
   BCMD_XPT_BandSel_eMax
}BCMD_XPT_BandSel_e;

typedef enum BCMD_MISC_RaveCtrlBitsMask_e {
	BCMD_MISC_RaveCtrlBitsMask_eIMEM_WR = 0x01,
	BCMD_MISC_RaveCtrlBitsMask_eIMEM_RD = 0x02,
	BCMD_MISC_RaveCtrlBitsMask_eDMEM_RD = 0x04,
	BCMD_MISC_RaveCtrlBitsMask_eRESERVED  = 0x08,
	BCMD_MISC_RaveCtrlBitsMask_eDISABLE_CLR = 0x10
}BCMD_MISC_RaveCtrlBitsMask_e;


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
	BCMD_SetMiscBitsField_OutCmdField_eRaveStatusBit = (6 <<2) + 3, /* 8 bit index */	
	BCMD_SetMiscBitsField_OutCmdField_eM2MStatusBits = (6 <<2),  /* 32 bit index */	
	BCMD_SetMiscBitsField_OutCmdField_eXPTStatusBits = (7 <<2),  /* 32 bit index */	
	BCMD_SetMiscBitsField_OutCmdField_eXPTCheckerViolationAddress = (8 <<2),  /* 32 bit index */	
	BCMD_SetMiscBitsField_OutCmdField_eXPTCheckerViolationIDs = (9 <<2)  /* 32 bit index */	
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
    BCMD_MISC_SetArchField_InCmdField_eDramSel = (7 << 2) + 1, /* 8 bit index */   
    BCMD_MISC_SetArchField_InCmdField_ePCIWin = (7 << 2) + 2, /* 8 bit index */   
    BCMD_MISC_SetArchField_InCmdField_eArchSel = (7 << 2) + 3 /* 8 bit index */   
} BCMD_MISC_SetArchField_InCmdField_e;

/*
BCMD_MISC_SetVichField_InCmdField_e
|-------------------|-------------------|-------------------|------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             | 
|-------------------|-------------------|-------------------|------------------| 
|RegPar0Start - 32                                                             | 
|-------------------|-------------------|-------------------|------------------|
|RegPar0End  - 32                                                              | 
|-------------------|-------------------|-------------------|------------------|
|RegPar1Start  - 32                                                            |
|-------------------|-------------------|-------------------|------------------|
|RegPar1End  - 32                                                              |
|-------------------|-------------------|-------------------|------------------|
|RegPar2Start - 32                                                             | 
|-------------------|-------------------|-------------------|------------------|
|RegPar2End  - 32                                                              | 
|-------------------|-------------------|-------------------|------------------|
|RegPar3Start  - 32                                                            |
|-------------------|-------------------|-------------------|------------------|
|RegPar3End  - 32                                                              |
|-------------------|-------------------|-------------------|------------------|
|Vich1 RegPar0Start - 32                                                       | 
|-------------------|-------------------|-------------------|------------------|
|Vich1 RegPar0End  - 32                                                        | 
|-------------------|-------------------|-------------------|------------------|
|Vich1 RegPar1Start  - 32                                                      |
|-------------------|-------------------|-------------------|------------------|
|Vich1 RegPar1End  - 32                                                        |
|-------------------|-------------------|-------------------|------------------|
|Vich1 RegPar2Start - 32                                                       | 
|-------------------|-------------------|-------------------|------------------|
|Vich1 RegPar2End  - 32                                                        | 
|-------------------|-------------------|-------------------|------------------|
|Vich1 RegPar3Start  - 32                                                      |
|-------------------|-------------------|-------------------|------------------|
|Vich1 RegPar3End  - 32                                                        |
|-------------------|-------------------|-------------------|------------------|
|Signature  - 160                                                              |
|-------------------|-------------------|-------------------|------------------|
|  unused           | unused            | KeyLayer - 8      | VirtKeyLad - 8   |
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
    BCMD_SetVichRegParField_InCmdField_eVDEC_Id    = (5 << 2) + 3,     /* VDEC-ID  */
    BCMD_SetVichRegParField_InCmdField_eNumRanges    = (6 << 2) + 3,     /* NumRegs  */

	BCMD_SetVichRegParField_InCmdField_eRegPar0Start = (7<<2) + 0, /* 32 bit index */
	BCMD_SetVichRegParField_InCmdField_eRegPar0End   = (8<<2) + 0, /* 32 bit index */
	/* ... */

	/* ... */
	BCMD_SetVichRegParField_InCmdField_eRegPar7Start = (22<<2) + 0, /* 32 bit index */
	BCMD_SetVichRegParField_InCmdField_eRegPar7End   = (23<<2) + 0 /* 32 bit index */
	/*
	    BCMD_SetVichRegParField_InCmdField_eSig          = (X<<2) + 0, 
	    BCMD_SetVichRegParField_InCmdField_eVKL        = (X << 2) + 3, 
	    BCMD_SetVichRegParField_InCmdField_eKeyLayer   = (X << 2) + 2, 
	    will be  at a variable length based on the number of Ranges
	*/ 

} BCMD_SetVichRegParField_InCmdField_e;

typedef enum BCMD_SetVichRegParField_OutCmdField_e
{
	BCMD_SetVichRegParField_OutCmdField_eNOT_USED    = (6 <<2) + 3 /* 8 bit index */	
} BCMD_SetVichRegParField_OutCmdField_e;


/*
BCMD_MISC_START_AVD_InCmdField_e
|-------------------|-------------------|-------------------|------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             | 
|-------------------|-------------------|-------------------|------------------| 
|  unused           | unused            | unused            |AVDId - 8         | 
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
|  unused           | unused            | KeyLayer - 8      | VirtKeyLad - 8   |
|-------------------|-------------------|-------------------|------------------|
*/
typedef enum BCMD_MISC_START_AVD_InCmdField_e
{
	BCMD_MISC_START_AVD_InCmdField_eAVDId       = (5<<2) + 3, /* 8 bit index */
	BCMD_MISC_START_AVD_InCmdField_eNumofReg    = (6<<2) + 3, /* 8 bit index */
	/* ... */

	/* ... */
	BCMD_MISC_START_AVD_InCmdField_eAddr1       = (7<<2) + 0, /* 32 bit index */
	BCMD_MISC_START_AVD_InCmdField_eValue1      = (8<<2) + 0 /* 32 bit index */
    /*
	BCMD_MISC_START_AVD_InCmdField_eSig = (X<<2) + 0, 
    BCMD_MISC_START_AVD_InCmdField_eVKL         = (X<<2) + 3, 
    BCMD_MISC_START_AVD_InCmdField_eKeyLayer    = (X<<2) + 2, 

        will be at a variable length based on the number of Ranges
	*/ 
} BCMD_MISC_START_AVD_InCmdField_e;

typedef enum BCMD_StartAVD_OutCmdField_e
{
	BCMD_StartAVD_OutCmdField_eNotUSED = (6 <<2) + 3, /* 8 bit index */	
    BCMD_StartAVD_OutCmdField_eNotUSED1   = (7<<2) + 0   /* 8 bit index */
}BCMD_StartAVD_OutCmdField_e;

/*
   PwrMgmtOp values:
   0 = Power down to Passive Standby mode
   1 = Power down to Active Standby mode with background check continuing
   2 = Power down to Active Standby mode with MIPS left active (returns ERROR)
   3 = Power up from Active Standby mode (#1)

   BkGntPeriod values:
   0 = XXX ms
   1 = YYY ms
   2 = ZZZ ms

BCMD_MISC_PowerMgmtField_InCmdField_e
|-------------------|-------------------|-------------------|------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             | 
|-------------------|-------------------|-------------------|------------------| 
|  unused           | unused            | unused            |PwrMgmtOp - 8     | 
|-------------------|-------------------|-------------------|------------------|
|  unused           | unused            | unused            |BkGndPeriod - 8   | 
|-------------------|-------------------|-------------------|------------------|

Output Parameters:
None
*/

typedef enum BCMD_MISC_PowerMgmtField_InCmdField_e
{
    BCMD_MISC_PowerMgmtField_InCmdField_ePwrMgmtOp = (5<<2) + 3,  /* 8 bit index */
#if defined(POWER_MGMT_BGDELAY)
    BCMD_MISC_PowerMgmtField_InCmdField_eBkGndPeriod = (6<<2) + 3,  /* 8 bit index */
#endif /* POWER_MGMT_BGDELAY */
	BCMD_MISC_PowerMgmtField_Reserved1               = (7<<2) + 3

} BCMD_MISC_PowerMgmtField_InCmdField_e;

#endif /* BSP_S_MISC_H__ */

