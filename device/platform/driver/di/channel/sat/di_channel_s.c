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
#include "di_channel_s.h"

#include "di_channel_priv.h"
#include "di_channel_attr.h"
#include "di_channel_util.h"

#include "drv_channel.h"
#include "drv_channel_config.h"

#include "drv_gpio.h"

// End Including Headers


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
// Start #define
#define CH_S_FREQUENCY_MIN		950 //000		//Unit MHz
#define CH_S_FREQUENCY_MAX 		2150 //000	//Unit MHz
#define CH_S_SYMBOLRATE_MIN		1000 		//Unit KSymbol/sec
#define CH_S_SYMBOLRATE_MAX		45000		//Unit KSymbol/sec

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
static void DisplayChannelInformation(
	HUINT32	ulInstanceId ,
	HUINT32 ulRequestId,
	CH_SAT_TuneParam_t* stChannelParam
)
{
#if !defined(__CH_DEBUG__) || !defined(__CH_RELEASE_MODE_DEBUG__)
	CH_UNUSED(ulInstanceId);
	CH_UNUSED(ulRequestId);
#endif

	CH_DI_Print(CH_LOG_DEFAULT, (" Request Lock >> ==============================================================\r\n"));

	CH_DI_Print(CH_LOG_DEFAULT, ("\r\n"));
	CH_DI_Print(CH_LOG_DEFAULT, ("	* Tuner Instance : (%d)\r\n", ulInstanceId));
	CH_DI_Print(CH_LOG_DEFAULT, ("	* Tuner RequestId : (%d)\r\n", ulRequestId));
	CH_DI_Print(CH_LOG_DEFAULT, ("	* Antennal Type : (%d)\r\n", stChannelParam->etAntennaType));
	CH_DI_Print(CH_LOG_DEFAULT, ("	* Local Frequency : %d [KHz]\r\n", stChannelParam->ulFrequency));
	CH_DI_Print(CH_LOG_DEFAULT, ("	* Symbol Rate : (%d) [KHz]\r\n", stChannelParam->ulSymbolRate));
	CH_DI_Print(CH_LOG_DEFAULT, ("	* Polarization : "));

	switch(stChannelParam->etPolarization)
	{
		case DI_CH_POLAR_AUTO:
			CH_DI_Print(CH_LOG_DEFAULT, ("Automatic\r\n"));
			break;
		case DI_CH_POLAR_HOR:
			CH_DI_Print(CH_LOG_DEFAULT, ("Horizontal\r\n"));
			break;
		case DI_CH_POLAR_VER:
			CH_DI_Print(CH_LOG_DEFAULT, ("Vertical\r\n"));
			break;
		case DI_CH_POLAR_LEFT:
			CH_DI_Print(CH_LOG_DEFAULT, ("Left\r\n"));
			break;
		case DI_CH_POLAR_RIGHT:
			CH_DI_Print(CH_LOG_DEFAULT, ("Right\r\n"));
			break;
	}

	CH_DI_Print(CH_LOG_DEFAULT, ("	* Code Rate : "));
	switch(stChannelParam->etFecCodeRate)
	{
		case DI_CH_SCODERATE_AUTO:
			CH_DI_Print(CH_LOG_DEFAULT, ("Automatic\r\n"));
			break;
		case DI_CH_SCODERATE_1_2:
			CH_DI_Print(CH_LOG_DEFAULT, ("1/2\r\n"));
			break;
		case DI_CH_SCODERATE_2_3:
			CH_DI_Print(CH_LOG_DEFAULT, ("2/3\r\n"));
			break;
		case DI_CH_SCODERATE_3_4:  /* 3 */
			CH_DI_Print(CH_LOG_DEFAULT, ("3/4\r\n"));
			break;
		case DI_CH_SCODERATE_5_6:
			CH_DI_Print(CH_LOG_DEFAULT, ("5/6\r\n"));
			break;
		case DI_CH_SCODERATE_7_8:
			CH_DI_Print(CH_LOG_DEFAULT, ("7/8\r\n"));
			break;
		case DI_CH_S2CODERATE_3_5:
			CH_DI_Print(CH_LOG_DEFAULT, ("3/5\r\n"));
			break;
		case DI_CH_S2CODERATE_4_5:
			CH_DI_Print(CH_LOG_DEFAULT, ("4/5\r\n"));
			break;
		case DI_CH_S2CODERATE_8_9:
			CH_DI_Print(CH_LOG_DEFAULT, ("8/9\r\n"));
			break;
		case DI_CH_S2CODERATE_9_10:  /* 9 */
			CH_DI_Print(CH_LOG_DEFAULT, ("9/10\r\n"));
			break;
		case DI_CH_SCODERATE_5_11:
			CH_DI_Print(CH_LOG_DEFAULT, ("5/11\r\n"));
			break;
		case DI_CH_SCODERATE_6_7:
			CH_DI_Print(CH_LOG_DEFAULT, ("6/7\r\n"));
			break;
	}


	CH_DI_Print(CH_LOG_DEFAULT, ("	* LNB Voltage : "));
	switch(stChannelParam->etLnbVoltage)
	{
		case DI_CH_LNB_VOLT_STD:
			CH_DI_Print(CH_LOG_DEFAULT, ("Standard\r\n"));
			break;
		case DI_CH_LNB_VOLT_HIGH:
			CH_DI_Print(CH_LOG_DEFAULT, ("High\r\n"));
			break;
	}

	CH_DI_Print(CH_LOG_DEFAULT, ("	* 22 KHz Tone : "));
	switch(stChannelParam->b22kTone)
	{
		case 0:
			CH_DI_Print(CH_LOG_DEFAULT, ("OFF\r\n"));
			break;
		case 1:
			CH_DI_Print(CH_LOG_DEFAULT, ("ON\r\n"));
			break;
	}

	CH_DI_Print(CH_LOG_DEFAULT, ("	* Diseqc : "));
	switch(stChannelParam->etDiseqcInput)
	{
		case DI_CH_DISEQC_INPUT_NONE:
			CH_DI_Print(CH_LOG_DEFAULT, ("NONE\r\n"));
			break;
		case DI_CH_DISEQC_INPUT_A:
			CH_DI_Print(CH_LOG_DEFAULT, ("A\r\n"));
			break;
		case DI_CH_DISEQC_INPUT_B:
			CH_DI_Print(CH_LOG_DEFAULT, ("B\r\n"));
			break;
		case DI_CH_DISEQC_INPUT_C:
			CH_DI_Print(CH_LOG_DEFAULT, ("C\r\n"));
			break;
		case DI_CH_DISEQC_INPUT_D:
			CH_DI_Print(CH_LOG_DEFAULT, ("D\r\n"));
			break;
		case DI_CH_DISEQC_INPUT_TB_A:
			CH_DI_Print(CH_LOG_DEFAULT, ("TB_A\r\n"));
			break;
		case DI_CH_DISEQC_INPUT_TB_B:
			CH_DI_Print(CH_LOG_DEFAULT, ("TB_B\r\n"));
			break;
		case DI_CH_DISEQC_INPUT_SCD_A:
			CH_DI_Print(CH_LOG_DEFAULT, ("SCD_A\r\n"));
			break;
		case DI_CH_DISEQC_INPUT_SCD_B:
			CH_DI_Print(CH_LOG_DEFAULT, ("SCD_B\r\n"));
			break;
	}

	CH_DI_Print(CH_LOG_DEFAULT, ("	* Diseqc Version : "));
	switch(stChannelParam->etDiseqcVersion)
	{
		case DI_CH_DISEQC_VER_1_0:
			CH_DI_Print(CH_LOG_DEFAULT, ("1.0\r\n"));
			break;
		case DI_CH_DISEQC_VER_1_1:
			CH_DI_Print(CH_LOG_DEFAULT, ("1.1\r\n"));
			break;
		case DI_CH_DISEQC_VER_1_2:
			CH_DI_Print(CH_LOG_DEFAULT, ("1.2\r\n"));
			break;
		case DI_CH_DISEQC_VER_1_3:
			CH_DI_Print(CH_LOG_DEFAULT, ("1.3\r\n"));
			break;
		case DI_CH_DISEQC_VER_2_0:
			CH_DI_Print(CH_LOG_DEFAULT, ("2.0\r\n"));
			break;
	}

	CH_DI_Print(CH_LOG_DEFAULT, ("	* Transition Spec : "));
	switch(stChannelParam->etTransSpec)
	{
		case DI_CH_TRANS_DVBS:
			CH_DI_Print(CH_LOG_DEFAULT, ("DVB\r\n"));
			break;
		case DI_CH_TRANS_DVBS2:
			CH_DI_Print(CH_LOG_DEFAULT, ("DVB-S2\r\n"));
			break;
	}

	CH_DI_Print(CH_LOG_DEFAULT, ("	* Modulation : "));
	switch(stChannelParam->etModulation)
	{
		case DI_CH_PSK_AUTO:
			CH_DI_Print(CH_LOG_DEFAULT, ("AUTO\r\n"));
			break;
		case DI_CH_PSK_QPSK:
			CH_DI_Print(CH_LOG_DEFAULT, ("QPSK\r\n"));
			break;
		case DI_CH_PSK_8PSK:
			CH_DI_Print(CH_LOG_DEFAULT, ("8PSK\r\n"));
			break;
	}

	CH_DI_Print(CH_LOG_DEFAULT, ("	* Pilot On-Off : "));
	switch(stChannelParam->etPilot)
	{
		case DI_CH_PILOT_AUTO:
			CH_DI_Print(CH_LOG_DEFAULT, ("PILOT AUTO\r\n"));
			break;
		case DI_CH_PILOT_OFF:
			CH_DI_Print(CH_LOG_DEFAULT, ("PILOT OFF\r\n"));
			break;
		case DI_CH_PILOT_ON:
			CH_DI_Print(CH_LOG_DEFAULT, ("PILOT ON\r\n"));
			break;
	}

	CH_DI_Print(CH_LOG_DEFAULT, ("	* ROLL OFF: "));
	switch(stChannelParam->etRollOff)
	{
		case DI_CH_ROLL_020:
			CH_DI_Print(CH_LOG_DEFAULT, ("ROLL 0.20\r\n"));
			break;
		case DI_CH_ROLL_025:
			CH_DI_Print(CH_LOG_DEFAULT, ("ROLL 0.25\r\n"));
			break;
		case DI_CH_ROLL_035:
			CH_DI_Print(CH_LOG_DEFAULT, ("ROLL 0.35\r\n"));
			break;
	}

	if((stChannelParam->etDiseqcInput == DI_CH_DISEQC_INPUT_SCD_A)
		|| (stChannelParam->etDiseqcInput == DI_CH_DISEQC_INPUT_SCD_B))
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("SCD Information\r\n"));
		CH_DI_Print(CH_LOG_DEFAULT, ("	* User Band(%d)\r\n", stChannelParam->ulScdUserBandNum));
		CH_DI_Print(CH_LOG_DEFAULT, ("	* User Frequency(%d)\r\n", stChannelParam->ulScdUserBandFreq));
		CH_DI_Print(CH_LOG_DEFAULT, ("	* Master Unit Id(%d)\r\n", stChannelParam->ulScdMasterUnitId));
	}
