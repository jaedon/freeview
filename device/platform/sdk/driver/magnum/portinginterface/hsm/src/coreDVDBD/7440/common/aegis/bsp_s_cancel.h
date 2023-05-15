/***************************************************************************
 *     Copyright (c) 2005-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsp_s_cancel.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 3/1/06 10:38a $
 * Security Processor Code for Register Access
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/7400/hsm/A0/common/aegis/bsp_s_cancel.h $
 * 
 * Hydra_Software_Devel/1   3/1/06 10:38a btan
 * PR 19931: Added HSM support for 7400A0
 * 
 * Hydra_Software_Devel/1   1/20/06 5:10p btan
 * PR 19140: Do not encrypt generic mode files.
 * 
 * Hydra_Software_Devel/3   1/17/06 9:38p btan
 * PR 19140: Do not encrypt generic mode files.
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

