/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: baob.c $
 * $brcm_Revision: Hydra_Software_Devel/30 $
 * $brcm_Date: 10/26/12 12:48p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/aob/3128/baob.c $
 * 
 * Hydra_Software_Devel/30   10/26/12 12:48p atanugul
 * SW3128-238: Deprecate enableFEC in BAOB_Settings structure; Update
 * BAOB_SetConfigSettings() with proper settings for BAOB_OutputMode
 * 
 * Hydra_Software_Devel/29   10/18/12 3:43p atanugul
 * SW3128-228: Add API Support for Reading New Front-End 312X, 314X, 346X
 * F/W Version IDs
 * 
 * Hydra_Software_Devel/28   10/10/12 4:41p mward
 * SW7435-403: Don't get more than nbrToGet in BAOB_GetSoftDecision().
 * 
 * Hydra_Software_Devel/27   9/28/12 2:33p atanugul
 * SW3128-228: Add support for  BAOB_GetVersionInfo()
 * 
 * Hydra_Software_Devel/26   9/7/12 12:28p atanugul
 * SW7425-3788: restructure HAB directory
 * 
 * Hydra_Software_Devel/25   9/7/12 11:42a atanugul
 * SW7425-3788: restructure HAB directory
 * 
 * Hydra_Software_Devel/24   5/16/12 7:08p atanugul
 * SW3128-164: Add Power Check for AOB outputMode
 * 
 * Hydra_Software_Devel/23   5/1/12 3:07p atanugul
 * SW3128-149: Add acquisitionTime, totalAcquisitionTime and rfFrequency
 * to BAOB_Status
 * 
 * Hydra_Software_Devel/22   4/27/12 12:05p atanugul
 * SW7435-145: Fix Coverity Defect
 * 
 * Hydra_Software_Devel/21   4/4/12 10:50a atanugul
 * SW3128-131: Add BAOB_GetConfigSettings() and BAOB_SetConfigSettings()
 * API
 * 
 * Hydra_Software_Devel/20   2/16/12 4:38p atanugul
 * SW3128-120: Change resolution for feGain, digitalAgc and equalizerGain
 * units
 * 
 * Hydra_Software_Devel/19   2/14/12 10:42a atanugul
 * SW3128-119: Update SetAcquireParams API to make the second parameter as
 * const
 * 
 * Hydra_Software_Devel/18   2/13/12 4:35p atanugul
 * SW3128-119: Implement new apis for SetAcquireParams, GetAcquireParams
 * and GetDefaultAcquireParams
 * 
 * Hydra_Software_Devel/17   2/8/12 3:40p atanugul
 * SW3128-120: Add Channelpower, feGain, digitalAgc, eqGain to AOB Status;
 * update status to match HAB document
 * 
 * Hydra_Software_Devel/16   1/18/12 12:07p atanugul
 * SW3128-110: move Nyquist Filter Selection from BAOB_AcquireParam to
 * BAOB_Settings
 * 
 * Hydra_Software_Devel/15   1/16/12 4:17p atanugul
 * SW3128-107: Cleanup
 * 
 * Hydra_Software_Devel/14   1/13/12 5:31p atanugul
 * SW3128-110: Add Nyquist filter Selection to OOB acquire parameters
 * 
 * Hydra_Software_Devel/12   12/22/11 3:38p atanugul
 * SW3128-100: Check for power status before sending HAB commands
 * 
 * Hydra_Software_Devel/11   12/19/11 2:14p atanugul
 * SW3128-76: Fix A0 build error
 * 
 * Hydra_Software_Devel/10   12/16/11 12:02p atanugul
 * SW3128-92: Add  FW auto detection support
 * 
 * Hydra_Software_Devel/9   12/14/11 3:04p atanugul
 * SW3128-76: Update BAOB_Acquire() according to the HAB document; update
 * description for BAOB_Callback
 * 
 * Hydra_Software_Devel/8   12/14/11 2:04p atanugul
 * SW3128-2: Add BAOB_GetSoftDecision() support for 3128
 * 
 * Hydra_Software_Devel/7   12/14/11 11:55a atanugul
 * SW3128-2: check for enableFEC in BAOB_Acquire()
 * 
 * Hydra_Software_Devel/6   12/13/11 12:34p atanugul
 * SW3128-76: Add "No Signal" support to BAOB_GetLockStatus()
 * 
 * Hydra_Software_Devel/5   12/12/11 5:32p atanugul
 * SW3128-92: Add support to auto detect 3123/4/8 and restrict features
 * based on the chipId
 * 
 * Hydra_Software_Devel/4   12/9/11 12:58p atanugul
 * SW3128-75: Move from "global core type" power hab commands to "core-
 * specific" power hab commands; cleanup
 * 
 * Hydra_Software_Devel/3   12/5/11 9:59p vishk
 * SW7425-1851: Fix coverity defects in nexus frontend
 * 
 * Hydra_Software_Devel/2   5/6/11 12:49p atanugul
 * SW3128-2: Fix AOB Status
 * 
 * Hydra_Software_Devel/1   4/14/11 5:02p atanugul
 * SW3128-2: Add 3128 AOB Support
 * 
 ****************************************************************************/
