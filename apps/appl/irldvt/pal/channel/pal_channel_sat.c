/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pal_ch_sat.c
	@brief	  Octo pal channel satellite part

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <math.h>

#include <hlib.h>
//#include <octo_common.h>
//#include <mwc_util.h>

//#include <db_svc.h>
//#include <db_param.h>

#include <pal_channel.h>
#include <pal_channel_sat.h>

#include <di_err.h>
#include <di_channel.h>
#include <di_channel_s.h>

#include "_pal_channel.h"

/*******************************************************************/
/************************     Definition   * ***********************/
/*******************************************************************/



#ifndef M_PI
#define M_PI	3.141592
#endif


/******************************
 *    DiSEqC 1.2 commands     *
 ******************************/
#define MOTOR_NUM_RETRY						3

#define MOTOR_DELAY_NORMAL					100
#define MOTOR_DELAY_RETRY					100

#define MOTOR_POSITION_MAX					48

#define MOTOR_POSITION_INVALID				0xFF
#define MOTOR_DEGREE_INVALID				0x7FFF


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct
{
	HBOOL					bMotorMoving;
	HUINT8					ucMotorPosition;
	HINT16					sMotorDegree;
} MotorContext_t;

typedef struct
{
	HBOOL					 bAvailable;
	PalCh_SatTunerCapacity_t	 stSatTunerInfo;

	HINT32					 nRfInputId;
} _palsat_TunerCapacity_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
//STATIC MotorContext_t				 s_MotorContext[NUM_MAX_ACTION];
STATIC MotorContext_t				 s_MotorContext[8];

STATIC _palsat_TunerCapacity_t		 s_astPalsat_TunerInfo[PAL_MAX_SATTUNER_NO];


/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/

#define _____________ConvPAL2DI_SAT________________
static CH_SAT_Polarization_e pal_ch_sat_ConvPAL2DI_Polarization(DxSat_Polarization_e polarization)
{
	CH_SAT_Polarization_e	ePolarization_DI;

	switch(polarization)
	{
		case eDxSAT_POLAR_HOR :
			ePolarization_DI = DI_CH_POLAR_HOR;
			break;
		case eDxSAT_POLAR_VER :
			ePolarization_DI = DI_CH_POLAR_VER;
			break;
		case eDxSAT_POLAR_LEFT :
			ePolarization_DI = DI_CH_POLAR_LEFT;
			break;
		case eDxSAT_POLAR_RIGHT :
			ePolarization_DI = DI_CH_POLAR_RIGHT;
			break;
		case eDxSAT_POLAR_AUTO :
			ePolarization_DI = DI_CH_POLAR_AUTO;
			break;
		default :
			HxLOG_Critical("\n\n");
			ePolarization_DI = 0xFF;
	}

	return ePolarization_DI;
}

static CH_SAT_CodeRate_e pal_ch_sat_ConvPAL2DI_CodeRate(DxSat_CodeRate_e codeRate)
{
	CH_SAT_CodeRate_e	eCodeRate_DI;

	switch(codeRate)
	{
		case eDxSAT_CODERATE_1_2 :
			eCodeRate_DI = DI_CH_SCODERATE_1_2;
			break;
		case eDxSAT_CODERATE_2_3 :
			eCodeRate_DI = DI_CH_SCODERATE_2_3;
			break;
		case eDxSAT_CODERATE_3_4 :
			eCodeRate_DI = DI_CH_SCODERATE_3_4;
			break;
		case eDxSAT_CODERATE_3_5 :
			eCodeRate_DI = DI_CH_S2CODERATE_3_5;
			break;
		case eDxSAT_CODERATE_4_5 :
			eCodeRate_DI = DI_CH_S2CODERATE_4_5;
			break;
		case eDxSAT_CODERATE_5_6 :
			eCodeRate_DI = DI_CH_SCODERATE_5_6;
			break;
		case eDxSAT_CODERATE_5_11 :
			eCodeRate_DI = DI_CH_SCODERATE_5_11;
			break;
		case eDxSAT_CODERATE_6_7 :
			eCodeRate_DI = DI_CH_SCODERATE_6_7;
			break;
		case eDxSAT_CODERATE_7_8 :
			eCodeRate_DI = DI_CH_SCODERATE_7_8;
			break;
		case eDxSAT_CODERATE_8_9 :
			eCodeRate_DI = DI_CH_S2CODERATE_8_9;
			break;
		case eDxSAT_CODERATE_9_10 :
			eCodeRate_DI = DI_CH_S2CODERATE_9_10;
			break;
		case eDxSAT_CODERATE_AUTO :
			eCodeRate_DI = DI_CH_SCODERATE_AUTO;
			break;
		default :
			HxLOG_Critical("\n\n");
			eCodeRate_DI = 0xFF;
			break;
	}

	return eCodeRate_DI;
}


static CH_SAT_LnbVoltage_e pal_ch_sat_ConvPAL2DI_LnbVoltage(DxSAT_LnbVoltage_e lnbVoltage)
{
	CH_SAT_LnbVoltage_e	eLnbVoltage_DI;

	switch(lnbVoltage)
	{
		case eDxLNB_VOLT_STD :
			eLnbVoltage_DI = DI_CH_LNB_VOLT_STD;
			break;
		case eDxLNB_VOLT_HIGH :
			eLnbVoltage_DI = DI_CH_LNB_VOLT_HIGH;
			break;
		default :
			HxLOG_Critical("\n\n");
			eLnbVoltage_DI = 0xFF;
			break;
	}

	return eLnbVoltage_DI;
}

static CH_SAT_DiseqcInput_e pal_ch_sat_ConvPAL2DI_DiseqcInput(DxSAT_DiseqcInput_e diseqcInput)
{
	CH_SAT_DiseqcInput_e	eDiseqcInput_DI;

	switch(diseqcInput)
	{
		case eDxDISEQC_INPUT_NONE :
			eDiseqcInput_DI = DI_CH_DISEQC_INPUT_NONE;
			break;
		case eDxDISEQC_INPUT_A :
			eDiseqcInput_DI = DI_CH_DISEQC_INPUT_A;
			break;
		case eDxDISEQC_INPUT_B :
			eDiseqcInput_DI = DI_CH_DISEQC_INPUT_B;
			break;
		case eDxDISEQC_INPUT_C :
			eDiseqcInput_DI = DI_CH_DISEQC_INPUT_C;
			break;
		case eDxDISEQC_INPUT_D :
			eDiseqcInput_DI = DI_CH_DISEQC_INPUT_D;
			break;
		default :
			HxLOG_Critical("\n\n");
			eDiseqcInput_DI = 0xFF;
			break;
	}

	return eDiseqcInput_DI;
}

static CH_SAT_ToneBurst_e pal_ch_sat_ConvPAL2DI_ToneBurst(DxSAT_ToneBurst_e toneBurst)
{
	CH_SAT_ToneBurst_e	toneBurst_DI;

	switch(toneBurst)
	{
		case eDxTONE_BURST_NONE :
			toneBurst_DI = DI_CH_TONE_BURST_NONE;
			break;

		case eDxTONE_BURST_A :
			toneBurst_DI = DI_CH_TONE_BURST_A;
			break;

		case eDxTONE_BURST_B :
			toneBurst_DI = DI_CH_TONE_BURST_B;
			break;

		default :
			HxLOG_Critical("\n\n");
			toneBurst_DI = 0xff;
			break;
	}

	return toneBurst_DI;
}

static CH_SAT_DiseqcVer_e pal_ch_sat_ConvPAL2DI_DiseqcVersion(DxSAT_DiseqcVer_e diseqcVer)
{
	CH_SAT_DiseqcVer_e	eDiseqcVersion_DI;

	switch(diseqcVer)
	{
		case eDxDISEQC_VER_1_0 :
			eDiseqcVersion_DI = DI_CH_DISEQC_VER_1_0;
			break;
		case eDxDISEQC_VER_1_1 :
			eDiseqcVersion_DI = DI_CH_DISEQC_VER_1_1;
			break;
		case eDxDISEQC_VER_1_2 :
			eDiseqcVersion_DI = DI_CH_DISEQC_VER_1_2;
			break;
		case eDxDISEQC_VER_1_3 :
			eDiseqcVersion_DI = DI_CH_DISEQC_VER_1_3;
			break;
		case eDxDISEQC_VER_2_0 :
			eDiseqcVersion_DI = DI_CH_DISEQC_VER_2_0;
			break;
		default :
			HxLOG_Critical("\n\n");
			eDiseqcVersion_DI = 0xFF;
			break;
	}

	return eDiseqcVersion_DI;
}

static CH_SAT_TransportSpec_e pal_ch_sat_ConvPAL2DI_TransSpec(DxSat_TransportSpec_e transSpec)
{
	CH_SAT_TransportSpec_e	eTransSpec_DI;

	switch(transSpec)
	{
		case eDxSAT_TRANS_DVBS :
			eTransSpec_DI = DI_CH_TRANS_DVBS;
			break;
		case eDxSAT_TRANS_DVBS2 :
			eTransSpec_DI = DI_CH_TRANS_DVBS2;
			break;
		default :
			HxLOG_Critical("\n\n");
			eTransSpec_DI = 0xFF;
			break;
	}

	return eTransSpec_DI;
}

static CH_SAT_Modulation_e pal_ch_sat_ConvPAL2DI_Modulation(DxSat_PskModulation_e pskModulation)
{
	CH_SAT_Modulation_e	eModulation_DI;

	switch(pskModulation)
	{
		case eDxSAT_PSK_QPSK :
			eModulation_DI = DI_CH_PSK_QPSK;
			break;
		case eDxSAT_PSK_8PSK :
			eModulation_DI = DI_CH_PSK_8PSK;
			break;
		case eDxSAT_PSK_AUTO :
			eModulation_DI = DI_CH_PSK_AUTO;
			break;
		default :
			HxLOG_Critical("\n\n");
			eModulation_DI = 0xFF;
			break;
	}

	return eModulation_DI;
}

static CH_SAT_Pilot_e pal_ch_sat_ConvPAL2DI_Pilot(DxSat_Pilot_e pilot)
{
	CH_SAT_Pilot_e	ePilot_DI;

	switch(pilot)
	{
		case eDxSAT_PILOT_OFF :
			ePilot_DI = DI_CH_PILOT_OFF;
			break;
		case eDxSAT_PILOT_ON :
			ePilot_DI = DI_CH_PILOT_ON;
			break;
		case eDxSAT_PILOT_AUTO :
			ePilot_DI = DI_CH_PILOT_AUTO;
			break;
		default :
			HxLOG_Critical("\n\n");
			ePilot_DI = 0xFF;
			break;
	}

	return ePilot_DI;
}



#define _______________ConvDI2PAL_CH_SAT___________________ /* 검색시에 AUTO로 설정되는 파라미터들만 DDI 값을 참조 */

DxSat_Polarization_e pal_ch_sat_ConvDI2PAL_Polarization(CH_SAT_Polarization_e ePolarization_DI)
{
	DxSat_Polarization_e	polarization_PAL;

	switch(ePolarization_DI)
	{
		case DI_CH_POLAR_HOR :
			polarization_PAL = eDxSAT_POLAR_HOR;
			break;
		case DI_CH_POLAR_VER :
			polarization_PAL = eDxSAT_POLAR_VER;
			break;
		case DI_CH_POLAR_LEFT :
			polarization_PAL = eDxSAT_POLAR_LEFT;
			break;
		case DI_CH_POLAR_RIGHT :
			polarization_PAL = eDxSAT_POLAR_RIGHT;
			break;
		case DI_CH_POLAR_AUTO :
			polarization_PAL = eDxSAT_POLAR_AUTO;
			break;
		default :
			HxLOG_Critical("\n\n");
			polarization_PAL = 0xFF;
	}

	return polarization_PAL;
}

DxSat_PskModulation_e pal_ch_sat_ConvDI2PAL_Modulation(CH_SAT_Modulation_e eModulation_DI)
{
	DxSat_PskModulation_e	pskMod_PAL;

	switch(eModulation_DI)
	{
		case DI_CH_PSK_QPSK :
			pskMod_PAL = eDxSAT_PSK_QPSK;
			break;
		case DI_CH_PSK_8PSK :
			pskMod_PAL = eDxSAT_PSK_8PSK;
			break;
		case DI_CH_PSK_AUTO :
			pskMod_PAL = eDxSAT_PSK_AUTO;
			break;
		default :
			HxLOG_Critical("\n\n");
			pskMod_PAL = 0xFF;
			break;
	}

	return pskMod_PAL;
}

DxSat_CodeRate_e pal_ch_sat_ConvDI2PAL_CodeRate(CH_SAT_CodeRate_e eCodeRate_DI)
{
	DxSat_CodeRate_e	codeRate_PAL;

	switch(eCodeRate_DI)
	{
		case DI_CH_SCODERATE_1_2 :
			codeRate_PAL = eDxSAT_CODERATE_1_2;
			break;
		case DI_CH_SCODERATE_2_3 :
			codeRate_PAL = eDxSAT_CODERATE_2_3;
			break;
		case DI_CH_SCODERATE_3_4 :
			codeRate_PAL = eDxSAT_CODERATE_3_4;
			break;
		case DI_CH_S2CODERATE_3_5 :
			codeRate_PAL = eDxSAT_CODERATE_3_5;
			break;
		case DI_CH_S2CODERATE_4_5 :
			codeRate_PAL = eDxSAT_CODERATE_4_5;
			break;
		case DI_CH_SCODERATE_5_6 :
			codeRate_PAL = eDxSAT_CODERATE_5_6;
			break;
		case DI_CH_SCODERATE_5_11 :
			codeRate_PAL = eDxSAT_CODERATE_5_11;
			break;
		case DI_CH_SCODERATE_6_7 :
			codeRate_PAL = eDxSAT_CODERATE_6_7;
			break;
		case DI_CH_SCODERATE_7_8 :
			codeRate_PAL = eDxSAT_CODERATE_7_8;
			break;
		case DI_CH_S2CODERATE_8_9 :
			codeRate_PAL = eDxSAT_CODERATE_8_9;
			break;
		case DI_CH_S2CODERATE_9_10 :
			codeRate_PAL = eDxSAT_CODERATE_9_10;
			break;
		case DI_CH_SCODERATE_AUTO :
			codeRate_PAL = eDxSAT_CODERATE_AUTO;
			break;
		default :
			HxLOG_Critical("\n\n");
			codeRate_PAL = 0xFF;
			break;
	}

	return codeRate_PAL;
}

