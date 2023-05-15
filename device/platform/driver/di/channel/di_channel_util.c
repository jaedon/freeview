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

#include "vkernel.h"

#include "di_channel_priv.h"
#include "di_channel_attr.h"
#include "di_channel_util.h"

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


HBOOL CompareTuneParam( int nChannelId )
{
	HBOOL			bResult = FALSE;
	DI_CH_Type_e 	etChannelType;
	CH_Status_e 	etCurrentState;
	HUINT32 		unSize = 0;
	void  			*pvSource;
	void 			*pvTarget;

	etChannelType 	= GetChannelType( nChannelId );
	pvSource 		= GetRequestedTuneInfo( nChannelId );
	pvTarget 		= GetPreviousLockedTuneInfo( nChannelId );
	//etCurrentState	= GetTuneStatus( nChannelId );
	etCurrentState	= GetPreviousTuneStatus( nChannelId );

	//CH_SAT_TuneParam_t		stPrvTuningInfo;
	//CH_SAT_TuneParam_t		stCurTuningInfo;

	if ( (pvSource == NULL) || (pvTarget == NULL) )
	{
		bResult = FALSE;
	}
	else
	{
		switch( etChannelType )
		{
			case DI_CH_TYPE_SAT:
				unSize = sizeof(CH_SAT_TuneParam_t);
			/*
				VK_memset(&stPrvTuningInfo, 0, unSize);
				VK_memset(&stCurTuningInfo, 0, unSize);

				VK_memcpy( &stPrvTuningInfo, pvTarget, unSize);
				VK_memcpy( &stCurTuningInfo, pvSource, unSize);


				CH_PrintMon("etAntennaType= %d	/ %d \n",stPrvTuningInfo.etAntennaType, stCurTuningInfo.etAntennaType);
				CH_PrintMon("ulFrequency= %d  / %d \n",stPrvTuningInfo.ulFrequency, stCurTuningInfo.ulFrequency);

				CH_PrintMon("ulSymbolRate= %d  / %d \n",stPrvTuningInfo.ulSymbolRate, stCurTuningInfo.ulSymbolRate);
				CH_PrintMon("etFecCodeRate= %d	/ %d \n",stPrvTuningInfo.etFecCodeRate, stCurTuningInfo.etFecCodeRate);
				CH_PrintMon("etPolarization= %d  / %d \n",stPrvTuningInfo.etPolarization, stCurTuningInfo.etPolarization);
				CH_PrintMon("etTransSpec= %d  / %d \n",stPrvTuningInfo.etTransSpec, stCurTuningInfo.etTransSpec);

				CH_PrintMon("etModulation= %d  / %d \n",stPrvTuningInfo.etModulation, stCurTuningInfo.etModulation);
				CH_PrintMon("etPilot= %d  / %d \n",stPrvTuningInfo.etPilot, stCurTuningInfo.etPilot);
				CH_PrintMon("etRollOff= %d	/ %d \n",stPrvTuningInfo.etRollOff, stCurTuningInfo.etRollOff);
				CH_PrintMon("etLnbVoltage= %d  / %d \n",stPrvTuningInfo.etLnbVoltage, stCurTuningInfo.etLnbVoltage);

				CH_PrintMon("etDiseqcVersion= %d  / %d \n",stPrvTuningInfo.etDiseqcVersion, stCurTuningInfo.etDiseqcVersion);

				CH_PrintMon("etDiseqcInput= %d	/ %d \n",stPrvTuningInfo.etDiseqcInput, stCurTuningInfo.etDiseqcInput);
				CH_PrintMon("etToneBurst= %d  / %d \n",stPrvTuningInfo.etToneBurst, stCurTuningInfo.etToneBurst);
				CH_PrintMon("b22kTone= %d  / %d \n",stPrvTuningInfo.b22kTone, stCurTuningInfo.b22kTone);

				CH_PrintMon("ulScdMasterUnitId= %d	/ %d \n",stPrvTuningInfo.ulScdMasterUnitId, stCurTuningInfo.ulScdMasterUnitId);
				CH_PrintMon("ulScdUserBandNum= %d  / %d \n",stPrvTuningInfo.ulScdUserBandNum, stCurTuningInfo.ulScdUserBandNum);
				CH_PrintMon("ulScdUserBandFreq= %d	/ %d \n",stPrvTuningInfo.ulScdUserBandFreq, stCurTuningInfo.ulScdUserBandFreq);
			*/
				break;

			case DI_CH_TYPE_TER:
				//PrintTerTuneParam((CH_TER_TuneParam_t *) pvSource);
				unSize = sizeof(CH_TER_TuneParam_t);
				break;

			case DI_CH_TYPE_CAB:
				unSize = sizeof(CH_CAB_TuneParam_t);
				break;

			case DI_CH_TYPE_ANA:
				//PrintAnaTuneParam((CH_ANA_TuneParam_t *) pvSource);
				unSize = sizeof(CH_ANA_TuneParam_t);
				break;

			default:
				CH_DI_Print(0, ("[CompareTuneParam] ERR! etChannelType(%d)", etChannelType));
				break;
		}

		if( VK_memcmp( pvTarget, pvSource, unSize) )
		{
			//New request differ from previous. Do nothing...
			CH_DI_Print(1, ("RETRY! \n"));
			CH_DI_Print(2, (" [%d] New request differ from previous \n\n", nChannelId));
		}
		else
		{
			if( etCurrentState == CH_STATUS_LOCKED )
			{
				bResult = TRUE;
				CH_DI_Print(1, ("PASS! \n"));
			}
			else
			{
				//do nothing...
				CH_DI_Print(1, ("RETRY! \n"));
				CH_DI_Print(2, (" [%d] Is Not Locked previous. \n\n", nChannelId));
			}
		}

	}

	return bResult;
}


