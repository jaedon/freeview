/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bads_docsis_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/41 $
 * $brcm_Date: 9/28/12 2:17p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ads/src/docsis/bads_docsis_priv.c $
 * 
 * Hydra_Software_Devel/41   9/28/12 2:17p atanugul
 * SW3128-228: Add support for  BADS_GetVersionInfo()
 * 
 * Hydra_Software_Devel/40   6/18/12 5:24p atanugul
 * SW7425-2983: Update bads.inc based on the new ADS directory structure
 * 
 * Hydra_Software_Devel/39   3/5/12 5:31p atanugul
 * SW3128-130: change the resolution for equalizerGain units to 1/100th of
 * dB
 * 
 * Hydra_Software_Devel/38   2/8/12 5:58p atanugul
 * SW3128-119: Add initializers for for SetAcquireParams, GetAcquireParams
 * and PowerUp
 * 
 * Hydra_Software_Devel/37   12/8/11 3:30p atanugul
 * SW3128-89: Add Initializer for ADS_RequestSpectrumAnalyzerData  and
 * BADS_GetSpectrumAnalyzerData
 * 
 * Hydra_Software_Devel/36   11/17/11 11:45a atanugul
 * SW3461-79: Add No Signal to GetLockStatus()
 * 
 * Hydra_Software_Devel/35   9/15/11 3:29p atanugul
 * SW3128-50: Initializer fot BADS_GetScanStatus()
 * 
 * Hydra_Software_Devel/34   8/31/11 5:54p nickh
 * SW7425-1129: Expose 3255 RPC GetBondingCapability() call and remove
 * call that is not implemented correctly
 * 
 * Hydra_Software_Devel/33   7/7/11 3:28p mward
 * SW7125-1017:  Add accumulating counters.
 * 
 * Hydra_Software_Devel/32   5/16/11 2:01p farshidf
 * SWDTV-6857: add the Scan calls
 * 
 * Hydra_Software_Devel/31   11/29/10 12:17p mward
 * SW7405-5021:  NULL-initialize pointers to functions which allow 7405
 * host to control LNA via 31xx tuner/demod chips.
 * 
 * Hydra_Software_Devel/30   10/27/10 3:29p mward
 * SW7420-1020:  Init AsyncStatus function pointers to NULL, not supported
 * yet.
 * 
 * Hydra_Software_Devel/29   10/6/10 2:49p mward
 * SW7405-4915: Add nexus api to control power saver mode in downstream
 * demod and tuner, out of banddemod and tuner and upstream. Also add
 * nexus api to control daisy chaining of the frontends.
 * 
 * Hydra_Software_Devel/28   8/30/10 3:39p farshidf
 * SW7550-543: update the function table
 * 
 * Hydra_Software_Devel/27   8/26/10 11:58a haisongw
 * SW7125-272:Coverity Defect ID:20353 DEADCODE bads_3255.c Product=97125
 *
 * Hydra_Software_Devel/26   2/11/10 11:07a farshidf
 * SW7550-262: make the API function compatible with 7550
 *
 * Hydra_Software_Devel/25   1/19/10 3:28p haisongw
 * SW7420-450: Functionality required to check spectrum inversion status
 * from 3255R ADS
 *
 * Hydra_Software_Devel/24   11/30/09 6:40p mward
 * SW7125-131: Allow 4 Rmagnum tuners.
 *
 * Hydra_Software_Devel/23   11/24/09 4:46p farshidf
 * SW7550-38: set the new param for 7550 to NULL
 *
 * Hydra_Software_Devel/22   10/14/09 2:06p mward
 * SW7405-2852: SW7405-3009: New structure members added to the
 * defDevSettings
 *
 * Hydra_Software_Devel/21   8/25/09 3:13p haisongw
 * SW7405-2839,SW7400-2477: (1)  Add BADS APIs to retrieve 3255 channel
 * bonding status (2) Add Power Management support
 *
 * Hydra_Software_Devel/20   10/13/08 11:53a anilmm
 * PR47842: BER calculation software implementation
 *
 * Hydra_Software_Devel/19   5/27/08 4:27p haisongw
 * PR43047: Add two parameters in BADS_GetStatus() to sync up with 325X
 * docsis 2.0.0 release
 *
 * Hydra_Software_Devel/18   3/12/08 4:01p haisongw
 * PR38683: 7118 only has two ADS channels
 *
 * Hydra_Software_Devel/17   1/11/08 4:58p haisongw
 * PR38683: Support both 3255 and 3252 in single Trinity Application
 *
 * Hydra_Software_Devel/16   12/20/07 6:02p haisongw
 * PR36061: speed up 325x QAM acqusition time
 *
 * Hydra_Software_Devel/15   10/12/07 1:28p haisongw
 * PR36060: add OCAP DPM support for three 3420 tuners
 *
 * Hydra_Software_Devel/14   10/12/07 1:13p haisongw
 * PR36061: speed up 3255 tuning speed
 *
 * Hydra_Software_Devel/13   8/8/07 5:37p mward
 * PR30640: Set the isLock? message back to BDBG_MSG, to reduce clutter.
 *
 * Hydra_Software_Devel/12   5/11/07 11:51a haisongw
 * PR30640: Add an option to enable/disable 325X ADS auto-reacquiring
 *
 * Hydra_Software_Devel/11   5/10/07 6:22p haisongw
 * PR30640: Add an option to enable/disable 325X ADS auto-reacquiring
 *
 * Hydra_Software_Devel/10   4/16/07 6:12p haisongw
 * PR25049: SetTop API function btuner_get_qam_status() returns incomplete
 * information
 *
 * Hydra_Software_Devel/9   10/31/06 3:36p haisongw
 * PR25385: do always callback when 3255 notifies host
 *
 * Hydra_Software_Devel/8   8/3/06 5:44p haisongw
 * PR23265: remove compiler warnings for BADS and BTNR
 *
 * Hydra_Software_Devel/7   7/31/06 7:04p haisongw
 * PR23108: Rmagnum sometime can't send notification to 74XX
 *
 * Hydra_Software_Devel/6   7/27/06 10:53a haisongw
 * PR 22368: Add AOB, AUS and TNR3255ob support
 *
 * Hydra_Software_Devel/5   5/12/06 5:31p haisongw
 * PR 20984: move private APIs bads_3255.h into bads_3255_priv.h
 *
 * Hydra_Software_Devel/4   4/11/06 4:53p dlwin
 * PR 19978: Merge 3255 support.
 *
 * Hydra_Software_Devel/PROD_DSG_REFSW_Devel/2   4/4/06 2:31p haisongw
 * PR 19978: segmentation fault when BADS_3255Close()
 *
 * Hydra_Software_Devel/PROD_DSG_REFSW_Devel/1   4/3/06 4:23p haisongw
 * PR 19978:add  tuner Settop API support on BCM97455 platform
 *
 * Hydra_Software_Devel/3   3/11/06 3:38p dlwin
 * PR 18598: Forgot to free memory device is closed.
 *
 * Hydra_Software_Devel/1   12/13/05 2:58p dlwin
 * PR 18598: Add new ADS for 3255
 *
 ***************************************************************************/

