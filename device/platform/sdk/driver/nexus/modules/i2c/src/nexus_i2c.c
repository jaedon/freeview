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
* $brcm_Workfile: nexus_i2c.c $
* $brcm_Revision: PROD_HUMAX_URSR/1 $
* $brcm_Date: 8/30/13 6:47p $
*
* API Description:
*   API name: I2c
*    Specific APIs related to I2c Control.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/i2c/src/nexus_i2c.c $
* 
* PROD_HUMAX_URSR/1   8/30/13 6:47p scho
* PROD_HUMAX_URSR: Make NEXUS_Platform_Init() asynchronous. Make
*  FrontendDeviceOpen asynchrnous. Use NEXUS_FrontendDevice_GetStatus()
*  to determine if the frontend device is successfully opened or if it
*  failed. Also implement parallel download of firmware to frontend
*  devices controlled by seperate I2C controllers. The same feature also
*  implemented for resume from S3 standby.
* 
* 18   8/6/12 11:16a erickson
* SW7231-929: virtualize nexus i2c interface
* 
* 17   6/19/12 12:42p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* 16   4/13/12 9:32a erickson
* SW7346-741: Add no sub address option for WriteRead
* 
* 15   4/2/12 3:02p vsilyaev
* SW7346-741:  Merged I2C WriteRead.
* 
* SWNOOS-527/1   4/2/12 2:10p agin
* SWNOOS-527:  Support I2C WriteRead.
* 
* 14   1/3/12 4:16p erickson
* SWNOOS-507: use memset and GetDefaultSettings to avoid uninitialized
*  memory for code mismatches
* 
* 13   1/3/12 1:51p erickson
* SWNOOS-507: add NEXUS_I2cSettings.fourByteXferMode
* 
* 12   12/2/11 6:32p jtna
* SW7425-1708: add i2c s3 standby power management
* 
* 11   4/6/11 5:01p erickson
* SW7420-1671: add warning
*
* 10   10/4/10 2:27p erickson
* SW35230-1525: merge NEXUS_I2c_WriteRead
*
* SW35230-1525/1   9/30/10 6:04p adtran
* SW35230-1525: add new NEXUS_I2c_WriteRead API
*
* 9   10/22/09 2:36p erickson
* SW7405-2686: add softI2c support
*
* 8   9/25/09 3:56p erickson
* SW7405-3038: added NEXUS_I2c_SwReset
*
* 7   4/2/09 11:36a erickson
* PR51899: merge
*
* PR51899/1   3/31/09 2:41p vle
* PR51899: Add I2C_ReadSwEDDC support to nexus
*
* 6   2/18/09 9:38a erickson
* PR52223: add NEXUS_I2c_SetSettings
*
* 5   1/26/09 12:05p erickson
* PR51468: global variable naming convention
*
* 4   12/16/08 2:54p erickson
* PR48497: merge
*
* HDMI_TX_Plugfest11_200811/1   11/13/08 7:35p vle
* PR 48987: Add run time option to enable I2C bit bang mode.
*
* 3   4/24/08 9:54a erickson
* PR41989: added NEXUS_I2c_ReadNoAddrNoAck
*
* 2   4/11/08 9:53a erickson
* PR41246: convert BDBG_OBJECT_UNSET to BDBG_OBJECT_DESTROY if freeing
*  memory
*
* 1   1/18/08 2:21p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/5   11/7/07 9:44a erickson
* PR36725: use BDBG_OBJECT instead of just checking handle != NULL
*
* Nexus_Devel/4   11/5/07 2:12p erickson
* PR36725: remove NEXUS_I2c_Get
*
* Nexus_Devel/3   10/10/07 11:29a jgarrett
* PR 35551: Revising I2C Handle for synchronization purposes
*
* Nexus_Devel/2   10/5/07 5:52p jgarrett
* PR 35744: Removing debug timeout
*
* Nexus_Devel/1   10/5/07 1:42p jgarrett
* PR 35744: Adding i2c module
*
* Nexus_Devel/2   9/12/07 4:35p jgarrett
* PR 34702: Adding I2C interface
*
* Nexus_Devel/1   9/12/07 10:39a jgarrett
* PR 34702: Adding initial IO headers
*
***************************************************************************/

#include "nexus_i2c_module.h"
#include "breg_i2c.h"
#include "bi2c.h"
#include "priv/nexus_core.h"
#include "nexus_base.h"
#include "nexus_platform_features.h"
#include "breg_i2c_priv.h"  /* For BREG_I2C_Impl */

BDBG_MODULE(nexus_i2c);

static struct magnum_i2c_channel {
    BI2C_ChannelHandle channel; /* Set only for local masters, NULL otherwise */
    BREG_I2C_Impl asyncReg;     /* A pointer to this will be returned when the module owner requests a handle */
    unsigned refcnt;
    bool s3standby;
    NEXUS_I2cSettings settings; /* settings used to open */
} g_magnum_i2c_channel[NEXUS_NUM_I2C_CHANNELS];

typedef struct NEXUS_I2c
{
    NEXUS_OBJECT(NEXUS_I2c);
    BLST_S_ENTRY(NEXUS_I2c) link; /* g_i2c_channels */
    unsigned index;
    BI2C_ChannelHandle channel; /* copy from g_magnum_i2c_channel[index] */
    NEXUS_ModuleHandle owner;   /* The module that created this handle.  Required for synchronization decisions. */
    BREG_I2C_Impl syncReg;      /* A pointer to this will be returned when someone other than the module owner requests a handle */
    BREG_I2C_Impl asyncReg;     /* copy from g_magnum_i2c_channel[index] */
    NEXUS_I2cSettings settings;
    bool first; /* first one to open can set settings */
} NEXUS_I2c;

static BLST_S_HEAD(nexus_i2c_list, NEXUS_I2c) g_i2c_channels;

static void NEXUS_I2c_P_FillSyncRegHandle(
    NEXUS_I2c *pI2cDevice,
    NEXUS_ModuleHandle moduleHandle
    );

void NEXUS_I2c_GetDefaultSettings(
    NEXUS_I2cSettings *pSettings    /* [out] */
    )
{
    BI2C_ChannelSettings channelSettings;

    BDBG_ASSERT(NULL != pSettings);

    BI2C_GetChannelDefaultSettings(g_NEXUS_i2cHandle, 0, &channelSettings);

    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->clockRate = (NEXUS_I2cClockRate)channelSettings.clkRate;
    pSettings->interruptMode = channelSettings.intMode;
    pSettings->timeout = channelSettings.timeoutMs;
    pSettings->burstMode = channelSettings.burstMode;
    pSettings->softI2c = channelSettings.softI2c;
    pSettings->fourByteXferMode = channelSettings.fourByteXferMode;
}

static NEXUS_Error nexus_i2c_p_open_channel(NEXUS_I2cHandle i2cHandle)
{
    struct magnum_i2c_channel *magnum_i2c = &g_magnum_i2c_channel[i2cHandle->index];
    /* there is an assumption that i2c functions are atomic and therefore the nexus i2c
    interface can be virtualized. 
    if a non-atomic function is added, a separate lock/unlock API must be required to make 
    the api transactional. */
    if (magnum_i2c->refcnt++ == 0) {
        BERR_Code errCode;
        BI2C_ChannelSettings channelSettings;
        BREG_I2C_Handle regHandle;
        const NEXUS_I2cSettings *pSettings = &i2cHandle->settings;
        
        magnum_i2c->settings = *pSettings;
        
        BDBG_CASSERT(BI2C_Clk_eClk400Khz == NEXUS_I2cClockRate_e400Khz);
        BI2C_GetChannelDefaultSettings(g_NEXUS_i2cHandle, i2cHandle->index, &channelSettings);
        channelSettings.clkRate = pSettings->clockRate;
        channelSettings.intMode = pSettings->interruptMode;
        channelSettings.timeoutMs = pSettings->timeout;
        channelSettings.burstMode = pSettings->burstMode;
        channelSettings.softI2c = pSettings->softI2c;
        channelSettings.fourByteXferMode = pSettings->fourByteXferMode;
        BDBG_MSG(("Open I2C Channel %d: clk %d, int %d, timeout %d, burst %d, softI2c %d, four byte xfer mode %d",
            i2cHandle->index, channelSettings.clkRate, channelSettings.intMode, channelSettings.timeoutMs, channelSettings.burstMode, channelSettings.softI2c, channelSettings.fourByteXferMode));
        errCode = BI2C_OpenChannel(g_NEXUS_i2cHandle, &magnum_i2c->channel, i2cHandle->index, &channelSettings);
        if ( errCode )
        {
            BDBG_ERR(("Unable to open I2C Device Channel"));
            return BERR_TRACE(errCode);
        }

        errCode = BI2C_CreateI2cRegHandle(magnum_i2c->channel, &regHandle);
        if ( errCode )
        {
            BDBG_ERR(("Unable to create BREG I2C Handle"));
            BI2C_CloseChannel(magnum_i2c->channel);
            return BERR_TRACE(errCode);
        }

        /* The default handle is our async version */
        BKNI_Memcpy(&magnum_i2c->asyncReg, regHandle, sizeof(BREG_I2C_Impl));
        BI2C_CloseI2cRegHandle(regHandle);
    }
    
    /* copy handles */
    i2cHandle->channel = magnum_i2c->channel;
    i2cHandle->asyncReg = magnum_i2c->asyncReg;
    
    i2cHandle->first = (magnum_i2c->refcnt == 1);
    
    return 0;
}

