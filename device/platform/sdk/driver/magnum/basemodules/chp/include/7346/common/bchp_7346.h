/***************************************************************************
 *     Copyright (c) 2006-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_7346.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 11/22/10 4:23p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7346/bchp_7346.h $
 * 
 * Hydra_Software_Devel/1   11/22/10 4:23p jrubio
 * SW7344-9: add initial version of 7346
 * 
 * Hydra_Software_Devel/1   11/15/10 4:57p jrubio
 * SW7346-9: add initial 7346 version
 * 
 *
 *
 ***************************************************************************/
#ifndef BCHP_7346_H__
#define BCHP_7346_H__

#include "bstd.h"
#include "bchp.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Chip Id */
#define BCHP_BCM7346                  0x7346

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
    Open a chip handle for Bcm7346 chip

Description:
    This function open a chip handle for Bcm7346 chip.

Input:
    hRegister - A valid Bcm7346 register handle previous open using BREG_Open.

Output:
    phChip - Upon successful completion of open this pointer is non NULL
    and contains valid information about this chip.  This handle is used
    on subsequences BCHP_??? API.  *phChip is NULL if failure.

See Also:
    BCHP_Close

**************************************************************************/
BERR_Code BCHP_Open7346
    ( BCHP_Handle                     *phChip,
      BREG_Handle                      hRegister );


#ifdef __cplusplus
}
#endif

#endif /* BCHP_7346_H__ */

/* End of File */
