/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bads.c $
 * $brcm_Revision: Hydra_Software_Devel/24 $
 * $brcm_Date: 10/18/12 3:42p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ads/src/common/bads.c $
 * 
 * Hydra_Software_Devel/24   10/18/12 3:42p atanugul
 * SW3128-228: Add API Support for Reading New Front-End 312X, 314X, 346X
 * F/W Version IDs
 * 
 * Hydra_Software_Devel/22   9/28/12 2:17p atanugul
 * SW3128-228: Add support for  BADS_GetVersionInfo()
 * 
 * Hydra_Software_Devel/21   2/14/12 10:42a atanugul
 * SW3128-119: Update SetAcquireParams API to make the second parameter as
 * const
 * 
 * Hydra_Software_Devel/20   2/8/12 5:14p atanugul
 * SW3128-119: Implement new apis for SetAcquireParams, GetAcquireParams
 * and GetDefaultAcquireParams
 * 
 * Hydra_Software_Devel/19   12/8/11 3:12p atanugul
 * SW3128-89: Add PI support for Spectrum Analyzer Data retrieval
 * 
 * Hydra_Software_Devel/18   11/16/11 4:23p atanugul
 * SW3461-79: Add No Signal to GetLockStatus()
 * 
 * Hydra_Software_Devel/17   9/15/11 3:19p atanugul
 * SW3128-50: Add BADS_GetScanStatus()
 * 
 * Hydra_Software_Devel/16   8/31/11 5:41p nickh
 * SW7425-1129: Expose 3255 RPC call that returns the number of bonded
 * channels reserved for Docsis
 * 
 * Hydra_Software_Devel/15   5/6/11 4:23p farshidf
 * SWDTV-6857: waring fix
 * 
 * Hydra_Software_Devel/14   5/6/11 2:22p farshidf
 * SWDTV-6857: merge main
 * 
 * Hydra_Software_Devel/SWDTV-6857/1   5/6/11 2:00p farshidf
 * SWDTV-6857: add the scan structure and Calls
 * 
 * Hydra_Software_Devel/13   11/24/10 8:07p agin
 * SW7405-5021:  Allow 7405 host to control LNA via 31xx tuner/demod
 * chips.
 * 
 * Hydra_Software_Devel/12   10/21/10 6:55p vishk
 * SW7420-1020: Request for non-blocking call support in Nexus frontend
 * 
 * Hydra_Software_Devel/11   10/5/10 7:30p VISHK
 * SW7405-4915: Add nexus api to control power saver mode in downstream
 * demod and tuner, out of banddemod and tuner and upstream. Also add
 * nexus api to control daisy chaining of the frontends.
 * 
 * Hydra_Software_Devel/10   8/30/10 3:58p farshidf
 * SW7550-543: merge to main branch
 * 
 * Hydra_Software_Devel/9   8/24/10 11:30a VISHK
 * SW7405-4808 : Connect reset status api for ADS and AOB to nexus api.
 * $brcm_Log: /magnum/portinginterface/ads/src/common/bads.c $
 * 
 * Hydra_Software_Devel/24   10/18/12 3:42p atanugul
 * SW3128-228: Add API Support for Reading New Front-End 312X, 314X, 346X
 * F/W Version IDs
 * 
 * Hydra_Software_Devel/22   9/28/12 2:17p atanugul
 * SW3128-228: Add support for  BADS_GetVersionInfo()
 * 
 * Hydra_Software_Devel/21   2/14/12 10:42a atanugul
 * SW3128-119: Update SetAcquireParams API to make the second parameter as
 * const
 * 
 * Hydra_Software_Devel/20   2/8/12 5:14p atanugul
 * SW3128-119: Implement new apis for SetAcquireParams, GetAcquireParams
 * and GetDefaultAcquireParams
 * 
 * Hydra_Software_Devel/19   12/8/11 3:12p atanugul
 * SW3128-89: Add PI support for Spectrum Analyzer Data retrieval
 * 
 * Hydra_Software_Devel/18   11/16/11 4:23p atanugul
 * SW3461-79: Add No Signal to GetLockStatus()
 * 
 * Hydra_Software_Devel/17   9/15/11 3:19p atanugul
 * SW3128-50: Add BADS_GetScanStatus()
 * 
 * Hydra_Software_Devel/16   8/31/11 5:41p nickh
 * SW7425-1129: Expose 3255 RPC call that returns the number of bonded
 * channels reserved for Docsis
 * 
 * Hydra_Software_Devel/15   5/6/11 4:23p farshidf
 * SWDTV-6857: waring fix
 * 
 * Hydra_Software_Devel/14   5/6/11 2:22p farshidf
 * SWDTV-6857: merge main
 * 
 * Hydra_Software_Devel/SWDTV-6857/1   5/6/11 2:00p farshidf
 * SWDTV-6857: add the scan structure and Calls
 * 
 * Hydra_Software_Devel/13   11/24/10 8:07p agin
 * SW7405-5021:  Allow 7405 host to control LNA via 31xx tuner/demod
 * chips.
 * 
 * Hydra_Software_Devel/12   10/21/10 6:55p vishk
 * SW7420-1020: Request for non-blocking call support in Nexus frontend
 * 
 * Hydra_Software_Devel/11   10/5/10 7:30p VISHK
 * SW7405-4915: Add nexus api to control power saver mode in downstream
 * demod and tuner, out of banddemod and tuner and upstream. Also add
 * nexus api to control daisy chaining of the frontends.
 * 
 * Hydra_Software_Devel/10   8/30/10 3:58p farshidf
 * SW7550-543: merge to main branch
 * 
 * Hydra_Software_Devel/SW7550-543/2   8/30/10 3:27p farshidf
 * SW7550-543: change the stopTimer to untune
 * 
 * Hydra_Software_Devel/SW7550-543/1   8/27/10 4:50p pinglong
 * SW7550-543: support channel switch between QAM and OFDM on bcm97590
 * 
 * Hydra_Software_Devel/8   2/12/10 2:46p mward
 * SW7550-262: Warning about newline at end of file.
 * 
 * Hydra_Software_Devel/7   2/11/10 11:02a farshidf
 * SW7550-262: add the re-acquire feature for 7550 QAM code
 * 
 * Hydra_Software_Devel/6   12/18/09 2:31p VISHK
 * SW7405-3580: BADS_ResetStatus checks the wrong function pointer.
 * 
 * Hydra_Software_Devel/5   11/5/09 1:47p VISHK
 * SW7405-3210: BADS_31xx_GetStatus not returning values for
 * correctedCount, uncorrectedCount and berRawCount
 * 
 * Hydra_Software_Devel/4   8/25/09 11:19a vishk
 * SW7405-2839: Bcm31xx: Add Power Management support for ADS
 * 
 * Hydra_Software_Devel/3   8/24/09 3:44p vishk
 * SW7405-2839: Bcm31xx: Add Power Management support for ADS
 * 
 * Hydra_Software_Devel/3   8/21/09 6:26p vishk
 * PR 57482: Bcm31xx: Add Power Management support for ADS
 * 
 * Hydra_Software_Devel/3   8/21/09 6:24p vishk
 * PR 57482: Bcm31xx: Add Power Management support for ADS
 * 
 * Hydra_Software_Devel/3   8/21/09 6:23p vishk
 * PR 57482: Bcm31xx: Add Power Management support for ADS
 * 
 * Hydra_Software_Devel/2   4/11/06 4:53p dlwin
 * PR 19978: Merge 3255 support.
 * 
 * Hydra_Software_Devel/PROD_DSG_REFSW_Devel/1   4/3/06 4:19p haisongw
 * PR 19978:add  tuner Settop API support on BCM97455 platform
 * 
 * Hydra_Software_Devel/1   12/13/05 2:56p dlwin
 * PR 18598: Add new ADS for 3255
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bads.h"
#include "bads_priv.h"


