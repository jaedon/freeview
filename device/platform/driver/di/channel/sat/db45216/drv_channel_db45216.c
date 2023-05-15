/* $Header: */
/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name:		drv_channel_kraken.c
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
// Start Including Headers

/* chip headers */
#include "string.h"
#include "htype.h"
#include "vkernel.h"

/* di headers */
#include "di_channel.h"
#include "di_channel_priv.h"
#include "di_channel_attr.h"

/* drv headers */
#include "drv_channel.h"
#include "drv_channel_lnb.h"

#include "drv_channel_db45216.h"

/* nexus headers */
#include "nexus_frontend.h"
#include "nexus_platform.h"
#include "nexus_frontend_45216.h"

// End Including Headers


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
// Start #define
#define NUM_OF_SAT_CHANNEL	(CONFIG_NUM_OF_SAT_CHANNEL)
#define MASTER_CHANNEL	(0)

#define MIN_WAIT_DELAY			(120)//(30)  /*tune command approximately 430 msec after tuning (116 msec after commanding acquire)*/
#define RS_STUFF_BYTE			(16) // Normal
#define ODU_SETUP_TIME_DISEQC	(200) /* TODO : Must be optimized... Outdoor Unit setup time 200ms for DISEQC */
#define ODU_SETUP_TIME_LNB		(100) /* Outdoor Unit setup time 100ms for LNB */
#define HAB_WAIT_TIME			(150)


#define MASTER_UNIT				(0)

#define CH_PRINT_LOW_LEVEL_ZAPPING_TIME


//#define DEBUG_SSI_SQI
// End #define

/*******************************************************************/
/****************************** Debug ****************************/
/*******************************************************************/


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
// Start global variablee
HINT32	g_nPrintBER;

extern HINT32 show_di_ch;

// End global variable


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
// Start static variablee

static NEXUS_FrontendHandle s_stFrontend[NUM_OF_SAT_CHANNEL];
static BKNI_EventHandle lockChangedEvent[NUM_OF_SAT_CHANNEL]= {NULL,};

#if (NEXUS_VERSION >= 1502)
static const NEXUS_FrontendSatelliteCodeRate
	g_cr_scan = {0,0,NEXUS_FrontendSatelliteCodeRateMode_eStandard,0},
	g_cr_1_4 = {1,4,NEXUS_FrontendSatelliteCodeRateMode_eStandard,0},
	g_cr_1_2 = {1,2,NEXUS_FrontendSatelliteCodeRateMode_eStandard,0},
	g_cr_2_3 = {2,3,NEXUS_FrontendSatelliteCodeRateMode_eStandard,0},
	g_cr_3_4 = {3,4,NEXUS_FrontendSatelliteCodeRateMode_eStandard,0},
	g_cr_5_6 = {5,6,NEXUS_FrontendSatelliteCodeRateMode_eStandard,0},
	g_cr_6_7 = {6,7,NEXUS_FrontendSatelliteCodeRateMode_eStandard,0},
	g_cr_7_8 = {7,8,NEXUS_FrontendSatelliteCodeRateMode_eStandard,0},
	g_cr_5_11 = {5,11,NEXUS_FrontendSatelliteCodeRateMode_eStandard,0},
	g_cr_3_5 = {3,5,NEXUS_FrontendSatelliteCodeRateMode_eStandard,0},
	g_cr_4_5 = {4,5,NEXUS_FrontendSatelliteCodeRateMode_eStandard,0},
	g_cr_9_10 = {9,10,NEXUS_FrontendSatelliteCodeRateMode_eStandard,0},
	g_cr_8_9 = {8,9,NEXUS_FrontendSatelliteCodeRateMode_eStandard,0};
#else
static const NEXUS_FrontendSatelliteCodeRate
	g_cr_scan = {0,0,0},
	g_cr_1_4 = {1,4,0},
	g_cr_1_2 = {1,2,0},
	g_cr_2_3 = {2,3,0},
	g_cr_3_4 = {3,4,0},
	g_cr_5_6 = {5,6,0},
	g_cr_6_7 = {6,7,0},
	g_cr_7_8 = {7,8,0},
	g_cr_5_11 = {5,11,0},
	g_cr_3_5 = {3,5,0},
	g_cr_4_5 = {4,5,0},
	g_cr_9_10 = {9,10,0},
	g_cr_8_9 = {8,9,0};
#endif
static CH_SAT_LnbStatus_t	s_stLnbState[NUM_OF_SAT_CHANNEL];

//static HUINT8 ucStuffByte = RS_STUFF_BYTE;

#ifdef CH_PRINT_LOW_LEVEL_ZAPPING_TIME
HUINT32 g_ulRequestTune[NUM_OF_SAT_CHANNEL];
HUINT32 g_ulLowLevelLockedTime[NUM_OF_SAT_CHANNEL] , g_ulLowLevelUnlockedTime[NUM_OF_SAT_CHANNEL] ;
#endif


/****************** Function 시작점에 필히 기록 ********************/
/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
// Function Name:
// Function Description:
// Parameter:
// Return Value:

#define __DB45216_PRIVATE_FUNCITONS__

/*********************** Static Function ***********************/



static HUINT32 GetFreqSearchRange( HUINT32 ulSymbolRate, CH_SAT_TransportSpec_e	etTransSpec )
{
	HUINT32 ulFreqSearchrange = 0;
	/*************************************
	        Normal Search range                                     Support CI+ Certi
		[Symbolrate]          [Search range]
		45 ~ 20Ms/s =	+/- (10MHz)                            45 ~ 22Ms/s    =	+/- (8MHz)
		20 ~ 18Ms/s =	+/- (8MHz)                              22 ~ 18Ms/s    =	+/- (6MHz)
		18 ~ 11Ms/s =	+/- (6MHz)                              18 ~ 11Ms/s    =	+/- (6MHz)
		11 ~ 8Ms/s	=	+/- (5MHz)                      11 ~ 8Ms/s	=	+/- (5MHz)
		8 ~ 4Ms/s	=	+/- (4MHz)                       8 ~ 4Ms/s	=	+/- (4MHz)
		4 ~ 1Ms/s	=	+/- (3.8MHz)                    4 ~ 1Ms/s	=	+/- (3.8MHz)
	**************************************/
	/********************************************************
	Search range freq + 0.12 * symbolrate ( DVB-S )
				      + 0.2 * symbolrate ( DVB-S2 )
	65n & 40n chip : Max Search range : 10Mhz
	28n : Max Search range : 16Mhz
	********************************************************/
	if( etTransSpec == DI_CH_TRANS_DVBS )
	{
		if(( ulSymbolRate >= 1000) && (ulSymbolRate <= 4000))
		{
			ulFreqSearchrange = 3800000-(ulSymbolRate*120);
		}
		else if(( ulSymbolRate > 4000) && (ulSymbolRate <= 8000))
		{
			ulFreqSearchrange = 4000000-(ulSymbolRate*120);
		}
		else if(( ulSymbolRate > 8000) && (ulSymbolRate <= 11000))
		{
			ulFreqSearchrange = 5000000-(ulSymbolRate*120);
		}
		else if(( ulSymbolRate > 11000) && (ulSymbolRate <= 18000))
		{
			ulFreqSearchrange = 6000000-(ulSymbolRate*120);
		}
#if defined( CONFIG_SUPPORT_CI_PLUS_CERTI )
		else if(( ulSymbolRate > 18000) && (ulSymbolRate <= 22000))
		{
			ulFreqSearchrange = 6000000;
		}
		else if(( ulSymbolRate > 22000) && (ulSymbolRate <= 45000))
		{
			ulFreqSearchrange = 8000000;
		}
		else
		{
			ulFreqSearchrange = 8000000;
		}
#else
		else if(( ulSymbolRate > 18000) && (ulSymbolRate <= 20000))
		{
			ulFreqSearchrange = 8000000-(ulSymbolRate*120);
		}
		else if(( ulSymbolRate > 20000) && (ulSymbolRate <= 45000))
		{
			ulFreqSearchrange = 10000000-(ulSymbolRate*120);
		}
		else
		{
			ulFreqSearchrange = 10000000-(ulSymbolRate*120);
		}
#endif
	}
	else
	{
		if(( ulSymbolRate >= 1000) && (ulSymbolRate <= 4000))
		{
			ulFreqSearchrange = 3800000-(ulSymbolRate*200);
		}
		else if(( ulSymbolRate > 4000) && (ulSymbolRate <= 8000))
		{
			ulFreqSearchrange = 4000000-(ulSymbolRate*200);
		}
		else if(( ulSymbolRate > 8000) && (ulSymbolRate <= 11000))
		{
			ulFreqSearchrange = 5000000-(ulSymbolRate*200);
		}
		else if(( ulSymbolRate > 11000) && (ulSymbolRate <= 18000))
		{
			ulFreqSearchrange = 6000000-(ulSymbolRate*200);
		}
#if defined( CONFIG_SUPPORT_CI_PLUS_CERTI )
		else if(( ulSymbolRate > 18000) && (ulSymbolRate <= 22000))
		{
			ulFreqSearchrange = 6000000;
		}
		else if(( ulSymbolRate > 22000) && (ulSymbolRate <= 45000))
		{
			ulFreqSearchrange = 8000000;
		}
		else
		{
			ulFreqSearchrange = 8000000;
		}
#else
		else if(( ulSymbolRate > 18000) && (ulSymbolRate <= 20000))
		{
			ulFreqSearchrange = 8000000-(ulSymbolRate*200);
		}
		else if(( ulSymbolRate > 20000) && (ulSymbolRate <= 32000))
		{
			ulFreqSearchrange = 10000000-(ulSymbolRate*200);
		}
		else if(( ulSymbolRate > 32000) && (ulSymbolRate <= 45000))
		{
			ulFreqSearchrange = 5000000;
		}
		else
		{
			ulFreqSearchrange = 10000000-(ulSymbolRate*200);
		}
#endif
	}
	
	return ulFreqSearchrange;
}