DxSat_Pilot_e pal_ch_sat_ConvDI2PAL_Pilot(CH_SAT_Pilot_e ePilot_DI)
{
	DxSat_Pilot_e	pilot_PAL;

	switch(ePilot_DI)
	{
		case DI_CH_PILOT_OFF :
			pilot_PAL = eDxSAT_PILOT_OFF;
			break;
		case DI_CH_PILOT_ON :
			pilot_PAL = eDxSAT_PILOT_ON;
			break;
		case DI_CH_PILOT_AUTO :
			pilot_PAL = eDxSAT_PILOT_AUTO;
			break;
		default :
			HxLOG_Critical("\n\n");
			pilot_PAL = 0xFF;
			break;
	}

	return pilot_PAL;
}

HINT8	*pal_ch_sat_GetCodeRateStr(DxSat_CodeRate_e fecCodeRate)
{
	switch(fecCodeRate)
	{
		ENUM_TO_STR(eDxSAT_CODERATE_NONE)
		ENUM_TO_STR(eDxSAT_CODERATE_1_2)
		ENUM_TO_STR(eDxSAT_CODERATE_2_3)
		ENUM_TO_STR(eDxSAT_CODERATE_3_4)
		ENUM_TO_STR(eDxSAT_CODERATE_5_6)
		ENUM_TO_STR(eDxSAT_CODERATE_7_8)
		ENUM_TO_STR(eDxSAT_CODERATE_3_5)
		ENUM_TO_STR(eDxSAT_CODERATE_4_5)
		ENUM_TO_STR(eDxSAT_CODERATE_5_11)
		ENUM_TO_STR(eDxSAT_CODERATE_6_7)
		ENUM_TO_STR(eDxSAT_CODERATE_8_9)
		ENUM_TO_STR(eDxSAT_CODERATE_9_10)
		ENUM_TO_STR(eDxSAT_CODERATE_AUTO)
	}

	return "UNKNOWN";
}

HINT8	*pal_ch_sat_GetPolarizationStr(DxSat_Polarization_e polarization)
{
	switch(polarization)
	{
		ENUM_TO_STR(eDxSAT_POLAR_HOR)
		ENUM_TO_STR(eDxSAT_POLAR_VER)
		ENUM_TO_STR(eDxSAT_POLAR_LEFT)
		ENUM_TO_STR(eDxSAT_POLAR_RIGHT)
		ENUM_TO_STR(eDxSAT_POLAR_AUTO)
	}

	return "UNKNOWN";
}

HINT8	*pal_ch_sat_GetTransSpecStr(DxSat_TransportSpec_e transSpec)
{
	switch(transSpec)
	{
		ENUM_TO_STR(eDxSAT_TRANS_DVBS)
		ENUM_TO_STR(eDxSAT_TRANS_DVBS2)
	}

	return "UNKNOWN";
}

HINT8	*pal_ch_sat_GetPskModStr(DxSat_PskModulation_e pskMod)
{
	switch(pskMod)
	{
		ENUM_TO_STR(eDxSAT_PSK_QPSK)
		ENUM_TO_STR(eDxSAT_PSK_8PSK)
		ENUM_TO_STR(eDxSAT_PSK_AUTO)
	}

	return "UNKNOWN";
}

HINT8	*pal_ch_sat_GetLnbVoltageStr(DxSAT_LnbVoltage_e lnbVoltage)
{
	switch(lnbVoltage)
	{
		ENUM_TO_STR(eDxLNB_VOLT_STD)
		ENUM_TO_STR(eDxLNB_VOLT_HIGH)
	}

	return "UNKNOWN";
}

HINT8	*pal_ch_sat_GetDiseqcVerStr(DxSAT_DiseqcVer_e diseqcVer)
{
	switch(diseqcVer)
	{
		ENUM_TO_STR(eDxDISEQC_VER_1_0)
		ENUM_TO_STR(eDxDISEQC_VER_1_1)
		ENUM_TO_STR(eDxDISEQC_VER_1_2)
		ENUM_TO_STR(eDxDISEQC_VER_1_3)
		ENUM_TO_STR(eDxDISEQC_VER_2_0)
	}

	return "UNKNOWN";
}

HINT8	*pal_ch_sat_GetDiseqcInputStr(DxSAT_DiseqcInput_e diseqcInput)
{
	switch(diseqcInput)
	{
		ENUM_TO_STR(eDxDISEQC_INPUT_NONE)
		ENUM_TO_STR(eDxDISEQC_INPUT_A)
		ENUM_TO_STR(eDxDISEQC_INPUT_B)
		ENUM_TO_STR(eDxDISEQC_INPUT_C)
		ENUM_TO_STR(eDxDISEQC_INPUT_D)
	}

	return "UNKNOWN";
}

HINT8	*pal_ch_sat_GetToneBurstStr(DxSAT_ToneBurst_e toneBurst)
{
	switch(toneBurst)
	{
		ENUM_TO_STR(eDxTONE_BURST_NONE)
		ENUM_TO_STR(eDxTONE_BURST_A)
		ENUM_TO_STR(eDxTONE_BURST_B)
	}

	return "UNKNOWN";
}

HINT8	*pal_ch_sat_GetScdInputStr(DxSAT_ScdInput_e scdInput)
{
	switch(scdInput)
	{
		ENUM_TO_STR(eDxSCD_INPUT_NONE)
		ENUM_TO_STR(eDxSCD_INPUT_SCD_A)
		ENUM_TO_STR(eDxSCD_INPUT_SCD_B)
	}

	return "UNKNOWN";
}

void pal_ch_sat_PrintDiParams(CH_SAT_TuneParam_t		*pstDiParam)
{
#ifdef CONFIG_DEBUG
	{
		HxLOG_Print("===================================================\n");
		HxLOG_Print("   SAT Tuning DI Parameters\n");
		HxLOG_Print("===================================================\n");
		HxLOG_Print("   etAntennaType     [%d]\n", pstDiParam->etAntennaType);
		HxLOG_Print("   ulFrequency       [%d]MHz\n", pstDiParam->ulFrequency);
		HxLOG_Print("   ulSymbolRate      [%d]\n", pstDiParam->ulSymbolRate);
		HxLOG_Print("   etFecCodeRate     [%d]", pstDiParam->etFecCodeRate);
		switch(pstDiParam->etFecCodeRate)
		{
			case DI_CH_SCODERATE_1_2: 	HxLOG_Print("\t--> 1/2\n"); 		break;
			case DI_CH_SCODERATE_2_3: 	HxLOG_Print("\t--> 2/3\n"); 		break;
			case DI_CH_SCODERATE_3_4: 	HxLOG_Print("\t--> 3/4\n"); 		break;
			case DI_CH_SCODERATE_5_6: 	HxLOG_Print("\t--> 5/6\n"); 		break;

			case DI_CH_SCODERATE_7_8: 	HxLOG_Print("\t--> 7/8\n"); 		break;
			case DI_CH_S2CODERATE_4_5: 	HxLOG_Print("\t--> 4/5\n"); 		break;
			case DI_CH_S2CODERATE_3_5: 	HxLOG_Print("\t--> 3/5\n"); 		break;
			case DI_CH_SCODERATE_6_7: 	HxLOG_Print("\t--> 6/7\n"); 		break;
			case DI_CH_SCODERATE_5_11: HxLOG_Print("\t--> 5/11\n"); 		break;
			case DI_CH_S2CODERATE_8_9: 	HxLOG_Print("\t--> 8/9\n"); 		break;
			case DI_CH_S2CODERATE_9_10: HxLOG_Print("\t--> 9/10\n"); 		break;
			case DI_CH_SCODERATE_AUTO: 	HxLOG_Print("\t--> AUTO\n"); 		break;
			default:					HxLOG_Print("\t--> unknown\n"); 	break;
		}
		HxLOG_Print("   etPolarization    [%d]", pstDiParam->etPolarization);
		switch(pstDiParam->etPolarization)
		{
			case DI_CH_POLAR_HOR: 	HxLOG_Print("\t--> HOR\n"); 		break;
			case DI_CH_POLAR_VER: 	HxLOG_Print("\t--> VER\n"); 		break;
			case DI_CH_POLAR_LEFT: 	HxLOG_Print("\t--> LEFT\n"); 	break;
			case DI_CH_POLAR_RIGHT: HxLOG_Print("\t--> RIGHT\n"); 	break;
			case DI_CH_POLAR_AUTO: 	HxLOG_Print("\t--> AUTO\n"); 	break;
			default:				HxLOG_Print("\t--> unknown\n"); 	break;
		}
		HxLOG_Print("   etTransSpec       [%d]", pstDiParam->etTransSpec);
		switch(pstDiParam->etTransSpec)
		{
			case DI_CH_TRANS_DVBS: 	HxLOG_Print("\t-->DVBS\n"); 		break;
			case DI_CH_TRANS_DVBS2: HxLOG_Print("\t-->DVBS2\n"); 	break;
			default:				HxLOG_Print("\t-->unknown\n"); 	break;
		}
		HxLOG_Print("   etModulation      [%d]", pstDiParam->etModulation);
		switch(pstDiParam->etModulation)
		{
			case DI_CH_PSK_QPSK: 	HxLOG_Print("\t-->QPSK\n"); 		break;
			case DI_CH_PSK_8PSK: 	HxLOG_Print("\t-->8PSK\n"); 		break;
			case DI_CH_PSK_AUTO: 	HxLOG_Print("\t-->Auto\n"); 		break;
			default:				HxLOG_Print("\t-->unknown\n"); 	break;
		}
		HxLOG_Print("   etPilot           [%d]\n", pstDiParam->etPilot);
		HxLOG_Print("   etRollOff         [%d]\n", pstDiParam->etRollOff);
		HxLOG_Print("   etLnbVoltage      [%d]", pstDiParam->etLnbVoltage);
		switch(pstDiParam->etLnbVoltage)
		{
			case DI_CH_LNB_VOLT_STD: 	HxLOG_Print("\t-->STANDARD\n"); 		break;
			case DI_CH_LNB_VOLT_HIGH: 	HxLOG_Print("\t-->HIGH\n"); 		break;
			default:				HxLOG_Print("\t-->unknown\n"); 	break;
		}
		HxLOG_Print("   etDiseqcVersion   [%d]", pstDiParam->etDiseqcVersion);
		switch(pstDiParam->etDiseqcVersion)
		{
			case DI_CH_DISEQC_VER_1_0: 		HxLOG_Print("\t--> 1.0\n"); 	break;
			case DI_CH_DISEQC_VER_1_1: 		HxLOG_Print("\t--> 1.1\n"); 	break;
			case DI_CH_DISEQC_VER_1_2: 		HxLOG_Print("\t--> 1.2\n"); 	break;
			case DI_CH_DISEQC_VER_1_3: 		HxLOG_Print("\t--> 1.3\n"); 	break;
			case DI_CH_DISEQC_VER_2_0: 		HxLOG_Print("\t--> 2.0\n"); 	break;
			default:						HxLOG_Print("\t--> unknown\n"); break;
		}
		HxLOG_Print("   etDiseqcInput     [%d]", pstDiParam->etDiseqcInput);
		switch(pstDiParam->etDiseqcInput)
		{
			case DI_CH_DISEQC_INPUT_NONE: 	HxLOG_Print("\t--> None\n"); 	break;
			case DI_CH_DISEQC_INPUT_A: 		HxLOG_Print("\t--> A\n"); 		break;
			case DI_CH_DISEQC_INPUT_B: 		HxLOG_Print("\t--> B\n"); 		break;
			case DI_CH_DISEQC_INPUT_C: 		HxLOG_Print("\t--> C\n"); 		break;
			case DI_CH_DISEQC_INPUT_D: 		HxLOG_Print("\t--> D\n"); 		break;
			default:						HxLOG_Print("\t--> unknown\n"); 	break;
		}
		HxLOG_Print("   etToneBurst       [%d]", pstDiParam->etToneBurst);
		switch(pstDiParam->etToneBurst)
		{
			case DI_CH_TONE_BURST_NONE: 	HxLOG_Print("\t--> NONE\n"); 	break;
			case DI_CH_TONE_BURST_A: 		HxLOG_Print("\t--> A\n"); 		break;
			case DI_CH_TONE_BURST_B: 		HxLOG_Print("\t--> B\n"); 		break;
			default:						HxLOG_Print("\t--> unknown\n");	break;
		}
		HxLOG_Print("   b22kTone          [%d]\n", pstDiParam->b22kTone);
		HxLOG_Print("===================================================\n");
	}
#endif
}

#define _______________RF_INPUT_SETTING___________________
STATIC HUINT32					 s_ulPalChSat_RfInputNum = 0;
STATIC PalCh_SatRfInput_t			 s_astPalChSat_RfInput[PAL_MAX_RFINPUT_NO];

