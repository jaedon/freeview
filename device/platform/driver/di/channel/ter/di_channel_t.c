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
#include "di_channel_t.h"

#include "di_channel_priv.h"
#include "di_channel_attr.h"
#include "di_channel_util.h"

#include "drv_channel.h"
#include "drv_channel_t.h"

#include "drv_gpio.h"

#include "drv_pm.h"
// End Including Headers


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
// Start #define
/* Unit : MHz
---------------------------------------
Band 		| Digital center frequency
------------+--------------------------
Low band 	|  51.00 to 156.5
Mid band 	| 162.00 to 442.00
High band 	| 450.00 to 858.00
---------------------------------------
*/

#define CH_T_FREQUENCY_MIN		 45000 //Unit KHz
#define CH_T_FREQUENCY_MAX 		862000 //Unit KHz

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

#ifdef CH_PRINT_DI_LEVEL_ZAPPING_TIME
HUINT32 g_ulRequestLock[CONFIG_NUM_OF_TER_CHANNEL];
HUINT32 g_ulDiLevelLockedTime[CONFIG_NUM_OF_TER_CHANNEL] , g_ulDiLevelUnlockedTime[CONFIG_NUM_OF_TER_CHANNEL] ;
#endif

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

void DisplayTerChannelInformation(
	HUINT32	ulInstanceId ,
	HUINT32 ulRequestId,
	CH_TER_TuneParam_t* stChannelParam
)
{
#if !defined(__CH_DEBUG__) || !defined(__CH_RELEASE_MODE_DEBUG__)
		CH_UNUSED(ulInstanceId);
		CH_UNUSED(ulRequestId);
#endif

	CH_DI_Print(CH_LOG_0, (" Request Lock TER>> ==============================================================\r\n"));

	CH_DI_Print(CH_LOG_0,("\r\n"));
	CH_DI_Print(CH_LOG_0, ("	* Tuner Instance : (%d)\r\n", ulInstanceId));
	CH_DI_Print(CH_LOG_0, ("	* Tuner RequestId : (%d)\r\n", ulRequestId));
	CH_DI_Print(CH_LOG_0, ("	* Local Frequency : %d [KHz]\r\n", stChannelParam->ulFrequency));
	CH_DI_Print(CH_LOG_0, ("	* Band Width  :  "));
	switch(stChannelParam->etBandWidth)
	{
		case DI_CH_BW_8MHZ:
			CH_DI_Print(CH_LOG_0, ("8 [MHz]\r\n"));
			break;
		case DI_CH_BW_7MHZ:
			CH_DI_Print(CH_LOG_0, ("7 [MHz]\r\n"));
			break;
		case DI_CH_BW_6MHZ:
			CH_DI_Print(CH_LOG_0, ("6 [MHz]\r\n"));
			break;
		case DI_CH_BW_5MHZ:
			CH_DI_Print(CH_LOG_0, ("5 [MHz]\r\n"));
			break;
		case DI_CH_BW_10MHZ:
			CH_DI_Print(CH_LOG_0, ("10 [MHz]\r\n"));
			break;
		case DI_CH_BW_1P7MHZ:
			CH_DI_Print(CH_LOG_0, ("1P7 [MHz]\r\n"));
			break;
	}

	CH_DI_Print(CH_LOG_0, ("	* Modulation : "));
	switch(stChannelParam->etModulation)
	{
		case DI_CH_TMOD_AUTO:
			CH_DI_Print(CH_LOG_0, ("AUTO\r\n"));
			break;
		case DI_CH_TMOD_QPSK:
			CH_DI_Print(CH_LOG_0, ("QPSK\r\n"));
			break;
		case DI_CH_TMOD_16QAM:
			CH_DI_Print(CH_LOG_0, ("16QAM\r\n"));
			break;
		case DI_CH_TMOD_64QAM:
			CH_DI_Print(CH_LOG_0, ("64QAM\r\n"));
			break;
		case DI_CH_TMOD_256QAM:
			CH_DI_Print(CH_LOG_0, ("256QAM\r\n"));
			break;
	}

	CH_DI_Print(CH_LOG_0, ("	* Hierarchy : "));
	switch(stChannelParam->etHierarchy)
	{
		case DI_CH_HIERARCHY_AUTO:
			CH_DI_Print(CH_LOG_0, ("AUTO\r\n"));
			break;
		case DI_CH_HIERARCHY_NONE:
			CH_DI_Print(CH_LOG_0, ("NONE\r\n"));
			break;
		case DI_CH_HIERARCHY_1:
			CH_DI_Print(CH_LOG_0, ("1\r\n"));
			break;
		case DI_CH_HIERARCHY_2:
			CH_DI_Print(CH_LOG_0, ("2\r\n"));
			break;
		case DI_CH_HIERARCHY_4:
			CH_DI_Print(CH_LOG_0, ("4\r\n"));
			break;
	}

	CH_DI_Print(CH_LOG_0, ("	* Code Rate : "));
	switch(stChannelParam->etCodeRate)
	{
		case DI_CH_TCODERATE_AUTO:
			CH_DI_Print(CH_LOG_0, ("AUTO\r\n"));
			break;
		case DI_CH_TCODERATE_1_2:
			CH_DI_Print(CH_LOG_0, ("1/2\r\n"));
			break;
		case DI_CH_TCODERATE_2_3:
			CH_DI_Print(CH_LOG_0, ("2/3\r\n"));
			break;
		case DI_CH_TCODERATE_3_4:  /* 3 */
			CH_DI_Print(CH_LOG_0, ("3/4\r\n"));
			break;
		case DI_CH_TCODERATE_5_6:
			CH_DI_Print(CH_LOG_0, ("5/6\r\n"));
			break;
		case DI_CH_TCODERATE_7_8:
			CH_DI_Print(CH_LOG_0, ("7/8\r\n"));
			break;
		case DI_CH_T2CODERATE_3_5:
			CH_DI_Print(CH_LOG_0, ("3/5\r\n"));
			break;
		case DI_CH_T2CODERATE_4_5:
			CH_DI_Print(CH_LOG_0, ("4/5\r\n"));
			break;

	}


	CH_DI_Print(CH_LOG_0,("	* Transmode : "));
	switch(stChannelParam->etTransMode)
	{
		case DI_CH_TRANSMODE_AUTO:
			CH_DI_Print(CH_LOG_0, ("AUTO\r\n"));
			break;
		case DI_CH_TRANSMODE_2K:
			CH_DI_Print(CH_LOG_0, ("2K\r\n"));
			break;
		case DI_CH_TRANSMODE_8K:
			CH_DI_Print(CH_LOG_0, ("8K\r\n"));
			break;
		case DI_CH_T2TRANSMODE_4K:
			CH_DI_Print(CH_LOG_0, ("4K\r\n"));
			break;
		case DI_CH_T2TRANSMODE_1K:
			CH_DI_Print(CH_LOG_0, ("1K\r\n"));
			break;
		case DI_CH_T2TRANSMODE_16K:
			CH_DI_Print(CH_LOG_0, ("16K\r\n"));
			break;
		case DI_CH_T2TRANSMODE_32K:
			CH_DI_Print(CH_LOG_0, ("32K\r\n"));
			break;
	}


	CH_DI_Print(CH_LOG_0, ("	* Guard Interval : "));
	switch(stChannelParam->etGuardInterval)
	{
		case DI_CH_GUARDINTERVAL_AUTO:
			CH_DI_Print(CH_LOG_0, ("AUTO\r\n"));
			break;
		case DI_CH_GUARDINTERVAL_1_32:
			CH_DI_Print(CH_LOG_0, ("1/32\r\n"));
			break;
		case DI_CH_GUARDINTERVAL_1_16:
			CH_DI_Print(CH_LOG_0, ("1/16\r\n"));
			break;
		case DI_CH_GUARDINTERVAL_1_8:
			CH_DI_Print(CH_LOG_0, ("1/8\r\n"));
			break;
		case DI_CH_GUARDINTERVAL_1_4:
			CH_DI_Print(CH_LOG_0, ("1/4\r\n"));
			break;
		case DI_CH_T2GUARDINTERVAL_1_128:
			CH_DI_Print(CH_LOG_0, ("1/128\r\n"));
			break;
		case DI_CH_T2GUARDINTERVAL_19_128:
			CH_DI_Print(CH_LOG_0, ("19/128\r\n"));
			break;
		case DI_CH_T2GUARDINTERVAL_19_256:
			CH_DI_Print(CH_LOG_0, ("19/256\r\n"));
			break;
	}

	CH_DI_Print(CH_LOG_0, ("	* Stream : "));
	switch(stChannelParam->etStream)
	{
		case DI_CH_STREAM_HP:
			CH_DI_Print(CH_LOG_0, ("HP\r\n"));
			break;
		case DI_CH_STREAM_LP:
			CH_DI_Print(CH_LOG_0, ("LP\r\n"));
			break;
	}

	CH_DI_Print(CH_LOG_0, ("	* Offset : "));
	switch(stChannelParam->etOffset)
	{
		case DI_CH_OFFSET_AUTO:
			CH_DI_Print(CH_LOG_0, ("AUTO\r\n"));
			break;
		case DI_CH_OFFSET_NONE:
			CH_DI_Print(CH_LOG_0, ("NONE\r\n"));
			break;
		case DI_CH_OFFSET_1_6_P:
			CH_DI_Print(CH_LOG_0, ("1.6P\r\n"));
			break;
		case DI_CH_OFFSET_1_6_N:
			CH_DI_Print(CH_LOG_0, ("1.6N\r\n"));
			break;
		case DI_CH_OFFSET_2_6_P:
			CH_DI_Print(CH_LOG_0, ("2.6P\r\n"));
			break;
		case DI_CH_OFFSET_2_6_N:
			CH_DI_Print(CH_LOG_0, ("2.6N\r\n"));
			break;
		case DI_CH_OFFSET_3_6_P:
			CH_DI_Print(CH_LOG_0, ("3.6P\r\n"));
			break;
		case DI_CH_OFFSET_3_6_N:
			CH_DI_Print(CH_LOG_0, ("3.6N\r\n"));
			break;
 	}


	CH_DI_Print(CH_LOG_0, ("	* System : "));
	switch(stChannelParam->etTerSystem)
	{
		case DI_CH_DVBT:
			CH_DI_Print(CH_LOG_0, ("DVB-T\r\n"));
			break;
		case DI_CH_DVBT2:
			CH_DI_Print(CH_LOG_0, ("DVB-T2\r\n"));
			break;
	}


CH_DI_Print(CH_LOG_0, ("==========================================================================\r\n"));
}

