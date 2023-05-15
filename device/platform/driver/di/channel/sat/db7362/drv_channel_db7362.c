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

#include "drv_channel_db7362.h"

/* nexus headers */
#include "nexus_frontend.h"
#include "nexus_platform.h"

// End Including Headers


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
// Start #define
#define MIN_WAIT_DELAY			(30)
#define RS_STUFF_BYTE			(16) // Normal
#define ODU_SETUP_TIME			(300) /* Outdoor Unit setup time 100ms */

//#define CH_PRINT_LOW_LEVEL_ZAPPING_TIME

//#define DEBUG_SSI_SQI
#if defined(CONFIG_SUPPORT_ADVANCED_SIGNALINFO)
#define HDI_SYM_RATE_UNITS          10
/* BER bitrate window. Multiple our instantaneous data bitrate by this value
   to come up with a bit window.
*/
#define BCMFRONT_BER_BIT_WINDOW     2	/* To accumulate 2 Window, we need 1 second*/
#define BCMFRONT_WINDOWCOUNT_BER 	10000000 /*For LDPC, this is error count*/

#define CONST_DVBS_KFEC			(0.35)
#define CONST_DVBS2_KFEC		(0.15)

#define CONST_KQPSK				(3)
#define CONST_K8PSK				(4.77)
#endif
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

#if defined(CONFIG_SUPPORT_ADVANCED_SIGNALINFO)
typedef struct {
    uint16_t snrDb10times;
    uint32_t errorCount;
} BCMFRONT_BER_TRANSFORM;

const BCMFRONT_BER_TRANSFORM gBerTransform[] =
{
	{ 61,216960}, { 62,205110}, { 63,193686}, { 64,182684}, { 65,172101}, { 66,161933}, { 67,152176}, { 68,142823}, { 69,133870}, { 70,125311},
	{ 71,117139}, { 72,109347}, { 73,101928}, { 74, 94873}, { 75, 88174}, { 76, 81824}, { 77, 75812}, { 78, 70130}, { 79, 64768}, { 80, 59716},
	{ 81, 54964}, { 82, 50502}, { 83, 46320}, { 84, 42406}, { 85, 38750}, { 86, 35342}, { 87, 32171}, { 88, 29226}, { 89, 26496}, { 90, 23971},
	{ 91, 21640}, { 92, 19493}, { 93, 17520}, { 94, 15711}, { 95, 14055}, { 96, 12544}, { 97, 11168}, { 98,  9918}, { 99,  8786}, {100,  7762},
	{101,  6839}, {102,  6010}, {103,  5266}, {104,  4601}, {105,  4008}, {106,  3481}, {107,  3014}, {108,  2602}, {109,  2239}, {110,  1920},
	{111,  1641}, {112,  1398}, {113,  1186}, {114,  1003}, {115,   845}, {116,   710}, {117,   593}, {118,   494}, {119,   410}, {120,   339},
	{121,   279}, {122,   228}, {123,   186}, {124,   151}, {125,   122}, {126,    98}, {127,    79}, {128,    63}, {129,    50}, {130,    39},
	{131,    31}, {132,    24}, {133,    19}, {134,    14}, {135,    11}, {136,     8}, {137,     6}, {138,     5}, {139,     4}, {140,     3},
	{141,     2}, {142,     1}, {143,     1}, {144,     1}, {145,     1}, {146,     0}, {147,     0}, {148,     0}, {149,     0}, {150,     0}
};
#endif

// End global variable


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
// Start static variablee

static NEXUS_FrontendHandle s_stFrontend[CONFIG_NUM_OF_SAT_CHANNEL];

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

static CH_SAT_LnbStatus_t	s_stLnbState[CONFIG_NUM_OF_SAT_CHANNEL];

//static HUINT8 ucStuffByte = RS_STUFF_BYTE;

#ifdef CH_PRINT_LOW_LEVEL_ZAPPING_TIME
HUINT32 g_ulRequestTune[CONFIG_NUM_OF_SAT_CHANNEL];
HUINT32 g_ulLowLevelLockedTime[CONFIG_NUM_OF_SAT_CHANNEL] , g_ulLowLevelUnlockedTime[CONFIG_NUM_OF_SAT_CHANNEL] ;
#endif


/****************** Function 시작점에 필히 기록 ********************/
/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
// Function Name:
// Function Description:
// Parameter:
// Return Value:

#define __DB7346_PRIVATE_FUNCITONS__

/*********************** Static Function ***********************/


static HUINT32 GetFreqSearchRange( HUINT32 ulSymbolRate )
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

	if(( ulSymbolRate >= 1000) && (ulSymbolRate <= 4000))
	{
		ulFreqSearchrange = 3800000;
	}
	else if(( ulSymbolRate > 4000) && (ulSymbolRate <= 8000))
	{
		ulFreqSearchrange = 4000000;
	}
	else if(( ulSymbolRate > 8000) && (ulSymbolRate <= 11000))
	{
		ulFreqSearchrange = 5000000;
	}
	else if(( ulSymbolRate > 11000) && (ulSymbolRate <= 18000))
	{
		ulFreqSearchrange = 6000000;
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
		ulFreqSearchrange = 8000000;
	}
	else if(( ulSymbolRate > 20000) && (ulSymbolRate <= 45000))
	{
		ulFreqSearchrange = 10000000;
	}
	else
	{
		ulFreqSearchrange = 10000000;
	}
#endif

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
		CH_DI_Print(CH_LOG_ARGS, ("[DB7346_NexusUpdateInfo] %x : DVBS, QPSK \n", etSatMode));
	}
	else if(etSatMode == NEXUS_FrontendSatelliteMode_eQpskLdpc)
	{
		pstTuneParam->etTransSpec = DI_CH_TRANS_DVBS2;
		pstTuneParam->etModulation	= DI_CH_PSK_QPSK;
		CH_DI_Print(CH_LOG_ARGS, ("[DB7346_NexusUpdateInfo] %x : DVBS2, QPSK \n", etSatMode));
	}
	else if(etSatMode == NEXUS_FrontendSatelliteMode_e8pskLdpc)
	{
		pstTuneParam->etTransSpec = DI_CH_TRANS_DVBS2;
		pstTuneParam->etModulation	= DI_CH_PSK_8PSK;
		CH_DI_Print(CH_LOG_ARGS, ("[DB7346_NexusUpdateInfo] %x : DVBS2, 8PSK \n", etSatMode));
	}
	else
	{
		pstTuneParam->etModulation	= DI_CH_PSK_AUTO;
	}

	CH_DI_Print(CH_LOG_ARGS, ("[DB7346_NexusUpdateInfo] etTransSpec [%d] , etModulation [%d]  \n", pstTuneParam->etTransSpec, pstTuneParam->etModulation ));

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

	pstNexusTuneParam->searchRange = GetFreqSearchRange(pstSatTuneParam->ulSymbolRate);

	pstNexusTuneParam ->ldpcPilotScan = true;
	pstNexusTuneParam ->ldpcPilot = false;
	pstNexusTuneParam ->ldpcPilotPll = false;

	pstNexusTuneParam->carrierOffset = 0;

	return;
}



