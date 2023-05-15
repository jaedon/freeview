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
#include "di_channel_priv.h"
#include "di_channel_attr.h"
//#include "./inc/di_channel_i2c.h"
#include "drv_channel.h"
#include "drv_i2c.h"
#include "drv_gpio.h"

#include "drv_channel_dp10024.h"
// End Including Headers
#if defined(PLATFORM_TM2063)
#include "mt2063.h"
#include "mt_userdef.h"
#endif

#if defined(PLATFORM_TN182I2)
#include "TDA182I2.h"
#endif

#if defined(PLATFORM_TM201)
#include "MxL201RF.h"
#include "MxL201RF_API.h"
#endif

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
// Start #define
//#define MAX_NUM_CAB_UNITS 2

////////////////////////////////////////////////////////////////
// AGC referance value Control Value
////////////////////////////////////////////////////////////////
#if defined(PLATFORM_TM201)
#define TDA10023_AGC_REF_NORMAL				(0x30)
#else
#define TDA10023_AGC_REF_NORMAL				(0x50)
#endif
////////////////////////////////////////////////////////////////
// AGC MIN, MAX Control Value
////////////////////////////////////////////////////////////////
#define TDA10023_AGC_MAX				(0xFF)
#define TDA10023_AGC_MIN				(0x00)

// Carrier frequency setting
//#define TUNER_FREQ_MIN			49000000  // PAL min. center freq
#define TUNER_FREQ_MIN			47000000  // PAL min. center freq

#define TUNER_FREQ_MAX			862000000 // PAL max. center freq
#define	TUNER_MIN_STEP			62500     // Tuner step freq

// Symbol rate setting
#define SR_MIN					1000 // min. SR value
#define SR_MAX					7200 // max. SR value

// interrupt period:  1000 ms
#define CH_TIMER_PERIEOD			1000
#define CH_TIMEOUT_CNT				1 // period = CH_TIMEOUT_CNT * 1000 msec
#define GET_DATA_INTERVAL			1 // Timer interval

#define DEFAULT_CAB_QAM				DI_CH_CMOD_64QAM
#define DEFAULT_CAB_SYMBOLRATE		3000

#define MASTER_DEMOD	0
#define SLAVE_DEMOD		1

#define MASTER_TUNER	0
#define SLAVE_TUNER		1

#define TDA10024_M_DEMOD_ADDRESS	0x0C //0x18
#define TDA10024_S_DEMOD_ADDRESS		0x0D //0x1a

#define SERIAL_MODE_OUTPUT
#define OUTPUT_RISING_EDGE


#if defined(PLATFORM_TM2063)
#define BOTH_CHBOARD    // Support different slave tuner I2C Address for Rev0.1, Rev0.2

#define M_TUNER_I2C_ADDR	0x60		//0xC0
#ifdef BOTH_CHBOARD
unsigned int S_TUNER_I2C_ADDR = 0x61; 		//0xC2
#else
#define S_TUNER_I2C_ADDR	0x61		//0xC2
#endif

#define CONST_LEVEL_NUM		61 //41
#endif

// End #define


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
// Start typedef
typedef struct
{
	HINT32			nStrength;			/**< Signal strength percentage Unit */
	HULONG			ulQuality;			/**< Signal quality percentage Unit */
	HUINT8			ucAgc;				/**< AGC */
	HULONG			ulBer;				/**< BER */
	HULONG			ulSnr;				/**< SNR */
	HUINT8			ucRfAgc;			/**< RFAGC */
	HUINT8			ucIfAgc;			/**< IFAGC */
	HULONG			ulUncorrected;		/**< Uncorrected */
	HFLOAT32		fSignalPower;		/**< Signal strength dBm or dBmV or dBuV */
}TDA10024_SignalQuality_t;

static TDA10024_SignalQuality_t st_10024SignalStatus[NUM_OF_CAB_CHANNEL];

// End typedef


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
// Start global variablee
#if (NUM_OF_CAB_CHANNEL==1)
const unsigned long g_ulSysClk[NUM_OF_CAB_CHANNEL] = {58666};
#elif (NUM_OF_CAB_CHANNEL==2)
const unsigned long g_ulSysClk[NUM_OF_CAB_CHANNEL] = {58666, 58666};
#else
const unsigned long g_ulSysClk[NUM_OF_CAB_CHANNEL] = {58666};
#endif

#if defined(PLATFORM_TM2063)
static Handle_t hTuner[NUM_OF_CAB_CHANNEL];
#endif
// End global variable

#if defined(PLATFORM_TN182I2)
void TN182I2_InitTuner(int nUnitId);
TDA182I2_ERR_CODE TN182I2_PowerMeasurement(int nUnitId, int* pnRFin);
#endif

#if defined(PLATFORM_TM2063)
void TM2063_InitTuner(int nUnitId);
#endif

#if defined(PLATFORM_TM201)
MxL201RF_TunerConfigS g_stSemTunerConfig[NUM_OF_CAB_CHANNEL];

#define M_TUNER_I2C_ADDR	0x60		//0xC0
#define S_TUNER_I2C_ADDR	0x63		//0xC6
int TM201_InitTuner(int nUnitId);
#endif

extern HINT32 show_di_ch;


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
// Start static variablee
static unsigned char	s_aucConfReg[NUM_OF_CAB_CHANNEL];			// basic configuration contgent
static unsigned char	s_aucClkConfReg[NUM_OF_CAB_CHANNEL]; 		// for calculating clock offset
static unsigned char	s_aucCarConfReg[NUM_OF_CAB_CHANNEL]; 		// for calculating carrier offset
static unsigned char	s_aucGainReg[NUM_OF_CAB_CHANNEL];    		// GAIN register content
static unsigned char	s_aucNyquistGain[NUM_OF_CAB_CHANNEL];			// Nyquist filter gain

static unsigned char s_ucIsLock[NUM_OF_CAB_CHANNEL];


/* Rev 0.2 Board */
#if defined(PLATFORM_TM2063)
static unsigned int aulRfAgc_94[CONST_LEVEL_NUM] = {
	25,24,22,21,19,19,18,18,17,15,
	13,13,11,13,12,10,10,8,8,6,
	6,4,4,2,1,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0
};

static unsigned int aulIfAgc_94[CONST_LEVEL_NUM] = {
	12,12,9,9,6,11,11,16,15,11,
	9,14,9,6,8,2,8,3,9,3,
	9,4,10,5,6,7,13,18,24,29,
	35,40,46,52,58,63,69,75,80,86,
	92,97,103,109,115,121,127,132,138,144,
	150,156,162,168,174,179,185,190,196,201,
	207
};

static unsigned int aulRfAgc_506[CONST_LEVEL_NUM] = {
	25,22,22,21,20,18,18,17,16,15,
	13,13,11,11,9,7,6,6,5,3,
	3,1,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0
};

static unsigned int aulIfAgc_506[CONST_LEVEL_NUM] = {
	16,8,13,14,15,11,11,12,13,13,
	9,14,9,15,10,7,6,12,9,4,
	10,5,7,13,19,24,30,35,41,47,
	52,58,64,69,75,80,86,92,98,103,
	109,116,121,127,133,140,146,151,157,163,
	169,175,181,187,193,198,203,208,255,255,
	255
};

static unsigned int aulRfAgc_810[CONST_LEVEL_NUM] = {
	25,22,22,21,20,18,18,17,16,15,
	13,13,11,11,9,7,6,6,5,3,
	3,1,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0
};

static unsigned int aulIfAgc_810[CONST_LEVEL_NUM] = {
	16,8,13,14,15,11,11,12,13,13,
	9,14,9,15,10,7,6,12,9,4,
	10,5,7,13,19,24,30,35,41,47,
	52,58,64,69,75,80,86,92,98,103,
	109,116,121,127,133,140,146,151,157,163,
	169,175,181,187,193,198,203,208,255,255,
	255
};

static int aulDownLevel[CONST_LEVEL_NUM] = {
	30, 29, 28, 27, 26, 25, 24, 23, 22, 21,
	20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10,
	9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
	-1, -2, -3, -4, -5, -6, -7, -8, -9, -10,
	-11, -12, -13, -14, -15, -16, -17, -18, -19, -20,
	-21, -22, -23, -24, -25, -26, -27, -28, -29, -30
};

#if 0
/* Rev 0.1 Board */

static unsigned int aulRfAgc_94[41] = {
	29,27,27,26,25,24,24,22,21,21,
	18,18,18,16,16,13,13,13,9,9,
	7,7,5,5,3,2,2,0,0,
	1,1,0,0,0,0,0,0,0,0,
	0
};

static unsigned int aulIfAgc_94[41] = {
	117,117,117,117,117,117,123,118,119,126,
	116,122,128,123,129,120,125,131,116,122,
	116,121,116,122,117,118,124,116,122,128,
	134,139,145,151,157,163,168,173,179,185,
	191
};
#endif
#if 0
static unsigned int aulRfAgc_94[41] = {
	28,25,24,24,23,22,20,18,18,18,
	15,14,13,12,12,10,8,8,6,6,
	6,5,3,3,2,2,2,1,1,1,
	1,1,0,0,0,0,0,0,0,0,
	0
};

static unsigned int aulIfAgc_94[41] = {
	92,83,81,87,87,87,83,78,83,89,
	80,80,81,82,87,83,78,84,79,85,
	90,90,85,90,92,97,102,103,108,114,
	119,125,124,130,136,141,147,153,159,165,
	171
};

static unsigned int aulRfAgc_506[41] = {
	24,23,22,22,21,20,19,17,16,15,
	15,12,12,10,9,9,8,6,6,6,
	5,2,2,2,1,1,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0
};

static unsigned int aulIfAgc_506[41] = {
	80,79,79,85,85,88,85,80,80,81,
	90,82,82,82,78,84,84,79,84,90,
	97,88,93,98,98,103,101,106,111,118,
	123,128,134,140,145,151,158,164,170,176,
	181
};

static unsigned int aulRfAgc_810[41] = {
	24,24,21,21,19,19,19,16,16,14,
	14,14,12,10,9,8,8,6,6,3,
	3,2,1,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0
};

static unsigned int aulIfAgc_810[41] = {
	97,103,92,98,93,99,105,94,100,95,
	101,106,102,98,99,99,104,100,105,95,
	101,101,99,96,101,107,112,118,124,130,
	136,141,147,152,159,164,170,176,181,187,
	193
};


static int aulDownLevel[41] = {
	20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10,
	9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
	-1, -2, -3, -4, -5, -6, -7, -8, -9, -10,
	-11, -12, -13, -14, -15, -16, -17, -18, -19, -20
};
#endif
#endif

// End static variable

static void DRV_CH_C_I2CEnable(HUINT8 unUnitId);
static void DRV_CH_C_I2CDisable(HUINT8 unUnitId);
/****************** Function 시작점에 필히 기록 ********************/
/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
// Function Name:
// Function Description:
// Parameter:
// Return Value:

#define __CH_C_PRIVATE_FUNCITONS__
int drv_ch_i2c_DEMODRead(int nUnitId, HUINT8 ucRegAddr, HUINT8 *pucBuffer, int nNumberToRead)
{
	int nResult = DI_CH_OK;
	int nRet = 0;//, i;
	HUINT8	ucDevAddr = TDA10024_M_DEMOD_ADDRESS;
	HUINT8	ucI2CBus = I2C_CHANNEL_CHANNEL;

	//	HUINT8	ucWriteValue = 0xc1;//0x41;
	//	HUINT8	aucDsrStatus[7] = {0,};

	if(nUnitId == MASTER_DEMOD )
	{
		ucDevAddr = TDA10024_M_DEMOD_ADDRESS;
		//ucI2CBus = 0;
		ucI2CBus = I2C_CHANNEL_CHANNEL;

	}
#if defined(CONFIG_I2C_MAP_FOR_2TUNER)
	else if(nUnitId == SLAVE_DEMOD)
	{
		ucDevAddr = TDA10024_S_DEMOD_ADDRESS;
		//ucI2CBus = 1;
		ucI2CBus = I2C_CHANNEL_CHANNEL2;

	}
#endif
	else
	{
		ucDevAddr = TDA10024_M_DEMOD_ADDRESS;
		//ucI2CBus = 0;
		ucI2CBus = I2C_CHANNEL_CHANNEL;

	}

	nRet = DRV_I2c_ReadA8( ucI2CBus, ucDevAddr, ucRegAddr, pucBuffer, nNumberToRead );
	if ( nRet != DI_CH_OK )
	{
		CH_DI_Print(0, (" Error[0x%x]", nRet));
		nResult = DI_CH_ERR;
	}
	else
	{
		//CH_PrintDebug(" ucRegValue[0x%x]\n", ucRegValue);
	}

	return nResult;

}


int drv_ch_i2c_DEMODWrite(int nUnitId,  HUINT8 ucRegAddr, HUINT8 *pucBuffer, int nNumberToWrite)
{
	int nResult = DI_CH_OK;
	int nRet = 0;//, i;
	HUINT8	ucDevAddr = TDA10024_M_DEMOD_ADDRESS;
	HUINT8	ucI2CBus = I2C_CHANNEL_CHANNEL;
	//	HUINT8	ucWriteValue = 0xc1;//0x41;
	//	HUINT8	aucDsrStatus[7] = {0,};

	if(nUnitId == MASTER_DEMOD )
	{
		ucDevAddr = TDA10024_M_DEMOD_ADDRESS;
		//ucI2CBus = 0;
		ucI2CBus = I2C_CHANNEL_CHANNEL;
	}
#if defined(CONFIG_I2C_MAP_FOR_2TUNER)
	else if(nUnitId == SLAVE_DEMOD)
	{
		ucDevAddr = TDA10024_S_DEMOD_ADDRESS;
		//ucI2CBus = 1;
		ucI2CBus = I2C_CHANNEL_CHANNEL2;
	}
#endif
	else
	{
		ucDevAddr = TDA10024_M_DEMOD_ADDRESS;
		//ucI2CBus = 0;
		ucI2CBus = I2C_CHANNEL_CHANNEL;
	}

		//CH_PrintErr(" ucI2CBus[%d], ucDevAddr[0x%x]", ucI2CBus, ucDevAddr);

	nRet = DRV_I2c_WriteA8(ucI2CBus, ucDevAddr, ucRegAddr, pucBuffer, nNumberToWrite);
	if ( nRet != DI_CH_OK )
	{
		CH_DI_Print(0, (" Error[0x%x]", nRet));
		nResult = DI_CH_ERR;
	}
	else
	{
		//CH_PrintDebug(" Success \n");
	}

	return nResult;

}


/* check if two AGC output are maximum. If then,  */
/* it means there is no signal or too weak signal */
unsigned char check_agc_lock(int nUnitId)
{
	unsigned char	val1, val2;

	/* Read the value of AGC loops */
	drv_ch_i2c_DEMODRead(nUnitId, SB_VAGC, &val1, 1);
	drv_ch_i2c_DEMODRead(nUnitId, SB_VAGC2, &val2, 1);
	CH_DI_Print(5, ("(VAGC_RF,VAGC_IF)=(0x%x,0x%x)\r\n", val1,val2));

	if ( (val1==0xff) && (val2==0xff) )
		return FALSE;
	else
		return TRUE;
}

/* Increase the BER counter width to 1.0e+8 */
void AdaptBerWindow( int nUnitId, HUINT32 ulSymbolRate, CH_CAB_Modulation_e etModulation )
{
	unsigned char 	val;
    unsigned char	bitsPerBaud;
	unsigned long	ElapsedTime;
	unsigned long	ElapsedBits;

    /* Define bit per symbol according to QAM mode */
    switch ( etModulation )
    {
    	case DI_CH_CMOD_16QAM : /* 16-QAM */
    		bitsPerBaud = 4;   break;
    	case DI_CH_CMOD_32QAM : /* 32-QAM */
    		bitsPerBaud = 5;   break;
    	case DI_CH_CMOD_64QAM : /* 64-QAM */
    		bitsPerBaud = 6;   break;
    	case DI_CH_CMOD_128QAM : /* 128-QAM */
    		bitsPerBaud = 7;   break;
    	case DI_CH_CMOD_256QAM : /* 256-QAM */
    		bitsPerBaud = 8;   break;
    	default : /* 256-QAM */
    		bitsPerBaud = 6;   break;
    }

	/* BER measurement period is equal to GET_DATA_INTERVAL */
	ElapsedTime = CH_TIMER_PERIEOD * GET_DATA_INTERVAL  ; // in msec  updated  by jino 2003/1/21

	/* calculate the number of the elapsed bits for GET_DATA_INTERVAL */
	ElapsedBits = bitsPerBaud * ElapsedTime * ulSymbolRate;

	/* select the BER counter width equal or more than the elapsed bits */
	if     (ElapsedBits > 10000000)
		val = R_RSCONF4;
	else if(ElapsedBits > 1000000)
		val = R_RSCONF3;
	else if(ElapsedBits > 125000)
		val = R_RSCONF2;
	else
		val = R_RSCONF1;

	drv_ch_i2c_DEMODWrite(nUnitId, SB_RSCONF, &val, 1);
}