BDBG_MODULE(bads);



#define CHK_RETCODE( rc, func )             \
do {                                        \
    if( (rc = BERR_TRACE(func)) != BERR_SUCCESS ) \
    {                                       \
        goto done;                          \
    }                                       \
} while(0)


#define DEV_MAGIC_ID            ((BERR_ADS_ID<<16) | 0xFACE)




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
BERR_Code BADS_Open(
    BADS_Handle *pAds,                  /* [out] Returns handle */
    BCHP_Handle hChip,                  /* [in] Chip handle */
    BREG_Handle hRegister,              /* [in] Register handle */
    BINT_Handle hInterrupt,             /* [in] Interrupt handle */
    const BADS_Settings *pDefSettings   /* [in] Default settings */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ENTER(BADS_Open);
    
    if( pDefSettings->funcPtr.Open != NULL )
    {
        retCode = pDefSettings->funcPtr.Open(pAds, hChip, hRegister, hInterrupt, pDefSettings );
    }
    else
    {
        *pAds = NULL;
        BDBG_WRN(("BADS_Open: Funtion Ptr is NULL"));
    }

    return( retCode );
}

BERR_Code BADS_Close(
    BADS_Handle hDev                    /* [in] Device handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ENTER(BADS_Close);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    if( hDev->settings.funcPtr.Close != NULL )
    {
        retCode = hDev->settings.funcPtr.Close( hDev );
    }
    else
    {
        BDBG_WRN(("BADS_Close: Funtion Ptr is NULL"));
    }

    BDBG_LEAVE(BADS_Close);
    return( retCode );
}

BERR_Code BADS_Init(
    BADS_Handle hDev                    /* [in] Device handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ENTER(BADS_Init);
    BDBG_ASSERT( hDev );
    
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    if( hDev->settings.funcPtr.Init != NULL )
    {
        retCode = hDev->settings.funcPtr.Init( hDev );
    }
    else
    {
        BDBG_WRN(("BADS_Init: Funtion Ptr is NULL"));
    }

    BDBG_LEAVE(BADS_Init);
    return( retCode );
}

BERR_Code BADS_GetVersion(
    BADS_Handle hDev,                   /* [in] Device handle */
    BADS_Version *pVersion              /* [out] Returns version */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ENTER(BADS_GetVersion);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    if( hDev->settings.funcPtr.GetVersion != NULL )
    {
        retCode = hDev->settings.funcPtr.GetVersion( hDev, pVersion );
    }
    else
    {
        BDBG_WRN(("BADS_GetVersion: Funtion Ptr is NULL"));
    }

    BDBG_LEAVE(BADS_GetVersion);
    return( retCode );
}

