/* $Header: */
/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name:		template.c
// Original Author: Full Name!!
// File Description:
// Module:
// Remarks:

/*******************************************************************/
/* Copyright (c) 2006 HUMAX Co., Ltd. 							   */
/* All rights reserved.											   */
/*******************************************************************/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
// Start Including Header Files
#include <memory.h>
#include "htype.h"
#include "vkernel.h"

#include "di_channel.h"
#include "di_channel_priv.h"
#include "di_channel_attr.h"
#include "di_channel_util.h"

#include "drv_channel.h"
#include "drv_channel_c.h"

#if defined (CONFIG_PLATFORM_DP10023)
#include "drv_channel_dp10023.h"
#endif

#if defined (CONFIG_PLATFORM_DP10024)
#include "drv_channel_dp10024.h"
#endif

#if defined (CONFIG_PLATFORM_DB3383)
#include "drv_channel_db3383.h"
#endif

#if defined (CONFIG_PLATFORM_DB7584)
#include "drv_channel_db7584.h"
#endif

// End Including Headers


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
// Start #define
// End #define


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
// Start typedef

// End typedef


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
// Start global variablee
extern HINT32 show_di_ch;

// End global variable


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
// Start static variablee

// End static variable


/****************** Function 시작점에 필히 기록 ********************/
/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
// Function Name:
// Function Description:
// Parameter:
// Return Value:

DI_CHANNEL_ERR CH_C_ReInitDevice( int nChannelId )
{
	int nResult = DI_CH_OK;

	CH_DI_Print(CH_LOG_ARGS,("CH_C_ReInitDevice Enter ...\n"));

    /* Initialise HW */
    nResult = CAB_ReInitDevice(nChannelId);

	return nResult;
}


DI_CHANNEL_ERR CH_C_UnInitDevice( int nChannelId )
{

    int nResult = DI_CH_OK;

    CH_DI_Print(CH_LOG_ARGS,("CH_C_UnInitDevice Enter ...\n"));

    nResult = CAB_UnInitDevice(nChannelId);

    return nResult;
}


DI_CHANNEL_ERR CH_C_InitDev( int nChannelId )
{
	int nResult = DI_CH_OK;
	int nRet	= DI_CH_OK;
	int nUnitId;

	CH_DI_Print(CH_LOG_ARGS, ("[CH_C_InitDev] Enter ...\n"));

	nUnitId = GetDeviceUnitId( nChannelId );

	if( nUnitId >= NUM_OF_CHANNEL)
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CH_C_InitDev] ERR! nUintId (%d) \n", nUnitId));
		return DI_CH_ERR;
	}

	DRV_C_InstallApi();

	/* Init Cable channel */
	nRet= CAB_InitDevice(nChannelId );
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CH_C_InitDev] ERR! CAB_InitDevice() nRet(%d) \n ", nRet));
		nResult = DI_CH_ERR;
	}

	CH_C_DisableTsOutput(nChannelId);

	CAB_SetPowerState( nChannelId, CH_POWER_OFF );

	return nResult;
}


