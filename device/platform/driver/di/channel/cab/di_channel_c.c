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
#include "htype.h"
#include "vkernel.h"

#include "di_channel.h"
#include "di_channel_c.h"

#include "di_channel_priv.h"
#include "di_channel_attr.h"
#include "di_channel_util.h"

#include "drv_channel.h"
#include "drv_channel_c.h"

// End Including Headers


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
// Start #define
#define CH_C_FREQUENCY_MIN		 47000 //Unit KHz
#define CH_C_FREQUENCY_MAX 		1002000 //Unit KHz


#define CH_C_SYMBOLRATE_MIN			1000 // min. SR value
#define CH_C_SYMBOLRATE_MAX			7200 // max. SR value

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
extern CH_InfoChannel_t g_stChannelConfig[NUM_OF_CHANNEL];
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

void DisplayCabChannelInformation(
	HUINT32	ulInstanceId ,
	HUINT32 ulRequestId,
	CH_CAB_TuneParam_t* stChannelParam
)
{
#if !defined(__CH_DEBUG__) || !defined(__CH_RELEASE_MODE_DEBUG__)
	CH_UNUSED(ulInstanceId);
	CH_UNUSED(ulRequestId);
#endif

	CH_DI_Print(CH_LOG_DEFAULT, (" Request Lock >> ===================================================\r\n"));
	CH_DI_Print(CH_LOG_DEFAULT, ("	* Tuner Instance : (%d)\r\n", ulInstanceId));
	CH_DI_Print(CH_LOG_DEFAULT, ("	* Tuner RequestId : (%d)\r\n", ulRequestId));
	CH_DI_Print(CH_LOG_DEFAULT, ("	* Local Frequency : %d [KHz]\r\n", stChannelParam->ulFrequency));
	CH_DI_Print(CH_LOG_DEFAULT, ("	* Symbol Rate : (%d) [KHz]\r\n", stChannelParam->ulSymbolRate));
	CH_DI_Print(CH_LOG_DEFAULT, ("	* Modulation : "));

	switch(stChannelParam->etModulation)
	{
		case DI_CH_CMOD_AUTO:
			CH_DI_Print(CH_LOG_DEFAULT, ("Automatic\r\n"));
			break;
		case DI_CH_CMOD_16QAM:
			CH_DI_Print(CH_LOG_DEFAULT, ("16QAM\r\n"));
			break;
		case DI_CH_CMOD_32QAM:
			CH_DI_Print(CH_LOG_DEFAULT, ("32QAM\r\n"));
			break;
		case DI_CH_CMOD_64QAM:
			CH_DI_Print(CH_LOG_DEFAULT, ("64QAM\r\n"));
			break;
		case DI_CH_CMOD_128QAM:
			CH_DI_Print(CH_LOG_DEFAULT, ("128QAM\r\n"));
			break;
		case DI_CH_CMOD_256QAM:
			CH_DI_Print(CH_LOG_DEFAULT, ("256QAM\r\n"));
			break;
	}

	CH_DI_Print(CH_LOG_DEFAULT, ("	* Spectrum : "));
	switch(stChannelParam->etSpectrum)
	{
		case DI_CH_INVERSION_AUTO:
			CH_DI_Print(CH_LOG_DEFAULT, ("Automatic\r\n"));
			break;
		case DI_CH_INVERSION_NORMAL:
			CH_DI_Print(CH_LOG_DEFAULT, ("Normal\n"));
			break;
		case DI_CH_INVERSION_INVERTED:
			CH_DI_Print(CH_LOG_DEFAULT, ("Inverted\n"));
			break;
	}
	CH_DI_Print(CH_LOG_DEFAULT, ("====================================================================\r\n"));
}

