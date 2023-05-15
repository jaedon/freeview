/******************************************************************************
 *    (c)2011-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_frontend_3472.c $
 * $brcm_Revision: 11 $
 * $brcm_Date: 12/6/12 4:35p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /nexus/modules/frontend/3472/src/nexus_frontend_3472.c $
 * 
 * 11   12/6/12 4:35p vishk
 * SW3472-36: Implement untune correctly and code cleanup
 * 
 * 10   12/6/12 4:07p vishk
 * SW3472-23: Enable signal quality percent and signal level percent for
 *  all layers.
 * 
 * 9   12/6/12 3:59p vishk
 * SW3461-206: Enable external gain support for 3472.
 * 
 * 8   12/5/12 6:20p vishk
 * SW3472-23: CLONE - CLONE - Add SSI and SQI for each "Layer" in HAB
 *  ISDBT Status Message - NEXUS
 * 
 * 7   11/12/12 5:52p vishk
 * SW7435-372: NEXUS_Frontend_GetSoftDecisions is slow and requires app to
 *  know internal query number for good performance
 * 
 * 6   10/26/12 2:53p vishk
 * SW3461-206: Add support for 3472 frontend.
 * 
 * 5   10/10/12 3:28p jtna
 * SW7425-3782: adapt to new TSMF api
 * 
 * 4   9/7/12 4:42p jrubio
 * SW7346-848: use new 3472 fw header and tnr header
 * 
 * 3   8/6/12 10:29p vishk
 * SW7425-3519: Fix warning.
 * 
 * 2   8/6/12 10:22p vishk
 * SW7425-3519: Add Standby functionality support for 3472.
 * 
 * 1   8/2/12 1:22p vishk
 * SW7346-848: Merge to mainline.
 * 
 * SW7346-848/7   7/27/12 2:42p vishk
 * SW7346-848: Populate Rs correctable and uncorrectable blocks with
 *  layerA and layerB status accumulated as one.
 * 
 * SW7346-848/6   7/27/12 2:06p vishk
 * SW7346-848: Populate Rs correctable and uncorrectable blocks with
 *  layerA status rather than LayerB.
 * 
 * SW7346-848/5   7/27/12 12:13p vishk
 * SW7346-848: Set the correct tuner application.
 * 
 * SW7346-848/4   7/26/12 2:53p vishk
 * SW7346-848: Remover unnecessary warning message.
 * 
 * SW7346-848/3   7/26/12 2:49p vishk
 * SW7346-848: Add status.
 * 
 * SW7346-848/2   7/23/12 6:37p vishk
 * SW7346-848: Fix channel number for ods channels.
 * 
 * SW7346-848/1   7/23/12 4:16p vishk
 * SW7346-848: Add support for BCM3472 BCM97346I2SFF platform.
 * 
 *****************************************************************************/
#include "nexus_frontend_module.h"
#include "nexus_platform_features.h"
#include "nexus_spi.h"
#include "priv/nexus_spi_priv.h"
#include "nexus_i2c.h"
#include "priv/nexus_i2c_priv.h"
#include "priv/nexus_gpio_priv.h"
#include "btnr.h"
#include "btnr_3472ib.h"
#include "bhab_3472.h"
#include "bhab_34xx_priv.h"
#include "bods_3472.h"
#include "bhab_3472_fw.h"
#include "bhab.h"
#include "bods.h"

#if NEXUS_HAS_MXT
#include "bmxt.h"
#include "bmxt_3472.h"
#endif

BDBG_MODULE(nexus_frontend_3472);

BDBG_OBJECT_ID(NEXUS_3472);

#define BCHIP_3472 0x3472

#define NEXUS_MAX_3472_FRONTENDS 2

#define NEXUS_3472_MAX_OFDM_CHN  NEXUS_MAX_3472_FRONTENDS

/* The 3472 has one downstream. */
#define NEXUS_MAX_3472_TUNERS  2

#define TOTAL_SOFTDECISIONS 30  

/***************************************************************************
 * Internal Module Structure
 ***************************************************************************/

typedef struct NEXUS_PreviousStatus
{
    unsigned fecCorrected;
    unsigned fecUncorrected;
    unsigned fecClean;
    unsigned viterbiUncorrectedBits;
    unsigned viterbiTotalBits;
    NEXUS_Time time;
}NEXUS_PreviousStatus;

typedef struct NEXUS_3472
{
    BDBG_OBJECT(NEXUS_3472) 
    BLST_S_ENTRY(NEXUS_3472) node;
    uint16_t  chipId;
    BHAB_Handle hab;
    unsigned    isrNumber;
    NEXUS_GpioHandle gpioInterrupt;
    uint16_t    i2cAddr;
    NEXUS_I2cHandle i2cHandle;
    uint16_t    spiAddr;
    NEXUS_SpiHandle spiHandle;
    unsigned    numfrontends;
    uint16_t revId; 
    BODS_Handle ods;
    BKNI_EventHandle isrEvent;
    NEXUS_EventCallbackHandle isrEventCallback;
    unsigned agcValue;                        /* Gain Value*/
    NEXUS_CallbackDesc updateGainCallbackDesc;    /* Callback will be called when the gain from the lna needs to be updated. */
    NEXUS_3461TunerRfInput rfInput;
    NEXUS_3461RfDaisyChain rfDaisyChain;
    bool enableRfLoopThrough;
    bool terrestrial;
    NEXUS_PreviousStatus previousQamStatus;
    BODS_SelectiveStatus odsStatus;
    bool isMtsif;
    NEXUS_FrontendDevice *pGenericDeviceHandle;
    unsigned                   count[NEXUS_MAX_3472_FRONTENDS];
    bool                       acquireInProgress[NEXUS_MAX_3472_FRONTENDS];
    NEXUS_FrontendHandle       frontendHandle[NEXUS_MAX_3472_FRONTENDS];
    unsigned                   numChannels[BODS_Standard_eLast];
    BTNR_Handle                tnr[NEXUS_MAX_3472_TUNERS]; 
    BODS_ChannelHandle         ods_chn[NEXUS_3472_MAX_OFDM_CHN];
    NEXUS_FrontendQamSettings  last_qam[NEXUS_MAX_3472_TUNERS];
    NEXUS_FrontendOfdmSettings last_ofdm[NEXUS_MAX_3472_TUNERS];
    NEXUS_IsrCallbackHandle    lockAppCallback[NEXUS_MAX_3472_FRONTENDS];
    NEXUS_IsrCallbackHandle    asyncStatusAppCallback[NEXUS_MAX_3472_FRONTENDS];
    NEXUS_IsrCallbackHandle    updateGainAppCallback[NEXUS_MAX_3472_TUNERS];
    bool                       isInternalAsyncStatusCall[NEXUS_MAX_3472_FRONTENDS];
    bool                       isPoweredOn[NEXUS_MAX_3472_FRONTENDS];
    NEXUS_FrontendOfdmAcquisitionMode lastAcquisitionMode[NEXUS_MAX_3472_FRONTENDS];
#ifdef NEXUS_SHARED_FRONTEND_INTERRUPT
    bool isMaster;
#endif  
#if NEXUS_HAS_MXT
    BMXT_Handle mxt;
    NEXUS_TsmfSettings tsmfSettings[NEXUS_MAX_3472_FRONTENDS];    
    NEXUS_FrontendDeviceMtsifConfig mtsifConfig;
#endif
} NEXUS_3472;

static BLST_S_HEAD(devList, NEXUS_3472) g_3472DevList = BLST_S_INITIALIZER(g_3472DevList);

typedef struct NEXUS_3472Channel
{
    unsigned chn_num; /* channel number */
    NEXUS_3472 *pDevice; /* 3472 device*/
} NEXUS_3472Channel;

/***************************************************************************
 * Module callback functions for tuning
 ***************************************************************************/
static void NEXUS_Frontend_P_3472_UnTune(void *handle);
static void NEXUS_Frontend_P_3472_ResetStatus(void *handle);
static NEXUS_Error NEXUS_Frontend_P_3472_ReadSoftDecisions(void *handle, NEXUS_FrontendSoftDecision *pDecisions, size_t length, size_t *pNumRead);
static uint16_t NEXUS_Frontend_P_Get3472Rev(const NEXUS_FrontendDevice3461OpenSettings *pSettings);
static void NEXUS_Frontend_P_3472_Close(NEXUS_FrontendHandle handle);
static void NEXUS_Frontend_P_3472_GetType(void *handle, NEXUS_FrontendType *type);
static NEXUS_Error NEXUS_Frontend_P_3472_GetFastStatus(void *handle, NEXUS_FrontendFastStatus *pStatus);
static NEXUS_Error NEXUS_Frontend_P_3472_TuneOfdm(void *handle, const NEXUS_FrontendOfdmSettings *pSettings);
static NEXUS_Error NEXUS_Frontend_P_3472_GetOfdmStatus(void *handle, NEXUS_FrontendOfdmStatus *pStatus);
static NEXUS_Error NEXUS_Frontend_P_3472_GetOfdmAsyncStatus(void *handle, NEXUS_FrontendOfdmStatus *pStatus);
static NEXUS_Error NEXUS_Frontend_P_3472_RequestOfdmAsyncStatus(void *handle);
static NEXUS_Error NEXUS_Frontend_P_3472_Standby(void *handle, bool enabled, const NEXUS_StandbySettings *pSettings);

static NEXUS_Error NEXUS_Frontend_P_3472_ReapplyTransportSettings(void *handle);

NEXUS_Error NEXUS_Frontend_P_3472_GetInternalGain(void *handle, const NEXUS_GainParameters *params, NEXUS_InternalGainSettings *pSettings);
NEXUS_Error NEXUS_Frontend_P_3472_GetExternalGain(void *handle, NEXUS_ExternalGainSettings *pSettings);
NEXUS_Error NEXUS_Frontend_P_3472_SetExternalGain(void *handle, const NEXUS_ExternalGainSettings *pSettings);
#if NEXUS_AMPLIFIER_SUPPORT
NEXUS_Error NEXUS_Frontend_P_3472_GetAmplifierStatus(void *handle, NEXUS_AmplifierStatus *pStatus);
NEXUS_Error NEXUS_Frontend_P_3472_SetAmplifierStatus(void *handle, const NEXUS_AmplifierStatus *pStatus);
#endif

/***************************************************************************
Summary:
    Lock callback handler for a 3472 Inband device
 ***************************************************************************/
static void NEXUS_Frontend_P_3472_callback_isr(void *pParam)
{ 
    NEXUS_3472Channel *pChannel;
    NEXUS_3472 *pDevice;    
    BDBG_ASSERT(NULL != pParam);
    pChannel = (NEXUS_3472Channel *)pParam;
    pDevice = pChannel->pDevice;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3472);
    
    if(pDevice->acquireInProgress[pChannel->chn_num]){
        pDevice->count[pChannel->chn_num]++;
    }
    if(pDevice->count[pChannel->chn_num] == 2){
        pDevice->acquireInProgress[pChannel->chn_num] = false;
        pDevice->count[pChannel->chn_num] = 0;
    }

    if ( pDevice->lockAppCallback[pChannel->chn_num] )
    {
        NEXUS_IsrCallback_Fire_isr(pDevice->lockAppCallback[pChannel->chn_num]);
    }
}

/***************************************************************************
Summary:
    Lock callback handler for a 3472 Inband device
 ***************************************************************************/
