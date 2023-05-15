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
 * $brcm_Revision: Hydra_Software_Devel/23 $
 * $brcm_Date: 10/9/09 1:54a $
 *
 * Module Description: 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7400/a0/common/bhsm_misc.h $
 * 
 * Hydra_Software_Devel/23   10/9/09 1:54a atruong
 * SW7405-3173: New HSM PI and Nexus API function for reading OTP IDs
 * 
 * Hydra_Software_Devel/22   4/14/09 10:55a atruong
 * PR54104: HSM PI Support for 35130A0
 * 
 * Hydra_Software_Devel/21   4/14/09 7:49a atruong
 * PR54104: HSM PI Support for 35130A0
 * 
 * Hydra_Software_Devel/20   1/30/09 4:56p atruong
 * PR51632: Support for 7336A0
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


#if (BCHP_CHIP==3563)
/**************************************************************************************************
Summary:
This function is to disable the forced-encryption-mode for RMX operation.

Description:
Just after the Aegis boot, RMX is enabled but, in forced encryption mode, so that clear data is not
allowed to go out on RMX right after system boot. Thus this function is needed by applications or
systems to disable "RMX encrytpion enforcement" at certain situation.

Calling Context:
It can be called anytime after system boot and HSM initialization are finished. 

Performance and Timing:
This is a synchronous/blocking function that will not return until it completes.

Input:
in_handle - BHSM_Handle, Host Secure module handle.

                        
Output:
none

Returns:
BERR_SUCCESS - success
BHSM_STATUS_FAILED - failure

See Also:
N/A
**************************************************************************************************/
BERR_Code   BHSM_DisableRmxForcedEncyptionMode (
                BHSM_Handle                             in_handle
);

#endif

#if (BCHP_CHIP != 3563) && (BCHP_CHIP != 3548) && (BCHP_CHIP != 3556) && (BCHP_CHIP != 35130)
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

/*#if defined (BCM7400A0) || defined(BCM7401B0)|| defined(BCM7401C0)||defined(BCM7118A0)*/
#if  (BCHP_CHIP== 7400 && BCHP_VER >= BCHP_VER_A0) || \
      (BCHP_CHIP== 7401 && BCHP_VER >= BCHP_VER_B0) ||  \
      (BCHP_CHIP== 7118 && BCHP_VER >= BCHP_VER_A0) ||  \
      (BCHP_CHIP== 7403 && BCHP_VER >= BCHP_VER_A0) ||  \
      (BCHP_CHIP== 7405 && BCHP_VER >= BCHP_VER_A0)  || \
      (BCHP_CHIP == 7325)||(BCHP_CHIP == 7335) || \
      (BCHP_CHIP == 7420) || (BCHP_CHIP == 7336)

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

#endif

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