STATIC HERROR pal_ch_sat_readTunerCapacity (void)
{
	HUINT32					 ulTunerGroupId, ulTunerNum = PAL_CH_GetNumOfTunerGroup();
	HUINT32					 ulRfInputCnt;
	HUINT32					 ulDeliType;
	DI_CH_TunerCapacity_t	 stTuneCapacity;
	DI_CHANNEL_ERR			 eDiErr;

	if (ulTunerNum > PAL_MAX_SATTUNER_NO)
	{
		HxLOG_Error ("DI_CH_S_GetTunerCapability error !!!!\n\n\n");
		HxLOG_Assert (0);

		return ERR_FAIL;
	}

	for (ulTunerGroupId = 0; ulTunerGroupId < ulTunerNum; ulTunerGroupId++)
	{
		PAL_CH_GetSupportDeliveryType (ulTunerGroupId, &ulDeliType);
		if (0 != (ulDeliType & eDxDELIVERY_TYPE_SAT))
		{
			eDiErr = DI_CH_S_GetTunerCapability (ulTunerGroupId, &stTuneCapacity);
			if (DI_CH_OK == eDiErr)
			{
				_palsat_TunerCapacity_t		*pstTunerInfo = s_astPalsat_TunerInfo + ulTunerGroupId;

				pstTunerInfo->bAvailable	= TRUE;
				pstTunerInfo->stSatTunerInfo.ulTunerGroupId = ulTunerGroupId;
				pstTunerInfo->stSatTunerInfo.ulNumRfInput = stTuneCapacity.ulNumOfInput;
				pstTunerInfo->stSatTunerInfo.bLoopThroughOut = stTuneCapacity.bLoopThroughOut;

				for (ulRfInputCnt = 0; ulRfInputCnt < stTuneCapacity.ulNumOfInput; ulRfInputCnt++)
				{
					pstTunerInfo->stSatTunerInfo.aulRfInputId[ulRfInputCnt] = (HUINT32)stTuneCapacity.anInputId[ulRfInputCnt];
				}
			}
			else
			{
				HxLOG_Error ("DI_CH_S_GetTunerCapability error !!!!\n\n\n");
				HxLOG_Assert (0);
			}
		}
	}

	return ERR_OK;
}

STATIC HERROR pal_ch_sat_getRfInputCapacity (void)
{
	HUINT32						 ulCount, ulRfInputIdx;
	HUINT32						 ulTunerIdx, ulTunerNum = PAL_CH_GetNumOfTunerGroup();
	PalCh_SatRfInput_t 			*pstRfInput;

	s_ulPalChSat_RfInputNum = 0;
	HxSTD_MemSet (s_astPalChSat_RfInput, 0, sizeof(PalCh_SatRfInput_t) * PAL_MAX_RFINPUT_NO);

	for (ulTunerIdx = 0; ulTunerIdx < ulTunerNum; ulTunerIdx++)
	{
		_palsat_TunerCapacity_t 	*pstTunerInfo = s_astPalsat_TunerInfo + ulTunerIdx;

		if (TRUE == pstTunerInfo->bAvailable)
		{
			for (ulCount = 0; ulCount < pstTunerInfo->stSatTunerInfo.ulNumRfInput; ulCount++)
			{
				ulRfInputIdx = pstTunerInfo->stSatTunerInfo.aulRfInputId[ulCount];
				if (ulRfInputIdx >= PAL_MAX_RFINPUT_NO)
				{
					HxLOG_Error ("Something wrong! Tuner Capacity Error !!!!!\n");
					HxLOG_Assert (0);
					break;
				}

				pstRfInput = &(s_astPalChSat_RfInput[ulRfInputIdx]);

				pstRfInput->bAvailable = TRUE;
				pstRfInput->aulConnectableTunerId [pstRfInput->ulConnectableTunerNum] = ulTunerIdx;
				pstRfInput->ulConnectableTunerNum ++;

				s_ulPalChSat_RfInputNum ++;
			}
		}
	}

	// RF Input Index가 0 ~ Num-1 임을 체크: Debug Mode일 때 System Setting이 제대로 되었는지 확인할 때 쓰면 좋음.
#if 1
	{
		HBOOL						 bRfInputContinuous = TRUE;

		s_ulPalChSat_RfInputNum = 0;

		for (ulRfInputIdx = 0; ulRfInputIdx < PAL_MAX_RFINPUT_NO; ulRfInputIdx++)
		{
			pstRfInput = &(s_astPalChSat_RfInput[ulRfInputIdx]);

			if (TRUE == pstRfInput->bAvailable)
			{
				if (TRUE == bRfInputContinuous)
				{
					s_ulPalChSat_RfInputNum ++;
				}
				else
				{
					HxLOG_Error ("Something wrong! Tuner Capacity Error !!!!!\n\n\n\n\n\n\n");
					HxLOG_Assert (0);

					return ERR_FAIL;
				}
			}
			else
			{
				bRfInputContinuous = FALSE;
			}
		}
	}
#endif

#if 0
	{
		HUINT32				 ulRfInputNum = 0;


		HLIB_LOG_SetLevel (HxLOG_DOMAIN, HxLOG_ALL);

		HxLOG_Print ("[%s:%d] Printing RF-Input capacity... Number of RF-Input=%d\n", __FUNCTION__, __LINE__, s_ulPalChSat_RfInputNum);

		for (ulCount = 0; ulCount < s_ulPalChSat_RfInputNum; ulCount++)
		{
			PalCh_SatRfInput_t 			*pstRfInput = &(s_astPalChSat_RfInput[ulCount]);

			if (TRUE == pstRfInput->bAvailable)
			{
				HxLOG_Print ("[%s:%d] RF Input ID (%d)\n", __FUNCTION__, __LINE__, ulCount);
				for (ulTunerIdx = 0; ulTunerIdx < pstRfInput->ulConnectableTunerNum; ulTunerIdx++)
				{
					HxLOG_Print ("[%s:%d]     Connectable Tuner : %d\n", __FUNCTION__, __LINE__, pstRfInput->aulConnectableTunerId[ulTunerIdx]);
				}
				HxLOG_Print ("\n");
				ulRfInputNum ++;
			}
		}

		HxLOG_Print ("[%s:%d] RF Input Num (%d)\n", __FUNCTION__, __LINE__, ulRfInputNum);
	}
#endif

	return ERR_OK;
}

#define _______________EXTERN_API_________________

/****************************************************************
************************** EXTERN API ****************************
****************************************************************/
HERROR PAL_CH_SAT_Init (void)
{
	HUINT32			 ulCount;
	HERROR			 hErr;

	HxSTD_MemSet (s_astPalsat_TunerInfo, 0, sizeof(_palsat_TunerCapacity_t) * PAL_MAX_SATTUNER_NO);

	for (ulCount = 0; ulCount < PAL_MAX_SATTUNER_NO; ulCount++)
	{
		s_astPalsat_TunerInfo[ulCount].nRfInputId = -1;
	}

	hErr = pal_ch_sat_readTunerCapacity();
	if (ERR_OK == hErr)
	{
		hErr = pal_ch_sat_getRfInputCapacity();
	}

	return hErr;
}

HERROR PAL_CH_SAT_GetTunerCapacity (HUINT32 ulTunerGroupId, PalCh_SatTunerCapacity_t *pstSatTunerCap)
{
	_palsat_TunerCapacity_t		*pstTunerCap;

	if ((ulTunerGroupId >= PAL_MAX_SATTUNER_NO) || (NULL == pstSatTunerCap))
	{
		return ERR_FAIL;
	}

	pstTunerCap = s_astPalsat_TunerInfo + ulTunerGroupId;
	if (TRUE != pstTunerCap->bAvailable)
	{
		return ERR_FAIL;
	}

	HxSTD_MemCopy (pstSatTunerCap, &(pstTunerCap->stSatTunerInfo), sizeof(PalCh_SatTunerCapacity_t));
	return ERR_OK;
}

