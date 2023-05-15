/**
 * @file	drv_channel_t.c
 * @brief	TER channel C source
 * @version $Revision: 1.0.0 $
 * @date 	$Date: 2007/06/13 11:00:00 $
 * @author 	???
 */

/** @defgroup CH_TER TER channel
 *  This is the module of CH TER.
 *  @{
 */

/******************************************************************************
 * Include Files
 ******************************************************************************/
#include "htype.h"
#include "vkernel.h"

#include "di_channel.h"
#include "di_channel_priv.h"
#include "di_channel_attr.h"

#include "drv_i2c.h"
#include "drv_gpio.h"

#include "drv_channel.h"
#include "drv_channel_config.h"

#if defined (CONFIG_PLATFORM_DT90507)
#include "drv_channel_dt90507.h"
#endif

#if defined (CONFIG_PLATFORM_DT90517)
#include "drv_channel_dt90517.h"
#endif

#if defined (CONFIG_PLATFORM_DS2820)
#include "drv_channel_cxd2820.h"
#endif

#if defined (CONFIG_PLATFORM_MXL683)
#include "drv_channel_mxl683.h"
#endif

/******************************************************************************
 * Symbol and Macro Definitions
 ******************************************************************************/
#define 	OFDM_UNLOCKED				0
#define 	OFDM_LOCKED				1
#define 	OFDM_CHECK_CANCELED		2

#define	WAIT_LOCK_TOTAL			50 		/*150 */
#define	LOCK_TRIAL					1
#define	ADC_POWER_ON_OFF_MASK	  	0x10

//#define CH_PRINT_LOW_LEVEL_ZAPPING_TIME
/******************************************************************************
 * Data Types
 ******************************************************************************/

/******************************************************************************
 * Local(static) Data Types
 ******************************************************************************/

/******************************************************************************
 * Extern Variables
 ******************************************************************************/
extern HINT32 show_di_ch;

/******************************************************************************
 * Global Variables
 ******************************************************************************/
#ifdef CH_PRINT_LOW_LEVEL_ZAPPING_TIME
HUINT32 g_ulRequestTune[CONFIG_NUM_OF_TER_CHANNEL];
HUINT32 g_ulLowLevelLockedTime[CONFIG_NUM_OF_TER_CHANNEL] , g_ulLowLevelUnlockedTime[CONFIG_NUM_OF_TER_CHANNEL] ;
#endif
/******************************************************************************
 * Local(static) Variables
 ******************************************************************************/
static CH_TER_AntStatus_t s_stAntState[CONFIG_NUM_OF_TER_CHANNEL];

/******************************************************************************
 * Extern Function Prototypes
 ******************************************************************************/

/******************************************************************************
 * Local(static) Function Prototypes
 ******************************************************************************/

#define __CH_T_PRIVATE_FUNCITONS__




/**
 * This function register Front key callback.
 *
 * @param[in]   pfnNotify    Callback function
 *
 * @return      DDI_Err_eSUCCESS on success, or other DDI_Err_t on failure
 *
 * @remarks     This function is asynchronous.
 *
 * @see         DDI_KEY_DeRegisterCallBack
 */
#define __CH_T_PUBLIC_FUNCITONS__



DI_CHANNEL_ERR CH_T_ReInitDevice( int nChannelId )
{
	int nResult = DI_CH_OK;

	CH_DI_Print(CH_LOG_4,("CH_T_ReInitDevice Enter ...\n"));

	/* Initialise HW */
	nResult = TER_ReInitDevice(nChannelId);

	return nResult;
}

DI_CHANNEL_ERR CH_T_UnInitDevice( int nChannelId )
{

    int nResult = DI_CH_OK;

    CH_DI_Print(CH_LOG_4,("CH_T_UnInitDevice Enter ...\n"));

    /* uninitialise HW */
    nResult = TER_UnInitDevice(nChannelId);

    return nResult;
}


