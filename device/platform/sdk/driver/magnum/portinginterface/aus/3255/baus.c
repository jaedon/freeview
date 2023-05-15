/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: baus.c $
 * $brcm_Revision: Hydra_Software_Devel/8 $
 * $brcm_Date: 9/1/10 5:05p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/aus/3255/baus.c $
 * 
 * Hydra_Software_Devel/8   9/1/10 5:05p haisongw
 * SW7125-265: Coverity Defect ID:20445 REVERSE_INULL baus.c Product=97125
 *
 * Hydra_Software_Devel/7   6/30/10 1:50p haisongw
 * SW7125-265:Coverity Defect ID:20445 REVERSE_INULL baus.c Product=97125
 *
 * Hydra_Software_Devel/6   5/5/08 6:02p haisongw
 * PR28691: Add power saving operation for OOB upstream since Docsis 2.0.0
 * Beta1 release
 *
 * Hydra_Software_Devel/5   6/13/07 6:17p haisongw
 * PR28691: fix typo in  BAUS_Close()
 *
 * Hydra_Software_Devel/4   3/12/07 5:52p haisongw
 * PR28647: add DVS178 upstream support
 *
 * Hydra_Software_Devel/3   9/7/06 7:15p haisongw
 * PR 22368: Add AOB, AUS and TNR3255ob support
 *
 * Hydra_Software_Devel/2   8/25/06 6:58p haisongw
 * PR 22368: Add AOB, AUS and TNR3255ob support
 *
 *
 ***************************************************************************/
#include "bstd.h"
#include "baus.h"
#include "baus_priv.h"


BDBG_MODULE(baus);

#define sizeInLong(x)	(sizeof(x)/sizeof(long))

#define	CHK_RETCODE( rc, func )				\
do {										\
	if( (rc = BERR_TRACE(func)) != BERR_SUCCESS ) \
	{										\
		goto done;							\
	}										\
} while(0)


#define	DEV_MAGIC_ID			((BERR_AUS_ID<<16) | 0xFACE)

#define	AUS_MX_PREMABLE_SZ		(0x7F)
#define	ONE_TO_MEGA(x)			((float) ((double) x / 1000000.0))
#define	MEGA_TO_ONE(x)			(x * 1000000)
#define	HERTZ_TO_MHZ(x)			ONE_TO_MEGA(x)
#define	MHZ_TO_HERTZ(x)			MEGA_TO_ONE(x)
#define	SYMBOL_TO_MSYM(x)		ONE_TO_MEGA(x)
#define	MSYM_TO_SYMBOL(x)		MEGA_TO_ONE(x)


/*******************************************************************************
*
*	Default Module Settings
*
*******************************************************************************/
static const BAUS_Settings defDevSettings =
{
	BRPC_DevId_3255_US0,					/* TODO: Is this required? */
	NULL,									/* BRPC handle, must be provided by application*/
	BAUS_SETTINGS_XTALFREQ,
	BAUS_SETTINGS_OPERATIONMODE
};

/*******************************************************************************
*
*	Private Module Functions
*
*******************************************************************************/
typedef enum
{
	BAUS_TransmitMode_eDisable = 0,
	BAUS_TransmitMode_eEnable
} BAUS_TransmitMode;