#include "bstd.h"
#include "bads.h"
#include "bads_priv.h"
#include "brpc.h"
#include "brpc_plat.h"
#include "brpc_3255.h"
#include "bads_3255.h"
#include "bads_docsis_priv.h"

BDBG_MODULE(bads_docsis);

#define sizeInLong(x)	(sizeof(x)/sizeof(uint32_t))
#define	CHK_RETCODE( rc, func )				\
do {										\
	if( (rc = BERR_TRACE(func)) != BERR_SUCCESS ) \
	{										\
		goto done;							\
	}										\
} while(0)


#define	MX_ADS_CHANNELS			(8)



#define	DEV_MAGIC_ID			((BERR_ADS_ID<<16) | 0xFACE)

#define	ONE_TO_MEGA(x)			((float) ((double) x / 1000000.0))
#define	MEGA_TO_ONE(x)			(x * 1000000)
#define	HERTZ_TO_MHZ(x)			ONE_TO_MEGA(x)
#define	MHZ_TO_HERTZ(x)			MEGA_TO_ONE(x)




/*******************************************************************************
*
*	Private Module Handles
*
*******************************************************************************/
typedef struct BADS_P_docsis_Handle				*BADS_docsis_Handle;
typedef struct BADS_P_docsis_Handle
{
	uint32_t magicId;					/* Used to check if structure is corrupt */
	BCHP_Handle hChip;
	BREG_Handle hRegister;
	BINT_Handle hInterrupt;
	BRPC_DevId devId;
	BRPC_Handle hRpc;
	BADS_Version verInfo;
	unsigned int mxChnNo;				/* Number of total channels*/
	unsigned int BondedChnNo;			/* Number of bonded channels*/
	unsigned int num2Reserve;			/* Number of bonded channels to reserve*/
	BADS_ChannelHandle hAdsChn[MX_ADS_CHANNELS];
} BADS_P_docsis_Handle;

typedef struct BADS_P_docsis_ChannelHandle			*BADS_docsis_ChannelHandle;
typedef struct BADS_P_docsis_ChannelHandle
{
	uint32_t magicId;					/* Used to check if structure is corrupt */
	BADS_Handle hAds;
	unsigned int chnNo;
	BRPC_DevId devId;
	BRPC_Handle hRpc;
	BADS_CallbackFunc pCallback[BADS_Callback_eLast];
	void *pCallbackParam[BADS_Callback_eLast];
	BADS_ChannelSettings settings;
	BADS_LockStatus lockStatus;			/* current lock status */
	BKNI_MutexHandle mutex;				/* mutex to protect lock status*/
	uint32_t accCorrectedCount;         /* Accumulated corrected block count. Reset on every reset status */
	uint32_t accUncorrectedCount;       /* Accumulated un corrected block count. Reset on every reset status */
} BADS_P_docsis_ChannelHandle;



/*******************************************************************************
*
*	Default Module Settings
*
*******************************************************************************/

