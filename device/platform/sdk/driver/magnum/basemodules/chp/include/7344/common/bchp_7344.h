/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_7344.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 3/14/12 12:39p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7344/bchp_7344.h $
 * 
 * Hydra_Software_Devel/2   3/14/12 12:39p randyjew
 * SW7344-286: Add 7344/7418 B1 support
 * 
 * Hydra_Software_Devel/1   11/15/10 4:57p jrubio
 * SW7344-9: add initial 7344 version
 * 
 *
 *
 ***************************************************************************/
#ifndef BCHP_7344_H__
#define BCHP_7344_H__

#include "bstd.h"
#include "bchp.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Chip Id */
#define BCHP_BCM7344                  0x7344
#define BCHP_BCM7418                  0x7418


/* Major revision */
#define BCHP_MAJOR_A                  (0x0)
#define BCHP_MAJOR_B                  (0x1)
#define BCHP_MAJOR_C                  (0x2)
#define BCHP_MAJOR_D                  (0x3)

/* Minor revision */
#define BCHP_MINOR_0                  (0)
#define BCHP_MINOR_1                  (1)
#define BCHP_MINOR_2                  (2)
#define BCHP_MINOR_3                  (3)
#define BCHP_MINOR_4                  (4)
#define BCHP_MINOR_5                  (5)

/***************************************************************************
Summary:
    Open a chip handle for Bcm7344 chip

Description:
    This function open a chip handle for Bcm7344 chip.

Input:
    hRegister - A valid Bcm7344 register handle previous open using BREG_Open.

Output:
    phChip - Upon successful completion of open this pointer is non NULL
    and contains valid information about this chip.  This handle is used
    on subsequences BCHP_??? API.  *phChip is NULL if failure.

See Also:
    BCHP_Close

**************************************************************************/
BERR_Code BCHP_Open7344
    ( BCHP_Handle                     *phChip,
      BREG_Handle                      hRegister );


#ifdef __cplusplus
}
#endif

#endif /* BCHP_7344_H__ */

/* End of File */