#if defined(CONFIG_SUPPORT_SELECTABLE_RF)
	CH_DI_Print(CH_LOG_DEFAULT, ("	* Selected ADC(%d)\r\n", stChannelParam->ulSelectedAdc));
#endif

CH_DI_Print(CH_LOG_DEFAULT, ("==========================================================================\r\n"));
}

static int CheckSatTuneParam (CH_SAT_TuneParam_t* stInputParam )
{
	int	nResult = DI_CH_OK;

	if( stInputParam == NULL )
	{
		return DI_CH_PARAM_ERR;
	}

	if( stInputParam->etAntennaType != DI_CH_ANT_TYPE_SCD )
	{
		if( (stInputParam->ulFrequency < CH_S_FREQUENCY_MIN) || (stInputParam->ulFrequency > CH_S_FREQUENCY_MAX) )
		{
			CH_DI_Print(CH_LOG_DEFAULT, ("[CheckSatTuneParam] ERR! ulFrequency (%d) \n", stInputParam->ulFrequency));
			nResult = DI_CH_PARAM_ERR;
		}
	}
	else
	{
		/* In case of SCD, Which range of frequency is used? */
	}

	if( (stInputParam->ulSymbolRate < CH_S_SYMBOLRATE_MIN) || (stInputParam->ulSymbolRate > CH_S_SYMBOLRATE_MAX) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CheckSatTuneParam] ERR! ulSymbolRate (%d) \n", stInputParam->ulSymbolRate));
		nResult = DI_CH_PARAM_ERR;
	}

	if( stInputParam->etTransSpec == DI_CH_TRANS_DVBS )
	{
		//Check code rate
		if( stInputParam->etFecCodeRate == DI_CH_SCODERATE_AUTO )
		{
			//It's OK
		}
		else if( ((int)stInputParam->etFecCodeRate < 0) || (stInputParam->etFecCodeRate > DI_CH_SCODERATE_7_8) )
		{
			CH_DI_Print(CH_LOG_DEFAULT, ("[CheckSatTuneParam] ERR! etFecCodeRate (%d) \n", stInputParam->etFecCodeRate));
			nResult = DI_CH_PARAM_ERR;
		}

		//Check modulation
		if( stInputParam->etModulation == DI_CH_PSK_AUTO )
		{
			//It's OK
		}
		else if( ((int)stInputParam->etModulation < 0) || (stInputParam->etModulation > DI_CH_PSK_QPSK) )
		{
			CH_DI_Print(CH_LOG_DEFAULT, ("[CheckSatTuneParam] ERR! etModulation (%d) \n", stInputParam->etModulation));
			nResult = DI_CH_NOT_SUPPORTED;
		}

	}
	else if( stInputParam->etTransSpec == DI_CH_TRANS_DVBS2 )
	{
		//Check code rate
		if( stInputParam->etFecCodeRate == DI_CH_SCODERATE_AUTO )
		{
			//It's OK
		}
		else if( ((int)stInputParam->etFecCodeRate < 0) || (stInputParam->etFecCodeRate > DI_CH_S2CODERATE_9_10) )
		{
			CH_DI_Print(CH_LOG_DEFAULT, ("[CheckSatTuneParam] ERR! etFecCodeRate (%d) \n", stInputParam->etFecCodeRate));
			nResult = DI_CH_PARAM_ERR;
		}

		//Check modulation
		if( stInputParam->etModulation == DI_CH_PSK_AUTO )
		{
			//It's OK
		}
		else if( ((int)stInputParam->etModulation < 0) || (stInputParam->etModulation > DI_CH_PSK_8PSK) )
		{
			CH_DI_Print(CH_LOG_DEFAULT, ("[CheckSatTuneParam] ERR! etModulation (%d) \n", stInputParam->etModulation));
			nResult = DI_CH_PARAM_ERR;
		}

	}
	else
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CheckSatTuneParam] ERR! etTransSpec (%d) \n", stInputParam->etTransSpec));
		nResult = DI_CH_PARAM_ERR;
	}

	if( ((int)stInputParam->etAntennaType < 0) || (stInputParam->etAntennaType > DI_CH_ANT_TYPE_SMATV) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CheckSatTuneParam] ERR! etAntennaType (%d) \n", stInputParam->etAntennaType));
		nResult = DI_CH_PARAM_ERR;
	}

	if( stInputParam->etAntennaType != DI_CH_ANT_TYPE_SCD )
	{
		if( ((int)stInputParam->etDiseqcVersion < 0) || (stInputParam->etDiseqcVersion > DI_CH_DISEQC_VER_2_0) )
		{
			CH_DI_Print(CH_LOG_DEFAULT, ("[CheckSatTuneParam] ERR! etDiseqcVersion (%d) \n", stInputParam->etDiseqcVersion));
			nResult = DI_CH_PARAM_ERR;
		}
	}
	else
	{
		/* In case of SCD, DiSEqC version not used */
	}

	if( ((int)stInputParam->etDiseqcInput < 0) || (stInputParam->etDiseqcInput > DI_CH_DISEQC_INPUT_SCD_B) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CheckSatTuneParam] ERR! etDiseqcInput (%d) \n", stInputParam->etDiseqcInput));
		nResult = DI_CH_PARAM_ERR;
	}

