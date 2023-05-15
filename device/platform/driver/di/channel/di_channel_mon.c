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
#include "di_channel_s.h"
#include "di_channel_t.h"
#include "di_channel_c.h"
#include "di_channel_a.h"

#include "di_channel_priv.h"
#include "di_channel_attr.h"
#include "di_channel_util.h"

#include "drv_channel.h"
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

#ifdef __CH_MONITOR__

#define __COMMON__STRING__
/******************************************************************************
 * COMMON
 ******************************************************************************/
char *pszCH_Status_e[] =
{
	"CH_STATUS_IDLE",
	"CH_STATUS_LOCKING",
	"CH_STATUS_SEARCHING",
	"CH_STATUS_LOCKED",
	"CH_STATUS_CANCLE",
	"CH_STATUS_UNLOCK",
	"CH_STATUS_ANT_FAULT"
};

char *pszCH_Active_e[] =
{
	"CH_ACTION_IDLE",
	"CH_ACTION_ON",
	"CH_ACTION_OFF"
};

char *pszCH_TsStatus_e[] =
{
	"CH_TS_PATH_ON",
	"CH_TS_PATH_OFF"
};

#define __SAT__STRING__
/******************************************************************************
 * SAT
 ******************************************************************************/

char *pszCH_SAT_AntennaType_e[] =
{
	"ANT_TYPE_LNB",					/**< LNB only */
	"ANT_TYPE_DISEQC",				/**< DiSEqC switch */
	"ANT_TYPE_SCD",					/**< Single Cable Distribution */
	"ANT_TYPE_SMATV"				/**< SMATV */
};


/**
 * FEC code rate.
 */
char *pszCH_SAT_CodeRate_e[] =
{
	"CODERATE_AUTO",				/**< Code rate : Auto detect */
	"CODERATE_1_2",				/**< Code rate : 1/2  DVB-S 	DVB-S2:QPSK 				*/
	"CODERATE_2_3",				/**< Code rate : 2/3  DVB-S 	DVB-S2:QPSK 	DVB-S2:8PSK */
	"CODERATE_3_4",				/**< Code rate : 3/4  DVB-S 	DVB-S2:QPSK 	DVB-S2:8PSK */
	"CODERATE_5_6",				/**< Code rate : 5/6  DVB-S 	DVB-S2:QPSK 	DVB-S2:8PSK */
	"CODERATE_7_8",				/**< Code rate : 7/8  DVB-S 								*/
	"CODERATE_4_5",				/**< Code rate : 4/5 			DVB-S2:QPSK 				*/
	"CODERATE_3_5",				/**< Code rate : 3/5  			DVB-S2:QPSK 	DVB-S2:8PSK */
	"CODERATE_6_7", 				/**< Code rate : 6/7 										*/
	"CODERATE_5_11",				/**< Code rate : 5/11 										*/
	"CODERATE_8_9",				/**< Code rate : 8/9  			DVB-S2:QPSK 	DVB-S2:8PSK */
	"CODERATE_9_10"				/**< Code rate : 9/10 			DVB-S2:QPSK 	DVB-S2:8PSK */
} ;

/**
 * Polarization.
 */
char *pszCH_SAT_Polarization_e[] =
{
	"POLAR_AUTO",					/**< Auto */
	"POLAR_HOR",					/**< Horizontal */
	"POLAR_VER",					/**< Vertical */
	"POLAR_LEFT",					/**< Left */
	"POLAR_RIGHT"					/**< Right */
} ;

/**
 * Transport Sepc.
 */
char *pszCH_SAT_TransportSpec_e[] =
{
	"TRANS_DVBS",					/**< DVB-S */
	"TRANS_DVBS2"					/**< DVB-S2 */
} ;

/**
 * SAT modulation.
 */
char *pszCH_SAT_Modulation_e[] =
{
	"PSK_AUTO",						/**< Auto */
	"PSK_QPSK",						/**< QPSK */
	"PSK_8PSK"						/**< 8PSK */
} ;

/**
 * DVB-S2 pilot on/off.
 */
char *pszCH_SAT_Pilot_e[] =
{
    "PILOT_AUTO",					/**< Pilot : Auto detect */
    "PILOT_OFF",					/**< Pilot : Off */
    "PILOT_ON"						/**< Pilot : On */
} ;

/**
 * DVB-S2 Roll off.
 */
char *pszCH_SAT_RollOff_e[] =
{
	"ROLL_020",						/**< Roll off 20 */
	"ROLL_025",						/**< Roll off 25 */
	"ROLL_035"						/**< Roll off 35 */
};

/**
 * LNB voltage.
 */