BERR_Code BADS_GetVersionInfo(
    BADS_Handle hDev,                        /* [in] Device handle */
    BFEC_VersionInfo *pVersionInfo /* [out] Returns version Info */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ENTER(BADS_GetVersionInfo);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    if( hDev->settings.funcPtr.GetVersionInfo != NULL )
    {
        retCode = hDev->settings.funcPtr.GetVersionInfo( hDev, pVersionInfo );
    }
    else
    {
        BDBG_WRN(("BADS_GetVersionInfo: Funtion Ptr is NULL"));
    }

    BDBG_LEAVE(BADS_GetVersionInfo);
    return( retCode );
}
    
BERR_Code BADS_GetBondingCapability(
    BADS_Handle hDev,                   /* [in] Device handle */
    unsigned int *bondedChannels         /* [out] Returns the number of bonded channels stored & reserved for Docsis */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ENTER(BADS_GetBondingCapability);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    if( hDev->settings.funcPtr.GetBondingCapability != NULL )
    {
        retCode = hDev->settings.funcPtr.GetBondingCapability( hDev, bondedChannels );
    }
    else
    {
        BDBG_WRN(("BADS_GetTotalChannels: Funtion Ptr is NULL"));
    }

    BDBG_LEAVE(BADS_GetBondingCapability);
    return( retCode );
}

BERR_Code BADS_GetTotalChannels(
    BADS_Handle hDev,                   /* [in] Device handle */
    unsigned int *totalChannels         /* [out] Returns total number downstream channels supported */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ENTER(BADS_GetTotalChannels);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    if( hDev->settings.funcPtr.GetTotalChannels != NULL )
    {
        retCode = hDev->settings.funcPtr.GetTotalChannels( hDev, totalChannels );
    }
    else
    {
        BDBG_WRN(("BADS_GetTotalChannels: Funtion Ptr is NULL"));
    }

    BDBG_LEAVE(BADS_GetTotalChannels);
    return( retCode );
}