#if 0 /* TODO_ILEE */
	if( (stInputParam->etToneBurst < 0) || (stInputParam->etToneBurst > DI_CH_TONE_BURST_B) )
	{
		CH_DI_Print(0, ("etToneBurst       [%d]", stInputParam->etToneBurst);
		nResult = DI_CH_PARAM_ERR;
	}

	CH_DI_Print("   etPolarization    [%d]\n", stInputParam->etPolarization);
	CH_DI_Print("   b22kTone          [%d]\n", stInputParam->b22kTone);
	CH_DI_Print("   etLnbVoltage      [%d]\n", stInputParam->etLnbVoltage);

	CH_DI_Print("   ulScdUserBandNum  [%d]\n", stInputParam->ulScdUserBandNum);
	CH_DI_Print("   ulScdUserBandFreq [%d]\n", stInputParam->ulScdUserBandFreq);

	CH_DI_Print("   etPilot           [%d]\n", stInputParam->etPilot);
	CH_DI_Print("   etRollOff         [%d]\n", stInputParam->etRollOff);
#endif

	return nResult;
}


DI_CHANNEL_ERR DI_CH_S_RegisterCallback( HUINT32 ulInstanceId, pfnDI_CH_NotifySignalInfo pfnNotify)
{
	int nResult = DI_CH_OK;
	int nChannelId;

	CH_DI_Print(CH_LOG_ARGS, ("[DI_CH_S_RegisterCallback] -----\n"));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_SAT );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_RegisterCallback] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	if ( (g_stChannelConfig[nChannelId].SignalCallback == NULL) \
		|| (pfnNotify == NULL) )
	{
		CH_DI_Print(CH_LOG_ARGS, ("Callback is registered...\n"));
		g_stChannelConfig[nChannelId].SignalCallback = pfnNotify;
	}
	else
	{
		/* callback is already regitered */
	}

	return nResult;
}