static HUINT32 ConvertStrengthLevel(
	NEXUS_FrontendSatelliteMode etSatMode,
	HUINT32 ulIfAgc,
	HUINT32 ulAgf
)
{
	HUINT32 ulStrength = 0;

	//CH_DI_Print(4, ("[ConvertStrengthLevel] Input_power: %d \n", nInputPower));

	if( etSatMode == NEXUS_FrontendSatelliteMode_eDvb )
	{
		if( ulIfAgc < 0xffffff00 )
		{
			if(ulIfAgc <=       0x34fff000)  // < -54dBm
				ulStrength = 100;
			else if( ulIfAgc <= 0x49000000)  // -58dBm
				ulStrength = 90;
			else if( ulIfAgc <= 0x6c000000)  // -62dBm
				ulStrength = 85;
			else if( ulIfAgc <= 0x8d000000)  // -66dBm
				ulStrength = 80;
			else if( ulIfAgc <= 0xa1000000)  // -68dBm
				ulStrength = 75;
			else if( ulIfAgc <= 0xb1000000)  // -70dBm
				ulStrength = 70;
			else if( ulIfAgc <= 0xc1000000)  // -72dBm
				ulStrength = 60;
			else if( ulIfAgc <= 0xd6000000)  // -74dBm
				ulStrength = 50;
			else //if( ulIfAgc <= 0xffffff00)  // -76dBm
				ulStrength = 40;

		}
		else
		{
			if( ulAgf <=      0x0c500000)  // -76dBm
				ulStrength = 40;
			else if( ulAgf <= 0x0f500000)  // -78dBm
				ulStrength = 30;
			else if( ulAgf <= 0x15000000)  // -80dBm
				ulStrength = 20;
			else                           // -82dBm
				ulStrength = 10;
		}
	}
	else
	{
		if( ulIfAgc < 0xffffff00 )
		{
			if(ulIfAgc <=       0x34fff000)  // < -35dBm
				ulStrength = 100;
			else if( ulIfAgc <= 0x55000000)  // -40dBm
				ulStrength = 90;
			else if( ulIfAgc <= 0x75000000)  // -45dBm
				ulStrength = 85;
			else if( ulIfAgc <= 0x99000000)  // -50dBm
				ulStrength = 80;
			else if( ulIfAgc <= 0xa9000000)  // -55dBm
				ulStrength = 75;
			else if( ulIfAgc <= 0xbb000000)  // -60dBm
				ulStrength = 70;
			else if( ulIfAgc <= 0xcd000000)  // -65dBm
				ulStrength = 60;
			else                             // -70dBm
				ulStrength = 50;

		}
		else
		{
			if( ulAgf <=      0x0f500000)  // -80dBm
				ulStrength = 50;
			else if( ulAgf <= 0x14000000)  // -85dBm
				ulStrength = 40;
			else if( ulAgf <= 0x18000000)  // -85dBm
				ulStrength = 30;
			else if( ulAgf <= 0x1dd00000)  // -85dBm
				ulStrength = 20;
			else
				ulStrength = 10;
		}
	}

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
		if( ulInSnr > 1245)	//9.8 dB
			ulQuality = 100;
		else if( ulInSnr > 1150)	//8.8dB
			ulQuality = 90;
		else if( ulInSnr > 1047)	//7.8dB
			ulQuality = 80;
		else if( ulInSnr > 962)	//6.8dB
			ulQuality = 70;
		else if( ulInSnr > 860)	//5.8dB
			ulQuality = 60;
		else if( ulInSnr > 765)	//4.8dB
			ulQuality = 50;
		else if( ulInSnr > 665)	//3.8dB
			ulQuality = 40;
		else if( ulInSnr > 602)	//3.4dB
			ulQuality = 30;
		else if( ulInSnr > 550) //2.8dB
			ulQuality = 20;
		else
			ulQuality = 10;
	}
	else
	{
#if 0
		if( ulInSnr > 1430)	//9.8 dB
			ulQuality = 100;
		else if( ulInSnr > 1327)	//8.8dB
			ulQuality = 90;
		else if( ulInSnr > 1225)	//7.8dB
			ulQuality = 80;
		else if( ulInSnr > 1130)	//6.8dB
			ulQuality = 70;
		else if( ulInSnr > 1035)	//5.8dB
			ulQuality = 60;
		else if( ulInSnr > 880)	//4.8dB
			ulQuality = 50;
		else if( ulInSnr > 835)	//3.8dB
			ulQuality = 40;
		else if( ulInSnr > 750)	//3.4dB
			ulQuality = 30;
		else if( ulInSnr > 735) //2.8dB
			ulQuality = 20;
		else
			ulQuality = 10;
#else /* @1450MHz/43200Ksym/8PSK */
		if( ulInSnr > 1290)	//12 dB
			ulQuality = 100;
		else if( ulInSnr > 1180)	//11dB
			ulQuality = 90;
		else if( ulInSnr > 1100)	//10dB
			ulQuality = 80;
		else if( ulInSnr > 1000)	//9dB
			ulQuality = 70;
		else if( ulInSnr > 900)	//8dB
			ulQuality = 60;
		else if( ulInSnr > 860)	//7.5dB
			ulQuality = 50;
		else if( ulInSnr > 820)	//7dB
			ulQuality = 40;
		else if( ulInSnr > 750)	//6.5dB
			ulQuality = 30;
		else if( ulInSnr > 720) //6dB
			ulQuality = 20;
		else
			ulQuality = 10;
#endif
	}

	CH_DI_Print(CH_LOG_ARGS, ("[ConvertQualityLevel] => ulQuality: (%d)	\n", ulQuality ));

	return ulQuality;

}

