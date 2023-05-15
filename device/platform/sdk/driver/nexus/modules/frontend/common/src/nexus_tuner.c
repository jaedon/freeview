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
* $brcm_Workfile: nexus_tuner.c $
* $brcm_Revision: 7 $
* $brcm_Date: 6/19/12 1:27p $
*
* API Description:
*   API name: Frontend Tuner
*    Generic APIs for tuner device control
*
* Revision History:
*
* $brcm_Log: /nexus/modules/frontend/common/src/nexus_tuner.c $
* 
* 7   6/19/12 1:27p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* 6   7/20/11 11:00p shyi
* SWDTV-7988: Fixed the size of memory to be cleared
* 
* 5   3/4/11 3:43p shyi
* SWDTV-5407: Added the linkage between reading RF input power from a
*  supported tuner and passing the read value to IFD
* 
* 4   8/26/10 12:00p jgarrett
* SW35230-900: Merging new tuner interfaces to main branch
* 
* SW35230-900/3   8/24/10 12:36p shyi
* SW35230-900: Added handling when NEXUS tuner handle is null, cleaned up
*  35230 specific settings and rectified a typo in common tuner driver
* 
* SW35230-900/2   8/20/10 2:09p shyi
* SW35230-900: Allowing only NEXUS I2C handle when creating new NEXUS
*  tuners, removing obsolete NEXUS tuner SetFrequency API and advertizing
*  new API tune
* 
* SW35230-900/1   8/12/10 8:24p shyi
* SW35230-900: Taking TFE tuner changes for I2C read and firmware
*  download capabilities
* 
* 3   1/23/09 7:25p jgarrett
* PR 51457: Frontend extension hooks for tuners and frontends/demods
* 
* 2   2/7/08 5:36p jgarrett
* PR 39407: Converting to common close functions
* 
* 1   1/18/08 2:19p jgarrett
* PR 38808: Merging to main branch
* 
* Nexus_Devel/2   10/10/07 11:29a jgarrett
* PR 35551: Adding debug
* 
* Nexus_Devel/1   10/5/07 5:52p jgarrett
* PR 35551: Adding initial version
* 
***************************************************************************/

#include "nexus_frontend_module.h"

BDBG_MODULE(nexus_tuner);

/***************************************************************************
Summary:
	Set the frequency of a tuner device
 ***************************************************************************/
NEXUS_Error NEXUS_Tuner_SetFrequency(
    NEXUS_TunerHandle handle,
    NEXUS_TunerMode mode,               /* Tuner Mode */
    unsigned frequency                  /* In Hz */
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Tuner);
    if ( handle->setFrequency )
    {
        BDBG_MSG(("Setting tuner frequency to %d, mode %d", frequency, mode));
        return handle->setFrequency(handle->pDeviceHandle, mode, frequency);
    }
    else
    {
        if ( NULL != handle->tune )
        {
            BDBG_ERR(("Please use NEXUS_Tuner_Tune() instead of "
                "NEXUS_Tuner_SetFrequency() for this tuner device."));
        }
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

/***************************************************************************
Summary:
    Close a tuner handle
***************************************************************************/
static void NEXUS_Tuner_P_Finalizer(
    NEXUS_TunerHandle handle
    )
{
    NEXUS_OBJECT_ASSERT(NEXUS_Tuner, handle);
    BDBG_ASSERT(handle->close != NULL);
    handle->close(handle->pDeviceHandle);
    NEXUS_OBJECT_DESTROY(NEXUS_Tuner, handle);
    BKNI_Free(handle);
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_Tuner, NEXUS_Tuner_Close);

/***************************************************************************
 * Generic method to create a nexus tuner.  It will be automatically
 * destroyed when NEXUS_Tuner_Close is called.
 ***************************************************************************/
NEXUS_TunerHandle NEXUS_Tuner_P_Create(
    void *pDeviceHandle
    )
{
    NEXUS_Tuner *pTuner;

    pTuner = BKNI_Malloc(sizeof(NEXUS_Tuner));
    if ( NULL == pTuner )
    {
        (void)BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }

    NEXUS_OBJECT_INIT(NEXUS_Tuner, pTuner);
    pTuner->pDeviceHandle = pDeviceHandle;

    return pTuner;
}

/***************************************************************************
Summary:
    Get default settings for a frontend extension
****************************************************************************/
void NEXUS_Tuner_P_GetDefaultExtensionSettings(
    NEXUS_TunerExtensionSettings *pSettings          /* [out] */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(NEXUS_TunerExtensionSettings));
}

/***************************************************************************
Summary:
    Create a nexus tuner handle from an extension

Description:
    This interface allows a custom platform to easily integrate tuner 
    functions for a tuner into the nexus framework.  Using this, the standard
    nexus frontend routines can control both your tuner and demodulator if 
    desired.
****************************************************************************/
NEXUS_TunerHandle NEXUS_Tuner_P_CreateExtension(
    const NEXUS_TunerExtensionSettings *pSettings
    )
{
    NEXUS_Tuner *pTuner;

    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(NULL != pSettings->close);
    BDBG_ASSERT(NULL != pSettings->setFrequency);

    pTuner = NEXUS_Tuner_P_Create(pSettings->pDeviceHandle);
    if ( pTuner )
    {
        pTuner->ifFrequency = pSettings->ifFrequency;
        pTuner->close = pSettings->close;
        pTuner->setFrequency = pSettings->setFrequency;
    }

    return pTuner;
}