DI_CHANNEL_ERR CH_T_InitDev( int nChannelId )
{
	int nResult = DI_CH_OK;
	HUINT8 unitId;

	CH_TsOutMode_e etOutputMode;

	CH_DI_Print(CH_LOG_4,("CH_T_InitDev Enter ...\n"));

	unitId = (HUINT8)GetDeviceUnitId( nChannelId );

	DRV_T_InstallApi();

	/* Initialise HW */
#if !defined (CONFIG_PLATFORM_MXL683)
	nResult = TER_InitDevice(nChannelId);
#else
{
	int i=0;
	while (i<3)
	{
		nResult = TER_InitDevice(nChannelId);

		if (nResult != DI_CH_OK)
			i++;
		else
			break;

		VK_TASK_Sleep(10);
	}
}
#endif

	etOutputMode = DRV_CH_GetTSOutputMode(nChannelId);
	CH_DI_Print(CH_LOG_4,("TS Output Mode = %d \n", etOutputMode));
	CH_T_SetTsOutputMode(nChannelId, etOutputMode);

	CH_T_DisableTsOutput(nChannelId);

#if !defined (CONFIG_PLATFORM_DT90517)
	CH_T_SetPowerState( nChannelId, CH_POWER_OFF );
#endif
	s_stAntState[unitId].etDesiredAntState = DI_CH_ANTENNA_OFF;
	s_stAntState[unitId].etCurrentAntState = DI_CH_ANTENNA_OFF;

	return nResult;
}


DI_CHANNEL_ERR CH_T_SetTune( int nChannelId, HUINT32 unWorkingId )
{
	int 				nResult = DI_CH_OK;
	unsigned char		lock_ofdm = 0;

#ifdef CH_PRINT_LOW_LEVEL_ZAPPING_TIME
	g_ulRequestTune[nChannelId] = VK_TIMER_GetSystemTick();
#endif

	DRV_CH_Lock( nChannelId );

	CH_DI_Print(CH_LOG_4,("[%s] Enter ...\n", __FUNCTION__));

	nResult = TER_SetTune(nChannelId, &lock_ofdm, unWorkingId);

	DRV_CH_UnLock( nChannelId );

	if( nResult == DI_CH_CANCEL_TUNE )
	{
		CH_PrintFunc(">>> TER DI_CH_CANCEL_TUNE\n");
		return DI_CH_CANCEL_TUNE;
	}

	if( lock_ofdm == OFDM_LOCKED )
	{
		//CH_DI_Print(2,(">>> TER CH_STATUS_LOCKED\n"));

		CH_T_EnableTsOutput(nChannelId);
		SetTuneStatus( nChannelId, CH_STATUS_LOCKED );
		CH_DI_Print(CH_LOG_1, (">>> [%d] TER CH_STATUS_LOCKED \n",nChannelId));

#ifdef CH_PRINT_LOW_LEVEL_ZAPPING_TIME
		g_ulLowLevelLockedTime[nChannelId] = VK_TIMER_GetSystemTick();
		CH_DI_Print(CH_LOG_0, ("###########################################\n" ));
		CH_DI_Print(CH_LOG_0, ("## [CH_T_SetTune] [%d] Lock Time( %d )ms ## \n" ,nChannelId, g_ulLowLevelLockedTime[nChannelId] - g_ulRequestTune[nChannelId] ));
		CH_DI_Print(CH_LOG_0, ("###########################################\n" ));
#endif

		DRV_CH_CallNotifyCallback( nChannelId, unWorkingId, DI_CH_SIGNAL_LOCKED );
	}
	else
	{
		//CH_DI_Print(2,(">>> TER CH_STATUS_UNLOCK\n"));

		SetTuneStatus( nChannelId, CH_STATUS_UNLOCK );

		CH_DI_Print(CH_LOG_1,(">>> [%d] TER CH_STATUS_UNLOCK \n",nChannelId));

#ifdef CH_PRINT_LOW_LEVEL_ZAPPING_TIME
		g_ulLowLevelUnlockedTime[nChannelId] = VK_TIMER_GetSystemTick();
		CH_DI_Print(CH_LOG_0, ("[CH_T_SetTune] [%d] UnLock Time( %d )ms  \n", nChannelId, g_ulLowLevelUnlockedTime[nChannelId] - g_ulRequestTune[nChannelId] ));
#endif

		DRV_CH_CallNotifyCallback( nChannelId, unWorkingId, DI_CH_SIGNAL_UNLOCK );
	}

	return nResult;
}

