/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_spi.c $
 * $brcm_Revision: 22 $
 * $brcm_Date: 9/28/12 2:43p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/spi/src/nexus_spi.c $
 * 
 * 22   9/28/12 2:43p vishk
 * SW7425-3873: lastByteContinueEnable support in NEXUS_Spi_SetSettings
 * 
 * 21   8/24/12 4:17p vishk
 * SW7358-345: Merge changes to mainline. Change the channel indexing to
 *  include spi core type.
 * 
 * 20   8/3/12 3:45p erickson
 * SW7425-1747: remove old code
 * 
 * 19   7/20/12 10:57a randyjew
 * SW7468-247: Fix for7468
 * 
 * 18   6/19/12 12:43p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 17   2/24/12 6:51p vishk
 * SW7231-68: add SPI support on top of I2C for 3461 interface
 * 
 * 16   11/12/10 3:31p gmohile
 * SW7408-145 : Fix 7408 support
 * 
 * 15   11/3/10 1:39p jhaberf
 * SW35125-1: Added 35125 DTV chip support
 * 
 * 14   6/9/10 2:48p jhaberf
 * SW35230-67: integrated latest nexus SPI support for the 35230
 * 
 * 13   5/19/10 5:20p jhaberf
 * SW35230-67: Added bHeap and bufferSize structure members for 35230
 * 
 * 12   5/13/10 4:32p jhaberf
 * SW35230-67: Added nexus spi changes for 35230 DTV chip
 * 
 * 11   1/7/10 11:33a jhaberf
 * SW35230-1: Added some #fidef's in order to get module building for
 *  35230 DTV chip
 * 
 * 10   1/6/10 11:33a mphillip
 * SW3548-2660: Merge interrupt vs. polling mode changes to main
 * 
 * SW3548-2660/2   1/6/10 11:29a mphillip
 * SW3548-2660: Rename variable exposing interrupt vs. polling mode for
 *  transfers
 * 
 * SW3548-2660/1   1/5/10 6:06p mphillip
 * SW3548-2660: Add a user setting to enable polling for transfers
 * 
 * 9   11/11/09 11:53a erickson
 * SW7405-3362: add dtl and rdsclk fields to NEXUS_SpiSettings
 *
 * 8   1/26/09 12:05p erickson
 * PR51468: global variable naming convention
 *
 * 7   1/26/09 11:07a erickson
 * PR51468: global variable naming convention
 *
 * 6   11/20/08 8:27a ahulse
 * PR49515: If set, override default settings with user set settings
 *
 * 5   8/18/08 10:28a katrep
 * PR45674: Compiler warnings in DEBUG=n builds
 *
 * 4   4/11/08 9:53a erickson
 * PR41246: convert BDBG_OBJECT_UNSET to BDBG_OBJECT_DESTROY if freeing
 *  memory
 *
 * 3   3/31/08 12:32p erickson
 * PR41073: check result of malloc and fail graciously
 *
 * 2   1/25/08 2:29p erickson
 * PR39016: implemented Read,Write,SetSettings
 *
 * 1   1/18/08 2:21p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/3   11/21/07 11:12a erickson
 * PR37423: update
 *
 * Nexus_Devel/2   11/20/07 2:23p erickson
 * PR37423: simplify module init
 *
 * Nexus_Devel/1   11/20/07 1:28p erickson
 * PR37423: added uart, gpio, spi modules
 *
 **************************************************************************/
#include "nexus_spi_module.h"
#include "bspi.h"
#include "breg_spi.h"
#if (BCHP_CHIP==7408 || BCHP_CHIP==7468)
#include "bchp_hif_mspi.h"
#else 
#include "bchp_mspi.h"
#endif
#include "priv/nexus_core.h"

BDBG_MODULE(nexus_spi);

#ifndef BCHP_MSPI_SPCR0_MSB_CPOL_MASK
#define BCHP_MSPI_SPCR0_MSB_CPOL_MASK BCHP_HIF_MSPI_SPCR0_MSB_CPOL_MASK
#define BCHP_MSPI_SPCR0_MSB_CPHA_MASK BCHP_HIF_MSPI_SPCR0_MSB_CPHA_MASK
#endif

NEXUS_ModuleHandle g_NEXUS_spiModule;
struct {
    NEXUS_SpiModuleSettings settings;
    BSPI_Handle spi;
} g_NEXUS_spi;

/****************************************
* Module functions
***************/