static BERR_Code BAUS_SetTransmitMode(
	BAUS_Handle hDev,						/* [in] Device handle */
	BAUS_TransmitMode transmitMode			/* [in] Requested transmit state */
	)
{
	BERR_Code retCode = BERR_SUCCESS;
	BRPC_Param_AUS_SetSetTransmitMode Param;
	BERR_Code retVal;

	BDBG_ENTER(BAUS_SetTransmitMode);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
	BDBG_ASSERT( hDev->hRpc );


	if( transmitMode == BAUS_TransmitMode_eDisable )
	{
		Param.devId = hDev->devId;
		Param.transmitMode = 0;
		CHK_RETCODE( retCode, BRPC_CallProc(hDev->hRpc, BRPC_ProcId_AUS_SetTransmitMode, (const uint32_t *)&Param, sizeInLong(Param), NULL, 0, &retVal));
		CHK_RETCODE( retCode, retVal );
		hDev->settings.isTransmitterEnabled = false;
		BDBG_MSG(("%s: Upstream Transmitter Disabled", __FUNCTION__));
	}
	else if( transmitMode == BAUS_TransmitMode_eEnable )
	{
		Param.devId = hDev->devId;
		Param.transmitMode = 1;
		CHK_RETCODE( retCode, BRPC_CallProc(hDev->hRpc, BRPC_ProcId_AUS_SetTransmitMode, (const uint32_t *)&Param, sizeInLong(Param), NULL, 0, &retVal));
		CHK_RETCODE( retCode, retVal );
		hDev->settings.isTransmitterEnabled = true;
		BDBG_MSG(("%s: Upstream Transmitter Enabled", __FUNCTION__));
	}
	else
	{
		retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
	}

done:
	BDBG_LEAVE(BAUS_SetTransmitMode);
	return( retCode );
}


/*******************************************************************************
*
*	Public Module Functions
*
*******************************************************************************/
BERR_Code BAUS_Open(
	BAUS_Handle *pAus,						/* [out] Returns handle */
	BCHP_Handle hChip,						/* [in] Chip handle */
	BREG_Handle hRegister,					/* [in] Register handle */
	const BAUS_Settings *pDefSettings		/* [in] Default settings */
	)
{
	BERR_Code retCode = BERR_SUCCESS;
 	BAUS_Handle hDev;
	BRPC_Param_AUS_Open Param;
	BERR_Code retVal;

	BDBG_ENTER(BAUS_Open);

	BSTD_UNUSED( hChip );
	BSTD_UNUSED( hRegister );

	*pAus = NULL;

	/* Alloc memory from the system heap */
	hDev = (BAUS_Handle) BKNI_Malloc( sizeof( BAUS_P_Handle ) );
	if( hDev == NULL )
	{
		retCode = BERR_OUT_OF_SYSTEM_MEMORY;
		BDBG_ERR(("BAUS_Open: BKNI_malloc() failed\n"));
		goto done;
	}
	if( pDefSettings == NULL )
	{
		retCode = BERR_INVALID_PARAMETER;
		BDBG_ERR(("BAUS_Open: Invalid BAUS_Settings\n"));
		goto done;
	}
	BKNI_Memset( hDev, 0x00, sizeof( BAUS_P_Handle ) );

	hDev->magicId = DEV_MAGIC_ID;
	hDev->hChip = hChip;
	hDev->hRegister = hRegister;
	hDev->hRpc = pDefSettings->hGeneric;	/* For this device, we need the RPC handle */
	hDev->devId = pDefSettings->devId;

	hDev->settings.xtalFreq = pDefSettings->xtalFreq;
	hDev->settings.operationMode = pDefSettings->operationMode;
	hDev->settings.isTransmitterEnabled = false;
	hDev->settings.premablePatternSz = 0;
	hDev->settings.rfFreq = 0;
	hDev->settings.powerLevel = 0;
	hDev->settings.symbolRate = 0;

	Param.devId = hDev->devId;
	Param.xtalFreq = pDefSettings->xtalFreq;

	CHK_RETCODE( retCode, BRPC_CallProc(hDev->hRpc, BRPC_ProcId_AUS_Open, (const uint32_t *)&Param, sizeInLong(Param), NULL, 0, &retVal));
	CHK_RETCODE( retCode, retVal );

	*pAus = hDev;

done:
	if( retCode != BERR_SUCCESS )
	{
		if( hDev != NULL )
		{
			BKNI_Free( (void *) hDev );
		}
	}
	BDBG_LEAVE(BAUS_Open);
	return( retCode );
}

