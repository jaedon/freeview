/**
 * @file	osd.c
 * @brief	FGDL OSD C source
 * @version $Revision: 1.0.0 $
 * @date 	$Date: 2007/06/13 11:00:00 $
 * @author 	???
 */

/** @defgroup FGDL_OSD On Screen Display
 *  This is the module of FGDL OSD.
 *  @{
 */

/******************************************************************************
 * Include Files
 ******************************************************************************/
#include "htype.h"
#include "vkernel.h"

#include "di_channel.h"
#include "../inc/di_channel_priv.h"
#include "../inc/di_channel_attr.h"
#include "../inc/di_channel_i2c.h"
#include "../inc/drv_channel.h"


/******************************************************************************
 * Symbol and Macro Definitions
 ******************************************************************************/
/******************************************************************************
 * Data Types
 ******************************************************************************/

/******************************************************************************
 * Local(static) Data Types
 ******************************************************************************/

/******************************************************************************
 * Extern Variables
 ******************************************************************************/

/******************************************************************************
 * Global Variables
 ******************************************************************************/

/******************************************************************************
 * Local(static) Variables
 ******************************************************************************/

/******************************************************************************
 * Extern Function Prototypes
 ******************************************************************************/

/******************************************************************************
 * Local(static) Function Prototypes
 ******************************************************************************/

/**
 * This function register Front key callback.
 *
 * @param[in]   pfnNotify    Callback function
 *
 * @return      DDI_Err_eSUCCESS on success, or other DDI_Err_t on failure
 *
 * @remarks     This function is asynchronous.
 *
 * @see         DDI_KEY_DeRegisterCallBack
 */

#define C_MT5131_SAMSUNG_NIM_LO_DIVIDER_STEP 166670 /* Hz */
//static int Top=0; //ATC : 0>124dBuVpp, 1>121dBuVpp, 2>118dBuVpp, 3>115dBuVpp, 4>112dBuVpp, 5>109dBuVpp
//static int CP=3, ATC=0, F_STEP=2, MODE=0, TUNER_ONLY_MODEL;

typedef enum{
	DB1 = 0,	/* Divider Byte 1 */
	DB2,		/* Divider Byte 2 */
	CB1_1,		/* Control Byte 1 1st */
	CB2,		/* Control Byte 2 */
	CB1_2		/* Control Byte 1 2rd */
} TDA6551_Reg_t;

typedef enum{
	TDA6651_TP_124 = 0,			/* AL2 AL1 AL0 : 0 0 0, 124 dBuV(p-p) */
	TDA6651_TP_121,				/* AL2 AL1 AL0 : 0 0 1, 121 dBuV(p-p) */
	TDA6651_TP_118,				/* AL2 AL1 AL0 : 0 1 0, 118 dBuV(p-p) */
	TDA6651_TP_115,				/* AL2 AL1 AL0 : 0 1 1, 115 dBuV(p-p) */
	TDA6651_TP_112,				/* AL2 AL1 AL0 : 1 0 0, 112 dBuV(p-p) */
	TDA6651_TP_109,				/* AL2 AL1 AL0 : 1 0 1, 109 dBuV(p-p) */
	TDA6651_TP_IAGC0,			/* AL2 AL1 AL0 : 1 1 0, Iagc = 0 */
	TDA6651_TP_VAGC3_5,			/* AL2 AL1 AL0 : 1 1 1, Vagc = 3.5 */
} TDA6651_AGC_TP_LEVEL;

typedef enum{
	TDA6651_CP_1 = 0,			/* CP2 CP1 CP0 : 0 0 0 , typical current : 38uA */
	TDA6651_CP_2,				/* CP2 CP1 CP0 : 0 0 1 , typical current : 54uA */
	TDA6651_CP_3,				/* CP2 CP1 CP0 : 0 1 0 , typical current : 83uA */
	TDA6651_CP_4,				/* CP2 CP1 CP0 : 0 1 1 , typical current : 122uA */
	TDA6651_CP_5,				/* CP2 CP1 CP0 : 1 0 0 , typical current : 163uA */
	TDA6651_CP_6,				/* CP2 CP1 CP0 : 1 0 1 , typical current : 254uA */
	TDA6651_CP_7,				/* CP2 CP1 CP0 : 1 1 0 , typical current : 400uA */
	TDA6651_CP_8				/* CP2 CP1 CP0 : 1 1 1 , typical current : 580uA */
} TDA6651_CP_NUM;