static const BADS_Settings defDevSettings =
{
    BRPC_DevId_3255,						/* TODO: Is this required? */
    NULL,									/* BRPC handle, must be provided by application*/
    {
        BADS_docsis_Open,
        BADS_docsis_Close,
        BADS_docsis_Init,
        BADS_docsis_GetVersion,
        NULL, /* BADS_GetVersionInfo */        
        BADS_docsis_GetBondingCapability,
        BADS_docsis_GetTotalChannels,
        BADS_docsis_OpenChannel,
        BADS_docsis_CloseChannel,
        BADS_docsis_GetDevice,
        BADS_docsis_GetChannelDefaultSettings,
        BADS_docsis_GetStatus,
        BADS_docsis_GetLockStatus,
        BADS_docsis_GetSoftDecision,
        BADS_docsis_InstallCallback,
        NULL, /* BADS_GetDefaultAcquireParams */
        NULL, /* BADS_SetAcquireParams */
        NULL, /* BADS_GetAcquireParams */       
        BADS_docsis_Acquire,
        BADS_docsis_EnablePowerSaver,
        NULL, /* DisablePowerSaver */
        BADS_docsis_ProcessNotification,
        NULL, /* SetDaisyChain */
        NULL, /* GetDaisyChain */
        BADS_docsis_ResetStatus,
        NULL, /* GetInterruptEventHandle */
        NULL, /* ProcessInterruptEvent */
        NULL, /* Untune */
        NULL, /* RequestAsyncStatus */
        NULL, /* GetAsyncStatus */
        NULL, /* GetScanStatus */
        NULL, /* ReadSlave */
        NULL,  /* WriteSlave */
        NULL, /*SetScanParam*/
        NULL, /* GetScanParam*/
        NULL, /* BADS_RequestSpectrumAnalyzerData */
        NULL /* BADS_GetSpectrumAnalyzerData  */
    },
    false,
    BADS_TransportData_eSerial,
    NULL,
    NULL
};

/*******************************************************************************
*
*	Private Module Data
*
*******************************************************************************/



/*******************************************************************************
*
*	Private Module Functions
*
*******************************************************************************/



/*******************************************************************************
*
*	Public Module Functions
*
*******************************************************************************/
BERR_Code BADS_docsis_Open(
	BADS_Handle *pAds,					/* [out] Returns handle */
	BCHP_Handle hChip,					/* [in] Chip handle */
	BREG_Handle hRegister,				/* [in] Register handle */
	BINT_Handle hInterrupt,				/* [in] Interrupt handle */
	const struct BADS_Settings *pDefSettings	/* [in] Default settings */
	)
{
	BERR_Code retCode = BERR_SUCCESS;
 	BADS_Handle hDev;
	unsigned int chnIdx;
	BADS_docsis_Handle hImplDev = NULL;


	BDBG_ENTER(BADS_docsis_Open);

	/* Alloc memory from the system heap */
	hDev = (BADS_Handle) BKNI_Malloc( sizeof( BADS_P_Handle ) );
	if( hDev == NULL )
	{
		retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		BDBG_ERR(("BADS_Open: BKNI_malloc() failed"));
		goto done;
	}
	BKNI_Memset( hDev, 0x00, sizeof( BADS_P_Handle ) );

	hDev->magicId = DEV_MAGIC_ID;
	hDev->settings = *pDefSettings;

	hImplDev = (BADS_docsis_Handle) BKNI_Malloc( sizeof( BADS_P_docsis_Handle ) );
	if( hImplDev == NULL )
	{
		retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		BDBG_ERR(("BADS_Open: BKNI_malloc() failed, impl"));
		goto done;
	}
	BKNI_Memset( hImplDev, 0x00, sizeof( BADS_P_docsis_Handle ) );


	hImplDev->magicId = DEV_MAGIC_ID;
	hImplDev->hChip = hChip;
	hImplDev->hRegister = hRegister;
	hImplDev->hInterrupt = hInterrupt;
	hImplDev->hRpc = pDefSettings->hGeneric;	/* For this device, we need the RPC handle */
	hImplDev->devId = pDefSettings->devId;
	hImplDev->mxChnNo = 0;	/* wait until the 3255 informs us */
	for( chnIdx = 0; chnIdx < MX_ADS_CHANNELS; chnIdx++ )
	{
		hImplDev->hAdsChn[chnIdx] = NULL;
	}
	hDev->pImpl = hImplDev;
	*pAds = hDev;


done:
	if( retCode != BERR_SUCCESS )
	{
		if( hDev != NULL )
		{
			BKNI_Free( hDev );
		}
		*pAds = NULL;
	}
	BDBG_LEAVE(BADS_docsis_Open);
	return( retCode );
}

BERR_Code BADS_docsis_Close(
	BADS_Handle hDev					/* [in] Device handle */
	)
{
	BERR_Code retCode = BERR_SUCCESS;


	BDBG_ENTER(BADS_docsis_Close);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

	BKNI_Free( (void *) hDev->pImpl );
	hDev->magicId = 0x00;		/* clear it to catch inproper use */
	BKNI_Free( (void *) hDev );

	BDBG_LEAVE(BADS_docsis_Close);
	return( retCode );
}


