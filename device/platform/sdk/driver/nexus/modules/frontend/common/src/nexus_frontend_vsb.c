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
* $brcm_Workfile: nexus_frontend_vsb.c $
* $brcm_Revision: 6 $
* $brcm_Date: 6/14/12 11:25a $
*
* API Description:
*   API name: Frontend QAM
*    Generic APIs for QAM tuning.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/frontend/common/src/nexus_frontend_vsb.c $
* 
* 6   6/14/12 11:25a jtna
* SW7231-828: merge
* 
* SW7231-828/1   6/5/12 5:06p vishk
* SW7231-828, SW7231-839: Add additional configuration to Nexus Frontend
*  module for 3517/20
* 
* 5   6/16/09 2:50p mward
* PR56038: Default autoAcquire = true.
* 
* 4   5/12/09 9:37a erickson
* PR54481: add NEXUS_FrontendVsbStatus.timeElapsed
*
* 3   1/23/09 3:32p jgarrett
* PR 51457: Adding frontend extension hooks for tuners and
*  frontends/demods
*
* 2   7/11/08 5:07p jtna
* PR42924: Add channel change measurements functionality
*
* 1   1/18/08 2:19p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/2   12/20/07 2:30p jgarrett
* PR 38019: Adding terrestrial flag (default=true)
*
* Nexus_Devel/1   10/5/07 5:52p jgarrett
* PR 35551: Adding initial version
*
***************************************************************************/

#include "nexus_frontend_module.h"

BDBG_MODULE(nexus_frontend_vsb);

BTRC_MODULE_DECLARE(ChnChange_Tune);

/***************************************************************************
Summary:
	Initialize a VSB settings structure to defaults
See Also:
    NEXUS_Frontend_TuneVsb
 ***************************************************************************/
void NEXUS_Frontend_GetDefaultVsbSettings(
    NEXUS_FrontendVsbSettings *pSettings
    )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->autoAcquire = true;
    pSettings->terrestrial = true;
    pSettings->ifFrequency = 44000000; /* 44 MHz*/
}

/***************************************************************************
Summary:
	Tune to a VSB channel
See Also:
    NEXUS_Frontend_InitVsbSettings
 ***************************************************************************/
NEXUS_Error NEXUS_Frontend_TuneVsb(
    NEXUS_FrontendHandle handle,
    const NEXUS_FrontendVsbSettings *pSettings
    )
{
    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(NULL != pSettings);
    BTRC_TRACE(ChnChange_Tune, START);

    NEXUS_Time_Get(&handle->resetStatusTime);

    if ( NULL == handle->tuneVsb )
    {
        if ( handle->pParentFrontend )
        {
            return NEXUS_Frontend_TuneVsb(handle->pParentFrontend, pSettings);
        }
        else
        {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else
    {
        return handle->tuneVsb(handle->pDeviceHandle, pSettings);
    }
}

/***************************************************************************
Summary:
	Get the status of a VSB tuner
 ***************************************************************************/
NEXUS_Error NEXUS_Frontend_GetVsbStatus( NEXUS_FrontendHandle handle, NEXUS_FrontendVsbStatus *pStatus )
{
    NEXUS_Error rc;

    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(NULL != pStatus);

    if ( NULL == handle->getVsbStatus )
    {
        if ( handle->pParentFrontend )
        {
            rc = NEXUS_Frontend_GetVsbStatus(handle->pParentFrontend, pStatus);
        }
        else
        {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else
    {
        rc = handle->getVsbStatus(handle->pDeviceHandle, pStatus);
    }
    if (!rc) {
        NEXUS_Time now;
        NEXUS_Time_Get(&now);
        pStatus->timeElapsed = NEXUS_Time_Diff(&now, &handle->resetStatusTime);
    }
    return rc;
}

