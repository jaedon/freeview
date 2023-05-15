/***************************************************************************
 *     Copyright (c) 2005-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsp_s_cancel.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 8/12/08 4:42p $
 * Security Processor Code for Register Access
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7601/a0/common/aegis/bsp_s_cancel.h $
 * 
 * Hydra_Software_Devel/1   8/12/08 4:42p atruong
 * PR45631: HSM PI support for BCM7601 platform
 * 
 * Aegis_Firmware_Devel/5   10/6/05 6:37p dbayani
 * PR201: Simplify cancel command so no input is required.
 * 
 * Aegis_Firmware_Devel/4   9/19/05 6:57p btan
 * PR 185: Updated the shared file for host compilation
 * 
 * Aegis_Firmware_Devel/3   8/22/05 8:22a dbayani
 * PR23: Added description of each host command.
 * 
 * Aegis_Firmware_Devel/2   6/14/05 5:19p dbayani
 * PR13830: Coding Conventions update,
 * 
 * Aegis_Firmware_Devel/1   6/10/05 11:06a dbayani
 * PR13830: Added shared enums and defines with MIPS to share directory.
 * 
 *
 ***************************************************************************/

 #ifndef BSP_S_CANCEL_H__
#define BSP_S_CANCEL_H__

/*

BCMD_Cancel_ExecuteCancelCommand - request to cancel an RSA, DSA, or DH
	operation.

Returned:
eStatus - Status of cancel command.

    
BCMD_Cancel_OutputCommandField_e: 
|-------------------|-------------------|-------------------|-------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             	| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused			|  unused			|  unused			| eStatus -	8		|
|-------------------|-------------------|-------------------|-------------------| 

 */

typedef enum BCMD_Cancel_OutputCommandField_e
{
    BCMD_Cancel_OutputCommandField_eStatus = (5<<2) + 3 /* 8 bit index */
}BCMD_Cancel_OutputCommandField_e;

#endif /* BSP_S_CANCEL_H__ */
