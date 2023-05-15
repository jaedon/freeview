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

#include "drv_channel_db4538.h"

/* nexus headers */
#include "nexus_frontend.h"
#include "nexus_platform.h"
#include "nexus_frontend_4538.h"

// End Including Headers


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
// Start #define
#define MIN_WAIT_DELAY			(30)
#define RS_STUFF_BYTE			(16) // Normal
#define ODU_SETUP_TIME_DISEQC	(200) /* TODO : Must be optimized... Outdoor Unit setup time 200ms for DISEQC */
#define ODU_SETUP_TIME_LNB		(100) /* Outdoor Unit setup time 100ms for LNB */

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

static NEXUS_FrontendHandle s_stFrontend[CONFIG_NUM_OF_SAT_CHANNEL];

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

#define __DB4538_PRIVATE_FUNCITONS__

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
			CH_DI_Print(4, ("[GetNexusCodeRate] default: etCodeRate(%d)!\n", etFecCodeRate));
			break;
	}

	CH_DI_Print(4, ("[GetNexusCodeRate] etFecCodeRate (%d) ->  etCodeRate (%d/%d) \n", etFecCodeRate, etCodeRate.numerator, etCodeRate.denominator));

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
			CH_DI_Print(4, (" NEXUS_FrontendSatelliteMode_eDvb'.. (DVB-S)\n"));
		}
		else
		{
			etSatMode = NEXUS_FrontendSatelliteMode_eDvb;//NEXUS_FrontendSatelliteMode_eDvbScan;
			CH_DI_Print(4, (" NEXUS_FrontendSatelliteMode_eDvbScan'.. (DVB-S, AUTO)\n"));
		}
	}
	else if( etTransSpec == DI_CH_TRANS_DVBS2 )
	{
		if( etModulation == DI_CH_PSK_QPSK )
		{
			etSatMode = NEXUS_FrontendSatelliteMode_eQpskLdpc;
			CH_DI_Print(4, (" NEXUS_FrontendSatelliteMode_eQpskLdpc'.. (DVB-S2, QPSK)\n"));
		}
		else if(etModulation == DI_CH_PSK_8PSK )
		{
			etSatMode = NEXUS_FrontendSatelliteMode_e8pskLdpc;
			CH_DI_Print(4, (" NEXUS_FrontendSatelliteMode_e8pskLdpc'.. (DVB-S2, 8PSK)\n"));
		}
		else
		{
			etSatMode = NEXUS_FrontendSatelliteMode_eBlindAcquisition;//NEXUS_FrontendSatelliteMode_eLdpcScan;
			CH_DI_Print(4, (" NEXUS_FrontendSatelliteMode_eLdpcScan'.. (DVB-S2, AUTO)\n"));
		}
	}
	else
	{
		CH_DI_Print(1, ("[GetNexusMode] ERR! etTransSpec (%d)!\n", etTransSpec));
	}

	return etSatMode;
}

static void ConvertNexusToDiParam( NEXUS_FrontendSatelliteMode etSatMode, NEXUS_FrontendSatelliteCodeRate etCodeRate, CH_SAT_TuneParam_t * pstTuneParam )
{
	if(etSatMode == NEXUS_FrontendSatelliteMode_eDvb)
	{
		pstTuneParam->etTransSpec = DI_CH_TRANS_DVBS;
		pstTuneParam->etModulation	= DI_CH_PSK_QPSK;
		CH_DI_Print(4, ("[DB4538_NexusUpdateInfo] %x : DVBS, QPSK \n", etSatMode));
	}
	else if(etSatMode == NEXUS_FrontendSatelliteMode_eQpskLdpc)
	{
		pstTuneParam->etTransSpec = DI_CH_TRANS_DVBS2;
		pstTuneParam->etModulation	= DI_CH_PSK_QPSK;
		CH_DI_Print(4, ("[DB4538_NexusUpdateInfo] %x : DVBS2, QPSK \n", etSatMode));
	}
	else if(etSatMode == NEXUS_FrontendSatelliteMode_e8pskLdpc)
	{
		pstTuneParam->etTransSpec = DI_CH_TRANS_DVBS2;
		pstTuneParam->etModulation	= DI_CH_PSK_8PSK;
		CH_DI_Print(4, ("[DB4538_NexusUpdateInfo] %x : DVBS2, 8PSK \n", etSatMode));
	}
	else
	{
		pstTuneParam->etModulation	= DI_CH_PSK_AUTO;
	}

	CH_DI_Print(4, ("[DB4538_NexusUpdateInfo] etTransSpec [%d] , etModulation [%d]  \n", pstTuneParam->etTransSpec, pstTuneParam->etModulation ));

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
		CH_DI_Print(4, ("DI_CH_SCODERATE_AUTO \n"));
	}

	CH_DI_Print(4, (" etFecCodeRate [%d] \n",  pstTuneParam->etFecCodeRate));

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
	HINT32  nInputPower
)
{
	HUINT32 ulStrength = 0;

	CH_UNUSED(etSatMode);

	CH_DI_Print(4, ("[ConvertStrengthLevel] nInputPower: %d \n", nInputPower));

		if(nInputPower >= -3100)  // < -22dBm
			ulStrength = 100;
		else if( nInputPower >= -3300)  // -26dBm
			ulStrength = 90;
		else if( nInputPower >= -4100)  // -30dBm
			ulStrength = 85;
		else if( nInputPower >= -4600)  // -30dBm
			ulStrength = 80;
		else if( nInputPower >= -5100)  // -34dBm
			ulStrength = 75;
		else if( nInputPower >= -5600)  // -34dBm
			ulStrength = 70;
		else if( nInputPower >= -6000)  // -38dBm
			ulStrength = 60;
		else if( nInputPower >= -6600)  // -42dBm
			ulStrength = 50;
		else if( nInputPower >= -7100)  // -46dBm
			ulStrength = 40;
		else if( nInputPower >= -7300)  // -50dBm
			ulStrength = 30;
		else if( nInputPower >= -7800)  // -54dBm
			ulStrength = 20;
		else
			ulStrength = 10;

	CH_DI_Print(4, ("[ConvertStrengthLevel] => ulStrength: (%d)  \n", ulStrength ));

	return ulStrength;
}



