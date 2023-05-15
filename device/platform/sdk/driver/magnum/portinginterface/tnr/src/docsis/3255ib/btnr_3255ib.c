/***************************************************************************
 *     Copyright (c) 2005-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btnr_3255ib.c $
 * $brcm_Revision: Hydra_Software_Devel/15 $
 * $brcm_Date: 2/4/11 6:09p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/tnr/3255ib/btnr_3255ib.c $
 * 
 * Hydra_Software_Devel/15   2/4/11 6:09p mward
 * SW7125-737:  Make sure duplicate BRPC_ProcId_TNR_EnablePowerSaver are
 * not sent.
 * 
 * Hydra_Software_Devel/14   6/22/10 6:11p vishk
 * SW7420-839: Add support to program AGC gain to 31xx TNR PI
 * 
 * Hydra_Software_Devel/13   11/19/09 5:35p haisongw
 * SW7400-2619:h3255IbDev is freed if NULL instead of not NULL
 *
 * Hydra_Software_Devel/12   8/25/09 3:20p haisongw
 * SW7405-2841: Add Power Management for 3255 Inband Tuner
 *
 * Hydra_Software_Devel/11   8/25/09 9:43a vishk
 * SW7405-2841: Bcm31xx: Add Power Management support for TNR
 *
 * Hydra_Software_Devel/10   8/24/09 5:56p vishk
 * SW7405-2841: Bcm31xx: Add Power Management support for TNR
 *
 * Hydra_Software_Devel/9   7/10/09 5:28p haisongw
 * PR18598: change BDBG_WRN to BDBG_MSG
 *
 * Hydra_Software_Devel/8   3/22/07 6:13p haisongw
 * PR25908: add BTNR_Get_PowerSaver() support
 *
 * Hydra_Software_Devel/7   3/12/07 5:47p haisongw
 * PR25908: Add BTNR_3255Ib_GetPowerSaver() support
 *
 * Hydra_Software_Devel/6   12/14/06 5:30p haisongw
 * PR25908: eCM_DSG_SW_v4.x/12271: Please add RPC support for tuner
 * standby
 *
 * Hydra_Software_Devel/5   11/27/06 2:39p haisongw
 * PR25908: eCM_DSG_SW_v4.x/12271: Please add RPC support for tuner
 * standby
 *
 * Hydra_Software_Devel/4   9/7/06 3:30p haisongw
 * PR 22368: Add AOB, AUS and TNR3255ob support
 *
 * Hydra_Software_Devel/3   8/3/06 5:44p haisongw
 * PR23265: remove compiler warnings for BADS and BTNR
 *
 * Hydra_Software_Devel/2   4/11/06 4:54p dlwin
 * PR 19978: Merge 3255 support.
 *
 * Hydra_Software_Devel/PROD_DSG_REFSW_Devel/1   4/3/06 4:15p haisongw
 * PR 19978:add  tuner Settop API support on BCM97455 platform
 *
 * Hydra_Software_Devel/1   12/13/05 3:17p dlwin
 * PR 18598: Add new TNR Inband for 3255
 *
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "btnr.h"
#include "btnr_priv.h"
#include "btnr_3255ib.h"
#include "brpc.h"
#include "brpc_3255.h"

BDBG_MODULE(btnr_3255ib);

#define sizeInLong(x)	(sizeof(x)/sizeof(long))

#define	DEV_MAGIC_ID			((BERR_TNR_ID<<16) | 0xFACE)

#define	CHK_RETCODE( rc, func )			\
do {										\
	if( (rc = BERR_TRACE(func)) != BERR_SUCCESS ) \
	{										\
		goto done;							\
	}										\
} while(0)


#define	HERTZ_TO_MHZ(x)			((float) ((float) x / 1000000.0))
#define	MHZ_TO_HERTZ(x)			(x * 1000000)






/*******************************************************************************
*
*	Private Module Handles
*
*******************************************************************************/


typedef struct BTNR_P_3255Ib_Handle 			*BTNR_3255Ib_Handle;
typedef struct BTNR_P_3255Ib_Handle
{
	uint32_t magicId;					/* Used to check if structure is corrupt */
	BREG_Handle hReg;
	BINT_Handle hInt;
	BRPC_DevId devId;
	BRPC_Handle hRpc;
	unsigned long ifFreq;
	unsigned long rfFreq;
	BTNR_TunerMode tunerMode;
	BTNR_PowerSaverSettings pwrSettings;
} BTNR_P_3255Ib_Handle;



