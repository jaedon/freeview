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
* $brcm_Workfile: nexus_frontend_4528.c $
* $brcm_Revision: 13 $
* $brcm_Date: 9/17/12 3:03p $
*
* API Description:
*   API name: Frontend 4528
*    APIs to open, close, and setup initial settings for a BCM4528
*    Dual-Channel Satellite Tuner/Demodulator Device.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/frontend/4528/src/nexus_frontend_4528.c $
* 
* 13   9/17/12 3:03p mphillip
* SWSATFE-214: Update 4528 support after reorganization
* 
* 12   7/19/12 4:15p jtna
* SW3472-14: add 3472 MXT support. change MTSIF in API naming to
*  MTSIF_TX, in order to differentiate with MTSIF_RX
* 
* 11   6/27/12 3:06p jtna
* SW7425-3350: remove NEXUS_FRONTEND_FIXED_MTSIF_CONFIG compile option.
*  it's no longer needed
* 
* 10   6/26/12 3:35p jtna
* SW7425-3062: add feature to propagate host parserband settings to
*  demod. deprecate NEXUS_Frontend_Get/SetTransportSettings
* 
* 9   6/8/12 3:28p jtna
* SW7425-3171: add NEXUS_Frontend_ReapplyTransportSettings
* 
* 8   6/5/12 2:57p jtna
* SW7425-2752: rework satellite frontend MTSIF connection
* 
* SW7425-2752/1   5/14/12 11:54a jtna
* SW7425-2752: rework MTSIF-frontend <-> host interface
* 
* 7   3/30/12 7:24p mphillip
* SW7425-2030: Enable all 16 tuners (4528_0 are frontends 0..7, 4528_1
*  are frontends 8..15)
* 
* 6   3/16/12 3:16p mphillip
* SW7425-2030: Determine ext_irq from bchp headers instead of hard-coded
*  numbers
* 
* 5   2/9/12 10:41a mphillip
* SW7425-1786: Set chip id to the ast chip, not the system, in case of
*  future use of NEXUS_AstDevice
* 
* 4   12/5/11 3:56p mphillip
* SW7425-1437: Enable mtsif
* 
* 3   12/5/11 2:38p jtna
* SW7425-1437: use BMXT to replace previous hack
* 
* 2   12/1/11 4:13p mphillip
* SW7425-1304: Change 4528 to HAB
* 
* 1   11/1/11 4:49p mphillip
* SW7425-1437: Merge 4528 to main
* 
* SATIP_7425_4528/4   7/26/11 1:25p vishk
* SW7425-798: add support to open 4 frontends from two 4528 chips
*  seperately.
* 
* SATIP_7425_4528/3   7/23/11 3:44p lihong
* SW7425-798: add support to open 4 frontends from two 4528 chips
*  seperately.
* 
* SATIP_7425_4528/2   7/12/11 12:11p vishk
* SWSATFE-132: Add nexus support for 4528 satellite frontend for 97425
*  SATIPSW platform. Support for second 4528 frontend. Temporary HACK.
* 
* SATIP_7425_4528/1   7/11/11 12:33p vishk
* SWSATFE-132: Add nexus support for 4528 satellite frontend for 97425
*  SATIPSW platform.
*
***************************************************************************/
#include "nexus_frontend_module.h"
#include "nexus_frontend_ast.h"
#include "priv/nexus_i2c_priv.h"
#include "priv/nexus_gpio_priv.h"
#include "bhab.h"
#include "bhab_4528.h"
#include "bhab_4528_fw.h"
#include "bast.h"
#include "bast_4528.h"
#if NEXUS_HAS_MXT
#include "bmxt.h"
#include "bmxt_4528.h"
#include "bchp_hif_cpu_intr1.h"
#endif

BDBG_MODULE(nexus_frontend_4528);

BDBG_OBJECT_ID(NEXUS_4528Device);