BERR_Code BAUS_Close(
	BAUS_Handle hDev						/* [in] Device handle */
	)
{
	BERR_Code retCode = BERR_SUCCESS;
	BRPC_Param_AUS_Close Param;
	BERR_Code retVal;

	BDBG_ENTER(BAUS_Close);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
	BDBG_ASSERT( hDev->hRpc );

	Param.devId = hDev->devId;
	CHK_RETCODE( retCode, BRPC_CallProc(hDev->hRpc, BRPC_ProcId_AUS_Close, (const uint32_t *)&Param, sizeInLong(Param), NULL, 0, &retVal));
	CHK_RETCODE( retCode, retVal );


	hDev->magicId = 0x00;		/* clear it to catch inproper use */
	BKNI_Free( (void *) hDev );

done:
	BDBG_LEAVE(BAUS_Close);
	return( retCode );
}

BERR_Code BAUS_GetDefaultSettings(
	BAUS_Settings *pDefSettings,			/* [out] Returns default setting */
	BCHP_Handle hChip						/* [in] Chip handle */
	)
{
	BERR_Code retCode = BERR_SUCCESS;

	BSTD_UNUSED(hChip);

	BDBG_ENTER(BAUS_GetDefaultSettings);

	*pDefSettings = defDevSettings;

	BDBG_LEAVE(BAUS_GetDefaultSettings);
	return( retCode );
}

BERR_Code BAUS_SetOperationMode(
	BAUS_Handle hDev,						/* [in] Device handle */
	BAUS_OperationMode operationMode		/* [in] Requested operation mode */
	)
{
	BERR_Code retCode = BERR_SUCCESS;
	BRPC_Param_AUS_SetOperationMode Param;
	BERR_Code retVal;

	BDBG_ENTER(BAUS_SetOperationMode);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
	BDBG_ASSERT( hDev->hRpc );

	switch( operationMode )
	{
		case BAUS_OperationMode_eAnnexA:
		case BAUS_OperationMode_eDvs178:
		case BAUS_OperationMode_eDocsis:
		case BAUS_OperationMode_ePod:
		case BAUS_OperationMode_eTestCw:
		case BAUS_OperationMode_eTestPn23:
		case BAUS_OperationMode_ePodAnnexA:
		case BAUS_OperationMode_ePodDvs178:

			Param.devId = hDev->devId;
			Param.operationMode = operationMode;

			CHK_RETCODE( retCode, BRPC_CallProc(hDev->hRpc, BRPC_ProcId_AUS_SetOperationMode, (const uint32_t *)&Param, sizeInLong(Param), NULL, 0, &retVal));
			CHK_RETCODE( retCode, retVal );

			hDev->settings.operationMode = operationMode;
			break;
		default:
			retCode = BERR_INVALID_PARAMETER;
			break;
	}

done:
	BDBG_LEAVE(BAUS_SetOperationMode);
	return( retCode );
}

BERR_Code BAUS_GetOperationMode(
	BAUS_Handle hDev,						/* [in] Device handle */
	BAUS_OperationMode *operationMode		/* [out] Current operation mode */
	)
{
	BERR_Code retCode = BERR_SUCCESS;


	BDBG_ENTER(BAUS_GetOperationMode);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

	*operationMode = hDev->settings.operationMode;

	BDBG_LEAVE(BAUS_GetOperationMode);
	return( retCode );
}

BERR_Code BAUS_SetSymbolRate(
	BAUS_Handle hDev,						/* [in] Device handle */
	unsigned long symbolRate				/* [in] Requested symbol rate, in baud */
	)
{
	BERR_Code retCode = BERR_SUCCESS;
	BRPC_Param_AUS_SetSymbolRate Param;
	BERR_Code retVal;

	BDBG_ENTER(BAUS_SetSymbolRate);
	BDBG_MSG(("%s: symbolRate=%d", __FUNCTION__, symbolRate));
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
	BDBG_ASSERT( hDev->hRpc );

	Param.devId = hDev->devId;
	Param.symbolRate = symbolRate;
	CHK_RETCODE( retCode, BRPC_CallProc(hDev->hRpc, BRPC_ProcId_AUS_SetSymbolRate, (const uint32_t *)&Param, sizeInLong(Param), NULL, 0, &retVal));
	CHK_RETCODE( retCode, retVal );

	hDev->settings.symbolRate = symbolRate;
done:
	BDBG_LEAVE(BAUS_SetSymbolRate);
	return( retCode );
}

