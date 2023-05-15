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
* $brcm_Workfile: nexus_frontend.c $
* $brcm_Revision: 72 $
* $brcm_Date: 11/1/12 4:46p $
*
* API Description:
*   API name: Frontend
*    Generic Frontend APIs.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/frontend/common/src/nexus_frontend.c $
*
* 72   11/1/12 4:46p jtna
* SW7346-928: fix broken nested for-loop in MTSIF daisy-chain config
*
* 71   10/23/12 4:24p khat
* SW7425-4037: NEXUS_Frontend_TransmitDebugPacket
*
* 70   10/19/12 5:32p erickson
* SW7425-3782: allow NEXUS_FrontendConnector to be acquired with Frontend
*
* 69   10/19/12 11:20a jtna
* SW7425-4037: add NEXUS_Frontend_TransmitDebugPacket
*
* 68   10/18/12 9:07p vishk
* SW3461-206: Add front end configuration settings for external, fixed-
*  gain amp (FGA) and optional FGA-bypass
*
* 67   10/8/12 5:34p jtna
* SW7425-3782: add a helpful hint about NEXUS_Frontend_GetConnector()
*
* 66   10/8/12 4:29p jtna
* SW7425-3782: use NEXUS_FrontendConnector for TSMF
*
* 65   10/8/12 2:52p erickson
* SW7435-372: add NEXUS_Frontend_ReadSoftDecisions
*
* 64   10/8/12 1:03p erickson
* SW7425-3782: add NEXUS_FrontendConnector for frontend -> transport
*  connections
*
* 63   10/3/12 5:00p jtna
* SW7425-3782: refactor nexus tsmf impl
*
* 62   9/17/12 3:05p erickson
* SW7435-344: remove manual NEXUS_OBJECT_REGISTER/UNREGISTER
*
* 61   9/6/12 12:05p jtna
* SW7425-3782: compiler warnings when not NEXUS_HAS_TSMF
*
* 60   9/5/12 6:09p jtna
* SW7425-3782: rework host and demod TSMF impl. both cases now go through
*  the host API
*
* 59   8/7/12 3:09p jtna
* SW7231-924: Add oob and ifd capabilities support for
*  NEXUS_Frontend_Acquire() and NEXUS_FrontendAcquireSettings
*
* 58   8/2/12 10:34a erickson
* SW7429-227: support NEXUS_NUM_PARSER_BANDS == 0
*
* 57   7/26/12 3:08p jtna
* SW7425-3514: add frontend GetTemperature APIs
*
* 56   7/25/12 10:18a gmohile
* SW7425-3519,SW7425-3520,SW7425-3077: Add private standby function
*
* 55   7/25/12 10:10a gmohile
* SW7425-3519,SW7425-3520,SW7425-3077: Add private standby function
*
* 54   7/23/12 5:24p jtna
* SW7425-3562: Coverity Defect ID:19232, 19153
*
* 53   7/23/12 11:35a vsilyaev
* SW7420-2328: Use per-thread storage to keep information about the
*  current client, remove per-module client information
*
* 52   7/18/12 6:23p jtna
* SW7346-928: add support for MTSIF frontend<->PB mapping in frontend
*  daisy-chain
*
* 51   6/27/12 5:17p mward
* SW7420-2085: Compiler warning for unused parameter.
*  nexus_unregister_NEXUS_FrontendHandle() can expand to nothing in some
*  configurations.  nexus_register_NEXUS_FrontendHandle() can return
*  error.  Check it, also prevents a compiler warning for
*  NEXUS_SERVER_SUPPORT=n.
*
* 50   6/27/12 3:06p jtna
* SW7425-3350: remove NEXUS_FRONTEND_FIXED_MTSIF_CONFIG compile option.
*  it's no longer needed
*
* 49   6/27/12 10:19a mward
* SW7425-3062:  Compiler warning for unused NEXUS_Frontend_P_MxtTimer.
*
* 48   6/26/12 3:34p jtna
* SW7425-3062: add feature to propagate host parserband settings to
*  demod. deprecate NEXUS_Frontend_Get/SetTransportSettings
*
* 47   6/22/12 11:50a erickson
* SW7420-2085: fix NEXUS_OBJECT close
*
* 46   6/21/12 2:35p erickson
* SW7420-2078: add NEXUS_Frontend_OpenStub
*
* 45   6/21/12 2:01p erickson
* SW7420-2078: refactor NEXUS_FrontendAcquireSettings using mode enum
*
* 44   6/20/12 4:22p erickson
* SW7420-2078: add NEXUS_Frontend_Acquire/Release
*
* 43   6/19/12 1:27p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
*
* 42   6/12/12 11:25a jtna
* SW7425-3171: fix use of wrong index
*
* 41   6/11/12 4:23p erickson
* SW3128-99: verify NEXUS_FrontendSpectrumSettings.data is CPU accessible
*  in the driver
*
* 40   6/11/12 3:57p jtna
* SW7425-3171: allow NEXUS_Frontend_ReapplyTransportSettings to
*  disconnect when NEXUS_ParserBandSettings.sourceTypeSettings.mtsif =
*  NULL
*
* 39   6/8/12 3:27p jtna
* SW7425-3171: add NEXUS_Frontend_ReapplyTransportSettings
*
* 38   5/15/12 5:43p jtna
* SW7425-2752: add more debug functionality
*
* 37   5/15/12 2:59p vishk
* SW7552-269, SW3128-155, SW3461-198: Add NEXUS_Frontend_WriteRegister()
*  and NEXUS_Frontend_ReadRegister().
*
* 36   5/13/12 5:05p jtna
* SW7425-2752: add NEXUS_FRONTEND_FIXED_MTSIF_CONFIG option to support
*  legacy mode
*
* 35   5/11/12 2:29p jtna
* SW7425-2752: add #include "bmxt.h"
*
* 34   5/10/12 6:52p jtna
* SW7425-2752: rework MTSIF-frontend <-> host interface
*
* 33   1/18/12 12:27p vishk
* SW3128-99: Add nexus support for spectrum data retreival.
*
* 3128_SPECTRUM_DATA/3   1/18/12 12:26p vishk
* SW3128-99: Add nexus support for spectrum data retreival.
*
* 3128_SPECTRUM_DATA/2   1/18/12 10:45a vishk
* SW3128-99: Add nexus support for spectrum data retreival.
*
* 3128_SPECTRUM_DATA/1   12/22/11 4:57p vishk
* SW3128-99:Add nexus support for spectrum data retreival.
*
* 32   11/17/11 4:43p vishk
* SW3461-96: Remove GetLockStatus and replace it with GetFastStatus
*
* 31   11/1/11 3:09p vishk
* SW3461-55: Means of returning BCM3461 chip revsion and firmware
*  revision via NEXUS i/f
*
* 30   10/6/11 5:21p jtna
* SW7425-1327: add NEXUS_Frontend_Get/SetTransportSettings()
*
* 29   10/6/11 9:53a jtna
* SW7425-1189: change NEXUS_Frontend_GetTsmfSettings() to return void
*
* 28   10/5/11 12:24p jtna
* SW7425-1189: add transport and frontend TSMF support
*
* 27   9/21/11 5:26p vishk
* SW3461-53: 3461 interrupt line 'stuck low' after multiple tune events.
*
* 26   1/6/11 7:42p shyi
* SW35230-2441: Moved out channel scan API to platform specific location
*
* 25   11/18/10 11:39a mward
* SW7125-737:  In NEXUS_Frontend_Close(), don't automatically untune if
*  frontends are defined for standby.  Explicit untune should be used for
*  increased flexibility.
*
* 24   9/2/10 11:45a shyi
* SW35230-1020: Merging to main branch
*
* SW35230-1020/1   8/26/10 12:01p shyi
* SW35230-1020: Added frequency scanning for DVB-T signal
*
* 23   8/20/09 4:48p erickson
* PR57770: convert FTM extension into more generic customer extension
*
* 22   7/21/09 10:00a erickson
* PR56958: fix NEXUS_Frontend_SetUserParameters, add comments to
*  NEXUS_Frontend_ScanFrequency
*
* 21   7/21/09 9:00a erickson
* PR56765: add ftm state and uninit
*
* 20   7/14/09 3:47p erickson
* PR56789: remove NEXUS_Frontend_SetPowerState. unimplemented and unused.
*
* 19   7/8/09 3:13p erickson
* PR56607: remove unused NEXUS_Frontend_P_Register function
*
* 18   7/8/09 6:53a erickson
* PR56607: add optional registerExtension operator so that callbacks can
*  be recreated with the handle that the user gets. this allows
*  NEXUS_StopCallbacks to work.
*
* 17   7/2/09 2:29p haisongw
* PR52808: No  Untune() for upstream
*
* 16   6/24/09 9:37a erickson
* PR53078: add support for frontend handle extensions in 73xx public API
*  implementations
*
* 15   5/15/09 12:01p erickson
* PR54843: untune on close
*
* 14   5/11/09 4:29p erickson
* PR53078: get time of tune or resetStatus so that elapsed time can be
*  returned with status
*
* 13   2/23/09 3:42p katrep
* PR52409: Fixed error messages due AST during nexus shutdown
*
* 12   2/11/09 3:38p jgarrett
* PR 51457: Renaming destroy extension routine
*
* 11   1/23/09 3:32p jgarrett
* PR 51457: Adding frontend extension hooks for tuners and
*  frontends/demods
*
* 10   12/11/08 4:07p erickson
* PR49556: separate frontend power management into analog and digital,
*  guarantee correct refcnt
*
* 9   10/8/08 1:12a erickson
* PR40799: provided untune takes precedence over dummy acquire. needed
*  for power down.
*
* 8   10/2/08 11:27a erickson
* PR40799: add optional untune function pointer
*
* 7   8/5/08 11:25a erickson
* PR45427: add NEXUS_Frontend_Untune
*
* 6   4/11/08 9:53a erickson
* PR41246: convert BDBG_OBJECT_UNSET to BDBG_OBJECT_DESTROY if freeing
*  memory
*
* 5   3/14/08 12:55p jgarrett
* PR 40108: Adding NEXUS_Frontend_ScanFrequency
*
* 4   3/14/08 11:31a jgarrett
* PR 40109: Adding NEXUS_Frontend_ResetStatus
*
* 3   3/12/08 10:25a erickson
* PR39584: added BDBG_OBJECT_ASSERT's
*
* 2   2/7/08 5:36p jgarrett
* PR 39407: Converting to common close functions
*
* 1   1/18/08 2:19p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/2   11/5/07 2:58p erickson
* PR36725: remove Get and GetCount
*
* Nexus_Devel/1   10/5/07 5:51p jgarrett
* PR 35551: Adding initial version
*
***************************************************************************/

#include "nexus_frontend_module.h"
#if defined(NEXUS_FRONTEND_45216)
#include "priv/nexus_frontend_mtsif_priv.h"
#endif
#include "nexus_class_verification.h"
#include "priv/nexus_core.h"
#if NEXUS_HAS_MXT
#include "bmxt.h"
#endif

BDBG_MODULE(nexus_frontend);

#define BDBG_MSG_TRACE(x) /*BDBG_MSG(x)*/

#define MAX_PB_PER_MTSIF_FRONTEND 4

/***************************************************************************
Summary:
    Return a list of capabilities for a given frontend object.
Description:
    This call returns a list of capabilities for a frontend object.  Because
    many frontends support multiple types of modulation schemes, this
    allows the application to distinguish the capabilities of one tuner
    versus another.  If a tuner has multiple capabilities, only one of
    the modes may be used at any one time.
See Also:
    NEXUS_Frontend_Get
 ***************************************************************************/
void NEXUS_Frontend_GetCapabilities(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendCapabilities *pCapabilities   /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);
    BDBG_ASSERT(NULL != pCapabilities);

    *pCapabilities = handle->capabilities;
}

NEXUS_Error NEXUS_Frontend_ReadSoftDecisions( NEXUS_FrontendHandle handle, NEXUS_FrontendSoftDecision *pDecisions, size_t length, size_t *pNumRead )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);
    BDBG_ASSERT(NULL != handle->pDeviceHandle);
    BDBG_ASSERT(NULL != pDecisions);
#if defined(HUMAX_PLATFORM_BASE) && (defined(NEXUS_FRONTEND_3461)||defined(NEXUS_FRONTEND_45216))
    if(NEXUS_Frontend_P_CheckDeviceOpen(handle)){
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }
#endif
    if ( NULL == handle->readSoftDecisions )
    {
        if ( handle->pParentFrontend )
        {
            return NEXUS_Frontend_ReadSoftDecisions(handle->pParentFrontend, pDecisions, length, pNumRead);
        }
        else if (handle->getSoftDecisions)
        {
            /* impl Read with Get */
            int rc;
            rc = NEXUS_Frontend_GetSoftDecisions(handle, pDecisions, length);
            if (rc) return BERR_TRACE(rc);
            *pNumRead = length;
            return 0;
        }
        else
        {
            return BERR_TRACE(NEXUS_NOT_SUPPORTED);
        }
    }
    else
    {
        return handle->readSoftDecisions(handle->pDeviceHandle, pDecisions, length, pNumRead);
    }
}

NEXUS_Error NEXUS_Frontend_GetSoftDecisions( NEXUS_FrontendHandle handle, NEXUS_FrontendSoftDecision *pDecisions, size_t length )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);
    BDBG_ASSERT(NULL != handle->pDeviceHandle);
    BDBG_ASSERT(NULL != pDecisions);