#include "bstd.h"
#include "baob.h"
#include "baob_priv.h"
#include "bhab.h"
#include "../../c0/bchp_leap_ctrl.h"
#include "../../c0/bchp_tm.h"
#include "../../c0/bchp_oob.h"

BDBG_MODULE(baob);

#define sizeInLong(x)   (sizeof(x)/sizeof(uint32_t))
#define CHK_RETCODE( rc, func )             \
do {                                        \
    if( (rc = BERR_TRACE(func)) != BERR_SUCCESS ) \
    {                                       \
        goto done;                          \
    }                                       \
} while(0)


#define DEV_MAGIC_ID            ((BERR_AOB_ID<<16) | 0xFACE)


/*******************************************************************************
*
*   Default Module Settings
*
*******************************************************************************/
static const BAOB_Settings defDevSettings =
{
    BHAB_DevId_eAOB0,                   
    NULL,                       /* Hab handle, must be provided by application*/
    BAOB_SETTINGS_XTALFREQ, 
    false,   
    true,                       /* Default is open drain mode. */
    true,                       /* Default is serial data. */
    BAOB_NyquistFilter_eRaisedCosine_50
};

static const BAOB_AcquireParam defAcquireParams = 
{
    BAOB_ModulationType_eDvs178Qpsk,
    5360537, /* Symbol Rate */
    true,
    BAOB_SpectrumMode_eAuto,
    BAOB_BerInputSrc_eFecOutput
};

/*******************************************************************************
*
*   Private Module Data
*
*******************************************************************************/


/*******************************************************************************
*
*   Private Module Functions
*
*******************************************************************************/
BERR_Code BAOB_3128_p_EventCallback_isr(
    void * pParam1, int param2
    )
{
    BAOB_Handle hDev = (BAOB_Handle) pParam1;
    BERR_Code retCode = BERR_SUCCESS;
    BHAB_InterruptType event = (BHAB_InterruptType) param2;

    BDBG_ENTER(BAOB_3128_p_EventCallback_isr);
    
    BDBG_ASSERT( hDev );
    switch (event) {
        case BHAB_Interrupt_eLockChange:
            if( hDev->pCallback[BAOB_Callback_eLockChange] != NULL )
            {
                (hDev->pCallback[BAOB_Callback_eLockChange])(hDev->pCallbackParam[BAOB_Callback_eLockChange] );
            }
        break;
        case BHAB_Interrupt_eOobAsyncStatusReady:
            {
                if( hDev->pCallback[BAOB_Callback_eAsyncStatusReady] != NULL )
                {
                    (hDev->pCallback[BAOB_Callback_eAsyncStatusReady])(hDev->pCallbackParam[BAOB_Callback_eAsyncStatusReady] );
                }
            }
            break;
        default:
            BDBG_WRN((" unknown event code from 3128"));
        break;
    }

    BDBG_LEAVE(BAOB_3128_p_EventCallback_isr);
    return( retCode );
}

/*******************************************************************************
*
*   Public Module Functions
*
*******************************************************************************/