char *pszCH_SAT_LnbVoltage_e[] =
{
	"LNB_VOLT_STD",					/**< Vertical : 11v, Horizontal 15v */
	"LNB_VOLT_HIGH"					/**< Vertical : 12v, Horizontal 16v */
};

/**
 * DiSEqC version.
 */
char *pszCH_SAT_DiseqcVer_e[] =
{
	"DISEQC_VER_1_0",				/**< DiSEqC Ver. 1.0 */
	"DISEQC_VER_1_1",				/**< DiSEqC Ver. 1.1 */
	"DISEQC_VER_1_2",				/**< DiSEqC Ver. 1.2 */
	"DISEQC_VER_1_3",				/**< DiSEqC Ver. 1.3 */
	"DISEQC_VER_2_0"				/**< DiSEqC Ver. 2.0 */
} ;

/**
 * SAT antenna position.
 */
char *pszCH_SAT_DiseqcInput_e[] =
{
	"DISEQC_INPUT_NONE",			/**< position : None */
	"DISEQC_INPUT_A",				/**< position : A */
	"DISEQC_INPUT_B",				/**< position : B */
	"DISEQC_INPUT_C",				/**< position : C */
	"DISEQC_INPUT_D",				/**< position : D */
	"DISEQC_INPUT_TB_A",			/**< position : TB_A */
	"DISEQC_INPUT_TB_B",			/**< position : TB_B */
	"DISEQC_INPUT_SCD_A",			/**< position : SCD A */
	"DISEQC_INPUT_SCD_B"			/**< position : SCD B */
} ;

/**
 * SAT Tone burst.
 */
char *pszCH_SAT_ToneBurst_e[] =
{
	"TONE_BURST_NONE",			/**< Tone burst : None */
	"TONE_BURST_A",				/**< Tone burst : A */
	"TONE_BURST_B"				/**< Tone burst : B */
} ;


#define __TER__STRING__
/******************************************************************************
 * TER
 ******************************************************************************/

char *pszCH_TER_BandWidth_e[] =
{
	"BW_8MHZ",
	"BW_7MHZ",
	"BW_6MHZ"
} ;


char *pszCH_TER_Moudulation_e[] =
{
	"MOD_AUTO",
	"MOD_QPSK",
	"MOD_16QAM",
	"MOD_64QAM"
} ;

char *pszCH_TER_Hierarchy_e[] =
{
	"HIERARCHY_AUTO",
	"HIERARCHY_NONE",
	"HIERARCHY_1",
	"HIERARCHY_2",
	"HIERARCHY_4"
} ;

char *pszCH_TER_CodeRate_e[] =
{
	"CODERATE_AUTO",
	"CODERATE_1_2",
	"CODERATE_2_3",
	"CODERATE_3_4",
	"CODERATE_5_6",
	"CODERATE_7_8"
} ;

char *pszCH_TER_TransMode_e[] =
{
	"TRANSMODE_AUTO",
	"TRANSMODE_2K",
	"TRANSMODE_8K",
} ;

char *pszCH_TER_GuardInterval_e[] =
{
	"GUARDINTERVAL_AUTO",
	"GUARDINTERVAL_1_32",
	"GUARDINTERVAL_1_16",
	"GUARDINTERVAL_1_8",
	"GUARDINTERVAL_1_4"
} ;

char *pszCH_TER_Stream_e[] =
{
	"STREAM_HP",
	"STREAM_LP"
} ;

char *pszCH_TER_Offset_e[] =
{
	"OFFSET_AUTO",
	"OFFSET_NONE",
	"OFFSET_1_6_P",
	"OFFSET_1_6_N",
	"OFFSET_2_6_P",
	"OFFSET_2_6_N",
	"OFFSET_3_6_P",
	"OFFSET_3_6_N",
} ;

char *pszCH_AntStatus_e[] =
{
	"ANT_POWER_OFF", 	/**< default value */
	"ANT_POWER_ON",		/**< 5 Volt On */
	"ANT_POWER_FAULT"	/**< Antenna short circuit */
} ;

#define __CAB__STRING__
/******************************************************************************
 * CAB
 ******************************************************************************/
char *pszCH_CAB_Modulation_e[] =
{
	"MOD_AUTO", 		/* Auto order : 64-> 256-> 128-> 32-> 16 */
	"MOD_16QAM",
	"MOD_32QAM",
	"MOD_64QAM",
	"MOD_128QAM",
	"MOD_256QAM"
} ;

char *pszCH_CAB_Spectrum_e[] =
{
	"INVERSION_AUTO",
	"INVERSION_NORMAL",
	"INVERSION_INVERTED"
} ;