#if defined(HUMAX_PLATFORM_BASE) && (defined(NEXUS_FRONTEND_3461)||defined(NEXUS_FRONTEND_45216))
    if(NEXUS_Frontend_P_CheckDeviceOpen(handle)){
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }
#endif
    if ( NULL == handle->getSoftDecisions )
    {
        if ( handle->pParentFrontend )
        {
            return NEXUS_Frontend_GetSoftDecisions(handle->pParentFrontend, pDecisions, length);
        }
        else if (handle->readSoftDecisions)
        {
            /* impl Get with Read */
            while (length) {
                size_t numRead;
                int rc;
                rc = NEXUS_Frontend_ReadSoftDecisions(handle, pDecisions, length, &numRead);
                if (rc) return BERR_TRACE(rc);
                length -= numRead;
            }
            return 0;
        }
        else
        {
            return BERR_TRACE(NEXUS_NOT_SUPPORTED);
        }
    }
    else
    {
        return handle->getSoftDecisions(handle->pDeviceHandle, pDecisions, length);
    }
}

NEXUS_Error NEXUS_Frontend_GetUserParameters( NEXUS_FrontendHandle handle, NEXUS_FrontendUserParameters *pParams )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);
    BDBG_ASSERT(NULL != pParams);
    *pParams = handle->userParameters;
    return BERR_SUCCESS;
}

NEXUS_Error NEXUS_Frontend_SetUserParameters( NEXUS_FrontendHandle handle, const NEXUS_FrontendUserParameters *pParams )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);
    BDBG_ASSERT(NULL != pParams);
    handle->userParameters = *pParams;
    return BERR_SUCCESS;
}

void NEXUS_Frontend_P_Init(void)
{
}

static struct {
    BLST_SQ_HEAD(nexus_frontend_list, NEXUS_Frontend) frontends;
} g_frontend;

NEXUS_FrontendHandle NEXUS_Frontend_P_Create(void *pDeviceHandle)
{
    NEXUS_FrontendHandle handle;

    BDBG_ASSERT(NULL != pDeviceHandle);
    handle = BKNI_Malloc(sizeof(NEXUS_Frontend));
    if (!handle) {
        BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }

    NEXUS_OBJECT_INIT(NEXUS_Frontend, handle);
    handle->pDeviceHandle = pDeviceHandle;
    /* must be append for implicit index to work */
    BLST_SQ_INSERT_TAIL(&g_frontend.frontends, handle, link);

    handle->connector = NEXUS_FrontendConnector_Create();
    if (!handle->connector) {
        goto error;
    }

    return handle;

error:
    NEXUS_Frontend_Close(handle);
    return NULL;
}

void NEXUS_Frontend_P_Destroy(NEXUS_FrontendHandle handle)
{
    BLST_SQ_REMOVE(&g_frontend.frontends, handle, NEXUS_Frontend, link);
    if (handle->connector) {
        NEXUS_FrontendConnector_Destroy(handle->connector);
    }
    BDBG_OBJECT_DESTROY(handle, NEXUS_Frontend);
    BKNI_Free(handle);
    return;
}

void NEXUS_Frontend_GetDefaultAcquireSettings( NEXUS_FrontendAcquireSettings *pSettings )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

NEXUS_FrontendHandle NEXUS_Frontend_Acquire( const NEXUS_FrontendAcquireSettings *pSettings )
{
    NEXUS_FrontendHandle frontend;
    unsigned index;
    for (frontend = BLST_SQ_FIRST(&g_frontend.frontends), index = 0; frontend; frontend = BLST_SQ_NEXT(frontend, link), index++) {
        BDBG_OBJECT_ASSERT(frontend, NEXUS_Frontend);
        if (frontend->acquired) continue;
        switch (pSettings->mode) {
        case NEXUS_FrontendAcquireMode_eByCapabilities:
            if ((!pSettings->capabilities.qam || frontend->capabilities.qam) &&
                (!pSettings->capabilities.vsb || frontend->capabilities.vsb) &&
                (!pSettings->capabilities.ofdm || frontend->capabilities.ofdm) &&
                (!pSettings->capabilities.outOfBand || frontend->capabilities.outOfBand) &&
                (!pSettings->capabilities.ifd || frontend->capabilities.ifd) &&
                (!pSettings->capabilities.satellite || frontend->capabilities.satellite))
            {
                frontend->acquired = true;
                NEXUS_OBJECT_REGISTER(NEXUS_FrontendConnector, frontend->connector, Acquire);
                return frontend;
            }
            break;
        case NEXUS_FrontendAcquireMode_eByIndex:
            if (pSettings->index == index) {
                frontend->acquired = true;
                NEXUS_OBJECT_REGISTER(NEXUS_FrontendConnector, frontend->connector, Acquire);
                return frontend;
            }
            break;
        default:
            BERR_TRACE(NEXUS_INVALID_PARAMETER); return NULL;
        }
    }
    return NULL;
}

void NEXUS_Frontend_Release( NEXUS_FrontendHandle frontend )
{
    BDBG_OBJECT_ASSERT(frontend, NEXUS_Frontend);
    NEXUS_Frontend_Untune(frontend);
    frontend->acquired = false;
}

/***************************************************************************
Summary:
    Close a frontend handle
***************************************************************************/
static void NEXUS_Frontend_P_Finalizer(
    NEXUS_FrontendHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);
    BDBG_ASSERT(NULL != handle->close);
    if ( handle->numExtensions > 0 )
    {
        BDBG_ERR(("Can not close a frontend with open extensions."));
        BDBG_ASSERT(handle->numExtensions == 0);
    }

#if (0==(NEXUS_HOST_CONTROL_EXTERNAL_FRONTEND_STANDBY+NEXUS_HOST_CONTROL_INTERNAL_FRONTEND_STANDBY))
    /* These standby use cases need to separate untune and close. Otherwise... */
    /* untune before closing. may result in power savings. */
    NEXUS_Frontend_Untune(handle);
#endif

#ifdef NEXUS_FRONTEND_CUSTOMER_EXTENSION
    NEXUS_Frontend_P_UninitExtension(handle);
#endif

    handle->close(handle);
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_Frontend, NEXUS_Frontend_Close);

void NEXUS_Frontend_Untune( NEXUS_FrontendHandle handle )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);
#if defined(HUMAX_PLATFORM_BASE) && (defined(NEXUS_FRONTEND_3461)||defined(NEXUS_FRONTEND_45216))
    if(NEXUS_Frontend_P_CheckDeviceOpen(handle)){
        BDBG_ERR(("Device open failed. Cannot untune."));
        BERR_TRACE(NEXUS_NOT_INITIALIZED);
        return;
    }
#endif
    if ( handle->untune )
    {
		handle->untune(handle->pDeviceHandle);
    }
    else if ( handle->pParentFrontend && handle->pParentFrontend->untune )
    {
        NEXUS_Frontend_Untune(handle->pParentFrontend);
    }
    else
    {
        /* This is a simple way to reset the app callback. In the future, we may want
                a chip-specific way of reseting state. */
        if ( handle->capabilities.qam )
        {
            NEXUS_FrontendQamSettings settings;
            NEXUS_Frontend_GetDefaultQamSettings(&settings);
            (void)NEXUS_Frontend_TuneQam(handle, &settings);
        }
        else if ( handle->capabilities.vsb )
        {
            NEXUS_FrontendVsbSettings settings;
            NEXUS_Frontend_GetDefaultVsbSettings(&settings);
            (void)NEXUS_Frontend_TuneVsb(handle, &settings);
        }
        else if ( handle->capabilities.satellite )
        {
            NEXUS_FrontendSatelliteSettings settings;
            NEXUS_Frontend_GetDefaultSatelliteSettings(&settings);
            settings.frequency=950000000UL;
            (void)NEXUS_Frontend_TuneSatellite(handle, &settings);
        }
        else if ( handle->capabilities.ofdm )
        {
            NEXUS_FrontendOfdmSettings settings;
            NEXUS_Frontend_GetDefaultOfdmSettings(&settings);
            (void)NEXUS_Frontend_TuneOfdm(handle, &settings);
        }
        else if ( handle->capabilities.analog )
        {
            NEXUS_FrontendAnalogSettings settings;
            NEXUS_Frontend_GetDefaultAnalogSettings(&settings);
            (void)NEXUS_Frontend_TuneAnalog(handle, &settings);
        }
        else if ( handle->capabilities.outOfBand )
        {
            NEXUS_FrontendOutOfBandSettings settings;
            NEXUS_Frontend_GetDefaultOutOfBandSettings(&settings);
            (void)NEXUS_Frontend_TuneOutOfBand(handle, &settings);
        }
        else if ( handle->capabilities.upstream )
        {
            /* no untune for upstream*/
        }
        else
        {
            BDBG_ERR(("Unable to untune. No tuner capabilities found."));
        }
    }
}

/***************************************************************************
Summary:
    Reset status values for a frontend
***************************************************************************/
void NEXUS_Frontend_ResetStatus(
    NEXUS_FrontendHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);
#if defined(HUMAX_PLATFORM_BASE) && (defined(NEXUS_FRONTEND_3461)||defined(NEXUS_FRONTEND_45216))
    if(NEXUS_Frontend_P_CheckDeviceOpen(handle)){
        BDBG_ERR(("Device open failed. Cannot untune."));
        BERR_TRACE(NEXUS_NOT_INITIALIZED);
        return;
    }
#endif
    NEXUS_Time_Get(&handle->resetStatusTime);
    if ( handle->resetStatus )
    {
        handle->resetStatus(handle->pDeviceHandle);
    }
    else if ( handle->pParentFrontend )
    {
        NEXUS_Frontend_ResetStatus(handle->pParentFrontend);
    }
}

/***************************************************************************
Summary:
    Get the Synchronous lock status of a frontend.
 ***************************************************************************/