static NEXUS_FrontendSatelliteCodeRate GetNexusCodeRate(
	CH_SAT_CodeRate_e		etFecCodeRate
)
{
	NEXUS_FrontendSatelliteCodeRate etCodeRate = g_cr_scan;

	switch(etFecCodeRate)
	{
		case DI_CH_SCODERATE_AUTO:
			etCodeRate = g_cr_scan;
			break;

		case DI_CH_SCODERATE_1_2:
			etCodeRate = g_cr_1_2;
			break;

		case DI_CH_SCODERATE_2_3:
			etCodeRate = g_cr_2_3;
			break;

		case DI_CH_SCODERATE_3_4:
			etCodeRate = g_cr_3_4;
			break;

		case DI_CH_SCODERATE_5_6:
			etCodeRate = g_cr_5_6;
			break;

		case DI_CH_SCODERATE_7_8:
			etCodeRate = g_cr_7_8;
			break;

		case DI_CH_S2CODERATE_3_5: /** DVB-S2:QPSK DVB-S2:8PSK */
			etCodeRate = g_cr_3_5;
			break;

		case DI_CH_S2CODERATE_4_5:  /** DVB-S2:QPSK */
			etCodeRate = g_cr_4_5;
			break;

		case DI_CH_SCODERATE_5_11:
			etCodeRate = g_cr_5_11;
			break;

		case DI_CH_SCODERATE_6_7:
			etCodeRate = g_cr_6_7;
			break;

		case DI_CH_S2CODERATE_8_9: /** DVB-S2:QPSK 	DVB-S2:8PSK */
			etCodeRate = g_cr_8_9;
			break;

		case DI_CH_S2CODERATE_9_10:  /** DVB-S2:QPSK 	DVB-S2:8PSK */
			etCodeRate = g_cr_9_10;
			break;

		default:
			CH_DI_Print(CH_LOG_DEFAULT, ("[GetNexusCodeRate] default: etCodeRate(%d)!\n", etFecCodeRate));
			break;
	}

	CH_DI_Print(CH_LOG_ARGS, ("[GetNexusCodeRate] etFecCodeRate (%d) ->  etCodeRate (%d/%d) \n", etFecCodeRate, etCodeRate.numerator, etCodeRate.denominator));

	return etCodeRate;
}


static NEXUS_FrontendSatelliteMode GetNexusMode(
				CH_SAT_TransportSpec_e	etTransSpec,
				CH_SAT_Modulation_e 	etModulation
)
{
	NEXUS_FrontendSatelliteMode etSatMode = NEXUS_FrontendSatelliteMode_eMax;

	if( etTransSpec == DI_CH_TRANS_DVBS )
	{
		if( etModulation == DI_CH_PSK_QPSK )
		{
			etSatMode = NEXUS_FrontendSatelliteMode_eDvb;
			CH_DI_Print(CH_LOG_ARGS, (" NEXUS_FrontendSatelliteMode_eDvb'.. (DVB-S)\n"));
		}
		else
		{
			etSatMode = NEXUS_FrontendSatelliteMode_eDvb;//NEXUS_FrontendSatelliteMode_eDvbScan;
			CH_DI_Print(CH_LOG_ARGS, (" NEXUS_FrontendSatelliteMode_eDvbScan'.. (DVB-S, AUTO)\n"));
		}
	}
	else if( etTransSpec == DI_CH_TRANS_DVBS2 )
	{
		if( etModulation == DI_CH_PSK_QPSK )
		{
			etSatMode = NEXUS_FrontendSatelliteMode_eQpskLdpc;
			CH_DI_Print(CH_LOG_ARGS, (" NEXUS_FrontendSatelliteMode_eQpskLdpc'.. (DVB-S2, QPSK)\n"));
		}
		else if(etModulation == DI_CH_PSK_8PSK )
		{
			etSatMode = NEXUS_FrontendSatelliteMode_e8pskLdpc;
			CH_DI_Print(CH_LOG_ARGS, (" NEXUS_FrontendSatelliteMode_e8pskLdpc'.. (DVB-S2, 8PSK)\n"));
		}
		else
		{
			etSatMode = NEXUS_FrontendSatelliteMode_eBlindAcquisition;//NEXUS_FrontendSatelliteMode_eLdpcScan;
			CH_DI_Print(CH_LOG_ARGS, (" NEXUS_FrontendSatelliteMode_eLdpcScan'.. (DVB-S2, AUTO)\n"));
		}
	}
	else
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[GetNexusMode] ERR! etTransSpec (%d)!\n", etTransSpec));
	}

	return etSatMode;
}

static void ConvertNexusToDiParam( NEXUS_FrontendSatelliteMode etSatMode, NEXUS_FrontendSatelliteCodeRate etCodeRate, CH_SAT_TuneParam_t * pstTuneParam )
{
	if(etSatMode == NEXUS_FrontendSatelliteMode_eDvb)
	{
		pstTuneParam->etTransSpec = DI_CH_TRANS_DVBS;
		pstTuneParam->etModulation	= DI_CH_PSK_QPSK;
		CH_DI_Print(CH_LOG_ARGS, ("[DB45216_NexusUpdateInfo] %x : DVBS, QPSK \n", etSatMode));
	}
	else if(etSatMode == NEXUS_FrontendSatelliteMode_eQpskLdpc)
	{
		pstTuneParam->etTransSpec = DI_CH_TRANS_DVBS2;
		pstTuneParam->etModulation	= DI_CH_PSK_QPSK;
		CH_DI_Print(CH_LOG_ARGS, ("[DB45216_NexusUpdateInfo] %x : DVBS2, QPSK \n", etSatMode));
	}
	else if(etSatMode == NEXUS_FrontendSatelliteMode_e8pskLdpc)
	{
		pstTuneParam->etTransSpec = DI_CH_TRANS_DVBS2;
		pstTuneParam->etModulation	= DI_CH_PSK_8PSK;
		CH_DI_Print(CH_LOG_ARGS, ("[DB45216_NexusUpdateInfo] %x : DVBS2, 8PSK \n", etSatMode));
	}
	else
	{
		pstTuneParam->etModulation	= DI_CH_PSK_AUTO;
	}

	CH_DI_Print(CH_LOG_ARGS, ("[DB45216_NexusUpdateInfo] etTransSpec [%d] , etModulation [%d]  \n", pstTuneParam->etTransSpec, pstTuneParam->etModulation ));

	if(etCodeRate.numerator == 1 && etCodeRate.denominator == 2)
		pstTuneParam->etFecCodeRate = DI_CH_SCODERATE_1_2;
	else if(etCodeRate.numerator == 2 && etCodeRate.denominator == 3)
		pstTuneParam->etFecCodeRate = DI_CH_SCODERATE_2_3;
	else if(etCodeRate.numerator == 3 && etCodeRate.denominator == 4)
		pstTuneParam->etFecCodeRate = DI_CH_SCODERATE_3_4;
	else if(etCodeRate.numerator == 5 && etCodeRate.denominator == 6)
		pstTuneParam->etFecCodeRate = DI_CH_SCODERATE_5_6;
	else if(etCodeRate.numerator == 7 && etCodeRate.denominator == 8)
		pstTuneParam->etFecCodeRate = DI_CH_SCODERATE_7_8;
	else if(etCodeRate.numerator == 3 && etCodeRate.denominator == 5)
		pstTuneParam->etFecCodeRate = DI_CH_S2CODERATE_3_5;
	else if(etCodeRate.numerator == 4 && etCodeRate.denominator == 5)
		pstTuneParam->etFecCodeRate = DI_CH_S2CODERATE_4_5;
	else if(etCodeRate.numerator == 8 && etCodeRate.denominator == 9)
		pstTuneParam->etFecCodeRate = DI_CH_S2CODERATE_8_9;
	else if(etCodeRate.numerator == 9 && etCodeRate.denominator == 10)
		pstTuneParam->etFecCodeRate = DI_CH_S2CODERATE_9_10;
	else
	{
		pstTuneParam->etFecCodeRate = DI_CH_SCODERATE_AUTO;
		CH_DI_Print(CH_LOG_ARGS, ("DI_CH_SCODERATE_AUTO \n"));
	}

	CH_DI_Print(CH_LOG_ARGS, (" etFecCodeRate [%d] \n",  pstTuneParam->etFecCodeRate));

	return;
}

static void ConvertDiToNexusParam( CH_SAT_TuneParam_t *pstSatTuneParam, NEXUS_FrontendSatelliteSettings *pstNexusTuneParam )
{

	if( pstSatTuneParam->etAntennaType == DI_CH_ANT_TYPE_SCD )
	{
		pstNexusTuneParam->frequency = pstSatTuneParam->ulScdUserBandFreq * MHZ_TO_HZ;
	}
	else
	{
		pstNexusTuneParam->frequency = pstSatTuneParam->ulFrequency * MHZ_TO_HZ;//KHZ_TO_HZ;
	}

	pstNexusTuneParam->symbolRate = pstSatTuneParam->ulSymbolRate * KHZ_TO_HZ;

	pstNexusTuneParam->mode = GetNexusMode( pstSatTuneParam->etTransSpec, pstSatTuneParam->etModulation );

	pstNexusTuneParam->codeRate = GetNexusCodeRate(pstSatTuneParam->etFecCodeRate);

	pstNexusTuneParam->searchRange = GetFreqSearchRange(pstSatTuneParam->ulSymbolRate, pstSatTuneParam->etTransSpec);

	pstNexusTuneParam ->ldpcPilotScan = true;
	pstNexusTuneParam ->ldpcPilot = false;
	pstNexusTuneParam ->ldpcPilotPll = false;

	pstNexusTuneParam->carrierOffset = 0;

	return;
}


uint32_t DBMTH_2560log10(uint32_t x)
{
	int32_t  x1 = 0;
	int32_t  x2 = 0;
	int32_t  x3 = 0;
	int32_t  x4 = 0;

	if (x == 0)
	return 0;

	x1 = 31;
	while (!((x >> x1) & 1))
	x1 = x1-1;
	x1 = x1+1;

	if (x1 > 20)
	x2 = (int32_t)(x >> (x1 - 8));
	else
	x2 = (int32_t)((x << 8) >> x1);

	x3 = -24381739 + x2*(62348 + (x2 << 7));
	x4 = 5907991 + (x2 << 16);

	return (unsigned)((770*(x3/(x4 >> 8) + (x1 << 8))) >> 8);
}


void SATFE_Platform_GetInputPower(int nUnitId, NEXUS_FrontendSatelliteAgcStatus Status, float *pPower)
{
#ifdef SATFE_USE_FLOATING_POINT
	double rf_gain, chan_gain, adj;
#else
	int32_t rf_gain=0, chan_gain=0, adj=0;
#endif
	int32_t x=0;
	uint32_t tI=0;
	uint32_t lf_int=0;

	CH_UNUSED(nUnitId);

	*pPower = 0;

	if (Status.agc[0].valid & 1)
	{
		lf_int = (((Status.agc[0].value >> 23) + 1) >> 1) & 0xFF;
		/* printf("lf_int=0x%X(%d)\n", lf_int, lf_int); */
		CH_DI_Print(CH_LOG_ARGS, ("lf_int=0x%X(%d)\n", lf_int, lf_int));

		if (lf_int >= 128)
		{
			if (lf_int >= 233)
				x = 256 - lf_int;
			else
				x = 23;
		}
		else if (lf_int >= 57)
			x = -57;
		else
			x = (int32_t)-((int32_t)lf_int);

#ifdef SATFE_USE_FLOATING_POINT
		rf_gain = (double)x * 0.5;
#else
		rf_gain = x * 8388608;
#endif
	}
	else
		rf_gain = 0;

	//if (Status.agc[nUnitId].valid & 2)
	{
		tI = (Status.agc[1].value >> 8);
		tI ^= (1 << 19);
#ifdef SATFE_USE_FLOATING_POINT
		chan_gain = 20.0 * log10((double)tI / 1024.0);
#else
		/* chan_gain is 8.24 format */
		chan_gain = (DBMTH_2560log10(tI) - DBMTH_2560log10(1024)) * 131072;
#endif
	}

#ifdef SATFE_USE_FLOATING_POINT
	adj = -28.0;
#else
	adj = -469762048;
#endif

#ifdef SATFE_USE_FLOATING_POINT
	*pPower = (float)(adj - chan_gain - rf_gain);
	/* printf("rf_gain=%f, chan_gain=%f, power=%f\n", -rf_gain, -chan_gain, *pPower); */
#else
	CH_DI_Print(CH_LOG_ARGS, ("adj=(%d), chan_gain=(%d), rf_gain=(%d)\n", adj,chan_gain , rf_gain));

	/* need to divide by 2^24 since the terms are in 8.24 format */
	*pPower = (float)((float)(adj - chan_gain - rf_gain) / 16777216.0);
#endif
}


