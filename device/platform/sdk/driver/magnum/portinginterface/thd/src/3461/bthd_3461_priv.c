/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bthd_3461_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/43 $
 * $brcm_Date: 10/18/12 5:16p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/thd/src/3461/bthd_3461_priv.c $
 * 
 * Hydra_Software_Devel/43   10/18/12 5:16p atanugul
 * SW3461-269: Merge to main
 * 
 * Hydra_Software_Devel/SW3461-252/2   10/17/12 12:05p vishk
 * SW3461-270: Add API Support for Reading New Front-End 312X, 314X, 346X
 * F/W Version IDs
 * 
 * Hydra_Software_Devel/SW3461-252/1   9/28/12 5:03p atanugul
 * SW3461-269: Add support for BTHD_GetVersionInfo()
 * 
 * Hydra_Software_Devel/42   9/7/12 11:43a atanugul
 * SW7425-3788: restructure HAB directory
 * 
 * Hydra_Software_Devel/41   8/3/12 3:07p atanugul
 * SW3461-229: Send receiver configuration HAB command for FW version 5.0
 * or newer
 * 
 * Hydra_Software_Devel/40   5/24/12 4:51p atanugul
 * SW3461-212: Remove duplicate eTransportData in BTHD_ThdStatus structure
 * 
 * Hydra_Software_Devel/39   5/23/12 6:04p atanugul
 * SW3461-212: Add TS out to THD and TC2 acquireparameters
 * 
 * Hydra_Software_Devel/38   5/10/12 11:44a atanugul
 * SW3461-203: Pass eCodeRateLP to FW in case of manual bTpsAcquire
 * 
 * Hydra_Software_Devel/37   3/12/12 6:56p atanugul
 * SW3461-176: 3461 lock recovery issue after Untune()
 * 
 * Hydra_Software_Devel/36   2/15/12 10:57a atanugul
 * SW3461-157: Memset handles in openchannel()
 * 
 * Hydra_Software_Devel/35   2/10/12 10:43a atanugul
 * SW3461-148: Using malloc instead of using memory from the stack
 * 
 * Hydra_Software_Devel/34   2/7/12 5:08p atanugul
 * SW3461-144: Change signalstrength units to 1/100th of a dBmV
 * 
 * Hydra_Software_Devel/33   2/7/12 11:50a atanugul
 * SW3461-146: Add "Scan Mode" to T and T2 Acquire Parameters to 3461 THD
 * and TC2 PI
 * 
 * Hydra_Software_Devel/32   2/3/12 6:10p atanugul
 * SW3461-141: Implement new apis for SetAcquireParams, GetAcquireParams
 * and PowerUp; cleanup
 * 
 * Hydra_Software_Devel/31   2/2/12 6:23p atanugul
 * SW3461-141: Implement new apis for SetAcquireParams, GetAcquireParams
 * and PowerUp
 * 
 * Hydra_Software_Devel/30   1/24/12 11:17a atanugul
 * SW3461-132: Cleanup
 * 
 * Hydra_Software_Devel/29   1/23/12 2:25p atanugul
 * SW3461-136: Fix signalstrength units
 * 
 * Hydra_Software_Devel/28   1/19/12 4:59p atanugul
 * SW3461-135: Add auto acquire on tune support for 3461
 * 
 * Hydra_Software_Devel/27   1/13/12 3:25p atanugul
 * SW3461-133: Add SignalLevelPercent and SignalQualityPercent to THD and
 * TC2 PIs
 * 
 * Hydra_Software_Devel/26   1/12/12 6:35p atanugul
 * SW3461-132: Enable HAB CRC check
 * 
 * Hydra_Software_Devel/25   1/12/12 5:54p atanugul
 * SW3461-132: Enable HAB CRC check
 * 
 * Hydra_Software_Devel/24   12/15/11 12:06p atanugul
 * SW3461-116: BCM3461 can not lock bandwidth 7MHz signal
 * 
 * Hydra_Software_Devel/23   12/7/11 3:34p atanugul
 * SW3461-3: Use firmware's default internal acquire parameters
 * 
 * Hydra_Software_Devel/22   11/30/11 3:54p atanugul
 * SW3461-77: Check for power status before sending HAB commands
 * 
 * Hydra_Software_Devel/21   11/16/11 3:35p atanugul
 * SW3461-79: Add No Signal to GetThdLockStatus()
 * 
 * Hydra_Software_Devel/20   11/16/11 1:01p atanugul
 * SW3461-79: Add No Signal to GetThdLockStatus()
 * 
 * Hydra_Software_Devel/19   11/2/11 4:44p atanugul
 * SW3461-3: Update GetVersion() based on BHAB_GetApVersion() update
 * 
 * Hydra_Software_Devel/18   10/18/11 5:27p atanugul
 * SW3461-67: Add 3461 B0 Support
 * 
 * Hydra_Software_Devel/17   10/5/11 5:09p atanugul
 * SW3461-3: Fix BTHD_3461_P_GetVersion() and
 * BTHD_3461_P_GetChipRevision()
 * 
 * Hydra_Software_Devel/16   9/6/11 9:31p vishk
 * SW3461-48: Fix 3461 frontend issues.
 * 
 * Hydra_Software_Devel/15   8/23/11 3:11p atanugul
 * SW3461-3: Fix eAcquisitionMode in BTHD_3461_P_AcquireThd()
 * 
 * Hydra_Software_Devel/14   8/19/11 5:07p atanugul
 * SW3461-18:  Fix THD status
 * 
 * Hydra_Software_Devel/13   8/18/11 3:34p atanugul
 * SW3461-3: Fix signalstrength
 * 
 * Hydra_Software_Devel/12   8/17/11 4:14p atanugul
 * SW3461-3: Fix modeGuardAcquire
 * 
 * Hydra_Software_Devel/11   8/17/11 3:30p atanugul
 * SW3461-36: Implement BTHD_GetLockStatus()
 * 
 * Hydra_Software_Devel/10   8/12/11 4:17p atanugul
 * SW3461-5: Fix GetSoftDecision()
 * 
 * Hydra_Software_Devel/9   8/12/11 12:16p atanugul
 * SW3461-5: Update BTHD_3461_P_GetThdAsyncStatus()
 * 
 * Hydra_Software_Devel/8   8/10/11 2:13p farshidf
 * SW3461-27: Match the Nexus settings
 * 
 * Hydra_Software_Devel/7   7/28/11 10:46a atanugul
 * SW3461-5: Fix compile error
 * 
 * Hydra_Software_Devel/6   7/22/11 5:39p atanugul
 * SW3461-14: Fix compile warning
 * 
 * Hydra_Software_Devel/5   7/22/11 5:34p atanugul
 * SW3461-14: Add signal strength to T and T2 Status
 * 
 * Hydra_Software_Devel/4   6/20/11 11:47a atanugul
 * SW3461-5: Update THD Status based on HAB document updates
 * 
 * Hydra_Software_Devel/3   6/16/11 9:26p vishk
 * SW3461-5: Add THD support to 3461
 * 
 * Hydra_Software_Devel/2   6/3/11 2:47p atanugul
 * SW3461-5: remove THD powerup from BTHD_Init
 * 
 * Hydra_Software_Devel/1   6/1/11 3:22p atanugul
 * SW3461-5: Add THD support to 3461
 *
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "bthd.h"
#include "bhab.h"
#include "bthd_priv.h"
#include "bthd_3461_priv.h"
#include "bmth.h"
#include "../../b0/bchp_leap_ctrl.h"