BERR_Code BAOB_GetDefaultSettings(
    BAOB_Settings *pDefSettings,        /* [out] Returns default setting */
    BCHP_Handle hChip                   /* [in] Chip handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BSTD_UNUSED(hChip);
    BDBG_ENTER(BAOB_GetDefaultSettings);

    *pDefSettings = defDevSettings;

    BDBG_LEAVE(BAOB_GetDefaultSettings);
    return( retCode );
}

BERR_Code BAOB_Open(
    BAOB_Handle *pAob,                  /* [out] Returns handle */
    BCHP_Handle hChip,                  /* [in] Chip handle */
    BREG_Handle hRegister,              /* [in] Register handle */
    BINT_Handle hInterrupt,             /* [in] Interrupt handle */
    const BAOB_Settings *pDefSettings   /* [in] Default settings */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BAOB_Handle hDev;
    unsigned int event=0;  
    uint16_t chipVer;    
    uint32_t familyId, chipId;    
    uint8_t apVer, minApVer;

    BDBG_ENTER(BAOB_Open);

    /* Alloc memory from the system heap */
    hDev = (BAOB_Handle) BKNI_Malloc( sizeof( BAOB_P_Handle ) );
    if( hDev == NULL )
    {
        retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        BDBG_ERR(("BAOB_Open: BKNI_malloc() failed"));
        goto done;
    }
    BKNI_Memset( hDev, 0x00, sizeof( BAOB_P_Handle ) );

    hDev->magicId = DEV_MAGIC_ID;
    hDev->hChip = hChip;
    hDev->hRegister = hRegister;
    hDev->hInterrupt = hInterrupt;
    hDev->hHab = pDefSettings->hGeneric;
    hDev->devId = pDefSettings->devId;
    hDev->enableFEC = pDefSettings->enableFEC;
    hDev->nyquist = pDefSettings->nyquist;
    hDev->bPowerdown = true; 
    hDev->outputMode = BAOB_OutputMode_eDifferentialDecoder;

    CHK_RETCODE(retCode, BKNI_CreateMutex(&hDev->mutex));
    BHAB_InstallInterruptCallback( hDev->hHab,  hDev->devId, BAOB_3128_p_EventCallback_isr , (void *) hDev, event);

    CHK_RETCODE(retCode, BHAB_GetApVersion(hDev->hHab, &familyId, &chipId, &chipVer, &apVer, &minApVer));  
    
    if((chipId == 0x00) && (familyId == 0x3128))
        chipId = 0x3128;
        
    if(chipId == 0x3128)
        *pAob = hDev;
    else
    {
        retCode = BERR_TRACE(BERR_NOT_SUPPORTED);
        BDBG_ERR(("AOB not supported on %x", chipId));        
    }

done:
    if( retCode != BERR_SUCCESS )
    {
        if( hDev != NULL )
        {
            BKNI_Free( hDev );
        }
        *pAob = NULL;
    }

    BDBG_LEAVE(BAOB_Open);
    return( retCode );
}

BERR_Code BAOB_Close(
    BAOB_Handle hDev                    /* [in] Device handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    
    BDBG_ENTER(BAOB_Close);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
    BDBG_ASSERT( hDev->hHab );

    BKNI_DestroyMutex(hDev->mutex);

    hDev->magicId = 0x00;       /* clear it to catch inproper use */
    BKNI_Free( (void *) hDev );

    BDBG_LEAVE(BAOB_Close);
    return( retCode );
}

BERR_Code BAOB_GetDefaultAcquireParams(
    BAOB_AcquireParam *obParams         /* [Out] default Out of band acquire parameters to use */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BAOB_GetDefaultAcquireParams);
  
    *obParams = defAcquireParams;

    BDBG_LEAVE(BAOB_GetDefaultAcquireParams);
    return( retCode );
}
    