#if defined(SUPPORT_DTG_SIGNAL_DISPLAY)
static HUINT32 ConvertDtgStrengthLevel(
	NEXUS_FrontendSatelliteMode etSatMode,
	HINT32  nInputPower
)
{
	HUINT32 ulStrength = 0;

	CH_UNUSED(etSatMode);

	CH_DI_Print(CH_LOG_ARGS, ("[ConvertDtgStrengthLevel] nInputPower: %d \n", nInputPower));

	if(nInputPower >= -2155)  // < -22dBm
		ulStrength = 100;
	else if( nInputPower >= -2569)  // -26dBm
		ulStrength = 90;
	else if( nInputPower >= -3050)  // -30dBm
		ulStrength = 80;
	else if( nInputPower >= -3317)  // -34dBm
		ulStrength = 70;
	else if( nInputPower >= -3785)  // -38dBm
		ulStrength = 60;
	else if( nInputPower >= -4200)  // -42dBm
		ulStrength = 50;
	else if( nInputPower >= -4530)  // -46dBm
		ulStrength = 40;
	else if( nInputPower >= -4995)  // -50dBm
		ulStrength = 30;
	else if( nInputPower >= -5400)  // -54dBm
		ulStrength = 20;
	else if( nInputPower >= -5800)  // -58dBm
		ulStrength = 10;
	else
		ulStrength = 0;

	CH_DI_Print(CH_LOG_ARGS, ("[ConvertDtgStrengthLevel] => ulStrength: (%d)  \n", ulStrength ));

	return ulStrength;
}



static HUINT32 ConvertDtgQualityLevel(
	NEXUS_FrontendSatelliteMode etSatMode,
	HUINT32 ulInSnr
)
{
	HUINT32 ulQuality = 0;

	CH_DI_Print(CH_LOG_ARGS, ("[ConvertDtgQualityLevel] ulInSnr: %d \n", ulInSnr));

	if( etSatMode == NEXUS_FrontendSatelliteMode_eDvb )
	{
		if( ulInSnr > 1458) 		//15 dB
			ulQuality = 100;
		else if( ulInSnr > 1373) 	//14dB
			ulQuality = 90;
		else if( ulInSnr > 1285) 	//13dB
			ulQuality = 80;
		else if( ulInSnr > 1193) 	//12dB
			ulQuality = 70;
		else if( ulInSnr > 1097) 	//11dB
			ulQuality = 60;
		else if( ulInSnr > 1005) 	//10dB
			ulQuality = 50;
		else if( ulInSnr > 907)		//9dB
			ulQuality = 40;
		else if( ulInSnr > 810)		//8dB
			ulQuality = 30;
		else if( ulInSnr > 712) 	//7dB
			ulQuality = 20;
		else if( ulInSnr > 618) 	//6dB
			ulQuality = 10;
		else 						// <5dB
			ulQuality = 0;
	}
	else if( etSatMode == NEXUS_FrontendSatelliteMode_eQpskLdpc )
	{
		if( ulInSnr > 1520)
			ulQuality = 100;
		else if( ulInSnr > 1420)
			ulQuality = 90;
		else if( ulInSnr > 1335)
			ulQuality = 80;
		else if( ulInSnr > 1236)
			ulQuality = 70;
		else if( ulInSnr > 1144)
			ulQuality = 60;
		else if( ulInSnr > 1045)
			ulQuality = 50;
		else if( ulInSnr > 950)
			ulQuality = 40;
		else if( ulInSnr > 844)
			ulQuality = 30;
		else if( ulInSnr > 760)
			ulQuality = 20;
		else if( ulInSnr > 650)
			ulQuality = 10;
		else
			ulQuality = 0;
	}
	else //NEXUS_FrontendSatelliteMode_e8pskLdpc
	{
		if( ulInSnr > 1691)
			ulQuality = 100;
		else if( ulInSnr > 1590)
			ulQuality = 90;
		else if( ulInSnr > 1500)
			ulQuality = 80;
		else if( ulInSnr > 1410)
			ulQuality = 70;
		else if( ulInSnr > 1310)
			ulQuality = 60;
		else if( ulInSnr > 1220)
			ulQuality = 50;
		else if( ulInSnr > 1125)
			ulQuality = 40;
		else if( ulInSnr > 1020)
			ulQuality = 30;
		else if( ulInSnr > 930)
			ulQuality = 20;
		else if( ulInSnr > 830)
			ulQuality = 10;
		else
			ulQuality = 7;
	}

	CH_DI_Print(CH_LOG_ARGS, ("[ConvertDtgQualityLevel] => ulQuality: (%d)	\n", ulQuality ));

	return ulQuality;

}
#else

static HUINT32 ConvertStrengthLevel(
	NEXUS_FrontendSatelliteMode etSatMode,
	HINT32  nInputPower
)
{
	HUINT32 ulStrength = 0;

	CH_UNUSED(etSatMode);

	if(nInputPower >= -3000)  // < -30dBm
			ulStrength = 100;
	else if( nInputPower >= -3500)  // -35dBm
		ulStrength = 90;
	else if( nInputPower >= -4000)  // -40dBm
		ulStrength = 85;
	else if( nInputPower >= -4500)  // -45dBm
		ulStrength = 80;
	else if( nInputPower >= -5000)  // -50dBm
		ulStrength = 75;
	else if( nInputPower >= -5500)  // -55dBm
		ulStrength = 70;
	else if( nInputPower >= -6000)  // -60dBm
		ulStrength = 60;
	else if( nInputPower >= -6500)  // -65dBm
		ulStrength = 50;
	else if( nInputPower >= -7000)  // -70dBm
		ulStrength = 40;
	else if( nInputPower >= -7500)  // -75dBm
		ulStrength = 30;
	else if( nInputPower >= -8000)  // -80dBm
		ulStrength = 20;
	else
		ulStrength = 10;

	CH_DI_Print(CH_LOG_ARGS, ("[ConvertStrengthLevel] => ulStrength: (%d)  \n", ulStrength ));

	return ulStrength;
}



static HUINT32 ConvertQualityLevel(
	NEXUS_FrontendSatelliteMode etSatMode,
	HUINT32 ulInSnr
)
{
	HUINT32 ulQuality = 0;

	CH_DI_Print(CH_LOG_ARGS, ("[ConvertQualityLevel] ulInSnr: %d \n", ulInSnr));

	if( etSatMode == NEXUS_FrontendSatelliteMode_eDvb )
	{
		if( ulInSnr > 1100) 		//15 dB
			ulQuality = 100;
		else if( ulInSnr > 1000) 	//14dB
			ulQuality = 90;
		else if( ulInSnr > 900) 	//13dB
			ulQuality = 80;
		else if( ulInSnr > 800) 	//12dB
			ulQuality = 70;
		else if( ulInSnr > 700) 	//11dB
			ulQuality = 60;
		else if( ulInSnr > 600) 	//10dB
			ulQuality = 50;
		else if( ulInSnr > 500)		//9dB
			ulQuality = 40;
		else if( ulInSnr > 400)		//8dB
			ulQuality = 30;
		else if( ulInSnr > 300) 	//7dB
			ulQuality = 20;
		else
			ulQuality = 10;

	}
	else if( etSatMode == NEXUS_FrontendSatelliteMode_eQpskLdpc )
	{
		if( ulInSnr > 1100)
			ulQuality = 100;
		else if( ulInSnr > 1000)
			ulQuality = 90;
		else if( ulInSnr > 900)
			ulQuality = 80;
		else if( ulInSnr > 800)
			ulQuality = 70;
		else if( ulInSnr > 700)
			ulQuality = 60;
		else if( ulInSnr > 600)
			ulQuality = 50;
		else if( ulInSnr > 500)
			ulQuality = 40;
		else if( ulInSnr > 400)
			ulQuality = 30;
		else if( ulInSnr > 300)
			ulQuality = 20;
		else
			ulQuality = 10;

	}
	else //NEXUS_FrontendSatelliteMode_e8pskLdpc
	{
		if( ulInSnr > 1100)
			ulQuality = 100;
		else if( ulInSnr > 1000)
			ulQuality = 90;
		else if( ulInSnr > 900)
			ulQuality = 80;
		else if( ulInSnr > 800)
			ulQuality = 70;
		else if( ulInSnr > 700)
			ulQuality = 60;
		else if( ulInSnr > 600)
			ulQuality = 50;
		else if( ulInSnr > 500)
			ulQuality = 40;
		else if( ulInSnr > 400)
			ulQuality = 30;
		else if( ulInSnr > 300)
			ulQuality = 20;
		else
			ulQuality = 10;
	}

	CH_DI_Print(CH_LOG_ARGS, ("[ConvertQualityLevel] => ulQuality: (%d)	\n", ulQuality ));

	return ulQuality;


}
#endif