static HUINT32 ConvertQualityLevel(
	NEXUS_FrontendSatelliteMode etSatMode,
	HUINT32 ulInSnr
)
{
	HUINT32 ulQuality = 0;

	CH_DI_Print(4, ("[ConvertQualityLevel] ulInSnr: %d \n", ulInSnr));

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

	CH_DI_Print(4, ("[ConvertQualityLevel] => ulQuality: (%d)	\n", ulQuality ));

	return ulQuality;


}

#if defined(SUPPORT_DTG_SIGNAL_DISPLAY)
static HUINT32 ConvertDtgStrengthLevel(
	NEXUS_FrontendSatelliteMode etSatMode,
	HINT32  nInputPower
)
{
	HUINT32 ulStrength = 0;

	CH_UNUSED(etSatMode);

	CH_DI_Print(4, ("[ConvertDtgStrengthLevel] nInputPower: %d \n", nInputPower));

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

	CH_DI_Print(4, ("[ConvertDtgStrengthLevel] => ulStrength: (%d)  \n", ulStrength ));

	return ulStrength;
}



static HUINT32 ConvertDtgQualityLevel(
	NEXUS_FrontendSatelliteMode etSatMode,
	HUINT32 ulInSnr
)
{
	HUINT32 ulQuality = 0;

	CH_DI_Print(4, ("[ConvertDtgQualityLevel] ulInSnr: %d \n", ulInSnr));

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

	CH_DI_Print(4, ("[ConvertDtgQualityLevel] => ulQuality: (%d)	\n", ulQuality ));

	return ulQuality;

}
#endif