BDBG_MODULE(bthd_3461_priv);

/*#define BTHD_DEBUG*/
/*#define BTHD_DO_POLLING*/

#define NO_AP_MODE

#define DEV_MAGIC_ID            ((BERR_THD_ID<<16) | 0xFACE)

/*******************************************************************************
*
*   Private Module Functions
*
*******************************************************************************/
/******************************************************************************
  BTHD_3461_P_EventCallback_isr()
 ******************************************************************************/
BERR_Code BTHD_3461_P_EventCallback_isr(
    void * pParam1, int param2
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTHD_Handle h = (BTHD_Handle) pParam1;
    BHAB_InterruptType event = (BHAB_InterruptType) param2;
    BTHD_3461_P_Handle *p3461 = (BTHD_3461_P_Handle *)(h->pImpl);        

    BDBG_ENTER(BTHD_3461_P_EventCallback_isr);
    BDBG_ASSERT( h );
    BDBG_ASSERT( p3461 );
    BDBG_ASSERT( p3461->hHab);
    BDBG_ASSERT( h->magicId == DEV_MAGIC_ID );

    switch (event) {
        case BHAB_Interrupt_eLockChange:
            {
                if( p3461->pCallback[BTHD_Callback_eLockChange] != NULL )
                {
                    (p3461->pCallback[BTHD_Callback_eLockChange])(p3461->pCallbackParam[BTHD_Callback_eLockChange] );
                }
            }
            break;
        case BHAB_Interrupt_eUpdateGain:
            {
                if( p3461->pCallback[BTHD_Callback_eUpdateGain] != NULL )
                {
                    (p3461->pCallback[BTHD_Callback_eUpdateGain])(p3461->pCallbackParam[BTHD_Callback_eUpdateGain] );
                }
            }
            break;
        case BHAB_Interrupt_eNoSignal:
            {
                BDBG_WRN(("BTHD No Signal"));
                if( p3461->pCallback[BTHD_Callback_eNoSignal] != NULL )
                {
                    (p3461->pCallback[BTHD_Callback_eNoSignal])(p3461->pCallbackParam[BTHD_Callback_eNoSignal] );
                }
            }
            break;
        case BHAB_Interrupt_eThdAsyncStatusReady:
            {
                if( p3461->pCallback[BTHD_Callback_eAsyncStatusReady] != NULL )
                {
                    (p3461->pCallback[BTHD_Callback_eAsyncStatusReady])(p3461->pCallbackParam[BTHD_Callback_eAsyncStatusReady] );
                }
            }
            break;
        default:
            BDBG_WRN((" unknown event code from 3461"));
            break;
    }

    BDBG_LEAVE(BTHD_3461_P_EventCallback_isr);
    return retCode;
}

/******************************************************************************
  BTHD_3461_P_PowerUp()
 ******************************************************************************/
BERR_Code BTHD_3461_P_PowerUp(
    BTHD_Handle h
    )
{
#ifndef BTHD_POWER_MANAGEMENT
    BSTD_UNUSED(h);
    BSTD_UNUSED(uiFlags);    
    return BERR_SUCCESS;
#else
    BERR_Code retCode; 
    uint8_t hab[5] = HAB_MSG_HDR(BTHD_ePowerCtrlOn, 0x0, BTHD_CORE_TYPE, BTHD_CORE_ID); 
    uint8_t hab1[5] = HAB_MSG_HDR(BTHD_ePowerCtrlRead, 0x0, BTHD_CORE_TYPE, BTHD_CORE_ID); 
    uint8_t configParams[9] = HAB_MSG_HDR(BTHD_eConfigParamsWrite, 0x4, BTHD_CORE_TYPE, BTHD_CORE_ID);
    uint32_t revision, checksum;
    BTHD_3461_P_Handle *p3461 = (BTHD_3461_P_Handle *)(h->pImpl);    

    BDBG_ENTER(BTHD_3461_P_PowerUp);
    BDBG_ASSERT( h );
    BDBG_ASSERT( p3461 );
    BDBG_ASSERT( p3461->hHab);
    BDBG_ASSERT( h->magicId == DEV_MAGIC_ID );
    
    BDBG_MSG(("IFD/THD powered on %d", p3461->bPowerdown));
    
    BKNI_Memset(p3461->readBuf, 0, sizeof(p3461->readBuf));

    BTHD_CHK_RETCODE(BHAB_SendHabCommand(p3461->hHab, hab1, 5, p3461->readBuf, 9, false, true, 5));  
   
    if(p3461->readBuf[4] == 0) 
        BTHD_CHK_RETCODE(BHAB_SendHabCommand(p3461->hHab, hab, 5, p3461->readBuf, 0, false, true, 5));

    p3461->bPowerdown = false;
    p3461->bPowerOffToOn = true;    
    BDBG_MSG(("BTHD_3461_P_PowerUp called"));
    
    BTHD_3461_P_GetVersion(h, &revision, &checksum); 

    if((revision >>8) >= 5) {
            configParams[4] = BTHD_CONFIG_PARAMS_BUF1;    
            configParams[5] = BTHD_CONFIG_PARAMS_BUF2;
            configParams[6] = BTHD_CONFIG_PARAMS_BUF3;
            BTHD_CHK_RETCODE(BHAB_SendHabCommand(p3461->hHab, configParams, 9, p3461->readBuf, 0, false, true, 9));
        }     

done:
    return retCode;
#endif    
}

/******************************************************************************
  BTHD_3461_P_PowerDown()
 ******************************************************************************/
BERR_Code BTHD_3461_P_PowerDown(
    BTHD_Handle h
    )
{
#ifndef BTHD_POWER_MANAGEMENT
    BSTD_UNUSED(h);
    return BERR_SUCCESS;
#else
    BERR_Code retCode;
    uint8_t hab[5] = HAB_MSG_HDR(BTHD_ePowerCtrlOff, 0x0, BTHD_CORE_TYPE, BTHD_CORE_ID); 
    BTHD_3461_P_Handle *p3461 = (BTHD_3461_P_Handle *)(h->pImpl);    

    BDBG_ENTER(BTHD_3461_P_PowerDown);
    BDBG_ASSERT( h );
    BDBG_ASSERT( p3461 );
    BDBG_ASSERT( p3461->hHab);
    BDBG_ASSERT( h->magicId == DEV_MAGIC_ID );

    BDBG_MSG(("IFD/THD powered off"));
    
    BDBG_ENTER(BTHD_3461_P_PowerDown);
    BDBG_ASSERT( h );
    BDBG_ASSERT( p3461 );
    BDBG_ASSERT( p3461->hHab );

    /*hab[7] = 1;    */
    BTHD_CHK_RETCODE(BHAB_SendHabCommand(p3461->hHab, hab, 5, p3461->readBuf, 0, false, true, 5));    
    p3461->bPowerdown = true;
    BDBG_MSG(("BTHD_3461_P_PowerDown called"));

done:
    return retCode;
#endif
}