BERR_Code BAOB_SetAcquireParams(
    BAOB_Handle hDev,                   /* [in] Device handle */
    const BAOB_AcquireParam *obParams         /* [in] Out of band acquire parameters to use */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    uint8_t buf[9] = HAB_MSG_HDR(BAOB_eAcquireParamsWrite, 0x4, BAOB_CORE_TYPE, BAOB_CORE_ID );

    BDBG_ENTER(BAOB_SetAcquireParams);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
    BDBG_ASSERT( hDev->hHab );

    BKNI_AcquireMutex(hDev->mutex);
    hDev->isLock = false;
    BKNI_ReleaseMutex(hDev->mutex);
  
    if(hDev->bPowerdown)
    {
        BDBG_ERR(("AOB core Powered Off"));
        retCode = BERR_TRACE(BAOB_ERR_POWER_DOWN);
    }
    else
    {    
        /* Set Acquire Params */      
        buf[4] = ((obParams->autoAcquire) ? 0x20 : 0x00);
        if(obParams->spectrum == BAOB_SpectrumMode_eAuto)
            buf[4] |= 0x10;
        if(obParams->spectrum == BAOB_SpectrumMode_eInverted)
            buf[4] |= 0x8;     
        switch (obParams->modType) {
            case BAOB_ModulationType_eDvs167Qpsk:
                if(obParams->symbolRate == 772000) {
                    buf[5] |= 0x18;
                }
                else if(obParams->symbolRate == 1544000){
                    buf[5] |= 0x28;
                }
                break;
            case BAOB_ModulationType_eDvs178Qpsk:
                break;
            default:
                BDBG_ERR(("Unsupported modulation type"));
        }
        buf[6] = obParams->berSrc << 6;
        buf[6] |= (hDev->nyquist << 4) & 0x30;
        CHK_RETCODE(retCode, BHAB_SendHabCommand(hDev->hHab, buf, 9, buf, 0, false, true, 9 ));
    }

done:
    BDBG_LEAVE(BAOB_SetAcquireParams);
    return( retCode );
}

BERR_Code BAOB_GetAcquireParams(
    BAOB_Handle hDev,                   /* [in] Device handle */
    BAOB_AcquireParam *obParams         /* [Out] Out of band acquire parameters to use */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    uint8_t buf[9] = HAB_MSG_HDR(BAOB_eAcquireParamsRead, 0, BAOB_CORE_TYPE, BAOB_CORE_ID );

    BDBG_ENTER(BAOB_GetAcquireParams);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
    BDBG_ASSERT( hDev->hHab );

    BKNI_AcquireMutex(hDev->mutex);
    hDev->isLock = false;
    BKNI_ReleaseMutex(hDev->mutex);
  
    if(hDev->bPowerdown)
    {
        BDBG_ERR(("AOB core Powered Off"));
        retCode = BERR_TRACE(BAOB_ERR_POWER_DOWN);
    }
    else
    {
        /* Get Acquire Params */
        CHK_RETCODE(retCode, BHAB_SendHabCommand(hDev->hHab, buf, 5, buf, 9, false, true, 9 ));
        
        obParams->autoAcquire = (buf[4] & 0x20) >> 5;
        switch((buf[4] & 0x18) >> 3){
            case 0:
                obParams->spectrum =  BAOB_SpectrumMode_eNoInverted;
                break;
            case 1:
                obParams->spectrum =  BAOB_SpectrumMode_eInverted;
                break;
            case 2:
                obParams->spectrum =  BAOB_SpectrumMode_eAuto;
                break;                
            default:
                obParams->spectrum =  BAOB_SpectrumMode_eAuto;
        }

        if((buf[5] & 0x8) >> 3)
        {
            obParams->modType = BAOB_ModulationType_eDvs167Qpsk;        
            switch((buf[5] & 0x70) >> 4)
            {
                case 1:

                    obParams->symbolRate = 772000;
                    break;              
                case 2:
                    obParams->symbolRate = 1544000;
                    break;               
                default:
                    obParams->symbolRate = 772000;              
            }
        }
        else
            obParams->modType = BAOB_ModulationType_eDvs178Qpsk;        
    
        obParams->berSrc = (buf[6] & 0xC0) >> 6;
    }

done:
    BDBG_LEAVE(BAOB_GetAcquireParams);
    return( retCode );
}

BERR_Code BAOB_Acquire(
    BAOB_Handle hDev,                   /* [in] Device handle */
    BAOB_AcquireParam *obParams         /* [in] Out of band acquire parameters to use */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    uint8_t buf[5] = HAB_MSG_HDR(BAOB_eAcquire, 0, BAOB_CORE_TYPE, BAOB_CORE_ID );

    BDBG_ENTER(BAOB_Acquire);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
    BDBG_ASSERT( hDev->hHab );
    BSTD_UNUSED( obParams );

    BKNI_AcquireMutex(hDev->mutex);
    hDev->isLock = false;
    BKNI_ReleaseMutex(hDev->mutex);
  
    if(hDev->bPowerdown)
    {
        BDBG_ERR(("AOB core Powered Off"));
        retCode = BERR_TRACE(BAOB_ERR_POWER_DOWN);
    }
    else
    {
        /* Acquire */  
        CHK_RETCODE(retCode, BHAB_SendHabCommand(hDev->hHab, buf, 5, buf, 0, false, true, 5 ));    
        BHAB_CHK_RETCODE(BHAB_EnableLockInterrupt(hDev->hHab, hDev->devId, true));  
    }

done:
    BDBG_LEAVE(BAOB_Acquire);
    return( retCode );
}