int CheckTerTuneParam ( CH_TER_TuneParam_t* pstTerParam )
{
	int	nResult = DI_CH_OK;

	if( (pstTerParam->ulFrequency < CH_T_FREQUENCY_MIN) || (pstTerParam->ulFrequency > CH_T_FREQUENCY_MAX) )
	{
		CH_DI_Print(CH_LOG_0, ("[CheckTerTuneParam] ERR! ulFrequency(%d)\n", pstTerParam->ulFrequency));
		nResult = DI_CH_PARAM_ERR;
	}

	if( pstTerParam->etBandWidth > DI_CH_BW_6MHZ )
	{
		CH_DI_Print(CH_LOG_0, ("[CheckTerTuneParam] ERR! etBandWidth(%d)\n", pstTerParam->etBandWidth));
		nResult = DI_CH_PARAM_ERR;
	}
#if 1
	if(pstTerParam->etModulation > DI_CH_TMOD_256QAM )
	{
		CH_DI_Print(CH_LOG_0, ("[CheckTerTuneParam] ERR! etModulation(%d)\n", pstTerParam->etModulation));
		nResult = DI_CH_PARAM_ERR;
	}
#endif
	if(pstTerParam->etHierarchy > DI_CH_HIERARCHY_4 )
	{
		CH_DI_Print(CH_LOG_0, ("[CheckTerTuneParam] ERR! etHierarchy(%d)\n", pstTerParam->etHierarchy));
		nResult = DI_CH_PARAM_ERR;
	}

	if(pstTerParam->etCodeRate > DI_CH_TCODERATE_7_8 )
	{
		CH_DI_Print(CH_LOG_0, ("[CheckTerTuneParam] ERR! etCodeRate(%d)\n", pstTerParam->etCodeRate));
		nResult = DI_CH_PARAM_ERR;
	}
#if 1//defined(CONFIG_DVB_T2)
	if( pstTerParam->etTerSystem == DI_CH_DVBT2 )
	{
#if 0
		if(pstTerParam->etTransMode > DI_CH_T2TRANSMODE_32K )
		{
			CH_DI_Print(0, ("[CheckTerTuneParam] ERR! etTransMode(%d)\n", pstTerParam->etTransMode));
			nResult = DI_CH_PARAM_ERR;
		}
#endif
		if(pstTerParam->etGuardInterval > DI_CH_T2GUARDINTERVAL_19_256 )
		{
			CH_DI_Print(CH_LOG_0, ("[CheckTerTuneParam] ERR! etGuardInterval(%d)\n", pstTerParam->etGuardInterval));
			nResult = DI_CH_PARAM_ERR;
		}

	}
	else if( pstTerParam->etTerSystem == DI_CH_DVBT )
	{
		if(pstTerParam->etTransMode > DI_CH_TRANSMODE_8K )
		{
			CH_DI_Print(CH_LOG_0, ("[CheckTerTuneParam] ERR! etTransMode(%d)\n", pstTerParam->etTransMode));
			nResult = DI_CH_PARAM_ERR;
		}

		if(pstTerParam->etGuardInterval > DI_CH_GUARDINTERVAL_1_4 )
		{
			CH_DI_Print(CH_LOG_0, ("[CheckTerTuneParam] ERR! etGuardInterval(%d)\n", pstTerParam->etGuardInterval));
			nResult = DI_CH_PARAM_ERR;
		}

	}
	else
	{
		CH_DI_Print(CH_LOG_0, ("[CheckTerTuneParam] ERR! etTerSystem(%d)\n", pstTerParam->etTerSystem));
		return nResult = DI_CH_PARAM_ERR;
	}
#else
	if(pstTerParam->etTransMode > DI_CH_TRANSMODE_8K )
	{
		CH_DI_Print(0, ("[CheckTerTuneParam] ERR! etTransMode(%d)\n", pstTerParam->etTransMode));
		nResult = DI_CH_PARAM_ERR;
	}

	if(pstTerParam->etGuardInterval > DI_CH_GUARDINTERVAL_1_4 )
	{
		CH_DI_Print(0, ("[CheckTerTuneParam] ERR! etGuardInterval(%d)\n", pstTerParam->etGuardInterval));
		nResult = DI_CH_PARAM_ERR;
	}
#endif
	if(pstTerParam->etStream > DI_CH_STREAM_LP )
	{
		CH_DI_Print(CH_LOG_0, ("[CheckTerTuneParam] ERR! etStream(%d)\n", pstTerParam->etStream));
		nResult = DI_CH_PARAM_ERR;
	}

	if(pstTerParam->etOffset > DI_CH_OFFSET_3_6_N )
	{
		CH_DI_Print(CH_LOG_0, ("[CheckTerTuneParam] ERR! etOffset(%d)\n", pstTerParam->etOffset));
		nResult = DI_CH_PARAM_ERR;
	}

	return nResult;
}