typedef struct NEXUS_4528Device
{
    BDBG_OBJECT(NEXUS_4528Device)
    BLST_S_ENTRY(NEXUS_4528Device) node;
    NEXUS_4528Settings settings;
    BAST_Handle astHandle;
    BKNI_EventHandle isrEvent;
    NEXUS_EventCallbackHandle isrCallback;
    BAST_ChannelHandle astChannels[NEXUS_4528_MAX_FRONTEND_CHANNELS];
    NEXUS_FrontendHandle handles[NEXUS_4528_MAX_FRONTEND_CHANNELS];
    uint32_t numChannels;   /* prototype to match BAST_GetTotalChannels */
    BHAB_Handle hab;
#if NEXUS_HAS_MXT
    BMXT_Handle mxt;
    NEXUS_FrontendDeviceMtsifConfig mtsifConfig;
#endif
} NEXUS_4528Device;

static BLST_S_HEAD(devList, NEXUS_4528Device) g_deviceList = BLST_S_INITIALIZER(g_deviceList);

static void NEXUS_Frontend_P_4528_IsrControl_isr(bool enable, void *pParam);
static void NEXUS_Frontend_P_4528_GpioIsrControl_isr(bool enable, void *pParam);
static void NEXUS_Frontend_P_4528_L1_isr(void *param1, int param2);
static void NEXUS_Frontend_P_4528_CloseCallback(NEXUS_FrontendHandle handle, void *pParam);
static void NEXUS_Frontend_P_4528_DestroyDevice(NEXUS_4528Device *pDevice);
static void NEXUS_Frontend_P_4528_IsrCallback(void *pParam);
static NEXUS_Error NEXUS_Frontend_P_4528_TuneSatellite(void *handle, const NEXUS_FrontendSatelliteSettings *pSettings);
static void NEXUS_Frontend_P_4528_Untune(void *handle);
static NEXUS_Error NEXUS_Frontend_P_4528_ReapplyTransportSettings(void *handle);

/***************************************************************************
Summary:
    Enable/Disable interrupts for a 4528 device
 ***************************************************************************/
static void NEXUS_Frontend_P_4528_IsrControl_isr(bool enable, void *pParam)
{
    int isrNumber = (int)pParam;

    if ( enable )
    {
        BDBG_MSG(("Enable 4528 Interrupt %u", isrNumber));
        NEXUS_Core_EnableInterrupt_isr(isrNumber);
    }
    else
    {
        BDBG_MSG(("Disable 4528 Interrupt %u", isrNumber));
        NEXUS_Core_DisableInterrupt_isr(isrNumber);
    }
}

/***************************************************************************
Summary:
    Enable/Disable gpio interrupts for a 4528 device
 ***************************************************************************/
static void NEXUS_Frontend_P_4528_GpioIsrControl_isr(bool enable, void *pParam)
{
    NEXUS_GpioHandle gpioHandle = (NEXUS_GpioHandle)pParam;

    if(enable){ 
        NEXUS_Gpio_SetInterruptEnabled_isr(gpioHandle, true);
    }
    else {
        NEXUS_Gpio_SetInterruptEnabled_isr(gpioHandle, false);
    }

   
}
static void NEXUS_Frontend_P_4528_IsrCallback(void *pParam)
{
    NEXUS_4528Device *pDevice = (NEXUS_4528Device *)pParam;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_4528Device);
    BHAB_ProcessInterruptEvent(pDevice->hab);
}

static void NEXUS_Frontend_P_4528_L1_isr(void *param1, int param2)
{
    BHAB_Handle hab = (BHAB_Handle)param2;
    BSTD_UNUSED(param1);
    BDBG_MSG(("4528 L1 ISR"));

    BHAB_HandleInterrupt_isr(hab);
}

static uint16_t NEXUS_Frontend_P_Is4528(const NEXUS_4528Settings *pSettings)
{
    BREG_I2C_Handle i2cHandle;
    uint8_t buf[4];
    uint16_t chipId=0;
    uint8_t subAddr;

    i2cHandle = NEXUS_I2c_GetRegHandle(pSettings->i2cDevice, NULL);
    buf[0]= 0x0;
    subAddr = 0x1;
    BREG_I2C_WriteNoAddr(i2cHandle, pSettings->i2cAddr, (uint8_t *)&subAddr, 1);
    BREG_I2C_ReadNoAddr(i2cHandle, pSettings->i2cAddr, buf, 1);
    chipId = buf[0];
    
    subAddr = 0x2;
    BREG_I2C_WriteNoAddr(i2cHandle, pSettings->i2cAddr, (uint8_t *)&subAddr, 1);
    BREG_I2C_ReadNoAddr(i2cHandle, pSettings->i2cAddr, buf, 1);
    chipId = (chipId <<8) | buf[0];

    BDBG_MSG(("chip ID = 0X%x", chipId));

    return chipId;
}


