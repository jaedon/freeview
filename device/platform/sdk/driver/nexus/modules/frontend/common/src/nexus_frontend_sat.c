/******************************************************************************
 *    (c)2008-2014 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: $
 * $brcm_Revision: $
 * $brcm_Date: $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: $
 *
 *****************************************************************************/
#include "nexus_frontend_module.h"
#include "nexus_frontend_sat.h"

BDBG_MODULE(nexus_frontend_sat);
BDBG_OBJECT_ID(NEXUS_SatDevice);
BDBG_OBJECT_ID(NEXUS_SatChannel);

BTRC_MODULE_DECLARE(ChnChange_Tune);
BTRC_MODULE_DECLARE(ChnChange_TuneLock);

/* Setup/teardown */
static void NEXUS_Frontend_P_Sat_Close(NEXUS_FrontendHandle handle);

/* Tuning */
NEXUS_Error NEXUS_Frontend_P_Sat_TuneSatellite(void *handle, const NEXUS_FrontendSatelliteSettings *pSettings);
void NEXUS_Frontend_P_Sat_Untune(void *handle);

/* Status */
static NEXUS_Error NEXUS_Frontend_P_Sat_GetSatelliteStatus(void *handle, NEXUS_FrontendSatelliteStatus *pStatus);
static void NEXUS_Frontend_P_Sat_ResetStatus(void *handle);
static NEXUS_Error NEXUS_Frontend_P_Sat_GetFastStatus(void *handle, NEXUS_FrontendFastStatus *pStatus);
static void NEXUS_Frontend_P_Sat_GetType(void *handle, NEXUS_FrontendType *type);
static NEXUS_Error NEXUS_Frontend_P_Sat_GetBertStatus(void *handle, NEXUS_FrontendBertStatus *pStatus);
static NEXUS_Error NEXUS_Frontend_P_Sat_ReadSoftDecisions(void *handle, NEXUS_FrontendSoftDecision *pDecisions, size_t length, size_t *pNumRead);
static NEXUS_Error NEXUS_Frontend_P_Sat_ReadSatelliteConfig(void *handle, unsigned id, void *buffer, unsigned bufferSize);
static NEXUS_Error NEXUS_Frontend_P_Sat_WriteSatelliteConfig(void *handle, unsigned id, const void *buffer, unsigned bufferSize);

/* Diseqc functionality */
static void NEXUS_Frontend_P_Sat_GetDiseqcSettings(void *handle, NEXUS_FrontendDiseqcSettings *pSettings);
static NEXUS_Error NEXUS_Frontend_P_Sat_SetDiseqcSettings(void *handle, const NEXUS_FrontendDiseqcSettings *pSettings);
static NEXUS_Error NEXUS_Frontend_P_Sat_GetDiseqcStatus(void *handle, NEXUS_FrontendDiseqcStatus *pStatus);
static NEXUS_Error NEXUS_Frontend_P_Sat_SendDiseqcMessage(void *handle, const uint8_t *pSendData, size_t sendDataSize, const NEXUS_CallbackDesc *pSendComplete);
static NEXUS_Error NEXUS_Frontend_P_Sat_GetDiseqcReply(void *handle, NEXUS_FrontendDiseqcMessageStatus *pStatus, uint8_t *pReplyBuffer, size_t replyBufferSize, size_t *pReplyLength);
static NEXUS_Error NEXUS_Frontend_P_Sat_ResetDiseqc(void *handle, uint8_t options);
static NEXUS_Error NEXUS_Frontend_P_Sat_SendDiseqcAcw(void *handle, uint8_t codeWord);

/* Extension support */
static NEXUS_Error NEXUS_Frontend_P_Sat_RegisterExtension(NEXUS_FrontendHandle parentHandle, NEXUS_FrontendHandle extensionHandle);

/* Event handlers */
static void NEXUS_Frontend_P_Sat_LockEventHandler(void *pParam);

/* Spectrum Analyzer */
static NEXUS_Error NEXUS_Frontend_P_Sat_RequestSpectrumData(void *handle, const NEXUS_FrontendSpectrumSettings *pSettings);
static void NEXUS_Frontend_P_Sat_SpectrumEventCallback(void *pParam);
static void NEXUS_Frontend_P_Sat_SpectrumDataReadyCallback(void *pParam);

/* Peak scan */
static void NEXUS_Frontend_P_Sat_PeakscanEventHandler(void *pParam);
static NEXUS_Error NEXUS_Frontend_P_Sat_SatellitePeakscan( void *handle, const NEXUS_FrontendSatellitePeakscanSettings *pSettings );
static NEXUS_Error NEXUS_Frontend_P_Sat_GetSatellitePeakscanResult( void *handle, NEXUS_FrontendSatellitePeakscanResult *pResult );
static NEXUS_Error NEXUS_Frontend_P_Sat_SatelliteToneSearch( void *handle, const NEXUS_FrontendSatelliteToneSearchSettings *pSettings );
static NEXUS_Error NEXUS_Frontend_P_Sat_GetSatelliteToneSearchResult( void *handle, NEXUS_FrontendSatelliteToneSearchResult *pResult );
static NEXUS_Error NEXUS_Frontend_P_Sat_GetSignalDetectStatus (void *handle, NEXUS_FrontendSatelliteSignalDetectStatus *pStatus);

/***************************************************************************
NEXUS -> PI Conversion Routines
***************************************************************************/
typedef struct NEXUS_SatModeEntry
{
    const NEXUS_FrontendSatelliteCodeRate *pCodeRate;
    BSAT_Mode mode;
} NEXUS_SatModeEntry;

static const NEXUS_FrontendSatelliteCodeRate
    g_cr_scan = {0,0,0},
    g_cr_1_2 = {1,2,0},
    g_cr_1_3 = {1,3,0},
    g_cr_1_4 = {1,4,0},
    g_cr_2_3 = {2,3,0},
    g_cr_2_5 = {2,5,0},
    g_cr_3_4 = {3,4,0},
    g_cr_5_6 = {5,6,0},
    g_cr_6_7 = {6,7,0},
    g_cr_7_8 = {7,8,0},
    g_cr_5_11 = {5,11,0},
    g_cr_3_5 = {3,5,0},
    g_cr_4_5 = {4,5,0},
    g_cr_9_10 = {9,10,0},
    g_cr_8_9 = {8,9,0};

static const struct NEXUS_SatModeEntry
g_sds_dvb_modes[] = {
    {&g_cr_1_2, BSAT_Mode_eDvbs_1_2},
    {&g_cr_2_3, BSAT_Mode_eDvbs_2_3},
    {&g_cr_3_4, BSAT_Mode_eDvbs_3_4},
    {&g_cr_5_6, BSAT_Mode_eDvbs_5_6},
    {&g_cr_7_8, BSAT_Mode_eDvbs_7_8},
    {NULL, BSAT_Mode_eDvbs_scan}
},
g_sds_dss_modes[] = {
    {&g_cr_1_2, BSAT_Mode_eDss_1_2},
    {&g_cr_2_3, BSAT_Mode_eDss_2_3},
    {&g_cr_6_7, BSAT_Mode_eDss_6_7},
    {NULL, BSAT_Mode_eDss_scan}
},
g_sds_dcii_modes[] = {
    {&g_cr_1_2, BSAT_Mode_eDcii_1_2},
    {&g_cr_2_3, BSAT_Mode_eDcii_2_3},
    {&g_cr_3_5, BSAT_Mode_eDcii_3_5},
    {&g_cr_3_4, BSAT_Mode_eDcii_3_4},
    {&g_cr_4_5, BSAT_Mode_eDcii_4_5},
    {&g_cr_5_6, BSAT_Mode_eDcii_5_6},
    {&g_cr_7_8, BSAT_Mode_eDcii_7_8},
    {&g_cr_5_11, BSAT_Mode_eDcii_5_11},
    {NULL, BSAT_Mode_eDcii_scan}
},
g_sds_qpsk_turbo_modes[] = {
    {&g_cr_1_2, BSAT_Mode_eTurbo_Qpsk_1_2},
    {&g_cr_2_3, BSAT_Mode_eTurbo_Qpsk_2_3},
    {&g_cr_3_4, BSAT_Mode_eTurbo_Qpsk_3_4},
    {&g_cr_5_6, BSAT_Mode_eTurbo_Qpsk_5_6},
    {&g_cr_7_8, BSAT_Mode_eTurbo_Qpsk_7_8},
    {NULL, BSAT_Mode_eTurbo_scan}
},
g_sds_8psk_turbo_modes[] = {
    {&g_cr_2_3, BSAT_Mode_eTurbo_8psk_2_3},
    {&g_cr_3_4, BSAT_Mode_eTurbo_8psk_3_4},
    {&g_cr_4_5, BSAT_Mode_eTurbo_8psk_4_5},
    {&g_cr_5_6, BSAT_Mode_eTurbo_8psk_5_6},
    {&g_cr_8_9, BSAT_Mode_eTurbo_8psk_8_9},
    {NULL, BSAT_Mode_eTurbo_scan}
},
g_sds_turbo_modes[] = {
    {&g_cr_1_2, BSAT_Mode_eTurbo_Qpsk_1_2},
    {&g_cr_2_3, BSAT_Mode_eTurbo_Qpsk_2_3},
    {&g_cr_3_4, BSAT_Mode_eTurbo_Qpsk_3_4},
    {&g_cr_5_6, BSAT_Mode_eTurbo_Qpsk_5_6},
    {&g_cr_7_8, BSAT_Mode_eTurbo_Qpsk_7_8},
    {&g_cr_2_3, BSAT_Mode_eTurbo_8psk_2_3},
    {&g_cr_3_4, BSAT_Mode_eTurbo_8psk_3_4},
    {&g_cr_4_5, BSAT_Mode_eTurbo_8psk_4_5},
    {&g_cr_5_6, BSAT_Mode_eTurbo_8psk_5_6},
    {&g_cr_8_9, BSAT_Mode_eTurbo_8psk_8_9},
    {NULL, BSAT_Mode_eTurbo_scan}
},
g_sds_8psk_ldpc_modes[] = {
    {&g_cr_3_5, BSAT_Mode_eDvbs2_8psk_3_5},
    {&g_cr_2_3, BSAT_Mode_eDvbs2_8psk_2_3},
    {&g_cr_3_4, BSAT_Mode_eDvbs2_8psk_3_4},
    {&g_cr_5_6, BSAT_Mode_eDvbs2_8psk_5_6},
    {&g_cr_8_9, BSAT_Mode_eDvbs2_8psk_8_9},
    {&g_cr_9_10, BSAT_Mode_eDvbs2_8psk_9_10},
    {NULL, BSAT_Mode_eDvbs2_scan}
},
g_sds_qpsk_ldpc_modes[] = {
    {&g_cr_1_2, BSAT_Mode_eDvbs2_Qpsk_1_2},
    {&g_cr_1_3, BSAT_Mode_eDvbs2_Qpsk_1_3},
    {&g_cr_1_4, BSAT_Mode_eDvbs2_Qpsk_1_4},
    {&g_cr_2_5, BSAT_Mode_eDvbs2_Qpsk_2_5},
    {&g_cr_3_5, BSAT_Mode_eDvbs2_Qpsk_3_5},
    {&g_cr_2_3, BSAT_Mode_eDvbs2_Qpsk_2_3},
    {&g_cr_3_4, BSAT_Mode_eDvbs2_Qpsk_3_4},
    {&g_cr_4_5, BSAT_Mode_eDvbs2_Qpsk_4_5},
    {&g_cr_5_6, BSAT_Mode_eDvbs2_Qpsk_5_6},
    {&g_cr_8_9, BSAT_Mode_eDvbs2_Qpsk_8_9},
    {&g_cr_9_10, BSAT_Mode_eDvbs2_Qpsk_9_10},
    {NULL, BSAT_Mode_eDvbs2_scan}
},
g_sds_16apsk_ldpc_modes[] = {
    {&g_cr_2_3, BSAT_Mode_eDvbs2_16apsk_2_3},
    {&g_cr_3_4, BSAT_Mode_eDvbs2_16apsk_3_4},
    {&g_cr_4_5, BSAT_Mode_eDvbs2_16apsk_4_5},
    {&g_cr_5_6, BSAT_Mode_eDvbs2_16apsk_5_6},
    {&g_cr_8_9, BSAT_Mode_eDvbs2_16apsk_8_9},
    {&g_cr_9_10, BSAT_Mode_eDvbs2_16apsk_9_10},
    {NULL, BSAT_Mode_eDvbs2_scan}
},
g_sds_32apsk_ldpc_modes[] = {
    {&g_cr_3_4, BSAT_Mode_eDvbs2_32apsk_3_4},
    {&g_cr_4_5, BSAT_Mode_eDvbs2_32apsk_4_5},
    {&g_cr_5_6, BSAT_Mode_eDvbs2_32apsk_5_6},
    {&g_cr_8_9, BSAT_Mode_eDvbs2_32apsk_8_9},
    {&g_cr_9_10, BSAT_Mode_eDvbs2_32apsk_9_10},
    {NULL, BSAT_Mode_eDvbs2_scan}
},
g_sds_ldpc_modes[] = {
    {&g_cr_1_2, BSAT_Mode_eDvbs2_Qpsk_1_2},
    {&g_cr_1_3, BSAT_Mode_eDvbs2_Qpsk_1_3},
    {&g_cr_1_4, BSAT_Mode_eDvbs2_Qpsk_1_4},
    {&g_cr_2_5, BSAT_Mode_eDvbs2_Qpsk_2_5},
    {&g_cr_3_5, BSAT_Mode_eDvbs2_Qpsk_3_5},
    {&g_cr_2_3, BSAT_Mode_eDvbs2_Qpsk_2_3},
    {&g_cr_3_4, BSAT_Mode_eDvbs2_Qpsk_3_4},
    {&g_cr_4_5, BSAT_Mode_eDvbs2_Qpsk_4_5},
    {&g_cr_5_6, BSAT_Mode_eDvbs2_Qpsk_5_6},
    {&g_cr_8_9, BSAT_Mode_eDvbs2_Qpsk_8_9},
    {&g_cr_9_10, BSAT_Mode_eDvbs2_Qpsk_9_10},
    {&g_cr_3_5, BSAT_Mode_eDvbs2_8psk_3_5},
    {&g_cr_2_3, BSAT_Mode_eDvbs2_8psk_2_3},
    {&g_cr_3_4, BSAT_Mode_eDvbs2_8psk_3_4},
    {&g_cr_5_6, BSAT_Mode_eDvbs2_8psk_5_6},
    {&g_cr_8_9, BSAT_Mode_eDvbs2_8psk_8_9},
    {&g_cr_9_10, BSAT_Mode_eDvbs2_8psk_9_10},
    {&g_cr_2_3, BSAT_Mode_eDvbs2_16apsk_2_3},
    {&g_cr_3_4, BSAT_Mode_eDvbs2_16apsk_3_4},
    {&g_cr_4_5, BSAT_Mode_eDvbs2_16apsk_4_5},
    {&g_cr_5_6, BSAT_Mode_eDvbs2_16apsk_5_6},
    {&g_cr_8_9, BSAT_Mode_eDvbs2_16apsk_8_9},
    {&g_cr_9_10, BSAT_Mode_eDvbs2_16apsk_9_10},
    {&g_cr_3_4, BSAT_Mode_eDvbs2_32apsk_3_4},
    {&g_cr_4_5, BSAT_Mode_eDvbs2_32apsk_4_5},
    {&g_cr_5_6, BSAT_Mode_eDvbs2_32apsk_5_6},
    {&g_cr_8_9, BSAT_Mode_eDvbs2_32apsk_8_9},
    {&g_cr_9_10, BSAT_Mode_eDvbs2_32apsk_9_10},
    {NULL, BSAT_Mode_eDvbs2_scan}
},
g_blind_acquisition_mode[] = {
    {NULL, BSAT_Mode_eBlindScan}
};