static int GetSignalStatus( int nUnitId, DI_CH_SignalQuality_t *pstSignalInfo )
{
	NEXUS_Error 	etNErrorCode = NEXUS_SUCCESS;
	NEXUS_FrontendSatelliteStatus stFrontendStatus;
	NEXUS_FrontendSatelliteAgcStatus AgcStatus;

	HFLOAT32 fInputPower = 0.0, fSignalPowerdBm = 0.0;

	HUINT32 ulQuality = 0;
	HUINT32 ulStrength = 0;

	etNErrorCode = NEXUS_Frontend_GetSatelliteStatus(s_stFrontend[nUnitId], &stFrontendStatus);
	if(etNErrorCode != NEXUS_SUCCESS)
	{
		CH_DI_Print(CH_LOG_DEFAULT, ( "[GetSignalStatus] [%d] ERR! NEXUS_Frontend_GetSatelliteStatus (0x%x)",nUnitId, etNErrorCode ));
		return DI_CH_ERR;
	}

	etNErrorCode = NEXUS_Frontend_GetSatelliteAgcStatus(s_stFrontend[nUnitId], &AgcStatus);
	if(etNErrorCode != NEXUS_SUCCESS)
	{
		CH_DI_Print(CH_LOG_DEFAULT, ( "[GetSignalStatus] [%d] ERR! NEXUS_Frontend_GetSatelliteAgcStatus (0x%x)",nUnitId, etNErrorCode ));
		return DI_CH_ERR;
	}

	CH_DI_Print(CH_LOG_TSINFO, ("----------------------------------------------------------------------------------------\n"));
	CH_DI_Print(CH_LOG_TSINFO, ("frequency            =(%u)Hz\n", stFrontendStatus.frequency                        ));
	CH_DI_Print(CH_LOG_TSINFO, ("demodLocked          =(%d)\n", stFrontendStatus.demodLocked                      ));
	CH_DI_Print(CH_LOG_TSINFO, ("bertLocked           =(%d)\n", stFrontendStatus.bertLocked                       ));
	CH_DI_Print(CH_LOG_TSINFO, ("symbolRate           =(%u)\n", stFrontendStatus.symbolRate                       ));
	CH_DI_Print(CH_LOG_TSINFO, ("symbolRateError      =(%d)\n", stFrontendStatus.symbolRateError                  ));
	CH_DI_Print(CH_LOG_TSINFO, ("carrierOffset        =(%d)\n", stFrontendStatus.carrierOffset                    ));
	//CH_DI_Print(CH_LOG_TSINFO, ("carrierError         =(%d)\n", stFrontendStatus.carrierError                     ));
	CH_DI_Print(CH_LOG_TSINFO, ("outputBitRate        =(%u)\n", stFrontendStatus.outputBitRate                    ));
	CH_DI_Print(CH_LOG_TSINFO, ("ifAgcLevel           =(%u)\n", stFrontendStatus.ifAgcLevel                       ));
	CH_DI_Print(CH_LOG_TSINFO, ("rfAgcLevel           =(%u)\n", stFrontendStatus.rfAgcLevel                       ));
	CH_DI_Print(CH_LOG_TSINFO, ("intAgcLevel          =(%u)\n", stFrontendStatus.intAgcLevel                      ));
	CH_DI_Print(CH_LOG_TSINFO, ("snrEstimate          =(%u)(1/100 dB)\n", stFrontendStatus.snrEstimate                      ));
	CH_DI_Print(CH_LOG_TSINFO, ("berEstimate          =(%u)\n", stFrontendStatus.berEstimate                      ));
	CH_DI_Print(CH_LOG_TSINFO, ("fecCorrected         =(%u)\n", stFrontendStatus.fecCorrected                     ));
	CH_DI_Print(CH_LOG_TSINFO, ("fecUncorrected       =(%u)\n", stFrontendStatus.fecUncorrected                   ));
	CH_DI_Print(CH_LOG_TSINFO, ("fecClean             =(%u)\n", stFrontendStatus.fecClean                         ));
	CH_DI_Print(CH_LOG_TSINFO, ("bitErrCorrected      =(%u)\n", stFrontendStatus.bitErrCorrected                  ));
	CH_DI_Print(CH_LOG_TSINFO, ("reacquireCount       =(%u)\n", stFrontendStatus.reacquireCount                   ));
#if (NEXUS_VERSION >= 1290)
	CH_DI_Print(CH_LOG_TSINFO, ("berErrors            =(%u)\n", stFrontendStatus.berErrorCount                    ));
#else
	CH_DI_Print(CH_LOG_TSINFO, ("berErrors            =(%u)\n", stFrontendStatus.berErrors                        ));
#endif
	CH_DI_Print(CH_LOG_TSINFO, ("preViterbiErrorCount =(%u)\n", stFrontendStatus.preViterbiErrorCount             ));
	CH_DI_Print(CH_LOG_TSINFO, ("mpegErrors           =(%u)\n", stFrontendStatus.mpegErrors                       ));
	CH_DI_Print(CH_LOG_TSINFO, ("mpegCount            =(%u)\n", stFrontendStatus.mpegCount                        ));
	CH_DI_Print(CH_LOG_TSINFO, ("----------------------------------------------------------------------------------------\n"));
	CH_DI_Print(CH_LOG_TSINFO, ("ifAgc                =(0x%x):(%u)\n", stFrontendStatus.ifAgc, stFrontendStatus.ifAgc ));
	CH_DI_Print(CH_LOG_TSINFO, ("rfAgc                =(0x%x):(%u)\n", stFrontendStatus.rfAgc, stFrontendStatus.rfAgc ));
	CH_DI_Print(CH_LOG_TSINFO, ("agf                  =(0x%x):(%d)\n", stFrontendStatus.agf, stFrontendStatus.agf     ));
	CH_DI_Print(CH_LOG_TSINFO, ("ifAgcLevel                =(0x%x):(%u)\n", stFrontendStatus.ifAgc, stFrontendStatus.ifAgcLevel));
	CH_DI_Print(CH_LOG_TSINFO, ("rfAgcLevel                =(0x%x):(%u)\n", stFrontendStatus.rfAgc, stFrontendStatus.rfAgcLevel));
	CH_DI_Print(CH_LOG_TSINFO, ("----------------------------------------------------------------------------------------\n"));
#if 0
	CH_DI_Print(CH_LOG_TSINFO, ("agc0                 =(0x%x):(%u)\n", stFrontendStatus.lnaStatus.agc0, stFrontendStatus.lnaStatus.agc0 ));
	CH_DI_Print(CH_LOG_TSINFO, ("agc1                 =(0x%x):(%u)\n", stFrontendStatus.lnaStatus.agc1, stFrontendStatus.lnaStatus.agc1 ));
	CH_DI_Print(CH_LOG_TSINFO, ("intConfig            =(0x%x):(%u)\n", stFrontendStatus.lnaStatus.intConfig, stFrontendStatus.lnaStatus.intConfig));
	CH_DI_Print(CH_LOG_TSINFO, ("extConfig            =(0x%x):(%u)\n", stFrontendStatus.lnaStatus.extConfig, stFrontendStatus.lnaStatus.extConfig));
	CH_DI_Print(CH_LOG_TSINFO, ("version              =(0x%x):(%u)\n", stFrontendStatus.lnaStatus.version, stFrontendStatus.lnaStatus.version));
	CH_DI_Print(CH_LOG_TSINFO, ("----------------------------------------------------------------------------------------\n"));
#endif
	CH_DI_Print(CH_LOG_TSINFO, ("chipId               =(%x)\n", stFrontendStatus.version.chipId                   ));
	CH_DI_Print(CH_LOG_TSINFO, ("chipVersion          =(%x)\n", stFrontendStatus.version.chipVersion              ));
	CH_DI_Print(CH_LOG_TSINFO, ("bondingOption        =(%x)\n", stFrontendStatus.version.bondingOption            ));
	CH_DI_Print(CH_LOG_TSINFO, ("MajVer               =(%x)\n", stFrontendStatus.version.apMicrocodeVersion       ));
	CH_DI_Print(CH_LOG_TSINFO, ("MinVer               =(%x)\n", stFrontendStatus.version.hostConfigurationVersion ));
	CH_DI_Print(CH_LOG_TSINFO, ("----------------------------------------------------------------------------------------\n"));

	SATFE_Platform_GetInputPower( nUnitId, AgcStatus, &fInputPower);
	//fSignalPowerdBm = fInputPower - 125.0;
	fSignalPowerdBm = fInputPower + 5.0;
	
	CH_DI_Print(CH_LOG_TSINFO, ( "[GetSignalStatus] nUnitId[%d]Frontend.mode = (%d) \n", nUnitId, stFrontendStatus.mode));
	CH_DI_Print(CH_LOG_TSINFO, ( "[GetSignalStatus] nUnitId[%d] RF InputPower  = (%3.2f )dBm \n", nUnitId, fSignalPowerdBm));
	CH_DI_Print(CH_LOG_TSINFO, ( "[GetSignalStatus] nUnitId[%d] agc valid= (%d) \n", nUnitId, AgcStatus.agc[nUnitId].valid));
	CH_DI_Print(CH_LOG_TSINFO, ( "[GetSignalStatus] nUnitId[%d] agc value  = (%u) \n", nUnitId, AgcStatus.agc[nUnitId].value));
	CH_DI_Print(CH_LOG_TSINFO, ("----------------------------------------------------------------------------------------\n"));

#if defined(SUPPORT_DTG_SIGNAL_DISPLAY)
	ulStrength = ConvertDtgStrengthLevel(stFrontendStatus.mode, (HINT32)(fSignalPowerdBm*100));
#else
	ulStrength = ConvertStrengthLevel(stFrontendStatus.mode, (HINT32)(fSignalPowerdBm*100));
#endif

	if( stFrontendStatus.demodLocked == true)
	{
		//ulQuality = ConvertDtgQualityLevel(stFrontendStatus.mode, stFrontendStatus.snrEstimate);
		ulQuality = ConvertQualityLevel(stFrontendStatus.mode, stFrontendStatus.snrEstimate);

		pstSignalInfo->fSnr = ((float)(stFrontendStatus.snrEstimate))/100;
		pstSignalInfo->ulAgc = 0;
		pstSignalInfo->ulRfAgc = (HUINT32)stFrontendStatus.rfAgc;
		pstSignalInfo->ulIfAgc = (HUINT32)stFrontendStatus.ifAgc;
		pstSignalInfo->ulUnCorrectedNo = stFrontendStatus.fecUncorrected;
		pstSignalInfo->fSignalInputPower = fSignalPowerdBm;
		pstSignalInfo->fBer = stFrontendStatus.berEstimate;
		pstSignalInfo->ulQuality = ulQuality;
		pstSignalInfo->ulStrength = ulStrength;
	}
	else
	{
		/* demod unlock */
		pstSignalInfo->fSnr = 0;
		pstSignalInfo->ulAgc = 0;
		pstSignalInfo->ulRfAgc = 0;
		pstSignalInfo->ulIfAgc = 0;
		pstSignalInfo->ulUnCorrectedNo = 0;
		pstSignalInfo->fSignalInputPower = 0;
		pstSignalInfo->fBer = 0;
		pstSignalInfo->ulQuality = 0;
		pstSignalInfo->ulStrength = ulStrength;

		CH_DI_Print(CH_LOG_DEFAULT, ( "[GetSignalStatus] [%d] Demod UnLocked \n", nUnitId));
	}
	return DI_CH_OK;
}


static void DiseqcCompleteCallback(void *data, int unused)
{
	BSTD_UNUSED(data);
	BSTD_UNUSED(unused);
}