void ClearBerStatus( int nUnitId, HUINT32 ulSymbolRate, CH_CAB_Modulation_e etModulation )
{
	unsigned char	data ;

	/* adapt BER counter window according to the number
	   of the elapsed bit for the GET_DATA_INTERVAL period */
	AdaptBerWindow(nUnitId, ulSymbolRate, etModulation);

	/* after lock, clear the value of BER conuter */
	data  = R_RSCONF3 & 0xdf;

	/* CLB_UNC bit = 0 */
	drv_ch_i2c_DEMODWrite(nUnitId, SB_RSCONF, &data, 1);

	/* CLB_UNC bit = 1 */
	data  = R_RSCONF3;
	drv_ch_i2c_DEMODWrite(nUnitId, SB_RSCONF, &data, 1);

}

static void ClearDemod( int nUnitId, CH_CAB_Modulation_e etModulation )
{
	unsigned char 	val;

	CH_DI_Print(5, ( "etModulation(%d)\n", etModulation ));

	/* INVIQ bit=1 (due to spectral inversion at analog IF circuitry)   */
	/* Difference between TDA10021 and VES1820x: existence of CLB2 bit
	   s_aucConfReg value of TDA10021 is greater than that of VES1820x by 2 */

#if defined(PLATFORM_TN182I2) || defined(PLATFORM_TM201)	// From 1T
	switch ( etModulation )
	{
		case DI_CH_CMOD_16QAM : //16-QAM
			s_aucConfReg[nUnitId]    = 0x03; break;
		case DI_CH_CMOD_32QAM : //32-QAM
			s_aucConfReg[nUnitId]    = 0x07; break;
		case DI_CH_CMOD_64QAM : //64-QAM
			s_aucConfReg[nUnitId]    = 0xb; break;  // IN JC-4000 0x2b used comment 050124 parkyr
		case DI_CH_CMOD_128QAM : //128-QAM
			s_aucConfReg[nUnitId]    = 0x0f; break;
		case DI_CH_CMOD_256QAM : //256-QAM
			s_aucConfReg[nUnitId]    = 0x13; break;
		default:
			s_aucConfReg[nUnitId]    = 0x0b; break;
	}
#else
	switch ( etModulation )
	{
		case DI_CH_CMOD_16QAM : //16-QAM
			s_aucConfReg[nUnitId]    = 0x23; break;
		case DI_CH_CMOD_32QAM : //32-QAM
			s_aucConfReg[nUnitId]    = 0x27; break;
		case DI_CH_CMOD_64QAM : //64-QAM
			s_aucConfReg[nUnitId]    = 0x2b; break;  // IN JC-4000 0x2b used comment 050124 parkyr
		case DI_CH_CMOD_128QAM : //128-QAM
			s_aucConfReg[nUnitId]    = 0x2f; break;
		case DI_CH_CMOD_256QAM : //256-QAM
			s_aucConfReg[nUnitId]    = 0x33; break;
		default:
			s_aucConfReg[nUnitId]    = 0x2b; break;
	}
#endif
	/* soft-reset the QAM logic */
	/* set the CLR bit to zero. CLB signal will automatically return to 1 after 5 XIN period */
	val = s_aucConfReg[nUnitId] & 0xfe;

	drv_ch_i2c_DEMODWrite(nUnitId, SB_CONF, &val, 1);

}


static void InitDemodPLL(int nUnitId)
{
	unsigned char	QAMreg[3];
	unsigned char	reg_val ;

	CH_DI_Print(5, ("UnitId(0x%x)\n", nUnitId));

	/* mdiv, ndiv, pll: PLL related setting : XTAL = 16MHZ, SYSCLK = 58.666MHZ */

	if(g_ulSysClk[nUnitId] == 58666)
	{
		QAMreg[0x00] = 0x0A; 	/* MDIV = 10 ( 0x0A ) 	--> M = 11*/
		QAMreg[0x01] = 0x80;	/* PDIV = 2, NDIV = 0 	--> N = 1, P=3 */
		QAMreg[0x02] = 0x0C; 	/* PLL3 REGISTER */
	}
	else
	{
		QAMreg[0x00] = 0x0B; 	/* MDIV = 11 ( 0x0A ) 	--> M = 12*/
		QAMreg[0x01] = 0x80;	/* PDIV = 2, NDIV = 0 	--> N = 1, P=3 */
		QAMreg[0x02] = 0x0C; 	/* PLL3 REGISTER */
	}
	/* To get proper output PLL frequency : refer TDA10023 NOTE */
	/* 1) First PLL has be bypassed with BYPPLL = 1 and PDPLL = 0  default mode after hard reset
			so not require setting */
	/*
	reg_val = QAMreg[0x02] |= 0x02;

	drv_ch_i2c_DEMODWrite(TDA_10023_SB_PLL3, &reg_val, 1);

	*/

	/* 2) The PLL must be set in power down mode with BYPPLL =1; PDPLL = 1 */

	reg_val = QAMreg[0x02] |0x03;
	drv_ch_i2c_DEMODWrite(nUnitId, TDA_10023_SB_PLL3, &reg_val, 1);


	/* 3) M,N,P loading */


	drv_ch_i2c_DEMODWrite(nUnitId, TDA_10023_SB_PLL1, &QAMreg[0x00], 2);


	/* 4) Finaly launched by setting BYPPLL =0 and PDPLL = 0 */

	reg_val = QAMreg[0x02];
	drv_ch_i2c_DEMODWrite(nUnitId, TDA_10023_SB_PLL3, &reg_val, 1);


}


void ManageBandTypeGain( int nUnitId, HUINT32 ulSymbolRate )
{
	unsigned char	uByte, uByteSat[7];
	signed char		bGain, bGainMax, bGainMin;
	unsigned long	uSATADC, uHALFADC, uSATDEC1, uSATDEC2, uSATDEC3, uSATAAF, uSATNYQ;
	signed char		bTEST_SAT, bTEST_HALFADC;
	unsigned short	uNbSamplesSSAT=0, uNbSamplesAGC=0;
	unsigned long	uWaitingTime1, uWaitingTime2;

	// Nb of samples used to refresh AGC values
    drv_ch_i2c_DEMODRead(nUnitId, SB_INVQ_AGC, &uByte, 1);
    switch ((uByte & 0x07))
    {
    	case 0:
			uNbSamplesAGC = 512;
    		break;
    	case 1:
			uNbSamplesAGC = 1024;
    		break;
    	case 2:
			uNbSamplesAGC = 2048;
    		break;
    	case 3:
			uNbSamplesAGC = 4096;
    		break;
    	case 4:
			uNbSamplesAGC = 8192;
    		break;
	}
	uWaitingTime1 = (50*uNbSamplesAGC)/( ulSymbolRate) + 1;

	// Nb of samples in STAT depends on reg SSAT
    drv_ch_i2c_DEMODRead(nUnitId, SB_GAIN, &uByte, 1);
    switch ((uByte & 0x03) )
    {
    	case 0:
			uNbSamplesSSAT = 16384;
    		break;
    	case 1:
			uNbSamplesSSAT = 8192;
    		break;
    	case 2:
			uNbSamplesSSAT = 4096;
    		break;
    	case 3:
			uNbSamplesSSAT = 2048;
    		break;
	}
	uWaitingTime2 = (2*uNbSamplesSSAT)/(  ulSymbolRate) + 1;

	bGainMin = 3;
	bGainMax = 3;

	if ( ulSymbolRate < 3800)
	{
		bGainMin = 2;
		bGainMax = 2;
	}

	bGain = bGainMin-1;

	do
	{
		// Increase GNYQ gain
		bGain++;
		drv_ch_i2c_DEMODRead(nUnitId, SB_GAIN, &uByte, 1);
		uByte &= 0x8f;
		uByte |= (bGain<<4);
		drv_ch_i2c_DEMODWrite(nUnitId, SB_GAIN, &uByte, 1);

		// Time to refresh the AGC output based on AGC_INTEG_NB
		// Value in function Tda10023InitTick must be in ms
		VK_TASK_Sleep(uWaitingTime1);
		// Time to refresh sat register (s) = Nb of samples selected in SSAT register / Sr
		// Value in function Tda10023InitTick must be in ms
		VK_TASK_Sleep(uWaitingTime2);

		// Read saturation counters
		drv_ch_i2c_DEMODRead(nUnitId, SB_SATNYQ, uByteSat, 7);
		uSATADC = uByteSat[1];
		uHALFADC = uByteSat[2];
		uSATDEC1 = uByteSat[3];
		uSATDEC2 = uByteSat[4];
		uSATDEC3 = uByteSat[5];
		uSATAAF = uByteSat[6];
		uSATNYQ = uByteSat[0];

		// Time to refresh sat register = Nb of samples selected in SSAT register / Sr
		VK_TASK_Sleep(uWaitingTime2);

		// Read twice HALFADC to take higher value
		drv_ch_i2c_DEMODRead(nUnitId, SB_HALFADC, &uByte, 1);
		uHALFADC = (uHALFADC>uByte) ? uHALFADC : uByte;

		bTEST_SAT = (uSATADC > 1) || (uSATDEC1 > 1) || (uSATDEC2 > 1) || (uSATDEC3 > 1) || (uSATAAF > 1) || (uSATNYQ > 1);
		bTEST_HALFADC = (uHALFADC == 0xFF);

	}
	while ((bGain < bGainMax) && (bTEST_SAT || bTEST_HALFADC));

}


/* This sub-function is made for the adjacent channel performace */
/* Specially for the ESSENT cable network response */
void manage_adjacent_channel_performance(int nUnitId, HUINT32 ulSymbolRate )
{
	unsigned char	val;

	// set POSAGC to 0
	drv_ch_i2c_DEMODRead(nUnitId, SB_AGCCONF2, &val,1);
	CH_DI_Print(5, ("read val(0x%x)\n", val));

	//val = 0x20;
#if defined(PLATFORM_TN182I2) || defined(PLATFORM_TM201) //090803
	val &= 0xcf;
#else
	val &= 0xdf;//by ilee
	//val &= 0xc7;//0xc8;//0xc7
#endif

	CH_DI_Print(6, ("write val(0x%x)\n", val));
	drv_ch_i2c_DEMODWrite(nUnitId, SB_AGCCONF2, &val,1);

	drv_ch_i2c_DEMODRead(nUnitId, SB_AGCCONF2, &val,1);
	CH_DI_Print(6, ("read val(0x%x)\n", val));

	// set AUTOGNYQ to 0
	drv_ch_i2c_DEMODRead(nUnitId, SB_GAIN, &val,1);
	val &= 0x7f;
	drv_ch_i2c_DEMODWrite(nUnitId, SB_GAIN, &val,1);

	if(ulSymbolRate < 3800)
	{
	    // set GAIN1 to  2
		drv_ch_i2c_DEMODRead(nUnitId, SB_GAIN, &val, 1);
		val = (val&0xfb)|0x04;   // set bit2 to 1
		drv_ch_i2c_DEMODWrite(nUnitId, SB_GAIN, &val, 1);
	}
	else
	{
		// set GAIN1 to	1
		drv_ch_i2c_DEMODRead(nUnitId, SB_GAIN, &val, 1);
		val = val&0xfb;   // set bit2 to 0
		drv_ch_i2c_DEMODWrite(nUnitId, SB_GAIN, &val, 1);
	}

	// set GAIN2 to 1 and GAIN3 to 1
	drv_ch_i2c_DEMODRead(nUnitId, SB_CLKCONF, &val,1);
	val &= 0xcf;
	drv_ch_i2c_DEMODWrite(nUnitId, SB_CLKCONF, &val,1);

	// set GAAF to 1
	drv_ch_i2c_DEMODRead(nUnitId, SB_GAIN2, &val,1);
	val &= 0x8f;
	drv_ch_i2c_DEMODWrite(nUnitId, SB_GAIN2, &val,1);

	// Set GNYQ to 0 and increase it while filters saturation indicator equals 0
	drv_ch_i2c_DEMODRead(nUnitId, SB_GAIN, &val,1);
	val &= 0x8f;
	if ( ulSymbolRate < 3800)
		val |= 0x20;
	else
		val |= 0x30;
	drv_ch_i2c_DEMODWrite(nUnitId, SB_GAIN, &val,1);

}


//#define	PARALLEL_MODE_B_OUTPUT // <10023> , jino , Parallel Mode B 가 아닐 것 같아서 disable 시킴,기존 Define 은 Mode A 임. 2006/06/20
static void set_TS_clock_rate(int nUnitId, HUINT32 ulSymbolRate, CH_CAB_Modulation_e etModulation )
{
	unsigned char val;
	unsigned int ulSR, etMod;

#ifdef PARALLEL_MODE_B_OUTPUT

	unsigned long	oclk;
	unsigned long	p_symbol;
	unsigned char	divp;

	/* OCLK is a division of the system clock by programmable factor DIVP */
	/* OCLK must be greater than N * SymbolRate / 8, N is spectral efficiency */

	p_symbol = ulSymbolRate / 8 ;

    switch ( etModulation )
    {
        case DI_CH_CMOD_16QAM : //16-QAM
			oclk = p_symbol * 4 ;  // N = 4
       		break;
        case DI_CH_CMOD_32QAM : //32-QAM
			oclk = p_symbol * 5 ;  // N = 5
        	break;
        case DI_CH_CMOD_64QAM : //64-QAM
			oclk = p_symbol * 6 ;  // N = 6
        	break;
        case DI_CH_CMOD_128QAM : //128-QAM
			oclk = p_symbol * 7 ;  // N = 7
        	break;
        case DI_CH_CMOD_256QAM : //256-QAM
			oclk = p_symbol * 8 ;  // N = 8
        	break;
		default: //64-QAM
			oclk = p_symbol * 8 ;  // N = 8
			break;
	}


	if( oclk < g_ulSysClk[nUnitId] / 16 )		divp = 0x0f;
	else if(oclk < g_ulSysClk[nUnitId] / 15 )	divp = 0x0e;
	else if(oclk < g_ulSysClk[nUnitId] / 14 )	divp = 0x0d;
	else if(oclk < g_ulSysClk[nUnitId] / 13 )	divp = 0x0c;
	else if(oclk < g_ulSysClk[nUnitId] / 12 )	divp = 0x0b;
	else if(oclk < g_ulSysClk[nUnitId] / 11 )	divp = 0x0a;
	else if(oclk < g_ulSysClk[nUnitId] / 10 )	divp = 0x09;
	else if(oclk < g_ulSysClk[nUnitId] / 9 )		divp = 0x08;
	else if(oclk < g_ulSysClk[nUnitId] / 8 )		divp = 0x07;
	else if(oclk < g_ulSysClk[nUnitId] / 7 )		divp = 0x06;
	else if(oclk < g_ulSysClk[nUnitId] / 6 )		divp = 0x05;
	else if(oclk < g_ulSysClk[nUnitId] / 5 )		divp = 0x04;
	else if(oclk < g_ulSysClk[nUnitId] / 4 )		divp = 0x03;
	else if(oclk < g_ulSysClk[nUnitId] / 3 )		divp = 0x02;
	else if(oclk < g_ulSysClk[nUnitId] / 2 )		divp = 0x01;
	else 							divp = 0x00;

        //thyou demod clock polarity falling -> rising   2006.05.09
	val = 0xa1;
	drv_ch_i2c_DEMODWrite(nUnitId , TDA_10023_SB_INTP1, &val, 1);


	val = divp << 4;
	CH_DI_Print(6, (" PARALLEL B SETTING divp = %d\n",divp));

	val = val & 0xf0;	// masking divp register
	val = val | 0x05;	// parallel mode B

	drv_ch_i2c_DEMODWrite(nUnitId , TDA_10023_SB_INTP2, &val, 1);

#elif defined(SERIAL_MODE_OUTPUT)
ulSR = ulSymbolRate;
etMod= etModulation;

#ifdef OUTPUT_RISING_EDGE
	val = 0xa1; /* Serial mode, rising edge */
#else
	val = 0xa0; /* Serial mode, falling edge */
#endif

	drv_ch_i2c_DEMODWrite(nUnitId , TDA_10023_SB_INTP1, &val, 1);


#ifdef SERIAL_MODE_OUTPUT
	val = 0x07;
#else
	val = 0x04;
#endif

	drv_ch_i2c_DEMODWrite(nUnitId, TDA_10023_SB_INTP2, &val, 1);

#elif defined(PARALLEL_MODE_A_OUTPUT)	/* Parallel mode A: TS parallel with jittered clock */
	val = 0xa5;//val = 0xe5; //0xa0;//by ilee
	drv_ch_i2c_DEMODWrite(nUnitId , TDA_10023_SB_INTP1, &val, 1);

	val = 0x04;
	drv_ch_i2c_DEMODWrite(nUnitId, TDA_10023_SB_INTP2, &val, 1);
#endif
}

