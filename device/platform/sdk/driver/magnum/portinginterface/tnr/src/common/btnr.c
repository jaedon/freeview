/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btnr.c $
 * $brcm_Revision: Hydra_Software_Devel/26 $
 * $brcm_Date: 10/18/12 3:42p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/tnr/src/common/btnr.c $
 * 
 * Hydra_Software_Devel/26   10/18/12 3:42p atanugul
 * SW3128-228: Add API Support for Reading New Front-End 312X, 314X, 346X
 * F/W Version IDs
 * 
 * Hydra_Software_Devel/24   9/28/12 3:06p atanugul
 * SW3128-228: Add support for BTNR_GetVersionInfo()
 * 
 * Hydra_Software_Devel/23   4/9/12 2:57p atanugul
 * SW3128-140: Add Spectrum Analyzer feature to the TNR PI
 * 
 * Hydra_Software_Devel/22   4/6/12 1:23p mward
 * SW3128-140: Compiler warning for newline.
 * 
 * Hydra_Software_Devel/22   4/6/12 11:58a mward
 * SW3128-140: Compiler warning for newline.
 * 
 * Hydra_Software_Devel/22   4/6/12 11:39a mward
 * SW3128-140: Compiler warning for newline.
 * 
 * Hydra_Software_Devel/22   4/6/12 11:26a mward
 * SW3128-140: Compiler warning for newline.
 * 
 * Hydra_Software_Devel/21   3/30/12 12:07p atanugul
 * SW3128-140: Add Spectrum Analyzer feature to the TNR PI
 * 
 * Hydra_Software_Devel/20   5/4/11 4:20p atanugul
 * SW7231-32: Reverting back to version 18
 * 
 * Hydra_Software_Devel/19   5/3/11 3:19p vishk
 * SW7231-32: Implement the Nexus code to support the 3461 frontend chip.
 * Undo the changes done since version 16.
 * 
 * Hydra_Software_Devel/18   4/19/11 10:07a atanugul
 * SW3461-3: Fix Compile Warning
 * 
 * Hydra_Software_Devel/17   4/11/11 5:15p atanugul
 * SW3128-2: Add Get_Settings and SetSettings support to 3128 TNR PI
 * 
 * Hydra_Software_Devel/16   6/22/10 6:13p vishk
 * SW7420-839: Add support to program AGC gain to 31xx TNR PI
 * 
 * Hydra_Software_Devel/15   11/17/09 7:59p farshidf
 * SW7550-38: update the code to the latest nexus call
 * 
 * Hydra_Software_Devel/14   11/10/09 7:11p farshidf
 * 7550-38: add the tuner code
 * 
 * Hydra_Software_Devel/13   8/24/09 6:05p vishk
 * SW7405-2841: Bcm31xx: Add Power Management support for TNR
 * 
 * Hydra_Software_Devel/12   8/21/09 6:19p vishk
 * PR 57484: Bcm31xx: Add Power Management support for TNR
 * 
 * Hydra_Software_Devel/11   6/7/05 8:35a dlwin
 * PR 14682: Added additional functions for Docsis and general use.  New
 * function 'BTNR_GetInfo()' returns additional informtion for the tuner.
 * 
 * Hydra_Software_Devel/10   4/1/05 5:25p dlwin
 * PR 14682: Added semi-private functions for QamCm to use to AGC
 * readings.  This required for Docsis integration.
 * 
 * Hydra_Software_Devel/9   10/21/03 6:19p dlwin
 * Removed all BDBG_ENTER(__FUNCTION__) and BDBG_LEAVE(__FUNCTION__) with
 * macro usage of function, instead of __FUNCTION__.
 * 
 * Hydra_Software_Devel/8   10/7/03 8:52a dlwin
 * Removed the need to call a create generic tuner  handle.
 * 
 * Hydra_Software_Devel/7   9/9/03 8:44a dlwin
 * Updated API function parameter comment for DocJet.
 * 
 * Hydra_Software_Devel/6   8/29/03 7:31a dlwin
 * - Removed reference to 'BFEH'
 * - Renamed 'BTNR_Set/GetTunerFreq()' to 'BTNR_Set/GetTunerRfFreq()'.
 * 
 * Hydra_Software_Devel/5   8/26/03 2:23p dlwin
 * Updated to used newer API, which is model like BREG_I2C and BREG
 * 
 * Hydra_Software_Devel/4   7/10/03 10:44a dlwin
 * Fixed a problem Open fails.
 * 
 * Hydra_Software_Devel/3   7/2/03 3:11p dlwin
 * Fixed with OpenChannel().
 * 
 * Hydra_Software_Devel/2   6/26/03 11:11a dlwin
 * Removed reference to 'bchp_7120.h' and 'bchp_3250.h'.
 * 
 * Hydra_Software_Devel/1   6/25/03 4:55p dlwin
 * Initial version
 * 
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "btnr.h"
#include "btnr_priv.h"

