/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btc2_3461.c $
 * $brcm_Revision: Hydra_Software_Devel/10 $
 * $brcm_Date: 10/18/12 5:16p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/tc2/src/3461/btc2_3461.c $
 * 
 * Hydra_Software_Devel/10   10/18/12 5:16p atanugul
 * SW3461-269: Merge to main
 * 
 * Hydra_Software_Devel/SW3461-252/1   9/28/12 5:06p atanugul
 * SW3461-269: Add support for BTC2_GetVersionInfo()
 * 
 * Hydra_Software_Devel/9   8/3/12 3:06p atanugul
 * SW3461-229: Send receiver configuration HAB command for FW version 5.0
 * or newer
 * 
 * Hydra_Software_Devel/8   5/23/12 6:03p atanugul
 * SW3461-212: Add TS out to THD and TC2 acquireparameters
 * 
 * Hydra_Software_Devel/7   2/2/12 6:18p atanugul
 * SW3461-141: Implement new apis for SetAcquireParams, GetAcquireParams
 * and GetDefaultAcquireParams
 * 
 * Hydra_Software_Devel/6   12/21/11 6:02p atanugul
 * SW3461-92: Add TC2 Selective Status support for 3461 PI
 * 
 * Hydra_Software_Devel/5   9/22/11 10:33a atanugul
 * SW3461-3: Merge to main
 * 
 * Hydra_Software_Devel/4   9/8/11 1:08p atanugul
 * SW3461-36: Add DVB T2 status API
 * 
 * Hydra_Software_Devel/3   8/17/11 3:31p atanugul
 * SW3461-36: Implement BTC2_GetLockStatus()
 * 
 * Hydra_Software_Devel/2   6/6/11 1:06p atanugul
 * SW3461-3:  remove untun(), getlockstatus(); fix resetStatus
 * 
 * Hydra_Software_Devel/1   3/16/11 4:14p atanugul
 * SW3461-3: Add support for BCM3461
 * 
 *
 ***************************************************************************/
#include "bhab.h"
#include "btc2.h"
#include "btc2_priv.h"
#include "btc2_3461.h"
#include "btc2_3461_priv.h"

BDBG_MODULE(btc2_3461);

/*******************************************************************************
*
*   Default Module Settings
*
*******************************************************************************/
static const BTC2_Settings defDevSettings =
{
    BHAB_DevId_eTC20,       
    NULL,                   /* BHAB handle, must be provided by application*/
    {
        BTC2_3461_Open,
        BTC2_3461_Close,
        BTC2_3461_Init,
        BTC2_3461_GetVersion,
        BTC2_3461_GetVersionInfo,
        BTC2_3461_GetTotalChannels,
        BTC2_3461_OpenChannel,
        BTC2_3461_CloseChannel,
        BTC2_3461_GetDevice,
        BTC2_3461_GetChannelDefaultSettings,
        BTC2_3461_GetStatus,
        BTC2_3461_GetLockStatus,        
        BTC2_3461_GetSoftDecision,
        BTC2_3461_InstallCallback,
        BTC2_3461_GetDefaultAcquireParams,
        BTC2_3461_SetAcquireParams,
        BTC2_3461_GetAcquireParams,       
        BTC2_3461_Acquire,
        BTC2_3461_EnablePowerSaver,
        BTC2_3461_DisablePowerSaver,
        BTC2_3461_ProcessNotification,
        BTC2_3461_ResetStatus,
        BTC2_3461_RequestAsyncStatus,
        BTC2_3461_GetAsyncStatus,
        BTC2_3461_ReadSlave,
        BTC2_3461_WriteSlave,
        BTC2_3461_RequestSelectiveAsyncStatus,
        BTC2_3461_GetSelectiveAsyncStatusReadyType,
        BTC2_3461_GetSelectiveAsyncStatus
    },
    false,
    NULL,
    NULL
};

static const BTC2_InbandParam defInbandParams = 
{
    BTC2_ModulationType_eDvbt2,
    BTC2_AcquireMode_eAuto,
    BTC2_Bandwidth_e8MHz,
    true,
    0,
    false  
};

/***************************************************************************
Summary:
    This function returns the default settings for TC2 module.

Description:
    This function is responsible for returns the default setting for 
    BTC2 module. The returning default setting should be when
    opening the device.

Returns:
    TODO:

See Also:
    BTC2_3461_Open()

****************************************************************************/
BERR_Code BTC2_3461_GetDefaultSettings(
    BTC2_Settings *pDefSettings,        /* [out] Returns default setting */
    BCHP_Handle hChip                   /* [in] Chip handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BTC2_3461_GetDefaultSettings);
    BSTD_UNUSED(hChip);

    *pDefSettings = defDevSettings;

    BDBG_LEAVE(BTC2_3461_GetDefaultSettings);
    return( retCode );
}

/***************************************************************************
Summary:
    This function returns the default settings for TC2 module.

Description:
    This function is responsible for returns the default setting for 
    BTC2 module. The returning default setting should be when
    opening the device.

Returns:
    TODO:

See Also:
    BTC2_3461_Open()

****************************************************************************/
BERR_Code BTC2_3461_GetDefaultAcquireParams(
    BTC2_InbandParam *ibParams          /* [out] default Inband Parameters */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BTC2_3461_GetDefaultAcquireParams);

    *ibParams = defInbandParams;

    BDBG_LEAVE(BTC2_3461_GetDefaultAcquireParams);
    return( retCode );
}