NEXUS_Error NEXUS_Frontend_GetFastStatus(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendFastStatus *pStatus
    )
{
    BDBG_ASSERT(NULL != handle);
#if defined(HUMAX_PLATFORM_BASE) && (defined(NEXUS_FRONTEND_3461)||defined(NEXUS_FRONTEND_45216))
    if(NEXUS_Frontend_P_CheckDeviceOpen(handle)){
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }
#endif
    if ( NULL == handle->getFastStatus )
    {
        if ( handle->pParentFrontend )
        {
            return NEXUS_Frontend_GetFastStatus(handle->pParentFrontend, pStatus);
        }
        else
        {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else
    {
        return handle->getFastStatus(handle->pDeviceHandle, pStatus);
    }
}

/***************************************************************************
Summary:
    Retrieve the chip family id, chip id, chip version and firmware version.
 ***************************************************************************/
void NEXUS_Frontend_GetType(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendType *pType    /* [out] */
    )
{
    BDBG_ASSERT(NULL != handle);
#if defined(HUMAX_PLATFORM_BASE) && (defined(NEXUS_FRONTEND_3461)||defined(NEXUS_FRONTEND_45216))
    if(NEXUS_Frontend_P_CheckDeviceOpen(handle)){
        BDBG_ERR(("Device open failed. Cannot get type."));
        BERR_TRACE(NEXUS_NOT_INITIALIZED);
        return;
    }
#endif
    if ( NULL == handle->getType )
    {
        if ( handle->pParentFrontend )
        {
            NEXUS_Frontend_GetType(handle->pParentFrontend, pType);
        }
        else
        {
            BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else
    {
        handle->getType(handle->pDeviceHandle, pType);
    }
}

void NEXUS_Frontend_GetDefaultSpectrumSettings(
    NEXUS_FrontendSpectrumSettings *pSettings          /* [out] */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}


NEXUS_Error NEXUS_Frontend_RequestSpectrumData(
    NEXUS_FrontendHandle handle,          /* Device channel handle */
    const NEXUS_FrontendSpectrumSettings *settings  /* spectrum settings */
    )
{
    BDBG_ASSERT(NULL != handle);

    if (!settings || !settings->data) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    if (!NEXUS_P_CpuAccessibleAddress(settings->data)) {
        /* this pointer must have driver-side mapping */
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
#if defined(HUMAX_PLATFORM_BASE) && (defined(NEXUS_FRONTEND_3461)||defined(NEXUS_FRONTEND_45216))
    if(NEXUS_Frontend_P_CheckDeviceOpen(handle)){
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }
#endif
    if ( NULL == handle->requestSpectrumData )
    {
        if ( handle->pParentFrontend )
        {
            return NEXUS_Frontend_RequestSpectrumData(handle->pParentFrontend, settings);
        }
        else
        {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else
    {
        return handle->requestSpectrumData(handle->pDeviceHandle, settings);
    }
}

void NEXUS_Frontend_P_SetPower(NEXUS_FrontendHandle handle, NEXUS_PowerManagementCore core, bool poweredUp)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);

    /* the nexus power management code requires strict reference counting. the frontend tune/untune api
    does not require strict balancing. this code provides that balancing. */
    if (core == NEXUS_PowerManagementCore_eDigitalFrontend) {
        if (handle->power.digital != poweredUp) {
            NEXUS_PowerManagement_SetCoreState(core, poweredUp);
            handle->power.digital = poweredUp;
        }
    }
    else if (core == NEXUS_PowerManagementCore_eAnalogFrontend) {
        if (handle->power.analog != poweredUp) {
            NEXUS_PowerManagement_SetCoreState(core, poweredUp);
            handle->power.analog = poweredUp;
        }
    }
}

void NEXUS_Frontend_P_GetDefaultExtensionSettings(
    NEXUS_FrontendExtensionSettings *pSettings          /* [out] */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

NEXUS_FrontendHandle NEXUS_Frontend_P_CreateExtension(
    const NEXUS_FrontendExtensionSettings *pSettings    /* Functions to override.  The close function is required, all other functions are optional.
                                                           If you do not want to extend a function, leave it in the default state and the parent's
                                                           handler will be called instead. */
    )
{
    NEXUS_Frontend *pFrontend;

    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(NULL != pSettings->close);
    BDBG_ASSERT(NULL != pSettings->parent);
    NEXUS_ASSERT_MODULE();

    pFrontend = NEXUS_Frontend_P_Create(pSettings->pDeviceHandle);
    if ( NULL == pFrontend )
    {
        (void)BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }

    if (pSettings->parent->registerExtension) {
        (*pSettings->parent->registerExtension)(pSettings->parent, pFrontend);
    }

    pFrontend->capabilities = pSettings->parent->capabilities;
    pFrontend->userParameters = pSettings->parent->userParameters;
    pFrontend->pParentFrontend = pSettings->parent;
    pFrontend->close = pSettings->close;
    pFrontend->tuneAnalog = pSettings->tuneAnalog;
    pFrontend->tuneOutOfBand = pSettings->tuneOutOfBand;
    pFrontend->tuneQam = pSettings->tuneQam;
    pFrontend->tuneUpstream = pSettings->tuneUpstream;
    pFrontend->tuneVsb = pSettings->tuneVsb;
    pFrontend->tuneSatellite = pSettings->tuneSatellite;
    pFrontend->getDiseqcSettings = pSettings->getDiseqcSettings;
    pFrontend->setDiseqcSettings = pSettings->setDiseqcSettings;
    pFrontend->sendDiseqcMessage = pSettings->sendDiseqcMessage;
    pFrontend->getDiseqcReply = pSettings->getDiseqcReply;
    pFrontend->sendDiseqcAcw = pSettings->sendDiseqcAcw;
    pFrontend->resetDiseqc = pSettings->resetDiseqc;
    pFrontend->tuneOfdm = pSettings->tuneOfdm;
    pFrontend->untune = pSettings->untune;

    pSettings->parent->numExtensions++;

    return pFrontend;
}

void NEXUS_Frontend_P_DestroyExtension(
    NEXUS_FrontendHandle handle
    )
{
    NEXUS_ASSERT_MODULE();
    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);
    BDBG_ASSERT(NULL != handle->pParentFrontend);
    BDBG_ASSERT(handle->pParentFrontend->numExtensions > 0);
    handle->pParentFrontend->numExtensions--;

    if (handle->pParentFrontend->registerExtension) {
        (*handle->pParentFrontend->registerExtension)(handle->pParentFrontend, NULL);
    }

    NEXUS_Frontend_P_Destroy(handle);
}

NEXUS_FrontendHandle NEXUS_Frontend_P_GetParentHandle(NEXUS_FrontendHandle handle)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);
    return handle->pParentFrontend;
}

#include "nexus_parser_band.h"


#if NEXUS_HAS_MXT
struct NEXUS_FrontendHostMtsifConfig g_NEXUS_Frontend_P_HostMtsifConfig;
#endif

#if NEXUS_HAS_MXT && NEXUS_HAS_TSMF
#include "priv/nexus_tsmf_priv.h"


static NEXUS_Error NEXUS_Frontend_P_SetTsmfInput(void *mxt, unsigned tsmfInput, unsigned tsmfIndex, const NEXUS_TsmfSettings *tsmfSettings)
{
    NEXUS_Error rc;
    BDBG_CASSERT(sizeof(NEXUS_TsmfFieldVerifyConfig) == sizeof(BMXT_TSMFFldVerifyConfig));
    BDBG_CASSERT(NEXUS_TsmfVersionChangeMode_eFrameChangeVer == (NEXUS_TsmfVersionChangeMode)BMXT_TSMFVersionChgMode_eFrameChgVer);
    BDBG_ASSERT(tsmfSettings->sourceType==NEXUS_TsmfSourceType_eMtsif);

    rc = BMXT_TSMF_SetFldVerifyConfig(mxt, tsmfIndex, (const BMXT_TSMFFldVerifyConfig *)&(tsmfSettings->fieldVerifyConfig));
    if (rc) {rc = BERR_TRACE(rc);} /* keep going */

    if (tsmfSettings->enabled) {
        BMXT_TSMFInputSel inputSelect = (BMXT_TSMFInputSel) tsmfInput;
        BDBG_MSG(("TSMF: IB%u -> TSMF%u", tsmfInput, tsmfIndex));

        if (tsmfSettings->semiAutomaticMode) {
            rc = BMXT_TSMF_EnableSemiAutoMode(mxt, inputSelect, tsmfIndex, tsmfSettings->slotMapLo, tsmfSettings->slotMapHi, tsmfSettings->relativeTsNum);
            if (rc) { return BERR_TRACE(rc); }
        }
        else {
            rc = BMXT_TSMF_EnableAutoMode(mxt, inputSelect, tsmfIndex, tsmfSettings->relativeTsNum);
            if (rc) { return BERR_TRACE(rc); }
        }
    }
    else {
        BDBG_MSG(("TSMF: TSMF%u disabled", tsmfIndex));
        rc = BMXT_TSMF_DisableTsmf(mxt, tsmfIndex);
        if (rc) { return BERR_TRACE(rc); }
    }
    return NEXUS_SUCCESS;
}

static NEXUS_Error NEXUS_Frontend_P_SetTsmfOutput(void *mxt, unsigned tsmfOutput, unsigned tsmfIndex, bool enabled)
{
    NEXUS_Error rc;
    BDBG_MSG(("TSMF: TSMF%u -> PB%u (enable %u)", tsmfIndex, tsmfOutput, enabled));
    rc = BMXT_TSMF_SetParserConfig(mxt, tsmfOutput, tsmfIndex, enabled);
    if (rc) { return BERR_TRACE(rc); }
    return NEXUS_SUCCESS;
}

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DEMOD_TSMF)
static NEXUS_Error NEXUS_Frontend_P_EnableParserOutput(void *mxt, unsigned parserIndex)
{
	BMXT_ParserConfig parserConfig;

	BMXT_GetParserConfig(mxt, parserIndex, &parserConfig);
	BDBG_ERR(("DemodPB Output: [Prev] PB%u Enable(%d)\n", parserIndex, parserConfig.Enable));
	parserConfig.Enable = true;
	BDBG_ERR(("DemodPB Output: [Next] PB%u Enable(%d)\n", parserIndex, parserConfig.Enable));
	BMXT_SetParserConfig(mxt, parserIndex, &parserConfig);

    return NEXUS_SUCCESS;
}
#endif
#endif

NEXUS_Error NEXUS_Frontend_P_SetTsmfConfig(NEXUS_FrontendHandle frontend, NEXUS_FrontendDeviceMtsifConfig *pConfig)
{
#if NEXUS_HAS_MXT && NEXUS_HAS_TSMF
    NEXUS_FrontendUserParameters userParams;
    BMXT_Handle mxt = pConfig->mxt;
    unsigned iTsmf, numTsmf, tsmfInput, tsmfIndex;
    NEXUS_Error rc = NEXUS_SUCCESS;

    NEXUS_Frontend_GetUserParameters(frontend, &userParams);
    tsmfInput = userParams.param1; /* input is the demod IB */

    BDBG_OBJECT_ASSERT(frontend, NEXUS_Frontend);

    NEXUS_Module_Lock(g_NEXUS_frontendModuleSettings.transport);
    numTsmf = NEXUS_Tsmf_GetNumHandles_priv();
    NEXUS_Module_Unlock(g_NEXUS_frontendModuleSettings.transport);

    for (iTsmf=0; iTsmf<numTsmf; iTsmf++) {
        NEXUS_TsmfHandle tsmf;
        NEXUS_TsmfSettings tsmfSettings;
        bool set = false;

        NEXUS_Module_Lock(g_NEXUS_frontendModuleSettings.transport);
        tsmf = NEXUS_Tsmf_GetHandle_priv(iTsmf, &tsmfIndex);
        BDBG_ASSERT(tsmf);

        if (NEXUS_Tsmf_TestFrontendInput_priv(tsmf, frontend->connector) && NEXUS_Tsmf_GetPending_priv(tsmf)) {
            set = true;
            NEXUS_Tsmf_SetPending_priv(tsmf, false);
        }
        NEXUS_Module_Unlock(g_NEXUS_frontendModuleSettings.transport);

        if (set) {
            /* set input: connect this frontend to all TSMF whose input matches this frontend */
            unsigned i;
            NEXUS_Tsmf_GetSettings(tsmf, &tsmfSettings);
            NEXUS_Frontend_P_SetTsmfInput(mxt, tsmfInput, tsmfIndex, &tsmfSettings);

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DEMOD_TSMF)
			/* skip tsmf monitoring parserband */
            /* for (i=0; i<NEXUS_MAX_PARSER_BANDS-1; i++) { */
            for (i=0; i<NEXUS_NUM_PARSER_BANDS-1; i++) {
                /* set output: connect this TSMF to all demod PB that are connected to host PBs whose input is this TSMF (yes, it's complicated...) */
                NEXUS_ParserBandSettings parserBandSettings;
                if (g_NEXUS_Frontend_P_HostMtsifConfig.hostPbSettings[i].connected) {
                    NEXUS_ParserBand_GetSettings(i, &parserBandSettings);
                    if (parserBandSettings.sourceType==NEXUS_ParserBandSourceType_eTsmf && parserBandSettings.sourceTypeSettings.tsmf==tsmf) {
                        NEXUS_Frontend_P_SetTsmfOutput(mxt, g_NEXUS_Frontend_P_HostMtsifConfig.hostPbSettings[i].demodPb, tsmfIndex, tsmfSettings.enabled);

						if((tsmfSettings.enabled==true) ||(tsmfSettings.parserforcesetting==true && tsmfSettings.relativeTsNum==0xFF))
						{
							if(tsmfSettings.enabled==true)
							{
								 BDBG_ERR(("TSMF connection: IB%u -> TSMF%u -> DemodPB%u -> TSNUM(%u)\n", tsmfInput, tsmfIndex, g_NEXUS_Frontend_P_HostMtsifConfig.hostPbSettings[i].demodPb, tsmfSettings.relativeTsNum));
							}
							/* Enable demodPb TS output */
                        	NEXUS_Frontend_P_EnableParserOutput(mxt, g_NEXUS_Frontend_P_HostMtsifConfig.hostPbSettings[i].demodPb);
						}
	                    }
                    }
                }
#else
            for (i=0; i<NEXUS_MAX_PARSER_BANDS; i++) {
                /* set output: connect this TSMF to all demod PB that are connected to host PBs whose input is this TSMF (yes, it's complicated...) */
                NEXUS_ParserBandSettings parserBandSettings;
                if (g_NEXUS_Frontend_P_HostMtsifConfig.hostPbSettings[i].connected) {
                    NEXUS_ParserBand_GetSettings(i, &parserBandSettings);
                    if (parserBandSettings.sourceType==NEXUS_ParserBandSourceType_eTsmf && parserBandSettings.sourceTypeSettings.tsmf==tsmf) {
                        NEXUS_Frontend_P_SetTsmfOutput(mxt, g_NEXUS_Frontend_P_HostMtsifConfig.hostPbSettings[i].demodPb, tsmfIndex, tsmfSettings.enabled);
                    }
                }
            }
#endif

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DEMOD_TSMF)
			if(tsmfSettings.enabled)
			{
				BDBG_ERR(("TSMF Connection!\n"));
				frontend->bEnabledTSMF = true;
				frontend->bConnectedNormal = false;
				frontend->nTSMFIndex = tsmfIndex;
            }
			else if(tsmfSettings.enabled==false && tsmfSettings.parserforcesetting==true && tsmfSettings.relativeTsNum==0xFF)
			{
				BDBG_ERR(("Normal Connection!\n"));
				frontend->bEnabledTSMF = false;
				frontend->bConnectedNormal = true;
				frontend->nTSMFIndex = 0;
			}
			else
			{
				frontend->bEnabledTSMF = false;
				frontend->bConnectedNormal = false;
				frontend->nTSMFIndex = 0;
			}
#endif
        }
    }

    return rc;

#else /* NEXUS_HAS_MXT && NEXUS_HAS_TSMF */
    BSTD_UNUSED(frontend);
    BSTD_UNUSED(pConfig);
    return NEXUS_SUCCESS;
#endif
}