typedef enum{
	TDA6651_BAND_LOW = 01,		/* BS5 BS4 BS3 BS2 BS1 : 0 0 0 0 1 */
	TDA6651_BAND_MID = 02,		/* BS5 BS4 BS3 BS2 BS1 : 0 0 0 1 0 */
	TDA6651_BAND_HIGH = 04		/* BS5 BS4 BS3 BS2 BS1 : 0 0 1 0 0 */
#if(0)
TDA6651_BAND_LOW = 17,		/* BS5 BS4 BS3 BS2 BS1 : 1 0 0 0 1 */
TDA6651_BAND_MID = 18,		/* BS5 BS4 BS3 BS2 BS1 : 1 0 0 1 0 */
TDA6651_BAND_HIGH = 24		/* BS5 BS4 BS3 BS2 BS1 : 1 1 0 0 0 */
#endif
} TDA6651_BAND_SWITCHING;

typedef enum{
	TDA6651_ATC_0 = 0,			/* AGC current = 200nA, AGC time constant = 2s */
	TDA6651_ATC_1				/* AGC current = 9uA, AGC time constant = 50ms */
} TDA6651_ATC_MODE;

typedef enum{
	TDA6551_PLL_STEP_62500 = 0,		/* 62.5(kHz),	R2 R1 R0 : 0 0 0 */
	TDA6551_PLL_STEP_142860,		/* 142.86(kHz),	R2 R1 R0 : 0 0 1 */
	TDA6551_PLL_STEP_166670,		/* 166.67(kHz),	R2 R1 R0 : 0 1 0 */
	TDA6551_PLL_STEP_50000,			/* 50(kHz),		R2 R1 R0 : 0 1 1 */
	TDA6551_PLL_STEP_125000			/* 125(kHz),	R2 R1 R0 : 1 0 0 */
} TDA6651_PLL_REF_DIVIDER;

typedef enum{
	TDA6651_NORMAL_XTOUT_OFF = 0,		/* T2 T1 T0 : 0 0 0, XTOUT : Disable, normal mode with XTOUT buffer off */
	TDA6651_NORMAL_XTOUT_ON,			/* T2 T1 T0 : 0 0 1, XTOUT : fxtal(4MHz), normal mode with XTOUT buffer on */
	TDA6651_CP_OFF,						/* T2 T1 T0 : 0 1 0, XTOUT : (1/2)fdiv, charge pump off */
	TDA6651_ALBC_TOGGLE,				/* T2 T1 T0 : 0 1 1, XTOUT : fxtal(4MHz), switch ALBC on or off */
	TDA6651_TEST_FCOMP,					/* T2 T1 T0 : 1 0 0, XTOUT : fcomp, test mode */
	TDA6651_TEST_HALFFDIV,				/* T2 T1 T0 : 1 0 1, XTOUT : (1/2)fdiv, test mode */
	TDA6651_CP_SINK,					/* T2 T1 T0 : 1 1 0, XTOUT : fxtal(4MHz), charge pump sinking current */
	TDA6651_CP_SOURCING					/* T2 T1 T0 : 1 1 1, XTOUT : disabled, charge pump sourcing current */
} TDA6651_TEST_MODE;