static void NEXUS_Frontend_P_3472_AsyncStatusCallback_isr(void *pParam)
{   
    NEXUS_3472Channel *pChannel;
    NEXUS_3472 *pDevice;    
    BDBG_ASSERT(NULL != pParam);
    pChannel = (NEXUS_3472Channel *)pParam;
    pDevice = pChannel->pDevice;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3472);
    
    if(!pDevice->isInternalAsyncStatusCall[pChannel->chn_num]){
        if (pDevice->asyncStatusAppCallback[pChannel->chn_num])
        {
            NEXUS_IsrCallback_Fire_isr(pDevice->asyncStatusAppCallback[pChannel->chn_num]);
        }
    }
}

/***************************************************************************
Summary:
    Enable/Disable interrupts for a 3472 device
 ***************************************************************************/
static void NEXUS_Frontend_P_3472_L1_isr(void *pParam1, int pParam2)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BHAB_Handle hab = (BHAB_Handle)pParam2;
#ifdef NEXUS_SHARED_FRONTEND_INTERRUPT
    NEXUS_3472 *pDevice, *pIntDevice = (NEXUS_3472 *)pParam1;
#else
    BSTD_UNUSED(pParam1);
#endif

    rc = BHAB_HandleInterrupt_isr(hab);
    if(rc){rc = BERR_TRACE(rc); goto done;}

#ifdef NEXUS_SHARED_FRONTEND_INTERRUPT
    for ( pDevice = BLST_S_FIRST(&g_3472DevList); NULL != pDevice; pDevice = BLST_S_NEXT(pDevice, node) )
    {
        BDBG_OBJECT_ASSERT(pDevice, NEXUS_3472);

        if ((pDevice->hab != NULL) && (pDevice->isrNumber== pIntDevice->isrNumber) && (pDevice->gpioInterrupt == pIntDevice->gpioInterrupt) && (pDevice->hab != hab))
        {
            rc = BHAB_HandleInterrupt_isr(pDevice->hab);
            if(rc){rc = BERR_TRACE(rc); goto done;}
        }
    }
#endif
done:
    return;
}
/***************************************************************************
Summary:
    Enable/Disable interrupts for a 3472 device
 ***************************************************************************/
static void NEXUS_Frontend_P_3472_GpioIsrControl_isr(bool enable, void *pParam)
{
    NEXUS_GpioHandle gpioHandle = (NEXUS_GpioHandle)pParam;

    if(enable){ 
        NEXUS_Gpio_SetInterruptEnabled_isr(gpioHandle, true);
    }
    else {
        NEXUS_Gpio_SetInterruptEnabled_isr(gpioHandle, false);
    }   
}

/***************************************************************************
Summary:
    Enable/Disable interrupts for a 3472 device
 ***************************************************************************/
static void NEXUS_Frontend_P_3472_IsrControl_isr(bool enable, void *pParam)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    int isrnumber = (int)pParam;
    
    if ( enable )
    {
        rc = NEXUS_Core_EnableInterrupt_isr(isrnumber);     
        if(rc) BERR_TRACE(rc); 
    }
    else
    {
        NEXUS_Core_DisableInterrupt_isr(isrnumber);
    }
}

/***************************************************************************
Summary:
    ISR Event Handler for a 3472 device
 ***************************************************************************/
static void NEXUS_Frontend_P_3472_IsrEvent(void *pParam)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BHAB_Handle hab = (BHAB_Handle)pParam;
  
    rc = BHAB_ProcessInterruptEvent(hab);
    if(rc) BERR_TRACE(rc);
}
/***************************************************************************
Summary:
    Initialize Hab for a 3472 device
***************************************************************************/
void NEXUS_Frontend_P_UnInit_3472_Hab(NEXUS_3472 *pDevice)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_GpioSettings gpioSettings;

    if(pDevice->isrNumber) {
#ifdef NEXUS_SHARED_FRONTEND_INTERRUPT
        if(pDevice->isMaster)
        {
            NEXUS_Core_DisconnectInterrupt(pDevice->isrNumber);
        }
#else
        NEXUS_Core_DisconnectInterrupt(pDevice->isrNumber);
#endif 
    }
    else if(pDevice->gpioInterrupt){    
#ifdef NEXUS_SHARED_FRONTEND_INTERRUPT
        if(pDevice->isMaster)
        {
            NEXUS_Gpio_SetInterruptCallback_priv(pDevice->gpioInterrupt, NULL, NULL, 0);
            NEXUS_Gpio_GetSettings(pDevice->gpioInterrupt,  &gpioSettings);
            gpioSettings.interruptMode = NEXUS_GpioInterrupt_eDisabled;
            gpioSettings.interrupt.callback = NULL;
            rc = NEXUS_Gpio_SetSettings(pDevice->gpioInterrupt, &gpioSettings);
            if(rc){rc = BERR_TRACE(rc); goto done;}
            
        }
#else
        NEXUS_Gpio_SetInterruptCallback_priv(pDevice->gpioInterrupt, NULL, NULL, 0);
        NEXUS_Gpio_GetSettings(pDevice->gpioInterrupt,  &gpioSettings);
        gpioSettings.interruptMode = NEXUS_GpioInterrupt_eDisabled;
        gpioSettings.interrupt.callback = NULL;
        NEXUS_Gpio_SetSettings(pDevice->gpioInterrupt, &gpioSettings);
        if(rc){rc = BERR_TRACE(rc); goto done;}
#endif    
    }

    if(pDevice->isrEvent) pDevice->isrEvent = NULL; 
    if(pDevice->isrEventCallback)NEXUS_UnregisterEvent(pDevice->isrEventCallback);
    pDevice->isrEventCallback = NULL;   
#ifdef NEXUS_SHARED_FRONTEND_INTERRUPT
    BKNI_EnterCriticalSection();
    BLST_S_REMOVE(&g_3472DevList, pDevice, NEXUS_3472, node);
    BKNI_LeaveCriticalSection();
    if(pDevice->hab) { 
        rc = BHAB_Close(pDevice->hab);
        if(rc){rc = BERR_TRACE(rc); goto done;}
    }
#else
    if(pDevice->hab) { 
        rc = BHAB_Close(pDevice->hab);
        if(rc){rc = BERR_TRACE(rc); goto done;}
    }
#endif
    pDevice->hab = NULL;
done:
    return;
}

/***************************************************************************
Summary:
    Initialize Hab for a 3472 device
***************************************************************************/
NEXUS_Error NEXUS_Frontend_P_Init_3472_Hab(NEXUS_3472 *pDevice, const NEXUS_FrontendDevice3461OpenSettings *pSettings)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BHAB_Settings stHabSettings;
    BREG_I2C_Handle i2cHandle;
    BREG_SPI_Handle spiHandle;
#ifdef NEXUS_SHARED_FRONTEND_INTERRUPT  
    NEXUS_3472 *ptempDevice = BLST_S_FIRST(&g_3472DevList);
#endif   
    rc = BHAB_3472_GetDefaultSettings(&stHabSettings);
    if(rc){rc = BERR_TRACE(rc); goto done;}


    stHabSettings.chipAddr = pSettings->i2cAddr;
    stHabSettings.slaveChipAddr = pSettings->i2cSlaveAddr;
    stHabSettings.isMtsif = pSettings->isMtsif;

    if(pSettings->isrNumber) {
        stHabSettings.interruptEnableFunc = NEXUS_Frontend_P_3472_IsrControl_isr;
        stHabSettings.interruptEnableFuncParam = (void*)pSettings->isrNumber;
    }
    else if(pSettings->gpioInterrupt){
        stHabSettings.interruptEnableFunc = NEXUS_Frontend_P_3472_GpioIsrControl_isr;
        stHabSettings.interruptEnableFuncParam = (void*)pSettings->gpioInterrupt;
    }
    
    if(pSettings->i2cDevice ){
        stHabSettings.chipAddr = pSettings->i2cAddr;
        i2cHandle = NEXUS_I2c_GetRegHandle(pSettings->i2cDevice, NEXUS_MODULE_SELF);
        if(i2cHandle == NULL ){rc = BERR_TRACE(NEXUS_NOT_INITIALIZED); goto done;}
        stHabSettings.isSpi = false;
        rc = BHAB_Open( &pDevice->hab, (void *)i2cHandle, &stHabSettings);
        if(rc){rc = BERR_TRACE(rc); goto done;}
    }
    else if(pSettings->spiDevice ){
        stHabSettings.chipAddr = pSettings->spiAddr;    
        spiHandle = NEXUS_Spi_GetRegHandle(pSettings->spiDevice);
        if(spiHandle == NULL ){rc = BERR_TRACE(NEXUS_NOT_INITIALIZED); goto done;}
        stHabSettings.isSpi = true;
        rc = BHAB_Open( &pDevice->hab, (void *)spiHandle, &stHabSettings);
        if(rc){rc = BERR_TRACE(rc); goto done;}
    }

#ifdef NEXUS_SHARED_FRONTEND_INTERRUPT
    /* disconnect the previous for shared interrupt, last one is master */
    if(ptempDevice != NULL){
        if(pSettings->isrNumber) {
            NEXUS_Core_DisconnectInterrupt(pSettings->isrNumber);
        }
        else if(pSettings->gpioInterrupt) {
            NEXUS_Gpio_SetInterruptCallback_priv(pSettings->gpioInterrupt, NULL, NULL, 0);
        }
    }
#endif

    /* Success opeining Hab.  Connect Interrupt */
    if(pSettings->isrNumber) {      
        rc = NEXUS_Core_ConnectInterrupt(pSettings->isrNumber, NEXUS_Frontend_P_3472_L1_isr, (void *)pDevice, (int)pDevice->hab);        
        if(rc){rc = BERR_TRACE(rc); goto done;}
    }
    else if(pSettings->gpioInterrupt){
        NEXUS_Gpio_SetInterruptCallback_priv(pSettings->gpioInterrupt, NEXUS_Frontend_P_3472_L1_isr, (void *)pDevice, (int)pDevice->hab);
    }

    if (pSettings->loadAP) 
    {
        rc = BHAB_InitAp(pDevice->hab, bcm3472_leap_image);
        if(rc){rc = BERR_TRACE(rc); goto done;}
    }

    /* Get events and register callbacks */
    rc = BHAB_GetInterruptEventHandle(pDevice->hab, &pDevice->isrEvent);
    if(rc){rc = BERR_TRACE(rc); goto done;}
    
    pDevice->isrEventCallback = NEXUS_RegisterEvent(pDevice->isrEvent, NEXUS_Frontend_P_3472_IsrEvent, pDevice->hab);
    if ( NULL == pDevice->isrEventCallback ){rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY); goto done; }
    
    return BERR_SUCCESS;

done:
    NEXUS_Frontend_P_UnInit_3472_Hab(pDevice);
    return rc;
}

/***************************************************************************
Summary:
    Get the default settings for a BCM3472 tuner
See Also:
    NEXUS_Frontend_Open3472
 ***************************************************************************/
void NEXUS_Frontend_GetDefault3461Settings(NEXUS_3461Settings *pSettings)
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->type = NEXUS_3461ChannelType_eIsdbt;
    pSettings->channelNumber = 0;
}
/***************************************************************************
Summary:
    UnInitialize all ADS/THD/TC2 channels.
 ***************************************************************************/