static void InitQAMdemod( int nUnitId, HUINT32 ulSymbolRate, CH_CAB_Modulation_e etModulation )
{
	unsigned char	QAMreg[0x10];
	unsigned char	val, uc_val;
	//unsigned char	nDECfilt;
	//unsigned long	swdyn, swstep, swlength;
	//unsigned long	_SbCarWait=0; 		// Carrier sweep convergence time = 2*SWDYN/SWSTEP*SWLENGTH

	CH_DI_Print(5, ("InitQAMdemod() \r\n"));

	InitDemodPLL(nUnitId); //<10023>

	/* EQstep : concerning to the short and long ghost performance */
	val = 0x29;  //default value EQSTRAC = 5, EQSTACQ = 1
	drv_ch_i2c_DEMODWrite(nUnitId, SB_EQSTEP, &val, 1);

	drv_ch_i2c_DEMODRead(nUnitId, SB_ADC, &val, 1);


	val = (val&~0xC0)|0x80;
	drv_ch_i2c_DEMODWrite(nUnitId, SB_ADC, &val, 1);

	val = 0xA0; //AUTOGNYQ = 1, GNYQ = 2로 변경, 2007/08/24
	drv_ch_i2c_DEMODWrite(nUnitId, SB_GAIN, &val, 1);

	val = 0x60;	//AGC Error Threshold 변경
	drv_ch_i2c_DEMODWrite(nUnitId, SB_ERAGC_THD, &val, 1);


	//QAMreg[0x00] = 0x1c; //0001 1100 => worse than 0x1a @ short ghost (delay <= 35 ns)
	//QAMreg[0x00] = 0x1b; //0001 1011 => worse than 0x1a @ short ghost (delay <= 35 ns)
	//QAMreg[0x00] = 0x19; //0001 1001 => better than 0x1a @ short ghost (delay <= 145 ns)
	// Conclution: Acquisition step size = xxxx x010

	/* intp: set TS stream clock */
	set_TS_clock_rate(nUnitId, ulSymbolRate, etModulation);

	val = R_RSCONF3;
	drv_ch_i2c_DEMODWrite(nUnitId, SB_RSCONF, &val, 1);

#ifdef ENABLE_TDA_ISR // #ifdef is added by jino 2003/2/12
	/* itsel: active LOW interrupt control */
	uc_val = 0x40; // interrupt disabled and output level is "1" always
	drv_ch_i2c_DEMODWrite(nUnitId, SB_ITSEL, &uc_val, 1);
#endif

	/* pola: rising edge in center of data duration       */
	/* RS check bytes are not provided at TS output */
	/* and OCLK is also fixed to a DC level         */
//	uc_val = 0xa1; //RS check bytes are provided
//	uc_val = 0xe1; // RS check bytes are not provided

#ifdef SERIAL_MODE_OUTPUT
#ifdef OUTPUT_RISING_EDGE
	uc_val = 0xa1; /* Serial mode, rising edge */
#else
	uc_val = 0xa0; /* Serial mode, falling edge */
#endif
#else
	uc_val = 0xe5;//0xa0; // <10023> 0xe5; // RS check bytes are not provided
#endif

	drv_ch_i2c_DEMODWrite(nUnitId, TDA_10023_SB_INTP1, &uc_val, 1); // <10023>


#if defined(PLATFORM_TN182I2) || defined(PLATFORM_TM201)	// From 1T
	uc_val=0x00;
	drv_ch_i2c_DEMODWrite(nUnitId, SB_PWMREF, &uc_val, 1);// <10023>
#else
	/* pwmref: meaningful if IF AGC is inactive */
	//uc_val = 0x00;//0x80->0x00  by kwonkh 20050803
	uc_val=0x80;//by ilee
	drv_ch_i2c_DEMODWrite(nUnitId, SB_PWMREF, &uc_val, 1);// <10023>
#endif

#ifdef SERIAL_MODE_OUTPUT
	uc_val = 0xA1;
#else
	uc_val = 0xA0;
#endif
	drv_ch_i2c_DEMODWrite(nUnitId, TDA_10023_SB_INTS1, &uc_val, 1);// <10023>


#ifdef SERIAL_MODE_OUTPUT
	uc_val = 0x20;
#else
	uc_val = 0x20;
#endif

	drv_ch_i2c_DEMODWrite(nUnitId, TDA_10023_SB_INTPS, &uc_val, 1);// <10023>


	uc_val = 0x16;
	drv_ch_i2c_DEMODWrite(nUnitId, SB_SWEEP, &uc_val, 1);// <10023>

#if defined(PLATFORM_TN182I2) || defined(PLATFORM_TM201)	// From 1T
	uc_val = 0x20; // both AGC output are configured as normal output type
	drv_ch_i2c_DEMODWrite(nUnitId, SB_AGCCONF2, &uc_val, 1);
#else
	//by ilee
	uc_val = 0x30; // both AGC output are configured as normal output type
	//uc_val = 0x28;
	drv_ch_i2c_DEMODWrite(nUnitId, SB_AGCCONF2, &uc_val, 1);
#endif

	/* RF AGC control */
	QAMreg[0x00] = 0xFF; /* RF AGC max threshold value */
#if defined(PLATFORM_TN182I2) || defined(PLATFORM_TM201)
	QAMreg[0x01] = 0x00; /* RF AGC min threshold value */
#else
	QAMreg[0x01] = 0xFF; /* RF AGC min threshold value */
#endif
	drv_ch_i2c_DEMODWrite(nUnitId, SB_TUNMAX, &QAMreg[0x00], 2);       // <10023>

	/* IF IFFREQ * 1.6 * 1000 == SYSCLK */
	/* IFFREQ = 36.15MHZ SYSCLK = 58MHZ */
#if defined(PLATFORM_TN182I2)  	// From 1T
	QAMreg[0x00] = 0xd3;
	QAMreg[0x01] = 0xf5;
#elif defined(PLATFORM_TM201)
	QAMreg[0x00] = 0x30;
	QAMreg[0x01] = 0xfa;
#else
	if(g_ulSysClk[nUnitId] == 58666)
	{
		QAMreg[0x00] = 0xd6; //d6;  //0x00; // <10023>
		QAMreg[0x01] = 0xfe; //fe; //0x80; // <10023>
	}
	else
	{
		QAMreg[0x00] = 0x44; //d6;  //0x00; // <10023>
		QAMreg[0x01] = 0xf8; //fe; //0x80; // <10023>
	}
#endif
	drv_ch_i2c_DEMODWrite(nUnitId,SB_DELTAF1, &QAMreg[0x00], 2);

	/* IF AGC control: IF_MIN determines TOP of the tuner */
	QAMreg[0x00] = TDA10023_AGC_MAX; // IF AGC max threshold value
	QAMreg[0x01] = TDA10023_AGC_MIN; 	// IF AGC min threshold value for MK3 Tuner kwonkh 20050804
#if 0//defined(PLATFORM_TN182I2)	// From 1T
	QAMreg[0x01] = 0x76;
#endif
	drv_ch_i2c_DEMODWrite(nUnitId, SB_IFMAX, &QAMreg[0x00], 2);// <10023>

#if defined (PLATFORM_TM2063)
	uc_val = 0xa7;
#else
	uc_val = 0xa3;
#endif

	drv_ch_i2c_DEMODWrite(nUnitId, TDA_10023_SB_CONTROL, &uc_val, 1);	// <10023>

	CH_DI_Print(5, ("InitQAMdemod() E \r\n"));

	// 2003/09/17 , jino : Init Demodulator's PLL again , requested by khkwon
	// because of unstability of Philips Demodulaotor's PLL..
	//InitDemodPLL(nUnitId); //<10023>
}

static unsigned char calc_no_decimation( int nUnitId, HUINT32 ulSymbolRate )
{
	HUINT32  SymbolRate;
	unsigned char ret_value;


	SymbolRate = ulSymbolRate / 10 ;
		CH_DI_Print(6, ("CR] SR of %d kHz \n",SymbolRate*10));

	if(SymbolRate < g_ulSysClk[nUnitId] / 1280){
		CH_DI_Print(4, ("Error! SR=%d kKz is too LOW\n", ulSymbolRate));
		ret_value = 3;
	}
	else if( SymbolRate < g_ulSysClk[nUnitId]/640 )
		ret_value = 3;
	else if( SymbolRate < g_ulSysClk[nUnitId]/320 )
		ret_value = 2;
	else if( SymbolRate < g_ulSysClk[nUnitId]/160 )
		ret_value = 1;
	else if( SymbolRate < g_ulSysClk[nUnitId]/80 )
		ret_value = 0;
	else
	{
		CH_DI_Print(4, ("Error! SR=%d kKz is too HIGH\n", ulSymbolRate));
		ret_value = 0;
	}

	return ret_value;
}

/***************************************************************
  Control the decimators in the baud rate recovery loop:
 - Set the number of decimation
 - Set the initial loop BW as large. Later the BW will be reduced.
****************************************************************/
unsigned char set_DecimateFilt( int nUnitId, HUINT32 ulSymbolRate )
{
	unsigned char	no_decimate, clkconf_val;

	/* Get the number of decimation */
	no_decimate = calc_no_decimation(nUnitId, ulSymbolRate);
	clkconf_val = no_decimate << 6;

	/* Updates the the baud rate recovery loop register value */
	clkconf_val = s_aucClkConfReg[nUnitId] | clkconf_val;
	s_aucClkConfReg[nUnitId] = clkconf_val;
	drv_ch_i2c_DEMODWrite(nUnitId, SB_CLKCONF, &clkconf_val, 1);

	/* the number of DEC will be used when the baud rate and its inverse are set */
	return no_decimate;
}

/* math function which calulate power of exponential */
unsigned short pow_positive(unsigned short mantissa, unsigned char exponent)
{
	unsigned char	i;
	unsigned short	result = 1;

	if(mantissa == 0)
	{
		CH_DI_Print(4, ("pow_positive(mantissa=0) error\r\n"));
	}
	else
	{
		for(i=0;i<exponent;i++)
		{
			result = result*mantissa;
		}
	}

	return (result);
}

/* set the baud rate and its inverse according *
 * to symbol rate and reference frequency      */
void set_baud_rate( int nUnitId, unsigned char ndec, HUINT32 ulSymbolRate )
{
	unsigned long 	BaudRate, InvBaudRate;
	unsigned char	val[4];
	unsigned char	no_decimate;

	no_decimate = ndec;

	/* set the baud rate */
	{
		/* BAUD = 2^24 * SR * 2^no_decimate / SYSCLK */
		/* and SYSCLK = 58666666, so 2^24 / SYSCLK = 0.285975 */
		/* symbolRate unit is kHz ..... BAUD = 0.285975 * symbolRate * 1000 * 2^no_decimate */
		/* BAUD = 285975 * symbolRate / 1000 * 2^no_decimate */
		if( g_ulSysClk[nUnitId] == 58666 )
		{
			BaudRate = 285975uL * ulSymbolRate / 1000 * pow_positive(2,no_decimate) ;
		}
		else
		{
			BaudRate = 262144uL * ulSymbolRate / 1000 * pow_positive(2,no_decimate) ;
		}

		val[0] = (unsigned char)(BaudRate & 0xff);
		val[1] = (unsigned char)((BaudRate>>8) & 0xff);
		val[2] = (unsigned char)((BaudRate>>16) & 0xff);
		/* set the baud rate inverse */

		InvBaudRate = 16uL/pow_positive(2, no_decimate)*g_ulSysClk[nUnitId] / ulSymbolRate;

		val[3] = ( InvBaudRate > 255)?  (unsigned char)255: (unsigned char)InvBaudRate;

		drv_ch_i2c_DEMODWrite(nUnitId, SB_BDR_LSB, &val[0], 4);
	}
}

/* select which anti-aliasing filter will be used */
static void select_AntialiasFilt( int nUnitId, HUINT32 ulSymbolRate )
{
	unsigned char	falias, gain_val,gain2_val;
	HUINT32 	 SymbolRate;

	SymbolRate = ulSymbolRate / 10;


	if(SymbolRate > g_ulSysClk[nUnitId] / 80){
		CH_DI_Print(4, ("Error! SR=%d kKz is too HIGH\n", ulSymbolRate));
		falias = 0;
	}
	else if( SymbolRate > g_ulSysClk[nUnitId]/123 )
		falias = 0;
	else if( SymbolRate > g_ulSysClk[nUnitId]/160)
		falias = 1;
	else if( SymbolRate > g_ulSysClk[nUnitId]/246)
		falias = 0;
	else if( SymbolRate > g_ulSysClk[nUnitId]/320)
		falias = 1;
	else if( SymbolRate > g_ulSysClk[nUnitId]/492)
		falias = 0;
	else if( SymbolRate > g_ulSysClk[nUnitId]/640)
		falias = 1;
	else if( SymbolRate > g_ulSysClk[nUnitId]/984)
		falias = 0;
	else if( SymbolRate > g_ulSysClk[nUnitId]/1280)
		falias = 1;
	else
	{
		CH_DI_Print(4, ("Error! SR=%d kKz is too LOW\n", ulSymbolRate));
		falias = 1;
	}

	gain2_val = ( (falias << 7) | ( NYQ_ROLLOFF & 0x03 ) | ( (NYQ_GAAF << 4) & 0x70));


	CH_DI_Print(6, ("AF GAIN2 = %2x\n",gain2_val));

	drv_ch_i2c_DEMODWrite(nUnitId, TDA_10023_SB_GAIN2, &gain2_val, 1);



	gain_val = 0xa4;
	s_aucGainReg[nUnitId] = gain_val;
	s_aucNyquistGain[nUnitId] = (gain_val>>4) & 0x07;      /* Nyquist filter gain factor: 0 - 7 */

	drv_ch_i2c_DEMODWrite(nUnitId , SB_GAIN, &gain_val, 1);
}