static unsigned int TDA6651_SetPLLRefDivider(TDA6651_PLL_REF_DIVIDER pllStepSize)
{
	unsigned int	pllStepSizeVal;

	switch(pllStepSize)
	{
		case TDA6551_PLL_STEP_62500:
			pllStepSizeVal = 62500;	/* Unit : Hz */
			break;

		case TDA6551_PLL_STEP_142860:
			pllStepSizeVal = 142860;	/* Unit : Hz */
			break;

		case TDA6551_PLL_STEP_166670:
			pllStepSizeVal = 166670;	/* Unit : Hz */
			break;

		case TDA6551_PLL_STEP_50000:
			pllStepSizeVal = 50000;	/* Unit : Hz */
			break;

		case TDA6551_PLL_STEP_125000:
			pllStepSizeVal = 125000;	/* Unit : Hz */
			break;

		default:	/* default : PLL_STEP_50000 */
			CH_PrintDebug("[setTunerPLLRefDivider]Default Step Size : 50000\n");
			pllStepSizeVal = 50000;	/* Unit : Hz */
			break;
	}

	return pllStepSizeVal;
}
static unsigned char TDA6651_SetChargePumpCurrent(unsigned int freq)  // Digital Charge Pump setting
{
	unsigned char	cpVal;

	if(freq <= 126000) 	cpVal = TDA6651_CP_3;
	else if((freq > 126000) && (freq <= 150000)) cpVal = TDA6651_CP_4;
	else if((freq > 150000) && (freq <= 175000)) cpVal = TDA6651_CP_5;
	else if((freq > 175000) && (freq <= 193000)) cpVal = TDA6651_CP_7;
	else if((freq > 193000) && (freq <= 336000)) cpVal = TDA6651_CP_3;
	else if((freq > 336000) && (freq <= 393000)) cpVal = TDA6651_CP_4;
	else if((freq > 393000) && (freq <= 425000)) cpVal = TDA6651_CP_5;
	else if((freq > 425000) && (freq <= 456000)) cpVal = TDA6651_CP_6;
	else if((freq > 456000) && (freq <= 480000)) cpVal = TDA6651_CP_7;
	else if((freq > 480000) && (freq <= 691000)) cpVal = TDA6651_CP_4;
	else if((freq > 691000) && (freq <= 811000)) cpVal = TDA6651_CP_5;
	else if((freq > 811000) && (freq <= 859000)) cpVal = TDA6651_CP_6;
	else if((freq > 859000) && (freq <= 896000)) cpVal = TDA6651_CP_8;
	else	cpVal = TDA6651_CP_3;

	return cpVal;
}

static unsigned char TDA6651_DVB_C_SetChargePumpCurrent(unsigned int freq)  // Digital Charge Pump setting
{
	unsigned char	cpVal;

	if(freq <= 126000) 	cpVal = TDA6651_CP_5;
	else if((freq > 126000) && (freq <= 166000)) cpVal = TDA6651_CP_6;
	else if((freq > 166000) && (freq <= 176000)) cpVal = TDA6651_CP_7;
	else if((freq > 176000) && (freq <= 196000)) cpVal = TDA6651_CP_8;
	else if((freq > 196000) && (freq <= 306000)) cpVal = TDA6651_CP_5;
	else if((freq > 306000) && (freq <= 386000)) cpVal = TDA6651_CP_6;
	else if((freq > 386000) && (freq <= 435000)) cpVal = TDA6651_CP_7;
	else if((freq > 435000) && (freq <= 482000)) cpVal = TDA6651_CP_8;
	else if((freq > 482000) && (freq <= 636000)) cpVal = TDA6651_CP_5;
	else if((freq > 636000) && (freq <= 736000)) cpVal = TDA6651_CP_6;
	else if((freq > 736000) && (freq <= 836000)) cpVal = TDA6651_CP_7;
	else if((freq > 836000) && (freq <= 903000)) cpVal = TDA6651_CP_8;
	else	cpVal = TDA6651_CP_5;

	return cpVal;
}
static unsigned char TDA6651_SetBandSwitching(unsigned int freq)
{
	unsigned char	bsVal = TDA6651_BAND_HIGH;

	if(freq <= 157000)
	{
		bsVal = TDA6651_BAND_LOW;
	}
	else if((freq > 157000) && (freq <= 443000))
	{
		bsVal = TDA6651_BAND_MID;
	}
	else if(freq > 443000)
	{
		bsVal = TDA6651_BAND_HIGH;
	}

	return bsVal;
}