BERR_Code BAOB_GetStatus(
    BAOB_Handle hDev,                   /* [in] Device handle */
    BAOB_Status *pStatus                /* [out] Returns status */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BSTD_UNUSED(hDev);
    BSTD_UNUSED(pStatus);

    return retCode; 
}

BERR_Code BAOB_GetAsyncStatus(
    BAOB_Handle hDev,                   /* [in] Device handle */
    BAOB_Status *pStatus                /* [out] Returns status */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    uint8_t buf[66] = HAB_MSG_HDR(BAOB_eGetStatus, 0, BAOB_CORE_TYPE, BAOB_CORE_ID );

    BDBG_ENTER(BAOB_GetAsyncStatus);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
    BDBG_ASSERT( hDev->hHab );
 
    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    if(hDev->bPowerdown)
    {
        BDBG_ERR(("AOB core Powered Off"));
        retCode = BERR_TRACE(BAOB_ERR_POWER_DOWN);
    }
    else
    {  
        CHK_RETCODE(retCode, BHAB_SendHabCommand(hDev->hHab, buf, 5, buf, 65, false, true, 65));
        
        /* Populate the status structure here. */
        pStatus->modType = ((buf[5] & 0x8) >> 3) ? BAOB_ModulationType_eDvs167Qpsk : BAOB_ModulationType_eDvs178Qpsk;
        pStatus->sysXtalFreq = 54000000;        
        pStatus->autoInvertSpectrum = (buf[4] & 0x10) >> 4;
        pStatus->autoAcquire = (buf[4] & 0x20) >> 5;
        pStatus->isFecLock = (buf[6] & 0x4) >> 2;
        pStatus->isQamLock = (buf[6] & 0x8) >> 3;  
        pStatus->isSpectrumInverted = buf[6] & 0x1;
        pStatus->bertSync = (buf[6] & 0x20) >> 5;
        
        switch((buf[6] & 0xC0) >> 6) 
        {
            case 0:
                pStatus->berSrc = BAOB_BerInputSrc_eRcvIChOutput;
                break;
            case 1:
                pStatus->berSrc = BAOB_BerInputSrc_eRcvQChOutput;
                break;
            case 2:
                pStatus->berSrc = BAOB_BerInputSrc_eRcvIQChOutputIntrlv;
                break;
            case 3:
                pStatus->berSrc = BAOB_BerInputSrc_eFecOutput;
                break;
            default:
                pStatus->berSrc = BAOB_BerInputSrc_eRcvIChOutput;
                break;
        }
       
        pStatus->reacqCount = (buf[0x8] << 8) | buf[0x9];
        pStatus->snrEstimate = (int32_t)(((int8_t)buf[0x0A] << 8) | buf[0x0B]);
        pStatus->sampleRate = (uint32_t)((buf[0xC] << 24) | (buf[0xD] << 16) | (buf[0xE] << 8) | buf[0xF]);    
        pStatus->symbolRate = (uint32_t)((buf[0x10] << 24) | (buf[0x11] << 16) | (buf[0x12] << 8) | buf[0x13]);
        pStatus->carrierFreqOffset = (int32_t)((buf[0x14] << 24) | (buf[0x15] << 16) | (buf[0x16] << 8) | buf[0x17])*1000;
        pStatus->carrierPhaseOffset = 0; /* not supported */
        pStatus->symbolError = (int32_t)((buf[0x18] << 24) | (buf[0x19] << 16) | (buf[0x1A] << 8) | buf[0x1B]);
        pStatus->berErrorCount = (uint32_t)((buf[0x1c] << 24) | (buf[0x1d] << 16) | (buf[0x1e] << 8) | buf[0x1f]); 
        pStatus->correctedCount = (uint32_t)((buf[0x20] << 24) | (buf[0x21] << 16) | (buf[0x22] << 8) | buf[0x23]);        
        pStatus->uncorrectedCount = (uint32_t)((buf[0x24] << 24) | (buf[0x25] << 16) | (buf[0x26] << 8) | buf[0x27]);
        pStatus->atmTotalCount = (uint32_t)((buf[0x28] << 24) | (buf[0x29] << 16) | (buf[0x2A] << 8) | buf[0x2B]);
        pStatus->atmLossCount = (uint32_t)((buf[0x2C] << 24) | (buf[0x2D] << 16) | (buf[0x2E] << 8) | buf[0x2F]);     
        pStatus->aobChannelPower = (int32_t)(((int8_t)buf[0x30] << 8) | buf[0x31])*100/256 + 4875;
        pStatus->feGain = (int32_t)(((int8_t)buf[0x32] << 8) | buf[0x33])*100/256;
        pStatus->digitalAgcGain = (int32_t)(((int8_t)buf[0x34] << 8) | buf[0x35])*100/256;
        pStatus->equalizerGain = (int32_t)(((int8_t)buf[0x36] << 8) | buf[0x37])*100/256;
        pStatus->acquisitionTime = (((int8_t)buf[0x38] << 8) | buf[0x39]);
        pStatus->totalAcquisitionTime = (((int8_t)buf[0x3A] << 8) | buf[0x3B]);
        pStatus->rfFrequency = ((buf[0x3C] << 24) | (buf[0x3D] << 16) | (buf[0x3E] << 8) | buf[0x3F]);      
    }

done:
    BDBG_LEAVE(BAOB_GetAsyncStatus);
    return retCode; 
}