static int GetSignalStatus( int nUnitId, DI_CH_SignalQuality_t *pstSignalInfo )
{
	NEXUS_Error 	etNErrorCode = NEXUS_SUCCESS;
	NEXUS_FrontendSatelliteStatus stFrontendStatus;

	float fInputPower;

	HUINT32 ulQuality = 0;
	HUINT32 ulStrength = 0;

	etNErrorCode = NEXUS_Frontend_GetSatelliteStatus(s_stFrontend[nUnitId], &stFrontendStatus);
	if(etNErrorCode != NEXUS_SUCCESS)
	{
		CH_DI_Print(0, ( "[GetSignalStatus] [%d] ERR! NEXUS_Frontend_GetSatelliteStatus (0x%x)",nUnitId, etNErrorCode ));
		return DI_CH_ERR;
	}

	CH_DI_Print(5, ("----------------------------------------------------------------------------------------\n"));
	CH_DI_Print(5, ("frequency            =(%u)Hz\n", stFrontendStatus.frequency                        ));
	CH_DI_Print(5, ("demodLocked          =(%d)\n", stFrontendStatus.demodLocked                      ));
	CH_DI_Print(5, ("bertLocked           =(%d)\n", stFrontendStatus.bertLocked                       ));
	CH_DI_Print(5, ("symbolRate           =(%u)\n", stFrontendStatus.symbolRate                       ));
	CH_DI_Print(5, ("symbolRateError      =(%d)\n", stFrontendStatus.symbolRateError                  ));
	CH_DI_Print(5, ("carrierOffset        =(%d)\n", stFrontendStatus.carrierOffset                    ));
	//CH_DI_Print(5, ("carrierError         =(%d)\n", stFrontendStatus.carrierError                     ));
	CH_DI_Print(5, ("outputBitRate        =(%u)\n", stFrontendStatus.outputBitRate                    ));
	CH_DI_Print(5, ("ifAgcLevel           =(%u)\n", stFrontendStatus.ifAgcLevel                       ));
	CH_DI_Print(5, ("rfAgcLevel           =(%u)\n", stFrontendStatus.rfAgcLevel                       ));
	CH_DI_Print(5, ("intAgcLevel          =(%u)\n", stFrontendStatus.intAgcLevel                      ));
	CH_DI_Print(5, ("snrEstimate          =(%u)(1/100 dB)\n", stFrontendStatus.snrEstimate                      ));
	CH_DI_Print(5, ("berEstimate          =(%u)\n", stFrontendStatus.berEstimate                      ));
	CH_DI_Print(5, ("fecCorrected         =(%u)\n", stFrontendStatus.fecCorrected                     ));
	CH_DI_Print(5, ("fecUncorrected       =(%u)\n", stFrontendStatus.fecUncorrected                   ));
	CH_DI_Print(5, ("fecClean             =(%u)\n", stFrontendStatus.fecClean                         ));
	CH_DI_Print(5, ("bitErrCorrected      =(%u)\n", stFrontendStatus.bitErrCorrected                  ));
	CH_DI_Print(5, ("reacquireCount       =(%u)\n", stFrontendStatus.reacquireCount                   ));
#if (NEXUS_VERSION >= 1290)
	CH_DI_Print(5, ("berErrors            =(%u)\n", stFrontendStatus.berErrorCount                    ));
#else
	CH_DI_Print(5, ("berErrors            =(%u)\n", stFrontendStatus.berErrors                        ));
#endif
	CH_DI_Print(5, ("preViterbiErrorCount =(%u)\n", stFrontendStatus.preViterbiErrorCount             ));
	CH_DI_Print(5, ("mpegErrors           =(%u)\n", stFrontendStatus.mpegErrors                       ));
	CH_DI_Print(5, ("mpegCount            =(%u)\n", stFrontendStatus.mpegCount                        ));
	CH_DI_Print(5, ("----------------------------------------------------------------------------------------\n"));
	CH_DI_Print(5, ("ifAgc                =(0x%x):(%u)\n", stFrontendStatus.ifAgc, stFrontendStatus.ifAgc ));
	CH_DI_Print(5, ("rfAgc                =(0x%x):(%u)\n", stFrontendStatus.rfAgc, stFrontendStatus.rfAgc ));
	CH_DI_Print(5, ("agf                  =(0x%x):(%d)\n", stFrontendStatus.agf, stFrontendStatus.agf     ));
	CH_DI_Print(5, ("----------------------------------------------------------------------------------------\n"));
#if 0
	CH_DI_Print(5, ("agc0                 =(0x%x):(%u)\n", stFrontendStatus.lnaStatus.agc0, stFrontendStatus.lnaStatus.agc0 ));
	CH_DI_Print(5, ("agc1                 =(0x%x):(%u)\n", stFrontendStatus.lnaStatus.agc1, stFrontendStatus.lnaStatus.agc1 ));
	CH_DI_Print(5, ("intConfig            =(0x%x):(%u)\n", stFrontendStatus.lnaStatus.intConfig, stFrontendStatus.lnaStatus.intConfig));
	CH_DI_Print(5, ("extConfig            =(0x%x):(%u)\n", stFrontendStatus.lnaStatus.extConfig, stFrontendStatus.lnaStatus.extConfig));
	CH_DI_Print(5, ("version              =(0x%x):(%u)\n", stFrontendStatus.lnaStatus.version, stFrontendStatus.lnaStatus.version));
	CH_DI_Print(5, ("----------------------------------------------------------------------------------------\n"));
#endif
	CH_DI_Print(5, ("chipId               =(%x)\n", stFrontendStatus.version.chipId                   ));
	CH_DI_Print(5, ("chipVersion          =(%x)\n", stFrontendStatus.version.chipVersion              ));
	CH_DI_Print(5, ("bondingOption        =(%x)\n", stFrontendStatus.version.bondingOption            ));
	CH_DI_Print(5, ("MajVer               =(%x)\n", stFrontendStatus.version.apMicrocodeVersion       ));
	CH_DI_Print(5, ("MinVer               =(%x)\n", stFrontendStatus.version.hostConfigurationVersion ));
	CH_DI_Print(5, ("----------------------------------------------------------------------------------------\n"));

	//SATFE_Platform_GetInputPower( nUnitId, stFrontendStatus.ifAgc, stFrontendStatus.agf, stFrontendStatus.frequency, &fInputPower);
	CH_DI_Print(5, ( "[GetSignalStatus] nUnitId[%d]Frontend.mode = (%d) \n", nUnitId, stFrontendStatus.mode));
	CH_DI_Print(5, ( "[GetSignalStatus] nUnitId[%d] fInputPower  = (%f) \n", nUnitId, fInputPower));
	CH_DI_Print(5, ("----------------------------------------------------------------------------------------\n"));

	//ulStrength = ConvertDtgStrengthLevel(stFrontendStatus.mode, (HINT32)(fInputPower*100));
	ulStrength = ConvertStrengthLevel(stFrontendStatus.mode, (HINT32)(fInputPower*100));

	if( stFrontendStatus.demodLocked == true)
	{
		//ulQuality = ConvertDtgQualityLevel(stFrontendStatus.mode, stFrontendStatus.snrEstimate);
		ulQuality = ConvertQualityLevel(stFrontendStatus.mode, stFrontendStatus.snrEstimate);

		pstSignalInfo->fSnr = ((float)(stFrontendStatus.snrEstimate))/100;
		pstSignalInfo->ulAgc = 0;
		pstSignalInfo->ulRfAgc = (HUINT32)stFrontendStatus.rfAgc;
		pstSignalInfo->ulIfAgc = (HUINT32)stFrontendStatus.ifAgc;
		pstSignalInfo->ulUnCorrectedNo = stFrontendStatus.fecUncorrected;
		pstSignalInfo->fSignalInputPower = fInputPower;
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

		CH_DI_Print(1, ( "[GetSignalStatus] [%d] Demod UnLocked \n", nUnitId));
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
				CH_DI_Print(3, (">>>>> Same TP in DISEQC!!! \n"));
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
				CH_DI_Print(3, (">>>>> Same TP in LNB!!! \n"));
			}
		}
		else // SCD
		{
			ulSetupTime = ODU_SETUP_TIME_DISEQC;
		}
	}
	else
	{
		CH_DI_Print(3, (">>>>> Skip : ODU_SETUP_TIME \n"));
	}

	CH_DI_Print(3, ("<<<<< ODU_SETUP_TIME (%d) \n", ulSetupTime));
	return ulSetupTime;

}