void NEXUS_Frontend_GetDefault4528Settings( NEXUS_4528Settings *pSettings )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

NEXUS_FrontendHandle NEXUS_Frontend_Open4528( const NEXUS_4528Settings *pSettings )
{
    unsigned i;
    NEXUS_Error errCode;
    NEXUS_4528Device *pDevice=NULL;
    NEXUS_FrontendHandle frontend;
    NEXUS_FrontendAstSettings astChannelSettings;
    uint16_t chipId=0;
    bool newDevice = false;

    BDBG_ASSERT(NULL != pSettings);

    for ( pDevice = BLST_S_FIRST(&g_deviceList); NULL != pDevice; pDevice = BLST_S_NEXT(pDevice, node) )
    {
        BDBG_OBJECT_ASSERT(pDevice, NEXUS_4528Device);
        if ((pSettings->i2cAddr == pDevice->settings.i2cAddr) && (pSettings->i2cDevice== pDevice->settings.i2cDevice))
        {
            break;
        }
    }

    if ( NULL == pDevice )
    {
        BAST_Settings astSettings;
        BHAB_Settings stHabSettings;
        BREG_I2C_Handle i2cHandle;

        BDBG_MSG(("Opening new 4528 device"));
        newDevice = true;
        chipId = NEXUS_Frontend_P_Is4528(pSettings);
        pDevice = BKNI_Malloc(sizeof(NEXUS_4528Device));
        if ( NULL == pDevice )
        {
            errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
            return NULL;
        }
        BKNI_Memset(pDevice, 0, sizeof(*pDevice));
        BDBG_OBJECT_SET(pDevice, NEXUS_4528Device);
        pDevice->settings = *pSettings;

        i2cHandle = NEXUS_I2c_GetRegHandle(pSettings->i2cDevice, NEXUS_MODULE_SELF);
        BDBG_ASSERT(NULL != i2cHandle);

        errCode = BHAB_4528_GetDefaultSettings(&stHabSettings);

        stHabSettings.chipAddr = pSettings->i2cAddr;
        if(pSettings->isrNumber) {
            stHabSettings.interruptEnableFunc = NEXUS_Frontend_P_4528_IsrControl_isr;
            stHabSettings.interruptEnableFuncParam = (void*)pSettings->isrNumber;
        }
        else if(pSettings->gpioInterrupt){
            stHabSettings.interruptEnableFunc = NEXUS_Frontend_P_4528_GpioIsrControl_isr;
            stHabSettings.interruptEnableFuncParam = (void*)pSettings->gpioInterrupt;
        }

        stHabSettings.slaveChipAddr = pSettings->i2cSlaveAddr;
        errCode = BHAB_Open( &pDevice->hab, (void *)i2cHandle, &stHabSettings);
        if ( errCode != BERR_SUCCESS ) { BDBG_ERR(("Frontend unable to initialize HAB")); goto err_init; }

        BAST_4528_GetDefaultSettings(&astSettings);
        astSettings.i2c.chipAddr = pSettings->i2cAddr;
        if(pSettings->isrNumber) {
            astSettings.i2c.interruptEnableFunc = NEXUS_Frontend_P_4528_IsrControl_isr;
            astSettings.i2c.interruptEnableFuncParam = (void*)pSettings->isrNumber;
        }
        else if(pSettings->gpioInterrupt){
            astSettings.i2c.interruptEnableFunc = NEXUS_Frontend_P_4528_GpioIsrControl_isr;
            astSettings.i2c.interruptEnableFuncParam = (void*)pSettings->gpioInterrupt;
        }
        
        errCode = BAST_Open(&pDevice->astHandle, NULL /*chip*/, pDevice->hab, NULL /*BINT*/, &astSettings);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
            goto err_init;
        }

        /* Determine number of channels -- they will be opened later */
        BAST_GetTotalChannels(pDevice->astHandle, &pDevice->numChannels);
        if ( pDevice->numChannels > NEXUS_4528_MAX_FRONTEND_CHANNELS )
        {
            BDBG_WRN(("This 4528 device supports more than the expected number of channels. Unused channels will be powered down"));
        }

        /* Open all channels prior to InitAp */
        for ( i = 0; i < pDevice->numChannels; i++ )
        {
            BAST_ChannelSettings bastChannelSettings;
            BAST_GetChannelDefaultSettings(pDevice->astHandle, i, &bastChannelSettings);
            errCode = BAST_OpenChannel(pDevice->astHandle, &pDevice->astChannels[i], i, &bastChannelSettings);
            if ( errCode ) {
                BDBG_ERR(("Unable to open channel %d", i));
                errCode = BERR_TRACE(errCode);
                goto err_init;
            }
        }

        if(pSettings->isrNumber) {      
            errCode = NEXUS_Core_ConnectInterrupt(pSettings->isrNumber,
                                                 NEXUS_Frontend_P_4528_L1_isr,
                                                 (void *)pDevice,
                                                 (int)pDevice->hab);
            if ( errCode != BERR_SUCCESS )
            {
                errCode = BERR_TRACE(errCode);
                goto err_init;
            }
        }

        BDBG_WRN(("Initializing 4528 Frontend core..."));
        /* Init the acquisition processor */
        errCode = BAST_InitAp(pDevice->astHandle, bcm4528_ap_image);
        if ( errCode ) {
            errCode = BERR_TRACE(errCode);
            goto err_init;
        }

        /* Success opeining the AST Device.  Connect Interrupt */
        
       
        BDBG_WRN(("Initializing 4528 core... Done"));

        BHAB_GetInterruptEventHandle(pDevice->hab, &pDevice->isrEvent);
        pDevice->isrCallback = NEXUS_RegisterEvent(pDevice->isrEvent, NEXUS_Frontend_P_4528_IsrCallback, pDevice);
        if ( NULL == pDevice->isrCallback )
        {
            errCode = BERR_TRACE(BERR_OS_ERROR);
            goto err_init;
        }       
        
        BLST_S_INSERT_HEAD(&g_deviceList, pDevice, node);
    }
    else
    {
        BDBG_MSG(("Found existing device"));
    }

    BDBG_OBJECT_ASSERT(pDevice, NEXUS_4528Device);

    if ( pSettings->channelNumber >= pDevice->numChannels )
    {
        BDBG_ERR(("Channel %u not supported on this device", pSettings->channelNumber));
        errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
        return NULL;
    }
    if ( NULL != pDevice->handles[pSettings->channelNumber] )
    {
        BDBG_ERR(("Channel %u already open", pSettings->channelNumber));
        errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
        return NULL;
    }

    BDBG_MSG(("Creating channel %u", pSettings->channelNumber));

    /* Open channel */
    NEXUS_Frontend_P_Ast_GetDefaultSettings(&astChannelSettings);
    astChannelSettings.astHandle = pDevice->astHandle;
    astChannelSettings.astChannel = pDevice->astChannels[pSettings->channelNumber];