/******************************************************************************
  BTHD_3461_P_Open()
 ******************************************************************************/
BERR_Code BTHD_3461_P_Open(
    BTHD_Handle *h,       /* [out] BTHD handle */
    BCHP_Handle hChip,    /* [in] chip handle */
    void        *pReg,    /* [in] pointer to register or i2c handle */
    BINT_Handle hInterrupt, /* [in] interrupt handle */
    const BTHD_Settings *pDefSettings /* [in] default settings */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTHD_Handle hDev;
    BTHD_3461_P_Handle *p3461;

    BSTD_UNUSED(hInterrupt);
    BSTD_UNUSED(hChip);
    BDBG_ASSERT(pReg);
    BDBG_ASSERT(pDefSettings);
    /*BDBG_ASSERT(pDefSettings->hTmr);*/

    /* allocate memory for the handle */
    hDev = (BTHD_Handle)BKNI_Malloc(sizeof(BTHD_P_Handle));
    BDBG_ASSERT(hDev);
    BKNI_Memset( hDev, 0x00, sizeof( BTHD_P_Handle ) );     
    p3461 = (BTHD_3461_P_Handle *)BKNI_Malloc(sizeof(BTHD_3461_P_Handle));
    BDBG_ASSERT(p3461);
    BKNI_Memset( p3461, 0x00, sizeof( BTHD_3461_P_Handle ) );    
    hDev->pImpl = (void*)p3461;
    
    p3461->hHab = (BHAB_Handle)pDefSettings->hGeneric;    /* For this device, we need the HAB handle */
    p3461->devId = (BHAB_DevId) BHAB_DevId_eTHD0;
    p3461->magicId = DEV_MAGIC_ID;
    /* initialize our handle */
    p3461->hRegister = (BREG_Handle)pReg;
    p3461->bPowerdown = true;
    p3461->bPowerOffToOn = false;
    BKNI_Memcpy((void*)(&(hDev->settings)), (void*)pDefSettings, sizeof(BTHD_Settings));

    hDev->magicId = DEV_MAGIC_ID;

    *h = hDev;
    

    return retCode;
}

/******************************************************************************
  BTHD_3461_P_Close()
 ******************************************************************************/
BERR_Code BTHD_3461_P_Close(
    BTHD_Handle h
    )
{
    BERR_Code retCode;
    BDBG_ASSERT(h);
    
    BDBG_ENTER(BTHD_3461_P_Close);
    BDBG_ASSERT( h );
    BDBG_ASSERT( h->magicId == DEV_MAGIC_ID );
   
    /* Power off on exit */
    retCode = BTHD_3461_P_PowerDown(h);

    BKNI_Free((void*)h->pImpl);
    BKNI_Free((void*)h);

    return retCode;
}

/******************************************************************************
  BTHD_3461_P_Init()
 ******************************************************************************/
BERR_Code BTHD_3461_P_Init(
    BTHD_Handle h,            /* [in] BTHD handle */
    const uint8_t *pHexImage, /* [in] pointer to microcode image. Set to NULL to use default image */
    uint32_t imageLength      /* [in] length of image. Not used in 3461 */
    )
{
    BERR_Code retCode = BERR_SUCCESS;   
   
    BSTD_UNUSED(h);
    
    BSTD_UNUSED(pHexImage);
    BSTD_UNUSED(imageLength);
  
    return retCode;
}


/******************************************************************************
  BTHD_3461_P_SetAcquireParams()
 ******************************************************************************/
