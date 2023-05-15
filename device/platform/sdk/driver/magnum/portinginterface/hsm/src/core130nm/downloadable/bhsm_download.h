/***************************************************************************
 *     Copyright (c) 2007-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhsm_download.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 9/18/07 1:30p $
 *
 * Module Description: 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7400/d0/downloadable/bhsm_download.h $
 * 
 * Hydra_Software_Devel/1   9/18/07 1:30p lyang
 * PR 34706: add support for 7400 D0 to portinginterface/hsm
 * 
 * 
 *
 ***************************************************************************/

#ifndef BHSM_DOWNLOAD_H__
#define BHSM_DOWNLOAD_H__


/* added to remove header file dependency */
#include "bhsm_priv.h"



#ifdef __cplusplus
extern "C" {
#endif




/* Module Specific Functions */





/*****************************************************************************
Summary:

This function allocates specified number of CA key slots for each key slot type.

Description:

There is a piece of hardware storage allocated for CA key slots.  The key slot table has 1530
(Refer to BCMD_XPTSECKEYTABLE_TOP) entries, where each entry has 64 bits. The 
total size could be varied from chip to chip.

There are 7 types of key slot being defined.  Each key slot in different key slot type consists 
of different number of entries as follow:
	Key slot type 0 always has 10 entries.
	Key slot type 1 always has 12 entries.
	Key slot type 2 always has 14 entries.
	Key slot type 3 always has 16 entries.
	Key slot type 4 always has 16 entries.
	Key slot type 5 always has 20 entries.
	Key slot type 6 always has 26 entries.

This function requests BSP to initialize the specified number of key slots for each key slot type.
the number of key slots can be set to zero if the slot type is not used. As long as the combination of 
all the key slots of for each key slot type does not exceed the maximum key slot table capacity, 
BSP will return succesfully. No CA key slot can be accessed until this function is called and 
returns successfully.

This function also keeps track of the allocation information, which can be used to check if a specific
key slot has been associated with certain pid channel.

Note that this function shall be called only once after system bootup. BSP shall return error if 
calling this function more than once. However, the key slot allocation in BSP, resulted from the first
successful function call, remains intact until the chip is reset.  For debugging purpose, the 
caller can call this function mutiple time, provided the same allocation is specified each time.
Calling this function multiple times with different allocation values shall have an undetermined 
consequence.


Calling Context:

After each board reset, this function shall be called only once before CA key slot can be utilized.
In some system that does not requried CA descrambling, this function can be omitted.


Performance and Timing:

This is a synchronous/blocking function that will not return until it is done or failed. 

Input:

in_handle  - BHSM_Handle, Host Secure module handle.
inoutp_initKeySlotIO  - BHSM_InitKeySlotIO_t

			
Output:

inoutp_initKeySlotIO -BHSM_InitKeySlotIO_t, 


Returns:

BERR_SUCCESS - success
BHSM_STATUS_FAILED - failure

See Also:
BHSM_AllocateCAKeySlot
BHSM_FreeCAKeySlot


*****************************************************************************/

/* End of Module Specific Functions */


#ifdef __cplusplus
}
#endif

#endif /* BHSM_KEYLADDER_H__ */







