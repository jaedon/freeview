/***************************************************************************
 *     (c)2007-2009 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: bchp_7429.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 9/14/11 4:22p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7429/bchp_7429.h $
 * 
 * Hydra_Software_Devel/2   9/14/11 4:22p katrep
 * SW7429-1:updated for 7429
 * 
 * Hydra_Software_Devel/1   8/9/11 6:56p katrep
 * SW7429-1:initial support
 * 
 * Hydra_Software_Devel/1   11/3/10 5:41p katrep
 * SW7429-4: add initial 7429 support
 *
 *
 *
 **************************************************************************/

#ifndef BCHP_7429_H__
#define BCHP_7429_H__

#include "bstd.h"
#include "bchp.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Chip Id */
#define BCHP_BCM7429                  0x7429

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
    Open a chip handle for Bcm7429 chip

Description:
    This function open a chip handle for Bcm7429 chip.

Input:
    hRegister - A valid Bcm7429 register handle previous open using BREG_Open.

Output:
    phChip - Upon successful completion of open this pointer is non NULL
    and contains valid information about this chip.  This handle is used
    on subsequences BCHP_??? API.  *phChip is NULL if failure.

See Also:
    BCHP_Close

**************************************************************************/
BERR_Code BCHP_Open7429
    ( BCHP_Handle                     *phChip,
      BREG_Handle                      hRegister );


#ifdef __cplusplus
}
#endif

#endif /* BCHP_7429_H__ */

/* End of File */
