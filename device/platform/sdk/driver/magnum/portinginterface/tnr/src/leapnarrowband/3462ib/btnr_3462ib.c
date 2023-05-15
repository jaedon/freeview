/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btnr_3462ib.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 12/4/12 4:08p $
 *
 * Module Description:
 *
 * Revision History: $
 *
 * $brcm_Log: /magnum/portinginterface/tnr/src/leapnarrowband/3462ib/btnr_3462ib.c $
 * 
 * Hydra_Software_Devel/4   12/4/12 4:08p atanugul
 * SW3462-51: Add Support for GetVersionInfo() API
 * 
 * Hydra_Software_Devel/3   9/7/12 9:50a atanugul
 * SW7425-3765: restructure TNR directory
 * 
 * Hydra_Software_Devel/2   7/11/12 5:20p atanugul
 * SW3472-8: Add support for 3472
 * 
 * Hydra_Software_Devel/1   4/10/12 11:09a atanugul
 * SW3462-4: Add TNR PI support for 3462
 * 
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "btnr.h"
#include "btnr_priv.h"
#include "btnr_3462ib.h"
#include "btnr_34xxib_priv.h"

BDBG_MODULE(btnr_3462);

#define DEV_MAGIC_ID                ((BERR_TNR_ID<<16) | 0xFACE)
#define BTNR_3462_SETTINGS_I2CADDR  (0x6C)          /* 7bit addr */
#define BTNR_3462_MAX_CHANNELS      1

/*******************************************************************************/
BERR_Code BTNR_3462_Open(
    BTNR_Handle *phDev,                 /* [output] Returns handle */
    BTNR_3462_Settings *pSettings, /* [Input] settings structure */
    BHAB_Handle hHab                  /* [Input] Hab Handle */   
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTNR_34xx_Handle h34xxDev;
    BTNR_P_34xx_Settings *pTnrImplData;
    BTNR_Handle hDev;

    BDBG_ENTER(BTNR_34xx_Open);
    BDBG_ASSERT( hHab );
    
    hDev = NULL;
    /* Alloc memory from the system heap */
    h34xxDev = (BTNR_34xx_Handle) BKNI_Malloc( sizeof( BTNR_P_34xx_Handle ) );
    if( h34xxDev == NULL )
    {
        retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        BDBG_ERR(("BTNR_3462_Open: BKNI_malloc() failed\n"));
        goto done;
    }
    BKNI_Memset( h34xxDev, 0x00, sizeof( BTNR_P_34xx_Handle ) );

    hDev = (BTNR_Handle) BKNI_Malloc( sizeof( BTNR_P_Handle ) );
    if( hDev == NULL )
    {
        retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        BDBG_ERR(("BTNR_3462_Open: BKNI_malloc() failed\n"));
        BKNI_Free( h34xxDev );
        goto done;
    }
    BKNI_Memset( hDev, 0x00, sizeof( BTNR_P_Handle ) );

    h34xxDev->magicId = DEV_MAGIC_ID;
    pTnrImplData = &h34xxDev->settings;
    pTnrImplData->rfFreq = 0;
    pTnrImplData->tunerMode = BTNR_TunerMode_eLast;
    h34xxDev->channelNo = pSettings->channelNo;
    h34xxDev->hHab = hHab;
    hDev->hDevImpl = (void *) h34xxDev;
    hDev->magicId = DEV_MAGIC_ID;
    hDev->pSetRfFreq = (BTNR_SetRfFreqFunc) BTNR_34xx_SetRfFreq;
    hDev->pGetRfFreq = (BTNR_GetRfFreqFunc) BTNR_34xx_GetRfFreq;
    hDev->pGetAgcRegVal = (BTNR_GetAgcRegValFunc) BTNR_P_34xx_GetAgcRegVal;
    hDev->pSetAgcRegVal = (BTNR_SetAgcRegValFunc) BTNR_34xx_SetAgcRegVal;
    hDev->pGetInfo = (BTNR_GetInfoFunc) BTNR_34xx_GetInfo;
    hDev->pClose = (BTNR_CloseFunc) BTNR_34xx_Close;
    hDev->pGetPowerSaver = (BTNR_GetPowerSaverFunc) BTNR_34xx_GetPowerSaver;
    hDev->pSetPowerSaver = (BTNR_SetPowerSaverFunc) BTNR_34xx_SetPowerSaver;
    hDev->pGetSettings = (BTNR_GetSettingsFunc) BTNR_34xx_GetSettings;
    hDev->pSetSettings = (BTNR_SetSettingsFunc) BTNR_34xx_SetSettings;
    hDev->pVersionInfo = (BTNR_GetVersionInfoFunc) BTNR_34xx_GetVersionInfo;
    
    if(pSettings->channelNo >= pSettings->mxChnNo)
    {
        retCode = BERR_TRACE(BERR_NOT_SUPPORTED); 
        BDBG_ERR(("Only %d TNR channels supported", pSettings->mxChnNo)); 
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
    BDBG_LEAVE(BTNR_3462_Open);
    return( retCode );
}

BERR_Code BTNR_3462_GetDefaultSettings(
    BTNR_3462_Settings *pDefSettings  /* [out] Returns default setting */
    )
{
    BDBG_ASSERT(NULL != pDefSettings);

    BKNI_Memset(pDefSettings, 0, sizeof(*pDefSettings));
    pDefSettings->i2cAddr = BTNR_3462_SETTINGS_I2CADDR;
    pDefSettings->mxChnNo = BTNR_3462_MAX_CHANNELS;    

    return BERR_SUCCESS;
}