/*******************************************************************************
*
*	Default Module Settings
*
*******************************************************************************/
static const BTNR_3255Ib_Settings defDevSettings =
{
	BRPC_DevId_3255_TNR0,					/* We need this to explicitly define which tuner we are using */
	NULL,									/* BRPC handle, must be provided by application*/
	BTNR_3255Ib_SETTINGS_IFFREQ
};



/*******************************************************************************
*
*	Private Module Data
*
*******************************************************************************/
static BERR_Code BTNR_3255Ib_Close(
	BTNR_Handle hDev					/* [in] Device handle, general Tuner */
	)
{
	BERR_Code retCode = BERR_SUCCESS;
	BTNR_3255Ib_Handle hImplDev;
	BRPC_Param_TNR_Close Param;
	BERR_Code retVal;

	BDBG_ENTER(BTNR_3255Ib_Close);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

	hImplDev = (BTNR_3255Ib_Handle) hDev->hDevImpl;
	BDBG_ASSERT( hImplDev->hRpc );

	Param.devId = hImplDev->devId;
	CHK_RETCODE( retCode, BRPC_CallProc(hImplDev->hRpc, BRPC_ProcId_TNR_Close, (const uint32_t *)&Param, sizeInLong(Param), NULL, 0, &retVal) );
	CHK_RETCODE( retCode, retVal );

	hDev->magicId = 0x00;		/* clear it to catch inproper use */
	BKNI_Free( (void *) hDev->hDevImpl );
	BKNI_Free( (void *) hDev );

done:
	BDBG_LEAVE(BTNR_3255Ib_Close);
	return( retCode );
}

static BERR_Code BTNR_3255Ib_SetRfFreq(
	BTNR_3255Ib_Handle hDev,			/* [in] Device handle, specific to 3255 */
	uint32_t rfFreq,					/* [in] Requested tuner freq., in Hertz */
    BTNR_TunerMode tunerMode			/* [in] Requested tuner mode */
    )
{
	BERR_Code retCode = BERR_SUCCESS;
	BRPC_Param_TNR_SetRfFreq Param;
	BERR_Code retVal;

	BDBG_ENTER(BTNR_3255Ib_SetRfFreq);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
	BSTD_UNUSED(tunerMode);

	BDBG_MSG(("%s: enter, freq=%d", __FUNCTION__, rfFreq));

	BDBG_ASSERT( hDev->hRpc );
	Param.devId = hDev->devId;
	Param.rfFreq = rfFreq;
	Param.tunerMode = tunerMode;

	hDev->pwrSettings.enable = false; /* power saver is disabled by BRPC_ProcId_TNR_SetRfFreq */
	CHK_RETCODE( retCode, BRPC_CallProc(hDev->hRpc, BRPC_ProcId_TNR_SetRfFreq, (const uint32_t *)&Param, sizeInLong(Param), NULL, 0, &retVal) );
	CHK_RETCODE( retCode, retVal);

	hDev->rfFreq = rfFreq;
	hDev->tunerMode = tunerMode;

done:
	BDBG_LEAVE(BTNR_3255Ib_SetRfFreq);
	return( retCode );
}

static BERR_Code BTNR_3255Ib_GetRfFreq(
	BTNR_3255Ib_Handle hDev,			/* [in] Device handle, specific to 3255 */
	uint32_t *rfFreq,					/* [output] Returns tuner freq., in Hertz */
	BTNR_TunerMode *tunerMode			/* [output] Returns tuner mode */
    )
{
	BERR_Code retCode = BERR_SUCCESS;

	BDBG_ENTER(BTNR_3255Ib_GetRfFreq);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

	*rfFreq = hDev->rfFreq;
	*tunerMode = hDev->tunerMode;

	BDBG_LEAVE(BTNR_3255Ib_GetRfFreq);
	return( retCode );
}