#define __DB4538_PUBLIC_FUNCITONS__

/*********************** External Function ***********************/
void DRV_S_InstallApi( void )
{
	NIM_InitDevice 			= &DB4538_InitDevice;
	NIM_SetTune 			= &DB4538_SetTune;
	NIM_CheckTuneState 		= &DB4538_CheckTuneState;
	NIM_CheckLock 			= &DB4538_CheckLock;
	NIM_GetSignalInfo 		= &DB4538_GetSignalInfo;
	NIM_SetPowerState 		= &DB4538_SetPowerState;
	NIM_EnableTsOutput 		= &DB4538_EnableTsOutput;
	NIM_DisableTsOutput 	= &DB4538_DisableTsOutput;
	NIM_SendDiseqcMsg 		= &DB4538_SendDiseqcMsg;
	NIM_UpdateTunedParam	= &DB4538_UpdateTunedParam;
	NIM_SetInputMode		= &DB4538_SetInputMode;
	NIM_SetInputSource		= &DB4538_SetInputSource;

	NIM_SetToneBurst		= &DB4538_SetToneBurst;

	LNB_Get22KhzTone		= &DB4538_LnbGet22KhzTone;
	LNB_Set22KhzTone		= &DB4538_LnbSet22KhzTone;

	LNB_InitDevice 			= &DRV_CH_LNB_Init;
	LNB_SetOutput			= &DRV_CH_LNB_SetOutput;
	LNB_SetPolarisation 	= &DRV_CH_LNB_SetPolar;
	LNB_CheckAntState		= &DRV_CH_LNB_CheckAntState;
	return;
}



