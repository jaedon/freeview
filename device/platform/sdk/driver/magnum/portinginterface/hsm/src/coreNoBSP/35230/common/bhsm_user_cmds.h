/***************************************************************************
 *     Copyright (c) 2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhsm_user_cmds.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 7/21/10 3:04p $
 *
 * Module Description: 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/35230/a0/common/bhsm_user_cmds.h $
 * 
 * Hydra_Software_Devel/1   7/21/10 3:04p cdisc
 * SW35230-581: adding bhsm_user_cmds for RNG support
 * 
 *
 ***************************************************************************/

#ifndef BHSM_USER_CMDS_H__
#define BHSM_USER_CMDS_H__

#include "bhsm_priv.h"


#include "bhsm.h"
#include "bhsm_keyladder.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 09/28/05,Allen.C */
#define UINT32_SIZE_IN_BYTES 	(sizeof(uint32_t))

/* Module Specific Functions */

#define BHSM_MAX_RANDOM_NUM_LEN	360
/* 2048 bit field in BCMD_KeyRamBuf_eFirstRNRam1024 or BCMD_KeyRamBuf_eSecondRNRam1024*/
#define MAX_DATA_LENTH_USER_RANDOM_NUMBER_RNRam1024		(2048/8)		

typedef struct BHSM_UserRandomNumberIO {

	/* In: Selects the random number generation type:
			0x00 = raw random number from RNG, i.e. BSP_RNG_RNG_CORE_CTRL_RNG_TYPE_RNGRAW 
			0x01 = random number processed according to FIPS186-2 'x' generation,  Use BSP_RNG_RNG_CORE_CTRL_RNG_TYPE_FIPS186_X
			0x02 = random number processed according to FIPS186-2 'k' generation; Use BSP_RNG_RNG_CORE_CTRL_RNG_TYPE_FIPS186_K     
			0x03 = RNG-SHA1;  Use BSP_RNG_RNG_CORE_CTRL_RNG_TYPE_RNGSHA                
	*/	
	unsigned char				ucRandomNumType; 

	/* BCMD_KeyRamBuf_e		keyDest; (Not included because no BSP)*/  


     /* In: Length of the Random Number in bytes. It should be 32-bit word aligned. If 'Destination'  equals to 
             BCMD_KeyRamBuf_eFirstRNRam1024 or BCMD_KeyRamBuf_eSecondRNRam1024,  the maximum size shall be 256. 
             If 'Destination'  equals to 0x00, the maximum size shall be 360 bytes.*/
	unsigned int				unDataLen;

	/* Out: 0x00 - for success, 
 	 	    	0xff - a generic error
 	 	    	0x01 - a bad ramdom number generated  */
	uint32_t					unStatus;  

	/* Out: the returned/generated random number according to the inputs*/
	unsigned char				aucData[BHSM_MAX_RANDOM_NUM_LEN];

} BHSM_UserRandomNumberIO_t;

/*****************************************************************************
Summary:

This function is provided for MIPS host to use the random number generating (RNG) engine inside BSP to produce random numbers.
TBD.

Description:

TBD.

Calling Context:

This function can be called any time after the system and BSP is initialized, when the applications decides to utilize the 
RNG engine inside BSP.


Performance and Timing:
This is a synchronous/blocking function that won'tl return until it is done or failed.  


Input:
in_handle - BHSM_Handle, Host Secure module handle.
inoutp_userRandomNumberIO - BHSM_UserRandomNumberIO_t.


Output:
inoutp_userRandomNumberIO - BHSM_UserRandomNumberIO_t.


Returns:
BERR_SUCCESS - success
BHSM_STATUS_FAILED - failure

See Also:
*****************************************************************************/
BERR_Code     BHSM_UserRandomNumber (
		BHSM_Handle					in_handle,
		BHSM_UserRandomNumberIO_t	*inoutp_userRandomNumberIO
);




#ifdef __cplusplus
}
#endif

#endif /* BHSM_USER_CMDS_H__ */