#if defined(CONFIG_SUPPORT_ADVANCED_SIGNALINFO)
static HUINT32 BerGetErrorCnt(HUINT32 snrDb100times, HUINT32 uCountWindow)
{
	uint16_t i;
	HUINT32	errorCount = 0;
	uint16_t snrDb10times;

	snrDb10times = ((snrDb100times + 5) / 10);
	if(snrDb10times <= 60)
	{
		errorCount = uCountWindow;
	}
	else if(snrDb10times >= 61 && snrDb10times <= 150)
	{
		for(i = 0; i < 90; i++)
		{
			if(gBerTransform[i].snrDb10times == snrDb10times)
			{
				errorCount = gBerTransform[i].errorCount;
				break;
			}
		}
	}
	else if(snrDb10times >= 151 && snrDb10times <= 160)
	{
		errorCount = 0;
	}
	else
	{
		errorCount = 0;
	}

	return errorCount;
}


static float CalculateBer(
	NEXUS_FrontendSatelliteMode etSatMode,
	HUINT32 ulSymbolrate,
	HUINT32 ulSnrDb100Times
)
{
	HUINT32 ulCountWindow = 0, ulBitErros = 0;
	float fBer = 0.0;

	if(etSatMode == NEXUS_FrontendSatelliteMode_eDvb)
	{
		ulCountWindow = ulSymbolrate * HDI_SYM_RATE_UNITS * BCMFRONT_BER_BIT_WINDOW;
	}
	else
	{
		ulCountWindow =  BCMFRONT_WINDOWCOUNT_BER;
	}

	ulBitErros = BerGetErrorCnt(ulSnrDb100Times, ulCountWindow);

	if(ulCountWindow!=0)
	{
		fBer = ulBitErros/(float)ulCountWindow;
	}

	CH_DI_Print(CH_LOG_SIGINFO, ("[CalculateBer] => fBer: (%3.2e)  \n", fBer ));

	return fBer;
}



static float CalculateEbNo(
	NEXUS_FrontendSatelliteMode etSatMode,
	NEXUS_FrontendSatelliteCodeRate etCodeRate,
	HUINT32 ulSnr )
{
	float fKp = 0.0;
	float fEbNo = 0.0;
	float fSnr = 0.0;

	fSnr = ulSnr/100.0;

	if(etSatMode == NEXUS_FrontendSatelliteMode_eDvb)
	{
		if(etCodeRate.numerator == 1 && etCodeRate.denominator == 2)
			fKp = 3.0;
		else if(etCodeRate.numerator == 2 && etCodeRate.denominator == 3)
			fKp = 1.76;
		else if(etCodeRate.numerator == 3 && etCodeRate.denominator == 4)
			fKp = 1.24;
		else if(etCodeRate.numerator == 5 && etCodeRate.denominator == 6)
			fKp = 0.79;
		else if(etCodeRate.numerator == 7 && etCodeRate.denominator == 8)
			fKp = 0.58;
		else
		{
			CH_DI_Print(CH_LOG_DEFAULT, ("[CalculateEbNo] ERR! Dvb Unknown code rate..."));
		}

		fEbNo = fSnr + CONST_DVBS_KFEC - CONST_KQPSK + fKp;
	}
	else if(etSatMode == NEXUS_FrontendSatelliteMode_eQpskLdpc)
	{
		if(etCodeRate.numerator == 1 && etCodeRate.denominator == 2)
			fKp = 3.0;
		else if(etCodeRate.numerator == 3 && etCodeRate.denominator == 5)
			fKp = 2.21;
		else if(etCodeRate.numerator == 2 && etCodeRate.denominator == 3)
			fKp = 1.76;
		else if(etCodeRate.numerator == 3 && etCodeRate.denominator == 4)
			fKp = 1.24;
		else if(etCodeRate.numerator == 4 && etCodeRate.denominator == 5)
			fKp = 0.97;
		else if(etCodeRate.numerator == 5 && etCodeRate.denominator == 6)
			fKp = 0.79;
		else if(etCodeRate.numerator == 8 && etCodeRate.denominator == 9)
			fKp = 0.51;
		else if(etCodeRate.numerator == 9 && etCodeRate.denominator == 10)
			fKp = 0.46;
		else
		{
			CH_DI_Print(CH_LOG_DEFAULT, ("[CalculateEbNo] ERR! QpskLdpc Unknown code rate..."));
		}

		fEbNo = fSnr + CONST_DVBS2_KFEC - CONST_KQPSK + fKp;

	}
	else if(etSatMode == NEXUS_FrontendSatelliteMode_e8pskLdpc)
	{
		if(etCodeRate.numerator == 3 && etCodeRate.denominator == 5)
			fKp = 2.21;
		else if(etCodeRate.numerator == 2 && etCodeRate.denominator == 3)
			fKp = 1.76;
		else if(etCodeRate.numerator == 3 && etCodeRate.denominator == 4)
			fKp = 1.24;
		else if(etCodeRate.numerator == 5 && etCodeRate.denominator == 6)
			fKp = 0.79;
		else if(etCodeRate.numerator == 8 && etCodeRate.denominator == 9)
			fKp = 0.51;
		else if(etCodeRate.numerator == 9 && etCodeRate.denominator == 10)
			fKp = 0.46;
		else
		{
			CH_DI_Print(CH_LOG_DEFAULT, ("[CalculateEbNo] ERR! 8pskLdpc Unknown code rate..."));
		}

		fEbNo = fSnr + CONST_DVBS2_KFEC - CONST_K8PSK + fKp;

	}
	else
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CalculateEbNo] ERR! Unknown mode..."));
	}

	CH_DI_Print(CH_LOG_SIGINFO, (" fEbNo [%f] \n",  fEbNo));

	return fEbNo;
}
#endif