#define B_AST_CHIP 4528
    astChannelSettings.astChip = B_AST_CHIP;
    astChannelSettings.closeFunction = NEXUS_Frontend_P_4528_CloseCallback;
    astChannelSettings.pCloseParam = pDevice;
    astChannelSettings.channelIndex = pSettings->channelNumber;
#if 0
    /* turn off what I don't support: last channel is qpsk only on 4528 */
    if (pSettings->channelNumber == NEXUS_4528_MAX_FRONTEND_CHANNELS-1)
    {
        astChannelSettings.capabilities.satelliteModes[NEXUS_FrontendSatelliteMode_eQpskTurbo] = false;
        astChannelSettings.capabilities.satelliteModes[NEXUS_FrontendSatelliteMode_eTurboQpsk] = false;
        astChannelSettings.capabilities.satelliteModes[NEXUS_FrontendSatelliteMode_e8pskTurbo] = false;
        astChannelSettings.capabilities.satelliteModes[NEXUS_FrontendSatelliteMode_eTurbo8psk] = false;
        astChannelSettings.capabilities.satelliteModes[NEXUS_FrontendSatelliteMode_eTurbo] = false;
        astChannelSettings.capabilities.satelliteModes[NEXUS_FrontendSatelliteMode_eQpskLdpc] = false;
        astChannelSettings.capabilities.satelliteModes[NEXUS_FrontendSatelliteMode_e8pskLdpc] = false;
        astChannelSettings.capabilities.satelliteModes[NEXUS_FrontendSatelliteMode_eLdpc] = false;
        astChannelSettings.capabilities.diseqc = false;
    }