static void nexus_i2c_p_close_channel(NEXUS_I2cHandle i2cHandle)
{
    struct magnum_i2c_channel *magnum_i2c = &g_magnum_i2c_channel[i2cHandle->index];
    BDBG_ASSERT(magnum_i2c->refcnt);
    if (--magnum_i2c->refcnt == 0) {
        BDBG_MSG(("Close I2C Channel %d", i2cHandle->index));
        BI2C_CloseChannel(magnum_i2c->channel);
    }
}
    
NEXUS_I2cHandle NEXUS_I2c_Open(
    unsigned channelIndex,
    const NEXUS_I2cSettings *pSettings
    )
{
    NEXUS_I2cHandle i2cHandle;
    NEXUS_I2cSettings settings;
    NEXUS_Error errCode;

    if ( channelIndex >= NEXUS_NUM_I2C_CHANNELS )
    {
        BDBG_ERR(("Channel index %u out of range", channelIndex));
        errCode = BERR_TRACE(BERR_INVALID_PARAMETER);
        return NULL;
    }

    if ( NULL == pSettings )
    {
        NEXUS_I2c_GetDefaultSettings(&settings);
        pSettings = &settings;
    }

    i2cHandle = BKNI_Malloc(sizeof(*i2cHandle));
    if (!i2cHandle) {
        errCode = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }
    NEXUS_OBJECT_INIT(NEXUS_I2c, i2cHandle);
    i2cHandle->settings = *pSettings;
    i2cHandle->index = channelIndex;
    BLST_S_INSERT_HEAD(&g_i2c_channels, i2cHandle, link);
    BDBG_MSG(("Opening I2C Channel %p, %u", i2cHandle, i2cHandle->index));
    
    errCode = nexus_i2c_p_open_channel(i2cHandle);
    if (errCode) {errCode = BERR_TRACE(errCode); goto error;}

    NEXUS_I2c_P_FillSyncRegHandle(i2cHandle, NEXUS_MODULE_SELF);
        
    return i2cHandle;

error:
    NEXUS_I2c_Close(i2cHandle);
    return NULL;
}

static void NEXUS_I2c_P_Finalizer(
    NEXUS_I2cHandle i2cHandle
    )
{
    NEXUS_OBJECT_ASSERT(NEXUS_I2c, i2cHandle);
    BDBG_MSG(("Closing I2C Channel %p, %u", i2cHandle, i2cHandle->index));
    if (i2cHandle->channel) {
        nexus_i2c_p_close_channel(i2cHandle);
    }
    BLST_S_REMOVE(&g_i2c_channels, i2cHandle, NEXUS_I2c, link);
    NEXUS_OBJECT_DESTROY(NEXUS_I2c, i2cHandle);
    BKNI_Free(i2cHandle);
    return;
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_I2c, NEXUS_I2c_Close);

void NEXUS_I2c_P_CloseAll(void)
{
    NEXUS_I2cHandle i2cHandle;
    while ((i2cHandle = BLST_S_FIRST(&g_i2c_channels))) {
        NEXUS_I2c_Close(i2cHandle);
    }
}

NEXUS_Error NEXUS_I2c_SetSettings( NEXUS_I2cHandle i2cHandle, const NEXUS_I2cSettings *pSettings)
{
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(i2cHandle, NEXUS_I2c);
    
    if (!i2cHandle->first) {
        return BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }

    BI2C_SetClk(i2cHandle->channel, pSettings->clockRate);

    rc = BI2C_SetBurstMode(i2cHandle->channel, pSettings->burstMode);
    if (rc) return BERR_TRACE(rc);

    i2cHandle->settings = *pSettings;
    return 0;
}

void NEXUS_I2c_GetSettings( NEXUS_I2cHandle i2cHandle, NEXUS_I2cSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(i2cHandle, NEXUS_I2c);
    *pSettings = i2cHandle->settings;
}

/*
Summary:
This function writes a programmable number of I2C registers using an 8 bit
sub address.
*/
NEXUS_Error NEXUS_I2c_Write(
    NEXUS_I2cHandle i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    uint8_t subAddr, /* 8 bit sub address */
    const uint8_t *pData, /* pointer to data to write */
    size_t length /* number of bytes to write */
    )
{
    NEXUS_Error errCode;
    BREG_I2C_Handle regHandle;

    BDBG_OBJECT_ASSERT(i2cHandle, NEXUS_I2c);

    if ( i2cHandle->owner == NEXUS_MODULE_SELF )
    {
        regHandle = &i2cHandle->asyncReg;
    }
    else
    {
        NEXUS_UnlockModule();
        regHandle = &i2cHandle->syncReg;
    }

    errCode = BREG_I2C_Write(regHandle, chipAddr, subAddr, pData, length);

    if ( i2cHandle->owner != NEXUS_MODULE_SELF )
    {
        NEXUS_LockModule();
    }

    return errCode;
}

/*
Summary:
This function writes and read a programmable number of I2C registers using an 8 bit
sub address in an atomic operation.
*/
NEXUS_Error NEXUS_I2c_WriteRead(
    NEXUS_I2cHandle i2cHandle,
    uint16_t chipAddr,
    uint8_t subAddr,
    const uint8_t *pWriteData,
    size_t writeLength,
    uint8_t *pReadData,
    size_t readLength
    )
{
    NEXUS_Error errCode;
    BREG_I2C_Handle regHandle;

    BDBG_OBJECT_ASSERT(i2cHandle, NEXUS_I2c);

    if ( i2cHandle->owner == NEXUS_MODULE_SELF )
    {
        regHandle = &i2cHandle->asyncReg;
    }
    else
    {
        NEXUS_UnlockModule();
        regHandle = &i2cHandle->syncReg;
    }

    errCode = BREG_I2C_WriteRead(regHandle, chipAddr, subAddr, pWriteData, writeLength, pReadData, readLength);

    if ( i2cHandle->owner != NEXUS_MODULE_SELF )
    {
        NEXUS_LockModule();
    }

    return errCode;
}

/*
Summary:
This function writes and read a programmable number of I2C registers using no sub address in an atomic operation.
*/
NEXUS_Error NEXUS_I2c_WriteReadNoAddr(
    NEXUS_I2cHandle i2cHandle,
    uint16_t chipAddr,
    const uint8_t *pWriteData,
    size_t writeLength,
    uint8_t *pReadData,
    size_t readLength
    )
{
    NEXUS_Error errCode;
    BREG_I2C_Handle regHandle;

    BDBG_OBJECT_ASSERT(i2cHandle, NEXUS_I2c);

    if ( i2cHandle->owner == NEXUS_MODULE_SELF )
    {
        regHandle = &i2cHandle->asyncReg;
    }
    else
    {
        NEXUS_UnlockModule();
        regHandle = &i2cHandle->syncReg;
    }

    errCode = BREG_I2C_WriteReadNoAddr(regHandle, chipAddr, pWriteData, writeLength, pReadData, readLength);

    if ( i2cHandle->owner != NEXUS_MODULE_SELF )
    {
        NEXUS_LockModule();
    }

    return errCode;
}

/*
Summary:
This function writes a programmable number of I2C registers using an 8 bit
sub address and no ack.
*/
NEXUS_Error NEXUS_I2c_WriteNoAck(
    NEXUS_I2cHandle i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    uint8_t subAddr, /* 8 bit sub address */
    const uint8_t *pData, /* pointer to data to write */
    size_t length /* number of bytes to write */
    )
{
    BERR_Code errCode;
    BREG_I2C_Handle regHandle;

    BDBG_OBJECT_ASSERT(i2cHandle, NEXUS_I2c);

    if ( i2cHandle->owner == NEXUS_MODULE_SELF )
    {
        regHandle = &i2cHandle->asyncReg;
    }
    else
    {
        NEXUS_UnlockModule();
        regHandle = &i2cHandle->syncReg;
    }

    errCode = BREG_I2C_WriteNoAck(regHandle, chipAddr, subAddr, pData, length);

    if ( i2cHandle->owner != NEXUS_MODULE_SELF )
    {
        NEXUS_LockModule();
    }

    return errCode;
}

/*
Summary:
This function writes a programmable number of I2C registers using a 16 bit
sub address.
*/
NEXUS_Error NEXUS_I2c_WriteA16(
    NEXUS_I2cHandle i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    uint16_t subAddr, /* 16 bit sub address */
    const uint8_t *pData, /* pointer to data to write */
    size_t length /* number of bytes to write */
    )
{
    BERR_Code errCode;
    BREG_I2C_Handle regHandle;

    BDBG_OBJECT_ASSERT(i2cHandle, NEXUS_I2c);

    if ( i2cHandle->owner == NEXUS_MODULE_SELF )
    {
        regHandle = &i2cHandle->asyncReg;
    }
    else
    {
        NEXUS_UnlockModule();
        regHandle = &i2cHandle->syncReg;
    }

    errCode = BREG_I2C_WriteA16(regHandle, chipAddr, subAddr, pData, length);

    if ( i2cHandle->owner != NEXUS_MODULE_SELF )
    {
        NEXUS_LockModule();
    }

    return errCode;
}