BERR_Code BAUS_GetSymbolRate(
	BAUS_Handle hDev,						/* [in] Device handle */
	unsigned long *symbolRate				/* [out] Current symbol rate, in baud */
	)
{
	BERR_Code retCode = BERR_SUCCESS;


	BDBG_ENTER(BAUS_GetSymbolRate);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

	*symbolRate = hDev->settings.symbolRate;

	BDBG_LEAVE(BAUS_GetSymbolRate);
	return( retCode );
}

BERR_Code BAUS_SetRfFreq(
	BAUS_Handle hDev,						/* [in] Device handle */
	unsigned long rfFreq					/* [in] Requested RF frequency, in Hertz */
	)
{
	BERR_Code retCode = BERR_SUCCESS;
	BRPC_Param_AUS_SetRfFreq Param;
	BERR_Code retVal;


	BDBG_ENTER(BAUS_SetRfFreq);
	BDBG_MSG(("%s: rfFreq=%d", __FUNCTION__, rfFreq));
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
	BDBG_ASSERT( hDev->hRpc );

	Param.devId = hDev->devId;
	Param.rfFreq = rfFreq;
	CHK_RETCODE( retCode, BRPC_CallProc(hDev->hRpc, BRPC_ProcId_AUS_SetRfFreq, (const uint32_t *)&Param, sizeInLong(Param), NULL, 0, &retVal));
	CHK_RETCODE( retCode, retVal );

	hDev->settings.rfFreq = rfFreq;
done:
	BDBG_LEAVE(BAUS_SetRfFreq);
	return( retCode );
}

BERR_Code BAUS_GetRfFreq(
	BAUS_Handle hDev,						/* [in] Device handle */
	unsigned long *rfFreq					/* [out] Current RF frequency, in Hertz */
	)
{
	BERR_Code retCode = BERR_SUCCESS;


	BDBG_ENTER(BAUS_GetRfFreq);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

	*rfFreq = hDev->settings.rfFreq;

	BDBG_LEAVE(BAUS_GetRfFreq);
	return( retCode );
}

BERR_Code BAUS_SetPowerLevel(
	BAUS_Handle hDev,						/* [in] Device handle */
	unsigned int powerLevel					/* [in] Requested power level, in hundredth of dBmV */
	)
{
	BERR_Code retCode = BERR_SUCCESS;
	BRPC_Param_AUS_SetPowerLevel Param;
	BERR_Code retVal;


	BDBG_ENTER(BAUS_SetPowerLevel);
	BDBG_MSG(("%s: powerLevel=%d", __FUNCTION__, powerLevel));
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
	BDBG_ASSERT( hDev->hRpc );

	Param.devId = hDev->devId;
	Param.powerLevel = powerLevel;
	CHK_RETCODE( retCode, BRPC_CallProc(hDev->hRpc, BRPC_ProcId_AUS_SetPowerLevel, (const uint32_t *)&Param, sizeInLong(Param), NULL, 0, &retVal));
	CHK_RETCODE( retCode, retVal );
	hDev->settings.powerLevel = powerLevel;

done:
	BDBG_LEAVE(BAUS_SetPowerLevel);
	return( retCode );
}

BERR_Code BAUS_GetPowerLevel(
	BAUS_Handle hDev,						/* [in] Device handle */
	unsigned int *powerLevel				/* [out] Current power level, in hundredth of dBmV */
	)
{
	BERR_Code retCode = BERR_SUCCESS;


	BDBG_ENTER(BAUS_GetPowerLevel);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

	*powerLevel = hDev->settings.powerLevel;

	BDBG_LEAVE(BAUS_GetPowerLevel);
	return( retCode );
}