static int GetSignalStatus( int nUnitId, DI_CH_SignalQuality_t *pstSignalInfo )
{
	NEXUS_Error 	etNErrorCode = NEXUS_SUCCESS;
	NEXUS_FrontendSatelliteStatus stFrontendStatus;
#if defined(CONFIG_SUPPORT_ADVANCED_SIGNALINFO)
	float fBer = 0.0, fEbNo = 0.0;
#endif
	HUINT32 ulStrength = 0, ulQuality = 0;

	etNErrorCode = NEXUS_Frontend_GetSatelliteStatus(s_stFrontend[nUnitId], &stFrontendStatus);
	if(etNErrorCode != NEXUS_SUCCESS)
	{
		CH_DI_Print(CH_LOG_DEFAULT, ( "[GetSignalStatus] [%d] ERR! NEXUS_Frontend_GetSatelliteStatus (0x%x)",nUnitId, etNErrorCode ));
		return DI_CH_ERR;
	}

	CH_DI_Print(CH_LOG_SIGINFO, ("----------------------------------------------------------------------------------------\n"));
	CH_DI_Print(CH_LOG_SIGINFO, ("frequency            =(%u)Hz\n", stFrontendStatus.frequency                        ));
	CH_DI_Print(CH_LOG_SIGINFO, ("demodLocked          =(%d)\n", stFrontendStatus.demodLocked                      ));
	CH_DI_Print(CH_LOG_SIGINFO, ("bertLocked           =(%d)\n", stFrontendStatus.bertLocked                       ));
	CH_DI_Print(CH_LOG_SIGINFO, ("symbolRate           =(%u)\n", stFrontendStatus.symbolRate                       ));
	CH_DI_Print(CH_LOG_SIGINFO, ("symbolRateError      =(%d)\n", stFrontendStatus.symbolRateError                  ));
	CH_DI_Print(CH_LOG_SIGINFO, ("carrierOffset        =(%d)\n", stFrontendStatus.carrierOffset                    ));
	//CH_DI_Print(CH_LOG_SIGINFO, ("carrierError         =(%d)\n", stFrontendStatus.carrierError                     ));
	CH_DI_Print(CH_LOG_SIGINFO, ("outputBitRate        =(%u)\n", stFrontendStatus.outputBitRate                    ));
	CH_DI_Print(CH_LOG_SIGINFO, ("ifAgcLevel           =(%u)\n", stFrontendStatus.ifAgcLevel                       ));
	CH_DI_Print(CH_LOG_SIGINFO, ("rfAgcLevel           =(%u)\n", stFrontendStatus.rfAgcLevel                       ));
	CH_DI_Print(CH_LOG_SIGINFO, ("intAgcLevel          =(%u)\n", stFrontendStatus.intAgcLevel                      ));
	CH_DI_Print(CH_LOG_SIGINFO, ("snrEstimate          =(%u)(1/100 dB)\n", stFrontendStatus.snrEstimate                      ));
	CH_DI_Print(CH_LOG_SIGINFO, ("berEstimate          =(%u)\n", stFrontendStatus.berEstimate                      ));
	CH_DI_Print(CH_LOG_SIGINFO, ("fecCorrected         =(%u)\n", stFrontendStatus.fecCorrected                     ));
	CH_DI_Print(CH_LOG_SIGINFO, ("fecUncorrected       =(%u)\n", stFrontendStatus.fecUncorrected                   ));
	CH_DI_Print(CH_LOG_SIGINFO, ("fecClean             =(%u)\n", stFrontendStatus.fecClean                         ));
	CH_DI_Print(CH_LOG_SIGINFO, ("bitErrCorrected      =(%u)\n", stFrontendStatus.bitErrCorrected                  ));
	CH_DI_Print(CH_LOG_SIGINFO, ("reacquireCount       =(%u)\n", stFrontendStatus.reacquireCount                   ));
#if NEXUS_VERSION >= 1290
	CH_DI_Print(CH_LOG_SIGINFO, ("berErrors            =(%u)\n", stFrontendStatus.berErrorCount                    ));
#else
	CH_DI_Print(CH_LOG_SIGINFO, ("berErrors            =(%u)\n", stFrontendStatus.berErrors                        ));
#endif
	CH_DI_Print(CH_LOG_SIGINFO, ("preViterbiErrorCount =(%u)\n", stFrontendStatus.preViterbiErrorCount             ));
	CH_DI_Print(CH_LOG_SIGINFO, ("mpegErrors           =(%u)\n", stFrontendStatus.mpegErrors                       ));
	CH_DI_Print(CH_LOG_SIGINFO, ("mpegCount            =(%u)\n", stFrontendStatus.mpegCount                        ));
	CH_DI_Print(CH_LOG_SIGINFO, ("----------------------------------------------------------------------------------------\n"));
	CH_DI_Print(CH_LOG_SIGINFO, ("ifAgc                =(0x%x):(%u)\n", stFrontendStatus.ifAgc, stFrontendStatus.ifAgc ));
	CH_DI_Print(CH_LOG_SIGINFO, ("rfAgc                =(0x%x):(%u)\n", stFrontendStatus.rfAgc, stFrontendStatus.rfAgc ));
	CH_DI_Print(CH_LOG_SIGINFO, ("agf                  =(0x%x):(%d)\n", stFrontendStatus.agf, stFrontendStatus.agf     ));
	CH_DI_Print(CH_LOG_SIGINFO, ("----------------------------------------------------------------------------------------\n"));
#if 0
	CH_DI_Print(CH_LOG_SIGINFO, ("agc0                 =(0x%x):(%u)\n", stFrontendStatus.lnaStatus.agc0, stFrontendStatus.lnaStatus.agc0 ));
	CH_DI_Print(CH_LOG_SIGINFO, ("agc1                 =(0x%x):(%u)\n", stFrontendStatus.lnaStatus.agc1, stFrontendStatus.lnaStatus.agc1 ));
	CH_DI_Print(CH_LOG_SIGINFO, ("intConfig            =(0x%x):(%u)\n", stFrontendStatus.lnaStatus.intConfig, stFrontendStatus.lnaStatus.intConfig));
	CH_DI_Print(CH_LOG_SIGINFO, ("extConfig            =(0x%x):(%u)\n", stFrontendStatus.lnaStatus.extConfig, stFrontendStatus.lnaStatus.extConfig));
	CH_DI_Print(CH_LOG_SIGINFO, ("version              =(0x%x):(%u)\n", stFrontendStatus.lnaStatus.version, stFrontendStatus.lnaStatus.version));
	CH_DI_Print(CH_LOG_SIGINFO, ("----------------------------------------------------------------------------------------\n"));
#endif
	CH_DI_Print(CH_LOG_SIGINFO, ("chipId               =(%x)\n", stFrontendStatus.version.chipId                   ));
	CH_DI_Print(CH_LOG_SIGINFO, ("chipVersion          =(%x)\n", stFrontendStatus.version.chipVersion              ));
	CH_DI_Print(CH_LOG_SIGINFO, ("bondingOption        =(%x)\n", stFrontendStatus.version.bondingOption            ));
	CH_DI_Print(CH_LOG_SIGINFO, ("MajVer               =(%x)\n", stFrontendStatus.version.apMicrocodeVersion       ));
	CH_DI_Print(CH_LOG_SIGINFO, ("MinVer               =(%x)\n", stFrontendStatus.version.hostConfigurationVersion ));
	CH_DI_Print(CH_LOG_SIGINFO, ("----------------------------------------------------------------------------------------\n"));

	ulStrength = ConvertStrengthLevel(stFrontendStatus.mode,stFrontendStatus.ifAgc,(HUINT32) stFrontendStatus.agf);


	if( stFrontendStatus.demodLocked == true)
	{
		ulQuality = ConvertQualityLevel(stFrontendStatus.mode, stFrontendStatus.snrEstimate);
#if defined(CONFIG_SUPPORT_ADVANCED_SIGNALINFO)
		fBer  = CalculateBer(stFrontendStatus.mode,stFrontendStatus.symbolRate,stFrontendStatus.snrEstimate);
		fEbNo = CalculateEbNo(stFrontendStatus.mode,stFrontendStatus.codeRate,stFrontendStatus.snrEstimate);
#endif
		pstSignalInfo->fSnr = ((float)(stFrontendStatus.snrEstimate))/100;
		pstSignalInfo->ulAgc = 0;
		pstSignalInfo->ulRfAgc = (HUINT32)stFrontendStatus.rfAgc;
		pstSignalInfo->ulIfAgc = (HUINT32)stFrontendStatus.ifAgc;
		pstSignalInfo->ulUnCorrectedNo = stFrontendStatus.fecUncorrected;
		pstSignalInfo->fSignalInputPower = 0;
		pstSignalInfo->ulQuality = ulQuality;
		pstSignalInfo->ulStrength = ulStrength;
#if defined(CONFIG_SUPPORT_ADVANCED_SIGNALINFO)
		pstSignalInfo->fBer = fBer;
		pstSignalInfo->fEbNo = fEbNo;
#else
		pstSignalInfo->fBer = 0;
		pstSignalInfo->fEbNo = 0;
#endif
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
		pstSignalInfo->ulQuality = 0;
		pstSignalInfo->ulStrength = ulStrength;
		pstSignalInfo->fBer = 0;
		pstSignalInfo->fEbNo = 0;

		CH_DI_Print(CH_LOG_SIGINFO, ( "[GetSignalStatus] [%d] Demod UnLocked \n", nUnitId));
	}

#if defined(CONFIG_SUPPORT_ADVANCED_SIGNALINFO)
	CH_DI_Print(CH_LOG_SIGINFO, ("----------------------------------------------------------------------------------------\n"));
	CH_DI_Print(CH_LOG_SIGINFO, ("pstSignalInfo->fSnr      =(%f)\n", pstSignalInfo->fSnr));
	CH_DI_Print(CH_LOG_SIGINFO, ("pstSignalInfo->fBer      =(%3.2e)\n", pstSignalInfo->fBer));
	CH_DI_Print(CH_LOG_SIGINFO, ("pstSignalInfo->fEbNo     =(%f)\n", pstSignalInfo->fEbNo));
	CH_DI_Print(CH_LOG_SIGINFO, ("----------------------------------------------------------------------------------------\n"));
#endif
	return DI_CH_OK;
}