int CH_T_GetSignalInfo (int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo)
{
	int nResult = DI_CH_OK;

	DRV_CH_Lock( nChannelId );

	nResult = TER_GetSignalInfo(nChannelId, pstSignalInfo);

	if(nResult != DI_CH_OK)
	{
		 pstSignalInfo->ulStrength =0;
		 pstSignalInfo->ulQuality = 0;
		 pstSignalInfo->ulAgc = 0;
		 pstSignalInfo->fSnr = 0;
	}

	DRV_CH_UnLock( nChannelId );

	return nResult;
}



void CH_T_SetPowerState( int nChannelId, CH_Power_e etPower )
{
	int nResult = DI_CH_OK;

	DRV_CH_Lock( nChannelId );

	nResult = TER_SetPowerState(nChannelId, etPower);

	DRV_CH_UnLock( nChannelId );

	return;
}


int CH_T_SetTsOutputMode ( int nChannelId, CH_TsOutMode_e etTsMode )
{
	int nResult = DI_CH_OK;

	DRV_CH_Lock( nChannelId );

	nResult = TER_SetTsOutputMode(nChannelId, etTsMode);

	DRV_CH_UnLock( nChannelId );

	return nResult;

}



int CH_T_EnableTsOutput ( int nChannelId )
{
	int nResult = DI_CH_OK;

	DRV_CH_Lock( nChannelId );

	nResult = TER_EnableTsOutput(nChannelId);

	SetTsStatus( nChannelId, CH_TS_PATH_ON );

	DRV_CH_UnLock( nChannelId );

	return nResult;
}



int CH_T_DisableTsOutput ( int nChannelId )
{
	int nResult = DI_CH_OK;

	DRV_CH_Lock( nChannelId );

	nResult = TER_DisableTsOutput(nChannelId);

	SetTsStatus( nChannelId, CH_TS_PATH_OFF );

	DRV_CH_UnLock( nChannelId );

	return nResult;
}

HBOOL CH_T_CheckLock ( int nChannelId, HUINT32 unWorkingID )
{
	HBOOL bLocked = TRUE;

	DRV_CH_Lock( nChannelId );

	bLocked = TER_CheckLock( nChannelId, unWorkingID );

	if( bLocked == FALSE )
	{
		VK_TASK_Sleep(100);//wait 100ms

		bLocked = TER_CheckLock( nChannelId, unWorkingID );
		if( bLocked == FALSE )
		{
			CH_DI_Print(CH_LOG_4,("Unlock!!!"));
		}
	}

	DRV_CH_UnLock( nChannelId );

	return bLocked;
}