void NEXUS_SpiModule_GetDefaultSettings(NEXUS_SpiModuleSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

NEXUS_ModuleHandle NEXUS_SpiModule_Init(const NEXUS_SpiModuleSettings *pSettings)
{
    BSPI_Settings spiSettings;
    BERR_Code rc;

    BDBG_ASSERT(!g_NEXUS_spiModule);
    g_NEXUS_spiModule = NEXUS_Module_Create("spi", NULL);
    if (pSettings) {
        g_NEXUS_spi.settings = *pSettings;
    }
    else {
        NEXUS_SpiModule_GetDefaultSettings(&g_NEXUS_spi.settings);
    }

    BSPI_GetDefaultSettings(&spiSettings, g_pCoreHandles->chp);
    rc = BSPI_Open(&g_NEXUS_spi.spi, g_pCoreHandles->chp, g_pCoreHandles->reg, g_pCoreHandles->bint, &spiSettings);
    if (rc) {rc=BERR_TRACE(NEXUS_UNKNOWN); return NULL;}

    return g_NEXUS_spiModule;
}

void NEXUS_SpiModule_Uninit(void)
{
    BSPI_Close(g_NEXUS_spi.spi);
    NEXUS_Module_Destroy(g_NEXUS_spiModule);
    g_NEXUS_spiModule = NULL;
}

/****************************************
* API functions
***************/

struct NEXUS_Spi {
    NEXUS_OBJECT(NEXUS_Spi);
    BSPI_ChannelHandle spiChannel;
    BREG_SPI_Handle spiReg;
    NEXUS_SpiSettings settings;
};

void NEXUS_Spi_GetDefaultSettings(NEXUS_SpiSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->txLeadingCapFalling = true;
    pSettings->clockActiveLow = true;
    pSettings->interruptMode = true;
}

NEXUS_SpiHandle NEXUS_Spi_Open(unsigned index, const NEXUS_SpiSettings *pSettings)
{
    BSPI_ChannelSettings channelSettings;
    NEXUS_SpiHandle spi;
    BERR_Code rc;
    unsigned totalChannels;
    NEXUS_SpiSettings defaultSettings;
    BSPI_ChannelInfo    channelInfo;

    if (!pSettings) {
        NEXUS_Spi_GetDefaultSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }
    channelInfo.channelNo = index & 0x1; /* Both HIF and UPG spi can only support two channels each, for now. */

    BSPI_GetTotalChannels(g_NEXUS_spi.spi, &totalChannels);
    if (channelInfo.channelNo >= totalChannels) {
        BDBG_ERR(("invalid Spi[%d]", index));
        return NULL;
    }

    spi = BKNI_Malloc(sizeof(*spi));
    if (!spi) {
        rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }
    NEXUS_OBJECT_INIT(NEXUS_Spi, spi);

    if((index>>16) == NEXUS_SpiCore_eHif){
        channelInfo.spiCore = BSPI_SpiCore_Hif;
        BSPI_GetChannelDefaultSettings_Ext(g_NEXUS_spi.spi, channelInfo, &channelSettings);
    }
    else{
        channelInfo.spiCore = BSPI_SpiCore_Upg;
        BSPI_GetChannelDefaultSettings_Ext(g_NEXUS_spi.spi, channelInfo, &channelSettings);
    }

    if ( pSettings->baud ) {
        channelSettings.baud                   = pSettings->baud;
    }
    if ( pSettings->bitsPerTransfer ) {
        channelSettings.bitsPerTxfr            = pSettings->bitsPerTransfer;
    }
    if ( pSettings->lastByteContinueEnable ) {
        channelSettings.lastByteContinueEnable = pSettings->lastByteContinueEnable;
    }
    if ( pSettings->useUserDtlAndDsclk ) {
        channelSettings.useUserDtlAndDsclk     = pSettings->useUserDtlAndDsclk;
    }
    if ( pSettings->ebiChipSelect) {
        channelSettings.ebiCs = pSettings->ebiChipSelect;
    }
    channelSettings.intMode                    = pSettings->interruptMode;
    spi->settings = *pSettings;

    rc = BSPI_OpenChannel(g_NEXUS_spi.spi, &spi->spiChannel, channelInfo.channelNo, &channelSettings);
    if (rc) {rc=BERR_TRACE(rc); goto error;}

    rc = BSPI_CreateSpiRegHandle(spi->spiChannel, &spi->spiReg);
    if (rc) {rc=BERR_TRACE(rc); goto error;}

    rc = NEXUS_Spi_SetSettings(spi, pSettings);
    if (rc) {rc=BERR_TRACE(rc); goto error;}

    return spi;
error:
    NEXUS_Spi_Close(spi);
    return NULL;
}

static void NEXUS_Spi_P_Finalizer(NEXUS_SpiHandle spi)
{
    NEXUS_OBJECT_ASSERT(NEXUS_Spi, spi);

    if (spi->spiReg) {
        BSPI_CloseSpiRegHandle(spi->spiReg);
    }
    if (spi->spiChannel) {
        BSPI_CloseChannel(spi->spiChannel);
    }

    NEXUS_OBJECT_DESTROY(NEXUS_Spi, spi);
    BKNI_Free(spi);
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_Spi, NEXUS_Spi_Close);

void NEXUS_Spi_GetSettings(NEXUS_SpiHandle spi, NEXUS_SpiSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(spi, NEXUS_Spi);
    *pSettings = spi->settings;
}

NEXUS_Error NEXUS_Spi_SetSettings(NEXUS_SpiHandle spi, const NEXUS_SpiSettings *pSettings)
{
    uint8_t clkConfig;
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(spi, NEXUS_Spi);

    /* if open-time only settings are changed, fail */
    if (pSettings->baud != spi->settings.baud ||
        pSettings->bitsPerTransfer != spi->settings.bitsPerTransfer ||
        pSettings->lastByteContinueEnable != spi->settings.lastByteContinueEnable ||
        pSettings->useUserDtlAndDsclk != spi->settings.useUserDtlAndDsclk ||
        pSettings->interruptMode != spi->settings.interruptMode)
    {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }    
    
    BSPI_GetClkConfig(spi->spiChannel, &clkConfig);
    if (pSettings->clockActiveLow)
        clkConfig |= BCHP_MSPI_SPCR0_MSB_CPOL_MASK; /* if 1, then 0 is active */
    else
        clkConfig &= ~BCHP_MSPI_SPCR0_MSB_CPOL_MASK; /* if 0, then 1 is active */
    if (pSettings->txLeadingCapFalling)
        clkConfig |= BCHP_MSPI_SPCR0_MSB_CPHA_MASK;
    else
        clkConfig &= ~BCHP_MSPI_SPCR0_MSB_CPHA_MASK;

    rc = BSPI_SetClkConfig(spi->spiChannel, clkConfig);
    if (rc) return BERR_TRACE(rc);

    rc = BSPI_SetDTLConfig(spi->spiChannel, pSettings->dtl);
    if (rc) return BERR_TRACE(rc);

    rc = BSPI_SetRDSCLKConfig(spi->spiChannel, pSettings->rdsclk);
    if (rc) return BERR_TRACE(rc);

    BSPI_SetLastByteContinueEnable(spi->spiChannel, pSettings->lastByteContinueEnable);

    spi->settings = *pSettings;
    return 0;
}

NEXUS_Error NEXUS_Spi_Write(NEXUS_SpiHandle spi, const uint8_t *pWriteData,
    size_t length)
{
    BERR_Code rc;
    BDBG_OBJECT_ASSERT(spi, NEXUS_Spi);
    rc = BREG_SPI_Write(spi->spiReg, pWriteData, length);
    if (rc) return BERR_TRACE(rc);
    return 0;
}

NEXUS_Error NEXUS_Spi_Read(NEXUS_SpiHandle spi, const uint8_t *pWriteData,
    uint8_t *pReadData, size_t length)
{
    BERR_Code rc;
    BDBG_OBJECT_ASSERT(spi, NEXUS_Spi);
    rc = BREG_SPI_Read(spi->spiReg, pWriteData, pReadData, length);
    if (rc) return BERR_TRACE(rc);
    return 0;
}

#if defined(HUMAX_PLATFORM_BASE)
NEXUS_Error NEXUS_Spi_WritebyMode(NEXUS_SpiHandle spi, unsigned char mode, const uint8_t *pWriteData,
    size_t length)
{
    BERR_Code rc;
    BDBG_OBJECT_ASSERT(spi, NEXUS_Spi);
    rc = BREG_SPI_WritebyMode(spi->spiReg, mode, pWriteData, length);
    if (rc) return BERR_TRACE(rc);
    return 0;
}

NEXUS_Error NEXUS_Spi_ReadbyMode(NEXUS_SpiHandle spi, unsigned char mode, const uint8_t *pWriteData,
    uint8_t *pReadData, size_t length)
{
    BERR_Code rc;
    BDBG_OBJECT_ASSERT(spi, NEXUS_Spi);
    rc = BREG_SPI_ReadbyMode(spi->spiReg,mode, pWriteData, pReadData, length);
    if (rc) return BERR_TRACE(rc);
    return 0;
}
#endif

BREG_SPI_Handle NEXUS_Spi_GetRegHandle(
    NEXUS_SpiHandle spiHandle
    )
{
    BDBG_OBJECT_ASSERT(spiHandle, NEXUS_Spi);

    return spiHandle->spiReg;
}