static HUINT32 GetOduSetupTime( int nChannelId, CH_SAT_TuneParam_t *pstSatTuneParam )
{
	HUINT32 ulSetupTime = 0;
	CH_SAT_TuneParam_t * pstPreviousLockedaram;

	pstPreviousLockedaram	= (CH_SAT_TuneParam_t *)GetPreviousLockedTuneInfo(nChannelId);

	if((pstPreviousLockedaram->ulFrequency == pstSatTuneParam->ulFrequency)
		&&(pstPreviousLockedaram->ulSymbolRate == pstSatTuneParam->ulSymbolRate))
	{
		if(pstSatTuneParam->etAntennaType == DI_CH_ANT_TYPE_DISEQC)
		{
			if((pstPreviousLockedaram->etDiseqcInput != pstSatTuneParam->etDiseqcInput)
				||(pstPreviousLockedaram->etPolarization != pstSatTuneParam->etPolarization)
				||(pstPreviousLockedaram->b22kTone != pstSatTuneParam->b22kTone))
			{
				ulSetupTime = ODU_SETUP_TIME_DISEQC;
			}
			else
			{
				CH_DI_Print(CH_LOG_ARGS	, (">>>>> Same TP in DISEQC!!! \n"));
			}
		}
		else if(pstSatTuneParam->etAntennaType == DI_CH_ANT_TYPE_LNB)
		{
			if((pstPreviousLockedaram->etPolarization != pstSatTuneParam->etPolarization)
				||(pstPreviousLockedaram->b22kTone != pstSatTuneParam->b22kTone))
			{
				ulSetupTime = ODU_SETUP_TIME_LNB;
			}
			else
			{
				CH_DI_Print(CH_LOG_ARGS	, (">>>>> Same TP in LNB!!! \n"));
			}
		}
		else // SCD
		{
			ulSetupTime = ODU_SETUP_TIME_DISEQC;
		}
	}
	else
	{
		CH_DI_Print(CH_LOG_ARGS	, (">>>>> Skip : ODU_SETUP_TIME \n"));
	}

	CH_DI_Print(CH_LOG_ARGS	, ("<<<<< ODU_SETUP_TIME (%d) \n", ulSetupTime));
	return ulSetupTime;

}

static void lock_callback(void *context, int param)
{
    CH_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);

}


#define __DB45216_PUBLIC_FUNCITONS__
/*********************** External Function ***********************/

int DB45216_ReInitDevice(int nChannelId)
{
	int nUnitId=0, i=0;

	NEXUS_PlatformConfiguration platformConfig;

	NEXUS_FrontendHandle frontend[NUM_OF_SAT_CHANNEL]={NULL,};
	NEXUS_FrontendDeviceHandle frontendDevice[NUM_OF_SAT_CHANNEL];
	NEXUS_FrontendDeviceStatus frontendDeviceStatus[NUM_OF_SAT_CHANNEL];

	CH_DI_Print(CH_LOG_DEFAULT, ("DB45216_ReInitDevice ...\n"));

	nUnitId = GetDeviceUnitId( nChannelId );

	if(nChannelId == MASTER_CHANNEL)
	{
		//ResetChannelHw(nUnitId);
		NEXUS_Platform_OpenFrontend(nUnitId);
		//NEXUS_Platform_InitFrontend();

		for ( i = 0; i < NUM_OF_SAT_CHANNEL; i++ )
		{
			NEXUS_FrontendAcquireSettings settings;
			NEXUS_Frontend_GetDefaultAcquireSettings(&settings);
			settings.capabilities.satellite = true;
			frontend[i] = NEXUS_Frontend_Acquire(&settings);
			if (!frontend[i])
			{
				CH_DI_Print(CH_LOG_DEFAULT, ("Unable to find Satellite-capable frontend\n"));
				return -1;
			}
		}

		frontendDevice[0] = NEXUS_Frontend_GetDevice(frontend[0]);
		frontendDevice[1] = NEXUS_Frontend_GetDevice(frontend[1]);
		frontendDevice[2] = NEXUS_Frontend_GetDevice(frontend[2]);
		frontendDevice[3] = NEXUS_Frontend_GetDevice(frontend[3]);

		while(1)
		{
			VK_TASK_Sleep(1500);
			NEXUS_FrontendDevice_GetStatus(frontendDevice[0],&frontendDeviceStatus[0]);
			NEXUS_FrontendDevice_GetStatus(frontendDevice[1],&frontendDeviceStatus[1]);
			NEXUS_FrontendDevice_GetStatus(frontendDevice[2],&frontendDeviceStatus[2]);
			NEXUS_FrontendDevice_GetStatus(frontendDevice[3],&frontendDeviceStatus[3]);

			if((frontendDeviceStatus[0].openFailed == false && frontendDeviceStatus[0].openPending == false) 
			&& (frontendDeviceStatus[1].openFailed == false && frontendDeviceStatus[1].openPending == false)
			&& (frontendDeviceStatus[2].openFailed == false && frontendDeviceStatus[2].openPending == false)
			&& (frontendDeviceStatus[3].openFailed == false && frontendDeviceStatus[3].openPending == false)
			)
			{
				CH_DI_Print(CH_LOG_DEFAULT, ("NEXUS_Platform_InitFrontend() is OK!!! \n"));
				break;
			}
			else
			{
				CH_DI_Print(CH_LOG_DEFAULT, ("############## 0. frontend openFailed(%d) openPending(%d) \n",frontendDeviceStatus[0].openFailed,frontendDeviceStatus[0].openPending));
			}
		}
	}

	NEXUS_Platform_GetConfiguration(&platformConfig);

	s_stFrontend[nUnitId] = platformConfig.frontend[nUnitId];
	if (NULL == s_stFrontend[nUnitId] )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("Error : Unable to find Satellite-capable frontend s_hFrontend[%d]\n", nUnitId));
		return DI_CH_ERR;
	}
	else
	{
		CH_DI_Print(CH_LOG_STATUS, ("Find Satellite-capable frontend s_hFrontend[%d]=[0x%x] \n", nUnitId, s_stFrontend[nUnitId]));
	}

	BKNI_CreateEvent(&lockChangedEvent[nUnitId]);

	if(nUnitId < 2)
	{
		SetRfInputId(nChannelId, 0);
	}
	else
	{
		SetRfInputId(nChannelId, 1);
	}

	//DRV_I2c_SetClock( 0, 100);
	return DI_CH_OK;

}

int DB45216_UnInitDevice( int nChannelId )
{
	int nUnitId=0;

	nUnitId = GetDeviceUnitId( nChannelId );

	SetTuneStatus( nChannelId, CH_STATUS_IDLE );

	if(nChannelId == MASTER_CHANNEL)
	{
		#if 0
		for ( i = 0; i < NUM_OF_SAT_CHANNEL; i++ )
		{
			CH_DI_Print(CH_LOG_DEFAULT, (" DB45216_UnInitDevice] NEXUS_Frontend_Untune [%d] !!!!!\n", i));
			NEXUS_Frontend_Untune(s_stFrontend[i]);
			VK_TASK_Sleep(HAB_WAIT_TIME);
		}
		#endif
		CH_DI_Print(CH_LOG_DEFAULT, ("[DB45216] DB45216_UnInitDevice(%d) NEXUS_Platform_UninitFrontend !!\n", nUnitId));
		NEXUS_Platform_UninitFrontend();
		VK_TASK_Sleep(HAB_WAIT_TIME);
	}
	
	return DI_CH_OK;
}


void DRV_S_InstallApi( void )
{
	NIM_InitDevice 			= &DB45216_InitDevice;
	NIM_SetTune 			= &DB45216_SetTune;
	NIM_CheckTuneState 		= &DB45216_CheckTuneState;
	NIM_CheckLock 			= &DB45216_CheckLock;
	NIM_GetSignalInfo 		= &DB45216_GetSignalInfo;
	NIM_SetPowerState 		= &DB45216_SetPowerState;
	NIM_EnableTsOutput 		= &DB45216_EnableTsOutput;
	NIM_DisableTsOutput 	= &DB45216_DisableTsOutput;
	NIM_SendDiseqcMsg 		= &DB45216_SendDiseqcMsg;
	NIM_UpdateTunedParam	= &DB45216_UpdateTunedParam;
	NIM_SetInputMode		= &DB45216_SetInputMode;
	NIM_SetInputSource		= &DB45216_SetInputSource;
	NIM_ResetDiseqc             = &DB45216_ResetDiseqc;
	NIM_SetToneBurst		= &DB45216_SetToneBurst;
	NIM_ReInitDevice		= &DB45216_ReInitDevice;
      NIM_UnInitDevice 		= &DB45216_UnInitDevice;
	NIM_GetDeviceStatus		= &DB45216_GetDeviceStatus;

	LNB_Get22KhzTone		= &DB45216_LnbGet22KhzTone;
	LNB_Set22KhzTone		= &DB45216_LnbSet22KhzTone;

	LNB_InitDevice 			= &DRV_CH_LNB_Init;
	LNB_SetOutput			= &DRV_CH_LNB_SetOutput;
	LNB_SetPolarisation 	= &DRV_CH_LNB_SetPolar;
	LNB_CheckAntState		= &DRV_CH_LNB_CheckAntState;
	return;
}



int DB45216_InitDevice( int nChannelId )
{
	int nUnitId=0, i=0;

	NEXUS_PlatformConfiguration platformConfig;

	NEXUS_FrontendHandle frontend[NUM_OF_SAT_CHANNEL]={NULL,};
	NEXUS_FrontendDeviceHandle frontendDevice[NUM_OF_SAT_CHANNEL];
	NEXUS_FrontendDeviceStatus frontendDeviceStatus[NUM_OF_SAT_CHANNEL];

	CH_DI_Print(CH_LOG_ARGS, ("DB45216_InitDevice ...\n"));

	nUnitId = GetDeviceUnitId( nChannelId );

	if(nChannelId == MASTER_CHANNEL)
	{
		//ResetChannelHw(nUnitId);
		NEXUS_Platform_OpenFrontend(nUnitId);
		//NEXUS_Platform_InitFrontend();

		for ( i = 0; i < NUM_OF_SAT_CHANNEL; i++ )
		{
			NEXUS_FrontendAcquireSettings settings;
			NEXUS_Frontend_GetDefaultAcquireSettings(&settings);
			settings.capabilities.satellite = true;
			frontend[i] = NEXUS_Frontend_Acquire(&settings);
			if (!frontend[i])
			{
				CH_DI_Print(CH_LOG_DEFAULT, ("Unable to find Satellite-capable frontend\n"));
				return -1;
			}
		}

		frontendDevice[0] = NEXUS_Frontend_GetDevice(frontend[0]);
		frontendDevice[1] = NEXUS_Frontend_GetDevice(frontend[1]);
		frontendDevice[2] = NEXUS_Frontend_GetDevice(frontend[2]);
		frontendDevice[3] = NEXUS_Frontend_GetDevice(frontend[3]);

		while(1)
		{
			VK_TASK_Sleep(1500);
			NEXUS_FrontendDevice_GetStatus(frontendDevice[0],&frontendDeviceStatus[0]);
			NEXUS_FrontendDevice_GetStatus(frontendDevice[1],&frontendDeviceStatus[1]);
			NEXUS_FrontendDevice_GetStatus(frontendDevice[2],&frontendDeviceStatus[2]);
			NEXUS_FrontendDevice_GetStatus(frontendDevice[3],&frontendDeviceStatus[3]);

			if((frontendDeviceStatus[0].openFailed == false && frontendDeviceStatus[0].openPending == false) 
				&& (frontendDeviceStatus[1].openFailed == false && frontendDeviceStatus[1].openPending == false)
				&& (frontendDeviceStatus[2].openFailed == false && frontendDeviceStatus[2].openPending == false)
				&& (frontendDeviceStatus[3].openFailed == false && frontendDeviceStatus[3].openPending == false)
			)
			{
				CH_DI_Print(CH_LOG_DEFAULT, ("NEXUS_Platform_InitFrontend() is OK!!! \n"));
				break;
			}
			else
			{
				CH_DI_Print(CH_LOG_DEFAULT, ("############## 0. frontend openFailed(%d) openPending(%d) \n",frontendDeviceStatus[0].openFailed,frontendDeviceStatus[0].openPending));
			}
		}
	}

	NEXUS_Platform_GetConfiguration(&platformConfig);

	s_stFrontend[nUnitId] = platformConfig.frontend[nUnitId];
	if (NULL == s_stFrontend[nUnitId] )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("Error : Unable to find Satellite-capable frontend s_hFrontend[%d]\n", nUnitId));
		return DI_CH_ERR;
	}
	else
	{
		CH_DI_Print(CH_LOG_STATUS, ("Find Satellite-capable frontend s_hFrontend[%d]=[0x%x] \n", nUnitId, s_stFrontend[nUnitId]));
	}

	BKNI_CreateEvent(&lockChangedEvent[nUnitId]);

	if(nUnitId < 2)
	{
		SetRfInputId(nChannelId, 0);
	}
	else
	{
		SetRfInputId(nChannelId, 1);
	}

	//DRV_I2c_SetClock( 0, 100);
	return DI_CH_OK;

}


