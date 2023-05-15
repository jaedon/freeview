/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Application
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */

/* chip manufacture's  header files */

/* humax header files */
#if !defined(CONFIG_OS_RTOS)
#include <memory.h>
#endif

#include <string.h>

#include "fdi_common.h"
#include "fdi_channel.h"

#include "di_err.h"
#include "di_channel.h"
#include "di_channel_s.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */

static unsigned long s_ulRequestId;


void INTRN_FDI_CHANNEL_S_ConvFDItoDI(FDI_CH_SAT_TUNE_PARAM_t *ptFDIParam, CH_SAT_TuneParam_t *ptDIParam)
{

	ptDIParam->ulFrequency = ptFDIParam->ulFrequency;				//unit KHz
	ptDIParam->ulSymbolRate = ptFDIParam->ulSymbolRate;
	ptDIParam->b22kTone = ptFDIParam->b22kTone;
	ptDIParam->ulScdUserBandNum = ptFDIParam->ulScdUserBandNum;
	ptDIParam->ulScdUserBandFreq = ptFDIParam->ulScdUserBandFreq;

	/*  etAntennaType  */
	switch( ptFDIParam->etAntennaType )
	{
		case FDI_CH_ANT_TYPE_LNB :
			ptDIParam->etAntennaType = DI_CH_ANT_TYPE_LNB;
			break;
		case FDI_CH_ANT_TYPE_DISEQC :
			ptDIParam->etAntennaType = DI_CH_ANT_TYPE_DISEQC;
			break;
		case FDI_CH_ANT_TYPE_SCD :
			ptDIParam->etAntennaType = DI_CH_ANT_TYPE_SCD;
			break;
		case FDI_CH_ANT_TYPE_SMATV :
			ptDIParam->etAntennaType = DI_CH_ANT_TYPE_SMATV;
			break;
		default :
			ptDIParam->etAntennaType = DI_CH_ANT_TYPE_LNB;
	}

	/*  etFecCodeRate  */
	switch( ptFDIParam->etFecCodeRate )
	{
		case FDI_CH_SCODERATE_AUTO :
			ptDIParam->etFecCodeRate = DI_CH_SCODERATE_AUTO;
			break;
		case FDI_CH_SCODERATE_1_2 :
			ptDIParam->etFecCodeRate = DI_CH_SCODERATE_1_2;
			break;
		case FDI_CH_SCODERATE_2_3 :
			ptDIParam->etFecCodeRate = DI_CH_SCODERATE_2_3;
			break;
		case FDI_CH_SCODERATE_3_4 :
			ptDIParam->etFecCodeRate = DI_CH_SCODERATE_3_4;
			break;
		case FDI_CH_SCODERATE_5_6 :
			ptDIParam->etFecCodeRate = DI_CH_SCODERATE_5_6;
			break;
		case FDI_CH_SCODERATE_7_8 :
			ptDIParam->etFecCodeRate = DI_CH_SCODERATE_7_8;
			break;
		case FDI_CH_S2CODERATE_3_5 :
			ptDIParam->etFecCodeRate = DI_CH_S2CODERATE_3_5;
			break;
		case FDI_CH_S2CODERATE_4_5 :
			ptDIParam->etFecCodeRate = DI_CH_S2CODERATE_4_5;
			break;
		case FDI_CH_S2CODERATE_8_9 :
			ptDIParam->etFecCodeRate = DI_CH_S2CODERATE_8_9;
			break;
		case FDI_CH_S2CODERATE_9_10 :
			ptDIParam->etFecCodeRate = DI_CH_S2CODERATE_9_10;
			break;
		case FDI_CH_SCODERATE_5_11 :
			ptDIParam->etFecCodeRate = DI_CH_SCODERATE_5_11;
			break;
		case FDI_CH_SCODERATE_6_7 :
			ptDIParam->etFecCodeRate = DI_CH_SCODERATE_6_7;
			break;
		default :
			ptDIParam->etFecCodeRate = DI_CH_SCODERATE_AUTO;
	}

	/*  etPolarization  */
	switch( ptFDIParam->etPolarization )
	{
		case FDI_CH_POLAR_AUTO :
			ptDIParam->etPolarization = DI_CH_POLAR_AUTO;
			break;
		case FDI_CH_POLAR_HOR :
			ptDIParam->etPolarization = DI_CH_POLAR_HOR;
			break;
		case FDI_CH_POLAR_VER :
			ptDIParam->etPolarization = DI_CH_POLAR_VER;
			break;
		case FDI_CH_POLAR_LEFT :
			ptDIParam->etPolarization = DI_CH_POLAR_LEFT;
			break;
		case FDI_CH_POLAR_RIGHT :
			ptDIParam->etPolarization = DI_CH_POLAR_RIGHT;
			break;
		default :
			ptDIParam->etPolarization = DI_CH_POLAR_AUTO;
	}

	/*  etTransSpec  */
	switch( ptFDIParam->etTransSpec )
	{
		case FDI_CH_TRANS_DVBS :
			ptDIParam->etTransSpec = DI_CH_TRANS_DVBS;
			break;
		case FDI_CH_TRANS_DVBS2 :
			ptDIParam->etTransSpec = DI_CH_TRANS_DVBS2;
			break;
		default :
			ptDIParam->etTransSpec = DI_CH_TRANS_DVBS;
	}

	/*  etDiseqcInput  */
	switch( ptFDIParam->etDiseqcInput )
	{
		case FDI_CH_DISEQC_INPUT_NONE :
			ptDIParam->etDiseqcInput = DI_CH_DISEQC_INPUT_NONE;
			break;
		case FDI_CH_DISEQC_INPUT_A :
			ptDIParam->etDiseqcInput = DI_CH_DISEQC_INPUT_A;
			break;
		case FDI_CH_DISEQC_INPUT_B :
			ptDIParam->etDiseqcInput = DI_CH_DISEQC_INPUT_B;
			break;
		case FDI_CH_DISEQC_INPUT_C :
			ptDIParam->etDiseqcInput = DI_CH_DISEQC_INPUT_C;
			break;
		case FDI_CH_DISEQC_INPUT_D :
			ptDIParam->etDiseqcInput = DI_CH_DISEQC_INPUT_D;
			break;
		case FDI_CH_DISEQC_INPUT_TB_A :
			ptDIParam->etDiseqcInput = DI_CH_DISEQC_INPUT_TB_A;
			break;
		case FDI_CH_DISEQC_INPUT_TB_B :
			ptDIParam->etDiseqcInput = DI_CH_DISEQC_INPUT_TB_B;
			break;
		case FDI_CH_DISEQC_INPUT_SCD_A :
			ptDIParam->etDiseqcInput = DI_CH_DISEQC_INPUT_SCD_A;
			break;
		case FDI_CH_DISEQC_INPUT_SCD_B :
			ptDIParam->etDiseqcInput = DI_CH_DISEQC_INPUT_SCD_B;
			break;
		default :
			ptDIParam->etDiseqcInput = DI_CH_DISEQC_INPUT_NONE;
	}

	/*  etModulation  */
	switch( ptFDIParam->etModulation )
	{
		case FDI_CH_PSK_AUTO :
			ptDIParam->etModulation = DI_CH_PSK_AUTO;
			break;
		case FDI_CH_PSK_QPSK :
			ptDIParam->etModulation = DI_CH_PSK_QPSK;
			break;
		case FDI_CH_PSK_8PSK :
			ptDIParam->etModulation = DI_CH_PSK_8PSK;
			break;
		default :
			ptDIParam->etModulation = DI_CH_PSK_AUTO;
	}

	/*  etPilot  */
	switch( ptFDIParam->etPilot )
	{
		case FDI_CH_PILOT_AUTO :
			ptDIParam->etPilot = DI_CH_PILOT_AUTO;
			break;
 		case FDI_CH_PILOT_OFF :
			ptDIParam->etPilot = DI_CH_PILOT_OFF;
			break;
		case FDI_CH_PILOT_ON :
			ptDIParam->etPilot = DI_CH_PILOT_ON;
			break;
		default :
			ptDIParam->etPilot = DI_CH_PILOT_AUTO;
 	}

	/*  etRollOff  */
	switch( ptFDIParam->etRollOff )
	{
		case FDI_CH_ROLL_020 :
			ptDIParam->etRollOff = DI_CH_ROLL_020;
			break;
		case FDI_CH_ROLL_025 :
			ptDIParam->etRollOff = DI_CH_ROLL_025;
			break;
		case FDI_CH_ROLL_035 :
			ptDIParam->etRollOff = DI_CH_ROLL_035;
			break;
		default :
			ptDIParam->etRollOff = DI_CH_ROLL_020;
  	}

	/*  etLnbVoltage  */
	switch( ptFDIParam->etLnbVoltage )
	{
		case FDI_CH_LNB_VOLT_STD :
			ptDIParam->etLnbVoltage = DI_CH_LNB_VOLT_STD;
			break;
		case FDI_CH_LNB_VOLT_HIGH :
			ptDIParam->etLnbVoltage = DI_CH_LNB_VOLT_HIGH;
			break;
		default :
			ptDIParam->etLnbVoltage = DI_CH_LNB_VOLT_STD;
  	}

	/*  etLnbVoltage  */
	switch( ptFDIParam->etDiseqcVersion )
	{
		case FDI_CH_DISEQC_VER_1_0 :
			ptDIParam->etDiseqcVersion = DI_CH_DISEQC_VER_1_0;
			break;
		case FDI_CH_DISEQC_VER_1_1 :
			ptDIParam->etDiseqcVersion = DI_CH_DISEQC_VER_1_1;
			break;
		case FDI_CH_DISEQC_VER_1_2 :
			ptDIParam->etDiseqcVersion = DI_CH_DISEQC_VER_1_2;
			break;
		case FDI_CH_DISEQC_VER_1_3 :
			ptDIParam->etDiseqcVersion = DI_CH_DISEQC_VER_1_3;
			break;
		case FDI_CH_DISEQC_VER_2_0 :
			ptDIParam->etDiseqcVersion = DI_CH_DISEQC_VER_2_0;
			break;
		default :
			ptDIParam->etDiseqcVersion = DI_CH_DISEQC_VER_1_0;
  	}

}