DI_CHANNEL_ERR DI_CH_T_RegisterCallback( HUINT32 ulInstanceId, pfnDI_CH_NotifySignalInfo pfnNotify)
{
	int nResult = DI_CH_OK;
	int nChannelId;

	CH_DI_Print(CH_LOG_4, ("[DI_CH_T_RegisterCallback] ---- \n"));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_TER );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_RegisterCallback] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	if ( (g_stChannelConfig[nChannelId].SignalCallback == NULL) \
		|| (pfnNotify == NULL) )
	{
		CH_DI_Print(CH_LOG_4, ("Callback is registered...\n"));
		g_stChannelConfig[nChannelId].SignalCallback = pfnNotify;
	}
	else
	{
		/* callback is already regitered */
	}

	return nResult;
}


DI_CHANNEL_ERR DI_CH_T_ActivateChannel( HUINT32 ulInstanceId )
{
	int nResult = DI_CH_OK;
	int nChannelId;

	CH_DI_Print(CH_LOG_4, ("[DI_CH_T_ActivateChannel] [%d]---- \n", ulInstanceId));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_TER );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_ActivateChannel] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}
	else
	{
#if defined(CONFIG_PLATFORM_TM201COMBO)
		CH_T_SetPowerState(nChannelId, CH_POWER_ON);
		TER_Initialize(nChannelId);
#else
		CH_T_SetPowerState(nChannelId, CH_POWER_ON);
#endif
	}

	return nResult;
}