static void SetQAMdemod( int nUnitId, CH_CAB_TuneParam_t *pstCabTuneParam )
{
	unsigned char	QAMreg[0x10];
	unsigned char	uc_val;
	unsigned char	nDECfilt;
	//unsigned long	swdyn, swstep, swlength;
	//unsigned long	_SbCarWait=0; 		// Carrier sweep convergence time = 2*SWDYN/SWSTEP*SWLENGTH

	CH_DI_Print(5, ("SetQAMdemod(%d) \n", pstCabTuneParam->ulFrequency));

	uc_val = 0x7f;	//DVB mode select

	drv_ch_i2c_DEMODWrite(nUnitId, TDA_10023_SB_RESET, &uc_val, 1);

#if defined(PLATFORM_TN182I2)
	//DP10024_SetFSampling(nUnitId, FALSE);
#endif

	/* intp: set TS stream clock */
	set_TS_clock_rate(nUnitId, pstCabTuneParam->ulSymbolRate, pstCabTuneParam->etModulation);

	switch ( pstCabTuneParam->etModulation )
	{
		case DI_CH_CMOD_16QAM : //16-QAM
			QAMreg[0x01] = TDA10023_AGC_REF_NORMAL; //AGCREF is same at both SW versions
			QAMreg[0x05] = 0x87; //<10023> 0x6e; //LOCKTHR = 0x87 @ v1.01
			QAMreg[0x08] = 0xA2; //<10023> 0x6e; //MSETH = 0xa2 @ v1.01
			QAMreg[0x09] = 0x91; //AREF is same at both SW versions
			CH_DI_Print(6, (">>>>>>> 16-QAM initial setup starts\n"));
			break;

		case DI_CH_CMOD_32QAM : //32-QAM
			QAMreg[0x01] = TDA10023_AGC_REF_NORMAL; //0x8c
			QAMreg[0x05] = 0x64; //<10023>0x4b; //0x64 @ v1.01
			QAMreg[0x08] = 0x74; //<10023> 0x4b; //0x74 @ v1.01
			QAMreg[0x09] = 0x96;
			CH_DI_Print(6, (">>>>>>> 32-QAM initial setup starts\n"));
			break;

		case DI_CH_CMOD_64QAM : //64-QAM
			QAMreg[0x01] = TDA10023_AGC_REF_NORMAL; //<10023> 0x6a; //from datasheet, better than 0x50 at very low level  //080216
			QAMreg[0x05] = 0x46; //0x46 @ v1.01
			QAMreg[0x08] = 0x43; //<10023> 0x45; //0x43 @ v1.01
			QAMreg[0x09] = 0x6A; //<10023> 0x6a;
			CH_DI_Print(6, (">>>>>>> 64-QAM initial setup starts\n"));
			break;

		case DI_CH_CMOD_128QAM : //128-QAM
			QAMreg[0x01] = TDA10023_AGC_REF_NORMAL; //<10023> 0x78; //although perf. is best at -65 dBm or lower, it is a little degraded at mnorma level (higher than -65 dBm)
			QAMreg[0x05] = 0x36;//<10023> 0x30; // @ v1.01
			QAMreg[0x08] = 0x34; //<10023> 0x30; // @ v1.01
			QAMreg[0x09] = 0x7e;
			CH_DI_Print(6, (">>>>>>> 128-QAM initial setup starts\n"));
			break;

		case DI_CH_CMOD_256QAM : //256-QAM
			QAMreg[0x01] = TDA10023_AGC_REF_NORMAL; //<10023> 0x78; //although perf. is best at -65 dBm or lower, it is a little degraded at mnorma level (higher than -65 dBm)
			QAMreg[0x05] = 0x26; //<10023> 0x23; //0x26 @ v1.01
			QAMreg[0x08] = 0x23; //0x23 @ v1.01
			QAMreg[0x09] = 0x6C; //<10023> 0x6b;
			CH_DI_Print(6, (">>>>>>> 256-QAM initial setup starts\n"));
			break;

		default: //64-QAM
			QAMreg[0x01] = TDA10023_AGC_REF_NORMAL;
			QAMreg[0x05] = 0x46; //<10023> 0x37;
			QAMreg[0x08] = 0x43; //<10023> 0x37;
			QAMreg[0x09] = 0x6a;
			CH_DI_Print(6, (">>>>>>> Default 64-QAM initial setup starts\r\n"));
			break;

	}

	/* agcconf: use the small AGC time constant at the initial phase and */
	/* then enlarge after Euqualizer turn-on  */
	QAMreg[0x02] =  R_AGCONF_FAST; //Acquisition Mode :: RF ATC : 3, IF ATC : 2

	/* clkconf: CLK_C coef is only programmed. Other params will be set later */
	s_aucClkConfReg[nUnitId]  = 0x1a; // <10023> R_CLKCONF_WIDE;
	QAMreg[0x03] =s_aucClkConfReg[nUnitId]; //<10023>  R_CLKCONF_WIDE; //0x0a

	/* carconf: Set the carrier acquisition range wide regardless of the symbol rate  */
	/* If the SR is greater than 3 MHz, the range will be reduced after locking       */
	s_aucCarConfReg[nUnitId] = R_CARCONF_MID_SR_ACQ;
	QAMreg[0x04] = s_aucCarConfReg[nUnitId];

	/* eqconf: TDA10021 EQ is enabled contrast to VES1820x */
	QAMreg[0x06] = R_EQCONF_ON; //0x77: Center Tap = 10th tap, ENADAPT=ENEQUAL=DFE=1

	/* eqstep */
	QAMreg[0x07] = R_EQSTEP;

	/* registers programming from reg[1] to reg[9] */
	drv_ch_i2c_DEMODWrite(nUnitId, SB_AGCREF, &QAMreg[1], 9);

	/* eqconf2: At first CTADAPT bit must to set to one
	   in order to achive the fine AGC operation       */
	uc_val = 0x70;
	drv_ch_i2c_DEMODWrite(nUnitId, SB_EQCONF2, &uc_val, 1);

	/* clkconf's NDEC[1..0] bits: set the number of the decimation */
	nDECfilt = set_DecimateFilt( nUnitId, pstCabTuneParam->ulSymbolRate );

	/* bdr_msb ~ bdr_inv: set the symbol rate and its inverse */
	set_baud_rate( nUnitId, nDECfilt, pstCabTuneParam->ulSymbolRate ); 		/* the symbol rate and its inverse */

	/* gain: set the gain of Nyquist filter and the anti-alias filter */
	select_AntialiasFilt( nUnitId, pstCabTuneParam->ulSymbolRate ); 		/* G_NYQ=2.0, G_AAF=1.0 */

	// Adjust dynamic sweep range during carrier recovery (Only 128QAM).
	if((pstCabTuneParam->ulSymbolRate < 3800) ||  (pstCabTuneParam->etModulation == DI_CH_CMOD_128QAM))
	{
		uc_val = 0x06;
		drv_ch_i2c_DEMODWrite(nUnitId, SB_SWEEP, &uc_val, 1);
	}
	else
	{
		uc_val = 0x16;
		drv_ch_i2c_DEMODWrite(nUnitId, SB_SWEEP, &uc_val, 1);
	}

#if !(defined(PLATFORM_TN182I2) || defined(PLATFORM_TM201)) //shkim2 090803
	if((pstCabTuneParam->ulFrequency < 121000000) && (pstCabTuneParam->etModulation == DI_CH_CMOD_64QAM))
	{
		uc_val = 0x5a;
		drv_ch_i2c_DEMODWrite(nUnitId, SB_AGCREF, &uc_val, 1);
	}
#endif
	manage_adjacent_channel_performance(nUnitId, pstCabTuneParam->ulSymbolRate );

}


/* Reduce the carrier recovery loop's acquisition range according to SR */
/* Affects performance of the receiver according to the selected value  */
void AdjustCarrierRecovBW( int nUnitId, HUINT32 ulSymbolRate )
{
	unsigned char val;

#if 1 //v1.x. better performance than v2.0
	/* set the carrier recovery loop's acquisition range and loop BW */
	{
		if(ulSymbolRate >= 3000)
		{
			val = TDA_10023_R_CARCONF_HIGH_SR_TRK; //0x0A(acquision phase) => 0x02 (tracking phase)
		}
		else if(ulSymbolRate >= 1500)
		{
			val = TDA_10023_R_CARCONF_MID_SR_TRK; //0x0C => 0x05
		}
		else
		{
			val = TDA_10023_R_CARCONF_LOW_SR_TRK; //0x0D => 0x09
		}
	}
#else //v2.0
	/* change the carrier recovery loop bandwidth according to to symbol rate */
	val = 0x05;
	if (ulSymbolRate < 3000)
		val = 0x02;
	if ( (ulSymbolRate < 1750) && (_chInfoWork.QAMmode < 4) )
		val = 0x0a;
#endif

	drv_ch_i2c_DEMODWrite(nUnitId, SB_CARCONF, &val, 1);
	s_aucCarConfReg[nUnitId]	= val;
}


/***************************************************************************
Performs QAM receiver locking trial based on Nyquist filter of the fixed
gain, then if the receiver is locked, it returns TRUE.
****************************************************************************/
unsigned char fast_lock_TDA( int nUnitId, HUINT32 unWorkingId, CH_CAB_TuneParam_t *pstCabTuneParam )
{
	unsigned char	val1;
	unsigned char count = 0;
	unsigned int unWId;
	int	nChannelId;

   	HBOOL bLocked = FALSE;

	unWId = unWorkingId;
	nChannelId = FindChannelID(nUnitId,DI_CH_TYPE_CAB);

	while( !bLocked )
	{
		/* CancelLock is called by MW before finishing the Locking Process */
		/* The purpose of this condition is to escape the loop(while) */
		if (GetTuneStatus(nChannelId) == CH_STATUS_CANCLE)
		{
			CH_DI_Print(0,("fast_lock_TDA()... Channel[%d] is Canceled by MW before finishing the locking process...\n", nChannelId));
			bLocked = FALSE;
			return DI_CH_CANCEL_TUNE;
		}

		CH_DI_Print(5, ("=== fast_lock_TDA(%d) : Lock Try [%d] === \r\n", nUnitId, count));
		count++;

		/* soft reset must be done after register set-up */
		ClearDemod( nUnitId, pstCabTuneParam->etModulation );

		VK_TASK_Sleep(10);		/* wait until AGC loop settles */

		/* GNYQ Setting */
		ManageBandTypeGain( nUnitId, pstCabTuneParam->ulSymbolRate );

		/* increase the AGC time constant */
		val1 = R_AGCONF_SLOW;	//RF ATC : 7, IF ATC : 7
		drv_ch_i2c_DEMODWrite(nUnitId, SB_AGCCONF, &val1, 1);

		/* wait for ALL sync */
		VK_TASK_Sleep(100);
		//by ilee for test
		//check_agc_lock( nUnitId );
		/***********************   Check the sync information ***********************/
		/* read the sync register value */
		drv_ch_i2c_DEMODRead(nUnitId, SB_SYNC, &val1, 1);
		CH_DI_Print(5, (">fast_lock_TDA() :  SYNC=0x%x\r\n", val1&0x4f));

		/* check if carrier was locked and if equalizer is at tracking mode */
		if((val1&0x02)!=0x02)
		{
			CH_DI_Print(1, ("carrier unlocked(cnt:%d)\r\n", count));
			bLocked = FALSE;
		}
		else
		{
			CH_DI_Print(1, ("CARRIER locked\r\n"));

			/*
			 * check if all of the front-end were synchronized
			 */
			//drv_ch_i2c_DEMODRead(nUnitId, SB_SYNC, &val1, 1);
			//CH_PrintDebug(">fast_lock_TDA() :  SYNC=0x%x\r\n", val1);
			if(((val1 & 0x48) == 0x08))
			{
				CH_DI_Print(1, ("FEC locked. \r\n"));
				/*  Reduce the varrier recovery loop BW 	 */
				AdjustCarrierRecovBW( nUnitId, pstCabTuneParam->ulSymbolRate );

				bLocked = TRUE;
				return TRUE;
			}
			else
			{
				CH_DI_Print(1, ("fec unlocked : val1(0x%x)\r\n", val1));
				bLocked	= FALSE;
			}
		} /* end of FEC check */

		/* This part do multiple trials for QAM LOCK, so possibility of lock
		is increased, but this process takes much time.                 */
		if(count == LOCK_TRIAL_TDA)
		{
			CH_DI_Print(1, ("fast_lock() failed to lock to the signal\r\n"));
		        bLocked	= FALSE;
		        return FALSE;
		}
	} /* end of while */

	return TRUE;
}

/* Calcaulte the estimated SNR value */
unsigned long measure_SNR_TDA( int nUnitId, CH_CAB_Modulation_e etModulation )
{
	unsigned long	ulSNRfact, ulMSE_thresh, ulMSE_offset, ulMSE_mul, ulMSE_add;
	unsigned char	ucMSE, ucMSE_prev, ucNo=3;

	do
	{
		ucNo--;
		drv_ch_i2c_DEMODRead(nUnitId, SB_MSE, &ucMSE_prev, 1);	// read MSE
		drv_ch_i2c_DEMODRead(nUnitId, SB_MSE, &ucMSE, 1);		// read MSE
	} while ((ucMSE_prev!=ucMSE) && ucNo);

	switch ( etModulation )
	{
		case DI_CH_CMOD_16QAM : /* 16-QAM: MSE=18:80 */
			ulMSE_thresh = MSE_THRESH_16QAM;
			ulMSE_offset = MSE_OFFSET_16QAM;
			ulMSE_mul	 = MSE_MUL_16QAM;
			ulMSE_add	 = MSE_ADD_16QAM;
			break;
		case DI_CH_CMOD_32QAM : /* 32-QAM: MSE=13:64 */
			ulMSE_thresh = MSE_THRESH_32QAM;
			ulMSE_offset = MSE_OFFSET_32QAM;
			ulMSE_mul	 = MSE_MUL_32QAM;
			ulMSE_add	 = MSE_ADD_32QAM;
			break;
		case DI_CH_CMOD_64QAM : /* 64-QAM: MSE=9:40 */
			ulMSE_thresh = MSE_THRESH_64QAM;
			ulMSE_offset = MSE_OFFSET_64QAM;
			ulMSE_mul	 = MSE_MUL_64QAM;
			ulMSE_add	 = MSE_ADD_64QAM;
			break;
		case DI_CH_CMOD_128QAM : /* 128-QAM: MSE=8:33 */
			ulMSE_thresh = MSE_THRESH_128QAM;
			ulMSE_offset = MSE_OFFSET_128QAM;
			ulMSE_mul	 = MSE_MUL_128QAM;
			ulMSE_add	 = MSE_ADD_128QAM;
			break;
		case DI_CH_CMOD_256QAM : /* 256-QAM: MSE=10:25 */
			ulMSE_thresh = MSE_THRESH_256QAM;
			ulMSE_offset = MSE_OFFSET_256QAM;
			ulMSE_mul	 = MSE_MUL_256QAM;
			ulMSE_add	 = MSE_ADD_256QAM;
			break;
		default:
			ulMSE_thresh = MSE_THRESH_64QAM;
			ulMSE_offset = MSE_OFFSET_64QAM;
			ulMSE_mul	 = MSE_MUL_64QAM;
			ulMSE_add	 = MSE_ADD_64QAM;
			break;
	}
	/* SNR factor = 10 * SNR */
	ulSNRfact = ulMSE_thresh/(ulMSE_mul * ucMSE + ulMSE_add) + ulMSE_offset;
	//CH_DI_Print(6, ("ucMSE:0x%x , ulSNRfact : %d(0x%x) \n", ucMSE , ulSNRfact,  ulSNRfact ));
	CH_DI_Print(2, ("   SNR[%d] :%ld \n", nUnitId, ulSNRfact));

	return ulSNRfact;
}

/*******************************************************************************
*	function	: getUncorr
*	Description	: the number of uncorrected Transport-Stream Packets
*	Status		:
********************************************************************************/
unsigned long getUncorr(int nUnitId)
{
	unsigned char ucUnCorr[4];
	unsigned long ulUnCorr;

	drv_ch_i2c_DEMODRead(nUnitId, TDA10024_CPT_TSP_UNCOR1_IND, &ucUnCorr[0], 4);

	ulUnCorr = (ucUnCorr[3] << 24) | (ucUnCorr[2] << 16) | (ucUnCorr[1] << 8) | ucUnCorr[0];
	CH_DI_Print(6, ("   UnCorr[%d] : [%d]\n", nUnitId, ulUnCorr));

	return ulUnCorr;
}

unsigned long getUnErr(int nUnitId)
{
	unsigned char ucUnErr[4];
	unsigned long ulUnErr;

	drv_ch_i2c_DEMODRead(nUnitId, TDA10024_CPT_TSP_OK1_IND, &ucUnErr[0], 4);

	ulUnErr = (ucUnErr[3] << 24) | (ucUnErr[2] << 16) | (ucUnErr[1] << 8) | ucUnErr[0];
	CH_DI_Print(6, ("   UnErr[%d] : [%d]\n", nUnitId, ulUnErr));

	return ulUnErr;
}

unsigned long getCorr(int nUnitId)
{
	unsigned char ucCorr[4];
	unsigned long ulCorr;

	drv_ch_i2c_DEMODRead(nUnitId, R_RSCONF2, &ucCorr[0], 4);

	ulCorr = (ucCorr[3] << 24) | (ucCorr[2] << 16) | (ucCorr[1] << 8) | ucCorr[0];
	CH_DI_Print(6, ("   Corr[%d] : [%d]\n", nUnitId, ulCorr));

	return ulCorr;
}


unsigned char getIfAgc(int nUnitId)
{
	unsigned char ucIfAgc;

	drv_ch_i2c_DEMODRead(nUnitId, SB_VAGC2, &ucIfAgc, 1);

	CH_DI_Print(6, ("   getIfAgc[%d] : [%d]\n", nUnitId, ucIfAgc));

	return ucIfAgc;

}