DI_CHANNEL_ERR DI_CH_S_ActivateChannel( HUINT32 ulInstanceId )
{
	int nResult = DI_CH_OK;
	int nRet = DI_CH_OK;
	int nChannelId;

	CH_DI_Print(CH_LOG_ARGS, ("[DI_CH_S_ActivateChannel] -----\n"));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_SAT );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_ActivateChannel] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}
	else
	{
		nRet = CH_S_SetPowerState( nChannelId, CH_POWER_ON );
		if(nRet != DI_CH_OK)
		{
			CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_ActivateChannel] ERR! CH_S_SetPowerState() nRet(%d) \n ", nRet));
			nResult = DI_CH_ERR;
		}
		//nResult = DRV_CH_ChangeTsPath( nChannelID );
	}

	return nResult;
}


DI_CHANNEL_ERR DI_CH_S_DeactivateChannel( HUINT32 ulInstanceId )
{
	int nResult = DI_CH_OK;
	int nRet = DI_CH_OK;
	int nChannelId;

	CH_SAT_TuneParam_t * pstSatTuneParam;
	int nMasterUnitId=0;

	CH_DI_Print(CH_LOG_ARGS, ("[DI_CH_S_DeactivateChannel] -----\n"));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_SAT );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_DeactivateChannel] ERR! ulInstanceId(%d)", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}
	else
	{
		CH_S_SetPowerState(nChannelId, CH_POWER_OFF);
		if(nRet != DI_CH_OK)
		{
			CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_DeactivateChannel] ERR! CH_S_SetPowerState() \n"));
			nResult = DI_CH_ERR;
		}
		CH_DI_Print(CH_LOG_DEFAULT, ("DI_CH_S_DeactivateChannel [%d ] CH_STATUS_IDLE: \n",nChannelId));
		SetTuneStatus ( nChannelId, CH_STATUS_IDLE );
	}

	pstSatTuneParam = (CH_SAT_TuneParam_t *)GetWorkingTuneInfo( nChannelId );

	if( pstSatTuneParam->etAntennaType == DI_CH_ANT_TYPE_SCD)
	{
		nMasterUnitId = pstSatTuneParam->ulScdMasterUnitId;

		/* Multi user 일 경우 automatic mode 를 사용하는 user 와 충돌이 날 가능성  */
		/*
		nRet = CH_S_ControlScd( nMasterUnitId, ODU_PowerOFF, pstSatTuneParam );
		if( nRet != DI_CH_OK )
		{
			CH_DI_Print(0, (">>>>>>>>>>>CH_S_ControlScd<<<<<<<<<<<<<< \n"));
			nRet = CH_S_ControlScd(  nMasterUnitId, ODU_PowerOFF, pstSatTuneParam );
		}
		else
		{
			//OK
		}
		*/
	}
	return nResult;
}