static BSAT_Mode NEXUS_Frontend_P_Sat_GetMode(const struct NEXUS_SatModeEntry *pModes, const NEXUS_FrontendSatelliteCodeRate *pCodeRate)
{
    /* NOTE: there are not non-zero bitsPerSymbol values in the look up tables. if someone specifies a non-zero bitsPerSymbol,
    this algorithm will result in a scan. */
    for ( ;; pModes++ )
    {
        if ( NULL == pModes->pCodeRate )
        {
            BDBG_MSG(("Use scan for coderate %d:%d:%d", pCodeRate->numerator, pCodeRate->denominator, pCodeRate->bitsPerSymbol));
            return pModes->mode;
        }
        if ( pModes->pCodeRate->numerator == pCodeRate->numerator &&
             pModes->pCodeRate->denominator == pCodeRate->denominator &&
             pModes->pCodeRate->bitsPerSymbol == pCodeRate->bitsPerSymbol )
        {
            return pModes->mode;
        }
    }
    return BSAT_Mode_eBlindScan;
}

static void NEXUS_Frontend_P_Sat_EnableSatChannel(NEXUS_SatChannelHandle pSatChannel)
{
    unsigned i;
    bool activated = false;
    if (!pSatChannel->satDevice->satActive[pSatChannel->channel]) {
        BDBG_MSG(("NEXUS_Frontend_P_Sat_EnableSatChannel: enabling %d",pSatChannel->channel));
        BSAT_PowerUpChannel(pSatChannel->satChannel);
        pSatChannel->satDevice->satActive[pSatChannel->channel] = true;
    }
    for (i=0; i < NEXUS_SAT_MAX_CHANNELS; i++) {
        if (i != pSatChannel->channel && pSatChannel->satDevice->satActive[i] && pSatChannel->satDevice->wfeMap[i] == pSatChannel->selectedAdc) {
            activated = true;
        }
    }
    if (!activated && !pSatChannel->satDevice->wfeActive[pSatChannel->selectedAdc]) {
        BDBG_MSG(("NEXUS_Frontend_P_Sat_EnableSatChannel: enabling wfe %d",pSatChannel->selectedAdc));
        BWFE_EnableInput(pSatChannel->satDevice->wfeChannels[pSatChannel->selectedAdc]);
        pSatChannel->satDevice->wfeActive[pSatChannel->selectedAdc] = true;
    }
}

static void NEXUS_Frontend_P_Sat_DisableSatChannel(NEXUS_SatChannelHandle pSatChannel)
{
    unsigned i;
    bool in_use = false;
    if (pSatChannel->satDevice->satActive[pSatChannel->channel]) {
        BDBG_MSG(("NEXUS_Frontend_P_Sat_DisableSatChannel: disabling %d",pSatChannel->channel));
        BSAT_PowerDownChannel(pSatChannel->satChannel);
        pSatChannel->satDevice->satActive[pSatChannel->channel] = false;
    }
    for (i=0; i < NEXUS_SAT_MAX_CHANNELS; i++) {
        if (i != pSatChannel->channel && pSatChannel->satDevice->satActive[i] && pSatChannel->satDevice->wfeMap[i] == pSatChannel->selectedAdc) {
            in_use = true;
        }
    }
    if (!in_use && pSatChannel->satDevice->wfeActive[pSatChannel->selectedAdc]) {
        BDBG_MSG(("NEXUS_Frontend_P_Sat_EnableSatChannel: disabling wfe %d",pSatChannel->selectedAdc));
        BWFE_DisableInput(pSatChannel->satDevice->wfeChannels[pSatChannel->selectedAdc]);
        pSatChannel->satDevice->wfeActive[pSatChannel->selectedAdc] = false;
    }
}

static void NEXUS_Frontend_P_Sat_EnableDsqChannel(NEXUS_SatChannelHandle pSatChannel)
{
    if (pSatChannel->capabilities.diseqc) {
        unsigned i;
        bool activated = false;
        for (i=0; i < NEXUS_SAT_MAX_CHANNELS; i++) {
            if (i != pSatChannel->channel && pSatChannel->satDevice->dsqActive[i] && pSatChannel->satDevice->wfeMap[i] == pSatChannel->diseqcIndex) {
                activated = true;
            }
        }
        if (!activated && !pSatChannel->satDevice->dsqActive[pSatChannel->diseqcIndex]) {
            BDBG_MSG(("NEXUS_Frontend_P_Sat_EnableDsqChannel: enabling %d",pSatChannel->diseqcIndex));
            BDSQ_PowerUpVsense(pSatChannel->satDevice->dsqChannels[pSatChannel->diseqcIndex]);
            BDSQ_PowerUpChannel(pSatChannel->satDevice->dsqChannels[pSatChannel->diseqcIndex]);
            pSatChannel->satDevice->dsqActive[pSatChannel->diseqcIndex] = true;
        }
    }
}

static void NEXUS_Frontend_P_Sat_DisableDsqChannel(NEXUS_SatChannelHandle pSatChannel)
{
    if (pSatChannel->capabilities.diseqc) {
        unsigned i;
        bool in_use = false;
        for (i=0; i < NEXUS_SAT_MAX_CHANNELS; i++) {
            if (i != pSatChannel->channel && pSatChannel->satDevice->dsqActive[i] && pSatChannel->satDevice->wfeMap[i] == pSatChannel->diseqcIndex) {
                in_use = true;
            }
        }
        if (!in_use && pSatChannel->satDevice->dsqActive[pSatChannel->diseqcIndex]) {
            BDBG_MSG(("NEXUS_Frontend_P_Sat_DisableDsqChannel: disabling %d",pSatChannel->diseqcIndex));
            BDSQ_PowerDownChannel(pSatChannel->satDevice->dsqChannels[pSatChannel->diseqcIndex]);
            BDSQ_PowerDownVsense(pSatChannel->satDevice->dsqChannels[pSatChannel->diseqcIndex]);
            pSatChannel->satDevice->dsqActive[pSatChannel->diseqcIndex] = false;
        }
    }
}

void NEXUS_Frontend_P_Sat_GetDefaultDeviceSettings( NEXUS_FrontendSatDeviceSettings *pSettings )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

void NEXUS_Frontend_P_Sat_GetDefaultChannelSettings( NEXUS_FrontendSatChannelSettings *pSettings )
{
    unsigned i;
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));

    /* by default, everything is supported. specific chips should turn off what they don't support. */
    pSettings->capabilities.satellite = true;
    pSettings->capabilities.diseqc = false;
    for (i = 0;i < NEXUS_FrontendSatelliteMode_eMax; i++)
    {
        pSettings->capabilities.satelliteModes[i] = true;
    }
}

NEXUS_SatDevice *NEXUS_Frontend_P_Sat_Create_Device(const NEXUS_FrontendSatDeviceSettings *pSettings)
{
    BERR_Code errCode;
    NEXUS_SatDevice *pSatDevice = NULL;
    bool deviceManagesDiseqc = false;

    BSTD_UNUSED(deviceManagesDiseqc);

    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(NULL != pSettings->satHandle);

    pSatDevice = BKNI_Malloc(sizeof(NEXUS_SatChannel));
    if ( NULL == pSatDevice )
    {
        errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }
    BKNI_Memset(pSatDevice, 0, sizeof(*pSatDevice));

    pSatDevice->settings = *pSettings; /* Save Settings */
    pSatDevice->habHandle = pSettings->habHandle;
    pSatDevice->satHandle = pSettings->satHandle;
    pSatDevice->dsqHandle = pSettings->dsqHandle;
    pSatDevice->wfeHandle = pSettings->wfeHandle;

    pSatDevice->numChannels = pSettings->numChannels;
    pSatDevice->numWfe = pSettings->numWfe;

    return pSatDevice;
}