void NEXUS_Frontend_P_UnInit3472(NEXUS_3472 *pDevice)
{
    NEXUS_Error rc = BERR_SUCCESS;
    unsigned int i;

#if NEXUS_HAS_MXT
    if (pDevice->mxt) {
        BMXT_Close(pDevice->mxt);
    }
#endif

    for ( i = 0; i < NEXUS_MAX_3472_TUNERS && NULL != pDevice->tnr[i]; i++) {
        if(pDevice->tnr[i]){
            rc = BTNR_Close(pDevice->tnr[i]);
            if(rc){rc = BERR_TRACE(rc); goto done;}         
            pDevice->tnr[i] = NULL;
        }
    }
    for ( i = 0; i < NEXUS_3472_MAX_OFDM_CHN && NULL != pDevice->ods_chn[i]; i++) {
        if(pDevice->ods_chn[i]){ 
            rc = BODS_CloseChannel(pDevice->ods_chn[i]);
            if(rc){rc = BERR_TRACE(rc); goto done;}
            (pDevice->ods_chn[i]) = NULL;
        }
    }
    if (pDevice->ods) {
        rc = BODS_Close(pDevice->ods);
        if(rc){rc = BERR_TRACE(rc); goto done;}
        pDevice->ods = NULL;
    }

done:
    return;
}

/***************************************************************************
Summary:
    Initialize all ADS/THD/TC2 channels.
 ***************************************************************************/
static NEXUS_Error NEXUS_Frontend_P_Init3472(NEXUS_3472 *pDevice)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    unsigned int i, j;
    BTNR_3472_Settings tnr3472_cfg;
    BODS_Settings odsCfg;
    BODS_ChannelSettings odsChnCfg;
    BTNR_PowerSaverSettings pwrSettings;

    rc = BODS_3472_GetDefaultSettings( &odsCfg, NULL);
    if(rc){rc = BERR_TRACE(rc); goto done;}

    odsCfg.hGeneric = pDevice->hab;
    rc = BODS_Open(&pDevice->ods, NULL, NULL, NULL, &odsCfg);
    if(rc){rc = BERR_TRACE(rc); goto done;}

    rc = BODS_Init(pDevice->ods);
    if(rc){rc = BERR_TRACE(rc); goto done;}

    for (i=0;i<BODS_Standard_eLast;i++) {
        rc = BODS_GetTotalChannels(pDevice->ods, (BODS_Standard)i, &pDevice->numChannels[i]);
        if(rc){rc = BERR_TRACE(rc); goto done;}
        
        if(pDevice->numChannels[i] > NEXUS_MAX_3472_TUNERS) {
            BDBG_MSG(("The maximum number of channels is incorrect. num_ch = %d, standard = %d", pDevice->numChannels[i], i));
            rc = BERR_TRACE(BERR_NOT_SUPPORTED); goto done;
        }
        
        rc = BODS_GetChannelDefaultSettings( pDevice->ods, (BODS_Standard)i, &odsChnCfg);
        if(rc){rc = BERR_TRACE(rc); goto done;}

        /* This needs to be fixed for every standard type. */
        for (j=0;j<pDevice->numChannels[i];j++) {
            odsChnCfg.channelNo = j;
            odsChnCfg.standard = i;
            rc = BODS_OpenChannel( pDevice->ods, &pDevice->ods_chn[j], &odsChnCfg);
            if(rc){rc = BERR_TRACE(rc); goto done;}
        }
    }

    for (i=0;i<NEXUS_MAX_3472_TUNERS;i++) {
        rc = BTNR_3472_GetDefaultSettings(&tnr3472_cfg);
        if(rc){rc = BERR_TRACE(rc); goto done;}
        tnr3472_cfg.channelNo = i;
        rc =  BTNR_3472_Open(&pDevice->tnr[i],&tnr3472_cfg, pDevice->hab);
        if(rc){rc = BERR_TRACE(rc); goto done;}

        pwrSettings.enable = false;
        rc = BTNR_SetPowerSaver(pDevice->tnr[i], &pwrSettings);
        if(rc){rc = BERR_TRACE(rc); goto done;}
    }

#if NEXUS_HAS_MXT
    {
        /* open MXT */
        BMXT_Settings mxtSettings;
        BMXT_3472_GetDefaultSettings(&mxtSettings);
        for (i=0; i<BMXT_MAX_NUM_MTSIF_TX; i++) {
            mxtSettings.MtsifTxCfg[i].Enable = pDevice->isMtsif; /* note, .isMtsif passed to BHAB_Open is independent of this */
        }
        for (i=0; i<BMXT_MAX_NUM_MTSIF_RX; i++) {
            if(pDevice->i2cAddr == 0x6e) { /* TODO: hack */
                mxtSettings.MtsifRxCfg[i].Enable = true;
            }
        }
        mxtSettings.hHab = pDevice->hab;
        /* TODO: set mxtSettings.chipRev correctly. 3472 HW is the same for A0/B0/C0 though, so we're not missing out */
        rc = BMXT_Open(&pDevice->mxt, NULL, NULL, &mxtSettings);
        if (rc!=BERR_SUCCESS) goto done;

{
        BMXT_ParserConfig parserConfig;
        BKNI_Memset(&pDevice->mtsifConfig, 0, sizeof(NEXUS_FrontendDeviceMtsifConfig));
        pDevice->mtsifConfig.pMxtHandle = pDevice->mxt;
        pDevice->mtsifConfig.numDemodIb = BMXT_GetNumResources(mxtSettings.chip, mxtSettings.chipRev, BMXT_ResourceType_eInputBand);
        pDevice->mtsifConfig.numDemodPb = BMXT_GetNumResources(mxtSettings.chip, mxtSettings.chipRev, BMXT_ResourceType_eParser);

        pDevice->mtsifConfig.chained = true;

        for (i=0; i<pDevice->mtsifConfig.numDemodPb; i++) {
            rc = BMXT_GetParserConfig(pDevice->mxt, i, &parserConfig);
            if (rc) { rc = BERR_TRACE(rc); goto done; }

            /* turn off demod parsers */
            parserConfig.Enable = false;

            /* cache other settings */
            pDevice->mtsifConfig.demodPbSettings[i].errorInputIgnore = parserConfig.ErrorInputIgnore;
            pDevice->mtsifConfig.demodPbSettings[i].timestampMode = (unsigned)parserConfig.TsMode;
            pDevice->mtsifConfig.demodPbSettings[i].dssMode = parserConfig.DssMode;

            rc = BMXT_SetParserConfig(pDevice->mxt, i, &parserConfig);
            if (rc) { rc = BERR_TRACE(rc); goto done; }
        }
}

    }
#endif

    return BERR_SUCCESS;
    
done:
    NEXUS_Frontend_P_UnInit3472(pDevice);
    return rc;
}

static uint16_t NEXUS_Frontend_P_Get3472Rev(const NEXUS_FrontendDevice3461OpenSettings *pSettings)
{   
    NEXUS_Error rc = NEXUS_SUCCESS;
    BREG_I2C_Handle i2cHandle;
    uint8_t buf[2];
    uint16_t revId=0xef;
    uint8_t subAddr;
    uint8_t wData[2], rData[4];

    if(pSettings->i2cDevice){
        i2cHandle = NEXUS_I2c_GetRegHandle(pSettings->i2cDevice, NULL); 
        if(i2cHandle == NULL ){rc = BERR_TRACE(NEXUS_NOT_INITIALIZED); goto done;}
        buf[0]= 0x0;
        subAddr = 0x3;
        rc = BREG_I2C_WriteNoAddr(i2cHandle, pSettings->i2cAddr, (uint8_t *)&subAddr, 1);
        if(rc){rc = BERR_TRACE(rc); goto done;}
        rc = BREG_I2C_ReadNoAddr(i2cHandle, pSettings->i2cAddr, buf, 1);
        if(rc){rc = BERR_TRACE(rc); goto done;}
        revId = buf[0];

        subAddr = 0x4;
        rc = BREG_I2C_WriteNoAddr(i2cHandle, pSettings->i2cAddr, (uint8_t *)&subAddr, 1);
        if(rc){rc = BERR_TRACE(rc); goto done;}
        rc = BREG_I2C_ReadNoAddr(i2cHandle, pSettings->i2cAddr, buf, 1);
        if(rc){rc = BERR_TRACE(rc); goto done;}
        revId = (revId <<8) | buf[0];
    }
    else if(pSettings->spiDevice){
        wData[0] = pSettings->spiAddr << 1;   /* Chip address is 0x40 << 1 */
        wData[1] = 0x03;                      /* Register offset. */

        rc = NEXUS_Spi_Read(pSettings->spiDevice, wData, rData, 4);
        if(rc){rc = BERR_TRACE(rc); goto done;}

        revId = (rData[2] <<8) | rData[3];        
    }

    return revId;
done:   
    return 0;   
}

static uint16_t NEXUS_Frontend_P_Is3472(const NEXUS_FrontendDevice3461OpenSettings *pSettings)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BREG_I2C_Handle i2cHandle;
    uint8_t buf[2];
    uint8_t subAddr;
    uint16_t chipId=0;
    uint8_t wData[2], rData[4];
    
    if(pSettings->i2cDevice){
        i2cHandle = NEXUS_I2c_GetRegHandle(pSettings->i2cDevice, NULL);
        if(i2cHandle == NULL ){rc = BERR_TRACE(NEXUS_NOT_INITIALIZED); goto done;}
        buf[0]= 0x0;
        subAddr = 0x1;
        rc = BREG_I2C_WriteNoAddr(i2cHandle, pSettings->i2cAddr, (uint8_t *)&subAddr, 1);
        if(rc){rc = BERR_TRACE(rc); goto done;}
        rc = BREG_I2C_ReadNoAddr(i2cHandle, pSettings->i2cAddr, buf, 1);
        if(rc){rc = BERR_TRACE(rc); goto done;}
        chipId = buf[0];
        subAddr = 0x2;
        rc = BREG_I2C_WriteNoAddr(i2cHandle, pSettings->i2cAddr, (uint8_t *)&subAddr, 1);
        if(rc){rc = BERR_TRACE(rc); goto done;}
        rc = BREG_I2C_ReadNoAddr(i2cHandle, pSettings->i2cAddr, buf, 1);
        if(rc){rc = BERR_TRACE(rc); goto done;}
        chipId = (chipId <<8) | buf[0]; 
    }
    else if (pSettings->spiDevice){
        wData[0] = pSettings->spiAddr << 1;   /* Chip address is 0x40 << 1 */
        wData[1] = 0x01;                      /* Register offset. */

        rc = NEXUS_Spi_Read(pSettings->spiDevice, wData, rData, 4);
        if(rc){rc = BERR_TRACE(rc); goto done;}

        chipId = (rData[2] <<8) | rData[3];
    }
    return chipId;
done:   
    return 0;
}

#if NEXUS_AMPLIFIER_SUPPORT
NEXUS_Error NEXUS_Frontend_P_3472_GetAmplifierStatus(void *handle, NEXUS_AmplifierStatus *pStatus)
{     
    NEXUS_Error  rc = NEXUS_SUCCESS;
    NEXUS_3472 *pDevice;
    BDBG_ASSERT(NULL != handle);
    pDevice = (NEXUS_3472 *)handle;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3472);

    BKNI_Memset(pStatus, 0, sizeof(*pStatus));

    if(pDevice->pGenericDeviceHandle->parent != NULL){
        rc = NEXUS_Frontend_P_GetAmplifierStatus(pDevice->pGenericDeviceHandle->parent, pStatus);
        if(rc){rc = BERR_TRACE(rc); goto done;}
    }
    else if(pDevice->pGenericDeviceHandle->amplifier != NULL){
        rc = NEXUS_Amplifier_GetStatus(pDevice->pGenericDeviceHandle->amplifier, pStatus);
        if(rc){rc = BERR_TRACE(rc); goto done;}
    }
    else {
        BDBG_ERR(("Amplifier not linked to the parent device."));
    }
    
done:
    return rc;
}