NEXUS_Error NEXUS_Frontend_ReapplyTransportSettings(NEXUS_FrontendHandle handle)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);
#if defined(HUMAX_PLATFORM_BASE) && (defined(NEXUS_FRONTEND_3461)||defined(NEXUS_FRONTEND_45216))
    if(NEXUS_Frontend_P_CheckDeviceOpen(handle)){
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }
#endif
    /* call the device-specific function because the cached MTSIF-config struct is per-device */
    if (NULL == handle->reapplyTransportSettings) {
        if (handle->pParentFrontend) {
            return NEXUS_Frontend_ReapplyTransportSettings(handle->pParentFrontend);
        }
        else {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else {
        return handle->reapplyTransportSettings(handle->pDeviceHandle);
    }
}

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DEMOD_TSMF)
NEXUS_Error NEXUS_Frontend_GetPathinfo(NEXUS_FrontendHandle handle)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);

    /* call the device-specific function because the cached MTSIF-config struct is per-device */
    if (NULL == handle->getpathinfo) {
        if (handle->pParentFrontend) {
            return NEXUS_Frontend_GetPathinfo(handle->pParentFrontend);
        }
        else {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else {
        return handle->getpathinfo(handle->pDeviceHandle);
    }
}
#else
NEXUS_Error NEXUS_Frontend_GetPathinfo(NEXUS_FrontendHandle handle)
{
	BSTD_UNUSED(handle);
	return NEXUS_SUCCESS;
}
#endif

#if defined(HUMAX_PLATFORM_BASE)
#if defined(CONFIG_DEMOD_TSMF)
NEXUS_Error NEXUS_Frontend_ReapplyTSMFSettings(NEXUS_FrontendHandle handle)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);

    if (NULL == handle->reapplyTSMFSettings) {
        if (handle->pParentFrontend) {
            return NEXUS_Frontend_ReapplyTSMFSettings(handle->pParentFrontend);
        }
        else {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else {
        return handle->reapplyTSMFSettings(handle->pDeviceHandle);
    }
}
#else
NEXUS_Error NEXUS_Frontend_ReapplyTSMFSettings(NEXUS_FrontendHandle handle)
{
	BSTD_UNUSED(handle);
	return NEXUS_SUCCESS;
}
#endif
#endif

void NEXUS_ParserBand_P_TEIError_priv_isr(NEXUS_ParserBand parserBand);
void NEXUS_ParserBand_P_LengthError_priv_isr(NEXUS_ParserBand parserBand);




NEXUS_Error NEXUS_Frontend_WriteRegister(NEXUS_FrontendHandle handle, unsigned address, uint32_t value )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);
#if defined(HUMAX_PLATFORM_BASE) && (defined(NEXUS_FRONTEND_3461)||defined(NEXUS_FRONTEND_45216))
    if(NEXUS_Frontend_P_CheckDeviceOpen(handle)){
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }
#endif
    if (NULL == handle->writeRegister) {
        if (handle->pParentFrontend) {
            return NEXUS_Frontend_WriteRegister(handle->pParentFrontend, address, value);
        }
        else {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else {
        return handle->writeRegister(handle->pDeviceHandle, address, value);
    }
}

NEXUS_Error NEXUS_Frontend_ReadRegister(NEXUS_FrontendHandle handle, unsigned address,  uint32_t *value )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);
#if defined(HUMAX_PLATFORM_BASE) && (defined(NEXUS_FRONTEND_3461)||defined(NEXUS_FRONTEND_45216))
    if(NEXUS_Frontend_P_CheckDeviceOpen(handle)){
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }
#endif
    if (NULL == handle->readRegister) {
        if (handle->pParentFrontend) {
            return NEXUS_Frontend_ReadRegister(handle->pParentFrontend, address, value);
        }
        else {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else {
        return handle->readRegister(handle->pDeviceHandle, address, value);
    }
}

NEXUS_FrontendHandle NEXUS_Frontend_OpenStub( unsigned index )
{
    BSTD_UNUSED(index);
    BERR_TRACE(NEXUS_NOT_SUPPORTED);
    return NULL;
}

NEXUS_Error NEXUS_Frontend_Standby_priv(NEXUS_FrontendHandle handle, bool enabled, const NEXUS_StandbySettings *pSettings)
{
#if NEXUS_POWER_MANAGEMENT
    BDBG_ASSERT(NULL != handle);
#if defined(HUMAX_PLATFORM_BASE) && (defined(NEXUS_FRONTEND_3461)||defined(NEXUS_FRONTEND_45216))
    if(NEXUS_Frontend_P_CheckDeviceOpen(handle)){
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }
#endif
    if(handle->standby)
    {
        return handle->standby(handle->pDeviceHandle, enabled, pSettings);
    }
    else if ( handle->pParentFrontend && handle->pParentFrontend->standby ) {
        return NEXUS_Frontend_Standby_priv(handle->pParentFrontend, enabled, pSettings);
    }
    else
    {
        /* Frontend does not have a standby api. This is a valid case */
        return NEXUS_SUCCESS;
    }
#else
    BSTD_UNUSED(handle);
    BSTD_UNUSED(enabled);
    BSTD_UNUSED(pSettings);
    return NEXUS_SUCCESS;
#endif
}

NEXUS_Error NEXUS_Frontend_GetTemperature(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendTemperature *pTemp
    )
{
    NEXUS_OBJECT_ASSERT(NEXUS_Frontend, handle);
#if defined(HUMAX_PLATFORM_BASE) && (defined(NEXUS_FRONTEND_3461)||defined(NEXUS_FRONTEND_45216))
    if(NEXUS_Frontend_P_CheckDeviceOpen(handle)){
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }
#endif
    if ( NULL == handle->getTemperature)
    {
        if ( handle->pParentFrontend )
        {
            return NEXUS_Frontend_GetTemperature(handle->pParentFrontend, pTemp);
        }
        else
        {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else
    {
        return handle->getTemperature(handle->pDeviceHandle, pTemp);
    }
}

NEXUS_Error NEXUS_Frontend_TransmitDebugPacket(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendDebugPacketType type,
    const uint8_t *pBuffer,
    size_t size
    )
{
    BDBG_ASSERT(NULL != handle);
#if defined(HUMAX_PLATFORM_BASE) && (defined(NEXUS_FRONTEND_3461)||defined(NEXUS_FRONTEND_45216))
    if(NEXUS_Frontend_P_CheckDeviceOpen(handle)){
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }
#endif
	if ( NULL == handle->transmitDebugPacket)
	{
		if (handle->pParentFrontend)
		{
			return NEXUS_Frontend_TransmitDebugPacket(handle->pParentFrontend, type, pBuffer, size);
		}
		else
		{
			return BERR_TRACE(BERR_NOT_SUPPORTED);
		}
	}
	else
	{
		return handle->transmitDebugPacket(handle->pDeviceHandle, type, pBuffer, size);
	}
}

NEXUS_FrontendConnectorHandle NEXUS_Frontend_GetConnector( NEXUS_FrontendHandle handle )
{
    NEXUS_OBJECT_ASSERT(NEXUS_Frontend, handle);
    return handle->connector;
}

NEXUS_Error NEXUS_Frontend_P_GetInternalGain(NEXUS_FrontendDeviceHandle handle, const NEXUS_GainParameters *params, NEXUS_InternalGainSettings *pSettings)
{
    BDBG_ASSERT(NULL != handle);
#if defined(HUMAX_PLATFORM_BASE) && (defined(NEXUS_FRONTEND_3461)||defined(NEXUS_FRONTEND_45216))
    if(NEXUS_FrontendDevice_P_CheckOpen(handle)){
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }
#endif
    if ( NULL == handle->getInternalGain)
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    else
    {
        return handle->getInternalGain(handle->pDevice, params, pSettings);
    }
}

NEXUS_Error NEXUS_Frontend_P_GetExternalGain(NEXUS_FrontendDeviceHandle handle, NEXUS_ExternalGainSettings *pSettings)
{
    BDBG_ASSERT(NULL != handle);
#if defined(HUMAX_PLATFORM_BASE) && (defined(NEXUS_FRONTEND_3461)||defined(NEXUS_FRONTEND_45216))
    if(NEXUS_FrontendDevice_P_CheckOpen(handle)){
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }
#endif
    if ( NULL == handle->getExternalGain)
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    else
    {
        return handle->getExternalGain(handle->pDevice,pSettings);
    }
}

NEXUS_Error NEXUS_Frontend_P_SetExternalGain(NEXUS_FrontendDeviceHandle handle, const NEXUS_ExternalGainSettings *pSettings)
{
    BDBG_ASSERT(NULL != handle);
#if defined(HUMAX_PLATFORM_BASE) && (defined(NEXUS_FRONTEND_3461)||defined(NEXUS_FRONTEND_45216))
    if(NEXUS_FrontendDevice_P_CheckOpen(handle)){
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }
#endif
    if ( NULL == handle->setExternalGain)
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    else
    {
        return handle->setExternalGain(handle->pDevice, pSettings);
    }
}

void NEXUS_FrontendDevice_GetDefaultLinkSettings(
    NEXUS_FrontendDeviceLinkSettings *pSettings
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->rfInput = NEXUS_FrontendDeviceRfInput_eDaisy;
}

NEXUS_FrontendDeviceHandle NEXUS_Frontend_GetDevice(
    NEXUS_FrontendHandle handle
    )
{
    BDBG_ASSERT(NULL != handle);

    if(NULL == handle->pGenericDeviceHandle)
    {
        BERR_TRACE(BERR_NOT_INITIALIZED);
        return NULL;
    }
    else
    {
        return handle->pGenericDeviceHandle;
    }

}

NEXUS_Error NEXUS_FrontendDevice_Link(
    NEXUS_FrontendDeviceHandle parentHandle,
    NEXUS_FrontendDeviceHandle childHandle,
    const NEXUS_FrontendDeviceLinkSettings *pSettings
    )
{
    NEXUS_Error rc=NEXUS_SUCCESS;

    BLST_D_INSERT_HEAD(&parentHandle->deviceChildList, childHandle, link);
    childHandle->parent = parentHandle;
    childHandle->linkSettings = *pSettings;

    return rc;
}
#if defined(HUMAX_PLATFORM_BASE) && (defined(NEXUS_FRONTEND_3461)||defined(NEXUS_FRONTEND_45216))
void NEXUS_FrontendDevice_Unlink(NEXUS_FrontendDeviceHandle parentHandle,NEXUS_FrontendDeviceHandle childHandle)
{
    NEXUS_FrontendDeviceHandle tempChildHandle = NULL;
    if(childHandle){
        BLST_D_REMOVE(&parentHandle->deviceChildList, childHandle, link);
    }
    else {
        /* Remove all the children of this device. */
        for ( tempChildHandle = BLST_D_FIRST(&parentHandle->deviceChildList); NULL != tempChildHandle; tempChildHandle = BLST_D_NEXT(tempChildHandle, link) )
        {
            BLST_D_REMOVE(&parentHandle->deviceChildList, tempChildHandle, link);
            tempChildHandle->parent = NULL;
        }
    }

    /* Remove the device from its parent's list of children. */
    if(parentHandle->parent != NULL){
        BLST_D_REMOVE(&parentHandle->parent->deviceChildList, parentHandle, link);
    }

}

void  NEXUS_FrontendDevice_Close(NEXUS_FrontendDeviceHandle handle)
{
    BDBG_ASSERT(NULL != handle);

    if ( NULL == handle->close)
    {
        BERR_TRACE(BERR_NOT_SUPPORTED);
        return;
    }
    else
    {
        handle->close(handle->pDevice);
    }
}
NEXUS_Error NEXUS_FrontendDevice_GetStatus(NEXUS_FrontendDeviceHandle handle, NEXUS_FrontendDeviceStatus *pStatus)
{
    BDBG_ASSERT(NULL != handle);

    if ( NULL == handle->getStatus)
    {
     return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    else
    {
     return handle->getStatus(handle->pDevice, pStatus);
    }
}
#else
void NEXUS_FrontendDevice_Unlink(NEXUS_FrontendDeviceHandle parentHandle,NEXUS_FrontendDeviceHandle childHandle)
{
    BSTD_UNUSED(parentHandle);
    BSTD_UNUSED(childHandle);
}

void  NEXUS_FrontendDevice_Close(NEXUS_FrontendDeviceHandle handle)
{
    BSTD_UNUSED(handle);
}

NEXUS_Error NEXUS_FrontendDevice_GetStatus(NEXUS_FrontendDeviceHandle handle, NEXUS_FrontendDeviceStatus *pStatus)
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pStatus);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}
#endif

NEXUS_Error NEXUS_FrontendDevice_LinkAmplifier(
    NEXUS_FrontendDeviceHandle handle,
    NEXUS_AmplifierHandle amplifierHandle
    )
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(NULL != amplifierHandle);
#if NEXUS_AMPLIFIER_SUPPORT
    handle->amplifier = amplifierHandle;
#endif
    return rc;
}

#if NEXUS_AMPLIFIER_SUPPORT
NEXUS_Error NEXUS_Frontend_P_GetAmplifierStatus(NEXUS_FrontendDeviceHandle handle, NEXUS_AmplifierStatus *pStatus)
{
    BDBG_ASSERT(NULL != handle);
#if defined(HUMAX_PLATFORM_BASE) && (defined(NEXUS_FRONTEND_3461)||defined(NEXUS_FRONTEND_45216))
    if(NEXUS_FrontendDevice_P_CheckOpen(handle)){
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }
#endif
    if ( NULL == handle->getAmplifierStatus)
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    else
    {
        return handle->getAmplifierStatus(handle->pDevice, pStatus);
    }
}

NEXUS_Error NEXUS_Frontend_P_SetAmplifierStatus(NEXUS_FrontendDeviceHandle handle, const NEXUS_AmplifierStatus *pStatus)
{
    BDBG_ASSERT(NULL != handle);
#if defined(HUMAX_PLATFORM_BASE) && (defined(NEXUS_FRONTEND_3461)||defined(NEXUS_FRONTEND_45216))
    if(NEXUS_FrontendDevice_P_CheckOpen(handle)){
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }
#endif
    if ( NULL == handle->setAmplifierStatus)
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    else
    {
        return handle->setAmplifierStatus(handle->pDevice, pStatus);
    }
}
#endif
#if defined(HUMAX_PLATFORM_BASE) && (defined(NEXUS_FRONTEND_3461)||defined(NEXUS_FRONTEND_45216))
NEXUS_Error NEXUS_FrontendDevice_P_CheckOpen(NEXUS_FrontendDeviceHandle handle)
{
    NEXUS_Error rc = NEXUS_SUCCESS;

iterate:
    BDBG_MSG(("openPending = %d, openFailed = %d", handle->openPending, handle->openFailed));
    if(handle->openFailed){
        rc = BERR_TRACE(NEXUS_NOT_INITIALIZED); goto done;
    }
    else if(handle->openPending){
        BKNI_Sleep(500);
        goto iterate;
    }
done:
    return rc;
}

NEXUS_Error NEXUS_Frontend_P_CheckDeviceOpen(NEXUS_FrontendHandle handle)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_FrontendDeviceHandle deviceHandle = NULL;

    deviceHandle = NEXUS_Frontend_GetDevice(handle);
    if(deviceHandle == NULL) goto done;

    rc = NEXUS_FrontendDevice_P_CheckOpen(deviceHandle);
done:
    return rc;
}
#else
NEXUS_Error NEXUS_FrontendDevice_P_CheckOpen(NEXUS_FrontendDeviceHandle handle)
{
    BSTD_UNUSED(handle);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

NEXUS_Error NEXUS_Frontend_P_CheckDeviceOpen(NEXUS_FrontendHandle handle)
{
    BSTD_UNUSED(handle);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}
#endif

void NEXUS_FrontendDevice_GetCapabilities(NEXUS_FrontendDeviceHandle handle, NEXUS_FrontendDeviceCapabilities *pCapabilities)
{
    BDBG_ASSERT(NULL != handle);

    if(NEXUS_FrontendDevice_P_CheckOpen(handle)){
        BDBG_ERR(("Device open failed. Cannot get device capabilities."));
        BERR_TRACE(NEXUS_NOT_INITIALIZED);
        return;
    }

    if ( NULL == handle->getCapabilities)
    {
        BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    else
    {
        handle->getCapabilities(handle->pDevice, pCapabilities);
    }
}

#if !defined(NEXUS_FRONTEND_45216) /* WA-7500, WA-8000 & WA-8500 */

#if NEXUS_HAS_MXT

void NEXUS_Frontend_P_DumpMtsifConfig(NEXUS_FrontendHandle frontend, const NEXUS_FrontendDeviceMtsifConfig *pConfig);


/* configure the demod transport core to route a given frontend to one or more parser bands on host */
NEXUS_Error NEXUS_Frontend_P_SetMtsifConfig(NEXUS_FrontendHandle frontend, NEXUS_FrontendDeviceMtsifConfig *pConfig)
{
#if NEXUS_HAS_MXT && NEXUS_NUM_PARSER_BANDS
    /*
    The app specifies a 2-point connection:
      frontend -> host_PB

    which Nexus must translate into an internal 3-point connection:
      demod_IB -> demod_PB -> host_PB

    the first arrow is arbitrary: just find a free demod_PB and use it
    the second arrow is fixed: this MUST be a 1-to-1 connection, otherwise we have multiple
      frontends feeding into the same host_PB. the HW doesn't enforce a 1-to-1 connection, so
      Nexus must.
    */

    unsigned i, j;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DEMOD_TSMF)
	unsigned unDemodPBstartIdx;
#endif
    unsigned demodIb;
    NEXUS_FrontendUserParameters userParams;
    BMXT_Handle mxt;
    NEXUS_Error rc = NEXUS_SUCCESS;
    bool connect = false;
#if defined(HUMAX_PLATFORM_BASE)
	bool Connection_Request = false;
#endif /* HUMAX_PLATFORM_BASE */

    BDBG_OBJECT_ASSERT(frontend, NEXUS_Frontend);
    mxt = pConfig->mxt;

    NEXUS_Frontend_GetUserParameters(frontend, &userParams);
    demodIb = userParams.param1;

    if (!userParams.isMtsif) {
        return NEXUS_SUCCESS;
    }
    if (mxt==NULL) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    if (pConfig->numDemodPb==0 || pConfig->numDemodPb>BMXT_MAX_NUM_PARSERS) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    if (demodIb >= pConfig->numDemodIb) { /* for all current cases, numDemodIb is 16, but no frontend uses IB>=8. if demodIb >=8, it's probably a sign that userParams.param1 is set incorrectly */
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    /* 1. find all host PBs whose MTSIF source is equal to this frontend handle. we must map those PBs to this frontend */
    for (i=0; i<NEXUS_NUM_PARSER_BANDS; i++) {
        NEXUS_ParserBand band = NEXUS_ParserBand_e0+i;
        NEXUS_ParserBandSettings bandSettings;
        bool isTsmf = false;
        NEXUS_ParserBand_GetSettings(band, &bandSettings);

#if NEXUS_HAS_TSMF
        if (bandSettings.sourceType==NEXUS_ParserBandSourceType_eTsmf) {
            /* IB -> TSMF -> PB -> PB connection. requires that we still make the normal IB->PB->PB connection and apply TSMF settings afterwards */
            NEXUS_Module_Lock(g_NEXUS_frontendModuleSettings.transport);
            isTsmf = NEXUS_Tsmf_TestFrontendInput_priv(bandSettings.sourceTypeSettings.tsmf, frontend->connector);
            NEXUS_Module_Unlock(g_NEXUS_frontendModuleSettings.transport);
        }
#endif

        if ((bandSettings.sourceType==NEXUS_ParserBandSourceType_eMtsif && bandSettings.sourceTypeSettings.mtsif==frontend->connector)
            || isTsmf)
        {
            bool need_new_connect = true;

#if defined(HUMAX_PLATFORM_BASE)
			Connection_Request = true;
#endif /* HUMAX_PLATFORM_BASE */

            /* 2. check all current connections for existing connections or conflicts */
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DEMOD_TSMF)
			/* check whether tsmf monitoring parserband or not */
			if(band == NEXUS_NUM_PARSER_BANDS-1)	/* tsmf monitoring parserband */
			{
				unDemodPBstartIdx=0;
			}
			else
			{
				unDemodPBstartIdx=1;
			}
            for (j=unDemodPBstartIdx; j<pConfig->numDemodPb; j++) {
#else
            for (j=0; j<pConfig->numDemodPb; j++) {
#endif
                if (pConfig->demodPbSettings[j].enabled) {
                    if (pConfig->demodPbSettings[j].virtualNum==(unsigned)band && pConfig->demodPbSettings[j].inputBandNum==demodIb) {
                        /* specified FE-> PB connection already exists due to tuning again without untune */
                        need_new_connect = false;
                        BDBG_MSG(("MTSIF exist:   %#lx:%#lx IB%u -> PB%u -> PB%u", frontend, mxt, demodIb, j, band));
                        connect = true;
                        break;
                    }
                    if (pConfig->demodPbSettings[j].virtualNum==(unsigned)band && pConfig->demodPbSettings[j].inputBandNum!=demodIb) {
                        /* a different FE is still connected to this PB. we only check for frontends on the same device */
                        /* TODO: could use g_NEXUS_Frontend_P_HostMtsifConfig to check other devices, if ever needed */
                        BDBG_ERR(("MTSIF conflict with existing connection: IB%u -> PB%u -> PB%u", pConfig->demodPbSettings[j].inputBandNum, j, band));
                        BDBG_ERR(("You must untune the old frontend before tuning with a new frontend using the same parser band"));
                        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
                    }
                }
            }

            if (!need_new_connect) {
                continue;
            }

            /* 3. pick a free demod PB */
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DEMOD_TSMF)
            for (j=unDemodPBstartIdx; j<pConfig->numDemodPb; j++) {
#else
            for (j=0; j<pConfig->numDemodPb; j++) {
#endif
                /* coverity [dead_error_condition:false] */
                if (pConfig->demodPbSettings[j].enabled==false) {
                    BMXT_ParserConfig parserConfig;
                    BMXT_ParserBandMapping bandMap;

                    if (pConfig->slave) {
                        /* if this is a chained frontend device, we have an additional requirement of not picking
                           a PB number that is already used by another frontend device */
                        unsigned k;
                        for (k=0; k<NEXUS_NUM_PARSER_BANDS; k++) {
                            if (g_NEXUS_Frontend_P_HostMtsifConfig.hostPbSettings[k].connected &&
                                g_NEXUS_Frontend_P_HostMtsifConfig.hostPbSettings[k].demodPb==j)
                            {
                                goto next;
                            }
                        }
                    }

                    BMXT_GetParserConfig(mxt, j, &parserConfig);
                    /* 4a. map demod IB->demod PB */
                    pConfig->demodPbSettings[j].inputBandNum = parserConfig.InputBandNumber = demodIb;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DEMOD_TSMF)
					/* Disable demodPb TS output except tsmf monitoring parserband*/
					if((j==0) && (band == NEXUS_NUM_PARSER_BANDS-1))
					{
                    pConfig->demodPbSettings[j].enabled = parserConfig.Enable = true;
					}
					else
					{
						pConfig->demodPbSettings[j].enabled = true;
						parserConfig.Enable = false;
					}
#else
                    pConfig->demodPbSettings[j].enabled = parserConfig.Enable = true;
#endif
                    BMXT_SetParserConfig(mxt, j, &parserConfig);

                    /* 4b. set virtual PB num of demod PB equal to host PB num */
                    BMXT_GetParserMapping(mxt, &bandMap);
                    pConfig->demodPbSettings[j].virtualNum = bandMap.FrontEnd[j].VirtualParserBandNum = band;
                    BMXT_SetParserMapping(mxt, &bandMap);
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DEMOD_TSMF)
					if(band != NEXUS_NUM_PARSER_BANDS-1)
					{
                    	BDBG_ERR(("-----------------------------------------------------\n"));
                    	BDBG_ERR(("MTSIF connect: %#lx:%#lx IB%u -> PB%u -> PB%u %s\n", frontend, mxt, demodIb, j, band, pConfig->slave?"(slave)":""));
                    	BDBG_ERR(("-----------------------------------------------------\n\n\n"));
					}
#else
                    BDBG_MSG(("MTSIF connect: %#lx:%#lx IB%u -> PB%u -> PB%u %s", frontend, mxt, demodIb, j, band, pConfig->slave?"(slave)":""));
#endif
                    connect = true;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DEMOD_TSMF)
					if((band != NEXUS_NUM_PARSER_BANDS-1) && (frontend->bEnabledTSMF == true) && (frontend->bConnectedNormal == false))
					{
						BMXT_TSMF_SetParserConfig(mxt, j, frontend->nTSMFIndex, true);
                        NEXUS_Frontend_P_EnableParserOutput(mxt, j);
                    	BDBG_ERR(("-----------------------------------------------------\n"));
						BDBG_ERR(("IB(%u) is already enabled TSMF(%u). Enable demodPB(%u) right now.\n", demodIb, frontend->nTSMFIndex,j));
                    	BDBG_ERR(("-----------------------------------------------------\n\n\n"));
					}

					if((band != NEXUS_NUM_PARSER_BANDS-1) && (frontend->bEnabledTSMF == false) && (frontend->bConnectedNormal == true))
					{
                        NEXUS_Frontend_P_EnableParserOutput(mxt, j);
                    	BDBG_ERR(("-----------------------------------------------------\n"));
	                	BDBG_ERR(("IB(%u) is connected Normal(Non-TSMF). Enable demodPB(%u) right now.\n", demodIb,j));
                    	BDBG_ERR(("-----------------------------------------------------\n\n\n"));
					}
#endif
                    g_NEXUS_Frontend_P_HostMtsifConfig.hostPbSettings[i].demodPb = j;
                    g_NEXUS_Frontend_P_HostMtsifConfig.hostPbSettings[i].connected = true;
                    break;
                }
next:
                BSTD_UNUSED(0);
            }
            /* did not find free demod PB */
            if (j>=pConfig->numDemodPb) {
                BDBG_ERR(("No demod PB available to connect: %#lx:%#lx IB%u -> PB? -> PB%u", frontend, mxt, demodIb, band));
                return BERR_TRACE(NEXUS_NOT_AVAILABLE);
            }
        }
        else if (bandSettings.sourceType==NEXUS_ParserBandSourceType_eMtsif && bandSettings.sourceTypeSettings.mtsif==NULL)
        {
            /* if NULL, disable all demod PBs feeding to this host PB. this is used by NEXUS_Frontend_ReapplyTransportSettings */
            for (j=0; j<pConfig->numDemodPb; j++) {
                if (pConfig->demodPbSettings[j].enabled && pConfig->demodPbSettings[j].virtualNum==i) {
                    BMXT_ParserConfig parserConfig;
                    BMXT_GetParserConfig(mxt, j, &parserConfig);
                    pConfig->demodPbSettings[j].enabled = parserConfig.Enable = false;
                    BMXT_SetParserConfig(mxt, j, &parserConfig);
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DEMOD_TSMF)
					/* Disable tsmf enable field of demod PB */
					BMXT_TSMF_SetParserConfig(mxt, j, 0, false);
#endif
                    BDBG_MSG(("MTSIF nullify: %#lx:%#lx IB%u -> PB%u -> PB%u", frontend, mxt, demodIb, j, pConfig->demodPbSettings[j].virtualNum));
                    g_NEXUS_Frontend_P_HostMtsifConfig.hostPbSettings[i].demodPb = 0;
                    g_NEXUS_Frontend_P_HostMtsifConfig.hostPbSettings[i].connected = false;
                }
            }
        }
    }