BERR_Code BTHD_3461_P_SetAcquireParams(
    BTHD_Handle h,                          /* [in] BTHD handle */
    const BTHD_InbandParams *pParams        /* [in] inband acquisition parameters */
    )
{
    BERR_Code retCode = BERR_SUCCESS; 
    uint8_t nGuard, nTransmissionMode, nConstellation, nHierarchy, nCodeRateLow, nCodeRateHigh, bandwidth;
    uint8_t hab[17] = HAB_MSG_HDR(BTHD_eAcquireParamsWrite, 0xC, BTHD_CORE_TYPE, BTHD_CORE_ID);   

    BTHD_3461_P_Handle *p3461 = (BTHD_3461_P_Handle *)(h->pImpl);

    BDBG_ENTER(BTHD_3461_P_SetAcquireParams);
    BDBG_ASSERT( h );
    BDBG_ASSERT( p3461 );
    BDBG_ASSERT( p3461->hHab);
    BDBG_ASSERT( h->magicId == DEV_MAGIC_ID );
    if(!(!p3461->bPowerOffToOn &&
        (p3461->previousAcquireParams.eAcquisitionMode == pParams->eAcquisitionMode) &&
        (p3461->previousAcquireParams.bandwidth == pParams->bandwidth) &&
        (p3461->previousAcquireParams.bTuneAcquire == pParams->bTuneAcquire) &&
        (p3461->previousAcquireParams.ePullinRange == pParams->ePullinRange) &&
        (p3461->previousAcquireParams.cciMode == pParams->cciMode) &&
        (p3461->previousAcquireParams.eModeGuardAcquire == pParams->eModeGuardAcquire) &&
        (p3461->previousAcquireParams.eGuardInterval == pParams->eGuardInterval) &&
        (p3461->previousAcquireParams.eTransmissionMode == pParams->eTransmissionMode) &&
        (p3461->previousAcquireParams.bTpsAcquire == pParams->bTpsAcquire)  &&
        (p3461->previousAcquireParams.decodeMode == pParams->decodeMode)  &&
        (p3461->previousAcquireParams.eCodeRateLP == pParams->eCodeRateLP)  &&
        (p3461->previousAcquireParams.eCodeRateHP == pParams->eCodeRateHP)  &&
        (p3461->previousAcquireParams.eModulation == pParams->eModulation)  &&
        (p3461->previousAcquireParams.eHierarchy == pParams->eHierarchy)))
    {     
        /* set Acquire Parameters */
        switch(pParams->eAcquisitionMode)
        {
            case BTHD_ThdAcquisitionMode_eAuto:
                hab[4] = 0x1;
                break;
            case BTHD_ThdAcquisitionMode_eManual:
                hab[4] = 0;
                break;            
            case BTHD_ThdAcquisitionMode_eScan:
                hab[4] = 0x2;
                break;             
            default:
                hab[4] = 0x1;            
        }
        
        switch (pParams->bandwidth)
        {
            case BTHD_Bandwidth_5Mhz:
                bandwidth = 3;
                break;
            case BTHD_Bandwidth_6Mhz:
                bandwidth = 2;
                break;
            case BTHD_Bandwidth_7Mhz:
                bandwidth = 1;
                break;
            case BTHD_Bandwidth_8Mhz:
                bandwidth = 0;
                break;
            default:
                bandwidth = 0;
        }        
        hab[4] |= (bandwidth << 2);
        hab[4] |= (pParams->ePullinRange << 4);
        hab[4] |= (pParams->cciMode << 5);
             
        if(pParams->eModeGuardAcquire == BTHD_ModeGuard_eManual)
        {
            hab[5] = 0;
            switch (pParams->eGuardInterval)
            {
                case BTHD_GuardInterval_e1_32:
                    nGuard = 0;
                    break;
                case BTHD_GuardInterval_e1_16:
                    nGuard = 1;
                    break;
                case BTHD_GuardInterval_e1_8:
                    nGuard = 2;
                    break;
                case BTHD_GuardInterval_e1_4:
                    nGuard = 3;
                    break;
                default:
                    nGuard = 0;
            }

            switch (pParams->eTransmissionMode)
            {
                case BTHD_TransmissionMode_e2K:
                    nTransmissionMode = 0;
                    break;
                case BTHD_TransmissionMode_e4K:
                    nTransmissionMode = 2;
                    break;
                case BTHD_TransmissionMode_e8K:
                    nTransmissionMode = 1;
                    break;
                default:
                    nTransmissionMode = 0;
            }
            hab[5] = (nGuard << 4) | (nTransmissionMode << 2);
        }
        else
            hab[5] = pParams->eModeGuardAcquire;
             
        hab[6] = (pParams->bTuneAcquire<< 7);
        
        /* If TPS acquire is set, than user will need to pass in coderate HP, coderate LP, and hierarchy */
        if (pParams->bTpsAcquire)
        {
            hab[12] = pParams->bTpsAcquire;
            hab[12] |= (!(pParams->decodeMode) << 1);       
        }
        else
        {
            switch (pParams->eModulation)
            {
                case BTHD_Modulation_eQpsk:
                    nConstellation = 0;
                    break;
                case BTHD_Modulation_e16Qam:
                    nConstellation = 1;
                    break;
                case BTHD_Modulation_e64Qam:
                    nConstellation = 2;
                    break;
                default:
                    nConstellation = 0;
            }
            
            switch (pParams->eHierarchy)
            {
                case BTHD_Hierarchy_0:
                    nHierarchy = 0;
                    break;
                case BTHD_Hierarchy_1:
                    nHierarchy = 1;
                    break;
                case BTHD_Hierarchy_2:
                    nHierarchy = 2;
                    break;
                case BTHD_Hierarchy_4:
                    nHierarchy = 3;                
                    break;
                default:
                    nHierarchy = 0;
            }
            
            hab[12] = (nHierarchy << 4) | (nConstellation << 2) | (!(pParams->decodeMode) << 1) | pParams->bTpsAcquire;
            
            switch (pParams->eCodeRateLP)
            {
                case BTHD_CodeRate_e1_2:
                    nCodeRateLow = 0;
                    break;
                case BTHD_CodeRate_e2_3:
                    nCodeRateLow = 1;
                    break;
                case BTHD_CodeRate_e3_4:
                    nCodeRateLow = 2;
                    break;
                case BTHD_CodeRate_e5_6:
                    nCodeRateLow = 3;
                    break;
                case BTHD_CodeRate_e7_8:
                    nCodeRateLow = 4;
                    break;                
                default:
                    nCodeRateLow = 0;
            }
            
            switch (pParams->eCodeRateHP)
            {
                case BTHD_CodeRate_e1_2:
                    nCodeRateHigh = 0;
                    break;
                case BTHD_CodeRate_e2_3:
                    nCodeRateHigh = 1;
                    break;
                case BTHD_CodeRate_e3_4:
                    nCodeRateHigh = 2;
                    break;
                case BTHD_CodeRate_e5_6:
                    nCodeRateHigh = 3;
                    break;
                case BTHD_CodeRate_e7_8:
                    nCodeRateHigh = 4;
                    break;                
                default:
                    nCodeRateHigh = 0;
            }     
            
            hab[13] = (nCodeRateLow << 4) | nCodeRateHigh;
        }

        BTHD_CHK_RETCODE(BHAB_SendHabCommand(p3461->hHab, hab, 17, p3461->readBuf, 0, false, true, 17 ));
        p3461->previousAcquireParams = *pParams;
        p3461->bPowerOffToOn = false;     
    }

done:
    return retCode;

}

/******************************************************************************
  BTHD_3461_P_GetAcquireParams()
 ******************************************************************************/