BERR_Code BADS_docsis_Init(
	BADS_Handle hDev					/* [in] Device handle */
	)
{
	BERR_Code retCode = BERR_SUCCESS;
	BERR_Code retVal;
	BADS_docsis_Handle hImplDev;
	BRPC_Param_ADS_GetTotalChannels outChnNoParam;
	BRPC_Param_ADS_GetVersion outVerParam;
	BRPC_Param_XXX_Get Param;

	BDBG_ENTER(BADS_docsis_Init);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

	hImplDev = (BADS_docsis_Handle) hDev->pImpl;
	BDBG_ASSERT( hImplDev );
	BDBG_ASSERT( hImplDev->hRpc );

	/* Get the version information for this device */
	CHK_RETCODE( retCode, BRPC_CallProc(hImplDev->hRpc, BRPC_ProcId_ADS_GetVersion, (const uint32_t *)&Param, sizeInLong(Param), (uint32_t *)&outVerParam, sizeInLong(outVerParam), &retVal) );
	CHK_RETCODE( retCode, retVal );
	hImplDev->verInfo.majVer = outVerParam.majVer;
	hImplDev->verInfo.minVer = outVerParam.minVer;
	BDBG_MSG((" 3255 return version majVer %d minVer%d",hImplDev->verInfo.majVer, hImplDev->verInfo.minVer));

	/* Get the number of In-Band Downstream channels available for Video use */
	CHK_RETCODE( retCode, BRPC_CallProc(hImplDev->hRpc, BRPC_ProcId_ADS_GetTotalChannels, (const uint32_t *)&Param, sizeInLong(Param), (uint32_t *)&outChnNoParam, sizeInLong(outChnNoParam), &retVal) );
	CHK_RETCODE( retCode, retVal );
	if (hImplDev->hRpc->rpc_disabled == true) /*return dummy channel num*/
		hImplDev->mxChnNo = 2;
	else
		hImplDev->mxChnNo = outChnNoParam.totalChannels;

	BDBG_MSG((" 3255 return total channels is %d", hImplDev->mxChnNo));

done:
	BDBG_LEAVE(BADS_docsis_Init);
	return( retCode );
}

BERR_Code BADS_3255_GetDefaultSettings(
	BADS_Settings *pDefSettings,		/* [out] Returns default setting */
	BCHP_Handle hChip					/* [in] Chip handle */
	)
{
	BERR_Code retCode = BERR_SUCCESS;

	BDBG_ENTER(BADS_docsis_GetDefaultSettings);

	BSTD_UNUSED(hChip);

	*pDefSettings = defDevSettings;

	BDBG_LEAVE(BADS_docsis_GetDefaultSettings);
	return( retCode );
}

BERR_Code BADS_docsis_GetVersion(
	BADS_Handle hDev,					/* [in] Device handle */
	BADS_Version *pVersion				/* [out] Returns version */
	)
{
	BERR_Code retCode = BERR_SUCCESS;
	BADS_docsis_Handle hImplDev;


	BDBG_ENTER(BADS_docsis_GetVersion);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

	hImplDev = (BADS_docsis_Handle) hDev->pImpl;
	BDBG_ASSERT( hImplDev );
	BDBG_ASSERT( hImplDev->hRpc );

	/* use saved data */
	*pVersion = hImplDev->verInfo;

	BDBG_LEAVE(BADS_docsis_GetVersion);
	return( retCode );
}

BERR_Code BADS_docsis_GetTotalChannels(
	BADS_Handle hDev,					/* [in] Device handle */
	unsigned int *totalChannels			/* [out] Returns total number downstream channels supported */
	)
{
	BERR_Code retCode = BERR_SUCCESS;
	BADS_docsis_Handle hImplDev;


	BDBG_ENTER(BADS_docsis_GetTotalChannels);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

	hImplDev = (BADS_docsis_Handle) hDev->pImpl;
	BDBG_ASSERT( hImplDev );
	BDBG_ASSERT( hImplDev->hRpc );

	/* use saved data */
	*totalChannels = hImplDev->mxChnNo;

	BDBG_LEAVE(BADS_docsis_GetTotalChannels);
	return( retCode );
}


BERR_Code BADS_docsis_GetBondingCapability(
	BADS_Handle hDev,					/* [in] Device handle */
	unsigned int *num			/* [out] Returns number of current bonded channels */
	)
{
	BERR_Code retCode = BERR_SUCCESS;
	BERR_Code retVal;
	BADS_docsis_Handle hImplDev;
	BRPC_Param_ADS_GetBondingCapability outParam;
	BRPC_Param_XXX_Get Param;

	BDBG_ENTER(BADS_docsis_GetBondingCapability);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

	hImplDev = (BADS_docsis_Handle) hDev->pImpl;
	BDBG_ASSERT( hImplDev );
	BDBG_ASSERT( hImplDev->hRpc );

	/* Get the bonding capability of 3255 */
	CHK_RETCODE( retCode, BRPC_CallProc(hImplDev->hRpc, BRPC_ProcId_ADS_GetBondingCapability, (const uint32_t *)&Param, sizeInLong(Param), (uint32_t *)&outParam, sizeInLong(outParam), &retVal) );
	CHK_RETCODE( retCode, retVal );

	*num = hImplDev->BondedChnNo = outParam.maxNum;
	BDBG_MSG((" 3255 BondingCapability: number of bonded channel is %d", hImplDev->BondedChnNo ));

done:
	BDBG_LEAVE(BADS_docsis_GetBondingCapability);
	return( retCode );
}