BERR_Code BADS_GetChannelDefaultSettings(
    BADS_Handle hDev,                   /* [in] Device handle */
    unsigned int channelNo,             /* [in] Channel number to default setting for */
    BADS_ChannelSettings *pChnDefSettings /* [out] Returns channel default setting */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ENTER(BADS_GetChannelDefaultSettings);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    if( hDev->settings.funcPtr.GetChannelDefaultSettings != NULL )
    {
        retCode = hDev->settings.funcPtr.GetChannelDefaultSettings( hDev, channelNo, pChnDefSettings );
    }
    else
    {
        BDBG_WRN(("BADS_GetChannelDefaultSettings: Funtion Ptr is NULL"));
    }

    BDBG_LEAVE(BADS_GetChannelDefaultSettings);
    return( retCode );
}

BERR_Code BADS_OpenChannel(
    BADS_Handle hDev,                   /* [in] Device handle */
    BADS_ChannelHandle *phChn,          /* [out] Returns channel handle */
    unsigned int channelNo,             /* [in] Channel number to open */
    const BADS_ChannelSettings *pChnDefSettings /* [in] Channel default setting */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ENTER(BADS_OpenChannel);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    *phChn = NULL;
    if( hDev->settings.funcPtr.OpenChannel != NULL )
    {
        retCode = hDev->settings.funcPtr.OpenChannel( hDev, phChn, channelNo, pChnDefSettings );
    }
    else
    {
        BDBG_WRN(("BADS_OpenChannel: Funtion Ptr is NULL"));
    }

    BDBG_LEAVE(BADS_OpenChannel);
    return( retCode );
}

BERR_Code BADS_CloseChannel(
    BADS_ChannelHandle hChn             /* [in] Device channel handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    

    BDBG_ENTER(BADS_CloseChannel);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    if( hChn->hAds->settings.funcPtr.CloseChannel != NULL )
    {
        retCode = hChn->hAds->settings.funcPtr.CloseChannel( hChn );
    }
    else
    {
        BDBG_WRN(("BADS_CloseChannel: Funtion Ptr is NULL"));
    }

    BDBG_LEAVE(BADS_CloseChannel);
    return( retCode );
}

BERR_Code BADS_GetDevice(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_Handle *phDev                  /* [out] Returns Device handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ENTER(BADS_GetDevice);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    *phDev = NULL;
    if( hChn->hAds->settings.funcPtr.GetDevice != NULL )
    {
        retCode = hChn->hAds->settings.funcPtr.GetDevice( hChn, phDev );
    }
    else
    {
        BDBG_WRN(("BADS_GetDevice: Funtion Ptr is NULL"));
    }

    BDBG_LEAVE(BADS_GetDevice);
    return( retCode );
}

BERR_Code BADS_GetDefaultAcquireParams(
    BADS_ChannelHandle hChn ,           /* [in] Device channel handle */
    BADS_InbandParam *ibParams          /* [out] default Inband Parameters */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ENTER(BADS_GetDefaultAcquireParams);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    if( hChn->hAds->settings.funcPtr.GetDefaultAcquireParams != NULL )
    {
        retCode = hChn->hAds->settings.funcPtr.GetDefaultAcquireParams( ibParams );
    }
    else
    {
        BDBG_WRN(("BADS_GetDefaultAcquireParams: Funtion Ptr is NULL"));
    }

    BDBG_LEAVE(BADS_GetDefaultAcquireParams);
    return( retCode );
}

BERR_Code BADS_SetAcquireParams(
    BADS_ChannelHandle hChn ,           /* [in] Device channel handle */
    const BADS_InbandParam *ibParams          /* [in] Inband Parameters to use */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ENTER(BADS_SetAcquireParams);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    if( hChn->hAds->settings.funcPtr.SetAcquireParams != NULL )
    {
        retCode = hChn->hAds->settings.funcPtr.SetAcquireParams( hChn, ibParams );
    }
    else
    {
        BDBG_WRN(("BADS_SetAcquireParams: Funtion Ptr is NULL"));
    }

    BDBG_LEAVE(BADS_SetAcquireParams);
    return( retCode );
}