DI_CHANNEL_ERR DI_CH_T_DeactivateChannel( HUINT32 ulInstanceId )
{
	int nResult = DI_CH_OK;
	int nChannelId;

	CH_DI_Print(CH_LOG_4, ("[DI_CH_T_DeactivateChannel] [%d]---- \n", ulInstanceId));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_TER );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_DeactivateChannel] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}
	else
	{
		SetTuneStatus ( nChannelId, CH_STATUS_IDLE );
		CH_T_SetPowerState(nChannelId, CH_POWER_OFF);
	}

	return nResult;
}




DI_CHANNEL_ERR DI_CH_T_RequestLock( HUINT32 ulInstanceId, HUINT32 ulRequestId, CH_TER_TuneParam_t * pstParam )
{
	int nRet 	= DI_CH_OK;
	int nResult = DI_CH_OK;
	int nChannelId;

	DisplayTerChannelInformation(ulInstanceId , ulRequestId, pstParam);


#ifdef CH_PRINT_DI_LEVEL_ZAPPING_TIME
       g_ulRequestLock[ulInstanceId] = VK_TIMER_GetSystemTick();
#endif

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_TER );
	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_0, ("DI_CH_T_RequestLock]  ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	nRet = CheckTerTuneParam( pstParam );
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_RequestLock] ERR! CheckTerTuneParam() nRet(%d) \n", nRet));
		return nRet;
	}
	else
	{
		SetTuneStatus( nChannelId, CH_STATUS_LOCKING );
	}

	nRet = UpdateTuneParam( nChannelId, ulRequestId, (void *) pstParam, sizeof(CH_TER_TuneParam_t) );
	if ( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_RequestLock] ERR! UpdateTuneParam() nRet(0x%x) \n", nRet));
		nResult = DI_CH_ERR;
	}

	CH_DI_Print(CH_LOG_4, ("[DI_CH_T_RequestLock] ulRequestId : [%d]\n", ulRequestId));