BERR_Code BAUS_GetStatus(
	BAUS_Handle hDev,						/* [in] Device handle */
	BAUS_Status *pStatus					/* [out] Returns status */
	)
{
#if 1 /*use local copies*/
	BERR_Code retCode = BERR_SUCCESS;

	BDBG_ENTER(BAUS_GetStatus);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

	pStatus->operationMode = hDev->settings.operationMode;
	pStatus->symbolRate = hDev->settings.symbolRate;
	pStatus->rfFreq = hDev->settings.rfFreq;
	pStatus->powerLevel = hDev->settings.powerLevel;
	pStatus->isPowerSaverEnabled = hDev->settings.isTransmitterEnabled;
	pStatus->sysXtalFreq = hDev->settings.xtalFreq;
	BDBG_LEAVE(BAUS_GetStatus);
	return( retCode );
#else
	BERR_Code retCode = BERR_SUCCESS;
	BERR_Code retVal;
	BRPC_AUS_Status usStatus;
	BRPC_Param_XXX_Get Param;

	BDBG_ENTER(BAUS_GetStatus);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

	Param.devId = hDev->devId;

	CHK_RETCODE( retCode, BRPC_CallProc(hDev->hRpc, BRPC_ProcId_AUS_GetStatus, (const uint32_t *)&Param, sizeInLong(Param), (uint32_t *)&usStatus, sizeInLong(usStatus), &retVal));
	CHK_RETCODE( retCode, retVal );

	pStatus->operationMode       = usStatus.operationMode;
	pStatus->symbolRate          = usStatus.symbolRate;
	pStatus->rfFreq              = usStatus.rfFreq;
	pStatus->powerLevel          = usStatus.powerLevel;
	pStatus->isPowerSaverEnabled = usStatus.powerSaveEnabled;
	pStatus->sysXtalFreq         = usStatus.xtalFreq;

done:
	BDBG_LEAVE(BAUS_GetStatus);
	return( retCode );

#endif
}


BERR_Code BAUS_TransmitStarvuePkt(
	BAUS_Handle hDev,						/* [in] Device handle */
    uint8_t *ucXmitBuffer,                  /* [in] Buffer to transmit */
    unsigned int size                       /* [in] Size of Buffer (in bytes) */
    )
{
	BRPC_Param_AUS_TxStarVuePkt  Param;
	int i;
	BERR_Code retVal,retCode = BERR_SUCCESS;

	BDBG_ENTER(BAUS_TransmitStarvuePkt);
	BDBG_ASSERT(ucXmitBuffer);

	if (size != 54)
	{
		BDBG_ERR(("BAUS_Xmit_Starvue_Pkt: Packet Size Must be 54 bytes!"));
		retCode = BERR_INVALID_PARAMETER;
	}
	else
	{
		Param.devId = BRPC_DevId_3255_US0;

		/* Move the buffer into an array of longs */
		for (i = 0; i < 13; i++)
		{
			Param.svBuffer[i] = (ucXmitBuffer[(i*4)  ] << 24) | (ucXmitBuffer[(i*4)+1] << 16) |
								(ucXmitBuffer[(i*4)+2] << 8)  | (ucXmitBuffer[(i*4)+3]) ;
		}
		Param.svBuffer[i] = (ucXmitBuffer[i*4] << 24) | (ucXmitBuffer[(i*4)+1] << 16);    /* Last 2 bytes*/

		CHK_RETCODE( retCode,BRPC_CallProc(hDev->hRpc, BRPC_ProcId_AUS_TxStarVuePkt, (const uint32_t *)&Param, sizeInLong(Param), NULL, 0, &retVal));
		if (retVal != BERR_SUCCESS)
			retCode = BERR_OUT_OF_DEVICE_MEMORY;    /* 3254 Starvue Ring Buffer is Full, Signal Error */

		BKNI_Delay(4000);	/* pace transmits to avoid overflow */
	}
done:
	BDBG_LEAVE(BAUS_TransmitStarvuePkt);
	return( retCode );
}


BERR_Code BAUS_EnableTransmitter(
	BAUS_Handle hDev						/* [in] Device handle */
	)
{
	BERR_Code retCode = BERR_SUCCESS;


	BDBG_ENTER(BAUS_EnableTransmitter);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

	CHK_RETCODE( retCode, BAUS_SetTransmitMode( hDev, BAUS_TransmitMode_eEnable ) );

done:
	BDBG_LEAVE(BAUS_EnableTransmitter);
	return( retCode );
}

