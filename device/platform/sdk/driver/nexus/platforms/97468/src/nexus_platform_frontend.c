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
* $brcm_Workfile: nexus_platform_frontend.c $
* $brcm_Revision: 5 $
* $brcm_Date: 5/14/12 4:36p $
*
* API Description:
*   API name: Platform Frontend
*    Platform Frontend Setup
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/97468/src/nexus_platform_frontend.c $
* 
* 5   5/14/12 4:36p randyjew
* SW7468-6: Fix prototyping warning
* 
* 4   1/27/11 4:33p randyjew
* SW7125-769: Fix Warning
* 
* 3   1/27/11 12:31p mward
* SW7125-769:  Stub NEXUS_Platform_OpenFrontend().
* 
* 2   10/7/09 12:18p lwhite
* SW7468-6: Adjust frontend
*
*
***************************************************************************/

#include "nexus_types.h"
#include "priv/nexus_core.h"
#include "nexus_i2c.h"
#include "bfpga.h"
#include "bfpga_name.h"
#include "nexus_frontend.h"
#include "nexus_platform_features.h"
#include "nexus_platform.h"
#include "nexus_platform_priv.h"
#include "nexus_base.h"
#if NEXUS_FRONTEND_3510
#include "nexus_tuner_3420.h"
#include "nexus_frontend_3510.h"
#elif NEXUS_FRONTEND_3520
#include "nexus_tuner_dtt76800.h"
#include "nexus_frontend_3520.h"
#endif

BDBG_MODULE(nexus_platform_frontend);


#if NEXUS_NUM_FRONTEND_CARD_SLOTS
static NEXUS_FrontendCardHandle g_frontendCards[NEXUS_NUM_FRONTEND_CARD_SLOTS];
#endif


NEXUS_Error NEXUS_Platform_InitFrontend(void)
{
    return NEXUS_SUCCESS; 
}

void NEXUS_Platform_UninitFrontend(void)
{
    #if NEXUS_NUM_FRONTEND_CARD_SLOTS
    int i;
    for ( i = NEXUS_NUM_FRONTEND_CARD_SLOTS-1; i >= 0; i-- )
    {
        if ( g_frontendCards[i] )
        {
            NEXUS_FrontendCard_Close(g_frontendCards[i]);
        }
    }
    #endif
}





BTRC_MODULE(ChnChange_TuneStreamer, ENABLE);

NEXUS_Error
NEXUS_Platform_GetStreamerInputBand(unsigned index, NEXUS_InputBand *pInputBand)
{
    BDBG_ASSERT(pInputBand);
    if (index > 0) {
        BDBG_ERR(("Only 1 streamer input available"));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    BTRC_TRACE(ChnChange_TuneStreamer, START);
    *pInputBand = NEXUS_InputBand_e0; 
    BTRC_TRACE(ChnChange_TuneStreamer, STOP);
    return NEXUS_SUCCESS;
}

NEXUS_FrontendHandle NEXUS_Platform_OpenFrontend(
    unsigned id /* platform assigned ID for this frontend. See NEXUS_FrontendUserParameters.id.
                   See nexus_platform_frontend.c for ID assignment and/or see 
                   nexus_platform_features.h for possible platform-specific macros.
                */
    )
{
    NEXUS_Error errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
    BSTD_UNUSED(id);
    BSTD_UNUSED(errCode);
    return NULL;
}