void NEXUS_Tuner_Init(
    NEXUS_TunerHandle tuner
    )
{
    BDBG_OBJECT_ASSERT(tuner, NEXUS_Tuner);
    if ( NULL != tuner->init )
    {
        tuner->init(tuner->pDeviceHandle);
    }
    else
    {
        (void)BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

void NEXUS_Tuner_GetStatus(
    NEXUS_TunerHandle tuner,
    NEXUS_TunerStatus *pStatus  /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(tuner, NEXUS_Tuner);
    BDBG_ASSERT(NULL != pStatus);
    if ( tuner->getStatus )
    {
        tuner->getStatus(tuner->pDeviceHandle, pStatus);
    }
    else
    {
        BKNI_Memset(pStatus, 0, sizeof(*pStatus));
        (void)BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

void NEXUS_Tuner_GetDefaultTuneSettings(
    NEXUS_TunerMode mode,
    NEXUS_TunerTuneSettings *pSettings  /* [out] */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->mode = mode;
    /* TODO: If specifics are required per-mode, set them here */
    /* TODO: If each tuner really needs its own GetDefaultTuneSettings, we need a handle passed into this function also. */
}

void *NEXUS_Tuner_P_GetAgcScript(NEXUS_TunerHandle tuner)
{
    BDBG_OBJECT_ASSERT(tuner, NEXUS_Tuner);
    if ( tuner->getAgcScript )
    {
        return tuner->getAgcScript(tuner->pDeviceHandle);
    }
    else
    {
        return NULL;
    }
}

NEXUS_Error NEXUS_Tuner_Tune(
    NEXUS_TunerHandle tuner,
    const NEXUS_TunerTuneSettings *pSettings
    )
{
    BDBG_OBJECT_ASSERT(tuner, NEXUS_Tuner);
    BDBG_ASSERT(NULL != pSettings);
    if ( tuner->tune )
    {
        return tuner->tune(tuner->pDeviceHandle, pSettings);
    }
    else if ( tuner->setFrequency )
    {
        return tuner->setFrequency(tuner->pDeviceHandle, pSettings->mode, pSettings->frequency);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

void NEXUS_Tuner_GetSettings(
    NEXUS_TunerHandle tuner,
    NEXUS_TunerSettings *pSettings  /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(tuner, NEXUS_Tuner);
    BDBG_ASSERT(NULL != pSettings);
    if ( tuner->getSettings )
    {
        tuner->getSettings(tuner->pDeviceHandle, pSettings);
    }
    else
    {
        (void)BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

NEXUS_Error NEXUS_Tuner_SetSettings(
    NEXUS_TunerHandle tuner,
    const NEXUS_TunerSettings *pSettings
    )
{
    BDBG_OBJECT_ASSERT(tuner, NEXUS_Tuner);
    BDBG_ASSERT(NULL != pSettings);
    if ( tuner->setSettings )
    {
        return tuner->setSettings(tuner->pDeviceHandle, pSettings);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

void NEXUS_Tuner_GetAttributes(
    NEXUS_TunerHandle tuner,
    NEXUS_TunerAttributes *pAttributes /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(tuner, NEXUS_Tuner);
    BDBG_ASSERT(NULL != pAttributes);
    if ( tuner->getAttributes )
    {
        tuner->getAttributes(tuner->pDeviceHandle, pAttributes);
    }
    else
    {
        BKNI_Memset(pAttributes, 0, sizeof(*pAttributes));
        (void)BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

NEXUS_Error NEXUS_Tuner_ReadPowerLevel(
    NEXUS_TunerHandle tuner,
    int *pPowerLevel
    )
{
    BDBG_OBJECT_ASSERT(tuner, NEXUS_Tuner);
    BDBG_ASSERT(NULL != pPowerLevel);
    if ( tuner->readPowerLevel )
    {
        return tuner->readPowerLevel(tuner->pDeviceHandle, pPowerLevel);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

#if NEXUS_TUNER_SUPPORT
static NEXUS_Error NEXUS_Tuner_P_SetFrequencyTNR(
    void *handle, 
    NEXUS_TunerMode mode, 
    unsigned frequency
    )
{
    BTNR_Handle tnrHandle = handle;
    return BTNR_SetTunerRfFreq(tnrHandle,
                               (uint32_t)frequency,
                               (BTNR_TunerMode) mode);
}

static void NEXUS_Tuner_P_CloseTNR(
    void *handle
    )
{
    BTNR_Handle tnrHandle = handle;
    BTNR_Close(tnrHandle);
}

/***************************************************************************
 * Method to create a tuner from a BTNR handle
 ***************************************************************************/
NEXUS_Tuner *NEXUS_Tuner_P_CreateFromBTNR(
    BTNR_Handle tnrHandle
    )
{
    NEXUS_Tuner *pTuner;

    pTuner = NEXUS_Tuner_P_Create(tnrHandle);
    if ( pTuner )
    {
        pTuner->pDeviceHandle = tnrHandle;
        pTuner->close = NEXUS_Tuner_P_CloseTNR;
        pTuner->setFrequency = NEXUS_Tuner_P_SetFrequencyTNR;
    }
    return pTuner;
}
#endif