int UpdatePreviousLockedParam( int nChannelId )
{
	int				nResult = DI_CH_OK;
	DI_CH_Type_e 	etChannelType;
	HUINT32 		unSize = 0;
	void  			*pvSource;
	void 			*pvTarget;

	etChannelType 	= GetChannelType( nChannelId );
	pvSource 		= GetRequestedTuneInfo( nChannelId );
	pvTarget 		= GetPreviousLockedTuneInfo( nChannelId );

	if ( (pvSource == NULL) || (pvTarget == NULL) )
	{
		return DI_CH_ERR;
	}
	else
	{
		switch( etChannelType )
		{
			case DI_CH_TYPE_SAT:
				unSize = sizeof(CH_SAT_TuneParam_t);
				break;

			case DI_CH_TYPE_TER:
				//PrintTerTuneParam((CH_TER_TuneParam_t *) pvSource);
				unSize = sizeof(CH_TER_TuneParam_t);
				break;

			case DI_CH_TYPE_CAB:
				unSize = sizeof(CH_CAB_TuneParam_t);
				break;

			case DI_CH_TYPE_ANA:
				//PrintAnaTuneParam((CH_ANA_TuneParam_t *) pvSource);
				unSize = sizeof(CH_ANA_TuneParam_t);
				break;

			default:
				CH_DI_Print(0, ("UpdatePreviousLockedParam] ERR! etChannelType(%d) \n", etChannelType));
				break;
		}

		VK_memcpy( pvTarget, pvSource, unSize);

	}

	return nResult;
}



int UpdateLockedParam( int nChannelId, void * pvTarget )
{
	int				nResult = DI_CH_OK;
	DI_CH_Type_e 	etChannelType;
	HUINT32 		unSize = 0;
	void * 			pvSource;

	etChannelType 	= GetChannelType( nChannelId );
	pvSource 		= GetWorkingTuneInfo( nChannelId );

	if ( (pvSource == NULL) || (pvTarget == NULL) )
	{
		return DI_CH_ERR;
	}
	else
	{
		switch( etChannelType )
		{
			case DI_CH_TYPE_SAT:
				unSize = sizeof(CH_SAT_TuneParam_t);
				break;

			case DI_CH_TYPE_TER:
				//PrintTerTuneParam((CH_TER_TuneParam_t *) pvSource);
				unSize = sizeof(CH_TER_TuneParam_t);
				break;

			case DI_CH_TYPE_CAB:
				unSize = sizeof(CH_CAB_TuneParam_t);
				break;
/*
			case DI_CH_TYPE_ANA:
				PrintAnaTuneParam((CH_ANA_TuneParam_t *) pvSource);
				unSize = sizeof(CH_ANA_TuneParam_t);
				break;
*/
			default:
				CH_DI_Print(0, ("[UpdateLockedParam] ERR! etChannelType(%d) \n", etChannelType));
				break;
		}

		VK_memcpy( pvTarget, pvSource, unSize);

	}

	return nResult;
}


