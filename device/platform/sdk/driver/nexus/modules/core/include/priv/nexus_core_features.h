/***************************************************************************
*     (c)2004-2012 Broadcom Corporation
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
* $brcm_Workfile: nexus_core_features.h $
* $brcm_Revision: 11 $
* $brcm_Date: 7/13/12 11:58a $
*
* API Description:
*   API name: Core Features
*       This file sets up chip-specific definitions for the core module
*
* Revision History:
*
* $brcm_Log: /nexus/modules/core/7400/include/priv/nexus_core_features.h $
* 
* 11   7/13/12 11:58a erickson
* SW7552-303: use OS-reported max_dcache_line_size instead of nexus macro
* 
* 10   3/14/12 11:19a erickson
* SW7401-4517: remove old code
* 
* 9   2/28/12 10:55a erickson
* SW7435-19: add 7435
* 
* 8   11/23/11 4:21p jgarrett
* SW7429-1: Adding 7429
* 
* 7   9/22/11 9:41a erickson
* SW7344-149: fix
* 
* 6   9/22/11 8:49a erickson
* SW7344-149: add 7344 B0 support
* 
* 5   9/7/11 11:23a erickson
* SW7346-470: add new 7346 B0 cache line size
* 
* 4   4/20/11 5:44p jhaberf
* SWDTV-6551: Added suppport for the 35233 DTV chip
* 
* 3   12/22/10 8:48a erickson
* SW7425-9: add 7425
* 
* 2   10/4/10 2:22p erickson
* SW7422-73: add 7422
* 
* 1   7/14/10 10:30a erickson
* SW7405-4621: all chips now share core/CHIP/include. make
*  nexus_core_features.h chip-generic.
*
* 2   6/30/08 5:38p erickson
* PR44165: move NEXUS_CORE_NUM_SECURE_HEAPS to nexus_platform_features.h
*
* 1   1/18/08 2:18p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/2   9/21/07 6:26p jgarrett
* PR 35002: Successful 97400 build
*
* Nexus_Devel/1   9/18/07 5:42p jgarrett
* PR 35002: Adding chip-specific features
*
***************************************************************************/
#ifndef NEXUS_CORE_FEATURES_H
#define NEXUS_CORE_FEATURES_H

/* Nested macros to build BCHP_OPEN and BINT_GETSETTINGS using BCHP_CHIP */
#define BCHP_OPEN_PROTOTYPE_P(CHIP) BCHP_Open##CHIP
#define BCHP_OPEN_PROTOTYPE(CHIP) BCHP_OPEN_PROTOTYPE_P(CHIP)
#define BINT_GETSETTINGS_PROTOTYPE_P(CHIP) BINT_##CHIP##_GetSettings
#define BINT_GETSETTINGS_PROTOTYPE(CHIP) BINT_GETSETTINGS_PROTOTYPE_P(CHIP)

/* Macros required by magnum */
#define BCHP_OPEN(pChip, reg) BCHP_OPEN_PROTOTYPE(BCHP_CHIP)((pChip), (reg))
#define BINT_GETSETTINGS() BINT_GETSETTINGS_PROTOTYPE(BCHP_CHIP)()

/* Use macros to create prototypes. This allows us to avoid needing to #include bchp_##BCHP_CHIP##.h and bint_##BCHP_CHIP##.h
with the c pre-processor. */
#include "bchp.h"
#include "bint_plat.h"
extern BERR_Code BCHP_OPEN_PROTOTYPE(BCHP_CHIP)(
    BCHP_Handle                     *phChip,
    BREG_Handle                      hRegister );
extern const BINT_Settings *BINT_GETSETTINGS_PROTOTYPE(BCHP_CHIP)( void );

#endif /* #ifndef NEXUS_CORE_FEATURES_H */