static unsigned char TDA6651_SetTakeOverPoint(unsigned int freq)
{
	unsigned char	tpVal ;

	if(freq < 443000)	/* VHF */
	{
		tpVal = TDA6651_TP_124;
	}
	else				/* UHF */
	{
		tpVal = TDA6651_TP_124;
	}

	return tpVal;
}

HBOOL  FQD1116AME_Tune( int nChannelType, HUINT32 ulFreqHz)
{
	//unsigned char	tuningData[4] = {0,};
	unsigned char	tuningData[5] = {0,};

	unsigned int 	pllStepVal;						/* Real value of PLL Step Size : Unit(Hz) */
	unsigned short 	divierRatio;					/* Divider Ratio */
	unsigned char	chargePump;						/* PLL Charge Pump Current */
	unsigned char	bandSwitching;					/* */
	unsigned char	tunerLockFlag = TRUE;
//	unsigned char	testMode;
	unsigned char	atcVal;
	unsigned char	takeoverPoint;
	//unsigned int	freqInput;
	unsigned int	pllStepSize;
	unsigned int	freqIF;
	unsigned int	LOfrequency;

	//freqInput = dwFreqkHz * 1000;	/* KHz -> Hz */
	if( nChannelType == DI_CH_TYPE_TER )
	{
		CH_PrintDebug(" DI_CH_TYPE_TER\n");
		pllStepSize = TDA6551_PLL_STEP_166670;
		freqIF = 36166666;
	}
	else if( nChannelType == DI_CH_TYPE_CAB )
	{
		CH_PrintDebug(" DI_CH_TYPE_CAB\n");
		pllStepSize = TDA6551_PLL_STEP_62500;
		freqIF = 36150000;
	}
	else
	{
		CH_PrintErr("");
	}

	LOfrequency = ulFreqHz + freqIF;

	CH_PrintDebug(" pllStepSize = %d\n", pllStepSize);
	CH_PrintDebug(" freqIF = %d\n", freqIF);

	CH_PrintDebug(" Selected tuner is FQD1116AME/BH freq = %d\n", ulFreqHz);

	/* Set PLL Step Size */
	pllStepVal = TDA6651_SetPLLRefDivider(pllStepSize);

	/* 1. Frequency Divider */
	/* Calculate the Divider Ratio(N) : N = (Input Frequency + IF Frequency)/(Reference Frequency) */
	divierRatio = (unsigned short)((ulFreqHz + freqIF)/(pllStepVal*1.0) + 0.5);

	/* Set the value to Register DIV1, DIV2 */
	tuningData[DB1] = (unsigned char)((divierRatio>>8) & 0x007f);
	tuningData[DB2] = (unsigned char)(divierRatio & 0x00ff);
	CH_PrintDebug(" divierRatio = %d\n", divierRatio);

	/* 2. PLL Reference Divider */
	tuningData[CB1_1] = (0xC8 | (pllStepSize));

	/* 3. Charge Pump & Band Selection */
	 if( nChannelType == DI_CH_TYPE_CAB )
	{
		chargePump = TDA6651_DVB_C_SetChargePumpCurrent(LOfrequency/1000);
		CH_PrintDebug("\n TDA6651_DVB_C_SetChargePumpCurrent ******  DI_CH_TYPE_CAB\n");
	}
	 else
	{
		chargePump = TDA6651_SetChargePumpCurrent(LOfrequency/1000);
		CH_PrintDebug(" \n TDA6651_SetChargePumpCurrent ********DI_CH_TYPE_TER\n");
	}
	bandSwitching = TDA6651_SetBandSwitching(ulFreqHz/1000);
	tuningData[CB2] = (chargePump << 5) | bandSwitching;

	/* Set the CB1 : AGC Take-Over-Point */
	atcVal = TDA6651_ATC_1;
	takeoverPoint = TDA6651_SetTakeOverPoint(LOfrequency/1000);
	tuningData[CB1_2] = (0x80 | (atcVal << 3) | (takeoverPoint));
//	tuningData[CB1_2] = 0x81;

	/* Display the Tuning Data */
	CH_PrintDebug("tuningData :[0]=0x%x, [1]=0x%x, [2]=0x%x, [3]=0x%x, [4]=0x%x\n",
	tuningData[DB1],tuningData[DB2],tuningData[CB1_1],tuningData[CB2], tuningData[CB1_2]);


	/* Write to the tuner : 2nd Tuning */
	//drv_tuner_i2c_Write(0, 5, &tuningData[DB1]);
	DRV_CH_WriteI2c( CONST_CH_I2C_BUS, FQD1116_TUNER_ADDRESS, 0, 0, tuningData, 5);
	//DRV_I2C_Write (0, 0xc2, NULL, 0, tuningData, 5);

    VK_TASK_Sleep(50);

	atcVal = TDA6651_ATC_0;
	takeoverPoint = TDA6651_SetTakeOverPoint(LOfrequency/1000);
	tuningData[CB1_2] = (0x80 | (atcVal << 3) | (takeoverPoint));
//	tuningData[CB1_2] = 0x81;

	/* Display the Tuning Data */
	CH_PrintDebug("tuningData :[0]=0x%x, [1]=0x%x, [2]=0x%x, [3]=0x%x, [4]=0x%x\n",
	tuningData[DB1],tuningData[DB2],tuningData[CB1_1],tuningData[CB2], tuningData[CB1_2]);

	DRV_CH_WriteI2c( CONST_CH_I2C_BUS, FQD1116_TUNER_ADDRESS, 0, 0, tuningData, 5);

    VK_TASK_Sleep(20);

	return tunerLockFlag;
}