void makeBer(unsigned int ulBer, int *ulExp, int *ulDecimal, int *ulFloat)
{

	if(ulBer > 999999)
	{
	         *ulExp = -2;
	         *ulDecimal = ulBer / 1000000;
	         *ulFloat = (ulBer % 1000000) / 10000;
	}
	else if(ulBer > 99999)
	{
	         *ulExp = -3;
	         *ulDecimal = ulBer / 100000;
	         *ulFloat = (ulBer % 100000) / 1000;
	}
	else if(ulBer > 9999)
	{
	         *ulExp = -4;
	         *ulDecimal = ulBer / 10000;
	         *ulFloat = (ulBer % 10000) / 100;
	}
	else if(ulBer > 999)
	{
	         *ulExp = -5;
	         *ulDecimal = ulBer / 1000;
	         *ulFloat = (ulBer % 1000) / 10;
	}
	else if(ulBer > 99)
	{
	         *ulExp = -6;
	         *ulDecimal = ulBer / 100;
	         *ulFloat = ulBer % 100;
	}
	else if(ulBer > 9)
	{
	         *ulExp = -7;
	         *ulDecimal = ulBer / 10;
	         *ulFloat = ulBer % 10;
	}
	else// if(ber > 0)
	{
	         *ulExp = -8;
	         *ulDecimal = ulBer;
	         *ulFloat = 0;
	}


	//CH_DI_Print( 6, ("   makeBer[%d][%d][%d]\n",*ulDecimal, *ulFloat, *ulExp));

}

unsigned long getBer(int nUnitId)
{
	unsigned char ucBer[3], ucBerDepth, ucPBER;
	unsigned long ulBer;

	drv_ch_i2c_DEMODRead(nUnitId, SB_BER_LSB, &ucBer[0], 3);

	ulBer = (ucBer[2] << 16) | (ucBer[1] << 8) | ucBer[0];
	//CH_PrintMon("   getBer[0x%x] (0x%x)(0x%x)(0x%x)\n",ulBer, ucBer[2], ucBer[1], ucBer[0]);

	drv_ch_i2c_DEMODRead(nUnitId, SB_RSCONF, &ucBerDepth, 1);
	ucPBER = (ucBerDepth & 0xc0) >> 6;
	CH_DI_Print(5, ("   getBerDepth[0x%x] (0x%x)\n",ucPBER, ucBerDepth));

	switch(ucPBER)
	{
	        case 0:  //bit5, 1,00E+05
	            ulBer *= 1000;
				CH_DI_Print(2, ("   BER :%ld \t 10^-5 \n",ulBer/1000));
	            break;
	        case 1:  //bit6, 1,00E+06
	            ulBer *= 100;
				CH_DI_Print(2, ("   BER :%ld \t 10^-6 \n",ulBer/100));
	            break;
	        case 2:  //bit7, 1,00E+07
	            ulBer *= 10;
				CH_DI_Print(2, ("   BER :%ld \t 10^-7 \n",ulBer/10));
	            break;
	        case 3:  //bit 8, 1,00E+08
				CH_DI_Print(2, ("   BER :%ld \t 10^-8 \n",ulBer));
	            break;
	}
	//CH_PrintMon("   getBer[%d]\n",ulBer);

	return ulBer;

}

void getRfAgc(int nUnitId, unsigned int *pulRfAgc)
{

#if defined(PLATFORM_TM2063)
	unsigned char ucLNA=0, ucRF=0, ucFIF=0, ucIFAGC=0;
	unsigned char ucVal1=0, ucVal2=0, ucVal3=0;

	DRV_CH_C_I2CEnable(nUnitId);
	MT2063_GetReg(hTuner[nUnitId], 0x14, &ucVal1);	//XO_STATUS
	MT2063_GetReg(hTuner[nUnitId], 0x15, &ucVal2);	// RF_STATUS
	MT2063_GetReg(hTuner[nUnitId], 0x16, &ucVal3);	// FIF_STATUS
	DRV_CH_C_I2CDisable(nUnitId);

	ucLNA = (ucVal1 & 0x1F);
	ucRF = (ucVal2 & 0x1F);
	ucIFAGC = (ucVal3 & 0xE0)>>5;
	ucFIF = (ucVal3 & 0x1F);

	CH_DI_Print(4, ("getRfAgc: ucLNA(%d), ucRF(%d), ucFIF(%d), ucIFAGC(%d)\n", ucLNA, ucRF, ucFIF, ucIFAGC));
	*pulRfAgc = (unsigned int)(ucLNA + ucRF + ucFIF);


#endif

#if defined(PLATFORM_TN182I2) || defined(PLATFORM_TM201)	// From 1T
	drv_ch_i2c_DEMODRead(nUnitId, SB_VAGC, (HUINT8 *)pulRfAgc, 1);
#endif

}

#if defined(PLATFORM_TN182I2) || defined(PLATFORM_TM201)	// From 1T
void getRfAgc10024(int nUnitId, unsigned char *pulRfAgc)
{
	drv_ch_i2c_DEMODRead(nUnitId, SB_VAGC, (HUINT8 *)pulRfAgc, 1);
}
#endif

#define __CH_C_PUBLIC_FUNCITONS__

static void DRV_CH_C_I2CEnable(HUINT8 unUnitId)
{
#if defined(PLATFORM_TN182I2) || defined(PLATFORM_TM201)
	unsigned char	reterr, regValue[2] = { 0, 0 } ;
	unsigned char	TDA10024_TEST_SCLT0_BIT = 0x40;

	reterr =drv_ch_i2c_DEMODRead(unUnitId, SB_TEST, regValue, 2);

	regValue[0] &= ~TDA10024_TEST_SCLT0_BIT;
	reterr = drv_ch_i2c_DEMODWrite(unUnitId, SB_TEST, regValue, 1);
	if (reterr != DI_CH_OK)
	{
		VK_TASK_Sleep(10);
		reterr = drv_ch_i2c_DEMODWrite(unUnitId, SB_TEST, regValue, 1);
		if (reterr != DI_CH_OK)
		{
			CH_DI_Print(0, ("DRV_CH_C_I2CEnable error \n"));
		}
	}

	regValue[0] |= 0x80 ;
	reterr = drv_ch_i2c_DEMODWrite(unUnitId, SB_TEST, regValue, 2);
	if (reterr != DI_CH_OK)
	{
		VK_TASK_Sleep(10);
		reterr = drv_ch_i2c_DEMODWrite(unUnitId, SB_TEST, regValue, 2);
		if (reterr != DI_CH_OK)
		{
			CH_DI_Print(0, ("DRV_CH_C_I2CEnable error \n"));
		}
	}
#else
	unsigned char		oldval = 0x40,newval;

	/*(BYPIIC == 1) The SCL_TUN, SDA_TUN I2C bus is connected to the master(SCL,SDA)I2C bus.*/
	//drv_ch_i2c_DEMODRead(unUnitId, SB_TEST, &oldval);
	newval = oldval | 0x80 ;
	drv_ch_i2c_DEMODWrite(unUnitId, SB_TEST, &newval, 1);

	//drv_ch_i2c_DEMODRead(unUnitId, SB_TEST, &oldval);
	//CH_PrintDebug(">>>>>>>>>>>>>>oldval(%x)\n", newval);
#endif
}

static void DRV_CH_C_I2CDisable(HUINT8 unUnitId)
{
#if defined(PLATFORM_TN182I2) || defined(PLATFORM_TM201)
	unsigned char	newval;
	unsigned char	reterr, regValue[2] = { 0, 0 } ;
	unsigned char	TDA10024_TEST_SCLT0_BIT = 0x40;

	reterr =drv_ch_i2c_DEMODRead(unUnitId, SB_TEST, regValue, 2);

	newval = ((~(0x80|TDA10024_TEST_SCLT0_BIT)& regValue[0]) | TDA10024_TEST_SCLT0_BIT);
	reterr = drv_ch_i2c_DEMODWrite(unUnitId, SB_TEST, regValue, 1);
	if (reterr != DI_CH_OK)
	{
		VK_TASK_Sleep(10);
		reterr = drv_ch_i2c_DEMODWrite(unUnitId, SB_TEST, regValue, 1);
		if (reterr != DI_CH_OK)
		{
			CH_DI_Print(0, ("DRV_CH_C_I2CDisable error \n"));
		}
	}

#else
	unsigned char		oldval,newval;

	drv_ch_i2c_DEMODRead(unUnitId, SB_TEST, &oldval, 1);
	newval = oldval & 0x7F ; /* (BYPIIC == 0 ) The SCL_TUN and SDA_TUN are tri-stated. */
	drv_ch_i2c_DEMODWrite(unUnitId, SB_TEST, &newval, 1);
#endif
}

/*******************************************************************
Config Tuner PLL
	Writing the tuning frequency into tuner & Program the tuner's PLL registers

*******************************************************************/
void DRV_CH_C_ConfigTunerPll( int nChannelId, HUINT32 ulFreqHz, CH_SelectIf_e etSelectIf )
{
	unsigned char unitId;
	int nChannelType, ifType;
#if defined(PLATFORM_TM2063)
	int status;
#endif
#if defined(PLATFORM_TN182I2)
	HUINT8 reterr;
#endif
#if defined(PLATFORM_TM201)
	HUINT32 ulRet = 0;
#endif

	ifType = etSelectIf;

	unitId = (HUINT8)GetDeviceUnitId( nChannelId );
	nChannelType = GetChannelType( nChannelId );

	CH_DI_Print(5, ("DRV_CH_C_ConfigTunerPll(unitId:%d)(%d)\n", unitId, ulFreqHz ));
	DRV_CH_C_I2CEnable(unitId);

#if defined(PLATFORM_TM2063)
	status = MT2063_Tune(hTuner[unitId], ulFreqHz);
	CH_DI_Print(5, ("DRV_CH_C_ConfigTunerPll(%x)\n", status));
#endif

#if defined(PLATFORM_TN182I2)
	{
		TDA182I2Request_t TuneRequest;

		TuneRequest.eTunerLock = tmFrontEndStateUnknown;
		TuneRequest.dwFrequency = ulFreqHz;	//Hz
		TuneRequest.dwStandard = tmTDA182I2_QAM_8MHz;

		reterr = TDA182I2SendRequest(unitId, &TuneRequest, TRT_DVBC);
		if(reterr)
		CH_DI_Print(4, (" [TDA182I2SendRequest] err : val (%d) \n\n\n" , reterr));
	}
#endif

#if defined(PLATFORM_TM201)
	ulRet = MxL_Tuner_RFTune(&g_stSemTunerConfig[unitId], ulFreqHz, MxL_BW_8MHz);
#endif
	DRV_CH_C_I2CDisable(unitId);
}

#if defined(PLATFORM_TM2063)
static int MT2063_CalRFIndex(int startIndex, unsigned int uIrfagc, unsigned char ucifagc, unsigned int *rfAgcTable, unsigned int *ifAgcTable,  int *offset)
{

	int i, DsPwrIndex=0;
       unsigned int  agcval;

       agcval = uIrfagc*5 + (255-ucifagc);

	for (i=startIndex; i<CONST_LEVEL_NUM; i++)
	{
		if ( agcval >= (rfAgcTable[i]*5 + 255-ifAgcTable[i]) )
		{
			DsPwrIndex = i;
			if (agcval == (rfAgcTable[i]*5 + 255-ifAgcTable[i]))
			{
				*offset = 0;
			}
			else
			{
				if( i == 0)
				{
					*offset = 0;
				}
				else
				{
					*offset = ((agcval - (rfAgcTable[i]*5 + 255-ifAgcTable[i])) *10) / ((rfAgcTable[i-1]*5 + 255-ifAgcTable[i-1]) - (rfAgcTable[i]*5 + 255-ifAgcTable[i]));
				}
			}
			break;
		}
		else
		{
			//CH_DI_Print(6,("RF : %d\n", i));
		}
	}

	if(i == CONST_LEVEL_NUM)
	{
		DsPwrIndex = CONST_LEVEL_NUM;
	}

	CH_DI_Print(6, ("[RF] table[%d] = %d, agc = %d, DsPwrIndex = %d, offset = %d\n", i, (rfAgcTable[i]*5 + 255-ifAgcTable[i]), agcval, DsPwrIndex, *offset));


	return DsPwrIndex;
}


static int  MT2063_CalIFIndex(int  startIndex, unsigned char agc, unsigned int *AgcTable, int *offset)
{

	unsigned int i, DsPwrIndex=0;
       unsigned int agcval;

       agcval = agc;



	for (i=startIndex; i<CONST_LEVEL_NUM; i++)
	{
		if ( (agcval <= AgcTable[i]) )
		{
			DsPwrIndex = i;
			if (agcval == AgcTable[i])
			{
				*offset = 0;
			}
			else
			{
				if( i == 0)
                            {
                                *offset = 0;
                            }
                            else
                            {
                                *offset = ((agcval - AgcTable[i]) *10) / (AgcTable[i-1] - AgcTable[i]);
                            }

			}
			break;
		}
	}

	if(i == CONST_LEVEL_NUM)
	{
		DsPwrIndex = CONST_LEVEL_NUM;
	}
	CH_DI_Print(6, ("[IF] table[%d] = %d, agc = %d, DsPwrIndex = %d, offset=%d\n", i, AgcTable[i], agcval, DsPwrIndex, *offset));
	return DsPwrIndex;
}


static int MT2063_SetSignalLevel(int rfPwrIndex, int ifPwrIndex, int rfoffset, int ifoffset)
{
	int	strength=0;
	int  level, offset;


	if( ifPwrIndex <= rfPwrIndex )
	{
		level = rfPwrIndex;
		offset = rfoffset;
	}
	else
	{
		level = ifPwrIndex;
		offset = ifoffset;
	}

	if(level == 0)
	{
		strength = 300;
	}
	else if(level == CONST_LEVEL_NUM)
	{
		strength = -300;
	}
	else
	{
		strength = (aulDownLevel[level]*10)+offset;

		if((strength > -5) && (strength < 0))
		{
			strength = 0;
		}
		else if((strength >= -9) && (strength <= -5))
		{
		 	strength = -10;
		}

	}

	CH_DI_Print(4, ("[SetSignalLevel] level[%d] = %d, offset = %d, strength = %d\n", level, aulDownLevel[level], offset, strength));

	return strength;
}


static int MT2063_GetSignalLevel(unsigned long ulFreq, unsigned int uIRfAgc, unsigned char ucIfAgc)
{
	int  strength=0, rfoffset=0, ifoffset=0;
	int  levelRfIndex=0, levelIfIndex=0;


	/* 주파수는 771MHz 로 고정. 우선..*/

	if(ulFreq < 506000)
	{
		levelRfIndex = MT2063_CalRFIndex(0, uIRfAgc, ucIfAgc, aulRfAgc_94, aulIfAgc_94, &rfoffset);
		levelIfIndex = MT2063_CalIFIndex(0, ucIfAgc, aulIfAgc_94, &ifoffset);
	}
	else if( (ulFreq >= 506000) && (ulFreq < 810000) )
	{
		levelRfIndex = MT2063_CalRFIndex(0, uIRfAgc, ucIfAgc, aulRfAgc_506, aulIfAgc_506, &rfoffset);
		levelIfIndex = MT2063_CalIFIndex(0, ucIfAgc, aulIfAgc_506, &ifoffset);
	}
	else if(ulFreq >= 810000)
	{
		levelRfIndex = MT2063_CalRFIndex(0, uIRfAgc, ucIfAgc, aulRfAgc_810, aulIfAgc_810, &rfoffset);
		levelIfIndex = MT2063_CalIFIndex(0, ucIfAgc, aulIfAgc_810, &ifoffset);
	}

	strength = MT2063_SetSignalLevel(levelRfIndex, levelIfIndex, rfoffset, ifoffset);

	CH_DI_Print(4, ("[SIGNAL_LEVEL] Frq = %ld, RfIndex = %d, IfIndex = %d, strength = %d\n", ulFreq, levelRfIndex, levelIfIndex, strength));

	return strength;

}
#endif