int CheckCabTuneParam ( CH_CAB_TuneParam_t	*pstCabParam )
{
	int	nResult = DI_CH_OK;

	if( (pstCabParam->ulFrequency < CH_C_FREQUENCY_MIN) || (pstCabParam->ulFrequency > CH_C_FREQUENCY_MAX) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CheckCabTuneParam] ERR! ulFrequency(%d)\n", pstCabParam->ulFrequency));
		nResult = DI_CH_PARAM_ERR;
	}

	if( (pstCabParam->ulSymbolRate < CH_C_SYMBOLRATE_MIN) || (pstCabParam->ulSymbolRate > CH_C_SYMBOLRATE_MAX) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CheckCabTuneParam] ERR! ulSymbolRate(%d)\n", pstCabParam->ulSymbolRate));
		nResult = DI_CH_PARAM_ERR;
	}

	if(pstCabParam->etModulation > DI_CH_CMOD_256QAM)
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CheckCabTuneParam] etModulation(%d)\n", pstCabParam->etModulation));
		nResult = DI_CH_PARAM_ERR;
	}

	if(pstCabParam->etSpectrum > DI_CH_INVERSION_INVERTED)
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CheckCabTuneParam] etSpectrum(%d)\n", pstCabParam->etSpectrum));
		nResult = DI_CH_PARAM_ERR;
	}

	return nResult;
}


DI_CHANNEL_ERR DI_CH_C_RegisterCallback( HUINT32 ulInstanceId, pfnDI_CH_NotifySignalInfo pfnNotify)
{
	int nResult = DI_CH_OK;
	int nChannelId;

	CH_DI_Print(CH_LOG_ARGS, ("[DI_CH_C_RegisterCallback] ---- \n "));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_CAB );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_C_RegisterCallback] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	if ( (g_stChannelConfig[nChannelId].SignalCallback == NULL) \
		|| (pfnNotify == NULL) )
	{
		CH_DI_Print(CH_LOG_STATUS, ("Callback is registered...\n"));
		g_stChannelConfig[nChannelId].SignalCallback = pfnNotify;
	}
	else
	{
		/* callback is already regitered */
	}

	return nResult;
}


DI_CHANNEL_ERR DI_CH_C_ActivateChannel( HUINT32 ulInstanceId )
{
	int nResult = DI_CH_OK;
	int nChannelId;

	CH_DI_Print(CH_LOG_ARGS, ("[DI_CH_C_ActivateChannel] -----\n"));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_CAB );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_C_ActivateChannel] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}
	else
	{
#if defined(CONFIG_PLATFORM_TM201COMBO)
		CH_C_SetPowerState( nChannelId, CH_POWER_ON);
		CAB_Initialize(nChannelId);
#else
		CH_C_SetPowerState( nChannelId, CH_POWER_ON);
#endif
	}

	return nResult;
}


DI_CHANNEL_ERR DI_CH_C_DeactivateChannel( HUINT32 ulInstanceId )
{
	int nResult = DI_CH_OK;
	int nChannelId;

	CH_DI_Print(CH_LOG_ARGS, ("[DI_CH_C_DeactivateChannel] --- \n "));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_CAB );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_C_DeactivateChannel] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}
	else
	{
		SetTuneStatus ( nChannelId, CH_STATUS_IDLE );
		CH_C_SetPowerState( nChannelId, CH_POWER_OFF);
	}

	return nResult;
}


DI_CHANNEL_ERR DI_CH_C_RequestLock( HUINT32 ulInstanceId, HUINT32 ulRequestId, CH_CAB_TuneParam_t * pstParam )
{
	int nRet 	= DI_CH_OK;
	int nResult = DI_CH_OK;
	int nChannelId;
	HUINT32 ulSearchMode = 0;

	CH_DI_Print(CH_LOG_ARGS, ("[DI_CH_C_RequestLock] ulRequestId : [%d] \n", ulRequestId));

	DisplayCabChannelInformation(ulInstanceId , ulRequestId, pstParam);

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_CAB );
	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_C_RequestLock] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	nRet = CheckCabTuneParam( pstParam );
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_C_RequestLock] ERR! CheckCabTuneParam() nRet(%d)\n", nRet));
		return nRet;
	}
	else
	{
		SetTuneStatus( nChannelId, CH_STATUS_LOCKING );
	}

	nRet = UpdateTuneParam( nChannelId, ulRequestId, (void *) pstParam, sizeof(CH_CAB_TuneParam_t) );
	if ( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_C_RequestLock] ERR! UpdateTuneParam() nRet(0x%x) \n", nRet));
		nResult = DI_CH_ERR;
	}