BERR_Code BADS_GetAcquireParams(
    BADS_ChannelHandle hChn ,           /* [in] Device channel handle */
    BADS_InbandParam *ibParams          /* [out] Inband Parameters to use */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ENTER(BADS_GetAcquireParams);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    if( hChn->hAds->settings.funcPtr.GetAcquireParams != NULL )
    {
        retCode = hChn->hAds->settings.funcPtr.GetAcquireParams( hChn, ibParams );
    }
    else
    {
        BDBG_WRN(("BADS_GetAcquireParams: Funtion Ptr is NULL"));
    }

    BDBG_LEAVE(BADS_GetAcquireParams);
    return( retCode );
}

BERR_Code BADS_Acquire(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_InbandParam *ibParam           /* [in] Inband Parameters to use */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ENTER(BADS_Acquire);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    if( hChn->hAds->settings.funcPtr.Acquire != NULL )
    {
        retCode = hChn->hAds->settings.funcPtr.Acquire( hChn, ibParam );
    }
    else
    {
        BDBG_WRN(("BADS_Acquire: Funtion Ptr is NULL"));
    }

    BDBG_LEAVE(BADS_Acquire);
    return( retCode );
}

BERR_Code BADS_GetStatus(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_Status *pStatus                /* [out] Returns status */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ENTER(BADS_GetStatus);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );
    
    if( hChn->hAds->settings.funcPtr.GetStatus != NULL )
    {
        retCode = hChn->hAds->settings.funcPtr.GetStatus( hChn, pStatus);
    }
    else
    {
        BDBG_WRN(("BADS_Status: Funtion Ptr is NULL"));
    }

    BDBG_LEAVE(BADS_GetStatus);
    return( retCode );
}

BERR_Code BADS_RequestAsyncStatus(
    BADS_ChannelHandle hChn            /* [in] Device channel handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ENTER(BADS_RequestAsyncStatus);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );
    
    if( hChn->hAds->settings.funcPtr.RequestAsyncStatus != NULL )
    {
        retCode = hChn->hAds->settings.funcPtr.RequestAsyncStatus( hChn);
    }
    else
    {
        BDBG_WRN(("BADS_RequestAsyncStatus: Funtion Ptr is NULL"));
    }

    BDBG_LEAVE(BADS_RequestAsyncStatus);
    return( retCode );
}

BERR_Code BADS_GetAsyncStatus(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_Status *pStatus                /* [out] Returns status */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ENTER(BADS_GetAsyncStatus);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );
    
    if( hChn->hAds->settings.funcPtr.GetAsyncStatus != NULL )
    {
        retCode = hChn->hAds->settings.funcPtr.GetAsyncStatus( hChn, pStatus);
    }
    else
    {
        BDBG_WRN(("BADS_AsyncStatus: Funtion Ptr is NULL"));
    }

    BDBG_LEAVE(BADS_GetAsyncStatus);
    return( retCode );
}

BERR_Code BADS_GetScanStatus(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_ScanStatus *pScanStatus        /* [out] Returns status */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ENTER(BADS_GetScanStatus);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );
    
    if( hChn->hAds->settings.funcPtr.GetScanStatus != NULL )
    {
        retCode = hChn->hAds->settings.funcPtr.GetScanStatus( hChn, pScanStatus);
    }
    else
    {
        BDBG_WRN(("BADS_GetScanStatus: Funtion Ptr is NULL"));
    }

    BDBG_LEAVE(BADS_GetScanStatus);
    return( retCode );
}

BERR_Code BADS_GetLockStatus(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_LockStatus *pLockStatus         /* [out] Returns lock status */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BADS_GetLockStatus);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    if( hChn->hAds->settings.funcPtr.GetLockStatus != NULL )
    {
        retCode = hChn->hAds->settings.funcPtr.GetLockStatus( hChn, pLockStatus );
    }
    else
    {
        BDBG_WRN(("BADS_GetLockStatus: Funtion Ptr is NULL"));
    }

    BDBG_LEAVE(BADS_GetLockStatus);
    return( retCode );
}

BERR_Code BADS_GetSoftDecision(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    int16_t nbrToGet,                   /* [in] Number values to get */
    int16_t *iVal,                      /* [out] Ptr to array to store output I soft decision */
    int16_t *qVal,                      /* [out] Ptr to array to store output Q soft decision */
    int16_t *nbrGotten                  /* [out] Number of values gotten/read */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ENTER(BADS_GetSoftDecision);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    if( hChn->hAds->settings.funcPtr.GetSoftDecision != NULL )
    {
        retCode = hChn->hAds->settings.funcPtr.GetSoftDecision( hChn, nbrToGet, iVal, qVal, nbrGotten );
    }
    else
    {
        BDBG_WRN(("BADS_GetSoftDecision: Funtion Ptr is NULL"));
    }

    BDBG_LEAVE(BADS_GetSoftDecision);
    return( retCode );
}

