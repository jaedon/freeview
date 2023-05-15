/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btnr_3128ib.c $
 * $brcm_Revision: Hydra_Software_Devel/10 $
 * $brcm_Date: 9/28/12 3:06p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/tnr/src/leapwideband/3128ib/btnr_3128ib.c $
 * 
 * Hydra_Software_Devel/10   9/28/12 3:06p atanugul
 * SW3128-228: Add support for BTNR_GetVersionInfo()
 * 
 * Hydra_Software_Devel/9   3/30/12 6:08p atanugul
 * SW3128-140: Add Spectrum Analyzer feature to the TNR PI
 * 
 * Hydra_Software_Devel/8   3/30/12 12:08p atanugul
 * SW3128-140: Add Spectrum Analyzer feature to the TNR PI
 * 
 * Hydra_Software_Devel/7   12/22/11 3:37p atanugul
 * SW3128-100: Check for power status before sending HAB commands
 * 
 * Hydra_Software_Devel/6   12/16/11 12:02p atanugul
 * SW3128-92: Add  FW auto detection support
 * 
 * Hydra_Software_Devel/5   12/12/11 5:31p atanugul
 * SW3128-92: Add support to auto detect 3123/4/8 and restrict features
 * based on the chipId
 * 
 * Hydra_Software_Devel/4   8/30/11 12:49p atanugul
 * SW3128-47: Add Support for BCM3123
 * 
 * Hydra_Software_Devel/3   4/11/11 5:15p atanugul
 * SW3128-2: Add Get_Settings and SetSettings support to 3128 TNR PI
 * 
 * Hydra_Software_Devel/2   3/4/11 3:18p vishk
 * SW7422-171: Nexus support for the 3128 (8 ADS core + tuner on daughter
 * board) plug on 7422 board
 * 
 * Hydra_Software_Devel/1   2/1/11 7:37a atanugul
 * SW3128-2: Add Support for BCM3128
 * 
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "btnr.h"
#include "btnr_priv.h"
#include "btnr_3128ib.h"
#include "btnr_312xib_priv.h"

BDBG_MODULE(btnr_3128ib);

#define DEV_MAGIC_ID                    ((BERR_TNR_ID<<16) | 0xFACE)
#define BTNR_3128Ib_SETTINGS_I2CADDR    (0x66)          /* 7bit addr */