BDBG_MODULE(btnr);


#define DEV_MAGIC_ID            ((BERR_TNR_ID<<16) | 0xFACE)

#define CHK_RETCODE( rc, func )     \
do {                                        \
    if( (rc = BERR_TRACE(func)) != BERR_SUCCESS ) \
    {                                       \
        goto done;                          \
    }                                       \
} while(0)


/*******************************************************************************
*
*   Private Module Handles
*
*******************************************************************************/



/*******************************************************************************
*
*   Default Module Settings
*
*******************************************************************************/



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



/*******************************************************************************
*
*   Public Module Functions
*
*******************************************************************************/
BERR_Code BTNR_SetTunerRfFreq(
    BTNR_Handle hDev,                   /* [in] Device handle */
    uint32_t freq,                      /* [in] Requested tuner freq., in Hertz */
    BTNR_TunerMode tunerMode            /* [in] Requested tuner mode */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ENTER(BTNR_SetTunerRfFreq);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
    
    if( hDev->pSetRfFreq != NULL )
    {
        CHK_RETCODE( retCode, hDev->pSetRfFreq( hDev->hDevImpl, freq, tunerMode ) );
    }

done:
    BDBG_LEAVE(BTNR_SetTunerRfFreq);
    return( retCode );
}

BERR_Code BTNR_GetTunerRfFreq(
    BTNR_Handle hDev,                   /* [in] Device handle */
    uint32_t *freq,                     /* [output] Returns tuner freq., in Hertz */
    BTNR_TunerMode *tunerMode           /* [output] Returns tuner mode */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ENTER(BTNR_GetTunerRfFreq);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    if( hDev->pGetRfFreq != NULL )
    {
        CHK_RETCODE( retCode, hDev->pGetRfFreq( hDev->hDevImpl, freq, tunerMode ) );
    }
    else
    {
        *freq = 0;
        *tunerMode = BTNR_TunerMode_eDigital;
    }

done:
    BDBG_LEAVE(BTNR_GetTunerRfFreq);
    return( retCode );
}

BERR_Code BTNR_GetInfo(
    BTNR_Handle hDev,                   /* [in] Device handle */
    BTNR_TunerInfo *tnrInfo             /* [out] Tuner information */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ENTER(BTNR_GetInfo);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    BKNI_Memset( tnrInfo, 0x00, sizeof( BTNR_TunerInfo ) );
    if( hDev->pGetInfo != NULL )
    {
        CHK_RETCODE( retCode, hDev->pGetInfo( hDev->hDevImpl, tnrInfo ) );
    }

done:
    BDBG_LEAVE(BTNR_GetInfo);
    return( retCode );
}

BERR_Code BTNR_Close(
    BTNR_Handle hDev                    /* [in] Device handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ENTER(BTNR_Close);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    if( hDev->pClose != NULL )
    {
        CHK_RETCODE( retCode, hDev->pClose( hDev ) );
    }

done:
    BDBG_LEAVE(BTNR_Close);
    return( retCode );
}

BERR_Code BTNR_P_GetTunerAgcRegVal(
    BTNR_Handle hDev,                   /* [in] Device handle */
    uint32_t regOffset,                 /* [in] AGC register offset */
    uint32_t *agcVal                    /* [out] output value */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ENTER(BTNR_P_GetTunerAgcRegVal);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    if( hDev->pGetAgcRegVal != NULL )
    {
        CHK_RETCODE( retCode, hDev->pGetAgcRegVal( hDev->hDevImpl, regOffset, agcVal ) );
    }
    else
    {
        *agcVal = 0;
        retCode = BERR_NOT_SUPPORTED;
    }

done:
    BDBG_LEAVE(BTNR_P_GetTunerAgcRegVal);
    return( retCode );
}

BERR_Code BTNR_SetTunerAgcRegVal(
    BTNR_Handle hDev,                   /* [in] Device handle */
    uint32_t regOffset,                 /* [in] AGC register offset */
    uint32_t *agcVal                    /* [in] input value */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ENTER(BTNR_SetTunerAgcRegVal);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    if( hDev->pSetAgcRegVal != NULL )
    {
        CHK_RETCODE( retCode, hDev->pSetAgcRegVal( hDev->hDevImpl, regOffset, agcVal ) );
    }
    else
    {
        retCode = BERR_NOT_SUPPORTED;
    }

done:
    BDBG_LEAVE(BTNR_P_GetTunerAgcRegVal);
    return( retCode );
}

BERR_Code BTNR_GetPowerSaver(
    BTNR_Handle hDev,                   /* [in] Device handle */
    BTNR_PowerSaverSettings *pwrSettings /* [out] Power saver settings. */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BTNR_GetPowerSaver);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
    
    if( hDev->pGetPowerSaver != NULL )
    {
        CHK_RETCODE( retCode, hDev->pGetPowerSaver( hDev->hDevImpl, pwrSettings ) );
    }
    else {
        retCode = BERR_INVALID_PARAMETER;
    }