#if defined(HUMAX_PLATFORM_BASE)
	if (!connect && Connection_Request)
#else
    if (!connect)
#endif /* HUMAX_PLATFORM_BASE */
    {
        BDBG_WRN(("No destination parser band(s) specified for this MTSIF frontend %#lx", frontend));
        BDBG_WRN(("You must set NEXUS_ParserBandSettings.sourceTypeSettings.mtsif before calling Tune"));
        BDBG_WRN(("See NEXUS_Frontend_GetConnector()"));
    }

#if 0 /* enable for more debug messages */
    NEXUS_Frontend_P_DumpMtsifConfig(frontend, pConfig);
#endif

    return rc;
#else
    BSTD_UNUSED(frontend);
    BSTD_UNUSED(pConfig);
    return NEXUS_SUCCESS;
#endif
}

NEXUS_Error NEXUS_Frontend_P_UnsetMtsifConfig(NEXUS_FrontendHandle frontend, NEXUS_FrontendDeviceMtsifConfig *pConfig)
{
#if NEXUS_HAS_MXT && NEXUS_NUM_PARSER_BANDS
    unsigned i;
    unsigned demodIb;
    NEXUS_FrontendUserParameters userParams;
    BMXT_Handle mxt;
    NEXUS_Error rc = NEXUS_SUCCESS;

    BDBG_OBJECT_ASSERT(frontend, NEXUS_Frontend);
    mxt = pConfig->mxt;

    NEXUS_Frontend_GetUserParameters(frontend, &userParams);
    demodIb = userParams.param1;

    if (!userParams.isMtsif) {
        return NEXUS_SUCCESS;
    }
    if (mxt==NULL) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    if (pConfig->numDemodPb==0 || pConfig->numDemodPb>BMXT_MAX_NUM_PARSERS) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    if (demodIb >= pConfig->numDemodIb) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    /* sever all MTSIF connections whose source is this frontend */
    for (i=0; i<pConfig->numDemodPb; i++) {
        if (pConfig->demodPbSettings[i].enabled && pConfig->demodPbSettings[i].inputBandNum==demodIb) {
            BMXT_ParserConfig parserConfig;

            /* disable parser */
            BMXT_GetParserConfig(mxt, i, &parserConfig);
            pConfig->demodPbSettings[i].enabled = parserConfig.Enable = false;
            BMXT_SetParserConfig(mxt, i, &parserConfig);

            #if 0 /* assume that a disabled parser's virtualnum does not matter; we don't have to bother changing it */
            BMXT_ParserBandMapping bandMap;
            BMXT_GetParserMapping(pDevice->mxt, &bandMap);
            bandMap.FrontEnd[i].VirtualParserBandNum = i;
            BMXT_SetParserMapping(pDevice->mxt, &bandMap);
            #endif

            BDBG_MSG(("MTSIF disconnect: %#lx:%#lx IB%u -> PB%u -> PB%u", frontend, mxt, demodIb, i, pConfig->demodPbSettings[i].virtualNum));
            g_NEXUS_Frontend_P_HostMtsifConfig.hostPbSettings[i].demodPb = 0;
            g_NEXUS_Frontend_P_HostMtsifConfig.hostPbSettings[i].connected = false;
        }
    }

    return rc;
#else
    BSTD_UNUSED(frontend);
    BSTD_UNUSED(pConfig);
    return NEXUS_SUCCESS;
#endif
}