void CH_T_CheckStatus ( int nChannelId, HUINT32 unWorkingId )
{

	CH_Status_e		etStatusOfChannel;
	HBOOL	bLocked = FALSE;
	int nAntState;

	etStatusOfChannel = GetTuneStatus( nChannelId );

	nAntState = TER_CheckAntState( nChannelId );
	if( nAntState == DI_CH_ANTENNA_FAULT )
	{
		SetTuneStatus( nChannelId, CH_STATUS_ANT_FAULT );
		etStatusOfChannel = GetTuneStatus( nChannelId );
		DRV_CH_CallNotifyCallback( nChannelId, unWorkingId, DI_CH_SIGNAL_ANTENNA_FAULT );
	}

	switch ( etStatusOfChannel )
	{
		case CH_STATUS_IDLE:
			break;

		case CH_STATUS_LOCKING:
			break;

		case CH_STATUS_LOCKED:
			bLocked = TER_CheckLock ( nChannelId, unWorkingId );
			if( bLocked != TRUE )
			{
				SetTuneStatus( nChannelId, CH_STATUS_UNLOCK );

				DRV_CH_CallNotifyCallback( nChannelId, unWorkingId, DI_CH_SIGNAL_UNLOCK );
			}
			else
			{
				//do nothing
			}

			break;

		case CH_STATUS_CANCLE:
			break;

		case CH_STATUS_UNLOCK:
			CH_T_SetTune( nChannelId, unWorkingId );
			break;

		case CH_STATUS_ANT_FAULT:
			TER_ProcessRecoverAntenna( nChannelId, unWorkingId );
			break;

		default:
			CH_DI_Print(CH_LOG_4, ("etStatusOfChannel(%d)", etStatusOfChannel));
			break;

	}

	return;

}

int CH_T_SetAntState( int nChannelId, CH_TER_AntStatus_e etAntState )
{
	return TER_SetAntState(nChannelId, etAntState);
}

CH_TER_AntStatus_e CH_T_GetAntState( int nChannelId )
{
	return TER_GetAntState(nChannelId);
}


int CH_T_GetStatus( int nChannelId )
{
	int nResult = DI_CH_OK;

	DRV_CH_Lock( nChannelId );

#if defined (CONFIG_PLATFORM_DT90507)
	nResult = DT90507_CheckStatus(nChannelId);

#elif defined (CONFIG_PLATFORM_DT90517)
	nResult = DT90517_CheckStatus(nChannelId);
#endif

	if( nResult != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_3,("[CH_T_GetStatus] Unlock!!!"));
		SetTuneStatus( nChannelId, CH_STATUS_UNLOCK);
	}
	else
	{
		SetTuneStatus( nChannelId, CH_STATUS_LOCKED );
	}

	DRV_CH_UnLock( nChannelId );

	return nResult;
}


int CH_T_CheckTsmfStatus( int nChannelId )
{
	int nResult = DI_CH_OK;

	DRV_CH_Lock( nChannelId );

#if defined (CONFIG_PLATFORM_DT90507)
	nResult = DT90507_CheckTsmfStatus(nChannelId);
#elif defined (CONFIG_PLATFORM_DT90517)
	nResult = DT90517_CheckTsmfStatus(nChannelId);
#endif
	if( nResult != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_2,("[CH_C_CheckTsmfStatus] Unlock!!!"));
		SetTuneStatus( nChannelId, CH_STATUS_UNLOCK);
	}
	else
	{
		SetTuneStatus( nChannelId, CH_STATUS_LOCKED );
	}

	DRV_CH_UnLock( nChannelId );

	return nResult;
}

int CH_T_GetAGC( int nChannelId, HUINT8 *ucRfAgc, HUINT8 *ucIfAgc)
{
	int nResult = DI_CH_OK;

#if defined (CONFIG_PLATFORM_DT90507)
	DT90507_GetAGC(nChannelId, ucRfAgc, ucIfAgc);
#elif defined (CONFIG_PLATFORM_DT90517)
	DT90517_GetAGC(nChannelId, ucRfAgc, ucIfAgc);
#else
	CH_UNUSED(nChannelId);
#endif
	CH_DI_Print(CH_LOG_4,("[PAL_QAM_GetAGC] RfAgc = 0x%x, IfAgc = 0x%x \n ", *ucRfAgc, *ucIfAgc));

	return nResult;
}