done:
    BDBG_LEAVE(BTNR_GetPowerSaver);
    return( retCode );
}

BERR_Code BTNR_SetPowerSaver(
    BTNR_Handle hDev,                   /* [in] Device handle */
    BTNR_PowerSaverSettings *pwrSettings /* [in] Power saver settings. */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BTNR_SetPowerSaver);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
    
    if( hDev->pSetPowerSaver != NULL )
    {
        CHK_RETCODE( retCode, hDev->pSetPowerSaver( hDev->hDevImpl, pwrSettings ) );
    }
    else {
        retCode = BERR_NOT_SUPPORTED;
    }

done:
    BDBG_LEAVE(BTNR_SetPowerSaver);
    return( retCode );
}

BERR_Code BTNR_GetSettings(
    BTNR_Handle hDev,           /* [in] Device handle */
    BTNR_Settings *settings     /* [out] TNR settings. */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BTNR_GetSettings);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
    
    if( hDev->pGetSettings != NULL )
    {
        CHK_RETCODE( retCode, hDev->pGetSettings( hDev->hDevImpl, settings ) );
    }
    else {
        retCode = BERR_INVALID_PARAMETER;
    }

done:
    BDBG_LEAVE(BTNR_GetSettings);
    return( retCode );
}

BERR_Code BTNR_SetSettings(
    BTNR_Handle hDev,           /* [in] Device handle */
    BTNR_Settings *settings     /* [in] TNR settings. */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BTNR_SetSettings);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
    
    if( hDev->pSetSettings != NULL )
    {
        CHK_RETCODE( retCode, hDev->pSetSettings( hDev->hDevImpl, settings ) );
    }
    else {
        retCode = BERR_NOT_SUPPORTED;
    }

done:
    BDBG_LEAVE(BTNR_SetSettings);
    return( retCode );
}

BERR_Code BTNR_RequestSpectrumAnalyzerData(
    BTNR_Handle hDev,     /* [in] Device handle */ 
    BTNR_SpectrumSettings *pSettings /* [in] spectrum settings */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BTNR_RequestSpectrumAnalyzerData);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
    
    if( hDev->pRequestSpectrumAnalyzerData != NULL )
    {
        CHK_RETCODE( retCode, hDev->pRequestSpectrumAnalyzerData( hDev->hDevImpl, pSettings ) );
    }
    else {
        retCode = BERR_NOT_SUPPORTED;
    }

done:
    BDBG_LEAVE(BTNR_RequestSpectrumAnalyzerData);
    return( retCode );
}

BERR_Code BTNR_GetSpectrumAnalyzerData(
    BTNR_Handle hDev,     /* [in] Device handle */ 
    BTNR_SpectrumData  *pSpectrumData /* [out] spectrum Data*/
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BTNR_GetSpectrumAnalyzerData);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
    
    if( hDev->pGetSpectrumAnalyzerData != NULL )
    {
        CHK_RETCODE( retCode, hDev->pGetSpectrumAnalyzerData( hDev->hDevImpl, pSpectrumData ) );
    }
    else {
        retCode = BERR_NOT_SUPPORTED;
    }

done:
    BDBG_LEAVE(BTNR_GetSpectrumAnalyzerData);
    return( retCode );
}

BERR_Code BTNR_InstallCallback(
    BTNR_Handle hDev,     /* [in] Device handle */
    BTNR_Callback callbackType, /* [in] Type of callback */
    BTNR_CallbackFunc pCallback_isr, /* [in] Function Ptr to callback */
    void *pParam                 /* [in] Generic parameter send on callback */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BTNR_InstallCallback);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
    
    if( hDev->pInstallCallback != NULL )
    {
        CHK_RETCODE( retCode, hDev->pInstallCallback( hDev->hDevImpl, callbackType, pCallback_isr, pParam ) );
    }
    else {
        retCode = BERR_NOT_SUPPORTED;
    }

done:
    BDBG_LEAVE(BTNR_InstallCallback);
    return( retCode );
}

BERR_Code BTNR_GetVersionInfo(
    BTNR_Handle hDev,              /* [in] Device handle */
    BFEC_VersionInfo *pVersionInfo /* [out] Returns version Info */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BTNR_GetVersionInfo);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
    
    if( hDev->pVersionInfo != NULL )
    {
        CHK_RETCODE( retCode, hDev->pVersionInfo( hDev->hDevImpl, pVersionInfo ) );
    }
    else {
        retCode = BERR_NOT_SUPPORTED;
    }

done:
    BDBG_LEAVE(BTNR_GetVersionInfo);
    return( retCode );
}    
    