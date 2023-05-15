/***************************************************************************
 *     Copyright (c) 2006-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_7125.h $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 2/15/11 2:36p $
 *
 * Module Description:
 *   See Module Overview below.
 *
 * Revision History:
 *
  *
 * $brcm_Log: /magnum/basemodules/chp/7125/bchp_7125.h $
 * 
 * Hydra_Software_Devel/5   2/15/11 2:36p mward
 * SW7125-790:  Accept Rev. E0.
 * 
 * Hydra_Software_Devel/4   7/22/10 2:37p mward
 * SW7125-513: 7124 PROD_REVISION, will read 7116.  Accept 7124 also in
 * case that changes in the future.
 * 
 * Hydra_Software_Devel/3   7/16/10 12:13p mward
 * SW7125-513: Non-DOCSIS bond-out is now called 7124, not 7116.
 * 
 * Hydra_Software_Devel/2   11/23/09 6:35p mward
 * SW7125-97: Add 7125 B0, allow B0 code to run on A0.
 * 
 * Hydra_Software_Devel/1   7/13/09 6:54p mward
 * PR55545:  Add bchp_7125.c,.h.
 * 
 * Hydra_Software_Devel/1   5/29/09 6:23p jrubio
 * PR55232: add 7125 BCHP
 * 
*
 * 
 ***************************************************************************/
#ifndef BCHP_7125_H__
#define BCHP_7125_H__

#include "bstd.h"
#include "bchp.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Chip Id */
#define BCHP_BCM7125                 0x7125
#define BCHP_BCM7119                 0x7119
#define BCHP_BCM7019                 0x7019
#define BCHP_BCM7025                 0x7025
#define BCHP_BCM7116                 0x7116
#define BCHP_BCM7124                 0x7124

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
    Open a chip handle for Bcm7125 chip

Description:
    This function open a chip handle for Bcm7125 chip.

Input:
    hRegister - A valid Bcm7125 register handle previous open using BREG_Open.

Output:
    phChip - Upon successful completion of open this pointer is non NULL
    and contains valid information about this chip.  This handle is used
    on subsequences BCHP_??? API.  *phChip is NULL if failure.

See Also:
    BCHP_Close

**************************************************************************/
BERR_Code BCHP_Open7125
    ( BCHP_Handle                     *phChip,
      BREG_Handle                      hRegister );


#ifdef __cplusplus
}
#endif

#endif /* BCHP_7125_H__ */

/* End of File */