int DB4538_InitDevice( int nChannelId )
{
	int				 nUnitId;
	NEXUS_Error 	etNErrorCode = NEXUS_SUCCESS;
	NEXUS_PlatformConfiguration platformConfig;

	nUnitId = GetDeviceUnitId( nChannelId );

	NEXUS_Platform_GetConfiguration(&platformConfig);

	s_stFrontend[nUnitId] = platformConfig.frontend[nUnitId];

	if( NULL == s_stFrontend[nUnitId] )
	{
		CH_DI_Print(0, ("[DB4538_InitDevice] ERR! Unable to find Satellite-capable frontend\n"));
		return DI_CH_ERR;
	}

	//CH_DI_Print(4,("CH[%d] s_stFrontend (%x), frontend(0x%x) \n", nUnitId, s_stFrontend[nUnitId], platformConfig.frontend[nUnitId] ));

	/* PKT_CLK runs continuously.*//* false=CLK runs continuously, true=CLK is suppressed when VALID not active */
	//etNErrorCode = NEXUS_Frontend_SetXportCtrl(s_stFrontend[nUnitId], FALSE);
	if(etNErrorCode != NEXUS_SUCCESS)
	{
		CH_DI_Print(0, ( "[DB4538_InitDevice] [%d] ERR! NEXUS_Frontend_SetXportCtrl() Error Code(0x%x)",nUnitId, etNErrorCode ));
		return DI_CH_ERR;
	}

	return DI_CH_OK;
}



int DB4538_SetTune( int nChannelId, CH_SAT_TuneParam_t *pstSatTuneParam )
{
	NEXUS_Error 	etNErrorCode = NEXUS_SUCCESS;
	NEXUS_FrontendSatelliteSettings satSettings;
	int nUnitId = 0;
	HUINT32 ulOduSetupTime = 0;

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
		CH_DI_Print(0, ( "[DB4538_SetTune] [%d] ERR! NEXUS_Frontend_SetStuffBytes() Error Code(0x%x)",nUnitId, etNErrorCode ));
		return DI_CH_ERR;
	}
	#endif

	ulOduSetupTime = GetOduSetupTime(nChannelId, pstSatTuneParam);
	if(ulOduSetupTime != 0)
	{
		VK_TASK_Sleep( ulOduSetupTime );
	}

	//satSettings.lockCallback.callback = lock_callback;
	//satSettings.lockCallback.context = frontend;

	CH_DI_Print(2, ("\n ############	Tuning NEXUS_Frontend_Satellite ############ \n"));

	#ifdef CH_PRINT_LOW_LEVEL_ZAPPING_TIME
	g_ulRequestTune[nUnitId] = VK_TIMER_GetSystemTick();
	#endif
	etNErrorCode = NEXUS_Frontend_TuneSatellite(s_stFrontend[nUnitId], &satSettings);
	if(etNErrorCode != NEXUS_SUCCESS)
	{
		CH_DI_Print(0, ( "[DB4538_SetTune] [%d] ERR! NEXUS_Frontend_TuneSatellite() Error Code(0x%x)",nUnitId, etNErrorCode ));
		return DI_CH_ERR;
	}

	return DI_CH_OK;
}



int DB4538_CheckTuneState( int nChannelId, CH_SAT_TransportSpec_e etSatSystem, HUINT32 ulSymbolRate )
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

	CH_DI_Print(3, ("nDelay [%d]:[%d] nRetryCount\n", nDelay, nRetryCount));
	VK_TASK_Sleep(MIN_WAIT_DELAY);

	for( i=0; i<nRetryCount; i++ )
	{
		etCurrentState =	GetTuneStatus(nChannelId);
		if( ( etCurrentState == CH_STATUS_IDLE) || (etCurrentState == CH_STATUS_CANCLE) )
		{
			CH_DI_Print(1, ("Cancel locking status...\n"));
			return DI_CH_CANCEL_TUNE;
		}

		bLockStatus = DB4538_CheckLock( nChannelId );
		CH_DI_Print(3, ("[%d] bLockStatus : %d [%d]\n", nChannelId, bLockStatus, i));

		if( bLockStatus == TRUE )
		{
			#ifdef CH_PRINT_LOW_LEVEL_ZAPPING_TIME
			g_ulLowLevelLockedTime[nChannelId] = VK_TIMER_GetSystemTick();
			CH_DI_Print(0, ("[DB4538_CheckTuneState] [%d] Lock Time( %d )ms  \n" ,nChannelId, g_ulLowLevelLockedTime[nChannelId] - g_ulRequestTune[nChannelId] ));
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
	CH_DI_Print(0, ("[DB4538_CheckTuneState] [%d] UnLock Time( %d )ms  \n", nChannelId, g_ulLowLevelUnlockedTime[nChannelId] - g_ulRequestTune[nChannelId] ));
	#endif

	return DI_CH_ERR;

}



HBOOL DB4538_CheckLock( int nChannelId )
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
		CH_DI_Print(0, ( "[DB4538_CheckLock] [%d] ERR! NEXUS_Frontend_GetSatelliteStatus() Error Code(0x%x)",nUnitId, etNErrorCode ));
	}
	else
	{
		CH_DI_Print(6, ("----------------------------------------------------------------------------------------\n"));
		CH_DI_Print(6, ( " chipId : chipVersion = [0x%x]:[0x%x] \n", status.version.chipId, status.version.chipVersion));
		CH_DI_Print(6, ("----------------------------------------------------------------------------------------\n"));
		CH_DI_Print(6, ("----------------------------------------------------------------------------------------\n"));
		CH_DI_Print(6, ( " apMicrocodeVersion = [Major:Minor][0x%x:0x%x]\n", status.version.apMicrocodeVersion, status.version.hostConfigurationVersion));
		CH_DI_Print(6, ("----------------------------------------------------------------------------------------\n"));

		CH_DI_Print(6, ("[DB4538_CheckLock] (%d) status.tunerLocked (%d)\n",nUnitId ,status.tunerLocked));
		CH_DI_Print(6, ("[DB4538_CheckLock] (%d) status.demodLocked (%d)\n",nUnitId , status.demodLocked));
		CH_DI_Print(6, ("[DB4538_CheckLock] (%d) status.bertLocked (%d)\n",nUnitId, status.bertLocked));

		if ( status.demodLocked == TRUE )
		{
			//LOCK
			nRet = TRUE;
			CH_DI_Print(6, (">>>>> LOCK [%d] <<<<<	\n", nUnitId));
		}
		else
		{
			//unlock
			nRet = FALSE;
			CH_DI_Print(6, (">>>>> UNLOCK [%d]<<<<<  \n",nUnitId));
		}
	}