NEXUS_Error NEXUS_Frontend_P_3472_SetAmplifierStatus(void *handle, const NEXUS_AmplifierStatus *pStatus)
{
    NEXUS_Error  rc = NEXUS_SUCCESS;
    NEXUS_3472 *pDevice;
    BDBG_ASSERT(NULL != handle);
    pDevice = (NEXUS_3472 *)handle;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3472);

    BSTD_UNUSED(pStatus);
    /* Set LNA/Amplifier parameters. */

    return rc;
}
#endif

NEXUS_Error NEXUS_Frontend_P_3472_GetInternalGain(void *handle, const NEXUS_GainParameters *params, NEXUS_InternalGainSettings *pSettings)
{   
    NEXUS_Error  rc = NEXUS_SUCCESS;
    NEXUS_3472 *pDevice;
    BHAB_InternalGainInputParams inputParams;
    BHAB_InternalGainSettings internalGain;
    NEXUS_GainParameters gainParams;
    BDBG_ASSERT(NULL != handle);
    pDevice = (NEXUS_3472 *)handle;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3472);

    pSettings->frequency = params->frequency;
    inputParams.frequency = params->frequency;

    rc = BHAB_GetInternalGain(pDevice->hab, &inputParams, &internalGain);
    if(rc){rc = BERR_TRACE(rc); goto done;}

    BKNI_Memset(pSettings, 0, sizeof(*pSettings));

    if(params->accumulateTillRootDevice){
        if(pDevice->pGenericDeviceHandle->parent){            
            gainParams.rfInput = pDevice->pGenericDeviceHandle->linkSettings.rfInput;
            gainParams.accumulateTillRootDevice = params->accumulateTillRootDevice;
            gainParams.frequency = params->frequency;
            rc = NEXUS_Frontend_P_GetInternalGain(pDevice->pGenericDeviceHandle->parent, &gainParams, pSettings);
            if(rc){rc = BERR_TRACE(rc); goto done;}
        }
    }

    if(params->rfInput == NEXUS_FrontendDeviceRfInput_eDaisy){
        pSettings->totalVariableGain += internalGain.internalGainDaisy;
    }
    else if(params->rfInput == NEXUS_FrontendDeviceRfInput_eLoopThrough){
        pSettings->totalVariableGain += internalGain.internalGainLoopThrough;
    }

    pSettings->daisyGain += internalGain.internalGainDaisy;
    pSettings->loopThroughGain += internalGain.internalGainLoopThrough;
done:
    return rc;
}
NEXUS_Error NEXUS_Frontend_P_3472_GetExternalGain(void *handle, NEXUS_ExternalGainSettings *pSettings)
{
    NEXUS_Error  rc = NEXUS_SUCCESS;
    NEXUS_3472 *pDevice;
    BHAB_ExternalGainSettings gain;
    BDBG_ASSERT(NULL != handle);
    pDevice = (NEXUS_3472 *)handle;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3472);
    BSTD_UNUSED(pSettings);

    rc = BHAB_GetExternalGain(pDevice->hab, &gain);
    if(rc){rc = BERR_TRACE(rc); goto done;}

    pSettings->bypassableGain = gain.externalGainBypassable;
    pSettings->totalGain = gain.externalGainTotal;

done:
    return rc;

}
NEXUS_Error NEXUS_Frontend_P_3472_SetExternalGain(void *handle, const NEXUS_ExternalGainSettings *pSettings)
{   
    NEXUS_Error  rc = NEXUS_SUCCESS;
    NEXUS_3472 *pDevice;
    BHAB_ExternalGainSettings externalGain;
    BDBG_ASSERT(NULL != handle);
    pDevice = (NEXUS_3472 *)handle;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3472);

    externalGain.externalGainTotal = pSettings->totalGain;
    externalGain.externalGainBypassable = pSettings->bypassableGain;

    rc = BHAB_SetExternalGain(pDevice->hab, &externalGain);
    if(rc){rc = BERR_TRACE(rc); goto done;}
    
done:   
    return rc;
}
/***************************************************************************
Summary:
    Get the default settings for a BCM3461 device.
See Also:
    NEXUS_FrontendDevice_Open3461
 ***************************************************************************/
void NEXUS_FrontendDevice_GetDefault3461OpenSettings(NEXUS_FrontendDevice3461OpenSettings *pSettings)
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->loadAP = true;
    pSettings->i2cSlaveAddr = 0x60;
    pSettings->inBandOpenDrain = true;
}

NEXUS_FrontendDeviceHandle NEXUS_FrontendDevice_Open3461(unsigned index, const NEXUS_FrontendDevice3461OpenSettings *pSettings)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_3472 *pDevice = NULL;
    uint16_t chipId=0, revId=0;
    NEXUS_FrontendDevice *pFrontendDevice = NULL;
    NEXUS_3461ProbeResults results;

    BSTD_UNUSED(index);

    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT((NULL != pSettings->i2cDevice) || (NULL != pSettings->spiDevice));

    rc = NEXUS_Frontend_Probe3461(pSettings, &results);
    if(rc){rc = BERR_TRACE(rc); goto err_create;} 

    for ( pDevice = BLST_S_FIRST(&g_3472DevList); NULL != pDevice; pDevice = BLST_S_NEXT(pDevice, node) )
    {
        BDBG_OBJECT_ASSERT(pDevice, NEXUS_3472);
        if(((pSettings->i2cAddr == pDevice->i2cAddr) && (pSettings->i2cDevice== pDevice->i2cHandle)) &&
            ((pSettings->spiAddr == pDevice->spiAddr) && (pSettings->spiDevice== pDevice->spiHandle)))
        {
            break;
        }
    }

    if ( NULL == pDevice)
    {
#ifdef NEXUS_SHARED_FRONTEND_INTERRUPT
        for ( pDevice = BLST_S_FIRST(&g_3472DevList); NULL != pDevice; pDevice = BLST_S_NEXT(pDevice, node) )
        {
            BDBG_OBJECT_ASSERT(pDevice, NEXUS_3472);
            if ((pSettings->gpioInterrupt == pDevice->gpioInterrupt) && (pSettings->isrNumber == pDevice->isrNumber))
            {
                pDevice->isMaster = false;
            }
        }
#endif
        chipId = NEXUS_Frontend_P_Is3472(pSettings);
        if ( chipId != 0x3472 )
        {
            rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto err_create;
        }
        revId = NEXUS_Frontend_P_Get3472Rev(pSettings);

        pFrontendDevice = BKNI_Malloc(sizeof(*pFrontendDevice));        
        if (NULL == pFrontendDevice) {rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY); goto err_create;}

        /* Memsetting the whole structure should cover initializing the child list. */
        BKNI_Memset(pFrontendDevice, 0, sizeof(*pFrontendDevice));

        pDevice = BKNI_Malloc(sizeof(NEXUS_3472));      
        if (NULL == pDevice) {rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY); goto err_create;}
        
        BKNI_Memset(pDevice, 0, sizeof(NEXUS_3472));
        BDBG_OBJECT_SET(pDevice, NEXUS_3472);

        pDevice->chipId = chipId;
        pDevice->revId = revId;
        pDevice->i2cAddr = pSettings->i2cAddr;
        pDevice->spiAddr = pSettings->spiAddr;
        pDevice->isrNumber = pSettings->isrNumber;
        pDevice->gpioInterrupt = pSettings->gpioInterrupt;
        pDevice->i2cHandle = pSettings->i2cDevice;
        pDevice->spiHandle = pSettings->spiDevice;
        pDevice->pGenericDeviceHandle = pFrontendDevice;
        pDevice->isMtsif = pSettings->isMtsif;
#ifdef NEXUS_SHARED_FRONTEND_INTERRUPT
        pDevice->isMaster = true;
#endif
        rc = NEXUS_Frontend_P_Init_3472_Hab(pDevice, pSettings);
        if(rc){rc = BERR_TRACE(rc); goto err_init_hab;}
        
        /* Initialize ADS channels, THD and TC2 */
        rc = NEXUS_Frontend_P_Init3472(pDevice);
        if(rc){rc = BERR_TRACE(rc); goto err_init;} 

#ifdef NEXUS_SHARED_FRONTEND_INTERRUPT
        BKNI_EnterCriticalSection();
        BLST_S_INSERT_HEAD(&g_3472DevList, pDevice, node);
        BKNI_LeaveCriticalSection();
#else
        BLST_S_INSERT_HEAD(&g_3472DevList, pDevice, node);
#endif

    }
    else
    {
        BDBG_MSG(("Found device"));
        return pDevice->pGenericDeviceHandle;       
    }

    pFrontendDevice->pDevice = pDevice;
    pFrontendDevice->familyId = results.chip.familyId;
    pFrontendDevice->getInternalGain = NEXUS_Frontend_P_3472_GetInternalGain;
    pFrontendDevice->getExternalGain = NEXUS_Frontend_P_3472_GetExternalGain;
    pFrontendDevice->setExternalGain = NEXUS_Frontend_P_3472_SetExternalGain;
#if NEXUS_AMPLIFIER_SUPPORT 
    pFrontendDevice->getAmplifierStatus = NEXUS_Frontend_P_3472_GetAmplifierStatus;
    pFrontendDevice->setAmplifierStatus = NEXUS_Frontend_P_3472_SetAmplifierStatus;
#endif
    return pFrontendDevice;

err_init:   
    NEXUS_Frontend_P_UnInit_3472_Hab(pDevice);
err_init_hab:
    if ( pDevice ) BKNI_Free(pDevice);
    pDevice = NULL;
err_create: 
    if(pFrontendDevice) BKNI_Free(pFrontendDevice);
    return NULL;
}

/***************************************************************************
Summary:
    Open a handle to a BCM3472 device.
 ***************************************************************************/