#define __ANA__STRING__
/******************************************************************************
 * ANA
 ******************************************************************************/
char *pszCH_ANA_System_e[] =
{
	"SYSTEM_BG",
	"SYSTEM_I",
	"SYSTEM_DK",
	"SYSTEM_L",
	"SYSTEM_LP",
	"SYSTEM_DTV",
	"SYSTEM_NUM"
} ;

/**
 * Tune mode.
 */
char *pszCH_ANA_TuneMode_e[] =
{
	"A_SET_TUNE",
	"A_SET_FREQ",
	"A_SET_SYSTEM",
	"A_SET_SEARCH",
	"RESERVED"
} ;

#endif /* #ifdef __CH_MONITOR__ */

void CH_MON_PrintTypeUnitId( DI_CH_Type_e etChannelType, int nUnitId )
{
#if !defined(__CH_DEBUG__) || !defined(__CH_RELEASE_MODE_DEBUG__)
	CH_UNUSED(nUnitId);
#endif

	CH_DI_Print(1, ("===================================================\n"));
	switch( etChannelType )
	{
		case DI_CH_TYPE_SAT:
			CH_DI_Print(1, ("\t Satellite[%d]\n", nUnitId));
			break;

		case DI_CH_TYPE_TER:
			CH_DI_Print(1, ("\t Terrestrial[%d]\n", nUnitId));
			break;

		case DI_CH_TYPE_CAB:
			CH_DI_Print(1,("\t Cable[%d]\n", nUnitId));
			break;

		case DI_CH_TYPE_ANA:
			CH_DI_Print(1, ("\t Analog[%d]\n", nUnitId));
			break;

		default:
			CH_DI_Print(0, ("etChannelType(%d)", etChannelType));
			break;
	}
	CH_DI_Print(1,("===================================================\n"));
}

void CH_MON_PrintAllState( CH_Status_e etCurrentStatus, CH_TsStatus_e etTsOnOff, CH_Active_e	etChannelAction )
{
#if !defined(__CH_DEBUG__) || !defined(__CH_RELEASE_MODE_DEBUG__)
	CH_UNUSED(etCurrentStatus);
	CH_UNUSED(etTsOnOff);
	CH_UNUSED(etChannelAction);
#endif

#ifdef __CH_MONITOR__
	CH_DI_Print(1, ("   etCurrentStatus   %s[%d]\n", 	pszCH_Status_e[etCurrentStatus], etCurrentStatus));
	CH_DI_Print(1, ("   etTsOnOff         %s[%d]\n", 	pszCH_TsStatus_e[etTsOnOff], etTsOnOff));
	CH_DI_Print(1, ("   etChannelAction   %s[%d]\n", 	pszCH_Active_e[etChannelAction], etChannelAction));
#else
	CH_DI_Print(1, ("   etCurrentStatus   [%d]\n", 	etCurrentStatus));
	CH_DI_Print(1, ("   etTsOnOff         [%d]\n", 	etTsOnOff));
	CH_DI_Print(1, ("   etChannelAction   [%d]\n", 	etChannelAction));
#endif
}



void CH_MON_PrintStrenthQuality( int nChannelId, DI_CH_Type_e etChannelType )
{
	int nRet = DI_CH_OK;
/*
#if defined(CONFIG_FRONTEND_SAT)
	HUINT8 ucStrength=0, ucQuality=0;
#endif
*/
/* #if (defined(CONFIG_FRONTEND_TER) || defined(CONFIG_FRONTEND_CAB)) */
	DI_CH_SignalQuality_t stSignalInfo;
//#endif

	switch( etChannelType )
	{

#if defined(CONFIG_FRONTEND_SAT)
		case DI_CH_TYPE_SAT:
			nRet = CH_S_GetSignalInfo( nChannelId, &stSignalInfo);
			if( nRet != DI_CH_OK )
			{
				CH_DI_Print(0, ("ERR! CH_S_GetSignalInfo() \n"));
			}
			else
			{
				CH_DI_Print(1, ("\t ucStrength(%d), ucQuality(%d)\n", stSignalInfo.ulStrength, stSignalInfo.ulQuality));
			}
			break;
#endif

#if defined(CONFIG_FRONTEND_TER)
		case DI_CH_TYPE_TER:
			nRet = CH_T_GetSignalInfo( nChannelId, &stSignalInfo);
			if( nRet != DI_CH_OK )
			{
				CH_DI_Print(0, ("ERR! CH_T_GetSignalInfo() \n"));
			}
			else
			{
				CH_DI_Print(1, ("\t ucStrength(%d), ucQuality(%d)\n", stSignalInfo.ulStrength, stSignalInfo.ulQuality));
			}
			break;
#endif

#if defined(CONFIG_FRONTEND_CAB)
		case DI_CH_TYPE_CAB:
			nRet = CH_C_GetSignalInfo( nChannelId, &stSignalInfo );
			if( nRet != DI_CH_OK )
			{
				CH_DI_Print(0, ("ERR! CH_C_GetStrengthQuality() \n"));
			}
			else
			{
				CH_DI_Print(1, ("\t ucStrength(%d), ucQuality(%d)\n", stSignalInfo.ulStrength, stSignalInfo.ulQuality));
			}
			break;
#endif

		case DI_CH_TYPE_ANA:
			break;

		default:
			CH_DI_Print(0, ("ERR! etChannelType(%d) \n", etChannelType));
			break;
	}

	CH_DI_Print(1, ("---------------------------------------------------\n"));
}