static void DiseqcCompleteCallback(void *data, int unused)
{
	BSTD_UNUSED(data);
	BSTD_UNUSED(unused);
}



#define __DB7346_PUBLIC_FUNCITONS__

/*********************** External Function ***********************/
void DRV_S_InstallApi( void )
{
	NIM_InitDevice 			= &DB7346_InitDevice;
	NIM_SetTune 			= &DB7346_SetTune;
	NIM_CheckTuneState 		= &DB7346_CheckTuneState;
	NIM_CheckLock 			= &DB7346_CheckLock;
	NIM_GetSignalInfo 		= &DB7346_GetSignalInfo;
	NIM_SetPowerState 		= &DB7346_SetPowerState;
	NIM_EnableTsOutput 		= &DB7346_EnableTsOutput;
	NIM_DisableTsOutput 	= &DB7346_DisableTsOutput;
	NIM_SendDiseqcMsg 		= &DB7346_SendDiseqcMsg;
	NIM_ReceiveDiseqcMsg 		= &DB7346_ReceiveDiseqcMsg;
	NIM_UpdateTunedParam	= &DB7346_UpdateTunedParam;
	NIM_SetInputMode		= &DB7346_SetInputMode;
	NIM_SetInputSource		= &DB7346_SetInputSource;
	NIM_SetToneBurst		= &DB7346_SetToneBurst;
	NIM_ResetDiseqc             = &DB7346_ResetDiseqc;
	LNB_Get22KhzTone		= &DB7346_LnbGet22KhzTone;
	LNB_Set22KhzTone		= &DB7346_LnbSet22KhzTone;

	LNB_InitDevice 			= &DRV_CH_LNB_Init;
	LNB_SetOutput			= &DRV_CH_LNB_SetOutput;
	LNB_SetPolarisation 	= &DRV_CH_LNB_SetPolar;
	LNB_CheckAntState		= &DRV_CH_LNB_CheckAntState;
	return;
}



int DB7346_InitDevice( int nChannelId )
{
	int				 nUnitId;
	NEXUS_Error 	etNErrorCode = NEXUS_SUCCESS;
	NEXUS_PlatformConfiguration platformConfig;

	nUnitId = GetDeviceUnitId( nChannelId );

	NEXUS_Platform_GetConfiguration(&platformConfig);

	s_stFrontend[nUnitId] = platformConfig.frontend[nUnitId];

	if( NULL == s_stFrontend[nUnitId] )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DB7346_InitDevice] ERR! Unable to find Satellite-capable frontend\n"));
		return DI_CH_ERR;
	}

	/* PKT_CLK runs continuously.*//* false=CLK runs continuously, true=CLK is suppressed when VALID not active */
	etNErrorCode = NEXUS_Frontend_SetXportCtrl(s_stFrontend[nUnitId], FALSE);
	if(etNErrorCode != NEXUS_SUCCESS)
	{
		CH_DI_Print(CH_LOG_DEFAULT, ( "[DB7346_InitDevice] [%d] ERR! NEXUS_Frontend_SetXportCtrl() Error Code(0x%x)",nUnitId, etNErrorCode ));
		return DI_CH_ERR;
	}

	return DI_CH_OK;
}



