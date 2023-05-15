/***************************************************************************
 *     Copyright (c) 2005-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhsm_misc.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 8/12/08 5:01p $
 *
 * Module Description: 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7601/a0/common/bhsm_misc.h $
 * 
 * Hydra_Software_Devel/1   8/12/08 5:01p atruong
 * PR45631: HSM PI support for BCM7601 platform
 * 
 * Hydra_Software_Devel/11   12/4/06 9:40a lyang
 * PR 25188: HSM, add function header, data struct header description for
 * those public APIs
 * 
 * Hydra_Software_Devel/10   12/1/06 10:40p lyang
 * PR 25188: HSM, add function header, data struct header description for
 * those public APIs
 * 
 * Hydra_Software_Devel/9   10/27/06 11:36a lyang
 * PR 23412: remove HSM_PI_FUNC, my tool should be compatible with the
 * current API Reporter. PR 25133: remove HSM_PI_FUNC, my tool should be
 * compatible with the current API Reporter
 * 
 * Hydra_Software_Devel/8   10/26/06 4:19p lyang
 * PR 23412: hsm pi include files should be re-factored so they are not
 * order dependent. PR 25133:HSM header files should include dependencies
 * 
 * Hydra_Software_Devel/7   10/11/06 6:28p lyang
 * PR 24073: Update HSM PI code for 7401C0 (add DTVBits, ICAMBits)
 * 
 * Hydra_Software_Devel/6   10/4/06 4:58p lyang
 * PR 24073: HSM: Update HSM PI code for 7401C0 (add DTVBits, ICAMBits)
 * 
 * Hydra_Software_Devel/5   9/26/06 3:08p btan
 * PR 23906: Converted BCHP_REV to BCHP_VER.
 * 
 * Hydra_Software_Devel/4   9/20/06 11:42a btan
 * PR 24370: replaced BHSM_SetRaveIO with BHSM_SetMiscBitsIO -- Take 2.
 * 
 * Hydra_Software_Devel/3   9/19/06 3:02p btan
 * PR 24370: replaced BHSM_SetRaveIO with BHSM_SetMiscBitsIO
 * 
 * Hydra_Software_Devel/2   9/5/06 5:42p dbayani
 * PR 24089: Update HSM RAVE enable PI.
 * 
 * Hydra_Software_Devel/1   2/28/06 8:20p btan
 * PR 19931: Added HSM support for 7400A0
 * 
 * Hydra_Software_Devel/1   1/20/06 5:14p btan
 * PR 19140: Do not encrypt generic mode files.
 * 
 * Hydra_Software_Devel/3   1/17/06 9:32p btan
 * PR 19140: Do not encrypt generic mode files.
 * 
 *
 ***************************************************************************/

#ifndef BHSM_MISC_H__
#define BHSM_MISC_H__



/* added to remove header file dependency chain*/
#include "bhsm_priv.h"
#include "bsp_s_misc.h"

#include "bhsm.h"