/* debug function to dump all known info */
void NEXUS_Frontend_P_DumpMtsifConfig(NEXUS_FrontendHandle frontend, const NEXUS_FrontendDeviceMtsifConfig *pConfig)
{
#if NEXUS_HAS_MXT && NEXUS_NUM_PARSER_BANDS
    BMXT_Handle mxt;
    BMXT_ParserConfig parserConfig;
    BMXT_ParserBandMapping parserMapping;
    NEXUS_ParserBandSettings parserBandSettings;
    unsigned i;
    unsigned const NUM_PARSERS = 9; /* minimum number of PB on demod chips. increase as needed for debug */

    BDBG_OBJECT_ASSERT(frontend, NEXUS_Frontend);
    BDBG_ASSERT(pConfig);

    mxt = pConfig->mxt;
    if (!mxt) {
        BDBG_ERR(("NEXUS_Frontend_P_DumpMtsifConfig: frontend %#lx has NULL MXT handle", frontend));
        return;
    }

    BDBG_MSG(("DumpMtsifConfig %#lx", mxt));
    /* print out real and cached demod PB settings */
    for (i=0; i<NUM_PARSERS; i++) {
        BMXT_GetParserConfig(mxt, i, &parserConfig);
        BMXT_GetParserMapping(mxt, &parserMapping);
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DEMOD_TSMF)
        BDBG_ERR(("demod PB[%u]: en %u:%u, IB %u:%u, virt %u:%u  EI(%u), TS(%u), AN(%u), AP(%u), DM(%u), PBTSMF(%u:%u), TSMF(%u,%u,%u,%u)", i,
            parserConfig.Enable, pConfig->demodPbSettings[i].enabled,
            parserConfig.InputBandNumber, pConfig->demodPbSettings[i].inputBandNum,
            parserMapping.FrontEnd[i].VirtualParserBandNum, pConfig->demodPbSettings[i].virtualNum,
            parserConfig.ErrorInputIgnore, parserConfig.TsMode, parserConfig.AcceptNulls, parserConfig.AllPass,
            parserConfig.DssMode,
			parserConfig.ParserTSMF_EN, parserConfig.ParserTSMF_SEL,
			parserConfig.TSMF_EN, parserConfig.TSMF_AutoEN, parserConfig.TSMF_InputSel, parserConfig.TSMF_RelativeTsNum));
#else
        BDBG_MSG(("demod PB[%u]: en %u:%u, IB %u:%u, virt %u:%u", i,
            parserConfig.Enable, pConfig->demodPbSettings[i].enabled,
            parserConfig.InputBandNumber, pConfig->demodPbSettings[i].inputBandNum,
            parserMapping.FrontEnd[i].VirtualParserBandNum, pConfig->demodPbSettings[i].virtualNum));
#endif
    }

    /* print out host PB settings */
    for (i=0; i<NEXUS_NUM_PARSER_BANDS; i++) {
        NEXUS_ParserBand_GetSettings(NEXUS_ParserBand_e0 + i, &parserBandSettings);
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DEMOD_TSMF)
        BDBG_ERR(("host PB[%2u]: %u:%#lx", i, parserBandSettings.sourceType, parserBandSettings.sourceType==NEXUS_ParserBandSourceType_eTsmf?parserBandSettings.sourceTypeSettings.tsmf:parserBandSettings.sourceType==NEXUS_ParserBandSourceType_eMtsif?parserBandSettings.sourceTypeSettings.mtsif:NULL));
#else
        BDBG_MSG(("host PB[%2u]: %u:%#lx", i, parserBandSettings.sourceType, parserBandSettings.sourceType==NEXUS_ParserBandSourceType_eMtsif?parserBandSettings.sourceTypeSettings.mtsif:NULL));
#endif
    }

    /* possible failure reasons:
       FE => demod_IB => demod_PB => host_PB

       1. first arrow
          - userParams.param1 is not set incorrect demod_IB
            - nexus_platform_frontend.c could have set it incorrectly due to human error, or if the channel num does not correspond to the IB num
              e.g. if 3128 channel 2 is not actually mapped to IB2
          - the app could have called NEXUS_Frontend_SetUserParameters and changed it

       2. second arrow
          - incorrect IB mapped to PB
          - PB not enabled
          this is not app-controlled, so not expected to fail

       3. third arrow
          - demod_PB's virtual PB num set incorrectly because host_PB's sourceTypeSettings.mtsif set incorrectly

       4. a different demod_PB is enabled and has the same virtual PB num
          within the same frontend device, this is not possible because we error out, but we cannot check other frontend devices
     */

#else
    BSTD_UNUSED(frontend);
    BSTD_UNUSED(pConfig);
#endif
}

#else /* NEXUS_HAS_MXT */

#endif /* NEXUS_HAS_MXT */

#if NEXUS_HAS_MXT
static void NEXUS_Frontend_P_MxtTimer(void* context)
{
    NEXUS_FrontendDeviceMtsifConfig *pConfig = context;
    BMXT_Handle mxt = pConfig->mxt;
    unsigned i, demodPb, hostPb;
    NEXUS_ParserBandSettings parserSettings;
    uint32_t status = BMXT_ReadIntStatusRegister(mxt, BMXT_IntReg_eFE_INTR_STATUS0);

    BDBG_MSG_TRACE(("NEXUS_Frontend_P_MxtTimer: status %#x", status));

    /* the interrupt should only fire if the int status matches the particular frontend that had the callback set */
    if (status) {
        /* bits 0~15 are length error for parser 0 through 15. bits 16~31 are transport error for parser 0 through 15 */
        for (i=0; i<16; i++) {
            demodPb = i;
            if (demodPb<pConfig->numDemodPb && ((status>>demodPb)&0x1)) {
                hostPb = pConfig->demodPbSettings[demodPb].virtualNum;
                NEXUS_ParserBand_GetSettings((NEXUS_ParserBand)hostPb, &parserSettings);
                if (parserSettings.lengthError.callback) {
                    BDBG_MSG_TRACE(("NEXUS_Frontend_P_MxtTimer: len callback, demod PB%u -> host PB%u", demodPb, hostPb));
                    BKNI_EnterCriticalSection();
                    NEXUS_ParserBand_P_LengthError_priv_isr((NEXUS_ParserBand)hostPb);
                    BKNI_LeaveCriticalSection();
                }
            }
        }
        for (i=16; i<32; i++) {
            demodPb = i-16;
            if (demodPb<pConfig->numDemodPb && ((status>>demodPb)&0x1)) {
                hostPb = pConfig->demodPbSettings[demodPb].virtualNum;
                NEXUS_ParserBand_GetSettings((NEXUS_ParserBand)hostPb, &parserSettings);
                if (parserSettings.teiError.callback) {
                    BDBG_MSG_TRACE(("NEXUS_Frontend_P_MxtTimer: tei callback, demod PB%u -> host PB%u", demodPb, hostPb));
                    BKNI_EnterCriticalSection();
                    NEXUS_ParserBand_P_TEIError_priv_isr((NEXUS_ParserBand)hostPb);
                    BKNI_LeaveCriticalSection();
                }
            }
        }
    }

    pConfig->timer = NEXUS_ScheduleTimer(1000, NEXUS_Frontend_P_MxtTimer, pConfig);
}
#endif

NEXUS_Error NEXUS_Frontend_P_SetTransportConfig(NEXUS_FrontendHandle frontend, NEXUS_FrontendDeviceMtsifConfig *pConfig)
{
#if NEXUS_HAS_MXT
    unsigned i, demodIb;
    NEXUS_FrontendUserParameters userParams;
    BMXT_Handle mxt;
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_ParserBandSettings parserSettings;
    BMXT_ParserConfig mxtParserConfig;
    bool setTimer = false;

    BDBG_OBJECT_ASSERT(frontend, NEXUS_Frontend);
    BDBG_ASSERT(pConfig);

    NEXUS_Frontend_GetUserParameters(frontend, &userParams);
    demodIb = userParams.param1;
    mxt = pConfig->mxt;

    if (!userParams.isMtsif) {
        return NEXUS_SUCCESS;
    }
    if (mxt==NULL) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    if (pConfig->numDemodPb==0 || pConfig->numDemodPb>BMXT_MAX_NUM_PARSERS) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    if (demodIb >= pConfig->numDemodIb) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    for (i=0; i<pConfig->numDemodPb; i++) {

        /* for each demod PB that's enabled and whose source is this frontend... */
        if (pConfig->demodPbSettings[i].enabled && pConfig->demodPbSettings[i].inputBandNum==demodIb) {

            /* grab the settings of the output PB on the host, and apply them to the demod PB */
            unsigned hostPb = pConfig->demodPbSettings[i].virtualNum;
            NEXUS_ParserBand_GetSettings((NEXUS_ParserBand)hostPb, &parserSettings);

            if ( (pConfig->demodPbSettings[i].errorInputIgnore != parserSettings.teiIgnoreEnabled) ||
                 (pConfig->demodPbSettings[i].timestampMode != (NEXUS_IS_DSS_MODE(parserSettings.transportType) ? BMXT_ParserTimestampMode_eBinary : BMXT_ParserTimestampMode_eMod300)) ||
                 (pConfig->demodPbSettings[i].dssMode != NEXUS_IS_DSS_MODE(parserSettings.transportType)) )
            {
                BDBG_MSG(("NEXUS_Frontend_P_SetTransportConfig: demod PB%u <- host PB%u", i, hostPb));
                BMXT_GetParserConfig(mxt, i, &mxtParserConfig);
                /* translate NEXUS_ParserBandSettings to BMXT_ParserConfig.
                   note the ParserBandSettings have already been applied to host; we'll just apply them to the demod as well.
                   some may be needed, some not. */
                mxtParserConfig.ErrorInputIgnore = parserSettings.teiIgnoreEnabled;
                mxtParserConfig.TsMode = NEXUS_IS_DSS_MODE(parserSettings.transportType) ? BMXT_ParserTimestampMode_eBinary : BMXT_ParserTimestampMode_eMod300;
                mxtParserConfig.DssMode = NEXUS_IS_DSS_MODE(parserSettings.transportType);
                /* ignore allPass and acceptNullPackets for demod. demod is always in allpass + acceptnull mode. */
                BMXT_SetParserConfig(mxt, i, &mxtParserConfig);

                pConfig->demodPbSettings[i].errorInputIgnore = mxtParserConfig.ErrorInputIgnore;
                pConfig->demodPbSettings[i].timestampMode = mxtParserConfig.TsMode;
                pConfig->demodPbSettings[i].dssMode = mxtParserConfig.DssMode;
            }

            if (parserSettings.teiError.callback || parserSettings.lengthError.callback) {
                setTimer = true;
            }
        }
    }

    if (setTimer) {
        if (!pConfig->timer) {
            pConfig->timer = NEXUS_ScheduleTimer(1000, NEXUS_Frontend_P_MxtTimer, pConfig);
        }
    }
    else {
        if (pConfig->timer) {
            NEXUS_CancelTimer(pConfig->timer);
            pConfig->timer = NULL;
        }
    }

    return rc;
#else
    BSTD_UNUSED(frontend);
    BSTD_UNUSED(pConfig);
    return NEXUS_SUCCESS;
#endif
}

