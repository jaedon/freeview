/***************************************************************************
*     (c)2004-2009 Broadcom Corporation
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
* $brcm_Workfile: nexus_tuner_3420.c $
* $brcm_Revision: 4 $
* $brcm_Date: 1/23/09 3:32p $
*
* API Description:
*   API name: Frontend 3420
*    APIs to open, close, and setup initial settings for a BCM3420
*    Tuner Device.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/frontend/3420/src/nexus_tuner_3420.c $
* 
* 4   1/23/09 3:32p jgarrett
* PR 51457: Adding frontend extension hooks for tuners and
*  frontends/demods
* 
* 3   3/4/08 9:48a jgarrett
* PR 39984: Adding 3420 support
* 
* 2   2/7/08 5:36p jgarrett
* PR 39407: Converting to common close functions
* 
* 1   1/18/08 2:19p jgarrett
* PR 38808: Merging to main branch
* 
* Nexus_Devel/1   10/10/07 11:29a jgarrett
* PR 35551: Adding initial version
* 
***************************************************************************/
#include "nexus_frontend_module.h"
#include "nexus_i2c.h"
#include "btnr_3420.h"
#include "priv/nexus_i2c_priv.h"

BDBG_MODULE(nexus_tuner_3420);

/***************************************************************************
Summary:
	Get the default settings for a BCM3420 tuner
See Also:
    NEXUS_Tuner_Open3420
 ***************************************************************************/
void NEXUS_Tuner_GetDefault3420Settings(
    NEXUS_3420Settings *pSettings   /* [out] */
    )
{
    BTNR_3420_Settings tnrSettings;

    BDBG_ASSERT(NULL != pSettings);

    BTNR_3420_GetDefaultSettings(&tnrSettings, NULL);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->enableAgcGain = tnrSettings.enableAgcGain;
    pSettings->i2cAddr = tnrSettings.i2cAddr;
    pSettings->ifFrequency = tnrSettings.ifFreq;
    pSettings->xtalFrequency = tnrSettings.xtalFreq;
}

/***************************************************************************
Summary:
	Open a handle to a BCM3420 device.
See Also:
    NEXUS_Tuner_Close3420
 ***************************************************************************/
NEXUS_TunerHandle NEXUS_Tuner_Open3420(
    const NEXUS_3420Settings *pSettings
    )
{
    BREG_I2C_Handle i2cHandle;
    BTNR_3420_Settings tnrSettings;
    NEXUS_Tuner *pTuner;
    NEXUS_Error errCode;
    BTNR_Handle tnrHandle;

    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(NULL != pSettings->i2cDevice);

    BTNR_3420_GetDefaultSettings(&tnrSettings, NULL);
    tnrSettings.enableAgcGain = pSettings->enableAgcGain;
    tnrSettings.i2cAddr = pSettings->i2cAddr;
    tnrSettings.ifFreq = pSettings->ifFrequency;
    tnrSettings.xtalFreq = pSettings->xtalFrequency;

    i2cHandle = NEXUS_I2c_GetRegHandle(pSettings->i2cDevice, NEXUS_MODULE_SELF);
    if ( NULL == i2cHandle )
    {
        errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
        return NULL;
    }

    errCode = BTNR_3420_Open(&tnrHandle,
                             NULL,
                             i2cHandle,
                             &tnrSettings);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        return NULL;
    }

    pTuner = NEXUS_Tuner_P_CreateFromBTNR(tnrHandle);
    if ( NULL == pTuner )
    {
        errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }

    pTuner->ifFrequency = tnrSettings.ifFreq;

    return pTuner;
}