#if defined(DEBUG_SSI_SQI)
	GetSignalStatus( nUnitId, &stSignalInfo );
#endif
	return(nRet);
}



int DB4538_GetSignalInfo( int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo )
{
	int nResult = DI_CH_OK;
	int nUnitId;

	nUnitId = GetDeviceUnitId( nChannelId );

	nResult = GetSignalStatus( nUnitId, pstSignalInfo);

	return nResult;
}



int DB4538_SetPowerState( int nChannelId, CH_Power_e etPower )
{
	int nResult = DI_CH_OK;
	int nUnitId;
	//	BERR_Code etNErrorCode = BERR_SUCCESS;
	//	BAST_P_ChannelHandle *pstChannelHandle;

	nUnitId = GetDeviceUnitId( nChannelId );

	if( etPower == CH_POWER_OFF ) // set standby
	{
		CH_DI_Print(2, (">>>>>NEXUS_PowerDown <<<<< \n"));
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
		CH_DI_Print(2, (">>>>>NEXUS_PowerUp <<<<<< \n"));
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



int DB4538_EnableTsOutput( int nChannelId )
{
	//NEXUS_Error 	etNErrorCode = NEXUS_SUCCESS;
	int nUnitId;

	nUnitId = GetDeviceUnitId( nChannelId );

	CH_DI_Print(2, ("[%d] Enable TS Output ===== \n", nUnitId));
	#if 0
	etNErrorCode = NEXUS_Frontend_SetTsOutput(s_stFrontend[nUnitId], true);
	if(etNErrorCode != NEXUS_SUCCESS)
	{
		CH_DI_Print(0, ( "[DB4538_EnableTsOutput] [%d] ERR! NEXUS_Frontend_SetTsOutput()  Error Code(0x%x)", nUnitId, etNErrorCode));
		return DI_CH_ERR;
	}
	#endif
	return DI_CH_OK;
}



int DB4538_DisableTsOutput( int nChannelId )
{
	NEXUS_Error 	etNErrorCode = NEXUS_SUCCESS;
	int nUnitId;

	nUnitId = GetDeviceUnitId( nChannelId );

	CH_DI_Print(2, ("[%d] Disable TS Output ===== \n", nUnitId));

	//etNErrorCode = NEXUS_Frontend_SetTsOutput(s_stFrontend[nUnitId], false);
	if(etNErrorCode != NEXUS_SUCCESS)
	{
		CH_DI_Print(0, ( "[DB4538_DisableTsOutput] [%d] ERR! NEXUS_Frontend_SetTsOutput()  Error Code(0x%x)", nUnitId, etNErrorCode));
		return DI_CH_ERR;
	}

	return DI_CH_OK;

}


int DB4538_SetToneBurst(int nChannelId, HBOOL b22kTone, CH_SAT_DiseqcInput_e etInput, CH_SAT_AntennaType_e etAntennaType, CH_SAT_DiseqcVer_e etDiseqcVer )
{
	int nResult	= DI_CH_OK;
	int nRfInputId;

	NEXUS_Error 	etNErrorCode = NEXUS_SUCCESS;

	nRfInputId = GetRfInputId( nChannelId );

	CH_UNUSED(etAntennaType);
	CH_UNUSED(etDiseqcVer);

	NEXUS_FrontendDiseqcSettings diseqcSettings;

	NEXUS_Frontend_GetDiseqcSettings(s_stFrontend[nRfInputId], &diseqcSettings);

	diseqcSettings.toneEnabled = b22kTone;
	diseqcSettings.toneMode = NEXUS_FrontendDiseqcToneMode_eEnvelope;
	diseqcSettings.replyDisabled = TRUE;

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

	etNErrorCode = NEXUS_Frontend_SetDiseqcSettings(s_stFrontend[nRfInputId], &diseqcSettings);
	if(etNErrorCode != NEXUS_SUCCESS)
	{
		CH_DI_Print(0, ( "[DB4538_SetToneBurst] [%d] ERR! NEXUS_Frontend_SetDiseqcSettings() ErrorCode(0x%x), etInput(%d) \n",nRfInputId, etNErrorCode, etInput ));
		nResult = DI_CH_ERR;
	}

	return nResult;
}


int DB4538_SendDiseqcMsg(
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

#if 0 // BCM4538 : ResetDiseqc is not needed !!!
	etNErrorCode = NEXUS_Frontend_ResetDiseqc(s_stFrontend[nRfInputId],0);
	if(etNErrorCode != NEXUS_SUCCESS)
	{
		CH_DI_Print(0, ( "[DB4538_SendDiseqcCommand] [%d] NEXUS_Frontend_ResetDiseqc() Error Code(0x%x)", nRfInputId, etNErrorCode));
		return DI_CH_ERR;
	}
#endif

#if 0 //TODO_ILEE
	VK_TASK_Sleep(10 );//Delay for Diseqc Msg Initial Reset Timing
	CH_DI_Print(0, ("===== DiseqcMessage ChannelId[%d]:[%d]nUnitId =====\n", nChannelId, nUnitId));
#endif
	CH_DI_Print(3, ("[%d] ===== DiseqcMessage Length(%d) =====\n", nChannelId, ucNumOfBuf));

	if(show_di_ch > 2)
	{
		for( i=0; i<ucNumOfBuf; i++)
		{
			DI_UART_Print("[%02x] ", pucSendBuf[i]);
		}
	}
	CH_DI_Print(3, ("\n===== DiseqcMessage End =======\n"));

	etNErrorCode = NEXUS_Frontend_SendDiseqcMessage(s_stFrontend[nRfInputId], pucSendBuf, ucNumOfBuf, &callback);
	if(etNErrorCode != NEXUS_SUCCESS)
	{
		CH_DI_Print(0, ( "[DB4538_SendDiseqcCommand] [%d] NEXUS_Frontend_SendDiseqcMessage() Error Code(0x%x)", nRfInputId, etNErrorCode));
		return DI_CH_ERR;
	}

return DI_CH_OK;
}



int DB4538_UpdateTunedParam( int nChannelId, CH_SAT_TuneParam_t * pstTuneParam )
{
	int   nResult = DI_CH_OK;
	int 	nUnitId;

	NEXUS_Error 	etNErrorCode = NEXUS_SUCCESS;
	NEXUS_FrontendSatelliteStatus stStatus;

	nUnitId = GetDeviceUnitId( nChannelId );

	if( (pstTuneParam->etFecCodeRate == DI_CH_SCODERATE_AUTO)
	|| (pstTuneParam->etModulation == DI_CH_PSK_AUTO) )
	{

		CH_DI_Print(4, ("[DB4538_UpdateTunedParam] ============================ \n"));

		etNErrorCode = NEXUS_Frontend_GetSatelliteStatus(s_stFrontend[nUnitId], &stStatus);
		if(etNErrorCode == NEXUS_SUCCESS)
		{
			ConvertNexusToDiParam(stStatus.mode, stStatus.codeRate, pstTuneParam );
		}
		else
		{
			CH_DI_Print(0, ("[DB4538_UpdateTunedParam] [%d] ERR! NEXUS_Frontend_GetSatelliteStatus() Error Code(0x%x) \n ", nUnitId, etNErrorCode));
			nResult = DI_CH_ERR;
		}
	}

	return nResult;

}



int DB4538_SetInputMode( int nChannelId, CH_SAT_InputMode_e etInputMode )
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
		CH_DI_Print(0, ( "[DB4538_SetInputMode] [%d] ERR! etInputMode(%d) \n",nUnitId, etInputMode ));
		return DI_CH_NOT_SUPPORTED;
	}

	//etNErrorCode = NEXUS_Frontend_Set7346LnaSettings(s_stFrontend[nUnitId], &stLnaSettings);
	if(etNErrorCode != NEXUS_SUCCESS)
	{
		CH_DI_Print(0, ( "[DB4538_SetInputMode] [%d] ERR! NEXUS_Frontend_Set7346LnaSettings() ErrorCode(0x%x), etInputMode(%d) \n",nUnitId, etNErrorCode, etInputMode ));
	}

	return etNErrorCode;
}