int CH_T_GetEwsInfo( int nChannelId, HUINT8 *bIsEws)
{
	int nResult = DI_CH_OK;

#if defined (CONFIG_PLATFORM_DT90507)
	DT90507_GetEwsInfo(nChannelId, bIsEws);
#elif defined (CONFIG_PLATFORM_DT90517)
	DT90517_GetEwsInfo(nChannelId, bIsEws);
#else
	CH_UNUSED(nChannelId);
#endif

	CH_DI_Print(CH_LOG_4, ("[CH_C_GetEwsInfo] bIsEws= %d \n ", *bIsEws));

	return nResult;
}

int CH_T_BERCalTime( int nChannelId, HUINT32 berCalMode)
{
	int nResult = DI_CH_OK;

#if defined (CONFIG_PLATFORM_DT90507)
	DT90507_SetBERCalTime(nChannelId, berCalMode);
#elif defined (CONFIG_PLATFORM_DT90517)
	DT90517_SetBERCalTime(nChannelId, berCalMode);
#else
	CH_UNUSED(nChannelId);
#endif

	CH_DI_Print(CH_LOG_4,("[CH_T_SetEwsInfo] berCalMode= %d \n ", berCalMode));

	return nResult;
}

int CH_T_StopTER(int nChannelId)
{
	int nResult = DI_CH_OK;
	CH_DI_Print(CH_LOG_4, ("[CH_T_StopTER] Unit[%d] Start\n", nChannelId));
#if defined (CONFIG_PLATFORM_DT90507)
	DT90507_Stop(nChannelId);
#elif defined (CONFIG_PLATFORM_DT90517)
	DT90517_Stop(nChannelId);
#endif
	CH_DI_Print(CH_LOG_4, ("[CH_T_StopTER]Unit[%d] End\n", nChannelId));
	return nResult;
}

int CH_T_GetDataPlpInfo( int nChannelId,  HUINT8 *pucPlpIds, HUINT8 *pucNumPlps )
{
	int nResult = DI_CH_OK;

	nResult = TER_GetDataPlpInfo( nChannelId,  pucPlpIds, pucNumPlps );

	return nResult;
}

int CH_T_GetDataPlpCount( int nChannelId,   HUINT8 *pucNumPlps )
{
	int nResult = DI_CH_OK;

	nResult = TER_GetDataPlpCount( nChannelId,  pucNumPlps );

	return nResult;
}

int CH_T_GetCellId( int nChannelId,   HUINT16 *pusCellId )
{
	int nResult = DI_CH_OK;

	nResult = TER_GetCellId(nChannelId, pusCellId);

	return nResult;
}

int CH_T_GetLockedInfo(int nChannelId, CH_TER_TuneParam_t * pstTerParam)
{
	int nResult = DI_CH_OK;

	nResult = TER_GetLockedInfo(nChannelId, pstTerParam);

	return nResult;
}

int CH_T_GetDeviceStatus(int nChannelId,DI_CH_DeviceStatus * pstDeviceStatus)
{
	int nResult = DI_CH_OK;

	DRV_CH_Lock( nChannelId );

	if(TER_GetDeviceStatus == NULL)
	{
		DRV_CH_UnLock( nChannelId );
		return DI_CH_NOT_SUPPORTED;
	}
	else
	{
		nResult = TER_GetDeviceStatus( nChannelId, pstDeviceStatus);
	}

	DRV_CH_UnLock( nChannelId );

	return nResult;
}

//JAMES
int CH_T_DumpRegister(int nChannelId)
{
	int nResult = DI_CH_OK;

	nResult = TER_DumpRegister(nChannelId);

	return nResult;
}

int CH_T_READ(int nChannelId, int uAddr)
{
	int nResult = DI_CH_OK;

	nResult = TER_READ(nChannelId, uAddr);

	return nResult;
}

int CH_T_WRITE(int nChannelId, int uAddr, int uData)
{
	int nResult = DI_CH_OK;

	nResult = TER_WRITE(nChannelId, uAddr, uData);

	return nResult;
}


/** @} */ // end of CH_TER