NEXUS_Error NEXUS_Frontend_P_UnsetTransportConfig(NEXUS_FrontendHandle frontend, NEXUS_FrontendDeviceMtsifConfig *pConfig)
{
#if NEXUS_HAS_MXT
    unsigned demodIb;
    NEXUS_FrontendUserParameters userParams;
    BMXT_Handle mxt;
    NEXUS_Error rc = NEXUS_SUCCESS;

    BDBG_OBJECT_ASSERT(frontend, NEXUS_Frontend);
    BDBG_ASSERT(pConfig);

    NEXUS_Frontend_GetUserParameters(frontend, &userParams);
    demodIb = userParams.param1;
    mxt = pConfig->mxt;

    if (!userParams.isMtsif) {
        return NEXUS_SUCCESS;
    }
    if (mxt==NULL) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    if (pConfig->numDemodPb==0 || pConfig->numDemodPb>BMXT_MAX_NUM_PARSERS) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    if (demodIb >= pConfig->numDemodIb) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    /* leave the settings, but kill the timer */
    if (pConfig->timer) {
        NEXUS_CancelTimer(pConfig->timer);
        pConfig->timer = NULL;
    }

    return rc;
#else
    BSTD_UNUSED(frontend);
    BSTD_UNUSED(pConfig);
    return NEXUS_SUCCESS;
#endif
}

#else /*#if defined(NEXUS_FRONTEND_45216)*/ /* From ursr 14.4.1 */

#if NEXUS_HAS_MXT

static void NEXUS_Frontend_P_MxtTimer(void* context)
{
    NEXUS_FrontendDeviceMtsifConfig *pConfig = context;
    BMXT_Handle mxt = pConfig->mxt;
    unsigned i;
    uint32_t status = BMXT_ReadIntStatusRegister(mxt, BMXT_IntReg_eFE_INTR_STATUS0);

    BDBG_MSG_TRACE(("NEXUS_Frontend_P_MxtTimer: status %#x", status));
    if (status) {
        unsigned lengthError = 0, transportError = 0; /* remap frontend PB to backend PB */
        /* bits 0~15 are length error for parser 0 through 15. bits 16~31 are transport error for parser 0 through 15 */
        for (i=0; i<16; i++) {
            if (i<pConfig->numDemodPb) {
                if ((status>>i)&0x1) {
                    lengthError |= 1 << pConfig->demodPbSettings[i].virtualNum;
                }
                if ((status>>(i+16))&0x1) {
                    transportError |= 1 << pConfig->demodPbSettings[i].virtualNum;
                }
            }
        }
        NEXUS_Module_Lock(g_NEXUS_frontendModuleSettings.transport);
        NEXUS_ParserBand_P_MtsifErrorStatus_priv(lengthError, transportError);
        NEXUS_Module_Unlock(g_NEXUS_frontendModuleSettings.transport);
    }

    pConfig->timer = NEXUS_ScheduleTimer(1000, NEXUS_Frontend_P_MxtTimer, pConfig);
}


#include "priv/nexus_transport_priv.h"

NEXUS_Error NEXUS_Frontend_P_InitMtsifConfig(NEXUS_FrontendDeviceMtsifConfig *pConfig, const BMXT_Settings *mxtSettings)
{
    BMXT_ParserConfig parserConfig;
    unsigned i;
    NEXUS_Error rc;
    BMXT_Handle mxt = pConfig->mxt;

    if (mxt==NULL) {
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }
    /* TODO: we could move BMXT_Open() here as well, but some platforms (e.g. 7366) would need to pass in an extra BREG handle to this function */

    BKNI_Memset(pConfig, 0, sizeof(NEXUS_FrontendDeviceMtsifConfig));
    pConfig->mxt = mxt;
    pConfig->numDemodIb = BMXT_GetNumResources(mxtSettings->chip, mxtSettings->chipRev, BMXT_ResourceType_eInputBand);
    pConfig->numDemodPb = BMXT_GetNumResources(mxtSettings->chip, mxtSettings->chipRev, BMXT_ResourceType_eParser);

    for (i=0; i<pConfig->numDemodPb; i++) {
        rc = BMXT_GetParserConfig(pConfig->mxt, i, &parserConfig);
        if (rc) { return BERR_TRACE(rc); }

        /* turn off demod parsers */
        parserConfig.Enable = false;

        /* cache other settings */
        pConfig->demodPbSettings[i].errorInputIgnore = parserConfig.ErrorInputIgnore;
        pConfig->demodPbSettings[i].timestampMode = (unsigned)parserConfig.TsMode;
        pConfig->demodPbSettings[i].dssMode = parserConfig.DssMode;

        rc = BMXT_SetParserConfig(pConfig->mxt, i, &parserConfig);
        if (rc) { return BERR_TRACE(rc); }
    }

    return NEXUS_SUCCESS;
}

void NEXUS_Frontend_P_DumpMtsifConfig(const NEXUS_FrontendDeviceMtsifConfig *pConfig);


/* configure the demod transport core to route a given frontend to one or more parser bands on host */
static NEXUS_Error NEXUS_Frontend_P_ConfigMtsifConfig(NEXUS_FrontendHandle frontend, bool enabled)
{
    unsigned i;
    unsigned demodIb, demodPb, demodMtsifTx;
    NEXUS_FrontendUserParameters userParams;
    NEXUS_FrontendDeviceMtsifConfig *pConfig = &frontend->pGenericDeviceHandle->mtsifConfig;
    NEXUS_FrontendDeviceMtsifConfig *pChainedConfig = frontend->pGenericDeviceHandle->chainedConfig; /* points to a chained MTSIF config, if it exists */
    BMXT_Handle mxt = pConfig->mxt;
    NEXUS_Error rc = NEXUS_SUCCESS;
    struct NEXUS_MtsifParserBandSettings mtsifConnections[MAX_PB_PER_MTSIF_FRONTEND];
    unsigned num;
    bool setTimer = false;

    NEXUS_Frontend_GetUserParameters(frontend, &userParams);
    demodIb = NEXUS_FRONTEND_USER_PARAM1_GET_INPUT_BAND(userParams.param1);
    demodMtsifTx = NEXUS_FRONTEND_USER_PARAM1_GET_MTSIF_TX(userParams.param1);

    if (!userParams.isMtsif) {
        return NEXUS_SUCCESS;
    }
    if (mxt==NULL) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    if (pConfig->numDemodIb==0 || pConfig->numDemodIb>BMXT_MAX_NUM_INPUT_BANDS || pConfig->numDemodPb==0 || pConfig->numDemodPb>BMXT_MAX_NUM_PARSERS) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    /* one device but two mtsif configs. this is a special case that requires setting the pointers to the correct config for this frontend handle */
    if (userParams.pParam2) { /* TODO: use a nexus_frontend_mtsif_priv.h macro to handle this case */
        if (frontend->pGenericDeviceHandle->chainedConfig==NULL) {
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }
        if (frontend->pGenericDeviceHandle->chainedConfig->mxt==NULL) {
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }

        /* swap configs */
        pChainedConfig = pConfig;
        pConfig = frontend->pGenericDeviceHandle->chainedConfig;
        mxt = pConfig->mxt;

        /* HAB/RPC-chained, but not MTSIF-chained */
        if (!pConfig->slave && !pChainedConfig->slave) {
            pChainedConfig = NULL;
        }
    }

    /* 1. find all host PBs whose MTSIF source is equal to this frontend handle. we must map those PBs to this frontend */
    if (enabled) {
        NEXUS_Module_Lock(g_NEXUS_frontendModuleSettings.transport);
        NEXUS_ParserBand_GetMtsifConnections_priv(frontend->connector, mtsifConnections, MAX_PB_PER_MTSIF_FRONTEND, &num);
        NEXUS_Module_Unlock(g_NEXUS_frontendModuleSettings.transport);
    }
    else {
        num = 0;
    }

    for (i=0; i<num; i++) {
        unsigned hostPb = mtsifConnections[i].index;
        const NEXUS_ParserBandSettings *pSettings = &mtsifConnections[i].settings;

        /* 2. check all current connections for existing connections or conflicts */
        for (demodPb=0; demodPb<pConfig->numDemodPb; demodPb++) {
            if (pConfig->demodPbSettings[demodPb].enabled) {
                if (pConfig->demodPbSettings[demodPb].virtualNum==hostPb) {
                    if (pConfig->demodPbSettings[demodPb].inputBandNum==demodIb) {
                        /* specified FE-> PB connection already exists due to tuning again without untune */
                        BDBG_MSG(("MTSIF exist:      IB%u -> PB%u -> PB%u (%p:%p)", demodIb, demodPb, hostPb, (void *)frontend, (void *)mxt));
                        break;
                    }
                    else {
                        /* mark the existing connection as NULL so we override it below
                           typical example: FE0 -> PB0 then FE1 -> PB0. FE0 is still tuned, but data not routed anywhere */
                        pConfig->demodPbSettings[demodPb].enabled = false;
                        pConfig->demodPbSettings[demodPb].connector = NULL;
                        BDBG_MSG(("MTSIF override:   IB%u -> PB%u -> PB%u (%p:%p)", pConfig->demodPbSettings[demodPb].inputBandNum, demodPb, hostPb, (void *)frontend, (void *)mxt));
                    }
                }
            }
        }

        if (demodPb != pConfig->numDemodPb) { /* connection already exists. skip over picking a free demod PB */
            goto apply_settings;
        }

        /* 3. pick a free demod PB */
        for (demodPb=0; demodPb<pConfig->numDemodPb; demodPb++) {
            /* coverity [dead_error_condition:false] */
            if (pConfig->demodPbSettings[demodPb].enabled==false) {
                BMXT_ParserConfig parserConfig;
                BMXT_ParserBandMapping bandMap;

                /* if chained, then the chainedConfig adds more restrictions to which PB we can pick */
                if (pChainedConfig) {
                    unsigned k;
                    if (!pConfig->slave) {
                        /* master has to avoid physical PBs that are in use by slave as virtual PBs.
                           could also do this by adding .enabledPassthrough to demodPbSettings instead */
                        for (k=0; k<pChainedConfig->numDemodPb; k++) {
                            if (pChainedConfig->demodPbSettings[k].enabled &&
                                pChainedConfig->demodPbSettings[k].virtualNum==demodPb)
                            {
                                goto next;
                            }
                        }
                    }
                    else {
                        /* slave has to check if virtual PB is in use by master as physical PB and error out */
                        if (pChainedConfig->demodPbSettings[hostPb].enabled) {
                            BDBG_ERR(("Unable to route data IB%u -> PB%u -> PB%u -> PB%u because PB%u is in use by chain master", demodIb, demodPb, hostPb, hostPb, hostPb));
                            demodPb = pConfig->numDemodPb; /* no way to get around this for now */
                            break;
                        }
                    }
                }

                BMXT_GetParserConfig(mxt, demodPb, &parserConfig);
                /* 4a. map demod IB->demod PB */
                pConfig->demodPbSettings[demodPb].inputBandNum = parserConfig.InputBandNumber = demodIb;
                pConfig->demodPbSettings[demodPb].enabled = parserConfig.Enable = true;
                pConfig->demodPbSettings[demodPb].connector = frontend->connector;
                if (pChainedConfig && !pConfig->slave) {
                    parserConfig.mtsifTxSelect = 0; /* master's own PBs are always routed through TX0 */
                }
                else {
                    parserConfig.mtsifTxSelect = demodMtsifTx; /* otherwise, honor the TX selection */
                }

                BMXT_SetParserConfig(mxt, demodPb, &parserConfig);

                /* if slave, then we need to change the passthrough routing on master */
                if (pConfig->slave) {
                    BMXT_Handle mxt_master;
                    BDBG_ASSERT(pChainedConfig); /* if slave, then by definition, pChainedConfig is non-NULL */
                    mxt_master = pChainedConfig->mxt;
                    BMXT_GetParserConfig(mxt_master, hostPb, &parserConfig);
                    BDBG_ASSERT(parserConfig.Enable == false);
                    parserConfig.mtsifTxSelect = 1;
                    BMXT_SetParserConfig(mxt_master, hostPb, &parserConfig);
                }

                /* 4b. set virtual PB num of demod PB equal to host PB num */
                BMXT_GetParserMapping(mxt, &bandMap);
                pConfig->demodPbSettings[demodPb].virtualNum = bandMap.FrontEnd[demodPb].VirtualParserBandNum = hostPb;
                BMXT_SetParserMapping(mxt, &bandMap);
                BDBG_MSG(("MTSIF connect:    IB%u -> PB%u -> PB%u (TX%u, %p:%p) %s", demodIb, demodPb, hostPb, parserConfig.mtsifTxSelect,
                    (void *)frontend, (void *)mxt, pChainedConfig?(pConfig->slave?"(chain slave) ":"(chain master)"):""));
                g_NEXUS_Frontend_P_HostMtsifConfig.hostPbSettings[hostPb].demodPb = demodPb;
                g_NEXUS_Frontend_P_HostMtsifConfig.hostPbSettings[hostPb].connected = true;
                break;
            }
next:
            BSTD_UNUSED(0);
        }

apply_settings:
        /* did not find free demod PB */
        if (demodPb>=pConfig->numDemodPb) {
            BDBG_ERR(("No demod PB available to connect: %p:%p IB%u -> PB? -> PB%u", (void *)frontend, (void *)mxt, demodIb, hostPb));
            return BERR_TRACE(NEXUS_NOT_AVAILABLE);
        }

        /* grab the settings of the output PB on the host, and apply them to the demod PB */
        if ( (pConfig->demodPbSettings[demodPb].errorInputIgnore != pSettings->teiIgnoreEnabled) ||
             (pConfig->demodPbSettings[demodPb].timestampMode != (NEXUS_IS_DSS_MODE(pSettings->transportType) ? BMXT_ParserTimestampMode_eBinary : BMXT_ParserTimestampMode_eMod300)) ||
             (pConfig->demodPbSettings[demodPb].dssMode != NEXUS_IS_DSS_MODE(pSettings->transportType)) )
        {
            BMXT_ParserConfig mxtParserConfig;
            BDBG_MSG(("MTSIF PB settings: demod PB%u <- host PB%u (DSS %u)", demodPb, hostPb, NEXUS_IS_DSS_MODE(pSettings->transportType)));
            BMXT_GetParserConfig(mxt, demodPb, &mxtParserConfig);
            /* translate NEXUS_ParserBandSettings to BMXT_ParserConfig.
               note the ParserBandSettings have already been applied to host; we'll just apply them to the demod as well.
               some may be needed, some not. */
            mxtParserConfig.ErrorInputIgnore = pSettings->teiIgnoreEnabled;
            mxtParserConfig.TsMode = NEXUS_IS_DSS_MODE(pSettings->transportType) ? BMXT_ParserTimestampMode_eBinary : BMXT_ParserTimestampMode_eMod300;
            mxtParserConfig.DssMode = NEXUS_IS_DSS_MODE(pSettings->transportType);
            /* ignore allPass and acceptNullPackets for demod. demod is always in allpass + acceptnull mode. */
            BMXT_SetParserConfig(mxt, demodPb, &mxtParserConfig);

            if (pConfig->demodPbSettings[demodPb].dssMode != NEXUS_IS_DSS_MODE(pSettings->transportType)) {
                BMXT_InputBandConfig mxtInputBandConfig;
                BDBG_MSG(("MTSIF IB settings: demod IB%u <- host PB%u (DSS %u)", demodIb, hostPb, NEXUS_IS_DSS_MODE(pSettings->transportType)));
                BMXT_GetInputBandConfig(mxt, demodIb, &mxtInputBandConfig);
                mxtInputBandConfig.DssMode = mxtParserConfig.DssMode;
                BMXT_SetInputBandConfig(mxt, demodIb, &mxtInputBandConfig);
            }

            pConfig->demodPbSettings[demodPb].errorInputIgnore = mxtParserConfig.ErrorInputIgnore;
            pConfig->demodPbSettings[demodPb].timestampMode = mxtParserConfig.TsMode;
            pConfig->demodPbSettings[demodPb].dssMode = mxtParserConfig.DssMode;
        }
        if (pSettings->teiError.callback || pSettings->lengthError.callback) {
            setTimer = true;
        }

#if NEXUS_HAS_TSMF
        if (!mtsifConnections[i].tsmf.valid) continue;
        /* set input: connect this frontend to all (could be more than one) TSMF whose input matches this frontend */
        NEXUS_Frontend_P_SetTsmfInput(mxt, demodIb, mtsifConnections[i].tsmf.hwIndex, &mtsifConnections[i].tsmf.settings);

        /* set output: connect this TSMF to all demod PB that are connected to host PBs whose input is this TSMF (yes, it's complicated...) */
        demodPb = g_NEXUS_Frontend_P_HostMtsifConfig.hostPbSettings[mtsifConnections[i].index].demodPb; /* demod TSMF output is the demod PB, not the host PB */
        NEXUS_Frontend_P_SetTsmfOutput(mxt, demodPb, mtsifConnections[i].tsmf.hwIndex, mtsifConnections[i].tsmf.settings.enabled);
#endif

    }

    /* find any previously-established connections that must now be disconnected. disconnects are only implicitly requested, so we must search */
    for (demodPb=0; demodPb<pConfig->numDemodPb; demodPb++) {

        if (!pConfig->demodPbSettings[demodPb].enabled) { continue; }
        if (pConfig->demodPbSettings[demodPb].connector != frontend->connector) {
            continue; /* if PB is being fed by a different frontend, then the new connection has already been established, so no need to disconnect the old one */
        }

        for (i=0; i<num; i++) {
            if (mtsifConnections[i].index == pConfig->demodPbSettings[demodPb].virtualNum) { break; } /* connection is still valid */
        }
        if (i == num) {
            /* connection no longer valid, so disconnect */
            BMXT_ParserConfig parserConfig;
            unsigned hostPb = pConfig->demodPbSettings[demodPb].virtualNum;
            BMXT_GetParserConfig(mxt, demodPb, &parserConfig);
            pConfig->demodPbSettings[demodPb].enabled = parserConfig.Enable = false;
            pConfig->demodPbSettings[demodPb].connector = NULL;
            BMXT_SetParserConfig(mxt, demodPb, &parserConfig);
            BDBG_MSG(("MTSIF disconnect: IB%u -> PB%u -> PB%u (%p:%p)", demodIb, demodPb, hostPb, (void *)frontend, (void *)mxt));
            g_NEXUS_Frontend_P_HostMtsifConfig.hostPbSettings[hostPb].demodPb = 0;
            g_NEXUS_Frontend_P_HostMtsifConfig.hostPbSettings[hostPb].connected = false;
#if NEXUS_HAS_TSMF
            /* disable/reset the parser's TSMF config. otherwise, a new frontend -> host_PB connection may inadvertently get TSMF-filtered */
            BMXT_TSMF_SetParserConfig(mxt, demodPb, 0 /* don't care */, false);
#endif
        }
    }

    if (setTimer) {
        if (!pConfig->timer) {
            pConfig->timer = NEXUS_ScheduleTimer(1000, NEXUS_Frontend_P_MxtTimer, pConfig);
        }
    }
    else {
        if (pConfig->timer) {
            NEXUS_CancelTimer(pConfig->timer);
            pConfig->timer = NULL;
        }
    }

#if 0 /* enable for more debug messages */
    NEXUS_Frontend_P_DumpMtsifConfig(pConfig);
    if (pChainedConfig) {
        NEXUS_Frontend_P_DumpMtsifConfig(pChainedConfig);
    }
#endif

    return rc;
}