BERR_Code BAOB_RequestAsyncStatus(
    BAOB_Handle hDev                   /* [in] Device handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    uint8_t buf[5] = HAB_MSG_HDR(BAOB_eRequestAsyncStatus, 0, BAOB_CORE_TYPE, BAOB_CORE_ID );    


    BDBG_ENTER(BAOB_RequestAsyncStatus);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
    BDBG_ASSERT( hDev->hHab );
    
    if(hDev->bPowerdown)
    {
        BDBG_ERR(("AOB core Powered Off"));
        retCode = BERR_TRACE(BAOB_ERR_POWER_DOWN);
    }
    else
        CHK_RETCODE(retCode, BHAB_SendHabCommand(hDev->hHab, buf, 5, buf, 0, false, true, 5));
    
done:
    BDBG_LEAVE(BAOB_RequestAsyncStatus);
    return retCode; 
}

BERR_Code BAOB_GetLockStatus(
    BAOB_Handle hDev,                   /* [in] Device handle */
    BAOB_LockStatus *pLockStatus        /* [out] Returns lock status */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    uint32_t sb;    
#if (BAOB_312X_VER != BCHP_VER_A0)    
    uint8_t status;
#endif  

    BDBG_ENTER(BAOB_GetLockStatus);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

#if (BAOB_312X_VER == BCHP_VER_A0)
    CHK_RETCODE(retCode, BHAB_ReadRegister(hDev->hHab, BCHP_LEAP_CTRL_SPARE, &sb));
    *pLockStatus = (sb >> 12) & 0x1;
#else    
    CHK_RETCODE(retCode, BHAB_ReadRegister(hDev->hHab, BCHP_LEAP_CTRL_SW_SPARE1, &sb));
    status = (sb >> 12) & 0xF;

    switch (status)
    {
        case 1:
            *pLockStatus = BAOB_LockStatus_eLocked;
            break;
          
        case 2:
            *pLockStatus = BAOB_LockStatus_eUnlocked;
            break;          

        case 3:
            *pLockStatus = BAOB_LockStatus_eNoSignal;
            break;
          
        default:
            retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
            break;
    }  
#endif 

done:
    BDBG_LEAVE(BAOB_GetLockStatus);    
    return retCode;
}