DI_CHANNEL_ERR CH_C_SetTune( int nChannelId, HUINT32 unWorkingId )
{
	int nRet = DI_CH_OK;
	int nUnitId;

	CH_CAB_TuneParam_t * pstCabTuneParam;
	CH_CAB_TuneParam_t * pstMWTuneParam;

	HUINT32 ulSearchMode = 0;

	DRV_CH_Lock( nChannelId );

	CH_DI_Print(CH_LOG_ARGS, ("[CH_C_SetTune] Enter ...\n"));

	nUnitId = GetDeviceUnitId( nChannelId );

	pstCabTuneParam = (CH_CAB_TuneParam_t *)GetWorkingTuneInfo( nChannelId );
	pstMWTuneParam	= (CH_CAB_TuneParam_t *)GetRequestedTuneInfo( nChannelId );

	if( pstCabTuneParam->etModulation == DI_CH_CMOD_AUTO )
	{
		ulSearchMode |= CH_C_MODULATION;
		SetSearchMode( nChannelId, ulSearchMode );
		pstCabTuneParam->etModulation = DI_CH_CMOD_64QAM;
	}

	/* Tune cable channel */
	nRet = CAB_SetTune( nChannelId, unWorkingId, pstCabTuneParam );

	DRV_CH_UnLock( nChannelId );

	/* CAB_SetTune에서 1) Locking 도중 혹은 2) Lock 성공한 후에 MW에서 Cancel을 내릴 경우에 대한 대비 */
	if (GetTuneStatus(nChannelId)==CH_STATUS_CANCLE)
	{
		CH_DI_Print(CH_LOG_STATUS, (">>> [%d] CAB CH_STATUS_CANCLE.. nRet[%d] \n",nChannelId, nRet));
		return DI_CH_CANCEL_TUNE;
	}

	if( nRet == DI_CH_OK )
	{
		CH_C_EnableTsOutput(nChannelId);
		SetTuneStatus( nChannelId, CH_STATUS_LOCKED );
		CH_DI_Print(CH_LOG_STATUS, (">>> [%d] CAB CH_STATUS_LOCKED \n",nChannelId));

		DRV_CH_CallNotifyCallback( nChannelId, unWorkingId, DI_CH_SIGNAL_LOCKED );
	}
	else if (nRet == DI_CH_CANCEL_TUNE)					/* CAB_SetTune이 DI_CH_CANCEL_TUNE을 return할 경우 처리 */
	{
		CH_DI_Print(CH_LOG_STATUS, (">>> [%d] CAB CH_STATUS_CANCLE \n",nChannelId));
		return DI_CH_CANCEL_TUNE;
	}
	else
	{
#if defined(CONFIG_PLATFORM_TM201COMBO)						// JC-8500BR has no needs to process Autosearch
		ulSearchMode = 0;
#else
		ulSearchMode = GetSearchMode( nChannelId );
#endif
		if( ulSearchMode != 0 )
		{
			nRet = SendChannelCommand( CH_CMD_AUTOSEARCH, nChannelId, unWorkingId );
			if ( nRet != DI_CH_OK )
			{
				CH_DI_Print(CH_LOG_DEFAULT, ("[CH_C_SetTune] nRet(0x%x) \n", nRet));
			}
		}
		else
		{
			/* return to original request tune param */
			{
				VK_memcpy( 	pstCabTuneParam, \
							pstMWTuneParam, \
							sizeof(CH_CAB_TuneParam_t) );

			}

			SetTuneStatus( nChannelId, CH_STATUS_UNLOCK );
			CH_DI_Print(CH_LOG_STATUS, (">>> [%d] CAB CH_STATUS_UNLOCK \n",nChannelId));

			DRV_CH_CallNotifyCallback( nChannelId, unWorkingId, DI_CH_SIGNAL_UNLOCK );

		}
	}

	return nRet;
}

HBOOL CH_C_CheckLock ( int nChannelId)
{
	HBOOL bLocked = TRUE;

	DRV_CH_Lock( nChannelId );

	bLocked = CAB_CheckLock( nChannelId );

	if( bLocked == FALSE )
	{
		VK_TASK_Sleep(100);//wait 100ms

		bLocked = CAB_CheckLock( nChannelId);
		if( bLocked == FALSE )
		{
			CH_DI_Print(CH_LOG_STATUS, ("[CH_C_CheckLock] Unlock!!! \n"));
		}
	}

	DRV_CH_UnLock( nChannelId );

	return bLocked;
}

void CH_C_CheckStatus ( int nChannelId, HUINT32 unWorkingId )
{
	//HUINT8	unitId;
	//HUINT32 unRequestId;
	//CH_TER_TuneParam_t * pstTerTuneParam;
	CH_Status_e		etStatusOfChannel;
	HBOOL	bLocked = FALSE;

	//unitId = (HUINT8)GetDeviceUnitId( nChannelId );
	etStatusOfChannel = GetTuneStatus( nChannelId );
	CH_DI_Print(CH_LOG_ARGS, ("[CH_C_CheckStatus] Enter(%d)(%d) ...\n", nChannelId, etStatusOfChannel));

	switch ( etStatusOfChannel )
	{
		case CH_STATUS_IDLE:
			break;

		case CH_STATUS_LOCKING:
			break;

		case CH_STATUS_LOCKED:
			bLocked = CH_C_CheckLock ( nChannelId );
			if( bLocked != TRUE )
			{
				SetTuneStatus( nChannelId, CH_STATUS_UNLOCK );

				DRV_CH_CallNotifyCallback( nChannelId, unWorkingId, DI_CH_SIGNAL_UNLOCK );
			}
			else
			{
				//do nothing
				//CH_C_GetSignalInfo( nChannelId, &stCabSignalStatus);
			}

			break;

		case CH_STATUS_CANCLE:
			break;

		case CH_STATUS_UNLOCK:
			CH_C_SetTune( nChannelId, unWorkingId );
			break;

		default:
			CH_DI_Print(CH_LOG_STATUS, ("etStatusOfChannel(%d)", etStatusOfChannel));
			break;

	}

	return;

}