BERR_Code BADS_EnablePowerSaver(
    BADS_ChannelHandle hChn         /* [in] Device channel handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ENTER(BADS_EnablePowerSaver);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    if( hChn->hAds->settings.funcPtr.EnablePowerSaver != NULL )
    {
        retCode = hChn->hAds->settings.funcPtr.EnablePowerSaver( hChn );
    }
    else
    {
        BDBG_WRN(("BADS_EnablePowerSaver: Funtion Ptr is NULL"));
    }

    BDBG_LEAVE(BADS_EnablePowerSaver);
    return( retCode );
}


BERR_Code BADS_DisablePowerSaver(
    BADS_ChannelHandle hChn         /* [in] Device channel handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ENTER(BADS_DisablePowerSaver);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    if( hChn->hAds->settings.funcPtr.DisablePowerSaver != NULL )
    {
        retCode = hChn->hAds->settings.funcPtr.DisablePowerSaver( hChn );
    }
    else
    {
        BDBG_WRN(("BADS_DisablePowerSaver: Funtion Ptr is NULL"));
    }

    BDBG_LEAVE(BADS_DisablePowerSaver);
    return( retCode );
}


BERR_Code BADS_ProcessNotification(
    BADS_ChannelHandle hChn,                /* [in] Device channel handle */
    unsigned int event                      /* [in] Event code and event data*/
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ENTER(BADS_ProcessNotification);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    if( hChn->hAds->settings.funcPtr.ProcessNotification != NULL )
    {
        retCode = hChn->hAds->settings.funcPtr.ProcessNotification( hChn, event);
    }
    else
    {
        BDBG_WRN(("BADS_ProcessNotification: Funtion Ptr is NULL"));
    }

    BDBG_LEAVE(BADS_ProcessNotification);
    return( retCode );
}

BERR_Code BADS_InstallCallback(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_Callback callbackType,         /* [in] Type of callback */
    BADS_CallbackFunc pCallback,        /* [in] Function Ptr to callback */
    void *pParam                        /* [in] Generic parameter send on callback */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ENTER(BADS_InstallCallback);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );
    
    if( hChn->hAds->settings.funcPtr.InstallCallback != NULL )
    {
        retCode = hChn->hAds->settings.funcPtr.InstallCallback( hChn, callbackType, pCallback, pParam );
    }
    else
    {
        BDBG_WRN(("BADS_InstallCallback: Funtion Ptr is NULL"));
    }

    BDBG_LEAVE(BADS_InstallCallback);
    return( retCode );
}

BERR_Code BADS_SetDaisyChain(
    BADS_Handle hDev,       /* [in] Returns handle */
    bool enableDaisyChain   /* [in] Eanble/disable daisy chain. */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    
    
    BDBG_ENTER(BADS_SetDaisyChain);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    if( hDev->settings.funcPtr.SetDaisyChain != NULL )
    {
            retCode = hDev->settings.funcPtr.SetDaisyChain( hDev, enableDaisyChain);
    }
    else
    {
        BDBG_WRN(("BADS_SetDaisyChain: Funtion Ptr is NULL"));
    }


    BDBG_LEAVE(BADS_SetDaisyChain);
    return( retCode );

}

BERR_Code BADS_GetDaisyChain(
    BADS_Handle hDev,           /* [in] Returns handle */
    bool *isEnableDaisyChain    /* [out] Eanble/disable daisy chain. */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    
    
    BDBG_ENTER(BADS_GetDaisyChain);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    if( hDev->settings.funcPtr.GetDaisyChain != NULL )
    {
            retCode = hDev->settings.funcPtr.GetDaisyChain( hDev, isEnableDaisyChain);
    }
    else
    {
        BDBG_WRN(("BADS_GetDaisyChain: Funtion Ptr is NULL"));
    }


    BDBG_LEAVE(BADS_GetDaisyChain);
    return( retCode );

}