NEXUS_FrontendHandle NEXUS_Frontend_Open3461(const NEXUS_3461Settings *pSettings)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_FrontendHandle frontendHandle = NULL;
    NEXUS_3472 *pDevice = NULL;
    unsigned chn_num=0;
    NEXUS_3472Channel *pChannel;
    NEXUS_FrontendDevice *pFrontendDevice = NULL;
    NEXUS_FrontendDevice3461OpenSettings openSettings;

    if(pSettings->device == NULL) {
        NEXUS_FrontendDevice_GetDefault3461OpenSettings(&openSettings);
        openSettings.configureWatchdog = pSettings->configureWatchdog;
        openSettings.gpioInterrupt = pSettings->gpioInterrupt;
        openSettings.i2cAddr = pSettings->i2cAddr;
        openSettings.i2cDevice  = pSettings->i2cDevice;
        openSettings.i2cSlaveAddr  = pSettings->i2cSlaveAddr;
        openSettings.inBandOpenDrain  = pSettings->inBandOpenDrain;
        openSettings.isMtsif  = pSettings->isMtsif;
        openSettings.isrNumber  = pSettings->isrNumber;
        openSettings.loadAP  = pSettings->loadAP;
        openSettings.spiAddr  = pSettings->spiAddr;
        openSettings.spiDevice  = pSettings->spiDevice;
        pFrontendDevice = NEXUS_FrontendDevice_Open3461(0, &openSettings);
        pDevice = (NEXUS_3472 *)pFrontendDevice->pDevice;
    }
    else {
        pDevice = (NEXUS_3472 *)pSettings->device->pDevice;
        pFrontendDevice =  pSettings->device;
    }

    chn_num = pSettings->channelNumber;

    if((pSettings->channelNumber >= pDevice->numChannels[BODS_Standard_eIsdbt])&& pSettings->type == NEXUS_3461ChannelType_eIsdbt)
    {
        BDBG_MSG((" channel number %d exceeds available downstream channels of %d", pSettings->channelNumber, pDevice->numChannels[BODS_Standard_eIsdbt]));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto err_create;
    }

    /* chekc if fronthandle is already opened*/
    if ( pDevice->frontendHandle[chn_num] != NULL )
    {
        return pDevice->frontendHandle[chn_num];
    }

    pChannel = (NEXUS_3472Channel*)BKNI_Malloc(sizeof(NEXUS_3472Channel));
    if ( NULL == pChannel ) {rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY); goto err_ch_alloc;}

    /* Create a Nexus frontend handle */
    frontendHandle = NEXUS_Frontend_P_Create(pChannel);
    if ( NULL == frontendHandle ) {rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY); goto err_alloc;}

    /* Establish device capabilities */
    frontendHandle->capabilities.ofdm = true;
    frontendHandle->capabilities.ofdmModes[NEXUS_FrontendOfdmMode_eIsdbt] = true;
    
    frontendHandle->requestOfdmAsyncStatus = NEXUS_Frontend_P_3472_RequestOfdmAsyncStatus;
    frontendHandle->getOfdmAsyncStatus = NEXUS_Frontend_P_3472_GetOfdmAsyncStatus;
    frontendHandle->getOfdmStatus = NEXUS_Frontend_P_3472_GetOfdmStatus;
    frontendHandle->tuneOfdm = NEXUS_Frontend_P_3472_TuneOfdm;
    frontendHandle->untune = NEXUS_Frontend_P_3472_UnTune;
    frontendHandle->resetStatus = NEXUS_Frontend_P_3472_ResetStatus;    
    frontendHandle->readSoftDecisions = NEXUS_Frontend_P_3472_ReadSoftDecisions;
    frontendHandle->close = NEXUS_Frontend_P_3472_Close;
    frontendHandle->getFastStatus = NEXUS_Frontend_P_3472_GetFastStatus;
    frontendHandle->getType = NEXUS_Frontend_P_3472_GetType;
    frontendHandle->standby = NEXUS_Frontend_P_3472_Standby;

    frontendHandle->reapplyTransportSettings = NEXUS_Frontend_P_3472_ReapplyTransportSettings;
    
    frontendHandle->pGenericDeviceHandle = pFrontendDevice;

    /* Create app callbacks */
    pDevice->lockAppCallback[chn_num] = NEXUS_IsrCallback_Create(frontendHandle, NULL);
    if ( NULL == pDevice->lockAppCallback[chn_num] ) { rc = BERR_TRACE(NEXUS_NOT_INITIALIZED); goto err_cbk_create;}

    pDevice->asyncStatusAppCallback[chn_num] = NEXUS_IsrCallback_Create(frontendHandle, NULL);
    if ( NULL == pDevice->asyncStatusAppCallback[chn_num] ) { rc = BERR_TRACE(NEXUS_NOT_INITIALIZED); goto err_cbk_create;}

    /* Install ODS callbacks. */
    rc = BODS_InstallCallback(pDevice->ods_chn[chn_num], BODS_Callback_eLockChange, (BODS_CallbackFunc)NEXUS_Frontend_P_3472_callback_isr, (void*)pChannel);
    if(rc){rc = BERR_TRACE(rc); goto err_cbk_create;} 
    rc = BODS_InstallCallback(pDevice->ods_chn[chn_num], BODS_Callback_eAsyncStatusReady, (BODS_CallbackFunc)NEXUS_Frontend_P_3472_AsyncStatusCallback_isr, (void*)pChannel);
    if(rc){rc = BERR_TRACE(rc); goto err_cbk_create;} 

    pDevice->frontendHandle[chn_num] = frontendHandle;
    frontendHandle->chip.familyId = 0x3472;
    frontendHandle->chip.id = 0x3472;
    /* save channel number in pChannel*/
    pChannel->chn_num = chn_num;
    pChannel->pDevice = pDevice;
    pDevice->numfrontends++;
    return frontendHandle;

err_cbk_create:
    for(chn_num=0; chn_num < NEXUS_MAX_3472_FRONTENDS; chn_num++){
        if ( pDevice->lockAppCallback[chn_num] ) NEXUS_IsrCallback_Destroy(pDevice->lockAppCallback[chn_num]);
        if ( pDevice->asyncStatusAppCallback[chn_num] ) NEXUS_IsrCallback_Destroy(pDevice->asyncStatusAppCallback[chn_num]);
    }
    if ( pDevice->updateGainAppCallback[0]) NEXUS_IsrCallback_Destroy(pDevice->updateGainAppCallback[0]);    
    if ( frontendHandle ) BKNI_Free(frontendHandle);
err_alloc:
    if ( pChannel ) BKNI_Free(pChannel);
    if(pDevice->numfrontends != 0)
        return NULL;
err_ch_alloc:
    NEXUS_Frontend_P_UnInit3472(pDevice);       
err_create: 
    return NULL;
}

/***************************************************************************
Summary:
    Close a handle to a BCM3472 device.
See Also:
    NEXUS_Frontend_Open3472
 ***************************************************************************/
static void NEXUS_Frontend_P_3472_Close(NEXUS_FrontendHandle handle)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_3472 *pDevice;
    NEXUS_3472Channel *pChannel;
    BTNR_PowerSaverSettings tnrPwrSettings;

    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);
    pChannel =(NEXUS_3472Channel*) handle->pDeviceHandle;
    BDBG_ASSERT(NULL != pChannel);
    pDevice = (NEXUS_3472 *)pChannel->pDevice;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3472);

    if (pChannel->chn_num >= NEXUS_MAX_3472_FRONTENDS)
    {
        BDBG_MSG((" Unsupported Frontend Handle"));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto done;
    }

    if(pDevice->isPoweredOn[pChannel->chn_num]) {
        NEXUS_Frontend_P_3472_UnTune(pChannel);
    } 
    tnrPwrSettings.enable = true;
    rc = BTNR_SetPowerSaver(pDevice->tnr[pChannel->chn_num], &tnrPwrSettings);
    if(rc){rc = BERR_TRACE(rc);}

    if ( NULL != pDevice->lockAppCallback[pChannel->chn_num])
        NEXUS_IsrCallback_Destroy(pDevice->lockAppCallback[pChannel->chn_num]);
    if ( NULL != pDevice->asyncStatusAppCallback[pChannel->chn_num])
        NEXUS_IsrCallback_Destroy(pDevice->asyncStatusAppCallback[pChannel->chn_num]);

    if (pChannel->chn_num < NEXUS_MAX_3472_FRONTENDS) {
        if(pDevice->updateGainAppCallback[pChannel->chn_num]) {
            NEXUS_IsrCallback_Destroy(pDevice->updateGainAppCallback[pChannel->chn_num]);
        }
    }
    NEXUS_Frontend_P_Destroy(handle);

    pDevice->frontendHandle[pChannel->chn_num] = NULL;
    BKNI_Free(pChannel);
    pDevice->numfrontends--;
    
    /* see if there are still open channels*/
    if (pDevice->numfrontends) return;

    NEXUS_Frontend_P_UnInit3472(pDevice);
    NEXUS_Frontend_P_UnInit_3472_Hab(pDevice);

#ifndef NEXUS_SHARED_FRONTEND_INTERRUPT
    BLST_S_REMOVE(&g_3472DevList, pDevice, NEXUS_3472, node);
#endif
    BDBG_OBJECT_DESTROY(pDevice, NEXUS_3472);
    BKNI_Free(pDevice);
done:
    return; 
}

static NEXUS_FrontendLockStatus  NEXUS_Frontend_P_GetLockStatus(unsigned lockStatus)
{
    switch ( lockStatus )
    {
    case BODS_LockStatus_eUnlocked:
        return NEXUS_FrontendLockStatus_eUnlocked;
    case BODS_LockStatus_eLocked:
        return NEXUS_FrontendLockStatus_eLocked;
    case BODS_LockStatus_eNoSignal:
        return NEXUS_FrontendLockStatus_eNoSignal;
    default:
        BDBG_WRN(("Unrecognized lock status (%d) ", lockStatus));
        BERR_TRACE(BERR_NOT_SUPPORTED);
        return NEXUS_FrontendLockStatus_eUnknown;
    }   
}
static NEXUS_Error NEXUS_Frontend_P_3472_GetFastStatus(void *handle, NEXUS_FrontendFastStatus *pStatus)
{
    NEXUS_Error  rc = NEXUS_SUCCESS;
    NEXUS_3472 *pDevice;
    unsigned lock;
    NEXUS_3472Channel *pChannel;          

    pChannel =(NEXUS_3472Channel*) handle;
    BDBG_ASSERT(NULL != pChannel);
    pDevice = (NEXUS_3472 *)pChannel->pDevice;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3472);

    if (pChannel->chn_num >= NEXUS_MAX_3472_FRONTENDS)
    {
        BDBG_MSG((" Unsupported Frontend Handle"));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto done;
    }

    rc = BODS_GetLockStatus(pDevice->ods_chn[pChannel->chn_num],  &lock);
    if(rc){rc = BERR_TRACE(rc); goto done;} 
    
    pStatus->lockStatus = NEXUS_Frontend_P_GetLockStatus(lock);
    pStatus->acquireInProgress = pDevice->acquireInProgress;
    return BERR_SUCCESS;
done:
    return rc;
}

static void NEXUS_Frontend_P_3472_ResetStatus(void *handle)
{
    NEXUS_Error  rc = NEXUS_SUCCESS;
    NEXUS_3472Channel *pChannel;
    NEXUS_3472 *pDevice;
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_3472Channel *)handle;
    pDevice = (NEXUS_3472 *)pChannel->pDevice;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3472);
    
   if(pChannel->chn_num < NEXUS_3472_MAX_OFDM_CHN){
        if(pDevice->isPoweredOn[pChannel->chn_num]){
            rc = BODS_ResetStatus(pDevice->ods_chn[pChannel->chn_num]);
            if(rc){rc = BERR_TRACE(rc); goto done;}
        }
    }
    else{
        BDBG_ERR((" Unsupported channel."));
        BERR_TRACE(BERR_NOT_SUPPORTED);
    }
done:
    return;
}

static NEXUS_Error NEXUS_Frontend_P_3472_Standby(void *handle, bool enabled, const NEXUS_StandbySettings *pSettings)
{
    NEXUS_Error  rc = NEXUS_SUCCESS;
    NEXUS_3472Channel *pChannel;
    NEXUS_3472 *pDevice;
    BTNR_PowerSaverSettings tnrPwrSettings;
    BODS_PowerSaverSettings odsPwrSettings;
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_3472Channel *)handle;
    pDevice = (NEXUS_3472 *)pChannel->pDevice;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3472);

    BSTD_UNUSED(pSettings);
        
    if (pChannel->chn_num >= NEXUS_MAX_3472_FRONTENDS)
    {
        BDBG_MSG((" Unsupported Frontend Handle"));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto done;
    }

    if(!enabled){        
        rc = BODS_DisablePowerSaver(pDevice->ods_chn[pChannel->chn_num], &odsPwrSettings);
        if(rc){rc = BERR_TRACE(rc); goto done;}
        pDevice->isPoweredOn[pChannel->chn_num] = true;    

        tnrPwrSettings.enable = false;
    }
    else {
        rc = BODS_EnablePowerSaver(pDevice->ods_chn[pChannel->chn_num], &odsPwrSettings);
        if(rc){rc = BERR_TRACE(rc); goto done;}
        pDevice->isPoweredOn[pChannel->chn_num] = false;
    
        tnrPwrSettings.enable = true;       
    }

    rc = BTNR_SetPowerSaver(pDevice->tnr[pChannel->chn_num], &tnrPwrSettings);
    if(rc){rc = BERR_TRACE(rc);}