#ifdef __CH_MONITOR__

void CH_MON_PrintSatTuneParam (CH_SAT_TuneParam_t* pstInputParam )
{
	CH_PrintMon("---------------------------------------------------\n");
	CH_PrintMon("   SAT TUNE PARAM\n");
	CH_PrintMon("---------------------------------------------------\n");
	//Antenna & DiSEqC info
	CH_PrintMon("### Antenna info ###\n");
	CH_PrintMon("   etAntennaType     %s[%d]\n", 	pszCH_SAT_AntennaType_e[pstInputParam->etAntennaType], pstInputParam->etAntennaType);

	if( pstInputParam->etAntennaType == DI_CH_ANT_TYPE_DISEQC )
	{
		CH_PrintMon("   etDiseqcVersion   %s[%d]\n", 	pszCH_SAT_DiseqcVer_e[pstInputParam->etDiseqcVersion], pstInputParam->etDiseqcVersion);
		CH_PrintMon("   etDiseqcInput     %s[%d]\n", 	pszCH_SAT_DiseqcInput_e[pstInputParam->etDiseqcInput], pstInputParam->etDiseqcInput);
		//CH_PrintMon("   etToneBurst       %s[%d]\n", 	pszCH_SAT_ToneBurst_e[pstInputParam->etToneBurst], pstInputParam->etToneBurst);
	}
	else if( pstInputParam->etAntennaType == DI_CH_ANT_TYPE_SCD )
	{
		CH_PrintMon("### SCIF info ###\n");
		CH_PrintMon("   ulScdMasterUnitId [%d]\n", 	pstInputParam->ulScdMasterUnitId);
		CH_PrintMon("   ulScdUserBandNum  [%d]\n", 	pstInputParam->ulScdUserBandNum);
		CH_PrintMon("   ulScdUserBandFreq [%d]\n", 	pstInputParam->ulScdUserBandFreq);
	}

	if( pstInputParam->etAntennaType != DI_CH_ANT_TYPE_SMATV )
	{
		//LNBR info
		CH_PrintMon("### LNBS info ###\n");
		CH_PrintMon("   etPolarization    %s[%d]\n", 	pszCH_SAT_Polarization_e[pstInputParam->etPolarization], pstInputParam->etPolarization);
		CH_PrintMon("   b22kTone          %s[%d]\n", 	((pstInputParam->b22kTone) ? "On":"Off"), pstInputParam->b22kTone);
		CH_PrintMon("   etLnbVoltage      %s[%d]\n", 	pszCH_SAT_LnbVoltage_e[pstInputParam->etLnbVoltage], pstInputParam->etLnbVoltage);
	}

	//Tuner info
	CH_PrintMon("### Tuning info ###\n");
	CH_PrintMon("   etTransSpec       %s[%d]\n", 	pszCH_SAT_TransportSpec_e[pstInputParam->etTransSpec], pstInputParam->etTransSpec);
	CH_PrintMon("   etModulation      %s[%d]\n", 	pszCH_SAT_Modulation_e[pstInputParam->etModulation], pstInputParam->etModulation);

	CH_PrintMon("   ulFrequency       [%d]MHz\n", 	pstInputParam->ulFrequency);
	CH_PrintMon("   ulSymbolRate      [%d]Ksymbol/sec\n", 	pstInputParam->ulSymbolRate);
	CH_PrintMon("   etFecCodeRate     %s[%d]\n", 	pszCH_SAT_CodeRate_e[pstInputParam->etFecCodeRate], pstInputParam->etFecCodeRate);

	if( pstInputParam->etTransSpec == DI_CH_TRANS_DVBS2 )
	{
		CH_PrintMon("   etPilot           %s[%d]\n", 	pszCH_SAT_Pilot_e[pstInputParam->etPilot], pstInputParam->etPilot);
		CH_PrintMon("   etRollOff         %s[%d]\n", 	pszCH_SAT_RollOff_e[pstInputParam->etRollOff], pstInputParam->etRollOff);
	}
	CH_PrintMon("---------------------------------------------------\n");
}