typedef enum{
	W0 = 0,
	W1,
	W2,
	W3,
	W4,
	W5,
	W6,
	W7,
	W8,
	W9,
	W10,
} TDA9898_Reg_t;

typedef enum{
	TDA9898_TV_DTV = 0,
	TDA9898_TV_ANALOG = 1
} TDA9898_TV_MODE;

typedef enum{
	TDA9898_BG_STANDARD = 0,
	TDA9898_I_STANDARD = 1,
	TDA9898_DK_STANDARD = 2,
	TDA9898_L_STANDARD = 3,
	TDA9898_L_ACCENT_STANDARD = 4
} TDA9898_SOUNDBANDPASS_STANDARD;

typedef enum{
	TDA9898_INPUT_PAL = 0,
	TDA9898_INPUT_BG = 1,
	TDA9898_INPUT_I = 2,
	TDA9898_INPUT_DK = 3,
	TDA9898_INPUT_SECAM_L = 4,
	TDA9898_INPUT_SECAM_L_ACCENT = 5,
	TDA9898_INPUT_DTV = 6
}TDA9898_IF_INPUT_SELECTION;

typedef enum{
	TDA9898_TOP_LOOP_M11 = 0,
	TDA9898_TOP_LOOP_0,
	TDA9898_TOP_LOOP_P15
}TDA9898_TOP_LOOP;

typedef enum{
	TDA9898_TOP_IFAGC_M19 = 0,
	TDA9898_TOP_IFAGC_0 = 16,
	TDA9898_TOP_IFAGC_P17 = 31
}TDA9898_TOP_IFAGC;

typedef enum{
	TDA9898_MODULATION_NEGATIVE = 0,
	TDA9898_MODULATION_POSITIVE = 1,
}TDA9898_MODULATION;

unsigned char TDA9898_SetTVMode(TDA9898_TV_MODE tvMode)
{
	unsigned char retTVMode;

	switch(tvMode)
	{
		case TDA9898_TV_DTV :
			retTVMode = TDA9898_TV_DTV;
			break;

		case TDA9898_TV_ANALOG :
			retTVMode = TDA9898_TV_ANALOG;
			break;

		default :
			retTVMode = TDA9898_TV_DTV;
			break;
	}

	return retTVMode;
}