#if defined(PLATFORM_TN182I2)  || defined(PLATFORM_TM201)
static unsigned int TDA182I2_GetQualityLevel( int nUnitId, CH_CAB_Modulation_e etModulation, HUINT32 ulSymbolRate)
{
	unsigned int ulQualityLevel=0, ulBer=0, ulElapsedTime=0, ulElapsedBits=0, ulBitErrorPerMbps=0, ulUnCorr=0;
	unsigned char ucBer[3], ucBitsPerBaud=0;

	drv_ch_i2c_DEMODRead(nUnitId, SB_BER_LSB, &ucBer[0], 3);

	ulBer = (ucBer[2] << 16) | (ucBer[1] << 8) | ucBer[0];

	ulElapsedTime = 1000 * 1;

	switch(etModulation)
	{
		case DI_CH_CMOD_16QAM:
			ucBitsPerBaud = 4;
			break;
		case DI_CH_CMOD_32QAM:
			ucBitsPerBaud = 5;
			break;
		case DI_CH_CMOD_64QAM:
			ucBitsPerBaud = 6;
			break;
		case DI_CH_CMOD_128QAM:
			ucBitsPerBaud = 7;
			break;
		case DI_CH_CMOD_256QAM:
			ucBitsPerBaud = 8;
			break;
		default:
			ucBitsPerBaud = 6;
			break;
	}

	ulElapsedBits = (unsigned int)(ucBitsPerBaud*(ulElapsedTime/1000.)*ulSymbolRate/1000.);

	ulBitErrorPerMbps = ulBer / ulElapsedBits;

	if(ulBitErrorPerMbps >= 10000)	ulQualityLevel = 10;
	else if(ulBitErrorPerMbps >= 7500)	ulQualityLevel = 20;
	else if(ulBitErrorPerMbps >= 5000)	ulQualityLevel = 30;
	else if(ulBitErrorPerMbps >= 2500)	ulQualityLevel = 40;
	else if(ulBitErrorPerMbps >= 1000)	ulQualityLevel = 50;
	else if(ulBitErrorPerMbps >= 750)	ulQualityLevel = 60;
	else if(ulBitErrorPerMbps >= 500)	ulQualityLevel = 70;
	else if(ulBitErrorPerMbps >= 250)	ulQualityLevel = 80;
	else if(ulBitErrorPerMbps >= 100)	ulQualityLevel = 90;
	else							ulQualityLevel = 100;

	ulUnCorr = getUncorr(nUnitId);

	CH_DI_Print(4, ("QualityLevel[%ld] BER(%ld) ulUnCorr(%d)\n", ulQualityLevel, ulBer, ulUnCorr));

	return ulQualityLevel;
}
#endif

static int DRV_CH_C_InbandStrength(int nChannelId)
{
	int nUnitId, lStrength=0;
#if defined(PLATFORM_TN182I2)
	int nResult = DI_CH_OK;
#endif
#if defined(PLATFORM_TM201)
	unsigned int ulRet=0;
	float fTunerPower=0.0;
#endif
	unsigned long ulFreq = 0;
	unsigned int	ulRfAgc=0;
	unsigned char ucIfAgc=0, ucRfAgc=0;
	CH_CAB_TuneParam_t * pstCabTuneParam;

	nUnitId = GetDeviceUnitId( nChannelId );
	pstCabTuneParam = (CH_CAB_TuneParam_t *)GetWorkingTuneInfo( nChannelId );

	ucIfAgc = getIfAgc(nUnitId);
#if defined(PLATFORM_TN182I2) || defined(PLATFORM_TM201)
	CH_UNUSED(ulRfAgc);
	getRfAgc10024(nUnitId, &ucRfAgc);
#else
	getRfAgc(nUnitId, &ulRfAgc);
#endif

	ulFreq = pstCabTuneParam->ulFrequency;
#if defined(PLATFORM_TM2063)
       lStrength = MT2063_GetSignalLevel(ulFreq, ulRfAgc, ucIfAgc);
#endif
#if defined(PLATFORM_TN182I2)
	nResult = TN182I2_PowerMeasurement(nUnitId, &lStrength);
	if(nResult)
	{
		CH_DI_Print(0, (" [TDA10024_GetStrengthAgc] Get Power Measurement err : %d \n" , nResult));
		return nResult;
	}

	if (lStrength == 20 && ucRfAgc >70)
	{
		lStrength = 10;
	}
#endif
#if defined(PLATFORM_TM201)
	DRV_CH_C_I2CEnable(nUnitId);
	ulRet = MxL_Check_RF_Input_Power(&g_stSemTunerConfig[nUnitId], &fTunerPower);
	if(ulRet)
	{
		CH_DI_Print(0, (" [MxL201_GetStrengthAgc] Get Power Measurement err : %d \n" , ulRet));
		DRV_CH_C_I2CDisable(nUnitId);
		return ulRet;
	}
	DRV_CH_C_I2CDisable(nUnitId);

	st_10024SignalStatus[nUnitId].fSignalPower = fTunerPower;
	CH_DI_Print(6, ("[MxL201_GetStrengthAgc] Get Power Measurement : [%.2f]dBm [%.2f]dBuV \n" , fTunerPower, fTunerPower+47.7+60.0));

	fTunerPower += 47.7; 	//( dBm -> dBmV)
	fTunerPower += 60.0;	//( dBmV -> dBuV)

	if(fTunerPower >= 55)	lStrength = 100;
	else if(fTunerPower >= 53)	lStrength = 90;
	else if(fTunerPower >= 51)	lStrength = 80;
	else if(fTunerPower >= 49)	lStrength = 70;
	else if(fTunerPower >= 47)	lStrength = 60;
	else if(fTunerPower >= 45)	lStrength = 50;
	else if(fTunerPower >= 43)	lStrength = 40;
	else if(fTunerPower >= 41)	lStrength = 30;
	else if(fTunerPower >= 39)	lStrength = 20;
	else 	lStrength = 10;
#endif
	return lStrength;

}

int DRV_CH_C_GetSignalStatus(int nChannelId, HBOOL isLock)
{
	int nResult = DI_CH_OK;
	int nRet = DI_CH_OK, nUnitId=0, nStrength=0;
	HUINT8 ucAgc=0, ucRfAgc=0, ucIfAgc=0;
	HULONG ulQuality = 0, ulSnr=0, ulUncorrected=0, ulBer;

	nUnitId = GetDeviceUnitId(nChannelId);

	if(isLock == TRUE)
	{
		//DP10024_LockInfo(nChannelId);
		nRet = DP10024_GetStrengthAgc(nChannelId, &nStrength, &ucAgc, &ucRfAgc, &ucIfAgc);
		if( nRet != DI_CH_OK )
		{
			nResult = DI_CH_ERR;
			CH_PrintErr(" ");
		}
		else
		{
			st_10024SignalStatus[nUnitId].nStrength = nStrength;
			st_10024SignalStatus[nUnitId].ucAgc = ucAgc;
			st_10024SignalStatus[nUnitId].ucRfAgc = ucRfAgc;
			st_10024SignalStatus[nUnitId].ucIfAgc = ucIfAgc;
		}

		nRet = DP10024_GetQualityBer( nChannelId, &ulQuality, &ulBer, &ulSnr, &ulUncorrected );
		if( nRet != DI_CH_OK )
		{
			nResult = DI_CH_ERR;
			CH_PrintErr(" ");
		}
		else
		{
			st_10024SignalStatus[nUnitId].ulQuality = ulQuality;
			st_10024SignalStatus[nUnitId].ulBer = ulBer;
			st_10024SignalStatus[nUnitId].ulSnr = ulSnr;
			st_10024SignalStatus[nUnitId].ulUncorrected = ulUncorrected;
		}

	}
	else	/* Unlock */
	{
		st_10024SignalStatus[nUnitId].nStrength = 0;
		st_10024SignalStatus[nUnitId].ucAgc = 0;
		st_10024SignalStatus[nUnitId].ulQuality = 0;
		st_10024SignalStatus[nUnitId].ulBer = 0;
		st_10024SignalStatus[nUnitId].ulSnr = 0;
		st_10024SignalStatus[nUnitId].ucRfAgc = 0;
		st_10024SignalStatus[nUnitId].ucIfAgc = 0;
		st_10024SignalStatus[nUnitId].ulUncorrected = 0;
	}
	CH_DI_Print(3, ("DRV_CH_C_GetSignalStatus Strength[%d] Quality[%ld]\n", nStrength, ulQuality));

	return nResult;
}

int DP10024_GetStrengthAgc( int nChannelId, HINT32 *pnStrength, HUINT8 *pucAgc, HUINT8 *pucRfAgc, HUINT8 *pucIfAgc)
{
	int nResult = DI_CH_OK;
	int nUnitId;
	unsigned int ulRfAgcTmp=0;

	nUnitId = GetDeviceUnitId( nChannelId );

#if defined(PLATFORM_TN182I2) || defined(PLATFORM_TM201)
	CH_UNUSED(ulRfAgcTmp);
	getRfAgc10024(nUnitId, pucRfAgc);
#else
	getRfAgc(nUnitId, &ulRfAgcTmp);
	*pucRfAgc = (HUINT8)ulRfAgcTmp;
#endif
	*pucIfAgc = getIfAgc(nUnitId);

	*pnStrength = DRV_CH_C_InbandStrength(nChannelId);

#if defined(PLATFORM_TN182I2) || defined(PLATFORM_TM201)
	*pucAgc = *pucRfAgc;
#else
	*pucAgc = *pucIfAgc;
#endif

	CH_DI_Print(3, ("   TDA10024_GetStrengthAgc \t[%d] dB (%ld)\n", *pnStrength, *pucAgc));
	return nResult;

}


int DP10024_GetQualityBer( int nChannelId, HULONG *pulQuality, HULONG *pulBer,  HULONG *pulSnr, HULONG *pulUncorrected )
{

	int nResult = DI_CH_OK;
	int nUnitId;

	CH_CAB_TuneParam_t * pstCabTuneParam;

	pstCabTuneParam = (CH_CAB_TuneParam_t *)GetWorkingTuneInfo( nChannelId );

	nUnitId = GetDeviceUnitId( nChannelId );

	//VK_TASK_Sleep(300);
	*pulBer = getBer(nUnitId);
	*pulSnr = measure_SNR_TDA( nUnitId, pstCabTuneParam->etModulation );

#if defined(PLATFORM_TN182I2) || defined(PLATFORM_TM201)
	*pulQuality = (HULONG)TDA182I2_GetQualityLevel(nUnitId, pstCabTuneParam->etModulation, pstCabTuneParam->ulSymbolRate);
#else
	*pulQuality = *pulSnr;
#endif
	CH_DI_Print(3, ("   TDA10024_GetQualityBer \t[%ld] dB (%ld) (%ld)\n", *pulQuality, *pulBer, *pulSnr));

	*pulUncorrected = getUncorr(nUnitId);

	CH_DI_Print(2, ("   UnCorr \t[%ld]\n", *pulUncorrected));

	return nResult;

}

#if 0
int DP10024_SetFSampling(int nUnitId, HBOOL enable)
{
	unsigned char	uc_val, uc_val1;

	drv_ch_i2c_DEMODRead(nUnitId, SB_CONF, &uc_val1, 1);

	if (enable)
	{
		/* set FSAMPLING bit to 1(output PLL clock equal to SACLK) */
		uc_val = uc_val1 | 0x20;
	}
	else
	{
		/* set FSAMPLING bit to 0(output PLL clock divided by 2) */
		uc_val = uc_val1 & 0xdf;
	}

	drv_ch_i2c_DEMODWrite(nUnitId, SB_CONF, &uc_val, 1);
	CH_PrintMon("DP10024_SetFSampling\n");

}
#endif

void DP10024_SoftReset(int nUnitId)
{
	unsigned char	uc_val;

	uc_val = 0x7f;	//DVB mode select

	drv_ch_i2c_DEMODWrite(nUnitId, TDA_10023_SB_RESET, &uc_val, 1);
	CH_DI_Print(5, ("DP10024_SoftReset\n"));
}


int DP10024_GpioReset(int nUnitId)
{

	if (nUnitId== 0)
	{
		DRV_GPIO_Write(GPIO_ID_nCH0_RST, 1);

		VK_TASK_Sleep(20);

		/* CAM ReSeT Signal INACTIVE */
		DRV_GPIO_Write(GPIO_ID_nCH0_RST, 0);

		VK_TASK_Sleep(20);

		/* CAM ReSeT Signal INACTIVE */
		DRV_GPIO_Write(GPIO_ID_nCH0_RST, 1);
		CH_DI_Print(5, (" CH_GPIO [%d] reset !!\n", nUnitId));
	}
	else
	{
		DRV_GPIO_Write(GPIO_ID_nCH1_RST, 1);

		VK_TASK_Sleep(20);

		/* CAM ReSeT Signal INACTIVE */
		DRV_GPIO_Write(GPIO_ID_nCH1_RST, 0);

		VK_TASK_Sleep(20);

		/* CAM ReSeT Signal INACTIVE */
		DRV_GPIO_Write(GPIO_ID_nCH1_RST, 1);
		CH_DI_Print(5, (" CH_GPIO [%d]reset !!\n", nUnitId));
	}

	return DI_CH_OK;
}


int DP10024_InitDevice( int nChannelId )
{
	int nUnitId;
	HUINT32 ulI2CBus = 0;
	CH_DI_Print(5, ("TDA10024_InitDevice ...S\n"));

	nUnitId = GetDeviceUnitId( nChannelId );

	DP10024_GpioReset(nUnitId);

#if defined(PLATFORM_TN182I2)	/* TDA182I2 Tuner set I2C speed 100K */
	if(nUnitId == MASTER_DEMOD )
	{
		ulI2CBus = 2;
		DRV_I2c_SetClock(ulI2CBus, DRV_I2C_CLOCK_100K);
	}
#elif defined(PLATFORM_TM201)
	if(nUnitId == MASTER_DEMOD )
	{
		ulI2CBus = I2C_CHANNEL_CHANNEL;

	}
#if defined(CONFIG_I2C_MAP_FOR_2TUNER)
	else if(nUnitId == SLAVE_DEMOD)
	{
		ulI2CBus = I2C_CHANNEL_CHANNEL2;
	}
#endif
	else
	{
		ulI2CBus = I2C_CHANNEL_CHANNEL;
	}
	DRV_I2c_SetClock(ulI2CBus, DRV_I2C_CLOCK_100K);
#endif

	/* TODO_ILEE0 : HW reset for HC1 model */
	if ( nUnitId == MASTER_DEMOD)
	{
		CH_DI_Print(6, ("TDA10024[%d] : I2C Addr[0x%02X]\n", nUnitId, TDA10024_M_DEMOD_ADDRESS));

	}
	else
	{
		CH_DI_Print(6, ("TDA10024[%d] : I2C Addr[0x%02X]\n", nUnitId, TDA10024_S_DEMOD_ADDRESS));

	}

	ClearDemod( nUnitId, DEFAULT_CAB_QAM );

	InitQAMdemod( nUnitId, DEFAULT_CAB_SYMBOLRATE, DEFAULT_CAB_QAM );
	CH_DI_Print(5, ("TDA10024_InitDevice ...E\n"));

#if defined(PLATFORM_TM2063)
	TM2063_InitTuner(nUnitId);
#endif

#if defined(PLATFORM_TN182I2)
	TN182I2_InitTuner(nUnitId);
#endif

#if defined(PLATFORM_TM201)
	CH_UNUSED(ulI2CBus);
	TM201_InitTuner(nUnitId);
#endif

	return DI_CH_OK;
}


int DP10024_SetTune( int nChannelId, HUINT32 ulWorkingId, CH_CAB_TuneParam_t *pstCabTuneParam )
{
#ifdef ENABLE_TDA_ISR
	unsigned char	data ;
#endif
	int nUnitId;
	HUINT32 ulFreqHz;
//	HUINT8	val;
	HUINT8	nResult = DI_CH_OK;


	nUnitId = GetDeviceUnitId( nChannelId );
	ulFreqHz = pstCabTuneParam->ulFrequency * KHZ_TO_HZ;

#if 0
	val = 0x23; // <10023>
	drv_ch_i2c_DEMODWrite(nUnitId, SB_CONF, &val, 1);
#endif
	CH_DI_Print(5, ("Enter ...(unitId:%d)(%d)\n", nUnitId, ulFreqHz));

	DRV_CH_C_ConfigTunerPll( nChannelId, ulFreqHz, DIRECT_IF );

	SetQAMdemod( nUnitId, pstCabTuneParam );

	nResult = fast_lock_TDA( nUnitId, ulWorkingId, pstCabTuneParam );
	if ( nResult == TRUE )
	{
#ifdef ENABLE_SNR_PRINT
		{
			unsigned long	SNRfactor;

			SNRfactor = measure_SNR_TDA( nUnitId, pstCabTuneParam->etModulation );
			CH_DI_Print(4, ("SNR =  %ld dB (%ld)\r\n", SNRfactor/10, SNRfactor));
		}
#endif

#ifdef ENABLE_TDA_ISR
		/* read the interrupt status register to prevent
		   the previous unlock state from reflecting the current lock status */
		drv_ch_i2c_DEMODRead(nUnitId, SB_ITSTAT, &data, 1);
		CH_DI_Print(6, ("PRE_LOCKProcess: ITSTATUS = 0x%2x\r\n",data));
		if( data & 0x02 ){
			if(_EnableUnlockMsgSend[nUnitId]){
				SendMessageSM(nUnitId, LOCK_TO_UNLOCK);

				_EnableUnlockMsgSend[nUnitId] = 0;/* disable for channel device to send the msg again after send the UNLOCK
									  messsage once. The UNLOCK msg can be sent once per SET_TUNER_TUNING_SET */
			}
			GotoINIT(nUnitId);
			return;
		}

		/* if ISR enabled, GPIO pin (PIN 29) be LOW level. GPIO pin is used as IRQ output pin */
		data = 0x82;
		drv_ch_i2c_DEMODWrite(nUnitId, SB_ITSEL, &data, 1);
#endif

/* remove AdaptBerWindow */
/* To reduce BER refresh time, BER depth of 10-7 is sufficient !!!*/
#if 0
		AdaptBerWindow( nUnitId, pstCabTuneParam->ulSymbolRate, pstCabTuneParam->etModulation );
#endif
		//GotoLOCK(nUnitId);
		CH_DI_Print(1, (">>> CAB CH_STATUS_LOCKED\n"));
		s_ucIsLock[nUnitId] = TRUE;
		return DI_CH_OK;

	}
	else if ( nResult == DI_CH_CANCEL_TUNE )
	{
		CH_DI_Print(1, (">>> CAB CH_STATUS_CANCLE\n"));
		s_ucIsLock[nUnitId] = FALSE;
		return DI_CH_CANCEL_TUNE;
	}
	else
	{
		CH_DI_Print(1, (">>> CAB CH_STATUS_UNLOCK\n"));
		s_ucIsLock[nUnitId] = FALSE;

		//DP10024_DumpRegister(nChannelId);
		//DP10024_SoftReset(nUnitId);
		return DI_CH_ERR;
	}

	/* TODO_ILEE : ClearBerStatus() */

}