int CH_C_GetSignalInfo(int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo)
{
	int nResult = DI_CH_OK;

	DRV_CH_Lock( nChannelId );

	nResult = CAB_GetStrengthQuality(nChannelId, pstSignalInfo);

	DRV_CH_UnLock( nChannelId );

	return nResult;
}



int CH_C_EnableTsOutput ( int nChannelId )
{
	int nResult = DI_CH_OK;
	//int nRet;
	int nUnitId;

	DRV_CH_Lock( nChannelId );

	nUnitId = GetDeviceUnitId( nChannelId );
	CH_DI_Print(CH_LOG_ARGS, ("[CH_C_EnableTsOutput] [%d]\n ", nUnitId));

	/* Enable TS output */
	CAB_EnableTsOutput( nUnitId );

	SetTsStatus( nChannelId, CH_TS_PATH_ON );

	DRV_CH_UnLock( nChannelId );

	return nResult;
}

int CH_C_DisableTsOutput ( int nChannelId )
{
	int nResult = DI_CH_OK;
	//int nRet;
	int nUnitId;

	DRV_CH_Lock( nChannelId );

	nUnitId = GetDeviceUnitId( nChannelId );
	CH_DI_Print(CH_LOG_ARGS, ("[CH_C_DisableTsOutput] [%d]\n ", nUnitId));

	/* Disable TS output */
	CAB_DisableTsOutput( nUnitId );

	SetTsStatus( nChannelId, CH_TS_PATH_OFF );

	DRV_CH_UnLock( nChannelId );

	return nResult;
}

int CH_C_SetTsOutputMode ( int ChannelId, CH_TsOutMode_e OutputMode )
{
	int nResult = DI_CH_OK;
	int nRet = TRUE;
	CH_TsStatus_e CurTsStatus;
	HUINT8 unitId;

	unitId = (HUINT8)GetDeviceUnitId( ChannelId );

	CurTsStatus = GetTsStatus( ChannelId );

	if( CurTsStatus == CH_TS_PATH_ON )
	{
		CH_C_DisableTsOutput( ChannelId );

		//nRet = API_SetOutputOptions( &s_stNimData[unitId], &s_stMPEG );
		//Set TS output mode
		if( nRet != TRUE )
		{
			CH_DI_Print(CH_LOG_DEFAULT, ("[CH_C_SetTsOutputMode] ERR! \n "));
			//TODO_ILEE : which kind of process is needed?
			return DI_CH_ERR;
		}
		else
		{
			CH_C_EnableTsOutput( ChannelId );
		}

	}
	else
	{
		//nRet = API_SetOutputOptions( &s_stNimData[unitId], &s_stMPEG );
		//Set TS output mode
		if( nRet != TRUE )
		{
			CH_DI_Print(CH_LOG_DEFAULT, ("[CH_C_SetTsOutputMode] ERR! \n "));
			//TODO_ILEE : which kind of process is needed?
			return DI_CH_ERR;
		}

	}

	SetTsOutputMode( ChannelId, OutputMode );

	return nResult;

}


void CH_C_SetPowerState( int nChannelId, CH_Power_e etPower )
{
	int nResult = DI_CH_OK;
	int nRet;
	int  	nUnitId;

	DRV_CH_Lock( nChannelId );

	nUnitId = GetDeviceUnitId( nChannelId );

	if( etPower == CH_POWER_OFF )
	{
		SetActiveStatus( nChannelId, CH_ACTION_OFF );
	}
	else
	{
		SetActiveStatus( nChannelId, CH_ACTION_ON );
	}

	CH_DI_Print(CH_LOG_ARGS, ("[CH_C_SetPowerState] Enter \n "));

	nRet = CAB_SetPowerState( nChannelId, etPower );

	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CH_C_SetPowerState] ERR! CAB_SetPowerState() \n"));
		nResult = DI_CH_ERR;
	}
	DRV_CH_UnLock( nChannelId );

}