#if !defined (CONFIG_PLATFORM_DT90517)
	if( CompareTuneParam( nChannelId ) )
	{
		CH_DI_Print(CH_LOG_4, ("Already locked and same request param... \n"));
		SetTuneStatus( nChannelId, CH_STATUS_LOCKED );
		DRV_CH_CallNotifyCallback( nChannelId, ulRequestId, DI_CH_SIGNAL_LOCKED );

		return DI_CH_OK;
	}
#endif

	nRet = SendChannelCommand( CH_CMD_LOCKING, nChannelId, ulRequestId );
	if ( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_RequestLock] ERR! nRet(0x%x) \n", nRet));
		nResult = DI_CH_ERR;
	}

	return nResult;

}



DI_CHANNEL_ERR DI_CH_T_CancelLock( HUINT32 ulInstanceId )
{
	int nResult = DI_CH_OK;
	int nChannelId;

	CH_DI_Print(CH_LOG_4, ("[DI_CH_T_CancelLock] ---- \n"));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_TER );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_CancelLock] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}
	else
	{
		SetTuneStatus ( nChannelId, CH_STATUS_CANCLE );
		CH_T_DisableTsOutput( nChannelId );
	}

	return nResult;
}


DI_CHANNEL_ERR DI_CH_T_GetLockedInfo( HUINT32 ulInstanceId, CH_TER_TuneParam_t * pstParam )
{
	int nResult = DI_CH_OK;
	CH_Status_e etTuneStatus;
	int nChannelId;

	CH_DI_Print(CH_LOG_4, ("[DI_CH_T_GetLockedInfo] ---- \n"));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_TER );
	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_GetLockedInfo] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	if ( pstParam == NULL )
	{
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_GetLockedInfo] ERR! pstParam NULL!\n"));
		return DI_CH_PARAM_ERR;
	}

	etTuneStatus = 	GetTuneStatus(nChannelId);
	CH_DI_Print(CH_LOG_4, ("etTuneStatus (%d)\n", etTuneStatus));

	if ( etTuneStatus == CH_STATUS_LOCKED )
	{
#if defined(CONFIG_SUPPORT_ADVANCED_TUNING)
		nResult = CH_T_GetLockedInfo( nChannelId, pstParam );
#else
		UpdateLockedParam( nChannelId, pstParam );
#endif
	}
	else
	{
		/* Unlocked channel */
		nResult = DI_CH_ERR;
	}

	return nResult;
}


DI_CHANNEL_ERR DI_CH_T_GetSignalInfo ( HUINT32 ulInstanceId, DI_CH_SignalQuality_t *pstSignalInfo )
{
	int nResult = DI_CH_OK;
	int nRet = DI_CH_OK;
	int nChannelId;

	CH_DI_Print(CH_LOG_4, ("[DI_CH_T_GetSignalInfo] ---- \n"));

	if ( pstSignalInfo == NULL )
	{
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_GetSignalInfo] ERR! pstSignalInfo = NULL! \n"));
		return DI_CH_PARAM_ERR;
	}

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_TER );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_GetSignalInfo] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	nRet = CH_T_GetSignalInfo( nChannelId, pstSignalInfo);
	if( nRet != DI_CH_OK )
	{
		nResult = DI_CH_ERR;
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_GetSignalInfo] ERR! CH_C_GetSignalInfo() nRet (%d) \n", nRet));
	}
	return nResult;
}