int UpdateTuneParam( int nChannelID, HUINT32 unRequestID, void * vpSource, HUINT32 unSize )
{
	int nResult = DI_CH_OK;

	if ( (vpSource == NULL)||(unSize == 0))
	{
		nResult = DI_CH_ERR;
	}
	else
	{
		g_stChannelConfig[nChannelID].stTuningInfo.unRequestID = unRequestID;
		VK_memcpy( g_stChannelConfig[nChannelID].stTuningInfo.pvRequestedParam, vpSource, unSize);
	}

	return nResult;
}


int SendChannelCommand( CH_Command_e etCommand, int nChannelID, HUINT32 unRequestID )
{
	int nRet = VK_OK;
	HUINT32 aunSendMsg[4];

	if ( (nChannelID >= NUM_OF_CHANNEL) || (nChannelID < 0) )
	{
		return DI_CH_PARAM_ERR;
	}

	aunSendMsg[0] = etCommand;
	aunSendMsg[1] = nChannelID;
	aunSendMsg[2] = unRequestID;
	aunSendMsg[3] = 0;

	nRet = VK_MSG_Send(g_stChannelConfig[nChannelID].stChannelRsc.ulQueueID, aunSendMsg, CONST_CH_Q_SIZE);
	if(nRet != VK_OK)
	{
		CH_DI_Print(0, ("[SendChannelCommand] ERR! nRet(0x%x) \n", nRet));
		return DI_CH_ERR;
	}
    return DI_CH_OK;
}

#define __DI_CH_UTILITY_FUNCTIONS__

void PrintSatTuneParam (CH_SAT_TuneParam_t* stInputParam )
{
#if !defined(__CH_DEBUG__) || !defined(__CH_RELEASE_MODE_DEBUG__)
	CH_UNUSED(stInputParam);
#endif

	CH_DI_Print(3, ("===================================================\n"));
	CH_DI_Print(3, ("   SAT TUNE PARAM\n"));
	CH_DI_Print(3, ("===================================================\n"));
	//Antenna & DiSEqC info
	CH_DI_Print(3, ("### Antenna info ###\n"));
	CH_DI_Print(3, ("   etAntennaType     [%d]\n", stInputParam->etAntennaType));
	CH_DI_Print(3, ("   etDiseqcVersion   [%d]\n", stInputParam->etDiseqcVersion));
	CH_DI_Print(3, ("   etDiseqcInput     [%d]\n", stInputParam->etDiseqcInput));
	CH_DI_Print(3, ("   etToneBurst       [%d]\n", stInputParam->etToneBurst));

	CH_DI_Print(3, ("### SCIF info ###\n"));
	CH_DI_Print(3, ("   ulScdMasterUnitId [%d]\n", stInputParam->ulScdMasterUnitId));
	CH_DI_Print(3, ("   ulScdUserBandNum  [%d]\n", stInputParam->ulScdUserBandNum));
	CH_DI_Print(3, ("   ulScdUserBandFreq [%d]\n", stInputParam->ulScdUserBandFreq));
	//LNBR info
	CH_DI_Print(3, ("### LNBS info ###\n"));
	CH_DI_Print(3, ("   etPolarization    [%d]\n", stInputParam->etPolarization));
	CH_DI_Print(3, ("   etLnbVoltage      [%d]\n", stInputParam->etLnbVoltage));
	CH_DI_Print(3,( "   b22kTone          [%d]\n", stInputParam->b22kTone));
	//Tuner info
	CH_DI_Print(3, ("### Tuning info ###\n"));
	CH_DI_Print(3, ("   ulFrequency       [%d]\n", stInputParam->ulFrequency));
	CH_DI_Print(3, ("   ulSymbolRate      [%d]\n", stInputParam->ulSymbolRate));
	CH_DI_Print(3, ("   etFecCodeRate     [%d]\n", stInputParam->etFecCodeRate));
	CH_DI_Print(3, ("   etTransSpec       [%d]\n", stInputParam->etTransSpec));
	CH_DI_Print(3, ("   etModulation      [%d]\n", stInputParam->etModulation));
	CH_DI_Print(3, ("   etPilot           [%d]\n", stInputParam->etPilot));
	CH_DI_Print(3,  ("   etRollOff         [%d]\n", stInputParam->etRollOff));
	CH_DI_Print(3, ("===================================================\n"));
}