DI_CHANNEL_ERR DI_CH_S_RequestLock( HUINT32 ulInstanceId, HUINT32 ulRequestId, CH_SAT_TuneParam_t * pstParam )
{
	int nRet 	= DI_CH_OK;
	int nResult = DI_CH_OK;
	int nChannelId;
	HUINT32 ulSearchMode = 0;

	if (pstParam == NULL)
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_C_RequestLock] ERR! pstParam is Null\n"));
		return DI_CH_ERR;
	}

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_SAT );
	if (pstParam)
	{
		if (pstParam->etAntennaType == DI_CH_ANT_TYPE_SCD)
		{
			pstParam->ulScdMasterUnitId = FindChannelID(pstParam->ulScdMasterUnitId , DI_CH_TYPE_SAT);
		}
	}

	CH_DI_Print(CH_LOG_ARGS, ("[DI_CH_S_RequestLock] nChannelId(%d)\n", nChannelId));

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_RequestLock] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

#if defined(CONFIG_SUPPORT_SELECTABLE_RF)
	pstParam->ulSelectedAdc = GetRfInputId(nChannelId);
#endif

	DisplayChannelInformation(ulInstanceId , ulRequestId, pstParam);

	nRet = CheckSatTuneParam( pstParam );
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_RequestLock] ERR! CheckSatTuneParam() nRet(%d)", nRet));
		return nRet;
	}
	else
	{
		SetTuneStatus( nChannelId, CH_STATUS_LOCKING );
	}

	nRet = UpdateTuneParam( nChannelId, ulRequestId, (void *) pstParam, sizeof(CH_SAT_TuneParam_t) );
	if ( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_RequestLock] ERR! UpdateTuneParam() nRet(0x%x)", nRet));
		nResult = DI_CH_ERR;
	}

	CH_DI_Print(CH_LOG_ARGS, ("[DI_CH_S_RequestLock] ulRequestId : [%d] \n", ulRequestId));

	if( CompareTuneParam( nChannelId ) )
	{
		CH_DI_Print(CH_LOG_TUNE, ("Already locked and same request param...\n"));
		SetTuneStatus( nChannelId, CH_STATUS_LOCKED );
		DRV_CH_CallNotifyCallback( nChannelId, ulRequestId, DI_CH_SIGNAL_LOCKED );

		return DI_CH_OK;
	}

	if( pstParam->etPolarization != DI_CH_POLAR_AUTO )
	{
		ulSearchMode = GetSearchMode( nChannelId );
		if( ulSearchMode & CH_S_POLAR )
		{
			SetSearchMode( nChannelId, ( ulSearchMode & (~CH_S_POLAR) ));
		}
	}
	
	if( pstParam->etFecCodeRate != DI_CH_SCODERATE_AUTO ) 
	{
		ulSearchMode = GetSearchMode( nChannelId );
		if( ulSearchMode & CH_S_CODERATE )
		{
			SetSearchMode( nChannelId, ( ulSearchMode & (~CH_S_CODERATE) ));
		}
	}

	nRet = SendChannelCommand( CH_CMD_LOCKING, nChannelId, ulRequestId );
	if ( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_RequestLock] ERR! SendChannelCommand() nRet(0x%x)", nRet));
		nResult = DI_CH_ERR;
	}

	return nResult;

}



