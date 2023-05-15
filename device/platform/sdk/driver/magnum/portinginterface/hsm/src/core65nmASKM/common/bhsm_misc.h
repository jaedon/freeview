/***************************************************************************
 *     Copyright (c) 2005-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhsm_misc.h $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 12/9/09 4:30p $
 *
 * Module Description: 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7420/b0/common/bhsm_misc.h $
 * 
 * Hydra_Software_Devel/5   12/9/09 4:30p atruong
 * SW7468-16: HSM PI Support For 7468A0
 * 
 * Hydra_Software_Devel/4   8/20/09 7:19p atruong
 * PR55169: HSM PI Support For 7420B0/7340A0/7342A0
 * PR55544: HSM PI Support For 7125A0
 * 
 * Hydra_Software_Devel/3   8/11/09 3:48p atruong
 * PR55231: HSM PI Support For 7340A0
 * 
 * Hydra_Software_Devel/2   8/5/09 10:42a atruong
 * PR55231: HSM PI Support for BCM7342A0
 * 
 * Hydra_Software_Devel/1   8/5/09 7:32a atruong
 * PR55169: HSM PI Support for 7420B0
 * 
 * Hydra_Software_Devel/1   6/29/09 5:38p atruong
 * PR55169: HSM PI Support for 7420A1
 * 
 * Hydra_Software_Devel/19   12/5/08 8:33p atruong
 * PR50045: HSM PI Support for BCM7420 A0
 * 
 * Hydra_Software_Devel/18   11/13/08 2:45p atruong
 * PR48869: Support HSM PI Raw Commands for BSP/Aegis
 * 
 * Hydra_Software_Devel/17   4/11/08 10:16a atruong
 * PR38948: HSM PI support for BCM93548
 * 
 * Hydra_Software_Devel/16   4/9/08 11:45a atruong
 * PR38948: HSM PI support for BCM3548
 * PR39464: HSM PI support for BCM3556
 * 
 * Hydra_Software_Devel/15   3/5/08 3:09p ptimariu
 * PR40084: add archID to BHSM_SetArchIO
 *  
 * Hydra_Software_Devel/14   10/25/07 1:31p lyang
 * PR36043: Modify makefile and split the code to allow HSM to build
 * without source covered by Security SLA
 * 
 * Hydra_Software_Devel/13   10/25/07 12:34p lyang
 * PR36043: Modify makefile and split the code to allow HSM to build
 * without source covered by Security SLA
 * 
 * Hydra_Software_Devel/12   8/3/07 2:45p lyang
 * PR 33325: add 7118B0 support and link
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
	/* In: When set, this will remove the RAVE from reset.  RAVE is placed in reset
	whenever code is written to RAVE IMEM. */
    bool                    				bDisableClear;

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


typedef enum BCMD_PCIArchType_e
{
	BCMD_PCIArchType_eNonPCIWin,
	BCMD_PCIArchType_ePCIWin,
	BCMD_PCIArchType_ePCIeWin,

	BCMD_PCIArchType_eMax
}	BCMD_PCIArchType_e;

typedef enum BCMD_ArchSelect_e
{
	BCMD_ArchSelect_eSel00,
	BCMD_ArchSelect_eSel01,
	BCMD_ArchSelect_eSel02,
	BCMD_ArchSelect_eSel03,

	BCMD_ArchSelect_eMax

}	BCMD_ArchSelect_e;

typedef enum BCMD_DRAMSelect_e
{
	BCMD_DRAMSelect_eSel00,
	BCMD_DRAMSelect_eSel01,

	BCMD_DRAMSelect_eMax

}	BCMD_DRAMSelect_e;



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

	/* In: defines the arch Select */	
	BCMD_ArchSelect_e	ArchSel;

	/*In: defines the PCI Arch type */
	BCMD_PCIArchType_e	PCIArch;

	/*In: defines DRAM Select */
	BCMD_DRAMSelect_e	DRAMSel;	

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
		BHSM_SetArchIO_t	*inoutp_setArchIO
);




#define	BHSM_OTP_ID_LEN			8		/* OTP ID is 8 bytes long */

typedef enum BHSM_OtpIdRead_e
{
	BHSM_OtpIdRead_eOTPIdA = 3,
	BHSM_OtpIdRead_eOTPIdB = 4,
	BHSM_OtpIdRead_eOTPIdC = 5,
	BHSM_OtpIdRead_eMax

}	BHSM_OtpIdRead_e;

/**************************************************************************************************
Summary:

Description:
Structure that defines BHSM_ReadOTPIdIO members

See Also:
BHSM_ReadOTPId()
**************************************************************************************************/

typedef struct BHSM_ReadOTPIdIO {

	/* In:   Which OTP ID to read */
	BHSM_OtpIdRead_e		OtpId;
	/* Out: 0 for success, otherwise failed. */
	uint32_t				unStatus;
	/* Out: if usStatus is successful, this is the OTP ID, as requested */
	unsigned char			aucOTPId[BHSM_OTP_ID_LEN];
	/* Out: actual number of bytes returned for OTP ID */
	uint32_t				unIdSize;

}	BHSM_ReadOTPIdIO_t;


BERR_Code	BHSM_ReadOTPId(
		BHSM_Handle			in_handle,
		BHSM_ReadOTPIdIO_t	*inoutp_readOTPIdIO
);



/* End of Module Specific Functions */
#if BHSM_SECURE_PI_SUPPORT_MISC
#include   "bhsm_misc_enc.h"
#endif


#ifdef __cplusplus
}
#endif

#endif /* BHSM_MISC_H__ */