#if defined(CONFIG_DI20)
DI_CHANNEL_ERR DI_CH_T_SetAntennaPower( HUINT32 ulInstanceId, DI_CH_AntStatus_e etAntState )
{
	int nResult = DI_CH_OK;
	int nChannelId;
	//int nRet;

	CH_DI_Print(CH_LOG_4, ("[DI_CH_T_SetAntennaPower] ulInstanceId(%d), etAntState(%d) \n", ulInstanceId, etAntState));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_TER );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_SetAntennaPower] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	nResult = CH_T_SetAntState( nChannelId, etAntState );
	if( nResult != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_SetAntennaPower] ERR! nResult(%d) \n", nResult));
	}

	return nResult;

}

#else
DI_CHANNEL_ERR DI_CH_T_SetAntennaPower( HUINT32 ulInstanceId, CH_TER_AntStatus_e etAntState )
{
	int nResult = DI_CH_OK;
	int nChannelId;
	//int nRet;

	CH_DI_Print(CH_LOG_4, ("[DI_CH_T_SetAntennaPower] ulInstanceId(%d), etAntState(%d) \n", ulInstanceId, etAntState));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_TER );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_SetAntennaPower] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	nResult = CH_T_SetAntState( nChannelId, etAntState );
	if( nResult != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_SetAntennaPower] ERR! nResult(%d) \n", nResult));
	}

	return nResult;

}
#endif

#if defined(CONFIG_DI20)
DI_CHANNEL_ERR DI_CH_T_GetAntennaPower( HUINT32 ulInstanceId, DI_CH_AntStatus_e *petAntState )
{
	int nResult = DI_CH_OK;
	int nChannelId;
	CH_TER_AntStatus_e etAntennaPower;

	CH_DI_Print(CH_LOG_4, ("[DI_CH_T_GetAntennaPower] ---- \n"));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_TER );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL )
							|| (petAntState == NULL))
	{
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_GetAntennaPower] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}
	else
	{
		etAntennaPower = CH_T_GetAntState( nChannelId );

		*petAntState = etAntennaPower;
	}

	return nResult;

}
#else
DI_CHANNEL_ERR DI_CH_T_GetAntennaPower( HUINT32 ulInstanceId, CH_TER_AntStatus_e *petAntState )
{
	int nResult = DI_CH_OK;
	int nChannelId;
	CH_TER_AntStatus_e etAntennaPower;

	CH_DI_Print(CH_LOG_4, ("[DI_CH_T_GetAntennaPower] ---- \n"));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_TER );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL )
							|| (petAntState == NULL))
	{
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_GetAntennaPower] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}
	else
	{
		etAntennaPower = CH_T_GetAntState( nChannelId );

		*petAntState = etAntennaPower;
	}

	return nResult;

}
#endif

//////// Japan(J-COM) platform  Request for api  /////////
DI_CHANNEL_ERR DI_CH_T_GetStatus( HUINT32 ulInstanceId)
{
	int nResult = DI_CH_OK;
	int nChannelId;

	CH_DI_Print(CH_LOG_4, ("[DI_CH_T_GetStatus] ---- \n "));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_TER );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_GetStatus] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	if ( CH_T_GetStatus(nChannelId)  == DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_3, ("[DI_CH_T_GetStatus]  LOCK! \n"));
		nResult = DI_CH_OK;
	}
	else
	{
		/* Unlocked channel */
		CH_DI_Print(CH_LOG_3, ("[DI_CH_T_GetStatus] UNLOCK! \n"));
		nResult = DI_CH_ERR;
	}

	return nResult;
}

DI_CHANNEL_ERR DI_CH_T_CheckTsmfStatus( HUINT32 ulInstanceId )
{
	int nResult = DI_CH_OK;
	int nChannelId;

	CH_DI_Print(CH_LOG_4, ("[DI_CH_T_CheckTsmfStatus] ---- \n "));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_TER );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_CheckTsmfStatus] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	if ( CH_T_CheckTsmfStatus(nChannelId)  == DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_2,(" TSMF LOCK! \n"));
	}
	else
	{
		/* Unlocked channel */
		CH_DI_Print(CH_LOG_2, (" TSMF UNLOCK! \n"));
		nResult = DI_CH_ERR;
	}

	return nResult;
}


DI_CHANNEL_ERR DI_CH_T_StopTER( HUINT32 ulInstanceId )
{
	int nResult = DI_CH_OK;
	int nChannelId;

	CH_DI_Print(CH_LOG_4, ("[DI_CH_T_StopTER] ---- \n"));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_TER );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_StopTER] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}
	else
	{
		CH_T_StopTER( nChannelId );
	}

	return nResult;
}