HERROR PAL_CH_LNB_StartTune(
						HUINT32							ulTunerGroupId,
						HUINT32							ulRequestId,
						HUINT32							ulFrequency,
						HUINT32							ulSymbolRate,
						DxSat_CodeRate_e					fecCodeRate,
						DxSat_Polarization_e				polarization,
						DxSat_TransportSpec_e				transSpec,
						DxSat_PskModulation_e 			pskMod,
						DxSat_Pilot_e						pilot,
						HUINT32 						ulLnbFreq,
						DxSAT_LnbVoltage_e 				lnbVoltage
						)
{
	CH_SAT_TuneParam_t		stDiParam;
	HERROR					err;

	HxLOG_Info("PAL_CH_LNB_StartTune()\n");

	HxSTD_memset(&stDiParam, 0, sizeof(CH_SAT_TuneParam_t)); /* 이거 안하면 garbage 때문에 다른 튜닝 파라미터로 인식함. */

#ifdef CONFIG_DEBUG
	{
		HxLOG_Print("xxxxxxxxxxxxxxxxxxxxx Tuning Parameters for LNB Only Antenna Type xxxxxxxxxxxxxxxxxxx\n");
		HxLOG_Print("\tulFrequency        [%d]MHz\n", ulFrequency);
		HxLOG_Print("\tulSymbolRate       [%d]\n", ulSymbolRate);
		HxLOG_Print("\tfecCodeRate        [%s]\n", pal_ch_sat_GetCodeRateStr(fecCodeRate));
		HxLOG_Print("\tpolarization       [%s]\n", pal_ch_sat_GetPolarizationStr(polarization));
		HxLOG_Print("\ttransSpec          [%s]\n", pal_ch_sat_GetTransSpecStr(transSpec));
		HxLOG_Print("\tpskMod             [%s]\n", pal_ch_sat_GetPskModStr(pskMod));
		HxLOG_Print("\tpilot              [%d]\n", pilot);
		HxLOG_Print("\tulLnbFreq          [%d]MHz\n", ulLnbFreq);
		HxLOG_Print("\tlnbVoltage         [%s]\n", pal_ch_sat_GetLnbVoltageStr(lnbVoltage));
		HxLOG_Print("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
	}
#endif
	if (ulLnbFreq == CH_UNIVERSAL_LNB_FREQ)
	{
		ulLnbFreq = PAL_CH_LNB_GetActualLnbFreq(ulFrequency);
		stDiParam.b22kTone = PAL_CH_LNB_GetActual22KTone(ulFrequency);
	}
	else if(ulLnbFreq >= (HUINT32)CH_MDU4_LNB_FREQ && ulLnbFreq <= (HUINT32)CH_MDU1_LNB_FREQ)
	{
		ulLnbFreq = PAL_CH_LNB_GetActualMduLnbFreq(ulLnbFreq, polarization, ulFrequency);
		stDiParam.b22kTone = FALSE;
	}
	else
	{
		stDiParam.b22kTone = FALSE;
	}

	stDiParam.etAntennaType = DI_CH_ANT_TYPE_LNB;
	stDiParam.ulFrequency = (HUINT32)abs((HINT32)ulFrequency - (HINT32)ulLnbFreq);
	stDiParam.etLnbVoltage = pal_ch_sat_ConvPAL2DI_LnbVoltage(lnbVoltage);
	stDiParam.etDiseqcInput = DI_CH_DISEQC_INPUT_NONE;
	stDiParam.etToneBurst = DI_CH_TONE_BURST_NONE;
	stDiParam.etDiseqcVersion = DI_CH_DISEQC_VER_1_0;
	stDiParam.ulSymbolRate = ulSymbolRate; /* SDB, MW, DI : kSymbols/s */
	stDiParam.etPolarization = pal_ch_sat_ConvPAL2DI_Polarization(polarization);
	stDiParam.etFecCodeRate = pal_ch_sat_ConvPAL2DI_CodeRate(fecCodeRate);
	stDiParam.etTransSpec = pal_ch_sat_ConvPAL2DI_TransSpec(transSpec);
	stDiParam.etModulation = pal_ch_sat_ConvPAL2DI_Modulation(pskMod);
	stDiParam.etPilot = pal_ch_sat_ConvPAL2DI_Pilot(pilot);
	stDiParam.etRollOff = DI_CH_ROLL_035;

	err = DI_CH_S_RequestLock(ulTunerGroupId,ulRequestId, &stDiParam);
	if(err != DI_CH_OK)
	{
		HxLOG_Error("Error from DI_CH_S_RequestLock() in PAL_CH_LNB_StartTune(): %d\n", err);
		return ERR_FAIL;
	}

	pal_ch_sat_PrintDiParams(&stDiParam);

	return ERR_OK;
}


HERROR PAL_CH_DISEQC_StartTune(
						HUINT32							ulTunerGroupId,
						HUINT32							ulRequestId,
						HUINT32							ulFrequency,
						HUINT32							ulSymbolRate,
						DxSat_CodeRate_e					fecCodeRate,
						DxSat_Polarization_e				polarization,
						DxSat_TransportSpec_e				transSpec,
						DxSat_PskModulation_e 			pskMod,
						DxSat_Pilot_e						pilot,
						HUINT32 						ulLnbFreq,
						DxSAT_LnbVoltage_e 				lnbVoltage,
						HBOOL							b22kTone,
						DxSAT_DiseqcVer_e					diseqcVersion,
						DxSAT_DiseqcInput_e				diseqcInput,
						DxSAT_ToneBurst_e					toneBurst,
						HUINT8							ucMotorPosition,
						HINT16							sSatLonggitude
						)
{
	CH_SAT_TuneParam_t		stDiParam;
	HERROR					err;

	HxLOG_Info("PAL_CH_DISEQC_StartTune()\n");

	HxSTD_memset(&stDiParam, 0, sizeof(CH_SAT_TuneParam_t)); /* 이거 안하면 garbage 때문에 다른 튜닝 파라미터로 인식함. */

#ifdef CONFIG_DEBUG
	{
		HxLOG_Print("xxxxxxxxxxxxxxxxxxxxx Tuning Parameters for DISEQC Antenna Type xxxxxxxxxxxxxxxxxxx\n");
		HxLOG_Print("\tulFrequency        [%d]MHz\n", ulFrequency);
		HxLOG_Print("\tulSymbolRate       [%d]\n", ulSymbolRate);
		HxLOG_Print("\tfecCodeRate        [%s]\n", pal_ch_sat_GetCodeRateStr(fecCodeRate));
		HxLOG_Print("\tpolarization       [%s]\n", pal_ch_sat_GetPolarizationStr(polarization));
		HxLOG_Print("\ttransSpec          [%s]\n", pal_ch_sat_GetTransSpecStr(transSpec));
		HxLOG_Print("\tpskMod             [%s]\n", pal_ch_sat_GetPskModStr(pskMod));
		HxLOG_Print("\tpilot              [%d]\n", pilot);
		HxLOG_Print("\tulLnbFreq          [%d]MHz\n", ulLnbFreq);
		HxLOG_Print("\tlnbVoltage         [%s]\n", pal_ch_sat_GetLnbVoltageStr(lnbVoltage));
		HxLOG_Print("\tb22kTone           [%d]\n", b22kTone);
		HxLOG_Print("\tdiseqcVersion      [%s]\n", pal_ch_sat_GetDiseqcVerStr(diseqcVersion));
		HxLOG_Print("\tdiseqcInput        [%s]\n", pal_ch_sat_GetDiseqcInputStr(diseqcInput));
		HxLOG_Print("\ttoneBurst          [%s]\n", pal_ch_sat_GetToneBurstStr(toneBurst));
		HxLOG_Print("\tucMotorPosition    [%d]\n", ucMotorPosition);
		HxLOG_Print("\tsSatLonggitude     [%d]\n", sSatLonggitude);
		HxLOG_Print("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
	}
#endif

	/* 주의 : DB, LNB info, MW 의 주파수는 MHz 단위임. DI는 kHz 단위임 */
	if (ulLnbFreq == CH_UNIVERSAL_LNB_FREQ)
	{
		ulLnbFreq = PAL_CH_LNB_GetActualLnbFreq(ulFrequency);
		stDiParam.b22kTone = PAL_CH_LNB_GetActual22KTone(ulFrequency);
	}
	else if(ulLnbFreq >= (HUINT32)CH_MDU4_LNB_FREQ && ulLnbFreq <= (HUINT32)CH_MDU1_LNB_FREQ)
	{
		ulLnbFreq = PAL_CH_LNB_GetActualMduLnbFreq(ulLnbFreq, polarization, ulFrequency);
		stDiParam.b22kTone = FALSE;
	}
	else
	{
		stDiParam.b22kTone = b22kTone;
	}

	stDiParam.etAntennaType = DI_CH_ANT_TYPE_DISEQC;
	stDiParam.ulFrequency = (HUINT32)abs((HINT32)ulFrequency - (HINT32)ulLnbFreq);
	stDiParam.etLnbVoltage = pal_ch_sat_ConvPAL2DI_LnbVoltage(lnbVoltage);
	stDiParam.etDiseqcInput = pal_ch_sat_ConvPAL2DI_DiseqcInput(diseqcInput);
	stDiParam.etToneBurst = pal_ch_sat_ConvPAL2DI_ToneBurst(toneBurst);
	stDiParam.etDiseqcVersion = pal_ch_sat_ConvPAL2DI_DiseqcVersion(diseqcVersion);
	stDiParam.ulSymbolRate = ulSymbolRate; /* SDB, MW, DI : kSymbols/s */
	stDiParam.etPolarization = pal_ch_sat_ConvPAL2DI_Polarization(polarization);
	stDiParam.etFecCodeRate = pal_ch_sat_ConvPAL2DI_CodeRate(fecCodeRate);
	stDiParam.etTransSpec = pal_ch_sat_ConvPAL2DI_TransSpec(transSpec);
	stDiParam.etModulation = pal_ch_sat_ConvPAL2DI_Modulation(pskMod);
	stDiParam.etPilot = pal_ch_sat_ConvPAL2DI_Pilot(pilot);
	stDiParam.etRollOff = DI_CH_ROLL_035;

	/* DI를 부른다... */
	err = DI_CH_S_RequestLock(ulTunerGroupId,ulRequestId, &stDiParam);
	if(err != DI_CH_OK)
	{
		HxLOG_Error("Error from DI_CH_S_RequestLock():%d\n",err);
		return ERR_FAIL;
	}

	pal_ch_sat_PrintDiParams(&stDiParam);

	return ERR_OK;
}



/*	- UserBandNum은 설정에 의하여 fix됨.
	- UserBandFreq는 설정에 의하여 fix됨. Tuner는 항상 이 주파수로만 튜닝되어야 함.
	- MasterUnitId는 시스템 설계시 fix됨.
	- Diseqc command로 [UserBandNum|BankNum|nFreq]를 보낸다.
	- BankNum은 Diseqc input, Polarization, 22K(H/L) 조합을 이용하여 DDI가 생성한다.
*/
HERROR PAL_CH_SCD_StartTune(
						HUINT32 						ulTunerGroupId,
						HUINT32 						ulRequestId,
						HUINT32 						ulFrequency,
						HUINT32 						ulSymbolRate,
						DxSat_CodeRate_e					fecCodeRate,
						DxSat_Polarization_e				polarization,
						DxSat_TransportSpec_e 			transSpec,
						DxSat_PskModulation_e 			pskMod,
						DxSat_Pilot_e 					pilot,
						HUINT32 						ulLnbFreq,
						DxSAT_LnbVoltage_e				lnbVoltage,
						HBOOL							b22kTone,
						DxSAT_ScdInput_e					scdInput,
						HUINT32 						ulScdUserBandNum,
						HUINT32 						ulScdUserBandFreq
						)
{
	CH_SAT_TuneParam_t		stDiParam;
	DI_CHANNEL_ERR			err;

	HxLOG_Info("PAL_CH_SCD_StartTune()\n");

	HxSTD_memset(&stDiParam, 0, sizeof(CH_SAT_TuneParam_t)); /* 이거 안하면 garbage 때문에 다른 튜닝 파라미터로 인식함. */

#ifdef CONFIG_DEBUG
	{
		HxLOG_Print("xxxxxxxxxxxxxxxxxxxxx Tuning Parameters for SCD Antenna Type xxxxxxxxxxxxxxxxxxx\n");
		HxLOG_Print("\tulFrequency        [%d]MHz\n", ulFrequency);
		HxLOG_Print("\tulSymbolRate       [%d]\n", ulSymbolRate);
		HxLOG_Print("\tfecCodeRate        [%s]\n", pal_ch_sat_GetCodeRateStr(fecCodeRate));
		HxLOG_Print("\tpolarization       [%s]\n", pal_ch_sat_GetPolarizationStr(polarization));
		HxLOG_Print("\ttransSpec          [%s]\n", pal_ch_sat_GetTransSpecStr(transSpec));
		HxLOG_Print("\tpskMod             [%s]\n", pal_ch_sat_GetPskModStr(pskMod));
		HxLOG_Print("\tpilot              [%d]\n", pilot);
		HxLOG_Print("\tulLnbFreq          [%d]MHz\n", ulLnbFreq);
		HxLOG_Print("\tlnbVoltage         [%s]\n", pal_ch_sat_GetLnbVoltageStr(lnbVoltage));
		HxLOG_Print("\tb22kTone           [%d]\n", b22kTone);
		HxLOG_Print("\tscdInput           [%s]\n", pal_ch_sat_GetScdInputStr(scdInput));
		HxLOG_Print("\tulScdUserBandNum   [%d]\n", ulScdUserBandNum);
		HxLOG_Print("\tulScdUserBandFreq  [%d]MHz\n", ulScdUserBandFreq);
		HxLOG_Print("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
	}
#endif

	/* 주의 : DB, LNB info, MW 의 주파수는 MHz 단위임. DI는 kHz 단위임 */
	if (ulLnbFreq == CH_UNIVERSAL_LNB_FREQ)
	{
		ulLnbFreq = PAL_CH_LNB_GetActualLnbFreq(ulFrequency);
		stDiParam.b22kTone = PAL_CH_LNB_GetActual22KTone(ulFrequency);
	}
	else if(ulLnbFreq >= (HUINT32)CH_MDU4_LNB_FREQ && ulLnbFreq <= (HUINT32)CH_MDU1_LNB_FREQ)
	{
		ulLnbFreq = PAL_CH_LNB_GetActualMduLnbFreq(ulLnbFreq, polarization, ulFrequency);
		stDiParam.b22kTone = b22kTone;
	}
	else
	{
		stDiParam.b22kTone = b22kTone;
	}

	stDiParam.etAntennaType = DI_CH_ANT_TYPE_SCD;
	stDiParam.ulFrequency = (HUINT32)abs((HINT32)ulFrequency - (HINT32)ulLnbFreq);
	stDiParam.etLnbVoltage = pal_ch_sat_ConvPAL2DI_LnbVoltage(lnbVoltage);

	if(scdInput == eDxSCD_INPUT_SCD_A)
	{
		stDiParam.etDiseqcInput = DI_CH_DISEQC_INPUT_SCD_A;
	}
	else if(scdInput == eDxSCD_INPUT_SCD_B)
	{
		stDiParam.etDiseqcInput = DI_CH_DISEQC_INPUT_SCD_B;
	}
	else
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	stDiParam.ulSymbolRate = ulSymbolRate; /* SDB, MW, DI : kSymbols/s */
	stDiParam.etPolarization = pal_ch_sat_ConvPAL2DI_Polarization(polarization);
	stDiParam.etFecCodeRate = pal_ch_sat_ConvPAL2DI_CodeRate(fecCodeRate);
	stDiParam.etTransSpec = pal_ch_sat_ConvPAL2DI_TransSpec(transSpec);
	stDiParam.etModulation = pal_ch_sat_ConvPAL2DI_Modulation(pskMod);
	stDiParam.etPilot = pal_ch_sat_ConvPAL2DI_Pilot(pilot);
	stDiParam.etRollOff = DI_CH_ROLL_035;

	// SCD Master Unit ID는 '실제 SCD 명령을 보내는 주체는 누구인가?' 를 가리키는 곳이다.
	// SCD 시에 Tuner 0에 연결하고 Loop through로 Tuner 1에 연결하는 것을 기본으로 잡기 때문에
	// Master는 Tuner 0이 된다.
	stDiParam.ulScdMasterUnitId = 0;

	stDiParam.ulScdUserBandNum = (HUINT8)ulScdUserBandNum;
	stDiParam.ulScdUserBandFreq = (HUINT16)ulScdUserBandFreq;

	/* DI를 부른다... */
	err = DI_CH_S_RequestLock(ulTunerGroupId, ulRequestId, &stDiParam);
	if(err != DI_CH_OK)
	{
		HxLOG_Error("Error from DI_CH_S_RequestLock():%d\n",err);
		return ERR_FAIL;
	}

	pal_ch_sat_PrintDiParams(&stDiParam);

	return ERR_OK;
}


HERROR PAL_CH_SMATV_StartTune(
						HUINT32							ulTunerGroupId,
						HUINT32 						ulRequestId,
						HUINT32							ulFrequency,
						HUINT32							ulSymbolRate,
						DxSat_CodeRate_e					fecCodeRate,
						DxSat_Polarization_e				polarization,
						DxSat_TransportSpec_e				transSpec,
						DxSat_PskModulation_e 			pskMod,
						DxSat_Pilot_e						pilot
						)
{
	CH_SAT_TuneParam_t		stDiParam;
	DI_CHANNEL_ERR			err;

	HxLOG_Info("PAL_CH_SMATV_StartTune()\n");

	HxSTD_memset(&stDiParam, 0, sizeof(CH_SAT_TuneParam_t)); /* 이거 안하면 garbage 때문에 다른 튜닝 파라미터로 인식함. */

#ifdef CONFIG_DEBUG
	{
		HxLOG_Print("xxxxxxxxxxxxxxxxxxxxx Tuning Parameters for SMATV Antenna Type xxxxxxxxxxxxxxxxxxx\n");
		HxLOG_Print("\tulFrequency        [%d]MHz\n", ulFrequency);
		HxLOG_Print("\tulSymbolRate       [%d]\n", ulSymbolRate);
		HxLOG_Print("\tfecCodeRate        [%s]\n", pal_ch_sat_GetCodeRateStr(fecCodeRate));
		HxLOG_Print("\tpolarization       [%s]\n", pal_ch_sat_GetPolarizationStr(polarization));
		HxLOG_Print("\ttransSpec          [%s]\n", pal_ch_sat_GetTransSpecStr(transSpec));
		HxLOG_Print("\tpskMod             [%s]\n", pal_ch_sat_GetPskModStr(pskMod));
		HxLOG_Print("\tpilot              [%d]\n", pilot);
		HxLOG_Print("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
	}
#endif

	stDiParam.etAntennaType = DI_CH_ANT_TYPE_SMATV;

	stDiParam.ulFrequency = ulFrequency;
	stDiParam.ulSymbolRate = ulSymbolRate; /* SDB, MW, DI : kSymbols/s */
	stDiParam.etFecCodeRate = pal_ch_sat_ConvPAL2DI_CodeRate(fecCodeRate);
	stDiParam.etPolarization = pal_ch_sat_ConvPAL2DI_Polarization(polarization);
	stDiParam.etTransSpec = pal_ch_sat_ConvPAL2DI_TransSpec(transSpec);
	stDiParam.etModulation = pal_ch_sat_ConvPAL2DI_Modulation(pskMod);
	stDiParam.etPilot = pal_ch_sat_ConvPAL2DI_Pilot(pilot);
	stDiParam.etRollOff = DI_CH_ROLL_035;
	stDiParam.b22kTone = FALSE;
	stDiParam.etLnbVoltage = DI_CH_LNB_VOLT_STD;
	stDiParam.etDiseqcInput = DI_CH_DISEQC_INPUT_NONE;
	stDiParam.etToneBurst = DI_CH_TONE_BURST_NONE;
	stDiParam.etDiseqcVersion = DI_CH_DISEQC_VER_1_0;

	/* DI를 부른다... */
	err = DI_CH_S_RequestLock(ulTunerGroupId, ulRequestId, &stDiParam);
	if(err != DI_CH_OK)
	{
		HxLOG_Error("Error from DI_CH_S_RequestLock():%d\n",err);
		return ERR_FAIL;
	}

	pal_ch_sat_PrintDiParams(&stDiParam);

	return ERR_OK;
}

HERROR PAL_CH_SAT_StopTune(HUINT32 ulTunerGroupId)
{
	DI_CHANNEL_ERR	err;

	HxLOG_Info("PAL_CH_SAT_StopTune()\n");

	err = DI_CH_S_CancelLock(ulTunerGroupId);

	if(err != DI_CH_OK)
	{
		HxLOG_Error("Error from DI_CH_S_CancelLock():%d\n",err);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HUINT32 PAL_CH_LNB_GetActualLnbFreq(HUINT32 ulTpFreq)
{
	if (abs((HINT32)(ulTpFreq - 9750)) < 1950) /* MHz */
	{
		return 9750;
	}
	else
	{
		return 10600;
	}
}


HUINT32 PAL_CH_LNB_GetActualMduLnbFreq(HUINT32 ulLnbFreq, DxSat_Polarization_e ePol, HUINT32 ulFreq)
{
HUINT32		ulActLnbFreq = 0;

	switch(ulLnbFreq)
	{
		case CH_MDU1_LNB_FREQ:
			if(ePol == eDxSAT_POLAR_HOR)
				ulActLnbFreq = 10450;
			else
				ulActLnbFreq = 10000;
			break;

		case CH_MDU2_LNB_FREQ:
			if (ulFreq >= 10780 && ulFreq <= 10950)
			{
				if(ePol == eDxSAT_POLAR_HOR)
					ulActLnbFreq = 9830;
				else
					ulActLnbFreq = 9550;
			}
			else if (ulFreq >= 11450 && ulFreq <= 11700)
			{
				if(ePol == eDxSAT_POLAR_HOR)
					ulActLnbFreq = 9925;
				else
					ulActLnbFreq = 9550;
			}
			else
				ulActLnbFreq = 0;
			break;

		case CH_MDU3_LNB_FREQ:
			if(ulFreq >= 10780 && ulFreq <= 10950)
			{
				if(ePol == eDxSAT_POLAR_HOR)
					ulActLnbFreq = 12930;
				else
					ulActLnbFreq = 9830;
			}
			else if(ulFreq >= 11450 && ulFreq <= 11700)
			{
				if(ePol == eDxSAT_POLAR_HOR)
					ulActLnbFreq = 9830;
				else
					ulActLnbFreq = 12930;
			}
			else
				ulActLnbFreq = 0;

			break;

		case CH_MDU4_LNB_FREQ:
			if(ulFreq >= 10780 && ulFreq <= 10950)
			{
				if(ePol == eDxSAT_POLAR_HOR)
					ulActLnbFreq = 12930;
				else
					ulActLnbFreq = 12650;
			}
			else if(ulFreq >= 11450 && ulFreq <= 11700 && ePol == eDxSAT_POLAR_HOR)
				ulActLnbFreq = 13035;
			else if(ulFreq >= 11429 && ulFreq <= 11700 && ePol == eDxSAT_POLAR_VER)
				ulActLnbFreq = 12650;
			else
				ulActLnbFreq = 0;

			break;

		default:
			HxLOG_Error("[PAL_CH_LNB_GetActualMduLnbFreq] unsupported MDU Freq (%d) \n", ulLnbFreq);
			ulActLnbFreq = 0;
			break;

	}

#if defined(CONFIG_DEBUG)
	/* for debugging */
	if(ulActLnbFreq == 0)
		HxLOG_Error("[PAL_CH_LNB_GetActualMduLnbFreq] ERROR : out of range freq : %d \n", ulFreq);
	else
		HxLOG_Info("[PAL_CH_LNB_GetActualMduLnbFreq] LnbFreq for cur MDU -> %d \n", ulActLnbFreq);
#endif

	return ulActLnbFreq;

}


HBOOL PAL_CH_LNB_GetActual22KTone(HUINT32 ulTpFreq)
{
	if (abs((HINT32)(ulTpFreq - 9750)) < 1950) /* MHz */
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

HERROR PAL_CH_LNB_GetLockedInfo(
								HUINT32				ulTunerGroupId,
								HUINT32				ulTpFreq,
								HUINT32				ulLnbFreq,
								HUINT32				*pulActualTpFreq, // MHz
								DxSat_Polarization_e	*pActualPolar,
								DxSat_CodeRate_e		*pActualCodeRate,
								DxSat_PskModulation_e	*pActualPskMod,
								DxSat_Pilot_e			*pActualPilot
								)
{
	HUINT32 			ulRealLnbFreq;
	HINT32				nFreqDiff1, nFreqDiff2;
	HUINT32 			ulPalFreq1, ulPalFreq2;

	CH_SAT_TuneParam_t	stDiParam;
	DI_CHANNEL_ERR		err;

	err = DI_CH_S_GetLockedInfo(ulTunerGroupId, &stDiParam);
	if(err != DI_CH_OK)
	{
		HxLOG_Error("Error from DI_CH_S_GetLockedInfo():%d\n",err);
		return ERR_FAIL;
	}

	/* 주의 : DB, LNB info, MW 의 주파수는 MHz 단위임. DDI는 kHz 단위임 */
	if (ulLnbFreq == CH_UNIVERSAL_LNB_FREQ)
	{
		ulRealLnbFreq = PAL_CH_LNB_GetActualLnbFreq(ulTpFreq);
	}
	else if(ulLnbFreq >= (HUINT32)CH_MDU4_LNB_FREQ && ulLnbFreq <= (HUINT32)CH_MDU1_LNB_FREQ)
	{
		ulRealLnbFreq = PAL_CH_LNB_GetActualMduLnbFreq(ulLnbFreq, stDiParam.etPolarization, ulTpFreq);
	}
	else
	{
		ulRealLnbFreq = ulLnbFreq;
	}

	ulPalFreq1 = ulRealLnbFreq + stDiParam.ulFrequency;
	ulPalFreq2 = ulRealLnbFreq - stDiParam.ulFrequency;
	nFreqDiff1 = (HUINT32)abs ((HINT32)ulTpFreq - (HINT32)ulPalFreq1);
	nFreqDiff2 = (HUINT32)abs ((HINT32)ulTpFreq - (HINT32)ulPalFreq2);

	*pulActualTpFreq = (nFreqDiff2 > nFreqDiff1) ? ulPalFreq1 : ulPalFreq2;

	*pActualPolar = pal_ch_sat_ConvDI2PAL_Polarization(stDiParam.etPolarization);
	*pActualCodeRate = pal_ch_sat_ConvDI2PAL_CodeRate(stDiParam.etFecCodeRate);
	*pActualPskMod = pal_ch_sat_ConvDI2PAL_Modulation(stDiParam.etModulation);
	*pActualPilot = pal_ch_sat_ConvDI2PAL_Pilot(stDiParam.etPilot);

	return ERR_OK;
}

HERROR PAL_CH_SMATV_GetLockedInfo(
								HUINT32				ulTunerGroupId,
								HUINT32				*pulActualTpFreq, // MHz
								DxSat_Polarization_e	*pActualPolar,
								DxSat_CodeRate_e		*pActualCodeRate,
								DxSat_PskModulation_e	*pActualPskMod,
								DxSat_Pilot_e			*pActualPilot
								)
{
	CH_SAT_TuneParam_t	stDiParam;
	DI_CHANNEL_ERR		err;

	err = DI_CH_S_GetLockedInfo(ulTunerGroupId, &stDiParam);
	if(err != DI_CH_OK)
	{
		HxLOG_Error("Error from DI_CH_S_GetLockedInfo():%d\n",err);
		return ERR_FAIL;
	}

	*pulActualTpFreq = stDiParam.ulFrequency;
	*pActualPolar = pal_ch_sat_ConvDI2PAL_Polarization(stDiParam.etPolarization);
	*pActualCodeRate = pal_ch_sat_ConvDI2PAL_CodeRate(stDiParam.etFecCodeRate);
	*pActualPskMod = pal_ch_sat_ConvDI2PAL_Modulation(stDiParam.etModulation);
	*pActualPilot = pal_ch_sat_ConvDI2PAL_Pilot(stDiParam.etPilot);

	return ERR_OK;
}

HERROR PAL_CH_SAT_GetSignalInfo(
								HUINT32			ulTunerGroupId,
								HUINT32			*pulSignalLevel,
								HUINT32			*pulSignalQuality,
								HUINT32			*pulAgc,
								float			*pfBer,
								float			*pfSnr,
								float			*pfSignalInputPower
								)
{
	DI_CHANNEL_ERR			err_DI;
	DI_CH_SignalQuality_t 	stSignalInfo;

	err_DI = DI_CH_S_GetSignalInfo(ulTunerGroupId, &stSignalInfo);
	if(err_DI == DI_CH_OK)
	{
		*pulSignalLevel		= stSignalInfo.ulStrength;
		*pulSignalQuality	= stSignalInfo.ulQuality;
		*pulAgc				= stSignalInfo.ulAgc;
		*pfBer				= stSignalInfo.fBer;
		*pfSnr				= stSignalInfo.fSnr;
		*pfSignalInputPower	= stSignalInfo.fSignalInputPower;

		return ERR_OK;
	}
	else
	{
		*pulSignalLevel		= 0;
		*pulSignalQuality	= 0;
		*pulAgc				= 0;
		*pfBer				= 0;
		*pfSnr				= 0;
		*pfSignalInputPower	= 0;

		return ERR_FAIL;
	}
}

HERROR PAL_CH_SAT_SendDiseqcCommand(HUINT8 ulTunerGroupId, HUINT8 *aucCommand, HUINT32 ulLength)
{
	DI_CHANNEL_ERR		err_DI;
	HUINT32				i;

	HxLOG_Info("PAL_CH_SAT_SendDiseqcCommand()");

	HxLOG_Print("\tDisecq command : ");
	for (i = 0; i < ulLength; i++)
	{
		HxLOG_Print("[0x%02x] ", aucCommand[i]);
	}
	HxLOG_Print("\n");

	err_DI = DI_CH_S_SendDiseqcMsg(ulTunerGroupId, aucCommand, ulLength);
	if(err_DI != DI_CH_OK)			HxLOG_Critical("Critical Eror!!!\n");

	return ERR_OK;
}

HERROR PAL_CH_SAT_SetAntennaType(HUINT32 ulTunerGroupId, DxAntennaConnection_e eAntType)
{
	HxLOG_Info("PAL_CH_SAT_SetAntennaType() : eAntType(%d)\n", eAntType);

	switch(eAntType)
	{
	case eDxANTCONN_ONECABLE:
		PAL_CH_SAT_SetRfInputConnection (0, 0);
		PAL_CH_SAT_SetRfInputConnection (1, 0);
		break;

	case eDxANTCONN_TWOSAME:
	case eDxANTCONN_TWODIFF:
		PAL_CH_SAT_SetRfInputConnection (0, 0);
		PAL_CH_SAT_SetRfInputConnection (1, 1);
		break;

	default:
		HxLOG_Error("Invalid value for Antenna Connection Type : (%d)\n", eAntType);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR PAL_CH_MOTOR_Stop(HUINT32 ulTunerGroupId)
{
	HUINT8 		aucCmd[3];
	HUINT32 	ulRetry;
	HERROR 		err;

	HxLOG_Info("PAL_CH_MOTOR_Stop()\n");

	/* ignore STOP command if already stopped */
	if (s_MotorContext[ulTunerGroupId].bMotorMoving == FALSE)
	{
		HxLOG_Info("\tMotor already stopped. No action.\n");
		return ERR_OK;
	}
	else
	{
		s_MotorContext[ulTunerGroupId].bMotorMoving = FALSE;
	}

	aucCmd[1] = 0x31;	/* Polar / Azimuth Positioner */
	aucCmd[2] = 0x60;	/* HALT: Stop positioner movement */

	for (ulRetry = 0; ulRetry < MOTOR_NUM_RETRY; ulRetry++) {
		/* Framing Byte: command from master, No reply required */
		/* 0xE0 for first transmission, 0xE1 for repeated transmission */
		aucCmd[0] = (ulRetry == 0)? 0xE0: 0xE1;

		err = PAL_CH_SAT_SendDiseqcCommand(ulTunerGroupId, aucCmd, 3);
		if(err != ERR_OK) HxLOG_Critical("Critical Eror!!!\n");

		VK_TASK_Sleep(MOTOR_DELAY_RETRY);
	}

	/* reset current motor position/degree to UNCERTAIN */
	s_MotorContext[ulTunerGroupId].ucMotorPosition = MOTOR_POSITION_INVALID;
	s_MotorContext[ulTunerGroupId].sMotorDegree = MOTOR_DEGREE_INVALID;

	return ERR_OK;
}


HERROR PAL_CH_MOTOR_GotoPosition(HUINT32 ulTunerGroupId, HUINT8 ucPosition)
{
	HUINT8 		aucCmd[4];
	HERROR		err;
	HUINT32 	i;
	HUINT32		ulNumTunerGroup;

	HxLOG_Info("PAL_CH_MOTOR_GotoPosition(position:%d)\n", ucPosition);
	if(ucPosition > MOTOR_POSITION_MAX) HxLOG_Critical("Critical Eror!!!\n");

	if (ucPosition == s_MotorContext[ulTunerGroupId].ucMotorPosition)
	{
		HxLOG_Print("Same motor position. No action.\n");
		return ERR_OK;
	}

	aucCmd[0] = 0xE0;	/* command from master, No reply required, first transmission */
	aucCmd[1] = 0x31;	/* Polar / Azimuth Positioner */
	aucCmd[2] = 0x6B;	/* Goto nn: Drive motor to Satellite position nn */
	aucCmd[3] = ucPosition;

	err = PAL_CH_SAT_SendDiseqcCommand(ulTunerGroupId, aucCmd, 4);
	if(err != ERR_OK) HxLOG_Critical("Critical Eror!!!\n");

	/* set motor state to "MOVING" */
	s_MotorContext[ulTunerGroupId].bMotorMoving = TRUE;

	/* set current motor position */
	s_MotorContext[ulTunerGroupId].ucMotorPosition = ucPosition;

	ulNumTunerGroup = pal_ch_GetNumOfTunerGroup();
	// 이전 degree값 초기화. DiSEq1.2 <-> 1.3간 전환시 문제
	for(i = 0; i < ulNumTunerGroup; i++)
	{
		s_MotorContext[i].sMotorDegree = MOTOR_DEGREE_INVALID;
	}

	VK_TASK_Sleep(MOTOR_DELAY_NORMAL);

	return ERR_OK;
}


HERROR PAL_CH_MOTOR_StorePosition(HUINT32 ulTunerGroupId, HUINT8 ucPosition)
{
	HUINT8 		aucCmd[4];
	HUINT32 	ulRetry;
	HERROR		err;

	HxLOG_Info("PAL_CH_MOTOR_StorePosition(position:%d)\n", ucPosition);
	if(ucPosition > MOTOR_POSITION_MAX) HxLOG_Critical("Critical Eror!!!\n");

	aucCmd[1] = 0x31;	/* Polar / Azimuth Positioner */
	aucCmd[2] = 0x6A;	/* Store nn: Store Satellite position & Enable limits */
	aucCmd[3] = ucPosition;	/* if ucPosition == 0, enable (Soft) limits */

	for (ulRetry = 0; ulRetry < MOTOR_NUM_RETRY; ulRetry++) {
		/* Framing Byte: command from master, No reply required */
		/* 0xE0 for first transmission, 0xE1 for repeated transmission */
		aucCmd[0] = (ulRetry == 0)? 0xE0: 0xE1;

		err = PAL_CH_SAT_SendDiseqcCommand(ulTunerGroupId, aucCmd, 4);
		if(err != ERR_OK) HxLOG_Critical("Critical Eror!!!\n");

		VK_TASK_Sleep(MOTOR_DELAY_RETRY);
	}

	return ERR_OK;
}


HERROR PAL_CH_MOTOR_Drive(HUINT32 ulTunerGroupId, CH_MotorDirection_t direction, CH_MotorDriveMode_t driveMode, HUINT8 ucValue)
{
	HUINT8 	aucCmd[4];
	HERROR	err;

	HxLOG_Info("PAL_CH_MOTOR_Drive()\n");

	HxLOG_Print("Motor drive : %s %s : %d\n",
		(direction == eMOTOR_DIR_EAST)? "EAST": "WEST",
		(driveMode == eMOTOR_DRIVE_MODE_CONTINUOUS)? "CONT":
			((driveMode == eMOTOR_DRIVE_MODE_TIMEOUT)? "TIME": "STEP"),
		ucValue);

	aucCmd[0] = 0xE0;	/* command from master, No reply required, first transmission */
	aucCmd[1] = 0x31;	/* Polar / Azimuth Positioner */

	switch (direction)
	{
		case eMOTOR_DIR_EAST:
			aucCmd[2] = 0x68;	/* Drive East: with optional timeout / steps */
			break;

		case eMOTOR_DIR_WEST:
			aucCmd[2] = 0x69;	/* Drive West: with optional timeout / steps */
			break;

		default:
			HxLOG_Error("Invalid value for DiSEqC motor direction(%d)\n", direction);
			return ERR_FAIL;
	}

	switch (driveMode)
	{
		case eMOTOR_DRIVE_MODE_CONTINUOUS:
			aucCmd[3] = 0x00;
			break;

		case eMOTOR_DRIVE_MODE_TIMEOUT:
			if ( (ucValue == 0) || (ucValue > 0x7F) )
			{
				HxLOG_Error("Invalid value for Timeout of DiSEqC motor driving(%d)\n", ucValue);
				return ERR_FAIL;
			}

			aucCmd[3] = ucValue;
			break;

		case eMOTOR_DRIVE_MODE_STEP:
			if ( (ucValue == 0) || (ucValue > 0x7F) )
			{
				HxLOG_Error("Invalid value for Steps of DiSEqC motor driving(%d)\n", ucValue);
				return ERR_FAIL;
			}

			aucCmd[3] = 0x100 - ucValue;
			break;
	}

	err = PAL_CH_SAT_SendDiseqcCommand(ulTunerGroupId, aucCmd, 4);
	if(err != ERR_OK) HxLOG_Critical("Critical Eror!!!\n");

	/* set motor state to "MOVING" */
	s_MotorContext[ulTunerGroupId].bMotorMoving = TRUE;

	/* reset current motor position/degree to UNCERTAIN */
	s_MotorContext[ulTunerGroupId].ucMotorPosition = MOTOR_POSITION_INVALID;
	s_MotorContext[ulTunerGroupId].sMotorDegree = MOTOR_DEGREE_INVALID;

	VK_TASK_Sleep(MOTOR_DELAY_NORMAL);

	return ERR_OK;
}

HERROR PAL_CH_MOTOR_SetLimit(HUINT32 ulTunerGroupId, CH_MotorDirection_t direction)
{
	HUINT8 		aucCmd[3];
	HUINT32 	ulRetry;
	HERROR 		err;

	HxLOG_Info("PAL_CH_MOTOR_SetLimit(%s)\n", (direction == eMOTOR_DIR_EAST)? "EAST": "WEST");

	aucCmd[1] = 0x31;	/* Polar / Azimuth Positioner */

	switch (direction)
	{
		case eMOTOR_DIR_EAST:
			aucCmd[2] = 0x66;	/* Limit E: Set East limit (& Enable recommended) */
			break;

		case eMOTOR_DIR_WEST:
			aucCmd[2] = 0x67;	/* Limit W: Set West limit (& Enable recommended) */
			break;

		default:
			HxLOG_Error("Invalid value for DiSEqC motor direction(%d)\n", direction);
			return ERR_FAIL;
	}

	for (ulRetry = 0; ulRetry < MOTOR_NUM_RETRY; ulRetry++)
	{
		/* Framing Byte: command from master, No reply required */
		/* 0xE0 for first transmission, 0xE1 for repeated transmission */
		aucCmd[0] = (ulRetry == 0)? 0xE0: 0xE1;

		err = PAL_CH_SAT_SendDiseqcCommand(ulTunerGroupId, aucCmd, 3);
		if(err != ERR_OK) HxLOG_Critical("Critical Eror!!!\n");

		VK_TASK_Sleep(MOTOR_DELAY_RETRY);
	}

	return ERR_OK;
}


HERROR PAL_CH_MOTOR_DisableLimit(HUINT32 ulTunerGroupId)
{
	HUINT8 		aucCmd[3];
	HUINT32 	ulRetry;
	HERROR		err;

	HxLOG_Info("PAL_CH_MOTOR_DisableLimit()\n");

	aucCmd[1] = 0x31;	/* Polar / Azimuth Positioner */
	aucCmd[2] = 0x63;	/* Limits Off: Disable limits */

	for (ulRetry = 0; ulRetry < MOTOR_NUM_RETRY; ulRetry++)
	{
		/* Framing Byte: command from master, No reply required */
		/* 0xE0 for first transmission, 0xE1 for repeated transmission */
		aucCmd[0] = (ulRetry == 0)? 0xE0: 0xE1;

		err = PAL_CH_SAT_SendDiseqcCommand(ulTunerGroupId, aucCmd, 3);
		if(err != ERR_OK) HxLOG_Critical("Critical Eror!!!\n");

		VK_TASK_Sleep(MOTOR_DELAY_RETRY);
	}

	return ERR_OK;
}

HERROR PAL_CH_MOTOR_EnableLimit(HUINT32 ulTunerGroupId)
{
	HUINT8 		aucCmd[4];
	HUINT32 	ulRetry;
	HERROR		err;

	HxLOG_Info("PAL_CH_MOTOR_EnableLimit()\n");

	aucCmd[1] = 0x31;	/* Polar / Azimuth Positioner */
	aucCmd[2] = 0x6A;	/* Store nn: Store Satellite position & Enable limits */
	aucCmd[3] = 0x00;

	for (ulRetry = 0; ulRetry < MOTOR_NUM_RETRY; ulRetry++)
	{
		/* Framing Byte: command from master, No reply required */
		/* 0xE0 for first transmission, 0xE1 for repeated transmission */
		aucCmd[0] = (ulRetry == 0)? 0xE0: 0xE1;

		err = PAL_CH_SAT_SendDiseqcCommand(ulTunerGroupId, aucCmd, 4);
		if(err != ERR_OK) HxLOG_Critical("Critical Eror!!!\n");

		VK_TASK_Sleep(MOTOR_DELAY_RETRY);
	}

	return ERR_OK;
}

HERROR PAL_CH_MOTOR_Recalculate(HUINT32 ulTunerGroupId, HUINT8 ucPosition)
{
	HUINT8 		aucCmd[4];
	HUINT32 	ulRetry;
	HERROR		err;

	HxLOG_Info("PAL_CH_MOTOR_Recalculate(ucPosition:%d)\n", ucPosition);

	aucCmd[1] = 0x31;	/* Polar / Azimuth Positioner */
	aucCmd[2] = 0x6F;	/* Set Posns.: (Re-)Calculate Satellite positions */
	aucCmd[3] = ucPosition;

	for (ulRetry = 0; ulRetry < MOTOR_NUM_RETRY; ulRetry++)
	{
		/* Framing Byte: command from master, No reply required */
		/* 0xE0 for first transmission, 0xE1 for repeated transmission */
		aucCmd[0] = (ulRetry == 0)? 0xE0: 0xE1;

		err = PAL_CH_SAT_SendDiseqcCommand(ulTunerGroupId, aucCmd, 4);
		if(err != ERR_OK) HxLOG_Critical("Critical Eror!!!\n");

		VK_TASK_Sleep(MOTOR_DELAY_RETRY);
	}

	/* reset current motor position/degree to UNCERTAIN */
	s_MotorContext[ulTunerGroupId].ucMotorPosition = MOTOR_POSITION_INVALID;
	s_MotorContext[ulTunerGroupId].sMotorDegree = MOTOR_DEGREE_INVALID;

	return ERR_OK;
}

/* converted true binary fraction value for 0.0, 0.1, ..., 0.9 */
/* 	0x0 0000 = 0.0000
	0x2 0010 = 0.1250
	0x3 0011 = 0.1875
	0x5 0101 = 0.3125
	0x6 0110 = 0.3750
	0x8 1000 = 0.5000
	0xA 1010 = 0.6250
	0xB 1011 = 0.6875
	0xD 1101 = 0.8125
	0xE 1110 = 0.8750 */
const HUINT8 c_ucFractionTable[10] = {0x0, 0x2, 0x3, 0x5, 0x6, 0x8, 0xA, 0xB, 0xD, 0xE};

HERROR PAL_CH_MOTOR_GotoDegree(HUINT32 ulTunerGroupId, HINT16 sDegree)
{
	HUINT8 		aucCmd[5];
	HERROR		err;
	HINT16		saveDegree = sDegree;
	HUINT32		i;
	HINT16		x, y;
	HUINT32		ulNumTunerGroup;

	HxLOG_Info("PAL_CH_MOTOR_GotoDegree(sDegree:%d)\n", sDegree);

	if ( (sDegree < -2560) || (sDegree > 2560 ))
	{
		HxLOG_Error("Invalid degree input (%d)\n", sDegree);
		return ERR_FAIL;
	}

	/* check current degree */
	if (sDegree == s_MotorContext[ulTunerGroupId].sMotorDegree)
	{
		HxLOG_Print("Same DiSEqC degree: Do NOT move.\n");
		return ERR_OK;
	}

	if(sDegree < 0)
	{
		aucCmd[3] = 0xE0;		/* go EAST */
		sDegree = -sDegree;		/* abs(sDegree) */
	}
	else
	{
		aucCmd[3] = 0xD0;		/* go WEST */
	}

	/* explode sDegree into "x.y" */
	x = (sDegree / 10) & 0xFF;
	y = c_ucFractionTable[sDegree % 10];

	aucCmd[0] = 0xE0;					/* command from master, No reply required, first transmission */
	aucCmd[1] = 0x31;					/* Polar / Azimuth Positioner */
	aucCmd[2] = 0x6E;					/* Goto xx: Drive motor to angular position */
	aucCmd[3] |= (x >> 4);				/* high nibble of x */
	aucCmd[4] = ((x & 0x0F) << 4) | y;	/* low nibble of x and y */

	err = PAL_CH_SAT_SendDiseqcCommand(ulTunerGroupId, aucCmd, 5);
	if(err != ERR_OK) HxLOG_Critical("Critical Eror!!!\n");

	/* set motor state to "NOT STOPPED" */
	s_MotorContext[ulTunerGroupId].bMotorMoving = TRUE;

	/* set current motor degree */
	s_MotorContext[ulTunerGroupId].sMotorDegree = saveDegree;

	ulNumTunerGroup = pal_ch_GetNumOfTunerGroup();
	// 이전 degree값 초기화. DiSEq1.2 <-> 1.3간 전환시 문제
	for(i = 0; i < ulNumTunerGroup; i++)
	{
		s_MotorContext[ulTunerGroupId].ucMotorPosition = MOTOR_POSITION_INVALID;
	}

	VK_TASK_Sleep(MOTOR_DELAY_NORMAL);

	return ERR_OK;
}

STATIC HINT8 *pal_ch_PrintFloat(HFLOAT64 dFloat, HINT32 nDigit)
{
	static char s_szFloatData[32];
	static char s_szFloatFormat[16];

	int power = 1;
	int i, x, y;

	for (i = 0; i < nDigit; i++)
		power *= 10;

	x = (int)(dFloat * power) / power;
	y = ((int)(dFloat * power) % power);
	y = (y > 0)? y: -y;

	HxSTD_PrintToStrN (s_szFloatFormat, 16, "%%d.%%0%dd", nDigit);
	HxSTD_PrintToStrN (s_szFloatData, 32, s_szFloatFormat, x, y);

	return s_szFloatData;
}

HINT16 local_motor_UsalsFormula(HINT16 sSatellite, HINT16 sLongitude, HINT16 sLatitude)
{
	/* refer to "formula.xls" from USALS */
	/* use character "R" for point P' to use in name of variable
	 * i.e. variable for length of side PP' can be namely "SidePR" */

	double dSatellite	= sSatellite / 10.0;
	double dLongitude	= sLongitude / 10.0;
	double dLatitude 	= sLatitude / 10.0;

	double dAngle_OCB, dAngle_xx, dRadian_xx;
	double dSide_PR, dSide_CR;
	double dSide_RH, dSide_PH, dSide_CH, dSide_BH;
	double dSide_PB, dSide_OP, dSide_OB;

	HINT16 sAngle_xx;

	if( (sSatellite > 1800) ||(sSatellite < -1800) )		/* step #14 */
	{
		HxLOG_Error("    USALS [14] invalid value for Satellite position.\n");
		return MOTOR_DEGREE_INVALID;
	}

	if( (sLongitude > 1800 || sLongitude < -1800) )			/* step #15 */
	{
		HxLOG_Error("    USALS [15] invalid value for Longitude.\n");
		return MOTOR_DEGREE_INVALID;
	}

	if( (sLatitude > 900 || sLatitude < -900) )				/* step #16 */
	{
		HxLOG_Error("    USALS [16] invalid value for Latitude.\n");
		return MOTOR_DEGREE_INVALID;
	}

	HxLOG_Print("    USALS [17] SAT Position : %s\n", pal_ch_PrintFloat(dSatellite, 1));
	HxLOG_Print("    USALS [18] ANT Longitude: %s\n", pal_ch_PrintFloat(dLongitude, 1));
	HxLOG_Print("    USALS [19] ANT Latitude : %s\n", pal_ch_PrintFloat(dLatitude, 1));

	dAngle_OCB = fabs(dSatellite - dLongitude);				/* step #20 */
	if(dAngle_OCB > 180.0)
	{
		dAngle_OCB -= 360.0;								/* step #21 */
	}

	if( (dAngle_OCB > 65.0) || (dAngle_OCB < (-65.0)) )
	{
		HxLOG_Error("    USALS [22] invalid value for angle OCB.\n");
		return MOTOR_DEGREE_INVALID;
	}
	else
	{
		HxLOG_Print("    USALS [22] Angle OCB: %s\n", pal_ch_PrintFloat(dAngle_OCB, 1));
	}

	dSide_PR = 6378 * sin(dLatitude * M_PI / 180.0);		/* step #23 */
	HxLOG_Print("    USALS [23]  Side PP': %s\n", pal_ch_PrintFloat(dSide_PR, 2));

	dSide_CR = sqrt(40678884 - dSide_PR*dSide_PR);			/* step #24 */
	HxLOG_Print("    USALS [24]  Side CP': %s\n", pal_ch_PrintFloat(dSide_CR, 2));

	dSide_RH = dSide_CR * sin(dAngle_OCB * M_PI / 180.0);	/* step #25 */
	HxLOG_Print("    USALS [25]  Side P'H: %s\n", pal_ch_PrintFloat(dSide_RH, 2));

	dSide_PH = sqrt(dSide_PR*dSide_PR + dSide_RH*dSide_RH);	/* step #26 */
	HxLOG_Print("    USALS [26]  Side PH : %s\n", pal_ch_PrintFloat(dSide_PH, 2));

	dSide_CH = sqrt(40678884 - dSide_PH*dSide_PH);			/* step #27 */
	HxLOG_Print("    USALS [27]  Side CH : %s\n", pal_ch_PrintFloat(dSide_CH, 2));

	dSide_BH = 42164.2 - dSide_CH;							/* step #28*/
	HxLOG_Print("    USALS [28]  Side BH : %s\n", pal_ch_PrintFloat(dSide_BH, 2));

	dSide_PB = sqrt(dSide_BH*dSide_BH + dSide_PH*dSide_PH);	/* step #29 */
	HxLOG_Print("    USALS [29]  Side PB : %s\n", pal_ch_PrintFloat(dSide_PB, 2));

	dSide_OP = sqrt((42164.2 - dSide_CR)*(42164.2 - dSide_CR) + dSide_PR*dSide_PR);		/* step #30 */
	HxLOG_Print("    USALS [30]  Side OP : %s\n", pal_ch_PrintFloat(dSide_OP, 2));

	dSide_OB = sqrt(3555639523.0 - 3555639523.0 * cos(dAngle_OCB * M_PI / 180));			/* step #31 */
	HxLOG_Print("    USALS [31]  Side OB : %s\n", pal_ch_PrintFloat(dSide_OB, 2));

	dRadian_xx = acos((dSide_PB*dSide_PB + dSide_OP*dSide_OP - dSide_OB*dSide_OB)
			/ (2 * dSide_PB * dSide_OP));					/* step #32 */
	HxLOG_Print("    USALS [32]  Rad(xx) : %s\n", pal_ch_PrintFloat(dRadian_xx, 4));

	dAngle_xx = dRadian_xx * 180.0 / M_PI;					/* step #33 */
	HxLOG_Print("    USALS [33]  Deg(xx) : %s\n", pal_ch_PrintFloat(dAngle_xx, 3));

	if( (dAngle_xx > 65.0) || (dAngle_xx < (-65.0)) )			/* step #34 */
	{
		HxLOG_Error("    USALS [34] invalid value for calculation result. Maybe error.\n");
		return MOTOR_DEGREE_INVALID;
	}

	dAngle_xx = (((dSatellite < dLongitude) && (dLatitude > 0)) || ((dSatellite > dLongitude) && (dLatitude < 0))) ?
			dAngle_xx : -dAngle_xx;							/* step #35 */
	HxLOG_Print("    USALS [35]  Deg(xx) : %s\n", pal_ch_PrintFloat(dAngle_xx, 3));

	if( ( (dSatellite < (-89.9)) && (dLongitude > 89.9) ) || ( (dSatellite > 89.9) && (dLongitude < (-89.9)) ) )
	{
		dAngle_xx = -dAngle_xx;								/* step #36, #37 */
	}

	if(dAngle_xx > 0)
	{
		sAngle_xx = (HINT16)( (dAngle_xx + 0.05) * 10.0);
	}
	else
	{
		sAngle_xx = (HINT16)( (dAngle_xx - 0.05) * 10.0);
	}

	HxLOG_Print("    USALS [37]  Deg(xx) : %d (x0.1) [Final]\n", sAngle_xx);

	return sAngle_xx;
}

HERROR PAL_CH_MOTOR_GotoSatellite(HUINT32 ulTunerGroupId, HINT16 sSatPos)
{
	HUINT32			ulValue;
	HINT16 			sMyLongitude, sMyLatitude;
	HERROR			hError;

	HxLOG_Info("PAL_CH_MOTOR_GotoSatellite(sSatPos:%d)\n", sSatPos);

	//hError = DB_PARAM_GetItem(eDB_PARAM_ITEM_MY_LONGITUDE, &ulValue, 0);
	if(hError != ERR_OK)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_MY_LONGITUDE) Error (%d) \n", hError);
	}
	sMyLongitude = (HINT16)ulValue;

	//hError = DB_PARAM_GetItem(eDB_PARAM_ITEM_MY_LATITUDE, &ulValue, 0);
	if(hError != ERR_OK)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_MY_LATITUDE) Error (%d) \n", hError);
	}
	sMyLatitude = (HINT16)ulValue;

	return PAL_CH_MOTOR_GotoDegree(ulTunerGroupId, local_motor_UsalsFormula(sSatPos, sMyLongitude, sMyLatitude));
}


/****************************************************************
************************** Converting ****************************
****************************************************************/

#define _____TUNING_INFO_CONVERTING_____

static HERROR pal_ch_sat_MakeSatTuneParamLnb (
						HUINT32							ulFrequency,
						HUINT32							ulSymbolRate,
						DxSat_CodeRate_e					fecCodeRate,
						DxSat_Polarization_e				polarization,
						DxSat_TransportSpec_e				transSpec,
						DxSat_PskModulation_e 			pskMod,
						DxSat_Pilot_e						pilot,
						HUINT32 						ulLnbFreq,
						DxSAT_LnbVoltage_e 				lnbVoltage,
						CH_SAT_TuneParam_t				*pstDiParam
						)
{
	if (pstDiParam == NULL)
	{
		return ERR_FAIL;
	}

	if (ulLnbFreq == CH_UNIVERSAL_LNB_FREQ)
	{
		ulLnbFreq = PAL_CH_LNB_GetActualLnbFreq(ulFrequency);
		pstDiParam->b22kTone = PAL_CH_LNB_GetActual22KTone(ulFrequency);
	}
	else if(ulLnbFreq >= (HUINT32)CH_MDU4_LNB_FREQ && ulLnbFreq <= (HUINT32)CH_MDU1_LNB_FREQ)
	{
		ulLnbFreq = PAL_CH_LNB_GetActualMduLnbFreq(ulLnbFreq, polarization, ulFrequency);
		pstDiParam->b22kTone = FALSE;
	}
	else
	{
		pstDiParam->b22kTone = FALSE;
	}

	pstDiParam->etAntennaType = DI_CH_ANT_TYPE_LNB;
	pstDiParam->ulFrequency = (HUINT32)abs((HINT32)ulFrequency - (HINT32)ulLnbFreq);
	pstDiParam->etLnbVoltage = pal_ch_sat_ConvPAL2DI_LnbVoltage(lnbVoltage);
	pstDiParam->etDiseqcInput = DI_CH_DISEQC_INPUT_NONE;
	pstDiParam->etToneBurst = DI_CH_TONE_BURST_NONE;
	pstDiParam->etDiseqcVersion = DI_CH_DISEQC_VER_1_0;
	pstDiParam->ulSymbolRate = ulSymbolRate; /* SDB, MW, DI : kSymbols/s */
	pstDiParam->etPolarization = pal_ch_sat_ConvPAL2DI_Polarization(polarization);
	pstDiParam->etFecCodeRate = pal_ch_sat_ConvPAL2DI_CodeRate(fecCodeRate);
	pstDiParam->etTransSpec = pal_ch_sat_ConvPAL2DI_TransSpec(transSpec);
	pstDiParam->etModulation = pal_ch_sat_ConvPAL2DI_Modulation(pskMod);
	pstDiParam->etPilot = pal_ch_sat_ConvPAL2DI_Pilot(pilot);
	pstDiParam->etRollOff = DI_CH_ROLL_035;

	return ERR_OK;
}

static HERROR pal_ch_sat_MakeSatTuneParamDiseqc (
						HUINT32							ulFrequency,
						HUINT32							ulSymbolRate,
						DxSat_CodeRate_e					fecCodeRate,
						DxSat_Polarization_e				polarization,
						DxSat_TransportSpec_e				transSpec,
						DxSat_PskModulation_e 			pskMod,
						DxSat_Pilot_e						pilot,
						HUINT32 						ulLnbFreq,
						DxSAT_LnbVoltage_e 				lnbVoltage,
						HBOOL							b22kTone,
						DxSAT_DiseqcVer_e					diseqcVersion,
						DxSAT_DiseqcInput_e				diseqcInput,
						DxSAT_ToneBurst_e					toneBurst,
						HUINT8							ucMotorPosition,
						HINT16							sSatLonggitude,
						CH_SAT_TuneParam_t				*pstDiParam
						)
{
	if (pstDiParam == NULL)
	{
		return ERR_FAIL;
	}

	if (ulLnbFreq == CH_UNIVERSAL_LNB_FREQ)
	{
		ulLnbFreq = PAL_CH_LNB_GetActualLnbFreq(ulFrequency);
		pstDiParam->b22kTone = PAL_CH_LNB_GetActual22KTone(ulFrequency);
	}
	else if(ulLnbFreq >= (HUINT32)CH_MDU4_LNB_FREQ && ulLnbFreq <= (HUINT32)CH_MDU1_LNB_FREQ)
	{
		ulLnbFreq = PAL_CH_LNB_GetActualMduLnbFreq(ulLnbFreq, polarization, ulFrequency);
		pstDiParam->b22kTone = b22kTone;
	}
	else
	{
		pstDiParam->b22kTone = b22kTone;
	}

	pstDiParam->etAntennaType = DI_CH_ANT_TYPE_DISEQC;
	pstDiParam->ulFrequency = (HUINT32)abs((HINT32)ulFrequency - (HINT32)ulLnbFreq);
	pstDiParam->etLnbVoltage = pal_ch_sat_ConvPAL2DI_LnbVoltage(lnbVoltage);
	pstDiParam->etDiseqcInput = pal_ch_sat_ConvPAL2DI_DiseqcInput(diseqcInput);
	pstDiParam->etToneBurst = pal_ch_sat_ConvPAL2DI_ToneBurst(toneBurst);
	pstDiParam->etDiseqcVersion = pal_ch_sat_ConvPAL2DI_DiseqcVersion(diseqcVersion);
	pstDiParam->ulSymbolRate = ulSymbolRate; /* SDB, MW, DI : kSymbols/s */
	pstDiParam->etPolarization = pal_ch_sat_ConvPAL2DI_Polarization(polarization);
	pstDiParam->etFecCodeRate = pal_ch_sat_ConvPAL2DI_CodeRate(fecCodeRate);
	pstDiParam->etTransSpec = pal_ch_sat_ConvPAL2DI_TransSpec(transSpec);
	pstDiParam->etModulation = pal_ch_sat_ConvPAL2DI_Modulation(pskMod);
	pstDiParam->etPilot = pal_ch_sat_ConvPAL2DI_Pilot(pilot);
	pstDiParam->etRollOff = DI_CH_ROLL_035;

	return ERR_OK;
}

static HERROR pal_ch_sat_MakeSatTuneParamScd (
						HUINT32 						ulFrequency,
						HUINT32 						ulSymbolRate,
						DxSat_CodeRate_e					fecCodeRate,
						DxSat_Polarization_e				polarization,
						DxSat_TransportSpec_e 			transSpec,
						DxSat_PskModulation_e 			pskMod,
						DxSat_Pilot_e 					pilot,
						HUINT32 						ulLnbFreq,
						DxSAT_LnbVoltage_e				lnbVoltage,
						HBOOL							b22kTone,
						DxSAT_ScdInput_e					scdInput,
						HUINT32 						ulScdUserBandNum,
						HUINT32 						ulScdUserBandFreq,
						CH_SAT_TuneParam_t				*pstDiParam
						)
{
	if (pstDiParam == NULL)
	{
		return ERR_FAIL;
	}

	if (ulLnbFreq == CH_UNIVERSAL_LNB_FREQ)
	{
		ulLnbFreq = PAL_CH_LNB_GetActualLnbFreq(ulFrequency);
		pstDiParam->b22kTone = PAL_CH_LNB_GetActual22KTone(ulFrequency);
	}
	else if(ulLnbFreq >= (HUINT32)CH_MDU4_LNB_FREQ && ulLnbFreq <= (HUINT32)CH_MDU1_LNB_FREQ)
	{
		ulLnbFreq = PAL_CH_LNB_GetActualMduLnbFreq(ulLnbFreq, polarization, ulFrequency);
		pstDiParam->b22kTone = b22kTone;
	}
	else
	{
		pstDiParam->b22kTone = b22kTone;
	}

	pstDiParam->etAntennaType = DI_CH_ANT_TYPE_SCD;
	pstDiParam->ulFrequency = (HUINT32)abs((HINT32)ulFrequency - (HINT32)ulLnbFreq);
	pstDiParam->etLnbVoltage = pal_ch_sat_ConvPAL2DI_LnbVoltage(lnbVoltage);

	if(scdInput == eDxSCD_INPUT_SCD_A)
	{
		pstDiParam->etDiseqcInput = DI_CH_DISEQC_INPUT_SCD_A;
	}
	else if(scdInput == eDxSCD_INPUT_SCD_B)
	{
		pstDiParam->etPolarization = DI_CH_DISEQC_INPUT_SCD_B;
	}
	else
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	pstDiParam->ulSymbolRate = ulSymbolRate;
	pstDiParam->etPolarization = pal_ch_sat_ConvPAL2DI_Polarization(polarization);
	pstDiParam->etFecCodeRate = pal_ch_sat_ConvPAL2DI_CodeRate(fecCodeRate);
	pstDiParam->etPolarization = pal_ch_sat_ConvPAL2DI_Polarization(polarization);
	pstDiParam->etTransSpec = pal_ch_sat_ConvPAL2DI_TransSpec(transSpec);
	pstDiParam->etModulation = pal_ch_sat_ConvPAL2DI_Modulation(pskMod);
	pstDiParam->etPilot = pal_ch_sat_ConvPAL2DI_Pilot(pilot);
	pstDiParam->etRollOff = DI_CH_ROLL_035;

	pstDiParam->ulScdUserBandNum = (HUINT8)ulScdUserBandNum;
	pstDiParam->ulScdUserBandFreq = (HUINT16)ulScdUserBandFreq;

	return ERR_OK;
}


static HERROR pal_ch_sat_MakeSatTuneParamSmatv (
						HUINT32							ulFrequency,
						HUINT32							ulSymbolRate,
						DxSat_CodeRate_e					fecCodeRate,
						DxSat_Polarization_e				polarization,
						DxSat_TransportSpec_e				transSpec,
						DxSat_PskModulation_e 			pskMod,
						DxSat_Pilot_e						pilot,
						CH_SAT_TuneParam_t				*pstDiParam
						)
{
	if (pstDiParam == NULL)
	{
		return ERR_FAIL;
	}

	pstDiParam->etAntennaType = DI_CH_ANT_TYPE_SMATV;

	pstDiParam->ulFrequency = ulFrequency;
	pstDiParam->ulSymbolRate = ulSymbolRate;
	pstDiParam->etFecCodeRate = pal_ch_sat_ConvPAL2DI_CodeRate(fecCodeRate);
	pstDiParam->etPolarization = pal_ch_sat_ConvPAL2DI_Polarization(polarization);
	pstDiParam->etTransSpec = pal_ch_sat_ConvPAL2DI_TransSpec(transSpec);
	pstDiParam->etModulation = pal_ch_sat_ConvPAL2DI_Modulation(pskMod);
	pstDiParam->etPilot = pal_ch_sat_ConvPAL2DI_Pilot(pilot);
	pstDiParam->etRollOff = DI_CH_ROLL_035;
	pstDiParam->b22kTone = FALSE;
	pstDiParam->etLnbVoltage = DI_CH_LNB_VOLT_STD;
	pstDiParam->etDiseqcInput = DI_CH_DISEQC_INPUT_NONE;
	pstDiParam->etToneBurst = DI_CH_TONE_BURST_NONE;
	pstDiParam->etDiseqcVersion = DI_CH_DISEQC_VER_1_0;

	return ERR_OK;
}

HERROR PAL_CH_LNB_MakeSatTuneParam (
						HUINT32							ulFrequency,
						HUINT32							ulSymbolRate,
						DxSat_CodeRate_e					fecCodeRate,
						DxSat_Polarization_e				polarization,
						DxSat_TransportSpec_e				transSpec,
						DxSat_PskModulation_e 			pskMod,
						DxSat_Pilot_e						pilot,
						HUINT32 						ulLnbFreq,
						DxSAT_LnbVoltage_e 				lnbVoltage,
						HUINT32							*pulDataSize,
						void							**ppvDiParam
						)
{
	CH_SAT_TuneParam_t	*pstDiParam;
	HERROR				 hErr;

	if (pulDataSize == NULL || ppvDiParam == NULL)
	{
		return ERR_FAIL;
	}

	pstDiParam = (CH_SAT_TuneParam_t *)HLIB_STD_MemAlloc (sizeof(CH_SAT_TuneParam_t));
	if (pstDiParam == NULL)
	{
		return ERR_FAIL;
	}

	HxSTD_memset (pstDiParam, 0, sizeof(CH_SAT_TuneParam_t));

	hErr = pal_ch_sat_MakeSatTuneParamLnb (ulFrequency, ulSymbolRate, fecCodeRate, polarization, transSpec,
											pskMod, pilot, ulLnbFreq, lnbVoltage, pstDiParam);

	if (hErr != ERR_OK)
	{
		HLIB_STD_MemFree (pstDiParam);
		return ERR_FAIL;
	}

	*pulDataSize = (HUINT32)sizeof(CH_SAT_TuneParam_t);
	*ppvDiParam  = (void *)pstDiParam;

	return ERR_OK;
}

HERROR PAL_CH_DISEQC_MakeSatTuneParam (
						HUINT32							ulFrequency,
						HUINT32							ulSymbolRate,
						DxSat_CodeRate_e					fecCodeRate,
						DxSat_Polarization_e				polarization,
						DxSat_TransportSpec_e				transSpec,
						DxSat_PskModulation_e 			pskMod,
						DxSat_Pilot_e						pilot,
						HUINT32 						ulLnbFreq,
						DxSAT_LnbVoltage_e 				lnbVoltage,
						HBOOL							b22kTone,
						DxSAT_DiseqcVer_e					diseqcVersion,
						DxSAT_DiseqcInput_e				diseqcInput,
						DxSAT_ToneBurst_e					toneBurst,
						HUINT8							ucMotorPosition,
						HINT16							sSatLonggitude,
						HUINT32 						*pulDataSize,
						void							**ppvDiParam
						)
{
	CH_SAT_TuneParam_t	*pstDiParam;
	HERROR				 hErr;

	if (pulDataSize == NULL || ppvDiParam == NULL)
	{
		return ERR_FAIL;
	}

	pstDiParam = (CH_SAT_TuneParam_t *)HLIB_STD_MemAlloc (sizeof(CH_SAT_TuneParam_t));
	if (pstDiParam == NULL)
	{
		return ERR_FAIL;
	}

	HxSTD_memset (pstDiParam, 0, sizeof(CH_SAT_TuneParam_t));

	hErr = pal_ch_sat_MakeSatTuneParamDiseqc (ulFrequency, ulSymbolRate, fecCodeRate, polarization, transSpec,
											pskMod, pilot, ulLnbFreq, lnbVoltage, b22kTone, diseqcVersion, diseqcInput,
											toneBurst, ucMotorPosition, sSatLonggitude, pstDiParam);

	if (hErr != ERR_OK)
	{
		HLIB_STD_MemFree (pstDiParam);
		return ERR_FAIL;
	}

	*pulDataSize = (HUINT32)sizeof(CH_SAT_TuneParam_t);
	*ppvDiParam  = (void *)pstDiParam;

	return ERR_OK;
}

HERROR PAL_CH_SCD_MakeSatTuneParam (
						HUINT32 						ulFrequency,
						HUINT32 						ulSymbolRate,
						DxSat_CodeRate_e					fecCodeRate,
						DxSat_Polarization_e				polarization,
						DxSat_TransportSpec_e 			transSpec,
						DxSat_PskModulation_e 			pskMod,
						DxSat_Pilot_e 					pilot,
						HUINT32 						ulLnbFreq,
						DxSAT_LnbVoltage_e				lnbVoltage,
						HBOOL							b22kTone,
						DxSAT_ScdInput_e					scdInput,
						HUINT32 						ulScdUserBandNum,
						HUINT32 						ulScdUserBandFreq,
						HUINT32 						*pulDataSize,
						void							**ppvDiParam
						)
{
	CH_SAT_TuneParam_t	*pstDiParam;
	HERROR				 hErr;

	if (pulDataSize == NULL || ppvDiParam == NULL)
	{
		return ERR_FAIL;
	}

	pstDiParam = (CH_SAT_TuneParam_t *)HLIB_STD_MemAlloc (sizeof(CH_SAT_TuneParam_t));
	if (pstDiParam == NULL)
	{
		return ERR_FAIL;
	}

	HxSTD_memset (pstDiParam, 0, sizeof(CH_SAT_TuneParam_t));

	hErr = pal_ch_sat_MakeSatTuneParamScd (ulFrequency, ulSymbolRate, fecCodeRate, polarization, transSpec,
											pskMod, pilot, ulLnbFreq, lnbVoltage, b22kTone,
											scdInput, ulScdUserBandNum, ulScdUserBandFreq, pstDiParam);

	if (hErr != ERR_OK)
	{
		HLIB_STD_MemFree (pstDiParam);
		return ERR_FAIL;
	}

	*pulDataSize = (HUINT32)sizeof(CH_SAT_TuneParam_t);
	*ppvDiParam  = (void *)pstDiParam;

	return ERR_OK;
}


HERROR PAL_CH_SMATV_MakeSatTuneParam (
						HUINT32							ulFrequency,
						HUINT32							ulSymbolRate,
						DxSat_CodeRate_e					fecCodeRate,
						DxSat_Polarization_e				polarization,
						DxSat_TransportSpec_e				transSpec,
						DxSat_PskModulation_e 			pskMod,
						DxSat_Pilot_e						pilot,
						HUINT32 						*pulDataSize,
						void							**ppvDiParam
						)
{
	CH_SAT_TuneParam_t	*pstDiParam;
	HERROR				 hErr;

	if (pulDataSize == NULL || ppvDiParam == NULL)
	{
		return ERR_FAIL;
	}

	pstDiParam = (CH_SAT_TuneParam_t *)HLIB_STD_MemAlloc (sizeof(CH_SAT_TuneParam_t));
	if (pstDiParam == NULL)
	{
		return ERR_FAIL;
	}

	HxSTD_memset (pstDiParam, 0, sizeof(CH_SAT_TuneParam_t));

	hErr = pal_ch_sat_MakeSatTuneParamSmatv (ulFrequency, ulSymbolRate, fecCodeRate, polarization, transSpec,
											pskMod, pilot, pstDiParam);

	if (hErr != ERR_OK)
	{
		HLIB_STD_MemFree (pstDiParam);
		return ERR_FAIL;
	}

	*pulDataSize = (HUINT32)sizeof(CH_SAT_TuneParam_t);
	*ppvDiParam  = (void *)pstDiParam;

	return ERR_OK;
}

#define _____SATELLITE_RF_INPUT_____
STATIC HERROR pal_ch_sat_checkRfInputConnectStatus (void)
{
	_palsat_TunerCapacity_t		*pstTuner;
	PalCh_SatRfInput_t			*pstRfInput;
	HUINT32						 ulRfInputId, ulTunerId;

	for (ulRfInputId = 0; ulRfInputId < s_ulPalChSat_RfInputNum; ulRfInputId++)
	{
		pstRfInput = &(s_astPalChSat_RfInput[ulRfInputId]);
		pstRfInput->ulConnectedTunerNum = 0;
	}

	for (ulTunerId = 0; ulTunerId < PAL_MAX_SATTUNER_NO; ulTunerId++)
	{
		pstTuner = &(s_astPalsat_TunerInfo[ulTunerId]);

		if ((TRUE == pstTuner->bAvailable) && (pstTuner->nRfInputId >= 0) && (pstTuner->nRfInputId < s_ulPalChSat_RfInputNum))
		{
			pstRfInput = &(s_astPalChSat_RfInput[pstTuner->nRfInputId]);

			pstRfInput->aulConnectedTunerId[pstRfInput->ulConnectedTunerNum] = ulTunerId;
			pstRfInput->ulConnectedTunerNum ++;
		}
	}

	return ERR_OK;
}


HUINT32 PAL_CH_SAT_CountRfInput (void)
{
	return s_ulPalChSat_RfInputNum;
}

HERROR PAL_CH_SAT_GetRfInput (HUINT32 ulRfInputIdx, PalCh_SatRfInput_t *pstRfInput)
{
	if ((ulRfInputIdx >= s_ulPalChSat_RfInputNum) || (NULL == pstRfInput))
	{
		return ERR_FAIL;
	}

	HxSTD_MemCopy (pstRfInput, &(s_astPalChSat_RfInput[ulRfInputIdx]), sizeof(PalCh_SatRfInput_t));
	return ERR_OK;
}

HERROR PAL_CH_SAT_SetRfInputConnection (HUINT32 ulTunerGroupId, HUINT32 ulRfInputId)
{
	DI_CHANNEL_ERR			 eDiErr;

	if ((ulTunerGroupId >= PAL_MAX_SATTUNER_NO) || (ulRfInputId >= s_ulPalChSat_RfInputNum))
	{
		return ERR_FAIL;
	}

	if (s_astPalsat_TunerInfo[ulTunerGroupId].nRfInputId == (HINT32)ulRfInputId)
	{
		return ERR_OK;
	}

	eDiErr = DI_CH_S_SetInputSource (ulTunerGroupId, ulRfInputId);
	if (DI_CH_OK != eDiErr)
	{
		return ERR_FAIL;
	}

	s_astPalsat_TunerInfo[ulTunerGroupId].nRfInputId = ulRfInputId;
	pal_ch_sat_checkRfInputConnectStatus();

	return ERR_OK;
}

HERROR PAL_CH_SAT_ResetRfInputConnection (HUINT32 ulTunerGroupId)
{
	if (ulTunerGroupId >= PAL_MAX_SATTUNER_NO)
	{
		return ERR_FAIL;
	}

	s_astPalsat_TunerInfo[ulTunerGroupId].nRfInputId = -1;
	pal_ch_sat_checkRfInputConnectStatus();

	return ERR_OK;
}