BERR_Code BAOB_GetSoftDecision(
    BAOB_Handle hDev,                   /* [in] Device handle */
    int16_t nbrToGet,                   /* [in] Number values to get */
    int16_t *iVal,                      /* [out] Ptr to array to store output I soft decision */
    int16_t *qVal,                      /* [out] Ptr to array to store output Q soft decision */
    int16_t *nbrGotten                  /* [out] Number of values gotten/read */
    )
{
    BERR_Code retCode = BERR_NOT_SUPPORTED;
    uint8_t i;    
    uint8_t buf[0x41] = HAB_MSG_HDR(BAOB_eGetConstellation, 0, BAOB_CORE_TYPE, BAOB_CORE_ID ); 

    BDBG_ENTER(BAOB_GetSoftDecision);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
    BDBG_ASSERT( hDev->hHab );     

    if(hDev->bPowerdown)
    {
        BDBG_ERR(("AOB core Powered Off"));
        retCode = BERR_TRACE(BAOB_ERR_POWER_DOWN);
    }
    else
    {      
        CHK_RETCODE(retCode, BHAB_SendHabCommand(hDev->hHab, buf, 5, buf, 0x41, false, true, 0x41));
      
        for (i = 0; i < 30 && i < nbrToGet; i++)
		{
            iVal[i] = buf[4+(2*i)] & 0xFF;
            qVal[i] = buf[5+(2*i)] & 0xFF;    
        }

        *nbrGotten = i;
    }

done:
    BDBG_LEAVE(BAOB_GetSoftDecision);
    return( retCode );
}

BERR_Code BAOB_EnablePowerSaver(
    BAOB_Handle hDev                    /* [in] Device handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    uint8_t buf[5] = HAB_MSG_HDR(BAOB_ePowerCtrlOff, 0, BAOB_CORE_TYPE, BAOB_CORE_ID); 
    
    BDBG_ENTER(BAOB_EnablePowerSaver);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
    BDBG_ASSERT( hDev->hHab );

    if(!hDev->bPowerdown)
    {
        CHK_RETCODE(retCode, BHAB_SendHabCommand(hDev->hHab, buf, 5, buf, 0, false, true, 5));
        hDev->bPowerdown = true;
    }

done:
    BDBG_LEAVE(BAOB_EnablePowerSaver);
    return( retCode );
}

BERR_Code BAOB_DisablePowerSaver(
    BAOB_Handle hDev                    /* [in] Device handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    uint8_t buf[5] = HAB_MSG_HDR(BAOB_ePowerCtrlOn, 0, BAOB_CORE_TYPE, BAOB_CORE_ID);  
    
    BDBG_ENTER(BAOB_DisablePowerSaver);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
    BDBG_ASSERT( hDev->hHab );

    if(hDev->bPowerdown)
    {    
        CHK_RETCODE(retCode, BHAB_SendHabCommand(hDev->hHab, buf, 5, buf, 0, false, true, 5));    
        hDev->bPowerdown = false;
    }
    
done:
    BDBG_LEAVE(BAOB_DisablePowerSaver);
    return( retCode );
}

BERR_Code BAOB_ProcessNotification(
    BAOB_Handle hDev,                       /* [in] Device handle */
    unsigned int event                      /* [in] Event code and event data*/
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    bool isFECLock, isQAMLock, isLock;
    unsigned int event_code;
    
    BDBG_ENTER(BAOB_ProcessNotification);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    event_code = event>>16;

    switch (event_code) {
        /*Set the event - Vish.*/
        case 1:
            isQAMLock = 1;
            isFECLock = 1;
            if (hDev->enableFEC){
                isLock = isQAMLock && isFECLock;
            } else {
                isLock = isQAMLock;
            }
            {
                if( hDev->pCallback[BAOB_Callback_eLockChange] != NULL )
                {
                    (hDev->pCallback[BAOB_Callback_eLockChange])(hDev->pCallbackParam[BAOB_Callback_eLockChange] );
                }
            }
            break;
        default:
            BDBG_WRN((" unknown event code from 3128 %x",event_code ));
            break;
    }

    BDBG_LEAVE(BAOB_ProcessNotification);
    return( retCode );
}

BERR_Code BAOB_InstallCallback(
    BAOB_Handle hDev,                   /* [in] Device handle */
    BAOB_Callback callbackType,         /* [in] Type of callback */
    BAOB_CallbackFunc pCallback,        /* [in] Function Ptr to callback */
    void *pParam                        /* [in] Generic parameter send on callback */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ENTER(BAOB_InstallCallback);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    switch( callbackType )
    {
        case BAOB_Callback_eLockChange:
            hDev->pCallback[callbackType] = pCallback;
            hDev->pCallbackParam[callbackType] = pParam;
            break;
        case BAOB_Callback_eAsyncStatusReady:
            hDev->pCallback[callbackType] = pCallback;
            hDev->pCallbackParam[callbackType] = pParam;
            break;
        default:
            retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
            break;
    }

    BDBG_LEAVE(BAOB_InstallCallback);
    return( retCode );
}