done:
    return rc;
}

static void NEXUS_Frontend_P_3472_UnTune(void *handle)
{
    NEXUS_Error  rc = NEXUS_SUCCESS;
    NEXUS_3472 *pDevice = (NEXUS_3472 *)handle;
    unsigned chn_num = 0;
    NEXUS_3472Channel *pChannel;
    BODS_PowerSaverSettings odsPwrSettings;

    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_3472Channel *)handle;
    pDevice = pChannel->pDevice;
    chn_num = pChannel->chn_num;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3472);

    if (chn_num >= NEXUS_3472_MAX_OFDM_CHN)
    {
        BDBG_ERR((" Unsupported channel."));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto done;
    }

    rc = BODS_EnablePowerSaver(pDevice->ods_chn[pChannel->chn_num], &odsPwrSettings);
    if(rc){rc = BERR_TRACE(rc); goto done;}
    pDevice->isPoweredOn[pChannel->chn_num] = false;

#if NEXUS_HAS_MXT
    NEXUS_Frontend_P_UnsetMtsifConfig(pDevice->frontendHandle[pChannel->chn_num], &pDevice->mtsifConfig);
    NEXUS_Frontend_P_UnsetTransportConfig(pDevice->frontendHandle[pChannel->chn_num], &pDevice->mtsifConfig);
#endif

done:
    return;
}

static NEXUS_Error NEXUS_Frontend_P_3472_ReadSoftDecisions(void *handle, NEXUS_FrontendSoftDecision *pDecisions, size_t length, size_t *pNumRead)
{
    size_t i;
    NEXUS_Error rc = NEXUS_SUCCESS;      
    int16_t return_length;
    NEXUS_3472 *pDevice;
    int16_t d_i[TOTAL_SOFTDECISIONS], d_q[TOTAL_SOFTDECISIONS];
    unsigned chn_num = 0;
    NEXUS_3472Channel *pChannel;

    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_3472Channel *)handle;
    pDevice = pChannel->pDevice;
    chn_num = pChannel->chn_num;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3472);

    if (chn_num >= NEXUS_3472_MAX_OFDM_CHN)
    {
        BDBG_ERR((" Unsupported channel."));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto done;
    }

    BKNI_Memset(pDecisions, 0, (sizeof(NEXUS_FrontendSoftDecision) * length));

    /* only make one call to ADS. if app needs more, they can loop. */    
    rc = BODS_GetSoftDecision(pDevice->ods_chn[chn_num], (int16_t)TOTAL_SOFTDECISIONS, d_i, d_q, &return_length);
    if(rc){rc = BERR_TRACE(rc); goto done;}

    for (i=0; (int)i<return_length && i<length; i++)
    {
        pDecisions[i].i = d_i[i] * 2;
        pDecisions[i].q = d_q[i] * 2;
    }
    *pNumRead = i;

done:
    return rc;
}

/***************************************************************************
Summary:
  Probe to see if a BCM3472 device exists with the specified settings

Description:
  Probe to see if a BCM3472 device exists with the specified settings

See Also:
    NEXUS_Frontend_Open3472
 ***************************************************************************/
NEXUS_Error NEXUS_Frontend_Probe3461(const NEXUS_FrontendDevice3461OpenSettings *pSettings, NEXUS_3461ProbeResults *pResults)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    uint16_t chipVer=0xffff;

    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(!((NULL == pSettings->i2cDevice) && (NULL == pSettings->spiDevice))); 
    BDBG_ASSERT(NULL != pResults);

    pResults->chip.familyId = 0x0;

    pResults->chip.familyId = (uint32_t)NEXUS_Frontend_P_Is3472(pSettings);
    if ( pResults->chip.familyId != 0x3472 )
    {
        BDBG_WRN(("pResults->chip.familyId = 0x%x", pResults->chip.familyId));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto done;
    }
    pResults->chip.id = pResults->chip.familyId;
    chipVer = NEXUS_Frontend_P_Get3472Rev(pSettings);
done:
    pResults->chip.version.major = (chipVer >> 8) + 1;
    pResults->chip.version.minor = chipVer & 0xff;
    
    return rc;
}

/***************************************************************************
Summary:
    Retrieve the chip family id, chip id, chip version and firmware version.
***************************************************************************/
static void NEXUS_Frontend_P_3472_GetType(void *handle, NEXUS_FrontendType *type)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_3472 *pDevice;
    uint32_t       familyId, chipId, chn_num;
    uint16_t       chipVer;
    uint8_t        majApVer, minApVer;
    NEXUS_3472Channel *pChannel;

    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_3472Channel *)handle;
    pDevice = pChannel->pDevice;
    chn_num = pChannel->chn_num;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3472);

    if (chn_num >= NEXUS_3472_MAX_OFDM_CHN)
    {
        BDBG_ERR((" Unsupported channel."));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto done;
    }

    type->chip.familyId = (uint32_t)pDevice->chipId;    
    type->chip.id = (uint32_t)pDevice->chipId;
    type->chip.version.major = (pDevice->revId >> 8) + 1;
    type->chip.version.minor = pDevice->revId & 0xff;

    BHAB_GetApVersion(pDevice->hab, &familyId, &chipId, &chipVer, &majApVer, &minApVer);
    if((type->chip.familyId != familyId) || (type->chip.id != chipId)){
        BDBG_ERR(("Type mismatch while retreiving chip id and family id."));
        BERR_TRACE(BERR_UNKNOWN); goto done;
    }
    type->firmwareVersion.major = majApVer;
    type->firmwareVersion.minor = minApVer;
done:
    return;
}

static NEXUS_Error NEXUS_Frontend_P_3472_TuneOfdm(void *handle, const NEXUS_FrontendOfdmSettings *pSettings)
{
    NEXUS_Error  rc = NEXUS_SUCCESS;
    NEXUS_3472 *pDevice = (NEXUS_3472 *)handle;
    BODS_AcquireParams odsParam;
    unsigned chn_num = 0;
    BTNR_Settings tnrSettings;
    unsigned temp_frequency;
    NEXUS_FrontendOfdmMode temp_mode;
    BODS_PowerSaverSettings pwrSettings;
    NEXUS_3472Channel *pChannel;

    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_3472Channel *)handle;
    pDevice = pChannel->pDevice;
    chn_num = pChannel->chn_num;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3472);

    if (chn_num >= NEXUS_3472_MAX_OFDM_CHN)
    {
        BDBG_ERR((" Unsupported channel."));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto done;
    }

#if NEXUS_HAS_MXT
    rc = NEXUS_Frontend_P_SetMtsifConfig(pDevice->frontendHandle[chn_num], &pDevice->mtsifConfig);
    if (rc) { rc = BERR_TRACE(rc); goto done; }
    rc = NEXUS_Frontend_P_SetTransportConfig(pDevice->frontendHandle[chn_num], &pDevice->mtsifConfig);
    if (rc) { rc = BERR_TRACE(rc); goto done; }
    rc = NEXUS_Frontend_P_SetTsmfConfig(pDevice->frontendHandle[chn_num], &pDevice->mtsifConfig);
    if (rc) { rc = BERR_TRACE(rc); goto done; }
#endif

    rc = BODS_GetDefaultAcquireParams(pDevice->ods_chn[chn_num], &odsParam);
    if(rc){rc = BERR_TRACE(rc); goto done;}

    NEXUS_IsrCallback_Set(pDevice->lockAppCallback[chn_num], &(pSettings->lockCallback));   
    NEXUS_IsrCallback_Set(pDevice->asyncStatusAppCallback[chn_num], &(pSettings->asyncStatusReadyCallback));

    if(!pDevice->isPoweredOn[chn_num])
        goto full_acquire;

    if((pSettings->acquisitionMode == NEXUS_FrontendOfdmAcquisitionMode_eScan) && (pDevice->lastAcquisitionMode[chn_num] == NEXUS_FrontendOfdmAcquisitionMode_eScan)){
        temp_frequency = pDevice->last_ofdm[0].frequency;
        pDevice->last_ofdm[0].frequency = pSettings->frequency ;
        temp_mode = pDevice->last_ofdm[0].mode;
        pDevice->last_ofdm[0].mode = pSettings->mode;

        if(!BKNI_Memcmp(pSettings, &pDevice->last_ofdm[0], sizeof(NEXUS_FrontendOfdmSettings))) {
            if (pDevice->tnr[chn_num])
            {
                pDevice->count[chn_num] = 0;
                pDevice->acquireInProgress[chn_num] = true;
                pDevice->last_ofdm[0] = *pSettings;
                rc = BTNR_SetTunerRfFreq(pDevice->tnr[chn_num], pSettings->frequency, BTNR_TunerMode_eDigital);
                if(rc){rc = BERR_TRACE(rc); goto retrack;} 
                return rc;
            }
        }
    }