static BERR_Code BTNR_3255Ib_GetInfo(
	BTNR_3255Ib_Handle hDev,			/* [in] Device handle, specific to 3255 */
	BTNR_TunerInfo *tnrInfo				/* [output] Tuner information */
    )
{
	BERR_Code retCode = BERR_SUCCESS;
	BRPC_Param_XXX_Get Param;
	BRPC_Param_TNR_GetVersion outParam;
	BERR_Code retVal;

	BDBG_ENTER(BTNR_3255Ib_GetInfo);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

	BDBG_ASSERT( hDev->hRpc );
	Param.devId = hDev->devId;

	CHK_RETCODE( retCode, BRPC_CallProc(hDev->hRpc, BRPC_ProcId_TNR_GetVersion, (const uint32_t *)&Param, sizeInLong(Param), (uint32_t *)&outParam, sizeInLong(outParam), &retVal) );
	CHK_RETCODE( retCode, retVal );

	tnrInfo->tunerMaker = outParam.manafactureId;
	tnrInfo->tunerId = outParam.modelId;
	tnrInfo->tunerMajorVer = outParam.majVer;
	tnrInfo->tunerMinorVer = outParam.minVer;

done:
	BDBG_LEAVE(BTNR_3255Ib_GetInfo);
	return( retCode );
}


static BERR_Code BTNR_3255Ib_GetAgcVal(
	BTNR_3255Ib_Handle hDev,			/* [in] Device handle, specific to 3255 */
	uint32_t regOffset,					/* [in] AGC register offset */
	uint32_t *agcVal					/* [out] output value */
	)
{
	BERR_Code retCode = BERR_SUCCESS;
	BRPC_Param_XXX_Get Param;
	BRPC_Param_TNR_GetAgcVal outParam;
	BERR_Code retVal;

	BDBG_ENTER(BTNR_3255Ib_GetAgcVal);
	BSTD_UNUSED(regOffset);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
	BDBG_ASSERT( hDev->hRpc );
	*agcVal = 0;

	Param.devId = hDev->devId;
	CHK_RETCODE( retCode, BRPC_CallProc(hDev->hRpc, BRPC_ProcId_TNR_GetVersion, (const uint32_t *)&Param, sizeInLong(Param), (uint32_t *)&outParam, sizeInLong(outParam), &retVal) );
	CHK_RETCODE( retCode, retVal );
	*agcVal = outParam.AgcVal;

done:
	BDBG_LEAVE(BTNR_3255Ib_GetAgcVal);
	return( retCode );
}

static BERR_Code BTNR_3255Ib_SetPowerSaver(
	BTNR_3255Ib_Handle hDev,			/* [in] Device handle */
	BTNR_PowerSaverSettings *pwrSettings	/* Tuner power saver information */
	)
{
	BERR_Code retCode = BERR_SUCCESS;
	BRPC_Param_TNR_EnablePowerSaver Param;
	BERR_Code retVal;
	BDBG_ENTER(BTNR_3255Ib_SetPowerSaver);

	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->hRpc );

	if (!hDev->pwrSettings.enable && pwrSettings->enable)
	{
		Param.devId = hDev->devId;
		hDev->pwrSettings.enable = pwrSettings->enable;
		CHK_RETCODE( retCode, BRPC_CallProc(hDev->hRpc, BRPC_ProcId_TNR_EnablePowerSaver, (const uint32_t *)&Param, sizeInLong(Param), NULL, 0, &retVal) );
		CHK_RETCODE( retCode, retVal );
	}
	else
	{
		BDBG_MSG((" auto enabled by BTNR_SetRfFreq()"));
	}
done:
	BDBG_LEAVE(BTNR_3255Ib_SetPowerSaver);
	return( retCode );
}


static BERR_Code BTNR_3255Ib_GetPowerSaver(
	BTNR_3255Ib_Handle hDev,			/* [in] Device handle */
	BTNR_PowerSaverSettings *pwrSettings	/* Tuner power saver information */
	)
{
	BERR_Code retCode = BERR_SUCCESS;
	BRPC_Param_XXX_Get Param;
	BRPC_Param_TNR_GetPowerSaver outParam;
	BERR_Code retVal;

	BDBG_ENTER(BTNR_3255Ib_GetPowerSaver);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->hRpc );
	Param.devId = hDev->devId;
	pwrSettings->enable = false;
	CHK_RETCODE( retCode, BRPC_CallProc(hDev->hRpc, BRPC_ProcId_TNR_GetPowerSaver, (const uint32_t *)&Param, sizeInLong(Param), (uint32_t *)&outParam, sizeInLong(outParam), &retVal) );
	CHK_RETCODE( retCode, retVal );
	pwrSettings->enable = outParam.powersaver;
	hDev->pwrSettings.enable = outParam.powersaver;
done:
	BDBG_LEAVE(BTNR_3255Ib_GetPowerSaver);
	return( retCode );
}


/*******************************************************************************
*
*	Public Module Functions
*
*******************************************************************************/