int DB7346_SetTune( int nChannelId, CH_SAT_TuneParam_t *pstSatTuneParam )
{
	NEXUS_Error 	etNErrorCode = NEXUS_SUCCESS;
	NEXUS_FrontendSatelliteSettings satSettings;
	int nUnitId = 0;

	nUnitId = GetDeviceUnitId( nChannelId );

	/* Reset(Untune) Before Settune  */
	NEXUS_Frontend_Untune(s_stFrontend[nUnitId]);

	//CH_DI_Print("4, (nUnitId(%d), frontend(%x) \n", nUnitId , frontend));

	NEXUS_Frontend_GetDefaultSatelliteSettings(&satSettings);

	ConvertDiToNexusParam( pstSatTuneParam, &satSettings);

	#if 0 // TODO:
	/* RS Stuff byte*/
	etNErrorCode = NEXUS_Frontend_SetStuffBytes(s_stFrontend[nUnitId], &ucStuffByte);// (16byte)
	if(etNErrorCode != NEXUS_SUCCESS)
	{
		CH_DI_Print(0, ( "[DB7346_SetTune] [%d] ERR! NEXUS_Frontend_SetStuffBytes() Error Code(0x%x)",nUnitId, etNErrorCode ));
		return DI_CH_ERR;
	}
	#endif

	CH_DI_Print(CH_LOG_TUNE, ("ODU_SETUP_TIME (%d) \n", ODU_SETUP_TIME));

	VK_TASK_Sleep( ODU_SETUP_TIME );

	//satSettings.lockCallback.callback = lock_callback;
	//satSettings.lockCallback.context = frontend;

	CH_DI_Print(CH_LOG_TUNE, ("\n ############	Tuning NEXUS_Frontend_Satellite ############ \n"));

	#ifdef CH_PRINT_LOW_LEVEL_ZAPPING_TIME
	g_ulRequestTune[nUnitId] = VK_TIMER_GetSystemTick();
	#endif
	etNErrorCode = NEXUS_Frontend_TuneSatellite(s_stFrontend[nUnitId], &satSettings);
	if(etNErrorCode != NEXUS_SUCCESS)
	{
		CH_DI_Print(CH_LOG_DEFAULT, ( "[DB7346_SetTune] [%d] ERR! NEXUS_Frontend_TuneSatellite() Error Code(0x%x)",nUnitId, etNErrorCode ));
		return DI_CH_ERR;
	}

	return DI_CH_OK;
}



int DB7346_CheckTuneState( int nChannelId, CH_SAT_TransportSpec_e etSatSystem, HUINT32 ulSymbolRate )
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
			nDelay = 100;
			nRetryCount = 35;
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
			nDelay = 100;
			nRetryCount = 40;
		}
		else
		{
			//100*30 = 3000ms
			nDelay = 100;
			nRetryCount = 30;
		}
	}

	CH_DI_Print(CH_LOG_TUNE, ("nDelay [%d]:[%d] nRetryCount\n", nDelay, nRetryCount));
	VK_TASK_Sleep(MIN_WAIT_DELAY);

	for( i=0; i<nRetryCount; i++ )
	{
		etCurrentState =	GetTuneStatus(nChannelId);
		if( ( etCurrentState == CH_STATUS_IDLE) || (etCurrentState == CH_STATUS_CANCLE) )
		{
			CH_DI_Print(CH_LOG_TUNE, ("Cancel locking status...\n"));
			return DI_CH_CANCEL_TUNE;
		}

		bLockStatus = DB7346_CheckLock( nChannelId );
		CH_DI_Print(CH_LOG_TUNE, ("[%d] bLockStatus : %d [%d]\n", nChannelId, bLockStatus, i));

		if( bLockStatus == TRUE )
		{
			#ifdef CH_PRINT_LOW_LEVEL_ZAPPING_TIME
			g_ulLowLevelLockedTime[nChannelId] = VK_TIMER_GetSystemTick();
			CH_DI_Print(CH_LOG_DEFAULT, ("[DB7346_CheckTuneState] [%d] Lock Time( %d )ms  \n" ,nChannelId, g_ulLowLevelLockedTime[nChannelId] - g_ulRequestTune[nChannelId] ));
			#endif
			CH_DI_Print(CH_LOG_TUNE, (">>>>>>>>>>>>>>>>>ACQ_LOCKED_AND_TRACKING [%d ms] \n", (i * nDelay)));
			return DI_CH_OK;
		}
		else
		{
			VK_TASK_Sleep(nDelay);
		}
	}

	#ifdef CH_PRINT_LOW_LEVEL_ZAPPING_TIME
	g_ulLowLevelUnlockedTime[nChannelId] = VK_TIMER_GetSystemTick();
	CH_DI_Print(CH_LOG_DEFAULT, ("[DB7346_CheckTuneState] [%d] UnLock Time( %d )ms  \n", nChannelId, g_ulLowLevelUnlockedTime[nChannelId] - g_ulRequestTune[nChannelId] ));
	#endif

	return DI_CH_ERR;

}