BERR_Code BADS_ResetStatus(
    BADS_ChannelHandle hChn     /* [in] Device channel handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    
    BDBG_ENTER(BADS_ResetStatus);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );
    
    if( hChn->hAds->settings.funcPtr.ResetStatus != NULL )
    {
        retCode = hChn->hAds->settings.funcPtr.ResetStatus( hChn);
    }
    else
    {
        BDBG_WRN(("BADS_ResetStatus: Funtion Ptr is NULL"));
    }

    BDBG_LEAVE(BADS_GetStatus);
    return( retCode );
}

BERR_Code BADS_GetInterruptEventHandle(
   BADS_Handle hDev,            /* [in] BADS handle */
   BKNI_EventHandle *phEvent /* [out] event handle */
)
{
    BERR_Code retCode = BERR_SUCCESS;
    
    
    BDBG_ENTER(BADS_GetInterruptEventHandle);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    if( hDev->settings.funcPtr.GetInterruptEventHandle != NULL )
    {
        retCode = hDev->settings.funcPtr.GetInterruptEventHandle(hDev, phEvent);
    }
    else
    {
        BDBG_WRN(("BADS_GetInterruptEventHandle: Funtion Ptr is NULL"));
    }

    BDBG_LEAVE(BADS_GetInterruptEventHandle);
    return( retCode );
}


BERR_Code BADS_ProcessInterruptEvent(
   BADS_Handle hDev            /* [in] BADS handle */
)
{
    BERR_Code retCode = BERR_SUCCESS;
    
    
    BDBG_ENTER(BADS_ProcessInterruptEvent);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    if( hDev->settings.funcPtr.ProcessInterruptEvent != NULL )
    {
        retCode = hDev->settings.funcPtr.ProcessInterruptEvent( hDev);
    }
    else
    {
        BDBG_WRN(("BADS_ProcessInterruptEvent: Funtion Ptr is NULL"));
    }

    BDBG_LEAVE(BADS_ProcessInterruptEvent);
    return( retCode );
}

BERR_Code BADS_Untune(
	BADS_Handle hDev			/* [in] BADS handle */
	)
{
	BERR_Code retCode = BERR_SUCCESS;


	BDBG_ENTER(BADS_Untune);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

	if( hDev->settings.funcPtr.Untune != NULL )
	{
		retCode = hDev->settings.funcPtr.Untune( hDev );
	}
	else
	{
		BDBG_WRN(("BADS_Untune: Funtion Ptr is NULL"));
	}

	BDBG_LEAVE(BADS_Untune);
	return( retCode );
}

BERR_Code BADS_ReadSlave(
    BADS_ChannelHandle hChn,     /* [in] Device channel handle */
	uint8_t chipAddr,			 /* [in] chip addr of the i2c slave device */
	uint32_t subAddr,			 /* [in] sub addr of the register to read from the slave device */
	uint8_t subAddrLen,			 /* [in] how many bytes is the sub addr? one to four*/
	uint32_t *data,				 /* [out] ptr to the data that we will read from the slave device */
	uint8_t dataLen				 /* [in] how many bytes are we going to read? one to four*/
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    
    BDBG_ENTER(BADS_ReadSlave);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );
    
    if( hChn->hAds->settings.funcPtr.ReadSlave != NULL )
    {
        retCode = hChn->hAds->settings.funcPtr.ReadSlave( hChn, chipAddr, subAddr, subAddrLen, data, dataLen );
    }
    else
    {
        BDBG_WRN(("BADS_ReadSlave: Funtion Ptr is NULL"));
    }

    BDBG_LEAVE(BADS_ReadSlave);
    return( retCode );
}

BERR_Code BADS_WriteSlave(
    BADS_ChannelHandle hChn,     /* [in] Device channel handle */
	uint8_t chipAddr,			 /* [in] chip addr of the i2c slave device */
	uint32_t subAddr,			 /* [in] sub addr of the register to read from the slave device */
	uint8_t subAddrLen,			 /* [in] how many bytes is the sub addr? one to four*/
	uint32_t *data,				 /* [in] ptr to the data that we will write to the slave device */
	uint8_t dataLen				 /* [in] how many bytes are we going to write? one to four*/
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    
    BDBG_ENTER(BADS_WriteSlave);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );
    
    if( hChn->hAds->settings.funcPtr.WriteSlave != NULL )
    {
        retCode = hChn->hAds->settings.funcPtr.WriteSlave(  hChn, chipAddr, subAddr, subAddrLen, data, dataLen );
    }
    else
    {
        BDBG_WRN(("BADS_WriteSlave: Funtion Ptr is NULL"));
    }

    BDBG_LEAVE(BADS_WriteSlave);
    return( retCode );
}