int DB45216_SetTune( int nChannelId, CH_SAT_TuneParam_t *pstSatTuneParam )
{
	NEXUS_Error 	etNErrorCode = NEXUS_SUCCESS;
	NEXUS_FrontendSatelliteSettings satSettings;
	int nUnitId = 0;
	HUINT32 ulOduSetupTime = 0;

      DRV_CH_LockReentrant();

	nUnitId = GetDeviceUnitId( nChannelId );
	NEXUS_Frontend_GetDefaultSatelliteSettings(&satSettings);
	ConvertDiToNexusParam( pstSatTuneParam, &satSettings);

	#if 0 // TODO:
	/* RS Stuff byte*/
	etNErrorCode = NEXUS_Frontend_SetStuffBytes(s_stFrontend[nUnitId], &ucStuffByte);// (16byte)
	if(etNErrorCode != NEXUS_SUCCESS)
	{
		CH_DI_Print(0, ( "[DB45216_SetTune] [%d] ERR! NEXUS_Frontend_SetStuffBytes() Error Code(0x%x)",nUnitId, etNErrorCode ));
	        DRV_CH_UnLockReentrant();
		return DI_CH_ERR;
	}
	#endif

	ulOduSetupTime = GetOduSetupTime(nChannelId, pstSatTuneParam);
	if(ulOduSetupTime != 0)
	{
		VK_TASK_Sleep( ulOduSetupTime );
	}

	satSettings.lockCallback.callback = lock_callback;
	satSettings.lockCallback.context = lockChangedEvent[nUnitId];

	CH_DI_Print(CH_LOG_TUNE, ("\n ############	Tuning NEXUS_Frontend_Satellite ############ \n"));

	#ifdef CH_PRINT_LOW_LEVEL_ZAPPING_TIME
	g_ulRequestTune[nUnitId] = VK_TIMER_GetSystemTick();
	#endif
	
	etNErrorCode = NEXUS_Frontend_TuneSatellite(s_stFrontend[nUnitId], &satSettings);
	if(etNErrorCode != NEXUS_SUCCESS)
	{
		CH_DI_Print(0, ( "[DB45216_SetTune] [%d] ERR! NEXUS_Frontend_TuneSatellite() Error Code(0x%x)",nUnitId, etNErrorCode ));
		DRV_CH_UnLockReentrant();
		return DI_CH_ERR;
	}
	DRV_CH_UnLockReentrant();

	return DI_CH_OK;
}



int DB45216_CheckTuneState( int nChannelId, CH_SAT_TransportSpec_e etSatSystem, HUINT32 ulSymbolRate )
{
	HBOOL	 bLockStatus = FALSE;
	int i;
	int nDelay = 0;
	int nRetryCount = 0;
	int nUnitId;
	CH_Status_e etCurrentState;

	nUnitId = GetDeviceUnitId( nChannelId );

	if( etSatSystem == DI_CH_TRANS_DVBS )
	{
		if( (ulSymbolRate < LOW_SYMBOL_RATE_LIMIT) ||
		(ulSymbolRate > HIGH_SYMBOL_RATE_LIMIT) )
		{
			//100*35 = 3500ms
			//130*25 = 3500ms
			nDelay = HAB_WAIT_TIME;	//need to access hab time 
			nRetryCount = 25;
		}
		else
		{
			//100*20 = 2000ms
			nDelay = 100;
			nRetryCount = 20;
		}
	}
	else //DI_CH_TRANS_DVBS2
	{
		if( (ulSymbolRate < LOW_SYMBOL_RATE_LIMIT) ||
		(ulSymbolRate > HIGH_SYMBOL_RATE_LIMIT) )
		{
			//100*40 = 4000ms
			//130*29 = 4000ms
			nDelay = HAB_WAIT_TIME;
			nRetryCount = 29;
		}
		else
		{
			//100*30 = 3000ms
			//100*23 = 3000ms
			nDelay = HAB_WAIT_TIME;
			nRetryCount = 23;
		}
	}

	CH_DI_Print(CH_LOG_STATUS, ("nDelay [%d]:[%d] nRetryCount\n", nDelay, nRetryCount));
	VK_TASK_Sleep(MIN_WAIT_DELAY);

	for( i=0; i<nRetryCount; i++ )
	{
		etCurrentState =	GetTuneStatus(nChannelId);
		if( ( etCurrentState == CH_STATUS_IDLE) || (etCurrentState == CH_STATUS_CANCLE) )
		{
			CH_DI_Print(CH_LOG_STATUS, ("Cancel locking status...\n"));
			return DI_CH_CANCEL_TUNE;
		}

		bLockStatus = DB45216_CheckLock( nChannelId );
		CH_DI_Print(CH_LOG_STATUS, ("[%d] bLockStatus : %d [%d]\n", nChannelId, bLockStatus, i));

		if( bLockStatus == TRUE )
		{
			#ifdef CH_PRINT_LOW_LEVEL_ZAPPING_TIME
			g_ulLowLevelLockedTime[nChannelId] = VK_TIMER_GetSystemTick();
			CH_DI_Print(CH_LOG_DEFAULT, ("[DB45216_CheckTuneState] [%d] Lock Time( %d )ms  \n" ,nChannelId, g_ulLowLevelLockedTime[nChannelId] - g_ulRequestTune[nChannelId] ));
			#endif
			CH_DI_Print(1, (">>>>>>>>>>>>>>>>>ACQ_LOCKED_AND_TRACKING [%d ms] \n", (i * nDelay)));
			return DI_CH_OK;
		}
		else
		{
			VK_TASK_Sleep(nDelay);
		}
	}

	#ifdef CH_PRINT_LOW_LEVEL_ZAPPING_TIME
	g_ulLowLevelUnlockedTime[nChannelId] = VK_TIMER_GetSystemTick();
	CH_DI_Print(CH_LOG_DEFAULT, ("[DB45216_CheckTuneState] [%d] UnLock Time( %d )ms  \n", nChannelId, g_ulLowLevelUnlockedTime[nChannelId] - g_ulRequestTune[nChannelId] ));
	#endif

	return DI_CH_ERR;

}



HBOOL DB45216_CheckLock( int nChannelId )
{
	int nRet =FALSE ;
	HUINT8 nUnitId;
	NEXUS_Error etNErrorCode = NEXUS_SUCCESS;

	NEXUS_FrontendFastStatus stFrontendStatus;
	nUnitId = GetDeviceUnitId( nChannelId );

	etNErrorCode = NEXUS_Frontend_GetFastStatus(s_stFrontend[nUnitId], &stFrontendStatus);
	if(etNErrorCode != NEXUS_SUCCESS)
	{
		CH_DI_Print(CH_LOG_DEFAULT, ( "[DB45216_CheckLock] [%d] ERR! NEXUS_Frontend_GetSatelliteStatus() Error Code(0x%x)",nUnitId, etNErrorCode ));
	}
	else
	{
		if(stFrontendStatus.lockStatus == NEXUS_FrontendLockStatus_eLocked)
		{
			//LOCK
			nRet = TRUE;
			CH_DI_Print(CH_LOG_TUNE, (">>>>> LOCK [%d] <<<<<	\n", nUnitId));
		}
		else
		{
			//unlock
			nRet = FALSE;
			CH_DI_Print(CH_LOG_TUNE, (">>>>> UNLOCK [%d]<<<<<  \n",nUnitId));
		}
	}

	return(nRet);
}



int DB45216_GetSignalInfo( int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo )
{
	int nResult = DI_CH_OK;
	int nUnitId;

	DRV_CH_LockReentrant();
	nUnitId = GetDeviceUnitId( nChannelId );
	nResult = GetSignalStatus( nUnitId, pstSignalInfo);
	DRV_CH_UnLockReentrant();

	return nResult;
}


int DB45216_SetPowerState( int nChannelId, CH_Power_e etPower )
{
	int nResult = DI_CH_OK;
	int nUnitId;
	//	BERR_Code etNErrorCode = BERR_SUCCESS;
	//	BAST_P_ChannelHandle *pstChannelHandle;

	nUnitId = GetDeviceUnitId( nChannelId );

	if( etPower == CH_POWER_OFF ) // set standby
	{
		CH_DI_Print(CH_LOG_STATUS, (">>>>>NEXUS_PowerDown <<<<< \n"));
		/*
		//DEV_TUNER_LOCK();
		//etBErrorCode = BAST_PowerDown(s_stBastChannelHandle[nUnitId], BAST_CORE_SDS);
		//DEV_TUNER_UNLOCK();
		if(etBErrorCode != BERR_SUCCESS)
		{
		CH_DI_Print(0, ( "nUnitId(%d): etBErrorCode(0x%x)", nUnitId, etBErrorCode ));
		nResult = DI_CH_ERR;
		}
		*/
	}
	else // unset standby
	{
		CH_DI_Print(CH_LOG_STATUS, (">>>>>NEXUS_PowerUp <<<<<< \n"));
		/*
		//DEV_TUNER_LOCK();
		//etBErrorCode = BAST_PowerUp(s_stBastChannelHandle[nUnitId], BAST_CORE_SDS);
		//DEV_TUNER_UNLOCK();
		if(etNErrorCode != BERR_SUCCESS)
		{
		CH_DI_Print(0, ( "nUnitId(%d): etBErrorCode(0x%x)", nUnitId, etBErrorCode) );
		nResult = DI_CH_ERR;
		}
		*/
	}

	return nResult;
}