DI_CHANNEL_ERR DI_CH_T_ResetGpio( HUINT32 ulInstanceId )
{
	int nResult = DI_CH_OK;
	int nChannelId;

	CH_DI_Print(CH_LOG_4, ("[DI_CH_T_ResetGpio] ---- \n"));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_TER );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_ResetGpio] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}
	else
	{
		TER_ResetGpio( nChannelId );
	}

	return nResult;
}

DI_CHANNEL_ERR DI_CH_T_Init( HUINT32 ulInstanceId )
{
	int nResult = DI_CH_OK;
	int nChannelId;

	CH_DI_Print(CH_LOG_4, ("[DI_CH_T_Init] ---- \n "));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_TER );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_Init] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}
	else
	{
		TER_Initialize(nChannelId);
	}

	return nResult;
}


DI_CHANNEL_ERR DI_CH_T_GetAgc( HUINT32 ulInstanceId,  HUINT8 *ucRfAgc, HUINT8 *ucIfAgc)
{
	int nResult = DI_CH_OK;
	int nRet = DI_CH_OK;
	int nChannelId;

	CH_DI_Print(CH_LOG_4, ("[DI_CH_T_GetAgc] ---- \n "));

	if ( ucRfAgc == NULL || ucIfAgc == NULL )
	{
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_GetAgc] ERR! \n "));
		return DI_CH_PARAM_ERR;
	}

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_TER );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_GetAgc] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	nRet = CH_T_GetAGC( nChannelId, ucRfAgc, ucIfAgc);
	if( nRet != DI_CH_OK )
	{
		nResult = DI_CH_ERR;
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_GetAgc] ERR! CH_T_GetAGC() nRet(%d) \n", nRet));
	}

	return nResult;

}

DI_CHANNEL_ERR DI_CH_T_GetEwsInfo( HUINT32 ulInstanceId,  HUINT8 *ucIsEws )
{
	int nResult = DI_CH_OK;
	int nRet = DI_CH_OK;
	int nChannelId;

	CH_DI_Print(CH_LOG_4, ("[DI_CH_T_GetEwsInfo] ---- \n "));

	if ( ucIsEws == NULL )
	{
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_GetEwsInfo] ERR! \n "));
		return DI_CH_PARAM_ERR;
	}

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_TER );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_GetEwsInfo] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	nRet = CH_T_GetEwsInfo( nChannelId, ucIsEws);
	if( nRet != DI_CH_OK )
	{
		nResult = DI_CH_ERR;
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_GetEwsInfo] ERR! CH_T_GetAGC() nRet(%d) \n", nRet));
	}

	return nResult;

}

DI_CHANNEL_ERR DI_CH_T_SetBERCalTime( HUINT32 ulInstanceId, HUINT32 berCalMode)
{
	int nResult = DI_CH_OK;
	int nRet = DI_CH_OK;
	int nChannelId;

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_TER );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_SetBERCalTime] ERR! ulInstanceId(%d)\n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	nRet = CH_T_BERCalTime( nChannelId, berCalMode);
	if( nRet != DI_CH_OK )
	{
		nResult = DI_CH_ERR;
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_SetBERCalTime] ERR! CH_T_BERCalTime() nRet(%d)\n", nRet));
	}

	return nResult;

}