/*
Summary:
This function writes a programmable number of I2C registers using a 24 bit
sub address.
*/
NEXUS_Error NEXUS_I2c_WriteA24(
    NEXUS_I2cHandle i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    uint32_t subAddr, /* 24 bit sub address */
    const uint8_t *pData, /* pointer to data to write */
    size_t length /* number of bytes to write */
    )
{
    BERR_Code errCode;
    BREG_I2C_Handle regHandle;

    BDBG_OBJECT_ASSERT(i2cHandle, NEXUS_I2c);

    if ( i2cHandle->owner == NEXUS_MODULE_SELF )
    {
        regHandle = &i2cHandle->asyncReg;
    }
    else
    {
        NEXUS_UnlockModule();
        regHandle = &i2cHandle->syncReg;
    }

    errCode = BREG_I2C_WriteA24(regHandle, chipAddr, subAddr, pData, length);

    if ( i2cHandle->owner != NEXUS_MODULE_SELF )
    {
        NEXUS_LockModule();
    }

    return errCode;
}

/*
Summary:
This function writes a programmable number of I2C registers without a sub address
(raw write).
*/
NEXUS_Error NEXUS_I2c_WriteNoAddr(
    NEXUS_I2cHandle i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    const uint8_t *pData, /* pointer to data to write */
    size_t length /* number of bytes to write */
    )
{
    BERR_Code errCode;
    BREG_I2C_Handle regHandle;

    BDBG_OBJECT_ASSERT(i2cHandle, NEXUS_I2c);

    if ( i2cHandle->owner == NEXUS_MODULE_SELF )
    {
        regHandle = &i2cHandle->asyncReg;
    }
    else
    {
        NEXUS_UnlockModule();
        regHandle = &i2cHandle->syncReg;
    }

    errCode = BREG_I2C_WriteNoAddr(regHandle, chipAddr, pData, length);

    if ( i2cHandle->owner != NEXUS_MODULE_SELF )
    {
        NEXUS_LockModule();
    }

    return errCode;
}

/*
Summary:
This function writes a programmable number of I2C registers without a sub address
(raw write), and without waiting for an ack.
*/
NEXUS_Error NEXUS_I2c_WriteNoAddrNoAck(
    NEXUS_I2cHandle i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    const uint8_t *pData, /* pointer to data to write */
    size_t length /* number of bytes to write */
    )
{
    BERR_Code errCode;
    BREG_I2C_Handle regHandle;

    BDBG_OBJECT_ASSERT(i2cHandle, NEXUS_I2c);

    if ( i2cHandle->owner == NEXUS_MODULE_SELF )
    {
        regHandle = &i2cHandle->asyncReg;
    }
    else
    {
        NEXUS_UnlockModule();
        regHandle = &i2cHandle->syncReg;
    }

    errCode = BREG_I2C_WriteNoAddrNoAck(regHandle, chipAddr, pData, length);

    if ( i2cHandle->owner != NEXUS_MODULE_SELF )
    {
        NEXUS_LockModule();
    }

    return errCode;
}

/*
Summary:
This function writes an I2C NVRAM device using an 8 bit sub address.
*/
NEXUS_Error NEXUS_I2c_WriteNvram(
    NEXUS_I2cHandle i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    uint8_t subAddr, /* 8 bit sub address */
    const uint8_t *pData, /* pointer to data to write */
    size_t length /* number of bytes to write */
    )
{
    BERR_Code errCode;
    BREG_I2C_Handle regHandle;

    BDBG_OBJECT_ASSERT(i2cHandle, NEXUS_I2c);

    if ( i2cHandle->owner == NEXUS_MODULE_SELF )
    {
        regHandle = &i2cHandle->asyncReg;
    }
    else
    {
        NEXUS_UnlockModule();
        regHandle = &i2cHandle->syncReg;
    }

    errCode = BREG_I2C_WriteNvram(regHandle, chipAddr, subAddr, pData, length);

    if ( i2cHandle->owner != NEXUS_MODULE_SELF )
    {
        NEXUS_LockModule();
    }

    return errCode;
}

/*
Summary:
This function reads a programmable number of I2C registers using an 8 bit
sub address.
*/
NEXUS_Error NEXUS_I2c_Read(
    NEXUS_I2cHandle i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    uint8_t subAddr, /* 8 bit sub address */
    uint8_t *pData, /* pointer to memory location to store read data */
    size_t length /* number of bytes to read */
    )
{
    BERR_Code errCode;
    BREG_I2C_Handle regHandle;

    BDBG_OBJECT_ASSERT(i2cHandle, NEXUS_I2c);

    if ( i2cHandle->owner == NEXUS_MODULE_SELF )
    {
        regHandle = &i2cHandle->asyncReg;
    }
    else
    {
        NEXUS_UnlockModule();
        regHandle = &i2cHandle->syncReg;
    }

    errCode = BREG_I2C_Read(regHandle, chipAddr, subAddr, pData, length);

    if ( i2cHandle->owner != NEXUS_MODULE_SELF )
    {
        NEXUS_LockModule();
    }

    return errCode;
}

/*
Summary:
This function reads a programmable number of I2C registers using an 8 bit
sub address and no ack..
*/
NEXUS_Error NEXUS_I2c_ReadNoAck(
    NEXUS_I2cHandle i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    uint8_t subAddr, /* 8 bit sub address */
    uint8_t *pData, /* pointer to memory location to store read data */
    size_t length /* number of bytes to read */
    )
{
    BERR_Code errCode;
    BREG_I2C_Handle regHandle;

    BDBG_OBJECT_ASSERT(i2cHandle, NEXUS_I2c);

    if ( i2cHandle->owner == NEXUS_MODULE_SELF )
    {
        regHandle = &i2cHandle->asyncReg;
    }
    else
    {
        NEXUS_UnlockModule();
        regHandle = &i2cHandle->syncReg;
    }

    errCode = BREG_I2C_ReadNoAck(regHandle, chipAddr, subAddr, pData, length);

    if ( i2cHandle->owner != NEXUS_MODULE_SELF )
    {
        NEXUS_LockModule();
    }

    return errCode;
}

/*
Summary:
This function reads a programmable number of I2C registers using a 16 bit
sub address.
*/
NEXUS_Error NEXUS_I2c_ReadA16(
    NEXUS_I2cHandle i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    uint16_t subAddr, /* 16 bit sub address */
    uint8_t *pData, /* pointer to memory location to store read data */
    size_t length /* number of bytes to read */
    )
{
    BERR_Code errCode;
    BREG_I2C_Handle regHandle;

    BDBG_OBJECT_ASSERT(i2cHandle, NEXUS_I2c);

    if ( i2cHandle->owner == NEXUS_MODULE_SELF )
    {
        regHandle = &i2cHandle->asyncReg;
    }
    else
    {
        NEXUS_UnlockModule();
        regHandle = &i2cHandle->syncReg;
    }

    errCode = BREG_I2C_ReadA16(regHandle, chipAddr, subAddr, pData, length);

    if ( i2cHandle->owner != NEXUS_MODULE_SELF )
    {
        NEXUS_LockModule();
    }

    return errCode;
}

/*
Summary:
This function reads a programmable number of I2C registers using a 24 bit
sub address.
*/
NEXUS_Error NEXUS_I2c_ReadA24(
    NEXUS_I2cHandle i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    uint32_t subAddr, /* 32 bit sub address */
    uint8_t *pData, /* pointer to memory location to store read data */
    size_t length /* number of bytes to read */
    )
{
    BERR_Code errCode;
    BREG_I2C_Handle regHandle;

    BDBG_OBJECT_ASSERT(i2cHandle, NEXUS_I2c);

    if ( i2cHandle->owner == NEXUS_MODULE_SELF )
    {
        regHandle = &i2cHandle->asyncReg;
    }
    else
    {
        NEXUS_UnlockModule();
        regHandle = &i2cHandle->syncReg;
    }

    errCode = BREG_I2C_ReadA24(regHandle, chipAddr, subAddr, pData, length);

    if ( i2cHandle->owner != NEXUS_MODULE_SELF )
    {
        NEXUS_LockModule();
    }

    return errCode;
}

/*
Summary:
This function reads a programmable number of I2C registers without a sub address
(raw read).
*/
NEXUS_Error NEXUS_I2c_ReadNoAddr(
    NEXUS_I2cHandle i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    uint8_t *pData, /* pointer to memory location to store read data */
    size_t length /* number of bytes to read */
    )
{
    BERR_Code errCode;
    BREG_I2C_Handle regHandle;

    BDBG_OBJECT_ASSERT(i2cHandle, NEXUS_I2c);

    if ( i2cHandle->owner == NEXUS_MODULE_SELF )
    {
        regHandle = &i2cHandle->asyncReg;
    }
    else
    {
        NEXUS_UnlockModule();
        regHandle = &i2cHandle->syncReg;
    }

    errCode = BREG_I2C_ReadNoAddr(regHandle, chipAddr, pData, length);

    if ( i2cHandle->owner != NEXUS_MODULE_SELF )
    {
        NEXUS_LockModule();
    }

    return errCode;
}

NEXUS_Error NEXUS_I2c_ReadNoAddrNoAck(
    NEXUS_I2cHandle i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    uint8_t *pData, /* pointer to memory location to store read data */
    size_t length /* number of bytes to read */
    )
{
    BERR_Code errCode;
    BREG_I2C_Handle regHandle;

    BDBG_OBJECT_ASSERT(i2cHandle, NEXUS_I2c);

    if ( i2cHandle->owner == NEXUS_MODULE_SELF )
    {
        regHandle = &i2cHandle->asyncReg;
    }
    else
    {
        NEXUS_UnlockModule();
        regHandle = &i2cHandle->syncReg;
    }

    errCode = BREG_I2C_ReadNoAddrNoAck(regHandle, chipAddr, pData, length);

    if ( i2cHandle->owner != NEXUS_MODULE_SELF )
    {
        NEXUS_LockModule();
    }

    return errCode;
}

