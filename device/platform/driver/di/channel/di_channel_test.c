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
#include "di_channel_a.h"
#include "di_channel_c.h"
#include "di_channel_s.h"
#include "di_channel_t.h"

#include "di_channel_priv.h"
#include "di_channel_attr.h"
#include "di_channel_util.h"
//#include "di_channel_i2c.h"

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
typedef struct TUNER_PARAM_tag {
	unsigned long	dwFreq;
	unsigned char	ucFFTMode;
	unsigned char	ucGuardInterval;
	unsigned char	ucConvolutionCode;
	unsigned char	ucQAMmode;
	unsigned char	ucHierarchy;
	unsigned char	ucOffset;
	unsigned char	ucStream;
	unsigned char	ucBandwidth;
	unsigned char	ucSearchmode;
	unsigned long ulRequestId;
} DRV_TUNER_PARAM_TER;

// End typedef


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
// Start global variablee

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

void TerCallback ( HUINT32 ulInstanceId, HUINT32 unRequestID, DI_CH_Type_e eChannelType , DI_CH_SignalInfo_e etSignalInfo )
{
	CH_UNUSED(ulInstanceId);CH_UNUSED(unRequestID);CH_UNUSED(eChannelType);
	int nRet = 0;
	CH_TER_TuneParam_t stTerLockedP;
//	DI_CH_SignalQuality_t stSignalInfo;

	switch ( etSignalInfo )
	{
		case DI_CH_SIGNAL_LOCKED:
			CH_PrintFunc( "\t\t\t >>>TER_SIGNAL_LOCKED<<<\n" );
			//nRet = DI_CH_T_GetLockedInfo( 1, &stTerLockedP );
			if ( nRet != DI_CH_OK )
			{
				CH_PrintErr(" ");
			}
#if 0
			nRet = DI_CH_T_GetSignalInfo( 1, &stSignalInfo);
			if ( nRet != DI_CH_OK )
			{
				CH_PrintErr(" ");
			}
#endif
			PrintTerTuneParam( &stTerLockedP );
			break;

		case DI_CH_SIGNAL_UNLOCK:
			CH_PrintFunc( "\t\t\t >>>TER_SIGNAL_UNLOCK<<<\n" );
			break;

		case DI_CH_SIGNAL_ANTENNA_FAULT:
			CH_PrintDebug( "DI_CH_SIGNAL_ANTENNA_FAULT\n" );
			break;

		default:
			CH_PrintErr("etSignalInfo(%d)", etSignalInfo);
			break;
	}

}


void SatCallback ( HUINT32 ulInstanceId, HUINT32 unRequestID, DI_CH_Type_e eChannelType , DI_CH_SignalInfo_e etSignalInfo )
{
	int nRet = 0;
	CH_UNUSED(unRequestID);CH_UNUSED(eChannelType);
	CH_SAT_TuneParam_t stSatLockedP;
	//DI_CH_SignalQuality_t stSignalInfo;

	CH_PrintDebug( "ulInstanceId[%d],unRequestID[%d],eChannelType[%d]\n", ulInstanceId, unRequestID, eChannelType );

	switch ( etSignalInfo )
	{
		case DI_CH_SIGNAL_LOCKED:
			CH_PrintFunc( "\t\t\t >>>SAT_SIGNAL_LOCKED<<<\n" );
			nRet = DI_CH_S_GetLockedInfo( ulInstanceId, &stSatLockedP );
			if ( nRet != DI_CH_OK )
			{
				CH_PrintErr(" ");
			}
			else
				PrintSatTuneParam( &stSatLockedP );
#if 0
			nRet = DI_CH_S_GetSignalInfo(ulInstanceId, &stSignalInfo);
			if ( nRet != DI_CH_OK )
			{
				CH_PrintErr(" ");
			}
#endif
			break;

		case DI_CH_SIGNAL_UNLOCK:
			CH_PrintFunc( "\t\t\t >>>SAT_SIGNAL_UNLOCK<<<\n" );
			break;

		case DI_CH_SIGNAL_ANTENNA_FAULT:
            if ( CMD_GetMsgUI() != NULL )
            {
                (int *)(CMD_GetMsgUI())("LNB FAULT\n");
            }
			CMD_Printf( "\t\t\t >>>DI_CH_SIGNAL_ANTENNA_FAULT<<<\n" );
			break;

		case DI_CH_SIGNAL_ANTENNA_OK:
            if ( CMD_GetMsgUI() != NULL )
            {
                (int *)(CMD_GetMsgUI())("LNB OK\n");
            }
			CMD_Printf( "\t\t\t >>>DI_CH_SIGNAL_ANTENNA_OK<<<\n" );
			break;

		default:
			CH_PrintErr("etSignalInfo(%d)", etSignalInfo);
			break;
	}

}


