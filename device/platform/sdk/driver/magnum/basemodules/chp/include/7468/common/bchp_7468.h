/***************************************************************************
 *     Copyright (c) 2006-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_7468.h $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 1/20/11 1:56p $
 *
 * Module Description:
 *   See Module Overview below.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7468/bchp_7468.h $
 * 
 * Hydra_Software_Devel/4   1/20/11 1:56p gmohile
 * SW7408-212 : Add support for avs settings
 * 
 * Hydra_Software_Devel/3   2/8/10 2:48p randyjew
 * SW7468-115: Add AVS support for 7468/7208
 * 
 * Hydra_Software_Devel/2   12/4/09 11:29a randyjew
 * SW7468-6: Update for 7208
 * 
 * Hydra_Software_Devel/1   9/28/09 3:53p lwhite
 * SW7468-6: Support for 7468
 * 
 * 
 * 
 ***************************************************************************/
#ifndef BCHP_7468_H__
#define BCHP_7468_H__

#include "bstd.h"
#include "bchp.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Chip Id */
#define BCHP_BCM7468                  0x7468
#define BCHP_BCM7208		      0x7208

/* Major revision */
#define BCHP_MAJOR_A                  (0)
#define BCHP_MAJOR_B                  (1)
#define BCHP_MAJOR_C                  (2)
#define BCHP_MAJOR_D                  (3)

/* Minor revision */
#define BCHP_MINOR_0                  (0)
#define BCHP_MINOR_1                  (1)
#define BCHP_MINOR_2                  (2)
#define BCHP_MINOR_3                  (3)
#define BCHP_MINOR_4                  (4)
#define BCHP_MINOR_5                  (5)

/***************************************************************************
Summary:
    Open a chip handle for Bcm7468 chip

Description:
    This function open a chip handle for Bcm7468 chip.

Input:
    hRegister - A valid Bcm7468 register handle previous open using BREG_Open.

Output:
    phChip - Upon successful completion of open this pointer is non NULL
    and contains valid information about this chip.  This handle is used
    on subsequences BCHP_??? API.  *phChip is NULL if failure.

See Also:
    BCHP_Close

**************************************************************************/
BERR_Code BCHP_Open7468
    ( BCHP_Handle                     *phChip,
      BREG_Handle                      hRegister );


#ifdef __cplusplus
}
#endif

#endif /* BCHP_7468_H__ */

/* End of File */