NEXUS_FrontendHandle NEXUS_Frontend_P_Sat_Create_Channel(const NEXUS_FrontendSatChannelSettings *pSettings)
{
    BERR_Code errCode;
    NEXUS_SatChannel *pSatChannel;
    bool deviceManagesDiseqc = false;

    BSTD_UNUSED(deviceManagesDiseqc);

    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(NULL != pSettings->satChannel);

    pSatChannel = BKNI_Malloc(sizeof(NEXUS_SatChannel));
    if ( NULL == pSatChannel )
    {
        errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }
    BKNI_Memset(pSatChannel, 0, sizeof(*pSatChannel));

    pSatChannel->settings = *pSettings; /* Save Settings */
    pSatChannel->satDevice = pSettings->satDevice;
    pSatChannel->satChannel = pSettings->satChannel;
    pSatChannel->satChip = pSettings->satChip;
    pSatChannel->channel = pSettings->channelIndex;
    pSatChannel->diseqcIndex = pSettings->diseqcIndex;

    pSatChannel->wfeHandle = pSettings->wfeHandle;
    pSatChannel->wfeChannel = pSettings->wfeChannel;

    errCode = BSAT_GetLockStateChangeEventHandle(pSatChannel->satChannel, &pSatChannel->lockEvent);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_lock_event;
    }

    pSatChannel->lockEventCallback = NEXUS_RegisterEvent(pSatChannel->lockEvent,
                                                     NEXUS_Frontend_P_Sat_LockEventHandler,
                                                     pSatChannel);
    if ( NULL == pSatChannel->lockEventCallback )
    {
        errCode = BERR_TRACE(BERR_OS_ERROR);
        goto err_lock_event_callback;
    }

    /* Create Frontend Handle */
    pSatChannel->frontendHandle = NEXUS_Frontend_P_Create(pSatChannel);
    if ( NULL == pSatChannel->frontendHandle )
    {
        errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto err_frontend_handle;
    }
    pSatChannel->frontendHandle->chip.id = pSettings->satChip;

    /* Set Capabilities */
    BDBG_ASSERT(pSettings->capabilities.satellite);
    pSatChannel->frontendHandle->capabilities = pSettings->capabilities;
    pSatChannel->capabilities = pSettings->capabilities;

    /* Install Hooks */
    pSatChannel->frontendHandle->close = NEXUS_Frontend_P_Sat_Close;

    /* Tuning */
    pSatChannel->frontendHandle->tuneSatellite = NEXUS_Frontend_P_Sat_TuneSatellite;
    pSatChannel->frontendHandle->untune = NEXUS_Frontend_P_Sat_Untune;

    /* Status */
    pSatChannel->frontendHandle->getSatelliteStatus = NEXUS_Frontend_P_Sat_GetSatelliteStatus;
    pSatChannel->frontendHandle->getType = NEXUS_Frontend_P_Sat_GetType;
    pSatChannel->frontendHandle->resetStatus = NEXUS_Frontend_P_Sat_ResetStatus;
    pSatChannel->frontendHandle->getFastStatus = NEXUS_Frontend_P_Sat_GetFastStatus;
    pSatChannel->frontendHandle->getBertStatus = NEXUS_Frontend_P_Sat_GetBertStatus;
    pSatChannel->frontendHandle->readSoftDecisions = NEXUS_Frontend_P_Sat_ReadSoftDecisions;
    pSatChannel->frontendHandle->readSatelliteConfig = NEXUS_Frontend_P_Sat_ReadSatelliteConfig;
    pSatChannel->frontendHandle->writeSatelliteConfig = NEXUS_Frontend_P_Sat_WriteSatelliteConfig;

    /* Diseqc */
    pSatChannel->frontendHandle->getDiseqcSettings = NEXUS_Frontend_P_Sat_GetDiseqcSettings;
    pSatChannel->frontendHandle->setDiseqcSettings = NEXUS_Frontend_P_Sat_SetDiseqcSettings;
    pSatChannel->frontendHandle->sendDiseqcAcw = NEXUS_Frontend_P_Sat_SendDiseqcAcw;
    pSatChannel->frontendHandle->sendDiseqcMessage = NEXUS_Frontend_P_Sat_SendDiseqcMessage;
    pSatChannel->frontendHandle->getDiseqcStatus = NEXUS_Frontend_P_Sat_GetDiseqcStatus;
    pSatChannel->frontendHandle->getDiseqcReply = NEXUS_Frontend_P_Sat_GetDiseqcReply;
    pSatChannel->frontendHandle->resetDiseqc = NEXUS_Frontend_P_Sat_ResetDiseqc;
#if NEXUS_HAS_FSK
    pSatChannel->getFskChannelHandle = pSettings->getFskChannelHandle;
#endif

    /* Peak Scan */
    pSatChannel->frontendHandle->satellitePeakscan = NEXUS_Frontend_P_Sat_SatellitePeakscan;
    pSatChannel->frontendHandle->getSatellitePeakscanResult = NEXUS_Frontend_P_Sat_GetSatellitePeakscanResult;
    pSatChannel->frontendHandle->satelliteToneSearch = NEXUS_Frontend_P_Sat_SatelliteToneSearch;
    pSatChannel->frontendHandle->getSatelliteToneSearchResult = NEXUS_Frontend_P_Sat_GetSatelliteToneSearchResult;
    pSatChannel->frontendHandle->getSatelliteSignalDetectStatus = NEXUS_Frontend_P_Sat_GetSignalDetectStatus;

    /* extensions */
    pSatChannel->frontendHandle->registerExtension = NEXUS_Frontend_P_Sat_RegisterExtension;

    /* spectrum analyzer */
    pSatChannel->frontendHandle->requestSpectrumData = NEXUS_Frontend_P_Sat_RequestSpectrumData;
    pSatChannel->deviceClearSpectrumCallbacks = pSettings->deviceClearSpectrumCallbacks;

    {
        BKNI_EventHandle saEvent;
        errCode = BWFE_GetSaDoneEventHandle(pSatChannel->wfeHandle, &saEvent);
        if ( errCode ) {
            errCode = BERR_TRACE(errCode);
            goto err_spectrum;
        }

        /* create event here, because of module lock requirements */
        errCode = BKNI_CreateEvent(&pSatChannel->spectrumDataReadyEvent);
        if ( errCode ) {
            errCode = BERR_TRACE(errCode);
            goto err_spectrum;
        }

        pSatChannel->spectrumDataReadyAppCallback = NEXUS_TaskCallback_Create(pSatChannel, NULL);
        if ( !pSatChannel->spectrumDataReadyAppCallback ) {
            errCode = BERR_TRACE(NEXUS_UNKNOWN);
            goto err_spectrum;
        }
        BDBG_MSG(("Spectrum: event: %p, data ready event: %p, data ready app callback: %p", (void *)saEvent,(void *)pSatChannel->spectrumDataReadyEvent,(void *)(unsigned long)pSatChannel->spectrumDataReadyAppCallback));
    }

    pSatChannel->diseqcIndex = pSettings->diseqcIndex;
    pSatChannel->getDiseqcChannelHandle = pSettings->getDiseqcChannelHandle;
    pSatChannel->getDiseqcChannelHandleParam = pSettings->getDiseqcChannelHandleParam;
    pSatChannel->setVoltage = pSettings->setVoltage;

    pSatChannel->lockAppCallback = NEXUS_TaskCallback_Create(pSatChannel->frontendHandle, NULL);
    if ( NULL == pSatChannel->lockAppCallback )
    {
        errCode = BERR_TRACE(BERR_OS_ERROR);
        goto err_lock_app_callback;
    }

    if (pSettings->capabilities.diseqc) {
        /* These must be created after the frontend handle has been created */
        pSatChannel->diseqcRxAppCallback = NEXUS_TaskCallback_Create(pSatChannel->frontendHandle, NULL);
        if (NULL == pSatChannel->diseqcRxAppCallback) {
            errCode = BERR_TRACE(BERR_OS_ERROR);
            goto err_diseqc_app_callback;
        }
        pSatChannel->diseqcTxAppCallback = NEXUS_TaskCallback_Create(pSatChannel->frontendHandle, NULL);
        if (NULL == pSatChannel->diseqcTxAppCallback) {
            errCode = BERR_TRACE(BERR_OS_ERROR);
            goto err_diseqc_app_callback;
        }
        if (pSettings->setDiseqcRxAppCallback) {
            pSettings->setDiseqcRxAppCallback(pSettings->getDiseqcChannelHandleParam, pSettings->diseqcIndex, pSatChannel->diseqcRxAppCallback);
        }
        if (pSettings->setDiseqcTxAppCallback) {
            pSettings->setDiseqcTxAppCallback(pSettings->getDiseqcChannelHandleParam, pSettings->diseqcIndex, pSatChannel->diseqcTxAppCallback);
        }

        if (pSettings->getDefaultDiseqcSettings) {
            BDSQ_ChannelSettings dsqChannelSettings;
            pSettings->getDefaultDiseqcSettings(pSatChannel, &dsqChannelSettings);
            pSatChannel->diseqcSettings.lnbEnabled = dsqChannelSettings.bEnableLNBPU;
            if (dsqChannelSettings.bOverrideFraming) {
                if (dsqChannelSettings.bExpectReply) {
                    pSatChannel->diseqcSettings.framing = NEXUS_FrontendDiseqcFraming_eExpectReply;
                } else {
                    pSatChannel->diseqcSettings.framing = NEXUS_FrontendDiseqcFraming_eDontExpectReply;
                }
            } else {
                pSatChannel->diseqcSettings.framing = NEXUS_FrontendDiseqcFraming_eDefault;
            }
            /* pSatChannel->diseqcSettings.toneEnabled is not provided by BDSQ_ChannelSettings */
            pSatChannel->diseqcSettings.toneMode = dsqChannelSettings.bEnvelope ? NEXUS_FrontendDiseqcToneMode_eEnvelope : NEXUS_FrontendDiseqcToneMode_eTone;
            /* pSatChannel->diseqcSettings.voltage is not provided by BDSQ_ChannelSettings */
            if (dsqChannelSettings.bToneburstB) {
                pSatChannel->diseqcSettings.toneBurst = NEXUS_FrontendDiseqcToneBurst_eNominal;
            } else if (dsqChannelSettings.bEnableToneburst) {
                pSatChannel->diseqcSettings.toneBurst = NEXUS_FrontendDiseqcToneBurst_eUnmodulated;
            } else {
                pSatChannel->diseqcSettings.toneBurst = NEXUS_FrontendDiseqcToneBurst_eNone;
            }
        } else {
            BKNI_Memset(&pSatChannel->diseqcSettings,0,sizeof(pSatChannel->diseqcSettings));
        }
        pSatChannel->diseqcSettings.enabled = true;
    }

    errCode = BSAT_GetAcqDoneEventHandle(pSatChannel->satChannel, &pSatChannel->peakscanEvent);
    if ( errCode == BERR_NOT_SUPPORTED) {
        pSatChannel->peakscanEvent = NULL; /* used to determine whether peakscan is available */
    }
    else {
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
            goto err_peak_scan_event;
        }

        pSatChannel->peakscanEventCallback = NEXUS_RegisterEvent(pSatChannel->peakscanEvent,
                                                             NEXUS_Frontend_P_Sat_PeakscanEventHandler,
                                                             pSatChannel);
        if ( NULL == pSatChannel->peakscanEventCallback )
        {
            errCode = BERR_TRACE(BERR_OS_ERROR);
            goto err_peak_scan_event_callback;
        }
    }

    if (pSatChannel->peakscanEvent) {
        pSatChannel->peakscanAppCallback = NEXUS_TaskCallback_Create(pSatChannel->frontendHandle, NULL);
        if ( NULL == pSatChannel->peakscanAppCallback )
        {
            errCode = BERR_TRACE(BERR_OS_ERROR);
            goto err_peak_scan_app_callback;
        }
    }

    pSatChannel->ftmCallback = NEXUS_TaskCallback_Create(pSatChannel->frontendHandle, NULL);
    if ( NULL == pSatChannel->ftmCallback )
    {
        errCode = BERR_TRACE(BERR_OS_ERROR);
        goto err_lock_app_callback;
    }

    /* fill in type information */
    {
        int i;
        unsigned base10[5] = {10000, 1000, 100, 10, 1};
        unsigned base16[5] = {0x10000, 0x1000, 0x100, 0x10, 1};
        unsigned family = 0, familyDec;
        BFEC_VersionInfo info;

        errCode = BSAT_GetVersionInfo(pSatChannel->satDevice->satHandle, &info);
        BKNI_Memset(&pSatChannel->type, 0, sizeof(pSatChannel->type));
        familyDec = pSettings->satChip;
        for (i=0; i<5; i++) {
            family += (familyDec / base10[i]) * base16[i];
            familyDec = familyDec % base10[i];
        }
        pSatChannel->type.chip.familyId = family;
        pSatChannel->type.chip.id = family;
        pSatChannel->type.chip.version.buildId = info.buildId;
        pSatChannel->type.chip.version.buildType = info.buildType;
        pSatChannel->type.chip.version.major = info.majorVersion;
        pSatChannel->type.chip.version.minor = info.minorVersion;
    }

    /* Success */
    BDBG_OBJECT_SET(pSatChannel, NEXUS_SatChannel);
    BDBG_MSG(("7366[%d]:fe:%p,dvc:%p",pSettings->channelIndex,(void *)pSatChannel->frontendHandle,(void *)pSatChannel));
    return pSatChannel->frontendHandle;

err_peak_scan_event:
err_peak_scan_event_callback:
err_peak_scan_app_callback:
err_lock_app_callback:
    if (pSatChannel->ftmCallback) {
        NEXUS_TaskCallback_Destroy(pSatChannel->ftmCallback);
    }
    if (pSatChannel->peakscanEvent) {
        NEXUS_TaskCallback_Destroy(pSatChannel->peakscanAppCallback);
    }
    if (pSatChannel->diseqcTxAppCallback) {
        NEXUS_TaskCallback_Destroy(pSatChannel->diseqcTxAppCallback);
    }
    if (pSatChannel->diseqcRxAppCallback) {
        NEXUS_TaskCallback_Destroy(pSatChannel->diseqcRxAppCallback);
    }