void AnaCallback ( int nChannelID, HUINT32 unRequestID, DI_CH_Type_e eChannelType , DI_CH_SignalInfo_e etSignalInfo )
{
	int nRet = 0;
	//CH_ANA_TuneParam_t stAnaLockedP;
//	DI_CH_SignalQuality_t stSignalInfo;
	CH_UNUSED(nChannelID);CH_UNUSED(unRequestID);CH_UNUSED(eChannelType);
	CH_PrintDebug( "ChanneId[%d],unRequestID[%d],eChannelType[%d]\n", nChannelID, unRequestID, eChannelType );

	switch ( etSignalInfo )
	{
		case DI_CH_SIGNAL_LOCKED:
			CH_PrintDebug( "DI_CH_SIGNAL_LOCKED\n" );
			//nRet = DI_CH_A_GetLockedInfo( 2, &stAnaLockedP );
			if ( nRet != DI_CH_OK )
			{
				CH_PrintErr(" ");
			}
#if 0
			nRet = DI_CH_T_GetSignalInfo( 2, &stSignalInfo);
			if ( nRet != DI_CH_OK )
			{
				CH_PrintErr(" ");
			}
			PrintAnaTuneParam( &stAnaLockedP );

#endif
			break;

		case DI_CH_SIGNAL_UNLOCK:
			CH_PrintDebug( "DI_CH_SIGNAL_UNLOCK\n" );
			break;

		case DI_CH_SIGNAL_ANTENNA_FAULT:
			CH_PrintDebug( "DI_CH_SIGNAL_ANTENNA_FAULT\n" );
			break;

		default:
			CH_PrintErr("etSignalInfo(%d)", etSignalInfo);
			break;
	}

}

static unsigned long	ulRequestId = -1;

static void TuneSat(unsigned long  ulInstanceId)
{
	CH_SAT_TuneParam_t stSatTuneP;
	//static HUINT32	unSyncId = 0;

	stSatTuneP.etAntennaType = DI_CH_ANT_TYPE_DISEQC;
#if 1 // SD
	stSatTuneP.ulFrequency = 1150; //000; //unit KHz
	stSatTuneP.ulSymbolRate = 28125;
	stSatTuneP.etFecCodeRate = DI_CH_SCODERATE_3_4;
	stSatTuneP.etPolarization = DI_CH_POLAR_HOR;
	stSatTuneP.etTransSpec = DI_CH_TRANS_DVBS;
	stSatTuneP.etDiseqcInput = DI_CH_DISEQC_INPUT_B;
	stSatTuneP.b22kTone = TRUE;

	stSatTuneP.etModulation = DI_CH_PSK_QPSK;
	stSatTuneP.etPilot = 0;
	stSatTuneP.etRollOff = 0;
	stSatTuneP.etLnbVoltage = DI_CH_LNB_VOLT_STD;
	stSatTuneP.etDiseqcVersion = DI_CH_DISEQC_VER_1_0;
	stSatTuneP.ulScdUserBandNum = 0;
	stSatTuneP.ulScdUserBandFreq = 0;

#else // HD
	stSatTuneP.ulFrequency = 1100; //000; //unit KHz
	stSatTuneP.ulSymbolRate = 27403;
	stSatTuneP.etFecCodeRate = DI_CH_S2CODERATE_9_10;
	stSatTuneP.etPolarization = DI_CH_POLAR_HOR;
	stSatTuneP.etTransSpec = DI_CH_TRANS_DVBS2;
	stSatTuneP.etDiseqcInput = DI_CH_DISEQC_INPUT_D;
	stSatTuneP.b22kTone = TRUE;

	stSatTuneP.etModulation = DI_CH_PSK_QPSK;
	stSatTuneP.etPilot = DI_CH_PILOT_OFF;
	stSatTuneP.etRollOff = DI_CH_ROLL_035;
	stSatTuneP.etLnbVoltage = DI_CH_LNB_VOLT_STD;
	stSatTuneP.etDiseqcVersion = DI_CH_DISEQC_VER_1_0;
	stSatTuneP.ulScdUserBandNum = 0;
	stSatTuneP.ulScdUserBandFreq = 0;
#endif

	ulRequestId++;

	DI_CH_S_RequestLock( ulInstanceId, ulRequestId, &stSatTuneP);

}


#if 0

