/***************************************************************************
 *     Copyright (c) 2003-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_7400.h $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 4/11/08 1:56p $
 *
 * Module Description:
 *   See Module Overview below.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7400/bchp_7400.h $
 * 
 * Hydra_Software_Devel/3   4/11/08 1:56p mward
 * PR39972: Support 7400E0.
 * 
 * Hydra_Software_Devel/2   1/16/06 11:27a jasonh
 * PR 19085: Removed unused defines.
 * 
 * Hydra_Software_Devel/1   1/13/06 12:31p jgarrett
 * PR 19007: Update base modules for 7400
 * 
 * Hydra_Software_Devel/Refsw_Devel_7400_A0/2   1/10/06 6:07p jgarrett
 * PR 19007:Updating BCHP for BCM7400
 * 
 * Hydra_Software_Devel/6   8/12/05 1:46p pblanco
 * PR16052: Added new CPU, DEBUG and AUX definitions.
 * 
 * Hydra_Software_Devel/5   8/5/05 12:40a ssavekar
 * PR 16052: New check-in for XVD
 * Added interrupt ID definition for "DataReadyISR". This is defined for
 * now as bit 2 in the interrupt status register.
 * 
 * Hydra_Software_Devel/4   7/26/05 10:14a pblanco
 * PR16052: Added DRAM control register definitions.
 * 
 * Hydra_Software_Devel/3   7/22/05 10:01a pblanco
 * PR16052: Added decoder register definitions.
 * 
 * Hydra_Software_Devel/2   7/21/05 3:01p pblanco
 * PR16052: Added IntgenClr definitions.
 * 
 * Hydra_Software_Devel/1   6/27/05 3:39p gmullen
 * PR15309: Added bchp_7401.?
 * 
 *
 ***************************************************************************/
#ifndef BCHP_7400_H__
#define BCHP_7400_H__

#include "bstd.h"
#include "bchp.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Chip Id */
#define BCHP_BCM7400                  0x7400

/* Major revision */
#define BCHP_MAJOR_A                  (0)
#define BCHP_MAJOR_B                  (1)
#define BCHP_MAJOR_C                  (2)
#define BCHP_MAJOR_D                  (3)
#define BCHP_MAJOR_E                  (4)

/* Minor revision */
#define BCHP_MINOR_0                  (0)
#define BCHP_MINOR_1                  (1)
#define BCHP_MINOR_2                  (2)
#define BCHP_MINOR_3                  (3)
#define BCHP_MINOR_4                  (4)
#define BCHP_MINOR_5                  (5)

/***************************************************************************
Summary:
	Open a chip handle for Bcm7400 chip

Description:
	This function open a chip handle for Bcm7400 chip.

Input:
	hRegister - A valid Bcm7400 register handle previous open using
	BREG_Open,

Output:
	phChip - Upon successful completion of open this pointer is non NULL
	and contains valid information about this chip.  This handle is used
	on subsequences BCHP_??? API.  *phChip is NULL if failure.

See Also:
	BCHP_Close

**************************************************************************/
BERR_Code BCHP_Open7400
	( BCHP_Handle                     *phChip,
	  BREG_Handle                      hRegister );


#ifdef __cplusplus
}
#endif

#endif /* BCHP_7400_H__ */

/* End of File */