err_diseqc_app_callback:
    NEXUS_Frontend_P_Destroy(pSatChannel->frontendHandle);
err_spectrum:
    if (pSatChannel->spectrumDataReadyAppCallback) {
        NEXUS_TaskCallback_Destroy(pSatChannel->spectrumDataReadyAppCallback);
    }
    if (pSatChannel->spectrumDataReadyEventCallback) {
        NEXUS_UnregisterEvent(pSatChannel->spectrumDataReadyEventCallback);
    }
    if (pSatChannel->spectrumEventCallback) {
        NEXUS_UnregisterEvent(pSatChannel->spectrumEventCallback);
    }
    if (pSatChannel->spectrumDataReadyEvent) {
        BKNI_DestroyEvent(pSatChannel->spectrumDataReadyEvent);
    }
err_frontend_handle:
    if (pSatChannel->peakscanEvent) {
        NEXUS_UnregisterEvent(pSatChannel->peakscanEventCallback);
    }
    NEXUS_UnregisterEvent(pSatChannel->lockEventCallback);
err_lock_event_callback:
err_lock_event:
    BKNI_Free(pSatChannel);

    return NULL;
}

static void NEXUS_Frontend_P_Sat_Close(NEXUS_FrontendHandle handle)
{
    NEXUS_SatChannel *pSatChannel;
    bool deviceManagesDiseqc = false;

    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);
    pSatChannel = handle->pDeviceHandle;
    BDBG_OBJECT_ASSERT(pSatChannel, NEXUS_SatChannel);

    if (pSatChannel->getDiseqcChannelHandle) {
        deviceManagesDiseqc = true;
    }
    /* Cleanup Channel */
    if (pSatChannel->peakscanEvent) {
        NEXUS_UnregisterEvent(pSatChannel->peakscanEventCallback);
        NEXUS_TaskCallback_Destroy(pSatChannel->peakscanAppCallback);
    }
    if (pSatChannel->lockAppCallback) {
        NEXUS_TaskCallback_Destroy(pSatChannel->lockAppCallback);
    }
    if (pSatChannel->lockEventCallback) {
        NEXUS_UnregisterEvent(pSatChannel->lockEventCallback);
    }
    if (pSatChannel->ftmCallback) {
        NEXUS_TaskCallback_Destroy(pSatChannel->ftmCallback);
    }

    if (pSatChannel->spectrumDataReadyAppCallback) {
        NEXUS_TaskCallback_Destroy(pSatChannel->spectrumDataReadyAppCallback);
    }
    if (pSatChannel->spectrumDataReadyEventCallback) {
        NEXUS_UnregisterEvent(pSatChannel->spectrumDataReadyEventCallback);
    }
    if (pSatChannel->spectrumEventCallback) {
        NEXUS_UnregisterEvent(pSatChannel->spectrumEventCallback);
    }
    if (pSatChannel->spectrumDataReadyEvent) {
        BKNI_DestroyEvent(pSatChannel->spectrumDataReadyEvent);
    }

    if (pSatChannel->capabilities.diseqc)
    {
        if (!deviceManagesDiseqc) {
            if (pSatChannel->diseqcEventCallback)
                NEXUS_UnregisterEvent(pSatChannel->diseqcEventCallback);
        }
        if (pSatChannel->diseqcRxAppCallback)
            NEXUS_TaskCallback_Destroy(pSatChannel->diseqcRxAppCallback);
        if (pSatChannel->diseqcTxAppCallback)
            NEXUS_TaskCallback_Destroy(pSatChannel->diseqcTxAppCallback);
    }

    NEXUS_Frontend_P_Destroy(pSatChannel->frontendHandle);

    /* Call post-close callback */
    if ( pSatChannel->settings.closeFunction )
    {
        pSatChannel->settings.closeFunction(handle, pSatChannel->settings.pCloseParam);
    }

    BKNI_Memset(pSatChannel, 0, sizeof(*pSatChannel));
    BKNI_Free(pSatChannel);
}

static void NEXUS_Frontend_P_Sat_LockEventHandler(void *pParam)
{
    NEXUS_SatChannel *pSatChannel = pParam;
    BDBG_OBJECT_ASSERT(pSatChannel, NEXUS_SatChannel);
    BDBG_MSG(("SAT Lock Event"));

    BTRC_TRACE(ChnChange_TuneLock, STOP);
    NEXUS_TaskCallback_Fire(pSatChannel->lockAppCallback);
}