full_acquire:
    switch ( pSettings->acquisitionMode )
    {
    case NEXUS_FrontendOfdmAcquisitionMode_eScan:
        odsParam.tuneAcquire = true;
        /* No break as we set the modes to manual for scan.*/
    case NEXUS_FrontendOfdmAcquisitionMode_eAuto:
        /* Due to get default parameters, the odsParam.tuneAcquire and thdParam.bTuneAcquire are set to false. */
        odsParam.acquisitionMode = BODS_AcquisitionMode_eAuto;
        break;  
    case NEXUS_FrontendOfdmAcquisitionMode_eManual:
        /* Due to get default parameters, the odsParam.tuneAcquire and thdParam.bTuneAcquire are set to false. */
        odsParam.acquisitionMode = BODS_AcquisitionMode_eManual;
        break;
    default:
        BDBG_MSG((" Unsupported Acquisition mode."));
        rc = BERR_TRACE(BERR_NOT_SUPPORTED); goto done;
    }
    
    if(!pDevice->isPoweredOn[chn_num]){
        rc = BODS_DisablePowerSaver(pDevice->ods_chn[chn_num], &pwrSettings);
        if(rc){rc = BERR_TRACE(rc); goto done;} 
        pDevice->isPoweredOn[chn_num] = true;
    }

    switch ( pSettings->mode )
    {
    case NEXUS_FrontendOfdmMode_eIsdbt: 
       switch ( pSettings->bandwidth )
        {
        case 1700000:
            odsParam.acquireParams.isdbt.bandwidth = BODS_IsdbtBandwidth_e1p7MHz;
            break;
        case 5000000:
            odsParam.acquireParams.isdbt.bandwidth = BODS_IsdbtBandwidth_e5MHz;      
            break;
        case 6000000:
            odsParam.acquireParams.isdbt.bandwidth = BODS_IsdbtBandwidth_e6MHz;      
            break;
        case 7000000:
            odsParam.acquireParams.isdbt.bandwidth = BODS_IsdbtBandwidth_e7MHz;      
            break;
        case 8000000:
            odsParam.acquireParams.isdbt.bandwidth = BODS_IsdbtBandwidth_e8MHz;      
            break;
        default:
            BDBG_MSG((" Unsupported bandwidth."));
            rc = BERR_TRACE(BERR_NOT_SUPPORTED); goto done;
        }
        switch (pSettings->cciMode)
        {
        case NEXUS_FrontendOfdmCciMode_eNone:
            odsParam.acquireParams.isdbt.cciMode = BODS_IsdbtCciMode_eNone;
            break;
        case NEXUS_FrontendOfdmCciMode_eAuto:
            odsParam.acquireParams.isdbt.cciMode = BODS_IsdbtCciMode_eAuto;     
            break;
        default:
            BDBG_MSG((" Unsupported bandwidth."));
            rc = BERR_TRACE(BERR_NOT_SUPPORTED); goto done;
        }

        odsParam.acquireParams.isdbt.carrierRange = (BODS_IsdbtCarrierRange)pSettings->pullInRange;
        if(pSettings->manualModeSettings){
            odsParam.acquireParams.isdbt.transGuardMode = BODS_IsdbtOfdmMode_eManual;
            odsParam.acquireParams.isdbt.guardInterval = pSettings->modeSettings.guardInterval;
            if((pSettings->modeSettings.mode>NEXUS_FrontendOfdmTransmissionMode_e1k) && (pSettings->modeSettings.mode<NEXUS_FrontendOfdmTransmissionMode_e16k))
                odsParam.acquireParams.isdbt.transmissionMode = pSettings->modeSettings.mode;           
            else {
                BDBG_MSG((" Unsupported ISDBT Transmission mode."));
                rc = BERR_TRACE(BERR_NOT_SUPPORTED); goto done;
            }
                
        }
        else {
            if(pSettings->modeSettings.modeGuard ==  NEXUS_FrontendOfdmModeGuard_eAutoIsdbtJapan){
                odsParam.acquireParams.isdbt.transGuardMode = BODS_IsdbtOfdmMode_eAutoJapan;
            }
            else{
                odsParam.acquireParams.isdbt.transGuardMode = NEXUS_FrontendOfdmModeGuard_eAutoIsdbtBrazil;
            }
        }
        
        odsParam.acquireParams.isdbt.tmccAcquire = true; /* CHECK THIS. */

        break;
    default:
        BDBG_ERR((" Unsupported mode."));
        rc = BERR_TRACE(BERR_NOT_SUPPORTED); goto done;
        break;
    }

    if (pDevice->tnr[chn_num])
    {
        rc = BTNR_GetSettings(pDevice->tnr[chn_num], &tnrSettings);
        if(rc){rc = BERR_TRACE(rc); goto done;}

        if(pSettings->mode == NEXUS_FrontendOfdmMode_eIsdbt){
            tnrSettings.std = BTNR_Standard_eIsdbt;
        }
        else {
            BDBG_WRN((" Unsupported mode."));
        }
        tnrSettings.bandwidth = pSettings->bandwidth;
        
        rc = BTNR_SetSettings(pDevice->tnr[chn_num], &tnrSettings);
        if(rc){rc = BERR_TRACE(rc); goto done;}
        
        pDevice->count[chn_num] = 0;
        pDevice->acquireInProgress[chn_num] = true;
        pDevice->lastAcquisitionMode[chn_num] = pSettings->acquisitionMode;

        rc = BTNR_SetTunerRfFreq(pDevice->tnr[chn_num], pSettings->frequency, BTNR_TunerMode_eDigital);
        if(rc){rc = BERR_TRACE(rc); goto done;}
    }
    
    rc = BODS_SetAcquireParams( pDevice->ods_chn[chn_num], &odsParam );
    if(rc){rc = BERR_TRACE(rc); goto done;}

    if ( pSettings->acquisitionMode != NEXUS_FrontendOfdmAcquisitionMode_eScan) {
        switch ( pSettings->mode )
        {
        case NEXUS_FrontendOfdmMode_eIsdbt:
            rc = BODS_Acquire(pDevice->ods_chn[chn_num], &odsParam);
            if(rc){rc = BERR_TRACE(rc); goto done;}         
            break;
        default:
            break;
        }
    }

    pDevice->last_ofdm[0] = *pSettings;    
    return BERR_SUCCESS;        
retrack:    
    pDevice->last_ofdm[0].frequency = temp_frequency;
    pDevice->last_ofdm[0].mode = temp_mode;
done:   
    NEXUS_Frontend_P_3472_UnTune(handle);
    return rc;
}

void NEXUS_FrontendDevice_GetDefault3461Settings(NEXUS_FrontendDevice3461Settings *pSettings)
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->agcValue = 0x1f; /* Max gain*/
}

NEXUS_Error NEXUS_FrontendDevice_Get3461Settings(NEXUS_FrontendDeviceHandle handle, NEXUS_FrontendDevice3461Settings *pSettings)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_3472 *pDevice = NULL;
    BDBG_ASSERT(handle != NULL);    

    pDevice = handle->pDevice;
    if (!pDevice) { rc = BERR_TRACE(NEXUS_INVALID_PARAMETER); goto done; }
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3472);

    pSettings->updateGainCallback = pDevice->updateGainCallbackDesc;
    pSettings->agcValue = pDevice->agcValue;  
    pSettings->rfInput = pDevice->rfInput;
    pSettings->rfDaisyChain = pDevice->rfDaisyChain;
    pSettings->enableRfLoopThrough = pDevice->enableRfLoopThrough;
    pSettings->terrestrial = pDevice->terrestrial;
done:
    return rc;
}

NEXUS_Error NEXUS_FrontendDevice_Set3461Settings(NEXUS_FrontendDeviceHandle handle, const NEXUS_FrontendDevice3461Settings *pSettings)                
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_3472 *pDevice = NULL;
    BHAB_ConfigSettings habConfigSettings;   

    pDevice = handle->pDevice;
    if (!pDevice) { rc = BERR_TRACE(NEXUS_INVALID_PARAMETER); goto done; }
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3472);

    if( (pSettings->rfDaisyChain != pDevice->rfDaisyChain) || (pSettings->enableRfLoopThrough != pDevice->enableRfLoopThrough) ||
        (pSettings->rfInput != pDevice->rfInput))
    {

        rc = BHAB_GetConfigSettings(pDevice->hab, &habConfigSettings);
        if(rc){rc = BERR_TRACE(rc); goto done;}
        
        habConfigSettings.daisyChain = pSettings->rfDaisyChain;
        habConfigSettings.enableLoopThrough = pSettings->enableRfLoopThrough;
        
        habConfigSettings.rfInputMode = pSettings->rfInput;
        habConfigSettings.tnrApplication = pSettings->terrestrial;

        rc = BHAB_SetConfigSettings(pDevice->hab, &habConfigSettings);
        if(rc){rc = BERR_TRACE(rc); goto done;}

        pDevice->rfDaisyChain = pSettings->rfDaisyChain;
        pDevice->enableRfLoopThrough = pSettings->enableRfLoopThrough;
        pDevice->rfInput = pSettings->rfInput;
        
        pDevice->terrestrial = pSettings->terrestrial;

    }
done:
    return rc;  
}

static NEXUS_Error NEXUS_Frontend_P_3472_RequestOfdmAsyncStatus(void *handle)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_3472 *pDevice;
    NEXUS_3472Channel *pChannel;
    unsigned chn_num;
    NEXUS_GainParameters params;
    NEXUS_InternalGainSettings settings;
    NEXUS_ExternalGainSettings externalGainSettings;

    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_3472Channel *)handle;
    pDevice = pChannel->pDevice;
    chn_num = pChannel->chn_num;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3472);

    if (chn_num >= NEXUS_3472_MAX_OFDM_CHN)
    {
        BDBG_ERR((" Unsupported channel."));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto done;
    }

    if(pDevice->pGenericDeviceHandle->parent){

        params.rfInput = pDevice->pGenericDeviceHandle->linkSettings.rfInput;
        params.accumulateTillRootDevice = true;    
        params.frequency = pDevice->last_ofdm[0].frequency;

        rc = NEXUS_Frontend_P_GetInternalGain(pDevice->pGenericDeviceHandle->parent, &params, &settings);
        if(rc){rc = BERR_TRACE(rc); goto done;}

        externalGainSettings.totalGain = settings.totalVariableGain;

        rc = NEXUS_Frontend_P_SetExternalGain(pDevice->pGenericDeviceHandle, &externalGainSettings);
        if(rc){rc = BERR_TRACE(rc); goto done;}
    }

    rc = BODS_RequestSelectiveAsyncStatus(pDevice->ods_chn[chn_num], BODS_SelectiveAsyncStatusType_eIsdbt);
    if(rc){rc = BERR_TRACE(rc); goto done;}

done:
    return rc;
}

static NEXUS_FrontendOfdmTransmissionMode NEXUS_Frontend_P_OdsToTransmissionMode(BODS_IsdbtTransmissionMode mode)
{
    switch ( mode )
    {
    case BODS_IsdbtTransmissionMode_e2K:
        return NEXUS_FrontendOfdmTransmissionMode_e2k;
    case BODS_IsdbtTransmissionMode_e4K:
        return NEXUS_FrontendOfdmTransmissionMode_e4k;
    case BODS_IsdbtTransmissionMode_e8K:
        return NEXUS_FrontendOfdmTransmissionMode_e8k;
    default:
        BDBG_WRN(("Unrecognized transmission mode."));
        BERR_TRACE(BERR_NOT_SUPPORTED);
        return BODS_IsdbtTransmissionMode_e8K;
    }
}

static NEXUS_FrontendOfdmGuardInterval NEXUS_Frontend_P_OdsToGuardInterval(BODS_IsdbtGuardInterval guard)
{
    switch ( guard )
    {
    case BODS_IsdbtGuardInterval_e1_4:
        return NEXUS_FrontendOfdmGuardInterval_e1_4;
    case BODS_IsdbtGuardInterval_e1_8:
        return NEXUS_FrontendOfdmGuardInterval_e1_8;
    case BODS_IsdbtGuardInterval_e1_16:
        return NEXUS_FrontendOfdmGuardInterval_e1_16;
    case BODS_IsdbtGuardInterval_e1_32:
        return NEXUS_FrontendOfdmGuardInterval_e1_32;
    default:
        BDBG_WRN(("Unrecognized guard interval (%d) reported by BODS", guard));
        BERR_TRACE(BERR_NOT_SUPPORTED);
        return NEXUS_FrontendOfdmGuardInterval_e1_4;
    }
}

static NEXUS_FrontendOfdmTimeInterleaving NEXUS_Frontend_P_ODSToTimeInterleaving(BODS_IsdbtTimeInterleaving magnum)
{
    switch (magnum) {
    default:
        BDBG_WRN(("Unknown Magnum time interleaving %d, defaulting to 0x", magnum));
        /* fall-through */
    case BODS_IsdbtTimeInterleaving_e0x:
        return NEXUS_FrontendOfdmTimeInterleaving_e0x;
    case BODS_IsdbtTimeInterleaving_e1x:
        return NEXUS_FrontendOfdmTimeInterleaving_e1x;
    case BODS_IsdbtTimeInterleaving_e2x:
        return NEXUS_FrontendOfdmTimeInterleaving_e2x;
    case BODS_IsdbtTimeInterleaving_e3x:
        return NEXUS_FrontendOfdmTimeInterleaving_e3x;
    }
}

static NEXUS_FrontendOfdmModulation NEXUS_Frontend_P_ODSToModulation( BODS_IsdbtModulation magnum )
{
    switch (magnum) {
    case BODS_IsdbtModulation_e64Qam:
        return NEXUS_FrontendOfdmModulation_eQam64;
    case BODS_IsdbtModulation_e16Qam:
        return NEXUS_FrontendOfdmModulation_eQam16;
    case BODS_IsdbtModulation_eDqpsk:
        return NEXUS_FrontendOfdmModulation_eDqpsk;
    default:
        BDBG_WRN(("Unrecognized ofdm modulation %d, defaulting to QPSK", magnum));
        /* fall-through */
    case BODS_IsdbtModulation_eQpsk:
        return NEXUS_FrontendOfdmModulation_eQpsk;
    }
}