BERR_Code BTHD_3461_P_GetAcquireParams(
    BTHD_Handle h,                          /* [in] BTHD handle */
    BTHD_InbandParams *pParams        /* [in] inband acquisition parameters */
    )
{
    BERR_Code retCode;
    uint8_t hab[17] = HAB_MSG_HDR(BTHD_eAcquireParamsRead, 0, BTHD_CORE_TYPE, BTHD_CORE_ID);   

    BTHD_3461_P_Handle *p3461 = (BTHD_3461_P_Handle *)(h->pImpl);

    BDBG_ENTER(BTHD_3461_P_GetAcquireParams);
    BDBG_ASSERT( h );
    BDBG_ASSERT( p3461 );
    BDBG_ASSERT( p3461->hHab);
    BDBG_ASSERT( h->magicId == DEV_MAGIC_ID );

    BKNI_Memset(p3461->readBuf, 0, sizeof(p3461->readBuf));
  
    BTHD_CHK_RETCODE(BHAB_SendHabCommand(p3461->hHab, hab, 5, p3461->readBuf, 17, false, true, 17 ));    
  
    switch(p3461->readBuf[4] & 0x3)
    {
        case 0:
            pParams->eAcquisitionMode = BTHD_ThdAcquisitionMode_eManual;
            break;
        case 1:
            pParams->eAcquisitionMode = BTHD_ThdAcquisitionMode_eAuto;
            break;            
        case 2:
            pParams->eAcquisitionMode = BTHD_ThdAcquisitionMode_eScan;
            break;             
        default:
            pParams->eAcquisitionMode = BTHD_ThdAcquisitionMode_eAuto;            
    }
    
    switch ((p3461->readBuf[4] & 0xC) >> 2)
    {
        case 0:
            pParams->bandwidth = BTHD_Bandwidth_8Mhz;
            break;
        case 1:
            pParams->bandwidth = BTHD_Bandwidth_7Mhz;
            break;
        case 2:
            pParams->bandwidth = BTHD_Bandwidth_6Mhz;
            break;
        case 3:
            pParams->bandwidth = BTHD_Bandwidth_5Mhz;
            break;
        default:
            pParams->bandwidth = BTHD_Bandwidth_8Mhz;
    }
  
    pParams->ePullinRange = (p3461->readBuf[4] & 0x10) >> 4;
    pParams->cciMode = (p3461->readBuf[4] & 0x20) >> 5;
    pParams->eModeGuardAcquire = p3461->readBuf[5] & 0x3;
      
    switch ((p3461->readBuf[5] & 0x30) >> 4)
    {
        case 0:
            pParams->eGuardInterval = BTHD_GuardInterval_e1_32;
            break;
        case 1:
            pParams->eGuardInterval = BTHD_GuardInterval_e1_16;
            break;
        case 2:
            pParams->eGuardInterval = BTHD_GuardInterval_e1_8;
            break;
        case 3:
            pParams->eGuardInterval = BTHD_GuardInterval_e1_4;
            break;
        default:
            pParams->eGuardInterval = BTHD_GuardInterval_e1_4;
    }

    switch ((p3461->readBuf[5] & 0xC) >> 2)
    {
        case 0:
            pParams->eTransmissionMode = BTHD_TransmissionMode_e2K;
            break;
        case 1:
            pParams->eTransmissionMode = BTHD_TransmissionMode_e8K;
            break;
        case 2:
            pParams->eTransmissionMode = BTHD_TransmissionMode_e4K;
            break;
        default:
            pParams->eTransmissionMode = BTHD_TransmissionMode_e2K;
    }
        
    pParams->bTuneAcquire = (p3461->readBuf[6] & 0x80) >> 7;
    pParams->bTpsAcquire = p3461->readBuf[12] & 0x1;
    pParams->decodeMode = !((p3461->readBuf[12] & 0x2) >> 1);
        
    switch ((p3461->readBuf[13] & 0x70) >> 4)
    {
        case 0:
            pParams->eCodeRateLP = BTHD_CodeRate_e1_2;
            break;
        case 1:
            pParams->eCodeRateLP = BTHD_CodeRate_e2_3;
            break;
        case 2:
            pParams->eCodeRateLP = BTHD_CodeRate_e3_4;
            break;
        case 3:
            pParams->eCodeRateLP = BTHD_CodeRate_e5_6;
            break;
        case 4:
            pParams->eCodeRateLP = BTHD_CodeRate_e7_8;
            break;                
        default:
            pParams->eCodeRateLP = BTHD_CodeRate_e1_2;
    }
    
    switch ((p3461->readBuf[12] & 0xC) >> 2)
    {
        case 0:
            pParams->eModulation = BTHD_Modulation_eQpsk;
            break;
        case 1:
            pParams->eModulation = BTHD_Modulation_e16Qam;
            break;
        case 2:
            pParams->eModulation = BTHD_Modulation_e64Qam;
            break;
        default:
            pParams->eModulation = BTHD_Modulation_e64Qam;
    }
    
    switch ((p3461->readBuf[13] & 0x30) >> 4)
    {
        case 0:
            pParams->eHierarchy = BTHD_Hierarchy_0;
            break;
        case 1:
            pParams->eHierarchy = BTHD_Hierarchy_1;
            break;
        case 2:
            pParams->eHierarchy = BTHD_Hierarchy_2;
            break;
        case 3:
            pParams->eHierarchy = BTHD_Hierarchy_4;                
            break;
        default:
            pParams->eHierarchy = BTHD_Hierarchy_0;
    }
    

    switch (p3461->readBuf[13] & 0x7)
    {        
        case 0:
            pParams->eCodeRateHP = BTHD_CodeRate_e1_2;
            break;
        case 1:
            pParams->eCodeRateHP =BTHD_CodeRate_e2_3;
            break;
        case 2:
            pParams->eCodeRateHP = BTHD_CodeRate_e3_4;
            break;
        case 3:
            pParams->eCodeRateHP = BTHD_CodeRate_e5_6;
            break;
        case 4:
            pParams->eCodeRateHP = BTHD_CodeRate_e7_8;
            break;                
        default:
            pParams->eCodeRateHP = BTHD_CodeRate_e1_2;
    }     

done:
    return retCode;

}


/******************************************************************************
  BTHD_3461_P_AcquireThd()
 ******************************************************************************/
BERR_Code BTHD_3461_P_AcquireThd(
    BTHD_Handle h,                          /* [in] BTHD handle */
    const BTHD_InbandParams *pParams        /* [in] inband acquisition parameters */
    )
{
    BERR_Code retCode;
    uint8_t hab1[5] = HAB_MSG_HDR(BTHD_eAcquire, 0, BTHD_CORE_TYPE, BTHD_CORE_ID);    

    BTHD_3461_P_Handle *p3461 = (BTHD_3461_P_Handle *)(h->pImpl);

    BDBG_ENTER(BTHD_3461_P_AcquireThd);
    BDBG_ASSERT( h );
    BDBG_ASSERT( p3461 );
    BDBG_ASSERT( p3461->hHab);
    BDBG_ASSERT( h->magicId == DEV_MAGIC_ID );
    BSTD_UNUSED( pParams );

    /* Acquire */    
    BTHD_CHK_RETCODE(BHAB_SendHabCommand(p3461->hHab, hab1, 5, p3461->readBuf, 0, false, true, 5 ));    
    BDBG_MSG(("BTHD_3461_P_AcquireThd called"));

done:
    return retCode;

}

/******************************************************************************
  BTHD_3461_P_TuneAcquire()
 ******************************************************************************/
BERR_Code BTHD_3461_P_TuneAcquire(
        BTHD_Handle h,                    /* [in] BTHD handle */
        const BTHD_InbandParams *pParams  /* [in] inband acquisition parameters */
        )
{   
    BERR_Code retCode;

    BDBG_ASSERT(h);
    BDBG_ASSERT(pParams);

    BTHD_CHK_RETCODE(BTHD_3461_P_AcquireThd(h, pParams));

done:
    return retCode;
}

/******************************************************************************
  BTHD_3461_P_RequestThdAsyncStatus()
 ******************************************************************************/
BERR_Code BTHD_3461_P_RequestThdAsyncStatus(
        BTHD_Handle h /* [in] BTHD handle */
        )
{
    BERR_Code retCode;
    uint8_t hab1[5] = HAB_MSG_HDR(BTHD_eRequestAsyncStatus, 0, BTHD_CORE_TYPE, BTHD_CORE_ID);
    BTHD_3461_P_Handle *p3461 = (BTHD_3461_P_Handle *)(h->pImpl);

    BDBG_ENTER(BTHD_3461_P_RequestThdAsyncStatus);
    BDBG_ASSERT( h );
    BDBG_ASSERT( p3461 );
    BDBG_ASSERT( p3461->hHab);
    BDBG_ASSERT( h->magicId == DEV_MAGIC_ID );
    
#ifdef BTHD_POWER_MANAGEMENT
    BDBG_MSG(("bPowerdown : %d",p3461->bPowerdown));

    if (true == p3461->bPowerdown)
        return BTHD_ERR_POWER_DOWN;
#endif

    BDBG_MSG(("BTHD_3461_P_RequestThdAsyncStatus Enter\n"));
    /* Request Status */
    BTHD_CHK_RETCODE(BHAB_SendHabCommand(p3461->hHab, hab1, 5, p3461->readBuf, 0, false, true, 5 )); 

done:
    return retCode;
}

