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
* $brcm_Workfile: nexus_timebase_priv.h $
* $brcm_Revision: 5 $
* $brcm_Date: 6/19/12 12:43p $
*
* API Description:
*   Internal timebase API.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/transport/7400/include/priv/nexus_timebase_priv.h $
* 
* 5   6/19/12 12:43p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* 4   10/31/11 7:47p bandrews
* SW7231-391: merge to main
* 
* SW7420-2078/1   10/25/11 5:22p bandrews
* SW7231-391: update parser band and timebase implementations to use
*  handles everywhere, even for legacy enum usage
* 
* 3   11/19/08 9:47a erickson
* PR49212: merge ASTM overrides
*
* PR49212/1   11/18/08 7:45p bandrews
* PR49212: ASTM overrides user settings, but it should not destroy them
*  when it does so
*
* 2   3/25/08 5:25p bandrews
* PR40090: Added compile flag for ASTM
*
* 1   3/24/08 3:16p bandrews
* PR40090: API changes
***************************************************************************/
#ifndef NEXUS_TIMEBASE_PRIV_H__
#define NEXUS_TIMEBASE_PRIV_H__

#include "nexus_types.h"
#include "nexus_timebase.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NEXUS_Timebase * NEXUS_TimebaseHandle;

#if NEXUS_HAS_ASTM
typedef enum NEXUS_TimebaseClockCoupling
{
    NEXUS_TimebaseClockCoupling_eInputClock, /* PCR, VSYNC, HSYNC, etc. */
    NEXUS_TimebaseClockCoupling_eInternalClock, /* internal XTAL */
    NEXUS_TimebaseClockCoupling_eMax /* enum terminator */
} NEXUS_TimebaseClockCoupling;

typedef struct NEXUS_TimebaseAstmSettings
{
    bool enabled; /* is astm enabled */
    NEXUS_TimebaseClockCoupling clockCoupling; /* lock to input or xtal */
    NEXUS_Callback pcrReceived_isr; /* Notify Astm when the a PCR arrives. */
    void * callbackContext; /* user context passed in isr callbacks */
} NEXUS_TimebaseAstmSettings;

void NEXUS_Timebase_GetAstmSettings_priv(
    NEXUS_TimebaseHandle timebase,
    NEXUS_TimebaseAstmSettings * pAstmSettings  /* [out] */
    );

NEXUS_Error NEXUS_Timebase_SetAstmSettings_priv(
    NEXUS_TimebaseHandle timebase,
    const NEXUS_TimebaseAstmSettings * pAstmSettings
    );
#endif /* NEXUS_HAS_ASTM */

NEXUS_Error NEXUS_Timebase_GetStatus_priv_isr(
    NEXUS_TimebaseHandle timebase,
    NEXUS_TimebaseStatus * pStatus
    );

NEXUS_Error NEXUS_Timebase_GetIndex_priv(
    NEXUS_TimebaseHandle timebase,
    unsigned * pIndex
    );

NEXUS_TimebaseHandle NEXUS_Timebase_Resolve_priv(
    NEXUS_Timebase timebase
    );

NEXUS_OBJECT_CLASS_DECLARE(NEXUS_Timebase);

#ifdef __cplusplus
}
#endif

#endif