void TDA9898_Set2ndSoundIF(TDA9898_SOUNDBANDPASS_STANDARD standard, unsigned char *mod, unsigned char *std)
{
	switch(standard)
	{
		case TDA9898_BG_STANDARD :
			*mod = 1;	/* MOD = X, Dont care지만 1로 셋팅 */
			*std = 1;	/* STD1 = 0, STD0 = 1 */
			break;

		case TDA9898_I_STANDARD :
			*mod = 1;	/* MOD = X, Dont care지만 1로 셋팅 */
			*std = 2;	/* STD1 = 1, STD0 = 0 */
			break;

		case TDA9898_DK_STANDARD :
			*mod = 1;	/* MOD = 1 */
			*std = 1;	/* STD1 = 0, STD0 = 1 */
			break;

		case TDA9898_L_STANDARD :
		case TDA9898_L_ACCENT_STANDARD :
			*mod = 0;	/* MOD = 0 */
			*std = 3;	/* STD1 = 1, STD0 = 1 */
			break;

		default :
			*mod = 0;
			*std = 0;
			break;
	}
}

void TDA9898_SetVisionIF(TDA9898_SOUNDBANDPASS_STANDARD standard, unsigned *sb, unsigned char *mod, unsigned char *std)
{
	switch(standard)
	{
		case TDA9898_BG_STANDARD :
			*sb = 0;	/* SB = 0(b) */
			*mod = 1;	/* MOD = 1(b) */
			*std = 3;	/* STD[4:2] = 011(b) */
			break;

		case TDA9898_I_STANDARD :
			*sb = 0;	/* SB = 0(b) */
			*mod = 1;	/* MOD = 1(b) */
			*std = 3;	/* STD[4:2] = 011(b) */
			break;

		case TDA9898_DK_STANDARD :
			*sb = 0;	/* SB = 0(b) */
			*mod = 1;	/* MOD = 1(b) */
			*std = 3;	/* STD[4:2] = 011(b) */
			break;

		case TDA9898_L_STANDARD :
			*sb = 0;	/* SB = 0(b) */
			*mod = 0;	/* MOD = 0(b) */
			*std = 3;	/* STD[4:2] = 011(b) */
			break;

		case TDA9898_L_ACCENT_STANDARD :
			*sb = 0;	/* SB = 0(b) */
			*mod = 0;	/* MOD = 0(b) */
			*std = 7;	/* STD[4:2] = 111(b) */
			break;

		default :	/* Set to BG Standard */
			*sb = 1;	/* SB = 1(b) */
			*mod = 1;	/* MOD = 1(b) */
			*std = 3;	/* STD[4:2] = 011(b) */
			break;
	}
}

void TDA9898_SetInputSelection(TDA9898_IF_INPUT_SELECTION inputSelection, unsigned char *iIFIn, unsigned char *vIFIn)
{
	switch(inputSelection)
	{
		case TDA9898_INPUT_PAL:
		case TDA9898_INPUT_BG :
		case TDA9898_INPUT_I :
		case TDA9898_INPUT_DK :
		case TDA9898_INPUT_SECAM_L :
		case TDA9898_INPUT_DTV :
			*iIFIn = 0;
			*vIFIn = 0;
			break;

		case TDA9898_INPUT_SECAM_L_ACCENT :
			*iIFIn = 2;
			*vIFIn = 1;
			break;

		default :
			*iIFIn = 0;
			*vIFIn = 0;
			break;
	}
}

void TDA9898_SetSoundTrap(TDA9898_SOUNDBANDPASS_STANDARD standard, unsigned char *fsFreq)
{
	switch(standard)
	{
		case TDA9898_BG_STANDARD :
			*fsFreq = 1;	/* FSFREQ1 FSFREQ0 : 0 1 */
			break;

		case TDA9898_I_STANDARD :
			*fsFreq = 2;	/* FSFREQ1 FSFREQ0 : 1 0 */
			break;

		case TDA9898_DK_STANDARD :
		case TDA9898_L_STANDARD :
		case TDA9898_L_ACCENT_STANDARD :
			*fsFreq = 3;	/* FSFREQ1 FSFREQ0 : 1 1 */
			break;
		default :	/* Set to BG Standard */
			*fsFreq = 1;	/* FSFREQ1 FSFREQ0 : 0 1 */
			break;
	}
}