/******************************************************************************
  BTHD_3461_P_GetThdAsyncStatus()
 ******************************************************************************/
BERR_Code BTHD_3461_P_GetThdAsyncStatus(
        BTHD_Handle h,           /* [in] BTHD handle */
        BTHD_THDStatus *pStatus  /* [out] THD status */
        )
{
    BERR_Code retCode;  
    uint8_t hab[0x45] = HAB_MSG_HDR(BTHD_eGetStatus, 0, BTHD_CORE_TYPE, BTHD_CORE_ID);  
    uint8_t codeRate, transmissionMode, guardInterval, modulation, hierarchy;
    int16_t signalStrength;
    BTHD_3461_P_Handle *p3461 = (BTHD_3461_P_Handle *)(h->pImpl);

    BDBG_ENTER(BTHD_3461_P_GetThdAsyncStatus);
    BDBG_ASSERT( h );
    BDBG_ASSERT( p3461 );
    BDBG_ASSERT( p3461->hHab);
    BDBG_ASSERT( h->magicId == DEV_MAGIC_ID );

#ifdef BTHD_POWER_MANAGEMENT
    BDBG_MSG(("bPowerdown : %d",p3461->bPowerdown));
    if (true == p3461->bPowerdown)
        return BTHD_ERR_POWER_DOWN;
#endif

    BDBG_MSG(("BTHD_3461_P_GetThdAsyncStatus Enter\n"));
    
    BKNI_Memset(p3461->readBuf, 0, sizeof(p3461->readBuf));  

    /* Get Status */
    BTHD_CHK_RETCODE(BHAB_SendHabCommand(p3461->hHab, hab, 5, p3461->readBuf, 0x45, false, true, 0x45 ));

    pStatus->bReceiverLock = (p3461->readBuf[0x5] & 0x1);
    pStatus->bFecLock = (p3461->readBuf[0x5] & 0x1);
    pStatus->ulReacqCount = (p3461->readBuf[0x8] << 8) | p3461->readBuf[0x9] ;    
    pStatus->nSnr = (p3461->readBuf[0xc] << 8) | p3461->readBuf[0xd];
    pStatus->eDecodeMode = !((p3461->readBuf[0x18] & 0x2) >> 1);
    if (pStatus->eDecodeMode == BTHD_Decode_Hp)
        codeRate = (p3461->readBuf[0x19] & 0x7);
    else
        codeRate = (p3461->readBuf[0x19] & 0x70) >> 4;
	
    switch(codeRate)
    {
        case 0:
            pStatus->eCodeRate = BTHD_CodeRate_e1_2;
            break;
        case 1:
            pStatus->eCodeRate = BTHD_CodeRate_e2_3;
            break;
        case 2:
            pStatus->eCodeRate = BTHD_CodeRate_e3_4;
            break;
        case 3:
            pStatus->eCodeRate = BTHD_CodeRate_e5_6;
            break;
        case 4:
            pStatus->eCodeRate = BTHD_CodeRate_e7_8;
            break;
        default:
            retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
            break;
    }

    transmissionMode = (p3461->readBuf[0x5] & 0x30) >> 4;

    switch(transmissionMode)
    {
        case 0:
            pStatus->eTransmissionMode = BTHD_TransmissionMode_e2K;
            break;
        case 1:
            pStatus->eTransmissionMode = BTHD_TransmissionMode_e8K;
            break;
        case 2:
            pStatus->eTransmissionMode = BTHD_TransmissionMode_e4K;
            break;
        default:
            retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
        break;
    }
    
    guardInterval = (p3461->readBuf[0x5] & 0xc0) >> 6;

    switch(guardInterval)
    {
        case 0:
            pStatus->eGuardInterval = BTHD_GuardInterval_e1_32;
            break;
        case 1:
            pStatus->eGuardInterval = BTHD_GuardInterval_e1_16;
            break;
        case 2:
            pStatus->eGuardInterval = BTHD_GuardInterval_e1_8;
            break;
        case 3:
            pStatus->eGuardInterval = BTHD_GuardInterval_e1_4;
            break;            
        default:
            retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
        break;
    }

    modulation = (p3461->readBuf[0x18] & 0xc) >> 2;
    
    switch(modulation)
    {
        case 0:
            pStatus->eModulation = BTHD_Modulation_eQpsk;
            break;
        case 1:
            pStatus->eModulation = BTHD_Modulation_e16Qam;
            break;
        case 2:
            pStatus->eModulation = BTHD_Modulation_e64Qam;
            break;         
        default:
            retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
        break;
    }
    
    hierarchy =( p3461->readBuf[0x18] & 0x30) >> 4;

    if(hierarchy == 3)
        pStatus->eHierarchy = BTHD_Hierarchy_4;
    else
        pStatus->eHierarchy = hierarchy;
    
    pStatus->lCarrierOffset = (p3461->readBuf[0x10] << 24) | (p3461->readBuf[0x11] << 16) | (p3461->readBuf[0x12] << 8) | p3461->readBuf[0x13];
    pStatus->lTimingOffset = (p3461->readBuf[0x14] << 24) | (p3461->readBuf[0x15] << 16) | (p3461->readBuf[0x16] << 8) | p3461->readBuf[0x17];
    pStatus->ulViterbiBer = (p3461->readBuf[0x24] << 24) | (p3461->readBuf[0x25] << 16) | (p3461->readBuf[0x26] << 8) | p3461->readBuf[0x27];
    pStatus->ulPreViterbiBer = (p3461->readBuf[0x20]  << 8) | ((p3461->readBuf[0x21]) << 16) | (p3461->readBuf[0x22] << 8) | p3461->readBuf[0x23];
    pStatus->ulViterbiUncorrectedBits = 0; /* Not Supported */
    pStatus->ulRsUncorrectedBlocks = (p3461->readBuf[0x3c] << 24) | (p3461->readBuf[0x3d] << 16) | (p3461->readBuf[0x3e] << 8) | p3461->readBuf[0x3f];
    pStatus->ulRsCorrectedBlocks = (p3461->readBuf[0x38] << 24) | (p3461->readBuf[0x39] << 16) | (p3461->readBuf[0x3a] << 8) | p3461->readBuf[0x3b];
    pStatus->ulRsCleanBlocks = (p3461->readBuf[0x34] << 24) | (p3461->readBuf[0x35] << 16) | (p3461->readBuf[0x36] << 8) | p3461->readBuf[0x37];
    pStatus->ulRsTotalBlocks = (p3461->readBuf[0x30] << 24) | (p3461->readBuf[0x31] << 16) | (p3461->readBuf[0x32] << 8) | p3461->readBuf[0x33];
    pStatus->bSpectrumInverted = (p3461->readBuf[0x5] & 0x8) >> 3;
    pStatus->nCellId = (p3461->readBuf[0x1c] << 8) | p3461->readBuf[0x1d];
    signalStrength= ((p3461->readBuf[0x40] << 8) | (p3461->readBuf[0x41] & 0xFF));   
    pStatus->nSignalStrength = signalStrength*100/256 + 4875;
    pStatus->signalLevelPercent = p3461->readBuf[0x42];
    pStatus->signalQualityPercent = p3461->readBuf[0x43];  
done:
    return retCode;
}