#if defined (PLATFORM_DP10023) || defined (CONFIG_PLATFORM_DB3383)
	CH_DI_Print(CH_LOG_ARGS, ("[DI_CH_C_RequestLock] ulRequestId : [%d] Nothing to do. \n", ulRequestId));
#else
	if( CompareTuneParam( nChannelId ) )
	{
		CH_DI_Print(CH_LOG_ARGS, ("[DI_CH_C_RequestLock] Already locked and same request param...\n"));
		SetTuneStatus( nChannelId, CH_STATUS_LOCKED );
		DRV_CH_CallNotifyCallback( nChannelId, ulRequestId, DI_CH_SIGNAL_LOCKED );

		return DI_CH_OK;
	}
#endif

	if( pstParam->etModulation != DI_CH_CMOD_AUTO )
	{
		ulSearchMode = GetSearchMode( nChannelId );
		if( ulSearchMode & CH_C_MODULATION )
		{
			SetSearchMode( nChannelId, ( ulSearchMode & (~CH_C_MODULATION) ));
		}
	}

	nRet = SendChannelCommand( CH_CMD_LOCKING, nChannelId, ulRequestId );
	if ( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_C_RequestLock] ERR! nRet(0x%x)", nRet));
		nResult = DI_CH_ERR;
	}

	return nResult;

}



DI_CHANNEL_ERR DI_CH_C_CancelLock( HUINT32 ulInstanceId )
{
	int nResult = DI_CH_OK;
	int nChannelId;

	CH_DI_Print(CH_LOG_ARGS, ("[DI_CH_C_CancelLock] --- \n "));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_CAB );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_C_CancelLock] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}
	else
	{
		SetTuneStatus ( nChannelId, CH_STATUS_CANCLE );
		CH_C_DisableTsOutput( nChannelId );
	}

	return nResult;
}


DI_CHANNEL_ERR DI_CH_C_GetLockedInfo( HUINT32 ulInstanceId, CH_CAB_TuneParam_t * pstParam )
{
	int nResult = DI_CH_OK;
	CH_Status_e etTuneStatus;
	int nChannelId;

	CH_DI_Print(CH_LOG_ARGS, ("[DI_CH_C_GetLockedInfo] ---- \n "));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_CAB );
	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_C_GetLockedInfo] ERR ! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	if ( pstParam == NULL )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_C_GetLockedInfo] ERR! Parma NULL! \n "));
		return DI_CH_PARAM_ERR;
	}

	etTuneStatus = 	GetTuneStatus(nChannelId);
	CH_DI_Print(CH_LOG_ARGS, (" etTuneStatus (%d)\n", etTuneStatus));

	if ( etTuneStatus == CH_STATUS_LOCKED )
	{
		UpdateLockedParam( nChannelId, pstParam );
	}
	else
	{
		/* Unlocked channel */
		nResult = DI_CH_ERR;
	}

	return nResult;
}


DI_CHANNEL_ERR DI_CH_C_GetSignalInfo ( HUINT32 ulInstanceId, DI_CH_SignalQuality_t *pstSignalInfo )
{
	int nResult = DI_CH_OK;
	int nRet = DI_CH_OK;
	int nChannelId;

	CH_DI_Print(CH_LOG_ARGS, ("[DI_CH_C_GetSignalInfo] ---- \n "));

	if ( pstSignalInfo == NULL )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_C_GetSignalInfo] ERR! \n"));
		return DI_CH_PARAM_ERR;
	}

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_CAB );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_C_GetSignalInfo] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	nRet = CH_C_GetSignalInfo( nChannelId, pstSignalInfo );
	if( nRet != DI_CH_OK )
	{
		nResult = DI_CH_ERR;
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_C_GetSignalInfo] ERR! CH_C_GetSignalInfo() nRet (%d) \n", nRet));
	}

	return nResult;
}