#ifdef __cplusplus
extern "C" {
#endif


/* Module Specific Functions */

/**************************************************************************************************
Summary:

Description:
Structure that defines BHSM_ReadScArrayIO members

See Also:
BHSM_ReadScArray
**************************************************************************************************/
typedef struct BHSM_ReadScArrayIO {

    /* Out: 0 for success, otherwise failed. */
    unsigned int		unStatus;
    
    /* Out: This array represents the scrambling control bits (SCB) for each of the 256 
    possible pid channels (2 bits per pid channel).  
    SCB for Pid0 = bits[1:0] of unScArrayStatus[0].
    SCB for Pid15 = bits[31:30] of unScArrayStatus[0].
    SCB for Pid255 = bits[31:30] of unScArrayStatus[15]. 
    
    Translation of SCB:
        2'b11: scrambled with odd key
	    2'b10: scrambled with even key 
	    2'b00: no scrambling */
	uint32_t			unScArrayStatus[16];

} BHSM_ReadScArrayIO_t;

/**************************************************************************************************
Summary:
This function is to read out a snapshot copy of scrambling controlling bits for all PID channels.

Description:
This function reads out the current SCB values for all PID channels (256).  This function can
be called multiple times, each time returning the current SCB values for each PID channel.

Calling Context:
It can be called anytime during actual (de)scrambling is occurring for one or multiple data 
streams.

Performance and Timing:
This is a synchronous/blocking function that will not return until it completes.

Input:
in_handle - BHSM_Handle, Host Secure module handle.
inoutp_readScArrayIO - BHSM_ReadScArrayIO_t.
			
Output:
inoutp_readScArrayIO - BHSM_ReadScArrayIO_t.

Returns:
BERR_SUCCESS - success
BHSM_STATUS_FAILED - failure

See Also:
N/A
**************************************************************************************************/
BERR_Code   BHSM_ReadScArray (
		BHSM_Handle				in_handle,
		BHSM_ReadScArrayIO_t		*inoutp_readScArrayIO
);


/**************************************************************************************************
Summary:

Description:
Structure that defines BHSM_SetMiscBitsIO members

See Also:
BHSM_SetMiscBits
**************************************************************************************************/
typedef struct BHSM_SetMiscBitsIO {

	/* In: value must be '0'. */
	BCMD_SetMiscBitsSubCmd_e	setMiscBitsSubCmd;
	
	/* In: value must be '1'. */
	bool 						bEnableWriteIMem;
	
	/* In: value must be '1'. */
	bool 						bEnableReadIMem;
	
	/* In: value must be '1'. */
	bool 						bEnableReadDMem;
	
	/* In: value must be '0'. */
    bool                   		 		bEnableEncBypass;
#if (BCHP_CHIP!=7400) ||  (BCHP_VER != BCHP_VER_A0)		
	/* In: When set, this will remove the RAVE from reset.  RAVE is placed in reset
	whenever code is written to RAVE IMEM. */
    bool                    				bDisableClear;
#endif    

#if defined (BCM7400A0) || defined(BCM7401B0)|| defined(BCM7401C0)||defined(BCM7118A0)
	/* In: reserved */
	bool               bSetMiscReservedBit0;

	/* In: reserved */
	bool               bSetMiscReservedBit1;
	
	/* In: reserved */	
	bool               bSetMiscReservedBit2;
	
	/* In: reserved */
	bool               bSetMiscReservedBit3;
	
	/* In: reserved */	
	bool               bSetMiscReservedBit4;
	
	/* In: reserved */	
	bool               bSetMiscReservedBit5;
#endif

	/* Out: 0 for success, otherwise failed. */
	uint32_t				unStatus;

	/* Out: returns original value of RAVE bits before overwriting with new values. */	
	uint8_t 				ucRaveStatus;

} BHSM_SetMiscBitsIO_t;


/**************************************************************************************************
Summary:
This function is used to remove RAVE from reset.

Description:
This function is used to remove the RAVE from reset.  RAVE is placed in reset whenever the Host
CPU writes to RAVE IMEM.  When RAVE is removed from reset, it will begin executing code from
IMEM.  Video/Audio will not be descrambled until the RAVE is out of reset.

Calling Context:
This function is called after RAVE firmware has been loaded into IMEM and descrambling of 
video/audio is ready to begin.

Performance and Timing:
This is a synchronous/blocking function that will not return until it completes.

Input:
in_handle  - BHSM_Handle, Host Secure module handle.
inoutp_setMiscBitsIO  - BHSM_SetMiscBitsIO_t.
			
Output:
inoutp_setMiscBitsIO  - BHSM_SetMiscBitsIO_t.

Returns:
BERR_SUCCESS - success
BHSM_STATUS_FAILED - failure

See Also:
N/A
**************************************************************************************************/
BERR_Code   BHSM_SetMiscBits (
		BHSM_Handle			in_handle,
		BHSM_SetMiscBitsIO_t		*inoutp_setMiscBitsIO
);

/**************************************************************************************************
Summary:

Description:
Structure that defines BHSM_SetArchIO members

See Also:
BHSM_SetArch
**************************************************************************************************/
typedef struct BHSM_SetArchIO {

	/* In: defines the start address for the secure region. */
	uint32_t			unLowerRangeAddress;
	
	/* In: defines the end address (inclusive) for the secure region. */	
	uint32_t			unUpperRangeAddress;	

	/* Out: 0 for success, otherwise failed. */
	uint32_t			unStatus;
} BHSM_SetArchIO_t;

/**************************************************************************************************
Summary:
This function configures the address range of secure region. It is currently designed for future 
enhanced features.

Description:
The host DRAM is partitioned into Secure Region and Global Region. This function specifies 
the address range of the secure region.  Everything outside the specified region would be  
the global region.   

Calling Context:
It shall be called after the heap is allocated for secure region and specific buffer pointers are
initialized.

Performance and Timing:
This is a synchronous/blocking function that will not return until it completes.

Input:
in_handle  - BHSM_Handle, Host Secure module handle.
inoutp_setArchIO  -  BHSM_SetArchIO_t.
			
Output:
inoutp_setArchIO  - BHSM_SetArchIO_t.

Returns:
BERR_SUCCESS - success
BHSM_STATUS_FAILED - failure

See Also:
N/A
**************************************************************************************************/
BERR_Code   BHSM_SetArch (
		BHSM_Handle			in_handle,
		BHSM_SetArchIO_t		*inoutp_setArchIO
);

#if !defined(BCM7401A0)

/**************************************************************************************************
Summary:

Description:
Enum that defines instances of BCMD_SetVichRegPar_Vich.

See Also:
BHSM_SetVichRegPar
**************************************************************************************************/
typedef enum BCMD_SetVichRegPar_Vich{
			BCMD_SetVichRegPar_Vich0=0, /* Used to specify Video Instruction Checker 0. */
			BCMD_SetVichRegPar_Vich1, /* Used to specify Video Instruction Checker 1. */
			BCMD_SetVichRegPar_Max /* Maximum number of Video Instruction Checkers is 2. */
}BCMD_SetVichRegPar_Vich_e;

/**************************************************************************************************
Summary:

Description:
Structure that defines BHSM_SetVichRegParIO members

See Also:
BHSM_SetVichRegPar
**************************************************************************************************/
typedef struct BHSM_SetVichRegParIO {

	/* In: which checker to set. */
	BCMD_SetVichRegPar_Vich_e    whichVich;
	
	/* In: start address for video checker 0 read access */
	uint32_t			unRange0Start;
	
	/* In: end address (inclusive) for video checker 0 read access */
	uint32_t			unRange0End;
	
	/* In: start address for video checker 1 read access */ 
	uint32_t			unRange1Start;
	
	/* In: end address (inclusive) for video checker 1 read access */
	uint32_t			unRange1End;

	/* Out: 0 for success, otherwise failed */
	uint32_t			unStatus;
} BHSM_SetVichRegParIO_t;


/**************************************************************************************************
Summary:
This function is used to protect certain AVD registers.

Description:
This function is used to protect AVD registers.  Once the AVD registers are protected, the
Host can no longer write to those registers.  It is assumed that the Host initially configures
these AVD registers and calls this command with authenticated code.

Calling Context:
This function is called after the Host CPU configures the AVD registers to the desired values.

Performance and Timing:
This is a synchronous/blocking function that will not return until it completes.

Input:
in_handle  - BHSM_Handle, Host Secure module handle.
inoutp_setVichIO  - BHSM_SetVichRegParIO_t.
							
Output:
inoutp_setVichIO  - BHSM_SetVichRegParIO_t.				

Returns:
BERR_SUCCESS - success
BHSM_STATUS_FAILED - failure

See Also:
N/A
**************************************************************************************************/
BERR_Code  BHSM_SetVichRegPar (
		BHSM_Handle					in_handle,
		BHSM_SetVichRegParIO_t		*inoutp_setVichIO
);

/* Number of debug scratchpad registers available to the Host for reading. */
#define  BSP_EXPT_CAUSE_STICKY_WORD32_NUM    (4)

/**************************************************************************************************
Summary:

Description:
Structure that defines BHSM_DebugDumpExcIO members

See Also:
BHSM_DebugDumpExc
**************************************************************************************************/
typedef struct BHSM_DebugDumpExcIO {

	/* Out: 0 for success, otherwise failed. */
	unsigned int		unStatus;
	
	/* Out: Indicates reset state of processors within chip as well as flag to indicate if
	watch dog timer0 or watch dog timer1 caused the reset. */
	uint32_t			unResetStatusReg;
	
	/* Out: Indicates the status of all checkers to determine which, if any, caused the 
	reset. */
	uint32_t			unExceptionStatus;
	
	/* Out: Value of PC at time of reset */
	uint32_t			unProgramCounter;
	
	/* Out: Storage for scratchpad debug registers used to indicate violating address
	of checker interrupt. */
	uint32_t			unStickyWord[BSP_EXPT_CAUSE_STICKY_WORD32_NUM];	
} BHSM_DebugDumpExcIO_t;


/**************************************************************************************************
Summary:
This function is to dump out extra debugging information from BSP, for debugging purpose in case 
of BSP generated reset.

Description:
This function is used for debugging purposes when the BSP resets the chip due to invalid
application execution. 

Calling Context:
This function is called after the chip returns from reset.

Performance and Timing:
This is a synchronous/blocking function that will not return until it completes.

Input:
in_handle  - BHSM_Handle, Host Secure module handle.
inoutp_debugDumpExcIO  - BHSM_DebugDumpExcIO_t.
			
Output:
inoutp_debugDumpExcIO  - BHSM_DebugDumpExcIO_t.
			
			
Returns:
BERR_SUCCESS - success
BHSM_STATUS_FAILED - failure

See Also:
N/A
**************************************************************************************************/
BERR_Code  BHSM_DebugDumpExc (
		BHSM_Handle					in_handle,
		BHSM_DebugDumpExcIO_t		*inoutp_debugDumpExcIO
);

/**************************************************************************************************
Summary:

Description:
Enum that defines instances of BCMD_SetPciMemWin_Slot.

See Also:
BHSM_SetPciMemWin
**************************************************************************************************/
typedef enum BCMD_SetPciMemWin_Slot{
			BCMD_SetPciMemWin_Slot0=0, /* Used to specify PCI Memory Window 0. */
			BCMD_SetPciMemWin_Slot1, /* Used to specify PCI Memory Window 1. */
			BCMD_SetPciMemWin_Max /* Maximum number of PCI Memory Windows is 2. */
}BCMD_SetPciMemWin_e;

/**************************************************************************************************
Summary:

Description:
Structure that defines BCMD_SetPciMemWinIO members

See Also:
BHSM_SetPciMemWin
**************************************************************************************************/
typedef struct BCMD_SetPciMemWinIO {

	/* In: Specify which window to set. */
	unsigned char    	whichWin;
	
	/* In: Start address of window. */
	uint32_t			unAddrRangeLow;  
	
	/* In: End address (inclusive) of window. */          
	uint32_t			unAddrRangeHigh;

	/* Out: 0 for success, otherwise failed. */
	uint32_t			unStatus;
} BCMD_SetPciMemWinIO_t;


/**************************************************************************************************
Summary:
This function is used to define up to 2 system DRAM windows that does not allow external PCI 
master access.

Description:
This function is used to define up to 2 system DRAM windows not intended for external PCI 
master access.  All memory client read/write accesses for these windows are allowed except 
for PCI.

Calling Context:
This function can be during boot process.

Performance and Timing:
This is a synchronous/blocking function that will not return until it completes.

Input:
in_handle  - BHSM_Handle, Host Secure module handle.
inoutp_setPciMemWinIO  - BCMD_SetPciMemWinIO_t.
			
Output:
inoutp_setPciMemWinIO  - BCMD_SetPciMemWinIO_t.

Returns:
BERR_SUCCESS - success
BHSM_STATUS_FAILED - failure

See Also:
N/A
**************************************************************************************************/
BERR_Code  BHSM_SetPciMemWin (
		BHSM_Handle					in_handle,
		BCMD_SetPciMemWinIO_t		*inoutp_setPciMemWinIO
);



#endif


/* End of Module Specific Functions */


#ifdef __cplusplus
}
#endif

#endif /* BHSM_MISC_H__ */