void CH_C_ProcessAutoSearch( int nChannelId, HUINT32 ulWorkingId )
{
	HUINT32			ulSearchMode;
	CH_C_AutoMode_e etSearchType = 0;
	CH_CAB_TuneParam_t * pstCabTuneParam;

	pstCabTuneParam = (CH_CAB_TuneParam_t *)GetWorkingTuneInfo( nChannelId );

	ulSearchMode = GetSearchMode( nChannelId );

	if( ulSearchMode & CH_C_MODULATION )
	{
		etSearchType = CH_C_MODULATION;
	}
	else
	{
		CH_DI_Print(CH_LOG_STATUS, ("[CH_C_ProcessAutoSearch] Auto Search is finished!!!\n"));
	}

#if defined(CONFIG_PLATFORM_DB3383) || defined(CONFIG_SUPPORT_QAM_ANNEX_B)
/* Support 64QAM, 256QAM for Japan */
	switch( etSearchType )
	{
		case CH_C_MODULATION:

			if( pstCabTuneParam->etModulation == DI_CH_CMOD_64QAM )
			{
				pstCabTuneParam->etModulation = DI_CH_CMOD_256QAM;
				SetSearchMode( nChannelId, ( ulSearchMode & (~CH_C_MODULATION) ) );
			}
			else
			{
				CH_DI_Print(CH_LOG_STATUS, ("[CH_C_ProcessAutoSearch] Already finished!!!\n"));
			}

			break;


		default:
			CH_DI_Print(CH_LOG_ARGS, ( "etSearchType(%d)\n", etSearchType) );
			break;
	}
#else
	switch( etSearchType )
	{
		case CH_C_MODULATION:

			if( pstCabTuneParam->etModulation == DI_CH_CMOD_64QAM )
			{
				pstCabTuneParam->etModulation = DI_CH_CMOD_256QAM;
			}
			else if( pstCabTuneParam->etModulation == DI_CH_CMOD_256QAM )
			{
				pstCabTuneParam->etModulation = DI_CH_CMOD_128QAM;
			}
			else if( pstCabTuneParam->etModulation == DI_CH_CMOD_128QAM )
			{
				pstCabTuneParam->etModulation = DI_CH_CMOD_32QAM;
			}
			else if( pstCabTuneParam->etModulation == DI_CH_CMOD_32QAM )
			{
				pstCabTuneParam->etModulation = DI_CH_CMOD_16QAM;
				SetSearchMode( nChannelId, ( ulSearchMode & (~CH_C_MODULATION) ) );
			}
			else
			{
				CH_DI_Print(CH_LOG_STATUS, ("[CH_C_ProcessAutoSearch] Already finished!!!\n"));
			}

			break;


		default:
			CH_DI_Print(CH_LOG_ARGS, ( "etSearchType(%d)\n", etSearchType) );
			break;
	}
#endif

	CH_C_SetTune( nChannelId, ulWorkingId );

}

int CH_C_GetTsmfData( int nChannelId, HUINT16 *ucTsNo, HINT32 *nTsId, HINT32 *nNetId)
{
	int nResult = DI_CH_OK;

#if defined (PLATFORM_DP10023)
	DP10023_TsmfData(nChannelId, ucTsNo, nTsId, nNetId);
	CH_DI_Print(CH_LOG_TSINFO, ("[CH_C_GetTsmfData] TsNo= %d, TsID= %d, NetID= %d \n ", *ucTsNo, *nTsId, *nNetId));
#elif defined (CONFIG_PLATFORM_DB3383) && defined (CONFIG_DEMOD_TSMF)
	DB3383_TsmfData(nChannelId, ucTsNo, nTsId, nNetId);
	CH_DI_Print(CH_LOG_TSINFO, ("[CH_C_GetTsmfData] TsNo= %d, TsID= %d, NetID= %d \n ", *ucTsNo, *nTsId, *nNetId));
#else
	CH_UNUSED(nChannelId);
	CH_UNUSED(ucTsNo);
	CH_UNUSED(nTsId);
	CH_UNUSED(nNetId);
#endif

	return nResult;
}

#if defined(CONFIG_DEMOD_TSMF)
void CH_C_GetPathInfo(void)
{
	HINT32 nChannelId=0;

	DRV_CH_Lock( nChannelId );

	CAB_GetPathInfo(nChannelId);

	DRV_CH_UnLock( nChannelId );

}
#endif