DI_CHANNEL_ERR DI_CH_C_GetDeviceStatus(HUINT32 ulInstanceId,DI_CH_DeviceStatus * pstDeviceStatus)
{
	int nRet = DI_CH_OK;
	int nChannelId;

	CH_DI_Print(CH_LOG_ARGS, ("[DI_CH_C_GetDeviceStatus] ---- \n "));

	if ( pstDeviceStatus == NULL )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_C_GetDeviceStatus] ERR! \n"));
		return DI_CH_PARAM_ERR;
	}

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_CAB );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_C_GetDeviceStatus] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	nRet = CH_C_GetDeviceStatus( nChannelId, pstDeviceStatus );
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_C_GetDeviceStatus] ERR! CH_C_GetSignalInfo() nRet (%d) \n", nRet));
	}

	return nRet;
}




//////// Japan(J-COM) platform  Request for api  /////////
DI_CHANNEL_ERR DI_CH_C_GetStatus( HUINT32 ulInstanceId, HUINT32 ulFrequency)
{
	int nResult = DI_CH_OK;
	int nChannelId;

	CH_DI_Print(CH_LOG_ARGS, ("[DI_CH_C_GetStatus] ---- \n "));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_CAB );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_C_GetStatus] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	if ( CH_C_GetStatus(nChannelId, ulFrequency)  == DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_STATUS, ("[DI_CH_C_GetStatus] LOCK! \n"));
		nResult = DI_CH_OK;
	}
	else
	{
		/* Unlocked channel */
		CH_DI_Print(CH_LOG_STATUS, ("[DI_CH_C_GetStatus] UNLOCK! \n"));
		nResult = DI_CH_ERR;
	}

	return nResult;
}

DI_CHANNEL_ERR DI_CH_C_GetTsmfInfo ( HUINT32 ulInstanceId, HUINT16 *ucTsNo, HINT32 *nTsId, HINT32 *nNetId)
{
	int nResult = DI_CH_OK;
	int nRet = DI_CH_OK;
	int nChannelId;

	CH_DI_Print(CH_LOG_ARGS, ("[DI_CH_C_GetTsmfInfo] ---- \n "));

	if ( ucTsNo == NULL || nTsId == NULL || nNetId == NULL  )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_C_GetTsmfInfo] ERR! \n"));
		return DI_CH_PARAM_ERR;
	}

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_CAB );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_C_GetTsmfInfo] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	nRet = CH_C_GetTsmfData( nChannelId, ucTsNo,  nTsId, nNetId);
	if( nRet != DI_CH_OK )
	{
		nResult = DI_CH_ERR;
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_C_GetTsmfInfo] ERR! CH_C_GetTsmfData() nRet (%d) \n", nRet));
	}

	return nResult;

}


DI_CHANNEL_ERR DI_CH_C_CheckTsmfLock( HUINT32 ulInstanceId, HUINT32 ulFrequency )
{
	int nResult = DI_CH_OK;
	int nChannelId;

	CH_DI_Print(CH_LOG_ARGS, ("[DI_CH_C_CheckTsmfLock] ---- \n "));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_CAB );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_C_CheckTsmfLock] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	if ( CH_C_CheckTsmfLock(nChannelId, ulFrequency)  == TRUE )
	{
		CH_DI_Print(CH_LOG_TSINFO, (" TSMF LOCK! \n"));
	}
	else
	{
		/* Unlocked channel */
		CH_DI_Print(CH_LOG_TSINFO, (" TSMF UNLOCK! \n"));
		nResult = DI_CH_ERR;
	}

	return nResult;
}


DI_CHANNEL_ERR DI_CH_C_Stop( HUINT32 ulInstanceId )
{
	int nResult = DI_CH_OK;
	int nChannelId;

	CH_DI_Print(CH_LOG_ARGS, ("[DI_CH_C_Stop] --- \n "));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_CAB );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_C_Stop] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}
	else
	{
		CH_C_StopQAM ( nChannelId );
	}

	return nResult;
}