BERR_Code BTNR_3255Ib_Open(
	BTNR_Handle *phDev,					/* [output] Returns handle */
	BCHP_Handle hChip,					/* [in] Chip handle */
	BREG_Handle hRegister,				/* [in] Register handle */
	BINT_Handle hInterrupt,				/* [in] Interrupt handle, Bcm3250 */
	const BTNR_3255Ib_Settings *pDefSettings /* [in] Default settings */
	)
{
	BERR_Code retCode = BERR_SUCCESS;
 	BTNR_3255Ib_Handle h3255IbDev;
	BTNR_Handle hDev = NULL;
	BRPC_Param_TNR_Open Param;
	BERR_Code retVal;

	BDBG_ENTER(BTNR_3255Ib_Open);
	BSTD_UNUSED(hChip);

	/* Alloc memory from the system heap */
	h3255IbDev = (BTNR_3255Ib_Handle) BKNI_Malloc( sizeof( BTNR_P_3255Ib_Handle ) );
	if( h3255IbDev == NULL )
	{
		retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		BDBG_ERR(("BTNR_3255Ib_Open: BKNI_malloc() failed\n"));
		goto done;
	}
	BKNI_Memset( h3255IbDev, 0x00, sizeof( BTNR_P_3255Ib_Handle ) );

	hDev = (BTNR_Handle) BKNI_Malloc( sizeof( BTNR_P_Handle ) );
	if( hDev == NULL )
	{
		retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		BDBG_ERR(("BTNR_3255Ib_Open: BKNI_malloc() failed\n"));
		goto done;
	}
	BKNI_Memset( hDev, 0x00, sizeof( BTNR_P_Handle ) );

	h3255IbDev->magicId = DEV_MAGIC_ID;
	h3255IbDev->hReg = hRegister;
	h3255IbDev->hInt = hInterrupt;
	h3255IbDev->ifFreq = pDefSettings->ifFreq;
	h3255IbDev->hRpc = pDefSettings->hGeneric;
	h3255IbDev->devId = pDefSettings->devId;
	BDBG_ASSERT( h3255IbDev->hRpc );

	Param.devId = h3255IbDev->devId;
	Param.ifFreq = h3255IbDev->ifFreq;
	CHK_RETCODE( retCode, BRPC_CallProc(h3255IbDev->hRpc, BRPC_ProcId_TNR_Open, (const uint32_t *)&Param, sizeInLong(Param), NULL, 0, &retVal) );
	CHK_RETCODE( retCode, retVal );

	hDev->hDevImpl = (void *) h3255IbDev;
	hDev->magicId = DEV_MAGIC_ID;
	hDev->pSetRfFreq = (BTNR_SetRfFreqFunc) BTNR_3255Ib_SetRfFreq;
	hDev->pGetRfFreq = (BTNR_GetRfFreqFunc) BTNR_3255Ib_GetRfFreq;
	hDev->pGetAgcRegVal = (BTNR_GetAgcRegValFunc) BTNR_3255Ib_GetAgcVal;
	hDev->pSetAgcRegVal = (BTNR_SetAgcRegValFunc) NULL;
	hDev->pGetInfo = (BTNR_GetInfoFunc) BTNR_3255Ib_GetInfo;
	hDev->pClose = (BTNR_CloseFunc) BTNR_3255Ib_Close;
	hDev->pGetPowerSaver = (BTNR_GetPowerSaverFunc)BTNR_3255Ib_GetPowerSaver;
	hDev->pSetPowerSaver = (BTNR_SetPowerSaverFunc)BTNR_3255Ib_SetPowerSaver;

done:
	if( retCode != BERR_SUCCESS )
	{
		if( hDev != NULL )
		{
			BKNI_Free( hDev );
		}
		if( h3255IbDev != NULL )
		{
			BKNI_Free( h3255IbDev );
		}
		hDev = NULL;
	}
	*phDev = hDev;
	BDBG_LEAVE(BTNR_3255Ib_Open);
	return( retCode );
}

BERR_Code BTNR_3255Ib_GetDefaultSettings(
	BTNR_3255Ib_Settings *pDefSettings,	/* [output] Returns default setting */
	BCHP_Handle hChip					/* [in] Chip handle */
	)
{
	BERR_Code retCode = BERR_SUCCESS;


	BDBG_ENTER(BTNR_3255Ib_GetDefaultSettings);
	BSTD_UNUSED(hChip);

	*pDefSettings = defDevSettings;

	BDBG_LEAVE(BTNR_3255Ib_GetDefaultSettings);
	return( retCode );
}