static void TuneTer(void)
{
	CH_TER_TuneParam_t stTerTuneP;
	static HUINT32	unSyncId = 0;

	stTerTuneP.ulFrequency = 594000;//506000;
	//stTerTuneP.ulFrequency = 506000;
	stTerTuneP.etBandWidth = DI_CH_BW_8MHZ;
	stTerTuneP.etModulation = DI_CH_TMOD_AUTO;
	stTerTuneP.etHierarchy=  DI_CH_HIERARCHY_AUTO;
	stTerTuneP.etCodeRate = DI_CH_TCODERATE_AUTO;
	stTerTuneP.etTransMode = DI_CH_TRANSMODE_AUTO;
	stTerTuneP.etGuardInterval = DI_CH_GUARDINTERVAL_AUTO;
	stTerTuneP.etStream = DI_CH_STREAM_HP;
	stTerTuneP.etOffset = DI_CH_OFFSET_AUTO;

	DI_CH_T_RequestLock( 1, unSyncId++, &stTerTuneP);

}


static void TuneDrv(void)
{

DRV_TUNER_PARAM_TER 	stChannelParam;

	//stChannelParam.dwFreq				= 594000000;//594000;
	stChannelParam.dwFreq				= 506000000;//594000;
	stChannelParam.ucFFTMode			= DI_CH_TRANSMODE_AUTO;
	stChannelParam.ucGuardInterval		= DI_CH_GUARDINTERVAL_AUTO;
	stChannelParam.ucConvolutionCode	= DI_CH_TCODERATE_AUTO;
	stChannelParam.ucQAMmode			= DI_CH_TMOD_AUTO;
	stChannelParam.ucHierarchy			= DI_CH_HIERARCHY_AUTO;
	stChannelParam.ucOffset				= DI_CH_OFFSET_AUTO;
	stChannelParam.ucStream				= DI_CH_STREAM_HP;
	stChannelParam.ucBandwidth			= DI_CH_BW_8MHZ;
	stChannelParam.ucSearchmode			= 0;
	stChannelParam.ulRequestId			= 0;

#if 0
stChannelParam.ter.nChannelType    = DDI_CHANNEL_TYPE_TER;
stChannelParam.ter.nFreq		   = 594000;
stChannelParam.ter.ucBandWidth	   = DI_CH_BW_8MHZ;
stChannelParam.ter.ucConstellation = DI_CH_TMOD_AUTO;//DI_CH_TMOD_64QAM;
stChannelParam.ter.ucHierarchy	   = DI_CH_HIERARCHY_AUTO;//DI_CH_HIERARCHY_NONE;
stChannelParam.ter.ucCodeRate	   = DI_CH_TCODERATE_AUTO;//DDI_CHANNEL_CODERATE_2_3;
stChannelParam.ter.ucTransMode	   = DI_CH_TRANSMODE_AUTO;//DI_CH_TRANSMODE_8K;
stChannelParam.ter.ucGuardInterval = DI_CH_GUARDINTERVAL_AUTO;//DI_CH_GUARDINTERVAL_1_8;
stChannelParam.ter.ucStream 	   = DI_CH_STREAM_HP;
stChannelParam.ter.ucOffset 	   = DI_CH_OFFSET_AUTO;//DI_CH_OFFSET_NONE;
#endif
//Drv_Tuner_Set(0, stChannelParam);

}
#endif
void DI_CH_TestModule (void)
{
	int nRet = 0, i;
	HUINT32 ulNumOfInstance = 0;
	HUINT32 ulChannelType = 0;
//	int nTunerId = 0;

	CH_PrintDebug( "\n ################### [di_channel_test.c] DI_CH_TestModule #####################\n");

	nRet = DI_CH_GetNumOfInstance(&ulNumOfInstance);
	if ( nRet != DI_CH_OK )
	{
		CH_PrintErr(" ");
		return;
	}

	for ( i=0; i<(int)ulNumOfInstance; i++ )
	{
		nRet = DI_CH_GetCapability( i, &ulChannelType);
		if ( nRet != DI_CH_OK )
		{
			CH_PrintErr(" ");
			return;
		}
		else
		{
			CH_PrintDebug( "InstanceId[%d], Type[0x%x]\n", i,  ulChannelType );
		}

		DI_CH_S_RegisterCallback( i, SatCallback);

		/* SAT test */
		DI_CH_S_ActivateChannel(i);
		TuneSat(i);
		DI_CH_S_DeactivateChannel(i);

		VK_TASK_Sleep(10000);

	}

	//DI_CH_S_RegisterCallback( 0, SatCallback);
	//DI_CH_T_RegisterCallback( 1, TerCallback);
	//DI_CH_A_RegisterCallback( 1, AnaCallback);

#if 0
	/* SAT test : channelId = 0 */
	DI_CH_S_ActivateChannel(0);
	//TestTuneSat();
	TuneSat();
	//DI_CH_CancelLock(0);
	DI_CH_S_DeactivateChannel(0);

	VK_TASK_Sleep(10000);

	/* TER test : channelId = 1 */
	DI_CH_T_ActivateChannel(1);
	TuneTer();
	DI_CH_T_DeactivateChannel(1);
#endif

}



/*********************** End of File ******************************/