NEXUS_Error NEXUS_Frontend_P_SetMtsifConfig(NEXUS_FrontendHandle frontend)
{
    NEXUS_Error rc;
    BDBG_OBJECT_ASSERT(frontend, NEXUS_Frontend);
    rc = NEXUS_Frontend_P_ConfigMtsifConfig(frontend, true);
    if (rc) return BERR_TRACE(rc);
    return 0;
}

void NEXUS_Frontend_P_UnsetMtsifConfig(NEXUS_FrontendHandle frontend)
{

    BDBG_OBJECT_ASSERT(frontend, NEXUS_Frontend);
    (void)NEXUS_Frontend_P_ConfigMtsifConfig(frontend, false);
}
#else
NEXUS_Error NEXUS_Frontend_P_InitMtsifConfig(NEXUS_FrontendDeviceMtsifConfig *pConfig, const void *mxtSettings)
{
    BSTD_UNUSED(pConfig);
    BSTD_UNUSED(mxtSettings);
    return 0;
}

NEXUS_Error NEXUS_Frontend_P_SetMtsifConfig(NEXUS_FrontendHandle frontend)
{
    BDBG_OBJECT_ASSERT(frontend, NEXUS_Frontend);
    return 0;
}

void NEXUS_Frontend_P_UnsetMtsifConfig(NEXUS_FrontendHandle frontend)
{
    BDBG_OBJECT_ASSERT(frontend, NEXUS_Frontend);
}

#endif

void NEXUS_Frontend_P_PostDumpMtsifConfig(void *context)
{
#if NEXUS_HAS_MXT
    unsigned i;
    NEXUS_FrontendDeviceMtsifConfig *pConfig = context;
    BMXT_Handle mxt = pConfig->mxt;
    BMXT_InputBandStatus ibStatus;
    BDBG_MSG(("PostDumpMtsifConfig %p (%s)", (void *)mxt, pConfig->slave?"slave":"master"));

    /* IB sync count */
    for (i=0; i<2; i++) {
        BMXT_GetInputBandStatus(mxt, &ibStatus);
        BDBG_MSG(("demod IB: %02x %02x %02x %02x   %02x %02x %02x %02x   %02x %02x %02x %02x   %02x %02x %02x %02x",
            ibStatus.syncCount[0], ibStatus.syncCount[1], ibStatus.syncCount[2], ibStatus.syncCount[3],
            ibStatus.syncCount[4], ibStatus.syncCount[5], ibStatus.syncCount[6], ibStatus.syncCount[7],
            ibStatus.syncCount[8], ibStatus.syncCount[9], ibStatus.syncCount[10], ibStatus.syncCount[11],
            ibStatus.syncCount[12], ibStatus.syncCount[13], ibStatus.syncCount[14], ibStatus.syncCount[15]));
    }
#else
    BSTD_UNUSED(context);
#endif
}

/* debug function to dump all known info */
void NEXUS_Frontend_P_DumpMtsifConfig(const NEXUS_FrontendDeviceMtsifConfig *pConfig)
{
#if NEXUS_HAS_MXT && NEXUS_NUM_PARSER_BANDS
    BMXT_Handle mxt = pConfig->mxt;
    BMXT_ParserConfig parserConfig;
    BMXT_ParserBandMapping parserMapping;
    unsigned i;
    const unsigned numDemodPb = pConfig->numDemodPb>16 ? 16:pConfig->numDemodPb;

    BDBG_ASSERT(pConfig);
    BDBG_ASSERT(mxt);

    BDBG_MSG(("DumpMtsifConfig %p (%s)", (void *)mxt, pConfig->slave?"slave":"master"));
    /* print out real and cached demod PB settings */
    for (i=0; i<numDemodPb; i++) {
        BMXT_GetParserConfig(mxt, i, &parserConfig);
        BMXT_GetParserMapping(mxt, &parserMapping);
        BDBG_MSG(("demod PB[%2u]: en %u:%u, IB %2u:%2u, virt %2u:%2u, TX %u", i,
            parserConfig.Enable, pConfig->demodPbSettings[i].enabled,
            parserConfig.InputBandNumber, pConfig->demodPbSettings[i].inputBandNum,
            parserMapping.FrontEnd[i].VirtualParserBandNum, pConfig->demodPbSettings[i].virtualNum,
            parserConfig.mtsifTxSelect));
    }

#if 0
    /* print out host PB settings */
    {
        struct NEXUS_MtsifParserBandSettings mtsifConnections[MAX_PB_PER_MTSIF_FRONTEND];
        unsigned i, num;

        NEXUS_Module_Lock(g_NEXUS_frontendModuleSettings.transport);
        NEXUS_ParserBand_GetMtsifConnections_priv(frontend->connector, mtsifConnections, MAX_PB_PER_MTSIF_FRONTEND, &num);
        NEXUS_Module_Unlock(g_NEXUS_frontendModuleSettings.transport);

        for (i=0; i<num; i++) {
            BDBG_MSG(("host PB[%2u]: %u:%#lx", mtsifConnections[i].index, mtsifConnections[i].settings.sourceType,
                mtsifConnections[i].settings.sourceType==NEXUS_ParserBandSourceType_eMtsif ? mtsifConnections[i].settings.sourceTypeSettings.mtsif : NULL));
        }
    }
#endif

    /* print MTSIF status */
    {
        BMXT_MtsifStatus mtsifStatus;
        BMXT_GetMtsifStatus(mxt, &mtsifStatus);
        for (i=0; i<2; i++) {
            if (mtsifStatus.tx[i].enabled) {
                BDBG_MSG(("MTSIF_TX[%u]: enabled, polarity %u, ifWidth %u", i, mtsifStatus.tx[i].clockPolarity, mtsifStatus.tx[i].interfaceWidth));
            }
        }
        for (i=0; i<1; i++) {
            if (mtsifStatus.rx[i].enabled) {
                BDBG_MSG(("MTSIF_RX[%u]: enabled, polarity %u, ifWidth %u", i, mtsifStatus.rx[i].clockPolarity, mtsifStatus.rx[i].interfaceWidth));
            }
        }
    }

    #if 0
    /* print the IB sync count later since it's only useful after the Tune */
    NEXUS_ScheduleTimer(4000, NEXUS_Frontend_P_PostDumpMtsifConfig, pConfig);
    #endif
#else
    BSTD_UNUSED(pConfig);
#endif
}

#endif /* #if defined(NEXUS_FRONTEND_45216)  */

NEXUS_Error NEXUS_Frontend_GetBertStatus(NEXUS_FrontendHandle handle, NEXUS_FrontendBertStatus *pStatus)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);

    if(NEXUS_Frontend_P_CheckDeviceOpen(handle)){
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }

    if ( NULL == handle->getBertStatus )
    {
        if ( handle->pParentFrontend )
        {
            return NEXUS_Frontend_GetBertStatus(handle->pParentFrontend, pStatus);
        }
        else
        {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else
    {
        return handle->getBertStatus(handle->pDeviceHandle, pStatus);
    }
}


void NEXUS_Frontend_GetDefaultOpenSettings(NEXUS_FrontendChannelSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

NEXUS_FrontendHandle NEXUS_Frontend_Open(const NEXUS_FrontendChannelSettings *pSettings)
{
	BSTD_UNUSED(pSettings);
    return NULL;
}