DI_CHANNEL_ERR DI_CH_S_CancelLock( HUINT32 ulInstanceId )
{
	int nResult = DI_CH_OK;
	int nChannelId;

	CH_DI_Print(CH_LOG_ARGS, ("[DI_CH_S_CancelLock] -----\n"));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_SAT );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_CancelLock] ERR! ulInstanceId(%d)", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}
	else
	{
		//CH_DI_Print(1,("\n CancelLock !! \n\n "));
		SetTuneStatus ( nChannelId, CH_STATUS_CANCLE );

		/* Turn TS off... */
		CH_S_DisableTsOutput( nChannelId );
	}

	return nResult;
}


DI_CHANNEL_ERR DI_CH_S_GetLockedInfo( HUINT32 ulInstanceId, CH_SAT_TuneParam_t * pstParam )
{
	int nResult = DI_CH_OK;
	CH_Status_e etTuneStatus;
	int nChannelId;

	CH_DI_Print(CH_LOG_ARGS, ("[DI_CH_S_GetLockedInfo] -----\n"));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_SAT );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_GetLockedInfo] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	if ( pstParam == NULL )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_GetLockedInfo] ERR!pstParam == NULL \n "));
		return DI_CH_PARAM_ERR;
	}

	etTuneStatus = 	GetTuneStatus(nChannelId);
	CH_DI_Print(CH_LOG_ARGS, ("etTuneStatus (%d) \n", etTuneStatus));

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


DI_CHANNEL_ERR DI_CH_S_GetSignalInfo ( HUINT32 ulInstanceId, DI_CH_SignalQuality_t *pstSignalInfo )
{
	int nResult = DI_CH_OK;
	int nRet	= DI_CH_OK;
	int nChannelId;

	if ( pstSignalInfo == NULL )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_GetSignalInfo] ERR! pstSignalInfo== NULL ! \n "));
		return DI_CH_PARAM_ERR;
	}

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_SAT );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("DI_CH_S_GetSignalInfo] ERR! [ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	nRet = CH_S_GetSignalInfo( nChannelId, pstSignalInfo);
	if( nRet != DI_CH_OK )
	{
		nResult = DI_CH_ERR;
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_GetSignalInfo] ERR! CH_S_GetSignalInfo() nRet (%d) \n", nRet));
	}

	return nResult;
}


DI_CHANNEL_ERR DI_CH_S_GetDeviceStatus(HUINT32 ulInstanceId,DI_CH_DeviceStatus * pstDeviceStatus)
{
	int nRet = DI_CH_OK;
	int nChannelId;

	CH_DI_Print(CH_LOG_ARGS, ("[DI_CH_S_GetDeviceStatus] ---- \n "));

	if ( pstDeviceStatus == NULL )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_GetDeviceStatus] ERR! \n"));
		return DI_CH_PARAM_ERR;
	}

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_SAT );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_GetDeviceStatus] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	nRet = CH_S_GetDeviceStatus( nChannelId, pstDeviceStatus );
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_GetDeviceStatus] ERR! CH_S_GetDeviceStatus() nRet (%d) \n", nRet));
	}

	return nRet;
}


DI_CHANNEL_ERR DI_CH_S_SendDiseqcMsg( HUINT32 ulInstanceId, HUINT8 * pucData, HUINT8 ucLength)
{
	int nResult = DI_CH_OK;
	int nRet	= DI_CH_OK;
	int nChannelId;

	CH_DI_Print(CH_LOG_ARGS, ("[DI_CH_S_SendDiseqcMsg] -----\n"));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_SAT );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_SendDiseqcMsg] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	if ( ( pucData == NULL ) || ( ucLength == 0 ))
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_SendDiseqcMsg] ERR! pucData == NULL \n "));
		return DI_CH_PARAM_ERR;
	}

	nRet = CH_S_ControlMotor( nChannelId, pucData, ucLength );
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_SendDiseqcMsg] ERR! CH_S_ControlMotor() nRet(%d) \n  ", nRet ));
		nResult = DI_CH_ERR;
	}

	return nResult;
}


DI_CHANNEL_ERR DI_CH_S_ReceiveDiseqcMsg( HUINT32 ulInstanceId, HUINT8 * pucData, HUINT8 ucLength, HUINT8 *pucReplyLength)
{
	int nResult = DI_CH_OK;
	int nRet	= DI_CH_OK;
	int nChannelId;

	CH_DI_Print(CH_LOG_ARGS, ("[DI_CH_S_SendDiseqcMsg] -----\n"));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_SAT );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_SendDiseqcMsg] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	if ( ( pucData == NULL ) || ( ucLength == 0 ))
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_SendDiseqcMsg] ERR! pucData == NULL \n "));
		return DI_CH_PARAM_ERR;
	}

	nRet = CH_S_ReceiveDiseqcMsg( nChannelId, pucData, ucLength, pucReplyLength);
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_SendDiseqcMsg] ERR! CH_S_ControlMotor() nRet(%d) \n  ", nRet ));
		nResult = DI_CH_ERR;
	}

	return nResult;
}