BERR_Code BADS_docsis_GetChannelDefaultSettings(
	BADS_Handle hDev,					/* [in] Device handle */
	unsigned int channelNo,				/* [in] Channel number to default setting for */
    BADS_ChannelSettings *pChnDefSettings /* [out] Returns channel default setting */
    )
{
	BERR_Code retCode = BERR_SUCCESS;
	BADS_docsis_Handle hImplDev;

	BDBG_ENTER(BADS_docsis_GetChannelDefaultSettings);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

	hImplDev = (BADS_docsis_Handle) hDev->pImpl;
	BDBG_ASSERT( hImplDev );
	BDBG_ASSERT( hImplDev->hRpc );

	if( channelNo < hImplDev->mxChnNo )
	{
		pChnDefSettings->ifFreq = BADS_SETTINGS_IFFREQ;
		pChnDefSettings->autoAcquire = false;
		pChnDefSettings->fastAcquire = false;
	} else		/* fall through the error state */
		retCode = BERR_TRACE(BERR_INVALID_PARAMETER);

	BDBG_LEAVE(BADS_docsis_GetChannelDefaultSettings);
	return( retCode );
}

BERR_Code BADS_docsis_OpenChannel(
	BADS_Handle hDev,					/* [in] Device handle */
	BADS_ChannelHandle *phChn,			/* [out] Returns channel handle */
	unsigned int channelNo,					/* [in] Channel number to open */
	const struct BADS_ChannelSettings *pChnDefSettings /* [in] Channel default setting */
	)
{
	BERR_Code retCode = BERR_SUCCESS;
	BADS_docsis_Handle hImplDev;
	BADS_docsis_ChannelHandle hImplChnDev = NULL;
 	BADS_ChannelHandle hChnDev;
	BRPC_Param_ADS_OpenChannel	Param;
	BERR_Code retVal;


	BDBG_ENTER(BADS_docsis_OpenChannel);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );


	hImplDev = (BADS_docsis_Handle) hDev->pImpl;
	BDBG_ASSERT( hImplDev );
	BDBG_ASSERT( hImplDev->hRpc );

	hChnDev = NULL;
	if( channelNo < hImplDev->mxChnNo )
	{
		if( hImplDev->hAdsChn[channelNo] == NULL )
		{
			/* Alloc memory from the system heap */
			hChnDev = (BADS_ChannelHandle) BKNI_Malloc( sizeof( BADS_P_ChannelHandle ) );
			if( hChnDev == NULL )
			{
				retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
				BDBG_ERR(("BADS_OpenChannel: BKNI_malloc() failed"));
				goto done;
			}
			BKNI_Memset( hChnDev, 0x00, sizeof( BADS_P_ChannelHandle ) );

			hChnDev->magicId = DEV_MAGIC_ID;
			hChnDev->hAds = hDev;

			hImplChnDev = (BADS_docsis_ChannelHandle) BKNI_Malloc( sizeof( BADS_P_docsis_ChannelHandle ) );
			if( hImplChnDev == NULL )
			{
				retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
				BDBG_ERR(("BADS_OpenChannel: BKNI_malloc() failed, impl"));
				goto done;
			}

			/* RPC open channel*/
			Param.devId = BRPC_DevId_3255_DS0 + channelNo;
			if (pChnDefSettings) Param.ifFreq = pChnDefSettings->ifFreq;

			CHK_RETCODE( retCode, BRPC_CallProc(hImplDev->hRpc, BRPC_ProcId_ADS_OpenChannel, (const uint32_t *)&Param, sizeInLong(Param), NULL, 0, &retVal));
			CHK_RETCODE( retCode, retVal );


			hImplChnDev->chnNo = channelNo;
			if (pChnDefSettings) hImplChnDev->settings = *pChnDefSettings;
			/* Save copies for quicker access */
			hImplChnDev->devId = BRPC_DevId_3255_DS0 + hImplChnDev->chnNo;
			hImplChnDev->hRpc = hImplDev->hRpc;
			CHK_RETCODE(retCode, BKNI_CreateMutex(&hImplChnDev->mutex));
			hImplDev->hAdsChn[channelNo] = hChnDev;
			hChnDev->pImpl = hImplChnDev;

			*phChn = hChnDev;
		}
		else
		{
			retCode = BERR_TRACE(BADS_ERR_NOTAVAIL_CHN_NO);
		}
	}
	else
	{
		retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
	}

done:
	if( retCode != BERR_SUCCESS )
	{
		if( hChnDev != NULL )
		{
			BKNI_Free( hChnDev );
			hImplDev->hAdsChn[channelNo] = NULL;
		}
		if( hImplChnDev != NULL )
		{
			BKNI_Free( hImplChnDev );
		}
		*phChn = NULL;
	}
	BDBG_LEAVE(BADS_docsis_OpenChannel);
	return( retCode );
}