/*
Summary:
This function is used to perform an Enhanced Display Data Channel read protocol.
*/
NEXUS_Error NEXUS_I2c_ReadEDDC(
    NEXUS_I2cHandle i2cHandle,  /* I2C Handle */
    uint8_t chipAddr,           /* chip address */
    uint8_t segment,            /* EDDC segment pointer */
    uint8_t subAddr,            /* 8-bit sub address */
    uint8_t *pData,             /* pointer to memory location to store read data  */
    size_t length               /* number of bytes to read */
    )
{
    BERR_Code errCode;
    BREG_I2C_Handle regHandle;

    BDBG_OBJECT_ASSERT(i2cHandle, NEXUS_I2c);

    if ( i2cHandle->owner == NEXUS_MODULE_SELF )
    {
        regHandle = &i2cHandle->asyncReg;
    }
    else
    {
        NEXUS_UnlockModule();
        regHandle = &i2cHandle->syncReg;
    }

    errCode = BREG_I2C_ReadEDDC(regHandle, chipAddr, segment, subAddr, pData, length);

    if ( i2cHandle->owner != NEXUS_MODULE_SELF )
    {
        NEXUS_LockModule();
    }

    return errCode;
}

/*
Summary:
This function is used to perform an Enhanced Display Data Channel write protocol.
*/
NEXUS_Error NEXUS_I2c_WriteEDDC(
    NEXUS_I2cHandle i2cHandle,  /* I2C Handle */
    uint8_t chipAddr,           /* chip address */
    uint8_t segment,            /* EDDC segment pointer */
    uint8_t subAddr,            /* 8-bit sub address */
    const uint8_t *pData,       /* pointer to data to write */
    size_t length               /* number of bytes to write */
    )
{
    BERR_Code errCode;
    BREG_I2C_Handle regHandle;

    BDBG_OBJECT_ASSERT(i2cHandle, NEXUS_I2c);

    if ( i2cHandle->owner == NEXUS_MODULE_SELF )
    {
        regHandle = &i2cHandle->asyncReg;
    }
    else
    {
        NEXUS_UnlockModule();
        regHandle = &i2cHandle->syncReg;
    }

    errCode = BREG_I2C_WriteEDDC(regHandle, chipAddr, segment, subAddr, pData, length);

    if ( i2cHandle->owner != NEXUS_MODULE_SELF )
    {
        NEXUS_LockModule();
    }

    return errCode;
}

/*
Summary:
This function writes a programmable number of I2C registers using an 8 bit
sub address.
*/
static BERR_Code NEXUS_I2c_P_WriteSync(
    void * i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    uint32_t subAddr, /* 8 bit sub address */
    const uint8_t *pData, /* pointer to data to write */
    size_t length /* number of bytes to write */
    )
{
    BERR_Code errCode;
    NEXUS_I2c *pDevice = i2cHandle;

    BDBG_ENTER(NEXUS_I2c_P_Write);

    NEXUS_Module_Lock(pDevice->owner);

    errCode = BREG_I2C_Write(&pDevice->asyncReg, chipAddr, subAddr, pData, length);

    NEXUS_Module_Unlock(pDevice->owner);

    BDBG_LEAVE(NEXUS_I2c_P_Write);
    return errCode;
}

static BERR_Code NEXUS_I2c_P_WriteSwSync(
    void * i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    uint32_t subAddr, /* 8 bit sub address */
    const uint8_t *pData, /* pointer to data to write */
    size_t length /* number of bytes to write */
    )
{
    BERR_Code errCode;
    NEXUS_I2c *pDevice = i2cHandle;

    BDBG_ENTER(NEXUS_I2c_P_WriteSw);

    NEXUS_Module_Lock(pDevice->owner);

    errCode = BREG_I2C_WriteSw(&pDevice->asyncReg, chipAddr, subAddr, pData, length);

    NEXUS_Module_Unlock(pDevice->owner);

    BDBG_LEAVE(NEXUS_I2c_P_WriteSw);
    return errCode;
}