int CH_C_GetStatus( int nChannelId, HUINT32 frequency)
{
	int nResult = DI_CH_OK;

	DRV_CH_Lock( nChannelId );

#if defined (PLATFORM_DP10023)
	nResult = DP10023_GetStatus(nChannelId, frequency);
#else
	CH_UNUSED(frequency);
#endif

	if( nResult != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_STATUS, ("[CH_C_GetStatus] Unlock!!!\n"));
		SetTuneStatus( nChannelId, CH_STATUS_UNLOCK);
	}
	else
	{
		SetTuneStatus( nChannelId, CH_STATUS_LOCKED);
	}

	DRV_CH_UnLock( nChannelId );

	return nResult;
}

HBOOL CH_C_CheckTsmfLock( int nChannelId, HUINT32 frequency )
{
	HBOOL bLocked = TRUE;

	DRV_CH_Lock( nChannelId );

#if defined (PLATFORM_DP10023)
	bLocked = DP10023_CheckTsmfLock(nChannelId, frequency);
#else
	CH_UNUSED(frequency);
#endif

	if( bLocked == FALSE )
	{
		CH_DI_Print(CH_LOG_STATUS, ("[CH_C_CheckTsmfStatus] Unlock!!!"));
		SetTuneStatus( nChannelId, CH_STATUS_UNLOCK);
	}
	else
	{
		SetTuneStatus( nChannelId, CH_STATUS_LOCKED);
	}

	DRV_CH_UnLock( nChannelId );

	return bLocked;
}

int CH_C_StopQAM(int nChannelId)
{
	int nResult = DI_CH_OK;

	CH_DI_Print(CH_LOG_ARGS, ("[PAL_QAM_Stop] Unit[%d] Start\n", nChannelId));

#if defined (PLATFORM_DP10023)
	DP10023_QAMStop(nChannelId);
#else
	CH_UNUSED(nChannelId);
#endif

	CH_DI_Print(CH_LOG_ARGS, ("[PAL_QAM_Stop]Unit[%d] End\n", nChannelId));

	return nResult;
}

int CH_C_GetAGC( int nChannelId, HUINT8 *ucRfAgc, HUINT8 *ucIfAgc)
{
	int nResult = DI_CH_OK;

#if defined (PLATFORM_DP10023)
	DP10023_GetAGC(nChannelId, ucRfAgc, ucIfAgc);
#else
	CH_UNUSED(nChannelId);
	CH_UNUSED(ucRfAgc);
	CH_UNUSED(ucIfAgc);
#endif

	CH_DI_Print(CH_LOG_SIGINFO, ("[PAL_QAM_GetAGC] RfAgc = 0x%x, IfAgc = 0x%x \n ", *ucRfAgc, *ucIfAgc));

	return nResult;
}

int CH_C_GetEwsInfo( int nChannelId, HUINT8 *ucIsEws)
{
	int nResult = DI_CH_OK;

#if defined (PLATFORM_DP10023)
	DP10023_GetEwsInfo(nChannelId, ucIsEws);
#else
	CH_UNUSED(nChannelId);
	CH_UNUSED(ucIsEws);
#endif

	CH_DI_Print(CH_LOG_TSINFO, ("[CH_C_GetEwsInfo] ucIsEws = 0x%x \n ", *ucIsEws));

	return nResult;
}


int CH_C_GetDeviceStatus(int nChannelId,DI_CH_DeviceStatus * pstDeviceStatus)
{
	int nResult = DI_CH_OK;

	DRV_CH_Lock( nChannelId );

	if(CAB_GetDeviceStatus == NULL)
	{
		DRV_CH_UnLock( nChannelId );
		return DI_CH_NOT_SUPPORTED;
	}
	else
	{
		nResult = CAB_GetDeviceStatus( nChannelId, pstDeviceStatus);
	}

	DRV_CH_UnLock( nChannelId );

	return nResult;
}


/* This API will be deprecated */
int CH_C_GetLockInfo(int nChannelId, HINT32 *nSnr, HINT32 *nber_decimal, HINT32 *nber_float, HINT32 *nber_exp, HINT32 *nUncorrected, HINT32 *nSignallevel)
{
	int nResult = DI_CH_OK;

#if defined (PLATFORM_DP10024)
	nResult = DP10024_LockInfo(nChannelId, nSnr, nber_decimal, nber_float, nber_exp, nUncorrected, nSignallevel);
#else
	CH_UNUSED(nChannelId);
	CH_UNUSED(nSnr);
	CH_UNUSED(nber_decimal);
	CH_UNUSED(nber_float);
	CH_UNUSED(nber_exp);
	CH_UNUSED(nUncorrected);
	CH_UNUSED(nSignallevel);
#endif

	return nResult;
}

/*********************** End of File ******************************/