/******************************************************************************
  BTHD_3461_P_GetThdLockStatus()
 ******************************************************************************/
BERR_Code BTHD_3461_P_GetThdLockStatus(
        BTHD_Handle h,           /* [in] BTHD handle */
        BTHD_LockStatus *pLockStatus  /* [out] THD status */
        )
{
    BERR_Code retCode;       
    BTHD_3461_P_Handle *p3461 = (BTHD_3461_P_Handle *)(h->pImpl);
    uint32_t sb;
#if (BTHD_3461_VER != BCHP_VER_A0)    
    uint8_t status;
#endif    

    BDBG_ENTER(BTHD_3461_P_GetThdAsyncStatus);
    BDBG_ASSERT( h );
    BDBG_ASSERT( p3461 );
    BDBG_ASSERT( p3461->hHab);
    BDBG_ASSERT( h->magicId == DEV_MAGIC_ID );

#ifdef BTHD_POWER_MANAGEMENT
    BDBG_MSG(("bPowerdown : %d",p3461->bPowerdown));
    if (true == p3461->bPowerdown)
        return BTHD_ERR_POWER_DOWN;
#endif

#if (BTHD_3461_VER == BCHP_VER_A0)
    BTHD_CHK_RETCODE(BHAB_ReadRegister(p3461->hHab, BCHP_LEAP_CTRL_SPARE, &sb));
    *pLockStatus = (sb >> 14) & 0x1;    
#else    
    BTHD_CHK_RETCODE(BHAB_ReadRegister(p3461->hHab, BCHP_LEAP_CTRL_SW_SPARE1, &sb));
    status = sb & 0xF;
    
    switch(status)
    {
        case 1:
            *pLockStatus = BTHD_LockStatus_eLocked;
            break;
        case 0: /* work-around for FW bug */
        case 2:
            *pLockStatus = BTHD_LockStatus_eUnlocked;
            break;
        case 3:
            *pLockStatus = BTHD_LockStatus_eNoSignal;
            break;
        default:
            retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
            break;
    }
#endif

done:
    return retCode;
}

/******************************************************************************
  BTHD_3461_P_GetThdStatus()
 ******************************************************************************/
BERR_Code BTHD_3461_P_GetThdStatus(
        BTHD_Handle h,           /* [in] BTHD handle */
        BTHD_THDStatus *pStatus  /* [out] THD status */
        )
{
    BSTD_UNUSED(h);
    BSTD_UNUSED(pStatus);

    BDBG_ERR(("BTHD_GetThdStatus not supported on this platform, please use BTHD_GetThdAsyncStatus"));
    
    return BERR_TRACE(BERR_NOT_SUPPORTED);    
}

/******************************************************************************
  BTHD_3461_P_ResetInbandStatus()
 ******************************************************************************/
BERR_Code BTHD_3461_P_ResetInbandStatus(
        BTHD_Handle h /* [in] BTHD handle */
        )
{
    BERR_Code retCode;
    uint8_t hab[5] = HAB_MSG_HDR(BTHD_eResetStatus, 0, BTHD_CORE_TYPE, BTHD_CORE_ID);
    BTHD_3461_P_Handle *p3461 = (BTHD_3461_P_Handle *)(h->pImpl);

    BDBG_ENTER(BTHD_3461_P_ResetInbandStatus);
    BDBG_ASSERT( h );
    BDBG_ASSERT( p3461 );
    BDBG_ASSERT( p3461->hHab);
    BDBG_ASSERT( h->magicId == DEV_MAGIC_ID );

#ifdef BTHD_POWER_MANAGEMENT
    BDBG_MSG(("bPowerdown : %d",p3461->bPowerdown));

    if (true == p3461->bPowerdown)
        return BTHD_ERR_POWER_DOWN;
#endif

    BTHD_CHK_RETCODE(BHAB_SendHabCommand(p3461->hHab, hab, 5, p3461->readBuf, 0, false, true, 5 ));  

done:
    return retCode;
}

/******************************************************************************
  BTHD_3461_P_GetChipRevision()
 ******************************************************************************/
BERR_Code BTHD_3461_P_GetChipRevision(
        BTHD_Handle h, uint8_t* revision)
{
    BERR_Code retCode = BERR_SUCCESS;
    uint32_t familId, chipId;
    uint16_t chipRev;
    uint8_t majApVer, minApVer;
    BTHD_3461_P_Handle *p3461 = (BTHD_3461_P_Handle *)(h->pImpl);    
    
    BDBG_ENTER(BTHD_3461_P_GetChipRevision);
    BDBG_ASSERT( h );
    BDBG_ASSERT( p3461 );
    BDBG_ASSERT( p3461->hHab);
    BDBG_ASSERT( h->magicId == DEV_MAGIC_ID );

    retCode = BHAB_GetApVersion(p3461->hHab, &familId, &chipId, &chipRev, &majApVer, &minApVer);    
    *(uint16_t *)revision = chipRev;

    return retCode;
}

/******************************************************************************
  BTHD_3461_P_GetVersion()
 ******************************************************************************/
BERR_Code BTHD_3461_P_GetVersion(
        BTHD_Handle h, uint32_t* revision, uint32_t* checksum)
{
    BERR_Code retCode = BERR_SUCCESS;
    uint32_t familId, chipId;
    uint16_t chipRev;
    uint8_t majApVer, minApVer;
    BTHD_3461_P_Handle *p3461 = (BTHD_3461_P_Handle *)(h->pImpl);    
    
    BDBG_ENTER(BTHD_3461_P_GetVersion);
    BDBG_ASSERT( h );
    BDBG_ASSERT( p3461 );
    BDBG_ASSERT( p3461->hHab);
    BDBG_ASSERT( h->magicId == DEV_MAGIC_ID );
    BSTD_UNUSED(checksum);

    retCode = BHAB_GetApVersion(p3461->hHab, &familId, &chipId, &chipRev, &majApVer, &minApVer);
    *revision = (majApVer << 8) | minApVer;

    return retCode;
}

/******************************************************************************
  BTHD_3461_P_GetVersionInfo()
 ******************************************************************************/