static BERR_Code NEXUS_I2c_P_WriteDummy(
    void * i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    uint32_t subAddr, /* 8 bit sub address */
    const uint8_t *pData, /* pointer to data to write */
    size_t length /* number of bytes to write */
    )
{
    BSTD_UNUSED(i2cHandle);
    BSTD_UNUSED(chipAddr);
    BSTD_UNUSED(subAddr);
    BSTD_UNUSED(pData);
    BSTD_UNUSED(length);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/*
Summary:
This function writes a programmable number of I2C registers using an 8 bit
sub address and no ack.
*/
static BERR_Code NEXUS_I2c_P_WriteNoAckSync(
    void * i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    uint32_t subAddr, /* 8 bit sub address */
    const uint8_t *pData, /* pointer to data to write */
    size_t length /* number of bytes to write */
    )
{
    BERR_Code errCode;
    NEXUS_I2c *pDevice = i2cHandle;

    BDBG_ENTER(NEXUS_I2c_P_Write);

    NEXUS_Module_Lock(pDevice->owner);

    errCode = BREG_I2C_WriteNoAck(&pDevice->asyncReg, chipAddr, subAddr, pData, length);

    NEXUS_Module_Unlock(pDevice->owner);

    BDBG_LEAVE(NEXUS_I2c_P_Write);
    return errCode;
}
static BERR_Code NEXUS_I2c_P_WriteNoAckDummy(
    void * i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    uint32_t subAddr, /* 8 bit sub address */
    const uint8_t *pData, /* pointer to data to write */
    size_t length /* number of bytes to write */
    )
{
    BSTD_UNUSED(i2cHandle);
    BSTD_UNUSED(chipAddr);
    BSTD_UNUSED(subAddr);
    BSTD_UNUSED(pData);
    BSTD_UNUSED(length);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/*
Summary:
This function writes a programmable number of I2C registers using a 16 bit
sub address.
*/
static BERR_Code NEXUS_I2c_P_WriteA16Sync(
    void * i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    uint32_t subAddr, /* 16 bit sub address */
    const uint8_t *pData, /* pointer to data to write */
    size_t length /* number of bytes to write */
    )
{
    BERR_Code errCode;
    NEXUS_I2c *pDevice = i2cHandle;

    BDBG_ENTER(NEXUS_I2c_P_Write);

    NEXUS_Module_Lock(pDevice->owner);

    errCode = BREG_I2C_WriteA16(&pDevice->asyncReg, chipAddr, subAddr, pData, length);

    NEXUS_Module_Unlock(pDevice->owner);

    BDBG_LEAVE(NEXUS_I2c_P_Write);
    return errCode;
}
static BERR_Code NEXUS_I2c_P_WriteA16Dummy(
    void * i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    uint32_t subAddr, /* 16 bit sub address */
    const uint8_t *pData, /* pointer to data to write */
    size_t length /* number of bytes to write */
    )
{
    BSTD_UNUSED(i2cHandle);
    BSTD_UNUSED(chipAddr);
    BSTD_UNUSED(subAddr);
    BSTD_UNUSED(pData);
    BSTD_UNUSED(length);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/*
Summary:
This function writes a programmable number of I2C registers using a 24 bit
sub address.
*/
static BERR_Code NEXUS_I2c_P_WriteA24Sync(
    void * i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    uint32_t subAddr, /* 24 bit sub address */
    const uint8_t *pData, /* pointer to data to write */
    size_t length /* number of bytes to write */
    )
{
    BERR_Code errCode;
    NEXUS_I2c *pDevice = i2cHandle;

    BDBG_ENTER(NEXUS_I2c_P_Write);

    NEXUS_Module_Lock(pDevice->owner);

    errCode = BREG_I2C_WriteA24(&pDevice->asyncReg, chipAddr, subAddr, pData, length);

    NEXUS_Module_Unlock(pDevice->owner);

    BDBG_LEAVE(NEXUS_I2c_P_Write);
    return errCode;
}
static BERR_Code NEXUS_I2c_P_WriteA24Dummy(
    void * i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    uint32_t subAddr, /* 24 bit sub address */
    const uint8_t *pData, /* pointer to data to write */
    size_t length /* number of bytes to write */
    )
{
    BSTD_UNUSED(i2cHandle);
    BSTD_UNUSED(chipAddr);
    BSTD_UNUSED(subAddr);
    BSTD_UNUSED(pData);
    BSTD_UNUSED(length);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/*
Summary:
This function writes a programmable number of I2C registers without a sub address
(raw write).
*/
static BERR_Code NEXUS_I2c_P_WriteNoAddrSync(
    void * i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    const uint8_t *pData, /* pointer to data to write */
    size_t length /* number of bytes to write */
    )
{
    BERR_Code errCode;
    NEXUS_I2c *pDevice = i2cHandle;

    BDBG_ENTER(NEXUS_I2c_P_Write);

    NEXUS_Module_Lock(pDevice->owner);

    errCode = BREG_I2C_WriteNoAddr(&pDevice->asyncReg, chipAddr, pData, length);

    NEXUS_Module_Unlock(pDevice->owner);

    BDBG_LEAVE(NEXUS_I2c_P_Write);
    return errCode;
}
static BERR_Code NEXUS_I2c_P_WriteNoAddrDummy(
    void * i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    const uint8_t *pData, /* pointer to data to write */
    size_t length /* number of bytes to write */
    )
{
    BSTD_UNUSED(i2cHandle);
    BSTD_UNUSED(chipAddr);
    BSTD_UNUSED(pData);
    BSTD_UNUSED(length);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/*
Summary:
This function writes a programmable number of I2C registers without a sub address
(raw write), and without waiting for an ack.
*/
static BERR_Code NEXUS_I2c_P_WriteNoAddrNoAckSync(
    void * i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    const uint8_t *pData, /* pointer to data to write */
    size_t length /* number of bytes to write */
    )
{
    BERR_Code errCode;
    NEXUS_I2c *pDevice = i2cHandle;

    BDBG_ENTER(NEXUS_I2c_P_Write);

    NEXUS_Module_Lock(pDevice->owner);

    errCode = BREG_I2C_WriteNoAddrNoAck(&pDevice->asyncReg, chipAddr, pData, length);

    NEXUS_Module_Unlock(pDevice->owner);

    BDBG_LEAVE(NEXUS_I2c_P_Write);
    return errCode;
}
static BERR_Code NEXUS_I2c_P_WriteNoAddrNoAckDummy(
    void * i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    const uint8_t *pData, /* pointer to data to write */
    size_t length /* number of bytes to write */
    )
{
    BSTD_UNUSED(i2cHandle);
    BSTD_UNUSED(chipAddr);
    BSTD_UNUSED(pData);
    BSTD_UNUSED(length);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/*
Summary:
This function writes an I2C NVRAM device using an 8 bit sub address.
*/
static BERR_Code NEXUS_I2c_P_WriteNvramSync(
    void * i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    uint32_t subAddr, /* 8 bit sub address */
    const uint8_t *pData, /* pointer to data to write */
    size_t length /* number of bytes to write */
    )
{
    BERR_Code errCode;
    NEXUS_I2c *pDevice = i2cHandle;

    BDBG_ENTER(NEXUS_I2c_P_Write);

    NEXUS_Module_Lock(pDevice->owner);

    errCode = BREG_I2C_WriteNvram(&pDevice->asyncReg, chipAddr, subAddr, pData, length);

    NEXUS_Module_Unlock(pDevice->owner);

    BDBG_LEAVE(NEXUS_I2c_P_Write);
    return errCode;
}
static BERR_Code NEXUS_I2c_P_WriteNvramDummy(
    void * i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    uint32_t subAddr, /* 8 bit sub address */
    const uint8_t *pData, /* pointer to data to write */
    size_t length /* number of bytes to write */
    )
{
    BSTD_UNUSED(i2cHandle);
    BSTD_UNUSED(chipAddr);
    BSTD_UNUSED(subAddr);
    BSTD_UNUSED(pData);
    BSTD_UNUSED(length);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/*
Summary:
This function reads a programmable number of I2C registers using an 8 bit
sub address.
*/
static BERR_Code NEXUS_I2c_P_ReadSync(
    void * i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    uint32_t subAddr, /* 8 bit sub address */
    uint8_t *pData, /* pointer to memory location to store read data */
    size_t length /* number of bytes to read */
    )
{
    BERR_Code errCode;
    NEXUS_I2c *pDevice = i2cHandle;

    BDBG_ENTER(NEXUS_I2c_P_Write);

    NEXUS_Module_Lock(pDevice->owner);

    errCode = BREG_I2C_Read(&pDevice->asyncReg, chipAddr, subAddr, pData, length);

    NEXUS_Module_Unlock(pDevice->owner);

    BDBG_LEAVE(NEXUS_I2c_P_Write);
    return errCode;
}

static BERR_Code NEXUS_I2c_P_ReadSwSync(
    void * i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    uint32_t subAddr, /* 8 bit sub address */
    uint8_t *pData, /* pointer to memory location to store read data */
    size_t length /* number of bytes to read */
    )
{
    BERR_Code errCode;
    NEXUS_I2c *pDevice = i2cHandle;

    BDBG_ENTER(NEXUS_I2c_P_ReadSw);

    NEXUS_Module_Lock(pDevice->owner);

    errCode = BREG_I2C_ReadSw(&pDevice->asyncReg, chipAddr, subAddr, pData, length);

    NEXUS_Module_Unlock(pDevice->owner);

    BDBG_LEAVE(NEXUS_I2c_P_ReadSw);
    return errCode;
}


static BERR_Code NEXUS_I2c_P_ReadDummy(
    void * i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    uint32_t subAddr, /* 8 bit sub address */
    uint8_t *pData, /* pointer to memory location to store read data */
    size_t length /* number of bytes to read */
    )
{
    BSTD_UNUSED(i2cHandle);
    BSTD_UNUSED(chipAddr);
    BSTD_UNUSED(subAddr);
    BSTD_UNUSED(pData);
    BSTD_UNUSED(length);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/*
Summary:
This function reads a programmable number of I2C registers using an 8 bit
sub address and no ack..
*/
static BERR_Code NEXUS_I2c_P_ReadNoAckSync(
    void * i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    uint32_t subAddr, /* 8 bit sub address */
    uint8_t *pData, /* pointer to memory location to store read data */
    size_t length /* number of bytes to read */
    )
{
    BERR_Code errCode;
    NEXUS_I2c *pDevice = i2cHandle;

    BDBG_ENTER(NEXUS_I2c_P_Write);

    NEXUS_Module_Lock(pDevice->owner);

    errCode = BREG_I2C_ReadNoAck(&pDevice->asyncReg, chipAddr, subAddr, pData, length);

    NEXUS_Module_Unlock(pDevice->owner);

    BDBG_LEAVE(NEXUS_I2c_P_Write);
    return errCode;
}
static BERR_Code NEXUS_I2c_P_ReadNoAckDummy(
    void * i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    uint32_t subAddr, /* 8 bit sub address */
    uint8_t *pData, /* pointer to memory location to store read data */
    size_t length /* number of bytes to read */
    )
{
    BSTD_UNUSED(i2cHandle);
    BSTD_UNUSED(chipAddr);
    BSTD_UNUSED(subAddr);
    BSTD_UNUSED(pData);
    BSTD_UNUSED(length);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/*
Summary:
This function reads a programmable number of I2C registers using a 16 bit
sub address.
*/
static BERR_Code NEXUS_I2c_P_ReadA16Sync(
    void * i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    uint32_t subAddr, /* 16 bit sub address */
    uint8_t *pData, /* pointer to memory location to store read data */
    size_t length /* number of bytes to read */
    )
{
    BERR_Code errCode;
    NEXUS_I2c *pDevice = i2cHandle;

    BDBG_ENTER(NEXUS_I2c_P_Write);

    NEXUS_Module_Lock(pDevice->owner);

    errCode = BREG_I2C_ReadA16(&pDevice->asyncReg, chipAddr, subAddr, pData, length);

    NEXUS_Module_Unlock(pDevice->owner);

    BDBG_LEAVE(NEXUS_I2c_P_Write);
    return errCode;
}
static BERR_Code NEXUS_I2c_P_ReadA16Dummy(
    void * i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    uint32_t subAddr, /* 16 bit sub address */
    uint8_t *pData, /* pointer to memory location to store read data */
    size_t length /* number of bytes to read */
    )
{
    BSTD_UNUSED(i2cHandle);
    BSTD_UNUSED(chipAddr);
    BSTD_UNUSED(subAddr);
    BSTD_UNUSED(pData);
    BSTD_UNUSED(length);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/*
Summary:
This function reads a programmable number of I2C registers using a 24 bit
sub address.
*/
static BERR_Code NEXUS_I2c_P_ReadA24Sync(
    void * i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    uint32_t subAddr, /* 32 bit sub address */
    uint8_t *pData, /* pointer to memory location to store read data */
    size_t length /* number of bytes to read */
    )
{
    BERR_Code errCode;
    NEXUS_I2c *pDevice = i2cHandle;

    BDBG_ENTER(NEXUS_I2c_P_Write);

    NEXUS_Module_Lock(pDevice->owner);

    errCode = BREG_I2C_ReadA24(&pDevice->asyncReg, chipAddr, subAddr, pData, length);

    NEXUS_Module_Unlock(pDevice->owner);

    BDBG_LEAVE(NEXUS_I2c_P_Write);
    return errCode;
}
static BERR_Code NEXUS_I2c_P_ReadA24Dummy(
    void * i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    uint32_t subAddr, /* 24 bit sub address */
    uint8_t *pData, /* pointer to memory location to store read data */
    size_t length /* number of bytes to read */
    )
{
    BSTD_UNUSED(i2cHandle);
    BSTD_UNUSED(chipAddr);
    BSTD_UNUSED(subAddr);
    BSTD_UNUSED(pData);
    BSTD_UNUSED(length);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/*
Summary:
This function reads a programmable number of I2C registers without a sub address
(raw read).
*/
static BERR_Code NEXUS_I2c_P_ReadNoAddrSync(
    void * i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    uint8_t *pData, /* pointer to memory location to store read data */
    size_t length /* number of bytes to read */
    )
{
    BERR_Code errCode;
    NEXUS_I2c *pDevice = i2cHandle;

    BDBG_ENTER(NEXUS_I2c_P_Write);

    NEXUS_Module_Lock(pDevice->owner);

    errCode = BREG_I2C_ReadNoAddr(&pDevice->asyncReg, chipAddr, pData, length);

    NEXUS_Module_Unlock(pDevice->owner);

    BDBG_LEAVE(NEXUS_I2c_P_Write);
    return errCode;
}

static BERR_Code NEXUS_I2c_P_ReadSwNoAddrSync(
    void * i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    uint8_t *pData, /* pointer to memory location to store read data */
    size_t length /* number of bytes to read */
    )
{
    BERR_Code errCode;
    NEXUS_I2c *pDevice = i2cHandle;

    BDBG_ENTER(NEXUS_I2c_P_ReadSw);

    NEXUS_Module_Lock(pDevice->owner);

    errCode = BREG_I2C_ReadSwNoAddr(&pDevice->asyncReg, chipAddr, pData, length);

    NEXUS_Module_Unlock(pDevice->owner);

    BDBG_LEAVE(NEXUS_I2c_P_ReadSw);
    return errCode;
}

static BERR_Code NEXUS_I2c_P_ReadNoAddrDummy(
    void * i2cHandle, /* I2C Handle */
    uint16_t chipAddr, /* 7 or 10 bit chip address (_unshifted_) */
    uint8_t *pData, /* pointer to memory location to store read data */
    size_t length /* number of bytes to read */
    )
{
    BSTD_UNUSED(i2cHandle);
    BSTD_UNUSED(chipAddr);
    BSTD_UNUSED(pData);
    BSTD_UNUSED(length);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/*
Summary:
This function is used to perform an Enhanced Display Data Channel read protocol.
*/
static BERR_Code NEXUS_I2c_P_ReadEDDCSync(
    void * i2cHandle,   /* I2C Handle */
    uint8_t chipAddr,           /* chip address */
    uint8_t segment,            /* EDDC segment pointer */
    uint32_t subAddr,            /* 8-bit sub address */
    uint8_t *pData,             /* pointer to memory location to store read data  */
    size_t length               /* number of bytes to read */
    )
{
    BERR_Code errCode;
    NEXUS_I2c *pDevice = i2cHandle;

    BDBG_ENTER(NEXUS_I2c_P_ReadEDDCSync);

    NEXUS_Module_Lock(pDevice->owner);

    errCode = BREG_I2C_ReadEDDC(&pDevice->asyncReg, chipAddr, segment, subAddr, pData, length);

    NEXUS_Module_Unlock(pDevice->owner);

    BDBG_LEAVE(NEXUS_I2c_P_ReadEDDCSync);
    return errCode;
}

static BERR_Code NEXUS_I2c_P_ReadSwEDDCSync(
    void * i2cHandle,   /* I2C Handle */
    uint8_t chipAddr,           /* chip address */
    uint8_t segment,            /* EDDC segment pointer */
    uint32_t subAddr,            /* 8-bit sub address */
    uint8_t *pData,             /* pointer to memory location to store read data  */
    size_t length               /* number of bytes to read */
    )
{
    BERR_Code errCode;
    NEXUS_I2c *pDevice = i2cHandle;

    BDBG_ENTER(NEXUS_I2c_P_ReadSwEDDCSync);

    NEXUS_Module_Lock(pDevice->owner);

    errCode = BREG_I2C_ReadSwEDDC(&pDevice->asyncReg, chipAddr, segment, subAddr, pData, length);

    NEXUS_Module_Unlock(pDevice->owner);

    BDBG_LEAVE(NEXUS_I2c_P_ReadSwEDDCSync);
    return errCode;
}

static BERR_Code NEXUS_I2c_P_ReadEDDCDummy(
    void * i2cHandle,   /* I2C Handle */
    uint8_t chipAddr,           /* chip address */
    uint8_t segment,            /* EDDC segment pointer */
    uint32_t subAddr,            /* 8-bit sub address */
    uint8_t *pData,             /* pointer to memory location to store read data  */
    size_t length               /* number of bytes to read */
    )
{
    BSTD_UNUSED(i2cHandle);
    BSTD_UNUSED(chipAddr);
    BSTD_UNUSED(segment);
    BSTD_UNUSED(subAddr);
    BSTD_UNUSED(pData);
    BSTD_UNUSED(length);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/*
Summary:
This function is used to perform an Enhanced Display Data Channel write protocol.
*/
static BERR_Code NEXUS_I2c_P_WriteEDDCSync(
    void * i2cHandle,           /* I2C Handle */
    uint8_t chipAddr,           /* chip address */
    uint8_t segment,            /* EDDC segment pointer */
    uint32_t subAddr,            /* 8-bit sub address */
    const uint8_t *pData,       /* pointer to data to write */
    size_t length               /* number of bytes to write */
    )
{
    BERR_Code errCode;
    NEXUS_I2c *pDevice = i2cHandle;

    BDBG_ENTER(NEXUS_I2c_P_Write);

    NEXUS_Module_Lock(pDevice->owner);

    errCode = BREG_I2C_WriteEDDC(&pDevice->asyncReg, chipAddr, segment, subAddr, pData, length);

    NEXUS_Module_Unlock(pDevice->owner);

    BDBG_LEAVE(NEXUS_I2c_P_Write);
    return errCode;
}
static BERR_Code NEXUS_I2c_P_WriteEDDCDummy(
    void * i2cHandle,           /* I2C Handle */
    uint8_t chipAddr,           /* chip address */
    uint8_t segment,            /* EDDC segment pointer */
    uint32_t subAddr,            /* 8-bit sub address */
    const uint8_t *pData,       /* pointer to data to write */
    size_t length               /* number of bytes to write */
    )
{
    BSTD_UNUSED(i2cHandle);
    BSTD_UNUSED(chipAddr);
    BSTD_UNUSED(segment);
    BSTD_UNUSED(subAddr);
    BSTD_UNUSED(pData);
    BSTD_UNUSED(length);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/* Magnum modules call BREG_I2C, Nexus modules call NEXUS_I2C.  Both cases need to synchronize */
static void NEXUS_I2c_P_FillSyncRegHandle(
    NEXUS_I2c *pI2cDevice,
    NEXUS_ModuleHandle moduleHandle
    )
{
    BDBG_ENTER(NEXUS_I2c_P_FillLocalRegHandle);
    pI2cDevice->owner = moduleHandle;
    BKNI_Memset(&pI2cDevice->syncReg, 0, sizeof(pI2cDevice->syncReg));
    pI2cDevice->syncReg.context = pI2cDevice;

    /*  This would be _much_ simpler if BREG handled NULL...  */
    if ( pI2cDevice->asyncReg.BREG_I2C_Write_Func )
        pI2cDevice->syncReg.BREG_I2C_Write_Func = NEXUS_I2c_P_WriteSync;
    else
        pI2cDevice->syncReg.BREG_I2C_Write_Func = NEXUS_I2c_P_WriteDummy;

    if ( pI2cDevice->asyncReg.BREG_I2C_WriteSw_Func )
        pI2cDevice->syncReg.BREG_I2C_WriteSw_Func = NEXUS_I2c_P_WriteSwSync;
    else
        pI2cDevice->syncReg.BREG_I2C_WriteSw_Func = NEXUS_I2c_P_WriteDummy;

    if ( pI2cDevice->asyncReg.BREG_I2C_WriteNoAck_Func )
        pI2cDevice->syncReg.BREG_I2C_WriteNoAck_Func = NEXUS_I2c_P_WriteNoAckSync;
    else
        pI2cDevice->syncReg.BREG_I2C_WriteNoAck_Func = NEXUS_I2c_P_WriteNoAckDummy;

    if ( pI2cDevice->asyncReg.BREG_I2C_WriteA16_Func )
        pI2cDevice->syncReg.BREG_I2C_WriteA16_Func = NEXUS_I2c_P_WriteA16Sync;
    else
        pI2cDevice->syncReg.BREG_I2C_WriteA16_Func = NEXUS_I2c_P_WriteA16Dummy;

    if ( pI2cDevice->asyncReg.BREG_I2C_WriteA24_Func )
        pI2cDevice->syncReg.BREG_I2C_WriteA24_Func = NEXUS_I2c_P_WriteA24Sync;
    else
        pI2cDevice->syncReg.BREG_I2C_WriteA24_Func = NEXUS_I2c_P_WriteA24Dummy;

    if ( pI2cDevice->asyncReg.BREG_I2C_WriteNoAddr_Func )
        pI2cDevice->syncReg.BREG_I2C_WriteNoAddr_Func = NEXUS_I2c_P_WriteNoAddrSync;
    else
        pI2cDevice->syncReg.BREG_I2C_WriteNoAddr_Func = NEXUS_I2c_P_WriteNoAddrDummy;

    if ( pI2cDevice->asyncReg.BREG_I2C_WriteNoAddrNoAck_Func )
        pI2cDevice->syncReg.BREG_I2C_WriteNoAddrNoAck_Func = NEXUS_I2c_P_WriteNoAddrNoAckSync;
    else
        pI2cDevice->syncReg.BREG_I2C_WriteNoAddrNoAck_Func = NEXUS_I2c_P_WriteNoAddrNoAckDummy;

    if ( pI2cDevice->asyncReg.BREG_I2C_WriteNvram_Func )
        pI2cDevice->syncReg.BREG_I2C_WriteNvram_Func = NEXUS_I2c_P_WriteNvramSync;
    else
        pI2cDevice->syncReg.BREG_I2C_WriteNvram_Func = NEXUS_I2c_P_WriteNvramDummy;

    if ( pI2cDevice->asyncReg.BREG_I2C_Read_Func )
        pI2cDevice->syncReg.BREG_I2C_Read_Func = NEXUS_I2c_P_ReadSync;
    else
        pI2cDevice->syncReg.BREG_I2C_Read_Func = NEXUS_I2c_P_ReadDummy;

    if ( pI2cDevice->asyncReg.BREG_I2C_ReadSw_Func )
        pI2cDevice->syncReg.BREG_I2C_ReadSw_Func = NEXUS_I2c_P_ReadSwSync;
    else
        pI2cDevice->syncReg.BREG_I2C_ReadSw_Func = NEXUS_I2c_P_ReadDummy;

    if ( pI2cDevice->asyncReg.BREG_I2C_ReadNoAck_Func )
        pI2cDevice->syncReg.BREG_I2C_ReadNoAck_Func = NEXUS_I2c_P_ReadNoAckSync;
    else
        pI2cDevice->syncReg.BREG_I2C_ReadNoAck_Func = NEXUS_I2c_P_ReadNoAckDummy;

    if ( pI2cDevice->asyncReg.BREG_I2C_ReadA16_Func )
        pI2cDevice->syncReg.BREG_I2C_ReadA16_Func = NEXUS_I2c_P_ReadA16Sync;
    else
        pI2cDevice->syncReg.BREG_I2C_ReadA16_Func = NEXUS_I2c_P_ReadA16Dummy;

    if ( pI2cDevice->asyncReg.BREG_I2C_ReadA24_Func )
        pI2cDevice->syncReg.BREG_I2C_ReadA24_Func = NEXUS_I2c_P_ReadA24Sync;
    else
        pI2cDevice->syncReg.BREG_I2C_ReadA24_Func = NEXUS_I2c_P_ReadA24Dummy;

    if ( pI2cDevice->asyncReg.BREG_I2C_ReadNoAddr_Func )
        pI2cDevice->syncReg.BREG_I2C_ReadNoAddr_Func = NEXUS_I2c_P_ReadNoAddrSync;
    else
        pI2cDevice->syncReg.BREG_I2C_ReadNoAddr_Func = NEXUS_I2c_P_ReadNoAddrDummy;

    if ( pI2cDevice->asyncReg.BREG_I2C_ReadSwNoAddr_Func )
        pI2cDevice->syncReg.BREG_I2C_ReadSwNoAddr_Func = NEXUS_I2c_P_ReadSwNoAddrSync;
    else
        pI2cDevice->syncReg.BREG_I2C_ReadSwNoAddr_Func = NEXUS_I2c_P_ReadNoAddrDummy;

    if ( pI2cDevice->asyncReg.BREG_I2C_ReadEDDC_Func )
        pI2cDevice->syncReg.BREG_I2C_ReadEDDC_Func = NEXUS_I2c_P_ReadEDDCSync;
    else
        pI2cDevice->syncReg.BREG_I2C_ReadEDDC_Func = NEXUS_I2c_P_ReadEDDCDummy;

    if ( pI2cDevice->asyncReg.BREG_I2C_ReadSwEDDC_Func )
        pI2cDevice->syncReg.BREG_I2C_ReadSwEDDC_Func = NEXUS_I2c_P_ReadSwEDDCSync;
    else
        pI2cDevice->syncReg.BREG_I2C_ReadSwEDDC_Func = NEXUS_I2c_P_ReadEDDCDummy;

    if ( pI2cDevice->asyncReg.BREG_I2C_WriteEDDC_Func )
        pI2cDevice->syncReg.BREG_I2C_WriteEDDC_Func = NEXUS_I2c_P_WriteEDDCSync;
    else
        pI2cDevice->syncReg.BREG_I2C_WriteEDDC_Func = NEXUS_I2c_P_WriteEDDCDummy;

    BDBG_LEAVE(NEXUS_I2c_P_FillLocalRegHandle);
}

/***************************************************************************
Summary:
    This function returns a BREG_I2C_Handle from a NEXUS_I2cHandle.

Description:
    This function returns a BREG_I2C_Handle that will handle internal
    synchronization properly based upon two factors.  The first is the module
    that created the I2C Handle (not all are local I2C Masters), and the
    second is the module that intends to use it.  Supported models are as
    follows:  1) Local I2C Master, any nexus module may use it.  2) Module-
    specific I2C Master (e.g. Frontend), only that module may use it.  If
    platform requires a BREG_I2C_Handle for some reason, it may pass NULL
    for the module handle here.  That is only supported for local i2c masters.

See Also:
    NEXUS_I2c_Close(), NEXUS_I2c_GetDefaultSettings()

****************************************************************************/

#if  defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_I2C_ASYNC)
BREG_I2C_Handle NEXUS_I2c_GetRegHandle(
    NEXUS_I2cHandle i2cHandle,
    NEXUS_ModuleHandle moduleHandle     /* The module requiring a BREG_I2C Handle, determines synchronization. model */
    )
{
	BDBG_OBJECT_ASSERT(i2cHandle, NEXUS_I2c);

	/* WARNING: This is a private function, but is called in nexus (mostly within frontend module) without the i2c module lock
			acquired. do not modify state or depend on non-atomic state here. In no cases is the nexus_i2c module's lock acquired
			when using the BREG_I2C_Handle returned by NEXUS_I2c_GetRegHandle */

	if ( moduleHandle == i2cHandle->owner || i2cHandle->owner == NEXUS_MODULE_SELF ){
		/* no additional lock is required, either because BI2C will lock, or because the owner is using its own i2c. */
		return &i2cHandle->asyncReg;
	}
	else
	{
		/* must lock i2cHandle->owner's mutex before making the i2c call */
		return &i2cHandle->syncReg;

	}
}

#else
BREG_I2C_Handle NEXUS_I2c_GetRegHandle(
    NEXUS_I2cHandle i2cHandle,
    NEXUS_ModuleHandle moduleHandle     /* The module requiring a BREG_I2C Handle, determines synchronization. model */
    )
{
    NEXUS_Error errCode;
    BDBG_OBJECT_ASSERT(i2cHandle, NEXUS_I2c);

    /* WARNING: This is a private function, but is called throughout nexus without the i2c module lock
    acquired. do not modify state or depend on non-atomic state here. currently, this function returns
    a magnum pointer which must be used outside of nexus module lock anyway. */

    if ( moduleHandle != i2cHandle->owner )
    {
        if ( NULL == moduleHandle || i2cHandle->owner == NEXUS_MODULE_SELF )
        {
            /* If the app requested it, always be sync.  If another module requested it, it must be a local i2c device */
            return &i2cHandle->syncReg;
        }
        else
        {
            BDBG_ERR(("This combination of I2C Handles is not supported"));
            errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
            return NULL;
        }
    }
    else
    {
        /* If a module requests it's own I2C Reg Handle, it must be asynchronous to avoid internal deadlock.*/
        return &i2cHandle->asyncReg;
    }
}
#endif

/***************************************************************************
Summary:
    This function initializes an I2C Callback structure to defaults
See Also:
    NEXUS_I2c_CreateHandle
****************************************************************************/
void NEXUS_I2c_InitCallbacks(
    NEXUS_I2cCallbacks *pCallbacks  /* [out] */
    )
{
    BKNI_Memset(pCallbacks, 0, sizeof(*pCallbacks));
}

/***************************************************************************
Summary:
    Create a NEXUS_I2cHandle for a module from a set of callbacks.

Description:
    This function allows another nexus module to create a new I2C handle
    that can be used with the standard NEXUS_I2c_Xxx routines by the application.
    This is required for several frontend devices.

See Also:
    NEXUS_I2c_DestroyHandle(), NEXUS_I2c_InitCallbacks()
****************************************************************************/
NEXUS_I2cHandle NEXUS_I2c_CreateHandle(
    NEXUS_ModuleHandle moduleHandle,            /* The module requiring a BREG_I2C Handle, determines synchronization model */
    void *pContext,                             /* This is the device context that will be passed to each callback */
    const NEXUS_I2cCallbacks *pI2cCallbacks     /* A list of callbacks supported by this device */
    )
{
    NEXUS_I2c *pI2cDevice;
    NEXUS_Error errCode;

    BDBG_ASSERT(NULL != moduleHandle);
    BDBG_ASSERT(NULL != pContext);
    BDBG_ASSERT(NULL != pI2cCallbacks);

    pI2cDevice = BKNI_Malloc(sizeof(NEXUS_I2c));
    if ( NULL == pI2cDevice )
    {
        errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }

    /* Set to defaults */
    BKNI_Memset(pI2cDevice, 0, sizeof(NEXUS_I2c));
    BDBG_OBJECT_SET(pI2cDevice, NEXUS_I2c);

    /* Fill async values with provided callbacks -- This would be _much_ simpler if BREG handled NULL... */
    pI2cDevice->asyncReg.context = pContext;
    if ( pI2cCallbacks->write )
        pI2cDevice->asyncReg.BREG_I2C_Write_Func = pI2cCallbacks->write;
    else
        pI2cDevice->asyncReg.BREG_I2C_Write_Func = NEXUS_I2c_P_WriteDummy;

    if ( pI2cCallbacks->writeSw )
        pI2cDevice->asyncReg.BREG_I2C_WriteSw_Func = pI2cCallbacks->writeSw;
    else
        pI2cDevice->asyncReg.BREG_I2C_WriteSw_Func = NEXUS_I2c_P_WriteDummy;

    if ( pI2cCallbacks->writeNoAck )
        pI2cDevice->asyncReg.BREG_I2C_WriteNoAck_Func = pI2cCallbacks->writeNoAck;
    else
        pI2cDevice->asyncReg.BREG_I2C_WriteNoAck_Func = NEXUS_I2c_P_WriteNoAckDummy;

    if ( pI2cCallbacks->writeA16 )
        pI2cDevice->asyncReg.BREG_I2C_WriteA16_Func = pI2cCallbacks->writeA16;
    else
        pI2cDevice->asyncReg.BREG_I2C_WriteA16_Func = NEXUS_I2c_P_WriteA16Dummy;

    if ( pI2cCallbacks->writeA24 )
        pI2cDevice->asyncReg.BREG_I2C_WriteA24_Func = pI2cCallbacks->writeA24;
    else
        pI2cDevice->asyncReg.BREG_I2C_WriteA24_Func = NEXUS_I2c_P_WriteA24Dummy;

    if ( pI2cCallbacks->writeNoAddr )
        pI2cDevice->asyncReg.BREG_I2C_WriteNoAddr_Func = pI2cCallbacks->writeNoAddr;
    else
        pI2cDevice->asyncReg.BREG_I2C_WriteNoAddr_Func = NEXUS_I2c_P_WriteNoAddrDummy;

    if ( pI2cCallbacks->writeNoAck )
        pI2cDevice->asyncReg.BREG_I2C_WriteNoAck_Func = pI2cCallbacks->writeNoAck;
    else
        pI2cDevice->asyncReg.BREG_I2C_WriteNoAck_Func = NEXUS_I2c_P_WriteNoAckDummy;

    if ( pI2cCallbacks->writeNvram )
        pI2cDevice->asyncReg.BREG_I2C_WriteNvram_Func = pI2cCallbacks->writeNvram;
    else
        pI2cDevice->asyncReg.BREG_I2C_WriteNvram_Func = NEXUS_I2c_P_WriteNvramDummy;

    if ( pI2cCallbacks->read )
        pI2cDevice->asyncReg.BREG_I2C_Read_Func = pI2cCallbacks->read;
    else
        pI2cDevice->asyncReg.BREG_I2C_Read_Func = NEXUS_I2c_P_ReadDummy;

    if ( pI2cCallbacks->readSw )
        pI2cDevice->asyncReg.BREG_I2C_ReadSw_Func = pI2cCallbacks->readSw;
    else
        pI2cDevice->asyncReg.BREG_I2C_ReadSw_Func = NEXUS_I2c_P_ReadDummy;

    if ( pI2cCallbacks->readNoAck )
        pI2cDevice->asyncReg.BREG_I2C_ReadNoAck_Func = pI2cCallbacks->readNoAck;
    else
        pI2cDevice->asyncReg.BREG_I2C_ReadNoAck_Func = NEXUS_I2c_P_ReadNoAckDummy;
    if ( pI2cCallbacks->readA16 )
        pI2cDevice->asyncReg.BREG_I2C_ReadA16_Func = pI2cCallbacks->readA16;
    else
        pI2cDevice->asyncReg.BREG_I2C_ReadA16_Func = NEXUS_I2c_P_ReadA16Dummy;

    if ( pI2cCallbacks->readA24 )
        pI2cDevice->asyncReg.BREG_I2C_ReadA24_Func = pI2cCallbacks->readA24;
    else
        pI2cDevice->asyncReg.BREG_I2C_ReadA24_Func = NEXUS_I2c_P_ReadA24Dummy;

    if ( pI2cCallbacks->readNoAddr )
        pI2cDevice->asyncReg.BREG_I2C_ReadNoAddr_Func = pI2cCallbacks->readNoAddr;
    else
        pI2cDevice->asyncReg.BREG_I2C_ReadNoAddr_Func = NEXUS_I2c_P_ReadNoAddrDummy;

    if ( pI2cCallbacks->readSwNoAddr )
        pI2cDevice->asyncReg.BREG_I2C_ReadSwNoAddr_Func = pI2cCallbacks->readSwNoAddr;
    else
        pI2cDevice->asyncReg.BREG_I2C_ReadSwNoAddr_Func = NEXUS_I2c_P_ReadNoAddrDummy;

    if ( pI2cCallbacks->readEDDC )
        pI2cDevice->asyncReg.BREG_I2C_ReadEDDC_Func = pI2cCallbacks->readEDDC;
    else
        pI2cDevice->asyncReg.BREG_I2C_ReadEDDC_Func = NEXUS_I2c_P_ReadEDDCDummy;

    if ( pI2cCallbacks->readSwEDDC )
        pI2cDevice->asyncReg.BREG_I2C_ReadSwEDDC_Func = pI2cCallbacks->readSwEDDC;
    else
        pI2cDevice->asyncReg.BREG_I2C_ReadSwEDDC_Func = NEXUS_I2c_P_ReadEDDCDummy;

    if ( pI2cCallbacks->writeEDDC )
        pI2cDevice->asyncReg.BREG_I2C_WriteEDDC_Func = pI2cCallbacks->writeEDDC;
    else
        pI2cDevice->asyncReg.BREG_I2C_WriteEDDC_Func = NEXUS_I2c_P_WriteEDDCDummy;

    NEXUS_I2c_P_FillSyncRegHandle(pI2cDevice, moduleHandle);

    /* Success */
    return pI2cDevice;
}

/***************************************************************************
Summary:
    Destroy a handle created with NEXUS_I2c_CreateHandle.

See Also:
    NEXUS_I2c_CreateHandle()
****************************************************************************/
void NEXUS_I2c_DestroyHandle(
    NEXUS_I2cHandle i2cHandle
    )
{
    BDBG_OBJECT_ASSERT(i2cHandle, NEXUS_I2c);
    BDBG_ASSERT(NULL == i2cHandle->channel);   /* Can't destroy a local channel, call close for that. */
    BDBG_OBJECT_DESTROY(i2cHandle, NEXUS_I2c);
    BKNI_Free(i2cHandle);
}

NEXUS_Error NEXUS_I2c_SwReset( NEXUS_I2cHandle i2cHandle )
{
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(i2cHandle, NEXUS_I2c);
    if (!i2cHandle->first) {
        return BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }
    rc = BI2C_SwReset(i2cHandle->channel);
    if (rc) return BERR_TRACE(rc);

    return 0;
}

NEXUS_Error NEXUS_I2cModule_Standby_priv(bool enabled, const NEXUS_StandbySettings *pSettings)
{
#if NEXUS_POWER_MANAGEMENT
    BERR_Code rc;
    unsigned i;

    if (enabled) {
        if (pSettings->mode==NEXUS_StandbyMode_eDeepSleep) { /* S3 */

            /* some I2C buses are part of the AON block and some are not. for the AON ones, 
               the BI2C_Channel should not be closed/re-opened. however, nothing is currently 
               using I2C during S3 standby, so we can treat them all the same for now */
            for (i=0; i<NEXUS_NUM_I2C_CHANNELS; i++) {
                struct magnum_i2c_channel *magnum_i2c = &g_magnum_i2c_channel[i];
                if (!magnum_i2c->channel) { continue; }
                BI2C_CloseChannel(magnum_i2c->channel);
                magnum_i2c->channel = NULL;
                magnum_i2c->s3standby = true; /* we can't rely on magnum_i2c->channel being NULL and must keep track of which channels we close, because a platform may not open all i2c channels to begin with */
            }
        }
    }
    else {
        for (i=0; i<NEXUS_NUM_I2C_CHANNELS; i++) {
            BI2C_ChannelSettings channelSettings;
            struct magnum_i2c_channel *magnum_i2c = &g_magnum_i2c_channel[i];
            if (magnum_i2c->channel || !magnum_i2c->s3standby) { continue; }
            
            BI2C_GetChannelDefaultSettings(g_NEXUS_i2cHandle, i, &channelSettings);
            channelSettings.clkRate = magnum_i2c->settings.clockRate;
            channelSettings.intMode = magnum_i2c->settings.interruptMode;
            channelSettings.timeoutMs = magnum_i2c->settings.timeout;
            channelSettings.burstMode = magnum_i2c->settings.burstMode;
            channelSettings.softI2c = magnum_i2c->settings.softI2c;
            channelSettings.fourByteXferMode = magnum_i2c->settings.fourByteXferMode;
            
            /* BI2C handle can be closed/re-opened, but the BREG_I2C handle must NOT be destroyed/re-created, 
               because other magnum modules (e.g. HDM) will keep using that handle */
            rc = BI2C_OpenChannel(g_NEXUS_i2cHandle, &magnum_i2c->channel, i, &channelSettings);
            if (rc) { return BERR_TRACE(rc); }

            /* asyncReg.context is normally set to the BI2C_ChannelHandle via BI2C_CreateI2cRegHandle and must be updated 
               syncReg.context is always set to NEXUS_magnum_i2c, so should not be touched */           
            magnum_i2c->asyncReg.context = magnum_i2c->channel;
            
            /* copy to all nexus handles */
            {
                NEXUS_I2cHandle i2cHandle;
                for (i2cHandle = BLST_S_FIRST(&g_i2c_channels); i2cHandle; i2cHandle = BLST_S_NEXT(i2cHandle, link)) {
                    if (i2cHandle->index == i) {
                        i2cHandle->channel = magnum_i2c->channel;
                        i2cHandle->asyncReg = magnum_i2c->asyncReg;
                    }
                }
            }
            
            magnum_i2c->s3standby = false;
        }
    }
    
    return NEXUS_SUCCESS;
#else
    BSTD_UNUSED(enabled);
    BSTD_UNUSED(pSettings);
    return NEXUS_SUCCESS;
#endif
}