int DB45216_EnableTsOutput( int nChannelId )
{
	//NEXUS_Error 	etNErrorCode = NEXUS_SUCCESS;
	int nUnitId;

	nUnitId = GetDeviceUnitId( nChannelId );

	CH_DI_Print(CH_LOG_ARGS, ("[%d] Enable TS Output ===== \n", nUnitId));

#if (NEXUS_VERSION >= 1404)
	NEXUS_Frontend_SetInputBandEnable(s_stFrontend[nUnitId], true);
#endif

	return DI_CH_OK;
}



int DB45216_DisableTsOutput( int nChannelId )
{
	int nUnitId;

	nUnitId = GetDeviceUnitId( nChannelId );

	CH_DI_Print(CH_LOG_ARGS, ("[%d] Disable TS Output ===== \n", nUnitId));

#if (NEXUS_VERSION >= 1404)
	NEXUS_Frontend_SetInputBandEnable(s_stFrontend[nUnitId], false);
#else
	NEXUS_Frontend_Untune(s_stFrontend[nUnitId]);
#endif

	return DI_CH_OK;
}


int DB45216_SetToneBurst(int nChannelId, HBOOL b22kTone, CH_SAT_DiseqcInput_e etInput, CH_SAT_AntennaType_e etAntennaType, CH_SAT_DiseqcVer_e etDiseqcVer )
{
	int nResult	= DI_CH_OK;
	int nRfInputId=0, nUnitId=0;

	NEXUS_Error 	etNErrorCode = NEXUS_SUCCESS;

	nRfInputId = GetRfInputId( nChannelId );
	nUnitId = GetDeviceUnitId( nChannelId );

	CH_DI_Print(CH_LOG_ARGS, ( "[%d] DB45216_SetToneBurst [%d] [%d] [%d] \n",nChannelId, nRfInputId, etInput, etAntennaType));

	CH_UNUSED(etAntennaType);
	CH_UNUSED(etDiseqcVer);

	NEXUS_FrontendDiseqcSettings diseqcSettings;

	//NEXUS_Frontend_GetDiseqcSettings(s_stFrontend[nRfInputId], &diseqcSettings);
	NEXUS_Frontend_GetDiseqcSettings(s_stFrontend[nUnitId], &diseqcSettings);

	diseqcSettings.toneEnabled = b22kTone;
	diseqcSettings.toneMode = NEXUS_FrontendDiseqcToneMode_eEnvelope;
	
#if (NEXUS_VERSION >= 1501)
	diseqcSettings.preTransmitDelay = 10;
#else
	diseqcSettings.replyDisabled = TRUE;
#endif

	switch( etInput )
	{
		case DI_CH_DISEQC_INPUT_A:
		case DI_CH_DISEQC_INPUT_C:
		case DI_CH_DISEQC_INPUT_TB_A:
			diseqcSettings.toneBurst = NEXUS_FrontendDiseqcToneBurst_eUnmodulated;
			break;

		case DI_CH_DISEQC_INPUT_B:
		case DI_CH_DISEQC_INPUT_D:
		case DI_CH_DISEQC_INPUT_TB_B:
			diseqcSettings.toneBurst = NEXUS_FrontendDiseqcToneBurst_eNominal;
			break;

		case DI_CH_DISEQC_INPUT_SCD_A:
		case DI_CH_DISEQC_INPUT_SCD_B:
		case DI_CH_DISEQC_INPUT_NONE:
			diseqcSettings.toneBurst = NEXUS_FrontendDiseqcToneBurst_eNone;
		default:
			break;
	}

	//etNErrorCode = NEXUS_Frontend_SetDiseqcSettings(s_stFrontend[nRfInputId], &diseqcSettings);
	etNErrorCode = NEXUS_Frontend_SetDiseqcSettings(s_stFrontend[nUnitId], &diseqcSettings);
	if(etNErrorCode != NEXUS_SUCCESS)
	{
		CH_DI_Print(CH_LOG_DEFAULT, ( "[DB45216_SetToneBurst] [%d] ERR! NEXUS_Frontend_SetDiseqcSettings() ErrorCode(0x%x), etInput(%d) \n",nRfInputId, etNErrorCode, etInput ));
		nResult = DI_CH_ERR;
	}

	return nResult;
}


int DB45216_ResetDiseqc(int nChannelId)
{
	NEXUS_Error 	etNErrorCode = NEXUS_SUCCESS;
	int 			nRfInputId;

	nRfInputId = GetRfInputId( nChannelId );

	CH_DI_Print(CH_LOG_ARGS, ( "[%d] DB45216_ResetDiseqc [%d]  \n",nChannelId, nRfInputId));

      etNErrorCode = NEXUS_Frontend_ResetDiseqc(s_stFrontend[nRfInputId],0);
	if(etNErrorCode != NEXUS_SUCCESS)
	{
		CH_DI_Print(CH_LOG_DEFAULT, ( "[DB45216_ResetDiseqc] [%d] NEXUS_Frontend_ResetDiseqc() Error Code(0x%x)", nRfInputId, etNErrorCode));
		return DI_CH_ERR;
	}
    
    return DI_CH_OK;
}


int DB45216_SendDiseqcMsg(
	int nChannelId,
	HUINT8 *pucSendBuf,
	HUINT8 ucNumOfBuf,
	CH_SAT_DiseqcInput_e etDiseqcInput
)
{
	NEXUS_Error 	etNErrorCode = NEXUS_SUCCESS;
	NEXUS_CallbackDesc 	callback;
	int i;
	int 			nRfInputId=0, nUnitId=0;

	nRfInputId = GetRfInputId( nChannelId );
	nUnitId = GetDeviceUnitId( nChannelId );

	VK_memset(&callback, 0, sizeof(NEXUS_CallbackDesc));

	callback.callback = DiseqcCompleteCallback;
	//callback.context = s_stFrontend[nRfInputId];
	callback.context = s_stFrontend[nUnitId];

	CH_UNUSED(etDiseqcInput);

#if 0 // BCM4538 : ResetDiseqc is not needed !!!
	etNErrorCode = NEXUS_Frontend_ResetDiseqc(s_stFrontend[nRfInputId],0);
	if(etNErrorCode != NEXUS_SUCCESS)
	{
		CH_DI_Print(0, ( "[DB45216_SendDiseqcCommand] [%d] NEXUS_Frontend_ResetDiseqc() Error Code(0x%x)", nRfInputId, etNErrorCode));
		return DI_CH_ERR;
	}
#endif

#if 0 //TODO_ILEE
	VK_TASK_Sleep(10 );//Delay for Diseqc Msg Initial Reset Timing
	CH_DI_Print(0, ("===== DiseqcMessage ChannelId[%d]:[%d]nUnitId =====\n", nChannelId, nUnitId));
#endif
	CH_DI_Print(CH_LOG_ARGS, ("[%d] ===== DiseqcMessage Length(%d) =====\n", nChannelId, ucNumOfBuf));

	if(show_di_ch > CH_LOG_ARGS)
	{
		for( i=0; i<ucNumOfBuf; i++)
		{
			DI_UART_Print("[%02x] ", pucSendBuf[i]);
		}
	}
	CH_DI_Print(CH_LOG_ARGS, ("\n===== DiseqcMessage End =======\n"));
#if 1
	//etNErrorCode = NEXUS_Frontend_SendDiseqcMessage(s_stFrontend[nRfInputId], pucSendBuf, ucNumOfBuf, &callback);
	etNErrorCode = NEXUS_Frontend_SendDiseqcMessage(s_stFrontend[nUnitId], pucSendBuf, ucNumOfBuf, &callback);
	if(etNErrorCode != NEXUS_SUCCESS)
	{
		CH_DI_Print(CH_LOG_DEFAULT, ( "[DB45216_SendDiseqcCommand] [%d] NEXUS_Frontend_SendDiseqcMessage() Error Code(0x%x)", nRfInputId, etNErrorCode));
		return DI_CH_ERR;
	}
#endif

	return DI_CH_OK;
}



int DB45216_UpdateTunedParam( int nChannelId, CH_SAT_TuneParam_t * pstTuneParam )
{
	int   nResult = DI_CH_OK;
	int 	nUnitId;

	NEXUS_Error 	etNErrorCode = NEXUS_SUCCESS;
	NEXUS_FrontendSatelliteStatus stStatus;
	NEXUS_FrontendSatelliteRuntimeSettings stRuntimeSettings;

	nUnitId = GetDeviceUnitId( nChannelId );

	if( (pstTuneParam->etFecCodeRate == DI_CH_SCODERATE_AUTO)
		|| (pstTuneParam->etModulation == DI_CH_PSK_AUTO) )
	{

		CH_DI_Print(CH_LOG_ARGS, ("[DB45216_UpdateTunedParam] ============================ \n"));

		etNErrorCode = NEXUS_Frontend_GetSatelliteStatus(s_stFrontend[nUnitId], &stStatus);
		if(etNErrorCode == NEXUS_SUCCESS)
		{
			ConvertNexusToDiParam(stStatus.mode, stStatus.codeRate, pstTuneParam );
		}
		else
		{
			CH_DI_Print(CH_LOG_DEFAULT, ("[DB45216_UpdateTunedParam] [%d] ERR! NEXUS_Frontend_GetSatelliteStatus() Error Code(0x%x) \n ", nUnitId, etNErrorCode));
			nResult = DI_CH_ERR;
		}
	}

	etNErrorCode = NEXUS_Frontend_GetSatelliteRuntimeSettings(s_stFrontend[nUnitId], &stRuntimeSettings);
	if(etNErrorCode != NEXUS_SUCCESS)
	{
		CH_DI_Print(CH_LOG_DEFAULT, ( "[DB45216_UpdateTunedParam] [%d] ERR! NEXUS_Frontend_GetSatelliteRuntimeSettings() ErrorCode(0x%x)\n",nUnitId, etNErrorCode ));
	}
	else
	{
		CH_DI_Print(CH_LOG_SIGINFO, ( "[DB45216_UpdateTunedParam] nUnitId[%d] : selectedAdc(%d) \n",nUnitId, stRuntimeSettings.selectedAdc ));
		CH_DI_Print(CH_LOG_SIGINFO, ( "[DB45216_UpdateTunedParam] nUnitId[%d] : externalBert.enabled(%d) \n",nUnitId, stRuntimeSettings.externalBert.enabled ));
		CH_DI_Print(CH_LOG_SIGINFO, ( "[DB45216_UpdateTunedParam] nUnitId[%d] : externalBert.invertClock(%d) \n",nUnitId, stRuntimeSettings.externalBert.invertClock ));

		pstTuneParam->ulSelectedAdc = stRuntimeSettings.selectedAdc;
	}

	return nResult;

}