/*******************************************************************************/
BERR_Code BTNR_3128Ib_Open(
    BTNR_Handle *phDev,                 /* [output] Returns handle */
    BTNR_3128Ib_Settings *pSettings, /* [Input] settings structure */
    BHAB_Handle hHab                  /* [Input] Hab Handle */   
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTNR_312xIb_Handle h312xIbDev;
    BTNR_P_312xIb_Settings *pTnrImplData;
    BTNR_Handle hDev;  
    uint16_t chipVer;    
    uint32_t familyId, chipId;    
    uint8_t apVer, minApVer;   

    BDBG_ENTER(BTNR_312xIb_Open);
    BDBG_ASSERT( hHab );
    
    hDev = NULL;
    /* Alloc memory from the system heap */
    h312xIbDev = (BTNR_312xIb_Handle) BKNI_Malloc( sizeof( BTNR_P_312xIb_Handle ) );
    if( h312xIbDev == NULL )
    {
        retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        BDBG_ERR(("BTNR_312xIb_Open: BKNI_malloc() failed\n"));
        goto done;
    }
    BKNI_Memset( h312xIbDev, 0x00, sizeof( BTNR_P_312xIb_Handle ) );

    hDev = (BTNR_Handle) BKNI_Malloc( sizeof( BTNR_P_Handle ) );
    if( hDev == NULL )
    {
        retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        BDBG_ERR(("BTNR_312xIb_Open: BKNI_malloc() failed\n"));
        BKNI_Free( h312xIbDev );
        goto done;
    }
    BKNI_Memset( hDev, 0x00, sizeof( BTNR_P_Handle ) );

    h312xIbDev->magicId = DEV_MAGIC_ID;
    pTnrImplData = &h312xIbDev->settings;
    pTnrImplData->rfFreq = 0;
    pTnrImplData->tunerMode = BTNR_TunerMode_eLast;
    h312xIbDev->channelNo = pSettings->channelNo;
    h312xIbDev->bPowerdown = true;

    h312xIbDev->hHab = hHab;
    hDev->hDevImpl = (void *) h312xIbDev;
    hDev->magicId = DEV_MAGIC_ID;
    h312xIbDev->devId = BHAB_DevId_eTNR0; /* Here the device id is always defaulted to channel 0. */    
    hDev->pSetRfFreq = (BTNR_SetRfFreqFunc) BTNR_312xIb_SetRfFreq;
    hDev->pGetRfFreq = (BTNR_GetRfFreqFunc) BTNR_312xIb_GetRfFreq;
    hDev->pGetAgcRegVal = (BTNR_GetAgcRegValFunc) BTNR_P_312xIb_GetAgcRegVal;
    hDev->pSetAgcRegVal = (BTNR_SetAgcRegValFunc) BTNR_312xIb_SetAgcRegVal;
    hDev->pGetInfo = (BTNR_GetInfoFunc) BTNR_312xIb_GetInfo;
    hDev->pClose = (BTNR_CloseFunc) BTNR_312xIb_Close;
    hDev->pGetPowerSaver = (BTNR_GetPowerSaverFunc) BTNR_312xIb_GetPowerSaver;
    hDev->pSetPowerSaver = (BTNR_SetPowerSaverFunc) BTNR_312xIb_SetPowerSaver;
    hDev->pGetSettings = (BTNR_GetSettingsFunc) BTNR_312xIb_GetSettings;  
    hDev->pSetSettings = (BTNR_SetSettingsFunc) BTNR_312xIb_SetSettings;
    hDev->pRequestSpectrumAnalyzerData = (BTNR_RequestSpectrumAnalyzerDataFunc) BTNR_312xIb_RequestSpectrumAnalyzerData;    
    hDev->pGetSpectrumAnalyzerData = (BTNR_GetSpectrumAnalyzerDataFunc) BTNR_312xIb_GetSpectrumAnalyzerData;    
    hDev->pInstallCallback = (BTNR_InstallCallbackFunc) BTNR_312xIb_InstallCallback;    
    hDev->pVersionInfo = (BTNR_GetVersionInfoFunc) BTNR_312xIb_GetVersionInfo;    

    retCode = BHAB_GetApVersion(h312xIbDev->hHab, &familyId, &chipId, &chipVer, &apVer, &minApVer);
    
    if((chipId == 0x00) && (familyId == 0x3128))
        chipId = 0x3128;
        
    h312xIbDev->mxChnNo = chipId & 0xF;
    
    if(pSettings->channelNo >= h312xIbDev->mxChnNo)
    {
        retCode = BERR_TRACE(BERR_NOT_SUPPORTED); 
        BDBG_ERR(("Only %d TNR channels supported on on %x", h312xIbDev->mxChnNo, chipId)); 
    }
    else
        *phDev = hDev;

done:
    if( retCode != BERR_SUCCESS )
    {
        if( hDev != NULL )
        {
            BKNI_Free( hDev );
        }
        *phDev = NULL;
    }     
    BDBG_LEAVE(BTNR_312xIb_Open);
    return( retCode );  
}

BERR_Code BTNR_3128Ib_GetDefaultSettings(
    BTNR_3128Ib_Settings *pDefSettings  /* [out] Returns default setting */
    )
{
    BDBG_ASSERT(NULL != pDefSettings);

    BKNI_Memset(pDefSettings, 0, sizeof(*pDefSettings));
    pDefSettings->i2cAddr = BTNR_3128Ib_SETTINGS_I2CADDR;

    return BERR_SUCCESS;
}