DI_CHANNEL_ERR DI_CH_S_SetLnbOutput( HUINT32 ulInstanceId, HBOOL bEnable )
{
	int nResult = DI_CH_OK;
	int nRet	= DI_CH_OK;
	int nChannelId;

	CH_DI_Print(CH_LOG_ARGS, ("[DI_CH_S_SetLnbOutput] -----\n"));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_SAT );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_SetLnbOutput] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	if ( bEnable < 0 )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_SetLnbOutput] ERR! bEnable (%d) \n ", bEnable));
		return DI_CH_PARAM_ERR;
	}

	nRet = CH_S_SetLnbOutput( nChannelId, bEnable );
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_SetLnbOutput] ERR! CH_S_SetLnbOutput()  nRet(%d)\n", nRet));
		nResult = DI_CH_ERR;
	}
	else
	{
		CH_DI_Print(CH_LOG_ARGS, ("Success : CH_S_SetLnbOutput\n"));
	}

	return nResult;
}



DI_CHANNEL_ERR DI_CH_S_SetLnb22KhzTone( HUINT32 ulInstanceId, HBOOL b22KhzTone )
{
	int nResult = DI_CH_OK;
	int nRet	= DI_CH_OK;
	int nChannelId;

	CH_DI_Print(CH_LOG_ARGS, ("[DI_CH_S_SetLnb22KhzTone] -----\n"));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_SAT );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_SetLnb22KhzTone] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	if ( b22KhzTone < 0 )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_SetLnb22KhzTone] ERR! b22KhzTone (%d) \n ", b22KhzTone));
		return DI_CH_PARAM_ERR;
	}

	nRet = CH_S_SetLnb22KhzTone( nChannelId, b22KhzTone );
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_SetLnb22KhzTone] ERR! CH_S_SetLnb22KhzTone()  nRet(%d)\n", nRet));
		nResult = DI_CH_ERR;
	}
	else
	{
		CH_DI_Print(CH_LOG_ARGS, ("Success : CH_S_SetLnb22KhzTone\n"));
	}

	return nResult;
}



DI_CHANNEL_ERR DI_CH_S_SetLnbPolarisation( HUINT32 ulInstanceId, CH_SAT_Polarization_e etPolar, CH_SAT_LnbVoltage_e etVoltage )
{
	int nResult = DI_CH_OK;
	int nRet	= DI_CH_OK;
	int nChannelId ;

	CH_DI_Print(CH_LOG_ARGS, ("[DI_CH_S_SetLnbPolarisation] -----\n"));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_SAT );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_SetLnbPolarisation] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	if ( ((int)etPolar < DI_CH_POLAR_AUTO) || (etPolar > DI_CH_POLAR_RIGHT) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_SetLnbPolarisation] ERR! etPolar (%d) \n ", etPolar));
		return DI_CH_PARAM_ERR;
	}

	if ( ((int)etVoltage < DI_CH_LNB_VOLT_STD) || (etVoltage > DI_CH_LNB_VOLT_HIGH) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_SetLnbPolarisation] ERR! etVoltage (%d) \n ", etVoltage));
		return DI_CH_PARAM_ERR;
	}

	nRet = CH_S_SetLnbPolarisation( nChannelId, etPolar, etVoltage );
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_SetLnbPolarisation] ERR! CH_S_SetLnbPolarisation()  nRet(%d)\n", nRet));
		nResult = DI_CH_ERR;
	}
	else
	{
		CH_DI_Print(CH_LOG_ARGS, ("Success : CH_S_SetLnbPolarisation\n"));
	}

	return nResult;
}

DI_CHANNEL_ERR DI_CH_S_SetLoopthGpio(HBOOL bHigh)
{
	DRV_Error 		dResult = DRV_OK;
	GPIO_STATE_t 	val = GPIO_HI;

	if(bHigh == TRUE)
	{
		val = GPIO_HI;
	}
	else
	{
		val = GPIO_LOW;
	}

	CH_DI_Print(CH_LOG_ARGS, ("DI_CH_S_SetLoopthroughGpio : val = %d \n", val));

	dResult = DRV_GPIO_Write(GPIO_ID_LOOPTHROUGH, val);
	if(dResult != DRV_OK)
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_SetLoopthGpio] DRV_GPIO_Write failed!\n"));
		return DI_CH_ERR;
	}

	return DI_CH_OK;
}