HBOOL DP10024_CheckLock( int nChannelId)
{
	int nUnitId;
	HBOOL bLocked = TRUE;
	//unWorkingID = unWorkingID;
	nUnitId = GetDeviceUnitId( nChannelId );

	HUINT8	val1,val2;

	/* Get the information of MPEG frame sync from the FEC core */
	drv_ch_i2c_DEMODRead(nUnitId, SB_SYNC, &val1, 1);
	CH_DI_Print(5, (">DP10024_IsLock(%d) :  SYNC=0x%x\r\n", nUnitId, val1));
	val2 = val1 & 0xff;// <10023>0x48; /* mask all bits but NODVB & FSYNC bit */
#if defined(PLATFORM_TN182I2) || defined(PLATFORM_TM201)
	if( (val2&0x8e) == 0x8e )
#else
	/* check if both MPEG frame and carrier were synchronized */
	if( (val2&0x0f) ==0x0f ) // <10023> if( val2 ==0x08 )
#endif
	{
		bLocked = TRUE;
	}
	else if( ((val1&0x40)==0x40) )
	{
		CH_DI_Print(1, ("No DVB packet\r\n"));
		bLocked = FALSE;
	}
	else
	{
		bLocked = FALSE;
	}

	DRV_CH_C_GetSignalStatus(nChannelId, bLocked);
	return bLocked;

}

int  DP10024_GetStrengthQuality(int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo)
{
	int nUnitId;

	HINT32	nBerDecimal, nBerFloat, nBerExp;

	CH_CAB_TuneParam_t * pstCabTuneParam;

	nUnitId = GetDeviceUnitId( nChannelId );
	pstCabTuneParam = (CH_CAB_TuneParam_t *)GetWorkingTuneInfo( nChannelId );

	makeBer(st_10024SignalStatus[nUnitId].ulBer, &nBerExp, &nBerDecimal, &nBerFloat);

	if(s_ucIsLock[nUnitId] ==TRUE)
	{
		pstSignalInfo->fSnr = (float)st_10024SignalStatus[nUnitId].ulSnr/10;
		pstSignalInfo->fBer = (float)st_10024SignalStatus[nUnitId].ulBer;
		pstSignalInfo->ulUnCorrectedNo = (HUINT32)st_10024SignalStatus[nUnitId].ulUncorrected;
		pstSignalInfo->ulRfAgc = (HUINT32)st_10024SignalStatus[nUnitId].ucRfAgc;
		pstSignalInfo->ulIfAgc = (HUINT32)st_10024SignalStatus[nUnitId].ucIfAgc;
		pstSignalInfo->ulStrength = (HUINT32)st_10024SignalStatus[nUnitId].nStrength;
		pstSignalInfo->ulAgc = (HUINT32)st_10024SignalStatus[nUnitId].ucAgc;
		pstSignalInfo->ulQuality = (HUINT32)st_10024SignalStatus[nUnitId].ulQuality;
		pstSignalInfo->fSignalInputPower = st_10024SignalStatus[nUnitId].fSignalPower;
	}
	else
	{
		pstSignalInfo->fSnr = 0;
		pstSignalInfo->fBer = 0;
		pstSignalInfo->ulUnCorrectedNo = 0;
		pstSignalInfo->ulRfAgc = 0;
		pstSignalInfo->ulIfAgc = 0;
		pstSignalInfo->ulStrength = 0;
		pstSignalInfo->ulAgc = 0;
		pstSignalInfo->ulQuality = 0;
		pstSignalInfo->fSignalInputPower = 0;
	}

	CH_DI_Print(0, ("===================================================\n"));
	CH_DI_Print(0, ("   CAB TUNE Unit [%d] Status(%s)\n", nUnitId, (s_ucIsLock[nUnitId] ==TRUE)?"Lock":"Unlock"));
	CH_DI_Print(0, ("===================================================\n"));
	CH_DI_Print(0, ("   Freq \t[%d]\n", pstCabTuneParam->ulFrequency));
	CH_DI_Print(0, ("   SR \t\t[%d]\n", pstCabTuneParam->ulSymbolRate));
	if(pstCabTuneParam->etModulation == DI_CH_CMOD_16QAM)
		CH_DI_Print(0, ("   Mod \t[%d]\n", 16));
	else if(pstCabTuneParam->etModulation == DI_CH_CMOD_32QAM)
		CH_DI_Print(0, ("   Mod \t[%d]\n", 32));
	else if(pstCabTuneParam->etModulation == DI_CH_CMOD_64QAM)
		CH_DI_Print(0, ("   Mod \t[%d]\n", 64));
	else if(pstCabTuneParam->etModulation == DI_CH_CMOD_128QAM)
		CH_DI_Print(0, ("   Mod \t[%d]\n", 128));
	else if(pstCabTuneParam->etModulation == DI_CH_CMOD_256QAM)
		CH_DI_Print(0, ("   Mod \t[%d]\n", 256));
	CH_DI_Print(0, ("   Spec \t[%d]\n", pstCabTuneParam->etSpectrum));
	CH_DI_Print(0, ("   SNR \t[%ld] dB (%ld)\n", (st_10024SignalStatus[nUnitId].ulSnr)/10, st_10024SignalStatus[nUnitId].ulSnr));
	CH_DI_Print(0, ("   BER \t[%d.%d*E(%d)]\n", nBerDecimal, nBerFloat, nBerExp));
	CH_DI_Print(0, ("   UnCorr \t[%ld]\n", st_10024SignalStatus[nUnitId].ulUncorrected));
	CH_DI_Print(0, ("   If Agc \t[%d]\n", st_10024SignalStatus[nUnitId].ucIfAgc));
	CH_DI_Print(0, ("   Rf Agc \t[%d]\n", st_10024SignalStatus[nUnitId].ucRfAgc));
	CH_DI_Print(0, ("   Strength \t[%d]\n", st_10024SignalStatus[nUnitId].nStrength));
	CH_DI_Print(0, ("   Quality \t[%ld]\n", st_10024SignalStatus[nUnitId].ulQuality));
	CH_DI_Print(0, ("   Power \t[%.2f] dBm\n", st_10024SignalStatus[nUnitId].fSignalPower));
	CH_DI_Print(0, ("===================================================\n"));

       return DI_CH_OK;
}

int DP10024_DumpRegister( int nChannelId)
{
	int nUnitId, i=0;
	unsigned char ucdata;
#if defined(PLATFORM_TM2063)
	unsigned int  ulTunerAddr=0;
#endif
	nUnitId = GetDeviceUnitId( nChannelId );

	CH_DI_Print(0, ("===================================================\n"));
	CH_DI_Print(0, ("   CAB TUNE Unit [%d] Demod Dump\n", nUnitId));
	CH_DI_Print(0, ("===================================================\n"));

	for(i=0; i<= 0xFE; i++)
	{
		drv_ch_i2c_DEMODRead(nUnitId, i, &ucdata, 1);
		CH_PrintMon("[0x%02X] \t[0x%02x]\n", i, ucdata);
	}

	CH_DI_Print(0, ("===================================================\n"));
	CH_DI_Print(0, ("   CAB TUNE Unit [%d] Tuner Dump\n", nUnitId));
	CH_DI_Print(0, ("===================================================\n"));

#if defined(PLATFORM_TM2063)
	if(nUnitId == MASTER_TUNER)
	{
		ulTunerAddr = M_TUNER_I2C_ADDR;
	}
	else if(nUnitId == SLAVE_TUNER)
	{
		ulTunerAddr = S_TUNER_I2C_ADDR;
	}
	else
	{
		ulTunerAddr = 0;
	}



	DRV_CH_C_I2CEnable(nUnitId);
	for(i=0; i<= 0x3C; i++)
	{
		MT_ReadSub(hTuner[nUnitId], ulTunerAddr, i, &ucdata, 1);
		CH_DI_Print(0, ("[0x%02X] \t[0x%02x]\n", i, ucdata));
	}
	DRV_CH_C_I2CDisable(nUnitId);
	CH_DI_Print(0, ("===================================================\n"));
#endif
#if defined(PLATFORM_TN182I2)
	DRV_CH_C_I2CEnable(nUnitId);
	drv_ch_i2c_TUNERDump(nUnitId);
	DRV_CH_C_I2CDisable(nUnitId);
	CH_DI_Print(0, ("===================================================\n"));

#endif
#if defined(PLATFORM_TM201)
	DRV_CH_C_I2CEnable(nUnitId);
	MxL_Dump_Register(&g_stSemTunerConfig[nUnitId]);
	DRV_CH_C_I2CDisable(nUnitId);
	CH_DI_Print(0, ("===================================================\n"));
#endif
	return DI_CH_OK;

}

int DP10024_EnableTsOutput ( int nChannelId )
{
	HUINT8 ucVal;
	int nUnitId;

	nUnitId = GetDeviceUnitId( nChannelId );

	drv_ch_i2c_DEMODRead(nUnitId, TDA_10023_SB_INTPS, &ucVal, 1);

	ucVal &= (~0x02);// 0x2c [1] TRIP : If 1, tri-stated, else active.

	drv_ch_i2c_DEMODWrite(nUnitId, TDA_10023_SB_INTPS, &ucVal, 1);

	return DI_CH_OK;
}

int DP10024_DisableTsOutput ( int nChannelId )
{
	HUINT8 ucVal;
	int nUnitId;

	nUnitId = GetDeviceUnitId( nChannelId );

	drv_ch_i2c_DEMODRead(nUnitId, TDA_10023_SB_INTPS, &ucVal, 1);

	ucVal |= 0x02;// 0x2c [1] TRIP : If 1, tri-stated, else active.

	drv_ch_i2c_DEMODWrite(nUnitId, TDA_10023_SB_INTPS, &ucVal, 1);

	return DI_CH_OK;

}

int DP10024_SetPowerState( int nChannelId, CH_Power_e etPower )
{

	unsigned char val;
	HUINT8 	ucVal;
	int nUnitId;

	nUnitId = GetDeviceUnitId( nChannelId );

	switch (etPower)
	{
		case CH_POWER_OFF:	//tmPowerStandby = 1

			//0x2e [7] TRIAGC : When TRIAGC=0, AGCTUN pin is active else tristate
			drv_ch_i2c_DEMODRead(nUnitId, SB_AGCCONF2, &ucVal,1);
			ucVal |= 0x40;
			drv_ch_i2c_DEMODWrite(nUnitId, SB_AGCCONF2, &ucVal,1);

			/* set STDBY bit of CONF to one */
			val = 0x80 | s_aucConfReg[nUnitId];

			CH_DI_Print(5, ("------------- standby mode -------------\n"));
			break;

		case CH_POWER_ON: //tmPowerOn = 0
			default:

			//0x2e [7] TRIAGC : When TRIAGC=0, AGCTUN pin is active else tristate
			drv_ch_i2c_DEMODRead(nUnitId, SB_AGCCONF2, &ucVal,1);
			ucVal &= (~0x40);
			drv_ch_i2c_DEMODWrite(nUnitId, SB_AGCCONF2, &ucVal,1);
			CH_DI_Print(5, ("CH_POWER_ON read val(0x%x)\n", ucVal));
#if defined(PLATFORM_TN182I2) || defined(PLATFORM_TM201)
			val = 0x03;	// FSampling from CH
#else
			val = 0x23; // <10023>
#endif
			CH_DI_Print(5, ("------------- Power on mode -------------\n"));
			break;
	}

	drv_ch_i2c_DEMODWrite( nUnitId, SB_CONF, &val, 1);

	return DI_CH_OK;
}


void DRV_C_InstallApi( void )
{
	CAB_InitDevice 			= &DP10024_InitDevice;
	CAB_SetTune 			= &DP10024_SetTune;
	//CAB_CheckTuneState 		= &DP10024_CheckTuneState;
	CAB_CheckLock 			= &DP10024_CheckLock;
	CAB_GetStrengthQuality 	= &DP10024_GetStrengthQuality;
	CAB_SetPowerState 		= &DP10024_SetPowerState;
	CAB_EnableTsOutput 		= &DP10024_EnableTsOutput;
	CAB_DisableTsOutput 	= &DP10024_DisableTsOutput;
	//CAB_UpdateTunedParam	= &DP10024_UpdateTunedParam;
	//CAB_GetChannelStatus	= &DP10024_GetChannelStatus;
	CAB_ResetGpio = &DP10024_GpioReset;

	return;
}


unsigned char DP10024_IsLock(int nUnitId)
{
	unsigned char	 val1;
   	HBOOL	bLocked  = FALSE;
//   	unsigned long ulUnCorr;

#if 0
	//by ilee for test
	check_agc_lock( nUnitId );
#endif
	/***********************   Check the sync information ***********************/
	/* read the sync register value */
	drv_ch_i2c_DEMODRead(nUnitId, SB_SYNC, &val1, 1);
	CH_DI_Print(3, (">DP10024_IsLock(%d) :  SYNC=0x%x\r\n", nUnitId, val1&0x4f));

	/* check if carrier was locked and if equalizer is at tracking mode */
	if((val1&0x02)!=0x02)
	{
		CH_DI_Print(2, ("carrier unlocked(%d)\r\n", nUnitId));
		bLocked = FALSE;
		return FALSE;
	}
	else
	{
		CH_DI_Print(2, ("CARRIER locked(%d)\r\n", nUnitId));
		return TRUE;
#if 0
		/*
		 * check if all of the front-end were synchronized
		 */
		drv_ch_i2c_DEMODRead(nUnitId, SB_AGCREF, &val, 1);
		CH_PrintDebug(">>>>>>> SB_AGCREF(0x%x)\r\n", val);

		if(((val1 & 0x48) == 0x08))
		{
			bLocked = TRUE;
			ulUnCorr = getUncorr(nUnitId);
			CH_PrintDebug("FEC locked(%d). Uncorrected(%d)\r\n", nUnitId, ulUnCorr);

			return TRUE;
		}
		else
		{
			CH_PrintDebug("fec unlocked(%d) : val1(0x%x)\r\n", nUnitId, val1);
			bLocked	= FALSE;
			return FALSE;

		}
#endif
	} /* end of FEC check */

}