NEXUS_Error NEXUS_Frontend_P_Sat_TuneSatellite( void *handle, const NEXUS_FrontendSatelliteSettings *pSettings )
{
    NEXUS_SatChannel *pSatChannel = handle;
    BSAT_AcqSettings acqSettings;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(pSatChannel, NEXUS_SatChannel);
    BDBG_ASSERT(NULL != pSettings);

    NEXUS_Frontend_P_Sat_EnableSatChannel(pSatChannel);

    BKNI_Memset(&acqSettings, 0, sizeof(acqSettings));

    if (pSettings->reacquireDisable)
        acqSettings.options |= BSAT_ACQ_DISABLE_REACQ;
    if (pSettings->nyquist20) {
        acqSettings.options |= BSAT_ACQ_NYQUIST_20;
    } else {
        acqSettings.options |= BSAT_ACQ_NYQUIST_35;
    }
    if (pSettings->dciiSplit)
        acqSettings.options |= BSAT_ACQ_DCII_SPLIT;
    if (pSettings->dciiSplitQ)
        acqSettings.options |= BSAT_ACQ_DCII_SPLIT_Q;
    if (pSettings->oQpsk)
        acqSettings.options |= BSAT_ACQ_OQPSK;
    if (pSettings->ldpcPilot)
        acqSettings.options |= BSAT_ACQ_PILOT;
    if (pSettings->ldpcPilotScan)
        acqSettings.options |= BSAT_ACQ_PILOT_SCAN_ENABLE;
    if (pSettings->tunerTestMode)
        acqSettings.options |= BSAT_ACQ_TUNER_TEST_MODE;

    if (pSettings->bertEnable) {
        BSAT_BertSettings bertSettings;
        BSAT_OutputTransportSettings xptSettings;

        acqSettings.options |= BSAT_ACQ_ENABLE_BERT;

        if (pSettings->mode != NEXUS_FrontendSatelliteMode_eDss) {
            BSAT_GetOutputTransportSettings (pSatChannel->satChannel, &xptSettings);
            xptSettings.bXbert = true;
            BSAT_SetOutputTransportSettings (pSatChannel->satChannel, &xptSettings);
        }

        BSAT_GetBertSettings(pSatChannel->satChannel, &bertSettings);
        bertSettings.bInvert = pSettings->pnDataInvert;
        BSAT_SetBertSettings(pSatChannel->satChannel, &bertSettings);
    }

    switch (pSettings->mode)
    {
    case NEXUS_FrontendSatelliteMode_eDvb:
        BDBG_MSG(("Tune DVB"));
        acqSettings.mode = NEXUS_Frontend_P_Sat_GetMode(g_sds_dvb_modes, &pSettings->codeRate);
        break;
    case NEXUS_FrontendSatelliteMode_eDss:
        BDBG_MSG(("Tune DSS"));
        acqSettings.mode = NEXUS_Frontend_P_Sat_GetMode(g_sds_dss_modes, &pSettings->codeRate);
        break;
    case NEXUS_FrontendSatelliteMode_eDcii:
        BDBG_MSG(("Tune DCII"));
        acqSettings.mode = NEXUS_Frontend_P_Sat_GetMode(g_sds_dcii_modes, &pSettings->codeRate);
        break;
    case NEXUS_FrontendSatelliteMode_eQpskLdpc:
        BDBG_MSG(("Tune QPSK LDPC"));
        acqSettings.mode = NEXUS_Frontend_P_Sat_GetMode(g_sds_qpsk_ldpc_modes, &pSettings->codeRate);
        break;
    case NEXUS_FrontendSatelliteMode_e8pskLdpc:
        BDBG_MSG(("Tune 8PSK LDPC"));
        acqSettings.mode = NEXUS_Frontend_P_Sat_GetMode(g_sds_8psk_ldpc_modes, &pSettings->codeRate);
        break;
    case NEXUS_FrontendSatelliteMode_eLdpc:
        BDBG_MSG(("Tune LDCP"));
        acqSettings.mode = NEXUS_Frontend_P_Sat_GetMode(g_sds_ldpc_modes, &pSettings->codeRate);
        break;
    case NEXUS_FrontendSatelliteMode_eQpskTurbo:
        BDBG_MSG(("Tune Turbo QPSK"));
        acqSettings.mode = NEXUS_Frontend_P_Sat_GetMode(g_sds_qpsk_turbo_modes, &pSettings->codeRate);
        break;
    case NEXUS_FrontendSatelliteMode_e8pskTurbo:
        BDBG_MSG(("Tune Turbo 8PSK"));
        acqSettings.mode = NEXUS_Frontend_P_Sat_GetMode(g_sds_8psk_turbo_modes, &pSettings->codeRate);
        break;
    case NEXUS_FrontendSatelliteMode_eTurbo:
        BDBG_MSG(("Tune Turbo"));
        acqSettings.mode = NEXUS_Frontend_P_Sat_GetMode(g_sds_turbo_modes, &pSettings->codeRate);
        break;
    case NEXUS_FrontendSatelliteMode_eBlindAcquisition:
        BDBG_MSG(("Blind acquisition"));
        acqSettings.mode = NEXUS_Frontend_P_Sat_GetMode(g_blind_acquisition_mode, &pSettings->codeRate);
        break;
    default:
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    acqSettings.freq = pSettings->frequency;
    acqSettings.adcSelect = pSatChannel->selectedAdc;
    acqSettings.symbolRate = pSettings->symbolRate;

    NEXUS_TaskCallback_Set(pSatChannel->lockAppCallback, &pSettings->lockCallback);

    /* Support external tuner if present */
    if (pSatChannel->settings.devices.tuner)
    {
        errCode = NEXUS_Tuner_SetFrequency(pSatChannel->settings.devices.tuner, NEXUS_TunerMode_eDigital, pSettings->frequency);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }

    if (pSettings->networkSpec != pSatChannel->lastSettings.networkSpec) {
        BSAT_NetworkSpec networkSpec;
        BDBG_MSG(("Changing networkSpec from %d to %d",pSatChannel->lastSettings.networkSpec,pSettings->networkSpec));
        switch (pSettings->networkSpec) {
        case NEXUS_FrontendSatelliteNetworkSpec_eDefault:
            networkSpec = BSAT_NetworkSpec_eDefault; break;
        case NEXUS_FrontendSatelliteNetworkSpec_eCustom1:
            networkSpec = BSAT_NetworkSpec_eEcho; break;
        case NEXUS_FrontendSatelliteNetworkSpec_eEuro:
            networkSpec = BSAT_NetworkSpec_eEuro; break;
        default:
            BDBG_WRN(("network spec %d is unsupported",pSettings->networkSpec));
            networkSpec = NEXUS_FrontendSatelliteNetworkSpec_eDefault; break;
        }
        BSAT_SetNetworkSpec(pSatChannel->satDevice->satHandle, networkSpec);
    }

    errCode = BSAT_SetSearchRange(pSatChannel->satChannel, pSettings->searchRange);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    pSatChannel->lastSettings = *pSettings; /* save after all config, but before the acquire. */

    if (pSettings->signalDetectMode) {
        /* For SAT, in signal detect mode, most parameters are ignored */
        BDBG_MSG(("fe: %p, channel: %d, chan handle: %p, freq: %u, symbol rate: %u, signal detect", handle, (void *)pSatChannel->channel, pSatChannel->satChannel, pSettings->frequency, pSettings->symbolRate));
        errCode = BSAT_StartSignalDetect(pSatChannel->satChannel, pSettings->symbolRate, pSettings->frequency, pSatChannel->selectedAdc);
        if (errCode) {
            return BERR_TRACE(errCode);
        }
    } else {
        BDBG_MSG(("fe: %p, channel: %d, chan handle: %p, freq: %u, symbol rate: %u", handle, (void *)pSatChannel->channel, pSatChannel->satChannel, acqSettings.freq, acqSettings.symbolRate));
        errCode = BSAT_Acquire(pSatChannel->satChannel, &acqSettings);
        if (errCode) {
            return BERR_TRACE(errCode);
        }
    }

    BTRC_TRACE(ChnChange_Tune, STOP);
    BTRC_TRACE(ChnChange_TuneLock, START);

    return BERR_SUCCESS;
}

void NEXUS_Frontend_P_Sat_Untune(void *handle)
{
    NEXUS_SatChannel *pSatChannel = handle;

    BDBG_OBJECT_ASSERT(pSatChannel, NEXUS_SatChannel);

    BDBG_MSG(("untune"));

    NEXUS_Frontend_P_Sat_DisableDsqChannel(pSatChannel);
    NEXUS_Frontend_P_Sat_DisableSatChannel(pSatChannel);
}

static void NEXUS_Frontend_P_Sat_GetType(void *handle, NEXUS_FrontendType *type)
{
    NEXUS_SatChannel *pSatChannel = handle;
    BDBG_OBJECT_ASSERT(pSatChannel, NEXUS_SatChannel);
    BKNI_Memcpy(type,(void *)&(pSatChannel->type),sizeof(*type));
}

static NEXUS_Error NEXUS_Frontend_P_Sat_GetFastStatus(void *handle, NEXUS_FrontendFastStatus *pStatus )
{
    NEXUS_SatChannel *pSatChannel = handle;
    bool isLocked = false,
         acqComplete = false;
    BERR_Code rc;
    BDBG_OBJECT_ASSERT(pSatChannel, NEXUS_SatChannel);
    BDBG_ASSERT(pStatus);

    NEXUS_Frontend_P_Sat_EnableSatChannel(pSatChannel);

    pStatus->lockStatus = NEXUS_FrontendLockStatus_eUnknown;
    rc = BSAT_GetLockStatus(pSatChannel->satChannel, &isLocked, &acqComplete);
    if (rc) {
        return BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }
    /* SAT only returns locked/unlocked, there is currently no "no signal" status. */
    pStatus->lockStatus = isLocked ? NEXUS_FrontendLockStatus_eLocked : NEXUS_FrontendLockStatus_eUnlocked;
    pStatus->acquireInProgress = !acqComplete;
    return NEXUS_SUCCESS;
}

static NEXUS_Error NEXUS_Frontend_P_Sat_GetSatelliteStatus( void *handle, NEXUS_FrontendSatelliteStatus *pStatus )
{
    BERR_Code errCode;
    NEXUS_SatChannel *pSatChannel = handle;
    BSAT_ChannelStatus satStatus;
    BFEC_VersionInfo info;
    NEXUS_Time now;

    BDBG_OBJECT_ASSERT(pSatChannel, NEXUS_SatChannel);
    BDBG_ASSERT(NULL != pStatus);

    NEXUS_Frontend_P_Sat_EnableSatChannel(pSatChannel);

    BKNI_Memset(pStatus, 0, sizeof(*pStatus));

    errCode = BSAT_GetVersionInfo(pSatChannel->satDevice->satHandle, &info);
    pStatus->version.chipId = pSatChannel->type.chip.id;

    errCode = BSAT_GetChannelStatus(pSatChannel->satChannel, &satStatus);
    if ( errCode ) {
        /* if not locked, BSAT_GetChannelStatus will fail. this is not an error. the status structure should just report not locked and no status available. */
        pStatus->statusError = NEXUS_FrontendSatelliteStatusError_eGetChannelStatusError;
        BDBG_MSG(("BSAT_GetChannelStatus returned %x",errCode));
        return 0;
    }
    NEXUS_Time_Get(&now);
    pStatus->timeElapsed = NEXUS_Time_Diff(&now, &pSatChannel->frontendHandle->resetStatusTime);

    pStatus->settings = pSatChannel->lastSettings; /* return last settings used in NEXUS_Frontend_TuneSatellite */
    pStatus->channel = pSatChannel->settings.channelIndex;

    pStatus->frequency = satStatus.acqSettings.freq; /* This value is also in pStatus->settings.frequency, so this is redundant. */
    pStatus->sampleClock = satStatus.sampleFreq;
    pStatus->carrierError = satStatus.carrierError;
    pStatus->outputBitRate = satStatus.outputBitrate;
    pStatus->symbolRate = satStatus.acqSettings.symbolRate;
    pStatus->symbolRateError = satStatus.symbolRateError;

    pStatus->snrEstimate = satStatus.snr * 100/256;  /* convert to 1/100 units */
    pStatus->mpegErrors = satStatus.mpegErrors;
    pStatus->mpegCount = satStatus.mpegCount;

    pStatus->tunerLocked = satStatus.bTunerLocked;
    pStatus->demodLocked = satStatus.bDemodLocked;

    pStatus->spectralInversion = satStatus.bSpinv ? NEXUS_FrontendSatelliteInversion_eInverted : NEXUS_FrontendSatelliteInversion_eNormal;
    if (BSAT_MODE_IS_LEGACY_QPSK(satStatus.mode))
    {
        pStatus->fecCorrected = satStatus.modeStatus.qpsk.rsCorrCount;
        pStatus->fecUncorrected = satStatus.modeStatus.qpsk.rsUncorrCount;
        pStatus->preViterbiErrorCount = satStatus.modeStatus.qpsk.preVitErrCount;
    }
    else if (BSAT_MODE_IS_DVBS2(satStatus.mode))
    {
        pStatus->fecCorrected = satStatus.modeStatus.dvbs2.corrBlocks;
        pStatus->fecUncorrected = satStatus.modeStatus.dvbs2.badBlocks;
        pStatus->fecClean = satStatus.modeStatus.dvbs2.totalBlocks -
                satStatus.modeStatus.dvbs2.corrBlocks -
                satStatus.modeStatus.dvbs2.badBlocks;
    }
    else if (BSAT_MODE_IS_TURBO(satStatus.mode))
    {
        pStatus->fecCorrected = satStatus.modeStatus.turbo.corrBlocks;
        pStatus->fecUncorrected = satStatus.modeStatus.turbo.badBlocks;
        pStatus->fecClean = satStatus.modeStatus.turbo.totalBlocks -
                satStatus.modeStatus.turbo.corrBlocks -
                satStatus.modeStatus.turbo.badBlocks;
   }

    /* Check mode to get code rate */
    switch (satStatus.mode) {
    case BSAT_Mode_eDvbs_scan:
    case BSAT_Mode_eDss_scan:
    case BSAT_Mode_eDcii_scan:
    case BSAT_Mode_eDvbs2_scan:
    case BSAT_Mode_eTurbo_scan:
        pStatus->codeRate = g_cr_scan;
        break;

    case BSAT_Mode_eDvbs_1_2:
    case BSAT_Mode_eDss_1_2:
    case BSAT_Mode_eDcii_1_2:
    case BSAT_Mode_eDvbs2_Qpsk_1_2:
    case BSAT_Mode_eTurbo_Qpsk_1_2:
        pStatus->codeRate = g_cr_1_2;
        break;

    case BSAT_Mode_eDvbs2_Qpsk_1_3:
        pStatus->codeRate = g_cr_1_3;
        break;

    case BSAT_Mode_eDvbs2_Qpsk_1_4:
        pStatus->codeRate = g_cr_1_4;
        break;

    case BSAT_Mode_eDvbs_2_3:
    case BSAT_Mode_eDss_2_3:
    case BSAT_Mode_eDcii_2_3:
    case BSAT_Mode_eDvbs2_Qpsk_2_3:
    case BSAT_Mode_eDvbs2_8psk_2_3:
    case BSAT_Mode_eTurbo_Qpsk_2_3:
    case BSAT_Mode_eTurbo_8psk_2_3:
    case BSAT_Mode_eDvbs2_16apsk_2_3:
        pStatus->codeRate = g_cr_2_3;
        break;

    case BSAT_Mode_eDvbs2_Qpsk_2_5:
        pStatus->codeRate = g_cr_2_5;
        break;

    case BSAT_Mode_eDvbs_3_4:
    case BSAT_Mode_eDcii_3_4:
    case BSAT_Mode_eDvbs2_Qpsk_3_4:
    case BSAT_Mode_eDvbs2_8psk_3_4:
    case BSAT_Mode_eTurbo_Qpsk_3_4:
    case BSAT_Mode_eTurbo_8psk_3_4:
    case BSAT_Mode_eDvbs2_16apsk_3_4:
    case BSAT_Mode_eDvbs2_32apsk_3_4:
        pStatus->codeRate = g_cr_3_4;
        break;

    case BSAT_Mode_eDcii_3_5:
    case BSAT_Mode_eDvbs2_Qpsk_3_5:
    case BSAT_Mode_eDvbs2_8psk_3_5:
        pStatus->codeRate = g_cr_3_5;
        break;

    case BSAT_Mode_eDcii_4_5:
    case BSAT_Mode_eDvbs2_Qpsk_4_5:
    case BSAT_Mode_eTurbo_8psk_4_5:
    case BSAT_Mode_eDvbs2_16apsk_4_5:
    case BSAT_Mode_eDvbs2_32apsk_4_5:
        pStatus->codeRate = g_cr_4_5;
        break;

    case BSAT_Mode_eDvbs_5_6:
    case BSAT_Mode_eDcii_5_6:
    case BSAT_Mode_eDvbs2_Qpsk_5_6:
    case BSAT_Mode_eDvbs2_8psk_5_6:
    case BSAT_Mode_eTurbo_Qpsk_5_6:
    case BSAT_Mode_eTurbo_8psk_5_6:
    case BSAT_Mode_eDvbs2_16apsk_5_6:
    case BSAT_Mode_eDvbs2_32apsk_5_6:
        pStatus->codeRate = g_cr_5_6;
        break;

    case BSAT_Mode_eDss_6_7:
        pStatus->codeRate = g_cr_6_7;
        break;

    case BSAT_Mode_eDvbs_7_8:
    case BSAT_Mode_eDcii_7_8:
    case BSAT_Mode_eTurbo_Qpsk_7_8:
        pStatus->codeRate = g_cr_7_8;
        break;

    case BSAT_Mode_eDcii_5_11:
        pStatus->codeRate = g_cr_5_11;
        break;

    case BSAT_Mode_eDvbs2_Qpsk_8_9:
    case BSAT_Mode_eDvbs2_8psk_8_9:
    case BSAT_Mode_eTurbo_8psk_8_9:
    case BSAT_Mode_eDvbs2_16apsk_8_9:
    case BSAT_Mode_eDvbs2_32apsk_8_9:
        pStatus->codeRate = g_cr_8_9;
        break;

    case BSAT_Mode_eDvbs2_Qpsk_9_10:
    case BSAT_Mode_eDvbs2_8psk_9_10:
    case BSAT_Mode_eDvbs2_16apsk_9_10:
    case BSAT_Mode_eDvbs2_32apsk_9_10:
        pStatus->codeRate = g_cr_9_10;
        break;

    default:
        break;
    }

    switch (satStatus.mode) {
    default:
    case BSAT_Mode_eDvbs_scan:
    case BSAT_Mode_eDvbs_1_2:
    case BSAT_Mode_eDvbs_2_3:
    case BSAT_Mode_eDvbs_3_4:
    case BSAT_Mode_eDvbs_5_6:
    case BSAT_Mode_eDvbs_7_8:
        pStatus->mode = NEXUS_FrontendSatelliteMode_eDvb;
        break;

    case BSAT_Mode_eDss_scan:
    case BSAT_Mode_eDss_1_2:
    case BSAT_Mode_eDss_2_3:
    case BSAT_Mode_eDss_6_7:
        pStatus->mode = NEXUS_FrontendSatelliteMode_eDss;
        break;

    case BSAT_Mode_eDcii_scan:
    case BSAT_Mode_eDcii_1_2:
    case BSAT_Mode_eDcii_2_3:
    case BSAT_Mode_eDcii_3_4:
    case BSAT_Mode_eDcii_3_5:
    case BSAT_Mode_eDcii_4_5:
    case BSAT_Mode_eDcii_5_6:
    case BSAT_Mode_eDcii_7_8:
    case BSAT_Mode_eDcii_5_11:
        pStatus->mode = NEXUS_FrontendSatelliteMode_eDcii;
        break;

    case BSAT_Mode_eDvbs2_scan:
    case BSAT_Mode_eDvbs2_Qpsk_1_2:
    case BSAT_Mode_eDvbs2_Qpsk_1_3:
    case BSAT_Mode_eDvbs2_Qpsk_1_4:
    case BSAT_Mode_eDvbs2_Qpsk_2_3:
    case BSAT_Mode_eDvbs2_Qpsk_2_5:
    case BSAT_Mode_eDvbs2_Qpsk_3_4:
    case BSAT_Mode_eDvbs2_Qpsk_3_5:
    case BSAT_Mode_eDvbs2_Qpsk_4_5:
    case BSAT_Mode_eDvbs2_Qpsk_5_6:
    case BSAT_Mode_eDvbs2_Qpsk_9_10:
    case BSAT_Mode_eDvbs2_Qpsk_8_9:
        pStatus->mode = NEXUS_FrontendSatelliteMode_eQpskLdpc;
        break;

    case BSAT_Mode_eDvbs2_8psk_2_3:
    case BSAT_Mode_eDvbs2_8psk_3_4:
    case BSAT_Mode_eDvbs2_8psk_3_5:
    case BSAT_Mode_eDvbs2_8psk_5_6:
    case BSAT_Mode_eDvbs2_8psk_8_9:
    case BSAT_Mode_eDvbs2_8psk_9_10:
        pStatus->mode = NEXUS_FrontendSatelliteMode_e8pskLdpc;
        break;

    case BSAT_Mode_eTurbo_scan:
    case BSAT_Mode_eTurbo_Qpsk_1_2:
    case BSAT_Mode_eTurbo_Qpsk_2_3:
    case BSAT_Mode_eTurbo_Qpsk_3_4:
    case BSAT_Mode_eTurbo_Qpsk_5_6:
    case BSAT_Mode_eTurbo_Qpsk_7_8:
        pStatus->mode = NEXUS_FrontendSatelliteMode_eTurboQpsk;
        break;

    case BSAT_Mode_eTurbo_8psk_2_3:
    case BSAT_Mode_eTurbo_8psk_3_4:
    case BSAT_Mode_eTurbo_8psk_4_5:
    case BSAT_Mode_eTurbo_8psk_5_6:
    case BSAT_Mode_eTurbo_8psk_8_9:
        pStatus->mode = NEXUS_FrontendSatelliteMode_eTurbo8psk;
        break;
    }

    return NEXUS_SUCCESS;
}

static void NEXUS_Frontend_P_Sat_ResetStatus( void *handle )
{
    NEXUS_SatChannel *pSatChannel = handle;

    NEXUS_Frontend_P_Sat_EnableSatChannel(pSatChannel);

    BSAT_ResetChannelStatus(pSatChannel->satChannel);
}

static NEXUS_Error NEXUS_Frontend_P_Sat_RegisterExtension(NEXUS_FrontendHandle parentHandle, NEXUS_FrontendHandle extensionHandle)
{
    NEXUS_SatChannel *pSatChannel;

    BDBG_OBJECT_ASSERT(parentHandle, NEXUS_Frontend);
    pSatChannel = parentHandle->pDeviceHandle;
    BDBG_OBJECT_ASSERT(pSatChannel, NEXUS_SatChannel);

    if (extensionHandle == NULL) {
        extensionHandle = parentHandle;
    }
    else {
        BDBG_OBJECT_ASSERT(extensionHandle, NEXUS_Frontend);
    }

    /* recreate callbacks with the extension handle. this allows NEXUS_StopCallbacks to work. */
    if (pSatChannel->diseqcRxAppCallback) {
        NEXUS_TaskCallback_Destroy(pSatChannel->diseqcRxAppCallback);
        pSatChannel->diseqcRxAppCallback = NEXUS_TaskCallback_Create(extensionHandle, NULL);
        if ( NULL == pSatChannel->diseqcRxAppCallback ) {
            return BERR_TRACE(BERR_OS_ERROR);
        }
    }
    if (pSatChannel->diseqcTxAppCallback) {
        NEXUS_TaskCallback_Destroy(pSatChannel->diseqcTxAppCallback);
        pSatChannel->diseqcTxAppCallback = NEXUS_TaskCallback_Create(extensionHandle, NULL);
        if ( NULL == pSatChannel->diseqcTxAppCallback ) {
            return BERR_TRACE(BERR_OS_ERROR);
        }
    }
    if (pSatChannel->peakscanAppCallback) {
        NEXUS_TaskCallback_Destroy(pSatChannel->peakscanAppCallback);
        pSatChannel->peakscanAppCallback = NEXUS_TaskCallback_Create(extensionHandle, NULL);
        if ( NULL == pSatChannel->peakscanAppCallback ) {
            return BERR_TRACE(BERR_OS_ERROR);
        }
    }
    if (pSatChannel->lockAppCallback) {
        NEXUS_TaskCallback_Destroy(pSatChannel->lockAppCallback);
        pSatChannel->lockAppCallback = NEXUS_TaskCallback_Create(extensionHandle, NULL);
        if ( NULL == pSatChannel->lockAppCallback ) {
            return BERR_TRACE(BERR_OS_ERROR);
        }
    }
    if (pSatChannel->ftmCallback) {
        NEXUS_TaskCallback_Destroy(pSatChannel->ftmCallback);
        pSatChannel->ftmCallback = NEXUS_TaskCallback_Create(extensionHandle, NULL);
        if ( NULL == pSatChannel->ftmCallback ) {
            return BERR_TRACE(BERR_OS_ERROR);
        }
    }

    return 0;
}

static void NEXUS_Frontend_P_Sat_GetDiseqcSettings(void *handle, NEXUS_FrontendDiseqcSettings *pSettings)
{
    NEXUS_SatChannel *pSatChannel = handle;

    BDBG_OBJECT_ASSERT(pSatChannel, NEXUS_SatChannel);

    NEXUS_Frontend_P_Sat_EnableDsqChannel(pSatChannel);

    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    if (pSatChannel->capabilities.diseqc)
    {
        BDBG_ASSERT(NULL != pSettings);
        *pSettings = pSatChannel->diseqcSettings;
    }
}

static NEXUS_Error NEXUS_Frontend_P_Sat_SetDiseqcSettings(void *handle, const NEXUS_FrontendDiseqcSettings *pSettings)
{
    BERR_Code errCode;
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_SatChannel *pSatChannel = handle;
    BDSQ_ChannelHandle dsqChannelHandle = NULL;

    BDBG_OBJECT_ASSERT(pSatChannel, NEXUS_SatChannel);
    BDBG_ASSERT(NULL != pSettings);

    NEXUS_Frontend_P_Sat_EnableDsqChannel(pSatChannel);

    if (pSatChannel->capabilities.diseqc) {
        BDSQ_ChannelSettings dsqSettings;

        if (pSettings->voltage >= NEXUS_FrontendDiseqcVoltage_eMax) { rc = BERR_TRACE(NEXUS_INVALID_PARAMETER); goto err; }
        if (pSettings->toneBurst >= NEXUS_FrontendDiseqcToneBurst_eMax) { rc = BERR_TRACE(NEXUS_INVALID_PARAMETER); goto err; }
        if (pSettings->toneMode >= NEXUS_FrontendDiseqcToneMode_eMax) { rc = BERR_TRACE(NEXUS_INVALID_PARAMETER); goto err; }

        if (pSatChannel->getDiseqcChannelHandle) {
            dsqChannelHandle = pSatChannel->getDiseqcChannelHandle(handle, pSatChannel->diseqcIndex);
        } else {
            dsqChannelHandle = pSatChannel->satDevice->dsqChannels[pSatChannel->diseqcIndex];
        }

        errCode = BDSQ_GetChannelSettings(dsqChannelHandle, &dsqSettings);
        if (errCode) { rc = BERR_TRACE(errCode); goto err; }

        dsqSettings.bEnableToneburst = ((pSettings->toneBurst == NEXUS_FrontendDiseqcToneBurst_eUnmodulated) || (pSettings->toneBurst == NEXUS_FrontendDiseqcToneBurst_eNominal));
        dsqSettings.bEnvelope = pSettings->toneMode == NEXUS_FrontendDiseqcToneMode_eEnvelope;
        dsqSettings.bToneburstB = (pSettings->toneBurst == NEXUS_FrontendDiseqcToneBurst_eNominal);
        dsqSettings.bEnableLNBPU = pSettings->lnbEnabled;
        switch (pSettings->framing) {
        case NEXUS_FrontendDiseqcFraming_eExpectReply:
            dsqSettings.bOverrideFraming = true;
            dsqSettings.bExpectReply = true;
            break;
        case NEXUS_FrontendDiseqcFraming_eDontExpectReply:
            dsqSettings.bOverrideFraming = true;
            dsqSettings.bExpectReply = false;
            break;
        case NEXUS_FrontendDiseqcFraming_eDefault:
        default:
            dsqSettings.bOverrideFraming = false;
            dsqSettings.bExpectReply = false;
            break;
        }

        BDBG_MSG(("tb: %d tbb: %d er: %d lnb: %d env: %d",
                dsqSettings.bEnableToneburst,
                dsqSettings.bToneburstB,
                dsqSettings.bExpectReply,
                dsqSettings.bEnableLNBPU,
                dsqSettings.bEnvelope));
        errCode = BDSQ_SetChannelSettings(dsqChannelHandle, &dsqSettings);
        if (errCode) { rc = BERR_TRACE(errCode); goto err; }

        BDBG_MSG(("enable tone: %d",pSettings->toneEnabled));
        errCode = BDSQ_SetTone(dsqChannelHandle, pSettings->toneEnabled);
        if (errCode) { rc = BERR_TRACE(errCode); goto err; }

        if (pSatChannel->setVoltage) {
            errCode = pSatChannel->setVoltage(pSatChannel, pSettings->voltage);
        } else {
            errCode = BDSQ_SetVoltage(dsqChannelHandle, pSettings->voltage == NEXUS_FrontendDiseqcVoltage_e18v);
        }
        BDBG_MSG(("voltage: %d",pSettings->voltage == NEXUS_FrontendDiseqcVoltage_e18v ? 18 : 13));
        if (errCode) { rc = BERR_TRACE(errCode); goto err; }

        /* TODO: add power management */
        pSatChannel->diseqcSettings = *pSettings;
    }

    return NEXUS_SUCCESS;
err:
    return rc;
}

static NEXUS_Error NEXUS_Frontend_P_Sat_GetDiseqcStatus( void *handle, NEXUS_FrontendDiseqcStatus *pStatus )
{
    BERR_Code errCode;
    NEXUS_SatChannel *pSatChannel = handle;
    BDSQ_ChannelHandle dsqChannelHandle = NULL;
    BDSQ_Status dsqStatus;
    NEXUS_Error rc = NEXUS_SUCCESS;
    uint16_t voltage;

    BDBG_OBJECT_ASSERT(pSatChannel, NEXUS_SatChannel);
    BDBG_ASSERT(NULL != pStatus);

    NEXUS_Frontend_P_Sat_EnableDsqChannel(pSatChannel);

    if (pSatChannel->capabilities.diseqc) {
        if (pSatChannel->getDiseqcChannelHandle) {
            dsqChannelHandle = pSatChannel->getDiseqcChannelHandle(handle, pSatChannel->diseqcIndex);
        } else {
            dsqChannelHandle = pSatChannel->satDevice->dsqChannels[pSatChannel->diseqcIndex];
        }
        errCode = BDSQ_GetVoltage(dsqChannelHandle, &voltage);
        if (errCode) { rc = BERR_TRACE(errCode); }
        pStatus->voltage = voltage;

        errCode = BDSQ_GetTone(dsqChannelHandle, &pStatus->toneEnabled);
        if (errCode) { rc = BERR_TRACE(errCode); }

        errCode = BDSQ_GetStatus(dsqChannelHandle, &dsqStatus);
        if (errCode) { rc = BERR_TRACE(errCode); }

        BDBG_CASSERT(((unsigned)BDSQ_SendStatus_eSuccess == (unsigned)NEXUS_FrontendDiseqcMessageStatus_eSuccess));
        BDBG_CASSERT(((unsigned)BDSQ_SendStatus_eRxOverflow == (unsigned)NEXUS_FrontendDiseqcMessageStatus_eRxOverflow));
        BDBG_CASSERT(((unsigned)BDSQ_SendStatus_eRxReplyTimeout == (unsigned)NEXUS_FrontendDiseqcMessageStatus_eRxReplyTimeout));
        BDBG_CASSERT(((unsigned)BDSQ_SendStatus_eRxParityError == (unsigned)NEXUS_FrontendDiseqcMessageStatus_eRxParityError));
        BDBG_CASSERT(((unsigned)BDSQ_SendStatus_eAcwTimeout == (unsigned)NEXUS_FrontendDiseqcMessageStatus_eAcwTimeout));
        BDBG_CASSERT(((unsigned)BDSQ_SendStatus_eBusy == (unsigned)NEXUS_FrontendDiseqcMessageStatus_eBusy));
        pStatus->sendStatus = dsqStatus.status;
    } else {
        rc = NEXUS_NOT_SUPPORTED;
    }

    return rc;
}

static NEXUS_Error NEXUS_Frontend_P_Sat_SendDiseqcMessage(void *handle, const uint8_t *pSendData, size_t sendDataSize, const NEXUS_CallbackDesc *pSendComplete)
{
    NEXUS_SatChannel *pSatChannel = handle;
    BERR_Code errCode;
    BDSQ_ChannelHandle dsqChannelHandle = NULL;
    NEXUS_Error rc = NEXUS_SUCCESS;

    BDBG_OBJECT_ASSERT(pSatChannel, NEXUS_SatChannel);

    NEXUS_Frontend_P_Sat_EnableDsqChannel(pSatChannel);

    if (pSatChannel->getDiseqcChannelHandle) {
        dsqChannelHandle = pSatChannel->getDiseqcChannelHandle(handle, pSatChannel->diseqcIndex);
    } else {
        dsqChannelHandle = pSatChannel->satDevice->dsqChannels[pSatChannel->diseqcIndex];
    }

    if (pSatChannel->capabilities.diseqc)
    {
        BDSQ_EnableRx(dsqChannelHandle,false);
        NEXUS_TaskCallback_Set(pSatChannel->diseqcTxAppCallback, pSendComplete);
        errCode = BDSQ_Write(dsqChannelHandle, pSendData, sendDataSize);
        if (errCode)
        {
            NEXUS_TaskCallback_Set(pSatChannel->diseqcTxAppCallback, NULL);
            return BERR_TRACE(errCode);
        }
        BDSQ_EnableRx(dsqChannelHandle,true);
    }
    else
    {
        rc = BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    return rc;
}

static NEXUS_Error NEXUS_Frontend_P_Sat_GetDiseqcReply(void *handle, NEXUS_FrontendDiseqcMessageStatus *pStatus, uint8_t *pReplyBuffer, size_t replyBufferSize, size_t *pReplyLength)
{
    NEXUS_SatChannel *pSatChannel = handle;
    BERR_Code errCode;
    BDSQ_Status status;
    BDSQ_ChannelHandle dsqChannelHandle = NULL;

    BDBG_OBJECT_ASSERT(pSatChannel, NEXUS_SatChannel);

    BDBG_ASSERT(NULL != pStatus);
    BDBG_ASSERT(NULL != pReplyBuffer);
    BDBG_ASSERT(replyBufferSize > 0);
    BDBG_ASSERT(NULL != pReplyLength);

    NEXUS_Frontend_P_Sat_EnableDsqChannel(pSatChannel);

    if (pSatChannel->getDiseqcChannelHandle) {
        dsqChannelHandle = pSatChannel->getDiseqcChannelHandle(handle, pSatChannel->diseqcIndex);
    } else {
        dsqChannelHandle = pSatChannel->satDevice->dsqChannels[pSatChannel->diseqcIndex];
    }

    NEXUS_TaskCallback_Set(pSatChannel->diseqcRxAppCallback, NULL);   /* just to be safe */

    BDBG_CASSERT((NEXUS_FrontendDiseqcMessageStatus)BDSQ_SendStatus_eBusy == NEXUS_FrontendDiseqcMessageStatus_eBusy);
    errCode = BDSQ_GetStatus(dsqChannelHandle, &status);
    *pStatus = status.status;

    if (errCode || status.status != BDSQ_SendStatus_eSuccess) {
        *pReplyLength = 0; /* error condition, reply is invalid */
        return BERR_TRACE(errCode);
    }

    if (!status.bRxExpected) {
        status.nRxBytes = 0; /* force it */
    }

    *pReplyLength = replyBufferSize<status.nRxBytes?replyBufferSize:status.nRxBytes;
    if (*pReplyLength) {
        uint8_t remaining;
        uint8_t shortReplySize;
        uint8_t shortReplyLength;
        if (replyBufferSize > 0xFF)
            shortReplySize = 0xFF;
        else
            shortReplySize = (uint8_t)replyBufferSize;
        errCode = BDSQ_Read(dsqChannelHandle, pReplyBuffer, shortReplySize, &shortReplyLength, &remaining);
        if (errCode) {
            *pReplyLength = 0; /* error condition, reply is invalid */
            return BERR_TRACE(errCode);
        }
        *pReplyLength = shortReplyLength;
    }

    return NEXUS_SUCCESS;
}

static NEXUS_Error NEXUS_Frontend_P_Sat_ResetDiseqc(void *handle, uint8_t options)
{
    BERR_Code errCode;
    NEXUS_SatChannel *pSatChannel = handle;
    BDSQ_ChannelHandle dsqChannelHandle = NULL;
    NEXUS_Error rc = NEXUS_SUCCESS;

    BDBG_OBJECT_ASSERT(pSatChannel, NEXUS_SatChannel);

    BSTD_UNUSED(options);

    NEXUS_Frontend_P_Sat_EnableDsqChannel(pSatChannel);

    if (pSatChannel->getDiseqcChannelHandle) {
        dsqChannelHandle = pSatChannel->getDiseqcChannelHandle(handle, pSatChannel->diseqcIndex);
    } else {
        dsqChannelHandle = pSatChannel->satDevice->dsqChannels[pSatChannel->diseqcIndex];
    }

    errCode = BDSQ_ResetChannel(dsqChannelHandle);
    if (errCode) { rc = BERR_TRACE(errCode); }

    return rc;
}

static NEXUS_Error NEXUS_Frontend_P_Sat_SendDiseqcAcw(void *handle, uint8_t codeWord)
{
    BERR_Code errCode;
    NEXUS_SatChannel *pSatChannel = handle;
    BDSQ_ChannelHandle dsqChannelHandle = NULL;
    NEXUS_Error rc = NEXUS_SUCCESS;

    BDBG_OBJECT_ASSERT(pSatChannel, NEXUS_SatChannel);

    NEXUS_Frontend_P_Sat_EnableDsqChannel(pSatChannel);

    if (pSatChannel->getDiseqcChannelHandle) {
        dsqChannelHandle = pSatChannel->getDiseqcChannelHandle(handle, pSatChannel->diseqcIndex);
    } else {
        dsqChannelHandle = pSatChannel->satDevice->dsqChannels[pSatChannel->diseqcIndex];
    }

    errCode = BDSQ_SendACW(dsqChannelHandle, codeWord);
    if (errCode) { rc = BERR_TRACE(errCode); }

    return rc;
}

static NEXUS_Error NEXUS_Frontend_P_Sat_GetBertStatus( void *handle, NEXUS_FrontendBertStatus *pStatus )
{
    BERR_Code errCode;
    NEXUS_SatChannel *pSatChannel = handle;
    BSAT_BertStatus bertStatus;

    BDBG_OBJECT_ASSERT(pSatChannel, NEXUS_SatChannel);
    BDBG_ASSERT(NULL != pStatus);

    BKNI_Memset(pStatus, 0, sizeof(*pStatus));

    errCode = BSAT_GetBertStatus(pSatChannel->satChannel, &bertStatus);
    if ( errCode ) {
        BERR_TRACE(errCode);
    }
    pStatus->enabled = bertStatus.bEnabled;
    pStatus->locked = bertStatus.bLock;
    pStatus->syncLost = bertStatus.bSyncLost;
    pStatus->syncAcquired = bertStatus.bSyncAcquired;
    pStatus->bitCount = (((uint64_t)bertStatus.bitCountHi)<<32) + bertStatus.bitCountLo;
    pStatus->errorCount = (((uint64_t)bertStatus.errorCountHi)<<32) + bertStatus.errorCountLo;

    return NEXUS_SUCCESS;
}

static NEXUS_Error NEXUS_Frontend_P_Sat_ReadSoftDecisions(void *handle, NEXUS_FrontendSoftDecision *pDecisions, size_t length, size_t *pNumRead)
{
    size_t i;
    BERR_Code errCode;
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_SatChannel *pSatChannel = handle;
    int16_t *d_i, *d_q;

/* Value capped by the maximum data size available in the HAB buffer */
#define NEXUS_FRONTEND_SAT_MAX_SOFT_DECISIONS 124

    BDBG_OBJECT_ASSERT(pSatChannel, NEXUS_SatChannel);

    NEXUS_Frontend_P_Sat_EnableSatChannel(pSatChannel);

    if (length > NEXUS_FRONTEND_SAT_MAX_SOFT_DECISIONS)
        length = NEXUS_FRONTEND_SAT_MAX_SOFT_DECISIONS;

    d_i = BKNI_Malloc(length * sizeof(*d_i));
    d_q = BKNI_Malloc(length * sizeof(*d_q));
    if (!d_i || !d_q) {
        rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        goto done;
    }
    errCode = BSAT_GetSoftDecisions(pSatChannel->satChannel, length, d_i, d_q);
    if ( errCode )
    {
        rc = BERR_TRACE(errCode);
    }
    for( i=0; i<length; i++)
    {
        pDecisions[i].i = d_i[i] * 256 * 2;
        pDecisions[i].q = d_q[i] * 256 * 2;
    }
    *pNumRead = length;

done:
    if (d_q)
        BKNI_Free(d_q);
    if (d_i)
        BKNI_Free(d_i);
    return rc;
#undef NEXUS_FRONTEND_SAT_MAX_SOFT_DECISIONS
}

static NEXUS_Error NEXUS_Frontend_P_Sat_ReadSatelliteConfig( void *handle, unsigned id, void *buffer, unsigned bufferSize )
{
    NEXUS_SatChannel *pSatChannel = handle;
    BDBG_OBJECT_ASSERT(pSatChannel, NEXUS_SatChannel);
    BDBG_ASSERT(buffer);

    NEXUS_Frontend_P_Sat_EnableSatChannel(pSatChannel);

    if (bufferSize != 4) { /* for SAT-based platforms, this should always be a uint32_t */
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    return BSAT_GetChannelConfig(pSatChannel->satChannel, id, buffer);
}

static NEXUS_Error NEXUS_Frontend_P_Sat_WriteSatelliteConfig( void *handle, unsigned id, const void *buffer, unsigned bufferSize )
{
    NEXUS_SatChannel *pSatChannel = handle;
    const uint32_t *value = (uint32_t *)buffer;
    BDBG_OBJECT_ASSERT(pSatChannel, NEXUS_SatChannel);
    BDBG_ASSERT(buffer);

    NEXUS_Frontend_P_Sat_EnableSatChannel(pSatChannel);

    if (bufferSize != 4) { /* for SAT-based platforms, this should always be a uint32_t */
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    return BSAT_SetChannelConfig(pSatChannel->satChannel, id, *value);
}

static void NEXUS_Frontend_P_Sat_SpectrumDataReadyCallback(void *pParam)
{
    NEXUS_SatChannel *pSatChannel = (NEXUS_SatChannel *)pParam;

    BDBG_ASSERT(NULL != pParam);
    BDBG_OBJECT_ASSERT(pSatChannel, NEXUS_SatChannel);

    BDBG_MSG(("NEXUS_Frontend_P_Sat_SpectrumDataReadyCallback: %p",pParam));
    NEXUS_TaskCallback_Fire(pSatChannel->spectrumDataReadyAppCallback);
}

static void NEXUS_Frontend_P_Sat_SpectrumEventCallback(void *pParam)
{
    NEXUS_SatChannel *pSatChannel = (NEXUS_SatChannel *)pParam;
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(pSatChannel, NEXUS_SatChannel);
    BDBG_ASSERT(pParam);

    BDBG_MSG(("NEXUS_Frontend_P_Sat_SpectrumEventCallback: %p",pParam));
    BDBG_MSG(("NEXUS_Frontend_P_Sat_SpectrumEventCallback: fe: %p",(void *)pSatChannel->frontendHandle));
    BDBG_MSG(("NEXUS_Frontend_P_Sat_SpectrumEventCallback: [%p,%p,%d]",(void *)pSatChannel->spectrumDataReadyEventCallback, (void *)pSatChannel->spectrumDataPointer,pSatChannel->spectrumDataLength));
    rc = BWFE_GetSaSamples(pSatChannel->wfeHandle, pSatChannel->spectrumDataPointer);
    BKNI_SetEvent(pSatChannel->spectrumDataReadyEvent);
}

static NEXUS_Error NEXUS_Frontend_P_Sat_RequestSpectrumData(void *handle, const NEXUS_FrontendSpectrumSettings *pSettings)
{
    NEXUS_SatChannel *pSatChannel = (NEXUS_SatChannel *)handle;
    BERR_Code rc;

    BWFE_SpecAnalyzerParams saParams;

    BDBG_OBJECT_ASSERT(pSatChannel, NEXUS_SatChannel);

    NEXUS_Frontend_P_Sat_EnableSatChannel(pSatChannel);

    BDBG_MSG(("NEXUS_Frontend_P_Sat_RequestSpectrumData: %p",handle));

    if (pSatChannel->deviceClearSpectrumCallbacks)
    {
        /* set up Spectrum Analyzer Event here */
        BKNI_EventHandle saEvent;

        pSatChannel->deviceClearSpectrumCallbacks(handle);

        rc = BWFE_GetSaDoneEventHandle(pSatChannel->wfeHandle, &saEvent);
        if ( rc ) {
            return BERR_TRACE(rc);
        }
        BDBG_ASSERT(saEvent);
        pSatChannel->spectrumEventCallback = NEXUS_RegisterEvent(saEvent, NEXUS_Frontend_P_Sat_SpectrumEventCallback, pSatChannel);
        if ( !pSatChannel->spectrumEventCallback ) {
            return BERR_TRACE(NEXUS_UNKNOWN);
        }
    }

    saParams.freqStartHz = pSettings->startFrequency;
    saParams.freqStopHz = pSettings->stopFrequency;
    saParams.numSamples = pSettings->numSamples;
    saParams.numSweeps = 1;

    NEXUS_TaskCallback_Set(pSatChannel->spectrumDataReadyAppCallback, &(pSettings->dataReadyCallback));
    if (pSatChannel->spectrumDataReadyEventCallback) {
        NEXUS_UnregisterEvent(pSatChannel->spectrumDataReadyEventCallback);
    }
    pSatChannel->spectrumDataReadyEventCallback = NEXUS_RegisterEvent(pSatChannel->spectrumDataReadyEvent, NEXUS_Frontend_P_Sat_SpectrumDataReadyCallback, handle);
    pSatChannel->spectrumDataPointer = pSettings->data;
    pSatChannel->spectrumDataLength = pSettings->dataLength;

    BDBG_MSG(("NEXUS_Frontend_P_Sat_RequestSpectrumData: [%p,%p,%d]",(void *)pSatChannel->spectrumDataReadyEventCallback, (void *)pSatChannel->spectrumDataPointer,pSatChannel->spectrumDataLength));
    rc = BWFE_ScanSpectrum(pSatChannel->wfeChannel, &saParams);
    if (rc) { BERR_TRACE(rc); return NEXUS_INVALID_PARAMETER; }

    return NEXUS_SUCCESS;
}

/* Peak Scan functions */
static void NEXUS_Frontend_P_Sat_PeakscanEventHandler(void *pParam)
{
    NEXUS_SatChannel *pSatChannel = pParam;
    NEXUS_SatellitePeakscanStatus *psStatus = &pSatChannel->peakscanStatus;
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(pSatChannel, NEXUS_SatChannel);

    if (psStatus->curFreq > psStatus->endFreq) {
        BDBG_WRN(("curFreq: %d > endFreq: %d, exiting scan",psStatus->curFreq,psStatus->endFreq));
        goto done;
    }

    if (pSatChannel->symbolRateScan) {
        BSAT_SymbolRateScanStatus srStatus;
        /* get results of this step */
        errCode = BSAT_GetSymbolRateScanStatus(pSatChannel->satChannel, &srStatus);
        if (errCode) {
            BDBG_ERR(("BSAT_GetSymbolRateScanStatus() error %#x", errCode));
            BERR_TRACE(errCode);
            goto blind_step;
        }
        if (srStatus.status != 0) {
            BDBG_ERR(("BSAT_GetSymbolRateScanStatus() scan status error %u", srStatus.status));
            goto blind_step;
        }

        BDBG_MSG(("%d Hz: Fb=%d, power=%#x, last: %d", srStatus.tunerFreq, srStatus.symbolRate, srStatus.peakPower, psStatus->lastSymRate));

        if (srStatus.symbolRate == psStatus->lastSymRate) {
            psStatus->symRateCount++;
            if (psStatus->maxPeakPower < srStatus.peakPower) {
                psStatus->maxPeakPower = srStatus.peakPower;
                psStatus->maxPeakFreq = srStatus.tunerFreq;
            }
            if (pSatChannel->peakscanStatus.singleScan) {
                psStatus->lastSymRate = srStatus.symbolRate;
                goto done;
            }
        }
        else {
            if (psStatus->symRateCount > 0) {
                BDBG_MSG(("Potential signal found at %d Hz (%d sym/sec)", psStatus->maxPeakFreq, psStatus->lastSymRate));
                goto done;
            } else
            {
                /* save new symbol rate */
                psStatus->symRateCount = 0;
                psStatus->lastSymRate = srStatus.symbolRate;
                psStatus->maxPeakPower = srStatus.peakPower;
                psStatus->maxPeakFreq = srStatus.tunerFreq;
                if (pSatChannel->peakscanStatus.singleScan) {
                    goto done;
                }
            }
        }
blind_step:
        psStatus->curFreq += psStatus->stepFreq;

        if (psStatus->curFreq < psStatus->endFreq) {
            errCode = BSAT_StartSymbolRateScan(pSatChannel->satChannel, psStatus->curFreq, psStatus->minSymbolRate, psStatus->maxSymbolRate, pSatChannel->selectedAdc);
            if (errCode != BERR_SUCCESS) {
                BDBG_ERR(("BAST_PeakScan() (blind scan) error %#x. Peak scan terminated", errCode));
                errCode = BERR_TRACE(errCode);
                goto done;
            }
            else {
                return;
            }
        }
        else {
            if (psStatus->symRateCount > 0) {
                BDBG_WRN(("Potential signal found at %d Hz (%d sym/sec), but reached end of scan range",
                    psStatus->maxPeakFreq, psStatus->lastSymRate));
            }
            else {
                BDBG_WRN(("No signal found using peak scan"));
                psStatus->maxPeakFreq = 0;
                psStatus->lastSymRate = 0;
            }
            psStatus->curFreq -= psStatus->stepFreq;
            goto done;
        }
    }

    return;

done:
    psStatus->scanFinished = true;
    NEXUS_TaskCallback_Fire(pSatChannel->peakscanAppCallback);
}

static NEXUS_Error NEXUS_Frontend_P_Sat_SatellitePeakscan( void *handle, const NEXUS_FrontendSatellitePeakscanSettings *pSettings )
{
    BERR_Code errCode;

    NEXUS_SatChannel *pSatChannel = handle;
    NEXUS_SatellitePeakscanStatus *psStatus = &pSatChannel->peakscanStatus;

    NEXUS_Frontend_P_Sat_EnableSatChannel(pSatChannel);

    BDBG_MSG(("NEXUS_Frontend_P_Sat_SatellitePeakscan: f:%d, min:%d, max:%d, r:%d, s:%d cb:[%p,%p,0x%x]",
            pSettings->frequency,
            pSettings->minSymbolRate,
            pSettings->maxSymbolRate,
            pSettings->frequencyRange,
            pSettings->frequencyStep,
            (void *)(unsigned long)pSettings->peakscanCallback.callback,
            pSettings->peakscanCallback.context,
            pSettings->peakscanCallback.param
            ));

    BDBG_OBJECT_ASSERT(pSatChannel, NEXUS_SatChannel);

    if (!pSatChannel->peakscanEvent) {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }

    if (pSettings->frequency - pSettings->frequencyRange < NEXUS_SATELLITE_PEAKSCAN_MIN_FREQ ||
        pSettings->frequency + pSettings->frequencyRange > NEXUS_SATELLITE_PEAKSCAN_MAX_FREQ) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    NEXUS_TaskCallback_Set(pSatChannel->peakscanAppCallback, &pSettings->peakscanCallback);

    /* setup status variables */
    psStatus->curFreq = pSettings->frequency - pSettings->frequencyRange;
    psStatus->endFreq = pSettings->frequency + pSettings->frequencyRange;
    psStatus->stepFreq = pSettings->frequencyStep;
    psStatus->symRateCount = 0;
    psStatus->lastSymRate = 0;
    psStatus->maxPeakPower = 0;
    psStatus->maxPeakFreq = 0;
    psStatus->scanFinished = false;
    psStatus->minSymbolRate = pSettings->minSymbolRate;
    psStatus->maxSymbolRate = pSettings->maxSymbolRate;
    psStatus->singleScan = (pSettings->frequencyRange==0);

    if (psStatus->singleScan) {
        psStatus->endFreq = psStatus->curFreq+1;
    }

    pSatChannel->symbolRateScan = true;
    pSatChannel->toneSearch = false;

    errCode = BSAT_StartSymbolRateScan(pSatChannel->satChannel, psStatus->curFreq, psStatus->minSymbolRate, psStatus->maxSymbolRate, pSatChannel->selectedAdc);
    if (errCode != BERR_SUCCESS) {
        BDBG_ERR(("BSAT_StartSymbolRateScan error %#x. Peak scan (symbol rate scan) not initiated", errCode));
        return BERR_TRACE(errCode);
    }

    return NEXUS_SUCCESS;
}

static NEXUS_Error NEXUS_Frontend_P_Sat_GetSatellitePeakscanResult( void *handle, NEXUS_FrontendSatellitePeakscanResult *pResult )
{
    NEXUS_SatChannel *pSatChannel = handle;
    BDBG_OBJECT_ASSERT(pSatChannel, NEXUS_SatChannel);

    if (pSatChannel->peakscanEvent) {
        if (pSatChannel->peakscanStatus.scanFinished) {
            pResult->peakFrequency = pSatChannel->peakscanStatus.maxPeakFreq;
            pResult->symbolRate = pSatChannel->peakscanStatus.lastSymRate;
            pResult->lastFrequency= pSatChannel->peakscanStatus.curFreq;
            pResult->peakPower = pSatChannel->peakscanStatus.maxPeakPower;
            return NEXUS_SUCCESS;
        }
        else {
            return BERR_TRACE(NEXUS_NOT_INITIALIZED);
        }
    }
    else {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
}

static NEXUS_Error NEXUS_Frontend_P_Sat_SatelliteToneSearch( void *handle, const NEXUS_FrontendSatelliteToneSearchSettings *pSettings )
{
    NEXUS_Error rc = NEXUS_NOT_SUPPORTED;
    NEXUS_SatChannel *pSatChannel = handle;
    BDBG_OBJECT_ASSERT(pSatChannel, NEXUS_SatChannel);

    NEXUS_Frontend_P_Sat_EnableSatChannel(pSatChannel);

    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);

    return rc;
}

static NEXUS_Error NEXUS_Frontend_P_Sat_GetSatelliteToneSearchResult( void *handle, NEXUS_FrontendSatelliteToneSearchResult *pResult )
{
    NEXUS_SatChannel *pSatChannel = handle;
    BDBG_OBJECT_ASSERT(pSatChannel, NEXUS_SatChannel);

    NEXUS_Frontend_P_Sat_EnableSatChannel(pSatChannel);

    if (pSatChannel->peakscanEvent) {
        if (pSatChannel->peakscanStatus.scanFinished) {
            pResult->peakFrequency = pSatChannel->peakscanStatus.maxPeakFreq;
            pResult->lastFrequency= pSatChannel->peakscanStatus.curFreq;
            pResult->peakPower = pSatChannel->peakscanStatus.maxPeakPower;
            pResult->frequencyStep = pSatChannel->peakscanStatus.stepFreq;
            return NEXUS_SUCCESS;
        }
        else {
            return BERR_TRACE(NEXUS_NOT_INITIALIZED);
        }
    }
    else {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
}

static NEXUS_Error NEXUS_Frontend_P_Sat_GetSignalDetectStatus (void *handle, NEXUS_FrontendSatelliteSignalDetectStatus *pStatus)
{
    NEXUS_SatChannel *pSatChannel = handle;
    BERR_Code rc;
    BSAT_SignalDetectStatus status;
    BDBG_OBJECT_ASSERT(pSatChannel, NEXUS_SatChannel);

    NEXUS_Frontend_P_Sat_EnableSatChannel(pSatChannel);

    rc = BSAT_GetSignalDetectStatus(pSatChannel->satChannel, &status);
    if (rc) {
        return BERR_TRACE(rc);
    } else {
        pStatus->enabled = status.bEnabled;
        pStatus->detectionComplete = status.bDone;
        pStatus->signalDetected = status.bTimingLoopLocked;
    }

    return NEXUS_SUCCESS;
}