BERR_Code BAOB_ResetStatus(
    BAOB_Handle hDev                    /* [in] Device handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    uint8_t buf[5] = HAB_MSG_HDR(BAOB_eResetStatus, 0, BAOB_CORE_TYPE, BAOB_CORE_ID ); 

    BDBG_ENTER(BAOB_ResetStatus);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
    BDBG_ASSERT( hDev->hHab );
 
    if(hDev->bPowerdown)
    {
        BDBG_ERR(("AOB core Powered Off"));
        retCode = BERR_TRACE(BAOB_ERR_POWER_DOWN);
    }
    else    
        CHK_RETCODE(retCode, BHAB_SendHabCommand(hDev->hHab, buf, 5, buf, 0, false, true, 5));

done:   
    BDBG_LEAVE(BAOB_ResetStatus);
    return( retCode );
}

BERR_Code BAOB_GetConfigSettings(
    BAOB_Handle hDev,                   /* [in] Device handle */
    BAOB_ConfigSettings *pConfigSettings /* [out] Config Settings */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BAOB_GetConfigSettings);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
 
    pConfigSettings->outputMode = hDev->outputMode; 
  
    BDBG_LEAVE(BAOB_GetConfigSettings);
    return( retCode );
}    

     
BERR_Code BAOB_SetConfigSettings(
    BAOB_Handle hDev,                   /* [in] Device handle */
    const BAOB_ConfigSettings *pConfigSettings /* [in] Config Settings */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    uint32_t val;

    BDBG_ENTER(BAOB_SetConfigSettings);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
 
    if(hDev->bPowerdown)
    {
        BDBG_ERR(("AOB core Powered Off"));
    }
    else
    {
        /* enable output to Pad, set for DRX and CRX use on GPO (General Purpose Output) pins */
        val = 0x00000100;
        CHK_RETCODE(retCode, BHAB_WriteRegister(hDev->hHab, BCHP_TM_CC_DRX_CTRL , &val));
        CHK_RETCODE(retCode, BHAB_WriteRegister(hDev->hHab, BCHP_TM_CC_CRX_CTRL , &val));    
        if(pConfigSettings->outputMode == BAOB_OutputMode_eFEC)
        {
            val = 0x1;
            CHK_RETCODE(retCode, BHAB_WriteRegister(hDev->hHab, BCHP_OOB_SERIAL_OUTPUT_CTRL, &val));
        }
        else
        {
            val = 0x3;
            CHK_RETCODE(retCode, BHAB_WriteRegister(hDev->hHab, BCHP_OOB_SERIAL_OUTPUT_CTRL, &val));
        }
        hDev->outputMode = pConfigSettings->outputMode;
    }

done:   
    BDBG_LEAVE(BAOB_SetConfigSettings);
    return( retCode );
}  
    
    
BERR_Code BAOB_GetVersionInfo(
    BAOB_Handle hDev,               /* [in] Device handle */
    BFEC_VersionInfo *pVersionInfo  /* [out] Returns version Info */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    uint8_t buf[29] = HAB_MSG_HDR(BAOB_eGetVersionInfo, 0, BAOB_CORE_TYPE, BAOB_CORE_ID );

    BDBG_ENTER(BAOB_GetVersionInfo);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
    BDBG_ASSERT( hDev->hHab );

    BKNI_AcquireMutex(hDev->mutex);
    hDev->isLock = false;
    BKNI_ReleaseMutex(hDev->mutex);
    
    CHK_RETCODE(retCode, BHAB_SendHabCommand(hDev->hHab, buf, 5, buf, 29, false, true, 29 ));
    pVersionInfo->majorVersion = (buf[4] << 8) | buf[5];
    pVersionInfo->minorVersion = (buf[6] << 8) | buf[7];
    pVersionInfo->buildType = (buf[8] << 8) | buf[9];
    pVersionInfo->buildId = (buf[10] << 8) | buf[11];

done:
    BDBG_LEAVE(BAOB_GetVersionInfo);
    return( retCode );
}    