int DB4538_SetInputSource( int nChannelId, DI_CH_InputSource_e etInputSource )
{
	int 			nUnitId;
	NEXUS_Error 	etNErrorCode = NEXUS_SUCCESS;
	//NEXUS_7346LnaSettings stLnaSettings;

	nUnitId = GetDeviceUnitId( nChannelId );

	if(etInputSource >= DI_CH_RF_INPUTMAX)
	{
		return DI_CH_PARAM_ERR;
	}

	CH_DI_Print(0, ( "[DB4538_SetInputSource] nUnitId[%d] : etInputSource(%d) \n",nUnitId, etInputSource ));

	if((nUnitId == MASTER_UNIT)&&(etInputSource == DI_CH_RF_INPUT2))
	{
		return DI_CH_HW_RESTRICTION;
	}

	if(nUnitId != MASTER_UNIT)
	{
		if(etInputSource == DI_CH_RF_INPUT1)
		{
			//stLnaSettings.out0  = NEXUS_7346LnaOutput_eOut0;
			//stLnaSettings.out1  = NEXUS_7346LnaOutput_eOut0;
			//stLnaSettings.daisy = NEXUS_7346LnaOutput_eNone;
		}
		else if((etInputSource == DI_CH_RF_INPUT2) || (etInputSource == DI_CH_RF_INPUTLT))
		{
			//stLnaSettings.out0  = NEXUS_7346LnaOutput_eOut0;
			//stLnaSettings.out1  = NEXUS_7346LnaOutput_eOut1;
			//stLnaSettings.daisy = NEXUS_7346LnaOutput_eNone;
		}
		else
		{
			CH_DI_Print(0, ( "[DB4538_SetInputSource] [%d] ERR! etInputSource(%d) \n",nUnitId, etInputSource ));
			return DI_CH_NOT_SUPPORTED;
		}

		//etNErrorCode = NEXUS_Frontend_Set7346LnaSettings(s_stFrontend[nUnitId], &stLnaSettings);
		if(etNErrorCode != NEXUS_SUCCESS)
		{
			CH_DI_Print(0, ( "[DB4538_SetInputSource] [%d] ERR! NEXUS_Frontend_Set7346LnaSettings() ErrorCode(0x%x), etInputMode(%d) \n",nUnitId, etNErrorCode, etInputSource ));
		}
		else
		{
			SetRfInputId(nChannelId, etInputSource);
		}

	}

	return etNErrorCode;
}