#endif
    frontend = NEXUS_Frontend_P_Ast_Create(&astChannelSettings);
    if ( !frontend )
    {
        errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
        NEXUS_Frontend_P_4528_CloseCallback(NULL, pDevice); /* Check if channel needs to be closed */
        return NULL;
    }
    frontend->tuneSatellite = NEXUS_Frontend_P_4528_TuneSatellite;
    frontend->untune = NEXUS_Frontend_P_4528_Untune;
    frontend->reapplyTransportSettings = NEXUS_Frontend_P_4528_ReapplyTransportSettings;
    
    pDevice->handles[pSettings->channelNumber] = frontend;

#if NEXUS_HAS_MXT
    if (newDevice)
    {
        /* open MXT */
        BERR_Code rc;
        BMXT_Settings mxtSettings;
        BMXT_4528_GetDefaultSettings(&mxtSettings);
        for (i=0; i<BMXT_NUM_MTSIF; i++) {
            mxtSettings.MtsifTxCfg[i].Enable = true;
        }
        mxtSettings.hHab = pDevice->hab;
        rc = BMXT_Open(&pDevice->mxt, NULL, NULL, &mxtSettings);
        if (rc!=BERR_SUCCESS) goto err_init;

{
        BMXT_ParserConfig parserConfig;
        BKNI_Memset(&pDevice->mtsifConfig, 0, sizeof(NEXUS_FrontendDeviceMtsifConfig));
        pDevice->mtsifConfig.pMxtHandle = pDevice->mxt;
        pDevice->mtsifConfig.numDemodIb = BMXT_GetNumResources(mxtSettings.chip, mxtSettings.chipRev, BMXT_ResourceType_eInputBand);
        pDevice->mtsifConfig.numDemodPb = BMXT_GetNumResources(mxtSettings.chip, mxtSettings.chipRev, BMXT_ResourceType_eParser);

        for (i=0; i<pDevice->mtsifConfig.numDemodPb; i++) {
            rc = BMXT_GetParserConfig(pDevice->mxt, i, &parserConfig);
            if (rc) { rc = BERR_TRACE(rc); goto err_init; }

            /* turn off demod parsers */
            parserConfig.Enable = false;

            /* cache other settings */
            pDevice->mtsifConfig.demodPbSettings[i].errorInputIgnore = parserConfig.ErrorInputIgnore;
            pDevice->mtsifConfig.demodPbSettings[i].timestampMode = (unsigned)parserConfig.TsMode;
            pDevice->mtsifConfig.demodPbSettings[i].dssMode = parserConfig.DssMode;

            rc = BMXT_SetParserConfig(pDevice->mxt, i, &parserConfig);
            if (rc) { rc = BERR_TRACE(rc); goto err_init; }
        }
}

    }
#endif

    return frontend;

err_init:
    NEXUS_Frontend_P_4528_DestroyDevice(pDevice);
    return NULL;
}


NEXUS_Error NEXUS_Frontend_P_Ast_TuneSatellite(void *handle, const NEXUS_FrontendSatelliteSettings *pSettings);
void NEXUS_Frontend_P_Ast_Untune(void *handle);

static NEXUS_Error NEXUS_Frontend_P_4528_TuneSatellite(void *handle, const NEXUS_FrontendSatelliteSettings *pSettings)
{
    NEXUS_AstDevice *pAstDevice = handle;
    /* for now, pCloseParam is our backdoor to a NEXUS_4528Device. the alternative is to add a new void* inside NEXUS_AstDevice */
    NEXUS_4528Device *p4528Device = pAstDevice->settings.pCloseParam;
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(p4528Device, NEXUS_4528Device);
    
    rc = NEXUS_Frontend_P_SetMtsifConfig(pAstDevice->frontendHandle, &p4528Device->mtsifConfig);
    if (rc) { return BERR_TRACE(rc); }
    rc = NEXUS_Frontend_P_SetTransportConfig(pAstDevice->frontendHandle, &p4528Device->mtsifConfig);
    if (rc) { return BERR_TRACE(rc); }

    return NEXUS_Frontend_P_Ast_TuneSatellite(pAstDevice, pSettings);
}