BERR_Code BADS_docsis_CloseChannel(
	BADS_ChannelHandle hChn				/* [in] Device channel handle */
	)
{
	BERR_Code retCode = BERR_SUCCESS;
	BADS_docsis_Handle hImplDev;
	BADS_docsis_ChannelHandle hImplChnDev;
	BADS_Handle hAds;
	unsigned int chnNo;
	BRPC_Param_ADS_CloseChannel Param;
	BERR_Code retVal;


	BDBG_ENTER(BADS_docsis_CloseChannel);
	BDBG_ASSERT( hChn );
	BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

	hImplChnDev = (BADS_docsis_ChannelHandle) hChn->pImpl;
	BDBG_ASSERT( hImplChnDev );
	BDBG_ASSERT( hImplChnDev->hRpc );

	Param.devId = hImplChnDev->devId;
	CHK_RETCODE( retCode, BRPC_CallProc(hImplChnDev->hRpc, BRPC_ProcId_ADS_CloseChannel, (const uint32_t *)&Param, sizeInLong(Param), NULL, 0, &retVal));
	CHK_RETCODE( retCode, retVal );

	hAds = hChn->hAds;
	hImplDev = (BADS_docsis_Handle) hAds->pImpl;
	BDBG_ASSERT( hImplDev );

	BKNI_DestroyMutex(hImplChnDev->mutex);
	chnNo = hImplChnDev->chnNo;
	hChn->magicId = 0x00;		/* clear it to catch inproper use */
	BKNI_Free( hChn->pImpl );
	BKNI_Free( hChn );
	hImplDev->hAdsChn[chnNo] = NULL;

done:
	BDBG_LEAVE(BADS_docsis_CloseChannel);
	return( retCode );
}

BERR_Code BADS_docsis_GetDevice(
	BADS_ChannelHandle hChn,			/* [in] Device channel handle */
	BADS_Handle *phDev					/* [out] Returns Device handle */
	)
{
	BERR_Code retCode = BERR_SUCCESS;


	BDBG_ENTER(BADS_docsis_GetDevice);
	BDBG_ASSERT( hChn );
	BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

	*phDev = hChn->hAds;

	BDBG_LEAVE(BADS_docsis_GetDevice);
	return( retCode );
}

BERR_Code BADS_docsis_Acquire(
	BADS_ChannelHandle hChn,			/* [in] Device channel handle */
	BADS_InbandParam *ibParam			/* [in] Inband Parameters to use */
	)
{
	BERR_Code retCode = BERR_SUCCESS;
	BADS_docsis_ChannelHandle hImplChnDev;
	BRPC_Param_ADS_Acquire Param;
	BERR_Code retVal;


	BDBG_ENTER(BADS_docsis_Acquire);
	BDBG_ASSERT( hChn );
	BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

	hImplChnDev = (BADS_docsis_ChannelHandle) hChn->pImpl;
	BDBG_ASSERT( hImplChnDev );
	BDBG_ASSERT( hImplChnDev->hRpc );

	BKNI_AcquireMutex(hImplChnDev->mutex);
	hImplChnDev->lockStatus = BADS_LockStatus_eUnlocked;
	BKNI_ReleaseMutex(hImplChnDev->mutex);

	hImplChnDev->accCorrectedCount = 0;		/* Reset accumulated counts. */
	hImplChnDev->accUncorrectedCount = 0;

	BDBG_MSG(("%s: modType=%d, symbolRate=%d", __FUNCTION__, ibParam->modType, ibParam->symbolRate));

	Param.devId = hImplChnDev->devId;
	Param.modType = ibParam->modType;
	Param.symbolRate = ibParam->symbolRate;
	Param.autoAcquire = hImplChnDev->settings.autoAcquire;
	Param.fastAcquire = hImplChnDev->settings.fastAcquire;
	CHK_RETCODE( retCode, BRPC_CallProc(hImplChnDev->hRpc, BRPC_ProcId_ADS_Acquire, (const uint32_t *)&Param, sizeInLong(Param), NULL, 0, &retVal));
	CHK_RETCODE( retCode, retVal );

done:
	BDBG_LEAVE(BADS_docsis_Acquire);
	return( retCode );
}