DI_CHANNEL_ERR DI_CH_C_ResetGpio( HUINT32 ulInstanceId )
{
	int nResult = DI_CH_OK;
	int nChannelId;

	CH_DI_Print(CH_LOG_ARGS, ("[DI_CH_C_ResetGpio] ---- \n "));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_CAB );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_C_ResetGpio] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}
	else
	{
		CAB_ResetGpio( nChannelId );
	}

	return nResult;
}

DI_CHANNEL_ERR DI_CH_C_Init( HUINT32 ulInstanceId )
{
	int nResult = DI_CH_OK;
	int nChannelId;

	CH_DI_Print(CH_LOG_ARGS, ("[DI_CH_C_Init] ---- \n "));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_CAB );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_C_Init] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}
	else
	{
		CAB_Initialize( nChannelId );
	}

	return nResult;
}


DI_CHANNEL_ERR DI_CH_C_GetAgc( HUINT32 ulInstanceId,  HUINT8 *ucRfAgc, HUINT8 *ucIfAgc)
{
	int nResult = DI_CH_OK;
	int nRet = DI_CH_OK;
	int nChannelId;

	CH_DI_Print(CH_LOG_ARGS, ("[DI_CH_C_GetAgc] ---- \n "));

	if ( ucRfAgc == NULL || ucIfAgc == NULL )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_C_GetAgc] ERR! \n "));
		return DI_CH_PARAM_ERR;
	}

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_CAB );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_C_GetAgc] ERR! ulInstanceId(%d)", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	nRet = CH_C_GetAGC( nChannelId, ucRfAgc, ucIfAgc);
	if( nRet != DI_CH_OK )
	{
		nResult = DI_CH_ERR;
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_C_GetAgc] ERR! CH_C_GetAGC() nRet(%d)\n ", nRet));
	}

	return nResult;
}

DI_CHANNEL_ERR DI_CH_C_GetEwsInfo( HUINT32 ulInstanceId,  HUINT8 *ucIsEws )
{
	int nResult = DI_CH_OK;
	int nRet = DI_CH_OK;
	int nChannelId;

	CH_DI_Print(CH_LOG_ARGS, ("[DI_CH_C_GetEwsInfo] ---- \n "));

	if ( ucIsEws == NULL )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_C_GetEwsInfo] ERR! \n "));
		return DI_CH_PARAM_ERR;
	}

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_CAB );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_C_GetEwsInfo] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	nRet = CH_C_GetEwsInfo( nChannelId, ucIsEws);
	if( nRet != DI_CH_OK )
	{
		nResult = DI_CH_ERR;
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_C_GetEwsInfo] ERR! CH_C_GetEwsInfo() nRet(%d) \n", nRet));
	}

	return nResult;
}

/* This API will be deprecated */
DI_CHANNEL_ERR DI_CH_C_GetLockInfo ( HUINT32 ulInstanceId, HINT32 *nSnr, HINT32 *nber_decimal, HINT32 *nber_float, HINT32 *nber_exp, HINT32 *nUncorrected, HINT32 *nSignallevel)
{
	int nResult = DI_CH_OK;
	int nRet = DI_CH_OK;
	int nChannelId;

	CH_DI_Print(CH_LOG_ARGS, ("[DI_CH_C_GetLockInfo] ---- \n "));

	if ( nSnr == NULL || nber_decimal == NULL || nber_float == NULL || nber_exp == NULL  || nUncorrected == NULL || nSignallevel == NULL )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_C_GetLockInfo] ERR! \n "));
		return DI_CH_PARAM_ERR;
	}

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_CAB );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_C_GetLockInfo] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	nRet = CH_C_GetLockInfo( nChannelId, nSnr, nber_decimal,  nber_float, nber_exp, nUncorrected, nSignallevel);
	if( nRet != DI_CH_OK )
	{
		nResult = DI_CH_ERR;
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_C_GetLockInfo] ERR! CH_C_GetLockInfo() nRet(%d) \n ", nRet));
	}

	return nResult;

}

/*********************** End of File ******************************/