static void NEXUS_Frontend_P_4528_Untune(void *handle)
{
    NEXUS_AstDevice *pAstDevice = handle;
    NEXUS_4528Device *p4528Device = pAstDevice->settings.pCloseParam;

    BDBG_OBJECT_ASSERT(p4528Device, NEXUS_4528Device);
    
    NEXUS_Frontend_P_UnsetMtsifConfig(pAstDevice->frontendHandle, &p4528Device->mtsifConfig);
    NEXUS_Frontend_P_UnsetTransportConfig(pAstDevice->frontendHandle, &p4528Device->mtsifConfig);
    
    NEXUS_Frontend_P_Ast_Untune(pAstDevice);
    return;
}

NEXUS_Error NEXUS_Frontend_P_4528_ReapplyTransportSettings(void *handle)
{
    NEXUS_AstDevice *pAstDevice = handle;
    NEXUS_4528Device *p4528Device = pAstDevice->settings.pCloseParam;
    NEXUS_Error rc;

    rc = NEXUS_Frontend_P_SetMtsifConfig(pAstDevice->frontendHandle, &p4528Device->mtsifConfig);
    if (rc) { return BERR_TRACE(rc); }
    rc = NEXUS_Frontend_P_SetTransportConfig(pAstDevice->frontendHandle, &p4528Device->mtsifConfig);
    if (rc) { return BERR_TRACE(rc); }
    
    return NEXUS_SUCCESS;
}


static void NEXUS_Frontend_P_4528_CloseCallback(NEXUS_FrontendHandle handle, void *pParam)
{
    unsigned i;
    NEXUS_4528Device *pDevice = pParam;

    BDBG_OBJECT_ASSERT(pDevice, NEXUS_4528Device);

    /* Mark handle as destroyed */
    if ( handle ) {
        for ( i = 0; i < pDevice->numChannels; i++ ) {
            if ( handle == pDevice->handles[i] ) {
                pDevice->handles[i] = NULL;
                break;
            }
        }
        BDBG_ASSERT(i < pDevice->numChannels);
    }

    /* See if any channels are still open */
    for ( i = 0; i < pDevice->numChannels; i++ ) {
        if ( pDevice->handles[i] ) {
            return;
        }
    }

    /* If we got here, the device is ready to be destroyed */
    NEXUS_Frontend_P_4528_DestroyDevice(pDevice);
}

static void NEXUS_Frontend_P_4528_DestroyDevice(NEXUS_4528Device *pDevice)
{
    unsigned i;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_4528Device);

#if NEXUS_HAS_MXT
    if (pDevice->mxt) {
        BMXT_Close(pDevice->mxt);
    }
#endif

    for ( i = 0; i < pDevice->numChannels; i++ )
    {
        if ( NULL != pDevice->handles[i] )
        {
            BDBG_ERR(("All channels must be closed before destroying device"));
            BDBG_ASSERT(NULL == pDevice->handles[i]);
        }
    }

    for ( i = 0; i < pDevice->numChannels && NULL != pDevice->astChannels[i]; i++) {
        BAST_CloseChannel(pDevice->astChannels[i]);
    }
    BAST_Close(pDevice->astHandle);

    BDBG_MSG(("Destroying 4528 device %p", pDevice));

    if (pDevice->isrCallback) {
        NEXUS_UnregisterEvent(pDevice->isrCallback);
    }
    NEXUS_Core_DisconnectInterrupt(pDevice->settings.isrNumber);
    
    BLST_S_REMOVE(&g_deviceList, pDevice, NEXUS_4528Device, node);
    BDBG_OBJECT_DESTROY(pDevice, NEXUS_4528Device);
    BKNI_Free(pDevice);
}