BERR_Code BAUS_DisableTransmitter(
	BAUS_Handle hDev						/* [in] Device handle */
	)
{
	BERR_Code retCode = BERR_SUCCESS;


	BDBG_ENTER(BAUS_DisableTransmitter);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

 	CHK_RETCODE( retCode, BAUS_SetTransmitMode( hDev, BAUS_TransmitMode_eDisable ) );

done:
	BDBG_LEAVE(BAUS_DisableTransmitter);
	return( retCode );
}


#if 0
BERR_Code BAUS_SetPreamblePattern(
	BAUS_Handle hDev,						/* [in] Device handle */
	uint8_t *pPattern,						/* [in, size_is(patternSize)] Requested premable pattern */
	unsigned patternSize					/* [in] Size of premable pattern */
	)
{
	BERR_Code retCode = BERR_SUCCESS;
	BRPC_Param_AUS_SetPreamblePattern Param;
	BERR_Code retVal;
	unsigned idx;

	BDBG_ENTER(BAUS_SetPreamblePattern);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
	BDBG_ASSERT( hDev->hRpc );

	if( patternSize <= AUS_MX_PREMABLE_SZ )
	{
		Param.devId = hDev->devId;
		for( idx = 0; idx < patternSize; idx++ )
		{
			Param.Pattern[idx] = pPattern[idx];
		}
		Param.patternSize = patternSize;
		CHK_RETCODE( retCode, BRPC_CallProc(hDev->hRpc, BRPC_ProcId_AUS_SetPreamblePattern, (const uint32_t *)&Param, sizeInLong(Param), NULL, 0, &retVal));
		CHK_RETCODE( retCode, retVal );
		hDev->settings.premablePatternSz = patternSize;
	}
	else
	{
		retCode = BERR_INVALID_PARAMETER;
	}


done:
	BDBG_LEAVE(BAUS_SetPreamblePattern);
	return( retCode );
}

BERR_Code BAUS_SetBurstProfile(
	BAUS_Handle hDev,						/* [in] Device handle */
	BAUS_BurstBank bankNbr,					/* [in] Burst bank to program */
	const BAUS_BurstProfile *burstProfile	/* [in] Burst bank profile to program */
	)
{
	BERR_Code retCode = BERR_SUCCESS;
	BRPC_Param_AUS_SetBurstProfile Param;
	BERR_Code retVal;


	BDBG_ENTER(BAUS_SetBurstProfile);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
	BDBG_ASSERT( hDev->hRpc );

	if( bankNbr <= BAUS_BurstBank_eNbr5 )
	{
		Param.devId = hDev->devId;
		Param.burstBank = bankNbr;
		Param.modulationType = burstProfile->modulationType;
		Param.diffEncodingOnOff = burstProfile->diffEncodingOnOff;
		Param.preambleLength = burstProfile->preambleLength;
		Param.preambleValOffset= burstProfile->preambleValOffset;
		Param.fecBytes = burstProfile->fecBytes;
		Param.fecCodewordInfoBytes= burstProfile->fecCodewordInfoBytes;
		Param.scramblerSeed = burstProfile->scramblerSeed;
		Param.maxBurstSize = burstProfile->maxBurstSize;
		Param.guardTimeSize = burstProfile->guardTimeSize;
		Param.lastCodewordLength = burstProfile->lastCodewordLength;
		Param.scramblerOnOff = burstProfile->scramblerOnOff;
		Param.nbrPreambleDiffEncoding = burstProfile->nbrPreambleDiffEncoding;
		CHK_RETCODE( retCode, BRPC_CallProc(hDev->hRpc, BRPC_ProcId_AUS_SetBurstProfile, (const uint32_t *)&Param, sizeInLong(Param), NULL, 0, &retVal));
		CHK_RETCODE( retCode, retVal );
	}
	else
	{
		retCode = BERR_INVALID_PARAMETER;
	}

done:
	BDBG_LEAVE(BAUS_SetBurstProfile);
	return( retCode );
}
#endif