int DB45216_SetInputMode( int nChannelId, CH_SAT_InputMode_e etInputMode )
{
	int 			nUnitId;
	NEXUS_Error 	etNErrorCode = NEXUS_SUCCESS;
	//NEXUS_7346LnaSettings stLnaSettings;

	nUnitId = GetDeviceUnitId( nChannelId );

	if(etInputMode == DI_CH_INPUT_ONE_SAME)
	{
		//stLnaSettings.out0  = NEXUS_7346LnaOutput_eOut0;
		//stLnaSettings.out1  = NEXUS_7346LnaOutput_eOut0;
		//stLnaSettings.daisy = NEXUS_7346LnaOutput_eNone;
	}
	else if(etInputMode == DI_CH_INPUT_TWO_DIFF)
	{
		//stLnaSettings.out0  = NEXUS_7346LnaOutput_eOut0;
		//stLnaSettings.out1  = NEXUS_7346LnaOutput_eOut1;
		//stLnaSettings.daisy = NEXUS_7346LnaOutput_eNone;
	}
	else
	{
		CH_DI_Print(CH_LOG_DEFAULT, ( "[DB45216_SetInputMode] [%d] ERR! etInputMode(%d) \n",nUnitId, etInputMode ));
		return DI_CH_NOT_SUPPORTED;
	}

	//etNErrorCode = NEXUS_Frontend_Set7346LnaSettings(s_stFrontend[nUnitId], &stLnaSettings);
	if(etNErrorCode != NEXUS_SUCCESS)
	{
		CH_DI_Print(CH_LOG_DEFAULT, ( "[DB45216_SetInputMode] [%d] ERR! NEXUS_Frontend_Set7346LnaSettings() ErrorCode(0x%x), etInputMode(%d) \n",nUnitId, etNErrorCode, etInputMode ));
	}

	return etNErrorCode;
}


int DB45216_SetInputSource( int nChannelId, DI_CH_InputSource_e etInputSource )
{
	int 			nUnitId;
	HUINT32 		ulSelectAdc = 0;
	NEXUS_Error 	etNErrorCode = NEXUS_SUCCESS;
	//NEXUS_7346LnaSettings stLnaSettings;
	NEXUS_FrontendSatelliteRuntimeSettings stRuntimeSettings;

	nUnitId = GetDeviceUnitId( nChannelId );

	if(etInputSource >= DI_CH_RF_INPUTMAX)
	{
		return DI_CH_PARAM_ERR;
	}

	CH_DI_Print(CH_LOG_ARGS, ( "[DB45216_SetInputSource] nUnitId[%d] : etInputSource(%d) \n",nUnitId, etInputSource ));

	if(etInputSource > DI_CH_RF_INPUT2)
	{
		return DI_CH_HW_RESTRICTION;
	}

	ulSelectAdc = (HUINT32)etInputSource;
#if 0
typedef struct NEXUS_FrontendSatelliteRuntimeSettings
{
    unsigned selectedAdc;     /* ADC mapping for this demod */
    struct {
        bool enabled;         /* Enable serial BERT output on this demod */
        bool invertClock;     /* Invert BERT_CLK output */
    } externalBert;
} NEXUS_FrontendSatelliteRuntimeSettings;
#endif
	etNErrorCode = NEXUS_Frontend_GetSatelliteRuntimeSettings(s_stFrontend[nUnitId], &stRuntimeSettings);
	if(etNErrorCode != NEXUS_SUCCESS)
	{
		CH_DI_Print(CH_LOG_DEFAULT, ( "[DB45216_SetInputSource] [%d] ERR! NEXUS_Frontend_GetSatelliteRuntimeSettings() ErrorCode(0x%x), etInputMode(%d) \n",nUnitId, etNErrorCode, etInputSource ));
	}
	else
	{
		CH_DI_Print(CH_LOG_ARGS, ( "[DB45216_SetInputSource] nUnitId[%d] : selectedAdc(%d) \n",nUnitId, stRuntimeSettings.selectedAdc ));
		CH_DI_Print(CH_LOG_ARGS, ( "[DB45216_SetInputSource] nUnitId[%d] : externalBert.enabled(%d) \n",nUnitId, stRuntimeSettings.externalBert.enabled ));
		CH_DI_Print(CH_LOG_ARGS, ( "[DB45216_SetInputSource] nUnitId[%d] : externalBert.invertClock(%d) \n",nUnitId, stRuntimeSettings.externalBert.invertClock ));
	}

	stRuntimeSettings.selectedAdc = ulSelectAdc;

	etNErrorCode = NEXUS_Frontend_SetSatelliteRuntimeSettings(s_stFrontend[nUnitId], &stRuntimeSettings);
	if(etNErrorCode != NEXUS_SUCCESS)
	{
		CH_DI_Print(CH_LOG_DEFAULT, ( "[DB45216_SetInputSource] [%d] ERR! NEXUS_Frontend_SetSatelliteRuntimeSettings() ErrorCode(0x%x), etInputMode(%d) \n",nUnitId, etNErrorCode, etInputSource ));
	}
	else
	{
		SetRfInputId(nChannelId, etInputSource);
	}
#if 0 /* for test */
	etNErrorCode = NEXUS_Frontend_GetSatelliteRuntimeSettings(s_stFrontend[nUnitId], &stRuntimeSettings);
	if(etNErrorCode != NEXUS_SUCCESS)
	{
		CH_DI_Print(0, ( "[DB45216_SetInputSource] [%d] ERR! NEXUS_Frontend_GetSatelliteRuntimeSettings() ErrorCode(0x%x), etInputMode(%d) \n",nUnitId, etNErrorCode, etInputSource ));
	}
	else
	{
		CH_DI_Print(0, ( "[DB45216_SetInputSource] nUnitId[%d] : selectedAdc(%d) \n",nUnitId, stRuntimeSettings.selectedAdc ));
		CH_DI_Print(0, ( "[DB45216_SetInputSource] nUnitId[%d] : externalBert.enabled(%d) \n",nUnitId, stRuntimeSettings.externalBert.enabled ));
		CH_DI_Print(0, ( "[DB45216_SetInputSource] nUnitId[%d] : externalBert.invertClock(%d) \n",nUnitId, stRuntimeSettings.externalBert.invertClock ));
	}
#endif
	return etNErrorCode;
}



HBOOL DB45216_LnbGet22KhzTone( int nChannelId )
{
	int 			nRfInputId, nUnitId;

	nRfInputId = GetRfInputId( nChannelId );
	nUnitId = GetDeviceUnitId( nChannelId );

	CH_DI_Print(CH_LOG_ARGS, ( "[%d] DB45216_LnbGet22KhzTone [%d] [%d] [%d] \n", nChannelId, nRfInputId ));

	//return s_stLnbState[nRfInputId].bToneEnabled;
	return s_stLnbState[nUnitId].bToneEnabled;

}



int DB45216_LnbSet22KhzTone( int nChannelId, HBOOL b22kTone )
{
	int nResult = DI_CH_OK;
	int nRfInputId, nUnitId;
	NEXUS_Error 	etNErrorCode = NEXUS_SUCCESS;

	nRfInputId = GetRfInputId( nChannelId );
	nUnitId = GetDeviceUnitId( nChannelId );

	NEXUS_FrontendDiseqcSettings diseqcSettings;

	CH_DI_Print(CH_LOG_ARGS, ( "[%d] DB45216_LnbSet22KhzTone [%d] [%d] [%d] \n", nChannelId, nRfInputId, b22kTone ));

	//NEXUS_Frontend_GetDiseqcSettings(s_stFrontend[nRfInputId], &diseqcSettings);
	NEXUS_Frontend_GetDiseqcSettings(s_stFrontend[nUnitId], &diseqcSettings);

	diseqcSettings.toneMode = NEXUS_FrontendDiseqcToneMode_eEnvelope;
	diseqcSettings.toneEnabled = (bool)b22kTone;
	//CH_DI_Print( "s_stFrontend (0x%x), frontend(0x%x) \n", s_stFrontend, frontend );

	//etNErrorCode = NEXUS_Frontend_SetDiseqcSettings(s_stFrontend[nRfInputId], &diseqcSettings);
	etNErrorCode = NEXUS_Frontend_SetDiseqcSettings(s_stFrontend[nUnitId], &diseqcSettings);
	if(etNErrorCode != NEXUS_SUCCESS)
	{
		CH_DI_Print(CH_LOG_DEFAULT, ( "[DB45216_LnbSet22KhzTone] [%d] ERR! NEXUS_Frontend_SetDiseqcSettings() ErrorCode(0x%x), bDiseqcTone(%d) \n",nRfInputId, etNErrorCode, diseqcSettings.toneEnabled ));
		return DI_CH_ERR;
	}

	//s_stLnbState[nRfInputId].bToneEnabled = b22kTone;
	s_stLnbState[nUnitId].bToneEnabled = b22kTone;

	return nResult;
} // end of drv_channel_LNB22KHzTone

int DB45216_GetDeviceStatus ( int nChannelId, DI_CH_DeviceStatus *pstDeviceStatus )
{
	int nUnitId = 0;
	NEXUS_Error nRet = NEXUS_SUCCESS;
	NEXUS_FrontendDeviceStatus stDevStatus;
	NEXUS_FrontendDeviceHandle deviceHandle = NULL;

	nUnitId = GetDeviceUnitId( nChannelId );

	deviceHandle = NEXUS_Frontend_GetDevice(s_stFrontend[nUnitId]);
	if(deviceHandle == NULL)
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("Error : NEXUS_Frontend_GetDevice() deviceHandle is null... \n"));
		return DI_CH_ERR;
	}

	nRet = NEXUS_FrontendDevice_GetStatus(deviceHandle, &stDevStatus);
	if(nRet != NEXUS_SUCCESS)
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("Error : NEXUS_FrontendDevice_GetStatus() nRet =[0x%x] \n",nRet));
		return DI_CH_ERR;
	}

	CH_DI_Print(CH_LOG_SIGINFO, (">>>>>>>>>>>> DB45216_GetDeviceStatus !!!!!\n"));
	CH_DI_Print(CH_LOG_SIGINFO, ("avs.enabled : [%d] \n",stDevStatus.avs.enabled));
	CH_DI_Print(CH_LOG_SIGINFO, ("avs.voltage : [%d] \n",stDevStatus.avs.voltage));
	CH_DI_Print(CH_LOG_SIGINFO, ("temperature : [%d] \n",stDevStatus.temperature));
	CH_DI_Print(CH_LOG_SIGINFO, (">>>>>>>>>>>> DB45216_GetDeviceStatus !!!!!\n"));
	pstDeviceStatus->avs.enabled = stDevStatus.avs.enabled;
	pstDeviceStatus->avs.voltage = stDevStatus.avs.voltage;
	pstDeviceStatus->temperature = stDevStatus.temperature;

	return DI_CH_OK;

}



/*********************** End of File ******************************/