void INTRN_FDI_CHANNEL_S_ConvDItoFDI(CH_SAT_TuneParam_t *ptDIParam, FDI_CH_SAT_TUNE_PARAM_t *ptFDIParam)
{

	ptFDIParam->ulFrequency = ptDIParam->ulFrequency;				//unit KHz
	ptFDIParam->ulSymbolRate = ptDIParam->ulSymbolRate;
	ptFDIParam->b22kTone = ptDIParam->b22kTone;
	ptFDIParam->ulScdUserBandNum = ptDIParam->ulScdUserBandNum;
	ptFDIParam->ulScdUserBandFreq = ptDIParam->ulScdUserBandFreq;

	/*  etAntennaType  */
	switch( ptDIParam->etAntennaType )
	{
		case DI_CH_ANT_TYPE_LNB :
			ptFDIParam->etAntennaType = FDI_CH_ANT_TYPE_LNB;
			break;
		case DI_CH_ANT_TYPE_DISEQC :
			ptFDIParam->etAntennaType = FDI_CH_ANT_TYPE_DISEQC;
			break;
		case DI_CH_ANT_TYPE_SCD :
			ptFDIParam->etAntennaType = FDI_CH_ANT_TYPE_SCD;
			break;
		case DI_CH_ANT_TYPE_SMATV :
			ptFDIParam->etAntennaType = FDI_CH_ANT_TYPE_SMATV;
			break;
		default :
			ptFDIParam->etAntennaType = FDI_CH_ANT_TYPE_LNB;
	}

	/*  etFecCodeRate  */
	switch( ptDIParam->etFecCodeRate )
	{
		case DI_CH_SCODERATE_AUTO :
			ptFDIParam->etFecCodeRate = FDI_CH_SCODERATE_AUTO;
			break;
		case DI_CH_SCODERATE_1_2 :
			ptFDIParam->etFecCodeRate = FDI_CH_SCODERATE_1_2;
			break;
		case DI_CH_SCODERATE_2_3 :
			ptFDIParam->etFecCodeRate = FDI_CH_SCODERATE_2_3;
			break;
		case DI_CH_SCODERATE_3_4 :
			ptFDIParam->etFecCodeRate = FDI_CH_SCODERATE_3_4;
			break;
		case DI_CH_SCODERATE_5_6 :
			ptFDIParam->etFecCodeRate = FDI_CH_SCODERATE_5_6;
			break;
		case DI_CH_SCODERATE_7_8 :
			ptFDIParam->etFecCodeRate = FDI_CH_SCODERATE_7_8;
			break;
		case DI_CH_S2CODERATE_3_5 :
			ptFDIParam->etFecCodeRate = FDI_CH_S2CODERATE_3_5;
			break;
		case DI_CH_S2CODERATE_4_5 :
			ptFDIParam->etFecCodeRate = FDI_CH_S2CODERATE_4_5;
			break;
		case DI_CH_S2CODERATE_8_9 :
			ptFDIParam->etFecCodeRate = FDI_CH_S2CODERATE_8_9;
			break;
		case DI_CH_S2CODERATE_9_10 :
			ptFDIParam->etFecCodeRate = FDI_CH_S2CODERATE_9_10;
			break;
		case DI_CH_SCODERATE_5_11 :
			ptFDIParam->etFecCodeRate = FDI_CH_SCODERATE_5_11;
			break;
		case DI_CH_SCODERATE_6_7 :
			ptFDIParam->etFecCodeRate = FDI_CH_SCODERATE_6_7;
			break;
		default :
			ptFDIParam->etFecCodeRate = FDI_CH_SCODERATE_AUTO;
	}

	/*  etPolarization  */
	switch( ptDIParam->etPolarization )
	{
		case DI_CH_POLAR_AUTO :
			ptFDIParam->etPolarization = FDI_CH_POLAR_AUTO;
			break;
		case DI_CH_POLAR_HOR :
			ptFDIParam->etPolarization = FDI_CH_POLAR_HOR;
			break;
		case DI_CH_POLAR_VER :
			ptFDIParam->etPolarization = FDI_CH_POLAR_VER;
			break;
		case DI_CH_POLAR_LEFT :
			ptFDIParam->etPolarization = FDI_CH_POLAR_LEFT;
			break;
		case DI_CH_POLAR_RIGHT :
			ptFDIParam->etPolarization = FDI_CH_POLAR_RIGHT;
			break;
		default :
			ptFDIParam->etPolarization = FDI_CH_POLAR_AUTO;
	}

	/*  etTransSpec  */
	switch( ptDIParam->etTransSpec )
	{
		case DI_CH_TRANS_DVBS :
			ptFDIParam->etTransSpec = FDI_CH_TRANS_DVBS;
			break;
		case DI_CH_TRANS_DVBS2 :
			ptFDIParam->etTransSpec = FDI_CH_TRANS_DVBS2;
			break;
		default :
			ptFDIParam->etTransSpec = FDI_CH_TRANS_DVBS;
	}

	/*  etDiseqcInput  */
	switch( ptDIParam->etDiseqcInput )
	{
		case DI_CH_DISEQC_INPUT_NONE :
			ptFDIParam->etDiseqcInput = FDI_CH_DISEQC_INPUT_NONE;
			break;
		case DI_CH_DISEQC_INPUT_A :
			ptFDIParam->etDiseqcInput = FDI_CH_DISEQC_INPUT_A;
			break;
		case DI_CH_DISEQC_INPUT_B :
			ptFDIParam->etDiseqcInput = FDI_CH_DISEQC_INPUT_B;
			break;
		case DI_CH_DISEQC_INPUT_C :
			ptFDIParam->etDiseqcInput = FDI_CH_DISEQC_INPUT_C;
			break;
		case DI_CH_DISEQC_INPUT_D :
			ptFDIParam->etDiseqcInput = FDI_CH_DISEQC_INPUT_D;
			break;
		case DI_CH_DISEQC_INPUT_TB_A :
			ptFDIParam->etDiseqcInput = FDI_CH_DISEQC_INPUT_TB_A;
			break;
		case DI_CH_DISEQC_INPUT_TB_B :
			ptFDIParam->etDiseqcInput = FDI_CH_DISEQC_INPUT_TB_B;
			break;
		case DI_CH_DISEQC_INPUT_SCD_A :
			ptFDIParam->etDiseqcInput = FDI_CH_DISEQC_INPUT_SCD_A;
			break;
		case DI_CH_DISEQC_INPUT_SCD_B :
			ptFDIParam->etDiseqcInput = FDI_CH_DISEQC_INPUT_SCD_B;
			break;
		default :
			ptFDIParam->etDiseqcInput = FDI_CH_DISEQC_INPUT_NONE;
	}

	/*  etModulation  */
	switch( ptDIParam->etModulation )
	{
		case DI_CH_PSK_AUTO :
			ptFDIParam->etModulation = FDI_CH_PSK_AUTO;
			break;
		case DI_CH_PSK_QPSK :
			ptFDIParam->etModulation = FDI_CH_PSK_QPSK;
			break;
		case DI_CH_PSK_8PSK :
			ptFDIParam->etModulation = FDI_CH_PSK_8PSK;
			break;
		default :
			ptFDIParam->etModulation = FDI_CH_PSK_AUTO;
	}

	/*  etPilot  */
	switch( ptDIParam->etPilot )
	{
		case DI_CH_PILOT_AUTO :
			ptFDIParam->etPilot = FDI_CH_PILOT_AUTO;
			break;
 		case DI_CH_PILOT_OFF :
			ptFDIParam->etPilot = FDI_CH_PILOT_OFF;
			break;
		case DI_CH_PILOT_ON :
			ptFDIParam->etPilot = FDI_CH_PILOT_ON;
			break;
		default :
			ptFDIParam->etPilot = FDI_CH_PILOT_AUTO;
 	}

	/*  etRollOff  */
	switch( ptDIParam->etRollOff )
	{
		case DI_CH_ROLL_020 :
			ptFDIParam->etRollOff = FDI_CH_ROLL_020;
			break;
		case DI_CH_ROLL_025 :
			ptFDIParam->etRollOff = FDI_CH_ROLL_025;
			break;
		case DI_CH_ROLL_035 :
			ptFDIParam->etRollOff = FDI_CH_ROLL_035;
			break;
		default :
			ptFDIParam->etRollOff = FDI_CH_ROLL_020;
  	}

	/*  etLnbVoltage  */
	switch( ptDIParam->etLnbVoltage )
	{
		case DI_CH_LNB_VOLT_STD :
			ptFDIParam->etLnbVoltage = FDI_CH_LNB_VOLT_STD;
			break;
		case DI_CH_LNB_VOLT_HIGH :
			ptFDIParam->etLnbVoltage = FDI_CH_LNB_VOLT_HIGH;
			break;
		default :
			ptFDIParam->etLnbVoltage = FDI_CH_LNB_VOLT_STD;
  	}

	/*  etLnbVoltage  */
	switch( ptDIParam->etDiseqcVersion )
	{
		case DI_CH_DISEQC_VER_1_0 :
			ptFDIParam->etDiseqcVersion = FDI_CH_DISEQC_VER_1_0;
			break;
		case DI_CH_DISEQC_VER_1_1 :
			ptFDIParam->etDiseqcVersion = FDI_CH_DISEQC_VER_1_1;
			break;
		case DI_CH_DISEQC_VER_1_2 :
			ptFDIParam->etDiseqcVersion = FDI_CH_DISEQC_VER_1_2;
			break;
		case DI_CH_DISEQC_VER_1_3 :
			ptFDIParam->etDiseqcVersion = FDI_CH_DISEQC_VER_1_3;
			break;
		case DI_CH_DISEQC_VER_2_0 :
			ptFDIParam->etDiseqcVersion = FDI_CH_DISEQC_VER_2_0;
			break;
		default :
			ptFDIParam->etDiseqcVersion = FDI_CH_DISEQC_VER_1_0;
  	}

}