BERR_Code BTHD_3461_P_GetVersionInfo(
    BTHD_Handle h,                 /* [in] BTHD handle  */
    BFEC_VersionInfo *pVersionInfo /* [out] Returns version Info */
)
{
    BERR_Code retCode = BERR_SUCCESS;
    uint8_t hab[29] = HAB_MSG_HDR(BTHD_eGetVersionInfo, 0, BTHD_CORE_TYPE, BTHD_CORE_ID);
    BTHD_3461_P_Handle *p3461 = (BTHD_3461_P_Handle *)(h->pImpl);

    BDBG_ENTER(BTHD_3461_P_ResetInbandStatus);
    BDBG_ASSERT( h );
    BDBG_ASSERT( p3461 );
    BDBG_ASSERT( p3461->hHab);
    BDBG_ASSERT( h->magicId == DEV_MAGIC_ID );

    BTHD_CHK_RETCODE(BHAB_SendHabCommand(p3461->hHab, hab, 5, p3461->readBuf, 29, false, true, 29 ));  
    pVersionInfo->majorVersion = (p3461->readBuf[4] << 8) | p3461->readBuf[5];
    pVersionInfo->minorVersion = (p3461->readBuf[6] << 8) | p3461->readBuf[7];
    pVersionInfo->buildType = (p3461->readBuf[8] << 8) | p3461->readBuf[9];
    pVersionInfo->buildId = (p3461->readBuf[10] << 8) | p3461->readBuf[11];
    
done:
    return retCode;
}

/******************************************************************************
  BTHD_3461_P_GetInterruptEventHandle()
 ******************************************************************************/
BERR_Code BTHD_3461_P_GetInterruptEventHandle(BTHD_Handle h, BKNI_EventHandle* hEvent)
{
    *hEvent = ((BTHD_3461_P_Handle *)(h->pImpl))->hInterruptEvent;
    return BERR_SUCCESS;
}

/******************************************************************************
  BTHD_3461_P_ProcessInterruptEvent()
 ******************************************************************************/
BERR_Code BTHD_3461_P_ProcessInterruptEvent(BTHD_Handle h)
{
    BSTD_UNUSED(h);
    return BERR_SUCCESS;
}

/******************************************************************************
  BTHD_3461_CheckStatus()
 ******************************************************************************/
BERR_Code BTHD_3461_CheckStatus(uint8_t status)
{
    BSTD_UNUSED(status);
    return BERR_SUCCESS;
}

/******************************************************************************
  BTHD_3461_P_GetSoftDecisionBuf()
 ******************************************************************************/
BERR_Code BTHD_3461_P_GetSoftDecisionBuf( 
    BTHD_Handle h,  /* [in] BTHD handle */
    int16_t *pI,    /* [out] 30 I-values */
    int16_t *pQ     /* [out] 30 Q-values */
    )
{
   BERR_Code retCode = BERR_SUCCESS;
   uint8_t idx;
   uint8_t hab[125] = HAB_MSG_HDR(BTHD_eGetConstellation, 0x0, BTHD_CORE_TYPE, BTHD_CORE_ID);      
   BTHD_3461_P_Handle *p3461 = (BTHD_3461_P_Handle *)(h->pImpl);

   BDBG_ENTER(BTHD_3461_P_GetSoftDecisionBuf);
   BDBG_ASSERT( h );
   BDBG_ASSERT( p3461 );
   BDBG_ASSERT( p3461->hHab);
   BDBG_ASSERT( h->magicId == DEV_MAGIC_ID );

#ifdef BTHD_POWER_MANAGEMENT
   if (true == p3461->bPowerdown)
       return BTHD_ERR_POWER_DOWN;
#endif

    BKNI_Memset(p3461->readBuf, 0, sizeof(p3461->readBuf));
    
    BTHD_CHK_RETCODE(BHAB_SendHabCommand(p3461->hHab, hab, 5, p3461->readBuf, 125, false, true, 125));  

    for (idx = 0;idx < 30; idx++ )
    {
        pI[idx] = (p3461->readBuf[4+(4*idx)] << 8) | p3461->readBuf[5+(4*idx)];
        pQ[idx] = (p3461->readBuf[6+(4*idx)] << 8) | p3461->readBuf[7+(4*idx)];
    }

done:
   return retCode;
}

/******************************************************************************
  BTHD_3461_P_GetLockStateChangeEvent()
 ******************************************************************************/
BERR_Code BTHD_3461_P_GetLockStateChangeEvent(BTHD_Handle h, BKNI_EventHandle* hEvent)
{
    *hEvent = ((BTHD_3461_P_Handle *)(h->pImpl))->hLockChangeEvent; 
    return BERR_SUCCESS;
}

/******************************************************************************
  BTHD_3461_P_GetLockEvent()
 ******************************************************************************/
BERR_Code BTHD_3461_P_GetLockEvent(BTHD_Handle h, BKNI_EventHandle* hEvent)
{
    *hEvent = ((BTHD_3461_P_Handle *)(h->pImpl))->hLockEvent;   
    return BERR_SUCCESS;
}

/******************************************************************************
  BTHD_3461_P_GetUnlockEvent()
 ******************************************************************************/
BERR_Code BTHD_3461_P_GetUnlockEvent(BTHD_Handle h, BKNI_EventHandle* hEvent)
{
    *hEvent = ((BTHD_3461_P_Handle *)(h->pImpl))->hUnlockEvent; 
    return BERR_SUCCESS;
}

/******************************************************************************
  BTHD_3461_P_InstallCallback()
 ******************************************************************************/
BERR_Code BTHD_3461_P_InstallCallback(
    BTHD_Handle h,                      /* [in] Device channel handle */
    BTHD_Callback callbackType,         /* [in] Type of callback */
    BTHD_CallbackFunc pCallback,        /* [in] Function Ptr to callback */
    void *pParam                        /* [in] Generic parameter send on callback */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTHD_3461_P_Handle *p3461 = (BTHD_3461_P_Handle *)(h->pImpl);    

    BDBG_ENTER(BTHD_3461_P_InstallCallback);
    BDBG_ASSERT( h );
    BDBG_ASSERT( p3461 );
    BDBG_ASSERT( p3461->hHab);
    BDBG_ASSERT( h->magicId == DEV_MAGIC_ID );

    switch( callbackType )
    {
        case BTHD_Callback_eLockChange:
            p3461->pCallback[callbackType] = pCallback;
            p3461->pCallbackParam[callbackType] = pParam;
            break;
        case BTHD_Callback_eUpdateGain:
            p3461->pCallback[callbackType] = pCallback;
            p3461->pCallbackParam[callbackType] = pParam;
            break;
        case BTHD_Callback_eNoSignal:
            p3461->pCallback[callbackType] = pCallback;
            p3461->pCallbackParam[callbackType] = pParam;
            break;
        case BTHD_Callback_eAsyncStatusReady:
            p3461->pCallback[callbackType] = pCallback;
            p3461->pCallbackParam[callbackType] = pParam;
            break;
        default:
            retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
            break;
    }

    BHAB_InstallInterruptCallback( p3461->hHab,   p3461->devId, BTHD_3461_P_EventCallback_isr , (void *)h, callbackType);

    BDBG_LEAVE(BTHD_3461_P_InstallCallback);
    return retCode;
}