static NEXUS_FrontendOfdmCodeRate NEXUS_Frontend_P_ODSToCodeRate( BODS_IsdbtCodeRate magnum )
{
    switch (magnum) {
    case BODS_IsdbtCodeRate_e1_2:
        return NEXUS_FrontendOfdmCodeRate_e1_2;
    case BODS_IsdbtCodeRate_e2_3:
        return NEXUS_FrontendOfdmCodeRate_e2_3;
    case BODS_IsdbtCodeRate_e3_4:
        return NEXUS_FrontendOfdmCodeRate_e3_4;
    case BODS_IsdbtCodeRate_e5_6:
        return NEXUS_FrontendOfdmCodeRate_e5_6;
    case BODS_IsdbtCodeRate_e7_8:
        return NEXUS_FrontendOfdmCodeRate_e7_8;
    default:
        return NEXUS_FrontendOfdmCodeRate_eMax;
    }
}

static NEXUS_Error NEXUS_Frontend_P_3472_GetOfdmAsyncStatus(void *handle, NEXUS_FrontendOfdmStatus *pStatus)
{
    NEXUS_Error  rc = NEXUS_SUCCESS;
    NEXUS_3472 *pDevice;
    NEXUS_3472Channel *pChannel;
    unsigned chn_num;

    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_3472Channel *)handle;
    pDevice = pChannel->pDevice;
    chn_num = pChannel->chn_num;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3472);

    if (chn_num >= NEXUS_3472_MAX_OFDM_CHN)
    {
        BDBG_ERR((" Unsupported channel."));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto done;
    }
    
    BKNI_Memset(pStatus, 0, sizeof(*pStatus));

    BKNI_Memset(&pDevice->odsStatus, 0, sizeof(pDevice->odsStatus));

    rc = BODS_GetSelectiveAsyncStatus(pDevice->ods_chn[chn_num], BODS_SelectiveAsyncStatusType_eIsdbt, &pDevice->odsStatus);
    if(rc){rc = BERR_TRACE(rc); goto done;}

    pStatus->receiverLock = pDevice->odsStatus.status.isdbt.receiverLock;
    pStatus->fecLock = pDevice->odsStatus.status.isdbt.fecLock;      
    pStatus->noSignalDetected = pDevice->odsStatus.status.isdbt.noSignalDetected;        
    pStatus->transmissionMode = NEXUS_Frontend_P_OdsToTransmissionMode(pDevice->odsStatus.status.isdbt.transmissionMode);
    pStatus->guardInterval = NEXUS_Frontend_P_OdsToGuardInterval(pDevice->odsStatus.status.isdbt.guardInterval);
    /*pStatus->gainOffset = pDevice->odsStatus.status.isdbt.gainOffset;*/
    pStatus->carrierOffset = pDevice->odsStatus.status.isdbt.carrierOffset;
    pStatus->timingOffset = pDevice->odsStatus.status.isdbt.timingOffset;
    pStatus->signalStrength = pDevice->odsStatus.status.isdbt.signalStrength/10;
    pStatus->snr = pDevice->odsStatus.status.isdbt.snr*100/256;
    pStatus->spectrumInverted = pDevice->odsStatus.status.isdbt.spectrumInverted;
    pStatus->reacquireCount = pDevice->odsStatus.status.isdbt.reacqCount;
    pStatus->ews = pDevice->odsStatus.status.isdbt.ews;
    pStatus->partialReception = pDevice->odsStatus.status.isdbt.partialReception;
    pStatus->fecCorrectedBlocks = pDevice->odsStatus.status.isdbt.layerAStatus.rsCorrectedBlocks + pDevice->odsStatus.status.isdbt.layerBStatus.rsCorrectedBlocks;
    pStatus->fecUncorrectedBlocks = pDevice->odsStatus.status.isdbt.layerAStatus.rsUncorrectedBlocks + pDevice->odsStatus.status.isdbt.layerBStatus.rsUncorrectedBlocks;
    pStatus->fecCleanBlocks = pDevice->odsStatus.status.isdbt.layerAStatus.rsCleanBlocks + pDevice->odsStatus.status.isdbt.layerBStatus.rsCleanBlocks;

    pStatus->modulationA =NEXUS_Frontend_P_ODSToModulation(pDevice->odsStatus.status.isdbt.layerAStatus.modulation);
    pStatus->codeRateA = NEXUS_Frontend_P_ODSToCodeRate(pDevice->odsStatus.status.isdbt.layerAStatus.codeRate);
    /* pStatus->isdbtAPreRS = pDevice->odsStatus.status.isdbt.layerAStatus.ulIsdbtAViterbiUncorrectedBits; */
    pStatus->timeInterleavingA = NEXUS_Frontend_P_ODSToTimeInterleaving(pDevice->odsStatus.status.isdbt.layerAStatus.timeInterleaving);
    pStatus->numSegmentsA = pDevice->odsStatus.status.isdbt.layerAStatus.numSegments;
    pStatus->fecCorrectedBlocksA = pDevice->odsStatus.status.isdbt.layerAStatus.rsCorrectedBlocks;
    pStatus->fecUncorrectedBlocksA = pDevice->odsStatus.status.isdbt.layerAStatus.rsUncorrectedBlocks;
    pStatus->fecCleanBlocksA = pDevice->odsStatus.status.isdbt.layerAStatus.rsCleanBlocks;
    pStatus->signalLevelPercentA = pDevice->odsStatus.status.isdbt.layerAStatus.signalLevelPercent;
    pStatus->signalQualityPercentA = pDevice->odsStatus.status.isdbt.layerAStatus.signalQualityPercent;  

    pStatus->modulationB =NEXUS_Frontend_P_ODSToModulation(pDevice->odsStatus.status.isdbt.layerBStatus.modulation);
    pStatus->codeRateB = NEXUS_Frontend_P_ODSToCodeRate(pDevice->odsStatus.status.isdbt.layerBStatus.codeRate);
    /* pStatus->isdbtBPreRS = pDevice->odsStatus.status.isdbt.layerBStatus.ulIsdbtBViterbiUncorrectedBits; */
    pStatus->timeInterleavingB = NEXUS_Frontend_P_ODSToTimeInterleaving(pDevice->odsStatus.status.isdbt.layerBStatus.timeInterleaving);
    pStatus->numSegmentsB = pDevice->odsStatus.status.isdbt.layerBStatus.numSegments;
    pStatus->fecCorrectedBlocksB = pDevice->odsStatus.status.isdbt.layerBStatus.rsCorrectedBlocks;
    pStatus->fecUncorrectedBlocksB = pDevice->odsStatus.status.isdbt.layerBStatus.rsUncorrectedBlocks;
    pStatus->fecCleanBlocksB = pDevice->odsStatus.status.isdbt.layerBStatus.rsCleanBlocks;
    pStatus->signalLevelPercentB = pDevice->odsStatus.status.isdbt.layerBStatus.signalLevelPercent;
    pStatus->signalQualityPercentB = pDevice->odsStatus.status.isdbt.layerBStatus.signalQualityPercent;  

    pStatus->modulationC =NEXUS_Frontend_P_ODSToModulation(pDevice->odsStatus.status.isdbt.layerCStatus.modulation);
    pStatus->codeRateC = NEXUS_Frontend_P_ODSToCodeRate(pDevice->odsStatus.status.isdbt.layerCStatus.codeRate);
    /* pStatus->isdbtCPreRS = pDevice->odsStatus.status.isdbt.layerCStatus.ulIsdbtCViterbiUncorrectedBits; */
    pStatus->timeInterleavingC = NEXUS_Frontend_P_ODSToTimeInterleaving(pDevice->odsStatus.status.isdbt.layerCStatus.timeInterleaving);
    pStatus->numSegmentsC = pDevice->odsStatus.status.isdbt.layerCStatus.numSegments;
    pStatus->fecCorrectedBlocksC = pDevice->odsStatus.status.isdbt.layerCStatus.rsCorrectedBlocks;
    pStatus->fecUncorrectedBlocksC = pDevice->odsStatus.status.isdbt.layerCStatus.rsUncorrectedBlocks;
    pStatus->fecCleanBlocksC = pDevice->odsStatus.status.isdbt.layerCStatus.rsCleanBlocks;
    pStatus->signalLevelPercentC = pDevice->odsStatus.status.isdbt.layerCStatus.signalLevelPercent;
    pStatus->signalQualityPercentC = pDevice->odsStatus.status.isdbt.layerCStatus.signalQualityPercent;  

 
    pStatus->settings = pDevice->last_ofdm[0];
 
done:
    return rc;
}

static NEXUS_Error NEXUS_Frontend_P_3472_GetOfdmStatus(void *handle, NEXUS_FrontendOfdmStatus *pStatus)
{
    NEXUS_Error rc = BERR_SUCCESS;
    NEXUS_3472 *pDevice;
    unsigned j=0, chn_num;
    uint32_t buf=0;
    NEXUS_3472Channel *pChannel;

    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_3472Channel *)handle;
    pDevice = pChannel->pDevice;
    chn_num = pChannel->chn_num;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3472);

    if (chn_num >= NEXUS_3472_MAX_OFDM_CHN)
    {
        BDBG_ERR((" Unsupported channel."));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto done;
    }

    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
   

    pDevice->isInternalAsyncStatusCall[pChannel->chn_num] = true;
 
    rc = NEXUS_Frontend_P_3472_RequestOfdmAsyncStatus(pChannel);
    if(rc){rc = BERR_TRACE(rc); goto done;} 

    for(j=0; j < 10; j++) {

        BKNI_Sleep(100);

        rc = BHAB_ReadRegister(pDevice->hab, BCHP_LEAP_CTRL_SW_SPARE0 , &buf);
        if(rc){rc = BERR_TRACE(rc); goto done;}
        
        if(((chn_num == 0) && (buf & BHAB_THD_CHN0_STATUS_RDY)) || ((chn_num == 1) && (buf & BHAB_THD_CHN1_STATUS_RDY)))
        {
            rc = NEXUS_Frontend_P_3472_GetOfdmAsyncStatus(pChannel, pStatus);
            if(rc){rc = BERR_TRACE(rc); goto done;}
            pDevice->isInternalAsyncStatusCall[pChannel->chn_num] = false;
            break;
        }
        else {
            continue;
        }
    }
    
    pStatus->settings = pDevice->last_ofdm[0];  
    
done:
    pDevice->isInternalAsyncStatusCall[pChannel->chn_num] = false;
    return rc;
}

NEXUS_Error NEXUS_Frontend_P_3472_ReapplyTransportSettings(void *handle)
{
    NEXUS_3472Channel *pChannel = (NEXUS_3472Channel *)handle;
    NEXUS_3472 *pDevice;
    unsigned chn_num;
    NEXUS_Error rc;
    
    BDBG_ASSERT(pChannel);
    pDevice = (NEXUS_3472 *)pChannel->pDevice;
    BDBG_ASSERT(pDevice);
    chn_num = pChannel->chn_num;
    rc = NEXUS_Frontend_P_SetMtsifConfig(pDevice->frontendHandle[chn_num], &pDevice->mtsifConfig);
    if (rc) { return BERR_TRACE(rc); }
    rc = NEXUS_Frontend_P_SetTransportConfig(pDevice->frontendHandle[chn_num], &pDevice->mtsifConfig);
    if (rc) { return BERR_TRACE(rc); }
    rc = NEXUS_Frontend_P_SetTsmfConfig(pDevice->frontendHandle[chn_num], &pDevice->mtsifConfig);
    if (rc) { return BERR_TRACE(rc); }

    return NEXUS_SUCCESS;
}