/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_S_Open(unsigned long ulDeviceId)
{
	DI_CHANNEL_ERR eDiErr;

	FDI_PRINT(FDI_PRT_DBGINFO, ("[FDI_CHANNEL_S_Open] : ++\n"));

	eDiErr = DI_CH_S_ActivateChannel((unsigned char)ulDeviceId);
	if(eDiErr != DI_CH_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CHANNEL_S_Open] FDI_CHANNEL_S_Open(%d) Error : %d\n", ulDeviceId, eDiErr));
		return FDI_ERR;
	}

	FDI_PRINT(FDI_PRT_DBGINFO, ("[FDI_CHANNEL_S_Open] : --\n"));

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_S_RequestLock(unsigned long ulDeviceId, FDI_CH_SAT_TUNE_PARAM_t *ptParam)
{
	DI_CHANNEL_ERR eDiErr;
	CH_SAT_TuneParam_t tDiParam;

	FDI_PRINT(0, ("[FDI_CHANNEL_S_RequestLock] : ++\n"));

	if( ptParam == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CHANNEL_S_RequestLock] Invalid Parameter. ptParams==NULL\n"));
		return FDI_ERR;
	}
	memset((void *)&tDiParam, 0, sizeof(CH_SAT_TuneParam_t));

	INTRN_FDI_CHANNEL_S_ConvFDItoDI(ptParam, &tDiParam);

	s_ulRequestId++;