BERR_Code BADS_docsis_GetStatus(
	BADS_ChannelHandle hChn,			/* [in] Device channel handle */
	BADS_Status *pStatus				/* [out] Returns status */
	)
{
	BERR_Code retCode = BERR_SUCCESS;
	BADS_docsis_ChannelHandle hImplChnDev;
	BRPC_Param_ADS_GetStatus outParam;
	BRPC_Param_XXX_Get Param;
	BERR_Code retVal;


	BDBG_ENTER(BADS_docsis_GetStatus);
	BDBG_ASSERT( hChn );
	BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

	hImplChnDev = (BADS_docsis_ChannelHandle) hChn->pImpl;
	BDBG_ASSERT( hImplChnDev );
	BDBG_ASSERT( hImplChnDev->hRpc );

	pStatus->isFecLock = pStatus->isQamLock = false;
	Param.devId = hImplChnDev->devId;

	CHK_RETCODE( retCode, BRPC_CallProc(hImplChnDev->hRpc, BRPC_ProcId_ADS_GetStatus, (const uint32_t *)&Param, sizeInLong(Param), (uint32_t *)&outParam, sizeInLong(outParam), &retVal) );
	CHK_RETCODE( retCode, retVal);

	hImplChnDev->accCorrectedCount += outParam.correctedCount;
	hImplChnDev->accUncorrectedCount += outParam.uncorrectedCount;

	pStatus->isPowerSaverEnabled = outParam.isPowerSaverEnabled;
	pStatus->modType = outParam.modType;
	pStatus->ifFreq = outParam.ifFreq;
	pStatus->symbolRate = outParam.symbolRate;
	pStatus->isFecLock = outParam.isFecLock;
	pStatus->isQamLock = outParam.isQamLock;
	pStatus->correctedCount = outParam.correctedCount;
	pStatus->uncorrectedCount = outParam.uncorrectedCount;
	pStatus->snrEstimate = outParam.snrEstimate;
	pStatus->agcIntLevel = outParam.agcIntLevel;
	pStatus->agcExtLevel = outParam.agcExtLevel;
	pStatus->carrierFreqOffset = outParam.carrierFreqOffset;
	pStatus->carrierPhaseOffset = outParam.carrierPhaseOffset;
	pStatus->rxSymbolRate = outParam.rxSymbolRate;
	pStatus->interleaveDepth = outParam.interleaveDepth;
	pStatus->goodRsBlockCount = outParam.goodRsBlockCount;
	pStatus->berRawCount = outParam.berRawCount;
	pStatus->dsChannelPower = outParam.dsChannelPower;
	pStatus->mainTap = outParam.mainTap;
	pStatus->equalizerGain = outParam.equalizerGain*100;
	pStatus->postRsBER = outParam.postRsBER;
	pStatus->elapsedTimeSec = outParam.elapsedTimeSec;
	pStatus->isSpectrumInverted = outParam.spectralInversion;
	pStatus->accCorrectedCount = hImplChnDev->accCorrectedCount;
	pStatus->accUncorrectedCount = hImplChnDev->accUncorrectedCount;
done:
	BDBG_LEAVE(BADS_docsis_GetStatus);
	return( retCode );
}

BERR_Code BADS_docsis_ResetStatus(
	BADS_ChannelHandle hChn				/* [in] Device channel handle */
	)
{
	BADS_docsis_ChannelHandle hImplChnDev;

	BDBG_ENTER(BADS_docsis_ResetStatus);
	BDBG_ASSERT( hChn );
	BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

	hImplChnDev = (BADS_docsis_ChannelHandle) hChn->pImpl;
	BDBG_ASSERT( hImplChnDev );
	hImplChnDev->accCorrectedCount = 0;
	hImplChnDev->accUncorrectedCount = 0;
	BDBG_LEAVE(BADS_docsis_ResetStatus);
	return( BERR_SUCCESS );
}

BERR_Code BADS_docsis_GetLockStatus(
	BADS_ChannelHandle hChn,			/* [in] Device channel handle */
    BADS_LockStatus *pLockStatus         /* [out] Returns lock status */
	)
{
	BERR_Code retCode = BERR_SUCCESS;
	BADS_docsis_ChannelHandle hImplChnDev;
#ifdef USE_RPC_GET_LOCK_STATUS
	BRPC_Param_ADS_GetLockStatus outParam;
	BRPC_Param_XXX_Get Param;
	BERR_Code retVal;
#endif

	BDBG_ENTER(BADS_docsis_GetLockStatus);
	BDBG_ASSERT( hChn );
	BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

	hImplChnDev = (BADS_docsis_ChannelHandle) hChn->pImpl;
	BDBG_ASSERT( hImplChnDev );
	BDBG_ASSERT( hImplChnDev->hRpc );
#ifndef USE_RPC_GET_LOCK_STATUS
	BKNI_AcquireMutex(hImplChnDev->mutex);
	*pLockStatus = hImplChnDev->lockStatus;
	BKNI_ReleaseMutex(hImplChnDev->mutex);
#else
	*pLockStatus = false;
	Param.devId = hImplChnDev->devId;
	CHK_RETCODE( retCode, BRPC_CallProc(hImplChnDev->hRpc, BRPC_ProcId_ADS_GetLockStatus, (const uint32_t *)&Param, sizeInLong(Param), (uint32_t *)&outParam, sizeInLong(outParam), &retVal) );
	CHK_RETCODE( retCode, retVal );
	*pLockStatus = (outParam.isQamLock == true) && (outParam.isFecLock == true);
done:
#endif
	BDBG_LEAVE(BADS_docsis_GetLockStatus);
	return( retCode );

}