void CH_MON_PrintTerTuneParam (CH_TER_TuneParam_t* pstInputParam )
{
	CH_TER_CodeRate_e etCodeRate = 0;

//	etCodeRate = CH_T_GetRealCodeRate();

	CH_PrintMon("---------------------------------------------------\n");
	CH_PrintMon("   TER TUNE PARAM\n");
	CH_PrintMon("---------------------------------------------------=\n");
	CH_PrintMon("   ulFrequency      [%d]KHz\n", pstInputParam->ulFrequency);
	CH_PrintMon("   etBandWidth      %s[%d]\n", pszCH_TER_BandWidth_e[pstInputParam->etBandWidth], pstInputParam->etBandWidth);
	CH_PrintMon("   etModulation     %s[%d]\n", pszCH_TER_Moudulation_e[pstInputParam->etModulation], pstInputParam->etModulation);
	CH_PrintMon("   etHierarchy      %s[%d]\n", pszCH_TER_Hierarchy_e[pstInputParam->etHierarchy], pstInputParam->etHierarchy);
	//CH_PrintMon("   etCodeRate       %s[%d]\n", pszCH_TER_CodeRate_e[pstInputParam->etCodeRate], pstInputParam->etCodeRate);
	CH_PrintMon("   etCodeRate       %s[%d]\n", pszCH_TER_CodeRate_e[etCodeRate], etCodeRate);
	CH_PrintMon("   etTransMode      %s[%d]\n", pszCH_TER_TransMode_e[pstInputParam->etTransMode], pstInputParam->etTransMode);
	CH_PrintMon("   etGuardInterval  %s[%d]\n", pszCH_TER_GuardInterval_e[pstInputParam->etGuardInterval], pstInputParam->etGuardInterval);
	CH_PrintMon("   etStream         %s[%d]\n", pszCH_TER_Stream_e[pstInputParam->etStream], pstInputParam->etStream);
	CH_PrintMon("   etOffset         %s[%d]\n", pszCH_TER_Offset_e[pstInputParam->etOffset], pstInputParam->etOffset);
	CH_PrintMon("---------------------------------------------------\n");
}

void CH_MON_PrintCabTuneParam (CH_CAB_TuneParam_t* pstInputParam )
{
	CH_PrintMon("---------------------------------------------------\n");
	CH_PrintMon("   CAB TUNE PARAM\n");
	CH_PrintMon("---------------------------------------------------\n");
	CH_PrintMon("   ulFrequency      [%d]KHz\n", pstInputParam->ulFrequency);
	CH_PrintMon("   ulSymbolRate     [%d]Ksymbol/sec\n", pstInputParam->ulSymbolRate);
	CH_PrintMon("   etModulation     %s[%d]\n", pszCH_CAB_Modulation_e[pstInputParam->etModulation], pstInputParam->etModulation);
	CH_PrintMon("   etSpectrum       %s[%d]\n", pszCH_CAB_Spectrum_e[pstInputParam->etSpectrum], pstInputParam->etSpectrum);
	CH_PrintMon("---------------------------------------------------\n");
}

void CH_MON_PrintAnaTuneParam (CH_ANA_TuneParam_t* pstInputParam )
{
	CH_PrintMon("---------------------------------------------------\n");
	CH_PrintMon("   ANA TUNE PARAM\n");
	CH_PrintMon("---------------------------------------------------\n");
	CH_PrintMon("   ulFrequency      [%d]KHz\n", pstInputParam->ulFrequency);
	CH_PrintMon("   ulFreqOffset     [%d]KHz\n", pstInputParam->ulFreqOffset);
	CH_PrintMon("   etAnalogSystem   %s[%d]\n", pszCH_ANA_System_e[pstInputParam->etAnalogSystem], pstInputParam->etAnalogSystem);
	CH_PrintMon("   bEnableAft       %s[%d]\n", ((pstInputParam->bEnableAft) ? "On":"Off"), pstInputParam->bEnableAft);
	CH_PrintMon("   etTuneMode       %s[%d]\n", pszCH_ANA_TuneMode_e[pstInputParam->etTuneMode], pstInputParam->etTuneMode);
	CH_PrintMon("---------------------------------------------------\n");
}

#endif /* __CH_MONITOR__ */
/*********************** End of File ******************************/