void PrintTerTuneParam (CH_TER_TuneParam_t* stInputParam )
{
#if !defined(__CH_DEBUG__) || !defined(__CH_RELEASE_MODE_DEBUG__)
		CH_UNUSED(stInputParam);
#endif

	CH_DI_Print(3, ("===================================================\n"));
	CH_DI_Print(3, ("   TER TUNE PARAM\n"));
	CH_DI_Print(3, ("===================================================\n"));
	CH_DI_Print(3, ("   ulFrequency      [%d]\n", stInputParam->ulFrequency));
	CH_DI_Print(3, ("   etBandWidth      [%d]\n", stInputParam->etBandWidth));
	CH_DI_Print(3, ("   etModulation     [%d]\n", stInputParam->etModulation));
	CH_DI_Print(3, ("   etHierarchy      [%d]\n", stInputParam->etHierarchy));
	CH_DI_Print(3, ("   etCodeRate       [%d]\n", stInputParam->etCodeRate));
	CH_DI_Print(3, ("   etTransMode      [%d]\n", stInputParam->etTransMode));
	CH_DI_Print(3, ("   etGuardInterval  [%d]\n", stInputParam->etGuardInterval));
	CH_DI_Print(3, ("   etStream         [%d]\n", stInputParam->etStream));
	CH_DI_Print(3, ("   etOffset         [%d]\n", stInputParam->etOffset));
	CH_DI_Print(3, ("   etTerSystem      [%d]\n", stInputParam->etTerSystem));
	CH_DI_Print(3, ("===================================================\n"));
}

void PrintCabTuneParam (CH_CAB_TuneParam_t* stInputParam )
{
#if !defined(__CH_DEBUG__) || !defined(__CH_RELEASE_MODE_DEBUG__)
	CH_UNUSED(stInputParam);
#endif

	CH_DI_Print(3, ("===================================================\n"));
	CH_DI_Print(3, ("   CAB TUNE PARAM\n"));
	CH_DI_Print(3, ("===================================================\n"));
	CH_DI_Print(3, ("   ulFrequency      [%d]\n", stInputParam->ulFrequency));
	CH_DI_Print(3, ("   ulSymbolRate     [%d]\n", stInputParam->ulSymbolRate));
	CH_DI_Print(3, ("   etModulation     [%d]\n", stInputParam->etModulation));
	CH_DI_Print(3, ("   etSpectrum       [%d]\n", stInputParam->etSpectrum));
	CH_DI_Print(3, ("===================================================\n"));
}

void PrintAnaTuneParam (CH_ANA_TuneParam_t* stInputParam )
{
#if !defined(__CH_DEBUG__) || !defined(__CH_RELEASE_MODE_DEBUG__)
		CH_UNUSED(stInputParam);
#endif
	CH_DI_Print(3, ("===================================================\n"));
	CH_DI_Print(3, ("   ANA TUNE PARAM\n"));
	CH_DI_Print(3, ("===================================================\n"));
	CH_DI_Print(3, ("   ulFrequency      [%d]\n", stInputParam->ulFrequency));
	CH_DI_Print(3, ("   ulFreqOffset     [%d]\n", stInputParam->ulFreqOffset));
	CH_DI_Print(3, ("   etAnalogSystem   [%d]\n", stInputParam->etAnalogSystem));
	CH_DI_Print(3, ("   bEnableAft       [%d]\n", stInputParam->bEnableAft));
	CH_DI_Print(3, ("   etTuneMode       [%d]\n", stInputParam->etTuneMode));
	CH_DI_Print(3, ("===================================================\n"));
}

/*********************** End of File ******************************/