#if (FACTORY_USE_CHANNEL_TER == YES)
	DI_CH_T_CancelLock(ulDeviceId);
	DI_CH_T_DeactivateChannel(ulDeviceId);
#endif
#if (FACTORY_USE_CHANNEL_CAB == YES)
	DI_CH_C_CancelLock(ulDeviceId);
	DI_CH_C_DeactivateChannel(ulDeviceId);
#endif

	DI_CH_S_DeactivateChannel(ulDeviceId);
#if defined(DI_DEMUX_EXTENSION)
	DI_DEMUX_BindChannel(ulDeviceId, DI_CH_TYPE_SAT);
#endif
	DI_CH_S_ActivateChannel(ulDeviceId);

	/* request lock  */
	eDiErr = DI_CH_S_RequestLock(ulDeviceId, s_ulRequestId, &tDiParam);
	if(eDiErr != DI_CH_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CHANNEL_S_RequestLock] DI_CH_S_RequestLock Error : %d\n", eDiErr));
		return FDI_ERR;
	}

	FDI_PRINT(0, ("[FDI_CHANNEL_S_RequestLock] : --\n"));

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_S_GetLockedInfo(unsigned long ulDeviceId,
											FDI_CH_SAT_TUNE_PARAM_t *ptParam)
{
	DI_CHANNEL_ERR eDiErr;
	CH_SAT_TuneParam_t tDiParam;

	if( ptParam == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CHANNEL_S_GetLockedInfo] Invalid Parameter. ptParams==NULL\n"));
		return FDI_ERR;
	}

	eDiErr = DI_CH_S_GetLockedInfo(ulDeviceId, &tDiParam);
	if(eDiErr != DI_CH_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CHANNEL_S_GetLockedInfo] DI_CH_S_GetLockedInfo Error : %d\n", eDiErr));
		return FDI_ERR;
	}

	INTRN_FDI_CHANNEL_S_ConvDItoFDI(&tDiParam, ptParam);

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_S_IsLocked(unsigned long ulDeviceId, BOOL *pbIsLocked)
{
	DI_CHANNEL_ERR eDiErr;
	CH_SAT_TuneParam_t tDiParam;

	if( pbIsLocked == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CHANNEL_S_IsLocked] Invalid Parameter. pbIsLocked==NULL\n"));
		return FDI_ERR;
	}

	eDiErr = DI_CH_S_GetLockedInfo(ulDeviceId, &tDiParam);
	if(eDiErr != DI_CH_OK)
	{
		*pbIsLocked = FALSE;
	}
	else
	{
		*pbIsLocked = TRUE;
	}

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_S_CancelLock(unsigned long ulDeviceId)
{
	DI_CHANNEL_ERR eDiErr;

	DI_CH_S_CancelLock(ulDeviceId);

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_S_GetSignalInfo(unsigned long ulDeviceId, FDI_CH_SAT_SIGNAL_QUALITY_t *ptSignalInfo)
{
	DI_CHANNEL_ERR eDiErr;
	DI_CH_SignalQuality_t tDiParam;
	HUINT32 ulRfAge=0, ulIfAgc=0, ulUncorrected=0;
	float fPower=0;
	HINT32 nBer = 0;

	if( ptSignalInfo == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CHANNEL_S_GetSignalInfo] Invalid Parameter. ptSignalInfo==NULL\n"));
		return FDI_ERR;
	}

//	DI_CHANNEL_ERR DI_CH_S_GetChannelStatus (HUINT32 ulInstanceId, HINT32 *pulBer, HUINT32 *pulRfAgc, HUINT32 *pulIfAgc, HUINT32 *pulUncorrect, float *pfPower);
	eDiErr = DI_CH_S_GetChannelStatus((HUINT32)ulDeviceId, &nBer, &ulRfAge, &ulIfAgc, &ulUncorrected, &fPower);
	if(eDiErr != DI_CH_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CHANNEL_S_GetSignalInfo] DI_CH_S_GetSignalInfo Error : %d\n", eDiErr));
		return FDI_ERR;
	}

	FDI_PRINT(0, ("[FDI_CHANNEL_S_GetSignalInfo] AGC:%f\n", fPower));
	FDI_PRINT(0, ("[FDI_CHANNEL_S_GetSignalInfo] BER:%d\n", nBer));

	ptSignalInfo->fAgc = fPower;
	ptSignalInfo->nBer = nBer;
	ptSignalInfo->fSnr = 0;
	ptSignalInfo->ulQuality = 0;
	ptSignalInfo->ulStrength = 0;

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_S_SetLnb22KhzTone(unsigned long ulDeviceId, unsigned char uc22KhzTone)
{
	DI_CHANNEL_ERR eDiErr;

	eDiErr = DI_CH_S_SetLnb22KhzTone(ulDeviceId, uc22KhzTone);
	if(eDiErr != DI_CH_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CHANNEL_S_SetLnb22KhzTone] DI_CH_S_SetLnb22KhzTone Error : %d\n", eDiErr));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_S_SetLnbPolarisation(unsigned long ulDeviceId,
												FDI_CH_POLARIZATION_e ePolarization,
												FDI_CH_LNB_VOLTAGE_e eVoltage)
{
	DI_CHANNEL_ERR eDiErr;

	eDiErr = DI_CH_S_SetLnbPolarisation(ulDeviceId, ePolarization, eVoltage);
	if(eDiErr != DI_CH_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CHANNEL_S_SetLnb22KhzTone] DI_CH_S_SetLnb22KhzTone Error : %d\n", eDiErr));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_S_SetLnbOutput(unsigned long ulDeviceId, BOOL bOnOff)
{
	DI_CHANNEL_ERR eDiErr;

	eDiErr = DI_CH_S_SetLnbOutput(ulDeviceId, bOnOff);
	if(eDiErr != DI_CH_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CHANNEL_S_SetLnbOutput] DI_CH_S_SetLnbOutput Error : %d\n", eDiErr));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_S_RegisterCallback(unsigned long ulDeviceId, pfnFDI_CHANNEL_NotifySignalInfo pfnNotify)
{
	DI_CHANNEL_ERR eDiErr;
	pfnDI_CH_NotifySignalInfo pfnDiNotify;

	pfnDiNotify = (pfnDI_CH_NotifySignalInfo)pfnNotify;
	eDiErr = DI_CH_S_RegisterCallback(ulDeviceId, pfnDiNotify);
	if(eDiErr != DI_CH_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CHANNEL_S_RegisterCallback] DI_CH_S_RegisterCallback Error : %d\n", eDiErr));
		return FDI_ERR;
	}
	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_S_SetLoopthrouth(BOOL bOnOff)
{
	DI_CHANNEL_ERR eDiErr;

	if( bOnOff == TRUE )
	{
		eDiErr = DI_CH_S_SetLoopthGpio(FALSE);
	}
	else
	{
		eDiErr = DI_CH_S_SetLoopthGpio(TRUE);
	}
	if(eDiErr != DI_CH_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CHANNEL_SetLoopthrouth] DI_CH_S_SetLoopthGpio Error : %d\n", eDiErr));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

FDI_ERR_CODE FDI_CHANNEL_S_SetInput(int nUnitDi, int nInputSource)
{
	FDI_ERR_CODE etResult = FDI_ERR;
	DI_CHANNEL_ERR etChannelResult = DI_CH_OK;

	etChannelResult = DI_CH_S_SetInputSource(nUnitDi, nInputSource);
	if(etChannelResult == DI_CH_OK)
	{
		etResult = FDI_NO_ERR;
		FDI_PRINT(FDI_PRT_DBGINFO, ("OK! SAT Input mode is changed \n"));
	}
	else
	{
		etResult = FDI_ERR;
		FDI_PRINT(FDI_PRT_ERROR, ("Fail! SAT Input mode is not changed \n"));
	}
	return etResult;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_S_SendDiseqcMsg( unsigned long ulDeviceId, unsigned char *pucData, unsigned char ucLength)
{
	DI_CHANNEL_ERR eDiErr;

	eDiErr = DI_CH_S_SendDiseqcMsg(ulDeviceId, pucData, ucLength);
	if(eDiErr != DI_CH_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CHANNEL_S_SendDiseqcMsg] DI_CH_S_SendDiseqcMsg Error : %d\n", eDiErr));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_S_ReceiveDiseqcMsg( unsigned long ulDeviceId, unsigned char *pucData, unsigned char ucLength, unsigned char *pucReplyLength)
{
	DI_CHANNEL_ERR eDiErr;

	eDiErr = DI_CH_S_ReceiveDiseqcMsg(ulDeviceId, pucData, ucLength, pucReplyLength);
	if(eDiErr != DI_CH_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CHANNEL_S_ReceiveDiseqcMsg] DI_CH_S_ReceiveDiseqcMsg Error : %d\n", eDiErr));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