/******************************************************************************
Summary:
   This function will pass the scan parameter to ADS.
Description:
  
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BADS_SetScanParam(
		BADS_ChannelHandle hChn,     /* [in] Device channel handle */ 
	    BADS_ChannelScanSettings *pChnScanSettings /* [in] Set Channel setting */
)
{
	BERR_Code retCode = BERR_SUCCESS;
    
    BDBG_ENTER(BADS_SetScanParam);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );
    
    if( hChn->hAds->settings.funcPtr.SetScanParam != NULL )
    {
        retCode = hChn->hAds->settings.funcPtr.SetScanParam(  hChn, pChnScanSettings);
    }
    else
    {
        BDBG_WRN(("BADS_SetScanParam: Funtion Ptr is NULL"));
    }

    BDBG_LEAVE(BADS_SetScanParam);
    return( retCode );
}
/******************************************************************************
Summary:
   This function will get the scan parameter from ADS.
Description:
  
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BADS_GetScanParam (
		BADS_ChannelHandle hChn,     /* [in] Device channel handle */ 
		BADS_ChannelScanSettings *pChnScanSettings /* [out] Returns channel default setting */
)
{
	BERR_Code retCode = BERR_SUCCESS;
    
    BDBG_ENTER(BADS_GetScanParam);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );
    
    if( hChn->hAds->settings.funcPtr.GetScanParam != NULL )
    {
        retCode = hChn->hAds->settings.funcPtr.GetScanParam(  hChn, pChnScanSettings);
    }
    else
    {
        BDBG_WRN(("BADS_GetScanParam: Funtion Ptr is NULL"));
    }

    BDBG_LEAVE(BADS_GetScanParam);
    return( retCode );
}

/******************************************************************************
Summary:
   This function sends request for spectrum analyzer data to the LEAP.
Description:
  
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BADS_RequestSpectrumAnalyzerData(
    BADS_ChannelHandle hChn,     /* [in] Device channel handle */ 
    BADS_SpectrumSettings *pSettings /* [in] spectrum settings */
    )
{
	BERR_Code retCode = BERR_SUCCESS;
    
    BDBG_ENTER(BADS_RequestSpectrumAnalyzerData);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );
    
    if( hChn->hAds->settings.funcPtr.RequestSpectrumAnalyzerData != NULL )
    {
        retCode = hChn->hAds->settings.funcPtr.RequestSpectrumAnalyzerData(  hChn, pSettings);
    }
    else
    {
        BDBG_WRN(("BADS_RequestSpectrumAnalyzerData: Funtion Ptr is NULL"));
    }

    BDBG_LEAVE(BADS_RequestSpectrumAnalyzerData);
    return( retCode );
}    

/******************************************************************************
Summary:
   This function gets spectrum analyzer data from the LEAP.
Description:
  
Returns:
   BERR_Code
******************************************************************************/    
BERR_Code BADS_GetSpectrumAnalyzerData(
    BADS_ChannelHandle hChn,     /* [in] Device channel handle */ 
    BADS_SpectrumData  *pSpectrumData /* [out] spectrum Data*/
    )
{
	BERR_Code retCode = BERR_SUCCESS;
    
    BDBG_ENTER(BADS_GetSpectrumAnalyzerData);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );
    
    if( hChn->hAds->settings.funcPtr.GetSpectrumAnalyzerData != NULL )
    {
        retCode = hChn->hAds->settings.funcPtr.GetSpectrumAnalyzerData(  hChn, pSpectrumData);
    }
    else
    {
        BDBG_WRN(("BADS_GetSpectrumAnalyzerData: Funtion Ptr is NULL"));
    }

    BDBG_LEAVE(BADS_GetSpectrumAnalyzerData);
    return( retCode );
}
    