HBOOL DB4538_LnbGet22KhzTone( int nChannelId )
{
	int 			nRfInputId;

	nRfInputId = GetRfInputId( nChannelId );

	return s_stLnbState[nRfInputId].bToneEnabled;

}



int DB4538_LnbSet22KhzTone( int nChannelId, HBOOL b22kTone )
{
	int nResult = DI_CH_OK;
	int nRfInputId;
	NEXUS_Error 	etNErrorCode = NEXUS_SUCCESS;

	nRfInputId = GetRfInputId( nChannelId );

	NEXUS_FrontendDiseqcSettings diseqcSettings;

	NEXUS_Frontend_GetDiseqcSettings(s_stFrontend[nRfInputId], &diseqcSettings);

	diseqcSettings.toneMode = NEXUS_FrontendDiseqcToneMode_eEnvelope;
	diseqcSettings.toneEnabled = (bool)b22kTone;
	//CH_DI_Print( "s_stFrontend (0x%x), frontend(0x%x) \n", s_stFrontend, frontend );

	etNErrorCode = NEXUS_Frontend_SetDiseqcSettings(s_stFrontend[nRfInputId], &diseqcSettings);
	if(etNErrorCode != NEXUS_SUCCESS)
	{
		CH_DI_Print(0, ( "[DB4538_LnbSet22KhzTone] [%d] ERR! NEXUS_Frontend_SetDiseqcSettings() ErrorCode(0x%x), bDiseqcTone(%d) \n",nRfInputId, etNErrorCode, diseqcSettings.toneEnabled ));
		return DI_CH_ERR;
	}

	s_stLnbState[nRfInputId].bToneEnabled = b22kTone;

	return nResult;
} // end of drv_channel_LNB22KHzTone


/*********************** End of File ******************************/