/**
 * MW get multiple PLP info.
 *
 * \param[in]	ulInstanceId	Logical Tuner ID
 * \param[out]	pucPlpIds		a data pointer that contain PLP info.
 * \param[out]	pucNumPlps		the number of PLPs.
 *
 * \return      OK, or other Error on failure
 *
 * \remarks     DVB-T2 only.
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_T_GetDataPlpInfo( HUINT32 ulInstanceId,  HUINT8 *pucPlpIds, HUINT8 *pucNumPlps )
{
	int nResult = DI_CH_OK;
	int nRet = DI_CH_OK;
	int nChannelId;

CH_DI_Print(CH_LOG_4, ("[DI_CH_T_GetDataPlpInfo] ---- \n"));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_TER );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_GetDataPlpInfo] ERR! ulInstanceId(%d)\n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	if ( ( pucPlpIds == NULL ) || ( pucNumPlps == NULL ) )
	{
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_GetDataPlpInfo] ERR! ulInstanceId(%d)\n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	nRet = CH_T_GetDataPlpInfo( nChannelId, pucPlpIds, pucNumPlps );
	if( nRet != DI_CH_OK)
	{
		nResult = DI_CH_ERR;
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_GetDataPlpInfo] ERR! CH_T_GetDataPlpInfo() nRet(%d)\n", nRet));
	}

	return nResult;
}

/**
 * MW get the number of multiple PLPId.
 *
 * \param[in]	ulInstanceId	Logical Tuner ID
 * \param[out]	pucNumPlps		the number of PLPs.
 *
 * \return      OK, or other Error on failure
 *
 * \remarks     DVB-T2 only.
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_T_GetDataPlpCount( HUINT32 ulInstanceId, HUINT8 *pucNumPlps )
{
	int nResult = DI_CH_OK;
	int nRet = DI_CH_OK;
	int nChannelId;

CH_DI_Print(CH_LOG_4, ("[DI_CH_T_GetDataPlpCount] ---- \n"));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_TER );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_GetDataPlpCount] ERR! ulInstanceId(%d)\n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	if ( pucNumPlps == NULL )
	{
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_GetDataPlpCount] ERR! ulInstanceId(%d)\n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	nRet = CH_T_GetDataPlpCount( nChannelId, pucNumPlps );
	if( nRet != DI_CH_OK)
	{
		nResult = DI_CH_ERR;
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_GetDataPlpCount] ERR!  nRet(%d)\n", nRet));
	}

	return nResult;
}


DI_CHANNEL_ERR DI_CH_T_GetCellId( HUINT32 ulInstanceId, HUINT16 *pusCellId )
{
	int nResult = DI_CH_OK;
	int nRet = DI_CH_OK;
	int nChannelId;

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_TER );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_GetCellId] ERR! ulInstanceId(%d)\n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	if ( pusCellId == NULL )
	{
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_GetCellId] ERR! ulInstanceId(%d)\n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	nRet = CH_T_GetCellId(nChannelId, pusCellId);
	if( nRet != DI_CH_OK)
	{
		nResult = DI_CH_ERR;
		CH_DI_Print(CH_LOG_0, ("[DI_CH_T_GetCellId] ERR!  nRet(%d)\n", nRet));
	}

	return nResult;
}


DI_CHANNEL_ERR DI_CH_T_SetLoopthGpio(HBOOL bHigh)
{
	DRV_Error 		dResult = DRV_OK;

#if !defined(CONFIG_EXT_DEMOD_POWER_CONTROL)
	GPIO_STATE_t 	val = GPIO_HI;
#endif
	CH_DI_Print(CH_LOG_4, ("DI_CH_T_SetLoopthGpio] LOOPTHROUGH %s \n", bHigh? "On":"Off"));

#if defined(CONFIG_EXT_DEMOD_POWER_CONTROL)
	DRV_PM_DemodPowerSet(bHigh);
#else
	if(bHigh == TRUE)
	{
		val = GPIO_HI;
	}
	else
	{
		val = GPIO_LOW;
	}

	CH_DI_Print(CH_LOG_4, ("DI_CH_T_SetLoopthGpio : val = %d \n", val));

	dResult = DRV_GPIO_Write(GPIO_ID_LOOPTHROUGH, val);
	if(dResult != DRV_OK)
	{
		CH_DI_Print(0, ("[DI_CH_T_SetLoopthGpio] DRV_GPIO_Write failed!\n"));
		return DI_CH_ERR;
	}
#endif

	return DI_CH_OK;
}

//JAMES
DI_CHANNEL_ERR DI_CH_T_Requestdump(HUINT32 ulInstanceId)
{
	int nResult = DI_CH_OK;
	int nChannelId;

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_TER );

	nResult = CH_T_DumpRegister(nChannelId);

	return nResult;
}

DI_CHANNEL_ERR DI_CH_T_READ(HUINT32 ulInstanceId, HUINT32 uAddr )
{
	int nResult = DI_CH_OK;
	int nChannelId;

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_TER );

	nResult = CH_T_READ(nChannelId, uAddr);

	return nResult;
}

DI_CHANNEL_ERR DI_CH_T_WRITE(HUINT32 ulInstanceId, HUINT32 uAddr, HUINT32 uData)
{
	int nResult = DI_CH_OK;
	int nChannelId;

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_TER );

	nResult = CH_T_WRITE(nChannelId, uAddr, uData);

	return nResult;
}

DI_CHANNEL_ERR DI_CH_T_GetDeviceStatus(HUINT32 ulInstanceId,DI_CH_DeviceStatus * pstDeviceStatus)
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

	nRet = CH_T_GetDeviceStatus( nChannelId, pstDeviceStatus );
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_C_GetDeviceStatus] ERR! CH_C_GetSignalInfo() nRet (%d) \n", nRet));
	}

	return nRet;
}


/*********************** End of File ******************************/