#if defined(PLATFORM_TM2063)
void TM2063_InitTuner(int nUnitId)
{
	unsigned int status = MT_OK;
	unsigned int ulTunerAddr = 0;
	unsigned char ucVal;
#if 1
	//unsigned int ulI2cBusMum = 0;

	if(nUnitId == MASTER_TUNER)
	{
		ulTunerAddr = M_TUNER_I2C_ADDR;
	}
	else if(nUnitId == SLAVE_TUNER)
	{
#ifdef BOTH_CHBOARD
		DRV_CH_C_I2CEnable(nUnitId);


		DRV_I2c_ReadA8( 3, S_TUNER_I2C_ADDR, 0, &ucVal, 1 );

		if (ucVal != 0x9e)
		{
			ulTunerAddr = M_TUNER_I2C_ADDR;
			S_TUNER_I2C_ADDR = M_TUNER_I2C_ADDR;

			DRV_I2c_ReadA8( 3, ulTunerAddr, 0, &ucVal, 1 );

		}
		else
		{
			ulTunerAddr = S_TUNER_I2C_ADDR;

		}

		DRV_CH_C_I2CDisable(nUnitId);

#else

		ulTunerAddr = S_TUNER_I2C_ADDR;
#endif

	}
	else
	{
		ulTunerAddr = 0;
	}
	CH_DI_Print(5, ("TM2063_InitTuner___S(%d)\n", ulTunerAddr));
#endif
	//status |= MT2063_Close(&hTuner);
	CH_DI_Print(6, ("MT2063[%d] : I2C Addr[0x%02X]\n", nUnitId, ulTunerAddr));


	DRV_CH_C_I2CEnable(nUnitId);

	//CH_PrintDebug("TM2063_InitTuner___1(%d)\n", status);
	status |= MT2063_Open(ulTunerAddr, &hTuner[nUnitId], (Handle_t)nUnitId);
	CH_DI_Print(5, ("MT2063_Open(%d)(0x%x)(0x%x)\n", nUnitId, status, hTuner[nUnitId]));

	//DRV_CH_C_I2CEnable(nUnitId);

	//status |= MT2063_SetParam(hTuner, MT2063_IF1_BW , 20000000);
	//status |= MT2063_SetParam(hTuner, MT2063_IF1_CENTER, 1220000000);
	status |= MT2063_SetParam(hTuner[nUnitId], MT2063_OUTPUT_FREQ, 36125000);
	status |= MT2063_SetParam(hTuner[nUnitId], MT2063_OUTPUT_BW, 8000000);
	status |= MT2063_SetParam(hTuner[nUnitId], MT2063_RCVR_MODE, MT2063_CABLE_QAM);
/*
	status |= MT2063_SetParam(hTuner[nUnitId], MT2063_VGAGC, 1);
	status |= MT2063_SetParam(hTuner[nUnitId], MT2063_DNC_1, 2);
	status |= MT2063_SetParam(hTuner[nUnitId], MT2063_PD1_TGT, 0x25);
	status |= MT2063_SetParam(hTuner[nUnitId], MT2063_PD2_TGT, 0x2a);
	status |= MT2063_SetParam(hTuner[nUnitId], MT2063_ACFIF_MAX,  5);
	status |= MT2063_ClearPowerMaskBits(hTuner[nUnitId], MT2063_NONE_SD);
*/
	//status |= MT2063_SetParam(hTuner[nUnitId], MT2063_PD1_TGT, 0x14);

	//status |= MT2063_SetPowerMaskBits(hTuner[nUnitId], MT2063_ALL_SD);

	//status |= MT2063_SetParam(hTuner, MT2063_PD2LEV, 7);
	//status |= MT2063_SetParam(hTuner, MT2063_VGA_GAIN_MODE, MT2131_VGA_FIXED_HIGH);
	CH_DI_Print(5, ("TM2063_InitTuner___E(0x%x)\n", status));
	DRV_CH_C_I2CDisable(nUnitId);
}
#endif

#if defined(PLATFORM_TN182I2)
void TN182I2_InitTuner(int nUnitId)
{
	TDA182I2_ERR_CODE       reterr = tmERR_BAD_PARAMETER;

	DRV_CH_C_I2CEnable(nUnitId);
	TDA182I2Init(nUnitId);

	reterr = TDA182I2Reset(nUnitId);
	DRV_CH_C_I2CDisable(nUnitId);
	CH_DI_Print(5, ("TN182I2_InitTuner___E(0x%x)\n", reterr));
}

TDA182I2_ERR_CODE TN182I2_PowerMeasurement(int nUnitId, int* pnRFin)
{
	TDA182I2_ERR_CODE   reterr = tmNOERR;
	unsigned char		ucGain1, ucGain2, ucLNA, ucRF, ucLPF, ucMixer, ucRfAgc=0;
	unsigned int		unLevel, ulRfAgc;
	float				fRFAgc, KConstant1 = -16.75, KConstant2 = -5.24, fRFin, fLevel;
	signed int			cal_LNA[10] = { -12, -9, -6, -3, 0, 3, 6, 9, 12, 15 };
	signed int			cal_RF[4] = { -11, -8, -5, -2 };
	signed int			cal_LPF[4] = { 0, 3, 6, 9 };
	signed int			cal_Mixer[5] = { 2, 5, 8, 11, 14 };
	signed int			LNA_Gain, RF_Gain, LPF_Gain, Mixer_Gain;


	DRV_CH_C_I2CEnable(nUnitId);

	reterr	= TDA182I2GetAGC1_Gain_Read(nUnitId, &ucGain1);
	reterr	= TDA182I2GetAGC4_Gain_Read(nUnitId, &ucGain2);

	DRV_CH_C_I2CDisable(nUnitId);

	if(reterr)
	{
		CH_DI_Print(0, ("[GetAGC_Gain] Error !\n"));
		return reterr;
	}

	CH_UNUSED(ulRfAgc);
	//getRfAgc(nUnitId, &ulRfAgc);
	getRfAgc10024(nUnitId, &ucRfAgc);

	CH_DI_Print(4, ("[TDA182I2PowerMeasurement] Tuner [0x32] 0x%2x \n" , ucGain1));
	CH_DI_Print(4, ("[TDA182I2PowerMeasurement] Tuner [0x34] 0x%2x \n" , ucGain2));
	ucLNA	= ucGain1 & 0x0f;
	ucRF	= (ucGain1 & 0x30) >> 4;
	ucLPF	= (ucGain2 & 0x18) >> 3;
	ucMixer = ucGain2 & 0x07;

	LNA_Gain		= cal_LNA[ucLNA];

	RF_Gain 		= cal_RF[ucRF];
	LPF_Gain		= cal_LPF[ucLPF];
	Mixer_Gain		= cal_Mixer[ucMixer];
	fRFAgc			= 0.3635 * ucRfAgc + 1;

	CH_DI_Print(4, ("[TDA182I2PowerMeasurement] ucLNA : 0x%2x \t ucLNA_Gain : %d \n" , ucLNA, LNA_Gain));
	CH_DI_Print(4, ("[TDA182I2PowerMeasurement] ucRF : 0x%2x \t ucRF_Gain : %d \n" , ucRF, RF_Gain));
	CH_DI_Print(4, ("[TDA182I2PowerMeasurement] ucLPF : 0x%2x \t ucLPF_Gain : %d \n" , ucLPF, LPF_Gain));
	CH_DI_Print(4, ("[TDA182I2PowerMeasurement] ucMixer : 0x%2x \t ucMixer_Gain : %d \n" , ucMixer, Mixer_Gain));
	CH_DI_Print(4, ("[TDA182I2PowerMeasurement] ucRfAgc : %d \t fRFAgc : %f \n" , ucRfAgc, fRFAgc));

#ifdef TRUEVISION_STRENGTH

// modify signal strength display value PHK_20101001
//  same signal strength display  value compare to IR-H101C

	unLevel = 150 - (LNA_Gain+RF_Gain+LPF_Gain+Mixer_Gain+ucRfAgc);

	if 	   (unLevel >= 85)	unLevel = 100;
	else if (unLevel >= 80)	unLevel = 90;
	else if (unLevel >= 75)	unLevel = 80;
	else if (unLevel >= 70)	unLevel = 70;
	else if (unLevel >= 65)	unLevel = 60;
	else if (unLevel >= 60)	unLevel = 50;
	else if (unLevel >= 55)	unLevel = 40;
	else if (unLevel >= 45)	unLevel = 30;
	else if (unLevel >= 40)	unLevel = 20;
	else if (unLevel >= 30)	unLevel = 15;
	else 					unLevel = 10;

#else
	if( (LNA_Gain == 15) && (Mixer_Gain >= 5))
	{
		fRFin = KConstant1 - LPF_Gain - Mixer_Gain - fRFAgc ;
		CH_DI_Print(4, (" weak : (%f) = (%f) - (%d) - (%d) - (%f)\n" , fRFin, KConstant1,LPF_Gain, Mixer_Gain,fRFAgc));
	}
	else
	{
		fRFin = KConstant2 - LNA_Gain - RF_Gain - LPF_Gain - Mixer_Gain - fRFAgc ;
		CH_DI_Print(4, (" strong : (%f) = (%f) - (%d) - (%d)  - (%d) - (%d) - (%f)\n" , fRFin, KConstant2,LNA_Gain,RF_Gain,LPF_Gain, Mixer_Gain,fRFAgc));

	}
	CH_DI_Print(4, ("[TDA182I2PowerMeasurement] fRFin : %f [dBm]\n" , fRFin));

	st_10024SignalStatus[nUnitId].fSignalPower = fRFin;

	fLevel = 150 + 2*fRFin;

	if (fLevel >100)	fLevel = 100;
	if (fLevel <10) 	fLevel = 10;

	 unLevel = (int)fLevel;
#endif

	*pnRFin = unLevel;

	CH_DI_Print(4, ("[TDA182I2PowerMeasurement] unLevel : %d  \n" , unLevel));

	return reterr;
}

#endif

#if defined(PLATFORM_TM201)
int TM201_InitTuner(int nUnitId)
{
	HUINT32 ulRet = 0;

	if(nUnitId == MASTER_TUNER)
	{
		g_stSemTunerConfig[nUnitId].I2C_Addr = M_TUNER_I2C_ADDR;
		g_stSemTunerConfig[nUnitId].ClkOut_Setting=MxL_CLKOUT_ENABLE;
	}
	else if(nUnitId == SLAVE_TUNER)
	{
		g_stSemTunerConfig[nUnitId].I2C_Addr = S_TUNER_I2C_ADDR;
		g_stSemTunerConfig[nUnitId].ClkOut_Setting=MxL_CLKOUT_DISABLE;
	}
	else
	{
		CH_DI_Print(0, ("MxL_Tuner_Init : Fail\n"));
		return DI_CH_ERR;
	}

	g_stSemTunerConfig[nUnitId].Mode=MxL_MODE_CAB_STD;
	g_stSemTunerConfig[nUnitId].Xtal_Freq=MxL_XTAL_16_MHZ;
	g_stSemTunerConfig[nUnitId].Xtal_Cap=MxL_XTAL_CAP_18_PF;
	g_stSemTunerConfig[nUnitId].IF_Freq=MxL_IF_6_MHZ;
	g_stSemTunerConfig[nUnitId].IF_Spectrum=MxL_NORMAL_IF;
	g_stSemTunerConfig[nUnitId].BW_MHz=MxL_BW_8MHz;
	g_stSemTunerConfig[nUnitId].RF_Freq_Hz=666000000;
	g_stSemTunerConfig[nUnitId].ClkOut_Amp=MxL_CLKOUT_AMP_12;

	CH_DI_Print(4, ("MxL_Tuner_Init(%d)\n", nUnitId));
	DRV_CH_C_I2CEnable(nUnitId);

	ulRet = MxL_Tuner_Init(&g_stSemTunerConfig[nUnitId]);
	if( ulRet != DI_CH_OK )
	{
		CH_DI_Print(0, ("MxL_Tuner_Init : nRet(%x)\n", ulRet));
	}
	MxL_Enable_LT(&g_stSemTunerConfig[nUnitId], FALSE);
	CH_DI_Print(4, ("MxL_Tuner_Init : OK nRet(%x)\n", ulRet));
	DRV_CH_C_I2CDisable(nUnitId);

	return ulRet;
}
#endif

void DP10024_ClearErrCount ( int nUnitId )
{
	HUINT8 ucVal;

	drv_ch_i2c_DEMODRead(nUnitId, SB_RSCONF, &ucVal, 1);

	ucVal  &= 0xdf;
	drv_ch_i2c_DEMODWrite(nUnitId, SB_RSCONF, &ucVal, 1);

	VK_TASK_Sleep(10);

	ucVal  |= 0x20;
	drv_ch_i2c_DEMODWrite(nUnitId, SB_RSCONF, &ucVal, 1);

	CH_DI_Print(5, ("DP10024_ClearErrCount....\n"));

}

/* This API will be deprecated */
int DP10024_LockInfo( int nChannelId, int *snr, int *ber_decimal, int *ber_float, int *ber_exp, int *uncorrected, int *signallevel)
{
	int nUnitId, lSignalLevel=0;
	unsigned long	SNRfactor=0, ulUnCorr=0, ulCorr=0, ulUnErr=0, ulBer=0;
	unsigned int ulRfAgc=0;
	unsigned char ucIfAgc=0;
	int ulExp=0, ulDecimal=0, ulFloat=0;
	CH_CAB_TuneParam_t * pstCabTuneParam;

	nUnitId = GetDeviceUnitId( nChannelId );
	pstCabTuneParam = (CH_CAB_TuneParam_t *)GetWorkingTuneInfo( nChannelId );
	SNRfactor = measure_SNR_TDA( nUnitId, pstCabTuneParam->etModulation );
	ulUnCorr = getUncorr(nUnitId);
	ulCorr = getCorr(nUnitId);
	ulUnErr = getUnErr(nUnitId);
	ucIfAgc = getIfAgc(nUnitId);
	ulBer = getBer(nUnitId);
	makeBer(ulBer, &ulExp, &ulDecimal, &ulFloat);
	getRfAgc(nUnitId, &ulRfAgc);
	lSignalLevel = DRV_CH_C_InbandStrength(nChannelId);

	CH_DI_Print(0, ("===================================================\n"));
	CH_DI_Print(0, ("   CAB TUNE Unit [%d] Status(%s)\n", nUnitId, (s_ucIsLock[nUnitId] ==TRUE)?"Lock":"Unlock"));
	CH_DI_Print(0, ("===================================================\n"));
	CH_DI_Print(0, ("   Freq \t[%d]\n", pstCabTuneParam->ulFrequency));
	CH_DI_Print(0, ("   SR \t\t[%d]\n", pstCabTuneParam->ulSymbolRate));
	if(pstCabTuneParam->etModulation == DI_CH_CMOD_16QAM)
		CH_DI_Print(0, ("   Mod \t[%d]\n", 16));
	else if(pstCabTuneParam->etModulation == DI_CH_CMOD_32QAM)
		CH_DI_Print(0, ("   Mod \t[%d]\n", 32));
	else if(pstCabTuneParam->etModulation == DI_CH_CMOD_64QAM)
		CH_DI_Print(0, ("   Mod \t[%d]\n", 64));
	else if(pstCabTuneParam->etModulation == DI_CH_CMOD_128QAM)
		CH_DI_Print(0, ("   Mod \t[%d]\n", 128));
	else if(pstCabTuneParam->etModulation == DI_CH_CMOD_256QAM)
		CH_DI_Print(0, ("   Mod \t[%d]\n", 256));
	CH_DI_Print(0, ("   Spec \t[%d]\n", pstCabTuneParam->etSpectrum));
	CH_DI_Print(0, ("   SNR \t[%ld] dB (%ld)\n", SNRfactor/10, SNRfactor));
	CH_DI_Print(0, ("   BER \t[%d.%d*E(%d)]\n", ulDecimal, ulFloat, ulExp));
	CH_DI_Print(0, ("   UnErr \t[%ld]\n", ulUnErr));
	CH_DI_Print(0, ("   Corr \t[%ld]\n", ulCorr));
	CH_DI_Print(0, ("   UnCorr \t[%ld]\n", ulUnCorr));
	CH_DI_Print(0, ("   If Agc \t[%d]\n", ucIfAgc));
	CH_DI_Print(0, ("   Rf Agc \t[%d]\n", ulRfAgc));
//	CH_DI_Print(0, ("   Signal Level \t[%d.%d]dBmV\n", lSignalLevel/10, abs(lSignalLevel%10)));
	CH_DI_Print(0, ("   Signal Level \t[%d.%d]dBuV\n", (lSignalLevel)/10, abs(lSignalLevel%10)));
	CH_DI_Print(0, ("===================================================\n"));

	if(s_ucIsLock[nUnitId] ==TRUE)
	{
		*snr = SNRfactor;

		*ber_decimal = ulDecimal;
		*ber_float = ulFloat;
		*ber_exp = ulExp;

		*uncorrected = ulUnCorr;

		*signallevel =  (lSignalLevel+600);
	}
	else
	{
		*snr = 0;

		*ber_decimal = 0;
		*ber_float = 0;
		*ber_exp = 0;

		*uncorrected = 0;

		*signallevel =  0;

	}
       return DI_CH_OK;

}


/*********************** End of File ******************************/