unsigned char TDA9898_SetAGCMode(TDA9898_MODULATION agcMode)
{
	unsigned char retAGCMode;

	switch(agcMode)
	{
		case TDA9898_MODULATION_NEGATIVE :
			retAGCMode = 2;		/* 10(b) = negative modulation */
			break;

		case TDA9898_MODULATION_POSITIVE :
			retAGCMode = 3;		/* 11(b) = positive modulation */
			break;

		default :	/* Set to NEGATIVE MODULATION */
			retAGCMode = 2;		/* 10(b) = negative modulation */
			break;
	}

	return retAGCMode;
}

unsigned int TDA9898_SetTOPforLOOP(TDA9898_TOP_LOOP topLoop)
{
	unsigned int retTop;

	switch(topLoop)
	{
		case TDA9898_TOP_LOOP_M11 :
			retTop = 0;
			break;

		case TDA9898_TOP_LOOP_0 :
			retTop = 16;
			break;

		case TDA9898_TOP_LOOP_P15 :
			retTop = 31;
			break;
	}

	return retTop;
}

unsigned int TDA9898_SetTOPforIFAGC(TDA9898_TOP_IFAGC topIFAgc)
{
	unsigned int retTop;

	switch(topIFAgc)
	{
		case TDA9898_TOP_IFAGC_M19 :
			retTop = 0;
			break;

		case TDA9898_TOP_IFAGC_0 :
			retTop = 16;
			break;

		case TDA9898_TOP_IFAGC_P17 :
			retTop = 31;
			break;
	}

	return retTop;
}


HUINT8 FQD1116_SetDigital( CH_SelectIf_e etSelectIf )
{
	HUINT8	ret = TRUE;
	unsigned char tuningData[11] = {0,};
//	unsigned char i;
//	unsigned char TV2, STD1_0, STD4_2, MOD, SB, IIFIN, VIFIN, FSFREQ, TAGC, TOPN, TOPP;

	CH_PrintDebug("etSelectIf(%d)\n", etSelectIf);

	if( etSelectIf == LOW_IF ) //(HB1, HC1 TER)
	{
	/* Ref Board */
	tuningData[W0] = 0x00;
	tuningData[W1] = 0x28;
	tuningData[W2] = 0x88;
	tuningData[W3] = 0x60;
	tuningData[W4] = 0x66;
	tuningData[W5] = 0xad;
	tuningData[W6] = 0x00;
	tuningData[W7] = 0x04;
	tuningData[W8] = 0x00;
	tuningData[W9] = 0x00;
	tuningData[W10] = 0x00;
	}
	else //DIRECT_IF (DB1 TER, HC1 CAB)
	{
	tuningData[0] = 0x00; //Subaddress of Data register W1
	tuningData[1] = 0x00; //data1
	tuningData[2] = 0x00;
	tuningData[3] = 0x00;
	tuningData[4] = 0x67;
	tuningData[5] = 0x1f;
	tuningData[6] = 0x01;
//	tuningData[7] = 0x04;
	tuningData[7] = 0x00;
	tuningData[8] = 0x00;
	tuningData[9] = 0x00;
	tuningData[10] = 0x00;
	}

#if 0
	for(i = 0; i < 11; i++)
	{
		CH_PrintDebug("[FQD1116_SetDigital] tuningData[%d] = 0x%x\n",i, tuningData[i]);
	}
#endif

	//drv_tuner9898_i2c_Write(0, 11, &tuningData[W0]);
	DRV_CH_WriteI2c( CONST_CH_I2C_BUS, FQD1116_IF_ADDRESS, 0, 0, tuningData, 11);

    //VK_TASK_Sleep(1000);

	return ret;
}

/** @} */ // end of FGDL_OSD