BERR_Code BADS_docsis_GetSoftDecision(
	BADS_ChannelHandle hChn,			/* [in] Device channel handle */
	int16_t nbrToGet,					/* [in] Number values to get */
	int16_t *iVal,						/* [out] Ptr to array to store output I soft decision */
	int16_t *qVal,						/* [out] Ptr to array to store output Q soft decision */
	int16_t *nbrGotten					/* [out] Number of values gotten/read */
	)
{
	BERR_Code retCode = BERR_SUCCESS;
	BADS_docsis_ChannelHandle hImplChnDev;
	BRPC_Param_ADS_GetSoftDecisions outParam;
	BRPC_Param_XXX_Get Param;
	BERR_Code retVal;
	int cnt;
	unsigned int idx;

	BDBG_ENTER(BADS_docsis_GetSoftDecision);
	BDBG_ASSERT( hChn );
	BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

	hImplChnDev = (BADS_docsis_ChannelHandle) hChn->pImpl;
	BDBG_ASSERT( hImplChnDev );
	BDBG_ASSERT( hImplChnDev->hRpc );

	Param.devId = hImplChnDev->devId;
	*nbrGotten = 0;
	for( cnt = nbrToGet; cnt > 0; cnt -= MX_IQ_PER_GET )
	{

		CHK_RETCODE( retCode, BRPC_CallProc(hImplChnDev->hRpc, BRPC_ProcId_ADS_GetSoftDecision, (const uint32_t *)&Param, sizeInLong(Param), (uint32_t *)&outParam, sizeInLong(outParam), &retVal ) );
		CHK_RETCODE( retCode, retVal );
		BDBG_MSG((" BRPC_ProcId_ADS_GetSoftDecision %d", outParam.nbrGotten));
		if (outParam.nbrGotten > (unsigned)cnt) outParam.nbrGotten = cnt;
		/* Copy one block at a time */
		for( idx = 0; idx < outParam.nbrGotten; idx++ )
		{
			*iVal++ = (int16_t) outParam.iVal[idx];
			*qVal++ = (int16_t) outParam.qVal[idx];
			*nbrGotten += 1;
		}
	}
done:
	BDBG_LEAVE(BADS_docsis_GetSoftDecision);
	return( retCode );
}

BERR_Code BADS_docsis_EnablePowerSaver(
	BADS_ChannelHandle hChn				/* [in] Device channel handle */
	)
{
	BERR_Code retCode = BERR_SUCCESS;
	BADS_docsis_ChannelHandle hImplChnDev;
	BRPC_Param_ADS_EnablePowerSaver Param;
	BERR_Code retVal;

	BDBG_ENTER(BADS_docsis_EnablePowerSaver);
	BDBG_ASSERT( hChn );
	BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

	hImplChnDev = (BADS_docsis_ChannelHandle) hChn->pImpl;
	BDBG_ASSERT( hImplChnDev );
	BDBG_ASSERT( hImplChnDev->hRpc );

	Param.devId = hImplChnDev->devId;
	CHK_RETCODE( retCode, BRPC_CallProc(hImplChnDev->hRpc, BRPC_ProcId_ADS_EnablePowerSaver, (const uint32_t *)&Param, sizeInLong(Param), NULL, 0, &retVal));
	CHK_RETCODE( retCode, retVal );

done:
	BDBG_LEAVE(BADS_docsis_EnablePowerSaver);
	return( retCode );
}


BERR_Code BADS_docsis_ProcessNotification(
	BADS_ChannelHandle hChn,				/* [in] Device channel handle */
	unsigned int event						/* [in] Event code and event data*/
	)
{
	BERR_Code retCode = BERR_SUCCESS;
	BADS_docsis_ChannelHandle hImplChnDev;
	BRPC_Notification_Event event_code;
	BADS_LockStatus lockStatus;

	BDBG_ENTER(BADS_docsis_ProcessNotification);
	BDBG_ASSERT( hChn );
	BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

	hImplChnDev = (BADS_docsis_ChannelHandle) hChn->pImpl;
	BDBG_ASSERT( hImplChnDev );
	BDBG_ASSERT( hImplChnDev->hRpc );

	event_code = event>>16;

	switch (event_code) {
		case BRPC_Notification_Event_LockStatusChanged:
			lockStatus = (event&BRPC_Qam_Lock) && (event&BRPC_Fec_Lock);
			BDBG_MSG((" ADS LockStatusChanged from 3255: lockStatus? %d", lockStatus));
			BKNI_AcquireMutex(hImplChnDev->mutex);
			hImplChnDev->lockStatus = lockStatus;
			BKNI_ReleaseMutex(hImplChnDev->mutex);
			{
				if( hImplChnDev->pCallback[BADS_Callback_eLockChange] != NULL )
				{
					(hImplChnDev->pCallback[BADS_Callback_eLockChange])(hImplChnDev->pCallbackParam[BADS_Callback_eLockChange] );
				}
			}
			break;
		default:
			BDBG_WRN((" unknown event code from 3255"));
			break;
	}

	BDBG_LEAVE(BADS_docsis_ProcessNotification);
	return( retCode );
}

BERR_Code BADS_docsis_InstallCallback(
	BADS_ChannelHandle hChn,			/* [in] Device channel handle */
	BADS_Callback callbackType,			/* [in] Type of callback */
	BADS_CallbackFunc pCallback,		/* [in] Function Ptr to callback */
	void *pParam						/* [in] Generic parameter send on callback */
	)
{
	BERR_Code retCode = BERR_SUCCESS;
	BADS_docsis_ChannelHandle hImplChnDev;


	BDBG_ENTER(BADS_docsis_InstallCallback);
	BDBG_ASSERT( hChn );
	BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

	hImplChnDev = (BADS_docsis_ChannelHandle) hChn->pImpl;
	BDBG_ASSERT( hImplChnDev );

	switch( callbackType )
	{
		case BADS_Callback_eLockChange:
			hImplChnDev->pCallback[callbackType] = pCallback;
			hImplChnDev->pCallbackParam[callbackType] = pParam;
			break;
		default:
			retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
			break;
	}

	BDBG_LEAVE(BADS_docsis_InstallCallback);
	return( retCode );
}