HBOOL DB7346_CheckLock( int nChannelId )
{
	int nRet =FALSE ;
	HUINT8 nUnitId;
	NEXUS_Error etNErrorCode = NEXUS_SUCCESS;
	NEXUS_FrontendSatelliteStatus status;
#if defined(DEBUG_SSI_SQI)
	DI_CH_SignalQuality_t stSignalInfo;
#endif
	nUnitId = GetDeviceUnitId( nChannelId );

	etNErrorCode = NEXUS_Frontend_GetSatelliteStatus(s_stFrontend[nUnitId], &status);
	if(etNErrorCode != NEXUS_SUCCESS)
	{
		CH_DI_Print(CH_LOG_DEFAULT, ( "[DB7346_CheckLock] [%d] ERR! NEXUS_Frontend_GetSatelliteStatus() Error Code(0x%x)",nUnitId, etNErrorCode ));
	}
	else
	{
		CH_DI_Print(CH_LOG_TUNE, ("----------------------------------------------------------------------------------------\n"));
		CH_DI_Print(CH_LOG_TUNE, ( " chipId : chipVersion = [0x%x]:[0x%x] \n", status.version.chipId, status.version.chipVersion));
		CH_DI_Print(CH_LOG_TUNE, ("----------------------------------------------------------------------------------------\n"));
		CH_DI_Print(CH_LOG_TUNE, ("----------------------------------------------------------------------------------------\n"));
		CH_DI_Print(CH_LOG_TUNE, ( " apMicrocodeVersion = [Major:Minor][0x%x:0x%x]\n", status.version.apMicrocodeVersion, status.version.hostConfigurationVersion));
		CH_DI_Print(CH_LOG_TUNE, ("----------------------------------------------------------------------------------------\n"));

		CH_DI_Print(CH_LOG_TUNE, ("[DB7346_CheckLock] (%d) status.tunerLocked (%d)\n",nUnitId ,status.tunerLocked));
		CH_DI_Print(CH_LOG_TUNE, ("[DB7346_CheckLock] (%d) status.demodLocked (%d)\n",nUnitId , status.demodLocked));
		CH_DI_Print(CH_LOG_TUNE, ("[DB7346_CheckLock] (%d) status.bertLocked (%d)\n",nUnitId, status.bertLocked));

		if ( status.demodLocked == TRUE )
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
#if defined(DEBUG_SSI_SQI)
	GetSignalStatus( nUnitId, &stSignalInfo );
#endif
	return(nRet);
}



int DB7346_GetSignalInfo( int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo )
{
	int nResult = DI_CH_OK;
	int nUnitId;

	nUnitId = GetDeviceUnitId( nChannelId );

	nResult = GetSignalStatus( nUnitId, pstSignalInfo);

	return nResult;
}



int DB7346_SetPowerState( int nChannelId, CH_Power_e etPower )
{
	int nResult = DI_CH_OK;
	int nUnitId;
	//	BERR_Code etNErrorCode = BERR_SUCCESS;
	//	BAST_P_ChannelHandle *pstChannelHandle;

	nUnitId = GetDeviceUnitId( nChannelId );

	if( etPower == CH_POWER_OFF ) // set standby
	{
		CH_DI_Print(CH_LOG_ARGS, (">>>>>NEXUS_PowerDown <<<<< \n"));
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
		CH_DI_Print(CH_LOG_ARGS, (">>>>>NEXUS_PowerUp <<<<<< \n"));
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



int DB7346_EnableTsOutput( int nChannelId )
{
	//NEXUS_Error 	etNErrorCode = NEXUS_SUCCESS;
	int nUnitId;

	nUnitId = GetDeviceUnitId( nChannelId );

	CH_DI_Print(CH_LOG_DEFAULT, ("[%d] Enable TS Output ===== \n", nUnitId));
	#if 0
	etNErrorCode = NEXUS_Frontend_SetTsOutput(s_stFrontend[nUnitId], true);
	if(etNErrorCode != NEXUS_SUCCESS)
	{
		CH_DI_Print(0, ( "[DB7346_EnableTsOutput] [%d] ERR! NEXUS_Frontend_SetTsOutput()  Error Code(0x%x)", nUnitId, etNErrorCode));
		return DI_CH_ERR;
	}
	#endif
	return DI_CH_OK;
}



int DB7346_DisableTsOutput( int nChannelId )
{
	NEXUS_Error 	etNErrorCode = NEXUS_SUCCESS;
	int nUnitId;

	nUnitId = GetDeviceUnitId( nChannelId );

	CH_DI_Print(CH_LOG_DEFAULT, ("[%d] Disable TS Output ===== \n", nUnitId));

	etNErrorCode = NEXUS_Frontend_SetTsOutput(s_stFrontend[nUnitId], false);
	if(etNErrorCode != NEXUS_SUCCESS)
	{
		CH_DI_Print(CH_LOG_DEFAULT, ( "[DB7346_DisableTsOutput] [%d] ERR! NEXUS_Frontend_SetTsOutput()  Error Code(0x%x)", nUnitId, etNErrorCode));
		return DI_CH_ERR;
	}

	return DI_CH_OK;
}


int DB7346_SetToneBurst(int nChannelId, HBOOL b22kTone, CH_SAT_DiseqcInput_e etInput, CH_SAT_AntennaType_e etAntennaType, CH_SAT_DiseqcVer_e etDiseqcVer )
{
	int nResult	= DI_CH_OK;
	int nRfInputId;
	NEXUS_Error 	etNErrorCode = NEXUS_SUCCESS;
	NEXUS_FrontendDiseqcSettings diseqcSettings;

	CH_UNUSED(etAntennaType);

	nRfInputId = GetRfInputId( nChannelId );

	diseqcSettings.toneEnabled = b22kTone;
	diseqcSettings.toneMode = NEXUS_FrontendDiseqcToneMode_eEnvelope;
#if (NEXUS_VERSION >= 1501)
	diseqcSettings.preTransmitDelay = 10;
#else
	if(etDiseqcVer == DI_CH_DISEQC_VER_2_0)
	{
		diseqcSettings.replyDisabled = FALSE;
	}
	else
	{
		diseqcSettings.replyDisabled = TRUE;
	}
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
			diseqcSettings.toneBurst = NEXUS_FrontendDiseqcToneBurst_eNone;
			break;
	}

	etNErrorCode = NEXUS_Frontend_SetDiseqcSettings(s_stFrontend[nRfInputId], &diseqcSettings);
	if(etNErrorCode != NEXUS_SUCCESS)
	{
		CH_DI_Print(CH_LOG_DEFAULT, ( "[DB7346_SetToneBurst] [%d] ERR! NEXUS_Frontend_SetDiseqcSettings() ErrorCode(0x%x), etInput(%d) \n",nRfInputId, etNErrorCode, etInput ));
		nResult = DI_CH_ERR;
	}

	return nResult;
}

int DB7346_ResetDiseqc(int nChannelId)
{
	NEXUS_Error 	etNErrorCode = NEXUS_SUCCESS;
	int 			nRfInputId;

	nRfInputId = GetRfInputId( nChannelId );

      etNErrorCode = NEXUS_Frontend_ResetDiseqc(s_stFrontend[nRfInputId],0);
	if(etNErrorCode != NEXUS_SUCCESS)
	{
		CH_DI_Print(CH_LOG_DEFAULT, ( "[DB7346_ResetDiseqc] [%d] NEXUS_Frontend_ResetDiseqc() Error Code(0x%x)", nRfInputId, etNErrorCode));
		return DI_CH_ERR;
	}

    return DI_CH_OK;
}

int DB7346_SendDiseqcMsg(
int nChannelId,
HUINT8 *pucSendBuf,
HUINT8 ucNumOfBuf,
CH_SAT_DiseqcInput_e etDiseqcInput
)
{
	NEXUS_Error 	etNErrorCode = NEXUS_SUCCESS;
	NEXUS_CallbackDesc 	callback;
	int i;
	int 			nRfInputId;

	nRfInputId = GetRfInputId( nChannelId );

	VK_memset(&callback, 0, sizeof(NEXUS_CallbackDesc));

	callback.callback = DiseqcCompleteCallback;
	callback.context = s_stFrontend[nRfInputId];

	CH_UNUSED(etDiseqcInput);

#if 0 //TODO_ILEE
	VK_TASK_Sleep(10 );//Delay for Diseqc Msg Initial Reset Timing
	CH_DI_Print(0, ("===== DiseqcMessage ChannelId[%d]:[%d]nUnitId =====\n", nChannelId, nUnitId));
#endif
	CH_DI_Print(CH_LOG_ARGS, ("[%d] ===== DiseqcMessage Length(%d) =====\n", nChannelId, ucNumOfBuf));

	if(show_di_ch == CH_LOG_ARGS)
	{
		for( i=0; i<ucNumOfBuf; i++)
		{
			CH_DI_Print(CH_LOG_ARGS, ("[%02x] ", pucSendBuf[i]));
		}
	}
	CH_DI_Print(CH_LOG_ARGS, ("\n===== DiseqcMessage End =======\n"));

	etNErrorCode = NEXUS_Frontend_SendDiseqcMessage(s_stFrontend[nRfInputId], pucSendBuf, ucNumOfBuf, &callback);
	if(etNErrorCode != NEXUS_SUCCESS)
	{
		CH_DI_Print(CH_LOG_DEFAULT, ( "[DB7346_SendDiseqcCommand] [%d] NEXUS_Frontend_SendDiseqcMessage() Error Code(0x%x)", nRfInputId, etNErrorCode));
		return DI_CH_ERR;
	}

return DI_CH_OK;
}


int DB7346_ReceiveDiseqcMsg(
int nChannelId,
HUINT8 *pucReceiveBuf,
HUINT8 ucNumOfBuf,
HUINT8 *pucReplyLength
)
{
	NEXUS_Error 	etNErrorCode = NEXUS_SUCCESS;
	int 			nRfInputId;
	NEXUS_FrontendDiseqcMessageStatus etStatus;
	size_t tdReplyLength=0, tdNumOfBuf=0;

	tdNumOfBuf = (size_t)ucNumOfBuf;
	nRfInputId = GetRfInputId( nChannelId );

	etNErrorCode = NEXUS_Frontend_GetDiseqcReply(s_stFrontend[nRfInputId], &etStatus, pucReceiveBuf,tdNumOfBuf, &tdReplyLength);
	CH_DI_Print(CH_LOG_ARGS, ( "[NEXUS_Frontend_GetDiseqcReply] etStatus[%d] tdReplyLength[%d] \n",etStatus, tdReplyLength));
	if(etNErrorCode != NEXUS_SUCCESS)
	{
		CH_DI_Print(CH_LOG_DEFAULT, ( "Error : NEXUS_Frontend_GetDiseqcReply() etNErrorCode[%d] \n",etNErrorCode ));
	}
	else
	{
		*pucReplyLength = (HUINT8)tdReplyLength;
	}

	return etNErrorCode;
}


int DB7346_UpdateTunedParam( int nChannelId, CH_SAT_TuneParam_t * pstTuneParam )
{
	int   nResult = DI_CH_OK;
	int 	nUnitId;

	NEXUS_Error 	etNErrorCode = NEXUS_SUCCESS;
	NEXUS_FrontendSatelliteStatus stStatus;

	nUnitId = GetDeviceUnitId( nChannelId );

	if( (pstTuneParam->etFecCodeRate == DI_CH_SCODERATE_AUTO)
	|| (pstTuneParam->etModulation == DI_CH_PSK_AUTO) )
	{

		CH_DI_Print(CH_LOG_ARGS, ("[DB7346_UpdateTunedParam] ============================ \n"));

		etNErrorCode = NEXUS_Frontend_GetSatelliteStatus(s_stFrontend[nUnitId], &stStatus);
		if(etNErrorCode == NEXUS_SUCCESS)
		{
			ConvertNexusToDiParam(stStatus.mode, stStatus.codeRate, pstTuneParam );
		}
		else
		{
			CH_DI_Print(CH_LOG_DEFAULT, ("[DB7346_UpdateTunedParam] [%d] ERR! NEXUS_Frontend_GetSatelliteStatus() Error Code(0x%x) \n ", nUnitId, etNErrorCode));
			nResult = DI_CH_ERR;
		}
	}

	return nResult;

}

int DB7346_SetInputMode( int nChannelId, CH_SAT_InputMode_e etInputMode )
{
	CH_UNUSED(nChannelId);
	CH_UNUSED(etInputMode);

	return DI_CH_NOT_SUPPORTED;
}



int DB7346_SetInputSource( int nChannelId, DI_CH_InputSource_e etInputSource )
{
	CH_UNUSED(nChannelId);
	CH_UNUSED(etInputSource);

	return DI_CH_NOT_SUPPORTED;
}




HBOOL DB7346_LnbGet22KhzTone( int nChannelId )
{
	int 			nRfInputId;

	nRfInputId = GetRfInputId( nChannelId );

	return s_stLnbState[nRfInputId].bToneEnabled;

}



int DB7346_LnbSet22KhzTone( int nChannelId, HBOOL b22kTone )
{
	int nResult = DI_CH_OK;
	int nRfInputId;
	NEXUS_Error 	etNErrorCode = NEXUS_SUCCESS;
	NEXUS_FrontendDiseqcSettings diseqcSettings;

	nRfInputId = GetRfInputId( nChannelId );

	NEXUS_Frontend_GetDiseqcSettings(s_stFrontend[nRfInputId], &diseqcSettings);

	diseqcSettings.toneMode = NEXUS_FrontendDiseqcToneMode_eEnvelope;
	diseqcSettings.toneEnabled = (bool)b22kTone;
	//CH_DI_Print( "s_stFrontend (0x%x), frontend(0x%x) \n", s_stFrontend, frontend );

	etNErrorCode = NEXUS_Frontend_SetDiseqcSettings(s_stFrontend[nRfInputId], &diseqcSettings);
	if(etNErrorCode != NEXUS_SUCCESS)
	{
		CH_DI_Print(CH_LOG_DEFAULT, ( "[DB7346_LnbSet22KhzTone] [%d] ERR! NEXUS_Frontend_SetDiseqcSettings() ErrorCode(0x%x), bDiseqcTone(%d) \n",nRfInputId, etNErrorCode, diseqcSettings.toneEnabled ));
		return DI_CH_ERR;
	}

	s_stLnbState[nRfInputId].bToneEnabled = b22kTone;

	return nResult;
} // end of drv_channel_LNB22KHzTone


/*********************** End of File ******************************/