/* This API will be deprecated */
DI_CHANNEL_ERR DI_CH_S_GetChannelStatus (HUINT32 ulInstanceId,  HINT32 *pulBer,  HUINT32 *pulRfAgc, HUINT32 *pulIfAgc, HUINT32 *pulUncorrect, float *pfPower)
{
	int nResult = DI_CH_OK;
	int nChannelId;

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_SAT );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL ) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_GetChannelStatus] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	CH_UNUSED(pulBer);
	CH_UNUSED(pulRfAgc);
	CH_UNUSED(pulIfAgc);
	CH_UNUSED(pulUncorrect);
	CH_UNUSED(pfPower);

	CH_DI_Print(CH_LOG_ARGS, ("[DI_CH_S_GetChannelStatus] This API has been deprecated.\n"));

	return nResult;

}



DI_CHANNEL_ERR DI_CH_S_SetInputMode( HUINT32 ulInstanceId, CH_SAT_InputMode_e etInputMode )
{
	int nResult = DI_CH_OK;
	int nRet	= DI_CH_OK;
	int nChannelId;

	CH_DI_Print(CH_LOG_ARGS, ("[DI_CH_S_SetInputMode] -----\n"));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_SAT );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_SetInputMode] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	if ( etInputMode >= DI_CH_INPUT_MAX )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_SetInputMode] ERR! etInputMode (%d) \n ", etInputMode));
		return DI_CH_PARAM_ERR;
	}

	nRet = CH_S_SetInputMode( nChannelId, etInputMode );
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_SetInputMode] ERR! DI_CH_S_SetInputMode()  nRet(%d)\n", nRet));
		nResult = DI_CH_ERR;
	}
	else
	{
		CH_DI_Print(CH_LOG_ARGS, ("Success : DI_CH_S_SetInputMode()... etInputMode(%d)\n", etInputMode));
	}

	return nResult;
}


DI_CHANNEL_ERR DI_CH_S_GetTunerCapability( HUINT32 ulInstanceId, DI_CH_TunerCapacity_t *pstTunerCap )
{
	int nResult = DI_CH_OK;

	int nChannelId = 0;
	int nRfInputId = 0;

	DI_CH_TunerCapacity_t * pstTunerCapDrv = NULL;

	if ( ulInstanceId >= DRV_CH_GetMaxInstance() )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_GetTunerCapability] ERR! InstanceId (%d) \n ", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_SAT );
	if( nChannelId != DI_CH_ERR )
	{
		nRfInputId = GetRfInputId(nChannelId);
	}
	else
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_GetTunerCapability] ERR! FindChannelID() nChannelId=(%d)\n ", nChannelId));
		return DI_CH_ERR;
	}

	if ( pstTunerCap == NULL )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_GetTunerCapability] ERR! pstTunerCap is NULL \n "));
		return DI_CH_PARAM_ERR;
	}
	else
	{
		pstTunerCapDrv = DRV_CH_GetTunerCapability((int)ulInstanceId);
		if( pstTunerCapDrv == NULL )
		{
			return DI_CH_ERR;
		}
		else
		{
			CH_DI_Print(CH_LOG_ARGS, ("[DRV_CH_GetTunerCapability]  ulNumOfInput = (%d) \n ", pstTunerCapDrv->ulNumOfInput));
			CH_DI_Print(CH_LOG_ARGS, ("[DRV_CH_GetTunerCapability]  bLoopThroughOut = (%d) \n ", pstTunerCapDrv->bLoopThroughOut));
			VK_memcpy(pstTunerCap, pstTunerCapDrv, sizeof(DI_CH_TunerCapacity_t));
			CH_DI_Print(CH_LOG_ARGS, ("[DRV_CH_GetTunerCapability]  Current RF Input of SAT[%d] = (%d) \n ", nChannelId, nRfInputId));
		}
	}

	return nResult;
}



DI_CHANNEL_ERR DI_CH_S_SetInputSource( HUINT32 ulInstanceId, DI_CH_InputSource_e etInputSource )
{
	int nResult = DI_CH_OK;
	int nRet	= DI_CH_OK;
	int nChannelId;

	CH_DI_Print(CH_LOG_ARGS, ("[DI_CH_S_SetInputSource] -----\n"));

	nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_SAT );

	if ( ( nChannelId < 0 ) || ( nChannelId >= NUM_OF_CHANNEL) )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_SetInputSource] ERR! ulInstanceId(%d) \n", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	if ( etInputSource >= DI_CH_RF_INPUTMAX || etInputSource == DI_CH_RF_INPUT_NONE )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_SetInputSource] ERR! etInputSource (%d) \n ", etInputSource));
		return DI_CH_PARAM_ERR;
	}

	nRet = CH_S_SetInputSource( nChannelId, etInputSource );
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DI_CH_S_SetInputSource] ERR! CH_S_SetInputMode()  nRet(%d)\n", nRet));
		nResult = DI_CH_ERR;
	}
	else
	{
		CH_DI_Print(CH_LOG_ARGS, ("Success : DI_CH_S_SetInputSource()... nChannelId(%d), etInputSource(%d)\n", nChannelId, etInputSource));
	}

	return nResult;
}

/*********************** End of File ******************************/

